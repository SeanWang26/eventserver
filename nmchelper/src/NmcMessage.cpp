#include "StdAfx.h"
#include "NmcCmdDefine.h"
#include "TinyXmlHelper.h"
#include "NmcCmdDefine.h"
#include "NmcMessage.h"
#include <assert.h>

static int static_nCmdSeq = 1;

NmcMessage::NmcMessage(void)
{
}


NmcMessage::~NmcMessage(void)
{

}
int NmcMessage::BuildLoginReqMsg(char *UserName, char* Password, char* Cookie, int NetworkType, string &Msg)
{
/*
<?xml version="1.0" encoding="gb2312"?>
<ExLoginReq Cache="">  //Cache?????????????????????????????????????????Cache??????????????????????????????????
	<UserName>admin</UserName>
	<Password>1234</Password>
	<NetworkType>0</NetworkType> //0????????1??????
</ExLoginReq>
*/
	tinyxml2::XMLDocument ReqDoc;
	if(XML_NO_ERROR!=ReqDoc.Parse("<?xml version=\"1.0\" encoding=\"gb2312\"?><ExLoginReq/>\n"))//
	{
		//
		return -1;
	}
	 
	tinyxml2::XMLElement *pRootElement = ReqDoc.RootElement();
	ADD_ATTR(pRootElement,Cache,Cookie);
	
	ADD_NODE_WITH_TEXT(ReqDoc,UserName,pRootElement,UserName);
	ADD_NODE_WITH_TEXT(ReqDoc,Password,pRootElement,Password);
	ADD_NODE_WITH_TEXT(ReqDoc,NetworkType,pRootElement,"0");


	tinyxml2::XMLPrinter streamer;
	ReqDoc.Accept(&streamer);
	string xmlstr(streamer.CStr());

	ExCommand Head;
	Head.nSrcType				= EX_SERVER_NMC;//ICMS_CLIENT_MONITOR;
	Head.nCmdType				= EX_NMC_LOGIN_REQ;
	Head.nCmdSeq				= ++static_nCmdSeq;
	Head.nContentSize			= xmlstr.size()+1;

	Msg.assign((char*)&Head, sizeof(Head));

	Msg.append(xmlstr.c_str(), xmlstr.size());


	return Head.nCmdSeq;
}
int NmcMessage::ParserLoginRspMsg(const string &Msg, std::string& strCache, int &nRetCode, std::string& strRemark)
{
	ExCommand *Head = (ExCommand *)Msg.data();
	assert(Head->nCmdType == EX_NMC_LOGIN_RSP);
	
	tinyxml2::XMLDocument RspDoc;
	const char* data = Msg.data()+sizeof(ExCommand);
	if(RspDoc.Parse(data) != XML_NO_ERROR)
	{
		return -1;
	}

	tinyxml2::XMLElement *pRootElement = RspDoc.RootElement();
	if (NULL == pRootElement)
	{
		return -21;
	}
	strCache = SAFE_ITEM(pRootElement->Attribute("Cache"));

	tinyxml2::XMLElement* RetValNode = pRootElement->FirstChildElement("RetVal");
	if(RetValNode==NULL)
	{
		return -3; 
	}

	if(RetValNode->QueryIntAttribute("Code", &nRetCode)!=XML_NO_ERROR)
	{
		return -4;
	}

	strRemark = SAFE_ITEM(RetValNode->Attribute("Remark"));

	tinyxml2::XMLElement* UserInfoNode = pRootElement->FirstChildElement("UserInfo");

	int UserId;
	if(UserInfoNode->QueryIntAttribute("UserId", &UserId)!=XML_NO_ERROR)
	{
		return -5;
	}
	
	return 0;
}
int NmcMessage::BuildLogoutReqMsg(long UserId, char* Cookie, string &Msg)
{
/*
<?xml version="1.0" encoding="gb2312"?>
<ExLogoutReq Cache="">
<UserId ></UserId>
</ExLogoutReq>
*/
	tinyxml2::XMLDocument ReqDoc;

	if(XML_NO_ERROR!=ReqDoc.Parse("<ExLogoutReq/>\n"))//
	{
		
		return -1;
	}
	 
	tinyxml2::XMLElement *pRootElement = ReqDoc.RootElement();
	ADD_ATTR(pRootElement,Cache,Cookie);

	char tmp[64] = {0};
	sprintf(tmp, "%ld", UserId);
	ADD_NODE_WITH_TEXT(ReqDoc,UserId,pRootElement,tmp);
	
	tinyxml2::XMLPrinter streamer;
	ReqDoc.Accept(&streamer);
	string xmlstr(streamer.CStr());

	ExCommand Head;
	Head.nSrcType				= EX_SERVER_NMC;//ICMS_CLIENT_MONITOR;
	Head.nCmdType				= EX_NMC_LOGIN_REQ;
	Head.nCmdSeq				= ++static_nCmdSeq;
	Head.nContentSize			= xmlstr.size()+1;

	Msg.assign((char*)&Head, sizeof(Head));

	Msg.append(xmlstr.c_str(), xmlstr.size());


	return Head.nCmdSeq;
}
int NmcMessage::ParserLogoutRspMsg(const string &Msg, std::string& strCache, int &nRetCode, std::string& strRemark)
{
	return 0;
}

int NmcMessage::BuildGetUserDefineDataReqMsg(int id, char* Cookie, string &ReqMsg)
{
/*
<?xml version="1.0" encoding="utf-8"?>
<ExGetUserDefineDataReq Cache=????/>
	<DataInfo DataId="-1"/> //-1??????????????????????
</ExGetUserDefineDataReq>
*/
	tinyxml2::XMLDocument ReqDoc;

	if(XML_NO_ERROR!=ReqDoc.Parse("<ExGetUserDefineDataReq/>\n"))//
	{
		return -1;
	}
	 
	tinyxml2::XMLElement *pRootElement = ReqDoc.RootElement();
	ADD_ATTR(pRootElement,Cache,Cookie);

	ADD_NODE(ReqDoc,DataInfo,pRootElement);
	ADD_ATTR(DataInfoNode,DataId,id);

	tinyxml2::XMLPrinter streamer;
	ReqDoc.Accept(&streamer);
	string xmlstr(streamer.CStr());

	ExCommand Head;
	Head.nSrcType				= EX_SERVER_NMC;
	Head.nCmdType				= EX_NMC_GET_USER_DEFINE_DATA_REQ;
	Head.nCmdSeq				= ++static_nCmdSeq;
	Head.nContentSize			= xmlstr.size()+1;

	ReqMsg.assign((char*)&Head, sizeof(Head));

	ReqMsg.append(xmlstr.c_str(), xmlstr.size());


	return Head.nCmdSeq;
}
int NmcMessage::ParserGetUserDefineDataMsg(const string &RspMsg, std::string& strCache, struct st_xy_user_data **ppuser_data, int *user_data_cnt,int &nRetCode, std::string& strRemark)
{
/*
<?xml version="1.0" encoding="utf-8"?>
<ExGetUserDefineDataRsp Cache=????>
	<RetVal Code="0" Remark="success"/>
	<DataInfoList>
		<DataInfo DataId="1" DataType="">
		<![CDATA[ ???? ]]>
		</DataInfo>
	??
	</DataInfoList>
</ExGetUserDefineDataRsp>
*/
	ExCommand *Head = (ExCommand *)RspMsg.data();
	assert(Head->nCmdType == EX_NMC_GET_USER_DEFINE_DATA_RSP);

	tinyxml2::XMLDocument RspDoc;
	const char* data = RspMsg.data()+sizeof(ExCommand);
	if(RspDoc.Parse(data) != XML_NO_ERROR)
	{
		return -1;
	}
	tinyxml2::XMLElement *pRootElement = RspDoc.RootElement();
	if (NULL == pRootElement)
	{
		return -21;
	}
	strCache = SAFE_ITEM(pRootElement->Attribute("Cache"));
	tinyxml2::XMLElement* RetValNode = pRootElement->FirstChildElement("RetVal");
	if(RetValNode==NULL)
	{
		return -3; 
	}
	if(RetValNode->QueryIntAttribute("Code", &nRetCode)!=XML_NO_ERROR)
	{
		return -4;
	}
	strRemark = SAFE_ITEM(RetValNode->Attribute("Remark"));

	//////////////////

	tinyxml2::XMLElement* DataInfoListNode = pRootElement->FirstChildElement("DataInfoList");
	if(DataInfoListNode==NULL)
		return -92860;

	vector<struct st_xy_user_data> TmpUserDataS;
	tinyxml2::XMLElement* DataInfoNode = DataInfoListNode->FirstChildElement("DataInfo");
	while(DataInfoNode)
	{
		struct st_xy_user_data user_data;

		DataInfoNode->QueryIntAttribute("DataId", &(user_data.id));
		DataInfoNode->QueryIntAttribute("DataType", &(user_data.type));
		GET_TEXT(DataInfo);

		if(chDataInfo[0]=='\0')
		{
			user_data.data = NULL;
			user_data.len = 0;
		}
		else
		{
			user_data.len = strlen(chDataInfo);
			user_data.data = (char*)malloc(user_data.len+1);
			memcpy(user_data.data,chDataInfo,user_data.len);
			user_data.data[user_data.len] = 0;
		}

		TmpUserDataS.push_back(user_data);

		DataInfoNode = DataInfoNode->NextSiblingElement("DataInfo");
	}

	*user_data_cnt = TmpUserDataS.size();
	*ppuser_data = (struct st_xy_user_data*)malloc(sizeof(struct st_xy_user_data)**user_data_cnt);
	for(int i=0; i<*user_data_cnt; ++i)
	{
		memcpy((*ppuser_data)+i, &(TmpUserDataS[i]), sizeof(struct st_xy_user_data));
	}

	return 0;
}
int NmcMessage::BuildSetUserDefineDataReqMsg(enum en_nmc_operator_type operator_type, struct st_xy_user_data *ppuser_data, char* Cookie, string &ReqMsg)
{
/*
<?xml version="1.0" encoding="utf-8"?>
<ExSetUserDefineDataReq Cache=????>
	<DataInfoList>
		<DataInfo OpType="0" OpSeq="1" DataId="-1" DataType="">
		<![CDATA[ ???? ]]>
		</DataInfo>
		//OpType??0????1????2????????????DataId?-1
		??
	</DataInfoList>
</ExSetUserDefineDataReq>
*/
	tinyxml2::XMLDocument ReqDoc;

	if(XML_NO_ERROR!=ReqDoc.Parse("<ExSetUserDefineDataReq/>\n"))//
	{
		return -1;
	}
	 
	tinyxml2::XMLElement *pRootElement = ReqDoc.RootElement();
	ADD_ATTR(pRootElement,Cache,Cookie);

	ADD_NODE(ReqDoc,DataInfoList,pRootElement);

	ADD_NODE(ReqDoc,DataInfo,DataInfoListNode);
	ADD_ATTR(DataInfoNode,OpType,operator_type);
	ADD_ATTR(DataInfoNode,OpSeq,0);
	ADD_ATTR(DataInfoNode,DataId,ppuser_data->id);

	XMLText* pDataFiledNode = ReqDoc.NewText(ppuser_data->data);
	pDataFiledNode->SetCData(true);
	DataInfoNode->LinkEndChild(pDataFiledNode);

	tinyxml2::XMLPrinter streamer;
	ReqDoc.Accept(&streamer);
	string xmlstr(streamer.CStr());

	ExCommand Head;
	Head.nSrcType				= EX_SERVER_NMC;
	Head.nCmdType				= EX_NMC_SET_USER_DEFINE_DATA_REQ;
	Head.nCmdSeq				= ++static_nCmdSeq;
	Head.nContentSize			= xmlstr.size()+1;

	ReqMsg.assign((char*)&Head, sizeof(Head));

	ReqMsg.append(xmlstr.c_str(), xmlstr.size());

	return Head.nCmdSeq;
}
int NmcMessage::ParserSetUserDefineDataRspMsg(const string &RspMsg, std::string& strCache, enum en_nmc_operator_type &operator_type, struct st_xy_user_data *ppuser_data, int &nRetCode, std::string& strRemark)
{
/*
<?xml version="1.0" encoding="utf-8"?> 
<ExSetUserDefineDataRsp Cache=????>
	<RetVal Code="0" Remark="success"/>
	<DataInfoList>
		<DataInfo OpType="0" OpSeq="1" DataId="1"/>
		??
	</DataInfoList>
</ExSetUserDefineDataRsp>
*/
	ExCommand *Head = (ExCommand *)RspMsg.data();
	assert(Head->nCmdType == EX_NMC_SET_USER_DEFINE_DATA_RSP);

	tinyxml2::XMLDocument RspDoc;
	const char* data = RspMsg.data()+sizeof(ExCommand);
	if(RspDoc.Parse(data) != XML_NO_ERROR)
	{
		return -1;
	}
	tinyxml2::XMLElement *pRootElement = RspDoc.RootElement();
	if (NULL == pRootElement)
	{
		return -21;
	}
	strCache = SAFE_ITEM(pRootElement->Attribute("Cache"));
	tinyxml2::XMLElement* RetValNode = pRootElement->FirstChildElement("RetVal");
	if(RetValNode==NULL)
	{
		return -3; 
	}
	if(RetValNode->QueryIntAttribute("Code", &nRetCode)!=XML_NO_ERROR)
	{
		return -4;
	}
	strRemark = SAFE_ITEM(RetValNode->Attribute("Remark"));

	//////////////////

	tinyxml2::XMLElement* DataInfoListNode = pRootElement->FirstChildElement("DataInfoList");
	if(DataInfoListNode==NULL)
		return -2860;

	tinyxml2::XMLElement* DataInfoNode = DataInfoListNode->FirstChildElement("DataInfo");
	if(DataInfoNode)
	{
		struct st_xy_user_data user_data;

		DataInfoNode->QueryIntAttribute("DataId", &(user_data.id));
		DataInfoNode->QueryIntAttribute("OpType", (int*)&operator_type);
	}
	
	return 0;
}

int NmcMessage::BuildHeartBeatReqMsg(long UserId, char* Cookie, string &ReqMsg)
{
	/*
	<?xml version="1.0" encoding="gb2312" ?>
	<ExHeartBeatReq Cache="NULL"/>	
	*/

	tinyxml2::XMLDocument ReqDoc;
	char cXml[256];
	sprintf(cXml, "<?xml version=\"1.0\" encoding=\"gb2312\"?>\n<ExHeartBeatReq  Cache=\"%s\" />\n", Cookie);
	if(XML_NO_ERROR!=ReqDoc.Parse(cXml))//
	{
		return -1;
	}

	tinyxml2::XMLPrinter streamer;
	ReqDoc.Accept(&streamer);
	string xmlstr(streamer.CStr());

	ExCommand Head;
	Head.nSrcType				= EX_SERVER_NMC;//ICMS_CLIENT_MONITOR;
	Head.nCmdType				= EX_HEART_BEAT_REQ;
	Head.nCmdSeq				= ++static_nCmdSeq;
	Head.nContentSize			= xmlstr.size()+1;

	ReqMsg.assign((char*)&Head, sizeof(Head));

	ReqMsg.append(xmlstr.c_str(), xmlstr.size());
	
	return Head.nCmdSeq;
}
int NmcMessage::ParserHeartBeatRspMsg(const string &RspMsg, std::string& strCache, int &nRetCode, std::string& strRemark)
{
	return 0;
}

