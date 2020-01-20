CONTIKI_PROJECT = servo_example
all: $(CONTIKI_PROJECT)
CONTIKI = ../..

MODULES += os/services/shell
MODULES += coresdk_cc13xx_cc26xx/source
MODULES += lib/servo

PLATFORMS_ONLY = cc26x0-cc13x0

PROJECT_SOURCEFILES += SERVO.c
CONTIKIDIRS+=lib/servo

include $(CONTIKI)/Makefile.identify-target

MODULES_REL += $(TARGET)

all: $(CONTIKI_PROJECT)

include $(CONTIKI)/Makefile.include
