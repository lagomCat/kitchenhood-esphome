#pragma once
#include "esphome.h"
#include "esphome/components/uart/uart.h"
#include <vector>
#include <utility> // для std::pair

namespace esphome {
namespace kitchen_hood {

// Новый тип для хранения байта и флага паузы
using ByteWithPause = std::pair<uint8_t, bool>;

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

  void set_pause_duration(uint32_t duration) { pause_duration_ = duration; }

  // Обновленные методы с новым типом последовательностей
  void set_header_sequence(const std::vector<ByteWithPause> &seq) { header_seq_ = seq; }
  void set_start1_sequence(const std::vector<ByteWithPause> &seq) { start1_seq_ = seq; }
  void set_start2_sequence(const std::vector<ByteWithPause> &seq) { start2_seq_ = seq; }
  void set_start3_sequence(const std::vector<ByteWithPause> &seq) { start3_seq_ = seq; }
  void set_start4_sequence(const std::vector<ByteWithPause> &seq) { start4_seq_ = seq; }
  void set_standby_sequence(const std::vector<ByteWithPause> &seq) { standby_seq_ = seq; }
  void set_poweron_sequence(const std::vector<ByteWithPause> &seq) { poweron_seq_ = seq; }
  void set_button_poweron_sequence(const std::vector<ByteWithPause> &seq) { button_poweron_seq_ = seq; }
  void set_button_poweroff_sequence(const std::vector<ByteWithPause> &seq) { button_poweroff_seq_ = seq; }

  static KitchenHood *instance;

 protected:
  enum State { BOOT_SEND, STANDBY, POWERON };
  State state_;

  // Обновленные последовательности
  std::vector<ByteWithPause> header_seq_;
  std::vector<ByteWithPause> start1_seq_;
  std::vector<ByteWithPause> start2_seq_;
  std::vector<ByteWithPause> start3_seq_;
  std::vector<ByteWithPause> start4_seq_;
  std::vector<ByteWithPause> standby_seq_;
  std::vector<ByteWithPause> poweron_seq_;
  std::vector<ByteWithPause> button_poweron_seq_;
  std::vector<ByteWithPause> button_poweroff_seq_;

  uint32_t pause_duration_ = 4000; // Значение по умолчанию (2 бита при 500 бод)

  int boot_repeats_sent_ = 0;
  bool request_poweron_ = false;
  bool request_poweroff_ = false;

  // Новый метод для отправки последовательности с паузами
  void send_sequence_with_pauses(const std::vector<ByteWithPause> &sequence);
};
}  // namespace kitchen_hood
}  // namespace esphome