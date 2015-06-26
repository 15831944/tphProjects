#include "stdafx.h"
#include "TestUnicodeIO.h"
#include "TabSubView3.h"
#include "afxdialogex.h"


IMPLEMENT_DYNAMIC(CTabSubView3, CDialog)

CTabSubView3::CTabSubView3(CWnd* pParent) : CDialog(CTabSubView3::IDD, pParent)
{
}

CTabSubView3::~CTabSubView3()
{
}

BEGIN_MESSAGE_MAP(CTabSubView3, CDialog)
    ON_WM_SIZE()
END_MESSAGE_MAP()

void CTabSubView3::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_TREE_MAIN, m_mainTree);
}

void CTabSubView3::OnSize(UINT nType, int cx, int cy)
{
    CDialog::OnSize(nType, cx, cy);
    if(m_mainTree.GetSafeHwnd() == NULL)
    {
        if(nType != SIZE_MINIMIZED)
        {
            m_oldCx = cx;
            m_oldCy = cy;
        }
        return;
    }
    LayoutControl(&m_mainTree, TopLeft, BottomRight, cx, cy);
    if(nType != SIZE_MINIMIZED)
    {
        m_oldCx = cx;
        m_oldCy = cy;
    }
    InvalidateRect(NULL);
}

BOOL CTabSubView3::OnInitDialog()
{
    CDialog::OnInitDialog();
    m_mainTree.SetParent(this);
    InertTempDataToTree();
    return TRUE;
}

BOOL CTabSubView3::PreTranslateMessage(MSG* pMsg)
{
    if(pMsg->message == WM_KEYDOWN) {
        switch(pMsg->wParam)
        {
        case VK_RETURN:
            return TRUE;
        case VK_ESCAPE:
            return TRUE;
        }
    }
    return CDialog::PreTranslateMessage(pMsg);
}

void CTabSubView3::LayoutControl(CWnd* pCtrl, LayoutRef refTopLeft, LayoutRef refBottomRight, int cx, int cy)
{
    CRect rcS;
    pCtrl->GetWindowRect(&rcS);
    ScreenToClient(&rcS);
    int deltaX = cx - m_oldCx;
    int deltaY = cy - m_oldCy;
    if(refTopLeft == TopLeft && refBottomRight == TopLeft)
    {
        pCtrl->MoveWindow(&rcS);
    }
    else if(refTopLeft == TopLeft && refBottomRight == TopRight)
    {
        pCtrl->MoveWindow(rcS.left, rcS.top, rcS.Width()+deltaX, rcS.Height());
    }
    else if(refTopLeft == TopLeft && refBottomRight == BottomLeft)
    {
        pCtrl->MoveWindow(rcS.left, rcS.top, rcS.Width(), rcS.Height()+deltaY);
    }
    else if(refTopLeft == TopLeft && refBottomRight == BottomRight)
    {
        pCtrl->MoveWindow(rcS.left, rcS.top, rcS.Width()+deltaX, rcS.Height()+deltaY);
    }
    else if(refTopLeft == TopRight && refBottomRight == TopRight)
    {
        pCtrl->MoveWindow(rcS.left+deltaX, rcS.top, rcS.Width(), rcS.Height());
    }
    else if(refTopLeft == TopRight && refBottomRight == BottomRight)
    {
        pCtrl->MoveWindow(rcS.left+deltaX, rcS.top, rcS.Width(), rcS.Height()+deltaY);
    }
    else if(refTopLeft == BottomLeft && refBottomRight == BottomLeft)
    {
        pCtrl->MoveWindow(rcS.left, rcS.top+deltaY, rcS.Width(), rcS.Height());
    }
    else if(refTopLeft == BottomLeft && refBottomRight == BottomRight)
    {
        pCtrl->MoveWindow(rcS.left, rcS.top+deltaY, rcS.Width()+deltaX, rcS.Height());
    }
    else if(refTopLeft == BottomRight && refBottomRight == BottomRight)
    {
        pCtrl->MoveWindow(rcS.left+deltaX, rcS.top+deltaY, rcS.Width(), rcS.Height());
    }
}

void CTabSubView3::InertTempDataToTree()
{
    COOLTREENODEINFO cni;
    CCoolTree::InitNodeInfo(this, cni);
    cni.nt = NT_CHECKBOX;
    for(int i1=0; i1<5; i1++)
    {
        CString str1;
        str1.Format(_T("level1: %d"), i1);
        HTREEITEM h1 = m_mainTree.InsertItem(str1, cni, FALSE);
        for(int i2=0; i2<5; i2++)
        {
            CString str2;
            str2.Format(_T("level2: %d,%d"), i1, i2);
            HTREEITEM h2 = m_mainTree.InsertItem(str2, cni, FALSE, FALSE, h1);
        }
        m_mainTree.Expand(h1, TVE_EXPAND);
    }
}


