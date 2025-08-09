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

void KitchenHood::setup() {}

void KitchenHood::press_power_on() { request_poweron_ = true; }
void KitchenHood::press_power_off() { request_poweroff_ = true; }



// ====== ОСНОВНОЙ ЦИКЛ ======
void KitchenHood::loop() {
  switch (state_) {
    case BOOT_SEND:
      if (boot_repeats_sent_ < 1) {
        kitchen_hood_uart_init();
        this->write_array(start1_seq_.data(), start1_seq_.size());
        this->flush();           // Ждём завершения передачи
        delay(18);
        this->write_array(start2_seq_.data(), start2_seq_.size());
        this->flush();           // Ждём завершения передачи
        delay(18);
        this->write_array(start3_seq_.data(), start3_seq_.size());
        this->flush();           // Ждём завершения передачи
        delay(18);
        this->write_array(start4_seq_.data(), start4_seq_.size());
        this->flush();           // Ждём завершения передачи 
        delay(18);       
        boot_repeats_sent_++;
      } else {
        state_ = STANDBY;
      }
      break;

    case STANDBY:
      if (request_poweron_) {
        this->write_array(button_poweron_seq_.data(), button_poweron_seq_.size());
        this->flush();           // Ждём завершения передачи
        delay(18);
        request_poweron_ = false;
        state_ = POWERON;
      } else {
        this->write_array(standby_seq_.data(), standby_seq_.size());
        this->flush();           // Ждём завершения передачи
        delay(18);
      }
      break;

    case POWERON:
      if (request_poweroff_) {
        this->write_array(button_poweroff_seq_.data(), button_poweroff_seq_.size());
        this->flush();           // Ждём завершения передачи
        delay(18);
        request_poweroff_ = false;
        state_ = STANDBY;
      } else {
        this->write_array(poweron_seq_.data(), poweron_seq_.size());
        this->flush();           // Ждём завершения передачи
        delay(18);
      }
      break;
  }
}

void KitchenHood::kitchen_hood_uart_init() {  
  const uart_port_t uart_num = UART_NUM_1;
  uart_set_line_inverse(uart_num, UART_SIGNAL_TXD_INV);
}



}  // namespace kitchen_hood
}  // namespace esphome