
// demo_clientapi.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// Cdemo_clientapiApp:
// �йش����ʵ�֣������ demo_clientapi.cpp
//

class Cdemo_clientapiApp : public CWinApp
{
public:
	Cdemo_clientapiApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern Cdemo_clientapiApp theApp;