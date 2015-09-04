
// nmc_mfc_testDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "nmc_mfc_test.h"
#include "nmc_mfc_testDlg.h"
#include "afxdialogex.h"
#include "DeviceListDialog.h"
#include "SlidingWindowTest.h"//漫游窗口
#include "DialogSurveyTest.h"
#include "DialogPushSteamTest.h"
#include <vector>
#include <new>
using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

typedef enum _MAT_WND_TYPE
{
	WND_TYPE_SINGLE = 0,
	WND_TYPE_LARGE	= 1,
	WND_TYPE_SW		= 2
}MAT_WND_TYPE;
typedef enum _MAT_WND_STATUS
{
	WND_STATUS_NULL = 0,
	WND_STATUS_REAL,
	WND_STATUS_VOD,
	WND_STATUS_SINGLE_SURVEY,
	WND_STATUS_GROUP_SURVEY,
	WND_STATUS_GIS,
	WND_STATUS_ALARM
}MAT_WND_STATUS;

//多字节转为utf-8
void ConvertGBKToUtf8(std::string& amp, std::string strGBK)
{ 
	int len=MultiByteToWideChar(CP_ACP, 0, (LPCSTR)strGBK.c_str(), -1, NULL,0); 
	unsigned short * wszUtf8 = new unsigned short[len+1]; 
	memset(wszUtf8, 0, len * 2 + 2); 
	MultiByteToWideChar(CP_ACP, 0, (LPCSTR)strGBK.c_str(), -1, (LPWSTR)wszUtf8, len); 
	len = WideCharToMultiByte(CP_UTF8, 0, (LPCWSTR)wszUtf8, -1, NULL, 0, NULL, NULL); 
	char *szUtf8=new char[len + 1]; 
	memset(szUtf8, 0, len + 1); 
	WideCharToMultiByte (CP_UTF8, 0, (LPCWSTR)wszUtf8, -1, szUtf8, len, NULL,NULL); 
	//strGBK = szUtf8; 
	amp=szUtf8;
	delete[] szUtf8; 
	delete[] wszUtf8; 
}

int GetUtf8Doc(char** pDoc, int& nDocLen, const char* in)
{
	int nMaxTransLen = 2 * (strlen(in)+1);

	WCHAR *pTmpUnicode = new WCHAR[nMaxTransLen];
	memset(pTmpUnicode, 0, sizeof(WCHAR));

	int nUniLen = MultiByteToWideChar(CP_UTF8, 0, in, strlen(in)+1, pTmpUnicode, nMaxTransLen);

	CHAR* pTmpAnsi = new CHAR[nMaxTransLen];
	memset(pTmpAnsi, 0, sizeof(CHAR));

	int nAnsiLen = WideCharToMultiByte(CP_ACP, 0, pTmpUnicode, nUniLen, pTmpAnsi, nMaxTransLen, NULL,0); 

	*pDoc = pTmpAnsi;
	return 0;
}

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

Cnmc_mfc_testDlg::Cnmc_mfc_testDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(Cnmc_mfc_testDlg::IDD, pParent)
	, m_NmcPort(40000)
	, m_MncIp(_T("192.168.3.167"))
	, m_NmcUser(_T("admin"))
	, m_NmcPwd(_T("admin"))

	, m_NdmName(_T("NdmName"))
	, m_NdmId(-1)
	, m_NdmIp(_T("192.168.3.167"))
	, m_NdmPort(30000)

	, m_WndType(WND_TYPE_SINGLE)
	, m_WndId(0)
	, m_OutputId(0)
	, m_SourceId(0)
	, m_CreateLargeRow(2)
	, m_CreateLargeCol(2)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void Cnmc_mfc_testDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_port, m_NmcPort);
	DDX_Text(pDX, IDC_EDIT_ip, m_MncIp);
	DDX_Text(pDX, IDC_EDIT_user, m_NmcUser);
	DDX_Text(pDX, IDC_EDIT_pwd, m_NmcPwd);

	DDX_Text(pDX, IDC_EDIT_matrixid, m_NdmId);
	DDX_Text(pDX, IDC_EDIT_matrixip, m_NdmIp);
	DDX_Text(pDX, IDC_EDIT_matrixport, m_NdmPort);

	DDX_Control(pDX, IDC_LIST_matrix, m_MatrixList);
	DDX_Control(pDX, IDC_LIST_output, m_OutputList);
	DDX_Control(pDX, IDC_LIST_windows, m_WindowsList);

	DDX_Text(pDX, IDC_EDIT_wndtype, m_WndType);
	DDX_Text(pDX, IDC_EDIT_wndid, m_WndId);
	DDX_Text(pDX, IDC_EDIT_outputid, m_OutputId);
	DDX_Text(pDX, IDC_EDIT1_Sourceid, m_SourceId);
	DDX_Text(pDX, IDC_EDIT_Ndmname, m_NdmName);
	DDX_Control(pDX, IDC_COMBO_OPTYPE, m_MatrixOpTypeComb);
	DDX_Control(pDX, IDC_LIST_LARGE_SCREEN, m_LargeScreenList);
	DDX_Control(pDX, IDC_LIST_LARGE_SUB_SCREEN,m_LargeSubScreenList);
	DDX_Control(pDX, IDC_COMBO_LAYOUT, m_LayoutTypeComb);
	DDX_Text(pDX, IDC_EDIT_Create_largerow, m_CreateLargeRow);
	DDX_Text(pDX, IDC_EDIT_CreateLargecol, m_CreateLargeCol);
	DDX_Control(pDX, IDC_LIST_DeviceSearch, m_SearchDevcieList);
}

