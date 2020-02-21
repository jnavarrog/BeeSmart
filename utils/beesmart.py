# -*- coding: utf-8 -*-

import paho.mqtt.client as paho
import subprocess 
import os 
import threading
import time
#from coapthon.client.helperclient import HelperClient
import urllib2
import datetime
import re

global sub
global pub
sub = paho.Client()
pub = paho.Client()
global mutex
mutex = threading.Lock()
global accessfile
accessfile = threading.Lock()

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
	global idapiario
	global descapiario
	global enable_log
	global connect3g
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
		elif "D_ID_APIARIO" in x:
			idapiario=x.split("=")[1]
			idapiario=idapiario.rstrip()
		elif "D_APIARIO_DESC" in x:
			descapiario=x.split("=")[1]
			descapiario=descapiario.rstrip()
		elif "D_ENABLE_LOG" in x:
			enable_log=x.split("=")[1]
			enable_log=enable_log.rstrip()
		elif "D_USE_3GMODEM" in x:
			connect3g=x.split("=")[1]
			connect3g=connect3g.rstrip()	
	LOG("Configuración cargada")
	f.close()
	
	f=open("NODELIST.dat","w")
	f.close()
	
	LOG("Se lanza thread mosquitto_tr")
	mosquitto_tr = threading.Thread(target=mosquitto_init, name='mosquitto_tr')
	mosquitto_tr.start()
	LOG("Se lanza thread tunslip6")
	tunslip6_tr = threading.Thread(target=run_tunslip)
	tunslip6_tr.start()
	time.sleep(5)
	LOG("Se lanza thread get_nodelist_tr")
	get_nodelist_tr = threading.Thread(target=get_nodelist)
	get_nodelist_tr.start()
	LOG("Se lanza thread get_alldata_tr")
	get_alldata_tr = threading.Thread(target=get_alldata)
	get_alldata_tr.start()
	LOG("Fin main")
	
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
   		print(nodeip)
   		LOG("Se actualiza la lista de nodos")
   		time.sleep(float(time_upd_nlist))
   				
def get_alldata():
	nodearray=[]
	while 1:
		accessfile.acquire()
		f=open("NODELIST.dat","r")
		line=f.readlines()
		for x in line:
			if "node_ipv6" in x:
				ipnode=x.split("-")[0].split("]")[0].split("[")[1].rstrip()
				nodearray.append(ipnode)
   		f.close
   		accessfile.release()
   		for ip in nodearray:
   			print(ip)
   			for res in data_resources.split(","):
   				coap_handler(ip, "get", res, "")
   		del nodearray[:]
   		LOG("Se recolectaron y publicar automaticamente todos los datos")
		time.sleep(float(time_between_reqall))

def run_tunslip():
	out = subprocess.Popen(['./tunslip6', '-s',tun_usbport,ipv6_tunnel], stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
	stdout,stderr = out.communicate()
	#print(stdout)
	
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
	LOG("INICIO: coap-client "+coap_address+" "+method+" "+resource+" "+data)	
	if method is 'get':
		out = subprocess.Popen(['coap-client', '-v', '0','-B',coap_max_wtime,'-m',method,'coap://['+coap_address+']/'+resource], stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
	if method is 'post' or method is 'put':
		out = subprocess.Popen(['coap-client', '-v', '0','-B',coap_max_wtime,'-m',method,'-e',data,'coap://['+coap_address+']/'+resource], stdout=subprocess.PIPE, stderr=subprocess.STDOUT)		
	stdout,stderr = out.communicate()
	LOG("FIN: coap-client "+coap_address+" "+method+" "+resource+" "+data)	
	datatosend=re.sub('v:1 t:CON.*]\s','',stdout)
	print(datatosend.rstrip())
	LOG("Publicando datos")
	pub.publish(idapiario+"/data", "["+timestamp+"]"+"["+coap_address+"]"+"["+resource+"]"+"{"+datatosend.rstrip()+"}")
	LOG("Datos publicados")
	mutex.release()
 	
def on_message(client, userdata, message):
	LOG("on message")
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
	elif "rpicommands" in message.topic:
		if "nodelist" in message.payload:
			mutex.acquire()
			LOG("Se ejecutó el comando nodelist")
			os.system("mosquitto_pub -p " + port + " -h " +  broker + " -t "+idapiario+"/rpicommands -u "+ user+ " -P "+ passw + " -f NODELIST.dat")
			mutex.release()
		elif "configfile" in message.payload:
			mutex.acquire()
			LOG("Se ejecutó el comando configfile")
			os.system("mosquitto_pub -p " + port + " -h " +  broker + " -t "+idapiario+"/rpicommands -u "+ user+ " -P "+ passw + " -f BEESMART.conf")
			mutex.release()
		elif "iprpi" in message.payload:
			mutex.acquire()
			LOG("Se ejecutó el comando iprpi")
			os.system("ifconfig > iptemp.temp")
			time.sleep(0.2)
			os.system("mosquitto_pub -p " + port + " -h " +  broker + " -t "+idapiario+"/rpicommands -u "+ user+ " -P "+ passw + " -f iptemp.temp")
			time.sleep(0.2)
			os.system("rm -rf iptemp.temp")
			mutex.release()
		elif "description" in message.payload:
			mutex.acquire()
			LOG("Se ejecutó el comando description")
			os.system("mosquitto_pub -p " + port + " -h " +  broker + " -t "+idapiario+"/rpicommands -u "+ user+ " -P "+ passw + " -m "+descapiario)
			mutex.release()
		elif "rebootrpi" in message.payload:
			mutex.acquire()
			LOG("Se ejecutó el comando rebootrpi")
			os.system("reboot now")
			mutex.release()
		else:
			mutex.acquire()
			LOG("Se ejecutó un comando invalido")
			os.system("mosquitto_pub -p " + port + " -h " +  broker + " -t "+idapiario+"/rpicommands -u "+ user+ " -P "+ passw + " -m comando invalido")
			mutex.release()
			
	
def mosquitto_init():
	pub.username_pw_set(user, passw)
	sub.username_pw_set(user, passw)
	#client.on_subscribe = on_subscribe
	#client.on_unsubscribe = on_unsubscribe
	sub.on_message = on_message
	#client.on_publish = on_publish
	#client.on_connect = on_connect
	#client.on_disconnect = on_disconnect
	sub.connect(broker, port)
	pub.connect(broker, port)
	sub.subscribe(idapiario+"/send/#", qos=1)
	sub.loop_forever()
	
def LOG(msg):
	if "yes" in enable_log:
		print(msg)
		ts = time.time()
		timestamp = datetime.datetime.fromtimestamp(ts).strftime("%d-%m-%Y (%H:%M:%S.%f)")
		os.system("echo '["+timestamp+"] "+msg+"' >> BEESMART.log")

	
if __name__== "__main__":
	main()
