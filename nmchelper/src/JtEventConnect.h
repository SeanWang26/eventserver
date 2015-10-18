#ifndef JTEVENTCONNECT_H
#define JTEVENTCONNECT_H

#include <string>
using namespace std;

#include "CachedAffair.h"
#include "JtEventServer.h"
#include "FramePkg.h"

#include <event2/listener.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>

class  JtConnectEventCallbackSink
{
public:
	virtual int OnRecvData(void* Cookie, unsigned char* pData, int dataLen)=0;
	virtual int OnConnected(void* Cookie)=0;
	virtual int OnClosed(void* Cookie)=0;
	virtual int OnReConnected(void* Cookie)=0;
};

struct bufferevent;

enum
{
	JTEVENTCONNECT_DISCONNECT = 1,//断开连接
	JTEVENTCONNECT_SENDDATA = 2,//发送数据
	JTEVENT_TEST_CMD
};

class JtEventConnect : public JtEventPeer, public OnGetFrameCallBack
{
private:
	string m_Ip;
	struct bufferevent *bev;
	evutil_socket_t m_sock ;
	CFramePkg<struct ST_ICMS_CMD_HEADER> m_FramePkg;

	JtConnectEventCallbackSink *m_Sink;
	void* m_EventCallbackUserData;

	int Connected;
private:
	virtual int OnGetFrame(unsigned char* Data, int len);

	//可写
	//static void Static_WriteCallback(struct bufferevent *bev, short events, void *ptr);
	//void WriteCallback(struct bufferevent *bev, short events);

	//事件
	static void Static_EventCallback(struct bufferevent *bev, short events, void *ptr);
	void EventCallback(struct bufferevent *bev, short events);
	
	//可读
	static void Static_ReadCallback(struct bufferevent *bev, void *ptr);
	void ReadCallback(struct bufferevent *bev);

	int DoDisconnectInner(int cmd, int seq);
public:
	virtual int OnAddToServer(JtEventServer *Server);
	virtual int OnRemoveFromServer();
	virtual const char* GetEventPeerType(){ return "JtEventConnect"; }

	JtEventConnect();
	~JtEventConnect();

	//int DoConnect(string Ip, uint16_t Port, int TimeOut);
	int DoConnectInner(string Ip, uint16_t Port, int TimeOut);
	int DoConnect(string Ip, uint16_t Port, int TimeOut,JtEventServer *Server);
	int DoDisconnect();
	
	int SendDataInner(const char* pData,int dataLen);
	int SendData(const char* pData,int dataLen);
	int SetJtEventCallbackSink(JtConnectEventCallbackSink *Sink, void* UserData);

	int TestCmd();
};























#endif
