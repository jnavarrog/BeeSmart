#include "contiki.h"
#include <HX711.h>
#include <Math.h>
#include <stdio.h>

PROCESS(balanza_lectura_continua, "balanza_lectura_continua");
AUTOSTART_PROCESSES(&balanza_lectura_continua);

PROCESS_THREAD(balanza_lectura_continua, ev, data) {
  PROCESS_BEGIN();
  printf("balanza_lectura_continua.c\n");
  static struct etimer period;
  static Hx711_Port hx711_port = GPIO_HAL_NULL_PORT;
  static Hx711_Pin hx711_pin_dout = IOID_25;
  static Hx711_Pin hx711_pin_sck = IOID_26;
  static Hx711_Object hx711_object;

  hx711_object = hx711_init(hx711_port, hx711_pin_dout, hx711_pin_sck);
  hx711_read_continuos(&hx711_object);

  while (1) {
    printf("Weight: %ld\n", hx711_object.weight);
    etimer_set(&period, CLOCK_SECOND);
    PROCESS_WAIT_UNTIL(etimer_expired(&period));
  }

  PROCESS_END();
}
