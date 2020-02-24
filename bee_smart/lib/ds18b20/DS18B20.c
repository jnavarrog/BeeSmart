#include <DS18B20.h>
#include <stdlib.h>

#define DS18B20_COMMAND_READ_SCRATCH_PAD 0xBE
#define DS18B20_COMMAND_START_CONVERSION 0x44
#define DS18B20_COMMAND_SKIP_ROM 0xCC
#define DS18B20_COMMAND_MATCH_ROM 0x55
#define DS18B20_COMMAND_SEARCH_ROM 0xF0

#define DS18B20_SEARCH_DISC 0b00
#define DS18B20_SEARCH_ALL0 0b01
#define DS18B20_SEARCH_ALL1 0b10
#define DS18B20_SEARCH_NODS 0b11

Ds18b20_Object ds18b20_init(OneWire_Object * ow_object_ptr, Ds18b20_Address address);

Ds18b20_Temperature ds18b20_convert_temp(OneWire_Byte lsb, OneWire_Byte msb);

void ds18b20_read_temp_from_address(Ds18b20_Object * ds18b20_object);

uint64_t ds18b20_search_once(uint8_t index, Ds18b20_Object * ds18b20_objects, OneWire_Object * ow_object_ptr);

Ds18b20_Object* ds18b20_search_all(
  Ds18b20_Object * ds18b20_objects, Ds18b20_Port port, Ds18b20_Pin pin, uint8_t max_amount
);

Ds18b20_Object ds18b20_init(OneWire_Object * ow_object_ptr, Ds18b20_Address address) {
  Ds18b20_Object ds18b20_object;

  ds18b20_object.initiated = true;
  ds18b20_object.address = address;
  ds18b20_object.ow_object_ptr = ow_object_ptr;

  return ds18b20_object;
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

void ds18b20_read_temp_from_address(Ds18b20_Object * ds18b20_object_ptr) {
  if (ds18b20_object_ptr->address != 0) {
    ow_reset(ds18b20_object_ptr->ow_object_ptr);
    ow_write_byte(ds18b20_object_ptr->ow_object_ptr, DS18B20_COMMAND_MATCH_ROM);
    ow_write_address(ds18b20_object_ptr->ow_object_ptr, ds18b20_object_ptr->address);
    ow_write_byte(ds18b20_object_ptr->ow_object_ptr, DS18B20_COMMAND_START_CONVERSION);

    ow_reset(ds18b20_object_ptr->ow_object_ptr);
    ow_write_byte(ds18b20_object_ptr->ow_object_ptr, DS18B20_COMMAND_MATCH_ROM);
    ow_write_address(ds18b20_object_ptr->ow_object_ptr, ds18b20_object_ptr->address);
    ow_write_byte(ds18b20_object_ptr->ow_object_ptr, DS18B20_COMMAND_READ_SCRATCH_PAD);

    Ds18b20_Temperature temperature = ds18b20_convert_temp(
      ow_read_byte(ds18b20_object_ptr->ow_object_ptr),
      ow_read_byte(ds18b20_object_ptr->ow_object_ptr)
    );

    ds18b20_object_ptr->temperature = temperature;
  } else {
    ds18b20_object_ptr->temperature.integer = 0;
    ds18b20_object_ptr->temperature.decimal = 0;
  }
}

uint64_t ds18b20_search_once(uint8_t index, Ds18b20_Object * ds18b20_objects, OneWire_Object * ow_object_ptr) {
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
    if (last_bit == DS18B20_SEARCH_ALL0) {
      if (i >= last_discrepancie) {
        clear_bit(&discrepancies, i);
        clear_bit(&address, i);
      }
    } else if (last_bit == DS18B20_SEARCH_ALL1) {
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
      ds18b20_objects[index] = ds18b20_init(ow_object_ptr, address);
      return discrepancies;
    }
    ow_write_bit(ow_object_ptr, (uint8_t) bit_at(address, i));
  }
  ds18b20_objects[index] = ds18b20_init(ow_object_ptr, address);
  last_address = address;
  return discrepancies;
}

Ds18b20_Object* ds18b20_search_all(
  Ds18b20_Object * ds18b20_objects, Ds18b20_Port port, Ds18b20_Pin pin, uint8_t max_amount
) {
  uint8_t i = 0;
  OneWire_Object * ow_object;
  ow_object = (OneWire_Object *) malloc(sizeof(OneWire_Object));
  ow_init(ow_object, port, pin);

  for(i = 0; i < max_amount; i++) {
    ds18b20_search_once(i, ds18b20_objects, ow_object);
  }

  return ds18b20_objects;
}
