#pragma once

#include "NMCUser.h"
#include "afxcmn.h"
#include "afxwin.h"
// CSlidingWindowTest �Ի���

class CSlidingWindowTest : public CDialogEx
{
	DECLARE_DYNAMIC(CSlidingWindowTest)

private:
	CListCtrl m_SWList;
	long m_handle;

	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButtonStickslidingwindow();
	afx_msg void OnBnClickedButtonslidingwininfo();
public:
	CSlidingWindowTest(long handle); 
	CSlidingWindowTest(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CSlidingWindowTest();

// �Ի�������
	enum { IDD = IDD_DIALOG_SW_Operate };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
public:


	afx_msg void OnBnClickedButtonDeleteslidingwindow();
	afx_msg void OnBnClickedButtoncreateslidingwindow();
private:
	CComboBox m_SwOutputid1;
	CComboBox m_SwOutputid2;
	CComboBox m_SwOutputid3;
	CComboBox m_SwOutputid4;

	//���Ŀ�
	int m_CSubWinWidth1;
	int m_CSubWinWidth2;
	int m_CSubWinWidth3;
	int m_CSubWinWidth4;

	//���ĸ�
	int m_CSubWinHeight1;
	int m_CSubWinHeight2;
	int m_CSubWinHeight3;
	int m_CSubWinHeight4;

	//��ʼλ��x
	int m_DisPlayX1;
	int m_DisPlayX2;
	int m_DisPlayX3;
	int m_DisPlayX4;

	//��ʼλ��y
	int m_DisPlayY1;
	int m_DisPlayY2;
	int m_DisPlayY3;
	int m_DisPlayY4;

	//������ʾ�Ŀ�
	int m_Width1;
	int m_Width2;
	int m_Width3;
	int m_Width4;

	//������ʾ�ĸ�
	int m_Height1;
	int m_Height2;
	int m_Height3;
	int m_Height4;

	int m_OutputStartX1;
	int m_OutputStartX2;
	int m_OutputStartX3;
	int m_OutputStartX4;

	int m_OutputStartY1;
	int m_OutputStartY2;
	int m_OutputStartY3;
	int m_OutputStartY4;
};
