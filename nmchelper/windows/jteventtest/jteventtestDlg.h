
// jteventtestDlg.h : ͷ�ļ�
//

#pragma once

#include "../../src/JtEventServer.h"
#include "../../src/JtEventListen.h"
#include "../../src/JtEventConnect.h"
#include "../../src/JtEventConnPeer.h"

// CjteventtestDlg �Ի���
class CjteventtestDlg : public CDialogEx
	, public JtListenEventCallbackSink
	, public JtConnectEventCallbackSink
	, public JtEventConnPeerCallbackSink
{
	JtEventServer *EventServer;
	JtEventListen *EventListen;
	JtEventConnect *EventConnect;
	JtEventConnPeer *ConnedPeer;
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
	
	//JtListenEventCallbackSink
	virtual int OnAccept(JtEventConnPeer *ConnPeer);

	//JtConnectEventCallbackSink
	virtual int OnRecvData(void* Cookie, unsigned char* pData, int dataLen);
	virtual int OnConnected(void* Cookie);
	virtual int OnClosed(void* Cookie);
	virtual int OnReConnected(void* Cookie);

	virtual int OnJtEventConnPeerRecvData(void* Cookie, unsigned char* pData, int dataLen);
	virtual int OnJtEventConnPeerConnected(void* Cookie);
	virtual int OnJtEventConnPeerClosed(void* Cookie);
	virtual int OnJtEventConnPeerReConnected(void* Cookie);

public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButtonTestCmd();
	afx_msg void OnBnClickedButtonTryconnect();
	afx_msg void OnBnClickedButtonconnecttest();
	afx_msg void OnBnClickedButtonbeconnecttest();
	afx_msg void OnBnClickedButtonconnectdisconnect();
	afx_msg void OnBnClickedButtonconneddisconnect();
	afx_msg void OnBnClickedButtonstartserver();
	afx_msg void OnBnClickedButtonstartlisten();
	afx_msg void OnBnClickedButtonstopserver();
};
