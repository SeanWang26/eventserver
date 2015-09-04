#include "StdAfx.h"

#if (defined(WIN32) || defined(WIN64))
#include <Windows.h>

#else
#include <dlfcn.h>
#include <pthread.h>
#include <tr1/memory>
#endif

#include <stdlib.h>
#include <string.h>
#include "NmcErrorCode.h"
#include "NMCDevice.h"
#include "NMCUser.h"
#include "jtprintf.h"
#include <string>
#include <map>
#include <set>
#include "usersdk.h"
#include "NMCDeviceType.h"
#include "JtDevcieSearch.h"

//lock to do......................
static set<NMCDevice*>* pNMCDeviceS = NULL;
static map<long, NMCDevice*>* pNMCUserStreamHandleS = NULL;

static int inited = 0;

static nmc_status_callback g_status_callback = NULL;
static void* g_userdata = NULL;

int NMC_CALL_TYPE nmc_init(nmc_status_callback status_callback, void* userdata)
{
	if(inited==0)
	{
#if (defined(WIN32) || defined(WIN64))
		WORD wVersionRequested = MAKEWORD(2, 2);
		WSADATA wsaData = {0};
		WSAStartup(wVersionRequested,&wsaData);
#endif
		inited = 1;
		pNMCDeviceS = new set<NMCDevice*>();
		pNMCUserStreamHandleS = new map<long, NMCDevice*>();

		g_status_callback = status_callback;
		g_userdata = userdata;

		return 0;
	}
	else
	{
		
	}

	return -1;
}

int NMC_CALL_TYPE nmc_uninit(void* data)
{
	g_status_callback = NULL;
	g_userdata = NULL;

	return 0;
}

static int NMC_CALL_TYPE g_nmc_status_callback(long handle, int type, void* data, int datalen, void* userdata)
{
	if(g_status_callback)
	{
		set<NMCDevice*>::iterator iter = pNMCDeviceS->find((NMCDevice*)userdata);
		if(iter!= pNMCDeviceS->end())
		{
			g_status_callback((long)userdata, type, data, datalen, g_userdata);
		}		
	}

	return 0;
}

long NMC_CALL_TYPE nmc_login(struct login_info *info)
{
	NMCDevice* Device = NMCDevice::CreateNMC(0);

	Device->Init(g_nmc_status_callback, Device);

	if(Device->Login(info)==0)
	{
		pNMCDeviceS->insert(Device);
		return (long)Device;
	}

	delete Device;

	return (long)-1;
}

int NMC_CALL_TYPE nmc_logout(long handle)
{
	NMCDevice* Device = (NMCDevice*) handle;
	set<NMCDevice*>::iterator iter = pNMCDeviceS->find(Device);
	if(iter==pNMCDeviceS->end())
	{
		return NMC_INVALIED_HANDLE;
	}
	
	return Device->Logout();
}

int NMC_CALL_TYPE nmc_get_userdefinedata(long handle, int id, struct st_xy_user_data **ppuser_data, int *user_data_cnt)
{
	NMCDevice* Device = (NMCDevice*) handle;
	set<NMCDevice*>::iterator iter = pNMCDeviceS->find(Device);
	if(iter==pNMCDeviceS->end())
	{
		return NMC_INVALIED_HANDLE;
	}

	return Device->GetUserDefineData(id, ppuser_data, user_data_cnt);
}

int NMC_CALL_TYPE nmc_set_userdefinedata(long handle, enum en_nmc_operator_type operator_type, struct st_xy_user_data *ppuser_data)
{
	NMCDevice* Device = (NMCDevice*) handle;
	set<NMCDevice*>::iterator iter = pNMCDeviceS->find(Device);
	if(iter==pNMCDeviceS->end())
	{
		return NMC_INVALIED_HANDLE;
	}
	
	return Device->SetUserDefineData(operator_type, ppuser_data);
}

