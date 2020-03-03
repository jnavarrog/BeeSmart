#include "gpio-hal.h"
#include <stdlib.h>
#include <OW.h>

#define ONEWIRE_OUTPUT_TIME_WRITE 10
#define ONEWIRE_INPUT_TIME_WRITE_1 45
#define ONEWIRE_OUTPUT_TIME_WRITE_0 35
#define ONEWIRE_INPUT_TIME_WRITE_0 10

#define ONEWIRE_OUTPUT_TIME_READ 6
#define ONEWIRE_INPUT_TIME_READ 4
#define ONEWIRE_HOLD_TIME_READ 45

#define ONEWIRE_OUTPUT_TIME_RESET 500
#define ONEWIRE_INPUT_TIME_RESET 70
#define ONEWIRE_HOLD_TIME_RESET 70

bool bit_at(uint64_t value, uint8_t index) {
  return (bool)(((uint64_t) 1 << (63 - index)) & value) == 1;
}

void set_bit(uint64_t* value, uint8_t index) {
  *(value) |= ((uint64_t) 1) << (63 - index);
}

void clear_bit(uint64_t* value, uint8_t index) {
  *(value) &= ~((uint64_t) 1) << (63 - index);
}

void toggle_bit(uint64_t* value, uint8_t index) {
  if (bit_at(*value, index)) {
    clear_bit(value, index);
  } else {
    set_bit(value, index);
  }
}

void ow_init(OneWire_Object * ow_object, OneWire_Port port, OneWire_Pin pin) {
  ow_object->initiated = true;
  ow_object->direction = ONEWIRE_DIRECTION_UNKNOWN;
  ow_object->port = port;
  ow_object->pin = pin;
}

void ow_set_output(OneWire_Object * ow_object_ptr) {
  gpio_hal_arch_pin_set_output(ow_object_ptr->port, ow_object_ptr->pin);
  ow_object_ptr->direction = ONEWIRE_DIRECTION_OUTPUT;
}

void ow_set_input(OneWire_Object * ow_object_ptr) {
  gpio_hal_arch_pin_set_input(ow_object_ptr->port, ow_object_ptr->pin);
  ow_object_ptr->direction = ONEWIRE_DIRECTION_INPUT;
}

void ow_set(OneWire_Object * ow_object_ptr) {
  gpio_hal_arch_write_pin(ow_object_ptr->port, ow_object_ptr->pin, 1);
}

void ow_clr(OneWire_Object * ow_object_ptr) {
  gpio_hal_arch_write_pin(ow_object_ptr->port, ow_object_ptr->pin, 0);
}

uint8_t ow_get_pin_state(OneWire_Object * ow_object_ptr) {
  return gpio_hal_arch_read_pin(ow_object_ptr->port, ow_object_ptr->pin);
}

void ow_delay(uint16_t us) {
  clock_delay_usec(us);
}

void ow_write_bit(OneWire_Object * ow_object_ptr, uint8_t bit) {
  ow_set_output(ow_object_ptr);
  ow_clr(ow_object_ptr);
  ow_delay(ONEWIRE_OUTPUT_TIME_WRITE);
  if(bit == 1) {
    ow_set_input(ow_object_ptr);
    ow_delay(ONEWIRE_INPUT_TIME_WRITE_1);
  } else {
    ow_delay(ONEWIRE_OUTPUT_TIME_WRITE_0);
    ow_set_input(ow_object_ptr);
    ow_delay(ONEWIRE_INPUT_TIME_WRITE_0);
  }
}

uint8_t ow_read_bit(OneWire_Object * ow_object_ptr) {
  uint8_t bit = 0;
  ow_set_output(ow_object_ptr);
  ow_clr(ow_object_ptr);
  ow_delay(ONEWIRE_OUTPUT_TIME_READ);
  ow_set_input(ow_object_ptr);
  ow_delay(ONEWIRE_INPUT_TIME_READ);
  bit = ow_get_pin_state(ow_object_ptr);
  ow_delay(ONEWIRE_HOLD_TIME_READ);
  return bit;
}

void ow_write_byte(OneWire_Object * ow_object_ptr, uint8_t byte) {
  uint8_t i = 8;
  while (i--) {
    ow_write_bit(ow_object_ptr, byte & 0x01);
    byte >>= 1;
  }
}

uint8_t ow_read_byte(OneWire_Object * ow_object_ptr) {
  uint8_t i = 8;
  uint8_t byte = 0;
  while (i--) {
    byte >>= 1;
    byte |= ((int) ow_read_bit(ow_object_ptr) << 7);
  }
  return byte;
}

void ow_write_address(OneWire_Object * ow_object_ptr, uint64_t address) {
  uint8_t i = 0;
  for (i = 0; i < 64; i++) {
    ow_write_bit(ow_object_ptr, (uint8_t) bit_at(address, i));
  }
}

uint8_t ow_reset(OneWire_Object * ow_object_ptr){
  ow_set_output(ow_object_ptr);
  ow_clr(ow_object_ptr);
  ow_delay(ONEWIRE_OUTPUT_TIME_RESET);
  ow_set_input(ow_object_ptr);
  ow_delay(ONEWIRE_INPUT_TIME_RESET);
  uint8_t presence = ow_get_pin_state(ow_object_ptr);
  ow_delay(ONEWIRE_HOLD_TIME_RESET);
  return !presence;
}
