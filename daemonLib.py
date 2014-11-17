# coding=utf-8
import random, time, Queue
from multiprocessing.managers import BaseManager
import mysql.connector
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
class getTaskMysql(object):
	"""docstring for ClassName"""
	def __init__(self):
		self.conn = mysql.connector.connect(user='root', password='root', database='test', use_unicode=True)
		self.cursor = self.conn.cursor()
		self.cursor.execute("set names utf8")
		self.conn.commit()
	def update(self,information):
		runid=information[0]
		time=information[1]
		memory=information[2]
		result=information[3]
		sql = "update user set time=%s memory=%s result=%s where runid='runid"     
		param = (time,memory,result)      
		n = self.cursor.execute(sql,param) 
		self.conn.commit()     
		print 'update',n      
	def insert(self):
		sql = "insert into source(id,run_id,source_code) values(%s,%s,%s)" 
		sql2 = "insert into source_information(id,run_id,time,memory,result,datetime) values(%s,%s,%s,%s,%s,%s)" 
		fp=open("code.txt","r")
		code=fp.read()
		# print code
		fp.close()
		param=(
			(1,1000,code),
			(2,1001,code),
			(3,1002,code),
		) 
		param2=(
			(1,1000,"","","Queueing",time.time()),
			(2,1001,"","","Queueing",time.time()),
			(3,1002,"","","Queueing",time.time()),
			)
		# n=self.cursor.executemany(sql,param) 
		m=self.cursor.executemany(sql2,param2)
		# print 'insertmany',n      
		self.conn.commit()
		# x=0
		# for i in range(10):
		# 	param=(int(x),int(x+1000),str(code))
		# 	n=self.cursor.executemany(sql,param)  
		# 	print 'insertmany',n      
		# 	self.conn.commit()
	def select1(self):
		#查询      
		# print time.ctime()
		n = self.cursor.execute("select * from source_information order by datetime desc") 
		# print n
		row=self.cursor.fetchall()  
		# print row[0]
		temp=row[0][1]
		# print temp
		return temp
		# self.cursor.close()
		# n2m=[]
		# n2m.append(temp)
		# print type(n2m)
		# sql="select * from source where run_id=%s "
		# m=self.cursor.execute(sql,n2m)
		
		# print m
		# return row[0]
	    		# for r in row[0]:      
	      #   			print r  
 	def select2(self,temp_id):
 		# self.cursor = self.conn.cursor()
 		m = self.cursor.execute("select * from source where run_id= %s",temp_id)
 		# print m
		rows=self.cursor.fetchall()
		print temp_id
		# print rows
		for row in rows:
			for x in row:
				print x

