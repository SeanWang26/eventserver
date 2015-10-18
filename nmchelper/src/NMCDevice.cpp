#include "StdAfx.h"

#include "NmcErrorCode.h"
#include "NMCDevice.h"
#include "NMCDeviceImpl.h"
#include "jtprintf.h"

NMCDevice* NMCDevice::CreateNMC(int Type)
{
	return new NMCDevice();
}


int NMCDevice::GlobalInit()
{
	return NMCDeviceImpl::GlobalInit();
}
int NMCDevice::GlobalUninit()
{
	return NMCDeviceImpl::GlobalUninit();
}
int NMCDevice::Init(nmc_status_callback status_callback, void* userdata)
{
	if(_Impl)
	{
		return _Impl->Init(status_callback, userdata);
	}

	return NMC_NOT_IMPLEMENT;	
}
int NMCDevice:: Uninit(void* data)
{
	return 0;
}
NMCDevice::NMCDevice() : _Impl(NULL)
{
	_Impl = new NMCDeviceImpl();
}
NMCDevice::~NMCDevice(void)
{
	if(_Impl)
	{
		delete _Impl;
		_Impl = NULL;
	}
}

long NMCDevice::Login(struct login_info *info)
{
	jtprintf("[NMCDevice::%s]%p\n", __FUNCTION__, _Impl);
	if(_Impl)
	{
		return _Impl->Login(info);
	}

	return NMC_NOT_IMPLEMENT;
}
int  NMCDevice::Logout()
{
	if(_Impl)
	{
		return _Impl->Logout();
	}

	return NMC_NOT_IMPLEMENT;
}

int NMCDevice::GetUserDefineData(int id, struct st_xy_user_data **ppuser_data, int *user_data_cnt)
{
	if(_Impl)
	{
		return _Impl->GetUserDefineData(id, ppuser_data, user_data_cnt);
	}

	return NMC_NOT_IMPLEMENT;
}
int NMCDevice::SetUserDefineData(enum en_nmc_operator_type operator_type, struct st_xy_user_data *ppuser_data)
{
	if(_Impl)
	{
		return _Impl->SetUserDefineData(operator_type, ppuser_data);
	}

	return NMC_NOT_IMPLEMENT;
}

int NMCDevice::GetLoginInfo(struct login_info *info)
{
	if(_Impl)
	{
		return _Impl->GetLoginInfo(info);
	}

	return NMC_NOT_IMPLEMENT;
}

int NMCDevice::GetRemoteDevice(int id, struct st_jn_device **dev, int *devcnt)
{
	if(_Impl)
	{
		return _Impl->GetRemoteDevice(id, dev, devcnt);
	}

	return NMC_NOT_IMPLEMENT;
}
int NMCDevice::GetAddRemoteDevice(struct st_jn_device *dev)
{
	if(_Impl)
	{
		return _Impl->GetAddRemoteDevice(dev);
	}

	return NMC_NOT_IMPLEMENT;
}
int NMCDevice::GetRemoveRemoteDevice(int DevId)
{
	if(_Impl)
	{
		return _Impl->GetRemoveRemoteDevice(DevId);
	}

	return NMC_NOT_IMPLEMENT;
}
int NMCDevice::GetSignalSource(struct st_jn_equ **equ, int *equcnt)
{
	if(_Impl)
	{
		return _Impl->GetSignalSource(equ, equcnt);
	}

	return NMC_NOT_IMPLEMENT;
}

int NMCDevice::AddSignalSource(struct st_jn_equ *equ)
{
	if(_Impl)
	{
		return _Impl->AddSignalSource(equ);
	}

	return NMC_NOT_IMPLEMENT;
}
int NMCDevice::RemoveSignalSource(int equid)
{
	if(_Impl)
	{
		return _Impl->RemoveSignalSource(equid);
	}

	return NMC_NOT_IMPLEMENT;
}
int NMCDevice::GetRemoteSignalSource(int deviceid, struct st_jn_equ **equ, int *equcnt)
{
	if(_Impl)
	{
		return _Impl->GetRemoteSignalSource(deviceid, equ, equcnt);
	}

	return NMC_NOT_IMPLEMENT;
}

