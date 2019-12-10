###Programar el launchpad 2650/3150 usando el script cc2538-bsl (bootloader)###

El objetivo de este tutorial es habilitar el ROM Bootloader para poder programar la placa usando el comando `make programa.upload`

Por defecto el ROM Bootloader viene deshabilitado, la idea será habilitarlo mediante el JTAG.

En el archivo `arch/cpu/cc26xx-cc13xx/cc13xx-cc26xx-conf.h` verificar que las constantes `ROM_BOOTLOADER_ENABLE` y `CCXXWARE_CONF_ROM_BOOTLOADER_ENABLE` valgan *1*

```
/*---------------------------------------------------------------------------*/
/**
 * \name ROM Bootloader configuration
 *
 * Enable/Disable the ROM bootloader in your image, if the board supports it.
 * Look in board.h to choose the DIO and corresponding level that will cause
 * the chip to enter bootloader mode.
 * @{
 */

#define ROM_BOOTLOADER_ENABLE	1

/* Backward compatibility */
#ifdef ROM_BOOTLOADER_ENABLE
#define CCXXWARE_CONF_ROM_BOOTLOADER_ENABLE ROM_BOOTLOADER_ENABLE
#endif

#ifndef CCXXWARE_CONF_ROM_BOOTLOADER_ENABLE
#define CCXXWARE_CONF_ROM_BOOTLOADER_ENABLE              1
#endif
/** @} */
/*---------------------------------------------------------------------------*/
```
En el archivo `contiki-ng/arch/cpu/cc26x0-cc13x0/lib/cc26xxware/startup_files/ccfg.c` (cc13xxware para el cc3150) verificar que los siguientes defines valgan:

```
#define SET_CCFG_BL_CONFIG_BOOTLOADER_ENABLE            0xC5       // Enable ROM boot loader
#define SET_CCFG_BL_CONFIG_BL_LEVEL                  	0x0        // Active low to open boot loader backdoor
#define SET_CCFG_BL_CONFIG_BL_PIN_NUMBER                0x0D       // DIO number for boot loader backdoor
#define SET_CCFG_BL_CONFIG_BL_ENABLE                 	0xC5       // Enabled boot loader backdoor
```
La constante `SET_CCFG_BL_CONFIG_BL_PIN_NUMBER` define que botón se utilizará para habilitar el "modo programador", utilizar el valor `0x0D` para usar el botón1 (DIO13) o el valor `0x0E` para usar el botón2 (DIO14)

En el archivo mencioando quedaría:

```
//#####################################
// Bootloader settings
//#####################################

#ifndef SET_CCFG_BL_CONFIG_BOOTLOADER_ENABLE
//#define SET_CCFG_BL_CONFIG_BOOTLOADER_ENABLE            0x00       // Disable ROM boot loader
#define SET_CCFG_BL_CONFIG_BOOTLOADER_ENABLE         0xC5       // Enable ROM boot loader
#endif

#ifndef SET_CCFG_BL_CONFIG_BL_LEVEL
#define SET_CCFG_BL_CONFIG_BL_LEVEL                  0x0        // Active low to open boot loader backdoor
//#define SET_CCFG_BL_CONFIG_BL_LEVEL                     0x1        // Active high to open boot loader backdoor
#endif

#ifndef SET_CCFG_BL_CONFIG_BL_PIN_NUMBER
#define SET_CCFG_BL_CONFIG_BL_PIN_NUMBER                0x0D       // DIO number for boot loader backdoor
#endif

#ifndef SET_CCFG_BL_CONFIG_BL_ENABLE
#define SET_CCFG_BL_CONFIG_BL_ENABLE                 0xC5       // Enabled boot loader backdoor
//#define SET_CCFG_BL_CONFIG_BL_ENABLE                    0xFF       // Disabled boot loader backdoor
#endif
```

Luego de realizar las modificaciones a Contiki-NG se deberá flashear la placa con cualquier programa (por ejemplo hello-world) utilizando el software de [TI Uniflash](http://www.ti.com/tool/UNIFLASH). Para esto primero se debe compilar el programa:

```
make TARGET=cc26x0-cc13x0 BOARD=launchpad/cc2650
```
Una vez finalizado el proceso de compilación se deberá cargar en el uniflash el archivo:

```
projectfolder/build/cc26x0-cc13x0/launchpad/cc2650/program_name.bin (cc3150 para el cc1350).
```

A partir de ahora la placa tiene habilitado el ROM Bootloader y se podrá programar utilizando el cc2538-bsl.

###Utilizando el comando make para programar la placa###

Una vez habilitado el ROM Bootloader se podrá programar la placa mediante el comando:

```
make programa.upload TARGET=cc26x0-cc13x0 BOARD=launchpad/cc2650 PORT=/dev/mi_usb

o

make programa.upload TARGET=cc26x0-cc13x0 BOARD=launchpad/cc1350 PORT=/dev/mi_usb
```

Lo único que hay que tener en cuenta, es que siempre antes de ejecutar el mencionado comando se debe presionar el **boton1** (o **boton2**, depende cual se haya elegido) y sin soltarlo presionar el botón **reset**, de esta forma se le "informa" a la placa que va a ser programada.

Nota: Si continua dando el error `ERROR: Timeout waiting for ACK/NACK after 'Synch (0x55 0x55)'` agregar la linea `BSL_FLAGS += -e -w -v -b 115200` en el archivo `contiki-ng/arch/plataform/cc26x0-cc13x0/launchpad/cc2650/Makefile.cc2650` (cc1350/Makefile.cc1350 para el 1350).


