#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "updatedialog.h"

#include <irsdev.h>
#include <math.h>

// class usb_hid_device_path_map_t
usb_hid_device_path_map_t::usb_hid_device_path_map_t():
  m_manufacturer(irst("RES")),
  m_product(irst("N4-25")),
  m_map(),
  m_changed(false),
  m_update_timer(irs::make_cnt_s(1))
{
}

const usb_hid_device_path_map_t::map_type&
usb_hid_device_path_map_t::get_map() const
{
  return m_map;
}

bool usb_hid_device_path_map_t::changed()
{
  bool changed = m_changed;
  m_changed = false;
  return changed;
}

void usb_hid_device_path_map_t::tick()
{
  if (m_update_timer.check()) {
    update();
  }
}

void usb_hid_device_path_map_t::update()
{
  #if IRS_USE_HID_WIN_API
  map_type map = m_map;
  m_map.clear();
  std::vector<irs::usb_hid_device_info_t> devs =
    irs::usb_hid_info_t::get_instance()->get_devices_info();
  for (size_type i = 0; i < devs.size(); i++) {
    //string_type device;

    if (m_manufacturer != devs[i].attributes.manufacturer) {
      continue;
    }
    /*if (m_product != devs[i].attributes.product) {
      continue;
    }*/

    /*device += devs[i].attributes.manufacturer;
    if (!device.empty() && !devs[i].attributes.product.empty()) {
      device += irst(" ");
    }*/
    /*device += devs[i].attributes.product;
    if (!device.empty() && !devs[i].attributes.serial_number.empty()) {
      device += irst(" ");
    }*/

    /*if (!device.empty() && !devs[i].path.empty()) {
      device += irst(" ");
    }
    device += devs[i].path;
*/
    device_info_t device_info;
    device_info.product = devs[i].attributes.product;
    device_info.path = devs[i].path;
    string_type id = devs[i].attributes.serial_number;
    m_map.insert(make_pair(id, device_info));
  }
  if (map != m_map) {
    m_changed = true;
  }
  #else //IRS_USE_HID_WIN_API
  m_map.insert(
    make_pair(irst("define IRS_USE_HID_WIN_API выключен!"), device_info_t()));
  #endif //IRS_USE_HID_WIN_API
}

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow),
  mp_settings(settings_t::instance()),
  m_stream_buf(100),
  mp_log_stream(new ofstream("hfftp.log", ios::out | ios::app)),
  mp_plain_text_edit_buf(),
  m_timer(),
  mp_client_dcp_udp_hardflow(),
  mp_server_dcp_udp_hardflow(),
  mp_dcp_client(),
  m_devices(),
  m_update_devices_loop_timer(irs::make_cnt_s(0.5)),

  m_usb_hid_device_path_map(),
  m_usb_devices()
{
  ui->setupUi(this);

  connect(&m_timer, SIGNAL(timeout()), this, SLOT(tick()));

  irs::loc();
  setlocale(LC_ALL, "Russian_Russia.1251");

  mp_plain_text_edit_buf.reset(new irs::plain_text_edit_buf(
    ui->logPlainTextEdit, plain_text_edit_buf_size));

  irs::mlog().rdbuf(&m_stream_buf);
  irs::mlog() << boolalpha;
  m_stream_buf.insert(mp_log_stream->rdbuf());
  m_stream_buf.insert(mp_plain_text_edit_buf.get());
  m_timer.start(10);

  IRS_DBG_MSG("Start");

  mp_settings->load();

  const hfftp_settings_t hfftp_settings =
    mp_settings->get_hfftp_settings();

  ui->localFileNameLineEdit->setText(
    irs::str_conv<QString>(hfftp_settings.local_file_name));

  ui->devicesTableWidget->setColumnCount(column_count);
  ui->devicesTableWidget->setHorizontalHeaderItem(device_mac_column,
    new QTableWidgetItem("MAC"));
  ui->devicesTableWidget->setHorizontalHeaderItem(device_type_column,
    new QTableWidgetItem(irs::str_conv<QString>(
    std::wstring(irst("Тип устройства")))));
  ui->devicesTableWidget->setHorizontalHeaderItem(
    device_hardware_version_column,
    new QTableWidgetItem(irs::str_conv<QString>(
    std::wstring(irst("H/W версия")))));
  ui->devicesTableWidget->setHorizontalHeaderItem(
    device_firmware_version_column,
    new QTableWidgetItem(irs::str_conv<QString>(
    std::wstring(irst("F/W версия")))));
  ui->devicesTableWidget->setHorizontalHeaderItem(device_ip_column,
    new QTableWidgetItem(irs::str_conv<QString>(
    std::wstring(irst("IP-адрес")))));
  ui->devicesTableWidget->setHorizontalHeaderItem(device_netmask_column,
    new QTableWidgetItem(irs::str_conv<QString>(
    std::wstring(irst("Маска")))));
  ui->devicesTableWidget->setHorizontalHeaderItem(device_gateway_column,
    new QTableWidgetItem(irs::str_conv<QString>(
    std::wstring(irst("Шлюз")))));
  ui->devicesTableWidget->setHorizontalHeaderItem(offer_ip_column,
    new QTableWidgetItem(irs::str_conv<QString>(
    std::wstring(irst("Задать IP-адрес")))));
  ui->devicesTableWidget->setHorizontalHeaderItem(offer_netmask_column,
    new QTableWidgetItem(irs::str_conv<QString>(
    std::wstring(irst("Задать маску")))));
  ui->devicesTableWidget->setHorizontalHeaderItem(offer_gateway_column,
    new QTableWidgetItem(irs::str_conv<QString>(
    std::wstring(irst("Задать шлюз")))));


  ui->usbDevicesTableWidget->setColumnCount(usb_device_column_count);
  ui->usbDevicesTableWidget->setHorizontalHeaderItem(usb_device_id_column,
    new QTableWidgetItem("ID"));
  ui->usbDevicesTableWidget->setHorizontalHeaderItem(usb_device_type_column,
    new QTableWidgetItem(irs::str_conv<QString>(
    std::wstring(irst("Тип устройства")))));
  ui->usbDevicesTableWidget->setHorizontalHeaderItem(
    usb_device_hardware_version_column,
    new QTableWidgetItem(irs::str_conv<QString>(
    std::wstring(irst("H/W версия")))));
  ui->usbDevicesTableWidget->setHorizontalHeaderItem(
    usb_device_firmware_version_column,
    new QTableWidgetItem(irs::str_conv<QString>(
    std::wstring(irst("F/W версия")))));
  start_dcp();
}

