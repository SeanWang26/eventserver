////////////////////矩阵控制器////////////////////////
#ifndef NMC_CMD_DEFINE_H
#define NMC_CMD_DEFINE_H

#pragma pack(push,1)
typedef struct stExternalCommand
{
	unsigned int 	nSrcType;
	unsigned int 	nCmdType;
	unsigned int 	nCmdSeq;
	unsigned int 	nContentSize;

	stExternalCommand()
	{
		nSrcType = 0;
		nCmdType = 0;
		nCmdSeq = 0;
		nContentSize = 0;
	};
}ExCommand;
#pragma pack(pop)

#define EX_SERVER_NMC                                   4

#define	EX_HEART_BEAT_REQ				                0x0b10 //服务器心跳检查
#define	EX_HEART_BEAT_RSP				                0x0b11

#define EX_NMC_LOGIN_REQ 								0x2300 //NMC登录信令
#define EX_NMC_LOGIN_RSP 								0x2301

#define EX_NMC_GET_PHYSIC_MONITOR_REQ					0x2100 //获取物理监视器
#define EX_NMC_GET_PHYSIC_MONITOR_RSP					0x2101

#define EX_NMC_SET_PHYSIC_MONITOR_REQ					0x2102 //设置物理监视器
#define EX_NMC_SET_PHYSIC_MONITOR_RSP					0x2103

#define EX_NMC_GET_MONITOR_FRAME_REQ					0x2104 //获取监视器边框
#define EX_NMC_GET_MONITOR_FRAME_RSP					0x2105

#define EX_NMC_SET_MONITOR_FRAME_REQ					0x2106 //设置监视器边框
#define EX_NMC_SET_MONITOR_FRAME_RSP					0x2107

#define EX_NMC_GET_USER_DEFINE_DATA_REQ					0x2120 //获取用户自定义数据
#define EX_NMC_GET_USER_DEFINE_DATA_RSP					0x2121

#define EX_NMC_SET_USER_DEFINE_DATA_REQ					0x2122 //设置用户自定义数据
#define EX_NMC_SET_USER_DEFINE_DATA_RSP					0x2123

#define EX_NMC_GET_SIGNAL_SOURCE_INFO_REQ               0x2110  //获取添加的外部nvr
#define EX_NMC_GET_SIGNAL_SOURCE_INFO_RSP               0x2111

#define EX_NMC_GET_MATRIX_INFO_REQ						0x2108 //获取解码器信息
#define EX_NMC_GET_MATRIX_INFO_RSP						0x2109

#define EX_NMC_SET_MATRIX_REQ							0x210a //设置解码器
#define EX_NMC_SET_MATRIX_RSP							0x210b

#define EX_NMC_GET_OUTPUT_MAPPING_REQ					0x210c //获取解码器输出与物理监视器对应关系
#define EX_NMC_GET_OUTPUT_MAPPING_RSP					0x210d

#define EX_NMC_SET_OUTPUT_MAPPING_REQ					0x210e //设置解码器输出与物理监视器对应关系
#define EX_NMC_SET_OUTPUT_MAPPING_RSP					0x210f

#define EX_NMC_MATRIX_CONN_STATUS_NOTIFY				0x21e0 //解码器连接状态通知



#define EX_NMC_GET_SIGNAL_SOURCE_INFO_REQ				0x2110 //获取信号源信息
#define EX_NMC_GET_SIGNAL_SOURCE_INFO_RSP				0x2111

#define EX_NMC_SET_SIGNAL_SOURCE_REQ					0x2112 //添加信号源
#define EX_NMC_SET_SIGNAL_SOURCE_RSP					0x2113

#define EX_NMC_GET_SIGNAL_SOURCE_PICTURE_PARA_REQ		0x2124 //获取信号源图像参数信息
#define EX_NMC_GET_SIGNAL_SOURCE_PICTURE_PARA_RSP		0x2125



#define EX_NMC_GET_LARGE_SCREEN_REQ						0x2200 //获取多解码器输出拼接方式
#define EX_NMC_GET_LARGE_SCREEN_RSP						0x2201

#define EX_NMC_ADD_LARGE_SCREEN_REQ						0x2202 //添加多解码器输出拼接方式
#define EX_NMC_ADD_LARGE_SCREEN_RSP						0x2203

#define EX_NMC_RMV_LARGE_SCREEN_REQ						0x2204 //删除多解码器输出拼接方式
#define EX_NMC_RMV_LARGE_SCREEN_RSP						0x2205

#define EX_NMC_GET_OUTPUT_SCREEN_LAYOUT_REQ				0x2206 //获取输出分屏方式
#define EX_NMC_GET_OUTPUT_SCREEN_LAYOUT_RSP				0x2207

#define EX_NMC_SET_OUTPUT_SCREEN_LAYOUT_REQ				0x2208 //设置输出分屏方式
#define EX_NMC_SET_OUTPUT_SCREEN_LAYOUT_RSP				0x2209



