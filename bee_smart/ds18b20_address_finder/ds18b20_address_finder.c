#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "contiki.h"
#include <DS18B20.h>
#include <ADDRESSES_FILE.h>

#include "sys/log.h"
#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_APP

int ds18b20_port_int = 0;
int ds18b20_pin_int = 23;
int ds18b20_amount_int = 9;

PROCESS(ds18b20_address_finder, "Buscador de direcciones 1-wire");
AUTOSTART_PROCESSES(&ds18b20_address_finder);

PROCESS_THREAD(ds18b20_address_finder, ev, data)
{
  static  Ds18b20_Object * ds18b20_objects;

  PROCESS_BEGIN();

  start_addresses_file(ds18b20_amount_int);
  ds18b20_objects = (Ds18b20_Object *) malloc(sizeof(Ds18b20_Object[ds18b20_amount_int]));
  ds18b20_search_all(ds18b20_objects, ds18b20_port_int, ds18b20_pin_int, ds18b20_amount_int, false);

  for (int i  = 0; i < ds18b20_amount_int; i++) {
    write_address(ds18b20_objects[i].address);
    printf("%d - %llx\n", i, ds18b20_objects[i].address);
  }

  free(ds18b20_objects);

  for (int i  = 0; i < ds18b20_amount_int; i++) {
    printf("%d - %llx\n", i, read_address(i));
  }

  PROCESS_END();
}
