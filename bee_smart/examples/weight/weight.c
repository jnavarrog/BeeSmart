#include "contiki.h"
#include <HX711_SENSOR.h>
#include <stdio.h>
#include <stdlib.h>

int hx711_port_int = GPIO_HAL_NULL_PORT;
int hx711_pin_dout_int = IOID_25;
int hx711_pin_sck_int = IOID_26;

PROCESS(hx711_example, "hx711_example");
AUTOSTART_PROCESSES(&hx711_example);

PROCESS_THREAD(hx711_example, ev, data) {
  static struct etimer periodic;
  PROCESS_BEGIN();
  printf("weight.c\n");

  SENSORS_ACTIVATE(hx711);
  hx711.configure(HX711_CONFIGURATION_PORT, hx711_port_int);
  hx711.configure(HX711_CONFIGURATION_PIN_DOUT, hx711_pin_dout_int);
  hx711.configure(HX711_CONFIGURATION_PIN_SCK, hx711_pin_sck_int);

  while(1) {
    etimer_set(&periodic, CLOCK_SECOND * 2);
    printf("------------- WEIGHT ----------\n");

    hx711.configure(HX711_CONFIGURATION_START_READ, 0);
    PROCESS_WAIT_UNTIL(hx711.status(0) == HX711_STATUS_GET_READ_READY);

    int weight_low = hx711.value(HX711_VALUE_WEIGHT_LOW);
    int weight_high = hx711.value(HX711_VALUE_WEIGHT_HIGH);

    printf("Weight: %d%d\n", weight_high, weight_low);
  }
  PROCESS_END();
}
