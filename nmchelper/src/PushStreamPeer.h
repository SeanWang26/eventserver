#ifndef PushStreamPeer_h
#define PushStreamPeer_h

#include "CachedAffair.h"
#include "JtEventConnect.h"
#include "NMCDeviceType.h"

typedef enum _STREAM_SESSION_TYPE
{
	STREAM_SESSION_TYPE_CTRL = 0,
	STREAM_SESSION_TYPE_RECV,
	STREAM_SESSION_TYPE_SEND
}STREAM_SESSION_TYPE;

typedef struct _StreamSessionHead
{
	int nSessionType;
	int nReserved[15];
}StreamSessionHead; //64byte

typedef struct _StreamHead
{
	int		nStreamType;
	int 	nFrameType;
	int 	nLength;
	int 	reserve[5];
}StreamHead; //32 byte

class NMCDeviceImpl;

class PushStreamPeer : public JtConnectEventCallbackSink
{
	NMCDeviceImpl*        m_Holder;
	JtEventConnect*       m_PushCmdConn;
	int                   m_Logined;
	JtStreamSender *      m_StreamSender;

	map<unsigned long long, tr1::shared_ptr<CCachedAffair> > m_cachedAffairMap;
	class CCachedAffairMapLock  m_lockCachedAffair;

	struct PushStreamInfo m_Inf;
	struct st_jn_equ m_jn_equ;

	int m_WindowType;
	int m_WindowId;

#if (defined(WIN32) || defined(WIN64))
	HANDLE m_hHeartBeatThreadEventHandle;
	HANDLE m_hHeartBeatThreadHandle;
	static unsigned int __stdcall  Static_HeartBeatThread(void *arg);
	void HeartBeatThread();
#else
	sem_t       m_hHeartBeatThreadEventHandle;
	pthread_t	m_nHeartBeatThreadId;
	static void* Static_HeartBeatThread(void* arg);
	void* HeartBeatThread();
#endif

	int HeartBeat();

	static int static_OnGotHeartBeatData(long cookie, unsigned char* pData, int dataLen);
	int OnGotHeartBeatData(unsigned char* pData, int dataLen);

	static int static_OnGotHeartBeatOverTime(long cookie);
	int OnGotHeartBeatOverTime();

	int DoRequest(JtEventConnect* Conn, const char* pData,int dataLen, struct ST_AFFAIR_CALLBACK* pAffairCallBack, int Seq, int Cmd);

	virtual int OnRecvData(void* Cookie, unsigned char* pData, int dataLen);
	virtual int OnConnected(void* Cookie);
	virtual int OnClosed(void* Cookie);
	virtual int OnReConnected(void* Cookie);
public:
	PushStreamPeer(NMCDeviceImpl* m_Holder);
	~PushStreamPeer(void);

	int GetUserStreamInfo(struct PushStreamInfo &m_Inf);

	int CreateUserStreamSource(void *arg);
	int DeleteUserStreamSource();
	int StartPushUserStream(long streamhandle);
	int StopPushUserStream(long streamhandle);
	int CtrlPushStream(int CtrlType, int windowtype, int windowid);
	int SetUserStreamSourceInfo(struct st_video_stream_info *info);
	int PushUserStreamVideo(struct st_video_stream_info *info, char* pstreamdata, int streamdatalen);
};

#endif