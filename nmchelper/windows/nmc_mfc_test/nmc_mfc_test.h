
// nmc_mfc_test.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CNmcMfcTestApp:
// �йش����ʵ�֣������ nmc_mfc_test.cpp
//

class CNmcMfcTestApp : public CWinApp
{
public:
	CNmcMfcTestApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CNmcMfcTestApp theApp;