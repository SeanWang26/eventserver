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
}
int JtEventPairPipe::AddToServer(JtEventServer *EventServer)
{
	//pairbev[1]在loop中用于接收信令
	m_Sink = dynamic_cast<JtPairPipeEventCallbackSink*>(EventServer);
	struct event_base * ebase = EventServer->GetBase();
	if(bufferevent_pair_new(ebase, BEV_OPT_THREADSAFE, pairbev))
	{
		return -1;
	}
	//BEV_OPT_CLOSE_ON_FREE
	bufferevent_setcb(pairbev[1], Static_ReadCallback, NULL, Static_EventCallback, this);
	
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
			//jtprintf("[%s]bufferevent_write fail\n", __FUNCTION__);
			return -2;
		}
		//bufferevent_flush(pairbev[0], EV_WRITE, BEV_FLUSH);		
		return 0;
	}
	
    return -1;
}
/*
int JtEventPairPipe::SetJtEventCallbackSink(JtPairPipeEventCallbackSink *Sink)
{
	m_Sink = Sink;
	return 0;
}
*/