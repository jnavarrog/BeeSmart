#include <SERVO.h>

#define SERVO_POSITION_MAX SERVO_POSITION_9
#define SERVO_POSITION_MIN SERVO_POSITION_0

#define SERVO_PERIOD_TICKS_100_US 200
#define SERVO_PERIOD_ON_MIN_100_US 1
#define SERVO_PERIOD_ON_MAX_100_US 28
#define SERVO_PERIOD_ON_STEP_100_US 3

#define RTIMER_TICK_100_US RTIMER_SECOND / 10000
#define SERVO_PERIOD_TICKS RTIMER_TICK_100_US * SERVO_PERIOD_TICKS_100_US
#define SERVO_PERIOD_ON_MIN_TICKS RTIMER_TICK_100_US * SERVO_PERIOD_ON_MIN_100_US
#define SERVO_PERIOD_ON_MAX_TICKS RTIMER_TICK_100_US * SERVO_PERIOD_ON_MAX_100_US
#define SERVO_PERIOD_ON_STEP_TICKS RTIMER_TICK_100_US * SERVO_PERIOD_ON_STEP_100_US

PROCESS(servo_thread, "servo_thread");

int servo_period_ticks = SERVO_PERIOD_TICKS;
int servo_on_period_ticks;
int servo_off_period_ticks;

static void servo_off_period_complete(struct rtimer * rt, void * ptr) {
  process_poll(&servo_thread);
}

static void servo_on_period_complete(struct rtimer * rt, void * ptr) {
  process_poll(&servo_thread);
}

void servo_set_rtimer_on_period(Servo_Object * servo_object_ptr) {
  servo_on_period_ticks =
    SERVO_PERIOD_ON_MIN_TICKS + (SERVO_PERIOD_ON_STEP_TICKS * (int) servo_object_ptr->position);

  if (servo_on_period_ticks > SERVO_PERIOD_ON_MAX_TICKS) {
    servo_on_period_ticks = SERVO_PERIOD_ON_MAX_TICKS;
  }

  servo_off_period_ticks = SERVO_PERIOD_TICKS - servo_on_period_ticks;

  servo_object_ptr->on = true;

  process_start(&servo_thread, servo_object_ptr);
  process_poll(&servo_thread);
}

void servo_set_output(Servo_Object * servo_object_ptr) {
  gpio_hal_arch_pin_set_output(servo_object_ptr->port, servo_object_ptr->pin);
}

void servo_pin_on(Servo_Object * servo_object_ptr) {
  gpio_hal_arch_write_pin(servo_object_ptr->port, servo_object_ptr->pin, 1);
}

void servo_pin_off(Servo_Object * servo_object_ptr) {
  gpio_hal_arch_write_pin(servo_object_ptr->port, servo_object_ptr->pin, 0);
}

Servo_Object servo_init(Servo_Port port, Servo_Pin pin) {
  Servo_Object servo_object;

  servo_object.port = port;
  servo_object.pin = pin;
  servo_object.position = 0;
  servo_object.on = false;
  servo_object.init = true;

  servo_set_output(&servo_object);
  servo_pin_off(&servo_object);

  return servo_object;
}

void servo_stop(Servo_Object * servo_object_ptr) {
  servo_object_ptr->on = false;
}

void servo_open(Servo_Object * servo_object_ptr) {
  servo_object_ptr->position = (Servo_Position) SERVO_POSITION_MAX;
  servo_set_rtimer_on_period(servo_object_ptr);
}

void servo_close(Servo_Object * servo_object_ptr) {
  servo_object_ptr->position = (Servo_Position) SERVO_POSITION_MIN;
  servo_set_rtimer_on_period(servo_object_ptr);
}

void servo_move(Servo_Object * servo_object_ptr, Servo_Position position) {
  servo_object_ptr->position = position;
  servo_set_rtimer_on_period(servo_object_ptr);
}

PROCESS_THREAD(servo_thread, ev, data) {
  static Servo_Object * servo_object_ptr;
  static struct rtimer rtimer_off_servo_period;
  static struct rtimer rtimer_on_servo_period;

  if ((Servo_Object *) data) {
    servo_object_ptr = (Servo_Object *) data;
  }

  PROCESS_BEGIN();

  while(1) {
    if(!(servo_object_ptr->on)) {
      PROCESS_YIELD_UNTIL(ev == PROCESS_EVENT_POLL);
    }
      servo_pin_on(servo_object_ptr);

      rtimer_set(
        &rtimer_on_servo_period, RTIMER_NOW() + servo_on_period_ticks, 1, servo_on_period_complete, NULL
      );

      PROCESS_YIELD_UNTIL(ev == PROCESS_EVENT_POLL);

      servo_pin_off(servo_object_ptr);

      rtimer_set(
        &rtimer_off_servo_period, RTIMER_NOW() + servo_off_period_ticks, 1, servo_off_period_complete, NULL
      );

      PROCESS_YIELD_UNTIL(ev == PROCESS_EVENT_POLL);
  }

  PROCESS_END();
}
