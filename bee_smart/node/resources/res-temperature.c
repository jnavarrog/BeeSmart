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
  res_temperature,
  "title=\"Temperature\",Desc=\"Devuelve la temperatura de todos los sensores\",GET=\"Valor de temperatura de todos los sensores (sin la dirección)\",POST|PUT=\"nf\",DEL=\"nf\"",
  res_get_handler,
  res_post_handler,
  res_put_handler,
  res_delete_handler
);

static void
res_get_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset){
  const char *len = NULL;
  char message[62];

  int ds18b20_amount_int_res = DS18B20_AMOUNT_INT;

  for(int i = 0; i < ds18b20_amount_int_res; i++) {
    ds18b20.configure(DS18B20_CONFIGURATION_INDEX, i);
    ds18b20.configure(DS18B20_CONFIGURATION_READ, 0);

    int address_low = ds18b20.value(DS18B20_VALUE_ADDRESS_LOW);
    int address_high = ds18b20.value(DS18B20_VALUE_ADDRESS_HIGH);
    int integer = ds18b20.value(DS18B20_VALUE_TEMPERATURE_INTEGER);
    int decimal = ds18b20.value(DS18B20_VALUE_TEMPERATURE_DECIMAL);

    printf("Address %x%x: %d,%d\n", address_high, address_low, integer, decimal);
    sprintf(message + strlen(message), ",%d.%d", integer, decimal);
  }

  sprintf(message + strlen(message), ",");

  int length = strlen(message);

  if(coap_get_query_variable(request, "len", &len)) {
    length = atoi(len);
    if(length < 0) {
      length = 0;
    }
    if(length > REST_MAX_CHUNK_SIZE) {
      length = REST_MAX_CHUNK_SIZE;
    }
    memcpy(buffer, message, length);
  } else {
    memcpy(buffer, message, length);
  }

  coap_set_header_content_format(response, TEXT_PLAIN);
  coap_set_header_etag(response, (uint8_t *)&length, 1);
  coap_set_payload(response, buffer, length);

  printf("[LOG: User] GET TEMPERATURE successful\n");
}

static void res_post_handler(
  coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset
) {
	const uint8_t * payload;
	coap_get_payload(request, &payload);
	printf("DATA: %s\n", payload);
	printf("[LOG: User] POST TEMPERATURE successful\n");
}

static void res_put_handler(
  coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset
) {
	const uint8_t * payload;
  coap_get_payload(request, &payload);
	printf("DATA: %s\n", payload);
	printf("[LOG: User] PUT TEMPERATURE successful\n");
}

static void res_delete_handler(
  coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset
) {
	const uint8_t * payload;
  coap_get_payload(request, &payload);
	printf("DATA: %s\n", payload);
	printf("[LOG: User] DELETE TEMPERATURE successful\n");
}

