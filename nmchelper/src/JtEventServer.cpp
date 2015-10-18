#include "StdAfx.h"
#include <string>
#include "JtEventPairPipe.h"

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

#include "jtprintf.h"

#include "NmcCmdDefine.h"
#include "JtEventServer.h"
#include "JtEventTimer.h"

#include "jtmutexautolock.h"

//static int JtEventServer_Seq=0;

JtEventServer::JtEventServer() : base(NULL), m_Started(0)
{
#if (defined(WIN32) || defined(WIN64))
		WSADATA wsa_data;	
		WSAStartup(0x0201, &wsa_data);
		evthread_use_windows_threads();
		tid = INVALID_HANDLE_VALUE;
#else
		evthread_use_pthreads();
		tid = -1;
#endif

	pEventPairPipe = NULL;
	m_Started = 0;
	InHandling = 0;
}
JtEventServer::~JtEventServer()
{
#ifdef _WIN32	
	//WSADATA wsa_data;	
	//WSAStartup(0x0201, &wsa_data);
#endif
}



#if (defined(WIN32) || defined(WIN64))
unsigned int JtEventServer::Static_StartInThread(void *arg)
{
	JtEventServer *Self = (JtEventServer *)arg;

	return Self->EventLoop();
}

#else

void* JtEventServer::Static_StartInThread(void *arg)
{
	JtEventServer *Self = (JtEventServer *)arg;
	Self->EventLoop();
	return NULL;
}
#endif

int JtEventServer::Started()
{
	m_Started = 1;
	return 0;
}

