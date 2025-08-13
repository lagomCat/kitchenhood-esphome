# Kitchen Hood UART Controller (ESPHome + ESP32)

## Описание

**Прокачай свою вытяжку ESPHome-ом!**

Этот проект позволяет эмулировать работу родной панели управления кухонной вытяжки (Krona) с помощью ESP32 и Home Assistant — удобное управление через Home Assistant или умную кнопку, чем не комфортные родные сенсорные кнопки на нижней панели.
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
            

### 2. C++ компоненты (custom_components/kitchen_hood/)
kitchen_hood.h — декларация класса и логики переключений.

kitchen_hood.cpp — реализация компонентов отправки байтов, состояния и синхронизации с Home Assistant.

Включены методы: press_motor_speed(), set_light(), set_sound() и синхронизация состояния.

### 3. Хочешь добавить умную Zigbee-кнопку?
В интерфейсе HA можно создать автоматизацию:

Single press — выбрать скорости моторa;

Long press — включить/выключить подсветку и звук;

Double press — быстрее перейти на макс. скорость.

## Known Issues / Issues
В данной реализации не удалось получить корректные коды кнопок переключения скоростей при включённой подсветке.
В режиме со звуком включён… отправляются коды кнопок, полученные при выключенной подсветке. В результате при отправке кнопки подсветка моргает.
Видимо, логика переключения режимов зависит от битов, но я не нашёл закономерности, поэтому используем фиксированные наборы байтов.
При отключённом звуке проблем нет — отправляется только код режима, без кнопки.

## Требования
Плата ESP32 (например, devboard).

Подключение UART TX к линии управления вытяжки (с учётом инверсии).

Компонент ESPHome (2012.6.3 или новее).

Home Assistant для управления кнопками и переключателями.

## Лицензия
Этот проект — полностью open source. Ты можешь использовать и модифицировать его под себя. ESPHome же базируется на лицензиях MIT (Python-часть) и GPLv3 (C++), поэтому при публикации прошивки с изменёнными частями логики будь внимателен к правилам лицензирования 
Reddit
.

## Контакты
Если что-то не работает или хочется улучшение — открывай issue или пиши в Discussions. Рад будем помочь!
