#include "kitchen_hood.h"

namespace esphome {
namespace kitchen_hood {

void KitchenHoodComponent::setup() {
  auto *hood = new KitchenHood(this->uart_);
  App.register_component(hood);
  KitchenHood::instance = hood;

  // Теперь указываем паузы прямо в последовательностях
  // {байт, нужна_ли_пауза_после_него}
  hood->set_header_sequence({{0x80, true}});
  
  hood->set_start1_sequence({
    {0x55, false},
    {0x55, false},
    {0x55, false},
    {0xFD, false}
  });
  
  hood->set_start2_sequence({
    {0x55, false},
    {0x55, false},
    {0x55, false},
    {0xFD, false}
  });
  
  hood->set_start3_sequence({
    {0x57, false},
    {0x55, false},
    {0x77, true},
    {0x77, true},
    {0xF7, false}
  });
  
  hood->set_start4_sequence({
    {0x57, false},
    {0x55, false},
    {0x77, true},
    {0x77, true},
    {0xF7, false}
  });

  hood->set_standby_sequence({
    {0x55, false},
    {0xD5, false},
    {0x77, true},    
    {0x77, true},
    {0xF7, false}
  });
  
  hood->set_poweron_sequence({
    {0x55, true},
    {0xD5, false},
    {0x77, true},
    {0xDD, false},
    {0xF7, false}
  });
  
  hood->set_button_poweron_sequence({
    {0x57, false},
    {0x57, false},
    {0xDD, false},
    {0xD7, false},
    {0x77, false}
  });
  
  hood->set_button_poweroff_sequence({
    {0x57, false},
    {0x55, false},
    {0x77, true},
    {0x77, true},
    {0xF7, false}
  });
}

}  // namespace kitchen_hood
}  // namespace esphome