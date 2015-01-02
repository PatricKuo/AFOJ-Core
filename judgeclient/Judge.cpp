#include <windows.h>
#include <winbase.h>
#include <process.h>
#include <iostream>
#include <conio.h>
#include <stdlib.h>
#include <io.h>
#include <time.h>
#include <queue>
#include <string>
#include <sstream>
#include <cstdio>
#include <psapi.h>
#include <queue>
#include <string>
#include <sstream>
 // #include "judge.h"
#include "define.h"
#include "util.cpp"
#include "tools.cpp"
#define BUFSIZE 4096
#define _WIN32_WINNT 0x0500
#define MAX_PATH 1024
#define AFTimelimit 2000
#define AFmemorylimit 64*1024
#define AFoutputlimit 64*1024
using namespace std;
PROCESS_INFORMATION pi;
int AFproblemId=-1;
int AFsolutionId=-1;
int AFresult=-1;
int AFmemory=-1;
int AFtime=-1;
int AFoutput=-1;
HANDLE InputFile;
HANDLE OutputFile;
DWORD AFprocode;
clock_t startt,endt;
char workPath[MAX_PATH]="work\\";
char dataPath[MAX_PATH]="data\\";
char logPath[MAX_PATH]="log\\";
char eachRunPath[MAX_PATH];
char exePath[MAX_PATH];
char compileCmd_str[MAX_PATH];
char inFileName[MAX_PATH];
char outFileName[MAX_PATH];
char standardOutFileName[MAX_PATH];
char runCmd_str[MAX_PATH];
unsigned int _stdcall CompileThread(PVOID pM){ 

	
	sprintf(compileCmd_str,"g++ -o work\\%d.exe work\\%d.cpp",AFsolutionId,AFsolutionId);
	system(compileCmd_str);
	// printf("%s\n",compileCmd_str);

	return 0;
}

int compile(){
	HANDLE ThreadCompile=(HANDLE)_beginthreadex(NULL, 0, CompileThread, NULL, NULL, NULL);
	if(ThreadCompile==NULL) {
		write_log(ERROR,"Create CompileThread Error");
		CloseHandle(ThreadCompile);
	}
	int _status=WaitForSingleObject(ThreadCompile,5000);
	if(_status>0)
		write_log(WARNING,"CompileThread TimeExceedd");
		// return 0;
	if( (_access(exePath, 0 )) != -1 )   {
		return 1;
	}
	else 
	{
		write_log(ERROR,"Compile Failed");
		AFresult=V_CE;
		return 0;
	}
	// char workPath[MAX_PATH]="work\\";
	// char buf[1024];
	// sprintf(buf, "%d", solutionId);
	// string name = buf;
	// string compileCmd_str="g++  -ansi -fno-asm -Wall -lm -s -std=c++98 -O2 -DONLINE_JUDGE -o %PATH%%NAME% %PATH%%NAME%.%EXT%  2>%PATH%%NAME%.txt";
	// string compile_string=compileCmd_str;
	// replace_all_distinct(compile_string,"%PATH%",workPath);
	// replace_all_distinct(compile_string,"%NAME%","solutionId");
	// replace_all_distinct(compile_string,"%EXT%","exe");
	// // replace_all_distinct(compile_string,"%EXE%",GL_languageExe);
	// strcpy(compileCmd_str,compile_string.c_str()); 
	
}

int init(){
	sprintf(eachRunPath,"%s%d\\",workPath,AFsolutionId);
	sprintf(exePath,"%s\\%d.exe",workPath,AFsolutionId);
	if( (_access(workPath, 0 )) == -1 )   CreateDirectory(workPath,NULL);
	if( (_access(eachRunPath, 0 )) == -1 )   CreateDirectory(eachRunPath,NULL);
	if( (_access(logPath, 0 )) == -1 )   CreateDirectory(logPath,NULL);
	write_log(INFO,"Init Environment");
	write_log(INFO,"Judge Runid:%d,ProblemID:%d",AFsolutionId,AFproblemId);
	printf("%s\n",exePath);
	AFresult=-1;
   	AFmemory=-1;
	AFtime=-1;
	AFoutput=-1;
	return 0;
}

