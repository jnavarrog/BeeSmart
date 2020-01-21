CONTIKI_PROJECT = todos
all: $(CONTIKI_PROJECT)
CONTIKI = ../..

MODULES += os/services/shell
MODULES += coresdk_cc13xx_cc26xx/source
MODULES += lib/ds18b20
MODULES += lib/ow
MODULES += lib/hx711

PLATFORMS_ONLY = cc26x0-cc13x0

CONTIKIDIRS+=lib/ow
CONTIKIDIRS+=lib/ds18b20
CONTIKIDIRS+=lib/hx711

PROJECT_SOURCEFILES += DS18B20.c
PROJECT_SOURCEFILES += HX711.c

include $(CONTIKI)/Makefile.identify-target

MODULES_REL += $(TARGET)

all: $(CONTIKI_PROJECT)

include $(CONTIKI)/Makefile.include
