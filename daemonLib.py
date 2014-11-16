# coding=utf-8
import random, time, Queue
from multiprocessing.managers import BaseManager
class taskSender(object):
	"""将数据传入队列，分发到各个子服务器"""
	def __init__(self):
		# self.runid=source[0]
		# self.source=source[1]
		self.runid=-1
		self.answer="Nothing"
		# 发送任务的队列:
		task_queue = Queue.Queue()
		# 接收结果的队列:
		result_queue = Queue.Queue()
		class QueueManager(BaseManager):
		    pass
		# 把两个Queue都注册到网络上, callable参数关联了Queue对象:
		QueueManager.register('get_task_queue', callable=lambda: task_queue)
		QueueManager.register('get_result_queue', callable=lambda: result_queue)
		# 绑定端口5000, 设置验证码'abc':
		manager = QueueManager(address=('', 5000), authkey='abc')
		# 启动Queue:
		manager.start()
		# 获得通过网络访问的Queue对象:
		self.task = manager.get_task_queue()
		self.result = manager.get_result_queue()
	def putQueue(self,source):
		print ("Now Put RunID %s" %str(source[0]))
		self.task.put(source)
	def getQueue(self):
		temp=self.result.get(timeout=20)
		self.runid=temp[0]
		self.answer=temp[1]
		return self.answer
		print ("RunID%s Result:%s" %self.runid,self. answer)
