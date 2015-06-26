#include "stdafx.h"
#include "TestUnicodeIO.h"
#include "TestUnicodeIODlg.h"
#include "afxdialogex.h"
#include <string>
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CTestUnicodeIODlg::CTestUnicodeIODlg(CWnd* pParent)
    : CDialogEx(CTestUnicodeIODlg::IDD, pParent)
{
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTestUnicodeIODlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_MAIN_TAB, m_mainTab);
}

BEGIN_MESSAGE_MAP(CTestUnicodeIODlg, CDialogEx)
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_WM_CREATE()
    ON_NOTIFY(TCN_SELCHANGE, IDC_MAIN_TAB, OnTcnSelchangeMainTab)
    ON_WM_SIZE()
    ON_WM_GETMINMAXINFO()
END_MESSAGE_MAP()

BOOL CTestUnicodeIODlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    CMenu* pSysMenu = GetSystemMenu(FALSE);

    SetIcon(m_hIcon, TRUE);
    SetIcon(m_hIcon, FALSE);

    m_mainTab.InsertItem(0,_T("20141124"));
    m_mainTab.InsertItem(1,_T("20141125"));
    m_mainTab.InsertItem(2,_T("20141126"));
    m_mainTab.InsertItem(3,_T("20141201"));
    m_mainTab.SetCurSel(2);
    m_tabView1.ShowWindow(FALSE);
    m_tabView2.ShowWindow(FALSE);
    m_tabView3.ShowWindow(TRUE);
    m_tabView4.ShowWindow(FALSE);
    LayoutTabViews();

    return TRUE;
}

void CTestUnicodeIODlg::OnSysCommand(UINT nID, LPARAM lParam)
{
    CDialogEx::OnSysCommand(nID, lParam);
}

void CTestUnicodeIODlg::OnPaint()
{
    if (IsIconic())
    {
        CPaintDC dc(this);
        SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);
        int cxIcon = GetSystemMetrics(SM_CXICON);
        int cyIcon = GetSystemMetrics(SM_CYICON);
        CRect rect;
        GetClientRect(&rect);
        int x = (rect.Width() - cxIcon + 1) / 2;
        int y = (rect.Height() - cyIcon + 1) / 2;
        dc.DrawIcon(x, y, m_hIcon);
    }
    else
    {
        CDialogEx::OnPaint();
    }
}

HCURSOR CTestUnicodeIODlg::OnQueryDragIcon()
{
    return static_cast<HCURSOR>(m_hIcon);
}


int CTestUnicodeIODlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CDialogEx::OnCreate(lpCreateStruct) == -1)
        return -1;
    m_tabView1.Create(IDD_TABSUBVIEW1,GetDlgItem(IDC_MAIN_TAB));
    m_tabView2.Create(IDD_TABSUBVIEW2,GetDlgItem(IDC_MAIN_TAB));
    m_tabView3.Create(IDD_TABSUBVIEW3,GetDlgItem(IDC_MAIN_TAB));
    m_tabView4.Create(IDD_TABSUBVIEW4,GetDlgItem(IDC_MAIN_TAB));

    return 0;
}


void CTestUnicodeIODlg::OnTcnSelchangeMainTab(NMHDR *pNMHDR, LRESULT *pResult)
{
    int CurSel = m_mainTab.GetCurSel();

    switch(CurSel)
    {
    case 0:
        m_tabView1.ShowWindow(TRUE);
        m_tabView2.ShowWindow(FALSE);
        m_tabView3.ShowWindow(FALSE);
        m_tabView4.ShowWindow(FALSE);
        break;
    case 1:
        m_tabView1.ShowWindow(FALSE);
        m_tabView2.ShowWindow(TRUE);
        m_tabView3.ShowWindow(FALSE);
        m_tabView4.ShowWindow(FALSE);
        break;
    case 2:
        m_tabView1.ShowWindow(FALSE);
        m_tabView2.ShowWindow(FALSE);
        m_tabView3.ShowWindow(TRUE);
        m_tabView4.ShowWindow(FALSE);
        break;
    case 3:
        m_tabView1.ShowWindow(FALSE);
        m_tabView2.ShowWindow(FALSE);
        m_tabView3.ShowWindow(FALSE);
        m_tabView4.ShowWindow(TRUE);
        break;
    default:
        break;
    }
    *pResult = 0;
}

void CTestUnicodeIODlg::LayoutControl(CWnd* pCtrl, LayoutRef refTopLeft, LayoutRef refBottomRight, int cx, int cy)
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

void CTestUnicodeIODlg::OnSize(UINT nType, int cx, int cy)
{
    CDialogEx::OnSize(nType, cx, cy);

    if(GetDlgItem(IDOK) == NULL)
    {
        if(nType != SIZE_MINIMIZED)
        {
            m_oldCx = cx;
            m_oldCy = cy;
        }
        return;
    }

    LayoutControl(GetDlgItem(IDOK), BottomRight, BottomRight, cx, cy);
    LayoutControl(GetDlgItem(IDCANCEL), BottomRight, BottomRight, cx, cy);
    LayoutControl(&m_mainTab, TopLeft, BottomRight, cx, cy);
    LayoutTabViews();
    if(nType != SIZE_MINIMIZED)
    {
        m_oldCx = cx;
        m_oldCy = cy;
    }
    InvalidateRect(NULL);
}

void CTestUnicodeIODlg::LayoutTabViews()
{
    CRect rTabClient;
    m_mainTab.GetClientRect(&rTabClient);
    rTabClient.top += 40;
    rTabClient.left += 25;
    rTabClient.bottom -= 5;
    rTabClient.right -= 5;
    m_tabView1.MoveWindow(&rTabClient);
    m_tabView2.MoveWindow(&rTabClient);
    m_tabView3.MoveWindow(&rTabClient);
    m_tabView4.MoveWindow(&rTabClient);
}


void CTestUnicodeIODlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
    lpMMI->ptMinTrackSize.x = 500;
    lpMMI->ptMinTrackSize.y = 400;
    CDialogEx::OnGetMinMaxInfo(lpMMI);
}
