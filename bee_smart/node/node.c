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
#include "contiki.h"
#include "coap-engine.h"
#include "orchestra.h"
#include "tsch.h"
#include <DS18B20_SENSOR.h>

/* Log configuration */
#include "sys/log.h"
#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_APP

//temp
int ds18b20_port_int = GPIO_HAL_NULL_PORT;
int ds18b20_pin_int = IOID_23;
int ds18b20_amount_int = DS18B20_AMOUNT_INT;

//Definición de los recursos (su implementación va en la carpeta resources)
extern coap_resource_t
  res_test,
  res_addresstemperature,
  res_temperature,
  res_reboot,
  res_weight,
  res_servo;


PROCESS(er_example_server, "Servidor CoAP");
AUTOSTART_PROCESSES(&er_example_server);

PROCESS_THREAD(er_example_server, ev, data)
{
  PROCESS_BEGIN();
  
  //temperature
  SENSORS_ACTIVATE(ds18b20);
  ds18b20.configure(DS18B20_CONFIGURATION_AMOUNT, ds18b20_amount_int);
  ds18b20.configure(DS18B20_CONFIGURATION_PORT, ds18b20_port_int);
  ds18b20.configure(DS18B20_CONFIGURATION_PIN, ds18b20_pin_int);
  ds18b20.configure(DS18B20_CONFIGURATION_START, 0);
  
  //servo
  servo_position_now=0;
  
  PROCESS_PAUSE();

  LOG_INFO("BEESMART - PROYECTO FIN DE CARRERA 2020 - NODO\n");

  //Activación y definición de la URI de los recursos
  coap_activate_resource(&res_test, "test");
  coap_activate_resource(&res_addresstemperature, "sensors/temperature/all");
  coap_activate_resource(&res_temperature, "sensors/temperature/temperature");
  coap_activate_resource(&res_reboot, "commands/reboot");
  coap_activate_resource(&res_weight, "sensors/weight");
  coap_activate_resource(&res_servo, "actuators/servo");

  /* Define application-specific events here. */
  while(1) {
    PROCESS_WAIT_EVENT();

  }                             /* while (1) */
  ds18b20.configure(DS18B20_CONFIGURATION_STOP, 0);
  PROCESS_END();
}
