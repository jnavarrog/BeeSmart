#include <HX711.h>
#include "lib/sensors.h"

extern const struct sensors_sensor hx711;

typedef enum Hx711_Configuration {
  HX711_CONFIGURATION_START_READ,
  HX711_CONFIGURATION_PIN_DOUT,
  HX711_CONFIGURATION_PIN_SCK,
  HX711_CONFIGURATION_PORT
} Hx711_Configuration;

typedef enum Hx711_Value {
  HX711_VALUE_WEIGHT_LOW,
  HX711_VALUE_WEIGHT_HIGH
} Hx711_Value;

typedef enum Hx711_Response {
  HX711_RESPONSE_ERROR,
  HX711_RESPONSE_SUCCESS
} Hx711_Response;

typedef enum Hx711_Status {
  HX711_STATUS_NOT_READY,
  HX711_STATUS_START_READ_READY,
  HX711_STATUS_GET_READ_READY
} Hx711_Status;