BEGIN_MESSAGE_MAP(Cnmc_mfc_testDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_Login, &Cnmc_mfc_testDlg::OnBnClickedButtonLogin)
	ON_BN_CLICKED(IDC_BUTTON_get_maxtrix, &Cnmc_mfc_testDlg::OnBnClickedButtongetmaxtrix)
	ON_BN_CLICKED(IDC_BUTTON_getoutput, &Cnmc_mfc_testDlg::OnBnClickedButtongetoutput)
	
	ON_BN_CLICKED(IDC_BUTTON_windowstop, &Cnmc_mfc_testDlg::OnBnClickedButtonwindowstop)
	ON_BN_CLICKED(IDC_BUTTON_playsource, &Cnmc_mfc_testDlg::OnBnClickedButtonplaysource)
	ON_BN_CLICKED(IDOK, &Cnmc_mfc_testDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_setmatrix, &Cnmc_mfc_testDlg::OnBnClickedButtonsetmatrix)
	ON_BN_CLICKED(IDC_BUTTON_SHOW_SCREEN_NUM, &Cnmc_mfc_testDlg::OnBnClickedButtonShowScreenNum)
	ON_BN_CLICKED(IDC_BUTTON_logou, &Cnmc_mfc_testDlg::OnBnClickedButtonlogou)
	ON_BN_CLICKED(IDC_BUTTON_GET_LARGE_SCREEN, &Cnmc_mfc_testDlg::OnBnClickedButtonGetLargeScreen)
	ON_BN_CLICKED(IDC_BUTTON_FILP, &Cnmc_mfc_testDlg::OnBnClickedButtonFilp)
	ON_BN_CLICKED(IDC_BUTTON_WND_FULL_SCREEN, &Cnmc_mfc_testDlg::OnBnClickedButtonWndFullScreen)
	ON_BN_CLICKED(IDC_BUTTON_DEL_LargeScreen, &Cnmc_mfc_testDlg::OnBnClickedButtonDelLargescreen)
	ON_BN_CLICKED(IDC_BUTTON_Create_large_Screen, &Cnmc_mfc_testDlg::OnBnClickedButtonCreatelargeScreen)
	ON_BN_CLICKED(IDC_BUTTON_Setlayout, &Cnmc_mfc_testDlg::OnBnClickedButtonSetlayout)
	ON_BN_CLICKED(IDC_BUTTON_SOURCE_NAME, &Cnmc_mfc_testDlg::OnBnClickedButtonSourceName)
//	ON_NOTIFY(HDN_ITEMCLICK, 0, &Cnmc_mfc_testDlg::OnHdnItemclickListwindows)
//	ON_NOTIFY(LVN_COLUMNCLICK, IDC_LIST_windows, &Cnmc_mfc_testDlg::OnLvnColumnclickListwindows)
	ON_NOTIFY(NM_CLICK, IDC_LIST_windows, &Cnmc_mfc_testDlg::OnNMClickListwindows)
	ON_NOTIFY(NM_CLICK, IDC_LIST_LARGE_SCREEN, &Cnmc_mfc_testDlg::OnNMClickListLargeScreen)
	/////ON_NOTIFY(NM_CLICK, IDC_LIST_sourcelist, &Cnmc_mfc_testDlg::OnNMClickListsourcelist)
	ON_BN_CLICKED(IDC_BUTTON_SIGNAL_OPERATOR, &Cnmc_mfc_testDlg::OnBnClickedButtonSignalOperator)
	ON_BN_CLICKED(IDC_BUTTON_SW_operator, &Cnmc_mfc_testDlg::OnBnClickedButtonSwoperator)
	ON_BN_CLICKED(IDC_BUTTON_Survey_Operate, &Cnmc_mfc_testDlg::OnBnClickedButtonSurveyOperate)
	ON_BN_CLICKED(IDC_BUTTON_SearchDevcie, &Cnmc_mfc_testDlg::OnBnClickedButtonSearchdevcie)
	ON_BN_CLICKED(IDC_BUTTON_Push_Stream_Test, &Cnmc_mfc_testDlg::OnBnClickedButtonPushStreamTest)
END_MESSAGE_MAP()


// Cnmc_mfc_testDlg 消息处理程序

