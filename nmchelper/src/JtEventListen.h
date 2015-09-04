#ifndef JtEventListen_H
#define JtEventListen_H

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

#include <event2/listener.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>

#include <string>
using namespace std;

#include "JtEventType.h"

class JtEventConnPeer;
class  JtListenEventCallbackSink
{
public:
	virtual int OnAccept(JtEventConnPeer *ConnPeer)=0;
};

struct evconnlistener;

class JtEventListen : public JtEventPeer
{
private:
	string m_Ip;
	int m_Port;
	struct evconnlistener *listener;

	JtListenEventCallbackSink *m_Sink;
	void* m_EventCallbackUserData;

private:
	//可写
	//static void Static_WriteCallback(struct bufferevent *bev, short events, void *ptr);
	//void WriteCallback(struct bufferevent *bev, short events);

	//事件
	static void Static_EventCallback(struct bufferevent *bev, short events, void *ptr);
	void EventCallback(struct bufferevent *bev, short events);
	
	//可读
	static void Static_ReadCallback(struct bufferevent *bev, void *ptr);
	void ReadCallback(struct bufferevent *bev);

	static void echo_read_cb(struct bufferevent *bev, void *ctx);
	static void echo_event_cb(struct bufferevent *bev, short events, void *ctx);

	static void accept_conn_cb(struct evconnlistener *listener, evutil_socket_t fd, struct sockaddr *address, int socklen, void *ctx);
	void AcceptConnCallback(JtEventConnPeer *ConnPeer);

	static void accept_error_cb(struct evconnlistener *listener, void *ctx);


public:
	virtual int OnAddToServer(JtEventServer *m_Server);
	virtual int OnRemoveFromServer();
	virtual const char* GetEventPeerType(){ return "JtEventListen"; }

	JtEventListen(int Port);
	~JtEventListen();
	
	int SetJtEventCallbackSink(JtListenEventCallbackSink *Sink, void* UserData);
	int SetJtEventListenPort(int Port);
};

#endif