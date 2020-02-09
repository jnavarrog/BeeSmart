# -*- coding: utf-8 -*-

import paho.mqtt.client as paho
import subprocess 
import os 
import threading
import time
from coapthon.client.helperclient import HelperClient
import urllib2
import datetime
import re
global client
client = paho.Client()

def main():
	global broker
	global port
	global user
	global passw
	global mutex
	global accessfile
	global time_upd_nlist
	global coap_max_wtime
	global time_between_reqall
	global data_resources
	global tun_usbport
	global ipv6_tunnel
	f=open("BEESMART.conf","r")
	line=f.readlines()
	#load BEESMART.conf configuration parameters
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
		elif "T_UPDATE_NODELIST" in x:
			time_upd_nlist=x.split("=")[1]
			time_upd_nlist=time_upd_nlist.rstrip()
		elif "T_COAP_MAX_WAIT_TIME" in x:
			coap_max_wtime=x.split("=")[1]
			coap_max_wtime=coap_max_wtime.rstrip()
		elif "T_REQUEST_ALL_DATA" in x:
			time_between_reqall=x.split("=")[1]
			time_between_reqall=time_between_reqall.rstrip()
		elif "L_COAP_DATA_RESOURCES" in x:
			data_resources=x.split("=")[1]
			data_resources=data_resources.rstrip()
		elif "D_USB_PORT_TUN" in x:
			tun_usbport=x.split("=")[1]
			tun_usbport=tun_usbport.rstrip()
		elif "D_IPV6_TUNNEL" in x:
			ipv6_tunnel=x.split("=")[1]
			ipv6_tunnel=ipv6_tunnel.rstrip()	
	f.close()
	
	f=open("NODELIST.dat","w")
	f.close()
		
	mutex = threading.Lock()
	accessfile = threading.Lock()
	mosquitto_tr = threading.Thread(target=mosquitto_init, name='mosquitto_tr')
	mosquitto_tr.start()
	tunslip6_tr = threading.Thread(target=run_tunslip)
	tunslip6_tr.start()
	time.sleep(5)
	get_nodelist_tr = threading.Thread(target=get_nodelist)
	get_nodelist_tr.start()
	get_alldata_tr = threading.Thread(target=get_alldata)
	get_alldata_tr.start()
	
def get_nodelist():
	nodeip=""
	f=open("IP_BR_DATA.dat","r")
	line=f.readlines()
	for x in line:
		if "IPv6_BR_GA" in x:
			ip_br=x.split("=")[1]
			ip_br=ip_br.rstrip()
	f.close()
	while 1:
		print("while node list")
		webURL=urllib2.urlopen('http://['+ip_br+']/')
		for y in webURL.readlines():
			if "node_ipv6" in y:
				nodeip=nodeip+y.rstrip()+"\n"

		accessfile.acquire()
		f=open("NODELIST.dat","w")
		f.write(nodeip)
   		f.close
   		accessfile.release()
   		nodeip=""
   		#print(nodeip)
   		time.sleep(float(time_upd_nlist))
   				
def get_alldata():
	while 1:
		print("while get all")
		accessfile.acquire()
		f=open("NODELIST.dat","r")
		line=f.readlines()
		for x in line:
			if "node_ipv6" in x:
				ipnode=x.split("-")[0].split("]")[0].split("[")[1].rstrip()
				print(ipnode)
				for res in data_resources.split(","):
					print(res)
					coap_handler(ipnode, "get", res, "")
					
   		f.close
   		accessfile.release()
		time.sleep(float(time_between_reqall))

def run_tunslip():
	out = subprocess.Popen(['./tunslip6', '-s',tun_usbport,ipv6_tunnel], stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
	stdout,stderr = out.communicate()
	print(stdout)
	
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
    	
def coap_handler(coap_address, method, resource, data):
	mutex.acquire()
	#usando coap-client de linux
	ts = time.time()
	timestamp = datetime.datetime.fromtimestamp(ts).strftime("%d-%m-%Y (%H:%M:%S.%f)")	
	if method is 'get':
		out = subprocess.Popen(['coap-client', '-v', '0','-B',coap_max_wtime,'-m',method,'coap://['+coap_address+']/'+resource], stdout=subprocess.PIPE, stderr=subprocess.STDOUT)		
	if method is 'post' or method is 'put':
		out = subprocess.Popen(['coap-client', '-v', '0','-B',coap_max_wtime,'-m',method,'-e',data,'coap://['+coap_address+']/'+resource], stdout=subprocess.PIPE, stderr=subprocess.STDOUT)		
	stdout,stderr = out.communicate()
	datatosend=re.sub('v:1 t:CON.*]\s','',stdout)
	print(datatosend.rstrip())
	client.publish("data", "["+timestamp+"]"+"["+coap_address+"]"+"["+resource+"]"+"{"+datatosend.rstrip()+"}")
	mutex.release()
	#usando la api de python
	#coapc = HelperClient(server=(coap_server, 5683))
	#cresponse = coapc.get(resource)
	#print(cresponse.payload)
	#coapc.stop()
 
def on_message(client, userdata, message):
#mosquitto_pub -h beesmart.cure.edu.uy -p 2000 -t "send/METHOD" -u pfc -P pfc -m "ADDRESS%TOPIC%DATA"
	#print("message received " ,str(message.payload.decode("utf-8")))
	#print("message topic=",message.topic)
	#print("message qos=",message.qos)
	#print("message retain flag=",message.retain)
	if "get" in message.topic:
		coap_handler(message.payload.split("%")[0], "get", message.payload.split("%")[1], "")
	elif "post" in message.topic:
		coap_handler(message.payload.split("%")[0], "post", message.payload.split("%")[1],message.payload.split("%")[2])
	elif "put" in message.topic:
		coap_handler(message.payload.split("%")[0], "put", message.payload.split("%")[1],message.payload.split("%")[2])
		
def mosquitto_init():
	client.username_pw_set(user, passw)
	#client.on_subscribe = on_subscribe
	#client.on_unsubscribe = on_unsubscribe
	client.on_message = on_message
	#client.on_publish = on_publish
	#client.on_connect = on_connect
	#client.on_disconnect = on_disconnect
	client.connect(broker, port)
	client.subscribe("send/#", qos=1)	
	client.loop_forever()

	
if __name__== "__main__":
	main()
