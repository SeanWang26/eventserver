// DialogPushSteamTest.cpp : 实现文件
//

#include "stdafx.h"
#include "nmc_mfc_test.h"
#include "DialogPushSteamTest.h"
#include "afxdialogex.h"
#include "NMCUser.h"

// CDialogPushSteamTest 对话框

IMPLEMENT_DYNAMIC(CDialogPushSteamTest, CDialogEx)
static int init = 0;
CDialogPushSteamTest::CDialogPushSteamTest(long handle, std::map<int,int> WndIdMapS) 
		: CDialogEx(CDialogPushSteamTest::IDD, NULL)
{
	if(init==0)
	{	
		init = 1;
		avcodec_register_all();
		av_register_all();
		AfxOleInit();
	}
	m_Handle = handle;
	m_WndIdMapS = WndIdMapS;
}
CDialogPushSteamTest::CDialogPushSteamTest(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDialogPushSteamTest::IDD, pParent)
{

}

CDialogPushSteamTest::~CDialogPushSteamTest()
{
}

void CDialogPushSteamTest::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_PsStreamINfo, m_PsStreamInfoList);
	DDX_Control(pDX, IDC_COMBO_ps_selectwndid, m_PSwndidComboList);
}


BEGIN_MESSAGE_MAP(CDialogPushSteamTest, CDialogEx)
	ON_BN_CLICKED(IDOK, &CDialogPushSteamTest::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_del_psstream, &CDialogPushSteamTest::OnBnClickedButtondelpsstream)
	ON_BN_CLICKED(IDC_BUTTON_Create_PsStream, &CDialogPushSteamTest::OnBnClickedButtonCreatePsstream)
	ON_BN_CLICKED(IDC_BUTTON_StartPushStream, &CDialogPushSteamTest::OnBnClickedButtonStartpushstream)
	ON_BN_CLICKED(IDC_BUTTON_ps_stopstream, &CDialogPushSteamTest::OnBnClickedButtonpsstopstream)
	ON_BN_CLICKED(IDC_BUTTON_ps_wndshowstream, &CDialogPushSteamTest::OnBnClickedButtonpswndshowstream)
	ON_BN_CLICKED(IDC_BUTTON_ps_wndstop, &CDialogPushSteamTest::OnBnClickedButtonpswndstop)
	ON_BN_CLICKED(IDC_BUTTONpsstreamstat, &CDialogPushSteamTest::OnBnClickedButtonpsstreamstat)
END_MESSAGE_MAP()

BOOL CDialogPushSteamTest::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_PsStreamInfoList.InsertColumn(0,_T("句柄值"));
	m_PsStreamInfoList.SetColumnWidth(0, 70);
	m_PsStreamInfoList.InsertColumn(1,_T("参数1"));
	m_PsStreamInfoList.SetColumnWidth(1, 120);
	m_PsStreamInfoList.InsertColumn(2,_T("参数2"));
	m_PsStreamInfoList.SetColumnWidth(2, 80);
	m_PsStreamInfoList.InsertColumn(3,_T("参数3"));
	m_PsStreamInfoList.SetColumnWidth(3, 50);
	m_PsStreamInfoList.InsertColumn(4,_T("参数4"));
	m_PsStreamInfoList.SetColumnWidth(4, 70);

	m_PsStreamInfoList.SetExtendedStyle(LVS_EX_FLATSB|LVS_EX_FULLROWSELECT);
	map<int,int>::iterator iter = m_WndIdMapS.begin();
	for(;iter!=m_WndIdMapS.end();++iter)
	{
		TCHAR temp[256] = {0};
		sprintf(temp, "%d", iter->first);
		m_PSwndidComboList.AddString(temp);
	}

	m_PSwndidComboList.SetCurSel(0);

	return 0;
}
// CDialogPushSteamTest 消息处理程序


void CDialogPushSteamTest::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	CDialogEx::OnOK();
}

