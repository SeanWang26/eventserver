#ifndef COMMUNICATION_WITH_NVR_H
#define COMMUNICATION_WITH_NVR_H

#include "devicetype.h"

#define  SUCCESS                  0				//成功
#define  UNKNOWN_ERROR   -1		//未知错误
#define  ABNORMAL_STATUS   -2	//exe信令处理超时异常
#define  THREAD_EXCESS   -3		//线程超量

#define  ABNORMAL_RSP  0

#define  STARTUP_REQ 					0
#define  LOGIN_REQ                      1
#define  LOGOUT_REQ 					2 
#define  GET_ENCODE_REQ 				3
#define  OPEN_VIDEO_STREAM_REQ  		4
#define  CLOSE_VIDEO_STREAM_REQ  		5
#define  OPEN_AUDIO_STREAM_REQ  		6
#define  CLOSE_AUDIO_STREAM_REQ  		7
#define  OPEN_ALARM_REQ                 8
#define  CLOSE_ALARM_REQ                9

//获取配置的子命令
#define GET_ENCODE_CONFIG               1//获取编码配置
#define GET_EQU_INFO                    2//获取主设备信息
#define GET_SUB_EQU_INFO                3//获取主设备信息
#define GET_RECORD_ENCODE_CONFIG        4//获取录像编码配置

#define SET_CONFIG_ADD_EQU              100  //添加一个主设备
#define SET_CONFIG_MOD_EQU              101  //添加一个主设备
#define SET_CONFIG_DEL_EQU              102  //删除一个主设备
#define SET_CONFIG_DEL_SUB_EQU          103  //删除一个子设备


//#pragma pack(4)
struct stHead
{
	unsigned int Size;
	unsigned int Sequence;
	unsigned int Type;
	unsigned int Result;
};

//打开设备
struct stLogin_Req
{
	char					Ip[256];
	unsigned int			Port;
	char					User[32];
	char					Password[32];
	jt_stream_callback      EventCallback;
	void*                   EventCallbackUserData;
};
struct stLogin_Rsp
{
	long long				DeviceHandle;
};

//关闭设备
struct stLogout_Req
{
	long long				DeviceHandle;
};
struct stLogout_Rsp
{
	long long				DeviceHandle;
};

//获取配置信息
struct stGetConfig_Req
{
	long long				DeviceHandle;
	unsigned int			Type;
	int	      		        Channel;
	int                     Codec;
};
struct stGetConfig_Rsp
{
	unsigned int			Type;
	unsigned int			Channel;
	unsigned int			Size;
	char*					Config;
};

//设置配置信息
struct stSetConfig_Req
{
	long long				DeviceHandle;
	unsigned int			Type;
	int	      		        Channel;
	int                     Codec;
	unsigned int			Size;
	char*					Config;
};
struct stSetConfig_Rsp
{
	long long				DeviceHandle;
	unsigned int			Type;
	int	      		        Channel;
	int                     Codec;
};

//打开视频流
struct stOpenVideoStream_Req
{
	long long               DeviceHandle;
	int						Channel;
	int                     Codec;
	jt_stream_callback      Callback;
	jt_stream_callback      Callback2;
	void*                   UserData;
};
struct stOpenVideoStream_Rsp
{
	long long		DeviceHandle;
	long			StreamHandle;
};

//关闭视频流
struct stCloseVideoStream_Req
{
	long long				DeviceHandle;
	long					StreamHandle;
	unsigned int			Channel;
	unsigned int			Codec;
};
struct stCloseVideoStream_Rsp
{
	long long				DeviceHandle;
	long					StreamHandle;
};

//打开音频流
struct stOpenAudioStream_Req
{
	long long				DeviceHandle;
	unsigned int			Channel;
	jt_stream_callback      Callback;
	void*                   UserData;
};
struct stOpenAudioStream_Rsp
{
	long long				DeviceHandle;
	long					ChannelHandle;
};
//关闭音频流
struct stCloseAudioStream_Req
{
	long long				DeviceHandle;
	long                    ChannelHandle;
	unsigned int			Channel;
};
struct stCloseAudioStream_Rsp
{
	long long				DeviceHandle;
	unsigned int			Channel;
};

//开启报警流
struct stOpenAlarmStream_Req
{
	long long				DeviceHandle;
	jt_stream_callback      Callback;
	void*                   UserData;	
};
struct stOpenAlarmStream_Rsp
{
	long long				DeviceHandle;
};
//关闭报警流
struct stCloseAlarmStream_Req
{
	long long				DeviceHandle;
};
struct stCloseAlarmStream_Rsp
{
	long long				DeviceHandle;
};

