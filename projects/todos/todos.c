#include "contiki.h"
#include <DS18B20.h>
#include <HX711.h>
#include <lib/sensors.h>
#include "todos.h"

PROCESS(ds18b20, "ds18b20");
AUTOSTART_PROCESSES(&ds18b20);

PROCESS_THREAD(ds18b20, ev, data) {
  static struct etimer periodic;
  PROCESS_BEGIN();
  printf("temperature.c\n");

  SENSORS_ACTIVATE(pfe_sensors);
  pfe_sensors.configure(PFE_SENSOR_CONFIGURATION_TYPE_TEMPERATURE_START, 0);

  while(1) {
    etimer_set(&periodic, 20);

    printf("------------- TEMPERATURES ----------\n");
    for(int i = 0; i < DS18B20_AMOUNT; i++) {
      pfe_sensors.configure(PFE_SENSOR_CONFIGURATION_TYPE_TEMPERATURE_INDEX, i);
      int temperature = pfe_sensors.value(PFE_SENSOR_TYPE_TEMPERATURE_TEMPERATURE);
      int address = pfe_sensors.value(PFE_SENSOR_TYPE_TEMPERATURE_ADDRESS);
      printf("Address: %x - Temperature: %d.%d\n", address, temperature / 256, temperature % 256);
    }

    PROCESS_WAIT_UNTIL(etimer_expired(&periodic));
  }
  PROCESS_END();
}

//----------------------------//

Ds18b20_Port ds18b20_port = GPIO_HAL_NULL_PORT;
Ds18b20_Pin ds18b20_pin = IOID_25;
Ds18b20_Object ds18b20_objects[DS18B20_AMOUNT];

Pfe_Sensor_Configuration pfe_sensor_configuration;

static int status(int type) {
  return (int) pfe_sensor_configuration.temperature_sensor_started;
}

static int value(int type) {
  int index;
  index = pfe_sensor_configuration.temperature_sensor_index;

  switch (type) {
    case PFE_SENSOR_TYPE_TEMPERATURE_TEMPERATURE:
      ds18b20_read_temp_from_address(&(ds18b20_objects[index]));
      return (int) ds18b20_objects[index].temperature.integer << 8 |
              (int) ds18b20_objects[index].temperature.decimal;
    case PFE_SENSOR_TYPE_TEMPERATURE_ADDRESS:
      return (int) ds18b20_objects[index].address << 8;
  }

  return -1;
}

static int configure(int type, int value) {
  switch (type) {
    case PFE_SENSOR_CONFIGURATION_TYPE_TEMPERATURE_START:
      ds18b20_search_all(ds18b20_objects, ds18b20_port, ds18b20_pin, DS18B20_AMOUNT);
      pfe_sensor_configuration.temperature_sensor_started = true;
      return status(type);
    case PFE_SENSOR_CONFIGURATION_TYPE_TEMPERATURE_INDEX:
      pfe_sensor_configuration.temperature_sensor_index = value;
      return status(type);
  }

  return -1;
}

SENSORS_SENSOR(pfe_sensors, "PFE_SENSORS", value, configure, status);
