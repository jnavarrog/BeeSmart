#!/bin/bash


#Script encargado de obtener de obtener todos los valores de todos los nodos

RECURSO="/test"

while IFS= read -r linea
do
	IP_NODO=`echo $linea | cut -f1 -d"-" | cut -f2 -d"="`

	SALIDA=`coap-client -v 1 -B 5 -m get coap://$IP_NODO$RECURSO`
	TSTAMP=`date "+%d-%m-%Y %T"`

	echo "["$TSTAMP"]"$IP_NODO=$SALIDA

done < NODOS.dat