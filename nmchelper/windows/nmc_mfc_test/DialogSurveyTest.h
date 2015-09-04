#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// CDialogSurveyTest �Ի���

class CDialogSurveyTest : public CDialogEx
{
	DECLARE_DYNAMIC(CDialogSurveyTest)

	long m_Handle;
public:
	CDialogSurveyTest(long Handle); 
	CDialogSurveyTest(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDialogSurveyTest();

// �Ի�������
	enum { IDD = IDD_DIALOG_SURVEY_TEST };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
private:
	CListCtrl m_SurveyPlanList;
	CListCtrl m_SurveyDeviceS;

	struct st_xy_survey_info *ppsurvey_info;
	int survey_info_cnt;

	int m_SurveyWindowId;
	CComboBox m_CombCtrlSurvey;
public:
	afx_msg void OnBnClickedButton1();
	afx_msg void OnNMClickListSurveyPlan(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButtonDelSurvey();
	afx_msg void OnBnClickedButtonAddSurvey();
	afx_msg void OnBnClickedButtonCtrlSurvey();
	afx_msg void OnBnClickedOk();
	
private:

};
