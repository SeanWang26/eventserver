
// nmc_mfc_testDlg.h : ͷ�ļ�
//

#pragma once
#include "afxcmn.h"

#include "NMCUser.h"
#include "afxwin.h"
#include <map>
// Cnmc_mfc_testDlg �Ի���
class Cnmc_mfc_testDlg : public CDialogEx
{
// ����
public:
	Cnmc_mfc_testDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_NMC_MFC_TEST_DIALOG };

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
private:
	static int NMC_CALL_TYPE static_NmcStatusCallback(long handle, int type, void* data, int datalen, void *userdata);
	int NmcStatusCallback(long handle, int type, void* data, int datalen);

	int m_NmcPort;
	CString m_MncIp;
	CString m_NmcUser;
	CString m_NmcPwd;

	long m_handle;

	int m_NdmId;
	CString m_NdmIp;
	int m_NdmPort;
	CString m_NdmName;

	CListCtrl m_MatrixList;
	CListCtrl m_OutputList;
	CListCtrl m_WindowsList;
	CListCtrl m_LargeScreenList;
	CListCtrl m_LargeSubScreenList;
	CListCtrl m_SearchDevcieList;

	CComboBox m_MatrixOpTypeComb;
	CComboBox m_LayoutTypeComb;

	int m_WndType;
	int m_WndId;
	int m_OutputId;
	int m_SourceId;
	int m_CreateLargeRow;
	int m_CreateLargeCol;

	std::map<int,int> m_IdMapS;     //���������id
	std::map<int,int> m_WndIdMapS;  //����id

	afx_msg void OnBnClickedButtonLogin();
	afx_msg void OnBnClickedButtongetmaxtrix();
	afx_msg void OnBnClickedButtongetoutput();
	afx_msg void OnBnClickedButtonwindowstop();
	afx_msg void OnBnClickedButtonplaysource();

	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButtonsetmatrix();

	
	afx_msg void OnBnClickedButtonShowScreenNum();
	afx_msg void OnBnClickedButtonlogou();
	afx_msg void OnBnClickedButtonGetLargeScreen();
	afx_msg void OnBnClickedButtonFilp();
	afx_msg void OnBnClickedButtonWndFullScreen();

	afx_msg void OnBnClickedButtonDelLargescreen();


	afx_msg void OnBnClickedButtonCreatelargeScreen();
	
	afx_msg void OnBnClickedButtonSetlayout();

	afx_msg void OnBnClickedButtonSourceName();
	
public:
	afx_msg void OnNMClickListwindows(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClickListLargeScreen(NMHDR *pNMHDR, LRESULT *pResult);
	///afx_msg void OnNMClickListsourcelist(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButtonSignalOperator();
	afx_msg void OnBnClickedButtonSwoperator();
	afx_msg void OnBnClickedButtonSurveyOperate();
	afx_msg void OnBnClickedButtonSearchdevcie();

	afx_msg void OnBnClickedButtonPushStreamTest();
};
