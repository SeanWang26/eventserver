#ifndef JTEVENTSERVER_H
#define JTEVENTSERVER_H

#include <list>
#include <vector>
#include <functional>

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

#include "JtEventType.h"
#include "JtEventPairPipe.h"
#include "CachedAffair.h"

class JtEventServer;
struct event_base;

class JtEventPairPipe;

class  JtEventServerCallbackSink
{
public:
	virtual int OnServerStart(JtEventServer* Server)=0;
	virtual int OnServerStop(JtEventServer* Server)=0;
};

class JtEventServer : public JtPairPipeEventCallbackSink
{
private:
	typedef std::function<void(int,int)> AsynFunctor;
	
	class AsyncEvent 
	{
	public:
		AsynFunctor m_Functor;
		int m_Cmd;
		int m_Seq;

		AsyncEvent(AsynFunctor &&_Functor, int Cmd, int Seq):m_Functor(_Functor), m_Cmd(Cmd), m_Seq(Seq)
		{
			
		}
	};

	CCachedAffairMapLock m_Lock;
	map<unsigned long long, tr1::shared_ptr<CCachedAffair> > m_cachedMap;

	//lock
	std::list<JtEventPeer *> m_PeerWait2AddS;

	std::vector<AsyncEvent> m_AsynEventS;

	struct event_base *base;
	int m_Started;
	JtEventPairPipe *pEventPairPipe;

	static void InitJtEventServer(void);
#if (defined(WIN32) || defined(WIN64))
	static void Static_StartInThread(void *arg);
#else
	static void *Static_StartInThread(void *arg);
#endif

	int Started();
	int EventLoop();
	
	void AddPeerInner(int cmd, int seq);

	virtual int OnRecvData(void* Cookie, unsigned char* pData, int dataLen);
	virtual int OnStateChanged(void* Cookie);

	int NotifyAddPeer();

public:
	JtEventServer();
	~JtEventServer();

	int Start();
	
	int Stop();
	int GenSeq();
	struct event_base *GetBase() { return base; }

	int AddPeer(JtEventPeer* Peer);
	int TestCmd();

	int DoInAsyn(AsynFunctor &&Functor);
};

#endif