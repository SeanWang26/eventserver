#ifndef JT_USER_SDK_H
#define JT_USER_SDK_H

#if (defined(WIN32) || defined(WIN64))

#ifndef JT_FRONT_USER_MODULE_EXPORTS
#pragma comment(lib,"frontplug.lib") 
#endif

#else

#endif

#include "devicetype.h"
#include "command.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef int (JT_CALL_TYPE *jt_init_dl)(void* );
typedef int (JT_CALL_TYPE *jt_uninit_dl)(void* );
typedef int (JT_CALL_TYPE *jt_set_event_callback_dl)(jt_stream_callback callback, void* userdata);

typedef void* (JT_CALL_TYPE *jt_create_device_dl)(unsigned int type);
typedef int (JT_CALL_TYPE *jt_distory_device_dl)(void* handle);

typedef int (JT_CALL_TYPE *jt_login_dl)(void* handle, struct stLogin_Req *req, struct stLogin_Rsp *rsp);
typedef int (JT_CALL_TYPE *jt_login_ex_dl)(void* handle, int type, struct stLogin_Req *req, struct stLogin_Rsp *rsp);//创建一个新的设备，并登陆
typedef int (JT_CALL_TYPE *jt_logout_dl)(void* handle, struct stLogout_Req *req, struct stLogout_Rsp *rsp);
typedef int (JT_CALL_TYPE *jt_logout_ex_dl)(void* handle, struct stLogout_Req *req, struct stLogout_Rsp *rsp);//退出登陆，删除设备

typedef int (JT_CALL_TYPE *jt_get_config_dl)(void* handle, struct stGetConfig_Req *req, struct stGetConfig_Rsp *rsp);
typedef int (JT_CALL_TYPE *jt_set_config_dl)(void* handle, struct stSetConfig_Req *req, struct stSetConfig_Rsp *rsp);
typedef int (JT_CALL_TYPE *jt_open_video_stream_dl)(void* handle, struct stOpenVideoStream_Req *req, struct stOpenVideoStream_Rsp *rsp);
typedef int (JT_CALL_TYPE *jt_close_video_stream_dl)(void* handle, struct stCloseVideoStream_Req *req, struct stCloseVideoStream_Rsp *rsp);
typedef int (JT_CALL_TYPE *jt_open_audio_stream_dl)(void* handle, struct stOpenAudioStream_Req *req, struct stOpenAudioStream_Rsp *rsp);
typedef int (JT_CALL_TYPE *jt_close_audio_stream_dl)(void* handle, struct stCloseAudioStream_Req *req, struct stCloseAudioStream_Rsp *rsp);
typedef int (JT_CALL_TYPE *jt_open_alarm_stream_dl)(void* handle, struct stOpenAlarmStream_Req *req, struct stOpenAlarmStream_Rsp *rsp);
typedef int (JT_CALL_TYPE *jt_close_alarm_stream_dl)(void* handle, struct stCloseAlarmStream_Req *req, struct stCloseAlarmStream_Rsp *rsp);
typedef int (JT_CALL_TYPE *jt_ptz_control_dl)(void* handle, struct stPTZControl_Req *req, struct stPTZControl_Rsp *rsp);
typedef int (JT_CALL_TYPE *jt_set_system_time_dl)(void* handle, struct stSetTime_Req *req, struct stSetTime_Rsp *rsp);
typedef int (JT_CALL_TYPE *jt_start_talk_dl)(void* handle, struct stStartTalk_Req *req, struct stStartTalk_Rsp *rsp);
typedef int (JT_CALL_TYPE *jt_stop_talk_dl)(void* handle, struct stStopTalk_Req *req, struct stStopTalk_Rsp *rsp);
typedef int (JT_CALL_TYPE *jt_send_talk_data_dl)(void* handle, struct stSendTalkData_Req *req, struct stSendTalkData_Rsp *rsp);
typedef int (JT_CALL_TYPE *jt_get_video_effect_dl)(void* handle, struct stGetVideoEffect_Req *req, struct stGetVideoEffect_Rsp *rsp);
typedef int (JT_CALL_TYPE *jt_set_video_effect_dl)(void* handle, struct stSetVideoEffect_Req *req, struct stSetVideoEffect_Rsp *rsp);
typedef int (JT_CALL_TYPE *jt_get_video_record_dl)(void* handle, struct stGetVideoRecord_Req *req, struct stGetVideoRecord_Rsp *rsp);
typedef int (JT_CALL_TYPE *jt_playback_control_dl)(void* handle, struct stPlaybackCtrl_Req *req, struct stPlaybackCtrl_Rsp *rsp);
typedef int (JT_CALL_TYPE *jt_manual_record_dl)(void* handle, stManualRecord_Req *req, struct stManualRecord_Rsp *rsp);//手动录像
typedef int (JT_CALL_TYPE *jt_set_guard_dl)(void* handle, struct stSetGuard_Req *req, struct stSetGuard_Rsp *rsp);//设置布防
typedef int (JT_CALL_TYPE *jt_get_guard_dl)(void* handle, struct stGetGuard_Req *req, struct stGetGuard_Rsp *rsp);//获取布防
typedef int (JT_CALL_TYPE *jt_reboot_control_dl)(void* handle, struct stReboot_Req *req, struct stReboot_Rsp *rsp);//重启，关机
typedef int (JT_CALL_TYPE *jt_set_output_dl)(struct device *, struct stSetOutput_Req *req, struct stSetOutput_Rsp *rsp);//输出控制

