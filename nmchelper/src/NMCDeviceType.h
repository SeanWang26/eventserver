#ifndef NMC_DEVICE_TYPE_H
#define NMC_DEVICE_TYPE_H

#include <string>
#include <vector>
using namespace std;

#include "jntype.h"
//c++ 层使用

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
	int                  Id;                      //一个输出在整个系统的编号，一个输出对应一个物理端口
	int                  Index;                   //一个输出在一个解码器的内部的编号
	int                  HasDisplay;              //是否连接显示器
	string               Resolution;         //支持的解析度,支持多个以逗号隔开,比如“1920x1080,1280x720”
};

struct stMatrixInfo
{
	int                            Id;                  //解码器的id号
	int                            Status;              //解码器的状态，在线或离线？？？？？？？？？？？？？？？？？？？？？？？？？
	string                         Name;                //解码器名称
	string                         Ip;                  //解码器ip
	int                            Port;                //登陆端口
	string                         User;                //登陆用户名
	string                         Password;            //登陆端口
	vector<struct stOutputnfo>     OutputInfoS;         //该解码器的输出信息
};

struct stWindowInfo
{
	int                     Id;              //窗口的全局id
	int                     Index;           //窗口在本输出的索引      
	int                     Status;          //窗口状态，0空闲、1实时流（包括RTSP和录屏）、2录像、3单窗口轮巡、4分组轮巡、5GIS、6报警
	int                     SourceId;        //内部nvr的id
	string                  SurveyUuid;      //暂时无用
	string                  Extend;          //sourceid为内部nvr的id时，Extend为一个子设备id
};

struct stOutputLayout
{
	int                             Id;                   //该布局方式的id号
	int                             Layout;               //布局方式，1代表一分屏，2代表2分屏，4代表4分屏，以此类推
	vector<struct stWindowInfo>   WindowInfoS;          //该布局下的窗口信息
};

struct PushStreamInfo
{
	string strIp;
	int    nPort;
	string strUser;
	string strPassword;
	string strGuid;
	int  nState; //0.未被占用    1.已被占用，但是还没开始推流   2.已开始推流
	int  jn_equ_id;
	int  jn_equ_sub_id;
};

#endif



