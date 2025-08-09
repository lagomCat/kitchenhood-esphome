#pragma once
#include "esphome.h"
#include "esphome/components/uart/uart.h"

namespace esphome {
namespace kitchen_hood {

class KitchenHoodComponent : public Component {
 public:
  void set_uart(uart::UARTComponent *uart) { uart_ = uart; }
  void setup() override;

 protected:
  uart::UARTComponent *uart_;
};

class KitchenHood : public Component, public uart::UARTDevice {
 public:
  explicit KitchenHood(uart::UARTComponent *parent);
  
  void setup() override;
  void loop() override;

  void kitchen_hood_uart_init();

  void press_power_on();
  void press_power_off();

  void set_standby_sequence(const std::vector<uint8_t> &seq) { standby_seq_ = seq; }
  void set_poweron_sequence(const std::vector<uint8_t> &seq) { poweron_seq_ = seq; }
  void set_button_poweron_sequence(const std::vector<uint8_t> &seq) { button_poweron_seq_ = seq; }
  void set_button_poweroff_sequence(const std::vector<uint8_t> &seq) { button_poweroff_seq_ = seq; }
  void set_start1_sequence(const std::vector<uint8_t> &seq) { start1_seq_ = seq; }
  void set_start2_sequence(const std::vector<uint8_t> &seq) { start2_seq_ = seq; }
  void set_start3_sequence(const std::vector<uint8_t> &seq) { start3_seq_ = seq; }
  void set_start4_sequence(const std::vector<uint8_t> &seq) { start4_seq_ = seq; }

  static KitchenHood *instance;

 protected:
  enum State { BOOT_SEND, STANDBY, POWERON };
  State state_;

  std::vector<uint8_t> start1_seq_;
  std::vector<uint8_t> start2_seq_;
  std::vector<uint8_t> start3_seq_;
  std::vector<uint8_t> start4_seq_;
  std::vector<uint8_t> standby_seq_;
  std::vector<uint8_t> poweron_seq_;
  std::vector<uint8_t> button_poweron_seq_;
  std::vector<uint8_t> button_poweroff_seq_;

  int boot_repeats_sent_ = 0;
  bool request_poweron_ = false;
  bool request_poweroff_ = false;
};

}  // namespace kitchen_hood
}  // namespace esphome