int NmcMessage::BuildGetRemoteDeviceReqMsg(int id, char* Cookie, string &ReqMsg)
{
/*
<?xml version="1.0" encoding="utf-8"?>
<ExGetSignalSourceInfoReq Cache=????>
	<SourceInfo SourceId=????/> //-1??????????????
</ExGetSignalSourceInfoReq>
*/
	const char* cReq = "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n" 
		"<ExGetSignalSourceInfoReq/>\n";

	tinyxml2::XMLDocument ReqDoc;
	if(XML_NO_ERROR!=ReqDoc.Parse(cReq))//
	{
		return -1;
	}

	tinyxml2::XMLElement *pRootElement = ReqDoc.RootElement();
	ADD_ATTR(pRootElement,Cache,Cookie);

	ADD_NODE(ReqDoc,SourceInfo,pRootElement);

	ADD_ATTR(SourceInfoNode,SourceId,id);
	
	tinyxml2::XMLPrinter streamer;
	ReqDoc.Accept(&streamer);
	string xmlstr(streamer.CStr());

	ExCommand Head;
	Head.nSrcType				= EX_SERVER_NMC;//ICMS_CLIENT_MONITOR;
	Head.nCmdType				= EX_NMC_GET_SIGNAL_SOURCE_INFO_REQ;
	Head.nCmdSeq				= ++static_nCmdSeq;
	Head.nContentSize			= xmlstr.size()+1;

	ReqMsg.assign((char*)&Head, sizeof(Head));

	ReqMsg.append(xmlstr.c_str(), xmlstr.size());

	return Head.nCmdSeq;
}
int NmcMessage::ParserGetRemoteDeviceRspMsg(const string &Msg, std::string& strCache, struct st_jn_device **DeviceS, int *DevCnt, int &nRetCode, std::string& strRemark)
{
	/*
	<?xml version="1.0" encoding="utf-8"?>
<ExGetSignalSourceInfoRsp Cache=????>
	<RetVal Code="0" Remark="success"/>
	<SourceInfoList>
		<SourceInfo SourceId=???? SourceType="" SourceName="" Ip="" Port="" Uri="" Username="" Password="" Extend=""/>
		//SourceType?? 0x0011?NVR??????0x0021?RTSP???????0x0022??????????????0x0031???????
		??
	</SourceInfoList>
</ExGetSignalSourceInfoRsp>
	*/
	ExCommand *Head = (ExCommand *)Msg.data();
	assert(Head->nCmdType == EX_NMC_GET_SIGNAL_SOURCE_INFO_RSP);

	tinyxml2::XMLDocument RspDoc;
	const char* data = Msg.data()+sizeof(ExCommand);
	if(RspDoc.Parse(data) != XML_NO_ERROR)
	{
		return -1;
	}

	tinyxml2::XMLElement *pRootElement = RspDoc.RootElement();
	if (NULL == pRootElement)
	{
		return -21;
	}
	strCache = SAFE_ITEM(pRootElement->Attribute("Cache"));

	tinyxml2::XMLElement* RetValNode = pRootElement->FirstChildElement("RetVal");
	if(RetValNode==NULL)
	{
		return -3; 
	}

	if(RetValNode->QueryIntAttribute("Code", &nRetCode)!=XML_NO_ERROR)
	{
		return -4;
	}

	strRemark = SAFE_ITEM(RetValNode->Attribute("Remark"));

	tinyxml2::XMLElement* SourceInfoListNode = pRootElement->FirstChildElement("SourceInfoList");
	if(SourceInfoListNode==NULL)
	{
		return -1;
	}

	vector<struct st_jn_device> DS;
	tinyxml2::XMLElement* SourceInfoNode = SourceInfoListNode->FirstChildElement("SourceInfo");
	while(SourceInfoNode!=NULL)
	{		
		struct st_jn_device device;
		if(SourceInfoNode->QueryIntAttribute("SourceId", &device.DeviceId)!=XML_NO_ERROR)
		{}

		if(device.DeviceId==1)
		{
			SourceInfoNode = SourceInfoNode->NextSiblingElement("SourceInfo");
			continue; //???????????nvr????????nvr id?1
		}

		if(SourceInfoNode->QueryIntAttribute("SourceType", &device.DeviceType)!=XML_NO_ERROR)
		{}

		const char* cSourceName = SourceInfoNode->Attribute("SourceName");
		if(cSourceName)
			strcpy(device.DeviceName, cSourceName);
		else
			device.DeviceName[0] = 0;

		const char* cIp = SourceInfoNode->Attribute("Ip");
		if(cSourceName)
			strcpy(device.IP, cIp);
		else
			device.IP[0] = 0;

		if(SourceInfoNode->QueryIntAttribute("Port", &device.Port)!=XML_NO_ERROR)
		{}

		const char* cUsername = SourceInfoNode->Attribute("Username");
		if(cUsername)
			strcpy(device.User, cUsername);
		else
			device.IP[0] = 0;
		
		const char* cPassword = SourceInfoNode->Attribute("Password");
		if(cPassword)
			strcpy(device.Password, cPassword);
		else
			device.Password[0] = 0;


		const char* cUri = SourceInfoNode->Attribute("Uri");
		if(cUri)
			strcpy(device.Uri, cUri);
		else
			device.Uri[0] = 0;
		
		device.Extend[0] = 0;

		DS.push_back(device);

		SourceInfoNode = SourceInfoNode->NextSiblingElement("SourceInfo");
	}
	
	if(DS.size())
	{
		*DeviceS = (struct st_jn_device *)calloc(DS.size(), sizeof(struct st_jn_device));
		int i=0;
		for(vector<struct st_jn_device>::iterator iter = DS.begin(); iter!=DS.end(); ++iter)
		{
			(*DeviceS)[i++] = *iter;
		}

		*DevCnt = DS.size();
	}

	return 0;
}
int NmcMessage::BuildAddRemoteDeviceReqMsg(struct st_jn_device *dev, char* Cookie, string &ReqMsg)
{
/*
<?xml version="1.0" encoding="utf-8"?>
<ExSetSignalSourceReq Cache="">
	<SourceInfo OpType="0" SourceId="-1" SourceType="" SourceName="" Ip="" Port="" Uri="" Username="" Password="" Extend=""/>
	// OpType??0????1????2????????????SourceId?-1
	//SourceType?? 0x0011?NVR??????0x0021?ONVIF?????0x0022??????????????0x0023?RTSP???? 0x0031???????
</ExSetSignalSourceReq>
*/
	const char* cReq = "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n" 
		"<ExSetSignalSourceReq/>\n";

	tinyxml2::XMLDocument ReqDoc;
	if(XML_NO_ERROR!=ReqDoc.Parse(cReq))//
	{
		return -1;
	}

	tinyxml2::XMLElement *pRootElement = ReqDoc.RootElement();
	ADD_ATTR(pRootElement,Cache,Cookie);

	ADD_NODE(ReqDoc,SourceInfo,pRootElement);

	ADD_ATTR(SourceInfoNode,OpType,0);
	ADD_ATTR(SourceInfoNode,SourceId,-1);
	ADD_ATTR(SourceInfoNode,SourceType,0x0011);
	ADD_ATTR(SourceInfoNode,SourceName,dev->DeviceName);
	ADD_ATTR(SourceInfoNode,Ip,dev->IP);
	ADD_ATTR(SourceInfoNode,Port,dev->Port);
	ADD_ATTR(SourceInfoNode,Uri,"");
	ADD_ATTR(SourceInfoNode,Username,dev->User);
	ADD_ATTR(SourceInfoNode,Password,dev->Password);
	ADD_ATTR(SourceInfoNode,Extend,"");

	tinyxml2::XMLPrinter streamer;
	ReqDoc.Accept(&streamer);
	string xmlstr(streamer.CStr());

	ExCommand Head;
	Head.nSrcType				= EX_SERVER_NMC;//ICMS_CLIENT_MONITOR;
	Head.nCmdType				= EX_NMC_SET_SIGNAL_SOURCE_REQ;
	Head.nCmdSeq				= ++static_nCmdSeq;
	Head.nContentSize			= xmlstr.size()+1;

	ReqMsg.assign((char*)&Head, sizeof(Head));

	ReqMsg.append(xmlstr.c_str(), xmlstr.size());

	return Head.nCmdSeq;
}
int NmcMessage::ParserAddRemoteDeviceRspMsg(const string &Msg, std::string& strCache, struct st_jn_device *dev, int &nRetCode, std::string& strRemark)
{
/*
<?xml version="1.0" encoding="utf-8"?>
<ExSetSignalSourceRsp Cache=????>
	<RetVal Code="0" Remark="success"/>
	<SourceInfo OpType="0" SourceId=""/>
</ExSetSignalSourceRsp>
*/
	ExCommand *Head = (ExCommand *)Msg.data();
	assert(Head->nCmdType == EX_NMC_SET_SIGNAL_SOURCE_RSP);

	tinyxml2::XMLDocument RspDoc;
	const char* data = Msg.data()+sizeof(ExCommand);
	if(RspDoc.Parse(data) != XML_NO_ERROR)
	{
		return -1;
	}

	tinyxml2::XMLElement *pRootElement = RspDoc.RootElement();
	if (NULL == pRootElement)
	{
		return -21;
	}
	strCache = SAFE_ITEM(pRootElement->Attribute("Cache"));

	tinyxml2::XMLElement* RetValNode = pRootElement->FirstChildElement("RetVal");
	if(RetValNode==NULL)
	{
		return -3; 
	}

	if(RetValNode->QueryIntAttribute("Code", &nRetCode)!=XML_NO_ERROR)
	{
		return -4;
	}

	strRemark = SAFE_ITEM(RetValNode->Attribute("Remark"));


	tinyxml2::XMLElement* SourceInfoNode = pRootElement->FirstChildElement("SourceInfo");
	if(SourceInfoNode->QueryIntAttribute("SourceId", &dev->DeviceId)!=XML_NO_ERROR)
	{
		return -4;
	}

	return 0;
}

int NmcMessage::BuildRemoveRemoteDeviceReqMsg(int DevId,  char* Cookie, string &ReqMsg)
{
/*
<?xml version="1.0" encoding="utf-8"?>
<ExSetSignalSourceReq Cache="">
	<SourceInfo OpType="0" SourceId="-1" SourceType="" SourceName="" Ip="" Port="" Uri="" Username="" Password="" Extend=""/>
	// OpType??0????1????2????????????SourceId?-1
	//SourceType?? 0x0011?NVR??????0x0021?ONVIF?????0x0022??????????????0x0023?RTSP???? 0x0031???????
</ExSetSignalSourceReq>
*/
	const char* cReq = "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n" 
		"<ExSetSignalSourceReq/>\n";

	tinyxml2::XMLDocument ReqDoc;
	if(XML_NO_ERROR!=ReqDoc.Parse(cReq))//
	{
		return -1;
	}

	tinyxml2::XMLElement *pRootElement = ReqDoc.RootElement();
	ADD_ATTR(pRootElement,Cache,Cookie);

	ADD_NODE(ReqDoc,SourceInfo,pRootElement);

	ADD_ATTR(SourceInfoNode,OpType,2);
	ADD_ATTR(SourceInfoNode,SourceId,DevId);

	ADD_ATTR(SourceInfoNode,SourceType,0x0011);
	ADD_ATTR(SourceInfoNode,SourceName,"");
	ADD_ATTR(SourceInfoNode,Ip,"");
	ADD_ATTR(SourceInfoNode,Port,20000);
	ADD_ATTR(SourceInfoNode,Uri,"");
	ADD_ATTR(SourceInfoNode,Username,"");
	ADD_ATTR(SourceInfoNode,Password,"");
	ADD_ATTR(SourceInfoNode,Extend,"");

	tinyxml2::XMLPrinter streamer;
	ReqDoc.Accept(&streamer);
	string xmlstr(streamer.CStr());

	ExCommand Head;
	Head.nSrcType				= EX_SERVER_NMC;//ICMS_CLIENT_MONITOR;
	Head.nCmdType				= EX_NMC_SET_SIGNAL_SOURCE_REQ;
	Head.nCmdSeq				= ++static_nCmdSeq;
	Head.nContentSize			= xmlstr.size()+1;

	ReqMsg.assign((char*)&Head, sizeof(Head));

	ReqMsg.append(xmlstr.c_str(), xmlstr.size());

	return Head.nCmdSeq;
}
int NmcMessage::ParserRemoveRemoteDeviceRspMsg(const string &Msg, std::string& strCache, int &nRetCode, std::string& strRemark)
{
/*
<?xml version="1.0" encoding="utf-8"?>
<ExSetSignalSourceRsp Cache=????>
	<RetVal Code="0" Remark="success"/>
	<SourceInfo OpType="0" SourceId=""/>
</ExSetSignalSourceRsp>
*/
	ExCommand *Head = (ExCommand *)Msg.data();
	assert(Head->nCmdType == EX_NMC_SET_SIGNAL_SOURCE_RSP);

	tinyxml2::XMLDocument RspDoc;
	const char* data = Msg.data()+sizeof(ExCommand);
	if(RspDoc.Parse(data) != XML_NO_ERROR)
	{
		return -1;
	}

	tinyxml2::XMLElement *pRootElement = RspDoc.RootElement();
	if (NULL == pRootElement)
	{
		return -21;
	}
	strCache = SAFE_ITEM(pRootElement->Attribute("Cache"));

	tinyxml2::XMLElement* RetValNode = pRootElement->FirstChildElement("RetVal");
	if(RetValNode==NULL)
	{
		return -3; 
	}

	if(RetValNode->QueryIntAttribute("Code", &nRetCode)!=XML_NO_ERROR)
	{
		return -4;
	}

	strRemark = SAFE_ITEM(RetValNode->Attribute("Remark"));

	tinyxml2::XMLElement* SourceInfoNode = pRootElement->FirstChildElement("SourceInfo");
	int DeviceId = 0;
	if(SourceInfoNode->QueryIntAttribute("SourceId", &DeviceId)!=XML_NO_ERROR)
	{
		return -4;
	}

	return 0;
}
int NmcMessage::BuildGetMatrixReqMsg(int matrixid, char* Cookie, string &ReqMsg)
{
/*
<?xml version="1.0" encoding="utf-8"?> 
<ExGetMatrixInfoReq Cache="">
<MatrixInfo MatrixId=""/> //-1?????????????????????
</ExGetMatrixInfoReq>
*/
	const char* cReq = "<?xml version=\"1.0\" encoding=\"gb2312\"?>\n" 
		"<ExGetMatrixInfoReq/>\n";

	tinyxml2::XMLDocument ReqDoc;
	if(XML_NO_ERROR!=ReqDoc.Parse(cReq))//
	{
		
		return -1;
	}

	tinyxml2::XMLElement *pRootElement = ReqDoc.RootElement();
	ADD_ATTR(pRootElement,Cache,Cookie);

	ADD_NODE(ReqDoc,MatrixInfo,pRootElement);

	ADD_ATTR(MatrixInfoNode,MatrixId,matrixid);
	
	tinyxml2::XMLPrinter streamer;
	ReqDoc.Accept(&streamer);
	string xmlstr(streamer.CStr());

	ExCommand Head;
	Head.nSrcType				= EX_SERVER_NMC;//ICMS_CLIENT_MONITOR;
	Head.nCmdType				= EX_NMC_GET_MATRIX_INFO_REQ;
	Head.nCmdSeq				= ++static_nCmdSeq;
	Head.nContentSize			= xmlstr.size()+1;

	ReqMsg.assign((char*)&Head, sizeof(Head));

	ReqMsg.append(xmlstr.c_str(), xmlstr.size());

	return Head.nCmdSeq;
}

int NmcMessage::ParserGetMatrixRspMsg(const string &Msg, std::string& strCache, std::vector<struct stMatrixInfo> &MatrixInfoS, int &nRetCode, std::string& strRemark)
{
/*
<?xml version="1.0" encoding="utf-8"?> 
<ExGetMatrixInfoRsp Cache=????>
<RetVal Code="0" Remark="success"/>
<MatrixInfoList>
	<MatrixInfo MatrixId="" MatrixStatus="" MatrixName="" Ip="" Port="" Username="" Password="">
	//MatrixStatus? 0?????1????
		<OutputInfoList>
		<OutputInfo OutputId="0" Index="0" HasDisplay=??1?? SupportedResolution="1920x1080"/> 
//Index ?????????????
//HasDisplay????????????
//SupportedResolution????????????????*?????????????????
??
</OutputInfoList>
</MatrixInfo>
??
</MatrixInfoList>
</ExGetMatrixInfoRsp>
*/
	ExCommand *Head = (ExCommand *)Msg.data();
	assert(Head->nCmdType == EX_NMC_GET_MATRIX_INFO_RSP);

	tinyxml2::XMLDocument RspDoc(true, COLLAPSE_WHITESPACE);
	const char* data = Msg.data()+sizeof(ExCommand);
	if(RspDoc.Parse(data) != XML_NO_ERROR)
	{
		return -1;
	}

	tinyxml2::XMLElement *pRootElement = RspDoc.RootElement();
	if (NULL == pRootElement)
	{
		return -21;
	}
	strCache = SAFE_ITEM(pRootElement->Attribute("Cache"));

	tinyxml2::XMLElement* RetValNode = pRootElement->FirstChildElement("RetVal");
	if(RetValNode==NULL)
	{
		return -3; 
	}

	if(RetValNode->QueryIntAttribute("Code", &nRetCode)!=XML_NO_ERROR)
	{
		return -4;
	}

	strRemark = SAFE_ITEM(RetValNode->Attribute("Remark"));

	tinyxml2::XMLElement* MatrixInfoListNode = pRootElement->FirstChildElement("MatrixInfoList");
	if(MatrixInfoListNode==NULL)
	{
		return -1;
	}
	
	tinyxml2::XMLElement* MatrixInfoNode = MatrixInfoListNode->FirstChildElement("MatrixInfo");
	while(MatrixInfoNode!=NULL)
	{		
		struct stMatrixInfo  MatrixInfo;
		if(MatrixInfoNode->QueryIntAttribute("MatrixId", &MatrixInfo.Id)!=XML_NO_ERROR)
		{}

		if(MatrixInfoNode->QueryIntAttribute("MatrixStatus", &MatrixInfo.Status)!=XML_NO_ERROR)
		{}

		const char* cMatrixName = MatrixInfoNode->Attribute("MatrixName");
		MatrixInfo.Name = cMatrixName;

		const char* cIp = MatrixInfoNode->Attribute("Ip");
		MatrixInfo.Ip = cIp;

		if(MatrixInfoNode->QueryIntAttribute("Port", &MatrixInfo.Port)!=XML_NO_ERROR)
		{}

		const char* cUsername = MatrixInfoNode->Attribute("Username");
		MatrixInfo.User = cUsername;

		const char* cPassword = MatrixInfoNode->Attribute("Password");
		MatrixInfo.Password = cPassword;
		
		tinyxml2::XMLElement* OutputInfoListNode = MatrixInfoNode->FirstChildElement("OutputInfoList");
		if(OutputInfoListNode==NULL)
		{
			MatrixInfoNode = MatrixInfoNode->NextSiblingElement("MatrixInfo");
			continue;
		}

		tinyxml2::XMLElement* OutputInfoNode = OutputInfoListNode->FirstChildElement("OutputInfo");
		while(OutputInfoNode)
		{
			struct stOutputnfo Outputnfo;
			if(OutputInfoNode->QueryIntAttribute("OutputId", &Outputnfo.Id)!=XML_NO_ERROR)
			{
			
			}

			if(OutputInfoNode->QueryIntAttribute("Index", &Outputnfo.Index)!=XML_NO_ERROR)
			{

			}

			if(OutputInfoNode->QueryIntAttribute("HasDisplay", &Outputnfo.HasDisplay)!=XML_NO_ERROR)
			{

			}

			const char* cSupportedResolution = OutputInfoNode->Attribute("SupportedResolution");
			if(cSupportedResolution)
			{
				Outputnfo.Resolution = cSupportedResolution;
			}
			
			MatrixInfo.OutputInfoS.push_back(Outputnfo);

			OutputInfoNode = OutputInfoNode->NextSiblingElement("OutputInfo");
		}

		MatrixInfoS.push_back(MatrixInfo);
		
		MatrixInfoNode = MatrixInfoNode->NextSiblingElement("MatrixInfo");
	}

	return 0;
}
int NmcMessage::BuildSetMatrixReqMsg(enum en_nmc_operator_type operator_type, struct st_matrix_info *matrix_info, char* Cookie, string &Msg)
{
/*
<?xml version="1.0" encoding="utf-8"?> 
<ExSetMatrixReq Cache=????>
<MatrixInfo OpType="0" MatrixId="-1" MatrixName="" Ip="" Port="" Username="" Password=""/> 
//OpType??0????1????2????????????MatrixId?-1
</ExSetMatrixReq>
*/
	tinyxml2::XMLDocument ReqDoc;

	if(XML_NO_ERROR!=ReqDoc.Parse("<ExSetMatrixReq/>\n"))//
	{
		
		return -1;
	}

	tinyxml2::XMLElement *pRootElement = ReqDoc.RootElement();
	ADD_ATTR(pRootElement,Cache,Cookie);

	ADD_NODE(ReqDoc,MatrixInfo,pRootElement);

	ADD_ATTR(MatrixInfoNode,OpType,operator_type);
	ADD_ATTR(MatrixInfoNode,MatrixId,matrix_info->id);
	ADD_ATTR(MatrixInfoNode,MatrixName,matrix_info->name);
	ADD_ATTR(MatrixInfoNode,Ip,matrix_info->ip);
	ADD_ATTR(MatrixInfoNode,Port,matrix_info->port);
	ADD_ATTR(MatrixInfoNode,Username,matrix_info->user);
	ADD_ATTR(MatrixInfoNode,Password,matrix_info->password);

	tinyxml2::XMLPrinter streamer;
	ReqDoc.Accept(&streamer);
	string xmlstr(streamer.CStr());

	ExCommand Head;
	Head.nSrcType				= EX_SERVER_NMC;//ICMS_CLIENT_MONITOR;
	Head.nCmdType				= EX_NMC_SET_MATRIX_REQ;
	Head.nCmdSeq				= ++static_nCmdSeq;
	Head.nContentSize			= xmlstr.size()+1;

	Msg.assign((char*)&Head, sizeof(Head));

	Msg.append(xmlstr.c_str(), xmlstr.size());

	return Head.nCmdSeq;
}
int NmcMessage::ParserSetMatrixRspMsg(const string &Msg, std::string& strCache, enum en_nmc_operator_type *operator_type, struct st_matrix_info *matrix_info, int &nRetCode, std::string& strRemark)
{
/*
<?xml version="1.0" encoding="utf-8"?> 
<ExSetMatrixRsp Cache=????>
<RetVal Code="0" Remark="success"/>
<MatrixInfo OpType="0" MatrixId="1" MatrixStatus="1"/>  
// MatrixStatus  0????? 1????
</ExSetMatrixRsp>
*/
	ExCommand *Head = (ExCommand *)Msg.data();
	assert(Head->nCmdType == EX_NMC_SET_MATRIX_RSP);

	tinyxml2::XMLDocument RspDoc;
	const char* data = Msg.data()+sizeof(ExCommand);
	if(RspDoc.Parse(data) != XML_NO_ERROR)
	{
		return -1;
	}

	tinyxml2::XMLElement *pRootElement = RspDoc.RootElement();
	if (NULL == pRootElement)
	{
		return -21;
	}
	strCache = SAFE_ITEM(pRootElement->Attribute("Cache"));

	tinyxml2::XMLElement* RetValNode = pRootElement->FirstChildElement("RetVal");
	if(RetValNode==NULL)
	{
		return -3; 
	}
	if(RetValNode->QueryIntAttribute("Code", &nRetCode)!=XML_NO_ERROR)
	{
		return -4;
	}
	strRemark = SAFE_ITEM(RetValNode->Attribute("Remark"));

	tinyxml2::XMLElement* MatrixInfoNode = pRootElement->FirstChildElement("MatrixInfo");
	if(MatrixInfoNode->QueryIntAttribute("MatrixId", &matrix_info->id)!=XML_NO_ERROR)
	{
		return -4;
	}

	if(MatrixInfoNode->QueryIntAttribute("MatrixStatus", &matrix_info->status)!=XML_NO_ERROR)
	{
		return -5;
	}

	return 0;
}

