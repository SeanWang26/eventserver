#include "StdAfx.h"

#if (defined(WIN32) || defined(WIN64))

#else
#include <fcntl.h>
#include <unistd.h>
#endif
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include "TinyXmlHelper.h"
#include "jtprintf.h"

#include "JtDevcieSearch.h"


#define JT_C_IP "224.0.0.87" //�ͻ��˵Ķಥ��
#define JT_S_IP "224.0.0.88" //����˵Ķಥ��

#define SEAN_GET_ADDR_INFO_REQ          0X0b00
#define SEAN_GET_ADDR_INFO_RSP          0X0b01

#define SEAN_SET_ADDR_INFO_REQ          0X0b02
#define SEAN_SET_ADDR_INFO_RSP          0X0b03

#define SEAN_SET_REBOOT_REQ             0X0b04
#define SEAN_SET_REBOOT_RSP             0X0b05

#define SEAN_SET_HALT_REQ               0X0b06
#define SEAN_SET_HALT_RSP               0X0b07

#define MCAST_BUFF_SIZE 4096

CJtDevcieSearch::CJtDevcieSearch(void)
{
	m_pstatus_callback = NULL;
	m_puserdata = NULL;

#if (defined(WIN32) || defined(WIN64))
	m_nSockClientFd = INVALID_SOCKET;
	m_nSockM        = INVALID_SOCKET;
#else
	m_nSockClientFd = -1;
#endif

	srand(time(0));

	m_uMagic = rand();

	m_Order = 1;

	m_running = 0;
}
CJtDevcieSearch::~CJtDevcieSearch(void)
{
	jtprintf("[%s]\n", __FUNCTION__);
#if (defined(WIN32) || defined(WIN64))
	if(m_nSockM!=INVALID_SOCKET)
	{
		closesocket(m_nSockM);//�رն�����뿪�ಥ��
		m_nSockM = INVALID_SOCKET;
	}
	if(m_nSockClientFd!=INVALID_SOCKET)
	{
		closesocket(m_nSockClientFd);
		m_nSockClientFd = INVALID_SOCKET;
	}
#else
	if(m_nSockClientFd!=-1)
	{
		close(m_nSockClientFd);
		m_nSockClientFd = -1;
	}
#endif

	m_running = 0;
}
int CJtDevcieSearch::Init(std::tr1::weak_ptr<CJtDevcieSearch> Self, nmc_status_callback status_callback, void* userdata)
{
	m_pstatus_callback = status_callback;
	m_puserdata = userdata;

	m_Self = Self;

	return 1;
}
void CJtDevcieSearch::Search()
{
	if(m_running==0)
	{
		m_running = 1;

		NmcJoinMulticastGroup();
	}

	SearchServerMulticast();
}

