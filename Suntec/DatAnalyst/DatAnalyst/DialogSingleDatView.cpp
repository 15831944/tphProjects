#include "stdafx.h"
#include "DatAnalyst.h"
#include "DialogSingleDatView.h"

IMPLEMENT_DYNAMIC(CDialogSingleDatView, CDialog)

CDialogSingleDatView::CDialogSingleDatView(CWnd* pParent /*=NULL*/)
: CDialog(CDialogSingleDatView::IDD, pParent)
{

}

CDialogSingleDatView::~CDialogSingleDatView()
{
}

void CDialogSingleDatView::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDialogSingleDatView, CDialog)
    ON_WM_SIZE()
    ON_WM_PAINT()
    ON_BN_CLICKED(IDC_BTN_PREV, &CDialogSingleDatView::OnBnClickedBtnPrev)
    ON_BN_CLICKED(IDC_BTN_NEXT, &CDialogSingleDatView::OnBnClickedBtnNext)
    ON_BN_CLICKED(IDC_BTN_GETPATH, &CDialogSingleDatView::OnBnClickedBtnGetpath)
    ON_WM_DROPFILES()
END_MESSAGE_MAP()

void CDialogSingleDatView::LayoutControl(CWnd* pCtrl, LayoutRef refTopLeft, LayoutRef refBottomRight, int cx, int cy)
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

void CDialogSingleDatView::OnSize(UINT nType, int cx, int cy)
{
    CDialog::OnSize(nType, cx, cy);
    if(GetDlgItem(IDC_BTN_GETPATH) == NULL)
    {
        if(nType != SIZE_MINIMIZED)
        {
            m_oldCx = cx;
            m_oldCy = cy;
        }
        return;
    }

    LayoutControl(GetDlgItem(IDC_BTN_GETPATH), TopLeft, TopLeft, cx, cy);
    LayoutControl(GetDlgItem(IDC_BTN_PREV), TopLeft, TopLeft, cx, cy);
    LayoutControl(GetDlgItem(IDC_BTN_NEXT), TopLeft, TopLeft, cx, cy);
    LayoutControl(GetDlgItem(IDC_STATIC_PICINFO), TopLeft, TopRight, cx, cy);

    if(nType != SIZE_MINIMIZED)
    {
        m_oldCx = cx;
        m_oldCy = cy;
    }
    InvalidateRect(NULL);
}

void CDialogSingleDatView::OnBnClickedBtnGetpath()
{
    CFileDialog  dlg(TRUE,NULL,NULL,OFN_FILEMUSTEXIST, _T(".dat"));
    if(IDOK!=dlg.DoModal())
        return;
    m_image.Destroy();
    //将外部图像文件装载到CImage对象中
    hResult = m_image.Load(dlg.GetFileName());
    if(FAILED(hResult))
    {
        MessageBox(_T("调用图像文件失败！"));
        return;
    }
    CWnd* pTheWnd = AfxGetMainWnd();
    pTheWnd->SetWindowPos(NULL, 0, 0, m_image.GetWidth() + 33, m_image.GetHeight()+150, SWP_NOMOVE);
    Invalidate();
}

void CDialogSingleDatView::OnPaint()
{
    CPaintDC dc(this);
    CRect recx;
    GetDlgItem(IDC_STATIC_PICINFO)->GetWindowRect(&recx);
    ScreenToClient(&recx); 
    if(!m_image.IsNull())
    {
        m_image.Draw(dc.m_hDC, recx.left, recx.bottom);
    }
}

void CDialogSingleDatView::OnBnClickedBtnPrev()
{
}

void CDialogSingleDatView::OnBnClickedBtnNext()
{
}

void CDialogSingleDatView::OnDropFiles(HDROP hDropInfo)
{
    UINT count;
    CString filePath;
    count = DragQueryFile(hDropInfo, 0xFFFFFFFF, NULL, 0);
    if(count > 0)
    {
        DragQueryFile(hDropInfo, 0, filePath.GetBuffer(256), 256);
        m_image.Destroy();
        //将外部图像文件装载到CImage对象中
        HRESULT hResult = m_image.Load(filePath);
        if(FAILED(hResult))
        {
            MessageBox(_T("调用图像文件失败！"));
            return;
        }
        CWnd* pTheWnd = AfxGetMainWnd();
        pTheWnd->SetWindowPos(NULL, 0, 0, m_image.GetWidth() + 33, m_image.GetHeight()+150, SWP_NOMOVE);
        Invalidate();
    }
    DragFinish(hDropInfo);
    CDialog::OnDropFiles(hDropInfo);
}
