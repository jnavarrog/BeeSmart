# -*- coding: utf-8 -*-

import paho.mqtt.client as paho

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
	mosquitto_run()

	
def on_message(client, userdata, msg):
	print(msg.topic+" "+str(msg.qos)+" "+str(msg.payload))
	# Filename to append
	filename = "newfile.txt"
	myfile = open(filename, 'a')
	myfile.write(msg.topic+" "+str(msg.qos)+" "+str(msg.payload)+"\n")
	myfile.close()
	
#def on_connect(client, userdata, flags, rc):
	#print("Connected With Result Code: {}".format(rc))

#def on_disconnect(client, userdata, rc):
	#print("Client Got Disconnected")
	
#def on_subscribe(client, userdata, mid, granted_qos):
	#print("Client Suscribed")

#def on_unsubscribe(client, userdata, mid):
	#print("Client UnSuscribed")
	
def mosquitto_run():
	client = paho.Client()
	client.username_pw_set(user, passw)
	#client.on_subscribe = on_subscribe
	#client.on_unsubscribe = on_unsubscribe
	client.on_message = on_message
	#client.on_connect = on_connect
	#client.on_disconnect = on_disconnect
	client.connect(broker, port)
	client.subscribe("prueba", qos=1)	
	client.loop_forever()
	
if __name__== "__main__":
	main()
