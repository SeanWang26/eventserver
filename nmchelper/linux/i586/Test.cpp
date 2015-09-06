#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "NMCUser.h"
#include <execinfo.h>

#include <dlfcn.h>

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
		assert(false);
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
	struct st_output_layout *ppoutput_layout = NULL;
	int output_layout_cnt = 0;

	//由nmc_get_output得到的都是单窗口类型的
	int res = nmc_get_output(m_handle, -1, &ppoutput_layout, &output_layout_cnt);
	if(res)
	{
		printf("nmc_get_output failed!\n");
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

	m_WndId = ppoutput_layout[0].window_info[0].id;
	nmc_free_output_info(ppoutput_layout, output_layout_cnt);
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

void tcpstream_test(long m_handle, char *Ip, int port)
{
	printf("[tcpstream_test]\n");

	//  1.添加一个Tcp的信号源
	struct st_jn_equ jn_equ;
	jn_equ.EquId = -1;
	strcpy(jn_equ.FactoryName, "TcpStream");
	static int nOpSeq = 0;
	char cEquName[64] = "";
	sprintf(cEquName, "tcp-%d", ++nOpSeq);
	jn_equ.nOpSeq = nOpSeq;
	strcpy(jn_equ.EquName, cEquName);
	
	strcpy(jn_equ.IP, Ip);
	
	jn_equ.Port = port;
	strcpy(jn_equ.UserName, "admin");
	strcpy(jn_equ.Password, "admin");
	jn_equ.ChannelNum = 1; //默认为1
	jn_equ.InputNum = 0;   //默认为0
	jn_equ.OutputNum = 0;  //默认为0
	jn_equ.nOpType = 0;    //默认为0==OP_TYPE_ADD
	jn_equ.nCtlgId = 1;    //默认为1
	int res = nmc_add_signal_source(m_handle, &jn_equ);
	if(res)
	{
		printf("nmc_add_signal_source failed! %d\n", res);
		return;
	}

	sleep(3);
	//  2.在窗口播放该信号源
	res = nmc_set_window_signal_source(m_handle, 0, m_WndId, 1, jn_equ.stSubEqu[0].SubEquId);
	if(res)
	{
		printf("nmc_rmv_signal_source failed!\n");
	}	

	sleep(3);
	
	//  3.在窗口停播该信号源
	res = nmc_clear_window_signal_source(m_handle, 0, 0, m_WndId);
	if(res)
	{
		printf("nmc_rmv_signal_source failed!\n");
	}

	sleep(1);
	
	//  4.删除该信号源
	int sub_equ_id=jn_equ.stSubEqu[0].SubEquId;
	res = nmc_rmv_signal_source(m_handle, sub_equ_id);
	if(res)
	{
		printf("nmc_rmv_signal_source failed!\n");
	}
	
}
/*int nvr_test()
{
	printf("[%s]nvr_test\n", __FUNCTION__);
	
	static void* nvrhandle = NULL;
	
	static jt_init_dl                            _jt_init_dl = 0;
	static jt_create_device_dl                   _jt_create_device_dl = 0;
	static jt_distory_device_dl                  _jt_distory_device_dl = 0;
	static jt_login_dl                           _jt_login_dl = 0;
	static jt_login_ex_dl                        _jt_login_ex_dl = 0;//创建一个新的设备，并登陆
	static jt_logout_dl                          _jt_logout_dl = 0;
	static jt_logout_ex_dl                       _jt_logout_ex_dl = 0;//退出登陆，删除设备
	static jt_get_config_dl                      _jt_get_config_dl = 0;
	static jt_set_config_dl                      _jt_set_config_dl = 0;

	static void* frontplug_dl = NULL;

	frontplug_dl = dlopen("./libjfront.so", RTLD_NOW|RTLD_GLOBAL|RTLD_DEEPBIND);//|RTLD_DEEPBIND
	if(frontplug_dl==NULL)
	{
		printf("[%s]dlopen ./libjfront.so failed\n", __FUNCTION__);
	}

	_jt_init_dl = (jt_init_dl)dlsym(frontplug_dl, "jt_init");
	_jt_create_device_dl = (jt_create_device_dl)dlsym(frontplug_dl, "jt_create_device");
	_jt_distory_device_dl = (jt_distory_device_dl)dlsym(frontplug_dl, "jt_distory_device");

	_jt_login_dl = (jt_login_dl)dlsym(frontplug_dl, "jt_login");
	_jt_logout_dl = (jt_logout_dl)dlsym(frontplug_dl, "jt_logout");

	_jt_login_ex_dl = (jt_login_ex_dl)dlsym(frontplug_dl, "jt_login_ex");
	_jt_logout_ex_dl = (jt_logout_ex_dl)dlsym(frontplug_dl, "jt_logout_ex");

	_jt_get_config_dl = (jt_get_config_dl)dlsym(frontplug_dl, "jt_get_config");
	_jt_set_config_dl = (jt_set_config_dl)dlsym(frontplug_dl, "jt_set_config");
	

	if(_jt_init_dl)
		_jt_init_dl(NULL);

	if(_jt_init_dl)
		nvrhandle = _jt_create_device_dl(1U);

	struct stLogin_Req req;
	strcpy(req.Ip, "192.168.3.167");
	strcpy(req.User, "admin");
	strcpy(req.Password, "admin");
	req.Port = 20000;
	req.EventCallback = NULL;
	req.EventCallbackUserData = NULL;

	//登陆
	if(_jt_login_dl==NULL || _jt_set_config_dl==NULL || _jt_logout_ex_dl==NULL)
		return -99;

	struct stLogin_Rsp rsp;
	int res = _jt_login_dl(nvrhandle, &req, &rsp);
	if(res)
	{
		return -99;
	}

	res = -99;
	int subequcnt = 0;
	st_jn_sub_equ *subequ = NULL;
	struct stGetConfig_Req zreq;
	struct stGetConfig_Rsp zrsp;
	
	//获取本机nvr上的所有主设备
	struct stGetConfig_Req rreq;
	struct stGetConfig_Rsp rrsp;

	rreq.Channel = -1;
	rreq.Codec = 0;
	rreq.Type = GET_EQU_INFO;
	rrsp.Config = NULL;
	rrsp.Size = 0;
	res = _jt_get_config_dl(nvrhandle, &rreq,&rrsp);
	if(res)
	{
		//goto end;
	}

	if(rrsp.Config==NULL || rrsp.Size==0)
	{
		//goto end;
		//return NMC_INVALIED_HANDLE;
	}

	//struct st_jn_equ *equ, int *equcnt
		
	//*equ = (struct st_jn_equ *)rrsp.Config;
	//*equcnt = rrsp.Size;

	//获取本机nvr上的所有子设备
	zreq.Channel = -1;
	zreq.Codec = 0;
	zreq.Type = GET_SUB_EQU_INFO;
	
	zrsp.Config = NULL;
	zrsp.Size = 0;
	
	res = _jt_get_config_dl(nvrhandle, &zreq,&zrsp);
	if(res)
	{
	
	}

	if(zrsp.Config==NULL || zrsp.Size==0)
	{
	
	}


}*/
int main(int argc, char** argv)
{
	needshowstackbackwhencrack();

	//nvr_test();
	
	nmc_init(static_NmcStatusCallback, 0);

	//nmc_search_device(NULL, NULL);
	//sleep(10);
	//return 0;
	
	struct login_info info;
	memset(&info, 0, sizeof(info));
	strcpy(info.ip, "192.168.3.177");
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

	get_get_matrix(m_handle);

	get_output(m_handle);

	get_source(m_handle);

	get_remote_source(m_handle);

	get_sw_info(m_handle);

	//添加一个TCP信号源并在窗口显示
	//tcpstream_test(m_handle, "192.168.3.45", 15479);

	nmc_logout(m_handle);	

	nmc_uninit(NULL);

	return 0;
}




















