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

//#include "usersdk.h"

/*
#ifndef JT_FRONT_API
#define JT_FRONT_API
#endif
#ifndef JT_CALL_TYPE
#define JT_CALL_TYPE
#endif

typedef int (JT_CALL_TYPE *jt_stream_callback)(int callback_type, void* data, int len, void** user);

typedef int (JT_CALL_TYPE *jt_init_dl)(void* );
typedef int (JT_CALL_TYPE *jt_uninit_dl)(void* );
typedef int (JT_CALL_TYPE *jt_set_event_callback_dl)(jt_stream_callback callback, void* userdata);

typedef void* (JT_CALL_TYPE *jt_create_device_dl)(unsigned int type);
typedef int (JT_CALL_TYPE *jt_distory_device_dl)(void* handle);

typedef int (JT_CALL_TYPE *jt_login_dl)(void* handle, struct stLogin_Req *req, struct stLogin_Rsp *rsp);
typedef int (JT_CALL_TYPE *jt_login_ex_dl)(void* handle, int type, struct stLogin_Req *req, struct stLogin_Rsp *rsp);//创建一个新的设备，并登陆
typedef int (JT_CALL_TYPE *jt_logout_dl)(void* handle, struct stLogout_Req *req, struct stLogout_Rsp *rsp);
typedef int (JT_CALL_TYPE *jt_logout_ex_dl)(void* handle, struct stLogout_Req *req, struct stLogout_Rsp *rsp);//退出登陆，删除设备

typedef int (JT_CALL_TYPE *jt_get_config_dl)(void* handle, struct stGetConfig_Req *req, struct stGetConfig_Rsp *rsp);
typedef int (JT_CALL_TYPE *jt_set_config_dl)(void* handle, struct stSetConfig_Req *req, struct stSetConfig_Rsp *rsp);
typedef int (JT_CALL_TYPE *jt_open_video_stream_dl)(void* handle, struct stOpenVideoStream_Req *req, struct stOpenVideoStream_Rsp *rsp);
typedef int (JT_CALL_TYPE *jt_close_video_stream_dl)(void* handle, struct stCloseVideoStream_Req *req, struct stCloseVideoStream_Rsp *rsp);
typedef int (JT_CALL_TYPE *jt_open_audio_stream_dl)(void* handle, struct stOpenAudioStream_Req *req, struct stOpenAudioStream_Rsp *rsp);
typedef int (JT_CALL_TYPE *jt_close_audio_stream_dl)(void* handle, struct stCloseAudioStream_Req *req, struct stCloseAudioStream_Rsp *rsp);
typedef int (JT_CALL_TYPE *jt_open_alarm_stream_dl)(void* handle, struct stOpenAlarmStream_Req *req, struct stOpenAlarmStream_Rsp *rsp);
typedef int (JT_CALL_TYPE *jt_close_alarm_stream_dl)(void* handle, struct stCloseAlarmStream_Req *req, struct stCloseAlarmStream_Rsp *rsp);
typedef int (JT_CALL_TYPE *jt_ptz_control_dl)(void* handle, struct stPTZControl_Req *req, struct stPTZControl_Rsp *rsp);
typedef int (JT_CALL_TYPE *jt_set_system_time_dl)(void* handle, struct stSetTime_Req *req, struct stSetTime_Rsp *rsp);
typedef int (JT_CALL_TYPE *jt_start_talk_dl)(void* handle, struct stStartTalk_Req *req, struct stStartTalk_Rsp *rsp);
typedef int (JT_CALL_TYPE *jt_stop_talk_dl)(void* handle, struct stStopTalk_Req *req, struct stStopTalk_Rsp *rsp);
typedef int (JT_CALL_TYPE *jt_send_talk_data_dl)(void* handle, struct stSendTalkData_Req *req, struct stSendTalkData_Rsp *rsp);
typedef int (JT_CALL_TYPE *jt_get_video_effect_dl)(void* handle, struct stGetVideoEffect_Req *req, struct stGetVideoEffect_Rsp *rsp);
typedef int (JT_CALL_TYPE *jt_set_video_effect_dl)(void* handle, struct stSetVideoEffect_Req *req, struct stSetVideoEffect_Rsp *rsp);
typedef int (JT_CALL_TYPE *jt_get_video_record_dl)(void* handle, struct stGetVideoRecord_Req *req, struct stGetVideoRecord_Rsp *rsp);
typedef int (JT_CALL_TYPE *jt_playback_control_dl)(void* handle, struct stPlaybackCtrl_Req *req, struct stPlaybackCtrl_Rsp *rsp);
typedef int (JT_CALL_TYPE *jt_manual_record_dl)(void* handle, stManualRecord_Req *req, struct stManualRecord_Rsp *rsp);//手动录像
typedef int (JT_CALL_TYPE *jt_set_guard_dl)(void* handle, struct stSetGuard_Req *req, struct stSetGuard_Rsp *rsp);//设置布防
typedef int (JT_CALL_TYPE *jt_get_guard_dl)(void* handle, struct stGetGuard_Req *req, struct stGetGuard_Rsp *rsp);//获取布防
typedef int (JT_CALL_TYPE *jt_reboot_control_dl)(void* handle, struct stReboot_Req *req, struct stReboot_Rsp *rsp);//重启，关机
typedef int (JT_CALL_TYPE *jt_set_output_dl)(struct device *, struct stSetOutput_Req *req, struct stSetOutput_Rsp *rsp);//输出控制
*/

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