void CDialogPushSteamTest::OnBnClickedButtondelpsstream()
{
	POSITION pos = m_PsStreamInfoList.GetFirstSelectedItemPosition();
	long handle = 0;
	if (NULL != pos)
	{
		int nItem	  = m_PsStreamInfoList.GetNextSelectedItem(pos);
		CString strId = m_PsStreamInfoList.GetItemText(nItem, 0);

		handle = atol(strId.GetBuffer());
	}
	else
	{
		MessageBox(_T("请选择一个流"));
		return;
	}

	int res = nmc_delete_user_stream_source(handle);
	if(res)
	{
		MessageBox(_T("nmc_delete_user_stream_source failed"));
		return;
	}

	MessageBox(_T("删除成功"));
}

void CDialogPushSteamTest::OnBnClickedButtonCreatePsstream()
{
	long streamhandle = nmc_create_user_stream_source(m_Handle, NULL);
	if(streamhandle==0)
	{
		//创建失败
		MessageBox(_T("创建失败"));
		return;
	}

	int nItem = m_PsStreamInfoList.GetItemCount();

	char tmp[64] = {0};
	sprintf(tmp, "%ld", streamhandle);

	nItem = m_PsStreamInfoList.InsertItem(nItem, tmp);
	m_PsStreamInfoList.SetItemText(nItem, 0, tmp);

	struct st_user_stream_state *ppuser_stream_state=0;
	int pstatecnt = 0;
	int res = nmc_get_push_user_stream_state(m_Handle, streamhandle, &ppuser_stream_state, &pstatecnt);
	if(res==0)
	{
		m_PsStreamInfoList.SetItemText(nItem, 1, ppuser_stream_state[0].ip);

		sprintf(tmp, "%d", ppuser_stream_state[0].port);
		m_PsStreamInfoList.SetItemText(nItem, 2, tmp);
		
		if(ppuser_stream_state[0].state==1)
		{
			m_PsStreamInfoList.SetItemText(nItem, 3, _T("使用"));
		}
		else if(ppuser_stream_state[0].state==2)
		{
			m_PsStreamInfoList.SetItemText(nItem, 3, _T("推流中"));
		}

	}

	MessageBox(_T("创建成功"));
}
static unsigned int __stdcall Static_PushStreamThread(void *arg)
{
	struct stPushInfo *Info = (stPushInfo *)arg;

#if 1
	AVFormatContext*			m_pFormatContext1=NULL;
	if (0 != avformat_open_input(&m_pFormatContext1,Info->StreamFile.GetBuffer(),NULL,0))
	{
		//MessageBox(_T("avformat_open_input"));
		return 0;
	}

	if (av_find_stream_info(m_pFormatContext1) <0)
	{
		//MessageBox(_T("avformat_open_input"));
		return 0;
	}

	char codecName[MAX_PATH]		= {0};
	AVCodecContext  *pCodecCtx1		= NULL;
	for (unsigned int idx = 0;idx < m_pFormatContext1->nb_streams;++idx)
	{
		pCodecCtx1			= m_pFormatContext1->streams [idx]->codec;
		switch (m_pFormatContext1->streams [idx]->codec->codec_type)
		{
		case AVMEDIA_TYPE_VIDEO:
			{
				avcodec_string(codecName,MAX_PATH-1,pCodecCtx1,0);
				AVCodec* pVideoCodec = avcodec_find_decoder(pCodecCtx1->codec_id);
				if (NULL == pVideoCodec)
					break;

				AVStream* m_pVideoStream = m_pFormatContext1->streams [idx];
				if (AV_NOPTS_VALUE  == m_pVideoStream ->start_time )
					m_pVideoStream ->start_time  = 0;

				pCodecCtx1->time_base.den = 1;
				pCodecCtx1->time_base.num = 24;
			}
			break;
		}
	}

	AVPacket	readPacket1;
	ZeroMemory(&readPacket1,sizeof(AVPacket));

	if (NULL != readPacket1.data )
		av_free_packet(&readPacket1);

	struct st_video_stream_info info;
	info.len = sizeof(info);
	info.codec_type = 0x80;
	info.fps = 25;
	info.width = 1280;
	info.height = 720;//1024;//720;
	info.frametype = 0;
	info.bitrate = 2*1000*1000;
	info.res[0] = (int)Info->streamHandle;
	int res = nmc_set_user_stream_source_info(Info->streamHandle, &info);
	if(res)
	{
		return 0;
	}

	Sleep(2000);

	while(Info->Running)
	{
		int cnt = 0;
		res = av_read_frame(m_pFormatContext1, &readPacket1);
		while(res>=0 && Info->Running)
		{
			cnt++;
			char temp[256] = {0};
			//sprintf(temp, "readPacket1 p %p, len %d, flags %d, cnt %d, %x %x %x %x %x\n"
			//	, readPacket1.data, readPacket1.size, readPacket1.flags, cnt
			//	, readPacket1.data[0], readPacket1.data[1], readPacket1.data[2], readPacket1.data[3], readPacket1.data[4]);

			//请填写正确的i帧标识
			if(readPacket1.flags)
				info.frametype =  1;//1：i帧, 2：b帧, 3：p帧
			else
				info.frametype =  3;

			res = nmc_push_user_stream_video_data(Info->streamHandle, &info, (char*)readPacket1.data, readPacket1.size);
			if(res)
			{
				char temp[256] = {0};
				sprintf(temp, "nmc_push_user_stream_video_data failed res %d\n", res);
				OutputDebugString(temp);
				break;
			}

			if (NULL != readPacket1.data )
				av_free_packet(&readPacket1);

			OutputDebugString(temp);
			Sleep(40);

			res = av_read_frame(m_pFormatContext1, &readPacket1);
			if(res<0)
			{
				res = avformat_seek_file(m_pFormatContext1, 0, INT64_MIN, 0, INT64_MAX, AVSEEK_FLAG_BYTE );
				if(res<0)
				{
					//MessageBox(_T("avformat_seek_file"));
					break;
				}
				cnt = 0;

				char temp[256] = {0};
				sprintf(temp, "avformat_seek_file res %d\n", res);
				OutputDebugString(temp);
				break;
				res = av_read_frame(m_pFormatContext1, &readPacket1);
			}
		}
	}
#endif

	_endthread();

	return 0;
}