void MainWindow::start_dcp()
{
  const udp_config_type udp_client_config =
    make_dcp_client_udp_flow_configuration();
  const udp_config_type udp_server_config =
    make_dcp_server_udp_flow_configuration();
  mp_client_dcp_udp_hardflow.reset(
    new irs::hardflow::udp_flow_t(udp_client_config));
  mp_server_dcp_udp_hardflow.reset(
    new irs::hardflow::udp_flow_t(udp_server_config));

  mp_dcp_client.reset(new irs::dcp::client_t(
    mp_client_dcp_udp_hardflow.get(),
    mp_server_dcp_udp_hardflow.get()));
}

MainWindow::udp_config_type
MainWindow::make_dcp_client_udp_flow_configuration()
{
  read_settings_from_ui();
  device_autoconfig_settings_t device_autoconfig_settings =
    mp_settings->get_device_autoconfig_settings();
  udp_config_type config;
  config.remote_host_port = device_autoconfig_settings.device_port;
  config.remote_host_name = irst("255.255.255.255");
  config.connections_mode = irs::hardflow::udplc_mode_limited;
  config.channel_max_count = 1;
  config.broadcast_allowed = true;
  return config;
}

MainWindow::udp_config_type
MainWindow::make_dcp_server_udp_flow_configuration()
{
  read_settings_from_ui();
  device_autoconfig_settings_t device_autoconfig_settings =
    mp_settings->get_device_autoconfig_settings();
  udp_config_type config;
  config.local_host_port = device_autoconfig_settings.local_port;
  config.connections_mode = irs::hardflow::udplc_mode_queue;
  config.channel_max_count = 1000;
  return config;
}

MainWindow::~MainWindow()
{
  read_settings_from_ui();
  mp_settings->save();
  delete ui;
}

