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
  ESP_LOGD("kitchen_hood", "KitchenHood setup");
}

unsigned long KitchenHood::sequence_duration_ms(const std::vector<uint8_t> &seq) {
  if (seq.empty()) return 0;
  return seq.size() * 10UL * bit_duration_ms_;
}

void KitchenHood::transmit_sequence(const std::vector<uint8_t> &seq) {
  if (seq.empty()) return;
  this->write_array(seq.data(), seq.size());
  log_hex(seq);

  unsigned long dur = sequence_duration_ms(seq);
  unsigned long pause_ms = inter_frame_bits_ * bit_duration_ms_;
  next_send_ts_ms_ = millis() + dur + pause_ms;
  ESP_LOGD("kitchen_hood", "transmit_sequence: bytes=%d duration=%lu ms pause=%lu ms next=%lu",
           (int)seq.size(), dur, pause_ms, next_send_ts_ms_);
}

void KitchenHood::log_hex(const std::vector<uint8_t> &seq) {
  std::string s;
  char buf[8];
  for (auto b : seq) {
    sprintf(buf, " %02X", b);
    s += buf;
  }
  ESP_LOGD("kitchen_hood", "TX:%s", s.c_str());
}

void KitchenHood::loop() {
		
  unsigned long now = millis();

  switch (state_) {
    case BOOT_SEND: {
      if (boot_repeats_sent_ < boot_repeats_target_) {
        if (now >= next_send_ts_ms_) {
          transmit_sequence(start_seq_);
          boot_repeats_sent_++;
        }
      } else {
        state_ = STANDBY;
        next_send_ts_ms_ = now;
        ESP_LOGD("kitchen_hood", "Boot done -> STANDBY");
      }
      break;
    }

    case STANDBY: {
      if (now >= next_send_ts_ms_) {
        if (request_poweron_) {
          transmit_sequence(standby_seq_);
          state_ = SENDING_BUTTON;
          ESP_LOGD("kitchen_hood", "STANDBY -> will send button_poweron");
        } else if (request_poweroff_) {
          transmit_sequence(standby_seq_);
          state_ = SENDING_BUTTON;
          ESP_LOGD("kitchen_hood", "STANDBY -> will send button_poweroff");
        } else {
          transmit_sequence(standby_seq_);
        }
      }
      break;
    }

    case SENDING_BUTTON: {
      if (now >= next_send_ts_ms_) {
        if (request_poweron_) {
          transmit_sequence(button_poweron_seq_);
          request_poweron_ = false;
          state_ = POWERON;
          next_send_ts_ms_ = millis();
          ESP_LOGD("kitchen_hood", "Sent button_poweron -> POWERON");
        } else if (request_poweroff_) {
          transmit_sequence(button_poweroff_seq_);
          request_poweroff_ = false;
          state_ = STANDBY;
          next_send_ts_ms_ = millis();
          ESP_LOGD("kitchen_hood", "Sent button_poweroff -> STANDBY");
        } else {
          state_ = STANDBY;
        }
      }
      break;
    }

    case POWERON: {
      if (now >= next_send_ts_ms_) {
        if (request_poweroff_) {
          transmit_sequence(poweron_seq_); // Сначала завершаем текущую итерацию
          state_ = SENDING_BUTTON;
          ESP_LOGD("kitchen_hood", "POWERON -> will send button_poweroff");
        } else {
          transmit_sequence(poweron_seq_);
        }
      }
      break;
    }
  }
}

void KitchenHood::press_power_on() {
  ESP_LOGD("kitchen_hood", "press_power_on requested");
  request_poweron_ = true;
}

void KitchenHood::press_power_off() {
  ESP_LOGD("kitchen_hood", "press_power_off requested");
  request_poweroff_ = true;
}

}  // namespace kitchen_hood
}  // namespace esphome