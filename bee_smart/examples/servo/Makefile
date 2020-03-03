CONTIKI_PROJECT = servo_example
all: $(CONTIKI_PROJECT)
CONTIKI = ../../..

MODULES += os/services/shell
MODULES += coresdk_cc13xx_cc26xx/source
MODULES += bee_smart/lib/servo
MODULES += bee_smart/sensor/servo_sensor

PLATFORMS_ONLY = cc26x0-cc13x0

PROJECT_SOURCEFILES += SERVO_SENSOR.c
CONTIKIDIRS+=bee_smart/lib/servo
CONTIKIDIRS+=bee_smart/sensor/servo_sensor

include $(CONTIKI)/Makefile.identify-target

MODULES_REL += $(TARGET)

all: $(CONTIKI_PROJECT)

include $(CONTIKI)/Makefile.include
