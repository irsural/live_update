
#include <irssysutils.h>

#include <QSettings>

#include "settings.h"


irs::string_t value_to_str(transport_protocol_t a_transport_protocol)
{
  switch (a_transport_protocol) {
    case tp_udp: {
      return irst("UDP");
    }
    case tp_tcp: {
      return irst("TCP");
    }
  }
  return irs::string_t();
}

transport_protocol_t str_to_value(const irs::string_t& a_str)
{
  irs::string_t str = irs::to_upper(a_str);
  if (str == irst("UDP")) {
    return tp_udp;
  } else if (str == irst("TCP")) {
    return tp_tcp;
  }
  throw std::logic_error("Invalid value");
  return tp_udp;
}

// class settings_t
settings_t *settings_t::mp_instance = 0;

settings_t::settings_t():
  m_device_autoconfig_settings(),
  m_hfftp_settings()
{
  mp_instance = this;
}

settings_t* settings_t::instance()
{
  return mp_instance ? mp_instance : new settings_t();
}

const device_autoconfig_settings_t& settings_t::get_device_autoconfig_settings()
{
  return m_device_autoconfig_settings;
}

void settings_t::set_device_autoconfig_settings(
  const device_autoconfig_settings_t& a_device_autoconfig_settings)
{
  m_device_autoconfig_settings = a_device_autoconfig_settings;
}

const hfftp_settings_t& settings_t::get_hfftp_settings()
{
  return m_hfftp_settings;
}

void settings_t::set_hfftp_settings(
  const hfftp_settings_t& a_hfftp_settings)
{
  m_hfftp_settings = a_hfftp_settings;
}

void settings_t::save()
{
  QSettings settings("settings.ini", QSettings::IniFormat);
  settings.beginGroup("device_autoconfig_settings");
  settings.setValue("local_port",
    irs::str_conv<QString>(m_device_autoconfig_settings.local_port));
  settings.setValue("device_ip",
    irs::str_conv<QString>(m_device_autoconfig_settings.device_ip));
  settings.setValue("device_port",
    irs::str_conv<QString>(m_device_autoconfig_settings.device_port));
  settings.endGroup();

  settings.beginGroup("hfftp_settings");
  settings.setValue("transport_protocol",
    irs::str_conv<QString>(value_to_str(m_hfftp_settings.transport_protocol)));
  settings.setValue("local_port",
    irs::str_conv<QString>(m_hfftp_settings.local_port));
  settings.setValue("device_port",
    irs::str_conv<QString>(m_hfftp_settings.device_port));
  settings.setValue("block_size", m_hfftp_settings.block_size);
  settings.setValue("timeout", m_hfftp_settings.timeout);
  settings.setValue("retransmit_max_count",
    m_hfftp_settings.retransmit_max_count);
  settings.setValue("local_file_name",
    irs::str_conv<QString>(m_hfftp_settings.local_file_name));
  settings.setValue("remote_file_name",
    irs::str_conv<QString>(m_hfftp_settings.remote_file_name));
  settings.endGroup();
}

void settings_t::load()
{
  QSettings settings("settings.ini", QSettings::IniFormat);
  settings.beginGroup("device_autoconfig_settings");
  m_device_autoconfig_settings.local_port = irs::str_conv<string_type>(
    settings.value("local_port", "31001").toString());
  m_device_autoconfig_settings.device_ip = irs::str_conv<string_type>(
    settings.value("device_ip", "192.168.0.214").toString());
  m_device_autoconfig_settings.device_port = irs::str_conv<string_type>(
    settings.value("device_port", "31002").toString());
  settings.endGroup();

  settings.beginGroup("hfftp_settings");
  string_type transport_protocol_str = irs::str_conv<string_type>(
    settings.value("transport_protocol",
    irs::str_conv<QString>(value_to_str(tp_udp))).toString());
  m_hfftp_settings.transport_protocol =
    str_to_value(transport_protocol_str);
  m_hfftp_settings.local_port = irs::str_conv<string_type>(
    settings.value("local_port", "31004").toString());
  m_hfftp_settings.device_port = irs::str_conv<string_type>(
    settings.value("device_port", "31003").toString());
  m_hfftp_settings.block_size = settings.value("block_size", 1400).toUInt();
  m_hfftp_settings.timeout = settings.value("timeout", 3).toDouble();
  m_hfftp_settings.retransmit_max_count =
    settings.value("retransmit_max_count", 3).toUInt();
  m_hfftp_settings.local_file_name = irs::str_conv<string_type>(
    settings.value("local_file_name").toString());
  m_hfftp_settings.remote_file_name = irs::str_conv<string_type>(
    settings.value("remote_file_name").toString());
  settings.endGroup();
}
