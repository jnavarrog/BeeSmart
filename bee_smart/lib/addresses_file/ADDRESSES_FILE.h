
#include <DS18B20.h>
#ifndef ADDRESSES_FILE_H_INCLUDED
#define ADDRESSES_FILE_H_INCLUDED

void start_addresses_file(int ds18b20_amount);
uint8_t write_address(Ds18b20_Address address);
Ds18b20_Address read_address(uint8_t index);

#endif
