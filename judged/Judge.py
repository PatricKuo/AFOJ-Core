# coding=utf-8
"""
变量命名规则：
	变量
		首个单词字母小写后面单词字母大写:
		judgeQueue
	函数 
		各个单词的首字母大写
"""

import random, time, Queue
from multiprocessing.managers import BaseManager
import MySQLdb
import os
import time, threading
from time import sleep
class TaskSender(object):
	"""将数据传入队列，分发到各个子服务器"""
	def __init__(self):
		# 发送任务的队列:
		task_queue = Queue.Queue()
		# 接收结果的队列:
		result_queue = Queue.Queue()
		class QueueManager(BaseManager):
		    pass
		# 把两个Queue都注册到网络上, callable参数关联了Queue对象:
		BaseManager.register('get_task_queue', callable=lambda: task_queue)
		BaseManager.register('get_result_queue', callable=lambda: result_queue)
		# 绑定端口5000, 设置验证码'abc':
		manager = BaseManager(address=('',8888), authkey='abc')
		# 启动Queue:
		manager.start()
		# 获得通过网络访问的Queue对象:
		self.task = manager.get_task_queue()
		self.result = manager.get_result_queue()
	def PutQueue(self,source):
		# print ("Now Put RunID %s" %str(source[0]))
		self.task.put(source)
		print "现在将RunId:%s 加入网络Queue发送" %source[0]
	def GetQueue(self):
		temp=self.result.get(timeout=300)
		print "GetQueue Running"
		print temp
		print "RunID%s ProID:%s Result:%s Time:%sms memory:%skb" %(temp[0],temp[1],temp[2],temp[3],temp[4])
		return temp

class GetTaskMySQL(object):
	"""将需要判定的内容从数据库中取出，将数据传给TaskSender"""
	def __init__(self):
		self.conn = MySQLdb.connect(user='root', passwd='root', db='test', use_unicode=True)
		self.cursor = self.conn.cursor()
		self.cursor.execute("set names utf8")
		self.conn.commit()
	def Update(self,information):
		RunID=information[0]
		time=information[1]
		memory=information[2]
		result=information[3]
		sql = "update oj_solution set time=%d, memory=%d, result=%d where solution_id=%d"  %(time,memory,result,RunID)   
		print sql
		self.cursor.execute(sql) 
		self.conn.commit() 
	def UpdateToCompile(self,RunID):
		sql="update oj_solution set result=1 where solution_id=%d"%RunID
		# print sql
		self.cursor.execute(sql)
		self.conn.commit()
	def Insert(self):
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
	def SelectSolution(self):
		sql="select * from oj_solution where result=0 order by in_date "
		n = self.cursor.execute(sql) 
		# print "查询得到的记录行数:%d" %n
		if n==0:
			return False
		rows=self.cursor.fetchone()  
		# print "查询得到的记录RunID：%d"%rows[0]
		# print rows
		return rows
 	def SelectCode(self,RunID):
 		sql="select * from oj_source_code where solution_id= %d" %RunID
 		# print sql
 		m = self.cursor.execute(sql)
 		if m==0:
 			return False
		rows=self.cursor.fetchone()
		return rows
	def Reset(self):
		self.cursor.close()
		self.conn.close()
		self.conn = MySQLdb.connect(user='root', passwd='root', db='test', use_unicode=True)
		self.cursor = self.conn.cursor()
		self.cursor.execute("set names utf8")
		self.conn.commit()

def ThreadLiveDetect(ThreadList=[]):
    u"""
        *   功能
            *   等待给定list中的线程执行完毕
        *   输入
            *   线程列表
        *   返回
            *   待列表中的所有线程执行完毕后返回
            *   不会检测死锁
     """
    LiveFlag =   True
    while   LiveFlag:#等待线程执行完毕
        LiveFlag =   False
        Running   =   0
        for t   in  ThreadList:
            if  t.isAlive():
                LiveFlag=True
                Running+=1
        print u"目前还有{}条线程正在运行,等待所有线程执行完毕".format(Running)
        # PrintInOneLine(   u"目前还有{}条线程正在运行,等待所有线程执行完毕".format(Running))
        time.sleep(5)
if __name__=='__main__':
	TSender=TaskSender()
	GTaskMySQL=GetTaskMySQL()
	localQueue=Queue.Queue(maxsize=100)
	threadList=[]
	def GetFromSQL():
		print "GetFromSQL Thread Start"
		while True:
			try:
				solutionInformation=GTaskMySQL.SelectSolution()
				# print solutionInformation
				solutionCode=GTaskMySQL.SelectCode(solutionInformation[0])
				# print sol
				content=[999,999,"code"]
				# RunID,ProblemID,Code
				content[0]=solutionInformation[0]
				content[1]=solutionInformation[1]
				content[2]=solutionCode[1]
				localQueue.put(content)
				GTaskMySQL.UpdateToCompile(content[0])
				print "现在将RunId:%d 加入LocalQueue 待发送"%content[0]
			except:
				sleep(3)
				GTaskMySQL.Reset()
	def SendTask():
		sleep(1)
		print "SendTask Thread Start"
		while True:
			try:
				if localQueue.full():
					print "localQueue is Full!"
					sleep(3)
				else:
					while True:
						if  localQueue.empty():
							break
						content=localQueue.get()
						TSender.PutQueue(content)
			except:
				print "SendTask Error"
	def ReciveTask():
		sleep(3)
		print "ReciveTask Thread Start"
		while True:
			try:
				result=TSender.GetQueue()
				print "现在从子服务器获得RunID:%d 的数据结果"%result[0]
				temp=[-1,-1,-1,-1,-1]
				temp[0]=result[0]
				temp[1]=result[3]
				temp[2]=result[4]
				temp[3]=result[2]
				GTaskMySQL.Update(temp)
				# print 
			except:
				sleep(2)
	temp=threading.Thread(target=GetFromSQL,name="GTaskMySQLThread")
	threadList.append(temp)
	temp=threading.Thread(target=SendTask,name="SendTaskThread")
	threadList.append(temp)
	temp=threading.Thread(target=ReciveTask,name="ReciveTaskThread")
	threadList.append(temp)
	for i in range(3):
		threadList[i].start()
	ThreadLiveDetect(threadList)