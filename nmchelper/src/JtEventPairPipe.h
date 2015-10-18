#ifndef JtEventPairPipe_H
#define JtEventPairPipe_H

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

#include "JtEventType.h"
#include "FramePkg.h"

class  JtPairPipeEventCallbackSink
{
public:
	virtual int OnRecvData(void* Cookie, unsigned char* pData, int dataLen)=0;
	virtual int OnStateChanged(void* Cookie)=0;///加参数。。。to do....
};

struct bufferevent;

class JtEventPairPipe : public OnGetFrameCallBack
{
private:

	struct bufferevent *pairbev[2]; //自身控制的管道
	CFramePkg<struct ST_ICMS_CMD_HEADER> m_FramePkg;

	virtual int OnGetFrame(unsigned char* Data, int len);

	//事件
	static void Static_EventCallback(struct bufferevent *bev, short events, void *ptr);
	void EventCallback(struct bufferevent *bev, short events);

	static void Static_ReadCallback(struct bufferevent *bev, void *ptr);
	void ReadCallback(struct bufferevent *bev);
	
	JtPairPipeEventCallbackSink *m_Sink;
public:
	JtEventPairPipe();
	virtual ~JtEventPairPipe(void);

	//virtual int OnAddToServer(JtEventServer *EventServer);
	//virtual int OnRemoveFromServer();
	//virtual const char* GetEventPeerType(){ return "JtEventListen"; }

	int AddToServer(JtEventServer *EventServer);

	//virtual int RemoveFromServer();
	//virtual const char* GetEventPeerType(){ return "JtEventPairPipe"; }

	int SendCmd(const char* pData,int dataLen);
	int SetJtEventCallbackSink(JtPairPipeEventCallbackSink *Sink);
};


#endif