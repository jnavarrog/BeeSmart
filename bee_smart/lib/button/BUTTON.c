#include <BUTTON.h>

static Button_Object button_object;

void button_get(Button_Object * button_object);
void button_set_input(Button_Object * button_object);
Button_Object * button_init(Button_Port port, Button_Pin pin);
void button_start();

void button_get(Button_Object * button_object) {
  button_object->value = gpio_hal_arch_read_pin(button_object->port, button_object->pin);
}

void button_set_input(Button_Object * button_object) {
  gpio_hal_arch_pin_set_input(button_object->port, button_object->pin);
  gpio_hal_arch_no_port_pin_cfg_set(button_object->pin, GPIO_HAL_PIN_CFG_PULL_UP);
}

Button_Object * button_init(
  Button_Port port, Button_Pin pin
) {
  button_object.port = port;
  button_object.pin = pin;
  button_start(&button_object);

  return &button_object;
}

void button_start(Button_Object * button_object) {
  button_set_input(button_object);
}
