#ifndef COMMUNICATION_WITH_NVR_H
#define COMMUNICATION_WITH_NVR_H

#include "devicetype.h"

#define  SUCCESS                  0				//�ɹ�
#define  UNKNOWN_ERROR   -1		//δ֪����
#define  ABNORMAL_STATUS   -2	//exe�����ʱ�쳣
#define  THREAD_EXCESS   -3		//�̳߳���

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

//��ȡ���õ�������
#define GET_ENCODE_CONFIG               1//��ȡ��������
#define GET_EQU_INFO                    2//��ȡ���豸��Ϣ
#define GET_SUB_EQU_INFO                3//��ȡ���豸��Ϣ
#define GET_RECORD_ENCODE_CONFIG        4//��ȡ¼���������

#define SET_CONFIG_ADD_EQU              100  //���һ�����豸
#define SET_CONFIG_MOD_EQU              101  //���һ�����豸
#define SET_CONFIG_DEL_EQU              102  //ɾ��һ�����豸
#define SET_CONFIG_DEL_SUB_EQU          103  //ɾ��һ�����豸


//#pragma pack(4)
struct stHead
{
	unsigned int Size;
	unsigned int Sequence;
	unsigned int Type;
	unsigned int Result;
};

//���豸
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

//�ر��豸
struct stLogout_Req
{
	long long				DeviceHandle;
};
struct stLogout_Rsp
{
	long long				DeviceHandle;
};

//��ȡ������Ϣ
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

//����������Ϣ
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

//����Ƶ��
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

//�ر���Ƶ��
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

//����Ƶ��
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
//�ر���Ƶ��
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

//����������
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
//�رձ�����
struct stCloseAlarmStream_Req
{
	long long				DeviceHandle;
};
struct stCloseAlarmStream_Rsp
{
	long long				DeviceHandle;
};

//�����Խ�
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
//�رնԽ�
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

//����
enum { JT_POWER_OFF=1, JT_REBOOT=2};

struct stReboot_Req
{
	long long				DeviceHandle;
	int                     Type;  ///�ػ�����
};
struct stReboot_Rsp
{
	long long				DeviceHandle;
	int                     Reslute;
};
//�趨ʱ��
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

//����ͼ�����
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

//��ȡͼ�����
struct stGetVideoEffect_Req
{
	long long				DeviceHandle;
	int						Channel;
};

struct stGetVideoEffect_Rsp
{
	long long				DeviceHandle;
	int						Channel;
	int                     BrightValue;                        // ����
	int	                    ContrastValue;                      //�Աȶ�
	int	                    SaturationValue;                    //���Ͷ�
	int      	            HueValue;                           //ɫ��
};

//��ȡ¼����Ϣ
struct stGetVideoRecord_Req
{
	long long				DeviceHandle;
	int						ChannelS[64];                    //���ͬʱ����64·��¼��
	int                     NewFind;                         //�Ƿ�Ϊһ���µĲ���
	char                    TaskUuid[64];                    //���µĲ���Ϊ�ϴε������
	char                    StartTime[32];					 //��ʼʱ��
	char                    EndTime[32];                     //����ʱ��
	int                     Reason[4];                       //���ͬʱ����4��ԭ���¼��
};

struct stGetVideoRecord_Rsp
{
	long long                       DeviceHandle;
	int                             TotalConut;
	char                    		TaskUuid[64];              // �����������
	int                     		NewFind;                   // �ļ�����	
	struct st_video_record_info*    VideoRecordInfo;
	int                             Conut;
};

//�طſ���
struct stPlaybackCtrl_Req
{
	long long				DeviceHandle;
	int                     Area;
	char                    TaskUuid[64];              // �����������
	int                     Channel;                   // ͨ����
	int                     Index;                     // �ļ�����
	int                     Action;                    // 0���ţ�1 ֹͣ��2�����3���ˣ�4֡����5֡�ˣ�6��ͣ��7�Ϸ�
	int                     Speed;                     // ȡֵ��Χ1-64
	long long               SeekTime;                  // 64λ���Ϸ��ƶ��������ʱ�� ""
	jt_stream_callback      Callback;
	void*                   UserData;
};

struct stPlaybackCtrl_Rsp
{
	long long				DeviceHandle;
	long					StreamHandle;
	char                    TaskUuid[64];              // �����������
	int                     index;                     // �ļ�����
	int                     Action;                    // 0���ţ�1 ֹͣ��2�����3���ˣ�4֡����5֡�ˣ�6��ͣ��7�Ϸ�
	int                     Speed;                     // ȡֵ��Χ1-64
	int                     SeekTime;                  // 64λ���Ϸ��ƶ��������ʱ�䣬��λ100����
};

enum { JT_START_MANUAL_RECORD=1, JT_STOP_MANUAL_RECORD=0};


