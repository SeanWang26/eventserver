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
����˵����sdk��ȫ�ֳ�ʼ������
����˵����
[in] status_callback��ȫ�ֵ��¼��ص�
[in] userdata��       �û�����
************************************************/
NMC_API int NMC_CALL_TYPE nmc_init(nmc_status_callback status_callback, void* userdata);

/************************************************
************************************************/
NMC_API int NMC_CALL_TYPE nmc_uninit(void* data);

/************************************************
����˵������½��nmc
����˵����
[in] info������������Ҫ����Ϣ�������struct login_info����˵��
����ֵ˵�����ɹ����ص�½�����ʧ�ܷ���-1ֵ
************************************************/
NMC_API long NMC_CALL_TYPE nmc_login(struct login_info *info);

/************************************************
����˵������½��nmc
����˵����
[in] handle����nmc_login�������������豸����ľ��
����ֵ˵�����ɹ�����0��ʧ�ܷ��ط�0ֵ
************************************************/
NMC_API int NMC_CALL_TYPE nmc_logout(long handle);

/************************************************
����˵������ȡ�û�͸������
����˵����
[in] handle����nmc_login�������������豸����ľ��
[in] id :����id�ţ�-1��ȡ����
[in] ppuser_data��struct st_xy_user_data *ָ�룬�����û�������Ϣ
[in] user_data_cnt�������û���������
����ֵ˵�����ɹ�����0��ʧ�ܷ��ط�0ֵ
************************************************/
NMC_API int NMC_CALL_TYPE nmc_get_userdefinedata(long handle, int id, struct st_xy_user_data **ppuser_data, int *user_data_cnt);

/************************************************
����˵���������û�͸������
����˵����
[in] handle����nmc_login�������������豸����ľ��
[in] operator_type :���û����ݵĲ�������
[in] ppuser_data :  Ҫ�������û�����
����ֵ˵�����ɹ�����0��ʧ�ܷ��ط�0ֵ
************************************************/
NMC_API int NMC_CALL_TYPE nmc_set_userdefinedata(long handle, enum en_nmc_operator_type operator_type, struct st_xy_user_data *ppuser_data);

/************************************************
����˵������ȡ��������Ϣ
����˵����
[in] handle����nmc_login�������������豸����ľ��
[in] matrixid :������id��-1��ȡ����
[in] matrix_info��struct st_matrix_info *ָ�룬���ؽ�������Ϣ
[in] matrix_info_cnt��struct st_matrix_info *ָ�룬���صĽ���������
����ֵ˵�����ɹ�����0��ʧ�ܷ��ط�0ֵ
************************************************/
NMC_API int NMC_CALL_TYPE nmc_get_matrix(long handle, int matrixid, struct st_matrix_info **ppmatrix_info, int *matrix_info_cnt);


/************************************************
����˵�������ý�������Ϣ
����˵����
[in] handle����nmc_login�������������豸����ľ��
[in] operator_type : �������ͣ�0���ӣ� 1�޸ģ� 2ɾ��
[in] matrix_info��struct st_matrix_info *ָ�룬����Ҫ�����Ľ�������Ϣ
����ֵ˵�����ɹ�����0��ʧ�ܷ��ط�0ֵ
************************************************/
NMC_API int NMC_CALL_TYPE nmc_set_matrix(long handle, enum en_nmc_operator_type operator_type, struct st_matrix_info *matrix_info);

/************************************************
����˵������ȡ����������ķ�����ʽ
����˵����
[in] handle����nmc_login�������������豸����ľ��
[in] outputid :���id��-1��ȡ����
[in] output_layout��struct st_output_layout *ָ�룬�������������Ϣ
[in] output_layout_cnt��struct st_output_layout *ָ�룬���صķ�����Ϣ����
����ֵ˵�����ɹ�����0��ʧ�ܷ��ط�0ֵ
************************************************/
NMC_API int NMC_CALL_TYPE nmc_get_output(long handle, int outputid, struct st_output_layout **ppoutput_layout, int *output_layout_cnt);


/************************************************
����˵�������ý���������ķ�����ʽ
����˵����
[in] handle����nmc_login�������������豸����ľ��
[in] output_layout :Ҫ���õ����
����ֵ˵�����ɹ�����0��ʧ�ܷ��ط�0ֵ
************************************************/
NMC_API int NMC_CALL_TYPE nmc_set_output(long handle, struct st_output_layout *output_layout);

