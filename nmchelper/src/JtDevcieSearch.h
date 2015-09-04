#ifndef JT_DEVICE_SEARCH_H
#define JT_DEVICE_SEARCH_H

#include <time.h>
#include "nmctype.h"

#if (defined(WIN32) || defined(WIN64))
#else
#include <tr1/memory>
#include <netinet/in.h>  
#include <sys/socket.h>  
#include <sys/types.h>  
#include <netinet/in.h>  
#include <arpa/inet.h>  
#endif

#pragma pack(push,1)
struct MuiltCastCmd
{
	unsigned int          uMagic;        //
	unsigned short        nCmdType;      //
	unsigned int          nCmdSeq;       //
	unsigned int          uIp[4];        //
	unsigned int short    uRPort;        //
	unsigned int 	      nContentSize;  //
	unsigned char         cChecksum;     //
};

//CJtDevcieSearch 该类必须使用用tr1::shared_ptr来创建！！！！！！！！！！！！！！！！！！！！！！！！！！！！！
class CJtDevcieSearch
{
	nmc_status_callback   m_pstatus_callback;
	void*				  m_puserdata;

#if (defined(WIN32) || defined(WIN64))
	SOCKET m_nSockClientFd;
	SOCKET m_nSockM;
	static void Static_StartReceiveThread(void *arg);
#else
	int m_nSockClientFd;
	static void* Static_StartReceiveThread(void *arg);
#endif
	void StartReceiveThread();

	struct sockaddr_in local_addr;/*本地地址*/

	int m_uMagic;
	int m_Order;

	std::tr1::shared_ptr<CJtDevcieSearch> m_Self;

	int NmcJoinMulticastGroup();
	void HandleUdpCmd_GetAddrInfoRsp(struct MuiltCastCmd *Cmd);
	int SearchServerMulticast();
	
	
public:
	CJtDevcieSearch(void);
	~CJtDevcieSearch(void);
		
	int Init(std::tr1::shared_ptr<CJtDevcieSearch> Self, nmc_status_callback status_callback, void* userdata);

	void Search();
};

#endif