//�ֶ�¼��
struct stManualRecord_Req
{
	long long				DeviceHandle;
	int                     Channel;
	int                     Codec;
	int                     Action;    //  1��ʼ¼�ƣ�0ֹͣ¼��
};

struct stManualRecord_Rsp
{
	long long				DeviceHandle;
	int                     Action; 
};


enum { PTZ_MOVE=0, PTZ_STOP, PTZ_TOUR_START, PTZ_TOUR_STOP, SET_DEFAULT_POS, GOTO_PRESET, SET_PRESET, CLEAR_PRESET, SET_TOUR_LIST, AUTO_FOUS};

//��̨����
/*typedef enum {PTZ_LEFT,PTZ_LEFT_UP,PTZ_UP,PTZ_UP_RIGHT,PTZ_RIGHT,PTZ_DOWN_RIGHT,PTZ_DOWN,PTZ_DOWN_LEFT,PTZ_ABSOLUTE_POS,PTZ_STOP_MOVE,\
	PTZ_RESET, PTZ_PUSH_FAR, PTZ_PULL_NEAR,PTZ_IRIS_SUB,PTZ_IRIS_ADD,PTZ_FOCUS_FAR,PTZ_FOCUS_NEAR}PTZ_DIRECTION;
*/
//PTZ����
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
	//��ʼʱ��
	unsigned char	 byStartHour;
	unsigned char	 byStartMin;
	//����ʱ��
	unsigned char	 byStopHour;
	unsigned char	 byStopMin;
};

struct stSpecialGuard
{
	//��ʼʱ��
	struct st_jt_daytime    StartTime;
	//����ʱ��
	struct st_jt_daytime	StopTime;
};


union unGuardTime
{
	struct stWeekGuard		WeekGuardTime[7][8];
	struct stSpecialGuard   SpecialGuardTime[16];
};

enum { JT_WEEK_GUARD=1, JT_SPECIAL_GUARD=2, JT_MANUAL_GUARD=3};

//���ò���
struct stSetGuard_Req
{
	long long				DeviceHandle;
	int                     ChannelS[64];  //����ͬʱ����64·
	int                     Type;         //�������ͣ�1���ܲ�����ʹ��stWeekGuard�ṹ��2����ʱ�䲼����ʹ��st_jt_daytime�ṹ, 3�ֶ�����
	unGuardTime             GuardTime;
};

//ʹ��ǰ���ʹ��memset��ֵ0
struct stSetGuard_Rsp
{
	long long				DeviceHandle;
	int                     Channel;
	int                     Type;         //�������ͣ�1���ܲ�����ʹ��WeekGuardTime�ṹ��2����ʱ�䲼����ʹ��SpecialGuardTime�ṹ, 3�ֶ�����
	unGuardTime             GuardTime;
};

//��ȡ����
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
	int                     Type;         //�������ͣ�1���ܲ�����ʹ��WeekGuardTime�ṹ��2����ʱ�䲼����ʹ��SpecialGuardTime�ṹ, 3�ֶ�����
	unGuardTime             GuardTime;
};

//���
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
	//�������֪ͨ
	//AddCommander(Startup_Req);

	//�򿪹ر�
	AddCommander(Login_Req);
	AddCommander(Login_Rsp);
	AddCommander(Logout_Req);
	AddCommander(Logout_Rsp);
	//��ȡ������Ϣ
	AddCommander(GetConfig_Req);
	AddCommander(GetConfig_Rsp);
	//�򿪹ر���Ƶ��
	AddCommander(OpenVideoStream_Req);
	AddCommander(OpenVideoStream_Rsp);
	AddCommander(CloseVideoStream_Req);
	AddCommander(CloseVideoStream_Rsp);
	//�򿪹ر���Ƶ��
	AddCommander(OpenAudioStream_Req);
	AddCommander(OpenAudioStream_Rsp);
	AddCommander(CloseAudioStream_Req);
	AddCommander(CloseAudioStream_Rsp);
	//�򿪹رձ�����
	AddCommander(OpenAlarmStream_Req);
	AddCommander(OpenAlarmStream_Rsp);
	AddCommander(CloseAlarmStream_Req);
	AddCommander(CloseAlarmStream_Rsp);
	//�򿪹رնԽ�
	AddCommander(StartTalk_Req);
	AddCommander(StartTalk_Rsp);
	AddCommander(StopTalk_Req);
	AddCommander(StopTalk_Rsp);
	//�����豸
	AddCommander(Reboot_Req);
	AddCommander(Reboot_Rsp);
	//����ʱ��
	AddCommander(SetTime_Req);
	AddCommander(SetTime_Rsp);
	//PTZ����
	AddCommander(PTZControl_Req);
	AddCommander(PTZControl_Rsp);
	//�������
	AddCommander(SetOutput_Req);
	AddCommander(SetOutput_Rsp);
};

struct stCommander
{
	AddCommander(Head);
	union unCommander Cmd;
};

















#endif
