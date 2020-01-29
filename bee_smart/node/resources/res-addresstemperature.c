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
 *      Example resource
 * \author
 *      Matthias Kovatsch <kovatsch@inf.ethz.ch>
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "coap-engine.h"
#include <DS18B20_SENSOR.h>
#include <math.h>
#include <stdio.h>

//Defino los handler para cada metodo
static void res_get_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);
static void res_post_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);
static void res_put_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);
static void res_delete_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);

//Defino el recurso
RESOURCE(res_addresstemperature,
         "title=\"Address-Temperature",
         res_get_handler,
	 res_post_handler,
	 res_put_handler,
         res_delete_handler);
         
int i = 0;

//GET
#define CHUNKS_TOTAL    512

static void
res_get_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
  int32_t strpos = 0;

  /* Check the offset for boundaries of the resource data. */
  if(*offset >= CHUNKS_TOTAL) {
    coap_set_status_code(response, BAD_OPTION_4_02);
    /* A block error message should not exceed the minimum block size (16). */

    const char *error_msg = "BlockOutOfScope";
    coap_set_payload(response, error_msg, strlen(error_msg));
    return;
  }
  //----------------------------------------------------------------------------------------

  int ds18b20_amount_int_res = DS18B20_AMOUNT_INT;
  
  ds18b20.configure(DS18B20_CONFIGURATION_INDEX, i);
  ds18b20.configure(DS18B20_CONFIGURATION_READ, 0);

  int address_low = ds18b20.value(DS18B20_VALUE_ADDRESS_LOW);
  int address_high = ds18b20.value(DS18B20_VALUE_ADDRESS_HIGH);
  int integer = ds18b20.value(DS18B20_VALUE_TEMPERATURE_INTEGER);
  int decimal = ds18b20.value(DS18B20_VALUE_TEMPERATURE_DECIMAL);

  strpos += snprintf((char *)buffer + strpos, preferred_size - strpos + 1,"|Address:%x%x-Temperature:%d,%d|", address_high, address_low, integer, decimal);
     
   printf("Address:%x%x Temperature:%d,%d iter: %d len %d\n", address_high, address_low, integer, decimal,i, strlen((char *)buffer)); 
   i++;
  
  //----------------------------------------------------------------------------------------

  /* snprintf() does not adjust return value if truncated by size. */
  if(strpos > preferred_size) {
    strpos = preferred_size;
    /* Truncate if above CHUNKS_TOTAL bytes. */
  }
  if(*offset + (int32_t)strpos > CHUNKS_TOTAL) {
    strpos = CHUNKS_TOTAL - *offset;
  }
  coap_set_payload(response, buffer, strpos);

  /* IMPORTANT for chunk-wise resources: Signal chunk awareness to REST engine. */
  *offset += strpos;

  /* Signal end of resource representation. */
  if(i == ds18b20_amount_int_res) {
    i=0;
    *offset = -1;
  }
}//end get

//POST
static void
res_post_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{

	const uint8_t * payload;

	/*int len = */ coap_get_payload(request, &payload);

	//Datos recibidos en el post (estan en la variable payload)
	printf("DATA: %s\n", payload);

	printf("[LOG: User] POST successful\n");


}//end post

//PUT
static void
res_put_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{

	const uint8_t * payload;

	/*int len = */ coap_get_payload(request, &payload);

	//Datos recibidos en el put (estan en la variable payload)
	printf("DATA: %s\n", payload);

	printf("[LOG: User] PUT successful\n");


}//end put

//DELETE
static void
res_delete_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{

  printf("[LOG: User] DELETE successful\n");

}// end delete

