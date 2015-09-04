#pragma once
#include "afxcmn.h"
#include <map>
#include "afxwin.h"
using namespace std;

// CDialogPushSteamTest �Ի���
struct stPushInfo
{
	long streamHandle;
	int WndId;
	CString StreamFile;
	HANDLE hPushStreamThread;
	int Running;
};

class CDialogPushSteamTest : public CDialogEx
{
	DECLARE_DYNAMIC(CDialogPushSteamTest)
	long m_Handle;
	CListCtrl m_PsStreamInfoList;
	CComboBox m_PSwndidComboList;

	std::map<int,int> m_WndIdMapS;  //����id
	std::map<long,struct stPushInfo *> m_StreamPullingS;  //

public:
	CDialogPushSteamTest(long handle, std::map<int,int> WndIdMapS);   // ��׼���캯��
	CDialogPushSteamTest(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDialogPushSteamTest();

// �Ի�������
	enum { IDD = IDD_DIALOG_PUSH_STRAEM_TEST };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButtondelpsstream();
	afx_msg void OnBnClickedButtonCreatePsstream();
	afx_msg void OnBnClickedButtonStartpushstream();
	afx_msg void OnBnClickedButtonpsstopstream();
	afx_msg void OnBnClickedButtonpswndshowstream();
	afx_msg void OnBnClickedButtonpswndstop();
	afx_msg void OnBnClickedButtonpsstreamstat();
};
