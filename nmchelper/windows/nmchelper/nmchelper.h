// nmchelper.h : nmchelper DLL ����ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CnmchelperApp
// �йش���ʵ�ֵ���Ϣ������� nmchelper.cpp
//

class CnmchelperApp : public CWinApp
{
public:
	CnmchelperApp();

// ��д
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
