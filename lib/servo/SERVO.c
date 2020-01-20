#include <SERVO.h>
#include <contiki.h>
#include <ti-lib.h>
#include <lpm.h>

#define PWM_PERIOD 0xFFFF

#define SERVO_POSITION_MAX 0xFF
#define SERVO_POSITION_MIN 0x0

#define PWM_MAX 63
#define PWM_MIN 1

LPM_MODULE(pwm_module, NULL, NULL, NULL, LPM_DOMAIN_PERIPH);

void pwm_start(Servo_Object * servo_object_ptr) {
  ti_lib_prcm_peripheral_run_enable(PRCM_PERIPH_TIMER0);
  ti_lib_prcm_peripheral_sleep_enable(PRCM_PERIPH_TIMER0);
  ti_lib_prcm_peripheral_deep_sleep_enable(PRCM_PERIPH_TIMER0);
  ti_lib_prcm_load_set();

  while(!ti_lib_prcm_load_get());

  ti_lib_ioc_port_configure_set(servo_object_ptr->pin, IOC_PORT_MCU_PORT_EVENT0, IOC_STD_OUTPUT);
  HWREG(GPT0_BASE + GPT_O_TAMR) = (TIMER_CFG_A_PWM) | GPT_TAMR_TAPWMIE;

  lpm_register_module(&pwm_module);
  ti_lib_timer_disable(GPT0_BASE, TIMER_A);

  servo_object_ptr->pwm_open = true;
}

void pwm_stop(Servo_Object * servo_object_ptr) {
  lpm_unregister_module(&pwm_module);

  ti_lib_timer_disable(GPT0_BASE, TIMER_A);
  ti_lib_prcm_peripheral_run_disable(PRCM_PERIPH_TIMER0);
  ti_lib_prcm_peripheral_sleep_disable(PRCM_PERIPH_TIMER0);
  ti_lib_prcm_peripheral_deep_sleep_disable(PRCM_PERIPH_TIMER0);
  ti_lib_prcm_load_set();

  while(!ti_lib_prcm_load_get());

  ti_lib_ioc_pin_type_gpio_input(servo_object_ptr->pin);
  ti_lib_ioc_io_input_set(servo_object_ptr->pin, IOC_INPUT_DISABLE);

  servo_object_ptr->pwm_open = false;
}

void pwm_set(Servo_Object * servo_object_ptr) {
  if (!servo_object_ptr->pwm_open) {
    return;
  }

  Servo_Position position = servo_object_ptr->position << 2;

  if (position < (Servo_Position) PWM_MIN) {
    position = (Servo_Position) PWM_MIN;
  }

  uint32_t pwm_period = (uint32_t) position * (uint32_t) PWM_PERIOD / 100;

  ti_lib_timer_load_set(GPT0_BASE, TIMER_A, (uint32_t) PWM_PERIOD);
  ti_lib_timer_match_set(GPT0_BASE, TIMER_A, pwm_period);
  ti_lib_timer_enable(GPT0_BASE, TIMER_A);
}

Servo_Object servo_init(Servo_Pin pin) {
  Servo_Object servo_object;

  servo_object.pin = pin;
  servo_object.position = 0;
  servo_object.pwm_open = false;

  return servo_object;
}

void servo_on(Servo_Object * servo_object_ptr) {
  pwm_start(servo_object_ptr);
}

void servo_off(Servo_Object * servo_object_ptr) {
  pwm_stop(servo_object_ptr);
}

void servo_open(Servo_Object * servo_object_ptr) {
  servo_object_ptr->position = (Servo_Position) SERVO_POSITION_MAX;
  pwm_set(servo_object_ptr);
}

void servo_close(Servo_Object * servo_object_ptr) {
  servo_object_ptr->position = (Servo_Position) SERVO_POSITION_MIN;
  pwm_set(servo_object_ptr);
}

void servo_move(Servo_Object * servo_object_ptr, Servo_Position position) {
  servo_object_ptr->position = position;
  pwm_set(servo_object_ptr);
}
