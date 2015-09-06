#include "StdAfx.h"

#if (defined(WIN32) || defined(WIN64))
	#if (defined(WIN32))
	#pragma message("using windows i586 include file "__FILE__) 
	#else
	#pragma message("using windows x64 include file "__FILE__) 
	#endif
#else
	#include <arpa/inet.h>
	#ifndef _LP64
	#warning "using linux i586 include file"
	#else
	#warning "using linux x64 include file"
	#endif
#endif

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#include "JtEventServer.h"
#include "CachedAffair.h"
#include "JtEventConnPeer.h"
#include "JtEventListen.h"

JtEventListen::JtEventListen(int Port):m_Port(Port)
{
	listener = NULL;

	m_Sink = 0;
}
JtEventListen::~JtEventListen()
{
	
	
}
void JtEventListen::echo_read_cb(struct bufferevent *bev, void *ctx)
{
        /* This callback is invoked when there is data to read on bev. */
        struct evbuffer *input = bufferevent_get_input(bev);
        struct evbuffer *output = bufferevent_get_output(bev);

        /* Copy all the data from the input buffer to the output buffer. */
        evbuffer_add_buffer(output, input);
}

void JtEventListen::echo_event_cb(struct bufferevent *bev, short events, void *ctx)
{
        if (events & BEV_EVENT_ERROR)
                perror("Error from bufferevent");
        if (events & (BEV_EVENT_EOF | BEV_EVENT_ERROR)) {
                bufferevent_free(bev);
        }
}
void JtEventListen::accept_conn_cb(struct evconnlistener *listener,
    evutil_socket_t fd, struct sockaddr *address, int socklen,
    void *ctx)
{
	JtEventListen *Listen = (JtEventListen *)ctx;

	JtEventConnPeer *ConnPeer = new JtEventConnPeer(fd, address, socklen);
	Listen->AcceptConnCallback(ConnPeer);
	
	/* We got a new connection! Set up a bufferevent for it. */
    /*  
	struct event_base *base = evconnlistener_get_base(listener);
	struct bufferevent *bev = bufferevent_socket_new(
			base, fd, BEV_OPT_CLOSE_ON_FREE);

	bufferevent_setcb(bev, echo_read_cb, NULL, echo_event_cb, NULL);

	bufferevent_enable(bev, EV_READ|EV_WRITE);
	*/
}
void JtEventListen::AcceptConnCallback(JtEventConnPeer *ConnPeer)
{
	if(m_Sink)
		m_Sink->OnAccept(ConnPeer);
}

void JtEventListen::accept_error_cb(struct evconnlistener *listener, void *ctx)
{
	JtEventListen * Listen = (JtEventListen *)ctx;
    struct event_base *base = evconnlistener_get_base(listener);
    int err = EVUTIL_SOCKET_ERROR();

    fprintf(stderr, "Got an error %d (%s) on the listener. "
            "Shutting down.\n", err, evutil_socket_error_to_string(err));

	////////event_base_loopexit(base, NULL);
}

int JtEventListen::OnAddToServer(JtEventServer *Server)
{
	SetServer(Server);

	struct sockaddr_in sin;

	memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = htonl(0);
    sin.sin_port = htons(m_Port);

    listener = evconnlistener_new_bind(Server->GetBase(), accept_conn_cb, this,
		LEV_OPT_CLOSE_ON_FREE|LEV_OPT_REUSEABLE|LEV_OPT_CLOSE_ON_EXEC, -1,
        (struct sockaddr*)&sin, sizeof(sin));

    if (!listener) {
            perror("Couldn't create listener");
            return 1;
    }
    evconnlistener_set_error_cb(listener, accept_error_cb);

	return 0;
}
int JtEventListen::OnRemoveFromServer()
{

	return 0;
}
int JtEventListen::SetJtEventCallbackSink(JtListenEventCallbackSink *Sink, void* UserData)
{
	m_Sink = Sink;
	m_EventCallbackUserData = UserData;
	return 0;
}
int JtEventListen::SetJtEventListenPort(int Port)
{
	m_Port = Port;
}
