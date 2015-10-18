
// jteventtestDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "jteventtest.h"
#include "jteventtestDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include <vector>
using namespace std;
static vector<JtEventConnect*> ConnS;
static vector<JtEventConnPeer*> ConnPeerS;

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


// CjteventtestDlg 对话框




CjteventtestDlg::CjteventtestDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CjteventtestDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	EventServer = 0;
	EventListen = 0;
	EventConnect = 0;
	ConnedPeer = 0;
}

void CjteventtestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CjteventtestDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CjteventtestDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_TEST_CMD, &CjteventtestDlg::OnBnClickedButtonTestCmd)
	ON_BN_CLICKED(IDC_BUTTON_TryConnect, &CjteventtestDlg::OnBnClickedButtonTryconnect)
	ON_BN_CLICKED(IDC_BUTTON_connect_test, &CjteventtestDlg::OnBnClickedButtonconnecttest)
	ON_BN_CLICKED(IDC_BUTTON_beconnect_test, &CjteventtestDlg::OnBnClickedButtonbeconnecttest)
	ON_BN_CLICKED(IDC_BUTTON_connect_disconnect, &CjteventtestDlg::OnBnClickedButtonconnectdisconnect)
	ON_BN_CLICKED(IDC_BUTTONconned_disconnect, &CjteventtestDlg::OnBnClickedButtonconneddisconnect)
	ON_BN_CLICKED(IDC_BUTTONStartserver, &CjteventtestDlg::OnBnClickedButtonstartserver)
	ON_BN_CLICKED(IDC_BUTTON_startlisten, &CjteventtestDlg::OnBnClickedButtonstartlisten)
	ON_BN_CLICKED(IDC_BUTTON_stopserver, &CjteventtestDlg::OnBnClickedButtonstopserver)
END_MESSAGE_MAP()


// CjteventtestDlg 消息处理程序

BOOL CjteventtestDlg::OnInitDialog()
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

	//Sleep(20000);
	//EventServer->Stop();
	//EventListen

	//JtEventServer *EventServer = new JtEventServer();
	//JtEventServer *EventServer = new JtEventServer();




	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CjteventtestDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CjteventtestDlg::OnPaint()
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
HCURSOR CjteventtestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CjteventtestDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	CDialogEx::OnOK();
}


void CjteventtestDlg::OnBnClickedButtonTestCmd()
{
	if(EventServer)
		EventServer->TestCmd();
}

unsigned int __stdcall Statc_StartInThread(void *arg)
{
	//JtEventServer *Self = (JtEventServer *)arg;

	//return Self->EventLoop();
}