//开启对讲
struct stStartTalk_Req
{
	long long				DeviceHandle;
	int						Channel;
	jt_talk_callback        TalkCallback;            
};
struct stStartTalk_Rsp
{
	long long				DeviceHandle;
	void*                   TalkHandle;
	int						Channel;
};
//关闭对讲
struct stStopTalk_Req
{
	long long				DeviceHandle;
	int						Channel;
};
struct stStopTalk_Rsp
{
	long long				DeviceHandle;
	int						Channel;
};

struct stSendTalkData_Req
{
	long long				DeviceHandle;
	int						Channel;
	unsigned char*          Data;
	int                     DataLen;
};

struct stSendTalkData_Rsp
{
	long long				DeviceHandle;
	int						Channel;
};

//重启
enum { JT_POWER_OFF=1, JT_REBOOT=2};

struct stReboot_Req
{
	long long				DeviceHandle;
	int                     Type;  ///关机重启
};
struct stReboot_Rsp
{
	long long				DeviceHandle;
	int                     Reslute;
};
//设定时间
struct stSetTime_Req
{
	long long				DeviceHandle;
	int						year;
	int						month;
	int						day;
	int						hour;
	int						minute;
	int						second;
};
struct stSetTime_Rsp
{
	long long				DeviceHandle;
	int						year;
	int						month;
	int						day;
	int						hour;
	int						minute;
	int						second;
};

//设置图像参数
struct stSetVideoEffect_Req
{
	long long				DeviceHandle;
	int						Channel;
	int                     BrightValue;
	int	                    ContrastValue;
	int	                    SaturationValue;
	int      	            HueValue;
};
struct stSetVideoEffect_Rsp
{
	long long				DeviceHandle;
	int                     Reslut;
};

//获取图像参数
struct stGetVideoEffect_Req
{
	long long				DeviceHandle;
	int						Channel;
};

struct stGetVideoEffect_Rsp
{
	long long				DeviceHandle;
	int						Channel;
	int                     BrightValue;                        // 亮度
	int	                    ContrastValue;                      //对比度
	int	                    SaturationValue;                    //饱和度
	int      	            HueValue;                           //色度
};

//获取录像信息
struct stGetVideoRecord_Req
{
	long long				DeviceHandle;
	int						ChannelS[64];                    //最多同时查找64路的录像
	int                     NewFind;                         //是否为一次新的查找
	char                    TaskUuid[64];                    //非新的查找为上次的任务号
	char                    StartTime[32];					 //开始时间
	char                    EndTime[32];                     //结束时间
	int                     Reason[4];                       //最多同时查找4种原因的录像
};

struct stGetVideoRecord_Rsp
{
	long long                       DeviceHandle;
	int                             TotalConut;
	char                    		TaskUuid[64];              // 检索的任务号
	int                     		NewFind;                   // 文件索引	
	struct st_video_record_info*    VideoRecordInfo;
	int                             Conut;
};

//回放控制
struct stPlaybackCtrl_Req
{
	long long				DeviceHandle;
	int                     Area;
	char                    TaskUuid[64];              // 检索的任务号
	int                     Channel;                   // 通道号
	int                     Index;                     // 文件索引
	int                     Action;                    // 0播放，1 停止，2快进，3快退，4帧进，5帧退，6暂停，7拖放
	int                     Speed;                     // 取值范围1-64
	long long               SeekTime;                  // 64位，拖放移动到的相对时间 ""
	jt_stream_callback      Callback;
	void*                   UserData;
};

struct stPlaybackCtrl_Rsp
{
	long long				DeviceHandle;
	long					StreamHandle;
	char                    TaskUuid[64];              // 检索的任务号
	int                     index;                     // 文件索引
	int                     Action;                    // 0播放，1 停止，2快进，3快退，4帧进，5帧退，6暂停，7拖放
	int                     Speed;                     // 取值范围1-64
	int                     SeekTime;                  // 64位，拖放移动到的相对时间，单位100纳秒
};

enum { JT_START_MANUAL_RECORD=1, JT_STOP_MANUAL_RECORD=0};


//手动录像
struct stManualRecord_Req
{
	long long				DeviceHandle;
	int                     Channel;
	int                     Codec;
	int                     Action;    //  1开始录制，0停止录制
};

struct stManualRecord_Rsp
{
	long long				DeviceHandle;
	int                     Action; 
};


enum { PTZ_MOVE=0, PTZ_STOP, PTZ_TOUR_START, PTZ_TOUR_STOP, SET_DEFAULT_POS, GOTO_PRESET, SET_PRESET, CLEAR_PRESET, SET_TOUR_LIST, AUTO_FOUS};

