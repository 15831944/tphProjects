#pragma once

#ifndef __AFXWIN_H__
#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"
#include "ApiRsp.h"
class Cdemo_serverapiApp : public CWinApp
{
public:
    Cdemo_serverapiApp();
public:
    virtual BOOL InitInstance();
    DECLARE_MESSAGE_MAP()

};

extern Cdemo_serverapiApp theApp;