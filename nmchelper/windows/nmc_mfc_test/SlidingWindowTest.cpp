// SlidingWindowTest.cpp : 实现文件
//

#include "stdafx.h"
#include "nmc_mfc_test.h"
#include "SlidingWindowTest.h"
#include "afxdialogex.h"
#include <set>

// CSlidingWindowTest 对话框

IMPLEMENT_DYNAMIC(CSlidingWindowTest, CDialogEx)

CSlidingWindowTest::CSlidingWindowTest(long handle)
	: CDialogEx(CSlidingWindowTest::IDD, NULL)
	, m_CSubWinWidth1(1920)
	, m_CSubWinWidth2(1920)
	, m_CSubWinWidth3(1920)
	, m_CSubWinWidth4(1920)

	, m_CSubWinHeight1(1080)
	, m_CSubWinHeight2(1080)
	, m_CSubWinHeight3(1080)
	, m_CSubWinHeight4(1080)

	, m_DisPlayX1(0)
	, m_DisPlayX2(960)
	, m_DisPlayX3(0)
	, m_DisPlayX4(960)

	, m_DisPlayY1(0)
	, m_DisPlayY2(0)
	, m_DisPlayY3(540)
	, m_DisPlayY4(540)

	, m_Width1(960)
	, m_Width2(960)
	, m_Width3(960)
	, m_Width4(960)

	, m_Height1(540)
	, m_Height2(540)
	, m_Height3(540)
	, m_Height4(540)

	, m_OutputStartX1(960)
	, m_OutputStartX2(0)
	, m_OutputStartX3(960)
	, m_OutputStartX4(0)

	, m_OutputStartY1(540)
	, m_OutputStartY2(540)
	, m_OutputStartY3(0)
	, m_OutputStartY4(0)
{
	m_handle = handle;
}
CSlidingWindowTest::CSlidingWindowTest(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSlidingWindowTest::IDD, pParent)
{

}
CSlidingWindowTest::~CSlidingWindowTest()
{
}

void CSlidingWindowTest::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_SlidingWinInfo, m_SWList);

	DDX_Control(pDX, IDC_COMBO_sw_output1, m_SwOutputid1);
	DDX_Control(pDX, IDC_COMBOsw_output2, m_SwOutputid2);
	DDX_Control(pDX, IDC_COMBOsw_output3, m_SwOutputid3);
	DDX_Control(pDX, IDC_COMBOsw_output4, m_SwOutputid4);

	DDX_Text(pDX, IDC_EDIT_sw_output_width1, m_CSubWinWidth1);
	DDX_Text(pDX, IDC_EDIT_sw_output_width2, m_CSubWinWidth2);
	DDX_Text(pDX, IDC_EDIT_sw_output_width3, m_CSubWinWidth3);
	DDX_Text(pDX, IDC_EDIT_sw_output_width4, m_CSubWinWidth4);

	DDX_Text(pDX, IDC_EDIT__sw_output_height1, m_CSubWinHeight1);
	DDX_Text(pDX, IDC_EDIT_sw_output_height2, m_CSubWinHeight2);
	DDX_Text(pDX, IDC_EDIT_sw_output_height3, m_CSubWinHeight3);
	DDX_Text(pDX, IDC_EDIT_sw_output_height4, m_CSubWinHeight4);

	DDX_Text(pDX, IDC_EDITstartx1, m_OutputStartX1);
	DDX_Text(pDX, IDC_EDITstartx2, m_OutputStartX2);
	DDX_Text(pDX, IDC_EDITstartx3, m_OutputStartX3);
	DDX_Text(pDX, IDC_EDITstartx4, m_OutputStartX4);

	DDX_Text(pDX, IDC_EDITstarty1, m_OutputStartY1 );
	DDX_Text(pDX, IDC_EDITstarty2, m_OutputStartY2);
	DDX_Text(pDX, IDC_EDITstarty3, m_OutputStartY3 );
	DDX_Text(pDX, IDC_EDITstarty4, m_OutputStartY4 );

	DDX_Text(pDX, IDC_EDIT_sw_width1, m_Width1);
	DDX_Text(pDX, IDC_EDIT_sw_width2, m_Width2);
	DDX_Text(pDX, IDC_EDIT_sw_width3, m_Width3);
	DDX_Text(pDX, IDC_EDIT_sw_width4, m_Width4);

	DDX_Text(pDX, IDC_EDIT_sw_height1, m_Height1);
	DDX_Text(pDX, IDC_sw_height2,      m_Height2);
	DDX_Text(pDX, IDC_EDIT_sw_height3, m_Height3);
	DDX_Text(pDX, IDC_EDIT_sw_height4, m_Height4);

	DDX_Text(pDX, IDC_EDIT_DisPlayX1, m_DisPlayX1 );
	DDX_Text(pDX, IDC_EDIT_DisPlayX2, m_DisPlayX2 );
	DDX_Text(pDX, IDC_EDIT_DisPlayX3, m_DisPlayX3 );
	DDX_Text(pDX, IDC_EDIT_DisPlayX4, m_DisPlayX4 );

	DDX_Text(pDX, IDC_EDIT_DisPlayY1, m_DisPlayY1);
	DDX_Text(pDX, IDC_DisPlayY2,      m_DisPlayY2);
	DDX_Text(pDX, IDC_EDIT_DisPlayY3, m_DisPlayY3);
	DDX_Text(pDX, IDC_EDIT_DisPlayY4, m_DisPlayY4);
}