int NmcMessage::BuildGetOutputReqMsg(int outputid, char* Cookie, string &Msg)
{
	/*
	<?xml version="1.0" encoding="utf-8"?>
	<ExGetOutputScreenLayoutReq Cache=????>
		<OutputInfo OutputId="1"/>
	</ExGetOutputScreenLayoutReq>
	*/

	const char* cReq = "<?xml version=\"1.0\" encoding=\"gb2312\"?>\n" 
		         "<ExGetOutputScreenLayoutReq/>\n";

	tinyxml2::XMLDocument ReqDoc;
	if(XML_NO_ERROR!=ReqDoc.Parse(cReq))//
	{
		
		return -1;
	}

	tinyxml2::XMLElement *pRootElement = ReqDoc.RootElement();
	ADD_ATTR(pRootElement,Cache,Cookie);

	ADD_NODE(ReqDoc,OutputInfo,pRootElement);
	ADD_ATTR(OutputInfoNode,OutputId,outputid);

	tinyxml2::XMLPrinter streamer;
	ReqDoc.Accept(&streamer);
	string xmlstr(streamer.CStr());

	ExCommand Head;
	Head.nSrcType				= EX_SERVER_NMC;//ICMS_CLIENT_MONITOR;
	Head.nCmdType				= EX_NMC_GET_OUTPUT_SCREEN_LAYOUT_REQ;
	Head.nCmdSeq				= ++static_nCmdSeq;
	Head.nContentSize			= xmlstr.size()+1;

	Msg.assign((char*)&Head, sizeof(Head));

	Msg.append(xmlstr.c_str(), xmlstr.size());

	return Head.nCmdSeq;
}
int NmcMessage::ParserGetOutputRspMsg(const string &Msg, std::string& strCache, struct st_output_layout **ppoutput_layout, int *output_layout_cnt, int &nRetCode, std::string& strRemark)
{
/*
<?xml version="1.0" encoding="gb2312" ?>
<ExGetOutputScreenLayoutRsp Cache="2450824">
<RetVal Code="0" Remark=""/>
<OutputInfoList>
	<OutputInfo OutputId="28" ScreenLayout="1">
		<WindowInfoList>
			<WindowInfo WndId="433" WndIndex="0" WndStatus="0" SourceId="-1" SurveyUuid="" Extend="-1"/>
		</WindowInfoList>
	</OutputInfo>
<OutputInfo OutputId="32" ScreenLayout="4">
<WindowInfoList>
<WindowInfo WndId="497" WndIndex="0" WndStatus="0" SourceId="-1" SurveyUuid="" Extend="-1"/>
<WindowInfo WndId="498" WndIndex="1" WndStatus="0" SourceId="-1" SurveyUuid="" Extend="-1"/>
<WindowInfo WndId="499" WndIndex="2" WndStatus="0" SourceId="-1" SurveyUuid="" Extend="-1"/>
<WindowInfo WndId="500" WndIndex="3" WndStatus="0" SourceId="-1" SurveyUuid="" Extend="-1"/>
</WindowInfoList>
</OutputInfo>
<OutputInfo OutputId="33" ScreenLayout="1">
<WindowInfoList>
<WindowInfo WndId="513" WndIndex="0" WndStatus="0" SourceId="-1" SurveyUuid="" Extend="-1"/>
</WindowInfoList>
</OutputInfo>
<OutputInfo OutputId="34" ScreenLayout="1">
<WindowInfoList>
<WindowInfo WndId="529" WndIndex="0" WndStatus="0" SourceId="-1" SurveyUuid="" Extend="-1"/>
</WindowInfoList>
</OutputInfo>
</OutputInfoList>
</ExGetOutputScreenLayoutRsp>
*/
	assert(output_layout_cnt);
	*output_layout_cnt = 0;

	ExCommand *Head = (ExCommand *)Msg.data();
	assert(Head->nCmdType == EX_NMC_GET_OUTPUT_SCREEN_LAYOUT_RSP);

	tinyxml2::XMLDocument RspDoc;
	const char* data = Msg.data()+sizeof(ExCommand);
	if(RspDoc.Parse(data) != XML_NO_ERROR)
	{
		
		return -1;
	}
	
	tinyxml2::XMLElement *pRootElement = RspDoc.RootElement();
	if (NULL == pRootElement)
	{
		return -21;
	}
	strCache = SAFE_ITEM(pRootElement->Attribute("Cache"));

	tinyxml2::XMLElement* RetValNode = pRootElement->FirstChildElement("RetVal");
	if(RetValNode==NULL)
	{
		return -3; 
	}

	if(RetValNode->QueryIntAttribute("Code", &nRetCode)!=XML_NO_ERROR)
	{
		return -4;
	}
	
	strRemark = SAFE_ITEM(RetValNode->Attribute("Remark"));

	const int max_output_info_cnt = 32;
	const int max_window_info_cnt = 32;

	tinyxml2::XMLElement* OutputInfoListNode = pRootElement->FirstChildElement("OutputInfoList");
	if(OutputInfoListNode==NULL)
	{
		return 0;
	}

	tinyxml2::XMLElement* OutputInfoNode = OutputInfoListNode->FirstChildElement("OutputInfo");
	if(OutputInfoNode==NULL)
	{
		return 0;
	}

	*ppoutput_layout = (struct st_output_layout *)calloc(max_output_info_cnt, sizeof(struct st_output_layout));

	int output_info_num = 0;
	while(OutputInfoNode!=NULL && output_info_num<max_output_info_cnt)
	{
		if(OutputInfoNode->QueryIntAttribute("OutputId", &(*ppoutput_layout)[output_info_num].id)!=XML_NO_ERROR)
		{}

		if(OutputInfoNode->QueryIntAttribute("ScreenLayout", &(*ppoutput_layout)[output_info_num].layout)!=XML_NO_ERROR)
		{}

		//const char* cIp = MatrixInfoNode->Attribute("Ip");
		//if(cIp) 
		//	strcpy((*ppmatrix_info)[matrix_info_num].ip, cIp);

		//if(MatrixInfoNode->QueryIntAttribute("Port", &(*ppmatrix_info)[matrix_info_num].port)!=XML_NO_ERROR)
		//{}

		tinyxml2::XMLElement* WindowInfoListNode = OutputInfoNode->FirstChildElement("WindowInfoList");
		if(WindowInfoListNode==NULL)
		{
			OutputInfoNode = OutputInfoNode->NextSiblingElement("OutputInfo");
			continue;
		}

		tinyxml2::XMLElement* WindowInfoNode = WindowInfoListNode->FirstChildElement("WindowInfo");
		(*ppoutput_layout)[output_info_num].window_info = NULL;
		if(WindowInfoNode)
			(*ppoutput_layout)[output_info_num].window_info = (struct st_window_info *)calloc(max_window_info_cnt, sizeof(struct st_window_info));

		struct st_window_info* window_info = (*ppoutput_layout)[output_info_num].window_info;
		int window_num = 0;
		while(WindowInfoNode && window_num<max_window_info_cnt)
		{
			int Value=0;
			if(WindowInfoNode->QueryIntAttribute("WndId", &Value)==XML_NO_ERROR)
			{
				window_info[window_num].id = Value;
			}

			if(WindowInfoNode->QueryIntAttribute("WndIndex", &Value)==XML_NO_ERROR)
			{
				window_info[window_num].index = Value;
			}

			if(WindowInfoNode->QueryIntAttribute("WndStatus", &Value)==XML_NO_ERROR)
			{
				window_info[window_num].status = Value;
			}

			if(WindowInfoNode->QueryIntAttribute("SourceId", &Value)==XML_NO_ERROR)
			{
				window_info[window_num].sourceid = Value;
			}

			if(WindowInfoNode->QueryIntAttribute("SubSourceId", &Value)==XML_NO_ERROR)
			{
				window_info[window_num].subsourceid = Value;
			}

			const char* cExtend = WindowInfoNode->Attribute("Extend");
			if(cExtend)
			{
				strcpy(window_info[window_num].Extend, cExtend);
			}

			++window_num;
			WindowInfoNode = WindowInfoNode->NextSiblingElement("WindowInfo");
		}

		(*ppoutput_layout)[output_info_num].window_info_cnt = window_num;

		output_info_num++;

		OutputInfoNode = OutputInfoNode->NextSiblingElement("OutputInfo");
	}

	*output_layout_cnt = output_info_num;

	//struct st_output_layout *p = *ppoutput_layout;
	return 0;
}
int NmcMessage::BuildSetOutputReqMsg(struct st_output_layout *output_layout, char* Cookie, string &Msg)
{
/*
<?xml version="1.0" encoding="utf-8"?>                       
<ExSetOutputScreenLayoutReq Cache=????>
<OutputInfo OutputId="1" ScreenLayout="4"/>
</ExSetOutputScreenLayoutReq>
*/
	tinyxml2::XMLDocument ReqDoc;

	if(XML_NO_ERROR!=ReqDoc.Parse("<ExSetOutputScreenLayoutReq/>\n"))//
	{
		
		return -1;
	}

	tinyxml2::XMLElement *pRootElement = ReqDoc.RootElement();
	ADD_ATTR(pRootElement,Cache,Cookie);

	ADD_NODE(ReqDoc,OutputInfo,pRootElement);

	ADD_ATTR(OutputInfoNode,OutputId,output_layout->id);
	ADD_ATTR(OutputInfoNode,ScreenLayout,output_layout->layout);

	XMLPrinter streamer;
	ReqDoc.Accept(&streamer);
	string xmlstr(streamer.CStr());

	ExCommand Head;
	Head.nSrcType				= EX_SERVER_NMC;//ICMS_CLIENT_MONITOR;
	Head.nCmdType				= EX_NMC_SET_OUTPUT_SCREEN_LAYOUT_REQ;
	Head.nCmdSeq				= ++static_nCmdSeq;
	Head.nContentSize			= xmlstr.size()+1;

	Msg.assign((char*)&Head, sizeof(Head));

	Msg.append(xmlstr.c_str(), xmlstr.size());

	return Head.nCmdSeq;
}
int NmcMessage::ParserSetOutputRspMsg(const string &Msg, std::string& strCache, struct st_output_layout *output_layout, int &nRetCode, std::string& strRemark)
{
/*
<?xml version="1.0" encoding="utf-8"?>                       
<ExSetOutputScreenLayoutRsp Cache=????>
<RetVal Code="0" Remark="success"/>
<OutputInfo OutputId="1" ScreenLayout="4"/>
</ExSetOutputScreenLayoutRsp>
*/
	ExCommand *Head = (ExCommand *)Msg.data();
	assert(Head->nCmdType == EX_NMC_SET_OUTPUT_SCREEN_LAYOUT_RSP);

	tinyxml2::XMLDocument RspDoc;
	const char* data = Msg.data()+sizeof(ExCommand);
	if(RspDoc.Parse(data) != XML_NO_ERROR)
	{
		return -1;
	}

	tinyxml2::XMLElement *pRootElement = RspDoc.RootElement();
	if (NULL == pRootElement)
	{
		return -21;
	}
	strCache = SAFE_ITEM(pRootElement->Attribute("Cache"));

	tinyxml2::XMLElement* RetValNode = pRootElement->FirstChildElement("RetVal");
	if(RetValNode==NULL)
	{
		return -3; 
	}

	if(RetValNode->QueryIntAttribute("Code", &nRetCode)!=XML_NO_ERROR)
	{
		return -4;
	}

	strRemark = SAFE_ITEM(RetValNode->Attribute("Remark"));

	tinyxml2::XMLElement* OutputInfoNode = pRootElement->FirstChildElement("OutputInfo");
	if(OutputInfoNode->QueryIntAttribute("OutputId", &output_layout->id)!=XML_NO_ERROR)
	{
		return -4;
	}

	if(OutputInfoNode->QueryIntAttribute("ScreenLayout", &output_layout->layout)!=XML_NO_ERROR)
	{
		return -5;
	}

	return 0;
}
int NmcMessage::BuildSetWindowSignalSourceReqMsg(int windowtype, int windowid, int sourceid, int subsourceid, char* Cookie, string &Msg)
{
/*
<?xml version="1.0" encoding="utf-8"?>
<ExSetWindowSignalSourceReq Cache=????>
<WindowInfo WndType="" WndId=""/>
<SignalSourceInfo SourceId="" SubSourceId=""/>
//SourceId?-1????????????
//??Source????NVR?????????????????SubSourceId?????????Id
</ExSetWindowSignalSourceReq>
*/
	tinyxml2::XMLDocument ReqDoc;

	if(XML_NO_ERROR!=ReqDoc.Parse("<ExSetWindowSignalSourceReq/>\n"))//
	{
		
		return -1;
	}

	tinyxml2::XMLElement *pRootElement = ReqDoc.RootElement();
	ADD_ATTR(pRootElement,Cache,Cookie);

	ADD_NODE(ReqDoc,WindowInfo,pRootElement);
	ADD_ATTR(WindowInfoNode,WndType,windowtype);
	ADD_ATTR(WindowInfoNode,WndId,windowid);

	ADD_NODE(ReqDoc,SignalSourceInfo,pRootElement);
	ADD_ATTR(SignalSourceInfoNode,SourceId,sourceid);
	ADD_ATTR(SignalSourceInfoNode,SubSourceId,subsourceid);
	
	tinyxml2::XMLPrinter streamer;
	ReqDoc.Accept(&streamer);
	string xmlstr(streamer.CStr());

	ExCommand Head;
	Head.nSrcType				= EX_SERVER_NMC;//ICMS_CLIENT_MONITOR;
	Head.nCmdType				= EX_NMC_SET_WINDOW_SIGNAL_SOURCE_REQ;
	Head.nCmdSeq				= ++static_nCmdSeq;
	Head.nContentSize			= xmlstr.size()+1;

	Msg.assign((char*)&Head, sizeof(Head));

	Msg.append(xmlstr.c_str(), xmlstr.size());

	return Head.nCmdSeq;
}
int NmcMessage::ParserSetWindowSignalSourceRspMsg(const string &Msg, std::string& strCache, int *windowtype, int *windowid, int &nRetCode, std::string& strRemark)
{
/*
<?xml version="1.0" encoding="utf-8"?>
<ExSetWindowSignalSourceRsp Cache=????>
<RetVal Code="0" Remark="success"/>
<WindowInfo WndType="" WndId=""/>
</ExSetWindowSignalSourceRsp>
*/
	ExCommand *Head = (ExCommand *)Msg.data();
	assert(Head->nCmdType == EX_NMC_SET_WINDOW_SIGNAL_SOURCE_RSP);

	tinyxml2::XMLDocument RspDoc;
	const char* data = Msg.data()+sizeof(ExCommand);
	if(RspDoc.Parse(data) != XML_NO_ERROR)
	{
		return -1;
	}
	tinyxml2::XMLElement *pRootElement = RspDoc.RootElement();
	if (NULL == pRootElement)
	{
		return -21;
	}
	strCache = SAFE_ITEM(pRootElement->Attribute("Cache"));
	tinyxml2::XMLElement* RetValNode = pRootElement->FirstChildElement("RetVal");
	if(RetValNode==NULL)
	{
		return -3; 
	}
	if(RetValNode->QueryIntAttribute("Code", &nRetCode)!=XML_NO_ERROR)
	{
		return -4;
	}
	strRemark = SAFE_ITEM(RetValNode->Attribute("Remark"));

	//////////////////

	tinyxml2::XMLElement* WindowInfoNode = pRootElement->FirstChildElement("WindowInfo");
	if(WindowInfoNode->QueryIntAttribute("WndType", windowtype)!=XML_NO_ERROR)
	{
		return -4;
	}

	if(WindowInfoNode->QueryIntAttribute("WndId", windowid)!=XML_NO_ERROR)
	{
		return -5;
	}

	return 0;
}
int NmcMessage::BuildClearWindowSignalSourceReqMsg(int windowtype, int outputid, int windowid, char* Cookie, string &ReqMsg)
{
/*
<?xml version="1.0" encoding="utf-8"?>
<ExClearWindowSignalSourceReq Cache=????>
<WindowInfo WndType="" OutputId="0" WndId="-1" />
//??WndId?-1???????????OutputId????????
</ExClearWindowSignalSourceReq>
*/
	tinyxml2::XMLDocument ReqDoc;

	if(XML_NO_ERROR!=ReqDoc.Parse("<ExClearWindowSignalSourceReq/>\n"))//
	{
		return -1;
	}

	tinyxml2::XMLElement *pRootElement = ReqDoc.RootElement();
	ADD_ATTR(pRootElement,Cache,Cookie);

	ADD_NODE(ReqDoc,WindowInfo,pRootElement);
	ADD_ATTR(WindowInfoNode,WndType,windowtype);
	ADD_ATTR(WindowInfoNode,WndId,windowid);
	ADD_ATTR(WindowInfoNode,OutputId,outputid);
	
	tinyxml2::XMLPrinter streamer;
	ReqDoc.Accept(&streamer);
	string xmlstr(streamer.CStr());

	ExCommand Head;
	Head.nSrcType				= EX_SERVER_NMC;//ICMS_CLIENT_MONITOR;
	Head.nCmdType				= EX_NMC_CLEAR_WINDOW_SIGNAL_SOURCE_REQ;
	Head.nCmdSeq				= ++static_nCmdSeq;
	Head.nContentSize			= xmlstr.size()+1;

	ReqMsg.assign((char*)&Head, sizeof(Head));

	ReqMsg.append(xmlstr.c_str(), xmlstr.size());

	return Head.nCmdSeq;
}
int NmcMessage::ParserClearWindowSignalSourceRspMsg(const string &RspMsg, std::string& strCache, int *windowtype, int *outputid, int *windowid, int &nRetCode, std::string& strRemark)
{
/*
<?xml version="1.0" encoding="utf-8"?>
<ExClearWindowSignalSourceRsp Cache=????>
<RetVal Code="0" Remark="success"/>
<WindowInfo WndType="" OutputId="0" WndId="" />
</ExClearWindowSignalSourceRsp>
*/
	ExCommand *Head = (ExCommand *)RspMsg.data();
	assert(Head->nCmdType == EX_NMC_CLEAR_WINDOW_SIGNAL_SOURCE_RSP);

	tinyxml2::XMLDocument RspDoc;
	const char* data = RspMsg.data()+sizeof(ExCommand);
	if(RspDoc.Parse(data) != XML_NO_ERROR)
	{
		return -1;
	}
	tinyxml2::XMLElement *pRootElement = RspDoc.RootElement();
	if (NULL == pRootElement)
	{
		return -21;
	}
	strCache = SAFE_ITEM(pRootElement->Attribute("Cache"));
	tinyxml2::XMLElement* RetValNode = pRootElement->FirstChildElement("RetVal");
	if(RetValNode==NULL)
	{
		return -3; 
	}
	if(RetValNode->QueryIntAttribute("Code", &nRetCode)!=XML_NO_ERROR)
	{
		return -4;
	}
	strRemark = SAFE_ITEM(RetValNode->Attribute("Remark"));

	//////////////////

	tinyxml2::XMLElement* WindowInfoNode = pRootElement->FirstChildElement("WindowInfo");
	if(WindowInfoNode->QueryIntAttribute("WndType", windowtype)!=XML_NO_ERROR)
	{
		return -4;
	}

	if(WindowInfoNode->QueryIntAttribute("WndId", windowid)!=XML_NO_ERROR)
	{
		return -5;
	}

	if(WindowInfoNode->QueryIntAttribute("OutputId", windowid)!=XML_NO_ERROR)
	{
		//return -5;
	}

	return 0;
}