int NMC_CALL_TYPE nmc_get_matrix(long handle, int matrixid, struct st_matrix_info **ppmatrix_info, int *matrix_info_cnt)
{
	NMCDevice* Device = (NMCDevice*) handle;
	set<NMCDevice*>::iterator iter = pNMCDeviceS->find(Device);
	if(iter==pNMCDeviceS->end())
	{
		return NMC_INVALIED_HANDLE;
	}

	std::vector<struct stMatrixInfo> MatrixInfoS;
	int res = Device->GetMatrix(matrixid, MatrixInfoS);
	if(res)
		return res;

	*matrix_info_cnt = MatrixInfoS.size();
	if(*matrix_info_cnt)
	{
		*ppmatrix_info = (struct st_matrix_info *)calloc(*matrix_info_cnt, sizeof(struct st_matrix_info ));
		for(int i=0 ; i<*matrix_info_cnt; ++i)
		{
			struct st_matrix_info *info = (*ppmatrix_info)+i;
			info->id = MatrixInfoS[i].Id;
			info->status = MatrixInfoS[i].Status;
			info->port = MatrixInfoS[i].Port;

			strncpy(info->name, MatrixInfoS[i].Name.c_str(), 63);
			strncpy(info->ip, MatrixInfoS[i].Ip.c_str(), 63);
			strncpy(info->password, MatrixInfoS[i].Password.c_str(), 63);
			strncpy(info->user, MatrixInfoS[i].User.c_str(), 63);

			info->output_info_cnt = MatrixInfoS[i].OutputInfoS.size();
			if(info->output_info_cnt)
			{
				info->output_info = (struct st_output_info *)calloc(info->output_info_cnt, sizeof(struct st_output_info));
				for(int j=0; j<info->output_info_cnt; ++j)
				{
					struct st_output_info *output_info = info->output_info+j;

					output_info->id = MatrixInfoS[i].OutputInfoS[j].Id;
					strncpy(output_info->resolution, MatrixInfoS[i].OutputInfoS[j].Resolution.c_str(), 254);
					output_info->index = MatrixInfoS[i].OutputInfoS[j].Index;
					output_info->hasdisplay = MatrixInfoS[i].OutputInfoS[j].HasDisplay;
				}
			}
		}
	}

	return 0;
}

int NMC_CALL_TYPE nmc_set_matrix(long handle, enum en_nmc_operator_type operator_type, struct st_matrix_info *matrix_info)
{
	NMCDevice* Device = (NMCDevice*) handle;
	set<NMCDevice*>::iterator iter = pNMCDeviceS->find(Device);
	if(iter==pNMCDeviceS->end())
	{

		return NMC_INVALIED_HANDLE;
	}

	return Device->SetMatrix(operator_type, matrix_info);
}

int NMC_CALL_TYPE nmc_get_output(long handle, int outputid, struct st_output_layout **ppoutput_layout, int *output_layout_cnt)
{
	NMCDevice* Device = (NMCDevice*) handle;
	set<NMCDevice*>::iterator iter = pNMCDeviceS->find(Device);
	if(iter==pNMCDeviceS->end())
	{
		return NMC_INVALIED_HANDLE;
	}

	return Device->GetOutput(outputid, ppoutput_layout, output_layout_cnt);
}

int NMC_CALL_TYPE nmc_set_output(long handle, struct st_output_layout *output_layout)
{
	NMCDevice* Device = (NMCDevice*) handle;
	set<NMCDevice*>::iterator iter = pNMCDeviceS->find(Device);
	if(iter==pNMCDeviceS->end())
	{
		return NMC_INVALIED_HANDLE;
	}

	return Device->SetOutput(output_layout);
}

int NMC_CALL_TYPE nmc_set_window_signal_source(long handle, int windowtype, int windowid, int sourceid, int subsourceid)
{
	NMCDevice* Device = (NMCDevice*) handle;
	set<NMCDevice*>::iterator iter = pNMCDeviceS->find(Device);
	if(iter==pNMCDeviceS->end())
	{
		return NMC_INVALIED_HANDLE;
	}

	return Device->SetWindowSignalSource(windowtype, windowid, sourceid, subsourceid);
}

int NMC_CALL_TYPE nmc_clear_window_signal_source(long handle, int windowtype, int outputid, int windowid)
{
	NMCDevice* Device = (NMCDevice*) handle;
	set<NMCDevice*>::iterator iter = pNMCDeviceS->find(Device);
	if(iter==pNMCDeviceS->end())
	{
		return NMC_INVALIED_HANDLE;
	}

	return Device->ClearWindowSignalSource(windowtype, outputid, windowid);
}



struct strtspinfo_cm
{
	string url;
	string port;
	string user;
	string password;
};



static std::map<std::string, struct strtspinfo_cm> rtspS_cm;
int JT_CALL_TYPE jt_nmc_event_stream_callback(int callback_type, void* data, int len, void** user)
{
	return 0;
}

