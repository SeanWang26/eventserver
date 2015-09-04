#pragma once

#include "jntype.h"
#include "nmctype.h"
#include "NMCDeviceType.h"

class NmcMessage
{
public:
	NmcMessage(void);
	~NmcMessage(void);

	int BuildLoginReqMsg(char *UserName, char* Password, char* Cookie, int NetworkType, string &ReqMsg);
	int ParserLoginRspMsg(const string &RspMsg, std::string& strCache, int &nRetCode, std::string& strRemark);

	int BuildLogoutReqMsg(long UserId, char* Cookie, string &ReqMsg);
	int ParserLogoutRspMsg(const string &RspMsg, std::string& strCache, int &nRetCode, std::string& strRemark);
	
	int BuildGetUserDefineDataReqMsg(int id, char* Cookie, string &ReqMsg);
	int ParserGetUserDefineDataMsg(const string &RspMsg, std::string& strCache, struct st_xy_user_data **ppuser_data, int *user_data_cnt, int &nRetCode, std::string& strRemark);
	
	int BuildSetUserDefineDataReqMsg(enum en_nmc_operator_type operator_type, struct st_xy_user_data *ppuser_data, char* Cookie, string &ReqMsg);
	int ParserSetUserDefineDataRspMsg(const string &RspMsg, std::string& strCache, enum en_nmc_operator_type &operator_type, struct st_xy_user_data *ppuser_data, int &nRetCode, std::string& strRemark);

	int BuildHeartBeatReqMsg(long UserId, char* Cookie, string &ReqMsg);
	int ParserHeartBeatRspMsg(const string &RspMsg, std::string& strCache, int &nRetCode, std::string& strRemark);
	
	//获取添加的外部nvr
	int BuildGetRemoteDeviceReqMsg(int id, char* Cookie, string &ReqMsg);
	int ParserGetRemoteDeviceRspMsg(const string &Msg, std::string& strCache, struct st_jn_device **DeviceS, int *DevCnt, int &nRetCode, std::string& strRemark);

	//添加一个远程设备
	int BuildAddRemoteDeviceReqMsg(struct st_jn_device *dev, char* Cookie, string &ReqMsg);
	int ParserAddRemoteDeviceRspMsg(const string &Msg, std::string& strCache, struct st_jn_device *dev, int &nRetCode, std::string& strRemark);

	//
	int BuildRemoveRemoteDeviceReqMsg(int DevId, char* Cookie, string &ReqMsg);
	int ParserRemoveRemoteDeviceRspMsg(const string &Msg, std::string& strCache, int &nRetCode, std::string& strRemark);

	int BuildGetMatrixReqMsg(int matrixid, char* Cookie, string &ReqMsg);
	int ParserGetMatrixRspMsg(const string &Msg, std::string& strCache, std::vector<struct stMatrixInfo> &MatrixInfoS, int &nRetCode, std::string& strRemark);
	
	int BuildSetMatrixReqMsg(enum en_nmc_operator_type operator_type, struct st_matrix_info *matrix_info, char* Cookie, string &ReqMsg);
	int ParserSetMatrixRspMsg(const string &RspMsg, std::string& strCache, enum en_nmc_operator_type *operator_type, struct st_matrix_info *matrix_info, int &nRetCode, std::string& strRemark);

	int BuildGetOutputReqMsg(int outputid, char* Cookie, string &ReqMsg);
	int ParserGetOutputRspMsg(const string &RspMsg, std::string& strCache, struct st_output_layout **ppoutput_layout, int *output_layout_cnt, int &nRetCode, std::string& strRemark);

	int BuildSetOutputReqMsg(struct st_output_layout *output_layout, char* Cookie, string &ReqMsg);
	int ParserSetOutputRspMsg(const string &RspMsg, std::string& strCache, struct st_output_layout *output_layout, int &nRetCode, std::string& strRemark);

