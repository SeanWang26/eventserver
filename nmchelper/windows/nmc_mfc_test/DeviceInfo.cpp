// DeviceInfo.cpp : 实现文件
//

#include "stdafx.h"
#include "nmc_mfc_test.h"
#include "DeviceInfo.h"
#include "afxdialogex.h"


// CDeviceInfo 对话框

IMPLEMENT_DYNAMIC(CDeviceInfo, CDialogEx)

CDeviceInfo::CDeviceInfo(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDeviceInfo::IDD, pParent)
	, m_Ip(_T("192.168.3.192"))
	, m_Port(20000)
	, m_User(_T("admin"))
	, m_Password(_T("admin"))
	, m_DeviceName(_T("设备名"))
{

}

CDeviceInfo::~CDeviceInfo()
{
}

void CDeviceInfo::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_EDIT1, m_Ip);
	DDX_Text(pDX, IDC_EDIT2, m_Port);
	DDX_Text(pDX, IDC_EDIT3, m_User);
	DDX_Text(pDX, IDC_EDIT4, m_Password);
	DDX_Text(pDX, IDC_EDIT5, m_DeviceName);
}


BEGIN_MESSAGE_MAP(CDeviceInfo, CDialogEx)
	ON_BN_CLICKED(IDOK, &CDeviceInfo::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDeviceInfo::OnBnClickedCancel)
END_MESSAGE_MAP()


// CDeviceInfo 消息处理程序


void CDeviceInfo::OnBnClickedOk()
{
	UpdateData(TRUE);

	if(m_Ip.GetLength()==0 || m_User.GetLength()==0 || m_Port==0 || m_DeviceName.GetLength()==0)
	{
		MessageBox(_T("参数错误"));
		return ;
	}

	CDialogEx::OnOK();
}


void CDeviceInfo::OnBnClickedCancel()
{


	CDialogEx::OnCancel();
}
