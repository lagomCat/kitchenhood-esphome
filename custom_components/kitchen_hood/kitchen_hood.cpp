#include "kitchen_hood.h"

namespace esphome {
namespace kitchen_hood {

KitchenHood *KitchenHood::instance = nullptr;

KitchenHood::KitchenHood(uart::UARTComponent *parent) : uart::UARTDevice(parent) {
  state_ = BOOT_SEND;
  last_ts_ms_ = 0;
  next_send_ts_ms_ = 0;
  instance = this;
}

void KitchenHood::setup() {
  
}

void KitchenHood::start_sequence(const std::vector<FrameStep>& seq) {
  current_seq_ = &seq;
  current_seq_index_ = 0;
  next_byte_ts_ms_ = millis();
  sequence_active_ = true;
}


void KitchenHood::press_power_on() {
  request_poweron_ = true;
}

void KitchenHood::press_power_off() {
  request_poweroff_ = true;
}

void KitchenHood::loop() {
  unsigned long now = millis();

  // Отправка последовательности по одному байту за вызов loop()
  if (sequence_active_ && current_seq_ && now >= next_byte_ts_ms_) {
    if (current_seq_index_ < current_seq_->size()) {
      const auto& step = (*current_seq_)[current_seq_index_];
      this->write_byte(step.byte);
      next_byte_ts_ms_ = now + step.pause_bits * bit_duration_ms_;
      current_seq_index_++;
    } else {
      sequence_active_ = false;
      current_seq_ = nullptr;
      current_seq_index_ = 0;
      next_send_ts_ms_ = now + 10 * bit_duration_ms_;
    }
  }

  switch (state_) {
    case BOOT_SEND:
      if (!sequence_active_ && boot_repeats_sent_ < boot_repeats_target_) {
        start_sequence(start_seq_);
        boot_repeats_sent_++;
      } else if (!sequence_active_ && boot_repeats_sent_ >= boot_repeats_target_) {
        state_ = STANDBY;
        next_send_ts_ms_ = millis();
      }
      break;

    case STANDBY:
      if (!sequence_active_ && now >= next_send_ts_ms_) {
        if (request_poweron_) {
          state_ = SENDING_BUTTON;
          request_poweron_ = false;
          last_button_action_ = POWER_ON;
          start_sequence(button_poweron_seq_);
        } else {
          start_sequence(standby_seq_);
        }
      }
      break;

    case SENDING_BUTTON:
      if (!sequence_active_ && now >= next_send_ts_ms_) {
        if (last_button_action_ == POWER_ON) {
          state_ = POWERON;
        } else if (last_button_action_ == POWER_OFF) {
          state_ = STANDBY;
        }
        last_button_action_ = NONE;
        next_send_ts_ms_ = millis();
      }
      break;

    case POWERON:
      if (!sequence_active_ && now >= next_send_ts_ms_) {
        if (request_poweroff_) {
          state_ = SENDING_BUTTON;
          request_poweroff_ = false;
          last_button_action_ = POWER_OFF;
          start_sequence(button_poweroff_seq_);
        } else {
          start_sequence(poweron_seq_);
        }
      }
      break;
  }
}

}  // namespace kitchen_hood
}  // namespace esphome