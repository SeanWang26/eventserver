////////////////////���������////////////////////////
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

#define	EX_HEART_BEAT_REQ				                0x0b10 //�������������
#define	EX_HEART_BEAT_RSP				                0x0b11

#define EX_NMC_LOGIN_REQ 								0x2300 //NMC��¼����
#define EX_NMC_LOGIN_RSP 								0x2301

#define EX_NMC_GET_PHYSIC_MONITOR_REQ					0x2100 //��ȡ���������
#define EX_NMC_GET_PHYSIC_MONITOR_RSP					0x2101

#define EX_NMC_SET_PHYSIC_MONITOR_REQ					0x2102 //�������������
#define EX_NMC_SET_PHYSIC_MONITOR_RSP					0x2103

#define EX_NMC_GET_MONITOR_FRAME_REQ					0x2104 //��ȡ�������߿�
#define EX_NMC_GET_MONITOR_FRAME_RSP					0x2105

#define EX_NMC_SET_MONITOR_FRAME_REQ					0x2106 //���ü������߿�
#define EX_NMC_SET_MONITOR_FRAME_RSP					0x2107

#define EX_NMC_GET_USER_DEFINE_DATA_REQ					0x2120 //��ȡ�û��Զ�������
#define EX_NMC_GET_USER_DEFINE_DATA_RSP					0x2121

#define EX_NMC_SET_USER_DEFINE_DATA_REQ					0x2122 //�����û��Զ�������
#define EX_NMC_SET_USER_DEFINE_DATA_RSP					0x2123

#define EX_NMC_GET_SIGNAL_SOURCE_INFO_REQ               0x2110  //��ȡ��ӵ��ⲿnvr
#define EX_NMC_GET_SIGNAL_SOURCE_INFO_RSP               0x2111

#define EX_NMC_GET_MATRIX_INFO_REQ						0x2108 //��ȡ��������Ϣ
#define EX_NMC_GET_MATRIX_INFO_RSP						0x2109

#define EX_NMC_SET_MATRIX_REQ							0x210a //���ý�����
#define EX_NMC_SET_MATRIX_RSP							0x210b

#define EX_NMC_GET_OUTPUT_MAPPING_REQ					0x210c //��ȡ����������������������Ӧ��ϵ
#define EX_NMC_GET_OUTPUT_MAPPING_RSP					0x210d

#define EX_NMC_SET_OUTPUT_MAPPING_REQ					0x210e //���ý���������������������Ӧ��ϵ
#define EX_NMC_SET_OUTPUT_MAPPING_RSP					0x210f

#define EX_NMC_MATRIX_CONN_STATUS_NOTIFY				0x21e0 //����������״̬֪ͨ



#define EX_NMC_GET_SIGNAL_SOURCE_INFO_REQ				0x2110 //��ȡ�ź�Դ��Ϣ
#define EX_NMC_GET_SIGNAL_SOURCE_INFO_RSP				0x2111

#define EX_NMC_SET_SIGNAL_SOURCE_REQ					0x2112 //����ź�Դ
#define EX_NMC_SET_SIGNAL_SOURCE_RSP					0x2113

#define EX_NMC_GET_SIGNAL_SOURCE_PICTURE_PARA_REQ		0x2124 //��ȡ�ź�Դͼ�������Ϣ
#define EX_NMC_GET_SIGNAL_SOURCE_PICTURE_PARA_RSP		0x2125



#define EX_NMC_GET_LARGE_SCREEN_REQ						0x2200 //��ȡ����������ƴ�ӷ�ʽ
#define EX_NMC_GET_LARGE_SCREEN_RSP						0x2201

#define EX_NMC_ADD_LARGE_SCREEN_REQ						0x2202 //��Ӷ���������ƴ�ӷ�ʽ
#define EX_NMC_ADD_LARGE_SCREEN_RSP						0x2203

#define EX_NMC_RMV_LARGE_SCREEN_REQ						0x2204 //ɾ������������ƴ�ӷ�ʽ
#define EX_NMC_RMV_LARGE_SCREEN_RSP						0x2205

#define EX_NMC_GET_OUTPUT_SCREEN_LAYOUT_REQ				0x2206 //��ȡ���������ʽ
#define EX_NMC_GET_OUTPUT_SCREEN_LAYOUT_RSP				0x2207

#define EX_NMC_SET_OUTPUT_SCREEN_LAYOUT_REQ				0x2208 //�������������ʽ
#define EX_NMC_SET_OUTPUT_SCREEN_LAYOUT_RSP				0x2209



#define EX_NMC_SET_WINDOW_SIGNAL_SOURCE_REQ				0x220a //���ý��봰���ź�Դ
#define EX_NMC_SET_WINDOW_SIGNAL_SOURCE_RSP				0x220b

