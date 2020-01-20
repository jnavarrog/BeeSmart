CONTIKI_PROJECT = temperature
all: $(CONTIKI_PROJECT)
CONTIKI = ../..

MODULES += os/services/shell
MODULES += coresdk_cc13xx_cc26xx/source
MODULES += lib/ds18b20
MODULES += lib/ow

PLATFORMS_ONLY = cc26x0-cc13x0

# CONTIKIDIRS+=lib/ow
# CONTIKIDIRS+=lib/ds18b20

PROJECT_SOURCEFILES += DS18B20.c
# PROJECT_SOURCEFILES += ow/OW.c

include $(CONTIKI)/Makefile.identify-target

MODULES_REL += $(TARGET)

all: $(CONTIKI_PROJECT)

include $(CONTIKI)/Makefile.include