	int BuildSetWindowSignalSourceReqMsg(int windowtype, int windowid, int sourceid, int subsourceid, char* Cookie, string &ReqMsg);
	int ParserSetWindowSignalSourceRspMsg(const string &RspMsg, std::string& strCache, int *windowtype, int *windowid, int &nRetCode, std::string& strRemark);

	int BuildClearWindowSignalSourceReqMsg(int windowtype, int outputid, int windowid, char* Cookie, string &ReqMsg);
	int ParserClearWindowSignalSourceRspMsg(const string &RspMsg, std::string& strCache, int *windowtype, int *outputid, int *windowid, int &nRetCode, std::string& strRemark);

	int BuildGetOutputMappingReqMsg(int mappingid, char* Cookie, string &ReqMsg);
	int ParserGetOutputMappingRspMsg(const string &RspMsg, std::string& strCache, struct st_output_mapping **mapping, int *mapping_cnt, int &nRetCode, std::string& strRemark);

	int BuildSetOutputMappingReqMsg(struct st_output_mapping *mapping, int mapping_cnt, char* Cookie, string &ReqMsg);
	int ParserSetOutputMappingRspMsg(const string &RspMsg, std::string& strCache, struct st_output_mapping **mapping, int *mapping_cnt, int &nRetCode, std::string& strRemark);
	
	//获取信号源图像参数信息
	int BuildGetSignalSourcePictureParaReqMsg(int sourceid, int subsourceid, char* Cookie, string &ReqMsg);
	int ParserGetSignalSourcePictureParaRspMsg(const string &RspMsg, std::string& strCache, struct st_output_mapping **mapping, int *mapping_cnt, int &nRetCode, std::string& strRemark);

	//获取多解码器输出拼接方式
	int BuildGetLargeScreenReqMsg(int ScreenId, char* Cookie, string &ReqMsg);
	int ParserGetLargeScreenRspMsg(const string &RspMsg, std::string& strCache, struct st_large_screen_info **large_screen_info, int *large_screen_info_cnt, int &nRetCode, std::string& strRemark);

	//添加多解码器输出拼接方式
	int BuildAddLargeScreenReqMsg(struct st_large_screen_info *large_screen_info, char* Cookie, string &ReqMsg);
	int ParserAddLargeScreenRspMsg(const string &RspMsg, std::string& strCache, int *ScreenId, int &nRetCode, std::string& strRemark);

	//删除多解码器输出拼接方式
	int BuildRmvLargeScreenReqMsg(int ScreenId, char* Cookie, string &ReqMsg);
	int ParserRmvLargeScreenRspMsg(const string &RspMsg, std::string& strCache, int &ScreenId, int &nRetCode, std::string& strRemark);

	//解码窗口图像调节
	int BuildSetWindowPictureParaReqMsg(int WndType, int WndId, struct st_pic_para_info *ppic_para_info, char* Cookie, string &ReqMsg);
	int ParserSetWindowPictureParaRspMsg(const string &RspMsg, std::string& strCache, int &WndType, int &WndId, int &nRetCode, std::string& strRemark);

	//解码器连接状态通知信令
	int ParserMatrixConnStatusNotifyMsg(const string &RspMsg, std::string& strCache, int &MatrixId, int &ConnStatus);

	//解码窗口画面叠加信息信令
	int BuildSetWindowOverlayInfoReqMsg(int OpType, int WndType, int WndId, int OverlayType, char* Cookie, string &ReqMsg);
	int ParserSetWindowOverlayInfoRspMsg(const string &RspMsg, std::string& strCache, int &WndType, int &WndId, int &nRetCode, std::string& strRemark);

	//解码窗口画面翻转信令
	int BuildFlipWindowReqMsg(int sourceid, int subsourceid, char* Cookie, string &ReqMsg);
	int ParserFlipWindowRspMsg(const string &RspMsg, std::string& strCache, int &WndType, int &WndId, int &nRetCode, std::string& strRemark);

