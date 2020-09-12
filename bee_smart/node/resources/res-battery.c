#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "coap-engine.h"
#include "batmon-sensor.h"

static void res_get_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);
static void res_post_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);
static void res_put_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);
static void res_delete_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);

RESOURCE(
  res_battery,
  "title=\"Weight\",Desc=\"Bateria de la colmena\",GET=\"Devuelve la bateria de la colmena\"",
  res_get_handler,
  res_post_handler,
  res_put_handler,
  res_delete_handler
);

static void res_get_handler(
  coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
  const char *len = NULL;
  char message[62];

  sprintf(message, "%d", (batmon_sensor.value(BATMON_SENSOR_TYPE_VOLT) * 125) >> 5);

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

  printf("[LOG: User] GET BATTERY successful\n");
}

static void res_post_handler(
  coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset
) {
	const uint8_t * payload;
	coap_get_payload(request, &payload);
	printf("DATA: %s\n", payload);
	printf("[LOG: User] POST BATTERY successful\n");
}

static void res_put_handler(
  coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset
) {
	const uint8_t * payload;
  coap_get_payload(request, &payload);
	printf("DATA: %s\n", payload);
	printf("[LOG: User] PUT BATTERY successful\n");
}

static void res_delete_handler(
  coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset
) {
	const uint8_t * payload;
  coap_get_payload(request, &payload);
	printf("DATA: %s\n", payload);
	printf("[LOG: User] DELETE BATTERY successful\n");
}
