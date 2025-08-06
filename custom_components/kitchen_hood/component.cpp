#include "kitchen_hood.h"

namespace esphome {
namespace kitchen_hood {

void KitchenHoodComponent::setup() {
  auto *hood = new KitchenHood(this->uart_);
  App.register_component(hood);
  KitchenHood::instance = hood;

  hood->set_start_sequence({0xA8, 0x55, 0x44, 0x44, 0x44});
  hood->set_standby_sequence({0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA});
  hood->set_poweron_sequence({0x80, 0x55, 0xD5, 0x77, 0xDD, 0xF7, 0xFF});
  hood->set_button_poweron_sequence({0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB});
  hood->set_button_poweroff_sequence({0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC});
}

}  // namespace kitchen_hood
}  // namespace esphome