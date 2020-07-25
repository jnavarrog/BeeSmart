#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "contiki.h"
#include "dev/eeprom.h"
#include <DS18B20.h>

#include "sys/log.h"
#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_APP

int ds18b20_port_int = 0;
int ds18b20_pin_int = 0x17;
int ds18b20_amount_int = 9;

// void print_saved_addresses() {
//   printf("Direcciones guardadas:\n");
//   uint64_t address = 0;
//   for (int i = 0; i < ds18b20_amount_int; i++) {
//     address = 0;
//     for (int j = 0; j < 8; j++) {
//       uint8_t byte = 0;
//       read_eeprom((eeprom_addr_t) i * 8 + j, &byte, 1);
//       address <<= 8;
//       address |= byte;
//     }
//     printf("%d-%llx\n", i, address);
//   }
// }

// void write_addresses(Ds18b20_Object * ds18b20_objects) {
//   for (int i = 0; i < ds18b20_amount_int; i++) {
//     for(int j = 0; j < 8; j++) {
//       uint8_t byte = (ds18b20_objects[i].address >> 8 * (7 - i)) && 0xFF;
//       write_eeprom((eeprom_addr_t) i * 8 + j, &byte, 1);
//     }
//   }
// }

PROCESS(ds18b20_address_finder, "Buscador de direcciones 1-wire");
AUTOSTART_PROCESSES(&ds18b20_address_finder);

PROCESS_THREAD(ds18b20_address_finder, ev, data)
{
  static  Ds18b20_Object * ds18b20_objects;

  PROCESS_BEGIN();

  while(1) {
    ds18b20_objects = (Ds18b20_Object *) malloc(sizeof(Ds18b20_Object[ds18b20_amount_int]));
    ds18b20_search_all(ds18b20_objects, ds18b20_port_int, ds18b20_pin_int, ds18b20_amount_int);

    for (int i  = 0; i < ds18b20_amount_int; i++) {
      printf("%d - %llx\n", i, ds18b20_objects[i].address);
    }

    // write_addresses(ds18b20_objects);

    free(ds18b20_objects);

    PROCESS_WAIT_EVENT();

    // print_saved_addresses();

    printf("\n\n");
  }

  PROCESS_END();
}
