#include <HX711.h>
#include <stdio.h>

#define HX711_SCK_1_TIME_OFF 60
#define HX711_SCK_0_TIME_DATA 1
#define HX711_SCK_1_TIME_DATA 1
#define HX711_DOUT_0_TIME_DATA 1

#define HX711_AVERAGE_SAMPLES 8
#define HX711_READING_CYCLES 25

void hx711_set(Hx711_Port port, Hx711_Pin pin) {
  gpio_hal_arch_write_pin(port, pin, 1);
}

void hx711_clr(Hx711_Port port, Hx711_Pin pin) {
  gpio_hal_arch_write_pin(port, pin, 0);
}

bool hx711_get(Hx711_Port port, Hx711_Pin pin) {
  return gpio_hal_arch_read_pin(port, pin);
}

void hx711_delay(uint16_t us) {
  clock_delay_usec(us);
}

void hx711_set_output(Hx711_Port port, Hx711_Pin pin) {
  gpio_hal_arch_pin_set_output(port, pin);
}

void hx711_set_input(Hx711_Port port, Hx711_Pin pin) {
  gpio_hal_arch_pin_set_input(port, pin);
}

void hx711_set_interrupts(Hx711_Pin pin) {
  gpio_hal_pin_cfg_t cfg = GPIO_HAL_PIN_CFG_EDGE_FALLING | GPIO_HAL_PIN_CFG_INT_ENABLE;
  gpio_hal_arch_no_port_pin_cfg_set(pin, cfg);
}

void hx711_unset_interrupts(Hx711_Pin pin) {
  gpio_hal_pin_cfg_t cfg = GPIO_HAL_PIN_CFG_INT_DISABLE;
  gpio_hal_arch_no_port_pin_cfg_set(pin, cfg);
}

void hx711_set_interrupt_handler(gpio_hal_event_handler_t * handler) {
  gpio_hal_register_handler(handler);
}

PROCESS(hx711_reader, "hx711_reader");

static void hx711_handler_function(Hx711_Pin_Mask pin_mask) {
  process_poll(&hx711_reader);
}

static Hx711_Handler hx711_handler = {
  .next = NULL,
  .handler = hx711_handler_function,
  .pin_mask = gpio_hal_pin_to_mask(0),
};

Hx711_Object hx711_init(Hx711_Port port, Hx711_Pin pin_dout, Hx711_Pin pin_sck) {
  Hx711_Object hx711_object;
  hx711_object.port = port;
  hx711_object.pin_dout = pin_dout;
  hx711_object.pin_sck = pin_sck;
  hx711_object.weight = 0;
  hx711_object.reading_type = NONE;
  hx711_object.handler = &hx711_handler;

  return hx711_object;
}

void hx711_reset(Hx711_Object * hx711_object) {
  hx711_set(hx711_object->port, hx711_object->pin_dout);
  hx711_delay(HX711_SCK_1_TIME_OFF);
  hx711_clr(hx711_object->port, hx711_object->pin_dout);
}

void hx711_start(Hx711_Object * hx711_object) {
  hx711_set_output(hx711_object->port, hx711_object->pin_sck);
  hx711_set_input(hx711_object->port, hx711_object->pin_dout);
  hx711_clr(hx711_object->port, hx711_object->pin_sck);
  hx711_reset(hx711_object);
  hx711_set_interrupts(hx711_object->pin_dout);
  hx711_object->handler->pin_mask = gpio_hal_pin_to_mask(hx711_object->pin_dout);
  hx711_set_interrupt_handler(hx711_object->handler);
  process_start(&hx711_reader, hx711_object);
}

void hx711_read_once(Hx711_Object * hx711_object) {
  hx711_object->reading_type = ONCE;
  hx711_start(hx711_object);
}

void hx711_read_continuos(Hx711_Object * hx711_object) {
  hx711_object->reading_type = CONTINUOS;
  hx711_start(hx711_object);
}

void hx711_stop(Hx711_Object * hx711_object) {
  hx711_object->reading_type = NONE;
  hx711_unset_interrupts(hx711_object->pin_dout);
}

Hx711_Weight hx711_read_weight_sample(Hx711_Object * hx711_object) {
  uint8_t i = 0;
  Hx711_Weight weight = 0;

  hx711_unset_interrupts(hx711_object->pin_dout);

  for(i = 0; i < HX711_READING_CYCLES; i++) {
    hx711_set(hx711_object->port, hx711_object->pin_sck);
    hx711_delay(HX711_SCK_1_TIME_DATA);
    hx711_clr(hx711_object->port, hx711_object->pin_sck);
    weight <<= 1;
    weight += hx711_get(hx711_object->port, hx711_object->pin_dout);
    hx711_delay(HX711_SCK_0_TIME_DATA);
  }

  hx711_set_interrupts(hx711_object->pin_dout);

  return weight;
}

PROCESS_THREAD(hx711_reader, ev, data) {
  static Hx711_Weight weights[HX711_AVERAGE_SAMPLES];
  static uint8_t i = 0;
  static Hx711_Object * hx711_object;

  if ((Hx711_Object *) data) {
    hx711_object = (Hx711_Object *) data;
  }

  PROCESS_BEGIN();

  while(hx711_object->reading_type != NONE) {
    PROCESS_YIELD_UNTIL(ev == PROCESS_EVENT_POLL);

    weights[i] = hx711_read_weight_sample(hx711_object);

    i++;

    if (i == HX711_AVERAGE_SAMPLES) {
      int j = 0;
      Hx711_Weight average = 0;

      for (j = 0; j < i; j++) {
        average += weights[j];
      }

      hx711_object->weight = average / HX711_AVERAGE_SAMPLES;

      i = 0;

      if (hx711_object->reading_type == ONCE) {
        hx711_object->reading_type = NONE;
      }
    }
  }
  hx711_stop(hx711_object);
  PROCESS_END();
}