/************************************************
����˵�������ý��봰���ź�
����˵����
[in] handle����nmc_login�������������豸����ľ��
[in] windowtype :�������ͣ�0��ƴ�ӣ�1ƴ��
[in] windowid :Ҫ���õ��������
[in] sourceid :Ҫ���õ�����ź�Դid
[in] subsourceid :��sourceid����NVR������������豸ʱ��subsourceid��ʾ���ź�ԴId
����ֵ˵�����ɹ�����0��ʧ�ܷ��ط�0ֵ
************************************************/
NMC_API int NMC_CALL_TYPE nmc_set_window_signal_source(long handle, int windowtype, int windowid, int sourceid, int subsourceid);

/************************************************
����˵������������ź�
����˵����
[in] handle����nmc_login�������������豸����ľ��
[in] windowtype :�������ͣ�0��ƴ�ӣ�1ƴ��
[in] windowid :Ҫ���õ��������id, ��Ϊ-1ʱ�����outputid�����е��ź�
[in] outputid :Ҫ���õ����id
[in] subsourceid :��sourceid����NVR������������豸ʱ��subsourceid��ʾ���ź�ԴId
����ֵ˵�����ɹ�����0��ʧ�ܷ��ط�0ֵ
************************************************/
NMC_API int NMC_CALL_TYPE nmc_clear_window_signal_source(long handle, int windowtype, int outputid, int windowid);


/************************************************
����˵������ȡ����nvr�ϵĿ�����ӵ��ź�Դ���
����˵����
[in] handle����nmc_login�������������豸����ľ��
[out] equ :����ָ�룬����һ��Ŀǰ���е��ź�Դ
[out] equcnt :����һ��Ŀǰ���е��ź�Դ������
����ֵ˵�����ɹ�����0��ʧ�ܷ��ط�0ֵ
************************************************/
NMC_API int NMC_CALL_TYPE nmc_get_signal_source_type(long handle, char* signaltype, int *len);




/************************************************
����˵������ȡ����nvr�ϵ������ź�Դ
����˵����
[in] handle����nmc_login�������������豸����ľ��
[out] equ :����ָ�룬����һ��Ŀǰ���е��ź�Դ
[out] equcnt :����һ��Ŀǰ���е��ź�Դ������
����ֵ˵�����ɹ�����0��ʧ�ܷ��ط�0ֵ
************************************************/
NMC_API int NMC_CALL_TYPE nmc_get_signal_source(long handle, struct st_jn_equ **equ, int *equcnt);

/************************************************
����˵������ȡԶ���豸
����˵����
[in] handle����nmc_login�������������豸����ľ��
[in] id��device��id��-1��ʾ��ȡ����
[out] dev : ����ָ�룬�����豸��Ϣ
[out] devcnt : �����豸������
����ֵ˵�����ɹ�����0��ʧ�ܷ��ط�0ֵ
************************************************/
NMC_API int NMC_CALL_TYPE nmc_get_remote_device(long handle, int devid, struct st_jn_device **dev, int *devcnt);

/************************************************
����˵������ȡԶ���豸
����˵����
[in] handle����nmc_login�������������豸����ľ��
[in] id��device��id��-1��ʾ��ȡ����
[out] dev : ����ָ�룬�����豸��Ϣ
[out] devcnt : �����豸������
����ֵ˵�����ɹ�����0��ʧ�ܷ��ط�0ֵ
************************************************/
NMC_API int NMC_CALL_TYPE nmc_add_remote_device(long handle, struct st_jn_device *dev);

/************************************************
����˵�����޸�Զ���豸
����˵����
[in] handle����nmc_login�������������豸����ľ��
[in] id��device��id��-1��ʾ��ȡ����
[out] dev : ����ָ�룬�����豸��Ϣ
[out] devcnt : �����豸������
����ֵ˵�����ɹ�����0��ʧ�ܷ��ط�0ֵ
************************************************/
NMC_API int NMC_CALL_TYPE nmc_modify_remote_device(long handle, struct st_jn_device *dev);

/************************************************
����˵������ȡԶ���豸
����˵����
[in] handle����nmc_login�������������豸����ľ��
[in] id��device��id��-1��ʾ��ȡ����
[out] dev : ����ָ�룬�����豸��Ϣ
[out] devcnt : �����豸������
����ֵ˵�����ɹ�����0��ʧ�ܷ��ط�0ֵ
************************************************/
NMC_API int NMC_CALL_TYPE nmc_remove_remote_device(long handle, int devid);

