#pragma once
#include "esphome.h"
#include "esphome/components/uart/uart.h"
#include <vector>
#include <utility> // std::pair
#include "esphome/components/template/switch/template_switch.h"

namespace esphome {
namespace kitchen_hood {

  // Тип для хранения байта и признака паузы
  // std::pair<uint8_t, bool> где:
  // - первый элемент - байт для отправки
  // - второй элемент - нужна ли пауза после этого байта
  using ByteWithPause = std::pair<uint8_t, bool>;

  // Основной компонент для регистрации в ESPHome
  class KitchenHoodComponent : public Component {
   public:
    // Устанавливает UART компонент для связи
    void set_uart(uart::UARTComponent *uart) { uart_ = uart; }
    void setup() override;

    protected:
     uart::UARTComponent *uart_; // Ссылка на UART компонент
};

     // Основной класс для управления кухонной вытяжкой
   // Наследуется от Component (ESPHome) и UARTDevice (для UART связи)
   class KitchenHood : public Component, public uart::UARTDevice {
   public:
    // Константы для улучшения читаемости и поддержки кода
    static constexpr uint8_t MAX_SPEED = 3;           // Максимальная скорость мотора (0-3)
    static constexpr uint8_t SPEED_COUNT = 4;          // Количество скоростей (0, 1, 2, 3)
    static constexpr uint32_t DEFAULT_PAUSE_DURATION = 4000;  // Пауза по умолчанию в микросекундах (4 мс)
    static constexpr uint32_t FRAME_DELAY = 18;        // Задержка между фреймами в миллисекундах
    static constexpr uint8_t BOOT_REPEATS = 1;         // Количество повторений при загрузке
    
    explicit KitchenHood(uart::UARTComponent *parent);

  void loop_task();

  void setup() override;
  void loop() override;

  // Управление скоростью мотора (0..3)
  // Вызывается при нажатии кнопок скорости в Home Assistant
  void press_motor_speed(uint8_t speed);

  // Управление подсветкой и звуком через переключатели Home Assistant
  void set_light(bool on);  // true = включить, false = выключить
  void set_sound(bool on);  // true = включить, false = выключить

  // Привязка переключателей Home Assistant для синхронизации состояния
  // Эти методы вызываются при инициализации для установки связи
  void bind_light_switch(esphome::template_::TemplateSwitch *sw) { light_switch_ = sw; }
  void bind_sound_switch(esphome::template_::TemplateSwitch *sw) { sound_switch_ = sw; }

  // Инициализация UART для работы с кухонной вытяжкой
  // Настраивает инверсию сигнала и другие параметры
  void kitchen_hood_uart_init();

  // Установка длительности паузы между байтами в микросекундах
  // По умолчанию 4000 мкс (4 мс) - это 2 бита при скорости 500 бод
  void set_pause_duration(uint32_t duration) { pause_duration_ = duration; }

  // Методы для задания последовательностей байтов для различных режимов
  // Каждая последовательность определяет, какие байты отправлять и где делать паузы
  
  // Заголовок - отправляется перед каждой командой
  void set_header_sequence(const std::vector<ByteWithPause> &seq) { header_seq_ = seq; }
  
  // Последовательности запуска (BOOT_SEND) - отправляются при загрузке
  void set_start1_sequence(const std::vector<ByteWithPause> &seq) { start1_seq_ = seq; }
  void set_start2_sequence(const std::vector<ByteWithPause> &seq) { start2_seq_ = seq; }
  void set_start3_sequence(const std::vector<ByteWithPause> &seq) { start3_seq_ = seq; }
  void set_start4_sequence(const std::vector<ByteWithPause> &seq) { start4_seq_ = seq; }

  // Последовательности для режимов скорости мотора (без подсветки)
  void set_motor_speed0_sequence(const std::vector<ByteWithPause> &seq) { motor_speed0_seq_ = seq; }
  void set_motor_speed1_sequence(const std::vector<ByteWithPause> &seq) { motor_speed1_seq_ = seq; }
  void set_motor_speed2_sequence(const std::vector<ByteWithPause> &seq) { motor_speed2_seq_ = seq; }
  void set_motor_speed3_sequence(const std::vector<ByteWithPause> &seq) { motor_speed3_seq_ = seq; }

  // Последовательности для режимов скорости мотора с включенной подсветкой
  void set_motor_speed0_light_sequence(const std::vector<ByteWithPause> &seq) { motor_speed0_light_seq_ = seq; }
  void set_motor_speed1_light_sequence(const std::vector<ByteWithPause> &seq) { motor_speed1_light_seq_ = seq; }
  void set_motor_speed2_light_sequence(const std::vector<ByteWithPause> &seq) { motor_speed2_light_seq_ = seq; }
  void set_motor_speed3_light_sequence(const std::vector<ByteWithPause> &seq) { motor_speed3_light_seq_ = seq; }

