#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "contiki.h"
#include "coap-engine.h"
#include "orchestra.h"
#include "tsch.h"
#include <DS18B20_SENSOR.h>
#include <SERVO_SENSOR.h>
#include <HX711_SENSOR.h>
#include <BUTTON_SENSOR.h>
#include "batmon-sensor.h"
#include <ADDRESSES_FILE.h>

#include "sys/log.h"
#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_APP

#define SEARCH_LED_PIN IOID_6
#define SEARCH_LED_PORT GPIO_HAL_NULL_PORT
#define SEARCH_LED_TOGGLE_US 0xFFFF
#define IO_VSS_PORT GPIO_HAL_NULL_PORT
#define IO_VSS_PIN IOID_15

void delay_seconds(int sec){
	rtimer_clock_t end = (RTIMER_NOW() + RTIMER_SECOND * sec);
	while(RTIMER_CLOCK_LT(RTIMER_NOW(), end)) {
    watchdog_periodic();
	}
}

bool search_led_status = false;
bool io_status = false;

int ds18b20_port_int = GPIO_HAL_NULL_PORT;
int ds18b20_pin_int = IOID_23;
int ds18b20_amount_int = 9;

int servo_pin_int = IOID_12;

int hx711_port_int = GPIO_HAL_NULL_PORT;
int hx711_pin_dout_int = IOID_25;
int hx711_pin_sck_int = IOID_26;

int wd_no_msg_timer = 0;
int max_no_msg_timer = 900;

Button_Pin limit_switch_pin = IOID_7;
Button_Port limit_switch_port = GPIO_HAL_NULL_PORT;

extern coap_resource_t
  res_test,
  res_addresstemperature,
  res_temperature,
  res_reboot,
  res_weight,
  res_servo,
  res_battery;

void io_on() {
  io_status = true;
  gpio_hal_arch_pin_set_output(IO_VSS_PORT, IO_VSS_PIN);
  gpio_hal_arch_write_pin(IO_VSS_PORT, IO_VSS_PIN, 1);
}

void io_off() {
  io_status = false;
  gpio_hal_arch_pin_set_output(IO_VSS_PORT, IO_VSS_PIN);
  gpio_hal_arch_write_pin(IO_VSS_PORT, IO_VSS_PIN, 0);
}

void search_led_on() {
  search_led_status = true;
  gpio_hal_arch_write_pin(SEARCH_LED_PORT, SEARCH_LED_PIN, 1);
}

void search_led_off() {
  search_led_status = false;
  gpio_hal_arch_write_pin(SEARCH_LED_PORT, SEARCH_LED_PIN, 0);
}

void search_led_toggle() {
  if (search_led_status) {
    search_led_off();
    search_led_status = false;
  } else {
    search_led_on();
    search_led_status = true;
  }
}

void test_temp() {
    for(int i = 0; i < ds18b20_amount_int; i++) {
    ds18b20.configure(DS18B20_CONFIGURATION_INDEX, i);
    ds18b20.configure(DS18B20_CONFIGURATION_READ, 0);
  }

  for(int i = 0; i < ds18b20_amount_int; i++) {
    ds18b20.configure(DS18B20_CONFIGURATION_INDEX, i);
    ds18b20.configure(DS18B20_CONFIGURATION_READ, 0);

    int address_low = ds18b20.value(DS18B20_VALUE_ADDRESS_LOW);
    int address_high = ds18b20.value(DS18B20_VALUE_ADDRESS_HIGH);
    int integer = ds18b20.value(DS18B20_VALUE_TEMPERATURE_INTEGER);
    int decimal = ds18b20.value(DS18B20_VALUE_TEMPERATURE_DECIMAL) / 2;

    printf("Address %x%x: %d,%d\n", address_high, address_low, integer, decimal);
  }
}

void test_weight() {
  hx711.configure(HX711_CONFIGURATION_START, 0);
  delay_seconds(3);
  printf("Weight: %04x%04x\n", hx711.value(HX711_VALUE_WEIGHT_HIGH), hx711.value(HX711_VALUE_WEIGHT_LOW));
  hx711.configure(HX711_CONFIGURATION_PAUSE, 0);
}

void test_door() {
  printf("Door: %d\n", button.value(BUTTON_VALUE));
}

void test_battery() {
  printf("Battery: %d\n", (batmon_sensor.value(BATMON_SENSOR_TYPE_VOLT) * 125) >> 5);
}

void test_servo() {
  for (int j = 0; j < 2; j++) {
    servo.configure(SERVO_CONFIGURATION_POSITION, SERVO_SENSOR_OPEN_POSITION);
    printf("Servo: 0\n");
    servo.value(SERVO_VALUE_MOVE);
    delay_seconds(3);
    test_door();

    servo.configure(SERVO_CONFIGURATION_POSITION, SERVO_SENSOR_CLOSE_POSITION);
    printf("Servo: 1\n");
    servo.value(SERVO_VALUE_MOVE);
    delay_seconds(3);
    test_door();
  }
}

