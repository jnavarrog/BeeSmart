#include <BUTTON_SENSOR.h>
#include <stdbool.h>
#include <stdlib.h>
#include <contiki.h>

Button_Pin button_pin = 0;
Button_Port button_port = 0;
Button_Object * button_object;

bool button_sensor_ready() {
  return button_object->pin >= 0 && button_object->port >= 0;
}

int button_sensor_start() {
  if (!button_sensor_ready()) {
    return BUTTON_RESPONSE_ERROR;
  }

  button_object = button_init(button_port, button_pin);
  return BUTTON_RESPONSE_SUCCESS;
}

Button_Value button_sensor_value() {
  button_get(button_object);
  return button_object->value;
}

/*--------------------------SENSOR INTERFACE----------------------------------*/
const struct sensors_sensor button;

static int value(int type) {
  switch(type) {
  case BUTTON_VALUE:
    return (int) button_sensor_value();
  }

  return BUTTON_RESPONSE_ERROR;
}

static int status(int type) {
  if (button_sensor_ready()) {
    return BUTTON_STATUS_READY;
  } else {
    return BUTTON_STATUS_NOT_READY;
  }
}

static int configure(int type, int c) {
  switch(type) {
    case BUTTON_CONFIGURATION_START:
      return button_sensor_start();

    case BUTTON_CONFIGURATION_PIN:
      button_pin = (Button_Pin) c;
      return BUTTON_RESPONSE_SUCCESS;

    case BUTTON_CONFIGURATION_PORT:
      button_pin = (Button_Port) c;
      return BUTTON_RESPONSE_SUCCESS;
  }

  return BUTTON_RESPONSE_ERROR;
}

SENSORS_SENSOR(button, "BUTTON", value, configure, status);
