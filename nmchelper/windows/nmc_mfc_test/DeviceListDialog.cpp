// DeviceListDialog.cpp : 实现文件
//

#include "stdafx.h"
#include <string>

#include "nmc_mfc_test.h"
#include "DeviceInfo.h"
#include "DeviceListDialog.h"
#include "afxdialogex.h"


// CDeviceListDialog 对话框

IMPLEMENT_DYNAMIC(CDeviceListDialog, CDialog)

//utf-8转为多字节
int GetUtf8Doc_dgfv(char** pDoc, int& nDocLen, const char* in)
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

CDeviceListDialog::CDeviceListDialog(long Handle)
	: CDialog(CDeviceListDialog::IDD, NULL)
	, m_DevType(_T("TcpStream"))
	, m_DevIp(_T("192.168.3.50"))
	, m_DevPort(8000)
	, m_DevUser(_T("admin"))
	, m_DevPwd(_T("admin"))
	, m_DevName(_T("tcp-es"))
{
	m_handle = Handle;
}

CDeviceListDialog::CDeviceListDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CDeviceListDialog::IDD, pParent)
{

}

CDeviceListDialog::~CDeviceListDialog()
{

}

void CDeviceListDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_DEVICES_INFO, m_DevList);
	DDX_Control(pDX, IDC_LIST_DEV_SIGNAL, m_SourceList);

	DDX_Text(pDX, IDC_EDIT_devtype, m_DevType);
	DDX_Text(pDX, IDC_EDIT_devip, m_DevIp);
	DDX_Text(pDX, IDC_EDIT_devport, m_DevPort);
	DDX_Text(pDX, IDC_EDIT_devuser, m_DevUser);
	DDX_Text(pDX, IDC_EDIT_devpwd, m_DevPwd);
	DDX_Text(pDX, IDC_EDIT_devName, m_DevName);


	DDX_Control(pDX, IDC_LIST_localsorcelist, m_LocalSourceList2);
}


BEGIN_MESSAGE_MAP(CDeviceListDialog, CDialog)
	ON_BN_CLICKED(IDC_BUTTON1, &CDeviceListDialog::OnBnClickedButton1)
	ON_BN_CLICKED(IDOK, &CDeviceListDialog::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_GET_DEV_SIGNAL, &CDeviceListDialog::OnBnClickedButtonGetDevSignal)
	ON_BN_CLICKED(IDC_BUTTON_ADD_REMOTE_DEVICE, &CDeviceListDialog::OnBnClickedButtonAddRemoteDevice)
	ON_BN_CLICKED(IDC_BUTTON_REMOVE_REMOTE_DEVCIE, &CDeviceListDialog::OnBnClickedButtonRemoveRemoteDevcie)
	ON_BN_CLICKED(IDC_BUTTON_addsource, &CDeviceListDialog::OnBnClickedButtonaddsource)
	ON_BN_CLICKED(IDC_BUTTON_getsource, &CDeviceListDialog::OnBnClickedButtongetsource)
	ON_BN_CLICKED(IDC_BUTTON_delsource, &CDeviceListDialog::OnBnClickedButtondelsource)
END_MESSAGE_MAP()

