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
        state_ = STANDBY;
      }
      break;

    case STANDBY:
  // В состоянии STANDBY запрос на выключение — бессмыслен (уже выключены)
  if (request_poweroff_) {
    request_poweroff_ = false;
  }

  if (request_poweron_) {
        this->send_sequence_with_pauses(header_seq_);
        this->send_sequence_with_pauses(button_poweron_seq_);
        delay(18);
        request_poweron_ = false;
        state_ = POWERON;
      } else {
        this->send_sequence_with_pauses(header_seq_);
        this->send_sequence_with_pauses(standby_seq_);
        delay(18);
      }
      break;

    case POWERON:
  // В состоянии POWERON запрос на включение — бессмыслен (уже включены)
  if (request_poweron_) {
    request_poweron_ = false;
  }

  if (request_poweroff_) {
    this->send_sequence_with_pauses(header_seq_);
    this->send_sequence_with_pauses(button_poweroff_seq_);
    delay(18);
    request_poweroff_ = false;
    state_ = STANDBY;
  } else {
    this->send_sequence_with_pauses(header_seq_);
    this->send_sequence_with_pauses(poweron_seq_);
    delay(18);
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
  for (const auto &byte_pause : sequence) {
    this->write_byte(byte_pause.first);
    this->flush();
    
    if (byte_pause.second) {
      delayMicroseconds(pause_duration_);
    }
  }
}



}  // namespace kitchen_hood
}  // namespace esphome