#include "updatedialog.h"
#include "ui_updatedialog.h"

UpdateDialog::UpdateDialog(QWidget *parent, update_dialog_mode_t a_mode,
  const mxip_t &a_ip, const string_type &a_device_path,
  const hfftp_settings_t& a_hfftp_settings
):
  QDialog(parent),
  ui(new Ui::UpdateDialog),
  m_timer(),
  m_ip(a_ip),
  m_hfftp_settings(a_hfftp_settings),
  mp_memory_file_read_only(NULL),
  mp_memory_file_write_only(NULL),
  mp_hfftp_client_hardflow(),
  mp_hfftp_client(),
  m_buffer(),
  m_delay_before_read_file(irs::make_cnt_s(1)),
  m_delay_before_destroy_hfttp(irs::make_cnt_s(3)),
  m_process(process_wait_program)
{
  ui->setupUi(this);

  connect(&m_timer, SIGNAL(timeout()), this, SLOT(tick()));

  read_file_to_buffer(a_hfftp_settings.local_file_name);

  irs::hfftp::client_t::configuration_t config;

  switch (a_hfftp_settings.transport_protocol) {
    case tp_udp: {
      string_type ip_str = mxip_to_str(m_ip);
      mp_hfftp_client_hardflow.reset(
        new irs::hardflow::udp_flow_t(irst(""), irst(""),
        ip_str, a_hfftp_settings.device_port,
        irs::hardflow::udplc_mode_limited,
        1, 0xFFFF, false, 24*60*60, false, 10));

      config.data_size = 1400;
      config.timeout = 7;
    } break;
    case tp_tcp: {
      irs_u16 port = 0;
      if (!irs::str_to_num(a_hfftp_settings.device_port, &port)) {
        throw std::logic_error("Invalid port");
      }
      mp_hfftp_client_hardflow.reset(
        new irs::hardflow::tcp_client_t(m_ip, port));

      config.data_size = 1400;
      config.timeout = 7;
    } break;
    case tp_usb_hid: {
      if (a_device_path.empty()) {
        throw std::logic_error("Invalid path");
      }
      const size_type channel_id = 1;
      mp_hfftp_client_hardflow.reset(
        new irs::hardflow::usb_hid_t(a_device_path, channel_id));

      config.data_size = 10000;
      config.timeout = 7;
    } break;
  }

  config.retransmit_max_count = 100;
  mp_hfftp_client.reset(new irs::hfftp::client_t(mp_hfftp_client_hardflow.get(),
    config));

  if (a_mode == update_dialog_mode_program) {
    mp_memory_file_read_only.reset(
      new irs::hfftp::memory_file_read_only_t(irs::vector_data(m_buffer),
      m_buffer.size()));
    mp_hfftp_client->put(a_hfftp_settings.remote_file_name,
      mp_memory_file_read_only.get());
    ui->actionLineEdit->setText(irs::str_conv<QString>(
      std::wstring(L"Заливка прошивки")));
  } else {
    mp_memory_file_write_only.reset(
      new irs::hfftp::memory_file_write_only_t());
    mp_hfftp_client->get(m_hfftp_settings.remote_file_name,
      mp_memory_file_write_only.get());
    ui->actionLineEdit->setText(irs::str_conv<QString>(
      std::wstring(L"Верификация прошивки")));
    m_process = process_wait_read_file;
  }
  m_timer.start(10);
}

void UpdateDialog::read_file_to_buffer(const string_type &a_file_name)
{
  bool success = false;
  std::ifstream ifile(irs::str_conv<std::string>(a_file_name).c_str(),
    ios::binary);

  if (ifile.fail()) {
    ifile.close();
    throw std::runtime_error("Файл открыть не удалось");
  }
  ifile.seekg(0, ios::end);
  size_type size = ifile.tellg();
  ifile.seekg(0, ios::beg);
  try {
    m_buffer.resize(size);
    ifile.read(reinterpret_cast<char*>(irs::vector_data(m_buffer)), size);
    if (!ifile.fail()) {
      success = true;
    }
    ifile.close();
  } catch (...) {
    ifile.close();
  }
  if (!success) {
    throw std::runtime_error("Файл прочитать не удалось");
  }
}

bool UpdateDialog::is_buffers_equals(const irs_u8* ap_buf_first,
  size_type a_buf_first_size, const irs_u8* ap_buf_second,
  size_type a_buf_second_size)
{
  if (a_buf_first_size != a_buf_second_size) {
    return false;
  }
  return (0 == memcmp(ap_buf_first, ap_buf_second, a_buf_first_size));
}

UpdateDialog::~UpdateDialog()
{
  mp_hfftp_client.reset();
  mp_hfftp_client_hardflow.reset();
  delete ui;
}

