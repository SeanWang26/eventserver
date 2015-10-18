#ifndef JtEventType_h
#define JtEventType_h

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

typedef std::function<int(int,int)> AsynFunctor;

class AsyncEvent 
{
public:
	AsynFunctor m_Functor;
	int m_Cmd;
	int m_Seq;
	void* m_Cookie;
	AsyncEvent()
	{
		m_Cmd = 0;
		m_Seq = 0;
		m_Cookie = NULL;
	}
	AsyncEvent(AsynFunctor &&_Functor, int Cmd, int Seq, void* cookie):m_Functor(_Functor), m_Cmd(Cmd), m_Seq(Seq), m_Cookie(cookie) 
	{

	}
};

class JtEventServer;

class JtEventPeer
{
private:
	JtEventServer *m_Server;     //
	long m_Id;                   //每一个peer的唯一标示
protected:
	int SetServer(JtEventServer *Server)
	{
		m_Server = Server;
		return 0;
	}
public:
	JtEventPeer():m_Server(NULL)
	{}
	virtual ~JtEventPeer(){};
	
	JtEventServer *GetServer() { return m_Server; } 
	
	virtual int OnAddToServer(JtEventServer *m_Server)=0;
	virtual int OnRemoveFromServer()=0;
	virtual const char* GetEventPeerType()=0;
};





























#endif