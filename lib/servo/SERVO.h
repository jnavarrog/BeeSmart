#include <stdint.h>
#include <stdbool.h>

typedef uint8_t Servo_Pin;
typedef uint8_t Servo_Position;

typedef struct Servo_Object {
  Servo_Pin       pin;
  Servo_Position  position;
  bool            pwm_open;
} Servo_Object;

extern Servo_Object servo_init(Servo_Pin pin);

extern void servo_on(Servo_Object * servo_object_ptr);

extern void servo_off(Servo_Object * servo_object_ptr);

extern void servo_open(Servo_Object * servo_object_ptr);

extern void servo_close(Servo_Object * servo_object_ptr);

extern void servo_move(Servo_Object * servo_object_ptr, Servo_Position position);
