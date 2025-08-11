#include "kitchen_hood.h"

namespace esphome {
namespace kitchen_hood {

void KitchenHoodComponent::setup() {
  auto *hood = new KitchenHood(this->uart_);
  App.register_component(hood);
  KitchenHood::instance = hood;

  // Общий заголовок
  hood->set_header_sequence({{0x80, true}});

  // Последовательности запуска (BOOT_SEND)
  hood->set_start1_sequence({{0x55, false}, {0x55, false}, {0x55, false}, {0xFD, false}});
  hood->set_start2_sequence({{0x55, false}, {0x55, false}, {0x55, false}, {0xFD, false}});
  hood->set_start3_sequence({{0x57, false}, {0x55, false}, {0x77, true}, {0x77, true}, {0xF7, false}});
  hood->set_start4_sequence({{0x57, false}, {0x55, false}, {0x77, true}, {0x77, true}, {0xF7, false}});

  // ------------------------
  // Массивы состояния
  // ------------------------
  hood->set_motor_speed0_sequence({{0x55, false}, {0xD5, false}, {0x77, true}, {0x77, true}, {0xF7, false}});
  hood->set_motor_speed1_sequence({{0x55, true}, {0xD5, false}, {0x77, true}, {0xDD, false}, {0xF7, false}});
  hood->set_motor_speed2_sequence({{0x55, false}, {0x57, true}, {0x77, true}, {0xD7, false}, {0xF7, false}});
  hood->set_motor_speed3_sequence({{0x55, true}, {0x57, true}, {0x77, true}, {0x75, true}, {0xFF, false}});

  hood->set_motor_speed0_light_sequence({{0x5D, false}, {0xD5, false}, {0xD7, false}, {0x77, true}, {0xD7, false}});
  hood->set_motor_speed1_light_sequence({{0x5D, false}, {0x57, true}, {0xD7, false}, {0xD7, false}, {0xD7, false}});
  hood->set_motor_speed2_light_sequence({{0x5D, false}, {0x5D, true}, {0xD7, false}, {0x77, false}, {0xD7, false}});
  hood->set_motor_speed3_light_sequence({{0x5D, false}, {0x77, false}, {0x77, false}, {0x77, false}, {0x5D, false}});

  // ------------------------
  // Коды кнопок
  // ------------------------
  hood->set_button_motor_speed0_sequence({{0x57, false}, {0x55, false}, {0x77, true}, {0x77, true}, {0xF7, false}});
  hood->set_button_motor_speed1_sequence({{0x57, false}, {0x57, false}, {0xDD, false}, {0xD7, false}, {0x77, false}});
  hood->set_button_motor_speed2_sequence({{0x57, false}, {0x5D, false}, {0xDD, false}, {0x77, false}, {0x77, false}});
  hood->set_button_motor_speed3_sequence({{0x57, false}, {0x77, false}, {0x75, true}, {0x57, true}, {0xF7, false}});
}

}  // namespace kitchen_hood
}  // namespace esphome