#if 0
static unsigned int __stdcall Static_PushStreamThread(void *arg)
{
	struct stPushInfo *Info = (stPushInfo *)arg;
	
#if 1
	AVFormatContext*			m_pFormatContext1=NULL;
	AVFormatContext*			m_pFormatContext2=NULL;
	if (0 != avformat_open_input(&m_pFormatContext1,Info->StreamFile.GetBuffer(),NULL,0))
	{
		//MessageBox(_T("avformat_open_input"));
		return 0;
	}

	if (av_find_stream_info(m_pFormatContext1) <0)
	{
		//MessageBox(_T("avformat_open_input"));
		return 0;
	}

	if (0 != avformat_open_input(&m_pFormatContext2,"C:\\Users\\tangjian\\Desktop\\raw1.bin",NULL,0))
	{
		//MessageBox(_T("avformat_open_input"));
		return 0;
	}

	if (av_find_stream_info(m_pFormatContext2) <0)
	{
		//MessageBox(_T("avformat_open_input"));
		return 0;
	}


	char codecName[MAX_PATH]		= {0};
	AVCodecContext  *pCodecCtx1		= NULL;
	for (unsigned int idx = 0;idx < m_pFormatContext1->nb_streams;++idx)
	{
		pCodecCtx1			= m_pFormatContext1->streams [idx]->codec;
		switch (m_pFormatContext1->streams [idx]->codec->codec_type)
		{
		case AVMEDIA_TYPE_VIDEO:
			{
				avcodec_string(codecName,MAX_PATH-1,pCodecCtx1,0);
				AVCodec* pVideoCodec = avcodec_find_decoder(pCodecCtx1->codec_id);
				if (NULL == pVideoCodec)
					break;

				AVStream* m_pVideoStream = m_pFormatContext1->streams [idx];
				if (AV_NOPTS_VALUE  == m_pVideoStream ->start_time )
					m_pVideoStream ->start_time  = 0;

				pCodecCtx1->time_base.den = 1;
				pCodecCtx1->time_base.num = 24;
			}
			break;
		}
	}

	AVCodecContext  *pCodecCtx2		= NULL;
	for (unsigned int idx = 0;idx < m_pFormatContext2->nb_streams;++idx)
	{
		pCodecCtx2			= m_pFormatContext2->streams [idx]->codec;
		switch (m_pFormatContext2->streams [idx]->codec->codec_type)
		{
		case AVMEDIA_TYPE_VIDEO:
			{
				avcodec_string(codecName,MAX_PATH-1,pCodecCtx2,0);
				AVCodec* pVideoCodec = avcodec_find_decoder(pCodecCtx2->codec_id);
				if (NULL == pVideoCodec)
					break;

				AVStream* m_pVideoStream = m_pFormatContext2->streams [idx];
				if (AV_NOPTS_VALUE  == m_pVideoStream ->start_time )
					m_pVideoStream ->start_time  = 0;

				pCodecCtx2->time_base.den = 1;
				pCodecCtx2->time_base.num = 24;
			}
			break;
		}
	}

	AVPacket	readPacket1;
	ZeroMemory(&readPacket1,sizeof(AVPacket));

	AVPacket	readPacket2;
	ZeroMemory(&readPacket2,sizeof(AVPacket));

	if (NULL != readPacket1.data )
		av_free_packet(&readPacket1);

	if (NULL != readPacket2.data )
		av_free_packet(&readPacket2);

	struct st_video_stream_info info;
	info.len = sizeof(info);
	info.codec_type = 0x80;
	info.fps = 25;
	info.width = 1280;
	info.height = 720;//1024;//720;
	info.frametype = 0;
	info.bitrate = 2*1000*1000;
	int res = nmc_set_user_stream_source_info(Info->streamHandle, &info);
	if(res)
	{
		//MessageBox(_T("nmc_set_user_stream_source_info"));
		return 0;
	}

	while(Info->Running)
	{
		int cnt = 0;
		res = av_read_frame(m_pFormatContext1, &readPacket1);
		while(res>=0 && Info->Running)
		{
			cnt++;
			char temp[256] = {0};
			sprintf(temp, "readPacket1 p %p, len %d, flags %d, cnt %d, %x %x %x %x %x\n"
				, readPacket1.data, readPacket1.size, readPacket1.flags, cnt
				, readPacket1.data[0], readPacket1.data[1], readPacket1.data[2], readPacket1.data[3], readPacket1.data[4]);

			if(readPacket1.flags)
				info.frametype =  1;//1：i帧, 2：b帧, 3：p帧
			else
				info.frametype =  3;

			res = nmc_push_user_stream_video_data(Info->streamHandle, &info, (char*)readPacket1.data, readPacket1.size);
			if(res)
			{
				char temp[256] = {0};
				sprintf(temp, "nmc_push_user_stream_video_data failed res %d\n", res);
				OutputDebugString(temp);
				break;
			}

			if (NULL != readPacket1.data )
				av_free_packet(&readPacket1);

			OutputDebugString(temp);
			Sleep(40);

			res = av_read_frame(m_pFormatContext1, &readPacket1);
			if(res<0)
			{
				res = avformat_seek_file(m_pFormatContext1, 0, INT64_MIN, 0, INT64_MAX, AVSEEK_FLAG_BYTE );
				if(res<0)
				{
					//MessageBox(_T("avformat_seek_file"));
					break;
				}
				cnt = 0;

				char temp[256] = {0};
				sprintf(temp, "avformat_seek_file res %d\n", res);
				OutputDebugString(temp);
				break;
				res = av_read_frame(m_pFormatContext1, &readPacket1);
			}
		}

		cnt = 0;
		res = av_read_frame(m_pFormatContext2, &readPacket2);
		while(res>=0 && Info->Running)
		{
			cnt++;
			char temp[256] = {0};
			sprintf(temp, "readPacket2 p %p, len %d, flags %d, cnt %d, %x %x %x %x %x\n"
				, readPacket2.data, readPacket2.size, readPacket2.flags, cnt
				, readPacket2.data[0], readPacket2.data[1], readPacket2.data[2], readPacket2.data[3], readPacket2.data[4]);
			info.frametype = readPacket2.flags;

			res = nmc_push_user_stream_video_data(Info->streamHandle, &info, (char*)readPacket2.data, readPacket2.size);
			if(res)
			{
				char temp[256] = {0};
				sprintf(temp, "nmc_push_user_stream_video_data failed res %d\n", res);
				OutputDebugString(temp);
				break;
			}

			if (NULL != readPacket2.data )
				av_free_packet(&readPacket2);

			OutputDebugString(temp);
			Sleep(40);

			res = av_read_frame(m_pFormatContext2, &readPacket2);
			if(res<0)
			{
				res = avformat_seek_file(m_pFormatContext2, 0, INT64_MIN, 0, INT64_MAX, AVSEEK_FLAG_BYTE );
				if(res<0)
				{
					//MessageBox(_T("avformat_seek_file"));
					break;
				}
				cnt = 0;

				char temp[256] = {0};
				sprintf(temp, "avformat_seek_file res %d\n", res);
				OutputDebugString(temp);
				break;
				res = av_read_frame(m_pFormatContext2, &readPacket2);
			}
		}
	}
#endif



	_endthread();

	return 0;
}
#endif

