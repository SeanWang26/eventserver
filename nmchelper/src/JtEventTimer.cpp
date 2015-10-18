#include "StdAfx.h"

#include <time.h>
#include "JtEventTimer.h"
#include "CachedAffair.h"
#include "jtprintf.h"

JtEventTimer::JtEventTimer()
{
}

JtEventTimer::~JtEventTimer()
{
	//if(timeout)
	//{
		event_del(&timeout);
		///event_free(&timeout); //作为定时器并未分配内存的
		//timeout = NULL;
	//}
}

void JtEventTimer::TimeoutCallBack(evutil_socket_t fd, short event, void *arg)
{	
	JtEventTimer *Self = (JtEventTimer *)arg;
	Self->Timeout(fd, event);
}

void JtEventTimer::Timeout(evutil_socket_t fd, short event)
{
	/*
	struct timeval newtime, difference, lasttime;
	double elapsed;	evutil_gettimeofday(&newtime, NULL);
	evutil_timersub(&newtime, &lasttime, &difference);
	elapsed = difference.tv_sec + (difference.tv_usec / 1.0e6);
	lasttime = newtime;
	*/

	///////////CCachedAffairMap::GetInstance()->CheckBeOverTime(time(0));
}

int JtEventTimer::OnAddToServer(JtEventServer *m_Server)
{
	event_assign(&timeout, m_Server->GetBase(), -1, EV_PERSIST, TimeoutCallBack, this);
	
	struct timeval tv;
	evutil_timerclear(&tv);
	tv.tv_sec = 1000;
	
	event_add(&timeout, &tv);
	return 0;
}

int JtEventTimer::OnRemoveFromServer()
{
	return 0;
}











