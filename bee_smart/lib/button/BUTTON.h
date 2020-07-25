#include <stdint.h>
#include <gpio-hal.h>
#include <stdbool.h>

typedef gpio_hal_port_t Button_Port;
typedef gpio_hal_pin_t Button_Pin;
typedef bool Button_Value;
typedef gpio_hal_pin_mask_t Button_Pin_Mask;
typedef gpio_hal_event_handler_t Button_Handler;

typedef uint32_t Button_Weight;

typedef struct Button_Object {
  Button_Port          port;
  Button_Pin           pin;
  Button_Value         value;
  Button_Handler       handler;
  void                 (*handler_on)(void);
  void                 (*handler_off)(void);
} Button_Object;


Button_Object * button_init(Button_Port port, Button_Pin pin, void * handler_on, void * handler_off);
bool button_get(Button_Object * button_object);
