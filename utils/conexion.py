# -*- coding: utf-8 -*-


import subprocess 
import os 
import threading
import time
import datetime
import re

ip="fd00::1/64"

def main():
	print("arrancando")
	chkcon_tr=threading.Thread(target=chkcon)
	chkcon_tr.start()


def chkcon():
	maxncon=0
	while 1:
		out = subprocess.Popen(['ifconfig','tun0'], stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
		stdout,stderr = out.communicate()
		#print(stdout)
		if ip.split("/")[0].rstrip() in stdout:
			print("hayyyy")
		else:
			print("no hay conexion")

		time.sleep(2)

if __name__== "__main__":
	main()

