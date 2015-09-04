#pragma once

#if (defined(WIN32) || defined(WIN64))

#else
#include <tr1/memory>
#include <semaphore.h>
#endif

#include <set>
#include <map>
#include <string>
using namespace std;

#include "jntype.h"
#include "nmctype.h"
#include "CachedAffair.h"
#include "NMCDeviceType.h"
#include "JtEventConnect.h"
#include "JtStreamSender.h"
#include "PushStreamPeer.h"

class NMCDeviceImpl : public JtConnectEventCallbackSink
{
	friend class PushStreamPeer;
	nmc_status_callback   m_pstatus_callback;
	void*				  m_puserdata;

	JtEventConnect*       m_Conn;
	JtStreamSender*       m_StreamSender;
	map<unsigned long long, tr1::shared_ptr<CCachedAffair> > m_cachedAffairMap;
	class CCachedAffairMapLock  m_lockCachedAffair;


	struct login_info               m_LoginInfo;
	int                             m_Logined;

#if (defined(WIN32) || defined(WIN64))
	HANDLE m_hHeartBeatThreadEventHandle;
	HANDLE m_hHeartBeatThreadHandle;
	static unsigned int __stdcall  Static_HeartBeatThread(void *arg);
	void HeartBeatThread();
#else
	sem_t       m_hHeartBeatThreadEventHandle;
	pthread_t	m_nHeartBeatThreadId;
	static void* Static_HeartBeatThread(void* arg);
	void* HeartBeatThread();
#endif

	void* nvrhandle;

	map<int, struct st_jn_device> m_NvrDeviceS; 

	map<long, tr1::shared_ptr<PushStreamPeer> >  m_PushStreamPeerS;

	int m_CheckFrq;
	
private:
	int HeartBeat();

	virtual int OnRecvData(void* Cookie, unsigned char* pData, int dataLen);
	virtual int OnConnected(void* Cookie);
	virtual int OnClosed(void* Cookie);
	virtual int OnReConnected(void* Cookie);

	int DoRequest(JtEventConnect* Conn, const char* pData,int dataLen, struct ST_AFFAIR_CALLBACK* pAffairCallBack, int Seq, int Cmd);

	static int static_OnGotHeartBeatData(long cookie, unsigned char* pData, int dataLen);
	int OnGotHeartBeatData(unsigned char* pData, int dataLen);

	static int static_OnGotHeartBeatOverTime(long cookie);
	int OnGotHeartBeatOverTime();

	int GetSignalSourceInner(struct login_info *loginfo, struct st_jn_equ **equ, int *equcnt);

public:
	NMCDeviceImpl(void);
	virtual ~NMCDeviceImpl(void);

	static int GlobalInit();

	int Init(nmc_status_callback status_callback, void* userdata);
	long Login(struct login_info *info);
	int Logout();

	int GetUserDefineData(int id, struct st_xy_user_data **ppuser_data, int *user_data_cnt);
	int SetUserDefineData(enum en_nmc_operator_type operator_type, struct st_xy_user_data *ppuser_data);

	int GetLoginInfo(struct login_info *info);

	int GetSignalSource(struct st_jn_equ **equ, int *equcnt);

	int AddSignalSource(struct st_jn_equ *equ);
	int RemoveSignalSource(int equid);
	int GetRemoteDevice(int id, struct st_jn_device **dev, int *devcnt);
	int GetAddRemoteDevice(struct st_jn_device *dev);
	int GetRemoveRemoteDevice(int DevId);
	int GetRemoteSignalSource(int deviceid, struct st_jn_equ **equ, int *equcnt);

	int GetMatrix(int matrixid, std::vector<struct stMatrixInfo> &MatrixInfoS);
	int SetMatrix(enum en_nmc_operator_type operator_type, struct st_matrix_info *matrix_info);

	int GetOutput(int outputid, struct st_output_layout **ppoutput_layout, int *output_layout_cnt);
	int SetOutput(struct st_output_layout *output_layout);

	int SetWindowSignalSource(int windowtype, int windowid, int sourceid, int subsourceid);
	int ClearWindowSignalSource(int windowtype, int outputid, int windowid);

	//拼接
	int GetLargeScreen(int screenid, struct st_large_screen_info **pplarge_screen_info, int *large_screen_info_cnt);
	int AddLargeScreen(struct st_large_screen_info *plarge_screen_info);
	int RmvLargeScreen(int screenid);

	//窗口操作
	int SetWindowOverlayInfo(int isshow, int wndyype, int wndid, int overlayyype);
	int FlipWindow(int wndtype, int wndid);
	int FullScreenWindow(int isfullscreen, int wndtype, int wndid);
	int ShowScreenNum(int matrixid, int Type);

	//漫游窗口
	int CreateSWWindow(int width, int height, struct st_create_sw_window_info *pinfo, int info_cnt);
	int DeleteSWWindow(int *sw_window_id_list, int sw_window_id_list_len);
	int StickSWWindow(int sw_window_id);
	int GetSWWindowInfo(int sw_window_id, struct st_sw_window_info** info, int *info_cnt);

	//获取轮询方案
	int GetSurveyPlan(int survey_id, struct st_xy_survey_info **ppsurvey_info, int *survey_info_cnt);
	int SetSurveyPlan(enum en_nmc_operator_type operator_type, struct st_xy_survey_info *psurvey_info);
	int CtrlWindowSurvey(int ctrl_type, int survey_id, int wndtype, int wndid);

	long CreateUserStreamSource(void *arg);
	int DeleteUserStreamSource(long streamhandle);
	int StartPushUserStream(long streamhandle);
	int StopPushUserStream(long streamhandle);
	int GetPushUserStreamState(long streamhandle, struct st_user_stream_state **ppuser_stream_state, int *pstatecnt);
	int SetUserStreamSourceInfo(long streamhandle, struct st_video_stream_info *info);
	int SetUserStreamSourceToWindow(long streamhandle, int windowtype, int windowid);
	int ClearUserStreamSourceFromWindow(long streamhandle, int windowtype, int windowid);
	int PushUserStreamVideo(long streamhandle, struct st_video_stream_info *info, char* pstreamdata, int streamdatalen);
};

