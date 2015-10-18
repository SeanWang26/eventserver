#ifndef JTEVENTSERVER_H
#define JTEVENTSERVER_H

#include <list>
#include <vector>
#include <set>

#include "jtmutex.h"

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

enum
{
	JTEVENTSERVER_NOTIFY_STOP_LOOP = 1,//停止服务
	JTEVENTSERVER_NOTIFY_ADD_PEER,//添加一个peer
	JTEVENTSERVER_TEST_CMD,//测试信令
};

class  JtEventServerCallbackSink
{
public:
	virtual int OnServerStart(JtEventServer* Server)=0;
	virtual int OnServerStop(JtEventServer* Server)=0;
};

class JtEventServer : public JtPairPipeEventCallbackSink
{
private:
	
	CCachedAffairMapLock m_Lock;
	map<unsigned long long, tr1::shared_ptr<CCachedAffair> > m_cachedMap;//不设置超时处理部分，必须被处理

	//lock
	JtMutex m_PeerLock;
	std::map<int, JtEventPeer *> m_PeerWait2AddS;//改为弱指针
	std::set<JtEventPeer *> m_PeerAddedS;//改为弱指针

	JtMutex m_EventLock;
	std::list<AsyncEvent> m_AsynEventS;

	struct event_base *base;
	int m_Started;
	JtEventPairPipe *pEventPairPipe;

	int InHandling;
	static void InitJtEventServer(void);
#if (defined(WIN32) || defined(WIN64))
	HANDLE  tid;
	unsigned threadID;
	static unsigned int __stdcall  Static_StartInThread(void *arg);
#else
	pthread_t tid;
	static void *Static_StartInThread(void *arg);
#endif

	int Started();
	int EventLoop();
	int IsInThread();

	int AddPeerInner(int cmd, int seq);
	int StopInner(int cmd, int seq);
	int TestCmdInner(int cmd, int seq);

	virtual int OnRecvData(void* Cookie, unsigned char* pData, int dataLen);
	virtual int OnStateChanged(void* Cookie);

	//int NotifyAddPeer();

public:
	JtEventServer();
	virtual ~JtEventServer();

	int Start();
	
	int Stop();
	int GenSeq();
	struct event_base *GetBase() { return base; }

	int AddPeer(JtEventPeer* Peer);
	int RmvPeer(JtEventPeer* Peer);

	int TestCmd();

	int DoInSync(AsyncEvent Event);
	int DoInAsyn(AsyncEvent Event);


};

#endif