#define EX_NMC_CLEAR_WINDOW_SIGNAL_SOURCE_REQ			0x220c //������봰���ź�Դ
#define EX_NMC_CLEAR_WINDOW_SIGNAL_SOURCE_RSP			0x220d

#define EX_NMC_SET_WINDOW_PICTURE_PARA_REQ				0x220e //���봰��ͼ�����
#define EX_NMC_SET_WINDOW_PICTURE_PARA_RSP				0x220f

#define EX_NMC_SET_WINDOW_OVERLAY_INFO_REQ				0x2210 //���봰�ڻ��������Ϣ
#define EX_NMC_SET_WINDOW_OVERLAY_INFO_RSP				0x2211

#define EX_NMC_FLIP_WINDOW_REQ							0x2212 //���봰�ڻ��淭ת
#define EX_NMC_FLIP_WINDOW_RSP							0x2213

#define EX_NMC_FULLSCREEN_WINDOW_REQ					0x2214 //���봰�ڻ���ȫ����ʾ
#define EX_NMC_FULLSCREEN_WINDOW_RSP					0x2215



#define EX_NMC_START_GROUP_SURVEY_REQ					0x2216 //����������Ѳ
#define EX_NMC_START_GROUP_SURVEY_RSP					0x2217

#define EX_NMC_STOP_GROUP_SURVEY_REQ					0x2218 //ֹͣ������Ѳ
#define EX_NMC_STOP_GROUP_SURVEY_RSP					0x2219

#define EX_NMC_START_WINDOW_SURVEY_REQ					0x221a //������������Ѳ
#define EX_NMC_START_WINDOW_SURVEY_RSP					0x221b

#define EX_NMC_STOP_WINDOW_SURVEY_REQ					0x221c //ֹͣ��������Ѳ
#define EX_NMC_STOP_WINDOW_SURVEY_RSP					0x221d

#define EX_NMC_SW_CREATE_WINDOW_REQ						0x221e // ������δ���
#define EX_NMC_SW_CREATE_WINDOW_RSP						0x221f

#define EX_NMC_SW_DEL_WINDOW_REQ						0x2220 // ɾ�����δ���
#define EX_NMC_SW_DEL_WINDOW_RSP						0x2221

#define EX_NMC_SW_STICK_WINDOW_REQ						0x2222 // �ö���ʾ���δ���
#define EX_NMC_SW_STICK_WINDOW_RSP						0x2223

#define EX_NMC_SW_GET_WIN_INFO_REQ						0x2224 //��ȡ���δ�����Դ��Ϣ
#define EX_NMC_SW_GET_WIN_INFO_RSP						0x2225

#define EX_NMC_SHOW_SCRNUM_PIC_REQ                      0x2226 //��ʾ��Ļ���
#define EX_NMC_SHOW_SCRNUM_PIC_RSP                      0x2227

#define EX_NMC_SW_CREATE_WINDOW_REQ                     0x221e //�������δ���
#define EX_NMC_SW_CREATE_WINDOW_RSP                     0x221f

#define EX_NMC_SW_DEL_WINDOW_REQ						0x2220 //ɾ�����δ���
#define EX_NMC_SW_DEL_WINDOW_RSP						0x2221

#define	EX_NMC_SW_STICK_WINDOW_REQ						0x2222 //�ö����δ���
#define EX_NMC_SW_STICK_WINDOW_RSP						0x2223

#define EX_NMC_SW_GET_WIN_INFO_REQ						0x2224 //��ȡ���δ�����Ϣ
#define EX_NMC_SW_GET_WIN_INFO_RSP						0x2225

#define EX_NMC_GET_SURVEY_PLAN_REQ						0x2216 //��ȡ��ѯ����
#define EX_NMC_GET_SURVEY_PLAN_RSP						0x2217

#define EX_NMC_SET_SURVEY_PLAN_REQ						0x2218 //������ѯ����
#define EX_NMC_SET_SURVEY_PLAN_RSP						0x2219

#define EX_NMC_CTRL_WINDOW_SURVEY_REQ                   0x221a //
#define EX_NMC_CTRL_WINDOW_SURVEY_RSP                   0x221b

//////////����virtualserver
#define EX_PREPARE_PUSH_STREAM_REQ						0x4000  //׼��������
#define EX_PREPARE_PUSH_STREAM_RSP						0x4001

#define EX_SET_STREAM_INFO_REQ								0x4002 //��������Ϣ
#define EX_SET_STREAM_INFO_RSP								0x4003

#define EX_CTRL_PUSH_STREAM_REQ								0x4004 //����������
#define EX_CTRL_PUSH_STREAM_RSP								0x4005

#endif