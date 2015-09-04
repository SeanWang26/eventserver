
// jteventtestDlg.h : ͷ�ļ�
//

#pragma once

#include "../../src/JtEventServer.h"
#include "../../src/JtEventListen.h"
#include "../../src/JtEventConnect.h"
#include "../../src/JtEventConnPeer.h"

// CjteventtestDlg �Ի���
class CjteventtestDlg : public CDialogEx, public JtListenEventCallbackSink
{
	JtEventServer *EventServer;
	JtEventListen *EventListen;
	JtEventConnect *EventConnect;

// ����
public:
	CjteventtestDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_JTEVENTTEST_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
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
