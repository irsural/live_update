#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QFileDialog>

#include "settings.h"

#include <irshfftp.h>
#include <irsstrmstd.h>
#include <irsdcp.h>

class usb_hid_device_path_map_t
{
public:
  typedef irs_size_t size_type;
  typedef irs::string_t string_type;
  struct device_info_t
  {
    string_type product;
    string_type path;
    bool operator==(const device_info_t& a_device_info) const
    {
      return (product == a_device_info.product) &&
        (path == a_device_info.path);
    }
  };

  typedef std::map<string_type, device_info_t> map_type;
  usb_hid_device_path_map_t();
  const map_type& get_map() const;
  bool changed();
  void tick();
private:
  void update();
  const string_type m_manufacturer;
  const string_type m_product;
  map_type m_map;
  bool m_changed;
  irs::loop_timer_t m_update_timer;
};

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  typedef std::size_t size_type;
  typedef irs::string_t string_type;
  typedef irs::hardflow::udp_flow_t::configuration_t udp_config_type;
  typedef irs::dcp::client_t::devices_type devices_type;
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();
private slots:
  void tick();
  void on_programPushButton_clicked();

  void on_searchDevicesPushButton_clicked();

  void on_offerIPPushButton_clicked();

  void on_deleteDevicesPushButton_clicked();

  void on_selectFilePushButton_clicked();

  void on_devicesTableWidget_itemSelectionChanged();

  void on_devicesTableWidget_cellChanged(int row, int column);

  void on_verifyPushButton_clicked();

  void on_programUSBDevicePushButton_clicked();

  void on_verifyUSBDevicePushButton_clicked();

  void on_usbDevicesTableWidget_itemSelectionChanged();

private:
  struct device_table_item_t
  {
    mxmac_t mac;
    irs::dcp::device_info_t device_info;
    mxip_t offer_ip;
    mxip_t offer_netmask;
    mxip_t offer_gateway;
    bool offer_dhcp_enabled;
    device_table_item_t():
      device_info(),
      offer_ip(mxip_t::any_ip()),
      offer_netmask(mxip_t::any_ip()),
      offer_gateway(mxip_t::any_ip()),
      offer_dhcp_enabled(false)
    {
    }
  };
  void read_settings_from_ui();
  void start_dcp();
  void client_changet_event(const irs::hfftp::client_t& a_client);
  bool is_ip_valid(const QString& a_str);
  bool is_mac_valid(const QString& a_str);
  std::vector<device_table_item_t> get_selected_devices();
  std::vector<string_type> get_selected_usb_devices();
  mxip_t str_to_mxip_default(const QString& a_str, const mxip_t& a_default_ip);
  udp_config_type make_dcp_client_udp_flow_configuration();
  udp_config_type make_dcp_server_udp_flow_configuration();
  void update_ethernet_devices_table();
  void update_usb_device_table();
  void update_buttons_configuration();
  void update_usb_buttons_configuration();
  enum {
    device_mac_column = 0,
    device_type_column,
    device_hardware_version_column,
    device_firmware_version_column,
    device_ip_column,
    device_netmask_column,
    device_gateway_column,
    offer_ip_column,
    offer_netmask_column,
    offer_gateway_column,
    column_count
  };

  enum {
    usb_device_id_column = 0,
    usb_device_type_column,
    usb_device_hardware_version_column,
    usb_device_firmware_version_column,
    usb_device_column_count
  };

  Ui::MainWindow *ui;
  settings_t* mp_settings;
  irs::union_streambuf m_stream_buf;
  irs::handle_t <ofstream> mp_log_stream;
  enum { plain_text_edit_buf_size = 500 };
  irs::handle_t<irs::plain_text_edit_buf> mp_plain_text_edit_buf;
  QTimer m_timer;
  irs::handle_t<irs::hardflow::udp_flow_t> mp_client_dcp_udp_hardflow;
  irs::handle_t<irs::hardflow::udp_flow_t> mp_server_dcp_udp_hardflow;
  irs::handle_t<irs::dcp::client_t> mp_dcp_client;
  devices_type m_devices;
  irs::loop_timer_t m_update_devices_loop_timer;

  usb_hid_device_path_map_t m_usb_hid_device_path_map;
  usb_hid_device_path_map_t::map_type m_usb_devices;
  //client_mode_t m_client_mode;
};

#endif // MAINWINDOW_H
