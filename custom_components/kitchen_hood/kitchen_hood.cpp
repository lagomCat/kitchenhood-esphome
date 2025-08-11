#include "kitchen_hood.h"
#include "driver/gpio.h"
#include "driver/uart.h"

namespace esphome {
namespace kitchen_hood {

KitchenHood *KitchenHood::instance = nullptr;

KitchenHood::KitchenHood(uart::UARTComponent *parent) : uart::UARTDevice(parent) {
  state_ = BOOT_SEND;
  instance = this;
}

void KitchenHood::setup() {
    // Сразу синхронизируем HA-переключатели при старте
  if (light_switch_) {
    light_switch_->publish_state(light_on_);
  }
  if (sound_switch_) {
    sound_switch_->publish_state(sound_on_);
  }
}

void KitchenHood::set_light(bool on) {
  light_on_ = on;
  if (light_switch_) {
    light_switch_->publish_state(light_on_); // обновляем в HA
  }
}

void KitchenHood::set_sound(bool on) {
  sound_on_ = on;
  if (sound_switch_) {
    sound_switch_->publish_state(sound_on_); // обновляем в HA
  }
}

// Кнопка выбора скорости (0..3)
void KitchenHood::press_motor_speed(uint8_t speed) {
  if (speed > 3) return;
  motor_speed_ = speed;
  speed_changed_ = true;
}

void KitchenHood::loop() {
  switch (state_) {
    case BOOT_SEND:
      if (boot_repeats_sent_ < 1) {
        kitchen_hood_uart_init();
        this->send_sequence_with_pauses(header_seq_);
        this->send_sequence_with_pauses(start1_seq_);
        delay(18);
        this->send_sequence_with_pauses(header_seq_);
        this->send_sequence_with_pauses(start2_seq_);
        delay(18);
        this->send_sequence_with_pauses(header_seq_);
        this->send_sequence_with_pauses(start3_seq_);
        delay(18);
        this->send_sequence_with_pauses(header_seq_);
        this->send_sequence_with_pauses(start4_seq_);
        delay(18);
        boot_repeats_sent_++;
      } else {
        state_ = MOTOR_SPEED0; // Начнём с выключенного мотора
      }
      break;

    default: {
      // Выбор нового состояния
      State new_state;
      if (!light_on_) {
        new_state = static_cast<State>(MOTOR_SPEED0 + motor_speed_);
      } else {
        new_state = static_cast<State>(MOTOR_SPEED0_LIGHT + motor_speed_);
      }

      // Если изменилась скорость — при включенном звуке отправляем код кнопки
      if (speed_changed_) {
        if (sound_on_) {
          this->send_sequence_with_pauses(header_seq_);
          switch (motor_speed_) {
            case 0: this->send_sequence_with_pauses(button_motor_speed0_seq_); break;
            case 1: this->send_sequence_with_pauses(button_motor_speed1_seq_); break;
            case 2: this->send_sequence_with_pauses(button_motor_speed2_seq_); break;
            case 3: this->send_sequence_with_pauses(button_motor_speed3_seq_); break;
          }
          delay(18);
        }
        speed_changed_ = false;
      }

      // Отправка текущего режима
      this->send_sequence_with_pauses(header_seq_);
      switch (new_state) {
        case MOTOR_SPEED0: this->send_sequence_with_pauses(motor_speed0_seq_); break;
        case MOTOR_SPEED1: this->send_sequence_with_pauses(motor_speed1_seq_); break;
        case MOTOR_SPEED2: this->send_sequence_with_pauses(motor_speed2_seq_); break;
        case MOTOR_SPEED3: this->send_sequence_with_pauses(motor_speed3_seq_); break;
        case MOTOR_SPEED0_LIGHT: this->send_sequence_with_pauses(motor_speed0_light_seq_); break;
        case MOTOR_SPEED1_LIGHT: this->send_sequence_with_pauses(motor_speed1_light_seq_); break;
        case MOTOR_SPEED2_LIGHT: this->send_sequence_with_pauses(motor_speed2_light_seq_); break;
        case MOTOR_SPEED3_LIGHT: this->send_sequence_with_pauses(motor_speed3_light_seq_); break;
        default: break;
      }
      delay(18);

      state_ = new_state;
    }
    break;
  }
}

void KitchenHood::kitchen_hood_uart_init() {
  const uart_port_t uart_num = UART_NUM_1;
  uart_set_line_inverse(uart_num, UART_SIGNAL_TXD_INV);
  delayMicroseconds(96);
}

void KitchenHood::send_sequence_with_pauses(const std::vector<ByteWithPause> &sequence) {
  for (const auto &bp : sequence) {
    this->write_byte(bp.first);
    this->flush();
    if (bp.second) {
      delayMicroseconds(pause_duration_);
    }
  }
}

}  // namespace kitchen_hood
}  // namespace esphome
