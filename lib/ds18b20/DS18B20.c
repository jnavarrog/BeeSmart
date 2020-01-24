#include <DS18B20.h>
#include <stdlib.h>
#include <stdbool.h>

#define DS18B20_COMMAND_READ_SCRATCH_PAD 0xBE
#define DS18B20_COMMAND_START_CONVERSION 0x44
#define DS18B20_COMMAND_SKIP_ROM 0xCC
#define DS18B20_COMMAND_MATCH_ROM 0x55
#define DS18B20_COMMAND_SEARCH_ROM 0xF0

#define DS18B20_SEARCH_DISC 0b00
#define DS18B20_SEARCH0 0b01
#define DS18B20_SEARCH1 0b10
#define DS18B20_SEARCH_NODS 0b11

typedef OneWire_Port Ds18b20_Port;
typedef OneWire_Pin Ds18b20_Pin;
typedef OneWire_Address Ds18b20_Address;

typedef struct Ds18b20_Temperature {
  uint8_t           integer;
  uint8_t           decimal;
} Ds18b20_Temperature;

typedef struct Ds18b20_Object {
  bool                initiated;
  OneWire_Address     address;
  Ds18b20_Temperature temperature;
  OneWire_Object      *ow_object_ptr;
} Ds18b20_Object;

OneWire_Object * ow_object_ptr = NULL;
Ds18b20_Object * ds18b20_objects = NULL;
Ds18b20_Port ds18b20_port = -1;
Ds18b20_Pin ds18b20_pin = -1;
uint8_t ds18b20_index = -1;
uint8_t ds18b20_amount = -1;

void ds18b20_init(uint8_t index, Ds18b20_Address address) {
  ds18b20_objects[index].initiated = true;
  ds18b20_objects[index].address = address;
  ds18b20_objects[index].ow_object_ptr = ow_object_ptr;
}

uint64_t ds18b20_search_once(uint8_t index) {
  uint8_t i;
  uint8_t last_bit = 0;
  uint8_t last_discrepancie = 0;
  OneWire_Address address = 0;
  static OneWire_Address last_address;
  static uint64_t discrepancies;

  address = last_address;

  ow_reset(ow_object_ptr);
  ow_write_byte(ow_object_ptr, DS18B20_COMMAND_SEARCH_ROM);

  if (index == 0) {
    discrepancies = 0;
    last_address = 0;
  } else {
    for (i = 0; i < 64; i++) {
       if (bit_at(discrepancies, i)) {
         last_discrepancie = i;
       }
    }
  }

  for (i = 0; i < 64; i++) {
    last_bit = 0;
    last_bit = (ow_read_bit(ow_object_ptr) << 1) | ow_read_bit(ow_object_ptr);
    if (last_bit == DS18B20_SEARCH0) {
      if (i >= last_discrepancie) {
        clear_bit(&discrepancies, i);
        clear_bit(&address, i);
      }
    } else if (last_bit == DS18B20_SEARCH1) {
      if (i >= last_discrepancie) {
        clear_bit(&discrepancies, i);
        set_bit(&address, i);
      }
    } else if (last_bit == DS18B20_SEARCH_DISC) {
      if (i == last_discrepancie) {
        toggle_bit(&address, i);
        clear_bit(&discrepancies, i);
      } else if (i > last_discrepancie) {
        set_bit(&discrepancies, i);
        set_bit(&address, i);
      }
    } else {
      ds18b20_init(index, address);
      return discrepancies;
    }
    ow_write_bit(ow_object_ptr, (uint8_t) bit_at(address, i));
  }
  ds18b20_init(index, address);
  last_address = address;
  return discrepancies;
}

void ds18b20_search() {
  uint8_t i = 0;
  uint64_t discrepancies = 0;

  ow_object_ptr = (OneWire_Object *) malloc(sizeof(OneWire_Object));
  ow_init(ow_object_ptr, ds18b20_port, ds18b20_pin);

  ds18b20_objects = (Ds18b20_Object *) malloc(sizeof(Ds18b20_Object) * ds18b20_amount);

  for(i = 0; i < ds18b20_amount; i++) {
    discrepancies = ds18b20_search_once(i);
    if (discrepancies == 0) {
      return;
    }
  }
}

