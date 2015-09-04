#include "StdAfx.h"
#if (defined(WIN32) || defined(WIN64))

#else
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>

#endif

#include <errno.h>
#include <string.h>
#include "jtprintf.h"
#include "JtStreamSender.h"

JtStreamSender::JtStreamSender()
{
	ConnSocket_ = -1;
	cLocalIP_ = "";
	nLocalPort_ = -1;
}
JtStreamSender::~JtStreamSender()
{
	CloseSocket();
}
int JtStreamSender::SetRemoteAddr(std::string Ip, int Port)
{
	cRemoteIP_ = Ip;
	nRemotePort_ = Port;
	return 0;
}
int JtStreamSender::SendData(char* Data, int DataLen)
{
	if(ConnSocket_==-1)
	{
		return -1;
	}

	if(send(ConnSocket_, Data, DataLen, 0)<=0)
	{
		jtprintf("send() failed: %d\n",errno);
		return -1;
	}

	return 0;
}
int JtStreamSender::DoConnect()
{
	CloseSocket();
	
	ConnSocket_	= socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); 
	if (ConnSocket_ < 0)
		return -1;

	jtprintf("[%s]%s, %d\n", __FUNCTION__, cRemoteIP_.c_str(), nRemotePort_);

	struct sockaddr_in	remote_sockaddr;
	memset(&remote_sockaddr, 0, sizeof(struct sockaddr_in));
	remote_sockaddr.sin_family 				= AF_INET;
	remote_sockaddr.sin_addr.s_addr			= inet_addr(cRemoteIP_.c_str());
	remote_sockaddr.sin_port				= htons(nRemotePort_); 
	
#if (defined(WIN32) || defined(WIN64))
	//1.先将socket设为非堵塞
	unsigned long lBlocked = 1;
	ioctlsocket(ConnSocket_,FIONBIO ,&lBlocked);

#else
	//1.先将socket设为非堵塞
	unsigned long lBlocked = 1;
	ioctl(ConnSocket_,FIONBIO ,&lBlocked);
#endif

	//2.设置连接超时
	struct timeval Timeout;
	Timeout.tv_sec = 2;
	Timeout.tv_usec = 0;
	
	fd_set writefds;
	FD_ZERO(&writefds);
	FD_SET(ConnSocket_,&writefds);
	
	fd_set readfds;
	FD_ZERO(&readfds);
	FD_SET(ConnSocket_,&readfds);

	int ret = connect(ConnSocket_, (const sockaddr*)&remote_sockaddr, sizeof(sockaddr_in));
	ret = select(ConnSocket_+1,	&readfds, &writefds, NULL, &Timeout);

	jtprintf("[%s]select %d\n", __FUNCTION__, ret);

	if(ret == 0)
	{
		CloseSocket();
		return -2; //连接超时
	}
	else if(ret < 0)
	{
		CloseSocket();
		return -3; //error
	}
	else
	{
		jtprintf("[%s]connected\n", __FUNCTION__);
		
#if (defined(WIN32) || defined(WIN64))
		//3.将socket设为堵塞
		lBlocked = 0;
		ioctlsocket(ConnSocket_, FIONBIO ,&lBlocked);
#else
		//3.将socket设为堵塞
		lBlocked = 0;
		ioctl(ConnSocket_, FIONBIO ,&lBlocked);
#endif
		struct sockaddr_in sin;
		memset(&sin, 0, sizeof(sin));

#if (defined(WIN32) || defined(WIN64))
		int size = sizeof(sin);
#else
		socklen_t size = sizeof(sin);
#endif
		if (getsockname(ConnSocket_, (struct sockaddr *)&sin, &size) == -1) 
		{
			CloseSocket();
			return -4;
		}
		else
		{
			cLocalIP_=inet_ntoa(sin.sin_addr);
			nLocalPort_ = sin.sin_port;

			return 0;
		}
	}

	return -6;
}
int JtStreamSender::DisConnect()
{
	return CloseSocket();
}
int JtStreamSender::CloseSocket()
{
	if(ConnSocket_!=-1)
	{
#if (defined(WIN32) || defined(WIN64))
		closesocket(ConnSocket_);
		ConnSocket_ = -1;
#else
		close(ConnSocket_);
		ConnSocket_ = -1;
#endif
	}

	cLocalIP_ = "";
	nLocalPort_ = -1;

	return 0;
}
int JtStreamSender::GetLocalAddr(std::string &Ip, int &Port)
{
	if(nLocalPort_ != -1)
	{
		Ip = cLocalIP_;
		Port = nLocalPort_;
		return 0;
	}

	return -1;
}