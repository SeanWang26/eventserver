#pragma once

// CDeviceListDialog 对话框
#include "NMCUser.h"
#include "afxcmn.h"

class CDeviceListDialog : public CDialog
{
	DECLARE_DYNAMIC(CDeviceListDialog)

	long m_handle;
	CListCtrl m_DevList;
	CListCtrl m_SourceList;
	CListCtrl m_LocalSourceList2;

	CString m_DevType;
	CString m_DevIp;
	int m_DevPort;
	CString m_DevUser;
	CString m_DevPwd;
	CString m_DevName;

public:
	
	CDeviceListDialog(long Handle);   // 标准构造函数
	CDeviceListDialog(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDeviceListDialog();

// 对话框数据
	enum { IDD = IDD_DIALOG_DEVICE_LIST };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedOk();	
	afx_msg void OnBnClickedButtonGetDevSignal();

	afx_msg void OnBnClickedButtonAddRemoteDevice();
	afx_msg void OnBnClickedButtonRemoveRemoteDevcie();
	afx_msg void OnBnClickedButtonaddsource();
	afx_msg void OnBnClickedButtongetsource();
	afx_msg void OnBnClickedButtondelsource();

};