BEGIN_MESSAGE_MAP(CSlidingWindowTest, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &CSlidingWindowTest::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON_StickSlidingWindow, &CSlidingWindowTest::OnBnClickedButtonStickslidingwindow)
	ON_BN_CLICKED(IDC_BUTTONSlidingWinInfo, &CSlidingWindowTest::OnBnClickedButtonslidingwininfo)
	ON_BN_CLICKED(IDC_BUTTON_DeleteSlidingWindow, &CSlidingWindowTest::OnBnClickedButtonDeleteslidingwindow)
	ON_BN_CLICKED(IDC_BUTTONCreateSlidingWindow, &CSlidingWindowTest::OnBnClickedButtoncreateslidingwindow)
END_MESSAGE_MAP()


// CSlidingWindowTest 消息处理程序
BOOL CSlidingWindowTest::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_SWList.InsertColumn(0,_T("窗口id"));
	m_SWList.SetColumnWidth(0, 70);
	m_SWList.InsertColumn(1,_T("窗口状态"));
	m_SWList.SetColumnWidth(1, 120);
	m_SWList.InsertColumn(2,_T("源id"));
	m_SWList.SetColumnWidth(2, 80);
	m_SWList.InsertColumn(3,_T("流id"));
	m_SWList.SetColumnWidth(3, 50);
	m_SWList.InsertColumn(4,_T("窗口类型"));
	m_SWList.SetColumnWidth(4, 70);

	m_SWList.SetExtendedStyle(LVS_EX_FLATSB|LVS_EX_FULLROWSELECT);
	
	struct st_output_layout *ppoutput_layout = NULL;
	int output_layout_cnt = 0;
	int res = nmc_get_output(m_handle, -1, &ppoutput_layout, &output_layout_cnt);

	for(int i=0; i<output_layout_cnt; ++i)
	{
		char tmp[64] = {0};
		sprintf(tmp, "%d", ppoutput_layout[i].id);
		m_SwOutputid1.AddString(tmp);
		m_SwOutputid2.AddString(tmp);
		m_SwOutputid3.AddString(tmp);
		m_SwOutputid4.AddString(tmp);
	}

	return 0;
}

void CSlidingWindowTest::OnBnClickedButton1()
{


}

void CSlidingWindowTest::OnBnClickedButtonStickslidingwindow()
{


}

void CSlidingWindowTest::OnBnClickedButtonslidingwininfo()
{
	struct st_sw_window_info* info = NULL;
	int info_cnt=0;
	int res = nmc_get_sw_windows_info(m_handle, -1, &info, &info_cnt);
	if(res)
	{
		MessageBox(_T("获取失败"));
		return;
	}

	m_SWList.DeleteAllItems();
	for(int i=0; i<info_cnt; ++i)
	{
		int nItem = m_SWList.GetItemCount();

		char tmp[64] = {0};
		sprintf(tmp, "%d", info[i].sw_window_id);

		nItem = m_SWList.InsertItem(nItem, tmp);
		m_SWList.SetItemText(nItem, 0, tmp);

		sprintf(tmp, "%d", info[i].sw_window_status);
		m_SWList.SetItemText(nItem, 1, tmp);

		sprintf(tmp, "%d", info[i].source_id);
		m_SWList.SetItemText(nItem, 2, tmp);

		sprintf(tmp, "%d", info[i].source_sub_id);
		m_SWList.SetItemText(nItem, 3, tmp);

		sprintf(tmp, "%d", 2);//漫游窗口都是2
		m_SWList.SetItemText(nItem, 4, tmp);
	}

	struct st_xy_user_data *puser_data = NULL;
	int user_data_cnt = 0;
	res = nmc_get_userdefinedata(m_handle, -1, &puser_data, &user_data_cnt);
	if(res)
	{
		MessageBox(_T("获取用户数据失败"));
		return;
	}

	if(puser_data&&user_data_cnt)
	{
		puser_data->id = 99;
		res = nmc_set_userdefinedata(m_handle, nmc_add, puser_data);
		if(res)
		{
			MessageBox(_T("获取用户数据失败"));
		}
	}

	nmc_free_userdefinedata(puser_data, user_data_cnt);
}


