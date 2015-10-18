#ifndef NMC_DEVICE_TYPE_H
#define NMC_DEVICE_TYPE_H

#include <string>
#include <vector>
using namespace std;

#include "jntype.h"
//c++ ��ʹ��

#ifdef WIN32 //windows platform

#ifdef NMC_USER_MODULE_EXPORTS
#define NMC_API __declspec(dllexport)
#else
#define NMC_API __declspec(dllimport)
#endif

#ifndef NMC_CALL_TYPE
#define NMC_CALL_TYPE	__stdcall  //__cdecl
#endif

#else //linux platform
 
#ifndef NMC_API
#define NMC_API  __attribute__ ((visibility ("default")))
#endif

#ifndef NMC_CALL_TYPE
#define NMC_CALL_TYPE
#endif

#endif

struct stOutputnfo
{
	int                  Id;                      //һ�����������ϵͳ�ı�ţ�һ�������Ӧһ������˿�
	int                  Index;                   //һ�������һ�����������ڲ��ı��
	int                  HasDisplay;              //�Ƿ�������ʾ��
	string               Resolution;         //֧�ֵĽ�����,֧�ֶ���Զ��Ÿ���,���硰1920x1080,1280x720��
};

struct stMatrixInfo
{
	int                            Id;                  //��������id��
	int                            Status;              //��������״̬�����߻����ߣ�������������������������������������������������
	string                         Name;                //����������
	string                         Ip;                  //������ip
	int                            Port;                //��½�˿�
	string                         User;                //��½�û���
	string                         Password;            //��½�˿�
	vector<struct stOutputnfo>     OutputInfoS;         //�ý������������Ϣ
};

struct stWindowInfo
{
	int                     Id;              //���ڵ�ȫ��id
	int                     Index;           //�����ڱ����������      
	int                     Status;          //����״̬��0���С�1ʵʱ��������RTSP��¼������2¼��3��������Ѳ��4������Ѳ��5GIS��6����
	int                     SourceId;        //�ڲ�nvr��id
	string                  SurveyUuid;      //��ʱ����
	string                  Extend;          //sourceidΪ�ڲ�nvr��idʱ��ExtendΪһ�����豸id
};

struct stOutputLayout
{
	int                             Id;                   //�ò��ַ�ʽ��id��
	int                             Layout;               //���ַ�ʽ��1����һ������2����2������4����4�������Դ�����
	vector<struct stWindowInfo>   WindowInfoS;          //�ò����µĴ�����Ϣ
};

struct PushStreamInfo
{
	string strIp;
	int    nPort;
	string strUser;
	string strPassword;
	string strGuid;
	int  nState; //0.δ��ռ��    1.�ѱ�ռ�ã����ǻ�û��ʼ����   2.�ѿ�ʼ����
	int  jn_equ_id;
	int  jn_equ_sub_id;
};

#endif



