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
#include "JtEventConnect.h"
//#include "ExternalDefine.h"

JtEventConnect::JtEventConnect() : JtEventPeer(), bev(NULL), 
								   m_Sink(NULL),m_EventCallbackUserData(NULL)
{
	Connected = 0;
}

JtEventConnect::~JtEventConnect()
{
	jtprintf("[%s]this %p\n", __FUNCTION__, this);
	//DoDisconnect();
}

int JtEventConnect::OnAddToServer(JtEventServer *Server)
{
	jtprintf("[%s]enter %p\n", __FUNCTION__, this);
	
	SetServer(Server);

    bev = bufferevent_socket_new(Server->GetBase(), -1, BEV_OPT_CLOSE_ON_FREE|BEV_OPT_THREADSAFE);//BEV_OPT_THREADSAFE
	if(!bev)//
	{
		jtprintf("[%s]bufferevent_socket_new bev %p\n", __FUNCTION__, bev);
		evutil_closesocket(m_sock);
		return -3;
	}

	//bufferevent_set_max_single_read(bev, 512);

	//int res = 
	bufferevent_setfd(bev, m_sock);
	//jtprintf("[%s]bufferevent_setfd res %d\n", __FUNCTION__, res);

    bufferevent_setcb(bev, Static_ReadCallback, NULL, Static_EventCallback, this);
	bufferevent_enable(bev, EV_READ|EV_WRITE);

	Connected = 1;
	if(m_Sink)
	{
		m_Sink->OnConnected(this);
	}

	jtprintf("[%s]leave %p\n", __FUNCTION__, this);
	
	return 0;
}
int JtEventConnect::OnRemoveFromServer()
{
	return 0;
}

void JtEventConnect::Static_EventCallback(struct bufferevent *bev, short events, void *ptr)
{
	JtEventConnect *Self = (JtEventConnect *)ptr;
	Self->EventCallback(bev, events);
}

void JtEventConnect::EventCallback(struct bufferevent *bev, short events)
{
	jtprintf("[%s]Enter, events %d\n", __FUNCTION__, events);

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
			 m_Sink->OnClosed(this);
    }
	else
	{
		//jtprintf("[%s]Enter, events %d, unknown\n", __FUNCTION__, events);
	}
}

void JtEventConnect::Static_ReadCallback(struct bufferevent *bev, void *ptr)
{
	JtEventConnect *Self = (JtEventConnect *)ptr;
	Self->ReadCallback(bev);
}

void JtEventConnect::ReadCallback(struct bufferevent *bev)
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