/************************************************
����˵������ȡԶ���豸
����˵����
[in] handle����nmc_login�������������豸����ľ��
[in] id��device��id��-1��ʾ��ȡ����
[out] dev : ����ָ�룬�����豸��Ϣ
[out] devcnt : �����豸������
����ֵ˵�����ɹ�����0��ʧ�ܷ��ط�0ֵ
************************************************/
NMC_API int NMC_CALL_TYPE nmc_get_remote_signal_source(long handle, int deviceid, struct st_jn_equ **equ, int *equcnt);

/*
����˵�������һ���ź�Դ��nmc��
����˵����
[in] handle����nmc_login�������������豸����ľ��
[in out] equ :������Ҫ��ӵ�struct st_jn_equ���ݽṹ��ָ�룬�μ�struct st_jn_equ����
����ֵ˵�����ɹ�����0��ʧ�ܷ��ط�0ֵ
*/
NMC_API int NMC_CALL_TYPE nmc_add_signal_source(long handle, struct st_jn_equ *equ);

/*
����˵������һ���ź�Դ��nmc��ɾ��
����˵����
[in] handle����nmc_login�������������豸����ľ��
[in] sub_equ_id:��Ҫɾ����һ�����豸��id�ţ���Ӧstruct st_jn_sub_equ�ṹ�е�SubEquId
���μ�struct st_jn_sub_equ����
����ֵ˵�����ɹ�����0��ʧ�ܷ��ط�0ֵ
*/
NMC_API int NMC_CALL_TYPE nmc_rmv_signal_source(long handle, int sub_equ_id);

/*
����˵������ȡ��ǰ���е�ƴ��������Ϣ
����˵����
[in] handle����nmc_login�������������豸����ľ��
[in] screenid:��Ҫ��ȡ�Ĵ�����id�ţ�-1��ʾ��ǰnmc�����д���
[in] pplarge_screen_info��struct st_large_screen_info *8ָ�룬����ƴ������Ϣ
[in] large_screen_info_cnt������ƴ������Ϣ����
����ֵ˵�����ɹ�����0��ʧ�ܷ��ط�0ֵ
*/
NMC_API int NMC_CALL_TYPE nmc_get_large_screen(long handle, int screenid, struct st_large_screen_info **pplarge_screen_info, int *large_screen_info_cnt);

/*
����˵������Ӷ���������ƴ��,��������������δ��ڣ�������Ч
����˵����
[in] handle����nmc_login�������������豸����ľ��
[in] plarge_screen_info��struct st_large_screen_info *, ��Ҫ��ӵ�ƴ�ӵ���Ϣ�����û���д
����ֵ˵�����ɹ�����0��ʧ�ܷ��ط�0ֵ
*/
NMC_API int NMC_CALL_TYPE nmc_add_large_screen(long handle, struct st_large_screen_info *plarge_screen_info);

/*
����˵����ɾ������������ƴ��
����˵����
[in] handle����nmc_login�������������豸����ľ��
[in] screenid����Ҫɾ���ĵ�ƴ������id��
����ֵ˵�����ɹ�����0��ʧ�ܷ��ط�0ֵ
*/
NMC_API int NMC_CALL_TYPE nmc_rmv_large_screen(long handle, int screenid);

/*
����˵�������봰�ڻ��������Ϣ��ֻ֧�ֵ�����
����˵����
[in] handle����nmc_login�������������豸����ľ��
[in] isshow���Ƿ���ʾ��0:����ʾ��1:��ʾ
[in] wndyype���������ͣ������ں�Ϊ0
[in] wndid������id
[in] overlayyype����Ҫ��ʾ����Ϣ����enum en_nmc_show_Type
����ֵ˵�����ɹ�����0��ʧ�ܷ��ط�0ֵ
*/
NMC_API int NMC_CALL_TYPE nmc_set_window_overlay_info(long handle, int isshow, int wndyype, int wndid, int overlayyype);

/*
����˵�������봰�ڻ��淭ת,ֻ�Ե�������Ч
����˵����
[in] handle����nmc_login�������������豸����ľ��
[in] wndtype���������ͣ������ں�Ϊ0
[in] wndid������id
����ֵ˵�����ɹ�����0��ʧ�ܷ��ط�0ֵ
*/
NMC_API int NMC_CALL_TYPE nmc_flip_window(long handle, int wndtype, int wndid);

