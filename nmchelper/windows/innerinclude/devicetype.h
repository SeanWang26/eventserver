#ifndef DEVICE_TYPE_H
#define DEVICE_TYPE_H

#if (defined(WIN32) || defined(WIN64))

#ifdef JT_FRONT_USER_MODULE_EXPORTS
#define JT_FRONT_API __declspec(dllexport)
#else
#define JT_FRONT_API __declspec(dllimport)
#endif

#ifndef JT_CALL_TYPE
#define JT_CALL_TYPE	__stdcall  //__cdecl
#endif

#else //linux platform

#ifndef JT_FRONT_API
#define JT_FRONT_API
#endif
#ifndef JT_CALL_TYPE
#define JT_CALL_TYPE
#endif

#endif

//对象定义
#define OBJECT_TYPE_DEVICE  1
#define OBJECT_TYPE_CHANNEL 2
#define OBJECT_TYPE_INPUT   3
#define OBJECT_TYPE_OUTPUT  4
#define OBJECT_TYPE_STREAM  5
#define OBJECT_TYPE_RECORD  6

//设备类型定义
#define DEVICE_JN              1U	//竣泰
#define DEVICE_DH              2U	//大华
#define DEVICE_HK              3U	//海康
#define DEVICE_SN              4U	//索尼
#define DEVICE_XM              5U	//雄迈
#define DEVICE_HH              6U	//黄河
#define DEVICE_JY              7U	//景阳
#define DEVICE_ONVIF           8U	//ONVIF
#define DEVICE_LK			   9U	//录酷软件 
#define DEVICE_RTSP			   10U	//RTSP
#define DEVICE_LB              11U	//领标

#define DEVICE_XX              99U	//sb
#define DEVICE_MAX             100U	//sb

#define VIDEO_STREAM_DATA           0x0000000000000001
#define AUDIO_STREAM_DATA           0x0000000000000002
#define ALARM_STREAM_DATA           0x0000000000000004
#define RGB_STREAM_DATA             0x0000000000000008

#define VIDEO_ENCODE_JPEG                                   0x0010
#define VIDEO_ENCODE_HIK_JPEG                               0x0011
#define VIDEO_ENCODE_DAHUA_JPEG                             0x0012

#define VIDEO_ENCODE_VIDEO_MPEG4                            0x0020
#define VIDEO_ENCODE_HIK_MPEG4                              0x0021
#define VIDEO_ENCODE_DAHUA_MPEG4                            0x0022

#define VIDEO_ENCODE_VIDEO_MPEG2                            0x0040
#define VIDEO_ENCODE_HIK_MPEG2                              0x0041

#define VIDEO_ENCODE_VIDEO_H264                             0x0080
#define VIDEO_ENCODE_HIK_H264                               0x0081
#define VIDEO_ENCODE_DAHUA_H264                             0x0082

#define VIDEO_ENCODE_SVAC                                   0x0086  //国标svac编码

#define VIDEO_ENCODE_UNKNOWN                                0xF000  //未知的编码格式


#define UNKNOWN_FRAME                                        0
#define I_FRAME                                              1
#define B_FRAME                                              2
#define P_FRAME                                              3

#define AUDIO_G711                                       1
#define AUDIO_G711A                                      10
#define AUDIO_G711U                                      11
#define AUDIO_OGG_VORBIT                                 12
#define AUDIO_G726                                       13
#define AUDIO_AAC                                        14
#define AUDIO_PCM                                        15
#define AUDIO_AMR                                        16

#define AUDIO_ENCODE_UNKOWN                              9999

#define JT_ALARM_TYPE_VIDEO_MOTION                          1000 //移动侦测
#define JT_ALARM_TYPE_VIDEO_LOSS                            1001 //视频丢失
#define JT_ALARM_TYPE_VIDEO_BLIND                           1002 //遮挡
#define JT_ALARM_TYPE_INPUT                                 1003 //输入
#define JT_ALARM_TYPE_STORAGE_NOT_EXIST                     1004 //储存不存在

#define JT_ALARM_TYPE_UNKNOWN                               9999 //未知

#define JPTZ_MAX_SEQUENCE_NUM                             16   //云台巡航可设置的最大预置点数

//录像和查找录像
#define JT_RECODE_ALL                                 2000 // 所有录像
#define JT_RECODE_PLAN                                2001 // 计划录像
#define JT_RECODE_MANUAL                              2002 // 手动录像
#define JT_RECODE_ALARM                               2003 // 报警录像，统称所有的报警
#define JT_RECODE_ALARM_VIDEO_MOTION                  2004 // 移动侦测

//回放控制
#define RECORD_PLAY                  0 //播放
#define RECORD_STOP                  1 //停止
#define RECORD_FAST_PLAY             2 //快进
#define RECORD_FAST_REWIND           3 //快退
#define RECORD_FRAME_RORWARD_X1      4 //帧进
#define RECORD_FRAME_BACKWARD_X1     5 //帧退
#define RECORD_FRAME_PAUSE           6 //暂停
#define RECORD_SEEK                  7 //拖放


