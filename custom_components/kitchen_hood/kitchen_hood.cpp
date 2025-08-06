#include "kitchen_hood.h"

namespace esphome {
namespace kitchen_hood {

KitchenHood *KitchenHood::instance = nullptr;

KitchenHood::KitchenHood(uart::UARTComponent *parent) : uart::UARTDevice(parent) {
  state_ = BOOT_SEND;
  instance = this;
}

void KitchenHood::setup() {}

void KitchenHood::press_power_on() { request_poweron_ = true; }
void KitchenHood::press_power_off() { request_poweroff_ = true; }

void KitchenHood::loop() {
  switch (state_) {
    case BOOT_SEND:
      if (boot_repeats_sent_ < 2) {
        this->write_array(start_seq_.data(), start_seq_.size());
        boot_repeats_sent_++;
      } else {
        state_ = STANDBY;
      }
      break;

    case STANDBY:
      if (request_poweron_) {
        this->write_array(button_poweron_seq_.data(), button_poweron_seq_.size());
        request_poweron_ = false;
        state_ = POWERON;
      } else {
        this->write_array(standby_seq_.data(), standby_seq_.size());
      }
      break;

    case POWERON:
      if (request_poweroff_) {
        this->write_array(button_poweroff_seq_.data(), button_poweroff_seq_.size());
        request_poweroff_ = false;
        state_ = STANDBY;
      } else {
        this->write_array(poweron_seq_.data(), poweron_seq_.size());
      }
      break;
  }
}

}  // namespace kitchen_hood
}  // namespace esphome