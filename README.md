# Kitchen Hood UART Controller (ESPHome + ESP32)

## Описание

Этот проект позволяет заменить штатную сенсорную плату кухонной вытяжки Krona на модуль ESP32 с интеграцией в Home Assistant через ESPHome. Управление вытяжкой осуществляется по UART (500 бод, инвертированный сигнал).

## Возможности

- Управление вытяжкой из Home Assistant (две кнопки: Включить и Выключить)
- Эмуляция оригинальных UART-команд вытяжки
- Гибкая настройка последовательностей команд и пауз между ними
- Поддержка инверсии сигнала UART

## Алгоритм работы

1. После включения ESP32 отправляет стартовую последовательность байтов (`start1_seq_ - start4_seq_`).
2. Затем бесконечно отправляет массив дежурного режима (`standby_seq_`).
3. При нажатии кнопки "Включить" в Home Assistant — после очередного standby отправляется массив кнопки включения (`button_poweron_seq_`), затем массив рабочего режима (`poweron_seq_`) повторяется бесконечно.
4. При нажатии кнопки "Выключить" — после очередного poweron отправляется массив кнопки выключения (`button_poweroff_seq_`), затем снова начинается цикл standby.


## Аппаратные требования

- ESP32
- Подключение к основной плате вытяжки по UART через преобразователь логических уровней 3.3 -->> 5 В (без инверсии).

## Установка

1. Скопируйте папку `custom_components/kitchen_hood` в ваш проект ESPHome.
2. Добавьте две кнопки в Home Assistant для управления вытяжкой.

## Пример конфигурации ESPHome

```yaml
esphome:
  name: kitchen-hood-emulator
  friendly_name: "ESP32 Kitchen Hood"

esp32:
  board: esp32dev
  framework:
    type: arduino

wifi:
  ssid: "Your_SSID"
  password: "************"
  manual_ip:
    static_ip: 192.168.1.17
    gateway: 192.168.1.1
    subnet: 255.255.255.0
  ap:
    ssid: "ESP32_KitchenHood"
    password: "**************"
    
    
ota:
  platform: esphome
  password: ""

api:
  encryption:
    key: kjhkjhkhhkhkhljjljjjkhkhjkjhkjhk=    

uart:
  tx_pin:
    number: GPIO13
    inverted: false
  baud_rate: 500
  id: uart_bus

kitchen_hood:
  uart_id: uart_bus
  
external_components:
  - source:
      type: local
      path: custom_components
    components: [kitchen_hood]

logger:    

button:
  - platform: template
    name: "Включить вытяжку"
    on_press:
      then:
        - lambda: |-
            if (kitchen_hood::KitchenHood::instance) {
              kitchen_hood::KitchenHood::instance->press_power_on();
            }

  - platform: template
    name: "Выключить вытяжку"
    on_press:
      then:
        - lambda: |-
            if (kitchen_hood::KitchenHood::instance) {
              kitchen_hood::KitchenHood::instance->press_power_off();
            }
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
