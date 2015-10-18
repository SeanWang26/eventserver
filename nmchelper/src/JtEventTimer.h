#ifndef JTEVENTTIMER_H
#define JTEVENTTIMER_H

#if (defined(WIN32) || defined(WIN64))
	#if (defined(WIN32))
	#pragma message("using windows i586 include file") 
	#include <event2/event_struct.h>
	#include <event2/event.h>
	#else
	#pragma message("using windows x64 include file") 
	#endif
#else
	#ifndef _LP64
	#include <i586/event2/event_struct.h>
	#include <i586/event2/event.h>
	#warning "using linux i586 include file"
	#else
	#warning "using linux x64 include file"
	#include <event2/event_struct.h>
	#include <event2/event.h>	
	#endif
#endif

#include "JtEventType.h"
#include "JtEventServer.h"

class  JtTimerEventCallbackSink
{
public:
	virtual int OnTime(void* Cookie)=0;
};

class JtEventTimer : public JtEventPeer
{
private:
	struct event timeout;
private:
	static void TimeoutCallBack(evutil_socket_t fd, short event, void *arg);
	void Timeout(evutil_socket_t fd, short event);
public:
	JtEventTimer();
	~JtEventTimer();

	virtual int OnAddToServer(JtEventServer *m_Server);
	virtual int OnRemoveFromServer();
	virtual const char* GetEventPeerType(){ return "JtEventTimer"; }

};


#endif
