CONTIKI_PROJECT = weight
all: $(CONTIKI_PROJECT)
CONTIKI = ../../..

MODULES += os/services/shell
MODULES += coresdk_cc13xx_cc26xx/source
MODULES += bee_smart/lib/hx711
MODULES += bee_smart/sensor/hx711_sensor

PLATFORMS_ONLY = cc26x0-cc13x0

CONTIKIDIRS+=bee_smart/lib/hx711
CONTIKIDIRS+=bee_smart/sensor/hx711_sensor

PROJECT_SOURCEFILES += HX711_SENSOR.c

include $(CONTIKI)/Makefile.identify-target

MODULES_REL += $(TARGET)

all: $(CONTIKI_PROJECT)

include $(CONTIKI)/Makefile.include
