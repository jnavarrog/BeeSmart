#include <BUTTON.h>
#include "lib/sensors.h"

extern const struct sensors_sensor button;

typedef enum Button_Configuration {
  BUTTON_CONFIGURATION_PIN,
  BUTTON_CONFIGURATION_PORT,
  BUTTON_CONFIGURATION_START,
} Button_Configuration;

typedef enum Button_Sensor_Value {
  BUTTON_VALUE,
} Button_Sensor_Value;

typedef enum Button_Response {
  BUTTON_RESPONSE_ERROR,
  BUTTON_RESPONSE_SUCCESS
} Button_Response;

typedef enum Button_Status {
  BUTTON_STATUS_NOT_READY,
  BUTTON_STATUS_READY
} Button_Status;