int NmcMessage::BuildGetOutputMappingReqMsg(int mappingid, char* Cookie, string &ReqMsg)
{
/*
<?xml version="1.0" encoding="utf-8"?> 
<ExGetOutputMappingReq Cache=????>
	<MappingInfo MappingId="-1"/> //-1????????????????
</ExGetOutputMappingReq>
*/

	tinyxml2::XMLDocument ReqDoc;

	if(XML_NO_ERROR!=ReqDoc.Parse("<ExGetOutputMappingReq/>\n"))//
	{
		return -1;
	}

	tinyxml2::XMLElement *pRootElement = ReqDoc.RootElement();
	ADD_ATTR(pRootElement,Cache,Cookie);

	ADD_NODE(ReqDoc,MappingInfo,pRootElement);
	ADD_ATTR(MappingInfoNode,MappingId,mappingid);

	tinyxml2::XMLPrinter streamer;
	ReqDoc.Accept(&streamer);
	string xmlstr(streamer.CStr());

	ExCommand Head;
	Head.nSrcType				= EX_SERVER_NMC;//ICMS_CLIENT_MONITOR;
	Head.nCmdType				= EX_NMC_GET_OUTPUT_MAPPING_REQ;
	Head.nCmdSeq				= ++static_nCmdSeq;
	Head.nContentSize			= xmlstr.size()+1;

	ReqMsg.assign((char*)&Head, sizeof(Head));

	ReqMsg.append(xmlstr.c_str(), xmlstr.size());

	return Head.nCmdSeq;
}
int NmcMessage::ParserGetOutputMappingRspMsg(const string &RspMsg, std::string& strCache, struct st_output_mapping **mapping, int *mapping_cnt, int &nRetCode, std::string& strRemark)
{
/*

<?xml version="1.0" encoding="utf-8"?> 
<ExGetOutputMappingRsp Cache=????>
<RetVal Code="0" Remark="success"/>
<MappingInfoList>
<MappingInfo MappingId="1" OutputId="" MonitorId=""/>
??
</MappingInfoList>
</ExGetOutputMappingRsp>
*/
	ExCommand *Head = (ExCommand *)RspMsg.data();
	assert(Head->nCmdType == EX_NMC_GET_OUTPUT_MAPPING_RSP);

	tinyxml2::XMLDocument RspDoc;
	const char* data = RspMsg.data()+sizeof(ExCommand);
	if(RspDoc.Parse(data) != XML_NO_ERROR)
	{
		return -1;
	}
	tinyxml2::XMLElement *pRootElement = RspDoc.RootElement();
	if (NULL == pRootElement)
	{
		return -21;
	}
	strCache = SAFE_ITEM(pRootElement->Attribute("Cache"));
	tinyxml2::XMLElement* RetValNode = pRootElement->FirstChildElement("RetVal");
	if(RetValNode==NULL)
	{
		return -3; 
	}
	if(RetValNode->QueryIntAttribute("Code", &nRetCode)!=XML_NO_ERROR)
	{
		return -4;
	}
	strRemark = SAFE_ITEM(RetValNode->Attribute("Remark"));

	//////////////////

	tinyxml2::XMLElement* MappingInfoListNode = pRootElement->FirstChildElement("MappingInfoList");
	if(MappingInfoListNode==NULL)
		return 0;

	const int max_mapping_cnt = 32;

	tinyxml2::XMLElement* MappingInfoNode = MappingInfoListNode->FirstChildElement("MappingInfo");
	if(MappingInfoNode)
		*mapping = (struct st_output_mapping *)calloc(max_mapping_cnt, sizeof(struct st_output_mapping));

	int _mapping_cnt = 0;
	while(MappingInfoNode && _mapping_cnt<max_mapping_cnt)
	{
		if(RetValNode->QueryIntAttribute("MappingId", &((*mapping)[_mapping_cnt].mappingid))!=XML_NO_ERROR)
		{
			return -4;
		}

		if(RetValNode->QueryIntAttribute("OutputId", &((*mapping)[_mapping_cnt].outputid))!=XML_NO_ERROR)
		{
			return -3;
		}

		if(RetValNode->QueryIntAttribute("MonitorId", &((*mapping)[_mapping_cnt].monitorid))!=XML_NO_ERROR)
		{
			return -34;
		}

		_mapping_cnt++;

		MappingInfoNode = MappingInfoNode->NextSiblingElement("MappingInfo");
	}

	*mapping_cnt = _mapping_cnt;

	return 0;
}
int NmcMessage::BuildSetOutputMappingReqMsg(struct st_output_mapping *mapping, int mapping_cnt, char* Cookie, string &ReqMsg)
{
	/*
	<?xml version="1.0" encoding="utf-8"?> 
	<ExSetOutputMappingReq Cache=????>
	<MappingInfoList>
	<MappingInfo OpType="0" OpSeq="1" MappingId="-1" OutputId="" MonitorId=""/>
	//OpType??0????1????2????????????MappingId?-1
	??
	<MappingInfoList>
	</ExSetOutputMappingReq>
	*/

	tinyxml2::XMLDocument ReqDoc;

	if(XML_NO_ERROR!=ReqDoc.Parse("<ExSetOutputMappingReq/>\n"))//
	{
		return -1;
	}

	tinyxml2::XMLElement *pRootElement = ReqDoc.RootElement();
	ADD_ATTR(pRootElement,Cache,Cookie);

	ADD_NODE(ReqDoc,MappingInfoList,pRootElement);

	for(int i=0; i<mapping_cnt; ++i)
	{
		ADD_NODE(ReqDoc,MappingInfo,MappingInfoListNode);
		ADD_ATTR(MappingInfoNode,OpType,nmc_add);//???????????????????????????????????????????????????????????????
		ADD_ATTR(MappingInfoNode,OpSeq,i);


	}

	

	tinyxml2::XMLPrinter streamer;
	ReqDoc.Accept(&streamer);
	string xmlstr(streamer.CStr());

	ExCommand Head;
	Head.nSrcType				= EX_SERVER_NMC;//ICMS_CLIENT_MONITOR;
	Head.nCmdType				= EX_NMC_SET_OUTPUT_MAPPING_REQ;
	Head.nCmdSeq				= ++static_nCmdSeq;
	Head.nContentSize			= xmlstr.size()+1;

	ReqMsg.assign((char*)&Head, sizeof(Head));

	ReqMsg.append(xmlstr.c_str(), xmlstr.size());

	return Head.nCmdSeq;
}
int NmcMessage::ParserSetOutputMappingRspMsg(const string &RspMsg, std::string& strCache, struct st_output_mapping **mapping, int *mapping_cnt, int &nRetCode, std::string& strRemark)
{
	/*
	EX_NMC_SET_OUTPUT_MAPPING_RSP
	<?xml version="1.0" encoding="utf-8"?> 
	<ExSetOutputMappingRsp Cache=????>
	<RetVal Code="0" Remark="success"/>
	<MappingInfoList>
	<MappingInfo OpType="0" OpSeq="1" MappingId="1"/>
	??
	</MappingInfoList>
	</ExSetOutputMappingRsp>
	*/
	ExCommand *Head = (ExCommand *)RspMsg.data();
	assert(Head->nCmdType == EX_NMC_SET_OUTPUT_MAPPING_RSP);

	tinyxml2::XMLDocument RspDoc;
	const char* data = RspMsg.data()+sizeof(ExCommand);
	if(RspDoc.Parse(data) != XML_NO_ERROR)
	{
		return -1;
	}
	tinyxml2::XMLElement *pRootElement = RspDoc.RootElement();
	if (NULL == pRootElement)
	{
		return -21;
	}
	strCache = SAFE_ITEM(pRootElement->Attribute("Cache"));
	tinyxml2::XMLElement* RetValNode = pRootElement->FirstChildElement("RetVal");
	if(RetValNode==NULL)
	{
		return -3; 
	}
	if(RetValNode->QueryIntAttribute("Code", &nRetCode)!=XML_NO_ERROR)
	{
		return -4;
	}
	strRemark = SAFE_ITEM(RetValNode->Attribute("Remark"));

	//////////////////

	tinyxml2::XMLElement* MappingInfoListNode = pRootElement->FirstChildElement("MappingInfoList");
	if(MappingInfoListNode==NULL)
		return 0;

	const int max_mapping_cnt = 32;

	tinyxml2::XMLElement* MappingInfoNode = pRootElement->FirstChildElement("MappingInfo");
	if(MappingInfoNode)
		*mapping = (struct st_output_mapping *)calloc(max_mapping_cnt, sizeof(struct st_output_mapping));

	int _mapping_cnt = 0;
	while(MappingInfoNode && _mapping_cnt<max_mapping_cnt)
	{
		int OpType = 0;
		int OpSeq = 0;
		int MappingId = 0;
		if(RetValNode->QueryIntAttribute("OpType", &OpType)!=XML_NO_ERROR)
		{
			return -4;
		}

		if(RetValNode->QueryIntAttribute("OpSeq", &OpSeq)!=XML_NO_ERROR)
		{
			return -3;
		}

		if(RetValNode->QueryIntAttribute("MappingId", &MappingId)!=XML_NO_ERROR)
		{
			return -34;
		}

		_mapping_cnt++;

		MappingInfoNode = MappingInfoNode->NextSiblingElement("MappingInfo");
	}

	*mapping_cnt = _mapping_cnt;

	return 0;
}
int NmcMessage::BuildGetSignalSourcePictureParaReqMsg(int sourceid, int subsourceid, char* Cookie, string &ReqMsg)
{
/*
<?xml version="1.0" encoding="utf-8"?>
<ExGetSignalSourcePictureParaReq Cache="">
<SourceInfo SourceId="" SubSourceId=""/> 
// SourceId?-1????????????????????
</ExGetSignalSourcePictureParaReq>
*/
	tinyxml2::XMLDocument ReqDoc;

	if(XML_NO_ERROR!=ReqDoc.Parse("<ExGetSignalSourcePictureParaReq/>\n"))//
	{
		return -1;
	}

	tinyxml2::XMLElement *pRootElement = ReqDoc.RootElement();
	ADD_ATTR(pRootElement,Cache,Cookie);

	ADD_NODE(ReqDoc,SourceInfo,pRootElement);
	ADD_ATTR(SourceInfoNode,SourceId,sourceid);
	ADD_ATTR(SourceInfoNode,SubSourceId,subsourceid);

	tinyxml2::XMLPrinter streamer;
	ReqDoc.Accept(&streamer);
	string xmlstr(streamer.CStr());

	ExCommand Head;
	Head.nSrcType				= EX_SERVER_NMC;//ICMS_CLIENT_MONITOR;
	Head.nCmdType				= EX_NMC_GET_SIGNAL_SOURCE_PICTURE_PARA_REQ;
	Head.nCmdSeq				= ++static_nCmdSeq;
	Head.nContentSize			= xmlstr.size()+1;

	ReqMsg.assign((char*)&Head, sizeof(Head));

	ReqMsg.append(xmlstr.c_str(), xmlstr.size());

	return Head.nCmdSeq;
}
int NmcMessage::ParserGetSignalSourcePictureParaRspMsg(const string &RspMsg, std::string& strCache, struct st_output_mapping **mapping, int *mapping_cnt, int &nRetCode, std::string& strRemark)
{
/*
EX_NMC_GET_SIGNAL_SOURCE_PICTURE_PARA_RSP
<?xml version="1.0" encoding="utf-8"?>
<ExGetSignalSourcePictureParaRsp Cache="">
<RetVal Code="0" Remark="success"/>
<PicParaList>
<PicPara SourceId="" SubSourceId="" Brightness="" Contrast="" Saturation="" Hue="" Sharpness="" NoiseReduction="" Extend=""/>
??
</PicParaList>
</ExGetSignalSourcePictureParaRsp>
*/
	ExCommand *Head = (ExCommand *)RspMsg.data();
	assert(Head->nCmdType == EX_NMC_GET_SIGNAL_SOURCE_PICTURE_PARA_RSP);

	tinyxml2::XMLDocument RspDoc;
	const char* data = RspMsg.data()+sizeof(ExCommand);
	if(RspDoc.Parse(data) != XML_NO_ERROR)
	{
		return -1;
	}
	tinyxml2::XMLElement *pRootElement = RspDoc.RootElement();
	if (NULL == pRootElement)
	{
		return -21;
	}
	strCache = SAFE_ITEM(pRootElement->Attribute("Cache"));
	tinyxml2::XMLElement* RetValNode = pRootElement->FirstChildElement("RetVal");
	if(RetValNode==NULL)
	{
		return -3; 
	}
	if(RetValNode->QueryIntAttribute("Code", &nRetCode)!=XML_NO_ERROR)
	{
		return -4;
	}
	strRemark = SAFE_ITEM(RetValNode->Attribute("Remark"));

	//////////////////

	tinyxml2::XMLElement* PicParaListNode = pRootElement->FirstChildElement("PicParaList");
	if(PicParaListNode==NULL)
		return 0;

	const int max_mapping_cnt = 32;

	tinyxml2::XMLElement* PicParaNode = PicParaListNode->FirstChildElement("PicPara");
	if(PicParaNode)
		*mapping = (struct st_output_mapping *)calloc(max_mapping_cnt, sizeof(struct st_output_mapping));

	int _mapping_cnt = 0;
	while(PicParaNode && _mapping_cnt<max_mapping_cnt)
	{
		int OpType = 0;
		int OpSeq = 0;
		int MappingId = 0;
		if(RetValNode->QueryIntAttribute("OpType", &OpType)!=XML_NO_ERROR)
		{
			return -4;
		}

		if(RetValNode->QueryIntAttribute("OpSeq", &OpSeq)!=XML_NO_ERROR)
		{
			return -3;
		}

		if(RetValNode->QueryIntAttribute("MappingId", &MappingId)!=XML_NO_ERROR)
		{
			return -34;
		}

		_mapping_cnt++;

		PicParaNode = PicParaNode->NextSiblingElement("PicPara");
	}

	*mapping_cnt = _mapping_cnt;

	return 0;
}
int NmcMessage::BuildGetLargeScreenReqMsg(int ScreenId, char* Cookie, string &ReqMsg)
{
/*
??????????????????????EX_GET_LARGE_SCREEN_REQ
<?xml version="1.0" encoding="utf-8"?>
<ExGetLargeScreenReq Cache="">
<LargeScreenInfo ScreenId="-1" /> //-1??????????????????????
</ExGetLargeScreenReq>
*/
	tinyxml2::XMLDocument ReqDoc;

	if(XML_NO_ERROR!=ReqDoc.Parse("<ExGetLargeScreenReq/>\n"))//
	{
		return -1;
	}

	tinyxml2::XMLElement *pRootElement = ReqDoc.RootElement();
	ADD_ATTR(pRootElement,Cache,Cookie);

	ADD_NODE(ReqDoc,LargeScreenInfo,pRootElement);
	ADD_ATTR(LargeScreenInfoNode,ScreenId,ScreenId);

	tinyxml2::XMLPrinter streamer;
	ReqDoc.Accept(&streamer);
	string xmlstr(streamer.CStr());

	ExCommand Head;
	Head.nSrcType				= EX_SERVER_NMC;//ICMS_CLIENT_MONITOR;
	Head.nCmdType				= EX_NMC_GET_LARGE_SCREEN_REQ;
	Head.nCmdSeq				= ++static_nCmdSeq;
	Head.nContentSize			= xmlstr.size()+1;

	ReqMsg.assign((char*)&Head, sizeof(Head));

	ReqMsg.append(xmlstr.c_str(), xmlstr.size());

	return Head.nCmdSeq;
}
int NmcMessage::ParserGetLargeScreenRspMsg(const string &RspMsg, std::string& strCache, struct st_large_screen_info **large_screen_info, int *large_screen_info_cnt, int &nRetCode, std::string& strRemark)
{
/*
<?xml version="1.0" encoding="utf-8"?>
<ExGetLargeScreenRsp Cache="">
<RetVal Code="0" Remark="success"/>
	<LargeScreenInfoList>
		<LargeScreenInfo ScreenId="" ScreenStatus="1" ScreenName="square9" Rows="3" Cols="3"/> 
			// ScreenStatus 0????1???????????RTSP?????????2????3????????????4??????????5?GIS??6?????
			<SignalSourceInfo SourceId="" SubSourceId=""/>
			//SourceId?-1????????????
			//??Source????NVR?????????????????SubSourceId?????????Id
			<SubScreenInfoList>
				<SubScreenInfo OutputId="" Row="0" Col="0" />
				<SubScreenInfo OutputId="" Row="0" Col="1" />
			</SubScreenInfoList>
		</LargeScreenInfo>
	</LargeScreenInfoList>
</ExGetLargeScreenRsp>	
*/
	assert(large_screen_info_cnt);
	*large_screen_info_cnt = 0;

	ExCommand *Head = (ExCommand *)RspMsg.data();
	assert(Head->nCmdType == EX_NMC_GET_LARGE_SCREEN_RSP);

	tinyxml2::XMLDocument RspDoc;
	const char* data = RspMsg.data()+sizeof(ExCommand);
	if(RspDoc.Parse(data) != XML_NO_ERROR)
	{
		return -1;
	}
	
	tinyxml2::XMLElement *pRootElement = RspDoc.RootElement();
	if (NULL == pRootElement)
	{
		return -21;
	}
	strCache = SAFE_ITEM(pRootElement->Attribute("Cache"));

	tinyxml2::XMLElement* RetValNode = pRootElement->FirstChildElement("RetVal");
	if(RetValNode==NULL)
	{
		return -3; 
	}

	if(RetValNode->QueryIntAttribute("Code", &nRetCode)!=XML_NO_ERROR)
	{
		return -4;
	}

	strRemark = SAFE_ITEM(RetValNode->Attribute("Remark"));

	const int max_large_screen_info_cnt = 32;
	const int max_subscreen_info_cnt = 32;

	tinyxml2::XMLElement* LargeScreenInfoListNode = pRootElement->FirstChildElement("LargeScreenInfoList");
	if(LargeScreenInfoListNode==NULL)
		return 0;

	tinyxml2::XMLElement* LargeScreenInfoNode = LargeScreenInfoListNode->FirstChildElement("LargeScreenInfo");
	if(LargeScreenInfoNode)
		*large_screen_info = (struct st_large_screen_info *)calloc(max_large_screen_info_cnt, sizeof(struct st_large_screen_info));
	else
		return 0;

	int _large_screen_info_cnt = 0;
	while(LargeScreenInfoNode!=NULL && _large_screen_info_cnt<max_large_screen_info_cnt)
	{
		if(LargeScreenInfoNode->QueryIntAttribute("ScreenId", &(*large_screen_info)[_large_screen_info_cnt].screenid)!=XML_NO_ERROR)
		{}

		if(LargeScreenInfoNode->QueryIntAttribute("ScreenStatus", &(*large_screen_info)[_large_screen_info_cnt].screenstatus)!=XML_NO_ERROR)
		{}

		const char *cScreenName = SAFE_ITEM(LargeScreenInfoNode->Attribute("ScreenName"));
		strncpy((*large_screen_info)[_large_screen_info_cnt].screenname, cScreenName, 63);

		if(LargeScreenInfoNode->QueryIntAttribute("Rows", &(*large_screen_info)[_large_screen_info_cnt].rows)!=XML_NO_ERROR)
		{}

		if(LargeScreenInfoNode->QueryIntAttribute("Cols", &(*large_screen_info)[_large_screen_info_cnt].cols)!=XML_NO_ERROR)
		{}

		tinyxml2::XMLElement* SignalSourceInfoNode = LargeScreenInfoNode->FirstChildElement("SignalSourceInfo");
		if(SignalSourceInfoNode)
		{
			if(SignalSourceInfoNode->QueryIntAttribute("SourceId", &(*large_screen_info)[_large_screen_info_cnt].sourceid)!=XML_NO_ERROR)
			{}

			if(SignalSourceInfoNode->QueryIntAttribute("SubSourceId", &(*large_screen_info)[_large_screen_info_cnt].subsourceid)!=XML_NO_ERROR)
			{}
		}
		else
		{
			(*large_screen_info)[_large_screen_info_cnt].sourceid = -1;
			(*large_screen_info)[_large_screen_info_cnt].subsourceid = -1;
		}

		tinyxml2::XMLElement* SubScreenInfoListNode = LargeScreenInfoNode->FirstChildElement("SubScreenInfoList");
		if(SubScreenInfoListNode==NULL)
		{
			LargeScreenInfoNode = LargeScreenInfoNode->NextSiblingElement("LargeScreenInfo");
			continue;
		}

		tinyxml2::XMLElement* SubScreenInfoNode = SubScreenInfoListNode->FirstChildElement("SubScreenInfo");
		(*large_screen_info)[_large_screen_info_cnt].psubscreen_info = NULL;
		if(SubScreenInfoNode)
			(*large_screen_info)[_large_screen_info_cnt].psubscreen_info = (struct st_subscreen_info *)calloc(max_subscreen_info_cnt, sizeof(struct st_subscreen_info));

		struct st_subscreen_info* psubscreen_info = (*large_screen_info)[_large_screen_info_cnt].psubscreen_info;
		int subscreen_info_num = 0;
		while(SubScreenInfoNode && subscreen_info_num<max_subscreen_info_cnt)
		{
			int Value=0;
			if(SubScreenInfoNode->QueryIntAttribute("OutputId", &Value)==XML_NO_ERROR)
			{
				psubscreen_info[subscreen_info_num].OutputId = Value;
			}

			if(SubScreenInfoNode->QueryIntAttribute("Row", &Value)==XML_NO_ERROR)
			{
				psubscreen_info[subscreen_info_num].Row = Value;
			}

			if(SubScreenInfoNode->QueryIntAttribute("Col", &Value)==XML_NO_ERROR)
			{
				psubscreen_info[subscreen_info_num].Col = Value;
			}
			
			++subscreen_info_num;
			SubScreenInfoNode = SubScreenInfoNode->NextSiblingElement("SubScreenInfo");
		}

		(*large_screen_info)[_large_screen_info_cnt].subscreen_info_cnt = subscreen_info_num;

		_large_screen_info_cnt++;

		LargeScreenInfoNode = LargeScreenInfoNode->NextSiblingElement("LargeScreenInfo");
	}

	*large_screen_info_cnt = _large_screen_info_cnt;

	return 0;
}
int NmcMessage::BuildAddLargeScreenReqMsg(struct st_large_screen_info *large_screen_info, char* Cookie, string &ReqMsg)
{
/*
?????????????????????EX_ADD_LARGE_SCREEN_REQ
<?xml version="1.0" encoding="utf-8"?>
<ExAddLargeScreenReq Cache="">
<LargeScreenInfo ScreenName="square9" Rows="3" Cols="3">
<SubScreenInfoList>
	<SubScreenInfo OutputId="" Row="0" Col="0" />
	<SubScreenInfo OutputId="" Row="0" Col="1" />
</SubScreenInfoList>
</LargeScreenInfo>
</ExAddLargeScreenReq>
*/
	tinyxml2::XMLDocument ReqDoc;

	if(XML_NO_ERROR!=ReqDoc.Parse("<ExAddLargeScreenReq/>\n"))//
	{
		return -1;
	}

	tinyxml2::XMLElement *pRootElement = ReqDoc.RootElement();
	ADD_ATTR(pRootElement,Cache,Cookie);

	ADD_NODE(ReqDoc,LargeScreenInfo,pRootElement);
	ADD_ATTR(LargeScreenInfoNode,ScreenName,large_screen_info[0].screenname);
	ADD_ATTR(LargeScreenInfoNode,Rows,large_screen_info[0].rows);
	ADD_ATTR(LargeScreenInfoNode,Cols,large_screen_info[0].cols);

	ADD_NODE(ReqDoc,SubScreenInfoList,LargeScreenInfoNode);
	for(int i=0; i<large_screen_info->subscreen_info_cnt; ++i)
	{
		ADD_NODE(ReqDoc,SubScreenInfo,SubScreenInfoListNode);
		ADD_ATTR(SubScreenInfoNode,OutputId,large_screen_info->psubscreen_info[i].OutputId);
		ADD_ATTR(SubScreenInfoNode,Row,large_screen_info->psubscreen_info[i].Row);
		ADD_ATTR(SubScreenInfoNode,Col,large_screen_info->psubscreen_info[i].Col);
	}
	
	tinyxml2::XMLPrinter streamer;
	ReqDoc.Accept(&streamer);
	string xmlstr(streamer.CStr());

	ExCommand Head;
	Head.nSrcType				= EX_SERVER_NMC;//ICMS_CLIENT_MONITOR;
	Head.nCmdType				= EX_NMC_ADD_LARGE_SCREEN_REQ;
	Head.nCmdSeq				= ++static_nCmdSeq;
	Head.nContentSize			= xmlstr.size()+1;

	ReqMsg.assign((char*)&Head, sizeof(Head));

	ReqMsg.append(xmlstr.c_str(), xmlstr.size());

	return Head.nCmdSeq;
}
int NmcMessage::ParserAddLargeScreenRspMsg(const string &RspMsg, std::string& strCache, int *ScreenId, int &nRetCode, std::string& strRemark)
{
/*
?????????????????????????EX_Add_LARGE_SCREEN_RSP
<?xml version="1.0" encoding="utf-8"?>
<ExAddLargeScreenRsp Cache="">
<RetVal Code="0" Remark="success"/>
<LargeScreenInfo ScreenId="1" />
</ExAddLargeScreenRsp>
*/
	ExCommand *Head = (ExCommand *)RspMsg.data();
	assert(Head->nCmdType == EX_NMC_ADD_LARGE_SCREEN_RSP);

	tinyxml2::XMLDocument RspDoc;
	const char* data = RspMsg.data()+sizeof(ExCommand);
	if(RspDoc.Parse(data) != XML_NO_ERROR)
	{
		return -1;
	}
	tinyxml2::XMLElement *pRootElement = RspDoc.RootElement();
	if (NULL == pRootElement)
	{
		return -21;
	}
	strCache = SAFE_ITEM(pRootElement->Attribute("Cache"));
	tinyxml2::XMLElement* RetValNode = pRootElement->FirstChildElement("RetVal");
	if(RetValNode==NULL)
	{
		return -3; 
	}
	if(RetValNode->QueryIntAttribute("Code", &nRetCode)!=XML_NO_ERROR)
	{
		return -4;
	}
	strRemark = SAFE_ITEM(RetValNode->Attribute("Remark"));

	//////////////////

	tinyxml2::XMLElement* LargeScreenInfoNode = pRootElement->FirstChildElement("LargeScreenInfo");
	if(LargeScreenInfoNode==NULL)
		return 0;

	if(LargeScreenInfoNode->QueryIntAttribute("ScreenId", ScreenId)!=XML_NO_ERROR)
	{
		return -4;
	}

	return 0;
}
int NmcMessage::BuildRmvLargeScreenReqMsg(int ScreenId, char* Cookie, string &ReqMsg)
{
/*
??????????????????????EX_RMV_LARGE_SCREEN_REQ
<?xml version="1.0" encoding="utf-8"?>
<ExRmvLargeScreenReq Cache="">
<LargeScreenInfo ScreenId="1" /> 
</ExRmvLargeScreenReq>
*/
	tinyxml2::XMLDocument ReqDoc;

	if(XML_NO_ERROR!=ReqDoc.Parse("<ExRmvLargeScreenReq/>\n"))//
	{
		return -1;
	}

	tinyxml2::XMLElement *pRootElement = ReqDoc.RootElement();
	ADD_ATTR(pRootElement,Cache,Cookie);

	ADD_NODE(ReqDoc,LargeScreenInfo,pRootElement);
	ADD_ATTR(LargeScreenInfoNode,ScreenId,ScreenId);

	tinyxml2::XMLPrinter streamer;
	ReqDoc.Accept(&streamer);
	string xmlstr(streamer.CStr());

	ExCommand Head;
	Head.nSrcType				= EX_SERVER_NMC;//ICMS_CLIENT_MONITOR;
	Head.nCmdType				= EX_NMC_RMV_LARGE_SCREEN_REQ;
	Head.nCmdSeq				= ++static_nCmdSeq;
	Head.nContentSize			= xmlstr.size()+1;

	ReqMsg.assign((char*)&Head, sizeof(Head));

	ReqMsg.append(xmlstr.c_str(), xmlstr.size());

	return Head.nCmdSeq;
}
int NmcMessage::ParserRmvLargeScreenRspMsg(const string &RspMsg, std::string& strCache, int &ScreenId, int &nRetCode, std::string& strRemark)
{
/*
??????????????????????????EX_NMC_RMV_LARGE_SCREEN_RSP
<?xml version="1.0" encoding="utf-8"?>
<ExRmvLargeScreenRsp Cache="">
<RetVal Code="0" Remark="success"/>
<LargeScreenInfo ScreenId="1" />
</ExRmvLargeScreenRsp>
*/
	ExCommand *Head = (ExCommand *)RspMsg.data();
	assert(Head->nCmdType == EX_NMC_RMV_LARGE_SCREEN_RSP);

	tinyxml2::XMLDocument RspDoc;
	const char* data = RspMsg.data()+sizeof(ExCommand);
	if(RspDoc.Parse(data) != XML_NO_ERROR)
	{
		return -1;
	}
	tinyxml2::XMLElement *pRootElement = RspDoc.RootElement();
	if (NULL == pRootElement)
	{
		return -21;
	}
	strCache = SAFE_ITEM(pRootElement->Attribute("Cache"));
	tinyxml2::XMLElement* RetValNode = pRootElement->FirstChildElement("RetVal");
	if(RetValNode==NULL)
	{
		return -3; 
	}
	if(RetValNode->QueryIntAttribute("Code", &nRetCode)!=XML_NO_ERROR)
	{
		return -4;
	}
	strRemark = SAFE_ITEM(RetValNode->Attribute("Remark"));

	//////////////////

	tinyxml2::XMLElement* LargeScreenInfoNode = pRootElement->FirstChildElement("LargeScreenInfo");
	if(LargeScreenInfoNode==NULL)
		return -1;


	if(LargeScreenInfoNode->QueryIntAttribute("ScreenId", &ScreenId)!=XML_NO_ERROR)
	{
		return -4;
	}

	return 0;
}
//????????????
int NmcMessage::BuildSetWindowPictureParaReqMsg(int WndType, int WndId, struct st_pic_para_info *ppic_para_info, char* Cookie, string &ReqMsg)
{
/*
????????????????EX_SET_WINDOW_PICTURE_PARA_REQ
<?xml version="1.0" encoding="utf-8"?>
<ExSetWindowPictureParaReq Cache=????>
<WindowInfo WndType="" WndId="" />
<PicParaInfo Brightness="" Contrast="" Saturation="" Hue="" Sharpness="" NoiseReduction="" /> 
</ExSetWindowPictureParaReq>
*/
	tinyxml2::XMLDocument ReqDoc;

	if(XML_NO_ERROR!=ReqDoc.Parse("<ExSetWindowPictureParaReq/>\n"))//
	{
		return -1;
	}

	tinyxml2::XMLElement *pRootElement = ReqDoc.RootElement();
	ADD_ATTR(pRootElement,Cache,Cookie);

	ADD_NODE(ReqDoc,WindowInfo,pRootElement);
	ADD_ATTR(WindowInfoNode,WndType,WndType);
	ADD_ATTR(WindowInfoNode,WndId,WndId);

	ADD_NODE(ReqDoc,PicParaInfo,pRootElement);
	ADD_ATTR(PicParaInfoNode,Brightness,ppic_para_info->brightness);
	ADD_ATTR(PicParaInfoNode,Contrast,ppic_para_info->contrast);
	ADD_ATTR(PicParaInfoNode,Saturation,ppic_para_info->saturation);
	ADD_ATTR(PicParaInfoNode,Hue,ppic_para_info->hue);
	ADD_ATTR(PicParaInfoNode,Sharpness,ppic_para_info->sharpness);
	ADD_ATTR(PicParaInfoNode,NoiseReduction,ppic_para_info->noisereduction);

	tinyxml2::XMLPrinter streamer;
	ReqDoc.Accept(&streamer);
	string xmlstr(streamer.CStr());

	ExCommand Head;
	Head.nSrcType				= EX_SERVER_NMC;//ICMS_CLIENT_MONITOR;
	Head.nCmdType				= EX_NMC_SET_WINDOW_PICTURE_PARA_REQ;
	Head.nCmdSeq				= ++static_nCmdSeq;
	Head.nContentSize			= xmlstr.size()+1;

	ReqMsg.assign((char*)&Head, sizeof(Head));

	ReqMsg.append(xmlstr.c_str(), xmlstr.size());

	return Head.nCmdSeq;
}
int NmcMessage::ParserSetWindowPictureParaRspMsg(const string &RspMsg, std::string& strCache, int &WndType, int &WndId, int &nRetCode, std::string& strRemark)
{
	/*
	<?xml version="1.0" encoding="utf-8"?>
	<ExSetWindowPictureParaRsp Cache=????>
	<RetVal Code="0" Remark="success"/>
	<WindowInfo WndType="" WndId="" />
	</ExSetWindowPictureParaRsp>
	*/

	ExCommand *Head = (ExCommand *)RspMsg.data();
	assert(Head->nCmdType == EX_NMC_SET_WINDOW_PICTURE_PARA_RSP);

	tinyxml2::XMLDocument RspDoc;
	const char* data = RspMsg.data()+sizeof(ExCommand);
	if(RspDoc.Parse(data) != XML_NO_ERROR)
	{
		return -1;
	}
	tinyxml2::XMLElement *pRootElement = RspDoc.RootElement();
	if (NULL == pRootElement)
	{
		return -21;
	}
	strCache = SAFE_ITEM(pRootElement->Attribute("Cache"));
	tinyxml2::XMLElement* RetValNode = pRootElement->FirstChildElement("RetVal");
	if(RetValNode==NULL)
	{
		return -3; 
	}
	if(RetValNode->QueryIntAttribute("Code", &nRetCode)!=XML_NO_ERROR)
	{
		return -4;
	}
	strRemark = SAFE_ITEM(RetValNode->Attribute("Remark"));

	//////////////////

	tinyxml2::XMLElement* WindowInfoNode = pRootElement->FirstChildElement("WindowInfo");
	if(WindowInfoNode==NULL)
		return 0;


	if(WindowInfoNode->QueryIntAttribute("WndType", &WndType)!=XML_NO_ERROR)
	{
		return -4;
	}

	if(WindowInfoNode->QueryIntAttribute("WndId", &WndId)!=XML_NO_ERROR)
	{
		return -3;
	}
	
	return 0;
}

