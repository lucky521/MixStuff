
// lucky003.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// Clucky003App:
// �йش����ʵ�֣������ lucky003.cpp
//

class Clucky003App : public CWinApp
{
public:
	Clucky003App();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern Clucky003App theApp;