
// AutoShutdown.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CAutoShutdownApp:
// �йش����ʵ�֣������ AutoShutdown.cpp
//

class CAutoShutdownApp : public CWinApp
{
public:
	CAutoShutdownApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CAutoShutdownApp theApp;