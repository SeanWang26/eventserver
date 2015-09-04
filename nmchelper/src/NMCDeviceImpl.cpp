#include "StdAfx.h"
#include <assert.h>

#if (defined(WIN32) || defined(WIN64))
#include <Windows.h>
#include <process.h>
#include <string.h>
#include <tchar.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <stdio.h>
#include <dlfcn.h>
#endif

#include <time.h>
#include "CachedAffair.h"
#include "NmcErrorCode.h"
#include "NMCDeviceImpl.h"
#include "NmcCmdDefine.h"
#include "NmcMessage.h"
#include "jtprintf.h"

#include "usersdk.h"

#if (defined(WIN32) || defined(WIN64))
static HINSTANCE frontplug_dl = NULL;
#else
static void* frontplug_dl = NULL;
#endif

#define FRONT_DLSYM(dl,funcname) do{ _##funcname##_dl = (funcname##_dl)GetProcAddress(dl, ""#funcname); if(_##funcname##_dl==NULL) { return-1; } }while(0)

static jt_init_dl                            _jt_init_dl = 0;
static jt_create_device_dl                   _jt_create_device_dl = 0;
static jt_distory_device_dl                  _jt_distory_device_dl = 0;
static jt_login_dl                           _jt_login_dl = 0;
static jt_login_ex_dl                        _jt_login_ex_dl = 0;//创建一个新的设备，并登陆
static jt_logout_dl                          _jt_logout_dl = 0;
static jt_logout_ex_dl                       _jt_logout_ex_dl = 0;//退出登陆，删除设备
static jt_get_config_dl                      _jt_get_config_dl = 0;
static jt_set_config_dl                      _jt_set_config_dl = 0;
/*
static jt_open_video_stream_dl               _jt_open_video_stream_dl = 0;
static jt_close_video_stream_dl              _jt_close_video_stream_dl = 0;
static jt_open_audio_stream_dl               _jt_open_audio_stream_dl = 0;
static jt_close_audio_stream_dl              _jt_close_audio_stream_dl = 0;
static jt_open_alarm_stream_dl               _jt_open_alarm_stream_dl = 0;
static jt_close_alarm_stream_dl              _jt_close_alarm_stream_dl = 0;
static jt_ptz_control_dl                     _jt_ptz_control_dl = 0;
static jt_set_system_time_dl                 _jt_set_system_time_dl = 0;
static jt_start_talk_dl                      _jt_start_talk_dl = 0;
static jt_stop_talk_dl                       _jt_stop_talk_dl = 0;
static jt_send_talk_data_dl                  _jt_send_talk_data_dl = 0;
*/

NMCDeviceImpl::NMCDeviceImpl(void)
{
	GlobalInit();

	m_pstatus_callback = NULL;
	m_puserdata = NULL;

	nvrhandle = NULL;

	m_Logined = 0;

#if (defined(WIN32) || defined(WIN64))
	m_hHeartBeatThreadEventHandle = NULL;
	m_hHeartBeatThreadHandle = (HANDLE)-1L;
#else
	m_nHeartBeatThreadId = -1;
#endif
	
	m_CheckFrq = 1;

	m_Conn = new JtEventConnect();
}
NMCDeviceImpl::~NMCDeviceImpl(void)
{
	m_pstatus_callback = NULL;
	m_puserdata = NULL;

	if(m_Logined)
	{
		Logout();
	}

	delete m_Conn;
}