int NMCDevice::GetMatrix(int matrixid, std::vector<struct stMatrixInfo> &MatrixInfoS)
{
	if(_Impl)
	{
		return _Impl->GetMatrix(matrixid, MatrixInfoS);
	}

	return NMC_NOT_IMPLEMENT;
}
int NMCDevice::SetMatrix(enum en_nmc_operator_type operator_type, struct st_matrix_info *matrix_info)
{
	if(_Impl)
	{
		return _Impl->SetMatrix(operator_type, matrix_info);
	}

	return NMC_NOT_IMPLEMENT;
}
int NMCDevice::GetOutput(int outputid, struct st_output_layout **ppoutput_layout, int *output_layout_cnt)
{
	if(_Impl)
	{
		return _Impl->GetOutput(outputid, ppoutput_layout, output_layout_cnt);
	}

	return NMC_NOT_IMPLEMENT;
}
int NMCDevice::SetOutput(struct st_output_layout *output_layout)
{
	if(_Impl)
	{
		return _Impl->SetOutput(output_layout);
	}

	return NMC_NOT_IMPLEMENT;
}
int NMCDevice::SetWindowSignalSource(int windowtype, int windowid, int sourceid, int subsourceid)
{
	if(_Impl)
	{
		return _Impl->SetWindowSignalSource(windowtype, windowid, sourceid, subsourceid);
	}

	return NMC_NOT_IMPLEMENT;
}
int NMCDevice::ClearWindowSignalSource(int windowtype, int outputid, int windowid)
{
	if(_Impl)
	{
		return _Impl->ClearWindowSignalSource(windowtype, outputid, windowid);
	}

	return NMC_NOT_IMPLEMENT;
}
int NMCDevice::GetLargeScreen(int screenid, struct st_large_screen_info **pplarge_screen_info, int *large_screen_info_cnt)
{
	if(_Impl)
	{
		return _Impl->GetLargeScreen(screenid, pplarge_screen_info, large_screen_info_cnt);
	}

	return NMC_NOT_IMPLEMENT;
}

int NMCDevice::AddLargeScreen(struct st_large_screen_info *plarge_screen_info)
{
	if(_Impl)
	{
		return _Impl->AddLargeScreen(plarge_screen_info);
	}

	return NMC_NOT_IMPLEMENT;
}

int NMCDevice::RmvLargeScreen(int screenid)
{
	if(_Impl)
	{
		return _Impl->RmvLargeScreen(screenid);
	}

	return NMC_NOT_IMPLEMENT;
}
int NMCDevice::SetWindowOverlayInfo(int isshow, int wndyype, int wndid, int overlayyype)
{
	if(_Impl)
	{
		return _Impl->SetWindowOverlayInfo(isshow, wndyype, wndid, overlayyype);
	}

	return NMC_NOT_IMPLEMENT;
}

int NMCDevice::FlipWindow(int wndtype, int wndid)
{
	if(_Impl)
	{
		return _Impl->FlipWindow(wndtype, wndid);
	}

	return NMC_NOT_IMPLEMENT;
}

int NMCDevice::FullScreenWindow(int isfullscreen, int wndtype, int wndid)
{
	if(_Impl)
	{
		return _Impl->FullScreenWindow(isfullscreen, wndtype, wndid);
	}

	return NMC_NOT_IMPLEMENT;
}
int NMCDevice::ShowScreenNum(int matrixid, int Type)
{
	if(_Impl)
	{
		return _Impl->ShowScreenNum(matrixid, Type);
	}

	return NMC_NOT_IMPLEMENT;
}
int NMCDevice::CreateSWWindow(int width, int height, struct st_create_sw_window_info *pinfo, int info_cnt)
{
	if(_Impl)
	{
		return _Impl->CreateSWWindow(width, height, pinfo, info_cnt);
	}

	return NMC_NOT_IMPLEMENT;
}

