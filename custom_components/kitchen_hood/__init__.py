import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import uart
from esphome.const import CONF_ID

DEPENDENCIES = ['uart']
AUTO_LOAD = []

CONF_KITCHEN_HOOD = 'kitchen_hood'
kitchen_hood_ns = cg.esphome_ns.namespace('kitchen_hood')
KitchenHoodComponent = kitchen_hood_ns.class_('KitchenHoodComponent', cg.Component)

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(KitchenHoodComponent),
    cv.Required(uart.CONF_UART_ID): cv.use_id(uart.UARTComponent),
}).extend(cv.COMPONENT_SCHEMA)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    
    # Изменили имя переменной с 'uart' на 'uart_component' чтобы избежать конфликта
    uart_component = await cg.get_variable(config[uart.CONF_UART_ID])
    cg.add(var.set_uart(uart_component))