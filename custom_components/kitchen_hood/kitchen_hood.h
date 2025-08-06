#pragma once

#include "esphome.h"
#include "esphome/components/uart/uart.h"

namespace esphome {
namespace kitchen_hood {

class KitchenHood : public Component, public uart::UARTDevice {
 public:
  explicit KitchenHood(uart::UARTComponent *parent);
  
  void setup() override;
  void loop() override;
  
  void press_power_on();
  void press_power_off();
  
  void set_start_sequence(const std::vector<uint8_t> &seq) { start_seq_ = seq; }
  void set_standby_sequence(const std::vector<uint8_t> &seq) { standby_seq_ = seq; }
  void set_poweron_sequence(const std::vector<uint8_t> &seq) { poweron_seq_ = seq; }
  void set_button_poweron_sequence(const std::vector<uint8_t> &seq) { button_poweron_seq_ = seq; }
  void set_button_poweroff_sequence(const std::vector<uint8_t> &seq) { button_poweroff_seq_ = seq; }
  void set_inter_frame_bits(uint32_t bits) { inter_frame_bits_ = bits; }

  static KitchenHood *instance;

 protected:
  enum State { BOOT_SEND, STANDBY, SENDING_BUTTON, POWERON };

  State state_;
  unsigned long last_ts_ms_;
  unsigned long next_send_ts_ms_;
  uint32_t bit_duration_ms_ = 2;
  uint32_t inter_frame_bits_ = 0;

  std::vector<uint8_t> start_seq_;
  std::vector<uint8_t> standby_seq_;
  std::vector<uint8_t> poweron_seq_;
  std::vector<uint8_t> button_poweron_seq_;
  std::vector<uint8_t> button_poweroff_seq_;

  int boot_repeats_sent_ = 0;
  int boot_repeats_target_ = 2;

  bool request_poweron_ = false;
  bool request_poweroff_ = false;

  void transmit_sequence(const std::vector<uint8_t> &seq);
  unsigned long sequence_duration_ms(const std::vector<uint8_t> &seq);
  void log_hex(const std::vector<uint8_t> &seq);
};

class KitchenHoodComponent : public Component {
 public:
  void set_uart(uart::UARTComponent *uart) { uart_ = uart; }
  void setup() override;
  
 protected:
  uart::UARTComponent *uart_;
};

}  // namespace kitchen_hood
}  // namespace esphome