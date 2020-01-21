/*
 * Copyright (c) 2013, Institute for Pervasive Computing, ETH Zurich
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file is part of the Contiki operating system.
 */

/**
 * \file
 *      Erbium (Er) CoAP Engine example.
 * \author
 *      Matthias Kovatsch <kovatsch@inf.ethz.ch>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "contiki.h"
#include "coap-engine.h"
#include "orchestra.h"
#include "tsch.h"

#include <lib/sensors.h>
#include <HX711.h>
#include <DS18B20.h>

/* Log configuration */
#include "sys/log.h"
#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_APP

typedef struct Pfe_Sensor_Configuration {
  bool                temperature_sensor_started;
  uint8_t             temperature_sensor_index;
  Hx711_Reading_Type  weight_reading_type;
} Pfe_Sensor_Configuration;

//Definición de los recursos (su implementación va en la carpeta resources)
extern coap_resource_t
  res_test,
  res_temperature;


PROCESS(er_example_server, "Servidor CoAP");
PROCESS(ds18b20, "ds18b20");
AUTOSTART_PROCESSES(&er_example_server, &ds18b20);

PROCESS_THREAD(er_example_server, ev, data)
{
  PROCESS_BEGIN();

  PROCESS_PAUSE();

  LOG_INFO("Starting Erbium Example Server\n");

  //Activación y definición de la URI de los recursos
  coap_activate_resource(&res_test, "test");
  coap_activate_resource(&res_temperature, "sensors/temperature");

  /* Define application-specific events here. */
  while(1) {
    PROCESS_WAIT_EVENT();

  }                             /* while (1) */

  PROCESS_END();
}

PROCESS_THREAD(ds18b20, ev, data) {
  static struct etimer periodic;
  PROCESS_BEGIN();
  printf("temperature.c\n");

  SENSORS_ACTIVATE(pfe_sensors);
  pfe_sensors.configure(PFE_SENSOR_CONFIGURATION_TYPE_TEMPERATURE_START, 0);

  while(1) {
    etimer_set(&periodic, 20);

    printf("------------- TEMPERATURES ----------\n");
    for(int i = 0; i < DS18B20_AMOUNT; i++) {
      pfe_sensors.configure(PFE_SENSOR_CONFIGURATION_TYPE_TEMPERATURE_INDEX, i);
      int temperature = pfe_sensors.value(PFE_SENSOR_TYPE_TEMPERATURE_TEMPERATURE);
      int address = pfe_sensors.value(PFE_SENSOR_TYPE_TEMPERATURE_ADDRESS);
      printf("Address: %x - Temperature: %d.%d\n", address, temperature / 256, temperature % 256);
    }

    PROCESS_WAIT_UNTIL(etimer_expired(&periodic));
  }
  PROCESS_END();
}

//----------------------------//

Ds18b20_Port ds18b20_port = GPIO_HAL_NULL_PORT;
Ds18b20_Pin ds18b20_pin = IOID_25;
Ds18b20_Object ds18b20_objects[DS18B20_AMOUNT];

Pfe_Sensor_Configuration pfe_sensor_configuration;

static int status(int type) {
  return (int) pfe_sensor_configuration.temperature_sensor_started;
}

static int value(int type) {
  int index;
  index = pfe_sensor_configuration.temperature_sensor_index;

  switch (type) {
    case PFE_SENSOR_TYPE_TEMPERATURE_TEMPERATURE:
      ds18b20_read_temp_from_address(&(ds18b20_objects[index]));
      return (int) ds18b20_objects[index].temperature.integer << 8 |
              (int) ds18b20_objects[index].temperature.decimal;
    case PFE_SENSOR_TYPE_TEMPERATURE_ADDRESS:
      return (int) ds18b20_objects[index].address << 8;
  }

  return -1;
}

static int configure(int type, int value) {
  switch (type) {
    case PFE_SENSOR_CONFIGURATION_TYPE_TEMPERATURE_START:
      ds18b20_search_all(ds18b20_objects, ds18b20_port, ds18b20_pin, DS18B20_AMOUNT);
      pfe_sensor_configuration.temperature_sensor_started = true;
      return status(type);
    case PFE_SENSOR_CONFIGURATION_TYPE_TEMPERATURE_INDEX:
      pfe_sensor_configuration.temperature_sensor_index = value;
      return status(type);
  }

  return -1;
}

SENSORS_SENSOR(pfe_sensors, "PFE_SENSORS", value, configure, status);
