#ifndef _NMC_USER_H
#define _NMC_USER_H

#if (defined(WIN32) || defined(WIN64))

#ifndef NMC_USER_MODULE_EXPORTS
#pragma comment(lib,"nmchelper.lib") 
#endif

#else

#endif

#include "jntype.h"
#include "nmctype.h"

#ifdef __cplusplus
extern "C"
{
#endif

/************************************************
函数说明：sdk的全局初始化函数
参数说明：
[in] status_callback：全局的事件回调
[in] userdata：       用户数据
************************************************/
NMC_API int NMC_CALL_TYPE nmc_init(nmc_status_callback status_callback, void* userdata);

/************************************************
************************************************/
NMC_API int NMC_CALL_TYPE nmc_uninit(void* data);

/************************************************
函数说明：登陆到nmc
参数说明：
[in] info：建立连接需要的信息，具体见struct login_info定义说明
返回值说明：成功返回登陆句柄，失败返回-1值
************************************************/
NMC_API long NMC_CALL_TYPE nmc_login(struct login_info *info);

/************************************************
函数说明：登陆到nmc
参数说明：
[in] handle：由nmc_login函数创建出得设备对象的句柄
返回值说明：成功返回0，失败返回非0值
************************************************/
NMC_API int NMC_CALL_TYPE nmc_logout(long handle);

/************************************************
函数说明：获取用户透明数据
参数说明：
[in] handle：由nmc_login函数创建出得设备对象的句柄
[in] id :数据id号，-1获取所有
[in] ppuser_data：struct st_xy_user_data *指针，返回用户数据信息
[in] user_data_cnt：返回用户数据数量
返回值说明：成功返回0，失败返回非0值
************************************************/
NMC_API int NMC_CALL_TYPE nmc_get_userdefinedata(long handle, int id, struct st_xy_user_data **ppuser_data, int *user_data_cnt);

/************************************************
函数说明：设置用户透明数据
参数说明：
[in] handle：由nmc_login函数创建出得设备对象的句柄
[in] operator_type :对用户数据的操作类型
[in] ppuser_data :  要操作的用户数据
返回值说明：成功返回0，失败返回非0值
************************************************/
NMC_API int NMC_CALL_TYPE nmc_set_userdefinedata(long handle, enum en_nmc_operator_type operator_type, struct st_xy_user_data *ppuser_data);

/************************************************
函数说明：获取解码器信息
参数说明：
[in] handle：由nmc_login函数创建出得设备对象的句柄
[in] matrixid :解码器id，-1获取所有
[in] matrix_info：struct st_matrix_info *指针，返回解码器信息
[in] matrix_info_cnt：struct st_matrix_info *指针，返回的解码器数量
返回值说明：成功返回0，失败返回非0值
************************************************/
NMC_API int NMC_CALL_TYPE nmc_get_matrix(long handle, int matrixid, struct st_matrix_info **ppmatrix_info, int *matrix_info_cnt);


/************************************************
函数说明：设置解码器信息
参数说明：
[in] handle：由nmc_login函数创建出得设备对象的句柄
[in] operator_type : 操作类型，0增加， 1修改， 2删除
[in] matrix_info：struct st_matrix_info *指针，传入要操作的解码器信息
返回值说明：成功返回0，失败返回非0值
************************************************/
NMC_API int NMC_CALL_TYPE nmc_set_matrix(long handle, enum en_nmc_operator_type operator_type, struct st_matrix_info *matrix_info);

/************************************************
函数说明：获取解码器输出的分屏方式
参数说明：
[in] handle：由nmc_login函数创建出得设备对象的句柄
[in] outputid :输出id，-1获取所有
[in] output_layout：struct st_output_layout *指针，返回输出分屏信息
[in] output_layout_cnt：struct st_output_layout *指针，返回的分屏信息数量
返回值说明：成功返回0，失败返回非0值
************************************************/
NMC_API int NMC_CALL_TYPE nmc_get_output(long handle, int outputid, struct st_output_layout **ppoutput_layout, int *output_layout_cnt);


/************************************************
函数说明：设置解码器输出的分屏方式
参数说明：
[in] handle：由nmc_login函数创建出得设备对象的句柄
[in] output_layout :要设置的输出
返回值说明：成功返回0，失败返回非0值
************************************************/
NMC_API int NMC_CALL_TYPE nmc_set_output(long handle, struct st_output_layout *output_layout);

/************************************************
函数说明：设置解码窗口信号
参数说明：
[in] handle：由nmc_login函数创建出得设备对象的句柄
[in] windowtype :窗口类型，0非拼接，1拼接
[in] windowid :要设置的输出窗口
[in] sourceid :要设置的输出信号源id
[in] subsourceid :当sourceid（如NVR）包含多个子设备时候，subsourceid表示子信号源Id
返回值说明：成功返回0，失败返回非0值
************************************************/
NMC_API int NMC_CALL_TYPE nmc_set_window_signal_source(long handle, int windowtype, int windowid, int sourceid, int subsourceid);

/************************************************
函数说明：清除窗口信号
参数说明：
[in] handle：由nmc_login函数创建出得设备对象的句柄
[in] windowtype :窗口类型，0非拼接，1拼接
[in] windowid :要设置的输出窗口id, 当为-1时，清除outputid上所有的信号
[in] outputid :要设置的输出id
[in] subsourceid :当sourceid（如NVR）包含多个子设备时候，subsourceid表示子信号源Id
返回值说明：成功返回0，失败返回非0值
************************************************/
NMC_API int NMC_CALL_TYPE nmc_clear_window_signal_source(long handle, int windowtype, int outputid, int windowid);


/************************************************
函数说明：获取本地nvr上的可以添加的信号源类别
参数说明：
[in] handle：由nmc_login函数创建出得设备对象的句柄
[out] equ :传入指针，返回一个目前已有的信号源
[out] equcnt :返回一个目前已有的信号源的数量
返回值说明：成功返回0，失败返回非0值
************************************************/
NMC_API int NMC_CALL_TYPE nmc_get_signal_source_type(long handle, char* signaltype, int *len);




/************************************************
函数说明：获取本地nvr上的已有信号源
参数说明：
[in] handle：由nmc_login函数创建出得设备对象的句柄
[out] equ :传入指针，返回一个目前已有的信号源
[out] equcnt :返回一个目前已有的信号源的数量
返回值说明：成功返回0，失败返回非0值
************************************************/
NMC_API int NMC_CALL_TYPE nmc_get_signal_source(long handle, struct st_jn_equ **equ, int *equcnt);

/************************************************
函数说明：获取远程设备
参数说明：
[in] handle：由nmc_login函数创建出得设备对象的句柄
[in] id：device的id，-1表示获取所有
[out] dev : 传入指针，返回设备信息
[out] devcnt : 返回设备的数量
返回值说明：成功返回0，失败返回非0值
************************************************/
NMC_API int NMC_CALL_TYPE nmc_get_remote_device(long handle, int devid, struct st_jn_device **dev, int *devcnt);

/************************************************
函数说明：获取远程设备
参数说明：
[in] handle：由nmc_login函数创建出得设备对象的句柄
[in] id：device的id，-1表示获取所有
[out] dev : 传入指针，返回设备信息
[out] devcnt : 返回设备的数量
返回值说明：成功返回0，失败返回非0值
************************************************/
NMC_API int NMC_CALL_TYPE nmc_add_remote_device(long handle, struct st_jn_device *dev);

/************************************************
函数说明：修改远程设备
参数说明：
[in] handle：由nmc_login函数创建出得设备对象的句柄
[in] id：device的id，-1表示获取所有
[out] dev : 传入指针，返回设备信息
[out] devcnt : 返回设备的数量
返回值说明：成功返回0，失败返回非0值
************************************************/
NMC_API int NMC_CALL_TYPE nmc_modify_remote_device(long handle, struct st_jn_device *dev);

/************************************************
函数说明：获取远程设备
参数说明：
[in] handle：由nmc_login函数创建出得设备对象的句柄
[in] id：device的id，-1表示获取所有
[out] dev : 传入指针，返回设备信息
[out] devcnt : 返回设备的数量
返回值说明：成功返回0，失败返回非0值
************************************************/
NMC_API int NMC_CALL_TYPE nmc_remove_remote_device(long handle, int devid);

/************************************************
函数说明：获取远程设备
参数说明：
[in] handle：由nmc_login函数创建出得设备对象的句柄
[in] id：device的id，-1表示获取所有
[out] dev : 传入指针，返回设备信息
[out] devcnt : 返回设备的数量
返回值说明：成功返回0，失败返回非0值
************************************************/
NMC_API int NMC_CALL_TYPE nmc_get_remote_signal_source(long handle, int deviceid, struct st_jn_equ **equ, int *equcnt);

/*
函数说明：添加一个信号源到nmc上
参数说明：
[in] handle：由nmc_login函数创建出得设备对象的句柄
[in out] equ :传入想要添加的struct st_jn_equ数据结构的指针，参见struct st_jn_equ定义
返回值说明：成功返回0，失败返回非0值
*/
NMC_API int NMC_CALL_TYPE nmc_add_signal_source(long handle, struct st_jn_equ *equ);

/*
函数说明：将一个信号源从nmc上删除
参数说明：
[in] handle：由nmc_login函数创建出得设备对象的句柄
[in] sub_equ_id:想要删除的一个子设备的id号，对应struct st_jn_sub_equ结构中的SubEquId
，参见struct st_jn_sub_equ定义
返回值说明：成功返回0，失败返回非0值
*/
NMC_API int NMC_CALL_TYPE nmc_rmv_signal_source(long handle, int sub_equ_id);

/*
函数说明：获取当前已有的拼接屏的信息
参数说明：
[in] handle：由nmc_login函数创建出得设备对象的句柄
[in] screenid:想要获取的大屏的id号，-1表示当前nmc上所有大屏
[in] pplarge_screen_info：struct st_large_screen_info *8指针，返回拼接屏信息
[in] large_screen_info_cnt：返回拼接屏信息数量
返回值说明：成功返回0，失败返回非0值
*/
NMC_API int NMC_CALL_TYPE nmc_get_large_screen(long handle, int screenid, struct st_large_screen_info **pplarge_screen_info, int *large_screen_info_cnt);

/*
函数说明：添加多解码器输出拼接,如果该屏上由漫游窗口，设置无效
参数说明：
[in] handle：由nmc_login函数创建出得设备对象的句柄
[in] plarge_screen_info：struct st_large_screen_info *, 想要添加的拼接的信息，由用户填写
返回值说明：成功返回0，失败返回非0值
*/
NMC_API int NMC_CALL_TYPE nmc_add_large_screen(long handle, struct st_large_screen_info *plarge_screen_info);

/*
函数说明：删除多解码器输出拼接
参数说明：
[in] handle：由nmc_login函数创建出得设备对象的句柄
[in] screenid：想要删除的的拼接屏的id号
返回值说明：成功返回0，失败返回非0值
*/
NMC_API int NMC_CALL_TYPE nmc_rmv_large_screen(long handle, int screenid);

/*
函数说明：解码窗口画面叠加信息，只支持单窗口
参数说明：
[in] handle：由nmc_login函数创建出得设备对象的句柄
[in] isshow：是否显示，0:不显示，1:显示
[in] wndyype：窗口类型，单窗口恒为0
[in] wndid：窗口id
[in] overlayyype：想要显示的信息类型enum en_nmc_show_Type
返回值说明：成功返回0，失败返回非0值
*/
NMC_API int NMC_CALL_TYPE nmc_set_window_overlay_info(long handle, int isshow, int wndyype, int wndid, int overlayyype);

/*
函数说明：解码窗口画面翻转,只对单窗口有效
参数说明：
[in] handle：由nmc_login函数创建出得设备对象的句柄
[in] wndtype：窗口类型，单窗口恒为0
[in] wndid：窗口id
返回值说明：成功返回0，失败返回非0值
*/
NMC_API int NMC_CALL_TYPE nmc_flip_window(long handle, int wndtype, int wndid);

/*
函数说明：解码窗口画面全屏显示
参数说明：
[in] handle：由nmc_login函数创建出得设备对象的句柄
[in] isfullscreen：是否全屏
[in] wndtype：窗口类型，单窗口恒为0
[in] wndid：窗口id
返回值说明：成功返回0，失败返回非0值
*/
NMC_API int NMC_CALL_TYPE nmc_fullscreen_window(long handle, int isfullscreen, int wndtype, int wndid);

/*
函数说明：解码窗口对应编号显示
参数说明：
[in] handle：由nmc_login函数创建出得设备对象的句柄
[in] matrixid：解码器id
[in] type：0显示编号，1显示背景图
返回值说明：成功返回0，失败返回非0值
*/
NMC_API int NMC_CALL_TYPE nmc_show_screen_num(long handle, int matrixid, int type);

/*
函数说明：创建漫游窗口
参数说明：
[in] handle：由nmc_login函数创建出得设备对象的句柄
[in] matrixid：解码器id
[in] type：0显示编号，1显示背景图
返回值说明：成功返回0，失败返回非0值
*/
NMC_API int NMC_CALL_TYPE nmc_create_sw_windows(long handle, int width, int height, struct st_create_sw_window_info *pinfo, int info_cnt);

/*
函数说明：创建漫游窗口
参数说明：
[in] handle：由nmc_login函数创建出得设备对象的句柄
[in] sw_window_id_list：要删除的漫游窗口id列表
[in] sw_window_id_list_len：要删除的漫游窗口id个数
返回值说明：成功返回0，失败返回非0值
*/
NMC_API int NMC_CALL_TYPE nmc_delete_sw_windows(long handle, int *sw_window_id_list, int sw_window_id_list_len);

/*
函数说明：置顶漫游窗口
参数说明：
[in] handle：由nmc_login函数创建出得设备对象的句柄
[in] sw_window_id_list：要置顶的漫游窗口id
返回值说明：成功返回0，失败返回非0值
*/
NMC_API int NMC_CALL_TYPE nmc_stick_sw_windows(long handle, int sw_window_id);

/*
函数说明：获取漫游窗口信息
参数说明：
[in] handle：由nmc_login函数创建出得设备对象的句柄
[in] sw_window_id：要获取的漫游窗口id
[in] sw_window_info：要获取的漫游窗口信息
返回值说明：成功返回0，失败返回非0值
*/

NMC_API int NMC_CALL_TYPE nmc_get_sw_windows_info(long handle, int sw_window_id, struct st_sw_window_info** info, int *info_cnt);


/*
函数说明:获取轮询方案信息
参数说明：
[in] survey_id    轮询id，-1，表示获取所有
[in] ppsurvey_info          设备信息
[in] len                  设备信息长度
返回值说明：成功返回0，失败返回非0
*/
NMC_API int NMC_CALL_TYPE nmc_get_survey_plan(long handle, int survey_id, struct st_xy_survey_info **ppsurvey_info, int *survey_info_cnt);

/*
函数说明:设置轮询方案
参数说明：
[in] survey_id    轮询id，-1，表示获取所有
[in] ppsurvey_info          设备信息
[in] len                  设备信息长度
返回值说明：成功返回0，失败返回非0
*/
NMC_API int NMC_CALL_TYPE nmc_set_survey_plan(long handle, enum en_nmc_operator_type operator_type, struct st_xy_survey_info *ppsurvey_info);
/*
函数说明:控制单窗口轮巡
参数说明：
[in] ctrl_type    0 启动单窗口轮巡， 1 停止单窗口轮巡
[in] survey_id    轮询id
[in] wndtype      窗口类型,目前只支持单窗口,填为0
[in] wndid        窗口id
返回值说明：成功返回0，失败返回非0
*/
NMC_API int NMC_CALL_TYPE nmc_ctrl_window_survey(long handle, int ctrl_type, int survey_id, int wndtype, int wndid);




/*
函数说明：创建一个用户推流源
参数说明：
[in] handle：由nmc_login函数创建出得设备对象的句柄
[in] arg：保留参数, 设置为NULL
返回值说明：成功返回非0标示流源句柄，失败返回0
*/

NMC_API long NMC_CALL_TYPE nmc_create_user_stream_source(long handle, void* arg);

/*
函数说明:删除用户推流源
参数说明：
[in] streamhandle：由nmc_create_user_stream_source函数创建出得流对象的句柄
返回值说明：成功返回0，失败返回非0
*/
NMC_API int NMC_CALL_TYPE nmc_delete_user_stream_source(long streamhandle);

NMC_API int NMC_CALL_TYPE nmc_start_push_user_stream(long streamhandle);

NMC_API int NMC_CALL_TYPE nmc_stop_push_user_stream(long streamhandle);

/*
函数说明：获取当前已有的用户推流源的状态
参数说明：
[in] handle ：由nmc_create_user_stream_source函数创建出得流对象的句柄，如果传入0，返回所有
[in] streamhandle ：由nmc_create_user_stream_source函数创建出得流对象的句柄，如果传入0，返回所有
[out] ppuser_stream_state : 传入指针，返回设备信息
[out] pstatecnt : 返回设备的数量
返回值说明：成功返回0，失败返回非0
*/
NMC_API int NMC_CALL_TYPE nmc_get_push_user_stream_state(long handle, long streamhandle, struct st_user_stream_state **ppuser_stream_state, int *pstatecnt);



/*
函数说明:设置用户推流源信息
参数说明：
[in] streamhandle：由nmc_create_user_stream_source函数创建出得流对象的句柄
[in] struct st_video_stream_info *info, 流源信息
[in] len, struct st_video_stream_info结构体长度
返回值说明：成功返回0，失败返回非0
*/
NMC_API int NMC_CALL_TYPE nmc_set_user_stream_source_info(long streamhandle, struct st_video_stream_info *info);

/*
函数说明:设置用户推流源到窗口
参数说明：
[in] streamhandle：由nmc_create_user_stream_source函数创建出得流对象的句柄
[in] windowtype 窗口类型
[in] windowid 窗口id
返回值说明：成功返回0，失败返回非0
*/
NMC_API int NMC_CALL_TYPE nmc_set_user_stream_source_to_window(long streamhandle, int windowtype, int windowid);

/*
函数说明:从窗口清除用户推流源
参数说明：
[in] streamhandle：由nmc_create_user_stream_source函数创建出得流对象的句柄
[in] windowtype 窗口类型
[in] windowid 窗口id, id为0，清除该流之前设置的所有窗口
返回值说明：成功返回0，失败返回非0
*/
NMC_API int NMC_CALL_TYPE nmc_clear_user_stream_source_from_window(long streamhandle, int windowtype, int windowid);
/*
函数说明:推送一帧视频流数据,阻塞发送
参数说明：
[in] streamhandle：由nmc_create_user_stream_source函数创建出得流对象的句柄
[in] info          流流源信息
[in] pstreamdata   流数据指针
[in] streamdatalen 流数据长度，注意:流数据必须是完整的一帧数据
返回值说明：成功返回0，失败返回非0
*/
NMC_API int NMC_CALL_TYPE nmc_push_user_stream_video_data(long streamhandle, struct st_video_stream_info *info, char* pstreamdata, int streamdatalen);



/*
函数说明:在网络里搜索设备
参数说明：
[in] device_info          设备信息
[in] len                  设备信息长度
返回值说明：成功返回0，失败返回非0
*/
NMC_API int NMC_CALL_TYPE nmc_search_device(struct st_xy_device_info **device_info, int *len);

/*
函数说明:修改设备网络参数
参数说明：
[in] device_info          设备信息
[in] len                  设备信息长度
返回值说明：成功返回0，失败返回非0
*/
NMC_API int NMC_CALL_TYPE nmc_change_device_net(struct st_xy_device_info *old_device_info, struct st_xy_device_info *new_device_info);



NMC_API int NMC_CALL_TYPE nmc_free_userdefinedata(struct st_xy_user_data *ppuser_data, int user_data_cnt);
NMC_API int NMC_CALL_TYPE nmc_free_matrix_info(struct st_matrix_info *pmatrix_info, int matrix_info_cnt);
NMC_API int NMC_CALL_TYPE nmc_free_output_info(struct st_output_layout *poutput_layout, int output_layout_cnt);
NMC_API int NMC_CALL_TYPE nmc_free_equ_info(struct st_jn_equ *equ, int equcnt);
NMC_API int NMC_CALL_TYPE nmc_free_large_screen_info(struct st_large_screen_info *plarge_screen_info, int large_screen_info_cnt);
NMC_API int NMC_CALL_TYPE nmc_free_sw_windows_info(struct st_sw_window_info* info, int info_cnt);
NMC_API int NMC_CALL_TYPE nmc_free_survey_plan_info(struct st_xy_survey_info *ppsurvey_info, int survey_info_cnt);

#ifdef __cplusplus
}
#endif

#endif