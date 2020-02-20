# -*- coding: utf-8 -*-


import subprocess 
import os 
import threading
import time
import datetime
import re

host="beesmart.cure.edu.uy"
pings="3"

def main():
	print("arrancando")
	modem3g("connect")
	chkcon_tr=threading.Thread(target=chkcon)
	chkcon_tr.start()


def chkcon():
	maxncon=0
	while 1:
		out = subprocess.Popen(['ping','-q','-c',pings,host], stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
		stdout,stderr = out.communicate()
		#print(stdout)
		if "3 received" in stdout:
			print("hayyyy")
			maxncon=0
		else:
			print("no hay conexion")
                        maxncon=maxncon+1

		print(maxncon)
		if maxncon == 5:
			print("desconectanndoooo")
			modem3g("disconnect")
			time.sleep(3)
			print("conectanndoooo")
			modem3g("connect")
		time.sleep(5)

def modem3g(action):
	out = subprocess.Popen(['sakis3g',action], stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
        stdout,stderr = out.communicate()
	#print(stdout)
	if "MF190 connected to Antel" in stdout:
		print("conectado")
	if "Disconnected." in stdout or "Not connected." in stdout:
		print("desconectado")

if __name__== "__main__":
	main()