int NmcMessage::ParserMatrixConnStatusNotifyMsg(const string &RspMsg, std::string& strCache, int &MatrixId, int &ConnStatus)
{
	/*
	??????????????????EX_NMC_MATRIX_CONN_STATUS_NOTIFY
	<?xml version="1.0" encoding="utf-8"?> 
	<ExMatrixConnStatusNotify Cache="">
	<MatrixInfo MatrixId="" ConnStatus="" />  //0?????1????
	</ExMatrixConnStatusNotify>
	*/

	ExCommand *Head = (ExCommand *)RspMsg.data();
	assert(Head->nCmdType == EX_NMC_MATRIX_CONN_STATUS_NOTIFY);

	tinyxml2::XMLDocument RspDoc;
	const char* data = RspMsg.data()+sizeof(ExCommand);
	if(RspDoc.Parse(data) != XML_NO_ERROR)
	{
		return -1;
	}
	tinyxml2::XMLElement *pRootElement = RspDoc.RootElement();
	if (NULL == pRootElement)
	{
		return -21;
	}
	strCache = SAFE_ITEM(pRootElement->Attribute("Cache"));

	//////////////////

	tinyxml2::XMLElement* MatrixInfoNode = pRootElement->FirstChildElement("MatrixInfo");
	if(MatrixInfoNode->QueryIntAttribute("MatrixId", &MatrixId)!=XML_NO_ERROR)
	{
		return -4;
	}

	if(MatrixInfoNode->QueryIntAttribute("ConnStatus", &ConnStatus)!=XML_NO_ERROR)
	{
		return -5;
	}

	return 0;
}

