CONTIKI_PROJECT = node
all: $(CONTIKI_PROJECT)

# Do not try to build on Sky because of code size limitation
PLATFORMS_EXCLUDE = sky z1

# Include CoAP resources
MODULES_REL += ./resources

CONTIKI = ../..

# Include the CoAP implementation
include $(CONTIKI)/Makefile.dir-variables
MODULES += $(CONTIKI_NG_APP_LAYER_DIR)/coap

# Habilitar shell
MODULES += os/services/shell

# Usar tsch orchestra
MAKE_MAC = MAKE_MAC_TSCH
MODULES += os/services/orchestra

#Sensores
MODULES += coresdk_cc13xx_cc26xx/source
MODULES += $(CONTIKI_NG_STORAGE_DIR)/cfs
MODULES += bee_smart/lib/addresses_file
MODULES += bee_smart/lib/ow
MODULES += bee_smart/lib/ds18b20
MODULES += bee_smart/sensor/ds18b20_sensor
CONTIKIDIRS+=bee_smart/lib/ow
CONTIKIDIRS+=bee_smart/lib/ds18b20
CONTIKIDIRS+=bee_smart/sensor/ds18b20_sensor
PROJECT_SOURCEFILES += DS18B20_SENSOR.c

MODULES += bee_smart/lib/servo
MODULES += bee_smart/sensor/servo_sensor
PROJECT_SOURCEFILES += SERVO_SENSOR.c
CONTIKIDIRS+=bee_smart/lib/servo
CONTIKIDIRS+=bee_smart/sensor/servo_sensor

MODULES += bee_smart/lib/hx711
MODULES += bee_smart/sensor/hx711_sensor
PROJECT_SOURCEFILES += HX711_SENSOR.c
CONTIKIDIRS+=bee_smart/lib/hx711
CONTIKIDIRS+=bee_smart/sensor/hx711_sensor

MODULES += bee_smart/lib/button
MODULES += bee_smart/sensor/button_sensor
PROJECT_SOURCEFILES += BUTTON.c
CONTIKIDIRS+=bee_smart/lib/button
CONTIKIDIRS+=bee_smart/sensor/button_sensor

include $(CONTIKI)/Makefile.include