unsigned int  _stdcall RunProgramThread(PVOID pM){
	


	// /输入输出重定向
	/* ChildIn_Write是子进程的输入句柄，ChildIn_Read是父进程用于写入子进程输入的句柄 */
	HANDLE ChildIn_Read, ChildIn_Write;
	/*ChildOut_Write是子进程的输出句柄，ChildOut_Read是父进程用于读取子进程输出的句柄*/
	HANDLE ChildOut_Read, ChildOut_Write;
  	// ChildIn_Write->ChildIn_Read->ChildOut_Write->ChildOut_Read
  	SECURITY_ATTRIBUTES saAttr = {0};
	saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
	saAttr.bInheritHandle = TRUE;
	saAttr.lpSecurityDescriptor = NULL;
	// 创建管道
	// bool temppipe=FALSE;
	CreatePipe(&ChildIn_Read, &ChildIn_Write, &saAttr, 0);
	CreatePipe(&ChildOut_Read, &ChildOut_Write, &saAttr, 0);
	SetHandleInformation(ChildIn_Write, HANDLE_FLAG_INHERIT, 0);
	SetHandleInformation(ChildOut_Read, HANDLE_FLAG_INHERIT, 0);
	SetErrorMode(SEM_NOGPFAULTERRORBOX );

	STARTUPINFO StartupInfo = {0};
	StartupInfo.cb = sizeof(STARTUPINFO);
	// ChildIn_Write->ChildIn_Read->ChildOut_Write->ChildOut_Read
	StartupInfo.hStdInput = ChildIn_Read;
	StartupInfo.hStdOutput = ChildOut_Write;
	StartupInfo.hStdError = ChildOut_Write;
	StartupInfo.dwFlags |= STARTF_USESTDHANDLES;
	

	CreateProcess(NULL,runCmd_str,NULL,NULL,TRUE,CREATE_SUSPENDED,NULL,NULL,&StartupInfo,&pi);
	InputFile= CreateFile(inFileName, GENERIC_READ, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_READONLY, NULL);
	ResumeThread(pi.hThread);
	CloseHandle(pi.hThread);
	bool flag=FALSE;
	startt=clock();
	while (true)
				{
					char buffer[BUFSIZE] = {0};
					DWORD BytesRead, BytesWritten;
					flag = ReadFile(InputFile, buffer, BUFSIZE, &BytesRead, NULL);
					if (!flag || (BytesRead == 0)) break;
					flag = WriteFile(ChildIn_Write, buffer, BytesRead, &BytesWritten, NULL);
					// cout<<"in WriteFile Flag "<<flag<<endl;
					if (!flag){ break;}
				}
				CloseHandle(InputFile);InputFile=NULL;
				CloseHandle(ChildIn_Write);ChildIn_Write=NULL;
				CloseHandle(ChildOut_Write);ChildOut_Write=NULL;
	OutputFile= CreateFile(outFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	while (true)
				{

					char buffer[BUFSIZE] = {0};
					DWORD BytesRead, BytesWritten;
					flag = ReadFile(ChildOut_Read, buffer, BUFSIZE, &BytesRead, NULL); //这行代码有问
					// cout<<"out  ReadFile flag"<<flag<<endl;
					// cout<<"BytesRead/buffer"<<BytesRead<<buffer<<endl;
					if (!flag || (BytesRead == 0)) break; //Bytes跳出Read=0 所以BRE

					flag = WriteFile(OutputFile, buffer, BytesRead, &BytesWritten, NULL);
					if (!flag) break;
					AFoutput+=BytesWritten;
					if(AFoutput>AFoutputlimit){
						AFresult = V_OLE;
						break;
					}
				}
				endt=clock();
				CloseHandle(ChildIn_Read);ChildIn_Read=NULL;
				CloseHandle(ChildOut_Read);ChildOut_Read=NULL;
				CloseHandle(OutputFile);OutputFile=NULL;


				
		
				// printf("The Program Used %d ms\n",endt-startt);
				write_log(INFO,"This case Used %d ms",endt-startt);
	return 0;
}

int Runsolution(){
	long caseTime=0;
	int i,case_;
	char buf[40960];
	// Work(solutionId,problemId);
	// AFprocode=0;
	for(i=0;;++i){
		case_=i;
		
		sprintf(inFileName,"%s%d\\test%d.in",dataPath,AFproblemId,case_);   
		sprintf(outFileName,"%s%d\\%d%_%d.out",workPath,AFsolutionId,AFsolutionId,case_);
		sprintf(standardOutFileName, "%s%d\\test%d.out",dataPath,AFproblemId,case_);
		sprintf(runCmd_str,"work\\%d.exe",AFsolutionId);
		// if(DEBUG) printf("inFileName:%s\n",inFileName);
		// if(DEBUG) printf("outFileName:%s\n",outFileName);
		// if(DEBUG) printf("standardOutFileName:%s\n",standardOutFileName);
		// if(DEBUG) printf("runCmd_str%s\n",runCmd_str);
		// sprintf(srcPath, "%s",outFileName);
		// sprintf(ansPath, "%s%d\\data%d.out",dataPath,GL_problemId,case_);

		if( (_access(inFileName, 0 )) == -1 )   {
			write_log(INFO,"All Test Completed");
			break ;
		}
		write_log(INFO,"Now Judge %d case",i);
		// GL_testcase = case_;
		HANDLE ThreadRun=(HANDLE)_beginthreadex(NULL, 0, RunProgramThread, NULL, NULL, NULL);
		if(ThreadRun==NULL) {
			write_log(ERROR,"Create ThreadRun ERROR");
			CloseHandle(ThreadRun);
		}
		
		DWORD status_ = WaitForSingleObject(ThreadRun,AFTimelimit+2000);   //放宽时限2S,返回值大于零说明超时.
		AFresult=compare(standardOutFileName,outFileName);
		write_log(INFO,"compare	return value:%s",VERDICT_NAME[AFresult]);
		if(status_>0){
			write_log(INFO,"ThreadRun TimeExceedd");
			// AFtime = AFTimelimit;
			if(AFresult==V_AC)
			{
				AFresult = V_TLE;
			}
		}
		if(InputFile!=NULL) CloseHandle(InputFile);
		if(OutputFile!=NULL) CloseHandle(OutputFile);
		write_log(INFO,"status	return value:%s",VERDICT_NAME[AFresult]);
		// if(InputFile!=NULL) CloseHandle(InputFile);
		// if(OutputFile!=NULL) CloseHandle(OutputFile);
		
		//AFreuslt 5(ac) 6(wa) 7(re) 10(pe)

		// if(DEBUG) printf("result is %d\n",result);
		#ifdef _WIN32_
		//get memory info
		PROCESS_MEMORY_COUNTERS   pmc; 
		unsigned long tmp_memory=0;
		if(GetProcessMemoryInfo(pi.hProcess,&pmc,sizeof(pmc))) { 
			tmp_memory=pmc.PeakWorkingSetSize/1024;
			if(tmp_memory>AFmemory) AFmemory=tmp_memory;
		}
		#endif
		//get process state
		GetExitCodeProcess(pi.hProcess, &AFprocode);
		if(RUN_existException(AFprocode)) AFresult=V_RE;
			// goto l;
		else if(AFprocode==STILL_ACTIVE){	//超时	
			// puts("TIME LIMIT");
			TerminateProcess(pi.hProcess, 0); 
			if(AFresult==V_AC) AFresult=V_TLE;
				// AFtime = AFTimelimit;
		}
		write_log(INFO,"AFprocode	return value:%s",VERDICT_NAME[AFresult]);
		caseTime = endt - startt;
		AFtime = (caseTime>AFtime)?caseTime:AFtime;
		if(caseTime<0){

			// caseTime = AFTimelimit;
			write_log(INFO,"caseTime <0");
		} 
		CloseHandle(pi.hProcess);
		// TerminateJobObject(G_job,0);//exit
		// CloseHandle(G_job);G_job=NULL;
		if(AFtime>=AFTimelimit){
			AFresult=V_TLE;
			// AFtime = AFTimelimit;
			// goto l;
		}
		write_log(INFO,"timelimit judge	return value:%s casetime:%dms AFtime:%d,startt:%d,endt:%d,endt-startt:%d",VERDICT_NAME[AFresult],caseTime,AFtime,startt,endt,endt-startt);
		if(AFmemory>=AFmemorylimit){
			AFresult=V_MLE;
			AFmemory = AFmemorylimit;
			// goto l;
		}
		write_log(INFO,"Case:%d Runid:%d ProblemID:%d Result:%s Time:%dms Memory:%d",i,AFsolutionId,AFproblemId,VERDICT_NAME[AFresult],caseTime,AFmemory);
		
	// 	if(DEBUG){
	// 	printf("AFsolutionId:%d\n",AFsolutionId);
	// 	printf("AFproblemId:%d\n",AFproblemId);
	// 	printf("Time used:%d\n",AFtime);
	// 	printf("AFmemory:%d\n",AFmemory);
	// 	printf("AFoutput:%d\n",AFoutput);
	// 	printf("AFresult:%d\n",AFresult);
	// 	printf("AFprocode:%d\n",AFprocode);
	}
	return 0;
		
}
// int data[3]={0};
 int judge(int a,int b,int data[]){
	AFproblemId=a;
	AFsolutionId=b;
	init();
	if(compile()) Runsolution();
	write_log(INFO,"Runid:%d ProblemID:%d Result:%s Time:%dms Memory:%d\n",AFsolutionId,AFproblemId,VERDICT_NAME[AFresult],AFtime,AFmemory);
	data[0]=AFresult;
	data[1]=AFtime;
	data[2]=AFmemory;
	return 0;
}
// static const char* LEVEL_NAME[] = {"SYSTEM_ERROR", "WARNING", "ERROR", "FATAL", "INFO"};
int main(int argc, char const *argv[]){
	/* code */\
	// int b[3];
	int data[3];
	judge(1182,20475 ,data);
	// judge(proid,runid,data)
	write_log(INFO,"Run Success");
	return 0;
}