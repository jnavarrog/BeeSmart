# -*- coding: utf-8 -*-

import paho.mqtt.client as paho
import subprocess 
import os 
import threading
import time
from coapthon.client.helperclient import HelperClient

global client
client = paho.Client()

def main():
	global broker
	global port
	global user
	global passw
	f=open("BEESMART.conf","r")
	line=f.readlines()
	for x in line:
		if "M_BROKER" in x:
			broker=x.split("=")[1]
			broker=broker.rstrip()
		elif "M_PORT" in x:
			port=x.split("=")[1]
			port=port.rstrip()
		elif "M_USER" in x:
			user=x.split("=")[1]
			user=user.rstrip()
		elif "M_PASS" in x:
			passw=x.split("=")[1]
			passw=passw.rstrip()
	f.close()
	mosquitto_tr = threading.Thread(target=mosquitto_init)
	mosquitto_tr.start()
	tunslip6_tr = threading.Thread(target=run_tunslip)
	tunslip6_tr.start()

	
def on_message(client, userdata, message):
	print("RECIBI ALGO")
	print("message received " ,str(message.payload.decode("utf-8")))
    	print("message topic=",message.topic)
    	print("message qos=",message.qos)
    	print("message retain flag=",message.retain)
	coap_handler("fd00::212:4b00:1375:df84", "get", "sensors/temperature/all")
	
#def on_connect(client, userdata, flags, rc):
	#print("Connected With Result Code: {}".format(rc))

#def on_disconnect(client, userdata, rc):
	#print("Client Got Disconnected")
	
#def on_subscribe(client, userdata, mid, granted_qos):
	#print("Client Suscribed")

#def on_unsubscribe(client, userdata, mid):
	#print("Client UnSuscribed")
	
#def on_publish(mqttc, obj, mid):
    	#print("-->mid: " + str(mid))
    	
def coap_handler(coap_server, method, resource):
	print("esta funcion va a mandar los mensajes coap")
	#usando coap-client de linux
	out = subprocess.Popen(['coap-client', '-v', '0','-B','3','-m',method,'coap://['+coap_server+']/'+resource], stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
	stdout,stderr = out.communicate()
	print(stdout)
	#usando la api de python
	#coapc = HelperClient(server=(coap_server, 5683))
	#cresponse = coapc.get(resource)
	#print(cresponse.payload)
	#coapc.stop()
	
def run_tunslip():
	#os.system("rm -rf IP_BR_DATA.dat")
	#time.sleep(0.2)
	#os.system("./tunslip6 -s /dev/ttyACM0 fd00::1/64")
	out = subprocess.Popen(['./tunslip6', '-s', '/dev/ttyACM0','fd00::1/64'], stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
	stdout,stderr = out.communicate()
	print(stdout)
	
def mosquitto_init():
	client.username_pw_set(user, passw)
	#client.on_subscribe = on_subscribe
	#client.on_unsubscribe = on_unsubscribe
	client.on_message = on_message
	#client.on_publish = on_publish
	#client.on_connect = on_connect
	#client.on_disconnect = on_disconnect
	client.connect(broker, port)
	client.subscribe("prueba", qos=1)	
	client.loop_forever()

	
if __name__== "__main__":
	main()