int JtEventConnect::OnGetFrame(unsigned char* Data, int len)
{

	if(m_Sink)
		m_Sink->OnRecvData(this, Data, len);

	return 0;
}
/*void JtEventConnect::Static_WriteCallback(struct bufferevent *bev, short events, void *ptr)
{
	JtEventConnect *Self = (JtEventConnect *)ptr;
	Self->WriteCallback(bev, events);
}

void JtEventConnect::WriteCallback(struct bufferevent *bev, short events)
{
	
}*/
int JtEventConnect::DoConnect(string Ip, uint16_t Port, int TimeOut,JtEventServer *Server)
{
	SetServer(Server);
	int res = DoConnectInner(Ip, Port, TimeOut);
	if(res == 0)
	{
		jtprintf("[JtEventConnect::DoConnect]DoConnectInner ok %p\n", this);
		Server->AddPeer(this);
		jtprintf("[JtEventConnect::DoConnect]AddPeer after this %p\n", this);
	}
	else
	{
		jtprintf("[JtEventConnect::DoConnect]DoConnectInner failed res %d, this %p\n", res, this);
	}
	
	return res;
}
int JtEventConnect::DoConnectInner(string Ip, uint16_t Port, int TimeOut)
{
	//jtprintf("[%s]\n", __FUNCTION__);

	struct sockaddr_in sin;

	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_port = htons(Port);
#ifdef WIN32
    sin.sin_addr.S_un.S_addr = inet_addr(Ip.c_str());  
#else  
    sin.sin_addr.s_addr = inet_addr(Ip.c_str());  
#endif

	m_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	evutil_make_socket_nonblocking(m_sock);
	evutil_make_socket_closeonexec(m_sock);

#if (defined(WIN32) || defined(WIN64))

	connect(m_sock, (struct sockaddr*)&sin, sizeof(sin));

#else 
	if(connect(m_sock, (struct sockaddr*)&sin, sizeof(sin)))
	{
		//jtprintf("[%s]connect %s\n", __FUNCTION__, strerror(errno));
		if(errno != EINPROGRESS && errno != EWOULDBLOCK)
		{
			//printf("connect(%d) error: %s\n", sock, strerror(errno));
			close(m_sock);
	
			return -1;
		}
	}
#endif	

	struct timeval Timeout = {4,0};
	fd_set writefds;
	FD_ZERO(&writefds);
	FD_SET(m_sock,&writefds);
	unsigned numFds = m_sock;

	int res=0;
	while(((res=select(numFds+1, NULL, &writefds,  NULL, &Timeout))==-1)&&errno==EINTR);
	if(res==0)
	{
		//printf("connect(%d) error: %s\n", sock, strerror(errno));
#if (defined(WIN32) || defined(WIN64))
		closesocket(m_sock);
#else  
		close(m_sock);
#endif		
		m_sock=-1;
		return -1;
	}

	if (FD_ISSET(m_sock, &writefds))
	{
		//jtprintf("[%s]writefds select ok\n", __FUNCTION__);

		////////////////evutil_socket_geterror();
#if (defined(WIN32) || defined(WIN64))
		int error = 0;
		int len = sizeof (error);
		if(getsockopt(m_sock, SOL_SOCKET, SO_ERROR, (char*)&error, &len) < 0)
#else  
		int error = 0;
		socklen_t len = sizeof (error);
		if(getsockopt(m_sock, SOL_SOCKET, SO_ERROR, &error, &len) < 0)
#endif
		{
			//printf ("getsockopt fail,connected fail\n");
			evutil_closesocket(m_sock);
			m_sock=-1;
			return -1;
		}

		if (error == ETIMEDOUT)
		{
			evutil_closesocket(m_sock);
			m_sock=-1;
			jtprintf ("[%s]connected timeout\n", __FUNCTION__);
			return -1;
		}

		if(error == ECONNREFUSED)
		{
			jtprintf("[%s]No one listening on the remote address.\n", __FUNCTION__);
			evutil_closesocket(m_sock);
			m_sock=-1;
			return -1;
		}

		if(error)
		{
			jtprintf("[%s]\n", __FUNCTION__);
			evutil_closesocket(m_sock);
			m_sock=-1;
			return -1;
		}		

		goto connected;
	}
	else 
	{
		jtprintf("[%s]DoConnect select failed, %s\n", __FUNCTION__, strerror(errno));
		evutil_closesocket(m_sock);
		m_sock=-1;
		return -3;
	}

connected:
	//jtprintf("[%s]DoConnect select ok, sock %d\n", __FUNCTION__, sock);

	jtprintf("[JtEventConnect::DoConnectInner]ok\n");
	m_FramePkg.SetCallBack(this);
	m_FramePkg.Init();
	//BEV_OPT_DEFER_CALLBACKS

	//jtprintf("[%s]GetServer()->GetBase() %p\n", __FUNCTION__, GetServer()->GetBase());
	//sleep(10);
	/*
    bev = bufferevent_socket_new(GetServer()->GetBase(), -1, BEV_OPT_CLOSE_ON_FREE|BEV_OPT_THREADSAFE);//BEV_OPT_THREADSAFE
	if(!bev)//
	{
		//jtprintf("[%s]DoConnect bev %p\n", __FUNCTION__, bev);
		evutil_closesocket(sock);

		return -3;
	}

	//bufferevent_set_max_single_read(bev, 512);

	res = bufferevent_setfd(bev, sock);
	//jtprintf("[%s]bufferevent_setfd res %d\n", __FUNCTION__, res);

    bufferevent_setcb(bev, Static_ReadCallback, NULL, Static_EventCallback, this);
	bufferevent_enable(bev, EV_READ|EV_WRITE);

	if(m_Sink)
	{
		m_Sink->OnConnected(this);
	}
	*/
	return 0;
}
int JtEventConnect::DoDisconnectInner(int cmd, int seq)
{
	if(bev)
	{
		bufferevent_disable(bev, EV_READ|EV_WRITE);
		//bufferevent_setcb(bev, NULL, NULL, NULL, this);
		bufferevent_free(bev);
		bev=0;
	}

	GetServer()->RmvPeer(this);

	Connected = 0;

	jtprintf("[JtEventConnect::DoDisconnectInner]ok\n");
	return 0;
}
int JtEventConnect::DoDisconnect()
{
	int res = 0;
	if(Connected)
	{
		Connected = 0;
		
		if(GetServer()==NULL)
			return 0;

		ExCommand Head;
		Head.nSrcType				= 0;
		Head.nCmdType				= JTEVENTCONNECT_DISCONNECT;
		Head.nCmdSeq				= GetServer()->GenSeq();
		Head.nContentSize			= 0;

		AsyncEvent Event(std::bind(&JtEventConnect::DoDisconnectInner,this,std::placeholders::_1,std::placeholders::_2), Head.nCmdType, Head.nCmdSeq, this);

		jtprintf("[JtEventConnect::DoDisconnect]---this %p-------before\n", this);
		res = GetServer()->DoInSync(Event);
		jtprintf("[JtEventConnect::DoDisconnect]---this %p-------after res %d\n", this, res);
	}
	else
	{
		jtprintf("[JtEventConnect::DoDisconnect]---this %p-------Connected %d ??\n", this, Connected);
	}

	return res;
}

