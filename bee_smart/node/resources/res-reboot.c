#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "coap-engine.h"

extern int wd_no_msg_timer;

static void res_get_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);
static void res_post_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);
static void res_put_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);
static void res_delete_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);

RESOURCE(res_reboot,
          "title=\"Reboot\",Desc=\"Reinicia el nodo\",GET=\"Reinicia el nodo\",POST|PUT=\"nf\",DEL=\"nf\"",
          res_get_handler,
          res_post_handler,
          res_put_handler,
          res_delete_handler);

static void
res_get_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
  wd_no_msg_timer = 0;
  const char *len = NULL;

  /* Some data that has the length up to REST_MAX_CHUNK_SIZE. For more, see the chunk resource. */

  char message[10];

  sprintf(message, "Reboot");

  int length = strlen(message);

  /* The query string can be retrieved by rest_get_query() or parsed for its key-value pairs. */
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

  coap_set_header_content_format(response, TEXT_PLAIN); /* text/plain is the default, hence this option could be omitted. */
  coap_set_header_etag(response, (uint8_t *)&length, 1);
  coap_set_payload(response, buffer, length);

  printf("[LOG: User] GET successful\n");

  printf("Rebooting...\n");
  watchdog_reboot();
}

static void
res_post_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{

	const uint8_t * payload;

  coap_get_payload(request, &payload);

	printf("DATA: %s\n", payload);

	printf("[LOG: User] POST successful\n");
}

static void
res_put_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{

	const uint8_t * payload;

	coap_get_payload(request, &payload);

	printf("DATA: %s\n", payload);

	printf("[LOG: User] PUT successful\n");
}

static void
res_delete_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{

  printf("[LOG: User] DELETE successful\n");
}