int NMCDevice::DeleteSWWindow(int *sw_window_id_list, int sw_window_id_list_len)
{
	if(_Impl)
	{
		return _Impl->DeleteSWWindow(sw_window_id_list, sw_window_id_list_len);
	}

	return NMC_NOT_IMPLEMENT;
}
int NMCDevice::StickSWWindow(int sw_window_id)
{
	if(_Impl)
	{
		return _Impl->StickSWWindow(sw_window_id);
	}

	return NMC_NOT_IMPLEMENT;
}
int NMCDevice::GetSWWindowInfo(int sw_window_id, struct st_sw_window_info** info, int *info_cnt)
{
	if(_Impl)
	{
		return _Impl->GetSWWindowInfo(sw_window_id, info, info_cnt);
	}

	return NMC_NOT_IMPLEMENT;
}
int NMCDevice::GetSurveyPlan(int survey_id, struct st_xy_survey_info **ppsurvey_info, int *survey_info_cnt)
{
	if(_Impl)
	{
		return _Impl->GetSurveyPlan(survey_id, ppsurvey_info, survey_info_cnt);
	}

	return NMC_NOT_IMPLEMENT;
}
int NMCDevice::SetSurveyPlan(enum en_nmc_operator_type operator_type, struct st_xy_survey_info *psurvey_info)
{
	if(_Impl)
	{
		return _Impl->SetSurveyPlan(operator_type, psurvey_info);
	}

	return NMC_NOT_IMPLEMENT;
}
int NMCDevice::CtrlWindowSurvey(int ctrl_type, int survey_id, int wndtype, int wndid)
{
	if(_Impl)
	{
		return _Impl->CtrlWindowSurvey(ctrl_type, survey_id, wndtype, wndid);
	}

	return NMC_NOT_IMPLEMENT;
}
long NMCDevice::CreateUserStreamSource(void *arg)
{
	if(_Impl)
	{
		return _Impl->CreateUserStreamSource(arg);
	}

	return 0;
}
int NMCDevice::DeleteUserStreamSource(long streamhandle)
{
	if(_Impl)
	{
		return _Impl->DeleteUserStreamSource(streamhandle);
	}

	return NMC_NOT_IMPLEMENT;
}
int NMCDevice::SetUserStreamSourceInfo(long streamhandle, struct st_video_stream_info *info)
{
	if(_Impl)
	{
		return _Impl->SetUserStreamSourceInfo(streamhandle, info);
	}

	return NMC_NOT_IMPLEMENT;
}
int NMCDevice::StartPushUserStream(long streamhandle)
{
	if(_Impl)
	{
		return _Impl->StartPushUserStream(streamhandle);
	}

	return NMC_NOT_IMPLEMENT;
}
int NMCDevice::StopPushUserStream(long streamhandle)
{
	if(_Impl)
	{
		return _Impl->StopPushUserStream(streamhandle);
	}

	return NMC_NOT_IMPLEMENT;
}
int NMCDevice::GetPushUserStreamState(long streamhandle, struct st_user_stream_state **ppuser_stream_state, int *pstatecnt)
{
	if(_Impl)
	{
		return _Impl->GetPushUserStreamState(streamhandle, ppuser_stream_state, pstatecnt);
	}

	return NMC_NOT_IMPLEMENT;
}
int NMCDevice::SetUserStreamSourceToWindow(long streamhandle, int windowtype, int windowid)
{
	if(_Impl)
	{
		return _Impl->SetUserStreamSourceToWindow(streamhandle, windowtype, windowid);
	}

	return NMC_NOT_IMPLEMENT;
}
int NMCDevice::ClearUserStreamSourceFromWindow(long streamhandle, int windowtype, int windowid)
{
	if(_Impl)
	{
		return _Impl->ClearUserStreamSourceFromWindow(streamhandle, windowtype, windowid);
	}

	return NMC_NOT_IMPLEMENT;
}
int NMCDevice::PushUserStreamVideo(long streamhandle, struct st_video_stream_info *info, char* pstreamdata, int streamdatalen)
{
	if(_Impl)
	{
		return _Impl->PushUserStreamVideo(streamhandle, info, pstreamdata, streamdatalen);
	}

	return NMC_NOT_IMPLEMENT;
}