//int JtEventConnect::DoSend(uint8_t *Data, uint32_t Len)
//{
//	return bufferevent_write(bev, Data, Len);
//}

int JtEventConnect::SendDataInner(const char* pData,int dataLen)
{
	/*
	if(bev)
	{
		if(bufferevent_write(bev, pData, dataLen))
		{
			//jtprintf("[%s]bufferevent_write fail\n", __FUNCTION__);
			return -2;
		}

		return 0;
	}
	*/
	return 0;
}

int JtEventConnect::SendData(const char* pData,int dataLen)
{
	//要不要投递到线程呢 to do....
	/*
	ExCommand Head;
	Head.nSrcType				= 0;
	Head.nCmdType				= JTEVENTCONNECT_SENDDATA;
	Head.nCmdSeq				= GetServer()->GenSeq();
	Head.nContentSize			= 0;

	AsyncEvent Event(std::tr1::bind(&JtEventConnect::DoDisconnectInner,this,std::tr1::placeholders::_1,std::tr1::placeholders::_2), Head.nCmdType, Head.nCmdSeq);

	int res = GetServer()->DoInSync(Event);
	*/

	if(bev)
	{
		if(bufferevent_write(bev, pData, dataLen))
		{
			jtprintf("[%s]bufferevent_write fail\n", __FUNCTION__);
			return -2;
		}

		return 0;
	}

	jtprintf("[%s]bufferevent_write fail 222\n", __FUNCTION__);
    return -1;
}
int JtEventConnect::SetJtEventCallbackSink(JtConnectEventCallbackSink *Sink, void* UserData)
{
	m_Sink = Sink;
	m_EventCallbackUserData = UserData;

	return 0;
}
int JtEventConnect::TestCmd()
{
	ExCommand Head;
	Head.nSrcType				= 0;
	Head.nCmdType				= JTEVENT_TEST_CMD;
	Head.nCmdSeq				= GetServer()->GenSeq();
	Head.nContentSize			= 0;

	return SendData((const char*)&Head,sizeof(Head));
}