int NMCDeviceImpl::GlobalInit()
{
	static int gGlobalInit = 0;
	if(frontplug_dl==NULL && gGlobalInit==0)													
	{
		gGlobalInit = 1;
		
#if (defined(WIN32) || defined(WIN64))
		TCHAR szPath[MAX_PATH];                                             
		if( !GetModuleFileName(NULL, szPath, MAX_PATH ) )              
		{                                                              
			printf("GetModuleFileName failed (%d)\n", GetLastError());              
			return FALSE;                                                             
		}                                                                           
		TCHAR *cp = _tcsrchr(szPath, _T('\\'));
		if (cp != NULL)
			*(cp+1) = _T('\0');

		_tcscat(szPath, _T("frontplug.dll"));
		frontplug_dl = LoadLibrary(szPath);                   
		if(frontplug_dl==NULL) 											    
		{													    
			jtprintf("LoadLibrary() frontplug failed, %u\n", GetLastError());                                             
		}

		_jt_init_dl = (jt_init_dl)GetProcAddress(frontplug_dl, "_jt_init@4");
		_jt_create_device_dl = (jt_create_device_dl)GetProcAddress(frontplug_dl, "_jt_create_device@4");
		_jt_distory_device_dl = (jt_distory_device_dl)GetProcAddress(frontplug_dl, "_jt_distory_device@4");
	
		_jt_login_dl = (jt_login_dl)GetProcAddress(frontplug_dl, "_jt_login@12");
		_jt_logout_dl = (jt_logout_dl)GetProcAddress(frontplug_dl, "_jt_logout@12");	

		_jt_login_ex_dl = (jt_login_ex_dl)GetProcAddress(frontplug_dl, "_jt_login_ex@16");
		_jt_logout_ex_dl = (jt_logout_ex_dl)GetProcAddress(frontplug_dl, "_jt_logout_ex@12");	

		_jt_get_config_dl = (jt_get_config_dl)GetProcAddress(frontplug_dl, "_jt_get_config@12");
		_jt_set_config_dl = (jt_set_config_dl)GetProcAddress(frontplug_dl, "_jt_set_config@12");
#else
		frontplug_dl = dlopen("./libjfront.so", RTLD_NOW|RTLD_GLOBAL|RTLD_DEEPBIND);
		if(frontplug_dl==NULL)
		{
			//jtprintf("[%s]dlopen ./libjfront.so failed\n", __FUNCTION__);
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

#endif		
		if(_jt_init_dl)
			_jt_init_dl(NULL);
	}

	return 0;
}

int NMCDeviceImpl::OnRecvData(void* Cookie, unsigned char* pData, int dataLen)
{
	if(Cookie==m_Conn)
	{
		ExCommand *Head = (ExCommand *)pData;
		if(Head->nCmdType == EX_NMC_MATRIX_CONN_STATUS_NOTIFY)
		{
			NmcMessage NmcMsg;
			string RspMsg;
			RspMsg.assign((char*)pData, dataLen);
			std::string strCache;
			int MatrixId = -1;
			int ConnStatus = -1;
			NmcMsg.ParserMatrixConnStatusNotifyMsg(RspMsg, strCache, MatrixId, ConnStatus);

			if(m_Logined && m_pstatus_callback)
			{
				if(ConnStatus==0)
				{
					m_pstatus_callback(0, NDM_EVENT_DISCONNECT, &MatrixId, sizeof(MatrixId), m_puserdata);
				}
				else if(ConnStatus==1)
				{
					m_pstatus_callback(0, NDM_EVENT_CONNECTED, &MatrixId, sizeof(MatrixId), m_puserdata);
				}
			}

			return 0;
		}

		//static int Static_CheckBeExptedData(map<unsigned long long, tr1::shared_ptr<CCachedAffair> >& cachedMap, CCachedAffairMapLock &Lock
		//	, unsigned char* pData, long dataLen);

		CCachedAffairMap::Static_CheckBeExptedData(m_cachedAffairMap, m_lockCachedAffair,(unsigned char*)pData,(long)dataLen);
	}

	return 0;
}
int NMCDeviceImpl::OnConnected(void* Cookie)
{

	return 0;
}
int NMCDeviceImpl::OnClosed(void* Cookie)
{
	if(Cookie==m_Conn)
	{
		if(m_Logined && m_pstatus_callback)
			m_pstatus_callback(0, NMC_EVENT_DISCONNECT, NULL, 0, m_puserdata);
	}
	
	return 0;
}
int NMCDeviceImpl::OnReConnected(void* Cookie)
{

	return 0;
}
int NMCDeviceImpl::DoRequest(JtEventConnect* Conn, const char* pData,int dataLen, struct ST_AFFAIR_CALLBACK* pAffairCallBack, int Seq, int Cmd)
{
	int res = Conn->SendData(pData, dataLen);
	if(res)
	{
		return NMC_RECEIVE_REQ_FAILED;
	}

	if(pAffairCallBack)
	{
		res = CCachedAffairMap::Static_PushNewAffair(m_cachedAffairMap, m_lockCachedAffair, pAffairCallBack
			,Seq,Cmd, Cmd+1);

		if(res<0)
		{
			return NMC_ERROR_PUSHNEWAFFAIR;
		}
	}

	return 0;
}
/*
int NMCDeviceImpl::DoRequest(const char* pData,int dataLen, struct ST_AFFAIR_CALLBACK* pAffairCallBack, int Seq, int Cmd)
{
	int res = m_Conn->SendData(pData, dataLen);
	if(res)
	{
		return NMC_RECEIVE_REQ_FAILED;
	}

	//STRY;
	if(pAffairCallBack)
	{
		res = CCachedAffairMap::Static_PushNewAffair(m_cachedAffairMap, m_lockCachedAffair, pAffairCallBack
			,Seq,Cmd, Cmd+1);

		if(res<0)
		{
			return -1;
		}
	}
	//SCATCH;

	return 0;
}
*/
int NMCDeviceImpl::static_OnGotHeartBeatData(long cookie, unsigned char* pData, int dataLen)
{
	NMCDeviceImpl* DeviceImpl = (NMCDeviceImpl*)cookie;
	return DeviceImpl->OnGotHeartBeatData(pData, dataLen);
}
int NMCDeviceImpl::OnGotHeartBeatData(unsigned char* pData,int dataLen)
{
	ExCommand *Head = (ExCommand *)pData;
	assert(Head->nCmdType == EX_HEART_BEAT_RSP);

	if(EX_HEART_BEAT_RSP==Head->nCmdType)
	{
		string RspMsg;
		std::string strCache;
		int nRetCode = 0;
		std::string strRemark;
		RspMsg.assign((char*)pData, dataLen);
		NmcMessage Msg;
		Msg.ParserHeartBeatRspMsg(RspMsg, strCache, nRetCode, strRemark);
	}

	return 0;
}
int NMCDeviceImpl::static_OnGotHeartBeatOverTime(long cookie)
{
	NMCDeviceImpl* DeviceImpl = (NMCDeviceImpl*)cookie;
	return DeviceImpl->OnGotHeartBeatOverTime();
}
int NMCDeviceImpl::OnGotHeartBeatOverTime()
{

	return 0;
}
int NMCDeviceImpl::Init(nmc_status_callback status_callback, void* userdata)
{
	m_pstatus_callback = status_callback;
	m_puserdata = userdata;

	if(_jt_init_dl)
		nvrhandle = _jt_create_device_dl(DEVICE_JN);

	return 0;
}

#if (defined(WIN32) || defined(WIN64))
unsigned int NMCDeviceImpl::Static_HeartBeatThread(void *arg)
{
	NMCDeviceImpl *DeviceImpl = (NMCDeviceImpl *)arg;
	DeviceImpl->HeartBeatThread();
	_endthread();

	return 0;
}
void NMCDeviceImpl::HeartBeatThread()
{
	DWORD res = 0;
	while(m_Logined)
	{
		res = WaitForSingleObject(m_hHeartBeatThreadEventHandle, 10000);
		if(res==WAIT_OBJECT_0)
		{
			break;
		}
		else if(res==WAIT_TIMEOUT)
		{
			HeartBeat();

			//if(m_CheckFrq++%2)
			//if(m_CheckFrq)
			{
				CCachedAffairMap::Static_CheckBeOverTime(m_cachedAffairMap, m_lockCachedAffair, time(0));
			}
		}
		else
		{
			break;
		}
	}
}
#else
void* NMCDeviceImpl::Static_HeartBeatThread(void* arg)
{
	NMCDeviceImpl *DeviceImpl = (NMCDeviceImpl *)arg;
	return DeviceImpl->HeartBeatThread();
}
void* NMCDeviceImpl::HeartBeatThread()
{
	int ret = 0;
	while(m_Logined)
	{
		struct timespec ts;
		ret = clock_gettime(CLOCK_REALTIME, &ts);
		ts.tv_sec += 2;
		ret = sem_timedwait(&m_hHeartBeatThreadEventHandle, &ts);
		if(ret == 0)
		{
			break;
		}
		else
		{			
			if(errno == ETIMEDOUT)
			{
				HeartBeat();

				//if(m_CheckFrq++%2)
				//if(m_CheckFrq)
				{
					CCachedAffairMap::Static_CheckBeOverTime(m_cachedAffairMap, m_lockCachedAffair, time(0));
				}

				continue;
			}

			break;
		}
	}	

	return NULL;
}
#endif

long NMCDeviceImpl::Login(struct login_info *info)
{
	m_LoginInfo = *info;

	m_Conn->SetJtEventCallbackSink(this, this);
	int res = m_Conn->DoConnect(info->ip, info->port, 5000);
	if(res)
	{
		return NMC_CONNECT_FAILED;
	}
 
	NmcMessage NmcMsg;
	string ReqMsg;
	char cCookie[65] = {0};
	sprintf(cCookie, "%ld", (long)this);
	int Seq = NmcMsg.BuildLoginReqMsg(info->user, info->password, cCookie, 1, ReqMsg);
	if(Seq==-1)
	{
		m_Conn->DoDisconnect();
		return NMC_GEN_REQ_FAILED;
	}

	ST_AFFAIR_CALLBACK AffairCallBack((long)this);
	res = DoRequest(m_Conn, ReqMsg.data(), ReqMsg.size()+1, &AffairCallBack, Seq, EX_NMC_LOGIN_REQ);
	if(res)
	{
		m_Conn->DoDisconnect();
		return NMC_SEND_REQ_FAILED;
	}

	string RspMsg;
	RspMsg = AffairCallBack.m_pRecvData;

	std::string strCache; 
	int nRetCode=-1; 
	std::string strRemark;
	if(NmcMsg.ParserLoginRspMsg(RspMsg, strCache, nRetCode, strRemark))
	{
		m_Conn->DoDisconnect();
		return NMC_PASER_RSP_FAILED;
	}

	if(nRetCode)
	{
		m_Conn->DoDisconnect();
		return nRetCode;
	}

	m_Logined = 1;

#if (defined(WIN32) || defined(WIN64))
	unsigned threadID;
	m_hHeartBeatThreadEventHandle = CreateEvent(NULL,TRUE,FALSE,NULL);
	m_hHeartBeatThreadHandle = (HANDLE)_beginthreadex(NULL, 0, Static_HeartBeatThread, this, 0, &threadID);
#else
	res = sem_init(&m_hHeartBeatThreadEventHandle, 0, 0);
	if(res==0)
	{
		if(0 != pthread_create(&m_nHeartBeatThreadId, NULL, Static_HeartBeatThread, (void *)this))
		{
			sem_destroy(&m_hHeartBeatThreadEventHandle);
			return -1;
		}
	}
#endif
	return 0;
}
int  NMCDeviceImpl::Logout()
{
	/*NmcMessage NmcMsg;
	string ReqMsg;
	char cCookie[65] = {0};
	sprintf(cCookie, "%ld", (long)this);
	int Seq = NmcMsg.BuildLogoutReqMsg(0, cCookie, ReqMsg);
	if(Seq==-1)
	{
		return NMC_GEN_REQ_FAILED;
	}

	ST_AFFAIR_CALLBACK AffairCallBack((DWORD_PTR)this);
	int res = DoRequest(ReqMsg.data(), ReqMsg.size()+1, &AffairCallBack, Seq);
	if(res)
	{
		return NMC_SEND_REQ_FAILED;
	}

	string RspMsg;
	RspMsg.assign((const char*)AffairCallBack.m_pRecvData, AffairCallBack.m_pRecvSize);

	std::string strCache;
	int nRetCode=-1;
	std::string strRemark;
	if(NmcMsg.ParserLogoutRspMsg(RspMsg, strCache, nRetCode, strRemark))
	{
		return NMC_PASER_RSP_FAILED;
	}

	if(nRetCode)
	{
		return nRetCode;
	}
	*/

	m_Logined = 0;

#if (defined(WIN32) || defined(WIN64))
	if(m_hHeartBeatThreadEventHandle)
		SetEvent(m_hHeartBeatThreadEventHandle);

	if(m_hHeartBeatThreadHandle!=(HANDLE)-1)
	{
		WaitForSingleObject(m_hHeartBeatThreadHandle, INFINITE);
		CloseHandle(m_hHeartBeatThreadHandle);
	}

	CloseHandle(m_hHeartBeatThreadEventHandle);
	
	m_hHeartBeatThreadEventHandle = NULL;
	m_hHeartBeatThreadHandle = (HANDLE)-1;
#else
	if(m_nHeartBeatThreadId!=(unsigned)(-1))
	{
		int ret = sem_post(&m_hHeartBeatThreadEventHandle);
		if(ret)
		{
			
		}
		pthread_join(m_nHeartBeatThreadId, NULL);
		m_nHeartBeatThreadId = -1;
		sem_destroy(&m_hHeartBeatThreadEventHandle);
	}
#endif

	m_Conn->DoDisconnect();//StopConnect();

	struct stLogout_Req reqout;
	struct stLogout_Rsp rspout;
	if(_jt_logout_dl)
		_jt_logout_dl(nvrhandle, &reqout, &rspout);

	if(_jt_distory_device_dl)
		_jt_distory_device_dl(nvrhandle);
	
	nvrhandle = NULL;

	return 0;
}
int NMCDeviceImpl::GetUserDefineData(int id, struct st_xy_user_data **ppuser_data, int *user_data_cnt)
{
	NmcMessage NmcMsg;
	string ReqMsg;
	char cCookie[65] = {0};
	sprintf(cCookie, "%ld", (long)this);
	
	int Seq = NmcMsg.BuildGetUserDefineDataReqMsg(id, cCookie, ReqMsg);
	if(Seq==-1)
	{
		return NMC_GEN_REQ_FAILED;
	}

	ST_AFFAIR_CALLBACK AffairCallBack((long)this);
	int res = DoRequest(m_Conn, ReqMsg.data(), ReqMsg.size()+1, &AffairCallBack, Seq, EX_NMC_GET_USER_DEFINE_DATA_REQ);
	if(res)
	{
		return NMC_SEND_REQ_FAILED;
	}

	string RspMsg;
	if(AffairCallBack.m_pRecvData.size()==0)
		return NMC_RECEIVE_REQ_FAILED;
	RspMsg = AffairCallBack.m_pRecvData;

	std::string strCache;
	int nRetCode=-1;
	std::string strRemark;

	if(NmcMsg.ParserGetUserDefineDataMsg(RspMsg, strCache, ppuser_data, user_data_cnt, nRetCode, strRemark))
	{
		return NMC_PASER_RSP_FAILED;
	}

	if(nRetCode)
	{
		return nRetCode;
	}

	return 0;	
}
int NMCDeviceImpl::SetUserDefineData(enum en_nmc_operator_type operator_type, struct st_xy_user_data *ppuser_data)
{
	NmcMessage NmcMsg;
	string ReqMsg;
	char cCookie[65] = {0};
	sprintf(cCookie, "%ld", (long)this);
	int Seq = NmcMsg.BuildSetUserDefineDataReqMsg(operator_type, ppuser_data, cCookie, ReqMsg);
	if(Seq==-1)
	{
		return NMC_GEN_REQ_FAILED;
	}

	ST_AFFAIR_CALLBACK AffairCallBack((long)this);
	int res = DoRequest(m_Conn, ReqMsg.data(), ReqMsg.size()+1, &AffairCallBack, Seq, EX_NMC_SET_USER_DEFINE_DATA_REQ);
	if(res)
	{
		return NMC_SEND_REQ_FAILED;
	}

	string RspMsg;
	if(AffairCallBack.m_pRecvData.size()==0)
		return NMC_RECEIVE_REQ_FAILED;
	RspMsg = AffairCallBack.m_pRecvData;

	std::string strCache;
	int nRetCode=-1;
	std::string strRemark;

	if(NmcMsg.ParserSetUserDefineDataRspMsg(RspMsg, strCache, operator_type, ppuser_data, nRetCode, strRemark))
	{
		return NMC_PASER_RSP_FAILED;
	}

	if(nRetCode)
	{
		return nRetCode;
	}

	return 0;	
}
int NMCDeviceImpl::HeartBeat()
{
	NmcMessage NmcMsg;
	string ReqMsg;
	char cCookie[65] = {0};
	sprintf(cCookie, "%ld", (long)this);
	int Seq = NmcMsg.BuildHeartBeatReqMsg(0, cCookie, ReqMsg);
	if(Seq==-1)
	{
		return NMC_GEN_REQ_FAILED;
	}

	ST_AFFAIR_CALLBACK AffairCallBack((long)this, static_OnGotHeartBeatData, static_OnGotHeartBeatOverTime);
	int res = DoRequest(m_Conn, ReqMsg.data(), ReqMsg.size()+1, &AffairCallBack, Seq, EX_HEART_BEAT_REQ);
	if(res)
	{
		return NMC_SEND_REQ_FAILED;
	}

	return 0;
}

int NMCDeviceImpl::GetLoginInfo(struct login_info *info)
{
	*info = m_LoginInfo;

	return 0;
}

int NMCDeviceImpl::GetSignalSourceInner(struct login_info *loginfo, struct st_jn_equ **equ, int *equcnt)
{
	int res = NMC_INVALIED_HANDLE;
	int subequcnt = 0;
	st_jn_sub_equ *subequ = NULL;
	struct stGetConfig_Req zreq;
	struct stGetConfig_Rsp zrsp;

	struct stLogin_Req req;
	//或者读取配置文件？？？？？？？？？？？？？？？
	strcpy(req.Ip, loginfo->ip);
	strcpy(req.User, loginfo->user);
	strcpy(req.Password, loginfo->password);
	req.Port = loginfo->port;
	req.EventCallback = NULL;
	req.EventCallbackUserData = NULL;

	//登陆
	if(_jt_login_dl==NULL || _jt_set_config_dl==NULL || _jt_logout_ex_dl==NULL)
		return NMC_INVALIED_HANDLE;

	struct stLogin_Rsp rsp;
	res = _jt_login_dl(nvrhandle, &req, &rsp);
	if(res)
	{
		return NMC_INVALIED_HANDLE;
	}

	//获取本机nvr上的所有主设备
	struct stGetConfig_Req rreq;
	struct stGetConfig_Rsp rrsp;

	rreq. Channel = -1;
	rreq.Codec = 0;
	rreq.Type = GET_EQU_INFO;
	rrsp.Config = NULL;
	rrsp.Size = 0;
	res = _jt_get_config_dl(nvrhandle, &rreq,&rrsp);
	if(res)
	{
		goto end;
	}

	if(rrsp.Config==NULL || rrsp.Size==0)
	{
		goto end;
		return NMC_ERROR_PARAMETER;
	}

	*equ = (struct st_jn_equ *)rrsp.Config;
	*equcnt = rrsp.Size;

	//获取本机nvr上的所有子设备
	zreq. Channel = -1;
	zreq.Codec = 0;
	zreq.Type = GET_SUB_EQU_INFO;
	
	zrsp.Config = NULL;
	zrsp.Size = 0;
	
	res = _jt_get_config_dl(nvrhandle, &zreq,&zrsp);
	if(res)
	{
		goto end;
	}

	if(zrsp.Config==NULL || zrsp.Size==0)
	{
		goto end;
	}

	subequ = (struct st_jn_sub_equ *)zrsp.Config;
	subequcnt = zrsp.Size;

	for(int i=0; i<*equcnt; ++i)
	{
		(*equ)[i].stSubEqu = (struct st_jn_sub_equ *)calloc(64, sizeof(struct st_jn_sub_equ));
		int x = 0;
		for(int j=0; j<subequcnt&&j<64; ++j)
		{
			if((*equ)[i].EquId==subequ[j].EquId)
			{
				memcpy((*equ)[i].stSubEqu+x, &subequ[j], sizeof(st_jn_sub_equ));
				x++;
			}
		}
		(*equ)[i].stSubEquCnt = x;
	}

	//free(subequ);

	if(zrsp.Config==NULL || zrsp.Size==0)
	{
		goto end;
		return NMC_INVALIED_HANDLE;
	}


end:
	{
		struct stLogout_Req oreq;
		struct stLogout_Rsp orsp;
		if(0==_jt_logout_dl(nvrhandle, &oreq, &orsp))
		{
			//ok
		}
	}

	return res;
}

int NMCDeviceImpl::GetSignalSource(struct st_jn_equ **equ, int *equcnt)
{
	struct login_info info;
	GetLoginInfo(&info);

	strcpy(info.user, "admin");
	strcpy(info.password, "admin");
	info.port = 20000;

	return GetSignalSourceInner(&info, equ, equcnt);
}
int NMCDeviceImpl::AddSignalSource(struct st_jn_equ *equ)
{
	int res = 0;

	equ->OutputNum = 0;
	equ->InputNum = 0;

	struct login_info info;
	GetLoginInfo(&info);

	struct stLogin_Req req;

	//或者读取配置文件？？？？？？？？？？？？？？？
	strcpy(req.Ip, info.ip);
	strcpy(req.User, "admin");
	strcpy(req.Password, "admin");
	req.Port = 20000;
	req.EventCallback = NULL;
	req.EventCallbackUserData = NULL;

	//登陆
	if(_jt_login_dl==NULL || _jt_set_config_dl==NULL || _jt_logout_dl==NULL)
		return NMC_INVALIED_HANDLE;

	//建立一个长的连接还是用一次，登一次？？？？？？？？？？？？？？？？

	struct stLogin_Rsp rsp;
	int ret = _jt_login_dl(nvrhandle, &req, &rsp);
	if(ret)
	{
		return NMC_INVALIED_HANDLE;
	}

	struct stSetConfig_Req creq;
	struct stSetConfig_Rsp crsp;
	creq. Channel = 0;
	creq.Codec = 0;
	creq.Type = SET_CONFIG_ADD_EQU;

	creq.Config = (char*)equ;
	creq.Size = 1;
	res = _jt_set_config_dl(nvrhandle, &creq, &crsp);
	if(res)
	{
		goto end;
	}

	/*
	*sourceid = 0;
	struct st_jn_sub_equ * jn_sub_equ= jn_equ.stSubEqu;
	for(int j=0; j<jn_equ.stSubEquCnt; ++j)
	{
		struct st_jn_sub_equ *sub_equ = &jn_sub_equ[j];
		*sourceid = sub_equ->SubEquId;
		free(jn_sub_equ);
		break;
		printf("%d", sub_equ->SubEquId);
	}*/

	struct stLogout_Req oreq;
	struct stLogout_Rsp orsp;

end:
	if(0==_jt_logout_dl(nvrhandle, &oreq, &orsp))
	{
		//ok
	}

	return res;
}
int NMCDeviceImpl::RemoveSignalSource(int equid)
{
	int res = NMC_INVALIED_HANDLE;

	struct login_info info;
	GetLoginInfo(&info);

	struct stLogin_Req req;

	//或者读取配置文件？？？？？？？？？？？？？？？
	strcpy(req.Ip, info.ip);
	strcpy(req.User, "admin");
	strcpy(req.Password, "admin");
	req.Port = 20000;
	req.EventCallback = NULL;
	req.EventCallbackUserData = NULL;

	//登陆
	if(_jt_login_dl==NULL || _jt_set_config_dl==NULL || _jt_logout_dl==NULL)
		return NMC_INVALIED_HANDLE;

	//建立一个长的连接还是用一次，登一次？？？？？？？？？？？？？？？？

	struct stLogin_Rsp rsp;
	int ret = _jt_login_dl(nvrhandle, &req, &rsp);
	if(ret)
	{
		return NMC_INVALIED_HANDLE;
	}

	struct stSetConfig_Req creq;
	struct stSetConfig_Rsp crsp;
	creq. Channel = 0;
	creq.Codec = 0;
	creq.Type = SET_CONFIG_DEL_SUB_EQU;
	
	struct st_jn_sub_equ jn_sub_equ;
	jn_sub_equ.SubEquId = equid;
	creq.Config = (char*)&jn_sub_equ;
	creq.Size = 1;
	res = _jt_set_config_dl(nvrhandle, &creq, &crsp);
	if(res)
	{
		goto end;
	}

	/*
	*sourceid = 0;
	struct st_jn_sub_equ * jn_sub_equ= jn_equ.stSubEqu;
	for(int j=0; j<jn_equ.stSubEquCnt; ++j)
	{
		struct st_jn_sub_equ *sub_equ = &jn_sub_equ[j];
		*sourceid = sub_equ->SubEquId;
		free(jn_sub_equ);
		break;
		printf("%d", sub_equ->SubEquId);
	}*/

	struct stLogout_Req oreq;
	struct stLogout_Rsp orsp;

end:
	if(0==_jt_logout_dl(nvrhandle, &oreq, &orsp))
	{
		//ok
	}

	return res;
}
int NMCDeviceImpl::GetRemoteDevice(int id, struct st_jn_device **DeviceS, int *DevCnt)
{
	char cCookie[65] = {0};
	sprintf(cCookie, "%ld", (long)this);

	NmcMessage NmcMsg;
	int res = 0;
	ST_AFFAIR_CALLBACK AffairCallBack((long)this);
	{
		string ReqMsg;
		int Seq = NmcMsg.BuildGetRemoteDeviceReqMsg(id, cCookie, ReqMsg);
		if(Seq==-1)
		{
			return NMC_GEN_REQ_FAILED;
		}

		res = DoRequest(m_Conn, ReqMsg.data(), ReqMsg.size()+1, &AffairCallBack, Seq, EX_NMC_GET_SIGNAL_SOURCE_INFO_REQ);
		if(res)
		{
			return NMC_SEND_REQ_FAILED;
		}
	}

	int nRetCode=-1;
	{
		std::string strCache="";

		std::string strRemark = "";
		string RspMsg;
		//RspMsg.assign((const char*)AffairCallBack.m_pRecvData, AffairCallBack.m_pRecvSize);
		RspMsg = AffairCallBack.m_pRecvData;
		if(NmcMsg.ParserGetRemoteDeviceRspMsg(RspMsg, strCache, DeviceS, DevCnt, nRetCode, strRemark))
		{
			return NMC_PASER_RSP_FAILED;
		}

		for(int i=0; i<*DevCnt; ++i)
		{
			m_NvrDeviceS[(*DeviceS)[i].DeviceId] = (*DeviceS)[i];
		}
	}

	if(nRetCode)
	{
		return nRetCode;
	}
	
	return 0;
}
int NMCDeviceImpl::GetAddRemoteDevice(struct st_jn_device *dev)
{
	char cCookie[65] = {0};
	sprintf(cCookie, "%ld", (long)this);

	NmcMessage NmcMsg;
	int res = 0;
	ST_AFFAIR_CALLBACK AffairCallBack((long)this);
	{
		string ReqMsg;
		int Seq = NmcMsg.BuildAddRemoteDeviceReqMsg(dev, cCookie, ReqMsg);
		if(Seq==-1)
		{
			return NMC_GEN_REQ_FAILED;
		}

		res = DoRequest(m_Conn, ReqMsg.data(), ReqMsg.size()+1, &AffairCallBack, Seq, EX_NMC_SET_SIGNAL_SOURCE_REQ);
		if(res)
		{
			return NMC_SEND_REQ_FAILED;
		}
	}

	int nRetCode=-1;
	{
		std::string strCache="";

		std::string strRemark = "";
		string RspMsg;
		//RspMsg.assign((const char*)AffairCallBack.m_pRecvData, AffairCallBack.m_pRecvSize);
		RspMsg = AffairCallBack.m_pRecvData;
		if(NmcMsg.ParserAddRemoteDeviceRspMsg(RspMsg, strCache, dev, nRetCode, strRemark))
		{
			return NMC_PASER_RSP_FAILED;
		}
	}

	if(nRetCode)
	{
		return nRetCode;
	}
	
	return 0;
}
int NMCDeviceImpl::GetRemoveRemoteDevice(int DevId)
{
	if(DevId==1)
		return -1;

	char cCookie[65] = {0};
	sprintf(cCookie, "%ld", (long)this);

	NmcMessage NmcMsg;
	int res = 0;
	ST_AFFAIR_CALLBACK AffairCallBack((long)this);
	{
		string ReqMsg;
		int Seq = NmcMsg.BuildRemoveRemoteDeviceReqMsg(DevId, cCookie, ReqMsg);
		if(Seq==-1)
		{
			return NMC_GEN_REQ_FAILED;
		}

		res = DoRequest(m_Conn, ReqMsg.data(), ReqMsg.size()+1, &AffairCallBack, Seq, EX_NMC_SET_SIGNAL_SOURCE_REQ);
		if(res)
		{
			return NMC_SEND_REQ_FAILED;
		}
	}

	int nRetCode=-1;
	{
		std::string strCache="";

		std::string strRemark = "";
		string RspMsg;
		//RspMsg.assign((const char*)AffairCallBack.m_pRecvData, AffairCallBack.m_pRecvSize);
		RspMsg = AffairCallBack.m_pRecvData;
		if(NmcMsg.ParserRemoveRemoteDeviceRspMsg(RspMsg, strCache, nRetCode, strRemark))
		{
			return NMC_PASER_RSP_FAILED;
		}
	}

	if(nRetCode)
	{
		return nRetCode;
	}
	
	return 0;
}
int NMCDeviceImpl::GetRemoteSignalSource(int deviceid, struct st_jn_equ **equ, int *equcnt)
{
	map<int, struct st_jn_device>::iterator iter = m_NvrDeviceS.find(deviceid);
	if(iter==m_NvrDeviceS.end())
	{
		return -1;
	}

	struct login_info info;
	memset(&info, 0, sizeof(info));

	if(deviceid==1)
		strcpy(info.ip, m_LoginInfo.ip);
	else
		strcpy(info.ip, iter->second.IP);

	info.port = iter->second.Port;
	strcpy(info.user, iter->second.User);
	strcpy(info.password, iter->second.Password);
	
	return GetSignalSourceInner(&info, equ, equcnt);
}
int NMCDeviceImpl::GetMatrix(int matrixid, std::vector<struct stMatrixInfo> &MatrixInfoS)
{
	char cCookie[65] = {0};
	sprintf(cCookie, "%ld", (long)this);

	NmcMessage NmcMsg;
	int res = 0;
	ST_AFFAIR_CALLBACK AffairCallBack((long)this);
	{
		string ReqMsg;
		int Seq = NmcMsg.BuildGetMatrixReqMsg(matrixid, cCookie, ReqMsg);
		if(Seq==-1)
		{
			return NMC_GEN_REQ_FAILED;
		}

		res = DoRequest(m_Conn, ReqMsg.data(), ReqMsg.size()+1, &AffairCallBack, Seq, EX_NMC_GET_MATRIX_INFO_REQ);
		if(res)
		{
			return NMC_SEND_REQ_FAILED;
		}
	}

	int nRetCode=-1;
	{
		std::string strCache="";

		std::string strRemark = "";
		string RspMsg;
		//RspMsg.assign((const char*)AffairCallBack.m_pRecvData, AffairCallBack.m_pRecvSize);
		RspMsg = AffairCallBack.m_pRecvData;
		if(NmcMsg.ParserGetMatrixRspMsg(RspMsg, strCache, MatrixInfoS, nRetCode, strRemark))
		{
			return NMC_PASER_RSP_FAILED;
		}
	}

	if(nRetCode)
	{
		return nRetCode;
	}
	
	return 0;
}
int NMCDeviceImpl::SetMatrix(enum en_nmc_operator_type operator_type, struct st_matrix_info *matrix_info)
{
	NmcMessage NmcMsg;
	string ReqMsg;
	char cCookie[65] = {0};
	sprintf(cCookie, "%ld", (long)this);
	int Seq = NmcMsg.BuildSetMatrixReqMsg(operator_type, matrix_info, cCookie, ReqMsg);
	if(Seq==-1)
	{
		return NMC_GEN_REQ_FAILED;
	}

	ST_AFFAIR_CALLBACK AffairCallBack((long)this);
	int res = DoRequest(m_Conn, ReqMsg.data(), ReqMsg.size()+1, &AffairCallBack, Seq, EX_NMC_SET_MATRIX_REQ);
	if(res)
	{
		return NMC_SEND_REQ_FAILED;
	}

	string RspMsg;
	if(AffairCallBack.m_pRecvData.size()==0)
		return NMC_RECEIVE_REQ_FAILED;
	RspMsg = AffairCallBack.m_pRecvData;

	std::string strCache;
	int nRetCode=-1;
	std::string strRemark;

	if(NmcMsg.ParserSetMatrixRspMsg(RspMsg, strCache, &operator_type, matrix_info, nRetCode, strRemark))
	{
		return NMC_PASER_RSP_FAILED;
	}

	if(nRetCode)
	{
		return nRetCode;
	}

	return 0;
}
int NMCDeviceImpl::GetOutput(int outputid, struct st_output_layout **ppoutput_layout, int *output_layout_cnt)
{
	NmcMessage NmcMsg;
	string ReqMsg;
	char cCookie[65] = {0};
	sprintf(cCookie, "%ld", (long)this);
	int Seq = NmcMsg.BuildGetOutputReqMsg(outputid, cCookie, ReqMsg);
	if(Seq==-1)
	{
		return NMC_GEN_REQ_FAILED;
	}

	ST_AFFAIR_CALLBACK AffairCallBack((long)this);
	int res = DoRequest(m_Conn, ReqMsg.data(), ReqMsg.size()+1, &AffairCallBack, Seq, EX_NMC_GET_OUTPUT_SCREEN_LAYOUT_REQ);
	if(res)
	{
		return NMC_SEND_REQ_FAILED;
	}

	string RspMsg;
	if(AffairCallBack.m_pRecvData.size()==0)
		return NMC_RECEIVE_REQ_FAILED;
	RspMsg = AffairCallBack.m_pRecvData;

	std::string strCache;
	int nRetCode=-1;
	std::string strRemark;
	if(NmcMsg.ParserGetOutputRspMsg(RspMsg, strCache, ppoutput_layout, output_layout_cnt, nRetCode, strRemark))
	{
		return NMC_PASER_RSP_FAILED;
	}

	if(nRetCode)
	{
		return nRetCode;
	}

	return 0;
}
int NMCDeviceImpl::SetOutput(struct st_output_layout *output_layout)
{
	NmcMessage NmcMsg;
	string ReqMsg;
	char cCookie[65] = {0};
	sprintf(cCookie, "%ld", (long)this);
	int Seq = NmcMsg.BuildSetOutputReqMsg(output_layout, cCookie, ReqMsg);
	if(Seq==-1)
	{
		return NMC_GEN_REQ_FAILED;
	}

	ST_AFFAIR_CALLBACK AffairCallBack((long)this);
	int res = DoRequest(m_Conn, ReqMsg.data(), ReqMsg.size()+1, &AffairCallBack, Seq, EX_NMC_SET_OUTPUT_SCREEN_LAYOUT_REQ);
	if(res)
	{
		return NMC_SEND_REQ_FAILED;
	}

	string RspMsg;
	if(AffairCallBack.m_pRecvData.size()==0)
		return NMC_RECEIVE_REQ_FAILED;
	RspMsg = AffairCallBack.m_pRecvData;

	std::string strCache;
	int nRetCode=-1;
	std::string strRemark;
	if(NmcMsg.ParserSetOutputRspMsg(RspMsg, strCache,output_layout, nRetCode, strRemark))
	{
		return NMC_PASER_RSP_FAILED;
	}

	if(nRetCode)
	{
		return nRetCode;
	}

	return 0;
}
int NMCDeviceImpl::SetWindowSignalSource(int windowtype, int windowid, int sourceid, int subsourceid)
{
	NmcMessage NmcMsg;
	string ReqMsg;
	char cCookie[65] = {0};
	sprintf(cCookie, "%ld", (long)this);
	int Seq = NmcMsg.BuildSetWindowSignalSourceReqMsg(windowtype, windowid, sourceid, subsourceid, cCookie, ReqMsg);
	if(Seq==-1)
	{
		return NMC_GEN_REQ_FAILED;
	}

	ST_AFFAIR_CALLBACK AffairCallBack((long)this);
	int res = DoRequest(m_Conn, ReqMsg.data(), ReqMsg.size()+1, &AffairCallBack, Seq, EX_NMC_SET_WINDOW_SIGNAL_SOURCE_REQ);
	if(res)
	{
		return NMC_SEND_REQ_FAILED;
	}

	string RspMsg;
	if(AffairCallBack.m_pRecvData.size()==0)
		return NMC_RECEIVE_REQ_FAILED;
	RspMsg = AffairCallBack.m_pRecvData;

	std::string strCache;
	int nRetCode=-1;
	std::string strRemark;
	if(NmcMsg.ParserSetWindowSignalSourceRspMsg(RspMsg, strCache, &windowtype, &windowid, nRetCode, strRemark))
	{
		return NMC_PASER_RSP_FAILED;
	}

	if(nRetCode)
	{
		return nRetCode;
	}

	return 0;
}
int NMCDeviceImpl::ClearWindowSignalSource(int windowtype, int outputid, int windowid)
{
	NmcMessage NmcMsg;
	string ReqMsg;
	char cCookie[65] = {0};
	sprintf(cCookie, "%ld", (long)this);
	int Seq = NmcMsg.BuildClearWindowSignalSourceReqMsg(windowtype, outputid, windowid, cCookie, ReqMsg);
	if(Seq==-1)
	{
		return NMC_GEN_REQ_FAILED;
	}

	ST_AFFAIR_CALLBACK AffairCallBack((long)this);
	int res = DoRequest(m_Conn, ReqMsg.data(), ReqMsg.size()+1, &AffairCallBack, Seq, EX_NMC_CLEAR_WINDOW_SIGNAL_SOURCE_REQ);
	if(res)
	{
		return NMC_SEND_REQ_FAILED;
	}

	string RspMsg;
	if(AffairCallBack.m_pRecvData.size()==0)
		return NMC_RECEIVE_REQ_FAILED;
	RspMsg = AffairCallBack.m_pRecvData;

	std::string strCache;
	int nRetCode=-1;
	std::string strRemark;
	if(NmcMsg.ParserClearWindowSignalSourceRspMsg(RspMsg, strCache, &windowtype, &outputid, &windowid, nRetCode, strRemark))
	{
		return NMC_PASER_RSP_FAILED;
	}

	if(nRetCode)
	{
		return nRetCode;
	}

	return 0;
}
int NMCDeviceImpl::GetLargeScreen(int screenid, struct st_large_screen_info **pplarge_screen_info, int *large_screen_info_cnt)
{
	NmcMessage NmcMsg;
	string ReqMsg;
	char cCookie[65] = {0};
	sprintf(cCookie, "%ld", (long)this);
	int Seq = NmcMsg.BuildGetLargeScreenReqMsg(screenid, cCookie, ReqMsg);
	if(Seq==-1)
	{
		return NMC_GEN_REQ_FAILED;
	}

	ST_AFFAIR_CALLBACK AffairCallBack((long)this);
	int res = DoRequest(m_Conn, ReqMsg.data(), ReqMsg.size()+1, &AffairCallBack, Seq, EX_NMC_GET_LARGE_SCREEN_REQ);
	if(res)
	{
		return NMC_SEND_REQ_FAILED;
	}

	string RspMsg;
	if(AffairCallBack.m_pRecvData.size()==0)
		return NMC_RECEIVE_REQ_FAILED;
	RspMsg = AffairCallBack.m_pRecvData;

	std::string strCache;
	int nRetCode=-1;
	std::string strRemark;
	if(NmcMsg.ParserGetLargeScreenRspMsg(RspMsg, strCache, pplarge_screen_info, large_screen_info_cnt, nRetCode, strRemark))
	{
		return NMC_PASER_RSP_FAILED;
	}

	if(nRetCode)
	{
		return nRetCode;
	}

	return 0;
}
int NMCDeviceImpl::AddLargeScreen(struct st_large_screen_info *plarge_screen_info)
{
	NmcMessage NmcMsg;
	string ReqMsg;
	char cCookie[65] = {0};
	sprintf(cCookie, "%ld", (long)this);
	int Seq = NmcMsg.BuildAddLargeScreenReqMsg(plarge_screen_info, cCookie, ReqMsg);
	if(Seq==-1)
	{
		return NMC_GEN_REQ_FAILED;
	}

	ST_AFFAIR_CALLBACK AffairCallBack((long)this);
	int res = DoRequest(m_Conn, ReqMsg.data(), ReqMsg.size()+1, &AffairCallBack, Seq, EX_NMC_ADD_LARGE_SCREEN_REQ);
	if(res)
	{
		return NMC_SEND_REQ_FAILED;
	}

	string RspMsg;
	if(AffairCallBack.m_pRecvData.size()==0)
		return NMC_RECEIVE_REQ_FAILED;
	RspMsg = AffairCallBack.m_pRecvData;

	std::string strCache;
	int nRetCode=-1;
	std::string strRemark;
	if(NmcMsg.ParserAddLargeScreenRspMsg(RspMsg, strCache, &plarge_screen_info->screenid, nRetCode, strRemark))
	{
		return NMC_PASER_RSP_FAILED;
	}

	if(nRetCode)
	{
		return nRetCode;
	}

	return 0;
}
int NMCDeviceImpl::RmvLargeScreen(int screenid)
{
	NmcMessage NmcMsg;
	string ReqMsg;
	char cCookie[65] = {0};
	sprintf(cCookie, "%ld", (long)this);
	int Seq = NmcMsg.BuildRmvLargeScreenReqMsg(screenid, cCookie, ReqMsg);
	if(Seq==-1)
	{
		return NMC_GEN_REQ_FAILED;
	}

	ST_AFFAIR_CALLBACK AffairCallBack((long)this);
	int res = DoRequest(m_Conn, ReqMsg.data(), ReqMsg.size()+1, &AffairCallBack, Seq, EX_NMC_RMV_LARGE_SCREEN_REQ);
	if(res)
	{
		return NMC_SEND_REQ_FAILED;
	}

	string RspMsg;
	if(AffairCallBack.m_pRecvData.size()==0)
		return NMC_RECEIVE_REQ_FAILED;
	RspMsg = AffairCallBack.m_pRecvData;

	std::string strCache;
	int nRetCode=-1;
	std::string strRemark;
	if(NmcMsg.ParserRmvLargeScreenRspMsg(RspMsg, strCache, screenid, nRetCode, strRemark))
	{
		return NMC_PASER_RSP_FAILED;
	}

	if(nRetCode)
	{
		return nRetCode;
	}

	return 0;
}
int NMCDeviceImpl::SetWindowOverlayInfo(int isshow, int wndyype, int wndid, int overlayyype)
{
	NmcMessage NmcMsg;
	string ReqMsg;
	char cCookie[65] = {0};
	sprintf(cCookie, "%ld", (long)this);
	int Seq = NmcMsg.BuildSetWindowOverlayInfoReqMsg(isshow, wndyype, wndid, overlayyype, cCookie, ReqMsg);
	if(Seq==-1)
	{
		return NMC_GEN_REQ_FAILED;
	}

	ST_AFFAIR_CALLBACK AffairCallBack((long)this);
	int res = DoRequest(m_Conn, ReqMsg.data(), ReqMsg.size()+1, &AffairCallBack, Seq, EX_NMC_SET_WINDOW_OVERLAY_INFO_REQ);
	if(res)
	{
		return NMC_SEND_REQ_FAILED;
	}

	string RspMsg;
	if(AffairCallBack.m_pRecvData.size()==0)
		return NMC_RECEIVE_REQ_FAILED;
	RspMsg = AffairCallBack.m_pRecvData;

	std::string strCache;
	int nRetCode=-1;
	std::string strRemark;
	if(NmcMsg.ParserSetWindowOverlayInfoRspMsg(RspMsg, strCache, wndyype, wndid, nRetCode, strRemark))
	{
		return NMC_PASER_RSP_FAILED;
	}

	if(nRetCode)
	{
		return nRetCode;
	}

	return 0;
}
int NMCDeviceImpl::FlipWindow(int wndtype, int wndid)
{
	NmcMessage NmcMsg;
	string ReqMsg;
	char cCookie[65] = {0};
	sprintf(cCookie, "%ld", (long)this);
	int Seq = NmcMsg.BuildFlipWindowReqMsg(wndtype, wndid, cCookie, ReqMsg);
	if(Seq==-1)
	{
		return NMC_GEN_REQ_FAILED;
	}

	ST_AFFAIR_CALLBACK AffairCallBack((long)this);
	int res = DoRequest(m_Conn, ReqMsg.data(), ReqMsg.size()+1, &AffairCallBack, Seq, EX_NMC_FLIP_WINDOW_REQ);
	if(res)
	{
		return NMC_SEND_REQ_FAILED;
	}

	string RspMsg;
	if(AffairCallBack.m_pRecvData.size()==0)
		return NMC_RECEIVE_REQ_FAILED;
	RspMsg = AffairCallBack.m_pRecvData;

	std::string strCache;
	int nRetCode=-1;
	std::string strRemark;
	if(NmcMsg.ParserFlipWindowRspMsg(RspMsg, strCache, wndtype, wndid, nRetCode, strRemark))
	{
		return NMC_PASER_RSP_FAILED;
	}

	if(nRetCode)
	{
		return nRetCode;
	}

	return 0;
}
int NMCDeviceImpl::FullScreenWindow(int isfullscreen, int wndtype, int wndid)
{
	NmcMessage NmcMsg;
	string ReqMsg;
	char cCookie[65] = {0};
	sprintf(cCookie, "%ld", (long)this);
	int Seq = NmcMsg.BuildFullscreenWindowReqMsg(isfullscreen, wndtype, wndid, cCookie, ReqMsg);
	if(Seq==-1)
	{
		return NMC_GEN_REQ_FAILED;
	}

	ST_AFFAIR_CALLBACK AffairCallBack((long)this);
	int res = DoRequest(m_Conn, ReqMsg.data(), ReqMsg.size()+1, &AffairCallBack, Seq, EX_NMC_FULLSCREEN_WINDOW_REQ);
	if(res)
	{
		return NMC_SEND_REQ_FAILED;
	}

	string RspMsg;
	if(AffairCallBack.m_pRecvData.size()==0)
		return NMC_RECEIVE_REQ_FAILED;
	RspMsg = AffairCallBack.m_pRecvData;

	std::string strCache;
	int nRetCode=-1;
	std::string strRemark;
	if(NmcMsg.ParserFullscreenWindowRspMsg(RspMsg, strCache, wndtype, wndid, nRetCode, strRemark))
	{
		return NMC_PASER_RSP_FAILED;
	}

	if(nRetCode)
	{
		return nRetCode;
	}

	return 0;
}
int NMCDeviceImpl::ShowScreenNum(int matrixid, int Type)
{
	NmcMessage NmcMsg;
	string ReqMsg;
	char cCookie[65] = {0};
	sprintf(cCookie, "%ld", (long)this);
	int Seq = NmcMsg.BuildShowScreenNumReqMsg(matrixid, Type, cCookie, ReqMsg);
	if(Seq==-1)
	{
		return NMC_GEN_REQ_FAILED;
	}

	ST_AFFAIR_CALLBACK AffairCallBack((long)this);
	int res = DoRequest(m_Conn, ReqMsg.data(), ReqMsg.size()+1, &AffairCallBack, Seq, EX_NMC_SHOW_SCRNUM_PIC_REQ);
	if(res)
	{
		return NMC_SEND_REQ_FAILED;
	}

	string RspMsg;
	if(AffairCallBack.m_pRecvData.size()==0)
		return NMC_RECEIVE_REQ_FAILED;
	RspMsg = AffairCallBack.m_pRecvData;

	std::string strCache;
	int nRetCode=-1;
	std::string strRemark;
	if(NmcMsg.ParserShowScreenNumRspMsg(RspMsg, strCache, nRetCode, strRemark))
	{
		return NMC_PASER_RSP_FAILED;
	}

	if(nRetCode)
	{
		return nRetCode;
	}

	return 0;
}
int NMCDeviceImpl::CreateSWWindow(int width, int height, struct st_create_sw_window_info *pinfo, int sw_window_info_cnt)
{
	NmcMessage NmcMsg;
	string ReqMsg;
	char cCookie[65] = {0};
	sprintf(cCookie, "%ld", (long)this);
	int Seq = NmcMsg.BuildCreateSWWindow(width, height, pinfo, sw_window_info_cnt, cCookie, ReqMsg);
	if(Seq==-1)
	{
		return NMC_GEN_REQ_FAILED;
	}

	ST_AFFAIR_CALLBACK AffairCallBack((long)this);
	int res = DoRequest(m_Conn, ReqMsg.data(), ReqMsg.size()+1, &AffairCallBack, Seq, EX_NMC_SW_CREATE_WINDOW_REQ);
	if(res)
	{
		return NMC_SEND_REQ_FAILED;
	}

	string RspMsg;
	if(AffairCallBack.m_pRecvData.size()==0)
		return NMC_RECEIVE_REQ_FAILED;
	RspMsg = AffairCallBack.m_pRecvData;

	std::string strCache;
	int nRetCode=-1;
	std::string strRemark;
	if(NmcMsg.ParserCreateSWWindow(RspMsg, strCache, &pinfo->sw_window_id, &pinfo->sw_sequence, nRetCode, strRemark))
	{
		return NMC_PASER_RSP_FAILED;
	}

	if(nRetCode)
	{
		if(nRetCode==-1)
			nRetCode = NMC_SERVER_RETURN_ERROR;

		return nRetCode;
	}

	return 0;
}
int NMCDeviceImpl::DeleteSWWindow(int *psw_window_id_list, int sw_window_id_list_len)
{
	NmcMessage NmcMsg;
	string ReqMsg;
	char cCookie[65] = {0};
	sprintf(cCookie, "%ld", (long)this);
	int Seq = NmcMsg.BuildDeleteSWWindow(psw_window_id_list, sw_window_id_list_len, cCookie, ReqMsg);
	if(Seq==-1)
	{
		return NMC_GEN_REQ_FAILED;
	}

	ST_AFFAIR_CALLBACK AffairCallBack((long)this);
	int res = DoRequest(m_Conn, ReqMsg.data(), ReqMsg.size()+1, &AffairCallBack, Seq, EX_NMC_SW_DEL_WINDOW_REQ);
	if(res)
	{
		return NMC_SEND_REQ_FAILED;
	}

	string RspMsg;
	if(AffairCallBack.m_pRecvData.size()==0)
		return NMC_RECEIVE_REQ_FAILED;
	RspMsg = AffairCallBack.m_pRecvData;

	std::string strCache;
	int nRetCode=-1;
	std::string strRemark;
	if(NmcMsg.ParserDeleteSWWindow(RspMsg, strCache, nRetCode, strRemark))
	{
		return NMC_PASER_RSP_FAILED;
	}

	if(nRetCode)
	{
		return nRetCode;
	}

	return 0;
}
int NMCDeviceImpl::StickSWWindow(int sw_window_id)
{
	NmcMessage NmcMsg;
	string ReqMsg;
	char cCookie[65] = {0};
	sprintf(cCookie, "%ld", (long)this);
	int Seq = NmcMsg.BuildStickSWWindow(sw_window_id, cCookie, ReqMsg);
	if(Seq==-1)
	{
		return NMC_GEN_REQ_FAILED;
	}

	ST_AFFAIR_CALLBACK AffairCallBack((long)this);
	int res = DoRequest(m_Conn, ReqMsg.data(), ReqMsg.size()+1, &AffairCallBack, Seq, EX_NMC_SW_STICK_WINDOW_REQ);
	if(res)
	{
		return NMC_SEND_REQ_FAILED;
	}

	string RspMsg;
	if(AffairCallBack.m_pRecvData.size()==0)
		return NMC_RECEIVE_REQ_FAILED;
	RspMsg = AffairCallBack.m_pRecvData;

	std::string strCache;
	int nRetCode=-1;
	std::string strRemark;
	if(NmcMsg.ParserStickSWWindow(RspMsg, strCache, nRetCode, strRemark))
	{
		return NMC_PASER_RSP_FAILED;
	}

	if(nRetCode)
	{
		return nRetCode;
	}

	return 0;
}
int NMCDeviceImpl::GetSWWindowInfo(int sw_window_id, struct st_sw_window_info** info, int *info_cnt)
{
	NmcMessage NmcMsg;
	string ReqMsg;
	char cCookie[65] = {0};
	sprintf(cCookie, "%ld", (long)this);
	int Seq = NmcMsg.BuildGetSWWindowInfo(sw_window_id, cCookie, ReqMsg);
	if(Seq==-1)
	{
		return NMC_GEN_REQ_FAILED;
	}

	ST_AFFAIR_CALLBACK AffairCallBack((long)this);
	int res = DoRequest(m_Conn, ReqMsg.data(), ReqMsg.size()+1, &AffairCallBack, Seq, EX_NMC_SW_GET_WIN_INFO_REQ);
	if(res)
	{
		return NMC_SEND_REQ_FAILED;
	}

	string RspMsg;
	if(AffairCallBack.m_pRecvData.size()==0)
		return NMC_RECEIVE_REQ_FAILED;
	RspMsg = AffairCallBack.m_pRecvData;

	std::string strCache;
	int nRetCode=-1;
	std::string strRemark;
	if(NmcMsg.ParserGetSWWindowInfo(RspMsg, strCache, info, info_cnt, nRetCode, strRemark))
	{
		return NMC_PASER_RSP_FAILED;
	}

	if(nRetCode)
	{
		return nRetCode;
	}

	return 0;
}

int NMCDeviceImpl::GetSurveyPlan(int survey_id, struct st_xy_survey_info **ppsurvey_info, int *survey_info_cnt)
{
	NmcMessage NmcMsg;
	string ReqMsg;
	char cCookie[65] = {0};
	sprintf(cCookie, "%ld", (long)this);
	int Seq = NmcMsg.BuildGetSurveyPlanReq(survey_id, cCookie, ReqMsg);
	if(Seq==-1)
	{
		return NMC_GEN_REQ_FAILED;
	}

	ST_AFFAIR_CALLBACK AffairCallBack((long)this);
	int res = DoRequest(m_Conn, ReqMsg.data(), ReqMsg.size()+1, &AffairCallBack, Seq, EX_NMC_GET_SURVEY_PLAN_REQ);
	if(res)
	{
		return NMC_SEND_REQ_FAILED;
	}

	string RspMsg;
	if(AffairCallBack.m_pRecvData.size()==0)
		return NMC_RECEIVE_REQ_FAILED;
	RspMsg = AffairCallBack.m_pRecvData;

	std::string strCache;
	int nRetCode=-1;
	std::string strRemark;
	if(NmcMsg.ParserGetSurveyPlanRsp(RspMsg, strCache, ppsurvey_info, survey_info_cnt, nRetCode, strRemark))
	{
		return NMC_PASER_RSP_FAILED;
	}

	if(nRetCode)
	{
		return nRetCode;
	}

	return 0;
}
int NMCDeviceImpl::SetSurveyPlan(enum en_nmc_operator_type operator_type, struct st_xy_survey_info *psurvey_info)
{
	NmcMessage NmcMsg;
	string ReqMsg;
	char cCookie[65] = {0};
	sprintf(cCookie, "%ld", (long)this);
	int Seq = NmcMsg.BuildSetSurveyPlanReq(operator_type, psurvey_info, cCookie, ReqMsg);
	if(Seq==-1)
	{
		return NMC_GEN_REQ_FAILED;
	}

	ST_AFFAIR_CALLBACK AffairCallBack((long)this);
	int res = DoRequest(m_Conn, ReqMsg.data(), ReqMsg.size()+1, &AffairCallBack, Seq, EX_NMC_SET_SURVEY_PLAN_REQ);
	if(res)
	{
		return NMC_SEND_REQ_FAILED;
	}

	string RspMsg;
	if(AffairCallBack.m_pRecvData.size()==0)
		return NMC_RECEIVE_REQ_FAILED;
	RspMsg = AffairCallBack.m_pRecvData;

	std::string strCache;
	int nRetCode=-1;
	std::string strRemark;
	if(NmcMsg.ParserSetSurveyPlanRsp(RspMsg, strCache, operator_type, psurvey_info, nRetCode, strRemark))
	{
		return NMC_PASER_RSP_FAILED;
	}

	if(nRetCode)
	{
		return nRetCode;
	}

	return 0;
}
int NMCDeviceImpl::CtrlWindowSurvey(int ctrl_type, int survey_id, int wndtype, int wndid)
{
	NmcMessage NmcMsg;
	string ReqMsg;
	char cCookie[65] = {0};
	sprintf(cCookie, "%ld", (long)this);
	int Seq = NmcMsg.BuildCtrlWindowSurveyReq(ctrl_type, survey_id, wndtype, wndid, cCookie, ReqMsg);
	if(Seq==-1)
	{
		return NMC_GEN_REQ_FAILED;
	}

	ST_AFFAIR_CALLBACK AffairCallBack((long)this);
	int res = DoRequest(m_Conn, ReqMsg.data(), ReqMsg.size()+1, &AffairCallBack, Seq, EX_NMC_SET_SURVEY_PLAN_REQ);
	if(res)
	{
		return NMC_SEND_REQ_FAILED;
	}

	string RspMsg;
	if(AffairCallBack.m_pRecvData.size()==0)
		return NMC_RECEIVE_REQ_FAILED;
	RspMsg = AffairCallBack.m_pRecvData;

	std::string strCache;
	int nRetCode=-1;
	std::string strRemark;
	if(NmcMsg.ParserCtrlWindowSurveyRsp(RspMsg, strCache, ctrl_type, survey_id, wndtype, wndid, nRetCode, strRemark))
	{
		return NMC_PASER_RSP_FAILED;
	}

	if(nRetCode)
	{
		return nRetCode;
	}

	return 0;
}
long NMCDeviceImpl::CreateUserStreamSource(void *arg)
{
	tr1::shared_ptr<PushStreamPeer> Peer(new PushStreamPeer(this));
	int res = Peer->CreateUserStreamSource(arg);
	if(res)
	{
		return 0;
	}
	
	m_PushStreamPeerS[(long)Peer.get()] = Peer;

	return (long)Peer.get();
}
int NMCDeviceImpl::DeleteUserStreamSource(long streamhandle)
{
	map<long, tr1::shared_ptr<PushStreamPeer> >::iterator iter = m_PushStreamPeerS.find(streamhandle);
	if(iter==m_PushStreamPeerS.end())
	{
		return NMC_INVALIED_HANDLE;
	}

	tr1::shared_ptr<PushStreamPeer> Peer = iter->second;

	m_PushStreamPeerS.erase(iter);

	return Peer->DeleteUserStreamSource();
}

int NMCDeviceImpl::StartPushUserStream(long streamhandle)
{
	map<long, tr1::shared_ptr<PushStreamPeer> >::iterator iter = m_PushStreamPeerS.find(streamhandle);
	if(iter==m_PushStreamPeerS.end())
	{
		return NMC_INVALIED_HANDLE;
	}

	return iter->second->StartPushUserStream(streamhandle);
}
int NMCDeviceImpl::StopPushUserStream(long streamhandle)
{
	map<long, tr1::shared_ptr<PushStreamPeer> >::iterator iter = m_PushStreamPeerS.find(streamhandle);
	if(iter==m_PushStreamPeerS.end())
	{
		return NMC_INVALIED_HANDLE;
	}
	
	return iter->second->StopPushUserStream(streamhandle);
}
int NMCDeviceImpl::GetPushUserStreamState(long streamhandle, struct st_user_stream_state **ppuser_stream_state, int *pstatecnt)
{
	int res = -1;
	if(streamhandle==0)
	{
		//返回所有的流状态
		int cnt = m_PushStreamPeerS.size();
		*pstatecnt = 0;
		*ppuser_stream_state = (struct st_user_stream_state *)calloc(cnt, sizeof(struct st_user_stream_state));

		for(map<long, tr1::shared_ptr<PushStreamPeer> >::iterator iter = m_PushStreamPeerS.begin(); iter!=m_PushStreamPeerS.end(); ++iter)
		{
			tr1::shared_ptr<PushStreamPeer> Peer_ = iter->second;
			struct PushStreamInfo Inf;
			res =  Peer_->GetUserStreamInfo(Inf);
			if(res==0)
			{
				(*ppuser_stream_state)[*pstatecnt].handle = (long)Peer_.get();
				strcpy((*ppuser_stream_state)[*pstatecnt].ip, Inf.strIp.c_str());
				(*ppuser_stream_state)[*pstatecnt].port = Inf.nPort;
				(*ppuser_stream_state)[*pstatecnt].state = Inf.nState;

				(*pstatecnt)++;
			}
		}
	}
	else
	{
		map<long, tr1::shared_ptr<PushStreamPeer> >::iterator iter = m_PushStreamPeerS.find(streamhandle);
		if(iter==m_PushStreamPeerS.end())
		{
			return NMC_INVALIED_HANDLE;
		}

		struct PushStreamInfo Inf;
		res =  iter->second->GetUserStreamInfo(Inf);
		if(res==0)
		{
			*pstatecnt = 1;
			*ppuser_stream_state = (struct st_user_stream_state *)calloc(1, sizeof(struct st_user_stream_state));
			(*ppuser_stream_state)->handle = streamhandle;
			strcpy((*ppuser_stream_state)->ip, Inf.strIp.c_str());
			(*ppuser_stream_state)->port = Inf.nPort;
			(*ppuser_stream_state)->state = Inf.nState;
		}
	}
	
	return 0;
}

int NMCDeviceImpl::SetUserStreamSourceInfo(long streamhandle, struct st_video_stream_info *info)
{
	map<long, tr1::shared_ptr<PushStreamPeer> >::iterator iter = m_PushStreamPeerS.find(streamhandle);
	if(iter==m_PushStreamPeerS.end())
	{
		return NMC_INVALIED_HANDLE;
	}

	return iter->second->SetUserStreamSourceInfo(info);
}
int NMCDeviceImpl::SetUserStreamSourceToWindow(long streamhandle, int windowtype, int windowid)
{
	map<long, tr1::shared_ptr<PushStreamPeer> >::iterator iter = m_PushStreamPeerS.find(streamhandle);
	if(iter==m_PushStreamPeerS.end())
	{
		return NMC_INVALIED_HANDLE;
	}
	
	return iter->second->CtrlPushStream(1, windowtype, windowid);
}
int NMCDeviceImpl::ClearUserStreamSourceFromWindow(long streamhandle, int windowtype, int windowid)
{
	map<long, tr1::shared_ptr<PushStreamPeer> >::iterator iter = m_PushStreamPeerS.find(streamhandle);
	if(iter==m_PushStreamPeerS.end())
	{
		return NMC_INVALIED_HANDLE;
	}

	return iter->second->CtrlPushStream(0, windowtype, windowid);
}
int NMCDeviceImpl::PushUserStreamVideo(long streamhandle, struct st_video_stream_info *info, char* pstreamdata, int streamdatalen)
{
	map<long, tr1::shared_ptr<PushStreamPeer> >::iterator iter = m_PushStreamPeerS.find(streamhandle);
	if(iter==m_PushStreamPeerS.end())
	{
		return NMC_INVALIED_HANDLE;
	}

	return iter->second->PushUserStreamVideo(info, pstreamdata, streamdatalen);
}
