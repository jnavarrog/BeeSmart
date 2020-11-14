#BEESMART

El programa princiapl beesmart.py se encarga de:
-Crear el tunel entre la RPI y el BR
-Detectar nodos automaticamente y agregarlo a la lista de nodos
-Recolectar automaticamente los datos de los sensores y publicarlos
-Recibir pedidos puntuales o comandos, procesarlos y responder
-Registrar en un log todos los sucesos de importancia

##REQUISITOS

-Based Debian Linux (v9 o superior)
-Python 2.7.17 (`apt-get install python2.7`)
-mosquitto-client (`apt-get install mosquitto-clients`)
-PAHO MQTT Python Client (`pip install paho-mqtt`)
-ifconfig (`apt-get install net-tools`)
-CoAP Client (`apt-get install libcoap-1-0`)
-sakis3g (` git clone https://github.com/Trixarian/sakis3g-source.git`)
-El programa debe ser ejecutado como root
-El archivo de BEESMART.conf y tunslip6 deben estar en la misma carpeta

###Instalación y configuración sakis3g
-Entrar a la carpeta sakis3g-source
-Si no está instalado, instalar libusb-1.0-0-dev y ppp
-En dependencies/usb-modeswitch/usb_modeswitch.h cambiar #include <libusb.h> por #include <libusb-1.0/libusb.h>
-./compile
-sudo cp build/sakis3gz /usr/bin/sakis3g
-En /etc/sakis3g.conf (hay un ejemplo en sakis3g-source/files)

```
USBDRIVER="option"
OTHER="USBMODEM"
USBMODEM="19d2:2003" # Se saca del comando lsusb
APN="antel.lte"
APN_USER="0"
APN_PASS="0"
```


##MQTT Topics

###IDAPIARIO/data
Todos los datos recolectados son publicados en este topic.

###IDAPIRARIO/rpicommands
Es donde se publican las salidas de los comandos ejecutados en la RPI (ver comunicación)

###IDAPIARIO/send/#
El programa principal se suscribe a este topic para recibir instrucciones desde el exterior. Ver la sección "Comunicación" por mas detalles.

##CoAP Resources

La siguiente lista de recursos se encuentra disponible en cada nodo

###test
-GET: Devuelte la dirección MAC del nodo
-POST: No hace nada
-PUT: No hace nada
-DELETE: No hace nada

###sensors/temperature/all
-GET: Devuelte la dirección de cada sensor de temperatura y su valor de temperatura en ese momento
-POST: No hace nada
-PUT: No hace nada
-DELETE: No hace nada

###sensors/temperature/temperature
-GET: Devuelte el valor de temperatura en ese momento de cada sensor de temperatura
-POST: No hace nada
-PUT: No hace nada
-DELETE: No hace nada

###commands/reboot
-GET: Reinicia el nodo
-POST: No hace nada
-PUT: No hace nada
-DELETE: No hace nada

###sensors/weight
-GET: Devuelte el peso de la colmena
-POST: No hace nada
-PUT: No hace nada
-DELETE: No hace nada

###actuators/servo
-GET: Devuelte la posición actual de la puerta (1 cerrada, 0 abierta)
-POST: Si se le envía un 1 se le indica que cierre la puerta, si se le envía un 0 se le indica que abra la puerta
-PUT: Si se le envía un 1 se le indica que cierre la puerta, si se le envía un 0 se le indica que abra la puerta
-DELETE: No hace nada

##COMUNICACIÓN

Cada vez que el apiario deba enviar datos (tanto de forma automática como respuesta a consultas puntuales) lo hará en el topic IDCOLMENA/data. A todo lo que es dato se le adjunta un timestap, el id del nodo que lo produjo, que tipo de dato es y el dato en si mismo para facilitar el procesamiento. La respuesta a la ejecución de "comandos" no incluye esta cabecera y se publica en IDAPIARIO/rpicommands

```
Formato datos: 
[TIMESTAMP][IPV6NODE][COAPRESOURCE]{DATOS}
```

