CONTIKI_PROJECT = ping
all: $(CONTIKI_PROJECT)
CONTIKI = ../..

PLATFORMS_ONLY = cc26x0-cc13x0 cc2538dk openmote-cc2538 zoul native simplelink

# Include optional target-specific module
include $(CONTIKI)/Makefile.identify-target
MODULES_REL += $(TARGET)

# Habilitar shell
MODULES += os/services/shell

# Usar tsch orchestra
MAKE_MAC = MAKE_MAC_TSCH
MODULES += os/services/orchestra

include $(CONTIKI)/Makefile.include
