#include "stdafx.h"
#include "TestUnicodeIO.h"
#include "TabSubView2.h"

IMPLEMENT_DYNAMIC(CTabSubView2, CDialog)

CTabSubView2::CTabSubView2(CWnd* pParent) : CDialog(CTabSubView2::IDD, pParent)
{
}

CTabSubView2::~CTabSubView2()
{
}

void CTabSubView2::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CTabSubView2, CDialog)
END_MESSAGE_MAP()

