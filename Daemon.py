#encoding=utf-8 
from daemonLib import *
import time
#任务分发模块
# TSender=taskSender()
# fp=open("code.txt","r")
# code=fp.read()
# fp.close()
# while  True:
# 	x=0
# 	for i in range(1000):
# 		source=[x,code]
# 		TSender.putQueue(source)
# 		x=x+1
# 	time.sleep(1)
# while True:
# 	try:
# 		print TSender.getQueue()
# 	except Queue.Empty:
# 		break
# 		print "All Work Is Finished"


#数据库获取模块
GTaskMysql=getTaskMysql()
# GTaskMysql.insert()
temp1=GTaskMysql.select1()
print temp1
GTaskMysql.select2(temp1)