JT_FRONT_API int JT_CALL_TYPE jt_init(void* );
JT_FRONT_API int JT_CALL_TYPE jt_uninit(void* );
JT_FRONT_API int JT_CALL_TYPE jt_set_event_callback(jt_stream_callback callback, void* userdata);

JT_FRONT_API void* JT_CALL_TYPE jt_create_device(unsigned int type);
JT_FRONT_API int JT_CALL_TYPE jt_distory_device(void* handle);

JT_FRONT_API int JT_CALL_TYPE jt_login(void* handle, struct stLogin_Req *req, struct stLogin_Rsp *rsp);
JT_FRONT_API int JT_CALL_TYPE jt_login_ex(void* handle, int type, struct stLogin_Req *req, struct stLogin_Rsp *rsp);//创建一个新的设备，并登陆
JT_FRONT_API int JT_CALL_TYPE jt_logout(void* handle, struct stLogout_Req *req, struct stLogout_Rsp *rsp);
JT_FRONT_API int JT_CALL_TYPE jt_logout_ex(void* handle, struct stLogout_Req *req, struct stLogout_Rsp *rsp);//退出登陆，删除设备

JT_FRONT_API int JT_CALL_TYPE jt_get_config(void* handle, struct stGetConfig_Req *req, struct stGetConfig_Rsp *rsp);
JT_FRONT_API int JT_CALL_TYPE jt_set_config(void* handle, struct stSetConfig_Req *req, struct stSetConfig_Rsp *rsp);
JT_FRONT_API int JT_CALL_TYPE jt_open_video_stream(void* handle, struct stOpenVideoStream_Req *req, struct stOpenVideoStream_Rsp *rsp);
JT_FRONT_API int JT_CALL_TYPE jt_close_video_stream(void* handle, struct stCloseVideoStream_Req *req, struct stCloseVideoStream_Rsp *rsp);
JT_FRONT_API int JT_CALL_TYPE jt_open_audio_stream(void* handle, struct stOpenAudioStream_Req *req, struct stOpenAudioStream_Rsp *rsp);
JT_FRONT_API int JT_CALL_TYPE jt_close_audio_stream(void* handle, struct stCloseAudioStream_Req *req, struct stCloseAudioStream_Rsp *rsp);
JT_FRONT_API int JT_CALL_TYPE jt_open_alarm_stream(void* handle, struct stOpenAlarmStream_Req *req, struct stOpenAlarmStream_Rsp *rsp);
JT_FRONT_API int JT_CALL_TYPE jt_close_alarm_stream(void* handle, struct stCloseAlarmStream_Req *req, struct stCloseAlarmStream_Rsp *rsp);
JT_FRONT_API int JT_CALL_TYPE jt_ptz_control(void* handle, struct stPTZControl_Req *req, struct stPTZControl_Rsp *rsp);
JT_FRONT_API int JT_CALL_TYPE jt_set_system_time(void* handle, struct stSetTime_Req *req, struct stSetTime_Rsp *rsp);
JT_FRONT_API int JT_CALL_TYPE jt_start_talk(void* handle, struct stStartTalk_Req *req, struct stStartTalk_Rsp *rsp);
JT_FRONT_API int JT_CALL_TYPE jt_stop_talk(void* handle, struct stStopTalk_Req *req, struct stStopTalk_Rsp *rsp);
JT_FRONT_API int JT_CALL_TYPE jt_send_talk_data(void* handle, struct stSendTalkData_Req *req, struct stSendTalkData_Rsp *rsp);
JT_FRONT_API int JT_CALL_TYPE jt_get_video_effect(void* handle, struct stGetVideoEffect_Req *req, struct stGetVideoEffect_Rsp *rsp);
JT_FRONT_API int JT_CALL_TYPE jt_set_video_effect(void* handle, struct stSetVideoEffect_Req *req, struct stSetVideoEffect_Rsp *rsp);
JT_FRONT_API int JT_CALL_TYPE jt_get_video_record(void* handle, struct stGetVideoRecord_Req *req, struct stGetVideoRecord_Rsp *rsp);
JT_FRONT_API int JT_CALL_TYPE jt_playback_control(void* handle, struct stPlaybackCtrl_Req *req, struct stPlaybackCtrl_Rsp *rsp);
JT_FRONT_API int JT_CALL_TYPE jt_manual_record(void* handle, stManualRecord_Req *req, struct stManualRecord_Rsp *rsp);//手动录像
JT_FRONT_API int JT_CALL_TYPE jt_set_guard(void* handle, struct stSetGuard_Req *req, struct stSetGuard_Rsp *rsp);//设置布防
JT_FRONT_API int JT_CALL_TYPE jt_get_guard(void* handle, struct stGetGuard_Req *req, struct stGetGuard_Rsp *rsp);//获取布防
JT_FRONT_API int JT_CALL_TYPE jt_reboot_control(void* handle, struct stReboot_Req *req, struct stReboot_Rsp *rsp);//重启，关机
JT_FRONT_API int JT_CALL_TYPE jt_set_output(struct device *, struct stSetOutput_Req *req, struct stSetOutput_Rsp *rsp);//输出控制


#ifdef __cplusplus
}
#endif

#endif