int NmcMessage::BuildSetWindowOverlayInfoReqMsg(int OpType, int WndType, int WndId, int OverlayType, char* Cookie, string &ReqMsg)
{
/*
???????????????????EX_SET_WINDOW_OVERLAY_INFO_REQ
<?xml version="1.0" encoding="utf-8"?>
<ExSetWindowOverlayInfoReq Cache=????>
<WindowInfo OpType=???? WndType="" WndId=??1?? OverlayType=????/>
// OpType???????????????? 0-????? 1-???
//OverlayType	????????????0x0001-??????????0x0002-????????0x0004-?????? 0x0008-???????????? 0x0010-???????
</ExSetWindowOverlayInfoReq>
*/
	tinyxml2::XMLDocument ReqDoc;

	if(XML_NO_ERROR!=ReqDoc.Parse("<ExSetWindowOverlayInfoReq/>\n"))//
	{
		return -1;
	}

	tinyxml2::XMLElement *pRootElement = ReqDoc.RootElement();
	ADD_ATTR(pRootElement,Cache,Cookie);

	ADD_NODE(ReqDoc,WindowInfo,pRootElement);
	ADD_ATTR(WindowInfoNode,OpType,OpType);
	ADD_ATTR(WindowInfoNode,WndType,WndType);
	ADD_ATTR(WindowInfoNode,WndId,WndId);
	ADD_ATTR(WindowInfoNode,OverlayType,OverlayType);

	tinyxml2::XMLPrinter streamer;
	ReqDoc.Accept(&streamer);
	string xmlstr(streamer.CStr());

	ExCommand Head;
	Head.nSrcType				= EX_SERVER_NMC;//ICMS_CLIENT_MONITOR;
	Head.nCmdType				= EX_NMC_SET_WINDOW_OVERLAY_INFO_REQ;
	Head.nCmdSeq				= ++static_nCmdSeq;
	Head.nContentSize			= xmlstr.size()+1;

	ReqMsg.assign((char*)&Head, sizeof(Head));

	ReqMsg.append(xmlstr.c_str(), xmlstr.size());

	return Head.nCmdSeq;
}
int NmcMessage::ParserSetWindowOverlayInfoRspMsg(const string &RspMsg, std::string& strCache, int &WndType, int &WndId, int &nRetCode, std::string& strRemark)
{
/*
???????????????????????EX_SET_WINDOW_OVERLAY_INFO_RSP
<?xml version="1.0" encoding="utf-8"?>                       
<ExSetWindowOverlayInfoRsp Cache=????>
<RetVal Code="0" Remark="success"/>
<WindowInfo WndType="" WndId="" />
</ ExSetWindowOverlayInfoRsp>
*/
	ExCommand *Head = (ExCommand *)RspMsg.data();
	assert(Head->nCmdType == EX_NMC_SET_WINDOW_OVERLAY_INFO_RSP);

	tinyxml2::XMLDocument RspDoc;
	const char* data = RspMsg.data()+sizeof(ExCommand);
	if(RspDoc.Parse(data) != XML_NO_ERROR)
	{
		return -1;
	}
	tinyxml2::XMLElement *pRootElement = RspDoc.RootElement();
	if (NULL == pRootElement)
	{
		return -21;
	}
	strCache = SAFE_ITEM(pRootElement->Attribute("Cache"));
	tinyxml2::XMLElement* RetValNode = pRootElement->FirstChildElement("RetVal");
	if(RetValNode==NULL)
	{
		return -3; 
	}
	if(RetValNode->QueryIntAttribute("Code", &nRetCode)!=XML_NO_ERROR)
	{
		return -4;
	}
	strRemark = SAFE_ITEM(RetValNode->Attribute("Remark"));

	//////////////////

	tinyxml2::XMLElement* WindowInfoNode = pRootElement->FirstChildElement("WindowInfo");
	if(WindowInfoNode==NULL)
		return 0;

	if(WindowInfoNode->QueryIntAttribute("WndType", &WndType)!=XML_NO_ERROR)
	{
		return -4;
	}

	if(WindowInfoNode->QueryIntAttribute("WndId", &WndId)!=XML_NO_ERROR)
	{
		return -3;
	}

	return 0;
}

int NmcMessage::BuildFlipWindowReqMsg(int WndType, int WndId, char* Cookie, string &ReqMsg)
{
/*
???????????
?????????T?????EX_FLIP_WINDOW_REQ
<?xml version="1.0" encoding="utf-8"?>
<ExFlipWindowReq Cache=????>
<WindowInfo WndType=???? WndId=????/>
</ExFlipWindowReq>
*/
	tinyxml2::XMLDocument ReqDoc;

	if(XML_NO_ERROR!=ReqDoc.Parse("<ExFlipWindowReq/>\n"))//
	{
		return -1;
	}

	tinyxml2::XMLElement *pRootElement = ReqDoc.RootElement();
	ADD_ATTR(pRootElement,Cache,Cookie);

	ADD_NODE(ReqDoc,WindowInfo,pRootElement);
	ADD_ATTR(WindowInfoNode,WndType,WndType);
	ADD_ATTR(WindowInfoNode,WndId,WndId);

	tinyxml2::XMLPrinter streamer;
	ReqDoc.Accept(&streamer);
	string xmlstr(streamer.CStr());

	ExCommand Head;
	Head.nSrcType				= EX_SERVER_NMC;//ICMS_CLIENT_MONITOR;
	Head.nCmdType				= EX_NMC_FLIP_WINDOW_REQ;
	Head.nCmdSeq				= ++static_nCmdSeq;
	Head.nContentSize			= xmlstr.size()+1;

	ReqMsg.assign((char*)&Head, sizeof(Head));

	ReqMsg.append(xmlstr.c_str(), xmlstr.size());

	return Head.nCmdSeq;
}
int NmcMessage::ParserFlipWindowRspMsg(const string &RspMsg, std::string& strCache, int &WndType, int &WndId, int &nRetCode, std::string& strRemark)
{
/*
???????T?????????EX_FLIP_WINDOW_RSP
<?xml version="1.0" encoding="utf-8"?>
<ExFlipWindowRsp Cache=????>
<RetVal Code="0" Remark="success"/>
<WindowInfo WndType=???? WndId=????/>
</ExFlipWindowRsp>
*/
	ExCommand *Head = (ExCommand *)RspMsg.data();
	assert(Head->nCmdType == EX_NMC_FLIP_WINDOW_RSP);

	tinyxml2::XMLDocument RspDoc;
	const char* data = RspMsg.data()+sizeof(ExCommand);
	if(RspDoc.Parse(data) != XML_NO_ERROR)
	{
		return -1;
	}
	tinyxml2::XMLElement *pRootElement = RspDoc.RootElement();
	if (NULL == pRootElement)
	{
		return -21;
	}
	strCache = SAFE_ITEM(pRootElement->Attribute("Cache"));
	tinyxml2::XMLElement* RetValNode = pRootElement->FirstChildElement("RetVal");
	if(RetValNode==NULL)
	{
		return -3; 
	}
	if(RetValNode->QueryIntAttribute("Code", &nRetCode)!=XML_NO_ERROR)
	{
		return -4;
	}
	strRemark = SAFE_ITEM(RetValNode->Attribute("Remark"));

	//////////////////

	tinyxml2::XMLElement* WindowInfoNode = pRootElement->FirstChildElement("WindowInfo");
	if(WindowInfoNode==NULL)
		return 0;


	if(WindowInfoNode->QueryIntAttribute("WndType", &WndType)!=XML_NO_ERROR)
	{
		return -4;
	}

	if(WindowInfoNode->QueryIntAttribute("WndId", &WndId)!=XML_NO_ERROR)
	{
		return -3;
	}

	return 0;
}
//???????????????????
int NmcMessage::BuildFullscreenWindowReqMsg(int OpType, int WndType, int WndId, char* Cookie, string &ReqMsg)
{
/*
????????????????????????????
???????????????????EX_FULLSCREEN_WINDOW_REQ
<?xml version="1.0" encoding="utf-8"?> 
<ExFullscreenWindowReq Cache=????> 
<WindowInfo OpType="" WndType=???? WndId=????/>
//OpType??0???????????????1??????????
</ExFullscreenWindowReq>
*/
	tinyxml2::XMLDocument ReqDoc;

	if(XML_NO_ERROR!=ReqDoc.Parse("<ExFullscreenWindowReq/>\n"))//
	{
		return -1;
	}

	tinyxml2::XMLElement *pRootElement = ReqDoc.RootElement();
	ADD_ATTR(pRootElement,Cache,Cookie);

	ADD_NODE(ReqDoc,WindowInfo,pRootElement);
	ADD_ATTR(WindowInfoNode,OpType,OpType);
	ADD_ATTR(WindowInfoNode,WndType,WndType);
	ADD_ATTR(WindowInfoNode,WndId,WndId);

	tinyxml2::XMLPrinter streamer;
	ReqDoc.Accept(&streamer);
	string xmlstr(streamer.CStr());

	ExCommand Head;
	Head.nSrcType				= EX_SERVER_NMC;//ICMS_CLIENT_MONITOR;
	Head.nCmdType				= EX_NMC_FULLSCREEN_WINDOW_REQ;
	Head.nCmdSeq				= ++static_nCmdSeq;
	Head.nContentSize			= xmlstr.size()+1;

	ReqMsg.assign((char*)&Head, sizeof(Head));

	ReqMsg.append(xmlstr.c_str(), xmlstr.size());

	return Head.nCmdSeq;
}
int NmcMessage::ParserFullscreenWindowRspMsg(const string &RspMsg, std::string& strCache, int &WndType, int &WndId, int &nRetCode, std::string& strRemark)
{
/*
???????????????????????EX_FULLSCREEN_WINDOW_RSP
<?xml version="1.0" encoding="utf-8"?>
<ExFullscreenWindowRsp Cache=????>
<RetVal Code="0" Remark="success"/>
<WindowInfo WndType=???? WndId=????/>
</ExFullscreenWindowRsp>	
*/
	ExCommand *Head = (ExCommand *)RspMsg.data();
	assert(Head->nCmdType == EX_NMC_FULLSCREEN_WINDOW_RSP);

	tinyxml2::XMLDocument RspDoc;
	const char* data = RspMsg.data()+sizeof(ExCommand);
	if(RspDoc.Parse(data) != XML_NO_ERROR)
	{
		return -1;
	}
	tinyxml2::XMLElement *pRootElement = RspDoc.RootElement();
	if (NULL == pRootElement)
	{
		return -21;
	}
	strCache = SAFE_ITEM(pRootElement->Attribute("Cache"));
	tinyxml2::XMLElement* RetValNode = pRootElement->FirstChildElement("RetVal");
	if(RetValNode==NULL)
	{
		return -3; 
	}
	if(RetValNode->QueryIntAttribute("Code", &nRetCode)!=XML_NO_ERROR)
	{
		return -4;
	}
	strRemark = SAFE_ITEM(RetValNode->Attribute("Remark"));

	//////////////////

	tinyxml2::XMLElement* WindowInfoNode = pRootElement->FirstChildElement("WindowInfo");
	if(WindowInfoNode==NULL)
		return 0;


	if(WindowInfoNode->QueryIntAttribute("WndType", &WndType)!=XML_NO_ERROR)
	{
		return -4;
	}

	if(WindowInfoNode->QueryIntAttribute("WndId", &WndId)!=XML_NO_ERROR)
	{
		return -3;
	}

	return 0;
}
int NmcMessage::BuildShowScreenNumReqMsg(int MatrixId, int Type, char* Cookie, string &ReqMsg)
{
/*
<?xml version="1.0" encoding="utf-8"?>
<ExShowScrNumReq Cache="">
<MatrixId>1</ MatrixId>
<Type>0</Type> //0 ??????????? 1 ????????? 
</ExShowScrNumReq >
*/
	tinyxml2::XMLDocument ReqDoc;

	if(XML_NO_ERROR!=ReqDoc.Parse("<ExShowScrNumReq/>\n"))//
	{
		return -1;
	}

	tinyxml2::XMLElement *pRootElement = ReqDoc.RootElement();
	ADD_ATTR(pRootElement,Cache,Cookie);

	char chTmp[32] = {0};
	sprintf(chTmp, "%d", MatrixId);
	ADD_NODE_WITH_TEXT(ReqDoc,MatrixId,pRootElement,chTmp);

	sprintf(chTmp, "%d", Type);
	ADD_NODE_WITH_TEXT(ReqDoc,Type,pRootElement,chTmp);

	tinyxml2::XMLPrinter streamer;
	ReqDoc.Accept(&streamer);
	string xmlstr(streamer.CStr());

	ExCommand Head;
	Head.nSrcType				= EX_SERVER_NMC;//ICMS_CLIENT_MONITOR;
	Head.nCmdType				= EX_NMC_SHOW_SCRNUM_PIC_REQ;
	Head.nCmdSeq				= ++static_nCmdSeq;
	Head.nContentSize			= xmlstr.size()+1;

	ReqMsg.assign((char*)&Head, sizeof(Head));

	ReqMsg.append(xmlstr.c_str(), xmlstr.size());

	return Head.nCmdSeq;
}
int NmcMessage::ParserShowScreenNumRspMsg(const string &RspMsg, std::string& strCache, int &nRetCode, std::string& strRemark)
{
/*
<?xml version="1.0" encoding="utf-8"?> 
<ExShowScrNumRsp Cache="">
<RetVal Code="0" Remark="success"/>
</ExShowScrNumRsp>
*/
	ExCommand *Head = (ExCommand *)RspMsg.data();
	assert(Head->nCmdType == EX_NMC_SHOW_SCRNUM_PIC_RSP);

	tinyxml2::XMLDocument RspDoc;
	const char* data = RspMsg.data()+sizeof(ExCommand);
	if(RspDoc.Parse(data) != XML_NO_ERROR)
	{
		return -1;
	}
	tinyxml2::XMLElement *pRootElement = RspDoc.RootElement();
	if (NULL == pRootElement)
	{
		return -21;
	}
	strCache = SAFE_ITEM(pRootElement->Attribute("Cache"));
	tinyxml2::XMLElement* RetValNode = pRootElement->FirstChildElement("RetVal");
	if(RetValNode==NULL)
	{
		return -3; 
	}
	if(RetValNode->QueryIntAttribute("Code", &nRetCode)!=XML_NO_ERROR)
	{
		return -4;
	}
	strRemark = SAFE_ITEM(RetValNode->Attribute("Remark"));

	return 0;
}
int NmcMessage::BuildCreateSWWindow(int width, int height, struct st_create_sw_window_info *pinfo, int info_cnt, char* Cookie, string &ReqMsg)
{
/*
<?xml version="1.0" encoding="utf-8"?>                       
<ExCreateSlidingWindowReq Cache=????>
<SlideWindowWidth>400<SlideWindowWidth/>
<SlideWindowHeight>300<SlideWindowHeight/>
	<SlidingWindowInfoList>
		<WindowInfo OutputID=??1?? OutputStartX="100" OutputStartY="100"  DisPlayX=??0?? DisPlayY=??0?? Width="400" Height="300" CSubWinWidth =??100?? CSubWinHeight =??200??/>
	// OutputStartX??????????????????????DisPlayX????????????????????????
// Width Height ?????????????????????CSubWinWidth ???????????????
??
	</SlidingWindowInfoList>
</ExCreateSlidingWindowReq>
*/

	tinyxml2::XMLDocument ReqDoc;

	if(XML_NO_ERROR!=ReqDoc.Parse("<ExCreateSlidingWindowReq/>\n"))//
	{
		return -1;
	}

	tinyxml2::XMLElement *pRootElement = ReqDoc.RootElement();
	ADD_ATTR(pRootElement,Cache,Cookie);

	char chTmp[32] = {0};
	sprintf(chTmp, "%d", width);
	ADD_NODE_WITH_TEXT(ReqDoc,SlideWindowWidth,pRootElement,chTmp);

	sprintf(chTmp, "%d", height);
	ADD_NODE_WITH_TEXT(ReqDoc,SlideWindowHeight,pRootElement,chTmp);

	ADD_NODE(ReqDoc,SlidingWindowInfoList,pRootElement);
	for(int i=0; i<info_cnt; ++i)
	{
		ADD_NODE(ReqDoc,WindowInfo,SlidingWindowInfoListNode);
		ADD_ATTR(WindowInfoNode,OutputID,pinfo[i].outputid);
		ADD_ATTR(WindowInfoNode,OutputStartX,pinfo[i].output_start_x);
		ADD_ATTR(WindowInfoNode,OutputStartY,pinfo[i].output_start_y);
		ADD_ATTR(WindowInfoNode,DisPlayX,pinfo[i].display_x);
		ADD_ATTR(WindowInfoNode,DisPlayY,pinfo[i].display_y);
		ADD_ATTR(WindowInfoNode,Width,pinfo[i].width);
		ADD_ATTR(WindowInfoNode,Height,pinfo[i].heigh);
		ADD_ATTR(WindowInfoNode,CSubWinWidth,pinfo[i].sub_win_width);
		ADD_ATTR(WindowInfoNode,CSubWinHeight,pinfo[i].sub_win_height);
	}
	
	tinyxml2::XMLPrinter streamer;
	ReqDoc.Accept(&streamer);
	string xmlstr(streamer.CStr());

	ExCommand Head;
	Head.nSrcType				= EX_SERVER_NMC;//ICMS_CLIENT_MONITOR;
	Head.nCmdType				= EX_NMC_SW_CREATE_WINDOW_REQ;
	Head.nCmdSeq				= ++static_nCmdSeq;
	Head.nContentSize			= xmlstr.size()+1;

	ReqMsg.assign((char*)&Head, sizeof(Head));

	ReqMsg.append(xmlstr.c_str(), xmlstr.size());

	return Head.nCmdSeq;
}
int NmcMessage::ParserCreateSWWindow(const string &RspMsg, std::string& strCache, int *SlidingWindowID, int *SWSequence, int &nRetCode, std::string& strRemark)
{
/*
<?xml version="1.0" encoding="utf-8"?>                       
<ExCreateSlidingWindowRsp Cache=????>
<SlidingWindowID>1<SlidingWindowID/>
<SWSequence>1<SWSequence/>
<RetVal Code="0" Remark="success"/>
</ExCreateSlidingWindowRsp>
*/

	ExCommand *Head = (ExCommand *)RspMsg.data();
	assert(Head->nCmdType == EX_NMC_SW_CREATE_WINDOW_RSP);

	tinyxml2::XMLDocument RspDoc;
	const char* data = RspMsg.data()+sizeof(ExCommand);
	if(RspDoc.Parse(data) != XML_NO_ERROR)
	{
		return -157419;
	}

	tinyxml2::XMLElement *pRootElement = RspDoc.RootElement();
	if (NULL == pRootElement)
	{
		return -24861;
	}
	strCache = SAFE_ITEM(pRootElement->Attribute("Cache"));
	tinyxml2::XMLElement* RetValNode = pRootElement->FirstChildElement("RetVal");
	if(RetValNode==NULL)
	{
		return -147953; 
	}

	if(RetValNode->QueryIntAttribute("Code", &nRetCode)!=XML_NO_ERROR)
	{
		return -99954;
	}
	strRemark = SAFE_ITEM(RetValNode->Attribute("Remark"));

	tinyxml2::XMLElement* SlidingWindowIDNode = pRootElement->FirstChildElement("SlidingWindowID");
	if(SlidingWindowIDNode==NULL)
	{
		return -1485265; 
	}
	GET_TEXT(SlidingWindowID);
	*SlidingWindowID = atoi(chSlidingWindowID);

	tinyxml2::XMLElement* SWSequenceNode = pRootElement->FirstChildElement("SWSequence");
	if(SWSequenceNode==NULL)
	{
		return -614865; 
	}

	GET_TEXT(SWSequence);
	*SWSequence = atoi(chSWSequence);

	return 0;
}
int NmcMessage::BuildDeleteSWWindow(int *psw_window_id_list, int sw_window_id_list_len, char* Cookie, string &ReqMsg)
{
/*
<?xml version="1.0" encoding="utf-8"?>                       
<ExDeleteSlidingWindowReq Cache=????>\
	<DelSWList>
		<SlidingWindowID>1<SlidingWindowID/>
	????
	<DelSWList>
</ExDeleteSlidingWindowReq>
*/
	tinyxml2::XMLDocument ReqDoc;

	if(XML_NO_ERROR!=ReqDoc.Parse("<ExDeleteSlidingWindowReq/>\n"))//
	{
		return -1;
	}

	tinyxml2::XMLElement *pRootElement = ReqDoc.RootElement();
	ADD_ATTR(pRootElement,Cache,Cookie);

	ADD_NODE(ReqDoc,DelSWList,pRootElement);
	char chTmp[32] = {0};
	for(int i=0; i<sw_window_id_list_len; ++i)
	{
		sprintf(chTmp, "%d", psw_window_id_list[i]);
		ADD_NODE_WITH_TEXT(ReqDoc,SlidingWindowID,DelSWListNode,chTmp);
	}
	
	tinyxml2::XMLPrinter streamer;
	ReqDoc.Accept(&streamer);
	string xmlstr(streamer.CStr());

	ExCommand Head;
	Head.nSrcType				= EX_SERVER_NMC;//ICMS_CLIENT_MONITOR;
	Head.nCmdType				= EX_NMC_SW_DEL_WINDOW_REQ;
	Head.nCmdSeq				= ++static_nCmdSeq;
	Head.nContentSize			= xmlstr.size()+1;

	ReqMsg.assign((char*)&Head, sizeof(Head));

	ReqMsg.append(xmlstr.c_str(), xmlstr.size());

	return Head.nCmdSeq;
}
int NmcMessage::ParserDeleteSWWindow(const string &RspMsg, std::string& strCache, int &nRetCode, std::string& strRemark)
{
/*
<?xml version="1.0" encoding="utf-8"?>                       
<ExDeleteSlidingWindowRsp Cache=????>
	<RetVal Code="0" Remark="success"/>
</ExDeleteSlidingWindowRsp>
*/
	ExCommand *Head = (ExCommand *)RspMsg.data();
	assert(Head->nCmdType == EX_NMC_SW_DEL_WINDOW_RSP);

	tinyxml2::XMLDocument RspDoc;
	const char* data = RspMsg.data()+sizeof(ExCommand);
	if(RspDoc.Parse(data) != XML_NO_ERROR)
	{
		return -1;
	}
	tinyxml2::XMLElement *pRootElement = RspDoc.RootElement();
	if (NULL == pRootElement)
	{
		return -21;
	}
	strCache = SAFE_ITEM(pRootElement->Attribute("Cache"));
	tinyxml2::XMLElement* RetValNode = pRootElement->FirstChildElement("RetVal");
	if(RetValNode==NULL)
	{
		return -3; 
	}

	if(RetValNode->QueryIntAttribute("Code", &nRetCode)!=XML_NO_ERROR)
	{
		return -4;
	}

	strRemark = SAFE_ITEM(RetValNode->Attribute("Remark"));

	return 0;
}
int NmcMessage::BuildStickSWWindow(int sw_window_id, char* Cookie, string &ReqMsg)
{
/*
<?xml version="1.0" encoding="utf-8"?>                       
<ExStickSlidingWindowReq Cache=????>
<SlidingWindowID>1<SlidingWindowID/>
</ExStickSlidingWindowReq>
*/
	tinyxml2::XMLDocument ReqDoc;

	if(XML_NO_ERROR!=ReqDoc.Parse("<ExStickSlidingWindowReq/>\n"))//
	{
		return -1;
	}

	tinyxml2::XMLElement *pRootElement = ReqDoc.RootElement();
	ADD_ATTR(pRootElement,Cache,Cookie);

	char chTmp[32] = {0};
	sprintf(chTmp, "%d", sw_window_id);
	ADD_NODE_WITH_TEXT(ReqDoc,SlidingWindowID,pRootElement,chTmp);
	
	tinyxml2::XMLPrinter streamer;
	ReqDoc.Accept(&streamer);
	string xmlstr(streamer.CStr());

	ExCommand Head;
	Head.nSrcType				= EX_SERVER_NMC;//ICMS_CLIENT_MONITOR;
	Head.nCmdType				= EX_NMC_SW_STICK_WINDOW_REQ;
	Head.nCmdSeq				= ++static_nCmdSeq;
	Head.nContentSize			= xmlstr.size()+1;

	ReqMsg.assign((char*)&Head, sizeof(Head));

	ReqMsg.append(xmlstr.c_str(), xmlstr.size());

	return Head.nCmdSeq;
}
int NmcMessage::ParserStickSWWindow(const string &RspMsg, std::string& strCache, int &nRetCode, std::string& strRemark)
{
/*
<?xml version="1.0" encoding="utf-8"?>                       
<ExStickSlidingWindowRsp Cache=????>
<RetVal Code="0" Remark="success"/>
</ExStickSlidingWindowRsp>
*/
	ExCommand *Head = (ExCommand *)RspMsg.data();
	assert(Head->nCmdType == EX_NMC_SW_STICK_WINDOW_RSP);

	tinyxml2::XMLDocument RspDoc;
	const char* data = RspMsg.data()+sizeof(ExCommand);
	if(RspDoc.Parse(data) != XML_NO_ERROR)
	{
		return -1;
	}
	tinyxml2::XMLElement *pRootElement = RspDoc.RootElement();
	if (NULL == pRootElement)
	{
		return -21;
	}
	strCache = SAFE_ITEM(pRootElement->Attribute("Cache"));
	tinyxml2::XMLElement* RetValNode = pRootElement->FirstChildElement("RetVal");
	if(RetValNode==NULL)
	{
		return -3; 
	}

	if(RetValNode->QueryIntAttribute("Code", &nRetCode)!=XML_NO_ERROR)
	{
		return -4;
	}

	strRemark = SAFE_ITEM(RetValNode->Attribute("Remark"));

	return 0;
}

