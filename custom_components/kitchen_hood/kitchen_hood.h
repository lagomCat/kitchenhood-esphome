#pragma once

#include "esphome.h"
#include "esphome/components/uart/uart.h"

namespace esphome {
namespace kitchen_hood {

struct FrameStep {
  uint8_t byte;
  uint16_t pause_bits; // пауза после этого байта в битах (0 — паузы нет)
};

class KitchenHood : public Component, public uart::UARTDevice {
 public:
  explicit KitchenHood(uart::UARTComponent *parent);

  void setup() override;
  void loop() override;

  void press_power_on();
  void press_power_off();

  void start_sequence(const std::vector<FrameStep>& seq);

  void set_standby_sequence(const std::vector<FrameStep> &seq) { standby_seq_ = seq; }
  void set_poweron_sequence(const std::vector<FrameStep> &seq) { poweron_seq_ = seq; }
  void set_button_poweron_sequence(const std::vector<FrameStep> &seq) { button_poweron_seq_ = seq; }
  void set_button_poweroff_sequence(const std::vector<FrameStep> &seq) { button_poweroff_seq_ = seq; }
  void set_bit_duration_ms(uint32_t ms) { bit_duration_ms_ = ms; }
  void set_start_sequence(const std::vector<FrameStep> &seq) { start_seq_ = seq; }

  static KitchenHood *instance;

 protected:
  enum State { BOOT_SEND, STANDBY, SENDING_BUTTON, POWERON };

  State state_;
  unsigned long last_ts_ms_;
  unsigned long next_send_ts_ms_;
  uint32_t bit_duration_ms_ = 2;

  std::vector<FrameStep> start_seq_;
  std::vector<FrameStep> standby_seq_;
  std::vector<FrameStep> poweron_seq_;
  std::vector<FrameStep> button_poweron_seq_;
  std::vector<FrameStep> button_poweroff_seq_;

  int boot_repeats_sent_ = 0;
  int boot_repeats_target_ = 2;

  bool request_poweron_ = false;
  bool request_poweroff_ = false;

  enum LastButtonAction { NONE, POWER_ON, POWER_OFF };
  LastButtonAction last_button_action_ = NONE;

  const std::vector<FrameStep>* current_seq_ = nullptr;
  size_t current_seq_index_ = 0;
  unsigned long next_byte_ts_ms_ = 0;
  bool sequence_active_ = false;
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