static int NMC_CALL_TYPE static_NmcStatusCallback(long handle, int type, void* data, int datalen, void *userdata)
{
	printf("static_NmcStatusCallback\n");	

	if(NDM_GET_SEARCH_DEVCIE==type)//搜到设备
	{
		struct st_xy_device_info *device_info = (struct st_xy_device_info *)data;

		printf("NDM_GET_SEARCH_DEVCIE name %s, %s, %s, %s\n"
			, device_info->name, device_info->ipv4, device_info->mask, device_info->gateway);	
	}
	
	return 0;
}

void get_get_matrix(long m_handle)
{
	struct st_matrix_info *pmatrix_info = NULL;
	int matrix_info_cnt = 0;

	int res = nmc_get_matrix(m_handle, -1, &pmatrix_info, &matrix_info_cnt);
	if(res)
	{
		printf("nmc_get_matrix failed\n");
		return ;
	}
	
	if(pmatrix_info==NULL || matrix_info_cnt==0)
	{
		printf("nmc_get_matrix null\n");
		return ;
	}

	for(int i=0; i<matrix_info_cnt; ++i)
	{
		printf("ip %s, id %d\n", pmatrix_info[i].ip, pmatrix_info[i].id);
	}

	for(int i=0; i<pmatrix_info->output_info_cnt; ++i)
	{
		struct st_output_info*  output_info = pmatrix_info->output_info;
		printf("id %d, index %d, hasdisplay %d, resolution %s\n", output_info->id, output_info->index, output_info->hasdisplay, output_info->resolution);
	}

	nmc_free_matrix_info(pmatrix_info, matrix_info_cnt);
}

int m_WndId = 0;

void get_output(long m_handle)
{
	//printf("2-get_output-\n");
	struct st_output_layout *ppoutput_layout = NULL;
	int output_layout_cnt = 0;

	//由nmc_get_output得到的都是单窗口类型的
	int res = nmc_get_output(m_handle, -1, &ppoutput_layout, &output_layout_cnt);
	//printf("3-get_output-\n");
	if(res)
	{
		printf("nmc_get_output failed! %d\n", res);
		return ;
	}

	for(int i=0; i<output_layout_cnt; ++i)
	{
		printf("[get_output]id %d, layout %d, window_info_cnt %d\n"
			, ppoutput_layout[i].id, ppoutput_layout[i].layout, ppoutput_layout[i].window_info_cnt);
		
		struct st_window_info* wndinfo = ppoutput_layout[i].window_info;
		int window_info_cnt = ppoutput_layout[i].window_info_cnt;
		for(int j=0; j<window_info_cnt; ++j)
		{
			printf("[get_output]wndinfo id %d, status %d, index %d, type 0, sourceid %d, subsourceid %d\n"
			, wndinfo[j].id, wndinfo[j].status, wndinfo[j].index, wndinfo[j].sourceid, wndinfo[j].subsourceid);
		}
	}

	printf("5-get_output- ppoutput_layout %p, output_layout_cnt %d\n", ppoutput_layout, output_layout_cnt);

	if(output_layout_cnt)
		m_WndId = ppoutput_layout[0].window_info[0].id;
	
	nmc_free_output_info(ppoutput_layout, output_layout_cnt);

	//printf("4-get_output-\n");
}
void get_source(long m_handle)
{
	//获取目前解码器上自带的所有信号源
	struct st_jn_equ *equ = NULL;
	int equcnt = 0;
	int res = nmc_get_signal_source(m_handle, &equ, &equcnt);
	if(res)
	{
		printf("nmc_get_signal_source failed\n");
		return;
	}

	for(int i=0; i<equcnt; ++i)
	{
		for(int j=0; j<equ[i].stSubEquCnt; ++j)
		{
			printf("[nmc_get_signal_source]SubEquId %d, EquId %d, SubName %s, IP %s\n"
				, equ[i].stSubEqu[j].SubEquId, equ[i].stSubEqu[j].EquId, equ[i].stSubEqu[j].SubName, equ[i].IP);
		}
	}

	nmc_free_equ_info(equ, equcnt);
}
void get_remote_source(long m_handle)
{


}
void get_sw_info(long m_handle)
{
	printf("[get_sw_info]\n");
	
	struct st_sw_window_info* info = NULL;
	int info_cnt = 0;
	int res = nmc_get_sw_windows_info(m_handle, -1, &info, &info_cnt);
	if(res)
	{
		printf("get_sw_info failed\n");
		return;
	}

	for(int j=0; j<info_cnt; ++j)
	{
		printf("[get_sw_info]sw_window_id %d, sw_window_status %d, source_id %d, source_sub_id %d\n"
			, info[j].sw_window_id, info[j].sw_window_status, info[j].source_id, info[j].source_sub_id);
	}	

	if(info)
		nmc_free_sw_windows_info(info, info_cnt);
}