void ds18b20_address_finder() {
  search_led_on();
  Ds18b20_Object * ds18b20_objects;
  start_addresses_file(ds18b20_amount_int);
  ds18b20_objects = (Ds18b20_Object *) malloc(sizeof(Ds18b20_Object[ds18b20_amount_int]));
  ds18b20_search_all(ds18b20_objects, ds18b20_port_int, ds18b20_pin_int, ds18b20_amount_int, false);

  search_led_off();

  for (int i  = 0; i < ds18b20_amount_int; i++) {
    printf("%d - %llx\n", i, ds18b20_objects[i].address);
  }

  for (int i  = 0; i < ds18b20_amount_int - 1; i++) {
    for(int j = i + 1; j < ds18b20_amount_int; j++) {
      if(ds18b20_objects[i].address == ds18b20_objects[j].address) {
        free(ds18b20_objects);
        return;
      }
    }
  }

  for (int i  = 0; i < ds18b20_amount_int; i++) {
    write_address(ds18b20_objects[i].address);
  }

  free(ds18b20_objects);

  ds18b20.configure(DS18B20_CONFIGURATION_RESTART_FROM_FILE, 0);

  clock_delay_usec(SEARCH_LED_TOGGLE_US);
  search_led_on();
  clock_delay_usec(SEARCH_LED_TOGGLE_US);
  search_led_off();
  clock_delay_usec(SEARCH_LED_TOGGLE_US);
  search_led_on();
  clock_delay_usec(SEARCH_LED_TOGGLE_US);
  search_led_off();
}

PROCESS(er_example_server, "Servidor CoAP");
AUTOSTART_PROCESSES(&er_example_server);

PROCESS_THREAD(er_example_server, ev, data)
{
  static struct etimer period;
  static struct etimer wd_period;
  PROCESS_BEGIN();
  SENSORS_ACTIVATE(batmon_sensor);

  SENSORS_ACTIVATE(button);
  button.configure(BUTTON_CONFIGURATION_PORT, limit_switch_port);
  button.configure(BUTTON_CONFIGURATION_PIN, limit_switch_pin);
  button.configure(BUTTON_CONFIGURATION_START, 0);

  SENSORS_ACTIVATE(ds18b20);
  ds18b20.configure(DS18B20_CONFIGURATION_AMOUNT, ds18b20_amount_int);
  ds18b20.configure(DS18B20_CONFIGURATION_PORT, ds18b20_port_int);
  ds18b20.configure(DS18B20_CONFIGURATION_PIN, ds18b20_pin_int);
  ds18b20.configure(DS18B20_CONFIGURATION_START_FROM_FILE, 0);
  io_on();
  test_temp();
  io_off();

  SENSORS_ACTIVATE(servo);
  servo.configure(SERVO_CONFIGURATION_PIN, servo_pin_int);
  servo.configure(SERVO_CONFIGURATION_START, 0);
  servo.configure(SERVO_CONFIGURATION_STOP, 0);
  servo.value(SERVO_VALUE_STOP);

  SENSORS_ACTIVATE(hx711);
  hx711.configure(HX711_CONFIGURATION_PORT, hx711_port_int);
  hx711.configure(HX711_CONFIGURATION_PIN_DOUT, hx711_pin_dout_int);
  hx711.configure(HX711_CONFIGURATION_PIN_SCK, hx711_pin_sck_int);
  hx711.configure(HX711_CONFIGURATION_INIT, 0);
  hx711.configure(HX711_CONFIGURATION_PAUSE, 0);
  PROCESS_PAUSE();

  LOG_INFO("BEESMART - PROYECTO FIN DE CARRERA 2020 - NODO\n");

  coap_activate_resource(&res_test, "test");
  coap_activate_resource(&res_temperature, "sensors/temperature/all");
  coap_activate_resource(&res_addresstemperature, "sensors/temperature/temperature");
  coap_activate_resource(&res_reboot, "commands/reboot");
  coap_activate_resource(&res_weight, "sensors/weight");
  coap_activate_resource(&res_servo, "actuators/servo");
  coap_activate_resource(&res_battery, "sensors/battery");

  while(1) {
    etimer_set(&wd_period, CLOCK_SECOND);
    PROCESS_WAIT_EVENT();
    if(etimer_expired(&wd_period)) {
      wd_no_msg_timer++;

      if (wd_no_msg_timer > max_no_msg_timer) {
        watchdog_reboot();
      } else if (!gpio_hal_arch_read_pin(GPIO_HAL_NULL_PORT, IOID_14)) {
        etimer_set(&period, CLOCK_SECOND);
        PROCESS_WAIT_UNTIL(etimer_expired(&period));
        if (!gpio_hal_arch_read_pin(GPIO_HAL_NULL_PORT, IOID_14)) {
          ds18b20_address_finder();
        }
      } else if (!gpio_hal_arch_read_pin(GPIO_HAL_NULL_PORT, IOID_13)) {
        etimer_set(&period, CLOCK_SECOND);
        PROCESS_WAIT_UNTIL(etimer_expired(&period));
        if (!gpio_hal_arch_read_pin(GPIO_HAL_NULL_PORT, IOID_13)) {
          search_led_on();
          test_temp();
          test_weight();
          test_battery();
          test_servo();
          search_led_off();
        }
      } else {
        if (wd_no_msg_timer % 30 == 0) {
          io_on();
          servo.value(SERVO_VALUE_STOP);
          hx711.configure(HX711_CONFIGURATION_START, 0);
        } else if (wd_no_msg_timer % 5 == 0) {
          io_off();
          servo.value(SERVO_VALUE_STOP);
        }
      }

      if (hx711.status(HX711_STATUS_PAUSED) && io_status) {
        io_off();
      }
    }
  }

  PROCESS_END();
}
