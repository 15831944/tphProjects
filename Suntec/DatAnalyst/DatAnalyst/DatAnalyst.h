#pragma once

#ifndef __AFXWIN_H__
#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"
class CDatAnalystApp : public CWinApp
{
public:
    CDatAnalystApp();
    ULONG_PTR m_gdiplusToken;

public:
    virtual BOOL InitInstance();
    DECLARE_MESSAGE_MAP()
};

extern CDatAnalystApp theApp;