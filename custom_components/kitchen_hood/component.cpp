#include "kitchen_hood.h"

namespace esphome {
namespace kitchen_hood {

void KitchenHoodComponent::setup() {
  auto *hood = new KitchenHood(this->uart_);
  App.register_component(hood);
  KitchenHood::instance = hood;

  hood->set_bit_duration_ms(2);

  // Пример: пауза 2 бита после третьего байта
  hood->set_start_sequence({
    {0xA8, 0}, {0x55, 0}, {0x44, 0}, {0x44, 0}, {0x44, 0}
  });
  hood->set_standby_sequence({
    {0xAA, 0}, {0xAA, 0}, {0xAA, 0}, {0xAA, 0}, {0xAA, 0}, {0xAA, 0}
  });
  hood->set_poweron_sequence({
    {0x80, 0}, {0x55, 0}, {0xD5, 0}, {0x77, 0}, {0xDD, 0}, {0xF7, 0}, {0xFF, 0}
  });
  hood->set_button_poweron_sequence({
    {0xBB, 0}, {0xBB, 0}, {0xBB, 0}, {0xBB, 0}, {0xBB, 0}, {0xBB, 0}, {0xBB, 0}
  });
  hood->set_button_poweroff_sequence({
    {0xCC, 0}, {0xCC, 0}, {0xCC, 0}, {0xCC, 0}, {0xCC, 0}, {0xCC, 0}, {0xCC, 0}
  });
}

}  // namespace kitchen_hood
}  // namespace esphome