int NMC_CALL_TYPE nmc_get_signal_source_type(long handle, char* signaltype, int *len)
{
	*len = 0;
	signaltype = NULL;
	return -1;
}

int NMC_CALL_TYPE nmc_get_signal_source(long handle, struct st_jn_equ **equ, int *equcnt)
{
	NMCDevice* Device = (NMCDevice*) handle;
	set<NMCDevice*>::iterator iter = pNMCDeviceS->find(Device);
	if(iter==pNMCDeviceS->end())
	{
		return NMC_INVALIED_HANDLE;
	}

	return Device->GetSignalSource(equ, equcnt);
}

int NMC_CALL_TYPE nmc_get_remote_device(long handle, int id, struct st_jn_device **dev, int *devcnt)
{
	NMCDevice* Device = (NMCDevice*) handle;
	set<NMCDevice*>::iterator iter = pNMCDeviceS->find(Device);
	if(iter==pNMCDeviceS->end())
	{
		return NMC_INVALIED_HANDLE;
	}

	return Device->GetRemoteDevice(id, dev, devcnt);
}

int NMC_CALL_TYPE nmc_add_remote_device(long handle, struct st_jn_device *dev)
{
	NMCDevice* Device = (NMCDevice*) handle;
	set<NMCDevice*>::iterator iter = pNMCDeviceS->find(Device);
	if(iter==pNMCDeviceS->end())
	{
		return NMC_INVALIED_HANDLE;
	}

	return Device->GetAddRemoteDevice(dev);
}
int NMC_CALL_TYPE nmc_modify_remote_device(long handle, struct st_jn_device *dev)
{


	return NMC_INVALIED_HANDLE;
}
int NMC_CALL_TYPE nmc_remove_remote_device(long handle, int devid)
{
	NMCDevice* Device = (NMCDevice*) handle;
	set<NMCDevice*>::iterator iter = pNMCDeviceS->find(Device);
	if(iter==pNMCDeviceS->end())
	{
		return NMC_INVALIED_HANDLE;
	}

	return Device->GetRemoveRemoteDevice(devid);
}


int NMC_CALL_TYPE nmc_get_remote_signal_source(long handle, int deviceid, struct st_jn_equ **equ, int *equcnt)
{
	NMCDevice* Device = (NMCDevice*) handle;
	set<NMCDevice*>::iterator iter = pNMCDeviceS->find(Device);
	if(iter==pNMCDeviceS->end())
	{
		return NMC_INVALIED_HANDLE;
	}

	return Device->GetRemoteSignalSource(deviceid, equ, equcnt);
}

int NMC_CALL_TYPE nmc_add_signal_source(long handle, struct st_jn_equ *equ)
{
	NMCDevice* Device = (NMCDevice*) handle;
	set<NMCDevice*>::iterator iter = pNMCDeviceS->find(Device);
	if(iter==pNMCDeviceS->end())
	{
		return NMC_INVALIED_HANDLE;
	}

	return Device->AddSignalSource(equ);
}
int NMC_CALL_TYPE nmc_rmv_signal_source(long handle, int sub_equ_id)
{
	NMCDevice* Device = (NMCDevice*) handle;
	set<NMCDevice*>::iterator iter = pNMCDeviceS->find(Device);
	if(iter==pNMCDeviceS->end())
	{
		return NMC_INVALIED_HANDLE;
	}

	return Device->RemoveSignalSource(sub_equ_id);
}
/*
NMC_API int NMC_CALL_TYPE nmc_get_output_mapping(long handle, int mapping, struct st_output_mapping **pplarge_screen_info, int *st_output_mapping_cnt);
NMC_API int NMC_CALL_TYPE nmc_set_output_mapping(long handle, int sub_equ_id);
int NMC_CALL_TYPE nmc_get_output_mapping(long handle, int mappingid, struct st_output_mapping **pplarge_screen_info, int *poutput_mapping_cnt)
{
	NMCDevice* Device = (NMCDevice*) handle;
	set<NMCDevice*>::iterator iter = pNMCDeviceS->find(Device);
	if(iter==pNMCDeviceS->end())
	{
		return NMC_INVALIED_HANDLE;
	}

	return NMC_NOT_IMPLEMENT;
	//return Device->GetOutputMapping(mappingid, pplarge_screen_info, poutput_mapping_cnt);
}
int NMC_CALL_TYPE nmc_set_output_mapping(long handle, int sub_equ_id)
{
	NMCDevice* Device = (NMCDevice*) handle;
	set<NMCDevice*>::iterator iter = pNMCDeviceS->find(Device);
	if(iter==pNMCDeviceS->end())
	{
		return NMC_INVALIED_HANDLE;
	}

	return NMC_NOT_IMPLEMENT;
	//return Device->SeOutputMapping(screenid, pplarge_screen_info, large_screen_info_cnt);
}
*/
int NMC_CALL_TYPE nmc_get_large_screen(long handle, int screenid, struct st_large_screen_info **pplarge_screen_info, int *large_screen_info_cnt)
{
	NMCDevice* Device = (NMCDevice*) handle;
	set<NMCDevice*>::iterator iter = pNMCDeviceS->find(Device);
	if(iter==pNMCDeviceS->end())
	{
		return NMC_INVALIED_HANDLE;
	}

	return Device->GetLargeScreen(screenid, pplarge_screen_info, large_screen_info_cnt);
}
int NMC_CALL_TYPE nmc_add_large_screen(long handle, struct st_large_screen_info *plarge_screen_info)
{
	NMCDevice* Device = (NMCDevice*) handle;
	set<NMCDevice*>::iterator iter = pNMCDeviceS->find(Device);
	if(iter==pNMCDeviceS->end())
	{
		return NMC_INVALIED_HANDLE;
	}

	return Device->AddLargeScreen(plarge_screen_info);
}

