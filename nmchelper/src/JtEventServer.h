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
	typedef std::function<void(int)> AsynFunctor;
	//lock
	std::list<JtEventPeer *> m_PeerWait2AddS;

	std::vector<AsynFunctor> m_AsynFuncS;

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
	
	void AddPeerInner(int arg);

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