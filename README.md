# Kitchen Hood UART Controller (ESPHome + ESP32)

## Описание

**Прокачай свою вытяжку ESPHome-ом!**

Этот проект позволяет эмулировать работу родной панели управления кухонной вытяжки (Krona) с помощью ESP32 и Home Assistant — удобнее управления через Home Assistant или умную кнопку, чем не комфортные родные сенсорные кнопки на нижней панели.
Управление вытяжкой осуществляется по UART (500 бод, инвертированный сигнал с кастомизацией (фиксированные паузы между байтами в фреймах).

![Пример фиксированных пауз в одном пакете байтов](images/UART1.png)

# Kitchen Hood ESPHome Emulator

---

##  Что это умеет

- **Полностью повторяет UART-сигналы вытяжки**: статусы мотор-режима и подсветки.
- **Поддержка 8 режимов состояния**: мотор (0–3 скорости) × подсветка (вкл/выкл).
- **Гибкая логика кнопок и переключателей»**:
  - Кнопки **Motor Speed 0–3** → переключают скорость мотора.
  - **Switches Light и Sound** → включение/выключение подсветки и звукового сопровождения.
  - Подсветка и звук синхронизируются между ESP и HA, без рассинхронизации после перезагрузки.
- **Надёжность без Wi-Fi — ESP не уходит в сон**:
  - Выключен **WiFi sleep** (`WiFi.setSleep(false)`).
  - `api.reboot_timeout: 0s` — не перезагружается при обесточении API.

---

##  Как использовать

### 1. Настроить `kitchenHood.yaml`:

```yaml
esphome:
  name: kitchen-hood-emulator
  friendly_name: "ESP32 Kitchen Hood"
  on_boot:
    priority: 800
    then:
      - lambda: |-
          WiFi.setSleep(false);

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
  reboot_timeout: 120s      
    
    
ota:
  platform: esphome
  password: ""

api:
  encryption:
    key: kjhkjhkhhkhkhljjljjjkhkhjkjhkjhk=  #Your key 
  reboot_timeout: 0s   

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

# Кнопки управления из Home Assistant
button:
  - platform: template
    name: "Motor Speed 0"
    on_press:
      then:
        - lambda: |-
            if (kitchen_hood::KitchenHood::instance) {
              kitchen_hood::KitchenHood::instance->press_motor_speed(0);
            }

  - platform: template
    name: "Motor Speed 1"
    on_press:
      then:
        - lambda: |-
            if (kitchen_hood::KitchenHood::instance) {
              kitchen_hood::KitchenHood::instance->press_motor_speed(1);
            }

  - platform: template
    name: "Motor Speed 2"
    on_press:
      then:
        - lambda: |-
            if (kitchen_hood::KitchenHood::instance) {
              kitchen_hood::KitchenHood::instance->press_motor_speed(2);
            }

  - platform: template
    name: "Motor Speed 3"
    on_press:
      then:
        - lambda: |-
            if (kitchen_hood::KitchenHood::instance) {
              kitchen_hood::KitchenHood::instance->press_motor_speed(3);
            }

  - platform: template
    name: "Light On"
    on_press:
      then:
        - lambda: |-
            if (kitchen_hood::KitchenHood::instance) {
              kitchen_hood::KitchenHood::instance->press_light_on();
            }

  - platform: template
    name: "Light Off"
    on_press:
      then:
        - lambda: |-
            if (kitchen_hood::KitchenHood::instance) {
              kitchen_hood::KitchenHood::instance->press_light_off();
            }

  - platform: template
    name: "Sound On"
    on_press:
      then:
        - lambda: |-
            if (kitchen_hood::KitchenHood::instance) {
              kitchen_hood::KitchenHood::instance->press_sound_on();
            }

  - platform: template
    name: "Sound Off"
    on_press:
      then:
        - lambda: |-
            if (kitchen_hood::KitchenHood::instance) {
              kitchen_hood::KitchenHood::instance->press_sound_off();
            }
```
            

## English

### Kitchen Hood UART Controller (ESPHome + ESP32)

This project replaces the original touch control board of a kitchen hood KRONA (Model: SELINA 900 Glass White S) with an ESP32 module, integrating it into Home Assistant via ESPHome. The hood is controlled via UART (500 baud, inverted signal).

**Features:**
- Control from Home Assistant (two buttons: On and Off)
- Emulates original UART commands
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

**Issues**
In this implementation, the button codes for switching speeds with the light on were not obtained. In the mode with the button click sound enabled, the codes of the buttons that were obtained with the light off are sent to the stream. As a result, at the moment of sending the button code, the light turns off briefly. Most likely, mode switching is performed by setting certain bits in the byte packet according to some pattern, but I was not able to determine this pattern, so fixed sets of bytes are sent in the code for each mode and button. With the sound off, there is no problem, since the button code is not sent to the stream (only the mode code changes).

**Installation:**
1. Copy `custom_components/kitchen_hood` to your ESPHome project.
2. Add two buttons in Home Assistant for control.

**ESPHome Example:**  
(see above)

---

## Лицензия

MIT License
