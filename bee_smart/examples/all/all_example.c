#include "contiki.h"
#include <SERVO_SENSOR.h>
#include <HX711_SENSOR.h>
#include <DS18B20_SENSOR.h>
#include <stdio.h>

#define SERVO_HOLD_PERIOD CLOCK_SECOND * 3
#define WEIGHT_WAIT_PERIOD CLOCK_SECOND * 3

int servo_pin_int = IOID_12;
int servo_port_int = GPIO_HAL_NULL_PORT;

int ds18b20_amount_int = 4;
int ds18b20_port_int = GPIO_HAL_NULL_PORT;
int ds18b20_pin_int = IOID_23;

int hx711_port_int = GPIO_HAL_NULL_PORT;
int hx711_pin_dout_int = IOID_25;
int hx711_pin_sck_int = IOID_26;

PROCESS(all_example, "all_example");
AUTOSTART_PROCESSES(&all_example);

PROCESS_THREAD(all_example, ev, data) {
  static struct etimer period;
  static struct etimer hold_period;
  static struct etimer weight_period;

  PROCESS_BEGIN();
  printf("all.c");

  SENSORS_ACTIVATE(servo);
  servo.configure(SERVO_CONFIGURATION_PORT, servo_port_int);
  servo.configure(SERVO_CONFIGURATION_PIN, servo_pin_int);
  servo.configure(SERVO_CONFIGURATION_START, 0);

  SENSORS_ACTIVATE(ds18b20);
  ds18b20.configure(DS18B20_CONFIGURATION_AMOUNT, ds18b20_amount_int);
  ds18b20.configure(DS18B20_CONFIGURATION_PORT, ds18b20_port_int);
  ds18b20.configure(DS18B20_CONFIGURATION_PIN, ds18b20_pin_int);
  ds18b20.configure(DS18B20_CONFIGURATION_START, 0);

  SENSORS_ACTIVATE(hx711);
  hx711.configure(HX711_CONFIGURATION_PORT, hx711_port_int);
  hx711.configure(HX711_CONFIGURATION_PIN_DOUT, hx711_pin_dout_int);
  hx711.configure(HX711_CONFIGURATION_PIN_SCK, hx711_pin_sck_int);

  while(1) {
    printf("----------ALL----------\n");

    servo.value(SERVO_VALUE_CLOSE);
    printf("Servo Closed\n");
    etimer_set(&hold_period, SERVO_HOLD_PERIOD);
    PROCESS_WAIT_UNTIL(etimer_expired(&hold_period));
    servo.value(SERVO_VALUE_STOP);
    printf("Servo Stopped\n");
    etimer_set(&period, CLOCK_SECOND);
    PROCESS_WAIT_UNTIL(etimer_expired(&period));

    for(int i = 0; i < ds18b20_amount_int; i++) {
      ds18b20.configure(DS18B20_CONFIGURATION_INDEX, i);
      ds18b20.configure(DS18B20_CONFIGURATION_READ, 0);
      int address_low = ds18b20.value(DS18B20_VALUE_ADDRESS_LOW);
      int address_high = ds18b20.value(DS18B20_VALUE_ADDRESS_HIGH);
      int integer = ds18b20.value(DS18B20_VALUE_TEMPERATURE_INTEGER);
      int decimal = ds18b20.value(DS18B20_VALUE_TEMPERATURE_DECIMAL);
      printf("Temperature in address %x%x: %d,%d\n", address_high, address_low, integer, decimal);
    }

    hx711.configure(HX711_CONFIGURATION_START_READ, 0);
    etimer_set(&weight_period, WEIGHT_WAIT_PERIOD);
    PROCESS_WAIT_UNTIL(hx711.status(0) == HX711_STATUS_GET_READ_READY || etimer_expired(&weight_period));
    int weight_low = hx711.value(HX711_VALUE_WEIGHT_LOW);
    int weight_high = hx711.value(HX711_VALUE_WEIGHT_HIGH);
    printf("Weight: %d%d\n", weight_high, weight_low);

    servo.value(SERVO_VALUE_OPEN);
    printf("Servo Open\n");
    etimer_set(&hold_period, SERVO_HOLD_PERIOD);
    PROCESS_WAIT_UNTIL(etimer_expired(&hold_period));
    servo.value(SERVO_VALUE_STOP);
    printf("Servo Stopped\n");
    etimer_set(&period, CLOCK_SECOND);
    PROCESS_WAIT_UNTIL(etimer_expired(&period));
  }

  ds18b20.configure(DS18B20_CONFIGURATION_STOP, 0);

  PROCESS_END();
}
