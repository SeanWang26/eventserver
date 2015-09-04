// DialogCreateLargeScreen.cpp : 实现文件
//

#include "stdafx.h"
#include "nmc_mfc_test.h"
#include "DialogCreateLargeScreen.h"
#include "afxdialogex.h"


// CDialogCreateLargeScreen 对话框

IMPLEMENT_DYNAMIC(CDialogCreateLargeScreen, CDialog)

CDialogCreateLargeScreen::CDialogCreateLargeScreen(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogCreateLargeScreen::IDD, pParent)
{

}

CDialogCreateLargeScreen::~CDialogCreateLargeScreen()
{
}

void CDialogCreateLargeScreen::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDialogCreateLargeScreen, CDialog)
END_MESSAGE_MAP()


// CDialogCreateLargeScreen 消息处理程序