	//解码窗口画面全屏显示信令
	int BuildFullscreenWindowReqMsg(int OpType, int WndType, int WndId, char* Cookie, string &ReqMsg);
	int ParserFullscreenWindowRspMsg(const string &RspMsg, std::string& strCache, int &WndType, int &WndId, int &nRetCode, std::string& strRemark);

	int BuildShowScreenNumReqMsg(int MatrixId, int Type, char* Cookie, string &ReqMsg);
	int ParserShowScreenNumRspMsg(const string &RspMsg, std::string& strCache, int &nRetCode, std::string& strRemark);

	int BuildCreateSWWindow(int width, int height, struct st_create_sw_window_info *pinfo, int info_cnt, char* Cookie, string &ReqMsg);
	int ParserCreateSWWindow(const string &RspMsg, std::string& strCache, int* SlidingWindowID, int* SWSequence, int &nRetCode, std::string& strRemark);

	int BuildDeleteSWWindow(int *psw_window_id_list, int sw_window_id_list_len, char* Cookie, string &ReqMsg);
	int ParserDeleteSWWindow(const string &RspMsg, std::string& strCache, int &nRetCode, std::string& strRemark);

	int BuildStickSWWindow(int sw_window_id, char* Cookie, string &ReqMsg);
	int ParserStickSWWindow(const string &RspMsg, std::string& strCache, int &nRetCode, std::string& strRemark);

	int BuildGetSWWindowInfo(int sw_window_id, char* Cookie, string &ReqMsg);
	int ParserGetSWWindowInfo(const string &RspMsg, std::string& strCache, struct st_sw_window_info** info, int *info_cnt, int &nRetCode, std::string& strRemark);
	
	//获取轮询方案
	int BuildGetSurveyPlanReq(int survey_id, char* cCookie, string &ReqMsg);
	int ParserGetSurveyPlanRsp(const string &RspMsg, string &strCache, struct st_xy_survey_info **ppsurvey_info, int *survey_info_cnt, int &nRetCode, std::string& strRemark);
	int BuildSetSurveyPlanReq(enum en_nmc_operator_type operator_type, struct st_xy_survey_info *psurvey_info, char* Cookie, string &ReqMsg);
	int ParserSetSurveyPlanRsp(const string &RspMsg, std::string& strCache, enum en_nmc_operator_type &operator_type, struct st_xy_survey_info *psurvey_info, int &nRetCode, std::string& strRemark);
	int BuildCtrlWindowSurveyReq(int ctrl_type, int survey_id, int wndtype, int wndid, char* Cookie, string &ReqMsg);
	int ParserCtrlWindowSurveyRsp(const string &RspMsg, std::string& strCache, int &ctrl_type, int &survey_id, int &wndtype, int &wndid, int &nRetCode, std::string& strRemark);
	
	int BuildCreateUserStreamSourceReq(char* Cookie, string &ReqMsg);
	int ParserCreateUserStreamSourceRsp(const string &RspMsg, std::string& strCache, struct PushStreamInfo &Inf, int &nRetCode, std::string& strRemark);

	int BuildSetUserStreamSourceInfoReq(char* Cookie, struct PushStreamInfo &StreamInfo, struct st_video_stream_info *info, string &ReqMsg);
	int ParserSetUserStreamSourceInfoRsp(const string &RspMsg, std::string& strCache, int &nRetCode, std::string& strRemark);

	int BuildCtrlPushStreamReq(char* Cookie, int ctrltype, string &strLocalIp, int nLocalPort, struct PushStreamInfo &Inf, string &ReqMsg);
	int ParserCtrlPushStreamRsp(const string &RspMsg, std::string& strCache, int &nRetCode, std::string& strRemark);

	int BuildClearUserStreamSourceFromWindowReq(char* Cookie, struct PushStreamInfo &Inf, string &ReqMsg);
	int ParserClearUserStreamSourceFromWindowRsp(const string &RspMsg, std::string& strCache, int &nRetCode, std::string& strRemark);
};