void MainWindow::read_settings_from_ui()
{
  device_autoconfig_settings_t device_autoconfig_setting;
  //device_autoconfig_setting.device_ip =
    //irs::str_conv<string_type>(ui->remoteIpLineEdit->text());
  //device_autoconfig_setting.device_port =
    //irs::str_conv<string_type>(ui->remotePortLineEdit->text());

  hfftp_settings_t hfftp_settings;
  /*hfftp_settings.server_ip =
    irs::str_conv<string_type>(ui->remoteIpLineEdit->text());
  hfftp_settings.server_port =
    irs::str_conv<string_type>(ui->remotePortLineEdit->text());
  hfftp_settings.block_size =
    ui->clientBlockSizeComboBox->currentText().toUInt();
  hfftp_settings.timeout = ui->clientTimeoutSpinBox->value();
  hfftp_settings.retransmit_max_count =
    ui->clientMaxRetransmitSpinBox->value();*/
  hfftp_settings.local_file_name =
    irs::str_conv<string_type>(ui->localFileNameLineEdit->text());
  mp_settings->set_hfftp_settings(hfftp_settings);
}

void MainWindow::client_changet_event(const irs::hfftp::client_t& a_client)
{
  if (a_client.connection_is_established()) {

  }
}

void MainWindow::tick()
{
  if (!mp_client_dcp_udp_hardflow.is_empty()) {
    mp_client_dcp_udp_hardflow->tick();
  }
  if (!mp_server_dcp_udp_hardflow.is_empty()) {
    mp_server_dcp_udp_hardflow->tick();
  }
  if (!mp_dcp_client.is_empty()) {
    mp_dcp_client->tick();
    if (m_update_devices_loop_timer.check()) {
      if (mp_dcp_client->get_devices() != m_devices) {
        m_devices = mp_dcp_client->get_devices();
        update_ethernet_devices_table();
      }
    }
  }

  m_usb_hid_device_path_map.tick();
  update_usb_device_table();
}

void MainWindow::update_ethernet_devices_table()
{
  ui->devicesTableWidget->setRowCount(m_devices.size());
  devices_type::const_iterator it = m_devices.begin();
  for (size_type i = 0; i < m_devices.size(); i++) {
    ui->devicesTableWidget->setItem(i, device_mac_column,
      new QTableWidgetItem(
      irs::str_conv<QString>(mxmac_to_str(it->first))));
    ui->devicesTableWidget->setItem(i, device_type_column,
      new QTableWidgetItem(
      irs::str_conv<QString>(irs::get_device_name(it->second.code))));
    ui->devicesTableWidget->setItem(i, device_hardware_version_column,
      new QTableWidgetItem(
      irs::str_conv<QString>(it->second.hardware_version)));
    ui->devicesTableWidget->setItem(i, device_firmware_version_column,
      new QTableWidgetItem(
      irs::str_conv<QString>(it->second.firmware_version)));
    ui->devicesTableWidget->setItem(i, device_ip_column,
      new QTableWidgetItem(irs::str_conv<QString>(mxip_to_str(
      it->second.device_configuration.ip))));
    ui->devicesTableWidget->setItem(i, device_netmask_column,
      new QTableWidgetItem(irs::str_conv<QString>(mxip_to_str(
      it->second.device_configuration.netmask))));
    ui->devicesTableWidget->setItem(i, device_gateway_column,
      new QTableWidgetItem(irs::str_conv<QString>(mxip_to_str(
      it->second.device_configuration.gateway))));
    ui->devicesTableWidget->setItem(i, offer_ip_column,
      new QTableWidgetItem(QString()));
    ui->devicesTableWidget->setItem(i, offer_netmask_column,
      new QTableWidgetItem(QString()));
    ui->devicesTableWidget->setItem(i, offer_gateway_column,
      new QTableWidgetItem(QString()));
    ++it;
  }
}