int NMC_CALL_TYPE nmc_rmv_large_screen(long handle, int screenid)
{
	NMCDevice* Device = (NMCDevice*) handle;
	set<NMCDevice*>::iterator iter = pNMCDeviceS->find(Device);
	if(iter==pNMCDeviceS->end())
	{
		return NMC_INVALIED_HANDLE;
	}

	return Device->RmvLargeScreen(screenid);
}

int NMC_CALL_TYPE nmc_set_window_overlay_info(long handle, int isshow, int wndyype, int wndid, int overlayyype)
{
	NMCDevice* Device = (NMCDevice*) handle;
	set<NMCDevice*>::iterator iter = pNMCDeviceS->find(Device);
	if(iter==pNMCDeviceS->end())
	{
		return NMC_INVALIED_HANDLE;
	}

	return Device->SetWindowOverlayInfo(isshow, wndyype, wndid, overlayyype);
}

int NMC_CALL_TYPE nmc_flip_window(long handle, int wndtype, int wndid)
{
	NMCDevice* Device = (NMCDevice*) handle;
	set<NMCDevice*>::iterator iter = pNMCDeviceS->find(Device);
	if(iter==pNMCDeviceS->end())
	{
		return NMC_INVALIED_HANDLE;
	}

	return Device->FlipWindow(wndtype, wndid);
}

int NMC_CALL_TYPE nmc_fullscreen_window(long handle, int isfullscreen, int wndtype, int wndid)
{
	NMCDevice* Device = (NMCDevice*) handle;
	set<NMCDevice*>::iterator iter = pNMCDeviceS->find(Device);
	if(iter==pNMCDeviceS->end())
	{
		return NMC_INVALIED_HANDLE;
	}

	return Device->FullScreenWindow(isfullscreen, wndtype, wndid);
}
int NMC_CALL_TYPE nmc_show_screen_num(long handle, int matrixid, int type)
{
	NMCDevice* Device = (NMCDevice*) handle;
	set<NMCDevice*>::iterator iter = pNMCDeviceS->find(Device);
	if(iter==pNMCDeviceS->end())
	{
		return NMC_INVALIED_HANDLE;
	}

	return Device->ShowScreenNum(matrixid, type);
}

int NMC_CALL_TYPE nmc_create_sw_windows(long handle, int width, int height, struct st_create_sw_window_info *pinfo, int info_cnt)
{
	NMCDevice* Device = (NMCDevice*) handle;
	set<NMCDevice*>::iterator iter = pNMCDeviceS->find(Device);
	if(iter==pNMCDeviceS->end())
	{
		return NMC_INVALIED_HANDLE;
	}
	
	return Device->CreateSWWindow(width, height, pinfo, info_cnt);
}

int NMC_CALL_TYPE nmc_delete_sw_windows(long handle, int *sw_window_id_list, int sw_window_id_list_len)
{
	NMCDevice* Device = (NMCDevice*) handle;
	set<NMCDevice*>::iterator iter = pNMCDeviceS->find(Device);
	if(iter==pNMCDeviceS->end())
	{
		return NMC_INVALIED_HANDLE;
	}
	
	return Device->DeleteSWWindow(sw_window_id_list, sw_window_id_list_len);
}

