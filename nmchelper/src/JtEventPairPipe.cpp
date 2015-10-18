#include "StdAfx.h"
#include "jtprintf.h"
#if (defined(WIN32) || defined(WIN64))
	#if (defined(WIN32))
	#include <event2/event-config.h>
	#include <event2/thread.h>
	#include <event2/util.h>
	#include <event2/event_struct.h>
	#include <event2/event.h>
	#include <event2/bufferevent.h>

	#pragma message("using windows i586 include file") 
	#else
	#pragma message("using windows x64 include file") 
	#endif

#include <process.h>
#else
#include <pthread.h>
#include <unistd.h>
	#ifndef _LP64
	#warning "using linux i586 include file"
	#include <i586/event2/event_struct.h>
	#include <i586/event2/thread.h>
	#include <i586/event2/event.h>
	#include <i586/event2/bufferevent.h>
	#else
	#warning "using linux x64 include file"
	#include <event2/thread.h>
	#include <event2/event_struct.h>
	#include <event2/event.h>
	#include <event2/bufferevent.h>
	#endif
#endif

#include "JtEventPairPipe.h"
#include "JtEventServer.h"

JtEventPairPipe::JtEventPairPipe()
{
	pairbev[0] = NULL;
	pairbev[1] = NULL;

	m_Sink = NULL;

	m_FramePkg.SetCallBack(this);
	m_FramePkg.Init();
}

JtEventPairPipe::~JtEventPairPipe()
{
	if(pairbev[0])
	{
		bufferevent_free(pairbev[0]);
		pairbev[0] = NULL;
	}
	
	if(pairbev[1])
	{
		//bufferevent_disable(pairbev[1], EV_READ|EV_WRITE);
		//bufferevent_free(pairbev[1]);
	}
}
int JtEventPairPipe::AddToServer(JtEventServer *EventServer)
{
	//pairbev[1]在loop中用于接收信令
	m_Sink = dynamic_cast<JtPairPipeEventCallbackSink*>(EventServer);
	//struct event_base * ebase = EventServer->GetBase();

#if 0
	//bufferevent_pair_new无法使loop阻塞
	if(bufferevent_pair_new(ebase, BEV_OPT_THREADSAFE, pairbev))
	{
		return -1;
	}
#else

	evutil_socket_t pair[2];
#if (defined(WIN32) || defined(WIN64))
	evutil_socketpair(AF_INET, SOCK_STREAM, 0, pair);
#else
	evutil_socketpair(AF_UNIX, SOCK_STREAM, 0, pair);
#endif

	jtprintf("[%s]bufferevent_setcb before1111111111, EventServer %p\n", __FUNCTION__, EventServer);
	pairbev[0] = bufferevent_socket_new(EventServer->GetBase(), pair[0], BEV_OPT_CLOSE_ON_FREE|BEV_OPT_THREADSAFE);
	pairbev[1] = bufferevent_socket_new(EventServer->GetBase(), pair[1], BEV_OPT_CLOSE_ON_FREE|BEV_OPT_THREADSAFE);

#endif

	jtprintf("[%s]bufferevent_setcb before %p, %p\n", __FUNCTION__, pairbev[0], pairbev[1]);
	bufferevent_setcb(pairbev[1], Static_ReadCallback, NULL, Static_EventCallback, this);
	jtprintf("[%s]bufferevent_setcb after\n", __FUNCTION__);
	
	if(bufferevent_enable(pairbev[1], EV_READ|EV_WRITE))
	{
		return -1;
	}

	return 0;
}
int JtEventPairPipe::OnGetFrame(unsigned char* Data, int len)
{
	if(m_Sink)
	{
		m_Sink->OnRecvData(this, Data, len);
	}
	return 0;
}
void JtEventPairPipe::Static_EventCallback(struct bufferevent *bev, short events, void *ptr)
{
	JtEventPairPipe *Self = (JtEventPairPipe *)ptr;
	Self->EventCallback(bev, events);
}

void JtEventPairPipe::EventCallback(struct bufferevent *bev, short events)
{
	//jtprintf("[%s]Enter, events %d\n", __FUNCTION__, events);

	if(events & BEV_EVENT_ERROR 
		|| events & BEV_EVENT_READING 
		|| events & BEV_EVENT_WRITING)
	{
		jtprintf("[%s]pairbev Enter, events %d, BEV_EVENT_ERROR\n", __FUNCTION__, events);
		if(m_Sink)
		{
			m_Sink->OnStateChanged(this);
		}
	}
	else
	{
		//jtprintf("[%s]Enter, events %d, unknown\n", __FUNCTION__, events);
	}
}
void JtEventPairPipe::Static_ReadCallback(struct bufferevent *bev, void *ptr)
{
	JtEventPairPipe *Self = (JtEventPairPipe *)ptr;
	Self->ReadCallback(bev);
}
void JtEventPairPipe::ReadCallback(struct bufferevent *bev)
{
	//jtprintf("[%s]Enter\n", __FUNCTION__);

	unsigned char tmp[512];
	size_t n;
	while (1) {
		n = bufferevent_read(bev, tmp, sizeof(tmp));
		if (n <= 0)
		{
			//jtprintf("[%s]No more data n %zu\n", __FUNCTION__, n);
			break; /* No more data. */
		}

		m_FramePkg.Receive(tmp, n);
	}
}
int JtEventPairPipe::SendCmd(const char* pData,int dataLen)
{
	if(pairbev[0])
	{
		if(bufferevent_write(pairbev[0], pData, dataLen))
		{
			jtprintf("[%s]bufferevent_write fail 11\n", __FUNCTION__);
			return -2;
		}
		//bufferevent_flush(pairbev[0], EV_WRITE, BEV_FLUSH);		
		return 0;
	}

	jtprintf("[%s]bufferevent_write fail 22\n", __FUNCTION__);
    return -1;
}
/*
int JtEventPairPipe::SetJtEventCallbackSink(JtPairPipeEventCallbackSink *Sink)
{
	m_Sink = Sink;
	return 0;
}
*/