BOOL CDeviceListDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_DevList.InsertColumn(0,_T("设备id"));
	m_DevList.SetColumnWidth(0, 70);
	m_DevList.InsertColumn(1,_T("设备ip"));
	m_DevList.SetColumnWidth(1, 120);
	m_DevList.InsertColumn(2,_T("设备端口"));
	m_DevList.SetColumnWidth(2, 80);
	m_DevList.InsertColumn(3,_T("用户名"));
	m_DevList.SetColumnWidth(3, 50);
	m_DevList.InsertColumn(4,_T("密码"));
	m_DevList.SetColumnWidth(4, 50);
	m_DevList.InsertColumn(5,_T("设备名"));
	m_DevList.SetColumnWidth(5, 50);

	m_DevList.SetExtendedStyle(LVS_EX_FLATSB|LVS_EX_FULLROWSELECT);

	m_SourceList.InsertColumn(0,_T("子设备id"));
	m_SourceList.SetColumnWidth(0, 60);
	m_SourceList.InsertColumn(1,_T("所属主设备"));
	m_SourceList.SetColumnWidth(1, 80);
	m_SourceList.InsertColumn(2,_T("设备名称"));
	m_SourceList.SetColumnWidth(2, 80);
	m_SourceList.InsertColumn(3,_T("子设备类型"));
	m_SourceList.SetColumnWidth(3, 80);
	m_SourceList.InsertColumn(4,_T("地址"));
	m_SourceList.SetColumnWidth(4, 160);
	m_SourceList.SetExtendedStyle(LVS_EX_FLATSB|LVS_EX_FULLROWSELECT);

	m_LocalSourceList2.InsertColumn(0,_T("子设备id"));
	m_LocalSourceList2.SetColumnWidth(0, 60);
	m_LocalSourceList2.InsertColumn(1,_T("所属主设备"));
	m_LocalSourceList2.SetColumnWidth(1, 80);
	m_LocalSourceList2.InsertColumn(2,_T("设备名称"));
	m_LocalSourceList2.SetColumnWidth(2, 80);
	m_LocalSourceList2.InsertColumn(3,_T("子设备类型"));
	m_LocalSourceList2.SetColumnWidth(3, 80);
	m_LocalSourceList2.InsertColumn(4,_T("地址"));
	m_LocalSourceList2.SetColumnWidth(4, 160);
	m_LocalSourceList2.SetExtendedStyle(LVS_EX_FLATSB|LVS_EX_FULLROWSELECT);

	return 0;
}
void CDeviceListDialog::OnBnClickedButton1()
{
	struct st_jn_device *dev = NULL;
	int devcnt = 0;

	if(m_handle)
	{
		int res = nmc_get_remote_device(m_handle, -1, &dev, &devcnt);
		if(res==0)
		{
			m_DevList.DeleteAllItems();
			for(int i=0; i<devcnt; ++i)
			{
				int nItem = m_DevList.GetItemCount();

				char tmp[64] = {0};
				sprintf(tmp, "%d", dev[i].DeviceId);

				nItem = m_DevList.InsertItem(nItem, tmp);
				m_DevList.SetItemText(nItem, 0, tmp);

				sprintf(tmp, "%s", dev[i].IP);
				m_DevList.SetItemText(nItem, 1, tmp);

				sprintf(tmp, "%d", dev[i].Port);
				m_DevList.SetItemText(nItem, 2, tmp);

				sprintf(tmp, "%s", dev[i].User);
				m_DevList.SetItemText(nItem,3, tmp);

				m_DevList.SetItemText(nItem,4, dev[i].Password);

				char* pDoc=NULL;
				int nDocLen = 0;
				GetUtf8Doc_dgfv(&pDoc, nDocLen, dev[i].DeviceName);
				m_DevList.SetItemText(nItem, 5, pDoc);
			}
		}	
	}

}


void CDeviceListDialog::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	CDialog::OnOK();
}