int NMC_CALL_TYPE nmc_stick_sw_windows(long handle, int sw_window_id)
{
	NMCDevice* Device = (NMCDevice*) handle;
	set<NMCDevice*>::iterator iter = pNMCDeviceS->find(Device);
	if(iter==pNMCDeviceS->end())
	{
		return NMC_INVALIED_HANDLE;
	}
	
	return Device->StickSWWindow(sw_window_id);
}

int NMC_CALL_TYPE nmc_get_sw_windows_info(long handle, int sw_window_id, struct st_sw_window_info** info, int *info_cnt)
{
	NMCDevice* Device = (NMCDevice*) handle;
	set<NMCDevice*>::iterator iter = pNMCDeviceS->find(Device);
	if(iter==pNMCDeviceS->end())
	{
		return NMC_INVALIED_HANDLE;
	}
	
	return Device->GetSWWindowInfo(sw_window_id, info, info_cnt);
}

int NMC_CALL_TYPE nmc_get_survey_plan(long handle, int survey_id, struct st_xy_survey_info **ppsurvey_info, int *survey_info_cnt)
{
	NMCDevice* Device = (NMCDevice*) handle;
	set<NMCDevice*>::iterator iter = pNMCDeviceS->find(Device);
	if(iter==pNMCDeviceS->end())
	{
		return NMC_INVALIED_HANDLE;
	}
	
	return Device->GetSurveyPlan(survey_id, ppsurvey_info, survey_info_cnt);
}

int NMC_CALL_TYPE nmc_set_survey_plan(long handle, enum en_nmc_operator_type operator_type, struct st_xy_survey_info *ppsurvey_info)
{
	NMCDevice* Device = (NMCDevice*) handle;
	set<NMCDevice*>::iterator iter = pNMCDeviceS->find(Device);
	if(iter==pNMCDeviceS->end())
	{
		return NMC_INVALIED_HANDLE;
	}
	
	return Device->SetSurveyPlan(operator_type, ppsurvey_info);
}
int NMC_CALL_TYPE nmc_ctrl_window_survey(long handle, int ctrl_type, int survey_id, int wndtype, int wndid)
{
	if(wndtype!=0)
	{
		//参数错误,目前只支持单窗口
		return NMC_ERROR_PARAMETER;
	}

	NMCDevice* Device = (NMCDevice*) handle;
	set<NMCDevice*>::iterator iter = pNMCDeviceS->find(Device);
	if(iter==pNMCDeviceS->end())
	{
		return NMC_INVALIED_HANDLE;
	}
	
	return Device->CtrlWindowSurvey(ctrl_type, survey_id, wndtype, wndid);
}
long NMC_CALL_TYPE nmc_create_user_stream_source(long handle, void* arg)
{
	NMCDevice* Device = (NMCDevice*) handle;
	set<NMCDevice*>::iterator iter = pNMCDeviceS->find(Device);
	if(iter==pNMCDeviceS->end())
	{
		return NMC_INVALIED_HANDLE;
	}
	
	long streamhandle = Device->CreateUserStreamSource(arg);
	if(streamhandle)
	{
		(*pNMCUserStreamHandleS)[streamhandle] = Device;
	}

	return streamhandle;
}

/*
函数说明:删除用户推流源
参数说明：
[in] streamhandle：由nmc_create_user_stream_source函数创建出得流对象的句柄
返回值说明：成功返回0，失败返回非0
*/
int NMC_CALL_TYPE nmc_delete_user_stream_source(long streamhandle)
{
	map<long, NMCDevice*>::iterator iter = pNMCUserStreamHandleS->find(streamhandle);
	if(iter==pNMCUserStreamHandleS->end())
	{
		return NMC_INVALIED_HANDLE;
	}

	NMCDevice* Device = (NMCDevice*) iter->second;
	set<NMCDevice*>::iterator iter2 = pNMCDeviceS->find(Device);
	if(iter2==pNMCDeviceS->end())
	{
		return NMC_INVALIED_HANDLE;
	}
	
	int res = Device->DeleteUserStreamSource(streamhandle);
	if(res)
	{
		
	}

	pNMCUserStreamHandleS->erase(streamhandle);

	return res;
}

