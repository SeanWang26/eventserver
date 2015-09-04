#include "StdAfx.h"
#include <assert.h>
#include <errno.h>

#include "jtprintf.h"
#include "NmcErrorCode.h"
#include "NmcCmdDefine.h"
#include "NmcMessage.h"
#include "NMCDeviceImpl.h"
#include "PushStreamPeer.h"

PushStreamPeer::PushStreamPeer(NMCDeviceImpl* Holder)
{
	m_Holder = Holder;
	m_PushCmdConn = NULL;
	m_StreamSender = NULL;

	m_WindowType = 0;
	m_WindowId = 0;

#if (defined(WIN32) || defined(WIN64))
	m_hHeartBeatThreadEventHandle = NULL;
	m_hHeartBeatThreadHandle = (HANDLE)-1L;
#else
	m_nHeartBeatThreadId = -1;
#endif

	memset(&m_jn_equ, 0, sizeof(m_jn_equ));
	m_Logined = 0;
/*
	srand(time(0));

	//要原子的 todo..............
	while(PushStreamHandleToken==0)
		PushStreamHandleToken = rand();
*/
}
PushStreamPeer::~PushStreamPeer(void)
{
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

	if(m_PushCmdConn)
	{
		m_PushCmdConn->DoDisconnect();
		delete m_PushCmdConn;
		m_PushCmdConn = NULL;
	}

	if(m_StreamSender)
	{
		m_StreamSender->DisConnect();
		delete m_StreamSender;
		m_StreamSender = NULL;
	}


}
#if (defined(WIN32) || defined(WIN64))
unsigned int PushStreamPeer::Static_HeartBeatThread(void *arg)
{
	PushStreamPeer *Self = (PushStreamPeer *)arg;
	Self->HeartBeatThread();
	_endthread();

	return 0;
}
void PushStreamPeer::HeartBeatThread()
{
	DWORD res = 0;
	while(m_Logined)
	{
		res = WaitForSingleObject(m_hHeartBeatThreadEventHandle, 5000);
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
void* PushStreamPeer::Static_HeartBeatThread(void* arg)
{
	NMCDeviceImpl *DeviceImpl = (NMCDeviceImpl *)arg;
	return DeviceImpl->HeartBeatThread();
}
void* PushStreamPeer::HeartBeatThread()
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
int PushStreamPeer::HeartBeat()
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
	int res = DoRequest(m_PushCmdConn, ReqMsg.data(), ReqMsg.size()+1, &AffairCallBack, Seq, EX_HEART_BEAT_REQ);
	if(res)
	{
		return NMC_SEND_REQ_FAILED;
	}

	return 0;
}
int PushStreamPeer::static_OnGotHeartBeatData(long cookie, unsigned char* pData, int dataLen)
{
	NMCDeviceImpl* DeviceImpl = (NMCDeviceImpl*)cookie;
	return DeviceImpl->OnGotHeartBeatData(pData, dataLen);
}
int PushStreamPeer::OnGotHeartBeatData(unsigned char* pData,int dataLen)
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
int PushStreamPeer::static_OnGotHeartBeatOverTime(long cookie)
{
	NMCDeviceImpl* DeviceImpl = (NMCDeviceImpl*)cookie;
	return DeviceImpl->OnGotHeartBeatOverTime();
}
int PushStreamPeer::OnGotHeartBeatOverTime()
{

	return 0;
}
int PushStreamPeer::OnRecvData(void* Cookie, unsigned char* pData, int dataLen)
{
	if(Cookie==m_PushCmdConn)
	{
		CCachedAffairMap::Static_CheckBeExptedData(m_cachedAffairMap, m_lockCachedAffair,(unsigned char*)pData,(long)dataLen);
	}

	return 0;
}
int PushStreamPeer::OnConnected(void* Cookie)
{
	return 0;
}
int PushStreamPeer::OnClosed(void* Cookie)
{
	if(Cookie==m_PushCmdConn)
	{
		if(m_Holder->m_Logined && m_Holder->m_pstatus_callback)
			m_Holder->m_pstatus_callback((long)this, NMC_PUSH_STREAM_DISCONNECT, NULL, 0, m_Holder->m_puserdata);
	}

	return 0;
}
int PushStreamPeer::OnReConnected(void* Cookie)
{
	return 0;
}
int PushStreamPeer::DoRequest(JtEventConnect* Conn, const char* pData,int dataLen, struct ST_AFFAIR_CALLBACK* pAffairCallBack, int Seq, int Cmd)
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
int PushStreamPeer::CreateUserStreamSource(void *arg)
{
	//创建信令连接
	if(m_PushCmdConn==NULL)
	{
		m_PushCmdConn = new JtEventConnect();
		m_PushCmdConn->SetJtEventCallbackSink(this, this);
		
		//int res = m_PushCmdConn->DoConnect("192.168.3.127", 41000, 5000);
		int res = m_PushCmdConn->DoConnect(m_Holder->m_LoginInfo.ip, 41000, 5000);
		if(res)
		{
			delete m_PushCmdConn;
			m_PushCmdConn = NULL;
			return NMC_CONNECT_FAILED;
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
	}

	//发送创建请求
	NmcMessage NmcMsg;
	string ReqMsg;
	char cCookie[65] = {0};
	sprintf(cCookie, "%ld", (long)this);
	int Seq = NmcMsg.BuildCreateUserStreamSourceReq(cCookie, ReqMsg);
	if(Seq==-1)
	{
		return NMC_GEN_REQ_FAILED;
	}

	ST_AFFAIR_CALLBACK AffairCallBack((long)this, NULL, NULL, 5000);
	int res = DoRequest(m_PushCmdConn, ReqMsg.data(), ReqMsg.size()+1, &AffairCallBack, Seq, EX_PREPARE_PUSH_STREAM_REQ);
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
	if(NmcMsg.ParserCreateUserStreamSourceRsp(RspMsg, strCache, m_Inf, nRetCode, strRemark))
	{
		return NMC_PASER_RSP_FAILED;
	}

	if(nRetCode)
	{
		return nRetCode;
	}
	
	//添加一个推流模式的设备
	m_jn_equ.EquId = -1;
	strcpy(m_jn_equ.FactoryName, "PS");
	static int nOpSeq = 0;
	char cEquName[64] = "";
	static int Handle = time(0);
	sprintf(cEquName, "PS-%ld", Handle++);
	m_jn_equ.nOpSeq = nOpSeq++;
	strcpy(m_jn_equ.EquName, cEquName);

	strcpy(m_jn_equ.IP, m_Inf.strIp.c_str());
	m_jn_equ.Port = m_Inf.nPort;
	strcpy(m_jn_equ.UserName, m_Inf.strUser.c_str());
	strcpy(m_jn_equ.Password, m_Inf.strPassword.c_str());
	m_jn_equ.ChannelNum = 1; //默认为1
	m_jn_equ.InputNum = 0;   //默认为0
	m_jn_equ.OutputNum = 0;  //默认为0
	m_jn_equ.nOpType = 0;    //默认为0==OP_TYPE_ADD
	m_jn_equ.nCtlgId = 1;    //默认为1
	m_jn_equ.nOpResult = -1;
	m_jn_equ.Extend[0] = 0;
	m_jn_equ.stSubEqu = NULL;
	m_jn_equ.stSubEquCnt = 0;
	res = m_Holder->AddSignalSource(&m_jn_equ);
	if(res<0)
	{
		//添加失败， 处理 todo....
		//DeleteUserStreamSource  .....

		return res;
	}

	m_Inf.nState = 1;

	if(m_jn_equ.nOpResult<0)
	{
		if(m_jn_equ.nOpResult==-1)
			return NMC_SERVER_RETURN_ERROR;

		return m_jn_equ.nOpResult;
	}

	m_Inf.jn_equ_id = m_jn_equ.EquId;
	m_Inf.jn_equ_sub_id = m_jn_equ.stSubEqu[0].SubEquId;

	return 0;
}
int PushStreamPeer::DeleteUserStreamSource()
{
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

	m_PushCmdConn->DoDisconnect();
	m_StreamSender->DisConnect();

	if(m_jn_equ.stSubEquCnt<=0)
	{
		return 0;
	}

	int res = m_Holder->RemoveSignalSource(m_jn_equ.stSubEqu[0].SubEquId);
	if(res)
	{
		jtprintf("[DeleteUserStreamSource]RemoveSignalSource()\n");   
	}

	free(m_jn_equ.stSubEqu);
	m_jn_equ.stSubEqu = NULL;
	m_jn_equ.stSubEquCnt = 0;
	memset(&m_jn_equ, 0, sizeof(m_jn_equ));

	return 0;
}
int PushStreamPeer::StartPushUserStream(long streamhandle)
{
	if(m_Inf.nState == 2)
	{
		return NMC_ERROR_CALL_ORDER;
	}

	//创建一个流句柄
	if(m_StreamSender==NULL)
	{
		m_StreamSender = new JtStreamSender();
		m_StreamSender->SetRemoteAddr(m_Inf.strIp, m_Inf.nPort);
	}
	
	int res =m_StreamSender->DoConnect();
	if(res)
	{
		jtprintf("[PushUserStreamVideo]m_StreamSender->DoConnect()\n");   
		return 0;
	}

	m_Inf.nState = 2; //状态转为推流态

	StreamSessionHead SessionHead;
	memset(&SessionHead, 0 , sizeof(SessionHead));
	SessionHead.nSessionType = STREAM_SESSION_TYPE_RECV;
	res = m_StreamSender->SendData((char*)&SessionHead, sizeof(SessionHead));
	if(res)
	{
		return NMC_SEND_DATA_FAILED;
	}

	NmcMessage NmcMsg;
	string ReqMsg;
	char cCookie[65] = {0};
	sprintf(cCookie, "%ld", (long)this);
	string strLocalIp;
	int nLocalPort;
	res = m_StreamSender->GetLocalAddr(strLocalIp, nLocalPort);
	if(res)
	{
		return NMC_RECEIVE_DATA_FAILED;
	}

	int Seq = NmcMsg.BuildCtrlPushStreamReq(cCookie, 1, strLocalIp, nLocalPort, m_Inf, ReqMsg);
	if(Seq==-1)
	{
		return NMC_GEN_REQ_FAILED;
	}

	ST_AFFAIR_CALLBACK AffairCallBack((long)this, NULL, NULL, 5000);
	res = DoRequest(m_PushCmdConn, ReqMsg.data(), ReqMsg.size()+1, &AffairCallBack, Seq, EX_CTRL_PUSH_STREAM_REQ);
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
	if(NmcMsg.ParserCtrlPushStreamRsp(RspMsg, strCache, nRetCode, strRemark))
	{
		return NMC_PASER_RSP_FAILED;
	}

	if(nRetCode)
	{
		return nRetCode;
	}

	return 0;
}
int PushStreamPeer::StopPushUserStream(long streamhandle)
{
	if(m_Inf.nState!=2)
	{
		return NMC_ERROR_CALL_ORDER;
	}

	if(m_StreamSender==NULL)
	{
		return NMC_ERROR_CALL_ORDER;
	}

	m_StreamSender->DisConnect();
	
	m_Inf.nState = 1;  //状态转为推流停止态

	NmcMessage NmcMsg;
	string ReqMsg;
	char cCookie[65] = {0};
	sprintf(cCookie, "%ld", (long)this);
	string strLocalIp;
	int nLocalPort;
	m_StreamSender->GetLocalAddr(strLocalIp, nLocalPort);
	int Seq = NmcMsg.BuildCtrlPushStreamReq(cCookie, 0, strLocalIp, nLocalPort, m_Inf, ReqMsg);
	if(Seq==-1)
	{
		return NMC_GEN_REQ_FAILED;
	}

	ST_AFFAIR_CALLBACK AffairCallBack((long)this, NULL, NULL, 5000);
	int res = DoRequest(m_PushCmdConn, ReqMsg.data(), ReqMsg.size()+1, &AffairCallBack, Seq, EX_CTRL_PUSH_STREAM_REQ);
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
	if(NmcMsg.ParserCtrlPushStreamRsp(RspMsg, strCache, nRetCode, strRemark))
	{
		return NMC_PASER_RSP_FAILED;
	}

	if(nRetCode)
	{
		return nRetCode;
	}

	return 0;
}
int PushStreamPeer::GetUserStreamInfo(struct PushStreamInfo &Inf)
{
	Inf = m_Inf;
	return 0;
}
int PushStreamPeer::SetUserStreamSourceInfo(struct st_video_stream_info *info)
{
	NmcMessage NmcMsg;
	string ReqMsg;
	char cCookie[65] = {0};
	sprintf(cCookie, "%ld", (long)this);
	int Seq = NmcMsg.BuildSetUserStreamSourceInfoReq(cCookie, m_Inf, info, ReqMsg);
	if(Seq==-1)
	{
		return NMC_GEN_REQ_FAILED;
	}

	ST_AFFAIR_CALLBACK AffairCallBack((long)this);
	int res = DoRequest(m_PushCmdConn, ReqMsg.data(), ReqMsg.size()+1, &AffairCallBack, Seq, EX_SET_STREAM_INFO_REQ);
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
	if(NmcMsg.ParserSetUserStreamSourceInfoRsp(RspMsg, strCache, nRetCode, strRemark))
	{
		return NMC_PASER_RSP_FAILED;
	}

	if(nRetCode)
	{
		return nRetCode;
	}

	return 0;
}
int PushStreamPeer::CtrlPushStream(int CtrlType, int windowtype, int windowid)
{
	int res = NMC_ERROR_UNKNOWN;
	if(CtrlType==0)
	{
		//先停止显示
		res = m_Holder->ClearWindowSignalSource(windowtype, 0, windowid);//窗口停止显示
		if(res)
		{
			//return res;
		}
	}
	else if(CtrlType && m_jn_equ.stSubEquCnt)
	{
		//在窗口上显示改信号
		m_WindowType = windowtype;
		m_WindowId = windowid;
		res = m_Holder->SetWindowSignalSource(windowtype, windowid, 1, m_jn_equ.stSubEqu[0].SubEquId);
		if(res)
		{
			return res;
		}
	}

	return 0;
}

int PushStreamPeer::PushUserStreamVideo(struct st_video_stream_info *info, char* pstreamdata, int streamdatalen)
{
	int res = -1;

	StreamHead Head;
	Head.nStreamType = 0;

	if(info->frametype==1)
		Head.nFrameType = 1;
	else
	{
		Head.nFrameType = 0;
	}

	Head.nLength = streamdatalen;
	Head.reserve[0]=0;

#if (defined(WIN32) || defined(WIN64))
	char temp[256] = {0};
	sprintf(temp, "SendData to %s, %d, %d\r\n", m_Inf.strIp.c_str(), m_Inf.nPort, info->res[0]);
	OutputDebugString(temp);
#else
	
#endif	

	res = m_StreamSender->SendData((char*)&Head, sizeof(Head));
	if(res)
	{
#if (defined(WIN32) || defined(WIN64))
		sprintf(temp, " m_StreamSender->SendData failed\r\n");
		OutputDebugString(temp);
#else
	
#endif
		return NMC_SEND_DATA_FAILED;
	}

	res = m_StreamSender->SendData(pstreamdata, streamdatalen);
	if(res)
	{
#if (defined(WIN32) || defined(WIN64))
		sprintf(temp, " m_StreamSender->SendData failed\r\n");
		OutputDebugString(temp);
#else
	
#endif
		return NMC_SEND_DATA_FAILED;
	}

	return 0;
}