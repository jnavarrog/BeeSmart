#include "contiki.h"
#include <DS18B20_SENSOR.h>
#include <Math.h>
#include <stdio.h>

int ds18b20_amount_int = 9;
int ds18b20_port_int = GPIO_HAL_NULL_PORT;
int ds18b20_pin_int = IOID_23;

PROCESS(ds18b20_example, "ds18b20_example");
AUTOSTART_PROCESSES(&ds18b20_example);

PROCESS_THREAD(ds18b20_example, ev, data) {
  static struct etimer periodic;
  PROCESS_BEGIN();
  printf("temperature.c\n");

  SENSORS_ACTIVATE(ds18b20);
  ds18b20.configure(DS18B20_CONFIGURATION_AMOUNT, ds18b20_amount_int);
  ds18b20.configure(DS18B20_CONFIGURATION_PORT, ds18b20_port_int);
  ds18b20.configure(DS18B20_CONFIGURATION_PIN, ds18b20_pin_int);
  ds18b20.configure(DS18B20_CONFIGURATION_START, 0);

  while(1) {
    etimer_set(&periodic, CLOCK_SECOND * 2);

    printf("------------- TEMPERATURES ----------\n");
    for(int i = 0; i < ds18b20_amount_int; i++) {
      ds18b20.configure(DS18B20_CONFIGURATION_INDEX, i);
      ds18b20.configure(DS18B20_CONFIGURATION_READ, 0);

      int address_low = ds18b20.value(DS18B20_VALUE_ADDRESS_LOW);
      int address_high = ds18b20.value(DS18B20_VALUE_ADDRESS_HIGH);
      int integer = ds18b20.value(DS18B20_VALUE_TEMPERATURE_INTEGER);
      int decimal = ds18b20.value(DS18B20_VALUE_TEMPERATURE_DECIMAL);

      printf("Address %x%x: %d,%d\n", address_high, address_low, integer, decimal);
    }

    PROCESS_WAIT_UNTIL(etimer_expired(&periodic));
  }
  ds18b20.configure(DS18B20_CONFIGURATION_STOP, 0);
  PROCESS_END();
}