void tcpstream_test(long m_handle, char *Ip, int port, char * username, char * password, char * factoryName)
{
	printf("[tcpstream_test]\n");

	//  1.添加一个Tcp的信号源
	struct st_jn_equ jn_equ;
	memset(&jn_equ,0,sizeof(jn_equ));
	jn_equ.EquId = -1;
	strcpy(jn_equ.FactoryName, factoryName);
	static int nOpSeq = 0;
	char cEquName[64] = "";
	sprintf(cEquName, "devt-%d", ++nOpSeq);
	jn_equ.nOpSeq = nOpSeq;
	strcpy(jn_equ.EquName, cEquName);
	
	strcpy(jn_equ.IP, Ip);
	
	jn_equ.Port = port;
	strcpy(jn_equ.UserName, username);
	strcpy(jn_equ.Password, password);
	jn_equ.ChannelNum = 1; //默认为1
	jn_equ.InputNum = 0;   //默认为0
	jn_equ.OutputNum = 0;  //默认为0
	jn_equ.nOpType = 0;    //默认为0==OP_TYPE_ADD
	jn_equ.nCtlgId = 1;    //默认为1

	time_t t0 = time(0);
	
	int res = nmc_add_signal_source(m_handle, &jn_equ);
	if(res)
	{
		printf("nmc_add_signal_source failed!\n");
		return;
	}

	time_t t1 = time(0);
	printf("add signal cost %ld\n", t1-t0);	

	sleep(1);

	//  2.在窗口播放该信号源
	res = nmc_set_window_signal_source(m_handle, 0, 1, 1, jn_equ.stSubEqu[0].SubEquId);
	if(res)
	{
		printf("nmc_rmv_signal_source failed!\n");
	}

	time_t t2 = time(0);

	printf("nmc_set_window_signal_source cost %ld\n", t2-t1);	

	sleep(1);

	//  3.在窗口停播该信号源

	time_t t3 = time(0);
	res = nmc_clear_window_signal_source(m_handle, 0, 0, 1);
	if(res)
	{
		printf("nmc_rmv_signal_source failed!\n");
	}

	time_t t4 = time(0);
	printf("nmc_clear_window_signal_source cost %ld\n", t3-t4);	
	
	//  4.删除该信号源
	int sub_equ_id=jn_equ.stSubEqu[0].SubEquId;
	res = nmc_rmv_signal_source(m_handle, sub_equ_id);
	if(res)
	{
		printf("nmc_rmv_signal_source failed!\n");
	}

	time_t t5 = time(0);

	nmc_free_add_equ_info(&jn_equ);
	
	printf("del signal cost %ld\n", t5-t1);	
	
	
}