Ds18b20_Temperature ds18b20_convert_temp(OneWire_Byte lsb, OneWire_Byte msb) {
  Ds18b20_Temperature temperature;
  uint8_t decimal;
  uint8_t integer;

  decimal = ((lsb & 0xF));
  integer = ((msb & 0x7) << 4) + ((lsb >> 4) & 0xF);

  temperature.decimal = decimal;
  temperature.integer = integer;

  return temperature;
}

void ds18b20_read() {
  if (ds18b20_objects[ds18b20_index].address != 0) {
    ow_reset(ow_object_ptr);
    ow_write_byte(ow_object_ptr, DS18B20_COMMAND_MATCH_ROM);
    ow_write_address(ow_object_ptr, ds18b20_objects[ds18b20_index].address);
    ow_write_byte(ow_object_ptr, DS18B20_COMMAND_START_CONVERSION);

    ow_reset(ow_object_ptr);
    ow_write_byte(ow_object_ptr, DS18B20_COMMAND_MATCH_ROM);
    ow_write_address(ow_object_ptr, ds18b20_objects[ds18b20_index].address);
    ow_write_byte(ow_object_ptr, DS18B20_COMMAND_READ_SCRATCH_PAD);

    Ds18b20_Temperature temperature = ds18b20_convert_temp(
      ow_read_byte(ow_object_ptr),
      ow_read_byte(ow_object_ptr)
    );

    ds18b20_objects[ds18b20_index].temperature = temperature;
  } else {
    ds18b20_objects[ds18b20_index].temperature.integer = 0;
    ds18b20_objects[ds18b20_index].temperature.decimal = 0;
  }
}

void ds18b20_stop() {
  free(ow_object_ptr);
  free(ds18b20_objects);
}

bool ds18b20_ready_to_search() {
  return ds18b20_amount >= 0 && ds18b20_pin >= 0 && ds18b20_port >= 0;
}

bool ds18b20_ready_to_read() {
  return ds18b20_amount >= 0 && ds18b20_pin >= 0 && ds18b20_port >= 0 && ds18b20_index >= 0;
}

/*--------------------------SENSOR INTERFACE----------------------------------*/
const struct sensors_sensor ds18b20;

static int value(int type) {
  if (!ds18b20_ready_to_read()) {
    return DS18B20_RESPONSE_ERROR;
  }

  switch(type) {
  case DS18B20_VALUE_ADDRESS:
    return (int) ds18b20_objects[ds18b20_index].address;

  case DS18B20_VALUE_TEMPERATURE_INTEGER:
    return (int) ds18b20_objects[ds18b20_index].temperature.integer;

  case DS18B20_VALUE_TEMPERATURE_DECIMAL:
    return (int) ds18b20_objects[ds18b20_index].temperature.decimal;
  }

  return DS18B20_RESPONSE_ERROR;
}

static int status(int type) {
  if (ds18b20_ready_to_read()) {
    return DS18B20_STATUS_READ_READY;
  } else if (ds18b20_ready_to_search()) {
    return DS18B20_STATUS_SEARCH_READY;
  } else {
    return DS18B20_STATUS_NOT_READY;
  }
}

static int configure(int type, int c) {
  switch(type) {
    case DS18B20_CONFIGURATION_SEARCH:
      if (!ds18b20_ready_to_search()) {
        return DS18B20_RESPONSE_ERROR;
      } else {
        ds18b20_search();
        return DS18B20_RESPONSE_SUCCESS;
      }

    case DS18B20_CONFIGURATION_READ:
      if (!ds18b20_ready_to_read()) {
        return DS18B20_RESPONSE_ERROR;
      } else {
        ds18b20_read();
        return DS18B20_RESPONSE_SUCCESS;
      }

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

    case DS18B20_CONFIGURATION_ERASE:
      ds18b20_stop();
      return DS18B20_RESPONSE_SUCCESS;
  }

  return DS18B20_RESPONSE_ERROR;
}

SENSORS_SENSOR(ds18b20, "DS18B20", value, configure, status);