void CDeviceListDialog::OnBnClickedButtonGetDevSignal()
{
	POSITION pos = m_DevList.GetFirstSelectedItemPosition();
	int id = 0;
	if (NULL != pos)
	{
		int nItem	  = m_DevList.GetNextSelectedItem(pos);
		CString strId = m_DevList.GetItemText(nItem, 0);

		id = atoi(strId.GetBuffer());
	}
	else
	{
		MessageBox(_T("请选择一个设备"));
		return;
	}

	struct st_jn_equ *equ = NULL;
	int equcnt = 0;
	int res = nmc_get_remote_signal_source(m_handle, id, &equ, &equcnt);
	if(res)
	{
		MessageBox(_T("nmc_get_remote_signal_source failed"));
		return;
	}
	
	m_SourceList.DeleteAllItems();
	for(int i=0; i<equcnt; ++i)
	{
		for(int j=0; j<equ[i].stSubEquCnt; ++j)
		{
			int nItem = m_SourceList.GetItemCount();

			char tmp[64] = {0};
			sprintf(tmp, "%d", equ[i].stSubEqu[j].SubEquId);

			nItem = m_SourceList.InsertItem(nItem, tmp);
			m_SourceList.SetItemText(nItem, 0, tmp);

			sprintf(tmp, "%d", equ[i].stSubEqu[j].EquId);
			m_SourceList.SetItemText(nItem, 1, tmp);

			char* pDoc=NULL;
			int nDocLen = 0;

			GetUtf8Doc_dgfv(&pDoc, nDocLen, equ[i].stSubEqu[j].SubName);

			m_SourceList.SetItemText(nItem, 2, pDoc);

			sprintf(tmp, "%d", equ[i].stSubEqu[j].SubType);
			m_SourceList.SetItemText(nItem, 3, tmp);

			m_SourceList.SetItemText(nItem, 4, equ[i].IP);
		}
	}

	nmc_free_equ_info(equ, equcnt);
}

//多字节转为utf-8
void ConvertGBKToUtf811(std::string& amp, std::string strGBK)
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

void CDeviceListDialog::OnBnClickedButtonAddRemoteDevice()
{
	CDeviceInfo Info;
	if(Info.DoModal()==IDOK)
	{
		Info.m_Ip;
		Info.m_Port;
		Info.m_User;
		Info.m_Password;

		struct st_jn_device dev;
		memset(&dev, 0, sizeof(dev));
		dev.DeviceId = -1;
		dev.DeviceType = 0x0011;

		std::string strDeviceName;
		ConvertGBKToUtf811(strDeviceName, Info.m_DeviceName.GetBuffer());
		
		strcpy(dev.IP, Info.m_Ip.GetBuffer());
		
		dev.Port = Info.m_Port;
		strcpy(dev.User, Info.m_User.GetBuffer());
		strcpy(dev.Password, Info.m_Password.GetBuffer());

		strcpy(dev.DeviceName, strDeviceName.c_str());

		int res = nmc_add_remote_device(m_handle, &dev);
		if(res)
		{
			MessageBox(_T("添加失败"));
		}
		else
		{
			MessageBox(_T("成功"));	
		}
	}
}


void CDeviceListDialog::OnBnClickedButtonRemoveRemoteDevcie()
{
	//获取目前解码器上自带的所有信号源
	POSITION pos = m_DevList.GetFirstSelectedItemPosition();
	int id = 0;
	if (NULL != pos)
	{
		int nItem	  = m_DevList.GetNextSelectedItem(pos);
		CString strId = m_DevList.GetItemText(nItem, 0);

		id = atoi(strId.GetBuffer());
	}
	else
	{
		MessageBox(_T("请选择一个设备"));
		return;
	}

	int res = nmc_remove_remote_device(m_handle, id);
	if(res)
	{
		MessageBox(_T("删除失败"));
	}
	else
	{
		MessageBox(_T("成功"));
	}
}


