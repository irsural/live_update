#ifndef SETTINGS_H
#define SETTINGS_H

#include <irsdefs.h>

#include <vector>

#include <irsstrdefs.h>
#include <irsstrconv.h>

#include <irsfinal.h>

/*struct server_settings_t
{
  typedef std::size_t size_type;
  typedef irs::string_t string_type;
  string_type port;
  size_type block_size;
  double timeout;
  size_type retransmit_max_count;
  server_settings_t():
    port(irst("5005")),
    block_size(512),
    timeout(3),
    retransmit_max_count(3)
  {
  }
};*/

enum transport_protocol_t {
  tp_udp,
  tp_tcp,
  tp_usb_hid
};

irs::string_t value_to_str(transport_protocol_t a_transport_protocol);
transport_protocol_t str_to_value(const irs::string_t& a_str);

struct device_autoconfig_settings_t
{
  typedef irs::string_t string_type;
  string_type local_port;
  string_type device_ip;
  string_type device_port;
  device_autoconfig_settings_t():
    local_port(irst("31001")),
    device_ip(irst("192.168.0.214")),
    device_port(irst("31002"))
  {
  }
};

struct hfftp_settings_t
{
  typedef std::size_t size_type;
  typedef irs::string_t string_type;
  transport_protocol_t transport_protocol;
  string_type local_port;
  string_type device_port;
  size_type block_size;
  double timeout;
  size_type retransmit_max_count;
  string_type local_file_name;
  string_type remote_file_name;
  hfftp_settings_t():
    transport_protocol(tp_udp),
    local_port(irst("31004")),
    device_port(irst("31003")/*,irst("31012")*/),
    block_size(512),
    timeout(3),
    retransmit_max_count(3),
    local_file_name(),
    remote_file_name(irst("program.img"))
  {
  }
};

class settings_t
{
public:
  typedef std::size_t size_type;
  typedef irs::string_t string_type;
  static settings_t* instance();
  const device_autoconfig_settings_t& get_device_autoconfig_settings();
  void set_device_autoconfig_settings(
    const device_autoconfig_settings_t& a_device_autoconfig_settings);
  const hfftp_settings_t& get_hfftp_settings();
  void set_hfftp_settings(const hfftp_settings_t& a_hfftp_settings);
  void save();
  void load();
private:
  settings_t();
  static settings_t* mp_instance;
  device_autoconfig_settings_t m_device_autoconfig_settings;
  hfftp_settings_t m_hfftp_settings;
};

#endif // SETTINGS_H
