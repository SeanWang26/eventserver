#pragma once


// CDialogCreateLargeScreen 对话框

class CDialogCreateLargeScreen : public CDialog
{
	DECLARE_DYNAMIC(CDialogCreateLargeScreen)

public:
	CDialogCreateLargeScreen(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDialogCreateLargeScreen();

// 对话框数据
	enum { IDD = IDD_DIALOG_Create_Large_Screen };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};
