#include <SERVO_SENSOR.h>
#include <stdbool.h>
#include <stdlib.h>
#include <contiki.h>

Servo_Port servo_port = 0;
Servo_Pin servo_pin = 0;
Servo_Object servo_object;
Servo_Position servo_position = SERVO_POSITION_0;
int servo_stop_delay = CLOCK_SECOND;

bool servo_sensor_ready_to_start() {
  return servo_port >= 0 && servo_pin >= 0 && servo_port >= 0;
}

bool servo_sensor_ready_to_move() {
  return servo_object.init;
}

int servo_sensor_start() {
  if (!servo_sensor_ready_to_start()) {
    return SERVO_RESPONSE_ERROR;
  }

  servo_object = servo_init(servo_port, servo_pin);
  return SERVO_RESPONSE_SUCCESS;
}

int servo_sensor_stop() {
  if (!servo_sensor_ready_to_move()) {
    return SERVO_RESPONSE_ERROR;
  }

  servo_stop(&servo_object);
  return SERVO_RESPONSE_SUCCESS;
}

int servo_sensor_move() {
  if (!servo_sensor_ready_to_move()) {
    return SERVO_RESPONSE_ERROR;
  }

  servo_move(&servo_object, servo_position);
  return SERVO_RESPONSE_SUCCESS;
}

int servo_sensor_open() {
  if (!servo_sensor_ready_to_move()) {
    return SERVO_RESPONSE_ERROR;
  }

  servo_open(&servo_object);
  return SERVO_RESPONSE_SUCCESS;
}

int servo_sensor_close() {
  if (!servo_sensor_ready_to_move()) {
    return SERVO_RESPONSE_ERROR;
  }

  servo_close(&servo_object);
  return SERVO_RESPONSE_SUCCESS;
}

PROCESS(servo_stop_pr, "Servo stop");
PROCESS_THREAD(servo_stop_pr, ev, data) {
  static struct etimer period;
  PROCESS_BEGIN();
  etimer_set(&period, servo_stop_delay);
  PROCESS_WAIT_UNTIL(etimer_expired(&period));
  servo_sensor_stop();
  PROCESS_END();
}

int servo_sensor_stop_with_delay() {
  process_start(&servo_stop_pr, NULL);
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
    return (int) servo_sensor_stop_with_delay();

  case SERVO_VALUE_POSITION:
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

    case SERVO_CONFIGURATION_PORT:
      servo_port = (Servo_Port) c;
      return SERVO_RESPONSE_SUCCESS;

    case SERVO_CONFIGURATION_POSITION:
      servo_position = (Servo_Position) c;
      return SERVO_RESPONSE_SUCCESS;

    case SERVO_CONFIGURATION_STOP_DELAY:
      servo_stop_delay = c;
      return SERVO_RESPONSE_SUCCESS;
  }

  return SERVO_RESPONSE_ERROR;
}

SENSORS_SENSOR(servo, "SERVO", value, configure, status);
