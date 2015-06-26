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

BEGIN_MESSAGE_MAP(CTabSubView2, CDialog)
    ON_WM_SIZE()
END_MESSAGE_MAP()

void CTabSubView2::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST_TABVIEW2MAIN, m_mainList);
}

void CTabSubView2::LayoutControl(CWnd* pCtrl, LayoutRef refTopLeft, LayoutRef refBottomRight, int cx, int cy)
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
        pCtrl->MoveWindow(rcS.left+deltaX, rcS.top, rcS.Width(), cy+deltaY);
    }
    else if(refTopLeft == BottomLeft && refBottomRight == BottomLeft)
    {
        pCtrl->MoveWindow(rcS.left, rcS.top+deltaY, rcS.Width(), rcS.Height());
    }
    else if(refTopLeft == BottomLeft && refBottomRight == BottomRight)
    {
        pCtrl->MoveWindow(rcS.left, rcS.top+deltaY, cx+deltaX, rcS.Height());
    }
    else if(refTopLeft == BottomRight && refBottomRight == BottomRight)
    {
        pCtrl->MoveWindow(rcS.left+deltaX, rcS.top+deltaY, rcS.Width(), rcS.Height());
    }
}

void CTabSubView2::OnSize(UINT nType, int cx, int cy)
{
    CDialog::OnSize(nType, cx, cy);
    if(m_mainList.GetSafeHwnd() == NULL)
    {
        if(nType != SIZE_MINIMIZED)
        {
            m_oldCx = cx;
            m_oldCy = cy;
        }
        return;
    }
    LayoutControl(&m_mainList, TopLeft, BottomRight, cx, cy);
    if(nType != SIZE_MINIMIZED)
    {
        m_oldCx = cx;
        m_oldCy = cy;
    }
    InvalidateRect(NULL);
}

BOOL CTabSubView2::OnInitDialog()
{
    CDialog::OnInitDialog();
    DWORD dwStyle = m_mainList.GetExtendedStyle();
    dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES;
    m_mainList.SetExtendedStyle(dwStyle);

    const int nColCount = 4;
    CString columnLabel[] = {_T("Flight Type"), _T("Taxiway"), _T("From") ,_T("To")};
    int DefaultColumnWidth[] = { 140, 140, 140, 140 };
    int nColFormat[] = { LVCFMT_NOEDIT, LVCFMT_DROPDOWN, LVCFMT_DROPDOWN, LVCFMT_DROPDOWN };

    LV_COLUMNEX lvc;
    lvc.mask = LVCF_WIDTH | LVCF_TEXT;

    CStringList strList;
    strList.RemoveAll();
    lvc.csList = &strList;

    lvc.fmt = nColFormat[0];
    lvc.pszText = columnLabel[0].GetBuffer();
    lvc.cx = 140;
    m_mainList.InsertColumn(0, &lvc);

    lvc.fmt = nColFormat[1];
    lvc.pszText = columnLabel[1].GetBuffer();
    lvc.cx = 140;
    lvc.csList = &strList;
    m_mainList.InsertColumn(1, &lvc);

    lvc.fmt = nColFormat[2];
    lvc.pszText = columnLabel[2].GetBuffer();
    lvc.cx = 140;
    lvc.csList = &strList;
    m_mainList.InsertColumn(2, &lvc);

    lvc.fmt = nColFormat[3];
    lvc.pszText = columnLabel[3].GetBuffer();
    lvc.cx = 140;
    lvc.csList = &strList;
    m_mainList.InsertColumn(3, &lvc);


    for(int i=0; i<10; i++)
    {
        CString strItem;
        strItem.Format(_T("item%d"), i+1);
        m_mainList.InsertItem(i, strItem);
        strItem.Format(_T("b%d"), i+1);
        m_mainList.SetItemText(i, 1, strItem);
        strItem.Format(_T("c%d"), i+1);
        m_mainList.SetItemText(i, 2, strItem);
        strItem.Format(_T("d%d"), i+1);
        m_mainList.SetItemText(i, 3, strItem);
    }

    return TRUE;
}

BOOL CTabSubView2::PreTranslateMessage(MSG* pMsg)
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