void MainWindow::update_usb_device_table()
{
  if (m_usb_hid_device_path_map.changed()) {
    m_usb_devices = m_usb_hid_device_path_map.get_map();
    usb_hid_device_path_map_t::map_type::const_iterator it =
      m_usb_devices.begin();
    ui->usbDevicesTableWidget->setRowCount(m_usb_devices.size());
    int row = 0;
    while (it != m_usb_devices.end()) {
      const QString id = irs::str_conv<QString>(it->first);
      usb_hid_device_path_map_t::device_info_t device_info = it->second;
      const QString product = irs::str_conv<QString>(device_info.product);
      ui->usbDevicesTableWidget->setItem(row, usb_device_id_column,
        new QTableWidgetItem(id));
      ui->usbDevicesTableWidget->setItem(row, usb_device_type_column,
        new QTableWidgetItem(product));
      row++;
      ++it;
    }
  }
}

void MainWindow::update_buttons_configuration()
{
  bool program_button_enabled = true;
  bool offer_ip_button_enabled = true;
  QList<QTableWidgetItem *> items = ui->devicesTableWidget->selectedItems();
  if (items.isEmpty()) {
    program_button_enabled = false;
    offer_ip_button_enabled = false;
  } else {
    std::set<int> selected_rows;
    for (int i = 0; i < items.size(); i++) {
      selected_rows.insert(items[i]->row());
    }
    if (selected_rows.empty()) {
      program_button_enabled = false;
      offer_ip_button_enabled = false;
    }
    std::set<int>::const_iterator row_it = selected_rows.begin();
    while (row_it != selected_rows.end()) {
      int row = *row_it;
      if (program_button_enabled) {
        QString ip_str =
          ui->devicesTableWidget->item(row, device_ip_column)->text();
        program_button_enabled = is_ip_valid(ip_str);
      }
      if (offer_ip_button_enabled) {
        QString mac_str =
          ui->devicesTableWidget->item(row, device_mac_column)->text();
        QString offer_ip_str =
          ui->devicesTableWidget->item(row, offer_ip_column)->text();
        offer_ip_button_enabled = is_mac_valid(mac_str) &&
          is_ip_valid(offer_ip_str);
      }
      ++row_it;
    }
  }
  if (program_button_enabled) {
    read_settings_from_ui();
    const hfftp_settings_t hfftp_settings = mp_settings->get_hfftp_settings();
    program_button_enabled = !hfftp_settings.local_file_name.empty();
  }
  ui->programPushButton->setEnabled(program_button_enabled);
  ui->verifyPushButton->setEnabled(program_button_enabled);
  ui->offerIPPushButton->setEnabled(offer_ip_button_enabled);
}

void MainWindow::update_usb_buttons_configuration()
{
  bool program_button_enabled = true;
  QList<QTableWidgetItem *> items = ui->usbDevicesTableWidget->selectedItems();
  if (items.isEmpty()) {
    program_button_enabled = false;
  } else {
    std::set<int> selected_rows;
    for (int i = 0; i < items.size(); i++) {
      selected_rows.insert(items[i]->row());
    }
    program_button_enabled = !selected_rows.empty();
  }
  ui->programUSBDevicePushButton->setEnabled(program_button_enabled);
  ui->verifyUSBDevicePushButton->setEnabled(program_button_enabled);
}

void MainWindow::on_programPushButton_clicked()
{
  read_settings_from_ui();
  const std::vector<device_table_item_t> devices = get_selected_devices();
  if (devices.empty()) {
    return;
  }
  mxip_t ip = /*irs::make_mxip(127,0,0,1);*/
    devices[0].device_info.device_configuration.ip;
  const hfftp_settings_t hfftp_settings = mp_settings->get_hfftp_settings();
  /*QString file_name = ui->localFileNameLineEdit->text();*/
  if (hfftp_settings.local_file_name.empty()) {
    return;
  }
  UpdateDialog dialog(this, update_dialog_mode_program, ip, string_type(),
    hfftp_settings);
  dialog.exec();
  #ifdef NOP
  read_settings_from_ui();
  hfftp_settings_t hfftp_settings = mp_settings->get_hfftp_settings();
  mxip_t server_ip = {192, 168, 0, 214};
  irs_u16 server_port = 30005;
  mp_hfftp_client_hardflow.reset(new irs::hardflow::tcp_client_t(
    server_ip, server_port));


  irs::hfftp::client_t::configuration_t config;
  config.data_max_size = hfftp_settings.block_size;
  config.timeout = hfftp_settings.timeout;
  config.retransmit_max_count = hfftp_settings.retransmit_max_count;
  mp_hfftp_client.reset(new irs::hfftp::client_t(mp_hfftp_client_hardflow.get(),
    config));

  mp_hfftp_client->set_changed_event(
    irs::make_event_1(this, &MainWindow::client_changet_event));

  /*mp_ofstream_file.reset(new irs::hfftp::ofstream_file_t(
    hfftp_settings.local_file_name));
  mp_hfftp_client->get(hfftp_settings.remote_file_name,
    mp_ofstream_file.get());*/
  mp_ifstream_file.reset(new irs::hfftp::ifstream_file_t(
    hfftp_settings.local_file_name));
  mp_hfftp_client->put("program.img", mp_ifstream_file.get());
  //m_client_mode = client_mode_get;
  ui->connectPushButton->setDisabled(true);
  ui->programPushButton->setDisabled(true);
  ui->progressBar->setValue(0);
  #endif // NOP
}