int NMC_CALL_TYPE nmc_set_user_stream_source_info(long streamhandle, struct st_video_stream_info *info)
{
	map<long, NMCDevice*>::iterator iter = pNMCUserStreamHandleS->find(streamhandle);
	if(iter==pNMCUserStreamHandleS->end())
	{
		return NMC_INVALIED_HANDLE;
	}

	NMCDevice* Device = (NMCDevice*) iter->second;
	set<NMCDevice*>::iterator iter2 = pNMCDeviceS->find(Device);
	if(iter2==pNMCDeviceS->end())
	{
		return NMC_INVALIED_HANDLE;
	}
	
	return Device->SetUserStreamSourceInfo(streamhandle, info);
}
int NMC_CALL_TYPE nmc_start_push_user_stream(long streamhandle)
{
	map<long, NMCDevice*>::iterator iter = pNMCUserStreamHandleS->find(streamhandle);
	if(iter==pNMCUserStreamHandleS->end())
	{
		return NMC_INVALIED_HANDLE;
	}

	NMCDevice* Device = (NMCDevice*) iter->second;
	set<NMCDevice*>::iterator iter2 = pNMCDeviceS->find(Device);
	if(iter2==pNMCDeviceS->end())
	{
		return NMC_INVALIED_HANDLE;
	}

	return Device->StartPushUserStream(streamhandle);
}
int NMC_CALL_TYPE nmc_stop_push_user_stream(long streamhandle)
{
	map<long, NMCDevice*>::iterator iter = pNMCUserStreamHandleS->find(streamhandle);
	if(iter==pNMCUserStreamHandleS->end())
	{
		return NMC_INVALIED_HANDLE;
	}

	NMCDevice* Device = (NMCDevice*) iter->second;
	set<NMCDevice*>::iterator iter2 = pNMCDeviceS->find(Device);
	if(iter2==pNMCDeviceS->end())
	{
		return NMC_INVALIED_HANDLE;
	}

	return Device->StopPushUserStream(streamhandle);
}
int NMC_CALL_TYPE nmc_get_push_user_stream_state(long handle, long streamhandle, struct st_user_stream_state **ppuser_stream_state, int *pstatecnt)
{
	NMCDevice* Device = (NMCDevice*) handle;
	set<NMCDevice*>::iterator iter = pNMCDeviceS->find(Device);
	if(iter==pNMCDeviceS->end())
	{
		return NMC_INVALIED_HANDLE;
	}

	return Device->GetPushUserStreamState(streamhandle, ppuser_stream_state, pstatecnt);
}
/*
函数说明:设置用户推流源到窗口
参数说明：
[in] streamhandle：由nmc_create_user_stream_source函数创建出得流对象的句柄
[in] windowtype 窗口类型
[in] windowid 窗口id
返回值说明：成功返回0，失败返回非0
*/
int NMC_CALL_TYPE nmc_set_user_stream_source_to_window(long streamhandle, int windowtype, int windowid)
{
	map<long, NMCDevice*>::iterator iter = pNMCUserStreamHandleS->find(streamhandle);
	if(iter==pNMCUserStreamHandleS->end())
	{
		return NMC_INVALIED_HANDLE;
	}

	NMCDevice* Device = (NMCDevice*) iter->second;
	set<NMCDevice*>::iterator iter2 = pNMCDeviceS->find(Device);
	if(iter2==pNMCDeviceS->end())
	{
		return NMC_INVALIED_HANDLE;
	}
	
	return Device->SetUserStreamSourceToWindow(streamhandle, windowtype, windowid);
}