int JtEventServer::EventLoop()
{
	//默认存在一个定时器
	//////JtEventTimer *Timer = new JtEventTimer();
	//////Timer->OnAddToServer(this);

	pEventPairPipe = new JtEventPairPipe();
	pEventPairPipe->AddToServer(this);
	
	//jtprintf("[%s]event_base_dispatch before\n", __FUNCTION__);

	Started();

	//int res = event_base_loop(base,EVLOOP_NO_EXIT_ON_EMPTY);
	int res = event_base_dispatch(base);
	if(res==0)
	{
		jtprintf("[%s]event_base_dispatch out res %d\n", __FUNCTION__, res);
	}
	else if(res==-1)
	{
		jtprintf("[%s]event_base_dispatch error res %d\n", __FUNCTION__, res);
	}
	else if(res==1)
	{
		jtprintf("[%s]event_base_dispatch no event res %d\n", __FUNCTION__, res);
	}
	else
	{
		jtprintf("[%s]event_base_dispatch res %d\n", __FUNCTION__, res);
	}
	
	/////delete Timer;
	delete pEventPairPipe;
	pEventPairPipe = NULL;
	//必须先移除所有的事件，然后再调用event_base_free
	event_base_free(base);

	base = NULL;
	//

	return 0;
}
int JtEventServer::IsInThread()
{
#if (defined(WIN32) || defined(WIN64))
	return threadID==GetCurrentThreadId();
#else
	return pthread_equal(tid, pthread_self());
#endif
}
int JtEventServer::OnRecvData(void* Cookie, unsigned char* pData, int dataLen)
{
	if(Cookie==pEventPairPipe)
	{
		//信令
		//ExCommand *pHead = (ExCommand *)pData;

#if 1
		{
			//JtMutexAutoLock L(m_EventLock);
			m_EventLock.Lock();
			InHandling = 1;
			while(!m_AsynEventS.empty())
			{
				jtprintf("[%s]m_AsynEventS size %d\n", __FUNCTION__, m_AsynEventS.size());
				
				AsyncEvent _AsynEventS = m_AsynEventS.front();
				m_AsynEventS.pop_front();
				m_EventLock.Unlock();
				
				_AsynEventS.m_Functor(_AsynEventS.m_Cmd, _AsynEventS.m_Seq);
				
				ExCommand Head;
				Head.nSrcType = 0;
				Head.nCmdType = _AsynEventS.m_Cmd;
				Head.nCmdSeq = _AsynEventS.m_Seq;
				Head.nContentSize = 0;

				jtprintf("[%s]_AsynEventS.m_Seq %d\n", __FUNCTION__, _AsynEventS.m_Seq);
				
				CCachedAffairMap::Static_CheckBeExptedData(m_cachedMap, m_Lock
					, (unsigned char*)&Head, sizeof(Head));	

				m_EventLock.Lock();
			}
			InHandling = 0;
			m_EventLock.Unlock();
			/*
			else
			{
				return 0;
			}
			*/
		}
#else	
		AsyncEvent _AsynEventS;
		int leftcnt=0;
		{
			JtMutexAutoLock L(m_EventLock);
			if(!m_AsynEventS.empty())
			{
				_AsynEventS = m_AsynEventS.front();
				m_AsynEventS.pop_front();
			}
			else
			{
				return 0;
			}

			leftcnt = m_AsynEventS.size();
		}

		jtprintf("[%s]_AsynEventS.m_Seq %d, cookie %p, left %d\n"
			, __FUNCTION__, _AsynEventS.m_Seq, _AsynEventS.m_Cookie, leftcnt);
		
		_AsynEventS.m_Functor(_AsynEventS.m_Cmd, _AsynEventS.m_Seq);

		ExCommand Head;
		Head.nSrcType = 0;
		Head.nCmdType = _AsynEventS.m_Cmd;
		Head.nCmdSeq = _AsynEventS.m_Seq;
		Head.nContentSize = 0;

		CCachedAffairMap::Static_CheckBeExptedData(m_cachedMap, m_Lock
			, (unsigned char*)&Head, sizeof(Head));	
#endif

		//for (size_t i = 0; i < _AsynEventS.size(); ++i)
		//{
		/*	_AsynEventS.m_Functor(_AsynEventS.m_Cmd, _AsynEventS.m_Seq);
			
			ExCommand Head;
			Head.nSrcType = 0;
			Head.nCmdType = _AsynEventS.m_Cmd;
			Head.nCmdSeq = _AsynEventS.m_Seq;
			Head.nContentSize = 0;
			CCachedAffairMap::Static_CheckBeExptedData(m_cachedMap, m_Lock
				, (unsigned char*)&Head, sizeof(Head));*/
		//}
	}
	
	return 0;
}
int JtEventServer::OnStateChanged(void* Cookie)
{
	if(Cookie==pEventPairPipe)
	{
		//信令

	}

	return 0;
}
int JtEventServer::Start()
{
	if(!m_Started)
	{
		////////////m_Started = 1;
		
		base = event_base_new();
		if(!base)
		{
			jtprintf("[%s]event_base_new failed %p\n", __FUNCTION__, base);
			return -1;
		}

		//res = bufferevent_pair_new(base, int options,
		//struct bufferevent *pair[2]);
		int res;
#if (defined(WIN32) || defined(WIN64))
		tid = (HANDLE)_beginthreadex(NULL, 0, Static_StartInThread, this, 0, &threadID);
		if(tid)
		{
			
		}

		while(m_Started==0)
		{
			Sleep(500);
		}
#else
		//pthread_attr_t attr;
		//pthread_attr_init(&attr);
		//完成了，stop函数改为非PTHREAD_CREATE_DETACHED的
		//pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
		//res = pthread_create(&tid, &attr, Static_StartInThread, this);
		//pthread_attr_destroy(&attr);

		jtprintf("[%s]pthread_create\n", __FUNCTION__);
		res = pthread_create(&tid, NULL, Static_StartInThread, this);
		while(m_Started==0)
		{
			sleep(1);
		}
#endif
		return res;
	}
	
	return 0;
}
//static int JtEventServer_static_nCmdSeq = 0;
int JtEventServer::GenSeq()
{
	static long sJtEventServerSeq = 1;

	return JT_AMIOC_ADD(sJtEventServerSeq,1);
}
int JtEventServer::StopInner(int cmd, int seq)
{
	//在此处移除掉该server上的所有事件
	//to do
	
	for(std::set<JtEventPeer *>::iterator iter = m_PeerAddedS.begin(); iter!=m_PeerAddedS.end(); ++iter)
	{
		//(*iter)->OnRemoveFromServer();  to do
	}

	if(base)
	{
		event_base_loopexit(base, NULL);
	}

	return 0;
}
int JtEventServer::Stop()
{
	AsyncEvent Event(std::bind(&JtEventServer::AddPeerInner,this,std::placeholders::_1,std::placeholders::_2)
		, JTEVENTSERVER_NOTIFY_STOP_LOOP, GenSeq(), this);

	int res = DoInSync(Event);

#if (defined(WIN32) || defined(WIN64))

	if(tid!=(HANDLE)-1)
	{
		WaitForSingleObject(tid, INFINITE);
		//CloseHandle(tid);
		tid = (HANDLE)-1;
	}
#else
	if(tid!=(pthread_t)-1)
	{
		pthread_join(tid, NULL);
		tid = -1;
	}
#endif
	
	return 0;

/*
	ExCommand Head;
	Head.nSrcType				= 0;
	Head.nCmdType				= JTEVENTSERVER_NOTIFY_STOP_LOOP;
	Head.nCmdSeq				= GenSeq();
	Head.nContentSize			= 0;

	pEventPairPipe->SendCmd((const char*)&Head, sizeof(Head));

#if (defined(WIN32) || defined(WIN64))
	Sleep(2000);
#else
	sleep(2);
#endif
*/

	return 0;
}