Para obtener datos de forma manual o enviarle comandos a la RPI o los nodos se utiliza el topic IDCOLMENA/send/# en combinación con el mensaje:

-GET
Para hacer un get (pedir un dato) a un nodo en particular se debe publicar en el topic `IDCOLMENA/send/get` el mensaje `NODEIPV6%RESOURCE%`

-POST
Para hacer un post y enviarle `DATA` a un nodo se debe publicar en el topic `IDCOLMENA/send/post` el mensaje `NODEIPV6%RESOURCE%DATA`

-PUT
Para hacer un post y enviarle `DATA` a un nodo se debe publicar en el topic `IDCOLMENA/send/put` el mensaje `NODEIPV6%RESOURCE%DATA`

Depende el `RESOURCE` que se utilice es el dato que se obtendrá (ver CoAP Resources).

-Comandos
Para enviarle comando a la RPI se debe publicar en el topic `IDCOLMENA/send/rpicommands` el mensaje `COMMAND`

-Comandos disponibles
nodelist: devuelve la lista de nodos
configfile: devuelve los valores del archivo BEESMART.conf
iprpi: devuelve la salida del comando ifconfig ejecutado en la RPI
description: devuelve la descripción del apiario
rebootrpi: reinicia la RPI

-Subir archivo de configuración
Publicar en `IDCOLMENA/send/beesmart.conf` el archivo de configuracion

##BEESMART.conF
En este archivo se almacena la configuración principal del programa beesmart.py

*D_ID_BEEHIVE*: ID que identifica al apiario (a cada RPI)
*D_BEEHIVE_DESC*: Descripción del apiario (ubicacion, nombre, etc)
*M_BROKER*: IP o URL del broker mqtt
*M_PORT*: Puerto del broker mqtt
*M_USE*: Usuario del broker mqtt
*M_PASS*: Contraseña del broker mqtt
*T_UPDATE_NODELIST*: Cada cuanto tiempo (en segundos) se actualiza la lista de nodos
*T_REQUEST_ALL_DATA*: Cada cuanto tiempo (en segundos) se recolecta y envía toda la información 
*T_COAP_MAX_WAIT_TIME*: Cuanto tiempo espera como máximo CoAP la respuesta de un nodo (en segundos)
*L_COAP_DATA_RESOURCES*: Lista de recursos CoAP que se consultarán automaticamente cada T_REQUEST_ALL_DATA segundos
*D_USB_PORT_TUN*: Ubicación en /dev del puerto USB al cual se conecta el BR
*D_IPV6_TUNNEL*: IPv6 que tomará la RPI (todos los nodos usarán el mismo prefijo)
*D_ENABLE_LOG: Habilitar log yes o no
*D_USE_3GMODEM: Conectarse usando el modem 3g yes o no
*D_ENABLE_CHK3GCON: Chequear y en caso de encontrar fallas tratar de reiniciar la conexión 3g yes o no
*D_ENABLE_CHKTUNNEL: Chequear y en caso de encontrar fallas tratar de reiniciar el tunel yes o no
*T_CHK3GCON: Cada cuanto tiempo chequear la conexión 3g
*T_CHKTUNNEL: Cada cuandto tiempo chequear el tunel

###Ejemplo

```
//BEESMART
D_ID_APIARIO=colmena1
D_APIARIO_DESC="colmena ubicada en algun lugar"
M_BROKER=beesmart.cure.edu.uy
M_PORT=2000
M_USER=pfc
M_PASS=pfc
T_UPDATE_NODELIST=300
T_REQUEST_ALL_DATA=900
T_COAP_MAX_WAIT_TIME=5
L_COAP_DATA_RESOURCES=test,sensors/temperature/all,sensors/weight,actuators/servo,sensors/temperature/temperature
D_USB_PORT_TUN=/dev/ttyACM0
D_IPV6_TUNNEL=fd00::1/64
D_ENABLE_LOG=yes
D_USE_3GMODEM=no
D_ENABLE_CHK3GCON=no
D_ENABLE_CHKTUNNEL=yes
T_CHK3GCON=5
T_CHKTUNNEL=5
```