/*
函数说明:从窗口清除用户推流源
参数说明：
[in] streamhandle：由nmc_create_user_stream_source函数创建出得流对象的句柄
[in] windowtype 窗口类型
[in] windowid 窗口id, id为0，清除该流之前设置的所有窗口
返回值说明：成功返回0，失败返回非0
*/
int NMC_CALL_TYPE nmc_clear_user_stream_source_from_window(long streamhandle, int windowtype, int windowid)
{
	map<long, NMCDevice*>::iterator iter = pNMCUserStreamHandleS->find(streamhandle);
	if(iter==pNMCUserStreamHandleS->end())
	{
		return NMC_INVALIED_HANDLE;
	}

	NMCDevice* Device = (NMCDevice*) iter->second;
	set<NMCDevice*>::iterator iter2 = pNMCDeviceS->find(Device);
	if(iter2==pNMCDeviceS->end())
	{
		return NMC_INVALIED_HANDLE;
	}
	
	return Device->ClearUserStreamSourceFromWindow(streamhandle, windowtype, windowid);
}
/*
函数说明:推送一帧视频流数据,阻塞发送
参数说明：
[in] streamhandle：由nmc_create_user_stream_source函数创建出得流对象的句柄
[in] info          流流源信息
[in] pstreamdata   流数据指针
[in] streamdatalen 流数据长度，注意:流数据必须是完整的一帧数据
返回值说明：成功返回0，失败返回非0
*/
int NMC_CALL_TYPE nmc_push_user_stream_video_data(long streamhandle, struct st_video_stream_info *info, char* pstreamdata, int streamdatalen)
{
	map<long, NMCDevice*>::iterator iter = pNMCUserStreamHandleS->find(streamhandle);
	if(iter==pNMCUserStreamHandleS->end())
	{
		return NMC_INVALIED_HANDLE;
	}

	NMCDevice* Device = (NMCDevice*) iter->second;
	set<NMCDevice*>::iterator iter2 = pNMCDeviceS->find(Device);
	if(iter2==pNMCDeviceS->end())
	{
		return NMC_INVALIED_HANDLE;
	}
	
	return Device->PushUserStreamVideo(streamhandle, info, pstreamdata, streamdatalen);
}

/*int NMC_CALL_TYPE nmc_set_window_rtsp_signal_source(long handle, int windowtype, int outputid, int windowid, char* url, int port, char *user, char* password, int *sourceid)
{
	NMCDevice* Device = (NMCDevice*) handle;
	set<NMCDevice*>::iterator iter = pNMCDeviceS->find(Device);
	if(iter==pNMCDeviceS->end())
	{
		return NMC_INVALIED_HANDLE;
	}

	struct login_info info;
	Device->GetLoginInfo(&info);

	struct stLogin_Req req;

	//或者读取配置文件？？？？？？？？？？？？？？？
	strcpy(req.Ip, info.ip);
	strcpy(req.User, "admin");
	strcpy(req.Password, "admin");
	req.Port = 20000;
	req.EventCallback = NULL;
	req.EventCallbackUserData = NULL;

	//登陆
	if(_jt_login_dl==NULL || _jt_set_config_dl==NULL || _jt_logout_dl==NULL)
		return NMC_INVALIED_HANDLE;

	//建立一个长的连接还是用一次，登一次？？？？？？？？？？？？？？？？

	struct stLogin_Rsp rsp;
	int ret = _jt_login_dl(nvrhandle, &req, &rsp);
	if(ret)
	{
		return NMC_INVALIED_HANDLE;
	}

	struct stSetConfig_Req creq;
	struct stSetConfig_Rsp crsp;
	creq. Channel = 0;
	creq.Codec = 0;
	creq.Type = SET_CONFIG_ADD_EQU;

	struct st_jn_equ jn_equ;

	//添加一个rtsp设备
	jn_equ.EquId = -1;
	strcpy(jn_equ.FactoryName, "RTSP");

	static int nOpSeq = 0;
	char cEquName[64] = "";
	sprintf(cEquName, "RTSP-%d", ++nOpSeq);
	jn_equ.nOpSeq = nOpSeq;
	strcpy(jn_equ.EquName, cEquName);
	strcpy(jn_equ.IP, url);
	jn_equ.Port = port;
	strcpy(jn_equ.UserName, user);
	strcpy(jn_equ.Password, password);
	jn_equ.ChannelNum = 1;
	jn_equ.InputNum = 0;
	jn_equ.OutputNum = 0;
	jn_equ.nOpType = 0;//OP_TYPE_ADD
	jn_equ.nCtlgId = 1;//
	
	creq.Config = (char*)&jn_equ;
	creq.Size = 1;
	int res = _jt_set_config_dl(nvrhandle, &creq, &crsp);
	if(res==0)
	{

	}

	*sourceid = 0;
	struct st_jn_sub_equ * jn_sub_equ= jn_equ.stSubEqu;
	for(int j=0; j<jn_equ.stSubEquCnt; ++j)
	{
		struct st_jn_sub_equ *sub_equ = &jn_sub_equ[j];
		*sourceid = sub_equ->SubEquId;
		free(jn_sub_equ);
		break;
		printf("%d", sub_equ->SubEquId);
	}

	struct stLogout_Req oreq;
	struct stLogout_Rsp orsp;

	if(0==_jt_logout_dl(nvrhandle, &oreq, &orsp))
	{
		//ok
	}
	
	if(*sourceid==0)
		return NMC_INVALIED_HANDLE;

	//*sourceid = 1;
	//int subsourceid = 1;
	//1为默认本机的nvr
	return Device->SetWindowSignalSource(windowtype, windowid, 1, *sourceid);
}

int NMC_CALL_TYPE nmc_clear_window_rtsp_signal_source(long handle, int windowtype, int outputid, int windowid, int sourceid)
{






	//添加一个rtsp到nvr得到sourceid  subsourceid
	sourceid = 1;
	int subsourceid = 1;




	//根据该rtsp
	NMCDevice* Device = (NMCDevice*) handle;
	set<NMCDevice*>::iterator iter = pNMCDeviceS->find(Device);
	if(iter==pNMCDeviceS->end())
	{
		return NMC_INVALIED_HANDLE;
	}
	
	return Device->ClearWindowSignalSource(windowtype, outputid, windowid);
}
*/