int NmcMessage::BuildGetSWWindowInfo(int sw_window_id, char* cCookie, string &ReqMsg)
{
/*
<?xml version="1.0" encoding="utf-8"?>                       
<ExGetSlidingWinInfoReq Cache=????>
<SlidingWindowID>1<SlidingWindowID/>
</ExGetSlidingWinInfoReq>
*/
	tinyxml2::XMLDocument ReqDoc;

	if(XML_NO_ERROR!=ReqDoc.Parse("<ExGetSlidingWinInfoReq/>\n"))//
	{
		return -1;
	}

	tinyxml2::XMLElement *pRootElement = ReqDoc.RootElement();
	ADD_ATTR(pRootElement,Cache,cCookie);

	char chTmp[32] = {0};
	sprintf(chTmp, "%d", sw_window_id);
	ADD_NODE_WITH_TEXT(ReqDoc,SlidingWindowID,pRootElement,chTmp);
	
	tinyxml2::XMLPrinter streamer;
	ReqDoc.Accept(&streamer);
	string xmlstr(streamer.CStr());

	ExCommand Head;
	Head.nSrcType				= EX_SERVER_NMC;//ICMS_CLIENT_MONITOR;
	Head.nCmdType				= EX_NMC_SW_GET_WIN_INFO_REQ;
	Head.nCmdSeq				= ++static_nCmdSeq;
	Head.nContentSize			= xmlstr.size()+1;

	ReqMsg.assign((char*)&Head, sizeof(Head));

	ReqMsg.append(xmlstr.c_str(), xmlstr.size());

	return Head.nCmdSeq;
}
int NmcMessage::ParserGetSWWindowInfo(const string &RspMsg, std::string& strCache, struct st_sw_window_info** info, int *info_cnt, int &nRetCode, std::string& strRemark)
{
	/*
<?xml version="1.0" encoding="utf-8"?>                       
<ExGetSlidingWinInfoRsp Cache=????>
<RetVal Code="0" Remark="success"/>
<SWInfoList>
	<SWInfo SWID=??1?? SWSatus=??1?? SourceId=???? SourceSubId=????/>
	????
</SWInfoList>
</ExGetSlidingWinInfoRsp>
	*/

	ExCommand *Head = (ExCommand *)RspMsg.data();
	assert(Head->nCmdType == EX_NMC_SW_GET_WIN_INFO_RSP);

	tinyxml2::XMLDocument RspDoc;
	const char* data = RspMsg.data()+sizeof(ExCommand);
	if(RspDoc.Parse(data) != XML_NO_ERROR)
	{
		return -1;
	}
	tinyxml2::XMLElement *pRootElement = RspDoc.RootElement();
	if (NULL == pRootElement)
	{
		return -21;
	}
	strCache = SAFE_ITEM(pRootElement->Attribute("Cache"));

	tinyxml2::XMLElement* RetValNode = pRootElement->FirstChildElement("RetVal");
	if(RetValNode==NULL)
	{
		return -3; 
	}

	if(RetValNode->QueryIntAttribute("Code", &nRetCode)!=XML_NO_ERROR)
	{
		return -4;
	}

	strRemark = SAFE_ITEM(RetValNode->Attribute("Remark"));

	tinyxml2::XMLElement* SWInfoListNode = pRootElement->FirstChildElement("SWInfoList");
	if(SWInfoListNode==NULL)
	{
		return -3; 
	}

	int max_info_cnt = 128;
	tinyxml2::XMLElement* SWInfoNode = SWInfoListNode->FirstChildElement("SWInfo");
	if(SWInfoNode)
		*info = (struct st_sw_window_info *)calloc(max_info_cnt, sizeof(struct st_sw_window_info));

	int i=0;
	while(SWInfoNode)
	{
		if(SWInfoNode->QueryIntAttribute("SWID",  &(*info)[i].sw_window_id)!=XML_NO_ERROR)
		{
			assert(false);
		}

		if(SWInfoNode->QueryIntAttribute("SWSatus",  &(*info)[i].sw_window_status)!=XML_NO_ERROR)
		{
			assert(false);
		}

		if(SWInfoNode->QueryIntAttribute("SourceId",  &(*info)[i].source_id)!=XML_NO_ERROR)
		{
			assert(false);
		}

		if(SWInfoNode->QueryIntAttribute("SourceSubId",  &(*info)[i].source_sub_id)!=XML_NO_ERROR)
		{
			assert(false);
		}
		
		i++;
		SWInfoNode = SWInfoNode->NextSiblingElement("SWInfo");
	}

	*info_cnt = i;

	return 0;
}
int NmcMessage::BuildGetSurveyPlanReq(int survey_id, char* cCookie, string &ReqMsg)
{
/*
<?xml version="1.0" encoding="utf-8"?>
<ExGetSurveyPlanReq Cache="">
	<SurveyInfo SurveyId="-1"/> //-1?????????????????
</ExGetSurveyPlanReq>
*/
	tinyxml2::XMLDocument ReqDoc;

	if(XML_NO_ERROR!=ReqDoc.Parse("<ExGetSurveyPlanReq/>\n"))//
	{
		return -1;
	}

	tinyxml2::XMLElement *pRootElement = ReqDoc.RootElement();
	ADD_ATTR(pRootElement,Cache,cCookie);

	char chTmp[32] = {0};
	sprintf(chTmp, "%d", survey_id);
	ADD_NODE(ReqDoc,SurveyInfo,pRootElement);
	ADD_ATTR(SurveyInfoNode,SurveyId,survey_id);

	tinyxml2::XMLPrinter streamer;
	ReqDoc.Accept(&streamer);
	string xmlstr(streamer.CStr());

	ExCommand Head;
	Head.nSrcType				= EX_SERVER_NMC;
	Head.nCmdType				= EX_NMC_GET_SURVEY_PLAN_REQ;
	Head.nCmdSeq				= ++static_nCmdSeq;
	Head.nContentSize			= xmlstr.size()+1;

	ReqMsg.assign((char*)&Head, sizeof(Head));

	ReqMsg.append(xmlstr.c_str(), xmlstr.size());

	return Head.nCmdSeq;
}
int NmcMessage::ParserGetSurveyPlanRsp(const string &RspMsg, string &strCache, struct st_xy_survey_info** ppsurvey_info, int *survey_info_cnt, int &nRetCode, std::string& strRemark)
{
/*
<?xml version="1.0" encoding="utf-8"?>
<ExGetSurveyPlanRsp Cache="">
	<RetVal Code="0" Remark="success"/>
	<SurveyInfoList>
		<SurveyInfo SurveyId="" SurveyName="" SurveyInterval="">
			<SignalSourceInfoList>
		         <SourceInfo SourceId="" SubSourceId=""/>
		??...
			</SignalSourceInfoList>
		</SurveyInfo>
		...
	</SurveyInfoList>
</ExGetSurveyPlanRsp>
*/
	ExCommand *Head = (ExCommand *)RspMsg.data();
	assert(Head->nCmdType == EX_NMC_GET_SURVEY_PLAN_RSP);

	tinyxml2::XMLDocument RspDoc;
	const char* data = RspMsg.data()+sizeof(ExCommand);
	if(RspDoc.Parse(data) != XML_NO_ERROR)
	{
		return -1;
	}
	tinyxml2::XMLElement *pRootElement = RspDoc.RootElement();
	if (NULL == pRootElement)
	{
		return -21;
	}
	strCache = SAFE_ITEM(pRootElement->Attribute("Cache"));

	tinyxml2::XMLElement* RetValNode = pRootElement->FirstChildElement("RetVal");
	if(RetValNode==NULL)
	{
		return -3; 
	}

	if(RetValNode->QueryIntAttribute("Code", &nRetCode)!=XML_NO_ERROR)
	{
		return -4;
	}

	strRemark = SAFE_ITEM(RetValNode->Attribute("Remark"));

	tinyxml2::XMLElement* SurveyInfoListNode = pRootElement->FirstChildElement("SurveyInfoList");
	if(SurveyInfoListNode==NULL)
	{
		return -3; 
	}

	vector<struct st_xy_survey_info> TmpSurveyInfoS;
	int max_info_cnt = 128;
	tinyxml2::XMLElement* SurveyInfoNode = SurveyInfoListNode->FirstChildElement("SurveyInfo");	
	while(SurveyInfoNode)
	{
		struct st_xy_survey_info survey_info;
		if(SurveyInfoNode->QueryIntAttribute("SurveyId",  &survey_info.survey_id)!=XML_NO_ERROR)
		{
			assert(false);
		}
		
		const char *chSurveyName = SAFE_ITEM(SurveyInfoNode->Attribute("SurveyName"));
		if(chSurveyName)
			strncpy(survey_info.surveyname, chSurveyName, 63);
		else
		{
			assert(false);
		}

		if(SurveyInfoNode->QueryIntAttribute("SurveyInterval",  &survey_info.surveyInterval)!=XML_NO_ERROR)
		{
			assert(false);
		}

		tinyxml2::XMLElement* SignalSourceInfoListNode = SurveyInfoNode->FirstChildElement("SignalSourceInfoList");
		if(SignalSourceInfoListNode==NULL)
		{
			return -56; 
		}
		
		tinyxml2::XMLElement* SourceInfoNode = SignalSourceInfoListNode->FirstChildElement("SourceInfo");
		if(SourceInfoNode==NULL)
		{
			return -56; 
		}

		vector<struct st_xy_sourceinfo> TmpSourceInfoS;
		while(SourceInfoNode)
		{
			struct st_xy_sourceinfo sourceinfo;
			sourceinfo.sourceid = 0;
			sourceinfo.subsourceid = 0;
			if(SourceInfoNode->QueryIntAttribute("SourceId", &(sourceinfo.sourceid))!=XML_NO_ERROR)
			{
				assert(false);
			}

			if(SourceInfoNode->QueryIntAttribute("SubSourceId", &(sourceinfo.subsourceid))!=XML_NO_ERROR)
			{
				assert(false);
			}

			TmpSourceInfoS.push_back(sourceinfo);

			SourceInfoNode = SourceInfoNode->NextSiblingElement("SourceInfo");
		}
		
		if(TmpSourceInfoS.size())
		{
			survey_info.pst_xy_sourceinfo = (struct st_xy_sourceinfo *)calloc(TmpSourceInfoS.size(), sizeof(struct st_xy_sourceinfo));
			survey_info.sourceinfo_cnt = TmpSourceInfoS.size();
			for(int i=0; i<survey_info.sourceinfo_cnt; ++i)
			{
				memcpy(survey_info.pst_xy_sourceinfo+i, &(TmpSourceInfoS[i]), sizeof(struct st_xy_sourceinfo));
			}
		}
		else
		{
			survey_info.pst_xy_sourceinfo = NULL;
			survey_info.sourceinfo_cnt = 0;
		}

		TmpSurveyInfoS.push_back(survey_info);

		SurveyInfoNode = SurveyInfoNode->NextSiblingElement("SurveyInfo");
	}

	if(TmpSurveyInfoS.size())
	{
		*ppsurvey_info = (struct st_xy_survey_info *)calloc(TmpSurveyInfoS.size(), sizeof(struct st_xy_survey_info));
		*survey_info_cnt = TmpSurveyInfoS.size();
		for(int i=0; i<*survey_info_cnt; ++i)
		{
			memcpy(*ppsurvey_info+i, &(TmpSurveyInfoS[i]), sizeof(struct st_xy_survey_info));
		}
	}
	else
	{
		*ppsurvey_info=NULL;
		*survey_info_cnt=0;
	}

	return 0;
}

int NmcMessage::BuildSetSurveyPlanReq(enum en_nmc_operator_type operator_type, struct st_xy_survey_info *psurvey_info, char* chCookie, string &ReqMsg)
{
/*
<?xml version="1.0" encoding="utf-8"?>
<ExSetSurveyPlanReq Cache="">
	<SurveyInfo OpType="" SurveyId="" SurveyName="" SurveyInterval=""/>
	// OpType??0????1????2????????????SurveyId?-1
	<SignalSourceInfoList>
		<SourceInfo SourceId="" SubSourceId=""/>
??...
	</SignalSourceInfoList>
</ExSetSurveyPlanReq>
*/
	tinyxml2::XMLDocument ReqDoc;

	if(XML_NO_ERROR!=ReqDoc.Parse("<ExSetSurveyPlanReq/>\n"))//
	{
		return -1;
	}

	tinyxml2::XMLElement *pRootElement = ReqDoc.RootElement();
	ADD_ATTR(pRootElement,Cache,chCookie);

	ADD_NODE(ReqDoc,SurveyInfo,pRootElement);
	ADD_ATTR(SurveyInfoNode,OpType,operator_type);
	ADD_ATTR(SurveyInfoNode,SurveyId,psurvey_info->survey_id);
	ADD_ATTR(SurveyInfoNode,SurveyName,psurvey_info->surveyname);
	ADD_ATTR(SurveyInfoNode,SurveyInterval,psurvey_info->surveyInterval);

	ADD_NODE(ReqDoc,SignalSourceInfoList,pRootElement);
	for(int i=0; i<psurvey_info->sourceinfo_cnt; i++)
	{
		ADD_NODE(ReqDoc,SourceInfo,SignalSourceInfoListNode);
		ADD_ATTR(SourceInfoNode,SourceId,psurvey_info->pst_xy_sourceinfo[i].sourceid);
		ADD_ATTR(SourceInfoNode,SubSourceId,psurvey_info->pst_xy_sourceinfo[i].subsourceid);	
	}

	tinyxml2::XMLPrinter streamer;
	ReqDoc.Accept(&streamer);
	string xmlstr(streamer.CStr());

	ExCommand Head;
	Head.nSrcType				= EX_SERVER_NMC;
	Head.nCmdType				= EX_NMC_SET_SURVEY_PLAN_REQ;
	Head.nCmdSeq				= ++static_nCmdSeq;
	Head.nContentSize			= xmlstr.size()+1;

	ReqMsg.assign((char*)&Head, sizeof(Head));

	ReqMsg.append(xmlstr.c_str(), xmlstr.size());

	return Head.nCmdSeq;
}