/*
����˵�������봰�ڻ���ȫ����ʾ
����˵����
[in] handle����nmc_login�������������豸����ľ��
[in] isfullscreen���Ƿ�ȫ��
[in] wndtype���������ͣ������ں�Ϊ0
[in] wndid������id
����ֵ˵�����ɹ�����0��ʧ�ܷ��ط�0ֵ
*/
NMC_API int NMC_CALL_TYPE nmc_fullscreen_window(long handle, int isfullscreen, int wndtype, int wndid);

/*
����˵�������봰�ڶ�Ӧ�����ʾ
����˵����
[in] handle����nmc_login�������������豸����ľ��
[in] matrixid��������id
[in] type��0��ʾ��ţ�1��ʾ����ͼ
����ֵ˵�����ɹ�����0��ʧ�ܷ��ط�0ֵ
*/
NMC_API int NMC_CALL_TYPE nmc_show_screen_num(long handle, int matrixid, int type);

/*
����˵�����������δ���
����˵����
[in] handle����nmc_login�������������豸����ľ��
[in] matrixid��������id
[in] type��0��ʾ��ţ�1��ʾ����ͼ
����ֵ˵�����ɹ�����0��ʧ�ܷ��ط�0ֵ
*/
NMC_API int NMC_CALL_TYPE nmc_create_sw_windows(long handle, int width, int height, struct st_create_sw_window_info *pinfo, int info_cnt);

/*
����˵�����������δ���
����˵����
[in] handle����nmc_login�������������豸����ľ��
[in] sw_window_id_list��Ҫɾ�������δ���id�б�
[in] sw_window_id_list_len��Ҫɾ�������δ���id����
����ֵ˵�����ɹ�����0��ʧ�ܷ��ط�0ֵ
*/
NMC_API int NMC_CALL_TYPE nmc_delete_sw_windows(long handle, int *sw_window_id_list, int sw_window_id_list_len);

/*
����˵�����ö����δ���
����˵����
[in] handle����nmc_login�������������豸����ľ��
[in] sw_window_id_list��Ҫ�ö������δ���id
����ֵ˵�����ɹ�����0��ʧ�ܷ��ط�0ֵ
*/
NMC_API int NMC_CALL_TYPE nmc_stick_sw_windows(long handle, int sw_window_id);

/*
����˵������ȡ���δ�����Ϣ
����˵����
[in] handle����nmc_login�������������豸����ľ��
[in] sw_window_id��Ҫ��ȡ�����δ���id
[in] sw_window_info��Ҫ��ȡ�����δ�����Ϣ
����ֵ˵�����ɹ�����0��ʧ�ܷ��ط�0ֵ
*/

NMC_API int NMC_CALL_TYPE nmc_get_sw_windows_info(long handle, int sw_window_id, struct st_sw_window_info** info, int *info_cnt);


/*
����˵��:��ȡ��ѯ������Ϣ
����˵����
[in] survey_id    ��ѯid��-1����ʾ��ȡ����
[in] ppsurvey_info          �豸��Ϣ
[in] len                  �豸��Ϣ����
����ֵ˵�����ɹ�����0��ʧ�ܷ��ط�0
*/
NMC_API int NMC_CALL_TYPE nmc_get_survey_plan(long handle, int survey_id, struct st_xy_survey_info **ppsurvey_info, int *survey_info_cnt);

/*
����˵��:������ѯ����
����˵����
[in] survey_id    ��ѯid��-1����ʾ��ȡ����
[in] ppsurvey_info          �豸��Ϣ
[in] len                  �豸��Ϣ����
����ֵ˵�����ɹ�����0��ʧ�ܷ��ط�0
*/
NMC_API int NMC_CALL_TYPE nmc_set_survey_plan(long handle, enum en_nmc_operator_type operator_type, struct st_xy_survey_info *ppsurvey_info);
/*
����˵��:���Ƶ�������Ѳ
����˵����
[in] ctrl_type    0 ������������Ѳ�� 1 ֹͣ��������Ѳ
[in] survey_id    ��ѯid
[in] wndtype      ��������,Ŀǰֻ֧�ֵ�����,��Ϊ0
[in] wndid        ����id
����ֵ˵�����ɹ�����0��ʧ�ܷ��ط�0
*/
NMC_API int NMC_CALL_TYPE nmc_ctrl_window_survey(long handle, int ctrl_type, int survey_id, int wndtype, int wndid);




