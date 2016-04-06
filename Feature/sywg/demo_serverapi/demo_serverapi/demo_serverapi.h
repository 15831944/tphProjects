#pragma once

#ifndef __AFXWIN_H__
#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
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