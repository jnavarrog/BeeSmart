#include <stdint.h>
#include <gpio-hal.h>
#include "sys/rtimer.h"
#include <stdbool.h>

typedef uint8_t Servo_Pin;

typedef enum Servo_Position {
  SERVO_POSITION_0,
  SERVO_POSITION_1,
  SERVO_POSITION_2,
  SERVO_POSITION_3,
  SERVO_POSITION_4,
  SERVO_POSITION_5,
  SERVO_POSITION_6,
  SERVO_POSITION_7,
  SERVO_POSITION_8,
  SERVO_POSITION_9
} Servo_Position;

typedef struct Servo_Object {
  Servo_Pin       pin;
  Servo_Position  position;
  bool            pwm_open;
  bool            init;
} Servo_Object;

extern Servo_Object servo_init(Servo_Pin pin);

extern void servo_off(Servo_Object * servo_object_ptr);

extern void servo_on(Servo_Object * servo_object_ptr);

extern void servo_open(Servo_Object * servo_object_ptr);

extern void servo_close(Servo_Object * servo_object_ptr);

extern void servo_move(Servo_Object * servo_object_ptr, Servo_Position position);
