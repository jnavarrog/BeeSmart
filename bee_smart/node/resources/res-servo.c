#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "coap-engine.h"
#include <SERVO_SENSOR.h>
#include <BUTTON_SENSOR.h>

#define SERVO_OPEN SERVO_SENSOR_OPEN_POSITION
#define SERVO_CLOSE SERVO_SENSOR_CLOSE_POSITION

extern int wd_no_msg_timer;

static void res_get_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);
static void res_post_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);
static void res_put_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);
static void res_delete_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);

RESOURCE(
  res_servo,
  "title=\"Servo\",Desc=\"Controla el servo que maneja la puerta de la colmena\",GET=\"Posición actual de la puerta\",POST=\"1 para abrir, 0 para cerrar\"",
  res_get_handler,
  res_post_handler,
  res_put_handler,
  res_delete_handler
);

static void res_get_handler(
  coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset
) {
  wd_no_msg_timer = 0;
  const char *len = NULL;
  char message[20];

  sprintf(message, "%d", button.value(BUTTON_VALUE));

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

  printf("[LOG User] GET SERVO successful\n");
}

static void res_post_handler(
  coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset
) {
    wd_no_msg_timer = 0;
    const uint8_t * payload;
    coap_get_payload(request, &payload);
    int action = atoi((const char *) payload);

    printf("DATA INT: %d\n", action);

   if (action == 1) {
      servo.configure(SERVO_CONFIGURATION_POSITION, SERVO_OPEN);
      servo.value(SERVO_VALUE_MOVE);
    } else if (action == 0) {
      servo.configure(SERVO_CONFIGURATION_POSITION, SERVO_CLOSE);
      servo.value(SERVO_VALUE_MOVE);
    }

	printf("DATA: %s\n", payload);

	printf("[LOG: User] POST SERVO successful\n");
}

static void res_put_handler(
  coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset
) {
	const uint8_t * payload;
  coap_get_payload(request, &payload);
	printf("DATA: %s\n", payload);
	printf("[LOG: User] PUT SERVO successful\n");
}

static void res_delete_handler(
  coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset
) {
	const uint8_t * payload;
  coap_get_payload(request, &payload);
	printf("DATA: %s\n", payload);
	printf("[LOG: User] DELETE SERVO successful\n");
}
