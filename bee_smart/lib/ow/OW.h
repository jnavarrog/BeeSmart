#include <gpio-hal.h>
#ifndef OW_H_INCLUDED
#define OW_H_INCLUDED

typedef gpio_hal_port_t OneWire_Port;
typedef gpio_hal_pin_t OneWire_Pin;

typedef enum OneWire_Direction {
  ONEWIRE_DIRECTION_UNKNOWN,
  ONEWIRE_DIRECTION_INPUT,
  ONEWIRE_DIRECTION_OUTPUT
} OneWire_Direction;

typedef struct OneWire_Object {
    bool                  initiated;
    OneWire_Direction     direction;
    OneWire_Port          port;
    OneWire_Pin           pin;
} OneWire_Object;

typedef uint64_t OneWire_Address;
typedef uint8_t OneWire_Byte;

void ow_delay(uint16_t us);

bool bit_at(uint64_t value, uint8_t index);

void set_bit(uint64_t* value, uint8_t index);

void clear_bit(uint64_t* value, uint8_t index);

void toggle_bit(uint64_t* value, uint8_t index);

void ow_write_bit(OneWire_Object * ow_object_ptr, uint8_t bit);

uint8_t ow_read_bit(OneWire_Object * ow_object_ptr);

void ow_init(OneWire_Object * ow_object, OneWire_Port port, OneWire_Pin pin);

void ow_write_byte(OneWire_Object * ow_object_ptr, OneWire_Byte byte);

OneWire_Byte ow_read_byte(OneWire_Object * ow_object_ptr);

void ow_write_address(OneWire_Object * ow_object_ptr, OneWire_Address address);

OneWire_Byte ow_reset(OneWire_Object * ow_object_ptr);

#endif