int JtEventServer::AddPeerInner(int cmd, int seq)
{
	//MessageBox(NULL, _T("测试命令"), _T("测试命令"), MB_OK);

	JtEventPeer *Peer = NULL;
	{
		JtMutexAutoLock L(m_PeerLock);
		std::map<int, JtEventPeer *>::iterator iter = m_PeerWait2AddS.find(seq);
		if(iter!=m_PeerWait2AddS.end())
		{
			Peer = iter->second;
			m_PeerWait2AddS.erase(seq);

			jtprintf("[%s]Peer %p, seq %d\n", __FUNCTION__, Peer, seq);
		}
	}

	if(Peer)
		return Peer->OnAddToServer(this);
	else
		return 0;
}

int JtEventServer::AddPeer(JtEventPeer* Peer)
{
	//lock.... to do.....
	//assert(Peer);

	AsyncEvent Event(std::bind(&JtEventServer::AddPeerInner,this,std::placeholders::_1,std::placeholders::_2)
		, JTEVENTSERVER_NOTIFY_ADD_PEER, GenSeq(), Peer);

	{
		JtMutexAutoLock L(m_PeerLock);
		m_PeerWait2AddS[Event.m_Seq]=(Peer);
	}
	
	jtprintf("[%s]m_Seq %d, AddPeer %p\n", __FUNCTION__, Event.m_Seq, Peer);
	
	int res = DoInSync(Event);

	return res;
}
int JtEventServer::RmvPeer(JtEventPeer* Peer)
{
	JtMutexAutoLock L(m_PeerLock);
	if(m_PeerAddedS.size())
		m_PeerAddedS.erase(Peer);

	/*
	std::set<JtEventPeer *>::iterator iter = m_PeerAddedS.find(Peer);
	if(iter!=m_PeerAddedS.end())
	{
		Peer->
	}
	*/

	return 0;
}
int JtEventServer::TestCmdInner(int cmd, int seq)
{
	return 0;
}
int JtEventServer::TestCmd()
{
	AsyncEvent Event(std::bind(&JtEventServer::TestCmdInner,this,std::placeholders::_1,std::placeholders::_2)
		, JTEVENTSERVER_TEST_CMD, GenSeq(), this);

	int res = DoInSync(Event);

	return 0;
}
//要得到返回值 to do....
int JtEventServer::DoInSync(AsyncEvent Event)
{
	int res = -1;
	if(IsInThread())
	{
		res = Event.m_Functor(Event.m_Cmd,Event.m_Seq);
	}
	else
	{
		if(1)///////////////??????????????????????????  z在那个线程循环时才发的？？？？？？to do。。。。。
		{
			//  1.先添加把异步Affair的同步成员初始完毕
			ST_AFFAIR_CALLBACK AffairCallBack((long)this,NULL,NULL,-1);
			tr1::shared_ptr<CCachedAffair> pCachedAffairItem(new CCachedAffair(&AffairCallBack));
			unsigned long long AffairId = CCachedAffairMap::Static_PushNewAffair_Pre(m_cachedMap, m_Lock
				, pCachedAffairItem, Event.m_Seq, Event.m_Cmd, Event.m_Cmd);

			ExCommand Head;
			Head.nSrcType				= 0;
			Head.nCmdType				= Event.m_Cmd;
			Head.nCmdSeq				= Event.m_Seq;
			Head.nContentSize			= 0;
			
			{
				//lock....
				JtMutexAutoLock L(m_EventLock);
				m_AsynEventS.push_back(Event);
			}

			if(!InHandling)
			{
				res = pEventPairPipe->SendCmd((const char*)&Head, sizeof(Head));
				if(res)
				{
					CCachedAffairMap::Static_CancelAffair_Pre(m_cachedMap, m_Lock, Event.m_Seq);
					jtprintf("[%s]Static_CancelAffair_Pre--------------------------------------\n", __FUNCTION__);
					return res;
				}
			}

			// 2.等待该同步事件 
			if(AffairCallBack.m_pOnGotData==NULL)
			{
				res = CCachedAffairMap::Static_PushNewAffair(m_cachedMap, m_Lock
					, pCachedAffairItem, Event.m_Seq);
			}
		}
	}

	if(res)
		jtprintf("[%s]res-------------------%d-------------------\n", __FUNCTION__, res);
	
	return res;
}
int JtEventServer::DoInAsyn(AsyncEvent Event)
{


	return 0;
}
/*
int JtEventServer::NotifyAddPeer()
{
	ExCommand Head;
	Head.nSrcType				= 0;
	Head.nCmdType				= JTEVENTSERVER_NOTIFY_ADD_PEER;
	Head.nCmdSeq				= GenSeq();
	Head.nContentSize			= 0;

	pEventPairPipe->SendCmd((const char*)&Head, sizeof(Head));

	ST_AFFAIR_CALLBACK AffairCallBack(NULL);
	int res = CCachedAffairMap::Static_PushNewAffair(m_cachedMap, m_Lock
		, &AffairCallBack, Head.nCmdSeq, Head.nCmdType, Head.nCmdType);

	return 0;
}
*/



