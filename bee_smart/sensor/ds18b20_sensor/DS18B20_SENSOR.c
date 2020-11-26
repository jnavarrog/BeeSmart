#include <DS18B20_SENSOR.h>
#include <stdbool.h>
#include <stdlib.h>
#include <ADDRESSES_FILE.h>

#define DS18B20_ADDRESS_HIGH_LENGHT 32
#define DS18B20_ADDRESS_LOW_MASK 0x00000000FFFFFFFF

uint8_t ds18b20_index = -1;
uint8_t ds18b20_amount = -1;
Ds18b20_Port ds18b20_port = -1;
Ds18b20_Pin ds18b20_pin = -1;
Ds18b20_Object * ds18b20_objects;

bool ds18b20_sensor_ready_to_start() {
  return ds18b20_amount >= 0 && ds18b20_pin >= 0 && ds18b20_port >= 0;
}

bool ds18b20_sensor_ready_to_read() {
  return ds18b20_amount >= 0 && ds18b20_pin >= 0 && ds18b20_port >= 0 && ds18b20_index >= 0;
}

int ds18b20_sensor_find_addresses(bool read_from_file) {
  if (!ds18b20_sensor_ready_to_read()) {
    return DS18B20_RESPONSE_ERROR;
  }

  ds18b20_search_all(ds18b20_objects, ds18b20_port, ds18b20_pin, ds18b20_amount, read_from_file);
  return DS18B20_RESPONSE_SUCCESS;
}

int ds18b20_sensor_start(bool read_from_file) {
  if (!ds18b20_sensor_ready_to_start()) {
    return DS18B20_RESPONSE_ERROR;
  }

  ds18b20_objects = (Ds18b20_Object *) malloc(sizeof(Ds18b20_Object[ds18b20_amount]));
  return ds18b20_sensor_find_addresses(read_from_file);
}

int ds18b20_sensor_read() {
  if (!ds18b20_sensor_ready_to_read()) {
    return DS18B20_RESPONSE_ERROR;
  }

  ds18b20_read_temp_from_address(&(ds18b20_objects[ds18b20_index]));
  return DS18B20_RESPONSE_SUCCESS;
}

int ds18b20_sensor_stop() {
  free(ds18b20_objects);
  return DS18B20_RESPONSE_SUCCESS;
}

/*--------------------------SENSOR INTERFACE----------------------------------*/
const struct sensors_sensor ds18b20;

static int value(int type) {
  switch(type) {
  case DS18B20_VALUE_ADDRESS_HIGH:
    return (int) (ds18b20_objects[ds18b20_index].address >> DS18B20_ADDRESS_HIGH_LENGHT);

  case DS18B20_VALUE_ADDRESS_LOW:
    return (int) (ds18b20_objects[ds18b20_index].address & DS18B20_ADDRESS_LOW_MASK);

  case DS18B20_VALUE_TEMPERATURE_INTEGER:
    return (int) ds18b20_objects[ds18b20_index].temperature.integer;

  case DS18B20_VALUE_TEMPERATURE_DECIMAL:
    return (int) ds18b20_objects[ds18b20_index].temperature.decimal;
  }

  return DS18B20_RESPONSE_ERROR;
}

static int status(int type) {
  if (ds18b20_sensor_ready_to_read()) {
    return DS18B20_STATUS_READ_READY;
  } else if (ds18b20_sensor_ready_to_start()) {
    return DS18B20_STATUS_SEARCH_READY;
  } else {
    return DS18B20_STATUS_NOT_READY;
  }
}

static int configure(int type, int c) {
  switch(type) {
    case DS18B20_CONFIGURATION_START:
      return ds18b20_sensor_start(false);

    case DS18B20_CONFIGURATION_START_FROM_FILE:
      return ds18b20_sensor_start(true);

    case DS18B20_CONFIGURATION_RESTART:
      return ds18b20_sensor_find_addresses(true);

    case DS18B20_CONFIGURATION_RESTART_FROM_FILE:
      return ds18b20_sensor_find_addresses(true);

    case DS18B20_CONFIGURATION_READ:
      return ds18b20_sensor_read();

    case DS18B20_CONFIGURATION_AMOUNT:
      ds18b20_amount = (uint8_t) c;
      return DS18B20_RESPONSE_SUCCESS;

    case DS18B20_CONFIGURATION_PIN:
      ds18b20_pin = (Ds18b20_Pin) c;
      return DS18B20_RESPONSE_SUCCESS;

    case DS18B20_CONFIGURATION_PORT:
      ds18b20_port = (Ds18b20_Port) c;
      return DS18B20_RESPONSE_SUCCESS;

    case DS18B20_CONFIGURATION_INDEX:
      ds18b20_index = (uint8_t) c;
      return DS18B20_RESPONSE_SUCCESS;

    case DS18B20_CONFIGURATION_STOP:
      return ds18b20_sensor_stop();
  }

  return DS18B20_RESPONSE_ERROR;
}

SENSORS_SENSOR(ds18b20, "DS18B20", value, configure, status);
