import subprocess 
import os 
import threading
import time
import urllib2
import datetime
import re

topic="test"
enviarcada=3

cont=0
while 1:
	ts = time.time()
	timestamp = datetime.datetime.fromtimestamp(ts).strftime("%d-%m-%Y (%H:%M:%S.%f)")
	os.system("mosquitto_pub -p 2000 -h beesmart.cure.edu.uy -t "+topic+" -u pfc -P pfc -m \"["+timestamp+"][idcolmena][coapresource]{este es el dato "+str(cont)+"}\"")
	cont=cont+1
	time.sleep(enviarcada)