int NMC_CALL_TYPE Cnmc_mfc_testDlg::static_NmcStatusCallback(long handle, int type, void* data, int datalen, void *userdata)
{
	Cnmc_mfc_testDlg *Dlg = (Cnmc_mfc_testDlg *)userdata;
	if(Dlg)
	{
		Dlg->NmcStatusCallback(handle, type, data, datalen);
	}
	
	return 0;
}
int Cnmc_mfc_testDlg::NmcStatusCallback(long handle, int type, void* data, int datalen)
{
	if(NMC_EVENT_DISCONNECT==type)
	{
		MessageBox(_T("nmc连接断开"));
		nmc_logout(handle);
	}
	else if(NDM_EVENT_DISCONNECT==type)
	{
		int MatrixId = *((int*)data);

		//MessageBox(_T("解码器断开"));
	}
	else if(NDM_EVENT_CONNECTED==type)
	{
		int MatrixId = *((int*)data);
		//MessageBox(_T("解码器连接"));
	}
	else if(NDM_GET_SEARCH_DEVCIE==type)//搜到设备
	{
		struct st_xy_device_info *device_info = (struct st_xy_device_info *)data;

		int nItem = m_SearchDevcieList.GetItemCount();
		nItem = m_SearchDevcieList.InsertItem(nItem, device_info->name);
		m_SearchDevcieList.SetItemText(nItem, 0, device_info->name);
		m_SearchDevcieList.SetItemText(nItem, 1, device_info->ipv4);
		m_SearchDevcieList.SetItemText(nItem, 2, "");
		m_SearchDevcieList.SetItemText(nItem, 3, device_info->mask);
		m_SearchDevcieList.SetItemText(nItem, 4, device_info->gateway);
	}
	else if(NMC_PUSH_STREAM_DISCONNECT==type)
	{
		MessageBox(_T("推流断开"));
	}
	return 0;
}

BOOL Cnmc_mfc_testDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	m_MatrixList.InsertColumn(0,_T("解码器地址"));
	m_MatrixList.SetColumnWidth(0, 120);
	m_MatrixList.InsertColumn(1,_T("解码器id"));
	m_MatrixList.SetColumnWidth(1, 70);
	m_MatrixList.InsertColumn(2,_T("解码器状态"));
	m_MatrixList.SetColumnWidth(2, 80);
	m_MatrixList.InsertColumn(3,_T("输出数"));
	m_MatrixList.SetColumnWidth(3, 50);
	m_MatrixList.InsertColumn(4,_T("解码器名称"));
	m_MatrixList.SetColumnWidth(4, 80);
	m_MatrixList.SetExtendedStyle(LVS_EX_FLATSB|LVS_EX_FULLROWSELECT);

	m_OutputList.InsertColumn(0,_T("输出id"));
	m_OutputList.SetColumnWidth(0, 50);
	m_OutputList.InsertColumn(1,_T("布局"));
	m_OutputList.SetColumnWidth(1, 40);
	m_OutputList.InsertColumn(2,_T("窗口数"));
	m_OutputList.SetColumnWidth(2, 50);
	m_OutputList.InsertColumn(3,_T("编号"));
	m_OutputList.SetColumnWidth(3, 60);

	m_OutputList.SetExtendedStyle(LVS_EX_FLATSB|LVS_EX_FULLROWSELECT);

	m_WindowsList.InsertColumn(0,_T("窗口id"));
	m_WindowsList.SetColumnWidth(0, 50);
	m_WindowsList.InsertColumn(1,_T("窗口状态"));
	m_WindowsList.SetColumnWidth(1, 60);
	m_WindowsList.InsertColumn(2,_T("窗口索引"));
	m_WindowsList.SetColumnWidth(2, 60);
	m_WindowsList.InsertColumn(3,_T("所属输出"));
	m_WindowsList.SetColumnWidth(3, 60);
	m_WindowsList.InsertColumn(4,_T("窗口类型"));
	m_WindowsList.SetColumnWidth(4, 60);
	m_WindowsList.InsertColumn(5,_T("源id"));
	m_WindowsList.SetColumnWidth(5, 40);
	m_WindowsList.InsertColumn(6,_T("源子id"));
	m_WindowsList.SetColumnWidth(6, 50);
	m_WindowsList.SetExtendedStyle(LVS_EX_FLATSB|LVS_EX_FULLROWSELECT);

	m_LargeScreenList.InsertColumn(0,_T("拼接屏id"));
	m_LargeScreenList.SetColumnWidth(0, 60);
	m_LargeScreenList.InsertColumn(1,_T("状态"));
	m_LargeScreenList.SetColumnWidth(1, 60);
	m_LargeScreenList.InsertColumn(2,_T("名称"));
	m_LargeScreenList.SetColumnWidth(2, 60);
	m_LargeScreenList.InsertColumn(3,_T("行"));
	m_LargeScreenList.SetColumnWidth(3, 60);
	m_LargeScreenList.InsertColumn(4,_T("列"));
	m_LargeScreenList.SetColumnWidth(4, 60);
	m_LargeScreenList.InsertColumn(5,_T("信源id"));
	m_LargeScreenList.SetColumnWidth(5, 60);
	m_LargeScreenList.InsertColumn(6,_T("信源子id"));
	m_LargeScreenList.SetColumnWidth(6, 60);
	m_LargeScreenList.InsertColumn(7,_T("窗口类型"));
	m_LargeScreenList.SetColumnWidth(7, 60);

	m_LargeScreenList.SetExtendedStyle(LVS_EX_FLATSB|LVS_EX_FULLROWSELECT);

	m_LargeSubScreenList.InsertColumn(0,_T("拼接子屏id"));
	m_LargeSubScreenList.SetColumnWidth(0, 80);
	m_LargeSubScreenList.InsertColumn(1,_T("所属拼接屏id"));
	m_LargeSubScreenList.SetColumnWidth(1, 90);
	m_LargeSubScreenList.InsertColumn(2,_T("所在行"));
	m_LargeSubScreenList.SetColumnWidth(2, 60);
	m_LargeSubScreenList.InsertColumn(3,_T("所在列"));
	m_LargeSubScreenList.SetColumnWidth(3, 60);
	m_LargeSubScreenList.SetExtendedStyle(LVS_EX_FLATSB|LVS_EX_FULLROWSELECT);

	m_MatrixOpTypeComb.AddString(_T("添加"));//0
	m_MatrixOpTypeComb.AddString(_T("修改"));//1
	m_MatrixOpTypeComb.AddString(_T("删除"));//2
	m_MatrixOpTypeComb.SetCurSel(0);

	m_LayoutTypeComb.AddString(_T("1分屏"));
	m_LayoutTypeComb.AddString(_T("2分屏"));
	m_LayoutTypeComb.AddString(_T("4分屏"));
	m_LayoutTypeComb.AddString(_T("6分屏"));
	m_LayoutTypeComb.AddString(_T("9分屏"));
	m_LayoutTypeComb.SetCurSel(0);
	//..........

	m_SearchDevcieList.InsertColumn(0,_T("name"));
	m_SearchDevcieList.SetColumnWidth(0, 80);
	m_SearchDevcieList.InsertColumn(1,_T("ip"));
	m_SearchDevcieList.SetColumnWidth(1, 120);
	m_SearchDevcieList.InsertColumn(2,_T("iface"));
	m_SearchDevcieList.SetColumnWidth(2, 60);
	m_SearchDevcieList.InsertColumn(3,_T("mask"));
	m_SearchDevcieList.SetColumnWidth(3, 60);
	m_SearchDevcieList.InsertColumn(4,_T("gateway"));
	m_SearchDevcieList.SetColumnWidth(4, 60);
	m_SearchDevcieList.SetExtendedStyle(LVS_EX_FLATSB|LVS_EX_FULLROWSELECT);
	
	nmc_init(static_NmcStatusCallback, this);
	
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void Cnmc_mfc_testDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void Cnmc_mfc_testDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR Cnmc_mfc_testDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void Cnmc_mfc_testDlg::OnBnClickedButtonLogin()
{
	UpdateData(TRUE);

	struct login_info info;
	memset(&info, 0, sizeof(info));
	strcpy(info.ip, m_MncIp.GetBuffer());
	info.port = m_NmcPort;
	strcpy(info.user, m_NmcUser.GetBuffer());
	strcpy(info.password, m_NmcPwd.GetBuffer());

	m_handle = nmc_login(&info);
	if(m_handle==-1L)
	{
		MessageBox(_T("登陆失败"));
		printf("nmc_login failed!\n");
		return;
	}

	MessageBox(_T("登陆成功"));
}


