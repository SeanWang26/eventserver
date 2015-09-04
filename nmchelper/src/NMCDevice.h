#pragma once

#include "nmctype.h"
#include "NMCDeviceType.h"
#include "jntype.h"

class NMCDeviceImpl;

class NMCDevice
{
private:
	NMCDeviceImpl *_Impl;
public:
	NMCDevice(void);
	~NMCDevice(void);

	static NMCDevice* CreateNMC(int Type);

	int Init(nmc_status_callback status_callback, void* userdata);
	int Uninit(void* data);

	long Login(struct login_info *info);
	int  Logout();

	int GetUserDefineData(int id, struct st_xy_user_data **ppuser_data, int *user_data_cnt);
	int SetUserDefineData(enum en_nmc_operator_type operator_type, struct st_xy_user_data *ppuser_data);

	int GetLoginInfo(struct login_info *info);

	int GetSignalSource(struct st_jn_equ **equ, int *equcnt);
	int AddSignalSource(struct st_jn_equ *equ);
	int RemoveSignalSource(int equid);
	int GetRemoteDevice(int id, struct st_jn_device **dev, int *devcnt);  //得到添加的nvr信息
	int GetAddRemoteDevice(struct st_jn_device *dev);
	int GetRemoveRemoteDevice(int DevId);
	int GetRemoteSignalSource(int deviceid, struct st_jn_equ **equ, int *equcnt); //获取非本地的nvr的设备

	int GetMatrix(int matrixid, std::vector<struct stMatrixInfo> &MatrixInfoS);
	int SetMatrix(enum en_nmc_operator_type operator_type, struct st_matrix_info *matrix_info);

	int GetOutput(int outputid, struct st_output_layout **ppoutput_layout, int *output_layout_cnt);
	int SetOutput(struct st_output_layout *output_layout);

	int SetWindowSignalSource(int windowtype, int windowid, int sourceid, int subsourceid);
	int ClearWindowSignalSource(int windowtype, int outputid, int windowid);

	int GetLargeScreen(int screenid, struct st_large_screen_info **pplarge_screen_info, int *large_screen_info_cnt);
	int AddLargeScreen(struct st_large_screen_info *plarge_screen_info);
	int RmvLargeScreen(int screenid);

	int SetWindowOverlayInfo(int isshow, int wndyype, int wndid, int overlayyype);
	int FlipWindow(int wndtype, int wndid);
	int FullScreenWindow(int isfullscreen, int wndtype, int wndid);

	int ShowScreenNum(int matrixid, int Type);

	int CreateSWWindow(int width, int height, struct st_create_sw_window_info *pinfo, int info_cnt);
	int DeleteSWWindow(int *sw_window_id_list, int sw_window_id_list_len);
	int StickSWWindow(int sw_window_id);
	int GetSWWindowInfo(int sw_window_id, struct st_sw_window_info** info, int *info_cnt);

	//轮询
	int GetSurveyPlan(int survey_id, struct st_xy_survey_info **ppsurvey_info, int *survey_info_cnt);      //获取轮询方案
	int SetSurveyPlan(enum en_nmc_operator_type operator_type, struct st_xy_survey_info *ppsurvey_info);   //设置轮询方案
	int CtrlWindowSurvey(int ctrl_type, int survey_id, int wndtype, int wndid);

	long CreateUserStreamSource(void *arg);
	int DeleteUserStreamSource(long streamhandle);
	int SetUserStreamSourceInfo(long streamhandle, struct st_video_stream_info *info);
	int StartPushUserStream(long streamhandle);
	int StopPushUserStream(long streamhandle);
	int GetPushUserStreamState(long streamhandle, struct st_user_stream_state **ppuser_stream_state, int *pstatecnt);
	int SetUserStreamSourceToWindow(long streamhandle, int windowtype, int windowid);
	int ClearUserStreamSourceFromWindow(long streamhandle, int windowtype, int windowid);
	int PushUserStreamVideo(long streamhandle, struct st_video_stream_info *info, char* pstreamdata, int streamdatalen);
};

