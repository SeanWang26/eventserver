#pragma once


// CDialogCreateLargeScreen �Ի���

class CDialogCreateLargeScreen : public CDialog
{
	DECLARE_DYNAMIC(CDialogCreateLargeScreen)

public:
	CDialogCreateLargeScreen(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDialogCreateLargeScreen();

// �Ի�������
	enum { IDD = IDD_DIALOG_Create_Large_Screen };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
};
