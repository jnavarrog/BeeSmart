CONTIKI_PROJECT = balanza_lectura_*
all: $(CONTIKI_PROJECT)
CONTIKI = ../..

MODULES += os/services/shell
MODULES += coresdk_cc13xx_cc26xx/source
MODULES += lib/HX711

PLATFORMS_ONLY = cc26x0-cc13x0

CONTIKIDIRS+=lib/HX711

PROJECT_SOURCEFILES += HX711.c

include $(CONTIKI)/Makefile.identify-target

MODULES_REL += $(TARGET)

all: $(CONTIKI_PROJECT)

include $(CONTIKI)/Makefile.include