void CJtDevcieSearch::HandleUdpCmd_GetAddrInfoRsp(struct MuiltCastCmd *Cmd)
{
    char* xml = (char*)(Cmd+1);
   
	tinyxml2::XMLDocument RspDoc;
	if(RspDoc.Parse(xml) != XML_NO_ERROR)
	{
		return;
	}

	tinyxml2::XMLElement *pRootElement = RspDoc.RootElement();
	if (NULL == pRootElement)
	{
		return;
	}

	struct st_xy_device_info device_info;
	memset(&device_info, 0, sizeof(device_info));
	device_info.len = sizeof(device_info);
	device_info.device_type = 0;

	////const char*chconfigport = SAFE_ITEM(pRootElement->Attribute("configport"));
	const char*chHostName = SAFE_ITEM(pRootElement->Attribute("HostName"));
	const char*chUuid = SAFE_ITEM(pRootElement->Attribute("Uuid"));

	tinyxml2::XMLElement* NetInterfaceListNode = pRootElement->FirstChildElement("NetInterfaceList");
	if(NetInterfaceListNode==NULL)
	{
		return;
	}

	tinyxml2::XMLElement* NetInterfaceNode = NetInterfaceListNode->FirstChildElement("NetInterface");
	while(NetInterfaceNode)
	{
		const char* chName = SAFE_ITEM(NetInterfaceNode->GetText());
		if(std::string(chName)=="lo")
		{
			NetInterfaceNode = NetInterfaceNode->NextSiblingElement();
			continue;
		}

		//const char* chstate = SAFE_ITEM(NetInterfaceNode->Attribute("state"));
		const char* chipv4 = SAFE_ITEM(NetInterfaceNode->Attribute("ipv4"));
		const char* chnetmask = SAFE_ITEM(NetInterfaceNode->Attribute("netmask"));
		const char* chnetgate = SAFE_ITEM(NetInterfaceNode->Attribute("netgate"));
		
		strcpy(device_info.ipv4, chipv4);
		strcpy(device_info.mask, chnetmask);
		strcpy(device_info.gateway, chnetgate);
		strcpy(device_info.iface, chName);

		char TmpName[1024];
		sprintf(TmpName, "%s-%s", chHostName, chUuid);
		strcpy(device_info.name, TmpName);

		if(device_info.ipv4[0]!='\0' && m_pstatus_callback)
			m_pstatus_callback(0, NDM_GET_SEARCH_DEVCIE, &device_info, device_info.len, m_puserdata);

		NetInterfaceNode = NetInterfaceNode->NextSiblingElement();
	}
}
#if (defined(WIN32) || defined(WIN64))
void CJtDevcieSearch::Static_StartReceiveThread(void *arg)
{
	CJtDevcieSearch *self = (CJtDevcieSearch *)arg;

	std::tr1::shared_ptr<CJtDevcieSearch> m_TmpSelf = self->m_Self.lock();
	if(m_TmpSelf)
		m_TmpSelf->StartReceiveThread();
}
int CJtDevcieSearch::NmcJoinMulticastGroup()
{
    int err = -1;

    m_nSockClientFd = WSASocket(AF_INET,SOCK_DGRAM,0,NULL,0,WSA_FLAG_MULTIPOINT_C_LEAF|WSA_FLAG_MULTIPOINT_D_LEAF| WSA_FLAG_OVERLAPPED); /*�����׽���*/
    if (m_nSockClientFd == INVALID_SOCKET)
    {
		printf("create socket failed with: %d\n",WSAGetLastError());
		return -1;
    }

	/*��ʼ����ַ*/
    memset(&local_addr, 0, sizeof(local_addr));
    local_addr.sin_family = AF_INET;
    local_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    local_addr.sin_port = htons(60125);//����Ϊ0���Զ�����˿�

	/*��socket*/
    err = bind(m_nSockClientFd,(struct sockaddr*)&local_addr, sizeof(local_addr)) ;
    if(err < 0)
    {
        printf("bind failed with: %d\n",WSAGetLastError());
		closesocket(m_nSockClientFd);
		m_nSockClientFd = INVALID_SOCKET;
        return -2;
    }

	struct sockaddr_in remote;
	remote.sin_family = AF_INET;
	remote.sin_port = local_addr.sin_port;//htons(MCAST_CLIENT_PORT);
	remote.sin_addr.s_addr = inet_addr(JT_C_IP);
	m_nSockM = WSAJoinLeaf(m_nSockClientFd,(SOCKADDR*)&remote,sizeof(remote), NULL,NULL,NULL,NULL, JL_BOTH);
	if(m_nSockM == INVALID_SOCKET)
	{
		printf("WSAJoinLeaf() failed : %d\n",WSAGetLastError()); 
		closesocket(m_nSockClientFd);
		m_nSockClientFd = INVALID_SOCKET;
		return -1;
	}

//#if (defined(WIN32) || defined(WIN64))
		_beginthread(Static_StartReceiveThread, NULL, this);
		Sleep(2000);
//#else
//		pthread_t tid;
//		pthread_attr_t attr;
//		pthread_attr_init(&attr);
//		//����ˣ�stop������Ϊ��PTHREAD_CREATE_DETACHED��
//		pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
//		res = pthread_create(&tid, &attr, Static_StartReceiveThread, this);
//		pthread_attr_destroy(&attr);
//		sleep(2);
//#endif


    return 0;
}
#else
void* CJtDevcieSearch::Static_StartReceiveThread(void *arg)
{
	CJtDevcieSearch *self = (CJtDevcieSearch *)arg;

	std::tr1::shared_ptr<CJtDevcieSearch> m_TmpSelf = self->m_Self.lock();
	if(m_TmpSelf)
		m_TmpSelf->StartReceiveThread();
	return NULL;
}
int CJtDevcieSearch::NmcJoinMulticastGroup()
{
    local_addr.sin_family = AF_INET;
    local_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    local_addr.sin_port = htons(60125);
	int len_srvr = sizeof(local_addr);
	m_nSockClientFd = socket(AF_INET, SOCK_DGRAM, 0);
	if(m_nSockClientFd==-1){
		jtprintf("[%s]m socket error, %d, %s\n", __FUNCTION__, errno, strerror(errno));
		return -1;
	}

	fcntl(m_nSockClientFd, F_SETFD, FD_CLOEXEC);
	if(bind(m_nSockClientFd, (struct sockaddr *)&local_addr, len_srvr)){
		jtprintf("m socket bind, %d, %s\n", errno, strerror(errno));

		close(m_nSockClientFd);
		m_nSockClientFd = -1;
		return -1;
	}

	//����udp��socket���԰�ͬһ���˿�
	int bReuseAddr = 1;
	setsockopt(m_nSockClientFd, SOL_SOCKET, SO_REUSEADDR, &bReuseAddr, sizeof(int));

	unsigned char ttl=255;
	int err = setsockopt(m_nSockClientFd,IPPROTO_IP,IP_MULTICAST_TTL,&ttl,sizeof(ttl)); 
	if(err < 0)
	{
		jtprintf("IP_MULTICAST_TTL, %d, %s\n", errno, strerror(errno));
		
		close(m_nSockClientFd);
		m_nSockClientFd = -1;
		return -3;
	}

	int loop = 1;
	setsockopt(m_nSockClientFd,IPPROTO_IP,IP_MULTICAST_LOOP,&loop,sizeof(loop));

	struct ip_mreq imr;
	imr.imr_multiaddr.s_addr = inet_addr(JT_C_IP);
	imr.imr_interface.s_addr = htonl(INADDR_ANY);
	if (setsockopt(m_nSockClientFd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (const char*)&imr, sizeof (struct ip_mreq)) < 0) {
		jtprintf("IP_ADD_MEMBERSHIP, %d, %s\n", errno, strerror(errno));
		
		close(m_nSockClientFd);
		m_nSockClientFd = -1;
		
		return -1;
	}
	
	pthread_t tid;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	//����ˣ�stop������Ϊ��PTHREAD_CREATE_DETACHED��
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	err = pthread_create(&tid, &attr, Static_StartReceiveThread, this);
	pthread_attr_destroy(&attr);
	sleep(2);

    return 0;
}
#endif
void CJtDevcieSearch::StartReceiveThread()
{
#if (defined(WIN32) || defined(WIN64))
	if(m_nSockClientFd == INVALID_SOCKET || m_nSockM == INVALID_SOCKET)
	{
		return;
	}

	int addr_len = 0;
#else
	if(m_nSockClientFd == -1)
	{
		return;
	}

	socklen_t addr_len = 0;
	
#endif
	struct sockaddr_in local_addr;/*���ص�ַ*/
	///////int err = -1;
	
    char buff[MCAST_BUFF_SIZE];
	bool bFinished = false;

	while( !bFinished )
	{
		//���ļ�����������
		fd_set readfd;
		//�ļ���������0
		FD_ZERO(&readfd);
		//���׽����ļ����������뵽�ļ�������������
		FD_SET(m_nSockClientFd, &readfd);

		struct timeval timeout;
		timeout.tv_sec=4;//��ʱʱ��Ϊ2��
		timeout.tv_usec=0;

		int nMaxFd = m_nSockClientFd + 1;
		int ret = select(nMaxFd,&readfd,NULL,NULL,&timeout);
		switch(ret){
	 		case -1:
				bFinished  = true;
				break;
	 		case 0:
				jtprintf("timeout\n");
				bFinished  = true;
				break;
			default:
				{
					jtprintf("get m ack\n");
					addr_len = sizeof(local_addr);
					memset(buff, 0, MCAST_BUFF_SIZE); /*��ս��ջ�����*/

					/*��������*/
					int nRecvLen = recvfrom(m_nSockClientFd, buff, MCAST_BUFF_SIZE, 0,(struct sockaddr*)&local_addr,&addr_len);
					if( nRecvLen == -1)
					{
						jtprintf("recvfrom()");
					}
					else
					{
						struct MuiltCastCmd *Cmd = (struct MuiltCastCmd *)buff;
						if(Cmd->uMagic==m_uMagic)
						{
							if(Cmd->nCmdType==SEAN_GET_ADDR_INFO_RSP)
							{
								HandleUdpCmd_GetAddrInfoRsp(Cmd);
							}
						}
					}
				}
				break;
		}
	}
/*
#if (defined(WIN32) || defined(WIN64))
	if(m_nSockM!=INVALID_SOCKET)
	{
		closesocket(m_nSockM);//�رն�����뿪�ಥ��
		m_nSockM = INVALID_SOCKET;
	}
	if(m_nSockClientFd!=INVALID_SOCKET)
	{
		closesocket(m_nSockClientFd);
		m_nSockClientFd = INVALID_SOCKET;
	}
#else
	if(m_nSockClientFd!=-1)
	{
		close(m_nSockClientFd);
		m_nSockClientFd = -1;
	}
#endif
*/
}

