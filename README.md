# Kitchen Hood UART Controller (ESPHome + ESP32)

## Описание

Этот проект позволяет заменить штатную сенсорную плату кухонной вытяжки Krona на модуль ESP32 с интеграцией в Home Assistant через ESPHome. Управление вытяжкой осуществляется по UART (500 бод, инвертированный сигнал).

## Возможности

- Управление вытяжкой из Home Assistant (две кнопки: Включить и Выключить)
- Эмуляция оригинальных UART-команд вытяжки
- Гибкая настройка последовательностей команд и пауз между ними
- Поддержка инверсии сигнала UART

## Алгоритм работы

1. После включения ESP32 отправляет дважды стартовую последовательность байтов (`start_bytes_array`).
2. Затем бесконечно отправляет массив дежурного режима (`standby_bytes_array`).
3. При нажатии кнопки "Включить" в Home Assistant — после очередного standby отправляется массив кнопки включения (`button_poweron_bytes_array`), затем массив рабочего режима (`poweron_bytes_array`) повторяется бесконечно.
4. При нажатии кнопки "Выключить" — после очередного poweron отправляется массив кнопки выключения (`button_poweroff_bytes_array`), затем снова начинается цикл standby.
5. Между фреймами можно вставлять паузы, кратные 2 мс (длительность одного бита).

## Аппаратные требования

- ESP32
- Подключение к основной плате вытяжки по UART (TX ESP32 -> RX вытяжки, с учётом инверсии сигнала)

## Установка

1. Скопируйте папку `custom_components/kitchen_hood` в ваш проект ESPHome.
2. Опишите массивы байтов в конфиге ESPHome.
3. Добавьте две кнопки в Home Assistant для управления вытяжкой.

## Пример конфигурации ESPHome

```yaml
external_components:
  - source: github://your-repo/kitchen_hood

uart:
  id: uart_bus
  tx_pin: GPIO13
  baud_rate: 500
  parity: NONE
  stop_bits: 1
  rx_inverted: true  # если требуется инверсия

kitchen_hood:
  uart_id: uart_bus
  start_sequence: [0xAA, 0xBB, 0xCC, 0xDD, 0xEE]
  standby_sequence: [0x01, 0x02, 0x03]
  poweron_sequence: [0x10, 0x20, 0x30]
  button_poweron_sequence: [0xA1]
  button_poweroff_sequence: [0xA0]
  inter_frame_bits: 10

button:
  - platform: template
    name: "Вытяжка ВКЛ"
    on_press:
      then:
        - kitchen_hood.press_power_on
  - platform: template
    name: "Вытяжка ВЫКЛ"
    on_press:
      then:
        - kitchen_hood.press_power_off
```

## English

### Kitchen Hood UART Controller (ESPHome + ESP32)

This project replaces the original touch control board of a kitchen hood Krona with an ESP32 module, integrating it into Home Assistant via ESPHome. The hood is controlled via UART (500 baud, inverted signal).

**Features:**
- Control from Home Assistant (two buttons: On and Off)
- Emulates original UART commands
- Flexible command and pause configuration
- UART signal inversion support

**How it works:**
1. On power-up, ESP32 sends the start byte sequence twice.
2. Then, it endlessly sends the standby byte sequence.
3. When "On" is pressed in HA, after the next standby, it sends the power-on button sequence, then endlessly sends the power-on sequence.
4. When "Off" is pressed, after the next power-on, it sends the power-off button sequence, then resumes standby loop.
5. Pauses between frames can be set in multiples of 2 ms (bit duration).

**Hardware:**
- ESP32
- UART connection to hood mainboard (TX ESP32 -> RX hood, with inversion if needed)

**Installation:**
1. Copy `custom_components/kitchen_hood` to your ESPHome project.
2. Define byte arrays in your ESPHome config.
3. Add two buttons in Home Assistant for control.

**ESPHome Example:**  
(see above)

---

## Лицензия

MIT License
