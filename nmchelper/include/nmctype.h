#ifndef NMC_TYPE_H
#define NMC_TYPE_H

//c ��ʹ��

#ifdef WIN32 //windows platform

#ifdef NMC_USER_MODULE_EXPORTS
#define NMC_API __declspec(dllexport)
#else
#define NMC_API __declspec(dllimport)
#endif

#ifndef NMC_CALL_TYPE
#define NMC_CALL_TYPE  	__stdcall  
#endif

#else //linux platform

#ifndef NMC_API
#define NMC_API __attribute__ ((visibility ("default")))
#endif

#ifndef NMC_CALL_TYPE
#define NMC_CALL_TYPE
#endif

#endif


enum
{
	NMC_EVENT_DISCONNECT         =1000,//���Ӷ���
	NDM_EVENT_DISCONNECT         =1001,//����������
	NDM_EVENT_CONNECTED          =1002,//����������
	NDM_GET_SEARCH_DEVCIE        =1003,//�������豸
	NMC_PUSH_STREAM_DISCONNECT   =1004,//�����Ͽ�
};

enum en_nmc_show_Type
{
	NMC_SHOW_IPC_NAME=1,         //��ʾ�ź�����
	NDM_SHOW_FRAME_PER_SECOND=2, //��ʾ֡��
	NDM_SHOW_BITREATE =4,        //��ʾ����
};

enum en_nmc_windowtType
{
	NMC_SINGLE_WINDOW=0,    //������   0
	NMC_LARGE_WINDOW,       //ƴ�Ӵ��� 1
	NMC_SLIDING_WINDOW,     //���δ��� 2
};

enum en_nmc_power_manage
{
	NMC_HALT=1,    // �ر�
	NMC_REBOOT,       // ����
};

/************************************************
����˵������ȡ״̬�仯�Ļص�����
����˵����
[out] handle�������豸���
[out] type��  ״̬����
[out] data��  ״̬����
����ֵ˵������ȷ����0������ȷ���ط�0ֵ
************************************************/
typedef int (NMC_CALL_TYPE *nmc_status_callback)(long handle, int type, void* data, int datalen, void *userdata);

//0��ӣ�1�޸ģ�2ɾ��
enum en_nmc_operator_type
{
	nmc_add=0,
	nmc_modify=1,
	nmc_remov=2,
};

//��½
struct login_info
{
	char                     ip[256];
	unsigned                 port;
	char                     user[32];
	char                     password[32];
};

//�û�����
struct st_xy_user_data
{
	int                     id;
	int                 type;
	char*                    data;
	int                     len;
};

struct st_output_info
{
	int                  id;                      //һ�����������ϵͳ�ı�ţ�һ�������Ӧһ������˿�
	int                  index;                   //һ�������һ�����������ڲ��ı��
	int                  hasdisplay;              //�Ƿ�������ʾ��
	char                 resolution[256];         //֧�ֵĽ�����,֧�ֶ���Զ��Ÿ���,���硰1920x1080,1280x720��
};

struct st_matrix_info
{
	int                     id;                  //��������id��
	int                     status;              //��������״̬�����߻����ߣ�������������������������������������������������
	char                    name[64];            //����������
	char                    ip[64];              //������ip
	int                     port;                //��½�˿�
	char                    user[64];            //��½�û���
	char                    password[64];        //��½�˿�
	struct st_output_info*  output_info;         //�ý������������Ϣ������ΪNULLʱ�û����Լ��ͷŸ�ָ��
	int                     output_info_cnt;     //output_info������
};

struct st_window_info
{
	int                     id;              //���ڵ�ȫ��id
	int                     index;           //�����ڱ����������      
	int                     status;          //����״̬��0���С�1ʵʱ��������RTSP��¼������2¼��3��������Ѳ��4������Ѳ��5GIS��6����
	int                     sourceid;        //�ڲ�nvr��id
	int                     subsourceid;        //�ڲ�nvr��id
	char                    SurveyUuid[256]; //��ʱ����
	char                    Extend[256];     //sourceidΪ�ڲ�nvr��idʱ��ExtendΪһ�����豸id
};