void CDialogPushSteamTest::OnBnClickedButtonStartpushstream()
{
	CString StreamFile;
	TCHAR szFile[8192] = {0};
	OPENFILENAME ofn; 		
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner	= m_hWnd;
	ofn.lpstrFile	= szFile;
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = _TEXT("All\0*.*\0");
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;

	ofn.Flags = OFN_READONLY|OFN_ALLOWMULTISELECT|OFN_EXPLORER; 
	if (GetOpenFileName(&ofn))
	{
		StreamFile = ofn.lpstrFile;
		UpdateData(FALSE);
	}

	if(StreamFile.GetLength()==0)
	{
		MessageBox(_T("没有文件"));
		return ;
	}

	POSITION pos = m_PsStreamInfoList.GetFirstSelectedItemPosition();
	long streamhandle = 0;
	if (NULL != pos)
	{
		int nItem	  = m_PsStreamInfoList.GetNextSelectedItem(pos);
		CString strId = m_PsStreamInfoList.GetItemText(nItem, 0);

		streamhandle = atol(strId.GetBuffer());
	}
	else
	{
		MessageBox(_T("请选择一个流"));
		return;
	}

	std::map<long,struct stPushInfo *>::iterator iter = m_StreamPullingS.find(streamhandle);
	if(iter==m_StreamPullingS.end())
	{
		struct stPushInfo *PushInfo = new stPushInfo;
		m_StreamPullingS[streamhandle] = PushInfo;
	}

	unsigned threadID;
	m_StreamPullingS[streamhandle]->streamHandle = streamhandle;
	m_StreamPullingS[streamhandle]->Running = 1;
	m_StreamPullingS[streamhandle]->StreamFile = StreamFile;
	m_StreamPullingS[streamhandle]->hPushStreamThread = (HANDLE)_beginthreadex(NULL, 0, Static_PushStreamThread, m_StreamPullingS[streamhandle], 0, &threadID);
	
	int res = nmc_start_push_user_stream(streamhandle);
	if(res)
	{
		TCHAR Msg[256] = {0};
		sprintf(Msg, _T("nmc_start_push_user_stream faied %d"), res);
		MessageBox(Msg);
	}

	return;
}