int NMC_CALL_TYPE nmc_search_device(struct st_xy_device_info **device_info, int *len)
{
	tr1::shared_ptr<CJtDevcieSearch> Search(new CJtDevcieSearch());
	Search->Init(Search, g_status_callback, g_userdata);
	Search->Search();

	return 0;
}

int NMC_CALL_TYPE nmc_free_matrix_info(struct st_matrix_info *pmatrix_info, int matrix_info_cnt)
{
	for(int i=0; i<matrix_info_cnt; ++i)
	{
		struct st_matrix_info *p = pmatrix_info+i;
		if(p->output_info)
		{
			free(p->output_info);
			p->output_info = NULL;
		}
	}

	free(pmatrix_info);

	return 0;
}

int NMC_CALL_TYPE nmc_free_userdefinedata(struct st_xy_user_data *puser_data, int user_data_cnt)
{
	if(puser_data==NULL)
		return 0;

	for(int i=0; i<user_data_cnt; ++i)
	{
		struct st_xy_user_data *p = puser_data+i;
		if(p->data)
		{
			free(p->data);
			p->data = NULL;
		}
	}

	free(puser_data);

	return 0;
}
int NMC_CALL_TYPE nmc_free_output_info(struct st_output_layout *poutput_layout, int output_layout_cnt)
{
	for(int i=0; i<output_layout_cnt; ++i)
	{
		struct st_output_layout *p = poutput_layout+i;
		if(p->window_info)
		{
			free(p->window_info);
			p->window_info = NULL;
		}
	}

	free(poutput_layout);

	return 0;
}
int NMC_CALL_TYPE nmc_free_equ_info(struct st_jn_equ *equ, int equcnt)
{
	//equ的指针是在jn模块里返回的，按理在windows下应在jn模块里释放!!!!! 以后再改 to do.........
	for(int i=0; i<equcnt; ++i)
	{
		struct st_jn_equ *p = equ+i;
		if(p->stSubEqu)
		{
			///free(p->stSubEqu); //以后再改
			p->stSubEqu = NULL;
		}
	}

	//free(equ); //以后再改

	return 0;
}

int NMC_CALL_TYPE nmc_free_large_screen_info(struct st_large_screen_info *plarge_screen_info, int large_screen_info_cnt)
{
	if(plarge_screen_info==NULL) return 0;

	for(int i=0; i<large_screen_info_cnt; ++i)
	{
		struct st_large_screen_info *p = plarge_screen_info+i;
		if(p->psubscreen_info)
		{
			free(p->psubscreen_info);
			p->psubscreen_info = NULL;
		}
	}

	free(plarge_screen_info);

	return 0;
}

int NMC_CALL_TYPE nmc_free_sw_windows_info(struct st_sw_window_info* info, int info_cnt)
{
	if(info)
		free(info);

	return 0;
}
int NMC_CALL_TYPE nmc_free_survey_plan_info(struct st_xy_survey_info *psurvey_info, int survey_info_cnt)
{
	if(psurvey_info==NULL) return 0;

	for(int i=0; i<survey_info_cnt; ++i)
	{
		struct st_xy_survey_info *p = psurvey_info+i;
		if(p->pst_xy_sourceinfo)
		{
			free(p->pst_xy_sourceinfo);
			p->pst_xy_sourceinfo = NULL;
		}
	}

	free(psurvey_info);

	return 0;
}
