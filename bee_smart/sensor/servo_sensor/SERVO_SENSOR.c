#include <SERVO_SENSOR.h>
#include <stdbool.h>
#include <stdlib.h>
#include <contiki.h>

Servo_Pin servo_pin = 0;
Servo_Object servo_object;
Servo_Position servo_position = SERVO_POSITION_0;

bool servo_sensor_ready_to_start() {
  return servo_pin >= 0;
}

bool servo_sensor_ready_to_move() {
  return servo_object.init;
}

int servo_sensor_start() {
  if (!servo_sensor_ready_to_start()) {
    return SERVO_RESPONSE_ERROR;
  }

  servo_object = servo_init(servo_pin);
  return SERVO_RESPONSE_SUCCESS;
}

int servo_sensor_stop() {
  if (!servo_sensor_ready_to_move()) {
    return SERVO_RESPONSE_ERROR;
  }

  servo_off(&servo_object);
  return SERVO_RESPONSE_SUCCESS;
}

int servo_sensor_move() {
  if (!servo_sensor_ready_to_move()) {
    return SERVO_RESPONSE_ERROR;
  }

  servo_on(&servo_object);
  servo_move(&servo_object, servo_position);
  return SERVO_RESPONSE_SUCCESS;
}

int servo_sensor_open() {
  if (!servo_sensor_ready_to_move()) {
    return SERVO_RESPONSE_ERROR;
  }

  servo_on(&servo_object);
  servo_open(&servo_object);
  return SERVO_RESPONSE_SUCCESS;
}

int servo_sensor_close() {
  if (!servo_sensor_ready_to_move()) {
    return SERVO_RESPONSE_ERROR;
  }

  servo_on(&servo_object);
  servo_close(&servo_object);
  return SERVO_RESPONSE_SUCCESS;
}

/*--------------------------SENSOR INTERFACE----------------------------------*/
const struct sensors_sensor servo;

static int value(int type) {
  switch(type) {
  case SERVO_VALUE_MOVE:
    return (int) servo_sensor_move();

  case SERVO_VALUE_CLOSE:
    return (int) servo_sensor_close();

  case SERVO_VALUE_OPEN:
    return (int) servo_sensor_open();

  case SERVO_VALUE_STOP:
    return (int) servo_sensor_stop();

  case SERVO_VALUE_POSITION:
  printf("%d\n", servo_position);
    return (int) servo_position;
  }

  return SERVO_RESPONSE_ERROR;
}

static int status(int type) {
  if (servo_sensor_ready_to_start()) {
    return SERVO_STATUS_READY_TO_START;
  } else if (servo_sensor_ready_to_move()) {
    return SERVO_STATUS_READY_TO_MOVE;
  } else {
    return SERVO_STATUS_NOT_READY;
  }
}

static int configure(int type, int c) {
  switch(type) {
    case SERVO_CONFIGURATION_START:
      return servo_sensor_start();

    case SERVO_CONFIGURATION_PIN:
      servo_pin = (Servo_Pin) c;
      return SERVO_RESPONSE_SUCCESS;

    case SERVO_CONFIGURATION_POSITION:
      printf("%d\n", c);
      servo_position = (Servo_Position) c;
      return SERVO_RESPONSE_SUCCESS;
  }

  return SERVO_RESPONSE_ERROR;
}

SENSORS_SENSOR(servo, "SERVO", value, configure, status);
