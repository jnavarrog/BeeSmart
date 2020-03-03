CONTIKI_PROJECT = all_example
all: $(CONTIKI_PROJECT)
CONTIKI = ../../..

MODULES += os/services/shell
MODULES += coresdk_cc13xx_cc26xx/source
MODULES += bee_smart/lib/servo
MODULES += bee_smart/lib/hx711
MODULES += bee_smart/lib/ow
MODULES += bee_smart/lib/ds18b20
MODULES += bee_smart/sensor/servo_sensor
MODULES += bee_smart/sensor/hx711_sensor
MODULES += bee_smart/sensor/ds18b20_sensor

PLATFORMS_ONLY = cc26x0-cc13x0

PROJECT_SOURCEFILES += SERVO_SENSOR.c
PROJECT_SOURCEFILES += HX711_SENSOR.c
PROJECT_SOURCEFILES += DS18B20_SENSOR.c

CONTIKIDIRS += bee_smart/lib/servo
CONTIKIDIRS += bee_smart/lib/hx711
CONTIKIDIRS += bee_smart/lib/ow
CONTIKIDIRS += bee_smart/lib/ds18b20
CONTIKIDIRS += bee_smart/sensor/servo_sensor
CONTIKIDIRS += bee_smart/sensor/hx711_sensor
CONTIKIDIRS += bee_smart/sensor/ds18b20_sensor

include $(CONTIKI)/Makefile.identify-target

MODULES_REL += $(TARGET)

all: $(CONTIKI_PROJECT)

include $(CONTIKI)/Makefile.include