/*
����˵��������һ���û�����Դ
����˵����
[in] handle����nmc_login�������������豸����ľ��
[in] arg����������, ����ΪNULL
����ֵ˵�����ɹ����ط�0��ʾ��Դ�����ʧ�ܷ���0
*/

NMC_API long NMC_CALL_TYPE nmc_create_user_stream_source(long handle, void* arg);

/*
����˵��:ɾ���û�����Դ
����˵����
[in] streamhandle����nmc_create_user_stream_source������������������ľ��
����ֵ˵�����ɹ�����0��ʧ�ܷ��ط�0
*/
NMC_API int NMC_CALL_TYPE nmc_delete_user_stream_source(long streamhandle);

NMC_API int NMC_CALL_TYPE nmc_start_push_user_stream(long streamhandle);

NMC_API int NMC_CALL_TYPE nmc_stop_push_user_stream(long streamhandle);

/*
����˵������ȡ��ǰ���е��û�����Դ��״̬
����˵����
[in] handle ����nmc_create_user_stream_source������������������ľ�����������0����������
[in] streamhandle ����nmc_create_user_stream_source������������������ľ�����������0����������
[out] ppuser_stream_state : ����ָ�룬�����豸��Ϣ
[out] pstatecnt : �����豸������
����ֵ˵�����ɹ�����0��ʧ�ܷ��ط�0
*/
NMC_API int NMC_CALL_TYPE nmc_get_push_user_stream_state(long handle, long streamhandle, struct st_user_stream_state **ppuser_stream_state, int *pstatecnt);



/*
����˵��:�����û�����Դ��Ϣ
����˵����
[in] streamhandle����nmc_create_user_stream_source������������������ľ��
[in] struct st_video_stream_info *info, ��Դ��Ϣ
[in] len, struct st_video_stream_info�ṹ�峤��
����ֵ˵�����ɹ�����0��ʧ�ܷ��ط�0
*/
NMC_API int NMC_CALL_TYPE nmc_set_user_stream_source_info(long streamhandle, struct st_video_stream_info *info);

/*
����˵��:�����û�����Դ������
����˵����
[in] streamhandle����nmc_create_user_stream_source������������������ľ��
[in] windowtype ��������
[in] windowid ����id
����ֵ˵�����ɹ�����0��ʧ�ܷ��ط�0
*/
NMC_API int NMC_CALL_TYPE nmc_set_user_stream_source_to_window(long streamhandle, int windowtype, int windowid);

/*
����˵��:�Ӵ�������û�����Դ
����˵����
[in] streamhandle����nmc_create_user_stream_source������������������ľ��
[in] windowtype ��������
[in] windowid ����id, idΪ0���������֮ǰ���õ����д���
����ֵ˵�����ɹ�����0��ʧ�ܷ��ط�0
*/
NMC_API int NMC_CALL_TYPE nmc_clear_user_stream_source_from_window(long streamhandle, int windowtype, int windowid);
/*
����˵��:����һ֡��Ƶ������,��������
����˵����
[in] streamhandle����nmc_create_user_stream_source������������������ľ��
[in] info          ����Դ��Ϣ
[in] pstreamdata   ������ָ��
[in] streamdatalen �����ݳ��ȣ�ע��:�����ݱ�����������һ֡����
����ֵ˵�����ɹ�����0��ʧ�ܷ��ط�0
*/
NMC_API int NMC_CALL_TYPE nmc_push_user_stream_video_data(long streamhandle, struct st_video_stream_info *info, char* pstreamdata, int streamdatalen);



/*
����˵��:�������������豸
����˵����
[in] device_info          �豸��Ϣ
[in] len                  �豸��Ϣ����
����ֵ˵�����ɹ�����0��ʧ�ܷ��ط�0
*/
NMC_API int NMC_CALL_TYPE nmc_search_device(struct st_xy_device_info **device_info, int *len);

/*
����˵��:�޸��豸�������
����˵����
[in] device_info          �豸��Ϣ
[in] len                  �豸��Ϣ����
����ֵ˵�����ɹ�����0��ʧ�ܷ��ط�0
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