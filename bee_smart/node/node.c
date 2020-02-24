#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "contiki.h"
#include "coap-engine.h"
#include "orchestra.h"
#include "tsch.h"
#include <DS18B20_SENSOR.h>
#include <SERVO_SENSOR.h>
#include <HX711_SENSOR.h>

#include "sys/log.h"
#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_APP

int ds18b20_port_int = GPIO_HAL_NULL_PORT;
int ds18b20_pin_int = IOID_23;
int ds18b20_amount_int = DS18B20_AMOUNT_INT;

#define SERVO_STOP_DELAY CLOCK_SECOND * 3

int servo_pin_int = IOID_12;
int servo_port_int = GPIO_HAL_NULL_PORT;

int hx711_port_int = GPIO_HAL_NULL_PORT;
int hx711_pin_dout_int = IOID_25;
int hx711_pin_sck_int = IOID_26;

extern coap_resource_t
  res_test,
  res_addresstemperature,
  res_temperature,
  res_reboot,
  res_weight,
  res_servo;


PROCESS(er_example_server, "Servidor CoAP");
AUTOSTART_PROCESSES(&er_example_server);

PROCESS_THREAD(er_example_server, ev, data)
{
  PROCESS_BEGIN();

  SENSORS_ACTIVATE(ds18b20);
  ds18b20.configure(DS18B20_CONFIGURATION_AMOUNT, ds18b20_amount_int);
  ds18b20.configure(DS18B20_CONFIGURATION_PORT, ds18b20_port_int);
  ds18b20.configure(DS18B20_CONFIGURATION_PIN, ds18b20_pin_int);
  ds18b20.configure(DS18B20_CONFIGURATION_START, 0);

  SENSORS_ACTIVATE(servo);
  servo.configure(SERVO_CONFIGURATION_PORT, servo_port_int);
  servo.configure(SERVO_CONFIGURATION_PIN, servo_pin_int);
  servo.configure(SERVO_CONFIGURATION_START, 0);
  servo.configure(SERVO_CONFIGURATION_STOP_DELAY, SERVO_STOP_DELAY);
  servo.value(SERVO_VALUE_STOP);

  SENSORS_ACTIVATE(hx711);
  hx711.configure(HX711_CONFIGURATION_PORT, hx711_port_int);
  hx711.configure(HX711_CONFIGURATION_PIN_DOUT, hx711_pin_dout_int);
  hx711.configure(HX711_CONFIGURATION_PIN_SCK, hx711_pin_sck_int);

  PROCESS_PAUSE();

  LOG_INFO("BEESMART - PROYECTO FIN DE CARRERA 2020 - NODO\n");

  coap_activate_resource(&res_test, "test");
  coap_activate_resource(&res_addresstemperature, "sensors/temperature/all");
  coap_activate_resource(&res_temperature, "sensors/temperature/temperature");
  coap_activate_resource(&res_reboot, "commands/reboot");
  coap_activate_resource(&res_weight, "sensors/weight");
  coap_activate_resource(&res_servo, "actuators/servo");

  while(1) {
    etimer_set(&period, SERVO_STOP_DELAY);
    PROCESS_WAIT_UNTIL(etimer_expired(&period));

    printf("SERVO POSITION %d\n", servo.value(SERVO_VALUE_POSITION));
    printf("SERVO ACTION %d\n", action);

    if (action == 1) {
      servo.configure(SERVO_CONFIGURATION_POSITION, SERVO_OPEN);
      servo.value(SERVO_VALUE_MOVE);
      action = 0;
    } else if (action == 0) {
      servo.configure(SERVO_CONFIGURATION_POSITION, SERVO_CLOSE);
      servo.value(SERVO_VALUE_MOVE);
      action = 1;
    }

    hx711.configure(HX711_CONFIGURATION_START_READ, 0);
    printf("WEIGHT %d%d\n", hx711.value(HX711_VALUE_WEIGHT_HIGH), hx711.value(HX711_VALUE_WEIGHT_LOW));

    for(int i = 0; i < ds18b20_amount_int; i++) {
      ds18b20.configure(DS18B20_CONFIGURATION_INDEX, i);
      ds18b20.configure(DS18B20_CONFIGURATION_READ, 0);

      int address_low = ds18b20.value(DS18B20_VALUE_ADDRESS_LOW);
      int address_high = ds18b20.value(DS18B20_VALUE_ADDRESS_HIGH);
      int integer = ds18b20.value(DS18B20_VALUE_TEMPERATURE_INTEGER);
      int decimal = ds18b20.value(DS18B20_VALUE_TEMPERATURE_DECIMAL);

      printf("Address %x%x: %d.%d\n", address_high, address_low, integer, decimal);
    }
  }

  ds18b20.configure(DS18B20_CONFIGURATION_STOP, 0);
  PROCESS_END();
}
