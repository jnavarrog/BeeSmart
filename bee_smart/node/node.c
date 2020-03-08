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

int servo_pin_int = IOID_12;

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
static struct etimer period;

  PROCESS_BEGIN();

  SENSORS_ACTIVATE(ds18b20);
  ds18b20.configure(DS18B20_CONFIGURATION_AMOUNT, ds18b20_amount_int);
  ds18b20.configure(DS18B20_CONFIGURATION_PORT, ds18b20_port_int);
  ds18b20.configure(DS18B20_CONFIGURATION_PIN, ds18b20_pin_int);
  ds18b20.configure(DS18B20_CONFIGURATION_START, 0);

  SENSORS_ACTIVATE(servo);
  servo.configure(SERVO_CONFIGURATION_PIN, servo_pin_int);
  servo.configure(SERVO_CONFIGURATION_START, 0);
  servo.configure(SERVO_CONFIGURATION_STOP, 0);
  servo.value(SERVO_VALUE_STOP);

  SENSORS_ACTIVATE(hx711);
  hx711.configure(HX711_CONFIGURATION_PORT, hx711_port_int);
  hx711.configure(HX711_CONFIGURATION_PIN_DOUT, hx711_pin_dout_int);
  hx711.configure(HX711_CONFIGURATION_PIN_SCK, hx711_pin_sck_int);
  hx711.configure(HX711_CONFIGURATION_START_READ, 0);

  PROCESS_PAUSE();

  LOG_INFO("BEESMART - PROYECTO FIN DE CARRERA 2020 - NODO\n");

  coap_activate_resource(&res_test, "test");
  coap_activate_resource(&res_addresstemperature, "sensors/temperature/all");
  coap_activate_resource(&res_temperature, "sensors/temperature/temperature");
  coap_activate_resource(&res_reboot, "commands/reboot");
  coap_activate_resource(&res_weight, "sensors/weight");
  coap_activate_resource(&res_servo, "actuators/servo");

  while(1) {
    PROCESS_WAIT_EVENT();
  }

  ds18b20.configure(DS18B20_CONFIGURATION_STOP, 0);
  PROCESS_END();
}
