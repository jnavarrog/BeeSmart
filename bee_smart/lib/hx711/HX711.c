#include <HX711.h>

#define HX711_SCK_1_TIME_OFF 60
#define HX711_SCK_0_TIME_DATA 1
#define HX711_SCK_1_TIME_DATA 1
#define HX711_DOUT_0_TIME_DATA 1

#define HX711_AVERAGE_SAMPLES 8
#define HX711_READING_CYCLES 25

uint8_t iteration = 0;
Hx711_Weight hx711_weight_samples[HX711_AVERAGE_SAMPLES];
Hx711_Object hx711_object;

void hx711_set(Hx711_Port port, Hx711_Pin pin);
void hx711_clr(Hx711_Port port, Hx711_Pin pin);
bool hx711_get(Hx711_Port port, Hx711_Pin pin);
void hx711_delay(uint16_t us);
void hx711_set_output(Hx711_Port port, Hx711_Pin pin);
void hx711_set_input(Hx711_Port port, Hx711_Pin pin);
void hx711_set_interrupts(Hx711_Pin pin);
void hx711_unset_interrupts(Hx711_Pin pin);
void hx711_set_interrupt_handler();
static void hx711_handler_function(Hx711_Pin_Mask pin_mask);
Hx711_Object hx711_init(Hx711_Port port, Hx711_Pin pin_dout, Hx711_Pin pin_sck);
void hx711_reset();
void hx711_start();
void hx711_stop();
Hx711_Weight hx711_read_weight_sample();
Hx711_Weight hx711_get_reading();

static Hx711_Handler hx711_handler = {
  .next = NULL,
  .handler = hx711_handler_function,
  .pin_mask = gpio_hal_pin_to_mask(0),
};

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

void hx711_set_interrupt_handler() {
  gpio_hal_register_handler(hx711_object.handler);
}

static void hx711_handler_function(Hx711_Pin_Mask pin_mask) {
  hx711_weight_samples[iteration] = hx711_read_weight_sample(hx711_object);
  iteration++;
  iteration %= HX711_AVERAGE_SAMPLES;

  if (iteration == 0) {
    uint8_t j = 0;
    Hx711_Weight average = 0;

    for (j = 0; j < HX711_AVERAGE_SAMPLES; j++) {
      average += hx711_weight_samples[j] / HX711_AVERAGE_SAMPLES;
    }

    hx711_object.weight = average;
  }
}

Hx711_Object hx711_init(Hx711_Port port, Hx711_Pin pin_dout, Hx711_Pin pin_sck) {
  hx711_object.port = port;
  hx711_object.pin_dout = pin_dout;
  hx711_object.pin_sck = pin_sck;
  hx711_object.weight = 0;
  hx711_object.handler = &hx711_handler;
  hx711_start(hx711_object);

  return hx711_object;
}

void hx711_reset() {
  hx711_set(hx711_object.port, hx711_object.pin_dout);
  hx711_delay(HX711_SCK_1_TIME_OFF);
  hx711_clr(hx711_object.port, hx711_object.pin_dout);
}

void hx711_start() {
  hx711_set_output(hx711_object.port, hx711_object.pin_sck);
  hx711_set_input(hx711_object.port, hx711_object.pin_dout);
  hx711_clr(hx711_object.port, hx711_object.pin_sck);
  hx711_reset(hx711_object);
  hx711_set_interrupts(hx711_object.pin_dout);
  hx711_object.handler->pin_mask = gpio_hal_pin_to_mask(hx711_object.pin_dout);
  hx711_set_interrupt_handler(hx711_object);
}

void hx711_pause() {
  hx711_set_output(hx711_object.port, hx711_object.pin_sck);
  hx711_set_output(hx711_object.port, hx711_object.pin_dout);
  hx711_unset_interrupts(hx711_object.pin_dout);
  hx711_object.handler->pin_mask = gpio_hal_pin_to_mask(hx711_object.pin_dout);
}

void hx711_stop() {
  hx711_unset_interrupts(hx711_object.pin_dout);
}

Hx711_Weight hx711_read_weight_sample() {
  uint8_t i = 0;
  Hx711_Weight weight = 0;

  hx711_unset_interrupts(hx711_object.pin_dout);

  for(i = 0; i < HX711_READING_CYCLES; i++) {
    hx711_set(hx711_object.port, hx711_object.pin_sck);
    hx711_delay(HX711_SCK_1_TIME_DATA);
    hx711_clr(hx711_object.port, hx711_object.pin_sck);
    weight <<= 1;
    weight += hx711_get(hx711_object.port, hx711_object.pin_dout);
    hx711_delay(HX711_SCK_0_TIME_DATA);
  }

  hx711_set_interrupts(hx711_object.pin_dout);

  return weight;
}

Hx711_Weight hx711_get_reading() {
  return hx711_object.weight;
}
