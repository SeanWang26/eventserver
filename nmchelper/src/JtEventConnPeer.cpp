#include "StdAfx.h"

#ifndef _WIN32
	#include <netinet/in.h>
#ifdef _XOPEN_SOURCE_EXTENDED
	#include <arpa/inet.h>
#endif
	#include <sys/socket.h>
	#include <unistd.h>
#endif

#include <stdio.h>
#include <errno.h>
#include "jtprintf.h"

#if (defined(WIN32) || defined(WIN64))
	#if (defined(WIN32))
	#pragma message("using windows i586 include file "__FILE__) 
	#include <event2/event_struct.h>
	#include <event2/bufferevent.h>
	#include <event2/event.h>
	#else
	#pragma message("using windows x64 include file "__FILE__)
	#endif
#else
	#ifndef _LP64
	#warning "using linux i586 include file "
	#include <i586/event2/event_struct.h>
	#include <i586/event2/bufferevent.h>
	#include <i586/event2/event.h>
	#else
	#warning "using linux x64 include file "
	#include <event2/event_struct.h>
	#include <event2/bufferevent.h>
	#include <event2/event.h>
	#endif
#endif

#include "NmcCmdDefine.h"
#include "JtEventConnPeer.h"

JtEventConnPeer::JtEventConnPeer(evutil_socket_t fd, struct sockaddr *address, int socklen) : JtEventPeer(), bev(NULL), 
								   m_Sink(NULL),m_EventCallbackUserData(NULL)
{
	struct sockaddr_in *address_in;
	address_in=(struct sockaddr_in *)address;
	m_Ip = inet_ntoa(address_in->sin_addr);
	m_fd=fd;
}

JtEventConnPeer::~JtEventConnPeer()
{
	DoDisconnect();
}

int JtEventConnPeer::OnAddToServer(JtEventServer *Server)
{
	SetServer(Server);

	m_FramePkg.SetCallBack(this);
	
    bev = bufferevent_socket_new(Server->GetBase(), -1, BEV_OPT_CLOSE_ON_FREE|BEV_OPT_THREADSAFE);//BEV_OPT_THREADSAFE
	if(!bev)//
	{
		//jtprintf("[%s]DoConnect bev %p\n", __FUNCTION__, bev);
		evutil_closesocket(m_fd);

		return -3;
	}

	//bufferevent_set_max_single_read(bev, 512);

	int res = bufferevent_setfd(bev, m_fd);
	//jtprintf("[%s]bufferevent_setfd res %d\n", __FUNCTION__, res);

    bufferevent_setcb(bev, Static_ReadCallback, NULL, Static_EventCallback, this);
	bufferevent_enable(bev, EV_READ|EV_WRITE);

	return 0;
}
int JtEventConnPeer::OnRemoveFromServer()
{
	return 0;
}

void JtEventConnPeer::Static_EventCallback(struct bufferevent *bev, short events, void *ptr)
{
	JtEventConnPeer *Self = (JtEventConnPeer *)ptr;
	Self->EventCallback(bev, events);
}

void JtEventConnPeer::EventCallback(struct bufferevent *bev, short events)
{
	//jtprintf("[%s]Enter, events %d\n", __FUNCTION__, events);
	
    if (events & BEV_EVENT_CONNECTED) {
		//jtprintf("[%s]Enter, events %d, BEV_EVENT_CONNECTED\n", __FUNCTION__, events);
    }
	else if(events & BEV_EVENT_ERROR 
		|| events & BEV_EVENT_READING 
		|| events & BEV_EVENT_WRITING)
	{
         /* An error occured while connecting. */
		// jtprintf("[%s]Enter, events %d, BEV_EVENT_ERROR\n", __FUNCTION__, events);
		 //m_EventCallback(, void* data, void** user);
		 DoDisconnect();

		 if(m_Sink)
			 m_Sink->OnJtEventConnPeerClosed(this);
    }
	else
	{
		//jtprintf("[%s]Enter, events %d, unknown\n", __FUNCTION__, events);
	}
}

