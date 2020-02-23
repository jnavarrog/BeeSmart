#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "coap-engine.h"
#include <DS18B20_SENSOR.h>
#include <math.h>
#include <stdio.h>

static void res_get_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);
static void res_post_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);
static void res_put_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);
static void res_delete_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);

RESOURCE(
  res_addresstemperature,
  "title=\"AddressTemperature\",Desc=\"Temperatura y dirección de los sensores de temperatura\",GET=\"Devuelve la dirección y el valor de temperatura de cada sensor\"",
  res_get_handler,
  res_post_handler,
  res_put_handler,
  res_delete_handler
);

int i = 0;

#define CHUNKS_TOTAL    512

static void res_get_handler(
  coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset
) {
  int32_t strpos = 0;

  if(*offset >= CHUNKS_TOTAL) {
    coap_set_status_code(response, BAD_OPTION_4_02);
    const char *error_msg = "BlockOutOfScope";
    coap_set_payload(response, error_msg, strlen(error_msg));
    return;
  }

  const char *iteration = NULL;
  if (coap_get_query_variable(request, "iteration", &iteration)) {
    i = atoi(iteration);
  }

  int ds18b20_amount_int_res = DS18B20_AMOUNT_INT;

  ds18b20.configure(DS18B20_CONFIGURATION_INDEX, i);
  ds18b20.configure(DS18B20_CONFIGURATION_READ, 0);

  int address_low = ds18b20.value(DS18B20_VALUE_ADDRESS_LOW);
  int address_high = ds18b20.value(DS18B20_VALUE_ADDRESS_HIGH);
  int integer = ds18b20.value(DS18B20_VALUE_TEMPERATURE_INTEGER);
  int decimal = ds18b20.value(DS18B20_VALUE_TEMPERATURE_DECIMAL);

  strpos += snprintf((char *) buffer + strpos, preferred_size - strpos + 1, ",%x%x:%d.%d", address_high, address_low, integer, decimal);

  printf("Address: %x%x Temperature: %d,%d iteration: %d\n", address_high, address_low, integer, decimal, i);

  if(strpos > preferred_size) {
    strpos = preferred_size;
  }
  if(*offset + (int32_t)strpos > CHUNKS_TOTAL) {
    strpos = CHUNKS_TOTAL - *offset;
  }
  coap_set_payload(response, buffer, strpos);

  *offset += strpos;

  if(i == ds18b20_amount_int_res) {
    i=0;
    *offset = -1;
  }

  printf("[LOG: User] GET ADDRESS TEMPERATURE successful\n");
}

static void res_post_handler(
  coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset
) {
	const uint8_t * payload;
	coap_get_payload(request, &payload);
	printf("DATA: %s\n", payload);
	printf("[LOG: User] POST ADDRESS TEMPERATURE successful\n");
}

static void res_put_handler(
  coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset
) {
	const uint8_t * payload;
  coap_get_payload(request, &payload);
	printf("DATA: %s\n", payload);
	printf("[LOG: User] PUT ADDRESS TEMPERATURE successful\n");
}

static void res_delete_handler(
  coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset
) {
	const uint8_t * payload;
  coap_get_payload(request, &payload);
	printf("DATA: %s\n", payload);
	printf("[LOG: User] DELETE ADDRESS TEMPERATURE successful\n");
}