void Cnmc_mfc_testDlg::OnBnClickedButtonsetmatrix()
{
	UpdateData(TRUE);

	enum en_nmc_operator_type operator_type;
	struct st_matrix_info matrix_info;

	matrix_info.id = m_NdmId;
	strcpy(matrix_info.ip, m_NdmIp.GetBuffer());
	matrix_info.port = m_NdmPort;

	strcpy(matrix_info.name, m_NdmName.GetBuffer());

	strcpy(matrix_info.user, "admin");
	strcpy(matrix_info.password, "admin");

	int MatrixOpTypeComb = m_MatrixOpTypeComb.GetCurSel();

	operator_type = (enum en_nmc_operator_type)MatrixOpTypeComb;

	int res = nmc_set_matrix(m_handle, operator_type, &matrix_info);
	if(res)
	{
		MessageBox(_T("nmc_set_matrix failed"));
	}
	else
	{
		MessageBox(_T("nmc_set_matrix success"));
	}
}

void Cnmc_mfc_testDlg::OnBnClickedButtongetmaxtrix()
{
	struct st_matrix_info *pmatrix_info = NULL;
	int matrix_info_cnt = 0;

	m_MatrixList.DeleteAllItems();

	//-1 获取所有的解码器
	int res = nmc_get_matrix(m_handle, -1, &pmatrix_info, &matrix_info_cnt);
	if(res)
	{
		MessageBox(_T("nmc_get_matrix failed"));
		return ;
	}

	if(pmatrix_info==NULL || matrix_info_cnt==0)
	{
		//MessageBox(_T("nmc_get_matrix null"));
		return ;
	}

	for(int i=0; i<matrix_info_cnt; ++i)
	{
		int nItem = m_MatrixList.GetItemCount();
		nItem = m_MatrixList.InsertItem(nItem, pmatrix_info[i].ip);
		m_MatrixList.SetItemText(nItem, 0, pmatrix_info[i].ip);
		char tmp[64] = {0};
		sprintf(tmp, "%d", pmatrix_info[i].id);
		m_MatrixList.SetItemText(nItem, 1, tmp);

		sprintf(tmp, "%d", pmatrix_info[i].status);
		m_MatrixList.SetItemText(nItem, 2, tmp);

		sprintf(tmp, "%d", pmatrix_info[i].output_info_cnt);
		m_MatrixList.SetItemText(nItem, 3, tmp);

		char* pDoc=NULL;
		int nDocLen = 0;
		GetUtf8Doc(&pDoc, nDocLen, pmatrix_info[i].name);
		m_MatrixList.SetItemText(nItem, 4, pDoc);
	}

	for(int i=0; i<pmatrix_info->output_info_cnt; ++i)
	{
		struct st_output_info*  output_info = pmatrix_info->output_info+i;
		printf("id %d, index %d, hasdisplay %d, resolution %s\n", output_info->id, output_info->index, output_info->hasdisplay, output_info->resolution);
		m_IdMapS[output_info->id] = output_info->index;
	}

	nmc_free_matrix_info(pmatrix_info, matrix_info_cnt);
}

