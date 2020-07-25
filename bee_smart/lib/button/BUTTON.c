#include <BUTTON.h>

static Button_Object button_object;

bool button_get(Button_Object * button_object);
void button_set_input(Button_Object * button_object);
void button_set_interrupts(Button_Object * button_object);
void button_set_interrupt_handler();
static void button_handler_function(Button_Pin_Mask pin_mask);
Button_Object * button_init(Button_Port port, Button_Pin pin, void * button_handler_on, void * button_handler_off);
void button_start();

static Button_Handler button_handler = {
  .next = NULL,
  .handler = button_handler_function,
  .pin_mask = gpio_hal_pin_to_mask(0),
};

bool button_get(Button_Object * button_object) {
  return gpio_hal_arch_read_pin(button_object->port, button_object->pin);
}

void button_set_input(Button_Object * button_object) {
  gpio_hal_arch_pin_set_input(button_object->port, button_object->pin);
}

void button_set_interrupts(Button_Object * button_object) {
  gpio_hal_pin_cfg_t cfg = GPIO_HAL_PIN_CFG_EDGE_BOTH | GPIO_HAL_PIN_CFG_INT_ENABLE | GPIO_HAL_PIN_CFG_PULL_UP;
  gpio_hal_arch_no_port_pin_cfg_set(button_object->pin, cfg);
}

void button_set_interrupt_handler() {
  gpio_hal_register_handler(&button_handler);
}

static void button_handler_function(Button_Pin_Mask pin_mask) {
  button_object.value = button_get(&button_object);
  if (button_object.value == 1) {
    (button_object.handler_on)();
  } else {
    (button_object.handler_off)();
  }
}

Button_Object * button_init(
  Button_Port port, Button_Pin pin, void * button_handler_on, void * button_handler_off
) {
  button_object.port = port;
  button_object.pin = pin;
  button_object.handler_on = button_handler_on;
  button_object.handler_off = button_handler_off;
  button_start(&button_object);

  return &button_object;
}

void button_start(Button_Object * button_object) {
  button_set_input(button_object);
  button_set_interrupts(button_object);
  button_object->value = button_get(button_object);
  button_handler.pin_mask = gpio_hal_pin_to_mask(button_object->pin);
  button_set_interrupt_handler();
}