void CjteventtestDlg::OnBnClickedButtonTryconnect()
{
	if(EventServer&&EventConnect==NULL)
	{
		EventConnect = new JtEventConnect();
		EventConnect->SetJtEventCallbackSink(this,NULL);
		int res = EventConnect->DoConnect("127.0.0.1", 20100, 3, EventServer);

		//EventServer->AddPeer(EventConnect);
	}

	JtEventConnect *EventConnect_ = new JtEventConnect();
	EventConnect_->SetJtEventCallbackSink(this,NULL);
	int res = EventConnect_->DoConnect("127.0.0.1", 20100, 4, EventServer);

	//EventServer->AddPeer(EventConnect_);	

	ConnS.push_back(EventConnect_);

	int i=100;
	while(i-->0)
	{
		EventConnect_ = new JtEventConnect();
		EventConnect_->SetJtEventCallbackSink(this,NULL);
		int res = EventConnect_->DoConnect("127.0.0.1", 20010, 4, EventServer);

		ConnS.push_back(EventConnect_);
	}
	
	/*
	while();
	unsigned int threadID = 0;
	HANDLE  tid = (HANDLE)_beginthreadex(NULL, 0, Statc_StartInThread, 0, 0, &threadID);
	if(tid)
	{

	}
	)*/
}
int CjteventtestDlg::OnAccept(JtEventConnPeer *ConnedPeer_)
{
	//assert(ConnedPeer_);
	if(EventServer)
	{
		if(ConnedPeer_)
		{
			ConnedPeer = ConnedPeer_;
			ConnedPeer->SetJtEventCallbackSink(this, NULL);
			EventServer->AddPeer(ConnedPeer);
			ConnPeerS.push_back(ConnedPeer);
		}
	}
	else
	{
		delete ConnedPeer;
	}

	return 0;
}
int CjteventtestDlg::OnRecvData(void* Cookie, unsigned char* pData, int dataLen)
{
	MessageBox(_T("OnRecvData"), _T("OnRecvData"), MB_OK);
	return 0;
}
int CjteventtestDlg::OnConnected(void* Cookie)
{
	//MessageBox(_T("OnConnected"), _T("OnConnected"), MB_OK);
	static int ii = 0;
	++ii;
	return 0;
}
int CjteventtestDlg::OnClosed(void* Cookie)
{
	//MessageBox(_T("OnClosed"), _T("OnClosed"), MB_OK);
	return 0;
}
int CjteventtestDlg::OnReConnected(void* Cookie)
{
	return 0;
}
int CjteventtestDlg::OnJtEventConnPeerRecvData(void* Cookie, unsigned char* pData, int dataLen)
{
	MessageBox(_T("OnJtEventConnPeerRecvData"), _T("OnJtEventConnPeerRecvData"), MB_OK);
	return 0;
}
int CjteventtestDlg::OnJtEventConnPeerConnected(void* Cookie)
{
	return 0;
}
int CjteventtestDlg::OnJtEventConnPeerClosed(void* Cookie)
{
	//MessageBox(_T("OnJtEventConnPeerClosed"), _T("OnJtEventConnPeerClosed"), MB_OK);
	return 0;
}
int CjteventtestDlg::OnJtEventConnPeerReConnected(void* Cookie)
{
	
	return 0;
}

void CjteventtestDlg::OnBnClickedButtonconnecttest()
{
	if(EventConnect)
		EventConnect->TestCmd();
}


void CjteventtestDlg::OnBnClickedButtonbeconnecttest()
{
	if(ConnedPeer)
		ConnedPeer->TestCmd();
}

void CjteventtestDlg::OnBnClickedButtonconnectdisconnect()
{
	if(EventConnect)
	{
		EventConnect->DoDisconnect();

		delete EventConnect;
		EventConnect = 0;
	}

	for(vector<JtEventConnect*>::iterator iter = ConnS.begin(); iter!=ConnS.end(); ++iter)
	{
		(*iter)->DoDisconnect();
		delete (*iter);
	}
	
	ConnS.clear();
}

void CjteventtestDlg::OnBnClickedButtonconneddisconnect()
{

/*
	if(ConnedPeer)
	{
		ConnedPeer->DoDisconnect();
		ConnedPeer = NULL;
	}
*/

	for(vector<JtEventConnPeer*>::iterator iter = ConnPeerS.begin(); iter!=ConnPeerS.end(); ++iter)
	{
		(*iter)->DoDisconnect();
		delete (*iter);
	}

	ConnPeerS.clear();

}

void CjteventtestDlg::OnBnClickedButtonstartserver()
{
	if(EventServer==NULL)
	{
		EventServer = new JtEventServer();
		EventServer->Start();	
	}
}

void CjteventtestDlg::OnBnClickedButtonstartlisten()
{
	if(EventServer && EventListen==NULL)
	{
		EventListen = new JtEventListen(20000);
		EventListen->SetJtEventCallbackSink(this,NULL);
		EventServer->AddPeer(EventListen);	
	}
}


void CjteventtestDlg::OnBnClickedButtonstopserver()
{
	if(EventServer)
	{
		EventServer->Stop();
		delete EventServer;
		EventServer = NULL;
	}
}
