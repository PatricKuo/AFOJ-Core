AFOJ-Core
=========

A Judge Core For AFOJ  By Python

分两个部分
judged judge-client

judge(Python) 是任务分发子系统
judge-client(Python+C) 是任务处理系统
judged 运行在Linux上
judge-client 运行在windows上

***
在judge-client 这一部分，最核心的一部分采用C编译成的DLL
删除文件 编译等任务使用Python解决
***
问题
=========
	judged
	1.采用了MultiProcess模块 分布式进程
	只能在linux上运行，在windows上运行抛出lamda错误
	我是Python小白 也不懂到底是为啥
	2.在运行一段时间后会发生段错误
	对 没错 就是Python 的段错误
	给一段错误结果：
	*** Error in `python': double free or corruption (out): 	0x00007fd6dc001970 ***
	已放弃 (核心已转储) 
	求大神指导！！！
	
	
