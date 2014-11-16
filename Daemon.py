from daemonLib import *
import time
TSender=taskSender()
fp=open("code.txt","r")
code=fp.read()
fp.close()
while  True:
	x=0
	for i in range(1000):
		source=[x,code]
		TSender.putQueue(source)
		x=x+1
	time.sleep(1)
while True:
	try:
		print TSender.getQueue()
	except Queue.Empty:
		break
		print "All Work Is Finished"
