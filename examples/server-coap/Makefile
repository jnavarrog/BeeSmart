CONTIKI_PROJECT = server-coap
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

include $(CONTIKI)/Makefile.include