int NmcMessage::ParserSetSurveyPlanRsp(const string &RspMsg, std::string& strCache, enum en_nmc_operator_type &operator_type, struct st_xy_survey_info *psurvey_info, int &nRetCode, std::string& strRemark)
{
/*
<?xml version="1.0" encoding="utf-8"?>
<ExSetSurveyPlanRsp Cache="">
	<RetVal Code="0" Remark="success"/>
	<SurveyInfo OpType="" SurveyId=""/>
</ExSetSurveyPlanRsp>
*/
	ExCommand *Head = (ExCommand *)RspMsg.data();
	assert(Head->nCmdType == EX_NMC_SET_SURVEY_PLAN_RSP);

	tinyxml2::XMLDocument RspDoc;
	const char* data = RspMsg.data()+sizeof(ExCommand);
	if(RspDoc.Parse(data) != XML_NO_ERROR)
	{
		return -1;
	}
	tinyxml2::XMLElement *pRootElement = RspDoc.RootElement();
	if (NULL == pRootElement)
	{
		return -21;
	}
	strCache = SAFE_ITEM(pRootElement->Attribute("Cache"));
	tinyxml2::XMLElement* RetValNode = pRootElement->FirstChildElement("RetVal");
	if(RetValNode==NULL)
	{
		return -3;
	}

	if(RetValNode->QueryIntAttribute("Code", &nRetCode)!=XML_NO_ERROR)
	{
		return -471;
	}

	strRemark = SAFE_ITEM(RetValNode->Attribute("Remark"));

	tinyxml2::XMLElement* SurveyInfoNode = pRootElement->FirstChildElement("SurveyInfo");
	if(SurveyInfoNode==NULL)
	{
		return -3; 
	}

	if(SurveyInfoNode->QueryIntAttribute("OpType", (int*)&operator_type)!=XML_NO_ERROR)
	{
		return -774;
	}

	if(SurveyInfoNode->QueryIntAttribute("SurveyId", &psurvey_info->survey_id)!=XML_NO_ERROR)
	{
		return -49;
	}

	return 0;
}
int NmcMessage::BuildCtrlWindowSurveyReq(int ctrl_type, int survey_id, int wndtype, int wndid, char* chCookie, string &ReqMsg)
{
/*
<?xml version="1.0" encoding="utf-8"?> 
<ExCtrlWindowSurveyReq Cache=????> 
	<SurveyInfo OpType="" SurveyId=""/>
	//0?????1???????????
	<WindowInfo WndType="" WndId=""/>
</ExCtrlWindowSurveyReq>
*/
	tinyxml2::XMLDocument ReqDoc;

	if(XML_NO_ERROR!=ReqDoc.Parse("<ExCtrlWindowSurveyReq/>\n"))//
	{
		return -1;
	}

	tinyxml2::XMLElement *pRootElement = ReqDoc.RootElement();
	ADD_ATTR(pRootElement,Cache,chCookie);

	ADD_NODE(ReqDoc,SurveyInfo,pRootElement);
	ADD_ATTR(SurveyInfoNode,OpType,ctrl_type);
	ADD_ATTR(SurveyInfoNode,SurveyId,survey_id);

	ADD_NODE(ReqDoc,WindowInfo,pRootElement);
	ADD_ATTR(WindowInfoNode,WndType,wndtype);
	ADD_ATTR(WindowInfoNode,WndId,wndid);
	
	tinyxml2::XMLPrinter streamer;
	ReqDoc.Accept(&streamer);
	string xmlstr(streamer.CStr());

	ExCommand Head;
	Head.nSrcType				= EX_SERVER_NMC;
	Head.nCmdType				= EX_NMC_CTRL_WINDOW_SURVEY_REQ;
	Head.nCmdSeq				= ++static_nCmdSeq;
	Head.nContentSize			= xmlstr.size()+1;

	ReqMsg.assign((char*)&Head, sizeof(Head));

	ReqMsg.append(xmlstr.c_str(), xmlstr.size());

	return Head.nCmdSeq;
}
int NmcMessage::ParserCtrlWindowSurveyRsp(const string &RspMsg, std::string& strCache, int &ctrl_type, int &survey_id, int &wndtype, int &wndid, int &nRetCode, std::string& strRemark)
{
/*
<?xml version="1.0" encoding="utf-8"?> 
<ExCtrlWindowSurveyRsp Cache=????>
	<RetVal Code="0" Remark="success"/>
	<SurveyInfo OpType="" SurveyId=""/>
	<WindowInfo WndType="" WndId=""/>
</ExCtrlWindowSurveyRsp>
*/
	ExCommand *Head = (ExCommand *)RspMsg.data();
	assert(Head->nCmdType == EX_NMC_CTRL_WINDOW_SURVEY_RSP);

	tinyxml2::XMLDocument RspDoc;
	const char* data = RspMsg.data()+sizeof(ExCommand);
	if(RspDoc.Parse(data) != XML_NO_ERROR)
	{
		return -1;
	}
	tinyxml2::XMLElement *pRootElement = RspDoc.RootElement();
	if (NULL == pRootElement)
	{
		return -21;
	}
	strCache = SAFE_ITEM(pRootElement->Attribute("Cache"));
	tinyxml2::XMLElement* RetValNode = pRootElement->FirstChildElement("RetVal");
	if(RetValNode==NULL)
	{
		return -3;
	}

	if(RetValNode->QueryIntAttribute("Code", &nRetCode)!=XML_NO_ERROR)
	{
		return -471;
	}

	strRemark = SAFE_ITEM(RetValNode->Attribute("Remark"));

	tinyxml2::XMLElement* SurveyInfoNode = pRootElement->FirstChildElement("SurveyInfo");
	if(SurveyInfoNode==NULL)
	{
		return -3; 
	}

	if(SurveyInfoNode->QueryIntAttribute("OpType", (int*)&ctrl_type)!=XML_NO_ERROR)
	{
		return -774;
	}

	if(SurveyInfoNode->QueryIntAttribute("SurveyId", &survey_id)!=XML_NO_ERROR)
	{
		return -49;
	}

	tinyxml2::XMLElement* WindowInfoNode = pRootElement->FirstChildElement("WindowInfo");
	if(WindowInfoNode==NULL)
	{
		return -3; 
	}

	if(WindowInfoNode->QueryIntAttribute("WndType", &wndtype)!=XML_NO_ERROR)
	{
		return -774;
	}

	if(WindowInfoNode->QueryIntAttribute("WndId", &wndid)!=XML_NO_ERROR)
	{
		return -49;
	}

	return 0;
}
	
int NmcMessage::BuildCreateUserStreamSourceReq(char* Cookie, string &ReqMsg)
{
/*
<?xml version="1.0" encoding="gb2312"?>
<ExPreparePushStreamReq Cache=""/>
*/
	tinyxml2::XMLDocument ReqDoc;

	if(XML_NO_ERROR!=ReqDoc.Parse("<ExPreparePushStreamReq/>\n"))//
	{
		return -1;
	}

	tinyxml2::XMLElement *pRootElement = ReqDoc.RootElement();
	ADD_ATTR(pRootElement,Cache,Cookie);
		
	tinyxml2::XMLPrinter streamer;
	ReqDoc.Accept(&streamer);
	string xmlstr(streamer.CStr());

	ExCommand Head;
	Head.nSrcType				= EX_SERVER_NMC;
	Head.nCmdType				= EX_PREPARE_PUSH_STREAM_REQ;
	Head.nCmdSeq				= ++static_nCmdSeq;
	Head.nContentSize			= xmlstr.size()+1;

	ReqMsg.assign((char*)&Head, sizeof(Head));

	ReqMsg.append(xmlstr.c_str(), xmlstr.size());

	return Head.nCmdSeq;
}
int NmcMessage::ParserCreateUserStreamSourceRsp(const string &RspMsg, std::string& strCache, struct PushStreamInfo &Inf, int &nRetCode, std::string& strRemark)
{
/*
<?xml version="1.0" encoding="gb2312"?>
<ExPreparePushStreamRsp Cache="">
	<RetVal Code="0" Remark="success"/>
	<RecvAddr Ip="192.168.1.11" Port="41002" />
</ExPreparePushStreamRsp>
*/
	ExCommand *Head = (ExCommand *)RspMsg.data();
	assert(Head->nCmdType == EX_PREPARE_PUSH_STREAM_RSP);

	tinyxml2::XMLDocument RspDoc;
	const char* data = RspMsg.data()+sizeof(ExCommand);
	if(RspDoc.Parse(data) != XML_NO_ERROR)
	{
		return -1;
	}

	tinyxml2::XMLElement *pRootElement = RspDoc.RootElement();
	if (NULL == pRootElement)
	{
		return -21;
	}
	strCache = SAFE_ITEM(pRootElement->Attribute("Cache"));

	tinyxml2::XMLElement* RetValNode = pRootElement->FirstChildElement("RetVal");
	if(RetValNode==NULL)
	{
		return -3; 
	}

	if(RetValNode->QueryIntAttribute("Code", &nRetCode)!=XML_NO_ERROR)
	{
		return -4;
	}

	strRemark = SAFE_ITEM(RetValNode->Attribute("Remark"));

	tinyxml2::XMLElement* RecvAddrNode = pRootElement->FirstChildElement("RecvAddr");
	if(RecvAddrNode==NULL)
	{
		return -3; 
	}

	Inf.strIp = SAFE_ITEM(RecvAddrNode->Attribute("Ip"));

	if(RecvAddrNode->QueryIntAttribute("Port",  &Inf.nPort)!=XML_NO_ERROR)
	{
		assert(false);
	}

	return 0;
}
int NmcMessage::BuildSetUserStreamSourceInfoReq(char* Cookie, struct PushStreamInfo &StreamInfo, struct st_video_stream_info *info, string &ReqMsg)
{
/*
<?xml version="1.0" encoding="gb2312"?>
<ExSetStreamInfoReq Cache="">
	<StreamInfo Ip="" Port="" BitRate="" FrameRate="" Width="" Height="" Extend=""/>
</ExSetStreamInfoReq>
*/
	tinyxml2::XMLDocument ReqDoc;

	if(XML_NO_ERROR!=ReqDoc.Parse("<ExSetStreamInfoReq/>\n"))//
	{
		return -1;
	}

	tinyxml2::XMLElement *pRootElement = ReqDoc.RootElement();
	ADD_ATTR(pRootElement,Cache,Cookie);

	ADD_NODE(ReqDoc,StreamInfo,pRootElement);
	//ADD_ATTR(StreamInfoNode,Uuid,StreamInfo.strGuid.c_str());
	//ADD_ATTR(StreamInfoNode,Uuid,StreamInfo.strIp.c_str());
	//ADD_ATTR(StreamInfoNode,Uuid,StreamInfo.nPort);
	ADD_ATTR(StreamInfoNode,CodecType,info->codec_type);
	ADD_ATTR(StreamInfoNode,BitRate,info->bitrate);
	ADD_ATTR(StreamInfoNode,FrameRate,info->fps);
	ADD_ATTR(StreamInfoNode,Width,info->width);
	ADD_ATTR(StreamInfoNode,Height,info->height);
	
	tinyxml2::XMLPrinter streamer;
	ReqDoc.Accept(&streamer);
	string xmlstr(streamer.CStr());

	ExCommand Head;
	Head.nSrcType				= EX_SERVER_NMC;
	Head.nCmdType				= EX_SET_STREAM_INFO_REQ;
	Head.nCmdSeq				= ++static_nCmdSeq;
	Head.nContentSize			= xmlstr.size()+1;

	ReqMsg.assign((char*)&Head, sizeof(Head));

	ReqMsg.append(xmlstr.c_str(), xmlstr.size());

	return Head.nCmdSeq;
}
int NmcMessage::ParserSetUserStreamSourceInfoRsp(const string &RspMsg, std::string& strCache, int &nRetCode, std::string& strRemark)
{
/*
<ExSetStreamInfoRsp Cache="">
	<RetVal Code="0" Remark="success"/>
</ExSetStreamInfoRsp>
*/
	ExCommand *Head = (ExCommand *)RspMsg.data();
	assert(Head->nCmdType == EX_SET_STREAM_INFO_RSP);

	tinyxml2::XMLDocument RspDoc;
	const char* data = RspMsg.data()+sizeof(ExCommand);
	if(RspDoc.Parse(data) != XML_NO_ERROR)
	{
		return -1;
	}

	tinyxml2::XMLElement *pRootElement = RspDoc.RootElement();
	if (NULL == pRootElement)
	{
		return -21;
	}
	strCache = SAFE_ITEM(pRootElement->Attribute("Cache"));

	tinyxml2::XMLElement* RetValNode = pRootElement->FirstChildElement("RetVal");
	if(RetValNode==NULL)
	{
		return -3; 
	}

	if(RetValNode->QueryIntAttribute("Code", &nRetCode)!=XML_NO_ERROR)
	{
		return -4;
	}

	strRemark = SAFE_ITEM(RetValNode->Attribute("Remark"));

	return 0;
}
int NmcMessage::BuildCtrlPushStreamReq(char* Cookie, int ctrltype, string &strLocalIp, int nLocalPort, struct PushStreamInfo &Inf, string &ReqMsg)
{
/*
<?xml version="1.0" encoding="gb2312"?>
<ExCtrlPushStreamReq Cache="">
	<CtrlInfo OpType="0"/>   //0???????1???????
	<SendAddr Uuid="" ClientIp="" ClientPort=""/>
</ExCtrlPushStreamReq>
*/
	tinyxml2::XMLDocument ReqDoc;

	if(XML_NO_ERROR!=ReqDoc.Parse("<ExCtrlPushStreamReq/>\n"))//
	{
		return -1;
	}

	tinyxml2::XMLElement *pRootElement = ReqDoc.RootElement();
	ADD_ATTR(pRootElement,Cache,Cookie);

	ADD_NODE(ReqDoc,CtrlInfo,pRootElement);
	ADD_ATTR(CtrlInfoNode,OpType,ctrltype);
	
	ADD_NODE(ReqDoc,SendAddr,pRootElement);
	//ADD_ATTR(SendAddrNode,Uuid,Inf.strGuid.c_str());
	ADD_ATTR(SendAddrNode,ClientIp,strLocalIp.c_str());
	ADD_ATTR(SendAddrNode,ClientPort,nLocalPort);
	
	tinyxml2::XMLPrinter streamer;
	ReqDoc.Accept(&streamer);
	string xmlstr(streamer.CStr());

	ExCommand Head;
	Head.nSrcType				= EX_SERVER_NMC;
	Head.nCmdType				= EX_CTRL_PUSH_STREAM_REQ;
	Head.nCmdSeq				= ++static_nCmdSeq;
	Head.nContentSize			= xmlstr.size()+1;

	ReqMsg.assign((char*)&Head, sizeof(Head));

	ReqMsg.append(xmlstr.c_str(), xmlstr.size());

	return Head.nCmdSeq;
}
int NmcMessage::ParserCtrlPushStreamRsp(const string &RspMsg, std::string& strCache, int &nRetCode, std::string& strRemark)
{
/*
<ExCtrlPushStreamRsp Cache="">
	<RetVal Code="0" Remark="success" />
</ExCtrlPushStreamRsp>
*/
	ExCommand *Head = (ExCommand *)RspMsg.data();
	assert(Head->nCmdType == EX_CTRL_PUSH_STREAM_RSP);

	tinyxml2::XMLDocument RspDoc;
	const char* data = RspMsg.data()+sizeof(ExCommand);
	if(RspDoc.Parse(data) != XML_NO_ERROR)
	{
		return -1;
	}

	tinyxml2::XMLElement *pRootElement = RspDoc.RootElement();
	if (NULL == pRootElement)
	{
		return -21;
	}
	strCache = SAFE_ITEM(pRootElement->Attribute("Cache"));

	tinyxml2::XMLElement* RetValNode = pRootElement->FirstChildElement("RetVal");
	if(RetValNode==NULL)
	{
		return -3; 
	}

	if(RetValNode->QueryIntAttribute("Code", &nRetCode)!=XML_NO_ERROR)
	{
		return -4;
	}

	strRemark = SAFE_ITEM(RetValNode->Attribute("Remark"));

	return 0;
}
int NmcMessage::BuildClearUserStreamSourceFromWindowReq(char* Cookie, struct PushStreamInfo &Inf, string &ReqMsg)
{
/*
<?xml version="1.0" encoding="gb2312"?>
<ExCtrlPushStreamReq Cache="">
	<CtrlInfo OpType="0"/>   //0???????1???????
	<SendAddr Uuid="" ClientIp="" ClientPort=""/>
</ExCtrlPushStreamReq>
*/
	tinyxml2::XMLDocument ReqDoc;

	if(XML_NO_ERROR!=ReqDoc.Parse("<ExCtrlPushStreamReq/>\n"))//
	{
		return -1;
	}

	tinyxml2::XMLElement *pRootElement = ReqDoc.RootElement();
	ADD_ATTR(pRootElement,Cache,Cookie);

	ADD_NODE(ReqDoc,CtrlInfo,pRootElement);
	ADD_ATTR(CtrlInfoNode,OpType,0);
	
	ADD_NODE(ReqDoc,SendAddr,pRootElement);
	ADD_ATTR(SendAddrNode,Uuid,Inf.strGuid.c_str());
	ADD_ATTR(SendAddrNode,ClientIp,Inf.strIp.c_str());
	ADD_ATTR(SendAddrNode,ClientPort,Inf.nPort);
	
	tinyxml2::XMLPrinter streamer;
	ReqDoc.Accept(&streamer);
	string xmlstr(streamer.CStr());

	ExCommand Head;
	Head.nSrcType				= EX_SERVER_NMC;
	Head.nCmdType				= EX_CTRL_PUSH_STREAM_REQ;
	Head.nCmdSeq				= ++static_nCmdSeq;
	Head.nContentSize			= xmlstr.size()+1;

	ReqMsg.assign((char*)&Head, sizeof(Head));

	ReqMsg.append(xmlstr.c_str(), xmlstr.size());

	return Head.nCmdSeq;

}
int NmcMessage::ParserClearUserStreamSourceFromWindowRsp(const string &RspMsg, std::string& strCache, int &nRetCode, std::string& strRemark)
{
/*
<ExCtrlPushStreamRsp Cache="">
	<RetVal Code="0" Remark="success"/>
</ExCtrlPushStreamRsp>
*/

	ExCommand *Head = (ExCommand *)RspMsg.data();
	assert(Head->nCmdType == EX_CTRL_PUSH_STREAM_RSP);

	tinyxml2::XMLDocument RspDoc;
	const char* data = RspMsg.data()+sizeof(ExCommand);
	if(RspDoc.Parse(data) != XML_NO_ERROR)
	{
		return -1;
	}

	tinyxml2::XMLElement *pRootElement = RspDoc.RootElement();
	if (NULL == pRootElement)
	{
		return -21;
	}
	strCache = SAFE_ITEM(pRootElement->Attribute("Cache"));

	tinyxml2::XMLElement* RetValNode = pRootElement->FirstChildElement("RetVal");
	if(RetValNode==NULL)
	{
		return -3; 
	}

	if(RetValNode->QueryIntAttribute("Code", &nRetCode)!=XML_NO_ERROR)
	{
		return -4;
	}

	strRemark = SAFE_ITEM(RetValNode->Attribute("Remark"));

	return 0;
}