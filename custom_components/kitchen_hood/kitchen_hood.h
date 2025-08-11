#pragma once
#include "esphome.h"
#include "esphome/components/uart/uart.h"
#include <vector>
#include <utility> // std::pair

namespace esphome {
namespace kitchen_hood {

// Тип для хранения байта и признака паузы
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

  // Управление из HA
  void press_motor_speed(uint8_t speed);
  void press_light_on();
  void press_light_off();
  void press_sound_on();
  void press_sound_off();

  void kitchen_hood_uart_init();

  void set_pause_duration(uint32_t duration) { pause_duration_ = duration; }

  // Методы для задания массивов
  void set_header_sequence(const std::vector<ByteWithPause> &seq) { header_seq_ = seq; }
  void set_start1_sequence(const std::vector<ByteWithPause> &seq) { start1_seq_ = seq; }
  void set_start2_sequence(const std::vector<ByteWithPause> &seq) { start2_seq_ = seq; }
  void set_start3_sequence(const std::vector<ByteWithPause> &seq) { start3_seq_ = seq; }
  void set_start4_sequence(const std::vector<ByteWithPause> &seq) { start4_seq_ = seq; }

  void set_motor_speed0_sequence(const std::vector<ByteWithPause> &seq) { motor_speed0_seq_ = seq; }
  void set_motor_speed1_sequence(const std::vector<ByteWithPause> &seq) { motor_speed1_seq_ = seq; }
  void set_motor_speed2_sequence(const std::vector<ByteWithPause> &seq) { motor_speed2_seq_ = seq; }
  void set_motor_speed3_sequence(const std::vector<ByteWithPause> &seq) { motor_speed3_seq_ = seq; }

  void set_motor_speed0_light_sequence(const std::vector<ByteWithPause> &seq) { motor_speed0_light_seq_ = seq; }
  void set_motor_speed1_light_sequence(const std::vector<ByteWithPause> &seq) { motor_speed1_light_seq_ = seq; }
  void set_motor_speed2_light_sequence(const std::vector<ByteWithPause> &seq) { motor_speed2_light_seq_ = seq; }
  void set_motor_speed3_light_sequence(const std::vector<ByteWithPause> &seq) { motor_speed3_light_seq_ = seq; }

  void set_button_motor_speed0_sequence(const std::vector<ByteWithPause> &seq) { button_motor_speed0_seq_ = seq; }
  void set_button_motor_speed1_sequence(const std::vector<ByteWithPause> &seq) { button_motor_speed1_seq_ = seq; }
  void set_button_motor_speed2_sequence(const std::vector<ByteWithPause> &seq) { button_motor_speed2_seq_ = seq; }
  void set_button_motor_speed3_sequence(const std::vector<ByteWithPause> &seq) { button_motor_speed3_seq_ = seq; }

  static KitchenHood *instance;

 protected:
  enum State {
    BOOT_SEND,
    MOTOR_SPEED0, MOTOR_SPEED1, MOTOR_SPEED2, MOTOR_SPEED3,
    MOTOR_SPEED0_LIGHT, MOTOR_SPEED1_LIGHT, MOTOR_SPEED2_LIGHT, MOTOR_SPEED3_LIGHT
  };
  State state_;

  // Последовательности
  std::vector<ByteWithPause> header_seq_;
  std::vector<ByteWithPause> start1_seq_;
  std::vector<ByteWithPause> start2_seq_;
  std::vector<ByteWithPause> start3_seq_;
  std::vector<ByteWithPause> start4_seq_;

  std::vector<ByteWithPause> motor_speed0_seq_;
  std::vector<ByteWithPause> motor_speed1_seq_;
  std::vector<ByteWithPause> motor_speed2_seq_;
  std::vector<ByteWithPause> motor_speed3_seq_;

  std::vector<ByteWithPause> motor_speed0_light_seq_;
  std::vector<ByteWithPause> motor_speed1_light_seq_;
  std::vector<ByteWithPause> motor_speed2_light_seq_;
  std::vector<ByteWithPause> motor_speed3_light_seq_;

  std::vector<ByteWithPause> button_motor_speed0_seq_;
  std::vector<ByteWithPause> button_motor_speed1_seq_;
  std::vector<ByteWithPause> button_motor_speed2_seq_;
  std::vector<ByteWithPause> button_motor_speed3_seq_;

  uint32_t pause_duration_ = 4000; // По умолчанию — 2 бита при 500 бод

  int boot_repeats_sent_ = 0;

  // Флаги и параметры
  bool light_on_ = false;
  bool sound_on_ = false;
  uint8_t motor_speed_ = 0; // 0..3
  bool speed_changed_ = false; // Чтобы отправить код кнопки один раз

  // Отправка байтов с паузами
  void send_sequence_with_pauses(const std::vector<ByteWithPause> &sequence);
};

}  // namespace kitchen_hood
}  // namespace esphome