void MainWindow::on_searchDevicesPushButton_clicked()
{
  if (!mp_dcp_client.is_empty()) {
    mp_dcp_client->command_get_device_status();
  }
}

void MainWindow::on_offerIPPushButton_clicked()
{
  const std::vector<device_table_item_t> devices = get_selected_devices();
  if (devices.empty()) {
    return;
  }
  if (mp_dcp_client->get_status() == irs_st_busy) {
    mp_dcp_client->abort();
  }
  irs::dcp::device_configuration_t config;
  mxmac_t mac = devices[0].mac;
  config.ip = devices[0].offer_ip;
  config.netmask = devices[0].offer_netmask;
  config.gateway = devices[0].offer_gateway;
  config.dhcp_enabled = devices[0].offer_dhcp_enabled;
  mp_dcp_client->command_offer_configuration(mac, config);
}

void MainWindow::on_deleteDevicesPushButton_clicked()
{
  mp_dcp_client->delete_devices();
  m_devices.clear();
  update_ethernet_devices_table();
}

void MainWindow::on_selectFilePushButton_clicked()
{
  QFileInfo fi(ui->localFileNameLineEdit->text());
  QString dir = fi.absoluteFilePath();
  QString fileName = QFileDialog::getOpenFileName(this, tr("Open file"), dir);
  if (!fileName.isEmpty()) {
    ui->localFileNameLineEdit->setText(fileName);
  }
}

void MainWindow::on_devicesTableWidget_itemSelectionChanged()
{
  update_buttons_configuration();
}

bool MainWindow::is_ip_valid(const QString& a_str)
{
  bool ip_valid = false;
  mxip_t ip;
  if (str_to_mxip(a_str, &ip)) {
    if ((ip != mxip_t::any_ip()) &&
      (ip != mxip_t::broadcast_ip())) {
      ip_valid = true;
    }
  }
  return ip_valid;
}

bool MainWindow::is_mac_valid(const QString& a_str)
{
  bool mac_valid = false;
  mxmac_t mac;
  if (str_to_mxmac(a_str, &mac)) {
    if ((mac != mxmac_t::zero_mac()) &&
      (mac != mxmac_t::broadcast_mac())) {
      mac_valid = true;
    }
  }
  return mac_valid;
}

std::vector<MainWindow::device_table_item_t>
MainWindow::get_selected_devices()
{
  std::vector<device_table_item_t> devices;
  QList<QTableWidgetItem *> items = ui->devicesTableWidget->selectedItems();
  if (!items.isEmpty()) {
    std::set<int> selected_rows;
    for (int i = 0; i < items.size(); i++) {
      selected_rows.insert(items[i]->row());
    }
    std::set<int>::const_iterator row_it = selected_rows.begin();
    while (row_it != selected_rows.end()) {
      device_table_item_t device;

      const int row = *row_it;
      QString mac_str =
        ui->devicesTableWidget->item(row, device_mac_column)->text();
      mxmac_t mac;
      bool mac_valid = false;
      if (str_to_mxmac(mac_str, &mac)) {
        if ((mac != mxmac_t::zero_mac()) &&
          (mac != mxmac_t::broadcast_mac())) {
          mac_valid = true;
        }
      }
      if (!mac_valid) {
        continue;
      }

      device.mac = mac;

      IRS_LIB_ASSERT(m_devices.find(mac) != m_devices.end());
      device.device_info = m_devices[mac];

      QString offer_ip_str =
        ui->devicesTableWidget->item(row, offer_ip_column)->text();
      device.offer_ip = str_to_mxip_default(offer_ip_str, mxip_t::any_ip());

      QString offer_netmask_str =
        ui->devicesTableWidget->item(row, offer_netmask_column)->text();
      device.offer_netmask = str_to_mxip_default(offer_netmask_str,
        mxip_t::any_ip());

      QString offer_gateway_str =
        ui->devicesTableWidget->item(row, offer_gateway_column)->text();
      device.offer_gateway = str_to_mxip_default(offer_gateway_str,
        mxip_t::any_ip());

      device.offer_dhcp_enabled = false;

      devices.push_back(device);

      ++row_it;
    }
  }
  return devices;
}