void CDialogPushSteamTest::OnBnClickedButtonpsstopstream()
{
	POSITION pos = m_PsStreamInfoList.GetFirstSelectedItemPosition();
	long streamhandle = 0;
	if (NULL != pos)
	{
		int nItem	  = m_PsStreamInfoList.GetNextSelectedItem(pos);
		CString strId = m_PsStreamInfoList.GetItemText(nItem, 0);

		streamhandle = atol(strId.GetBuffer());
	}
	else
	{
		MessageBox(_T("请选择一个流"));
		return;
	}
	
	m_StreamPullingS[streamhandle]->Running = 0;
	WaitForSingleObject(m_StreamPullingS[streamhandle]->hPushStreamThread, INFINITE);
	CloseHandle(m_StreamPullingS[streamhandle]->hPushStreamThread);

	int res = nmc_stop_push_user_stream(m_StreamPullingS[streamhandle]->streamHandle);
	if(res)
	{
		TCHAR Msg[256] = {0};
		sprintf(Msg, _T("nmc_stop_push_user_stream failed %d"), res);
		MessageBox(Msg);
	}

	m_StreamPullingS[streamhandle]->streamHandle = -1;
	m_StreamPullingS[streamhandle]->Running = 0;
	m_StreamPullingS[streamhandle]->StreamFile = "";
	m_StreamPullingS[streamhandle]->hPushStreamThread = INVALID_HANDLE_VALUE;

	//m_StreamPullingS.erase()
}