struct st_output_layout
{
	int                     id;                   //�ò��ַ�ʽ��id��
	int                     layout;               //���ַ�ʽ��1����һ������2����2������4����4�������Դ�����
	struct st_window_info*  window_info;          //�ò����µĴ�����Ϣ
	int                     window_info_cnt;      //������Ϣ����
};

struct st_output_mapping
{
	int                     mappingid;          //ӳ���ϵ���
	int                     outputid;           //������
	int                     monitorid;          //��Ļ���
};

struct st_pic_para_info
{
	int brightness;            //����  ȡֵ 0~100
	int contrast;              //�Աȶ� ȡֵ  0~100
	int saturation;            //���Ͷ� ȡֵ  0~100
	int hue;                   //ɫ��   ȡֵ  0~100
	int sharpness;             //�����  ȡֵ  0~100
	int noisereduction;        //����    ȡֵ  0~100 
};

struct st_subscreen_info
{
	int OutputId;                 //���id��
	int Row;                      //��ƴ�����е�λ�� ��
	int Col;                      //��ƴ�����е�λ�� ��
};

//ƴ������Ϣ
struct st_large_screen_info
{
	int                       screenid;                //
	int                       screenstatus;            //
	int                       rows;                    //
	int                       cols;                    //
	char                      screenname[64];          //
	int                       sourceid;                //
	int                       subsourceid;             //
	struct st_subscreen_info  *psubscreen_info;        //
	int                       subscreen_info_cnt;      //
};

//�������δ���
struct st_create_sw_window_info
{
	int sw_window_id;
	int sw_sequence;     //���ڴ���(���δ����ص��ǣ����ڵĴ���ֵԽ��Ĵ���Խ�ڶ���)

	int outputid;        //���id��
	int output_start_x;  //�����δ�����outputid��Ļ����ʾ����ʼ��xλ��
	int output_start_y;  //�����δ�����outputid��Ļ����ʾ����ʼ��yλ��

	int display_x;     //�����δ�����outputid��Ļ����ʾ�Լ������������ʼxλ��
	int display_y;     //�����δ�����outputid��Ļ����ʾ�Լ������������ʼyλ��

	int width;
	int heigh;

	int sub_win_width;
	int sub_win_height;
};

//���δ��ڵ�ǰ״̬
struct st_sw_window_info
{
	int sw_window_id;      //���δ���id��
	int sw_window_status;  //����״̬, 0:����  1:��ʵʱ��
	int source_id;         //��ʵʱ��ʱ��Դid
	int source_sub_id;     //��ʵʱ��Դ��id
};

//��Ƶͼ��������Ϣ
struct st_video_stream_info
{
	int len;            //���ṹ�峤��sizeof(struct st_video_stream_info)
	int codec_type;     //��������,Ŀǰֻ֧��H.264,ȡֵ0x0080
	int width;          //��
	int height;         //��
	int fps;            //֡��
	int bitrate;        //����,��λbit
	int frametype;      //1��i֡, 2��b֡, 3��p֡
	int res[4];         
};

//�û�����״̬
struct st_user_stream_state
{
	int handle;
	int state;
	char ip[64];
	int  port;
};

struct st_xy_device_info
{
	int   len;               //���ṹ�峤��sizeof(struct st_video_stream_info)
	int   device_type;       //�豸����
	char  ipv4[32];          //ip��ַ
	char  mask[32];          //����
	char  gateway[32];       //��ַ
	char  name[128];         //����
	///int   res[64];

	char  iface[16];         //�ӿ�����
	int   res[56];           //
};

struct st_jn_device
{
	int             DeviceId;
	int             DeviceType;
	char 	        DeviceName[256];
	char 	        IP[256];
	int 	        Port;
	char 	        User[64];
	char 	        Password[64];
	char            Uri[128];
	char 	        Extend[128];
};

struct st_xy_sourceinfo
{
	int sourceid;
	int subsourceid;
};

struct st_xy_survey_info
{
	int  survey_id;
	char surveyname[64];
	int  surveyInterval;
	struct st_xy_sourceinfo *pst_xy_sourceinfo;
	int    sourceinfo_cnt;
};


#endif