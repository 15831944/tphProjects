#include "stdafx.h"
#include "TermPlan.h"
#include "DlgAircraftEntrySpec.h"
#include "afxdialogex.h"


IMPLEMENT_DYNAMIC(CDlgAircraftEntrySpec, CDialog)

CDlgAircraftEntrySpec::CDlgAircraftEntrySpec(CWnd* pParent)
 : CDialog(CDlgAircraftEntrySpec::IDD, pParent)
{
}

CDlgAircraftEntrySpec::~CDlgAircraftEntrySpec()
{
}

void CDlgAircraftEntrySpec::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_TREE_ENTRYPROC, m_treeProcs);
    DDX_Control(pDX, IDC_LIST_PAXTYPE, m_listPaxTYpe);
}


BEGIN_MESSAGE_MAP(CDlgAircraftEntrySpec, CDialog)
    ON_WM_SIZE()
    ON_WM_GETMINMAXINFO()
END_MESSAGE_MAP()

void CDlgAircraftEntrySpec::OnSize(UINT nType, int cx, int cy)
{
    CDialog::OnSize(nType, cx, cy);
    CWnd* pCtrl = GetDlgItem(IDOK);
    if(pCtrl == NULL)
    {
        if(nType != SIZE_MINIMIZED)
        {
            m_oldCx = cx;
            m_oldCy = cy;
        }
        return;
    }

    LayoutControl(&m_treeProcs, TopLeft,  BottomLeft, cx, cy);
    LayoutControl(&m_listPaxTYpe, TopLeft,  BottomRight, cx, cy);
    LayoutControl(GetDlgItem(IDC_GROUPBOX_ENTRYPROC), TopLeft,  BottomLeft, cx, cy);
    LayoutControl(GetDlgItem(IDC_GROUPBOX_PAXTYPE), TopLeft,  BottomRight, cx, cy);
    LayoutControl(GetDlgItem(IDC_BTN_SAVE), BottomRight,  BottomRight, cx, cy);
    LayoutControl(GetDlgItem(IDOK), BottomRight,  BottomRight, cx, cy);
    LayoutControl(GetDlgItem(IDCANCEL), BottomRight,  BottomRight, cx, cy);
    if(nType != SIZE_MINIMIZED)
    {
        m_oldCx = cx;
        m_oldCy = cy;
    }
    InvalidateRect(NULL);
}
void CDlgAircraftEntrySpec::LayoutControl(CWnd* pCtrl, LayoutRef refTopLeft, LayoutRef refBottomRight, int cx, int cy)
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

void CDlgAircraftEntrySpec::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
    lpMMI->ptMinTrackSize.x = 400;
    lpMMI->ptMinTrackSize.y = 220;
    CDialog::OnGetMinMaxInfo(lpMMI);
}

