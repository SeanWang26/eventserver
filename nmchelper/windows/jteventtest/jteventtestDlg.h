
// jteventtestDlg.h : 头文件
//

#pragma once

#include "../../src/JtEventServer.h"
#include "../../src/JtEventListen.h"
#include "../../src/JtEventConnect.h"
#include "../../src/JtEventConnPeer.h"

// CjteventtestDlg 对话框
class CjteventtestDlg : public CDialogEx, public JtListenEventCallbackSink
{
	JtEventServer *EventServer;
	JtEventListen *EventListen;
	JtEventConnect *EventConnect;

// 构造
public:
	CjteventtestDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_JTEVENTTEST_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

	virtual int OnAccept(JtEventConnPeer *ConnPeer);
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButtonTestCmd();
	afx_msg void OnBnClickedButtonTryconnect();
};
