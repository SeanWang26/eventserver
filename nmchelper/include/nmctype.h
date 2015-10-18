#ifndef NMC_TYPE_H
#define NMC_TYPE_H

//c 层使用

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
	NMC_EVENT_DISCONNECT         =1000,//连接断线
	NDM_EVENT_DISCONNECT         =1001,//解码器断线
	NDM_EVENT_CONNECTED          =1002,//解码器连接
	NDM_GET_SEARCH_DEVCIE        =1003,//搜索到设备
	NMC_PUSH_STREAM_DISCONNECT   =1004,//推流断开
};

enum en_nmc_show_Type
{
	NMC_SHOW_IPC_NAME=1,         //显示信号名字
	NDM_SHOW_FRAME_PER_SECOND=2, //显示帧率
	NDM_SHOW_BITREATE =4,        //显示码率
};

enum en_nmc_windowtType
{
	NMC_SINGLE_WINDOW=0,    //单窗口   0
	NMC_LARGE_WINDOW,       //拼接窗口 1
	NMC_SLIDING_WINDOW,     //漫游窗口 2
};

enum en_nmc_power_manage
{
	NMC_HALT=1,    // 关闭
	NMC_REBOOT,       // 重启
};

/************************************************
函数说明：获取状态变化的回调函数
参数说明：
[out] handle：调用设备句柄
[out] type：  状态类型
[out] data：  状态数据
返回值说明：正确返回0，不正确返回非0值
************************************************/
typedef int (NMC_CALL_TYPE *nmc_status_callback)(long handle, int type, void* data, int datalen, void *userdata);

//0添加，1修改，2删除
enum en_nmc_operator_type
{
	nmc_add=0,
	nmc_modify=1,
	nmc_remov=2,
};

//登陆
struct login_info
{
	char                     ip[256];
	unsigned                 port;
	char                     user[32];
	char                     password[32];
};

//用户数据
struct st_xy_user_data
{
	int                     id;
	int                 type;
	char*                    data;
	int                     len;
};

struct st_output_info
{
	int                  id;                      //一个输出在整个系统的编号，一个输出对应一个物理端口
	int                  index;                   //一个输出在一个解码器的内部的编号
	int                  hasdisplay;              //是否连接显示器
	char                 resolution[256];         //支持的解析度,支持多个以逗号隔开,比如“1920x1080,1280x720”
};

struct st_matrix_info
{
	int                     id;                  //解码器的id号
	int                     status;              //解码器的状态，在线或离线？？？？？？？？？？？？？？？？？？？？？？？？？
	char                    name[64];            //解码器名称
	char                    ip[64];              //解码器ip
	int                     port;                //登陆端口
	char                    user[64];            //登陆用户名
	char                    password[64];        //登陆端口
	struct st_output_info*  output_info;         //该解码器的输出信息，，不为NULL时用户须自己释放该指针
	int                     output_info_cnt;     //output_info的条数
};

struct st_window_info
{
	int                     id;              //窗口的全局id
	int                     index;           //窗口在本输出的索引      
	int                     status;          //窗口状态，0空闲、1实时流（包括RTSP和录屏）、2录像、3单窗口轮巡、4分组轮巡、5GIS、6报警
	int                     sourceid;        //内部nvr的id
	int                     subsourceid;        //内部nvr的id
	char                    SurveyUuid[256]; //暂时无用
	char                    Extend[256];     //sourceid为内部nvr的id时，Extend为一个子设备id
};

struct st_output_layout
{
	int                     id;                   //该布局方式的id号
	int                     layout;               //布局方式，1代表一分屏，2代表2分屏，4代表4分屏，以此类推
	struct st_window_info*  window_info;          //该布局下的窗口信息
	int                     window_info_cnt;      //窗口信息条数
};

struct st_output_mapping
{
	int                     mappingid;          //映射关系编号
	int                     outputid;           //输出编号
	int                     monitorid;          //屏幕编号
};

struct st_pic_para_info
{
	int brightness;            //亮度  取值 0~100
	int contrast;              //对比度 取值  0~100
	int saturation;            //饱和度 取值  0~100
	int hue;                   //色度   取值  0~100
	int sharpness;             //明锐度  取值  0~100
	int noisereduction;        //降噪    取值  0~100 
};

struct st_subscreen_info
{
	int OutputId;                 //输出id号
	int Row;                      //在拼接屏中的位置 行
	int Col;                      //在拼接屏中的位置 列
};

//拼接屏信息
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

//创建漫游窗口
struct st_create_sw_window_info
{
	int sw_window_id;
	int sw_sequence;     //窗口次序(漫游窗口重叠是，窗口的次序，值越大的窗口越在顶层)

	int outputid;        //输出id号
	int output_start_x;  //该漫游窗口在outputid屏幕上显示的起始的x位置
	int output_start_y;  //该漫游窗口在outputid屏幕上显示的起始的y位置

	int display_x;     //该漫游窗口在outputid屏幕上显示自己本幅画面的起始x位置
	int display_y;     //该漫游窗口在outputid屏幕上显示自己本幅画面的起始y位置

	int width;
	int heigh;

	int sub_win_width;
	int sub_win_height;
};

//漫游窗口当前状态
struct st_sw_window_info
{
	int sw_window_id;      //漫游窗口id号
	int sw_window_status;  //窗口状态, 0:空闲  1:有实时流
	int source_id;         //有实时流时的源id
	int source_sub_id;     //有实时流源子id
};

//视频图像码流信息
struct st_video_stream_info
{
	int len;            //本结构体长度sizeof(struct st_video_stream_info)
	int codec_type;     //编码类型,目前只支持H.264,取值0x0080
	int width;          //宽
	int height;         //高
	int fps;            //帧率
	int bitrate;        //码率,单位bit
	int frametype;      //1：i帧, 2：b帧, 3：p帧
	int res[4];         
};

//用户推流状态
struct st_user_stream_state
{
	int handle;
	int state;
	char ip[64];
	int  port;
};

struct st_xy_device_info
{
	int   len;               //本结构体长度sizeof(struct st_video_stream_info)
	int   device_type;       //设备类型
	char  ipv4[32];          //ip地址
	char  mask[32];          //掩码
	char  gateway[32];       //地址
	char  name[128];         //名字
	///int   res[64];

	char  iface[16];         //接口名字
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