void CDialogPushSteamTest::OnBnClickedButtonpswndshowstream()
{
	int wndsel = m_PSwndidComboList.GetCurSel();
	if(wndsel==-1)
	{
		MessageBox(_T("选择一个窗口"));
		return ;	
	}

	CString strWndId;
	m_PSwndidComboList.GetLBText(m_PSwndidComboList.GetCurSel(), strWndId);
	int WndId= _ttoi(strWndId);

	POSITION pos = m_PsStreamInfoList.GetFirstSelectedItemPosition();
	long streamhandle = 0;
	if (NULL != pos)
	{
		int nItem	  = m_PsStreamInfoList.GetNextSelectedItem(pos);
		CString strId = m_PsStreamInfoList.GetItemText(nItem, 0);

		streamhandle = atol(strId.GetBuffer());
	}
	else
	{
		MessageBox(_T("请选择一个流"));
		return;
	}

	std::map<long,struct stPushInfo *>::iterator iter = m_StreamPullingS.find(streamhandle);
	if(iter==m_StreamPullingS.end())
	{
		MessageBox(_T("没有该流"));
		return;
	}

	int res = nmc_set_user_stream_source_to_window(streamhandle, 0, WndId);
	if(res)
	{
		TCHAR Msg[256] = {0};
		sprintf(Msg, _T("nmc_set_user_stream_source_to_window faied %d"), res);
		MessageBox(Msg);
	}

}


void CDialogPushSteamTest::OnBnClickedButtonpswndstop()
{
	int wndsel = m_PSwndidComboList.GetCurSel();
	if(wndsel==-1)
	{
		MessageBox(_T("选择一个窗口"));
		return ;	
	}

	CString strWndId;
	m_PSwndidComboList.GetLBText(m_PSwndidComboList.GetCurSel(), strWndId);
	int WndId= _ttoi(strWndId);

	int res = -1;
#if 0
	res = nmc_clear_window_signal_source(m_Handle, 0, 0, WndId);
#else
	POSITION pos = m_PsStreamInfoList.GetFirstSelectedItemPosition();
	long streamhandle = 0;
	if (NULL != pos)
	{
		int nItem	  = m_PsStreamInfoList.GetNextSelectedItem(pos);
		CString strId = m_PsStreamInfoList.GetItemText(nItem, 0);

		streamhandle = atol(strId.GetBuffer());
	}
	else
	{
		MessageBox(_T("请选择一个流"));
		return;
	}

	std::map<long,struct stPushInfo *>::iterator iter = m_StreamPullingS.find(streamhandle);
	if(iter==m_StreamPullingS.end())
	{
		MessageBox(_T("没有该流"));
		return;
	}

	res = nmc_clear_user_stream_source_from_window(streamhandle, 0, WndId);
#endif
	if(res)
	{
		MessageBox(_T("停止失败"));
		return;
	}
}


void CDialogPushSteamTest::OnBnClickedButtonpsstreamstat()
{
	struct st_user_stream_state *puser_stream_state = NULL;
	int statecnt = 0;
	int res = nmc_get_push_user_stream_state(m_Handle, 0, &puser_stream_state, &statecnt);


}
