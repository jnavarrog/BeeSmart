#include <SERVO.h>
#include "lib/sensors.h"

extern const struct sensors_sensor servo;

typedef enum Servo_Configuration {
  SERVO_CONFIGURATION_START,
  SERVO_CONFIGURATION_POSITION,
  SERVO_CONFIGURATION_PIN,
  SERVO_CONFIGURATION_PORT,
  SERVO_CONFIGURATION_STOP_DELAY
} Servo_Configuration;

typedef enum Servo_Value {
  SERVO_VALUE_MOVE,
  SERVO_VALUE_CLOSE,
  SERVO_VALUE_OPEN,
  SERVO_VALUE_STOP,
  SERVO_VALUE_POSITION
} Servo_Value;

typedef enum Servo_Response {
  SERVO_RESPONSE_ERROR,
  SERVO_RESPONSE_SUCCESS
} Servo_Response;

typedef enum Servo_Status {
  SERVO_STATUS_NOT_READY,
  SERVO_STATUS_READY_TO_START,
  SERVO_STATUS_READY_TO_MOVE
} Servo_Status;