//云台方向
/*typedef enum {PTZ_LEFT,PTZ_LEFT_UP,PTZ_UP,PTZ_UP_RIGHT,PTZ_RIGHT,PTZ_DOWN_RIGHT,PTZ_DOWN,PTZ_DOWN_LEFT,PTZ_ABSOLUTE_POS,PTZ_STOP_MOVE,\
	PTZ_RESET, PTZ_PUSH_FAR, PTZ_PULL_NEAR,PTZ_IRIS_SUB,PTZ_IRIS_ADD,PTZ_FOCUS_FAR,PTZ_FOCUS_NEAR}PTZ_DIRECTION;
*/
//PTZ控制
struct stPTZControl_Req
{
	long long				DeviceHandle;
	int						Channel;
	unsigned int			Action;
	int						Direction;
	int						Speed;
	int						TourDelayTime;
	int						TourNum;
	int						PresetNum;
	char					PresetName[32];
	int						SequenceGroup[JPTZ_MAX_SEQUENCE_NUM];
	int						SequenceNum;
	int						Preset;
};
struct stPTZControl_Rsp
{
	long long				DeviceHandle;
	int						Channel;
	unsigned int			Action;
	int						Speed;
	int						Preset;					
};

struct stWeekGuard
{
	//开始时间
	unsigned char	 byStartHour;
	unsigned char	 byStartMin;
	//结束时间
	unsigned char	 byStopHour;
	unsigned char	 byStopMin;
};

struct stSpecialGuard
{
	//开始时间
	struct st_jt_daytime    StartTime;
	//结束时间
	struct st_jt_daytime	StopTime;
};


union unGuardTime
{
	struct stWeekGuard		WeekGuardTime[7][8];
	struct stSpecialGuard   SpecialGuardTime[16];
};

enum { JT_WEEK_GUARD=1, JT_SPECIAL_GUARD=2, JT_MANUAL_GUARD=3};

//设置布防
struct stSetGuard_Req
{
	long long				DeviceHandle;
	int                     ChannelS[64];  //可以同时设置64路
	int                     Type;         //布防类型，1按周布防，使用stWeekGuard结构，2特殊时间布防，使用st_jt_daytime结构, 3手动布防
	unGuardTime             GuardTime;
};

//使用前最好使用memset来值0
struct stSetGuard_Rsp
{
	long long				DeviceHandle;
	int                     Channel;
	int                     Type;         //布防类型，1按周布防，使用WeekGuardTime结构，2特殊时间布防，使用SpecialGuardTime结构, 3手动布防
	unGuardTime             GuardTime;
};

//获取布防
struct stGetGuard_Req
{
	long long				DeviceHandle;
	int                     Channel;
	int                     Type;
};

struct stGetGuard_Rsp
{
	long long				DeviceHandle;
	int                     Channel;
	int                     Type;         //布防类型，1按周布防，使用WeekGuardTime结构，2特殊时间布防，使用SpecialGuardTime结构, 3手动布防
	unGuardTime             GuardTime;
};

//输出
struct stSetOutput_Req
{
	long long				DeviceHandle;
	int						AlarmOutNum;
	int						Enable;
	int						AlarmOutState;
};
struct stSetOutput_Rsp
{
	long long				DeviceHandle;
	int						AlarmOutNum;
	int						Enable;
	int						AlarmOutState;
};



#define AddCommander(STRUCT) struct	st##STRUCT	STRUCT

union unCommander
{
	//启动完毕通知
	//AddCommander(Startup_Req);

	//打开关闭
	AddCommander(Login_Req);
	AddCommander(Login_Rsp);
	AddCommander(Logout_Req);
	AddCommander(Logout_Rsp);
	//获取编码信息
	AddCommander(GetConfig_Req);
	AddCommander(GetConfig_Rsp);
	//打开关闭视频流
	AddCommander(OpenVideoStream_Req);
	AddCommander(OpenVideoStream_Rsp);
	AddCommander(CloseVideoStream_Req);
	AddCommander(CloseVideoStream_Rsp);
	//打开关闭音频流
	AddCommander(OpenAudioStream_Req);
	AddCommander(OpenAudioStream_Rsp);
	AddCommander(CloseAudioStream_Req);
	AddCommander(CloseAudioStream_Rsp);
	//打开关闭报警流
	AddCommander(OpenAlarmStream_Req);
	AddCommander(OpenAlarmStream_Rsp);
	AddCommander(CloseAlarmStream_Req);
	AddCommander(CloseAlarmStream_Rsp);
	//打开关闭对讲
	AddCommander(StartTalk_Req);
	AddCommander(StartTalk_Rsp);
	AddCommander(StopTalk_Req);
	AddCommander(StopTalk_Rsp);
	//重启设备
	AddCommander(Reboot_Req);
	AddCommander(Reboot_Rsp);
	//设置时间
	AddCommander(SetTime_Req);
	AddCommander(SetTime_Rsp);
	//PTZ控制
	AddCommander(PTZControl_Req);
	AddCommander(PTZControl_Rsp);
	//报警输出
	AddCommander(SetOutput_Req);
	AddCommander(SetOutput_Rsp);
};

struct stCommander
{
	AddCommander(Head);
	union unCommander Cmd;
};

















#endif
