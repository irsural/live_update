#ifndef UPDATEDIALOG_H
#define UPDATEDIALOG_H

#include <irsdefs.h>

#include <irsnetdefs.h>
#include <irshfftp.h>

#include <QDialog>
#include <QTimer>

#include "settings.h"

#include <irsfinal.h>

namespace Ui {
class UpdateDialog;
}

enum update_dialog_mode_t {
  update_dialog_mode_program,
  update_dialog_mode_verify
};

class UpdateDialog : public QDialog
{
  Q_OBJECT

public:
  typedef size_t size_type;
  typedef irs::string_t string_type;
  explicit UpdateDialog(QWidget *parent, update_dialog_mode_t a_mode,
    const mxip_t& a_ip,             // Только для Ethernet-устройств
    const string_type& a_device_path, // Только для USB-устройств
    const hfftp_settings_t& a_hfftp_settings);
  ~UpdateDialog();
private slots:
  void tick();
private:
  //typedef std::size_t size_type;
  //typedef irs::string_t string_type;
  enum {
    file_max_size = 0xFFFFFFFF
  };
  enum process_t {
    process_wait_program,
    process_delay_before_read_file,
    process_wait_read_file,
    process_wait_before_correct_closing,
    process_wait_commands
  };
  void read_file_to_buffer(const string_type& a_file_name);
  bool is_buffers_equals(const irs_u8* ap_buf_first, size_type a_buf_first_size,
    const irs_u8* ap_buf_second, size_type a_buf_second_size);
  Ui::UpdateDialog *ui;
  QTimer m_timer;
  mxip_t m_ip;

  const hfftp_settings_t m_hfftp_settings;
  irs::handle_t<irs::hfftp::memory_file_read_only_t> mp_memory_file_read_only;
  irs::handle_t<irs::hfftp::memory_file_write_only_t> mp_memory_file_write_only;
  irs::handle_t<irs::hardflow_t> mp_hfftp_client_hardflow;
  irs::handle_t<irs::hfftp::client_t> mp_hfftp_client;
  std::vector<irs_u8> m_buffer;
  irs::timer_t m_delay_before_read_file;
  irs::timer_t m_delay_before_destroy_hfttp;
  process_t m_process;
};

#endif // UPDATEDIALOG_H