void* testloop(void *arg)
{
	while(1)
	{
		nmc_init(static_NmcStatusCallback, 0);

		//getchar();
		//nmc_search_device(NULL, NULL);
		int i = 0;
		//while(1)

		bool sw = true;

		time_t t0 = time(0);
	
		struct login_info info;
		memset(&info, 0, sizeof(info));
		strcpy(info.ip, "192.168.3.32");
		info.port = 40000;
		strcpy(info.user, "admin");
		strcpy(info.password, "admin");

		long m_handle = nmc_login(&info);
		if(m_handle==-1L)
		{
			printf("nmc_login failed!\n");
			return (void*)-1;
		}

		printf("nmc_login ok!\n");	

#if 1
		//getchar();
		
		get_get_matrix(m_handle);

		//getchar();
		
		get_output(m_handle);

		//getchar();
		
		get_source(m_handle);

		//getchar();
		
		get_remote_source(m_handle);

		//getchar();
		
		get_sw_info(m_handle);

		//getchar();
		

		time_t t1 = time(0);
		
		int res;
		if(sw)
		{
			res = nmc_set_window_signal_source(m_handle, 0, 385, 1, 33718);
		}
		else
		{
			res = nmc_set_window_signal_source(m_handle, 0, 385, 1, 33719);
		}
		sw = !sw;
		if(res)
		{
			printf("nmc_rmv_signal_source failed!\n"); 
		}
		time_t t2 = time(0);
		
		//sleep(1);

		time_t t3 = time(0);
		res = nmc_clear_window_signal_source(m_handle, 0, 1, 385);
		if(res)
		{

		}
		time_t t4 = time(0);
		printf("prepare cost %ld\n", t1-t0);	
		printf("nmc_set_window_signal_source cost %ld\n", t2-t1);	
		printf("nmc_clear_window_signal_source cost %ld\n", t3-t4);	

		res;
		if(sw)
		{
			tcpstream_test(m_handle, "192.168.3.65", 37777, "admin", "admin", "DH");
		}
		else
		{
			tcpstream_test(m_handle, "192.168.3.29", 80, "admin", "admin", "ONVIF");
		}

		sw = !sw;

		//添加一个TCP信号源并在窗口显示
		//tcpstream_test(m_handle, "192.168.3.45", 15479);

		printf("nmc_logout before!\n");	

#endif
		nmc_logout(m_handle);

		//getchar();
	
		printf("nmc_uninit 111!\n");	

		//nmc_uninit((void*)1);
		nmc_uninit(NULL);

		sleep(3);
	}
	
}


struct setinfo
{
	int nvrid;
	int equsubid;
	int outputid;
	int windowid;
};

void *test_all(void* arg)
{
	struct setinfo* _setinfo = (struct setinfo*)arg;
	
	while(1)
	{
		
		//sleep(4);
		//getchar();
		//nmc_search_device(NULL, NULL);
		
		nmc_init(static_NmcStatusCallback, 0);
		int i = 0;
		//while(1)

		bool sw = true;

		time_t t0 = time(0);
	
		struct login_info info;
		memset(&info, 0, sizeof(info));
		strcpy(info.ip, "192.168.3.86");
		info.port = 40000;
		strcpy(info.user, "admin");
		strcpy(info.password, "admin");

		printf("nmc_login ip %s, %d, %s %s\n"
			, info.ip, info.port, info.user, info.password);	

		long m_handle = nmc_login(&info);
		if(m_handle==-1L)
		{
			printf("nmc_login failed!\n");
			sleep(3);
			continue;
		}

		printf("nmc_login ok!\n");	

		//getchar();
#if 1		
		get_get_matrix(m_handle);

		//getchar();
		
		get_output(m_handle);

		//getchar();
		
		///////////get_source(m_handle);

		//getchar();
		
		//////////////////get_remote_source(m_handle);

		//getchar();
		
		get_sw_info(m_handle);

		//getchar();
		

		time_t t1 = time(0);

		//int res = nmc_set_window_signal_source(m_handle, 0, _setinfo->windowid, _setinfo->nvrid, _setinfo->equsubid);
		//if(res)
		//{
		//	printf("nmc_rmv_signal_source failed!\n"); 
		//}
		
		time_t t2 = time(0);
		
	//	sleep(5);

		time_t t3 = time(0);
/*		res = nmc_clear_window_signal_source(m_handle, 0, 1, 385);
		if(res)
		{

		}
*/		
		time_t t4 = time(0);
		///printf("prepare cost %ld\n", t1-t0);	
	///	printf("nmc_set_window_signal_source cost %ld\n", t2-t1);	
		///printf("nmc_clear_window_signal_source cost %ld\n", t3-t4);	

/*
		res;
		if(sw)
		{
			tcpstream_test(m_handle, "192.168.3.65", 37777, "admin", "admin", "DH");
		}
		else
		{
			tcpstream_test(m_handle, "192.168.3.29", 80, "admin", "admin", "ONVIF");
		}

		sw = !sw;
*/
		//添加一个TCP信号源并在窗口显示
		//tcpstream_test(m_handle, "192.168.3.45", 15479);

		///printf("nmc_logout before!\n");	
#endif
		nmc_logout(m_handle);
		//break;
	
		//getchar();
	
		///printf("nmc_uninit 111!\n");	

		///nmc_uninit((void*)1);
		nmc_uninit(NULL);

		//break;
		sleep(3);

	}
}


