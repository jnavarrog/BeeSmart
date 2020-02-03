#!/bin/bash


#Script encargado de obtener la lista de nodos conectados al border router y guardarla en el archivo NODOS.dat

IP_BR=`cat IP_BR_DATA.dat | grep -m1 IPv6_BR_GA | cut -f2 -d"="`

wget -O nodos.temp http://[$IP_BR] &>/dev/null
cat nodos.temp | grep node_ipv6 > NODOS.dat
rm -rf nodos.temp
