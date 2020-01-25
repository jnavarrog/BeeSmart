CONTIKI_PROJECT = temperature
all: $(CONTIKI_PROJECT)
CONTIKI = ../../..

MODULES += os/services/shell
MODULES += coresdk_cc13xx_cc26xx/source
MODULES += bee_smart/lib/ow
MODULES += bee_smart/lib/ds18b20
MODULES += bee_smart/sensor/ds18b20_sensor

PLATFORMS_ONLY = cc26x0-cc13x0

CONTIKIDIRS+=bee_smart/lib/ow
CONTIKIDIRS+=bee_smart/lib/ds18b20
CONTIKIDIRS+=bee_smart/sensor/ds18b20_sensor

PROJECT_SOURCEFILES += DS18B20_SENSOR.c

include $(CONTIKI)/Makefile.identify-target

MODULES_REL += $(TARGET)

all: $(CONTIKI_PROJECT)

include $(CONTIKI)/Makefile.include
