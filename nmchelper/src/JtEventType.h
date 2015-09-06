#ifndef JtEventType_h
#define JtEventType_h

#if (defined(WIN32) || defined(WIN64))
	#if (defined(WIN32))
	#pragma message("using windows i586 include file "__FILE__) 
	#else
	#pragma message("using windows x64 include file "__FILE__) 
	#endif
#else
	#ifndef _LP64
	#warning "using linux i586 include file"
	#else
	#warning "using linux x64 include file"
	#endif
#endif

enum
{
	JTEVENT_NOTIFY_STOP_LOOP = 1,//添加一个peer
	JTEVENT_NOTIFY_ADD_PEER,//添加一个peer
	JTEVENT_TEST_CMD,//测试信令
};

class JtEventServer;

class JtEventPeer
{
private:
	JtEventServer *m_Server;
	int m_Timeout;
protected:
	int SetServer(JtEventServer *Server)
	{
		m_Server = Server;
		return 0;
	}
public:
	JtEventPeer()
	{}
	JtEventServer *GetServer() { return m_Server; } 
	virtual ~JtEventPeer(){};

	virtual int OnAddToServer(JtEventServer *m_Server)=0;
	virtual int OnRemoveFromServer()=0;
	virtual const char* GetEventPeerType()=0;
	//void SetTimeOut(int Timeout) { m_Timeout=Timeout; } 
};





























#endif