//云台
enum{JPTZ_UP=0, JPTZ_RIGHT_UP, JPTZ_RIGHT, JPTZ_RIGHT_DOWN, 
     JPTZ_DOWN, JPTZ_LEFT_DOWN, JPTZ_LEFT, JPTZ_LEFT_UP, 
     JPTZ_STOP,
     JPTZ_PUSH_FAR, JPTZ_PULL_NEAR, 
     JPTZ_IRIS_ADD, JPTZ_IRIS_SUB, 
	 JPTZ_FOCUS_FAR, JPTZ_FOCUS_NEAR , 
     JSET_PRESET, JCLEAR_PRESET,JGOTO_PRESET,
     JADD_TO_LOOP,JDEL_FROM_LOOP,JSTART_LOOP,JSTOP_LOOP,JREMOVE_LOOP,
     JPTZ_RESET
};

struct st_jt_daytime
{
	int year;					// 时标:年
	int month; 					// 时标:月
	int day;					// 时标:日
	int hour;					// 时标:时
	int minute;					// 时标:分
	int second;					// 时标:秒
};

struct encode_info
{
	int enable;
	int encodetype;
	int fps;
	int width;
	int height;
	int quality;
	int bitrate;
	int bitratectl;//0:变码率, 1:定码率
	int gop;
};

struct st_stream_data
{
	unsigned int streamtype;
	char* pdata;
	int   datalen;
	union 
	{
		struct
		{
			unsigned int encode;     //视频编码方式
			unsigned int frametype;  //i帧等...
			unsigned int width;      
			unsigned int height;     
			float        fps;        //帧率
			unsigned int bitrate;    //比特率
		}video_stream_info;
		struct
		{
			unsigned int  encode;    //音频编码方式
			unsigned char channel;   //声道数
			int           frequency; //采样率
			int           bitrate;   //比特率
			unsigned char depth;     //单位bit
			unsigned int  reserved;  //reserved
		}audio_stream_info;
		struct
		{	
			int           channelid;
			int           reason;
		}alarm_stream_info;
	}stream_info;
	unsigned long long llbegintime;
	unsigned long long llrelativetimetick;	//数据相对时间. 100ns
	int year;					// 时标:年
	int month; 					// 时标:月
	int day;					// 时标:日
	int hour;					// 时标:时
	int minute;					// 时标:分
	int second;					// 时标:秒

	unsigned int reserved[3];
};

struct st_video_record_info
{
	int subequid;
	int area;
	int index;
	int reason;
	char begintime[32];
	char endtime[32];
	int locked;
	char locktime[32];
};

//如果是停止的回调，用户可能会修改*user的值
typedef int (JT_CALL_TYPE *jt_stream_callback)(int callback_type, void* data, int len, void** user);

//如果是停止的回调，用户可能会修改*user的值
typedef void* (JT_CALL_TYPE *jt_talk_callback)(int callback_type, void* data, int len, void** user);


//回调事件类型
#define CALLBACK_TYPE_VIDEO_STREAM               1     //视频数据         流会产生该事件
#define CALLBACK_TYPE_AUDIO_STREAM               2     //音频流数据       通道会产生该事件
#define CALLBACK_TYPE_ALARM_STREAM               3     //报警流数据       设备会产生该事件

#define CALLBACK_TYPE_VIDEO_STREAM_OPENED        10     //视频流流打开了   流会产生该事件
#define CALLBACK_TYPE_VIDEO_STREAM_CLOSEED       11     //视频流关闭了     流会产生该事件

#define CALLBACK_TYPE_AUDIO_STREAM_OPENED        12     //音频流打开了    通道会产生该事件
#define CALLBACK_TYPE_AUDIO_STREAM_CLOSEED       13     //音频流关闭了    通道会产生该事件

#define CALLBACK_TYPE_ALARM_STREAM_OPENED        14     //报警流打开了    设备会产生该事件
#define CALLBACK_TYPE_ALARM_STREAM_CLOSEED       15     //报警流关闭了    设备会产生该事件

#define CALLBACK_TYPE_VIDEO_INfO_CHANGED         16     //视频流参数发生变化
#define CALLBACK_TYPE_AUDIO_INfO_CHANGED         17     //音频流参数发生变化

#define CALLBACK_TYPE_TALK_OPNED                 20     //对讲开始了
#define CALLBACK_TYPE_TALK_CLOSED                21     //对讲结束了
#define CALLBACK_TYPE_TALK_THREAD_TERMAL         22     //对讲线程终结了

#define CALLBACK_TYPE_DEVICE_INIT                98     //设备被初始化了    设备，通道，流会产生该事件
#define CALLBACK_TYPE_DEVICE_DELETED             99     //设备被删除了       设备，通道，流会产生该事件
#define CALLBACK_TYPE_DEVICE_DISCONNECT          100    //设备断线

#define CALLBACK_TYPE_JNDEVICE_NOTIFY            200    //jn类型的设备的通知回调


#endif
