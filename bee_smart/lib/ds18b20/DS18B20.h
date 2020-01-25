#include <OW.h>

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

extern Ds18b20_Object * ds18b20_search_all(
  Ds18b20_Object * ds18b20_objects, Ds18b20_Port port, Ds18b20_Pin pin, uint8_t max_amount
);

extern void ds18b20_read_temp_from_address(Ds18b20_Object * ds18b20_object);