void CSlidingWindowTest::OnBnClickedButtonDeleteslidingwindow()
{
	//获取目前解码器上自带的所有信号源
	POSITION pos = m_SWList.GetFirstSelectedItemPosition();
	int id = 0;
	if (NULL != pos)
	{
		int nItem	  = m_SWList.GetNextSelectedItem(pos);
		CString strId = m_SWList.GetItemText(nItem, 0);

		id = atoi(strId.GetBuffer());
	}
	else
	{
		MessageBox(_T("请选择一个设备"));
		return;
	}

	int swid = id;
	int res = nmc_delete_sw_windows(m_handle, &swid, 1);
	if(res)
	{
		MessageBox(_T("删除失败"));
	}
	else
	{
		MessageBox(_T("成功"));
	}
}

void CSlidingWindowTest::OnBnClickedButtoncreateslidingwindow()
{
	UpdateData(TRUE);

	int CurSel1 = m_SwOutputid1.GetCurSel();
	int CurSel2 = m_SwOutputid2.GetCurSel();
	int CurSel3 = m_SwOutputid3.GetCurSel();
	int CurSel4 = m_SwOutputid4.GetCurSel();
	if(CurSel1==-1 || CurSel2==-1 || CurSel3==-1 || CurSel4==-1)
	{
		MessageBox(_T("填写输出id"));
		return;
	}

	std::set<int> Ids;


	//demo里的漫游窗口只在4个输出里
	struct st_create_sw_window_info sw_window_info[4];
	int info_cnt=4;

	//取得选中的值
	CString selStr;
	int nIndex = m_SwOutputid1.GetCurSel();
	m_SwOutputid1.GetLBText(nIndex,selStr);
	
	sw_window_info[0].outputid = atoi(selStr.GetBuffer());
	sw_window_info[0].sw_window_id = -1;
	sw_window_info[0].sw_sequence = -1;
	sw_window_info[0].output_start_x = m_OutputStartX1;
	sw_window_info[0].output_start_y = m_OutputStartY1;
	sw_window_info[0].display_x = m_DisPlayX1;
	sw_window_info[0].display_y = m_DisPlayY1;
	sw_window_info[0].width = m_Width1;
	sw_window_info[0].heigh = m_Height1;
	sw_window_info[0].sub_win_height = m_CSubWinHeight1 ;
	sw_window_info[0].sub_win_width = m_CSubWinWidth1;

	nIndex = m_SwOutputid2.GetCurSel();
	m_SwOutputid2.GetLBText(nIndex,selStr);

	sw_window_info[1].outputid = atoi(selStr.GetBuffer());
	sw_window_info[1].sw_window_id = -1;
	sw_window_info[1].sw_sequence = -1;
	sw_window_info[1].output_start_x = m_OutputStartX2;
	sw_window_info[1].output_start_y = m_OutputStartY2;
	sw_window_info[1].display_x = m_DisPlayX2;
	sw_window_info[1].display_y = m_DisPlayY2;
	sw_window_info[1].width = m_Width2;
	sw_window_info[1].heigh = m_Height2;
	sw_window_info[1].sub_win_height = m_CSubWinHeight2;
	sw_window_info[1].sub_win_width = m_CSubWinWidth2;

	nIndex = m_SwOutputid3.GetCurSel();
	m_SwOutputid3.GetLBText(nIndex,selStr);

	sw_window_info[2].outputid = atoi(selStr.GetBuffer());
	sw_window_info[2].sw_window_id = -1;
	sw_window_info[2].sw_sequence = -1;
	sw_window_info[2].output_start_x = m_OutputStartX3;
	sw_window_info[2].output_start_y = m_OutputStartY3;
	sw_window_info[2].display_x = m_DisPlayX3;
	sw_window_info[2].display_y = m_DisPlayY3;
	sw_window_info[2].width = m_Width3;
	sw_window_info[2].heigh = m_Height3;
	sw_window_info[2].sub_win_height = m_CSubWinHeight3;
	sw_window_info[2].sub_win_width = m_CSubWinWidth3;

	nIndex = m_SwOutputid4.GetCurSel();
	m_SwOutputid4.GetLBText(nIndex,selStr);

	sw_window_info[3].outputid = atoi(selStr.GetBuffer());
	sw_window_info[3].sw_window_id = -1;
	sw_window_info[3].sw_sequence = -1;
	sw_window_info[3].output_start_x = m_OutputStartX4;
	sw_window_info[3].output_start_y = m_OutputStartY4;
	sw_window_info[3].display_x = m_DisPlayX4;
	sw_window_info[3].display_y = m_DisPlayY4;
	sw_window_info[3].width = m_Width4;
	sw_window_info[3].heigh = m_Height4;
	sw_window_info[3].sub_win_height = m_CSubWinHeight4;
	sw_window_info[3].sub_win_width = m_CSubWinWidth4;

	Ids.insert(sw_window_info[0].outputid);
	Ids.insert(sw_window_info[1].outputid);
	Ids.insert(sw_window_info[2].outputid);
	Ids.insert(sw_window_info[3].outputid);
	
	if(Ids.size()<4)
	{
		MessageBox(_T("有相同参数"));
		return;
	}

	int res = nmc_create_sw_windows(m_handle, 1920, 1080, sw_window_info, info_cnt);
}