void Cnmc_mfc_testDlg::OnBnClickedButtongetoutput()
{
	struct st_output_layout *ppoutput_layout = NULL;
	int output_layout_cnt = 0;

	m_WndIdMapS.clear();

	//由nmc_get_output得到的都是单窗口类型的
	int res = nmc_get_output(m_handle, -1, &ppoutput_layout, &output_layout_cnt);
	if(res)
	{
		MessageBox(_T("nmc_get_output failed"));
		printf("nmc_get_output failed!\n");
		return ;
	}

	m_OutputList.DeleteAllItems();
	m_WindowsList.DeleteAllItems();
	for(int i=0; i<output_layout_cnt; ++i)
	{
		int nItem = m_OutputList.GetItemCount();

		char tmp[64] = {0};
		sprintf(tmp, "%d", ppoutput_layout[i].id);
		nItem = m_OutputList.InsertItem(nItem, tmp);

		m_OutputList.SetItemText(nItem, 0, tmp);
		
		sprintf(tmp, "%d", ppoutput_layout[i].layout);
		m_OutputList.SetItemText(nItem, 1, tmp);

		sprintf(tmp, "%d", ppoutput_layout[i].window_info_cnt);
		m_OutputList.SetItemText(nItem, 2, tmp);

		int OutputIndex = 1;
		std::map<int,int>::iterator iter = m_IdMapS.find(ppoutput_layout[i].id);
		if(iter!=m_IdMapS.end())
		{
			OutputIndex = iter->second;
		}

		sprintf(tmp, "%d", OutputIndex);
		m_OutputList.SetItemText(nItem, 3, tmp);

		struct st_window_info* wndinfo = ppoutput_layout[i].window_info;
		int window_info_cnt = ppoutput_layout[i].window_info_cnt;
		for(int j=0; j<window_info_cnt; ++j)
		{
			int nItem = m_WindowsList.GetItemCount();

			char tmp[64] = {0};
			sprintf(tmp, "%d", wndinfo[j].id);

			nItem = m_WindowsList.InsertItem(nItem, tmp);
			m_WindowsList.SetItemText(nItem, 0, tmp);
			
			sprintf(tmp, "%d", wndinfo[j].status);
			m_WindowsList.SetItemText(nItem, 1, tmp);

			sprintf(tmp, "%d", wndinfo[j].index);
			m_WindowsList.SetItemText(nItem, 2, tmp);

			sprintf(tmp, "%d", ppoutput_layout[i].id);
			m_WindowsList.SetItemText(nItem, 3, tmp);

			m_WindowsList.SetItemText(nItem, 4, "0");//由nmc_get_output得到的都是单窗口类型的

			sprintf(tmp, "%d", wndinfo[j].sourceid);
			m_WindowsList.SetItemText(nItem, 5, tmp);//由nmc_get_output得到的都是单窗口类型的

			sprintf(tmp, "%d", wndinfo[j].subsourceid);
			m_WindowsList.SetItemText(nItem, 6, tmp);//由nmc_get_output得到的都是单窗口类型的

			m_WndIdMapS[wndinfo[j].id] = wndinfo[j].index;
		}		
	}
	
	nmc_free_output_info(ppoutput_layout, output_layout_cnt);
}

void Cnmc_mfc_testDlg::OnBnClickedButtonplaysource()
{
	UpdateData(TRUE);

	if(m_WndId==0 || m_SourceId==0)
	{
		MessageBox(_T("参数错误"));
		return;
	}
	
	int res = nmc_set_window_signal_source(m_handle, m_WndType, m_WndId, 1, m_SourceId);
	if(res)
	{
		MessageBox(_T("nmc_rmv_signal_source failed"));
		printf("nmc_rmv_signal_source failed!\n");
	}

}

void Cnmc_mfc_testDlg::OnBnClickedButtonwindowstop()
{
	UpdateData(TRUE);
	int res = nmc_clear_window_signal_source(m_handle, m_WndType, m_OutputId, m_WndId);
	if(res)
	{
		MessageBox(_T("nmc_clear_window_signal_source failed"));
		printf("nmc_rmv_signal_source failed!\n");
	}
}


