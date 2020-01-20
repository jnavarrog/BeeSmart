#include "contiki.h"
#include <DS18B20.h>
#include <Math.h>
#include <stdio.h>

#define DS18B20_AMOUNT 9

Ds18b20_Port ds18b20_port = GPIO_HAL_NULL_PORT;
Ds18b20_Pin ds18b20_pin = IOID_25;

Ds18b20_Object ds18b20_objects[DS18B20_AMOUNT];

PROCESS(ds18b20, "ds18b20");
AUTOSTART_PROCESSES(&ds18b20);

PROCESS_THREAD(ds18b20, ev, data) {
  static struct etimer periodic;
  PROCESS_BEGIN();
  printf("ow_example.c\n");

  ds18b20_search_all(ds18b20_objects, ds18b20_port, ds18b20_pin, DS18B20_AMOUNT);

  while(1) {
    etimer_set(&periodic, 20);

    printf("------------- TEMPERATURES ----------\n");
    for(int i = 0; i < DS18B20_AMOUNT; i++) {
      ds18b20_read_temp_from_address(&(ds18b20_objects[i]));

      printf(
        "Address %llx: %d,%d\n",
        ds18b20_objects[i].address,
        ds18b20_objects[i].temperature.integer,
        ds18b20_objects[i].temperature.decimal
      );
    }

    PROCESS_WAIT_UNTIL(etimer_expired(&periodic));
  }
  PROCESS_END();
}
