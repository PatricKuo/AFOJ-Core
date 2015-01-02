
# coding=utf-8
import os, time, random
import shutil,sys
import Queue
from time import sleep
from ctypes import *  
from multiprocessing import Process
from multiprocessing import Pool
from multiprocessing.managers import BaseManager
{"ALL", "QUEUING", "COMPILING", "CE", "RUNNING","AC","WA","RE","TLE","MLE","PE","OLE","RF","OOC","SE"};
result_flag=(
	(0,"ALL"),
	(1,"QUEUING"),
	(2, "COMPILING"),
	(3,"CE"),
	(4,"RUNNING"),
	(5,"AC"),
	(6,"WA"),
	(7,"RE"),
	(8,"TLE"),
	(9,"MLE"),
	(10,"PE"),
	(11,"OLE"),
	(12,"RF"),
	(13,"OOC"),
	(14,"SE"),
	)

JudgeQueue=Queue.Queue(maxsize=10)
dll = CDLL("judge.dll")  
JudgeDLL=dll.judge
class CreateFile(object):
	"""docstring for CreateFile"""
	def __init__(self,):
		self.path='work\\'
		if not os.path.exists("work\\"):
			os.mkdir('work')
		# super(CreateFile, self).__init__()
		# self.arg = arg
	def create(self,solutionID,code):
		filepath=self.path+str(solutionID)+'.cpp'
		if os.path.exists(filepath):
			os.remove(filepath)
		fp=open(filepath,'a')
		fp.write(code)                                                                                                                           
		fp.close()
	def filedel(self,solutionID):
		codepath=self.path+str(solutionID)+'.cpp'
		exepath=self.path+str(solutionID)+'.exe'
		outfilepath=self.path+str(solutionID)
# 		if os.path.exists(codepath):
# 			os.remove(codepath)
# 		if os.path.exists(exepath):
# 			os.remove(exepath)
# 		if os.path.isdir(outfilepath):
# 			shutil.rmtree(outfilepath)
def work(name):
	CF=CreateFile()
	if os.path.exists('results'+str(name)+'.txt'):
		os.remove('results'+str(name)+'.txt')
	fp=open('results'+str(name)+'.txt','a')
	fp.write('Run task %s (%s)...\n' % (name, os.getpid()))
	while(True):
		if JudgeQueue.empty():
			print 'JudgeQueue is empty'
			print  "Wait For The Task"
		else:
			temp=JudgeQueue.get()
			CF.create(temp[0],temp[2])
			ThreeIntegers=c_int*3
			Data=ThreeIntegers(0,0,0)
			JudgeDLL(temp[1],temp[0],Data)
			sleep(0.5)
			CF.filedel(temp[0])
			reslut_string="solutionID:%d problemID:%d Result:%s Time:%dms \n"%(temp[0],temp[1],result_flag[Data[0]][1],Data[1])
			fp.write(reslut_string)
		return Data
	fp.close()
class TaskReciever(object):
	"""接收从服务器传输的数据 判断 返回结果"""
	def __init__(self):
		class QueueManager(BaseManager):
    			pass
    			# 由于这个QueueManager只从网络上获取Queue，所以注册时只提供名字:
		BaseManager.register('get_task_queue')
		BaseManager.register('get_result_queue')
		# 连接到服务器，也就是运行taskmanager.py的机器:
		server_addr = '10.1.142.100'
		print('Connect to server %s...' % server_addr)
		# 端口和验证码注意保持与taskmanager.py设置的完全一致:
		m = BaseManager(address=(server_addr, 8888), authkey='abc')
		# 从网络连接:
		m.connect()
		# 获取Queue的对象:
		self.task = m.get_task_queue()
		self.result = m.get_result_queue()
	def Judge(self):
		# 从task队列取任务,并把结果写入result队列:
		while True:
			try:
				n=self.task.get(timeout=10,block=False)
				print "Judge RunID:%d problemID:%d" %(n[0],n[1])
				print "code:%s"%n[2]
				temp=[n[0],n[1],"Wrong"]
				JudgeQueue.put(n)
				data=work(0)
				temp=[n[0],n[1],data[0],data[1],data[2]]
				self.result.put(temp,block=False)
			except:
				time.sleep(3)
		print("task queue is empty")

if __name__=='__main__':
	createFile=CreateFile()
	TReciever=TaskReciever()
	TReciever.Judge()
