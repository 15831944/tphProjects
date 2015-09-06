#include "stdafx.h"
#include "DatAnalyst.h"
#include "DialogMultiPics.h"

IMPLEMENT_DYNAMIC(CDialogMultiPics, CDialog)

CDialogMultiPics::CDialogMultiPics(CWnd* pParent)
: CDialog(CDialogMultiPics::IDD, pParent)
{
    m_picLoadedIdx = 0;
}

CDialogMultiPics::~CDialogMultiPics()
{
}

void CDialogMultiPics::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDialogMultiPics, CDialog)
    ON_BN_CLICKED(IDC_BTN_RESET, &CDialogMultiPics::OnBnClickedBtnReset)
    ON_WM_SIZE()
    ON_WM_DROPFILES()
    ON_BN_CLICKED(IDC_BTN_FITSIZE_MULPIC, &CDialogMultiPics::OnBnClickedBtnFitsizeMulpic)
END_MESSAGE_MAP()

BOOL CDialogMultiPics::OnInitDialog()
{
    CDialog::OnInitDialog();
    GetDlgItem(IDC_BTN_FITSIZE_MULPIC)->EnableWindow(FALSE);
    GetDlgItem(IDC_BTN_RESET)->EnableWindow(FALSE);
    return TRUE;
}

void CDialogMultiPics::OnSize(UINT nType, int cx, int cy)
{
    CDialog::OnSize(nType, cx, cy);
    if(GetDlgItem(IDC_BTN_RESET) == NULL)
    {
        if(nType != SIZE_MINIMIZED)
        {
            m_oldCx = cx;
            m_oldCy = cy;
        }
        return;
    }

    LayoutControl(GetDlgItem(IDC_BTN_FITSIZE_MULPIC), TopLeft, TopLeft, cx, cy);
    LayoutControl(GetDlgItem(IDC_BTN_RESET), TopLeft, TopLeft, cx, cy);
    LayoutControl(GetDlgItem(IDC_EDIT_CURFILEPATH_MULPIC), TopLeft, TopRight, cx, cy);
    LayoutControl(GetDlgItem(IDC_PICTURE_MULPIC), TopLeft, BottomRight, cx, cy);
    LayoutControl(GetDlgItem(IDC_STATIC_PICINFO_MULPIC), BottomLeft, BottomRight, cx, cy);

    if(nType != SIZE_MINIMIZED)
    {
        m_oldCx = cx;
        m_oldCy = cy;
    }
    InvalidateRect(NULL);
}

void CDialogMultiPics::LayoutControl(CWnd* pCtrl, LayoutRef refTopLeft, LayoutRef refBottomRight, int cx, int cy)
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

void CDialogMultiPics::OnDropFiles(HDROP hDropInfo)
{
    UINT count;
    count = (int)DragQueryFile(hDropInfo, 0xFFFFFFFF, NULL, 0);
    for(int i=0; i<count; i++)
    {
        CString strDropFile;
        DragQueryFile(hDropInfo, i, strDropFile.GetBuffer(256), 256);
        m_vecCurFilePaths.push_back(strDropFile);
        return;
    }
    DragFinish(hDropInfo);
    ShowAllPictures();
    CDialog::OnDropFiles(hDropInfo);
}

void CDialogMultiPics::OnBnClickedBtnReset()
{
    m_picLoadedIdx = 0;
    //m_pGdiplusBitmap->
}

void CDialogMultiPics::OnBnClickedBtnFitsizeMulpic()
{
    ResizeWindowToFitImage();
}

// load new pictures set in 'm_vecCurFilePaths' and show them.
void CDialogMultiPics::ShowAllPictures()
{
    if(m_picLoadedIdx>(int)m_vecCurFilePaths.size())
    {
        return;
    }

    for(; m_picLoadedIdx<(int)m_vecCurFilePaths.size(); m_picLoadedIdx++)
    {
        
    }
}

// reset window size to fit the new image.
void CDialogMultiPics::ResizeWindowToFitImage()
{
    CWnd* pTheWnd = AfxGetMainWnd();
    CRect winRect, picRect;
    pTheWnd->GetWindowRect(&winRect);
    GetDlgItem(IDC_PICTURE)->GetWindowRect(&picRect);
    int deltaWidth = winRect.Width() - picRect.Width();
    int deltaHeight = winRect.Height() - picRect.Height();
    int x1 = m_pGdiplusBitmap->GetWidth() + deltaWidth;
    int x2 = m_pGdiplusBitmap->GetHeight() + deltaHeight;
    pTheWnd->SetWindowPos(NULL, 0, 0, x1, x2, SWP_NOMOVE);
    return;
}

