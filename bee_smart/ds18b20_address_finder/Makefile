CONTIKI_PROJECT = ds18b20_address_finder
all: $(CONTIKI_PROJECT)

CONTIKI = ../..

#Sensores
MODULES += coresdk_cc13xx_cc26xx/source
MODULES += bee_smart/lib/ow
MODULES += bee_smart/lib/ds18b20
CONTIKIDIRS+=bee_smart/lib/ow
CONTIKIDIRS+=bee_smart/lib/ds18b20

include $(CONTIKI)/Makefile.include