void JtEventConnPeer::Static_ReadCallback(struct bufferevent *bev, void *ptr)
{
	JtEventConnPeer *Self = (JtEventConnPeer *)ptr;
	Self->ReadCallback(bev);
}

void JtEventConnPeer::ReadCallback(struct bufferevent *bev)
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

int JtEventConnPeer::OnGetFrame(unsigned char* Data, int len)
{
	if(m_Sink)
		m_Sink->OnJtEventConnPeerRecvData(this, Data, len);

	return 0;
}
/*void JtEventConnPeer::Static_WriteCallback(struct bufferevent *bev, short events, void *ptr)
{
	JtEventConnPeer *Self = (JtEventConnPeer *)ptr;
	Self->WriteCallback(bev, events);
}

void JtEventConnPeer::WriteCallback(struct bufferevent *bev, short events)
{
	
}*/

/*
int JtEventConnPeer::DoConnect(string Ip, uint16_t Port, int TimeOut)
{
	m_FramePkg.SetCallBack(this);
	
    bev = bufferevent_socket_new(GetServer()->GetBase(), -1, BEV_OPT_CLOSE_ON_FREE|BEV_OPT_THREADSAFE);//BEV_OPT_THREADSAFE
	if(!bev)//
	{
		return -3;
	}

	//bufferevent_set_max_single_read(bev, 512);

	////////////////////////////res = bufferevent_setfd(bev, sock);
	//jtprintf("[%s]bufferevent_setfd res %d\n", __FUNCTION__, res);

    bufferevent_setcb(bev, Static_ReadCallback, NULL, Static_EventCallback, this);
	bufferevent_enable(bev, EV_READ|EV_WRITE);

	if(m_Sink)
	{
		m_Sink->OnJtEventConnPeerConnected(this);
	}

	m_FramePkg.Init();

	return 0;
}
*/
int JtEventConnPeer::DoDisconnectInner(int cmd, int seq)
{
	if(bev)
	{
		bufferevent_disable(bev, EV_READ|EV_WRITE);
		//bufferevent_setcb(bev, NULL, NULL, NULL, this);
		bufferevent_free(bev);
		bev=0;
	}

	GetServer()->RmvPeer(this);

	return 0;
}

int JtEventConnPeer::DoDisconnect()
{
	ExCommand Head;
	Head.nSrcType				= 0;
	Head.nCmdType				= JTEVENTCONNPEER_DISCONNECT;
	Head.nCmdSeq				= GetServer()->GenSeq();
	Head.nContentSize			= 0;

	AsyncEvent Event(std::bind(&JtEventConnPeer::DoDisconnectInner,this,std::placeholders::_1,std::placeholders::_2), Head.nCmdType, Head.nCmdSeq,this);
	int res = GetServer()->DoInSync(Event);

	/*
	if(bev)
	{
		bufferevent_disable(bev, EV_READ|EV_WRITE);
		//bufferevent_setcb(bev, NULL, NULL, NULL, this);
		bufferevent_free(bev);
		bev=0;
	}
	*/

	return res;
}

//int JtEventConnPeer::DoSend(uint8_t *Data, uint32_t Len)
//{
//	return bufferevent_write(bev, Data, Len);
//}

int JtEventConnPeer::SendData(const char* pData,int dataLen)
{
	if(bev)
	{
		if(bufferevent_write(bev, pData, dataLen))
		{
			//jtprintf("[%s]bufferevent_write fail\n", __FUNCTION__);
			return -2;
		}
				
		return 0;
	}
	
    return -1;
}
int JtEventConnPeer::SetJtEventCallbackSink(JtEventConnPeerCallbackSink *Sink, void* UserData)
{
	m_Sink = Sink;
	m_EventCallbackUserData = UserData;
		
	return 0;
}
int JtEventConnPeer::TestCmd()
{
	ExCommand Head;
	Head.nSrcType				= 0;
	Head.nCmdType				= 11;//JTEVENT_TEST_CMD;
	Head.nCmdSeq				= GetServer()->GenSeq();
	Head.nContentSize			= 0;

	return SendData((const char*)&Head,sizeof(Head));
}

