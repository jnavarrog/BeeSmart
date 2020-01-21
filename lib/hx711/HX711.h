#include <stdint.h>
#include <gpio-hal.h>

typedef gpio_hal_port_t Hx711_Port;
typedef gpio_hal_pin_t Hx711_Pin;
typedef gpio_hal_pin_mask_t Hx711_Pin_Mask;
typedef gpio_hal_event_handler_t Hx711_Handler;

typedef uint32_t Hx711_Weight;

typedef enum Hx711_Reading_Type {
  HX711_READING_TYPE_NONE,
  HX711_READING_TYPE_ONCE,
  HX711_READING_TYPE_CONTINUOS
} Hx711_Reading_Type;

typedef struct Hx711_Object {
  Hx711_Port          port;
  Hx711_Pin           pin_sck;
  Hx711_Pin           pin_dout;
  Hx711_Weight        weight;
  Hx711_Reading_Type  reading_type;
  Hx711_Handler *     handler;
} Hx711_Object;


Hx711_Object hx711_init(Hx711_Port port, Hx711_Pin pin_dout, Hx711_Pin pin_sck);

void hx711_read_once(Hx711_Object * hx711_object);

void hx711_read_continuos(Hx711_Object * hx711_object);

void hx711_stop(Hx711_Object * hx711_object);