std::vector<MainWindow::string_type>
MainWindow::get_selected_usb_devices()
{
  std::vector<string_type> devices;
  QList<QTableWidgetItem *> items = ui->usbDevicesTableWidget->selectedItems();
  if (!items.isEmpty()) {
    std::set<int> selected_rows;
    for (int i = 0; i < items.size(); i++) {
      selected_rows.insert(items[i]->row());
    }
    std::set<int>::const_iterator row_it = selected_rows.begin();
    while (row_it != selected_rows.end()) {

      const int row = *row_it;
      QString id_str =
        ui->usbDevicesTableWidget->item(row, usb_device_id_column)->text();
      string_type id = irs::str_conv<string_type>(id_str);
      usb_hid_device_path_map_t::map_type::const_iterator it =
        m_usb_devices.find(id);
      if (it != m_usb_devices.end()) {
        devices.push_back(it->second.path);
      }

      ++row_it;
    }
  }
  return devices;
}

mxip_t MainWindow::str_to_mxip_default(const QString& a_str,
  const mxip_t& a_default_ip)
{
  mxip_t ip = mxip_t::any_ip();
  if (!str_to_mxip(a_str, &ip)) {
    ip = a_default_ip;
  }
  return ip;
}

void MainWindow::on_devicesTableWidget_cellChanged(int /*row*/, int /*column*/)
{
  update_buttons_configuration();
}

void MainWindow::on_verifyPushButton_clicked()
{
  read_settings_from_ui();
  const std::vector<device_table_item_t> devices = get_selected_devices();
  if (devices.empty()) {
    return;
  }
  mxip_t ip = /*irs::make_mxip(127,0,0,1);*/
    devices[0].device_info.device_configuration.ip;
  const hfftp_settings_t hfftp_settings = mp_settings->get_hfftp_settings();
  /*QString file_name = ui->localFileNameLineEdit->text();*/
  if (hfftp_settings.local_file_name.empty()) {
    return;
  }
  UpdateDialog dialog(this, update_dialog_mode_verify, ip,
    string_type(), hfftp_settings);
  dialog.exec();
}

void MainWindow::on_programUSBDevicePushButton_clicked()
{
  read_settings_from_ui();
  const std::vector<string_type> devices = get_selected_usb_devices();
  if (devices.empty()) {
    return;
  }
  hfftp_settings_t hfftp_settings = mp_settings->get_hfftp_settings();
  if (hfftp_settings.local_file_name.empty()) {
    return;
  }

  hfftp_settings.transport_protocol = tp_usb_hid;
  UpdateDialog dialog(this, update_dialog_mode_program, mxip_t::zero_ip(),
    devices[0], hfftp_settings);
  dialog.exec();
}

void MainWindow::on_verifyUSBDevicePushButton_clicked()
{
  read_settings_from_ui();
  const std::vector<string_type> devices = get_selected_usb_devices();
  if (devices.empty()) {
    return;
  }
  hfftp_settings_t hfftp_settings = mp_settings->get_hfftp_settings();
  if (hfftp_settings.local_file_name.empty()) {
    return;
  }

  hfftp_settings.transport_protocol = tp_usb_hid;
  UpdateDialog dialog(this, update_dialog_mode_verify, mxip_t::zero_ip(),
    devices[0], hfftp_settings);
  dialog.exec();
}

void MainWindow::on_usbDevicesTableWidget_itemSelectionChanged()
{
  update_usb_buttons_configuration();
}