void Cnmc_mfc_testDlg::OnBnClickedOk()
{
	nmc_logout(m_handle);

	nmc_uninit(NULL);

	CDialogEx::OnOK();
}


void Cnmc_mfc_testDlg::OnBnClickedButtonShowScreenNum()
{
	//显示屏幕窗口编号
	int nItem = -1;
	static int sstype = 0;

	POSITION pos = m_MatrixList.GetFirstSelectedItemPosition();
	if (NULL != pos)
	{
		nItem		= m_MatrixList.GetNextSelectedItem(pos);
		CString strId = m_MatrixList.GetItemText(nItem, 1);
		
		int matrixid = atoi(strId.GetBuffer());
		int res = nmc_show_screen_num(m_handle, matrixid, sstype);
		if(res)
		{
			MessageBox(_T("nmc_show_screen_num failed"));
			printf("nmc_show_screen_num failed!\n");
		}
		if(sstype==0)
		{
			sstype=1;//显示背景图
		}
		else 
		{
			sstype=0;//显示编号
		}
	}	
}


void Cnmc_mfc_testDlg::OnBnClickedButtonlogou()
{
	// TODO: 在此添加控件通知处理程序代码
	nmc_logout(m_handle);
}


void Cnmc_mfc_testDlg::OnBnClickedButtonGetLargeScreen()
{
	//获取多解码器输出拼接方式
	int screenid=-1;
	struct st_large_screen_info *pplarge_screen_info=NULL;
	int large_screen_info_cnt=0;
	int res = nmc_get_large_screen(m_handle, screenid, &pplarge_screen_info, &large_screen_info_cnt);
	if(res)
	{
		MessageBox(_T("nmc_get_large_screen failed"));
		printf("nmc_get_large_screen failed!\n");
	}

	m_LargeScreenList.DeleteAllItems();
	m_LargeSubScreenList.DeleteAllItems();
	for(int i=0; i<large_screen_info_cnt; ++i)
	{
		int nItem = m_LargeScreenList.GetItemCount();

		char tmp[64] = {0};
		sprintf(tmp, "%d", pplarge_screen_info[i].screenid);

		nItem = m_LargeScreenList.InsertItem(nItem, tmp);
		m_LargeScreenList.SetItemText(nItem, 0, tmp);

		sprintf(tmp, "%d", pplarge_screen_info[i].screenstatus);
		m_LargeScreenList.SetItemText(nItem, 1, tmp);

		m_LargeScreenList.SetItemText(nItem, 2, pplarge_screen_info[i].screenname);

		sprintf(tmp, "%d", pplarge_screen_info[i].rows);
		m_LargeScreenList.SetItemText(nItem, 3, tmp);

		sprintf(tmp, "%d", pplarge_screen_info[i].cols);
		m_LargeScreenList.SetItemText(nItem, 4, tmp);

		sprintf(tmp, "%d", pplarge_screen_info[i].sourceid);
		m_LargeScreenList.SetItemText(nItem, 5, tmp);

		sprintf(tmp, "%d", pplarge_screen_info[i].subsourceid);
		m_LargeScreenList.SetItemText(nItem, 6, tmp);

		m_LargeScreenList.SetItemText(nItem, 7, "1");//拼接的窗口类型为1

		for(int j=0; j<pplarge_screen_info[i].subscreen_info_cnt; ++j)
		{
			int nItem = m_LargeSubScreenList.GetItemCount();

			char tmp[64] = {0};
			sprintf(tmp, "%d", pplarge_screen_info[i].psubscreen_info[j].OutputId);

			nItem = m_LargeSubScreenList.InsertItem(nItem, tmp);
			m_LargeSubScreenList.SetItemText(nItem, 0, tmp);

			sprintf(tmp, "%d", pplarge_screen_info[i].screenid);
			m_LargeSubScreenList.SetItemText(nItem, 1, tmp);

			sprintf(tmp, "%d", pplarge_screen_info[i].psubscreen_info[j].Row);
			m_LargeSubScreenList.SetItemText(nItem, 2, tmp);

			sprintf(tmp, "%d", pplarge_screen_info[i].psubscreen_info[j].Col);
			m_LargeSubScreenList.SetItemText(nItem, 3, tmp);
		}
	}

	nmc_free_large_screen_info(pplarge_screen_info, large_screen_info_cnt);
}

void Cnmc_mfc_testDlg::OnBnClickedButtonFilp()
{
	//解码窗口画面翻转
	POSITION pos = m_WindowsList.GetFirstSelectedItemPosition();
	if (NULL != pos)
	{
		int nItem = -1;
		nItem		= m_WindowsList.GetNextSelectedItem(pos);
		CString strId = m_WindowsList.GetItemText(nItem, 0);
		int wndid = atoi(strId.GetBuffer());

		CString strType = m_WindowsList.GetItemText(nItem, 4);
		int wndtype = atoi(strType.GetBuffer());

		int res = nmc_flip_window(m_handle, wndtype, wndid);
		if(res)
		{
			MessageBox(_T("nmc_flip_window failed"));
			printf("nmc_flip_window failed!\n");
		}
	}
}

