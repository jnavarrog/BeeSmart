#include "contiki.h"
#include <SERVO.h>
#include <stdio.h>

PROCESS(servo_example, "servo_example");
AUTOSTART_PROCESSES(&servo_example);

PROCESS_THREAD(servo_example, ev, data) {
  static struct etimer period;
  static uint8_t duty = 0;

  Servo_Pin pin;
  Servo_Object servo_object;

  pin = 25;
  servo_object = servo_init(pin);
  servo_on(&servo_object);

  PROCESS_BEGIN();

  while(1) {
    printf("----------SERVO----------\n");
    if (duty % 2 == 1) {
      servo_open(&servo_object);
    } else {
      servo_close(&servo_object);
    }

    printf("Pin: %d, position: %d, pwm_open: %d\n", servo_object.pin, servo_object.position, servo_object.pwm_open);

    duty += 1;

    etimer_set(&period, CLOCK_SECOND);
    PROCESS_WAIT_UNTIL(etimer_expired(&period));
  }

  PROCESS_END();
}
