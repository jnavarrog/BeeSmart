#include "contiki.h"
#include <HX711.h>
#include <Math.h>
#include <stdio.h>

PROCESS(balanza_lectura_simple, "balanza_lectura_simple");
AUTOSTART_PROCESSES(&balanza_lectura_simple);

PROCESS_THREAD(balanza_lectura_simple, ev, data) {
  PROCESS_BEGIN();
  printf("balanza_lectura_simple.c\n");
  static Hx711_Port hx711_port = GPIO_HAL_NULL_PORT;
  static Hx711_Pin hx711_pin_dout = IOID_25;
  static Hx711_Pin hx711_pin_sck = IOID_26;
  static Hx711_Object hx711_object;

  hx711_object = hx711_init(hx711_port, hx711_pin_dout, hx711_pin_sck);
  hx711_read_once(&hx711_object);

  while (hx711_object.weight == 0) {
    PROCESS_YIELD();
  }

  printf("Weight: %ld\n", hx711_object.weight);

  PROCESS_END();
}
