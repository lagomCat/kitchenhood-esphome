#include "kitchen_hood.h"
#include "driver/gpio.h"
#include "driver/uart.h"

namespace esphome {
namespace kitchen_hood {

// Статический указатель на экземпляр класса для доступа из lambda-выражений
// Это позволяет вызывать методы класса из YAML конфигурации ESPHome
KitchenHood *KitchenHood::instance = nullptr;

// Конструктор класса KitchenHood
// Инициализирует состояние и устанавливает статический указатель
KitchenHood::KitchenHood(uart::UARTComponent *parent) : uart::UARTDevice(parent) {
  state_ = BOOT_SEND;  // Начинаем с состояния загрузки
  instance = this;      // Устанавливаем статический указатель
  
  // Используем константы вместо магических чисел
  pause_duration_ = KitchenHood::DEFAULT_PAUSE_DURATION;
  boot_repeats_sent_ = 0;
}

// Метод setup вызывается ESPHome при инициализации компонента
// Здесь мы синхронизируем состояние переключателей Home Assistant
void KitchenHood::setup() {
  // Сразу синхронизируем состояние переключателей Home Assistant при старте
  // Это предотвращает рассинхронизацию после перезагрузки ESP32
  if (light_switch_) {
    light_switch_->publish_state(light_on_);
  }
  if (sound_switch_) {
    sound_switch_->publish_state(sound_on_);
  }

  xTaskCreatePinnedToCore(
  [](void*) {
    for (;;) {
      if (kitchen_hood::KitchenHood::instance) {
        kitchen_hood::KitchenHood::instance->loop_task();
      }
      vTaskDelay(pdMS_TO_TICKS(10));
    }
  },
  "hood_loop", 4096, nullptr, 1, nullptr, 1
);

}

// Метод для управления подсветкой кухонной вытяжки
// Вызывается при изменении состояния переключателя в Home Assistant
void KitchenHood::set_light(bool on) {
  light_on_ = on;  // Обновляем внутреннее состояние
  
  // Синхронизируем состояние с Home Assistant
  if (light_switch_) {
    light_switch_->publish_state(light_on_);
  }
}

// Метод для управления звуковым сопровождением кухонной вытяжки
// Вызывается при изменении состояния переключателя в Home Assistant
void KitchenHood::set_sound(bool on) {
  sound_on_ = on;  // Обновляем внутреннее состояние
  
  // Синхронизируем состояние с Home Assistant
  if (sound_switch_) {
    sound_switch_->publish_state(sound_on_);
  }
}

// Метод для выбора скорости мотора кухонной вытяжки
// Вызывается при нажатии кнопок скорости в Home Assistant
// Параметр speed: 0 = выключен, 1 = низкая, 2 = средняя, 3 = максимальная
void KitchenHood::press_motor_speed(uint8_t speed) {
  // Проверяем корректность скорости с использованием константы
  if (speed > KitchenHood::MAX_SPEED) return;
  
  motor_speed_ = speed;      // Устанавливаем новую скорость
  speed_changed_ = true;     // Отмечаем, что скорость изменилась
}

// Основной цикл работы компонента
// Управляет состояниями и отправляет соответствующие UART команды
//Пустой. Будем запускать в отдельном Таске.
void KitchenHood::loop() {}


void KitchenHood::loop_task() {
  switch (state_) {
    case BOOT_SEND:
      // Отправляем последовательности загрузки заданное количество раз
      if (boot_repeats_sent_ < KitchenHood::BOOT_REPEATS) {
        kitchen_hood_uart_init();
        this->send_sequence_with_pauses(header_seq_);
        this->send_sequence_with_pauses(start1_seq_);
        delay(KitchenHood::FRAME_DELAY);
        this->send_sequence_with_pauses(header_seq_);
        this->send_sequence_with_pauses(start2_seq_);
        delay(KitchenHood::FRAME_DELAY);
        this->send_sequence_with_pauses(header_seq_);
        this->send_sequence_with_pauses(start3_seq_);
        delay(KitchenHood::FRAME_DELAY);
        this->send_sequence_with_pauses(header_seq_);
        this->send_sequence_with_pauses(start4_seq_);
        delay(KitchenHood::FRAME_DELAY);
        boot_repeats_sent_++;
      } else {
        state_ = MOTOR_SPEED0; // Начинаем с выключенного мотора
      }
      break;

    default: {
      // Выбор нового состояния на основе текущих настроек
      State new_state;
      if (!light_on_) {
        new_state = static_cast<State>(MOTOR_SPEED0 + motor_speed_);
      } else {
        new_state = static_cast<State>(MOTOR_SPEED0_LIGHT + motor_speed_);
      }

      // Если изменилась скорость и включен звук - отправляем код кнопки
      if (speed_changed_) {
        if (sound_on_) {
          this->send_sequence_with_pauses(header_seq_);
          switch (motor_speed_) {
            case 0: this->send_sequence_with_pauses(button_motor_speed0_seq_); break;
            case 1: this->send_sequence_with_pauses(button_motor_speed1_seq_); break;
            case 2: this->send_sequence_with_pauses(button_motor_speed2_seq_); break;
            case 3: this->send_sequence_with_pauses(button_motor_speed3_seq_); break;
          }
          delay(KitchenHood::FRAME_DELAY);
        }
        speed_changed_ = false;
      }

      // Отправляем текущий режим работы
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
      delay(KitchenHood::FRAME_DELAY);

      state_ = new_state;
    }
    break;
  }
}






// Метод для инициализации UART для работы с кухонной вытяжкой
// Настраивает специальные параметры UART, необходимые для протокола вытяжки
void KitchenHood::kitchen_hood_uart_init() {
  const uart_port_t uart_num = UART_NUM_1;  // Используем UART1
  
  // Включаем инверсию сигнала TXD - это необходимо для протокола вытяжки
  // Инвертированный сигнал обеспечивает правильную логику уровней
  uart_set_line_inverse(uart_num, UART_SIGNAL_TXD_INV);
  
  // Небольшая задержка для стабилизации сигнала после изменения настроек
  delayMicroseconds(96);
}

// Метод для отправки последовательности байтов с паузами
// Это основной метод для отправки команд на кухонную вытяжку
// Параметр sequence: вектор пар {байт, нужна_ли_пауза_после_байта}
void KitchenHood::send_sequence_with_pauses(const std::vector<ByteWithPause> &sequence) {
  // Проходим по всем байтам в последовательности
  for (const auto &bp : sequence) {
    // Отправляем байт
    this->write_byte(bp.first);
    this->flush();               // Принудительно отправляем данные
    
    // Если нужна пауза после этого байта
    if (bp.second) {
      // Используем стандартную функцию delayMicroseconds для точных пауз
      // Это то, что работало в оригинальном коде и обеспечивает точность
      delayMicroseconds(pause_duration_);
    }
  }
}

}  // namespace kitchen_hood
}  // namespace esphome
