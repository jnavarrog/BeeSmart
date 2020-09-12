#include <HX711_SENSOR.h>
#include <stdbool.h>
#include <stdlib.h>

#define HX711_WEIGHT_HIGH_LENGHT 16
#define HX711_WEIGHT_LOW_MASK 0x0000FFFF

Hx711_Port hx711_port = 0;
Hx711_Pin hx711_pin_dout = 0;
Hx711_Pin hx711_pin_sck = 0;
Hx711_Object hx711_object;

bool hx711_sensor_ready_to_start_read() {
  return hx711_port >= 0 && hx711_pin_dout >= 0 && hx711_pin_sck >= 0;
}

bool hx711_sensor_ready_to_get_read() {
  return hx711_sensor_ready_to_start_read() && hx711_object.weight != 0;
}

int hx711_sensor_start_read() {
  if (!hx711_sensor_ready_to_start_read()) {
    return HX711_RESPONSE_ERROR;
  }

  hx711_object = hx711_init(hx711_port, hx711_pin_dout, hx711_pin_sck);
  return HX711_RESPONSE_SUCCESS;
}


/*--------------------------SENSOR INTERFACE----------------------------------*/
const struct sensors_sensor hx711;

static int value(int type) {
  switch(type) {
  case HX711_VALUE_WEIGHT_HIGH:
    return (int) (hx711_object.weight >> HX711_WEIGHT_HIGH_LENGHT);

  case HX711_VALUE_WEIGHT_LOW:
    return (int) (hx711_object.weight & HX711_WEIGHT_LOW_MASK);
  }

  return HX711_RESPONSE_ERROR;
}

static int status(int type) {
  if (hx711_sensor_ready_to_get_read()) {
    return HX711_STATUS_GET_READ_READY;
  } else if (hx711_sensor_ready_to_start_read()) {
    return HX711_STATUS_START_READ_READY;
  } else {
    return HX711_STATUS_NOT_READY;
  }
}

static int configure(int type, int c) {
  switch(type) {
    case HX711_CONFIGURATION_START_READ:
      return hx711_sensor_start_read();

    case HX711_CONFIGURATION_PIN_DOUT:
      hx711_pin_dout = (Hx711_Pin) c;
      return HX711_RESPONSE_SUCCESS;

    case HX711_CONFIGURATION_PIN_SCK:
      hx711_pin_sck = (Hx711_Pin) c;
      return HX711_RESPONSE_SUCCESS;

    case HX711_CONFIGURATION_PORT:
      hx711_port = (Hx711_Port) c;
      return HX711_RESPONSE_SUCCESS;
  }

  return HX711_RESPONSE_ERROR;
}

SENSORS_SENSOR(hx711, "HX711", value, configure, status);