void Cnmc_mfc_testDlg::OnBnClickedButtonWndFullScreen()
{
	//解码窗口画面全屏显示
	/*
	static int isfullscreen = 0;

	POSITION pos = m_WindowsList.GetFirstSelectedItemPosition();
	if (NULL != pos)
	{
		int nItem = -1;
		nItem		= m_WindowsList.GetNextSelectedItem(pos);
		CString strId = m_WindowsList.GetItemText(nItem, 0);
		int wndid = atoi(strId.GetBuffer());

		CString strType = m_WindowsList.GetItemText(nItem, 4);
		int wndtype = atoi(strType.GetBuffer());

		int res = nmc_fullscreen_window(m_handle, isfullscreen, wndtype, wndid);
		if(res)
		{
			MessageBox(_T("nmc_fullscreen_window failed"));
			printf("nmc_fullscreen_window failed!\n");
		}

		isfullscreen = !isfullscreen;
	}
	*/
}


void Cnmc_mfc_testDlg::OnBnClickedButtonDelLargescreen()
{
	int nItem = -1;

	POSITION pos = m_LargeScreenList.GetFirstSelectedItemPosition();
	if (NULL != pos)
	{
		nItem		= m_LargeScreenList.GetNextSelectedItem(pos);
		CString strId = m_LargeScreenList.GetItemText(nItem, 0);

		int screenid = atoi(strId.GetBuffer());
		int res = nmc_rmv_large_screen(m_handle, screenid);
		if(res)
		{
			MessageBox(_T("nmc_rmv_large_screen failed"));
		}
		else
		{
			MessageBox(_T("nmc_rmv_large_screen ok"));
		}
	}
}


void Cnmc_mfc_testDlg::OnBnClickedButtonCreatelargeScreen()
{
	UpdateData(TRUE);

	struct st_large_screen_info large_screen_info;
	large_screen_info.screenid = -1;
	large_screen_info.sourceid = -1;
	large_screen_info.subsourceid = -1;

	sprintf(large_screen_info.screenname, "%dX%d", m_CreateLargeRow, m_CreateLargeCol);
	
	large_screen_info.psubscreen_info = NULL;
	large_screen_info.subscreen_info_cnt = 0;
	large_screen_info.rows = m_CreateLargeRow;
	large_screen_info.cols = m_CreateLargeCol;

	vector<CString> OutputIdS;
	for(int i=0; i<m_OutputList.GetItemCount(); i++)
	{
		if(m_OutputList.GetItemState(i, LVIS_SELECTED) == LVIS_SELECTED)
		{
			CString strId = m_OutputList.GetItemText(i, 0);
			OutputIdS.push_back(strId);
		}
	}

	large_screen_info.subscreen_info_cnt = OutputIdS.size();
	if(large_screen_info.subscreen_info_cnt==0 || m_CreateLargeRow*m_CreateLargeCol != large_screen_info.subscreen_info_cnt)
	{
		MessageBox(_T("拼接数量错误"));
		return;
	}

	large_screen_info.psubscreen_info = (struct st_subscreen_info *)calloc(large_screen_info.subscreen_info_cnt, sizeof(struct st_subscreen_info));
	for(int n=0; n<m_CreateLargeRow; ++n)
	{
		for(int m=0; m<m_CreateLargeRow; ++m)
		{
			large_screen_info.psubscreen_info[m_CreateLargeRow * n + m].OutputId = atoi(OutputIdS[m_CreateLargeRow * n + m].GetBuffer());
			large_screen_info.psubscreen_info[m_CreateLargeRow * n + m].Row = n;
			large_screen_info.psubscreen_info[m_CreateLargeRow * n + m].Col = m;
		}
	}
	
	int res = nmc_add_large_screen(m_handle, &large_screen_info);
	if(res)
	{
		MessageBox(_T("nmc_add_large_screen failed"));
		printf("nmc_rmv_large_screen failed!\n");
	}
	else
	{
		MessageBox(_T("nmc_add_large_screen ok"));
		OnBnClickedButtonGetLargeScreen();
	}

	free(large_screen_info.psubscreen_info);
}


void Cnmc_mfc_testDlg::OnBnClickedButtonSetlayout()
{
	struct st_output_layout output_layout;

	POSITION pos = m_OutputList.GetFirstSelectedItemPosition();
	if (NULL != pos)
	{
		int nItem		= m_OutputList.GetNextSelectedItem(pos);
		CString strId = m_OutputList.GetItemText(nItem, 0);

		output_layout.id = atoi(strId.GetBuffer());
	}

	int CurSel = m_LayoutTypeComb.GetCurSel();
	if(CurSel==0)
		output_layout.layout = 1;
	else if(CurSel==1)
		output_layout.layout = 2;
	else if(CurSel==2)
		output_layout.layout = 4;
	else if(CurSel==3)
		output_layout.layout = 6;
	else if(CurSel==4)
		output_layout.layout = 9;
	else 
		return;

	output_layout.window_info = NULL;
	output_layout.window_info_cnt  = 0;

	//设置分屏
	int res = nmc_set_output(m_handle, &output_layout);
	if(res)
	{
		MessageBox(_T("nmc_set_output failed"));
	}
	else
	{
		MessageBox(_T("nmc_set_output success"));
	}
}