void UpdateDialog::tick()
{
  switch (m_process) {
    case process_wait_program: {
      if (mp_hfftp_client->status() == irs_st_ready) {
        IRS_LIB_DBG_MSG("Клиент завершил задачу успешно");
        mp_memory_file_read_only.reset();
        ui->actionLineEdit->setText(irs::str_conv<QString>(
          std::wstring(L"Ждем одну секунду перед проверкой")));
        m_delay_before_read_file.start();
        m_process = process_delay_before_read_file;
      } else if (mp_hfftp_client->status() == irs_st_error) {
        IRS_LIB_DBG_MSG("Клиент завершил задачу с ошибкой");
        ui->actionLineEdit->setText(irs::str_conv<QString>(
          std::wstring(L"Залить прошивку не удалось")));
        ui->actionLineEdit->setStyleSheet("QLineEdit { color: red }");
        mp_memory_file_read_only.reset();
        mp_hfftp_client.reset();
        mp_hfftp_client_hardflow.reset();
        m_process = process_wait_commands;
      }
    } break;
    case process_delay_before_read_file: {
      if (m_delay_before_read_file.check()) {
        mp_memory_file_write_only.reset(
          new irs::hfftp::memory_file_write_only_t());
        mp_hfftp_client->get(m_hfftp_settings.remote_file_name,
          mp_memory_file_write_only.get());
        ui->actionLineEdit->setText(irs::str_conv<QString>(
          std::wstring(L"Верификация прошивки")));
        m_process = process_wait_read_file;
      }
    } break;
    case process_wait_read_file: {
      if (mp_hfftp_client->status() != irs_st_busy) {
        if (mp_hfftp_client->status() == irs_st_ready) {
          IRS_LIB_DBG_MSG("Клиент завершил задачу успешно");
          std::vector<irs_u8> buffer(mp_memory_file_write_only->size());
          mp_memory_file_write_only->read(irs::vector_data(buffer),
            buffer.size());
          if (is_buffers_equals(irs::vector_data(buffer), buffer.size(),
            irs::vector_data(m_buffer), m_buffer.size())) {
            ui->actionLineEdit->setText(irs::str_conv<QString>(
              std::wstring(L"Верификация завершена. Различий не обнаружено")));
            ui->actionLineEdit->setStyleSheet("QLineEdit { color: green }");
          } else {
            ui->actionLineEdit->setText(irs::str_conv<QString>(
              std::wstring(L"Верификация завершена. Найдены различия")));
            ui->actionLineEdit->setStyleSheet("QLineEdit { color: red }");
          }
        } else if (mp_hfftp_client->status() == irs_st_error) {
          IRS_LIB_DBG_MSG("Клиент завершил задачу с ошибкой");
          ui->actionLineEdit->setText(irs::str_conv<QString>(
            std::wstring(L"Прочитать прошивку не удалось")));
          ui->actionLineEdit->setStyleSheet("QLineEdit { color: red }");
        }
        mp_memory_file_write_only.reset();
        //mp_hfftp_client.reset();
        //mp_hfftp_client_hardflow.reset();
        m_delay_before_destroy_hfttp.start();
        m_process = process_wait_before_correct_closing;
      }
    } break;
    case process_wait_before_correct_closing: {
      if (m_delay_before_destroy_hfttp.check()) {
        mp_hfftp_client.reset();
        mp_hfftp_client_hardflow.reset();
        m_process = process_wait_commands;
      }
    } break;
    case process_wait_commands: {
      // Ожидаем закрытия
    } break;
  }

  if (!mp_hfftp_client_hardflow.is_empty()) {
    for (int i = 0; i < 100; i++) {
      mp_hfftp_client_hardflow->tick();
    }
  }
  if (!mp_hfftp_client.is_empty()) {
    for (int i = 0; i < 100; i++) {
      mp_hfftp_client->tick();
    }
    ui->transferredBytesLineEdit->setText(irs::str_conv<QString>(
      irs::num_to_str(mp_hfftp_client->transferred_bytes())));
    if (mp_hfftp_client->status() == irs_st_busy) {
      if (mp_hfftp_client->file_size_is_known()) {
        ui->fileSizeLineEdit->setText(irs::str_conv<QString>(
          irs::num_to_str(mp_hfftp_client->get_file_size())));
        ui->progressBar->setMaximum(100);
        if (mp_hfftp_client->get_file_size() > 0) {
          ui->progressBar->setValue(
            (static_cast<double>(mp_hfftp_client->transferred_bytes())/
          mp_hfftp_client->get_file_size())*100);
        } else {
          ui->progressBar->setValue(ui->progressBar->maximum());
        }
      } else {
        ui->progressBar->setMaximum(0);
        ui->fileSizeLineEdit->setText(QString());
      }
    } else {
      if (mp_hfftp_client->status() == irs_st_ready) {
        ui->progressBar->setMaximum(100);
        ui->progressBar->setValue(100);
      } else {
        ui->progressBar->setMaximum(100);
        ui->progressBar->setValue(0);
      }
    }
  }
}
