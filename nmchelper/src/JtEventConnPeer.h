#ifndef JtEventConnPeer_H
#define JtEventConnPeer_H

	#include <event2/event_struct.h>
	#include <event2/bufferevent.h>
	#include <event2/event.h>

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


#include <string>
using namespace std;

#include "CachedAffair.h"
#include "JtEventServer.h"
#include "FramePkg.h"

enum
{
	JTEVENTCONNPEER_DISCONNECT = 1,//断开
};

class  JtEventConnPeerCallbackSink
{
public:
	virtual int OnJtEventConnPeerRecvData(void* Cookie, unsigned char* pData, int dataLen)=0;
	virtual int OnJtEventConnPeerConnected(void* Cookie)=0;
	virtual int OnJtEventConnPeerClosed(void* Cookie)=0;
	virtual int OnJtEventConnPeerReConnected(void* Cookie)=0;
};

class JtEventConnPeer : public JtEventPeer, public OnGetFrameCallBack
{
private:
	evutil_socket_t m_fd;
	string m_Ip;
	struct bufferevent *bev;
	CFramePkg<struct ST_ICMS_CMD_HEADER> m_FramePkg;

	JtEventConnPeerCallbackSink *m_Sink;
	void* m_EventCallbackUserData;

private:
	virtual int OnGetFrame(unsigned char* Data, int len);

	//可写
	//static void Static_WriteCallback(struct bufferevent *bev, short events, void *ptr);
	//void WriteCallback(struct bufferevent *bev, short events);

	//事件
	static void Static_EventCallback(struct bufferevent *bev, short events, void *ptr);
	void EventCallback(struct bufferevent *bev, short events);
	
	//可读
	static void Static_ReadCallback(struct bufferevent *bev, void *ptr);
	void ReadCallback(struct bufferevent *bev);

	int DoDisconnectInner(int cmd, int seq);

public:
	virtual int OnAddToServer(JtEventServer *m_Server);
	virtual int OnRemoveFromServer();
	virtual const char* GetEventPeerType(){ return "JtEventConnect"; }

	JtEventConnPeer(evutil_socket_t fd, struct sockaddr *address, int socklen);
	~JtEventConnPeer();

	//int DoConnect(string Ip, uint16_t Port, int TimeOut);
	int DoDisconnect();
	
	int SendData(const char* pData,int dataLen);
	int SetJtEventCallbackSink(JtEventConnPeerCallbackSink *Sink, void* UserData);

	int TestCmd();
};


#endif