
// jteventtest.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CjteventtestApp:
// �йش����ʵ�֣������ jteventtest.cpp
//

class CjteventtestApp : public CWinApp
{
public:
	CjteventtestApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CjteventtestApp theApp;