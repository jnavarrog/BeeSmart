#include "contiki.h"
#include <SERVO_SENSOR.h>
#include <stdio.h>

#define SERVO_HOLD_PERIOD CLOCK_SECOND * 3

int servo_pin_int = IOID_12;

PROCESS(servo_example, "servo_example");
AUTOSTART_PROCESSES(&servo_example);

PROCESS_THREAD(servo_example, ev, data) {
  static struct etimer period;
  static struct etimer hold_period;

  PROCESS_BEGIN();
  printf("servo.c");

  SENSORS_ACTIVATE(servo);
  servo.configure(SERVO_CONFIGURATION_PIN, servo_pin_int);
  servo.configure(SERVO_CONFIGURATION_START, 0);

  while(1) {
    printf("----------SERVO----------\n");
    servo.configure(SERVO_CONFIGURATION_POSITION, SERVO_POSITION_1);
    servo.value(SERVO_VALUE_MOVE);
    printf("Position: %d\n", SERVO_POSITION_1);
    etimer_set(&period, CLOCK_SECOND);
    PROCESS_WAIT_UNTIL(etimer_expired(&period));

    servo.configure(SERVO_CONFIGURATION_POSITION, SERVO_POSITION_3);
    servo.value(SERVO_VALUE_MOVE);
    printf("Position: %d\n", SERVO_POSITION_3);
    etimer_set(&period, CLOCK_SECOND);
    PROCESS_WAIT_UNTIL(etimer_expired(&period));

    servo.configure(SERVO_CONFIGURATION_POSITION, SERVO_POSITION_5);
    servo.value(SERVO_VALUE_MOVE);
    printf("Position: %d\n", SERVO_POSITION_5);
    etimer_set(&period, CLOCK_SECOND);
    PROCESS_WAIT_UNTIL(etimer_expired(&period));

    servo.configure(SERVO_CONFIGURATION_POSITION, SERVO_POSITION_7);
    servo.value(SERVO_VALUE_MOVE);
    printf("Position: %d\n", SERVO_POSITION_7);
    etimer_set(&period, CLOCK_SECOND);
    PROCESS_WAIT_UNTIL(etimer_expired(&period));

    servo.configure(SERVO_CONFIGURATION_POSITION, SERVO_POSITION_9);
    servo.value(SERVO_VALUE_MOVE);
    printf("Position: %d\n", SERVO_POSITION_9);
    etimer_set(&period, CLOCK_SECOND);
    PROCESS_WAIT_UNTIL(etimer_expired(&period));

    servo.value(SERVO_VALUE_CLOSE);
    printf("Closed\n");
    etimer_set(&hold_period, SERVO_HOLD_PERIOD);
    PROCESS_WAIT_UNTIL(etimer_expired(&hold_period));
    servo.value(SERVO_VALUE_STOP);
    printf("Stopped\n");
    etimer_set(&period, CLOCK_SECOND);
    PROCESS_WAIT_UNTIL(etimer_expired(&period));

    servo.value(SERVO_VALUE_OPEN);
    printf("Open\n");
    etimer_set(&hold_period, SERVO_HOLD_PERIOD);
    PROCESS_WAIT_UNTIL(etimer_expired(&hold_period));
    servo.value(SERVO_VALUE_STOP);
    printf("Stopped\n");
    etimer_set(&period, CLOCK_SECOND);
    PROCESS_WAIT_UNTIL(etimer_expired(&period));
  }

  PROCESS_END();
}
