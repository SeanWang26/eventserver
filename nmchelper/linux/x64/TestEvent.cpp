#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <assert.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include "NMCUser.h"
#include <execinfo.h>

#include <dlfcn.h>
#include <pthread.h>

#include "../src/JtEventServer.h"
#include "../src/JtEventConnect.h"


static void jt_show_signal_type(int value)
{
	switch(value)
	{
		case 2:
			fprintf(stderr, "\n SIGINT : 点击了Ctrl+C 关闭\n");
		break;
		case 3:
			fprintf(stderr, "\n SIGQUIT \n");
		break;
		case 8:
			fprintf(stderr, "\n SIGFPE : 浮点异常\n");
		break;
		case 9:
			fprintf(stderr, "\n SIGKILL \n");
		break;
		case 11:
			fprintf(stderr, "\n SIGSEGV : 段错误\n");
		break;
		case 13:
			fprintf(stderr, "\n SIGPIPE : 管道异常\n");
		break;
		case 15:
			fprintf(stderr, "\n SIGTERM : 被另一个进程关闭\n");
		break;
		default:
			fprintf(stderr, "\n Unknow type %d \n", value);
	}
	
}
static void jt_handle_exception(int value)
{
	fprintf(stderr, "\n11#########handle_exception  catch signal %d ,"
		" show signal name use $kill -l  #########\n", value);
	void *stack_p[20];
	char **stack_info;
	int size = 0;
	jt_show_signal_type(value);

	size = backtrace(stack_p, sizeof(stack_p));
	stack_info = backtrace_symbols(stack_p, size);
	
	fprintf(stderr, "%d stack frames.\n", size);

	for(int i = 0; i < size; i++)
	{
		fprintf(stderr, "%s\n", stack_info[i]);
	}

	//在守护里再输出
	fprintf(stderr, "\n22#########handle_exception  catch ERROR %d ,"
		" show use $kill -l  #########\n",value);
	fprintf(stderr, "%d 行 stack frames.\n", size);
	
	for(int i = 0; i < size; i++)
	{
		fprintf(stderr,"%s\n", stack_info[i]);
	}

	if(stack_info)
	{
		free(stack_info);
		stack_info = NULL;
	}

	//ShowMemInfo();
	fflush(stdout);

	if(value == SIGINT || value == SIGQUIT
		|| value == SIGFPE || value == SIGKILL
		|| value == SIGSEGV || value == SIGTERM || value == SIGABRT || value == SIGALRM)
	{	
		//printf("kill -9 $(lsof -i:%d | sed -n '2p' | awk '{print $2}') \n", gLoginPort);
		//sprintf(strKillTheSamePortLnvrCmd, "kill -9 $(lsof -i:%d | sed -n '2p' | awk '{print $2}')", gLoginPort);
		//system(strKillTheSamePortLnvrCmd);
		exit(0);
	}

}

void needshowstackbackwhencrack(void)
{
	signal(SIGQUIT, jt_handle_exception);
	signal(SIGKILL, jt_handle_exception);
	signal(SIGFPE,	jt_handle_exception);
	signal(SIGSEGV, jt_handle_exception);
	signal(SIGTERM, jt_handle_exception);
	signal(SIGBUS,	jt_handle_exception);
	signal(SIGABRT,	jt_handle_exception);
	signal(SIGALRM, jt_handle_exception);
	signal(SIGPIPE, SIG_IGN);
}
inline int create_detached_thread(pthread_t *tid, void* (*func)(void *), void* arg)
{
	int res = 0;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	res = pthread_create(tid, &attr, func, arg);
	pthread_attr_destroy(&attr);

	return res;
}

JtEventServer *g_JtEventServer = NULL;

void *test_all(void* arg)
{
	//JtEventServer *pJtEventServer = NULL;
	//pJtEventServer = new JtEventServer();
	//pJtEventServer->Start();	
	
	while(1)
	{
		JtEventConnect*	m_Conn = new JtEventConnect();
		m_Conn->DoConnect("192.168.3.86", 22, 5, g_JtEventServer);
		
		m_Conn->DoDisconnect();

		delete m_Conn;

		sleep(3);
	}
}

int main(int argc, char** argv)
{
	needshowstackbackwhencrack();

	g_JtEventServer = new JtEventServer();
	g_JtEventServer->Start();	
	
	for(int i=0; i<100; ++i)
	{
		printf("while(i++) %d\n", i);	
		pthread_t tid;
		create_detached_thread(&tid, test_all, NULL);
	}

	getchar();
}