struct setinfo _setinfo[16] = {{2,1,1,1} ,{2,4,1,2} ,{2,5,1,3} ,{2,6,1,4}
							  ,{2,8,1,5} ,{2,9,1,6} ,{2,12,1,7} ,{2,32,1,8}
							  ,{2,33,1,9} ,{2,38,1,10},{2,40,1,11},{2,41,1,12}
							  ,{2,109,1,13},{2,64,1,14},{2,62,1,15},{2,63,1,16}};
							  
int main(int argc, char** argv)
{
	needshowstackbackwhencrack();

	//nvr_test();

	//testloop(NULL);
#if 1	

	for(int i=0; i<50; ++i)
	{
		printf("while(i++) %d\n", i);	
		pthread_t tid;
		//test_all(&_setinfo[i]);
		create_detached_thread(&tid, test_all, &_setinfo[i]);
	}

	//getchar();
	sleep(1000000);

#if 0
	int i=100;
	while(i--)
	{
		pthread_t tid;
		create_detached_thread(&tid, test_all, NULL);
	}


	
	sleep(10000);
#endif

#endif

#if 0	
	int i=20;
	while(i--)
	{
	
	nmc_init(static_NmcStatusCallback, 0);

	//getchar();
	//nmc_search_device(NULL, NULL);
	int i = 0;
	//while(1)

	bool sw = true;


		time_t t0 = time(0);
	
		struct login_info info;
		memset(&info, 0, sizeof(info));
		strcpy(info.ip, "192.168.3.86");
		info.port = 40000;
		strcpy(info.user, "admin");
		strcpy(info.password, "admin");

		long m_handle = nmc_login(&info);
		if(m_handle==-1L)
		{
			printf("nmc_login failed!\n");
			return -1;
		}

		printf("nmc_login ok!\n");	

		//getchar();
		
		get_get_matrix(m_handle);

		//getchar();
		
		get_output(m_handle);

		//getchar();
		
		get_source(m_handle);

		//getchar();
		
		get_remote_source(m_handle);

		//getchar();
		
		get_sw_info(m_handle);

		//getchar();
		

		time_t t1 = time(0);
		
		int res;
		if(sw)
		{
			res = nmc_set_window_signal_source(m_handle, 0, 385, 1, 33718);
		}
		else
		{
			res = nmc_set_window_signal_source(m_handle, 0, 385, 1, 33719);
		}
		sw = !sw;
		if(res)
		{
			printf("nmc_rmv_signal_source failed!\n"); 
		}
		time_t t2 = time(0);
		
		//sleep(1);

		time_t t3 = time(0);
		res = nmc_clear_window_signal_source(m_handle, 0, 1, 385);
		if(res)
		{

		}
		time_t t4 = time(0);
		printf("prepare cost %ld\n", t1-t0);	
		printf("nmc_set_window_signal_source cost %ld\n", t2-t1);	
		printf("nmc_clear_window_signal_source cost %ld\n", t3-t4);	

		res;
		if(sw)
		{
			tcpstream_test(m_handle, "192.168.3.65", 37777, "admin", "admin", "DH");
		}
		else
		{
			tcpstream_test(m_handle, "192.168.3.29", 80, "admin", "admin", "ONVIF");
		}

		sw = !sw;

		//添加一个TCP信号源并在窗口显示
		//tcpstream_test(m_handle, "192.168.3.45", 15479);

		printf("nmc_logout before!\n");	

		nmc_logout(m_handle);
		//break;
	

		//getchar();
	
		printf("nmc_uninit 111!\n");	

		//nmc_uninit((void*)1);
		nmc_uninit(NULL);
	}
#endif
	
	printf("nmc_uninit 222!\n");	
	
	return 0;
}




