void Cnmc_mfc_testDlg::OnBnClickedButtonSourceName()
{
	/*
	int nItem = -1;
	
	POSITION pos = m_WindowsList.GetFirstSelectedItemPosition();
	if (NULL != pos)
	{
		nItem		= m_WindowsList.GetNextSelectedItem(pos);
		CString strId = m_WindowsList.GetItemText(nItem, 0);

		int wndid = atoi(strId.GetBuffer());
		int wndyype = 0;
		static int isshow=0;
		isshow = !isshow;
		//0x0001-摄像头名称、0x0002-每秒帧数、0x0004-码流
		int res = nmc_set_window_overlay_info(m_handle, isshow,  wndyype, wndid, NMC_SHOW_IPC_NAME);
		if(res)
		{
			MessageBox(_T("nmc_set_window_overlay_info failed"));
		}
		else
		{
			MessageBox(_T("nmc_set_window_overlay_info ok"));
		}
	}
	*/
}

void Cnmc_mfc_testDlg::OnNMClickListwindows(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	*pResult = 0;

	POSITION pos = m_WindowsList.GetFirstSelectedItemPosition();  
	if(pos == NULL) return;  
	int item = m_WindowsList.GetNextSelectedItem(pos);  
	

	CString strTmp = m_WindowsList.GetItemText(item, 0);
	int WndId = atoi(strTmp.GetBuffer());

	strTmp = m_WindowsList.GetItemText(item, 1);
	int WndStatus = atoi(strTmp.GetBuffer());

	strTmp = m_WindowsList.GetItemText(item, 2);
	int WndIndex = atoi(strTmp.GetBuffer());

	strTmp = m_WindowsList.GetItemText(item, 3);
	int WndOutputId = atoi(strTmp.GetBuffer());

	strTmp = m_WindowsList.GetItemText(item, 4);
	int WndType = atoi(strTmp.GetBuffer());

	strTmp = m_WindowsList.GetItemText(item, 5);
	int SourceId = atoi(strTmp.GetBuffer());

	strTmp = m_WindowsList.GetItemText(item, 6);
	int SubSourceId = atoi(strTmp.GetBuffer());

	m_WndType = WndType;
	m_WndId = WndId;
	m_OutputId = WndOutputId;
	m_SourceId = SubSourceId;

	UpdateData(FALSE);
}


void Cnmc_mfc_testDlg::OnNMClickListLargeScreen(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;

	POSITION pos = m_LargeScreenList.GetFirstSelectedItemPosition();  
	if(pos == NULL) return;  
	int item = m_LargeScreenList.GetNextSelectedItem(pos);  
	
	CString strTmp = m_LargeScreenList.GetItemText(item, 0);
	int WndId = atoi(strTmp.GetBuffer());

	strTmp = m_LargeScreenList.GetItemText(item, 1);
	int WndStatus = atoi(strTmp.GetBuffer());

	strTmp = m_LargeScreenList.GetItemText(item, 6);
	int SubSourceId = atoi(strTmp.GetBuffer());

	strTmp = m_LargeScreenList.GetItemText(item, 7);
	int WndType = atoi(strTmp.GetBuffer());

	m_WndType = WndType;
	m_WndId = WndId;
	m_OutputId = -1;
	m_SourceId = SubSourceId;

	UpdateData(FALSE);
}

/*
void Cnmc_mfc_testDlg::OnNMClickListsourcelist(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;

	POSITION pos = m_SourceList.GetFirstSelectedItemPosition();  
	if(pos == NULL) return;
	int item = m_SourceList.GetNextSelectedItem(pos);  
	
	CString strTmp = m_SourceList.GetItemText(item, 0);
	int SubSourceId = atoi(strTmp.GetBuffer());

	m_SourceId = SubSourceId;

	UpdateData(FALSE);
}
*/

void Cnmc_mfc_testDlg::OnBnClickedButtonSignalOperator()
{
	// TODO: 在此添加控件通知处理程序代码
	CDeviceListDialog Dialog(m_handle);
	Dialog.DoModal();

}


void Cnmc_mfc_testDlg::OnBnClickedButtonSwoperator()
{
	//漫游窗口操作
	CSlidingWindowTest Dialog(m_handle); 
	Dialog.DoModal();
}

void Cnmc_mfc_testDlg::OnBnClickedButtonSurveyOperate()
{
	CDialogSurveyTest SurveyTest(m_handle);
	SurveyTest.DoModal();
}

void Cnmc_mfc_testDlg::OnBnClickedButtonSearchdevcie()
{
	m_SearchDevcieList.DeleteAllItems();
	nmc_search_device(NULL, 0);
}


void Cnmc_mfc_testDlg::OnBnClickedButtonPushStreamTest()
{
	CDialogPushSteamTest PushStreamTest(m_handle, m_WndIdMapS);
	PushStreamTest.DoModal();
}
