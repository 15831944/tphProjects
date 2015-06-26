#include "stdafx.h"
#include "TestUnicodeIO.h"
#include "TabSubView3.h"
#include "afxdialogex.h"


IMPLEMENT_DYNAMIC(CTabSubView3, CDialog)

CTabSubView3::CTabSubView3(CWnd* pParent)
    : CDialog(CTabSubView3::IDD, pParent)
{

}

CTabSubView3::~CTabSubView3()
{
}

void CTabSubView3::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_TREE_MAIN, m_mainTree);
}


BEGIN_MESSAGE_MAP(CTabSubView3, CDialog)
END_MESSAGE_MAP()


