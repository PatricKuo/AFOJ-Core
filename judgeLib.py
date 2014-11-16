# coding=utf-8
import time, sys, Queue
from multiprocessing.managers import BaseManager
class taskReciever(object):
	"""docstring for taskReciever"""
	def __init__(self):
		class QueueManager(BaseManager):
    			pass
    			# 由于这个QueueManager只从网络上获取Queue，所以注册时只提供名字:
		QueueManager.register('get_task_queue')
		QueueManager.register('get_result_queue')
		# 连接到服务器，也就是运行taskmanager.py的机器:
		server_addr = '127.0.0.1'
		print('Connect to server %s...' % server_addr)
		# 端口和验证码注意保持与taskmanager.py设置的完全一致:
		m = QueueManager(address=(server_addr, 5000), authkey='abc')
		# 从网络连接:
		m.connect()
		# 获取Queue的对象:
		self.task = m.get_task_queue()
		self.result = m.get_result_queue()
	def judge(self):
		# 从task队列取任务,并把结果写入result队列:
		while True:
			try:
				n=self.task.get(timeout=10,block=False)
				print "Judge RunID:%s" %str(n[0])
				print n[1]+'\n'
				temp=[n[0],"Wrong"]
				self.result.put(temp,block=False)
			except Queue.Empty:
				time.sleep(3)
			# self.result.put("Wrong")
		print("task queue is empty")