void CDeviceListDialog::OnBnClickedButtonaddsource()
{
	UpdateData(TRUE);

	if(m_DevName.GetLength()==0)
	{
		MessageBox(_T("设备名为空"));
		return;
	}
	
	//添加一个rtsp设备
	struct st_jn_equ jn_equ;
	jn_equ.EquId = -1;
	strcpy(jn_equ.FactoryName, m_DevType.GetBuffer());
	//strcpy(jn_equ.FactoryName, "RTSP");
	static int nOpSeq = 0;
	char cEquName[64] = "";
	sprintf(cEquName, "tcp-%d", ++nOpSeq);
	jn_equ.nOpSeq = nOpSeq;
	strcpy(jn_equ.EquName, m_DevName.GetBuffer());
	
	strcpy(jn_equ.IP, m_DevIp.GetBuffer());
	
	jn_equ.Port = 554;
	strcpy(jn_equ.UserName, m_DevUser.GetBuffer());
	strcpy(jn_equ.Password, m_DevPwd.GetBuffer());
	jn_equ.ChannelNum = 1; //默认为1
	jn_equ.InputNum = 0;   //默认为0
	jn_equ.OutputNum = 0;  //默认为0
	jn_equ.nOpType = 0;    //默认为0==OP_TYPE_ADD
	jn_equ.nCtlgId = 1;    //默认为1
	int res = nmc_add_signal_source(m_handle, &jn_equ);
	if(res)
	{
		printf("nmc_add_signal_source failed!\n");
		return;
	}

	int sub_equ_id = -1;
	struct st_jn_sub_equ * jn_sub_equ= jn_equ.stSubEqu;
	for(int j=0; j<jn_equ.stSubEquCnt; ++j)
	{
		struct st_jn_sub_equ *sub_equ = &jn_sub_equ[j];
		sub_equ_id = sub_equ->SubEquId;
		printf("%d", sub_equ->SubEquId);
	}
}

//utf-8转为多字节
int GetUtf8Docsss(char** pDoc, int& nDocLen, const char* in)
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

void CDeviceListDialog::OnBnClickedButtongetsource()
{
	//获取目前解码器上自带的所有信号源
	struct st_jn_equ *equ = NULL;
	int equcnt = 0;
	int res = nmc_get_signal_source(m_handle, &equ, &equcnt);
	if(res)
	{
		MessageBox(_T("nmc_get_signal_source failed"));
		return;
	}

	m_LocalSourceList2.DeleteAllItems();
	for(int i=0; i<equcnt; ++i)
	{
		for(int j=0; j<equ[i].stSubEquCnt; ++j)
		{
			int nItem = m_LocalSourceList2.GetItemCount();

			char tmp[64] = {0};
			sprintf(tmp, "%d", equ[i].stSubEqu[j].SubEquId);

			nItem = m_LocalSourceList2.InsertItem(nItem, tmp);
			m_LocalSourceList2.SetItemText(nItem, 0, tmp);

			sprintf(tmp, "%d", equ[i].stSubEqu[j].EquId);
			m_LocalSourceList2.SetItemText(nItem, 1, tmp);

			char* pDoc=NULL;
			int nDocLen = 0;
			GetUtf8Docsss(&pDoc, nDocLen, equ[i].stSubEqu[j].SubName);
			m_LocalSourceList2.SetItemText(nItem, 2, pDoc);

			sprintf(tmp, "%d", equ[i].stSubEqu[j].SubType);
			m_LocalSourceList2.SetItemText(nItem, 3, tmp);

			m_LocalSourceList2.SetItemText(nItem, 4, equ[i].IP);
		}
	}

	nmc_free_equ_info(equ, equcnt);
}


void CDeviceListDialog::OnBnClickedButtondelsource()
{
	//子设备id, 未完成................
	//获取目前解码器上自带的所有信号源
	POSITION pos = m_LocalSourceList2.GetFirstSelectedItemPosition();
	int id = 0;
	if (NULL != pos)
	{
		int nItem	  = m_LocalSourceList2.GetNextSelectedItem(pos);
		CString strId = m_LocalSourceList2.GetItemText(nItem, 0);

		id = atoi(strId.GetBuffer());
	}
	else
	{
		MessageBox(_T("请选择一个设备"));
		return;
	}

	int res = nmc_rmv_signal_source(m_handle, id);
	if(res)
	{
		MessageBox(_T("nmc_rmv_signal_source failed"));
	}	
	else
	{
		MessageBox(_T("nmc_rmv_signal_source ok"));
	}
}