#define EX_NMC_SET_WINDOW_SIGNAL_SOURCE_REQ				0x220a //设置解码窗口信号源
#define EX_NMC_SET_WINDOW_SIGNAL_SOURCE_RSP				0x220b

#define EX_NMC_CLEAR_WINDOW_SIGNAL_SOURCE_REQ			0x220c //清除解码窗口信号源
#define EX_NMC_CLEAR_WINDOW_SIGNAL_SOURCE_RSP			0x220d

#define EX_NMC_SET_WINDOW_PICTURE_PARA_REQ				0x220e //解码窗口图像调节
#define EX_NMC_SET_WINDOW_PICTURE_PARA_RSP				0x220f

#define EX_NMC_SET_WINDOW_OVERLAY_INFO_REQ				0x2210 //解码窗口画面叠加信息
#define EX_NMC_SET_WINDOW_OVERLAY_INFO_RSP				0x2211

#define EX_NMC_FLIP_WINDOW_REQ							0x2212 //解码窗口画面翻转
#define EX_NMC_FLIP_WINDOW_RSP							0x2213

#define EX_NMC_FULLSCREEN_WINDOW_REQ					0x2214 //解码窗口画面全屏显示
#define EX_NMC_FULLSCREEN_WINDOW_RSP					0x2215



#define EX_NMC_START_GROUP_SURVEY_REQ					0x2216 //启动分组轮巡
#define EX_NMC_START_GROUP_SURVEY_RSP					0x2217

#define EX_NMC_STOP_GROUP_SURVEY_REQ					0x2218 //停止分组轮巡
#define EX_NMC_STOP_GROUP_SURVEY_RSP					0x2219

#define EX_NMC_START_WINDOW_SURVEY_REQ					0x221a //启动单窗口轮巡
#define EX_NMC_START_WINDOW_SURVEY_RSP					0x221b

#define EX_NMC_STOP_WINDOW_SURVEY_REQ					0x221c //停止单窗口轮巡
#define EX_NMC_STOP_WINDOW_SURVEY_RSP					0x221d

#define EX_NMC_SW_CREATE_WINDOW_REQ						0x221e // 添加漫游窗口
#define EX_NMC_SW_CREATE_WINDOW_RSP						0x221f

#define EX_NMC_SW_DEL_WINDOW_REQ						0x2220 // 删除漫游窗口
#define EX_NMC_SW_DEL_WINDOW_RSP						0x2221

#define EX_NMC_SW_STICK_WINDOW_REQ						0x2222 // 置顶显示漫游窗口
#define EX_NMC_SW_STICK_WINDOW_RSP						0x2223

#define EX_NMC_SW_GET_WIN_INFO_REQ						0x2224 //获取漫游窗口资源信息
#define EX_NMC_SW_GET_WIN_INFO_RSP						0x2225

#define EX_NMC_SHOW_SCRNUM_PIC_REQ                      0x2226 //显示屏幕编号
#define EX_NMC_SHOW_SCRNUM_PIC_RSP                      0x2227

#define EX_NMC_SW_CREATE_WINDOW_REQ                     0x221e //创建漫游窗口
#define EX_NMC_SW_CREATE_WINDOW_RSP                     0x221f

#define EX_NMC_SW_DEL_WINDOW_REQ						0x2220 //删除漫游窗口
#define EX_NMC_SW_DEL_WINDOW_RSP						0x2221

#define	EX_NMC_SW_STICK_WINDOW_REQ						0x2222 //置顶漫游窗口
#define EX_NMC_SW_STICK_WINDOW_RSP						0x2223

#define EX_NMC_SW_GET_WIN_INFO_REQ						0x2224 //获取漫游窗口信息
#define EX_NMC_SW_GET_WIN_INFO_RSP						0x2225

#define EX_NMC_GET_SURVEY_PLAN_REQ						0x2216 //获取轮询方案
#define EX_NMC_GET_SURVEY_PLAN_RSP						0x2217

#define EX_NMC_SET_SURVEY_PLAN_REQ						0x2218 //设置轮询方案
#define EX_NMC_SET_SURVEY_PLAN_RSP						0x2219

#define EX_NMC_CTRL_WINDOW_SURVEY_REQ                   0x221a //
#define EX_NMC_CTRL_WINDOW_SURVEY_RSP                   0x221b

//////////发往virtualserver
#define EX_PREPARE_PUSH_STREAM_REQ						0x4000  //准备推送流
#define EX_PREPARE_PUSH_STREAM_RSP						0x4001

#define EX_SET_STREAM_INFO_REQ								0x4002 //设置流信息
#define EX_SET_STREAM_INFO_RSP								0x4003

#define EX_CTRL_PUSH_STREAM_REQ								0x4004 //控制推送流
#define EX_CTRL_PUSH_STREAM_RSP								0x4005

#endif