  // Последовательности для кодов кнопок (отправляются при изменении скорости)
  // Используются только когда включен звук для обратной связи
  void set_button_motor_speed0_sequence(const std::vector<ByteWithPause> &seq) { button_motor_speed0_seq_ = seq; }
  void set_button_motor_speed1_sequence(const std::vector<ByteWithPause> &seq) { button_motor_speed1_seq_ = seq; }
  void set_button_motor_speed2_sequence(const std::vector<ByteWithPause> &seq) { button_motor_speed2_seq_ = seq; }
  void set_button_motor_speed3_sequence(const std::vector<ByteWithPause> &seq) { button_motor_speed3_seq_ = seq; }

  // Статический указатель на экземпляр класса для доступа из lambda-выражений
  // Это позволяет вызывать методы класса из YAML конфигурации ESPHome
  static KitchenHood *instance;

 protected:
  // Перечисление состояний работы кухонной вытяжки
  enum State {
    BOOT_SEND,                    // Состояние загрузки - отправка последовательностей запуска
    MOTOR_SPEED0, MOTOR_SPEED1, MOTOR_SPEED2, MOTOR_SPEED3,           // Режимы скорости без подсветки
    MOTOR_SPEED0_LIGHT, MOTOR_SPEED1_LIGHT, MOTOR_SPEED2_LIGHT, MOTOR_SPEED3_LIGHT  // Режимы скорости с подсветкой
  };
  State state_;

  // Последовательности байтов для различных режимов работы
  // Каждая последовательность содержит пары {байт, нужна_ли_пауза}
  
  // Заголовок - отправляется перед каждой командой
  std::vector<ByteWithPause> header_seq_;
  
  // Последовательности запуска (BOOT_SEND) - отправляются при загрузке для инициализации
  std::vector<ByteWithPause> start1_seq_;
  std::vector<ByteWithPause> start2_seq_;
  std::vector<ByteWithPause> start3_seq_;
  std::vector<ByteWithPause> start4_seq_;

  // Последовательности для режимов скорости мотора без подсветки
  std::vector<ByteWithPause> motor_speed0_seq_;
  std::vector<ByteWithPause> motor_speed1_seq_;
  std::vector<ByteWithPause> motor_speed2_seq_;
  std::vector<ByteWithPause> motor_speed3_seq_;

  // Последовательности для режимов скорости мотора с включенной подсветкой
  std::vector<ByteWithPause> motor_speed0_light_seq_;
  std::vector<ByteWithPause> motor_speed1_light_seq_;
  std::vector<ByteWithPause> motor_speed2_light_seq_;
  std::vector<ByteWithPause> motor_speed3_light_seq_;

  // Последовательности для кодов кнопок - отправляются при изменении скорости
  // Используются только когда включен звук для обратной связи
  std::vector<ByteWithPause> button_motor_speed0_seq_;
  std::vector<ByteWithPause> button_motor_speed1_seq_;
  std::vector<ByteWithPause> button_motor_speed2_seq_;
  std::vector<ByteWithPause> button_motor_speed3_seq_;

  // Настройки и счетчики
  uint32_t pause_duration_ = DEFAULT_PAUSE_DURATION; // Длительность паузы между байтами в микросекундах
  int boot_repeats_sent_ = 0;                        // Счетчик отправленных последовательностей загрузки

  // Флаги состояния системы
  bool light_on_ = false;      // Состояние подсветки (true = включена, false = выключена)
  bool sound_on_ = false;      // Состояние звука (true = включен, false = выключен)
  uint8_t motor_speed_ = 0;    // Текущая скорость мотора (0..3)
  bool speed_changed_ = false; // Флаг изменения скорости - отправляем код кнопки только при изменении

  // Ссылки на переключатели Home Assistant для синхронизации состояния
  // Эти указатели устанавливаются при инициализации для двусторонней связи
  esphome::template_::TemplateSwitch *light_switch_ = nullptr;  // Переключатель подсветки
  esphome::template_::TemplateSwitch *sound_switch_ = nullptr;  // Переключатель звука
  
  // Отправка байтов с паузами
  // Это основной метод для отправки команд на кухонную вытяжку
  // Параметр sequence: вектор пар {байт, нужна_ли_пауза_после_байта}
  void send_sequence_with_pauses(const std::vector<ByteWithPause> &sequence);
};

}  // namespace kitchen_hood
}  // namespace esphome

