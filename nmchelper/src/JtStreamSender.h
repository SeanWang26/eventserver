#ifndef JtStreamSender_h
#define JtStreamSender_h


#include <string>

#if (defined(WIN32) || defined(WIN64))
	#include <winsock2.h>
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
	#include <sys/socket.h>
#endif

#include <stdio.h>

class JtStreamSender
{

#if (defined(WIN32) || defined(WIN64))
	SOCKET						ConnSocket_;
#else
	int 						ConnSocket_;
#endif

	std::string cRemoteIP_;
	int  nRemotePort_;

	std::string cLocalIP_;
	int  nLocalPort_;

	int CloseSocket();
public:
	JtStreamSender();
	~JtStreamSender();
	int DoConnect();
	int DisConnect();
	int SetRemoteAddr(std::string Ip, int Port);
	int SendData(char* Data, int DataLen);
	int GetLocalAddr(std::string &Ip, int &Port);
};



#endif