int CJtDevcieSearch::SearchServerMulticast()
{
    int nSockFd = -1;
    struct sockaddr_in mcast_addr;
    nSockFd = socket(AF_INET, SOCK_DGRAM, 0);/*�����׽���*/
    if (nSockFd == -1)
    {
        perror("socket()");
        return -1;
    }

    memset(&mcast_addr, 0, sizeof(mcast_addr));/*��ʼ��IP�ಥ��ַΪ0*/
    mcast_addr.sin_family = AF_INET;                /*����Э��������ΪAF*/
	mcast_addr.sin_addr.s_addr = inet_addr(JT_S_IP); /*���öಥIP��ַ*/
	mcast_addr.sin_port = htons(62626);        /*���öಥ�˿�*/
   
    struct MuiltCastCmd Cmd;
    Cmd.uMagic = m_uMagic;
    Cmd.nCmdType = SEAN_GET_ADDR_INFO_REQ;
    Cmd.nCmdSeq = 0;
    Cmd.uIp[0] = htonl(inet_addr(JT_C_IP));
    Cmd.uRPort =  htons(local_addr.sin_port);
    Cmd.nContentSize = 0;
    Cmd.cChecksum = 0;

    int nSendLen = sendto(nSockFd,	/*�׽���������*/
                    (const char *)&Cmd,	/*����*/
                    sizeof(Cmd),	/*����*/
                    0,
                    (struct sockaddr*)&mcast_addr,
                    sizeof(mcast_addr));
	if(nSendLen<=0)
	{
		jtprintf("[%s]sendto error\n", __FUNCTION__); 
	}

#if (defined(WIN32) || defined(WIN64))
	closesocket(nSockFd);
	nSockFd = -1;
#else
	close(nSockFd);
	nSockFd = -1;
#endif

	return 0;
}
int CJtDevcieSearch::ChangeDeviceNet(struct st_xy_device_info *old_device_info, struct st_xy_device_info *new_device_info)
{
#if 1
	char *uuid = strchr(old_device_info->name,'-');
	if(uuid==NULL)
		return -1;

	uuid=uuid+1;

	char info[2048] = "";

	sprintf(info, "<?xml version=\"1.0\" encoding=\"gb2312\"?>\n"
		"<ExSetAddrInfoReq  Uuid=\"%s\" >"
		"<NetInterfaceList>"
		"<NetInterface state=\"0\" ipv4=\"%s\" ipv6=\"0\"  netmask=\"%s\" netgate=\"%s\" dns=\"0\" >%s</NetInterface>"
		"</NetInterfaceList>"
		"</ExSetAddrInfoReq>", 
		uuid,
		new_device_info->ipv4, 
		new_device_info->mask, 
		new_device_info->gateway,
		new_device_info->iface);

	int nSockFd = -1;
	struct sockaddr_in mcast_addr;
	nSockFd = socket(AF_INET, SOCK_DGRAM, 0);/*�����׽���*/
	if (nSockFd == -1)
	{
		perror("socket()");
		return -1;
	}

	memset(&mcast_addr, 0, sizeof(mcast_addr));/*��ʼ��IP�ಥ��ַΪ0*/
	mcast_addr.sin_family = AF_INET;                /*����Э��������ΪAF*/
	mcast_addr.sin_addr.s_addr = inet_addr(JT_S_IP); /*���öಥIP��ַ*/
	mcast_addr.sin_port = htons(62626);        /*���öಥ�˿�*/

	int ninfo = strlen(info);
	int szMuiltCastCmd = sizeof(struct MuiltCastCmd);
	char *cmd = new char[szMuiltCastCmd+ninfo];

	struct MuiltCastCmd* Cmd = (struct MuiltCastCmd*)cmd;
	Cmd->uMagic = m_uMagic;
	Cmd->nCmdType = SEAN_SET_ADDR_INFO_REQ;
	Cmd->nCmdSeq = 0;
	Cmd->uIp[0] = htonl(inet_addr(JT_C_IP));
	Cmd->uRPort =  htons(local_addr.sin_port);
	Cmd->nContentSize = ninfo;
	Cmd->cChecksum = 0;

	memcpy(Cmd+1, info, ninfo);

	int nSendLen = sendto(nSockFd,	/*�׽���������*/
		(const char *)Cmd,	/*����*/
		sizeof(struct MuiltCastCmd)+ninfo,	/*����*/
		0,
		(struct sockaddr*)&mcast_addr,
		sizeof(mcast_addr));
	if(nSendLen<=0)
	{
		jtprintf("[%s]sendto error\n", __FUNCTION__); 
	}

#if (defined(WIN32) || defined(WIN64))
	closesocket(nSockFd);
	nSockFd = -1;
#else
	close(nSockFd);
	nSockFd = -1;
#endif

	delete [] cmd;

#endif

	return 0;
}
int CJtDevcieSearch::ShutdownDevice(struct st_xy_device_info *device_info, int type)
{
#if 1
	char *uuid = strchr(device_info->name,'-');
	if(uuid==NULL)
		return -1;

	uuid=uuid+1;

	char info[2048] = "";
	if(type==NMC_REBOOT)//����
		sprintf(info, "<?xml version=\"1.0\" encoding=\"gb2312\"?><ExRebootReq Uuid=\"%s\" ></ExRebootReq>", uuid);
	else if(type==NMC_HALT)//�ػ�
		sprintf(info, "<?xml version=\"1.0\" encoding=\"gb2312\"?><ExHaltReq Uuid=\"%s\" ></ExHaltReq>", uuid);
	
	int nSockFd = -1;
	struct sockaddr_in mcast_addr;
	nSockFd = socket(AF_INET, SOCK_DGRAM, 0);/*�����׽���*/
	if (nSockFd == -1)
	{
		perror("socket()");
		return -1;
	}

	memset(&mcast_addr, 0, sizeof(mcast_addr));/*��ʼ��IP�ಥ��ַΪ0*/
	mcast_addr.sin_family = AF_INET;                /*����Э��������ΪAF*/
	mcast_addr.sin_addr.s_addr = inet_addr(JT_S_IP); /*���öಥIP��ַ*/
	mcast_addr.sin_port = htons(62626);        /*���öಥ�˿�*/

	int ninfo = strlen(info);
	int szMuiltCastCmd = sizeof(struct MuiltCastCmd);
	char *cmd = new char[szMuiltCastCmd+ninfo];

	struct MuiltCastCmd* Cmd = (struct MuiltCastCmd*)cmd;
	Cmd->uMagic = m_uMagic;

	if(type==NMC_REBOOT)//����
		Cmd->nCmdType = SEAN_SET_REBOOT_REQ;
	else if(type==NMC_HALT)//�ػ�
		Cmd->nCmdType = SEAN_SET_HALT_REQ;

	Cmd->nCmdSeq = 0;
	Cmd->uIp[0] = htonl(inet_addr(JT_C_IP));
	Cmd->uRPort =  htons(local_addr.sin_port);
	Cmd->nContentSize = ninfo;
	Cmd->cChecksum = 0;

	memcpy(Cmd+1, info, ninfo);

	int nSendLen = sendto(nSockFd,	/*�׽���������*/
		(const char *)Cmd,	/*����*/
		sizeof(struct MuiltCastCmd)+ninfo,	/*����*/
		0,
		(struct sockaddr*)&mcast_addr,
		sizeof(mcast_addr));
	if(nSendLen<=0)
	{
		jtprintf("[%s]sendto error\n", __FUNCTION__); 
	}

#if (defined(WIN32) || defined(WIN64))
	closesocket(nSockFd);
	nSockFd = -1;
#else
	close(nSockFd);
	nSockFd = -1;
#endif

	delete [] cmd;

#endif

	return 0;
}