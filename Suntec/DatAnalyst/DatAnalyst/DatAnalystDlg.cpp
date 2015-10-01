#include "stdafx.h"
#include "DatAnalyst.h"
#include "DatAnalystDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CDatAnalystDlg::CDatAnalystDlg(CWnd* pParent /*=NULL*/)
: CDialog(CDatAnalystDlg::IDD, pParent)
{
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CDatAnalystDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_TAB1, m_tab);
}

BEGIN_MESSAGE_MAP(CDatAnalystDlg, CDialog)
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, &CDatAnalystDlg::OnTcnSelchangeTab1)
    ON_WM_CREATE()
    ON_WM_SIZE()
    ON_WM_GETMINMAXINFO()
END_MESSAGE_MAP()

int CDatAnalystDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    CDialog::OnCreate(lpCreateStruct);
    m_multiPicsDlg.Create(IDD_MULTIPICTURES, GetDlgItem(IDC_TAB1));
    m_mulDatProcDlg.Create(IDD_MULTIDATPROCESS,GetDlgItem(IDC_TAB1));
    m_singleDatViewDlg.Create(IDD_SINGLEDATVIEW,GetDlgItem(IDC_TAB1));
    return 0;
}

BOOL CDatAnalystDlg::OnInitDialog()
{
    CDialog::OnInitDialog();
    SetIcon(m_hIcon, TRUE);
    SetIcon(m_hIcon, FALSE);

    m_tab.InsertItem(0, _T("图片叠加"));
    m_tab.InsertItem(0, _T("单个查看"));
    m_tab.InsertItem(0, _T("批量解析"));
    m_tab.SetCurSel(2);
    m_mulDatProcDlg.ShowWindow(FALSE);
    m_singleDatViewDlg.ShowWindow(FALSE);
    m_multiPicsDlg.ShowWindow(TRUE);
    LayoutTabViews();
    return TRUE;
}

void CDatAnalystDlg::OnPaint()
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
        CDialog::OnPaint();
    }
}

HCURSOR CDatAnalystDlg::OnQueryDragIcon()
{
    return static_cast<HCURSOR>(m_hIcon);
}

void CDatAnalystDlg::OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult)
{
    m_mulDatProcDlg.ShowWindow(FALSE);
    m_singleDatViewDlg.ShowWindow(FALSE);
    m_multiPicsDlg.ShowWindow(FALSE);
    int CurSel = m_tab.GetCurSel();
    switch(CurSel)
    {
       case 0:
           m_mulDatProcDlg.ShowWindow(TRUE);
           break;
       case 1:
           m_singleDatViewDlg.ShowWindow(TRUE);
           break;
       case 2:
           m_multiPicsDlg.ShowWindow(TRUE);
           break;
       default:
           break;
           ;
    }
    *pResult = 0;
}
void CDatAnalystDlg::OnSize(UINT nType, int cx, int cy)
{
    CDialog::OnSize(nType, cx, cy);
    if(GetDlgItem(IDOK) == NULL)
    {
        if(nType != SIZE_MINIMIZED)
        {
            m_oldCx = cx;
            m_oldCy = cy;
        }
        return;
    }

    LayoutControl(&m_tab, TopLeft, BottomRight, cx, cy);
    LayoutControl(GetDlgItem(IDOK), BottomRight, BottomRight, cx, cy);
    LayoutTabViews();
    if(nType != SIZE_MINIMIZED)
    {
        m_oldCx = cx;
        m_oldCy = cy;
    }
    InvalidateRect(NULL);
}

void CDatAnalystDlg::LayoutTabViews()
{
    CRect rTabClient;
    m_tab.GetClientRect(&rTabClient);
    rTabClient.top += 30;
    rTabClient.left += 5;
    rTabClient.bottom -= 5;
    rTabClient.right -= 5;
    m_mulDatProcDlg.MoveWindow(&rTabClient);
    m_singleDatViewDlg.MoveWindow(&rTabClient);
    m_multiPicsDlg.MoveWindow(&rTabClient);
}

void CDatAnalystDlg::LayoutControl(CWnd* pCtrl, LayoutRef refTopLeft, LayoutRef refBottomRight, int cx, int cy)
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

void CDatAnalystDlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
    lpMMI->ptMinTrackSize.x = 400;   //最小x宽度
    lpMMI->ptMinTrackSize.y = 300;   //最小y高度
    CDialog::OnGetMinMaxInfo(lpMMI);
}
