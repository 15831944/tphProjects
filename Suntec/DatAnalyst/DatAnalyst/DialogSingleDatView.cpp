#include "stdafx.h"
#include "DatAnalyst.h"
#include "DialogSingleDatView.h"
#include "MyImageType.h"

#define DAT_SUCCESS 0

IMPLEMENT_DYNAMIC(CDialogSingleDatView, CDialog)

CDialogSingleDatView::CDialogSingleDatView(CWnd* pParent)
: CDialog(CDialogSingleDatView::IDD, pParent)
{

}

CDialogSingleDatView::~CDialogSingleDatView()
{
}

void CDialogSingleDatView::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_BTN_PREV, m_btnPrev);
    DDX_Control(pDX, IDC_BTN_NEXT, m_btnNext);
}


BEGIN_MESSAGE_MAP(CDialogSingleDatView, CDialog)
    ON_WM_SIZE()
    ON_WM_PAINT()
    ON_WM_DROPFILES()
    ON_BN_CLICKED(IDC_BTN_PREV, &CDialogSingleDatView::OnBnClickedBtnPrev)
    ON_BN_CLICKED(IDC_BTN_NEXT, &CDialogSingleDatView::OnBnClickedBtnNext)
    ON_BN_CLICKED(IDC_BTN_GETPATH, &CDialogSingleDatView::OnBnClickedBtnGetpath)
    ON_BN_CLICKED(IDC_BTN_PREVDAT, &CDialogSingleDatView::OnBnClickedBtnPrevdat)
    ON_BN_CLICKED(IDC_BTN_NEXTDAT, &CDialogSingleDatView::OnBnClickedBtnNextdat)
END_MESSAGE_MAP()

BOOL CDialogSingleDatView::OnInitDialog()
{
    CDialog::OnInitDialog();
    this->SetCurShowPic(0);
    GetDlgItem(IDC_BTN_PREVDAT)->ShowWindow(FALSE);
    GetDlgItem(IDC_BTN_NEXTDAT)->ShowWindow(FALSE);
    return TRUE;
}

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
    LayoutControl(GetDlgItem(IDC_BTN_PREVDAT), TopLeft, TopLeft, cx, cy);
    LayoutControl(GetDlgItem(IDC_BTN_NEXTDAT), TopLeft, TopLeft, cx, cy);
    LayoutControl(GetDlgItem(IDC_STATIC_PICINFO), TopLeft, TopRight, cx, cy);
    LayoutControl(GetDlgItem(IDC_STATIC_PICINFO_DETAIL), BottomLeft, BottomRight, cx, cy);

    if(nType != SIZE_MINIMIZED)
    {
        m_oldCx = cx;
        m_oldCy = cy;
    }
    InvalidateRect(NULL);
}

void CDialogSingleDatView::OnBnClickedBtnGetpath()
{
    TCHAR szFilters[]= _T("Dat Files (*.dat)|*.dat|Jpg Files (*.jpg)|*.jpg|Png Files (*.png)|*.png||");
    CFileDialog dlg(TRUE,NULL,NULL,OFN_FILEMUSTEXIST, szFilters);
    if(IDOK!=dlg.DoModal())
        return;
    CString strDropFileName = dlg.GetPathName();
    if(GetBinaryDataType(strDropFileName) == ImageType_Jpg ||
       GetBinaryDataType(strDropFileName) == ImageType_Png)
    {
        LoadNormalImageFromDisk(strDropFileName);
    }
    else if(GetBinaryDataType(strDropFileName) == ImageType_Dat)
    {
        int iErr = 0;
        m_datParser.Init(iErr, strDropFileName);
        SetCurShowPic(0);
        ShowOneBinaryDataInDatByIndex(m_curShowPic);
    }
    return;
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
    SetCurShowPic(m_curShowPic - 1);
    ShowOneBinaryDataInDatByIndex(m_curShowPic);
}

void CDialogSingleDatView::OnBnClickedBtnNext()
{
    SetCurShowPic(m_curShowPic + 1);
    ShowOneBinaryDataInDatByIndex(m_curShowPic);
}

void CDialogSingleDatView::OnDropFiles(HDROP hDropInfo)
{
    UINT count;
    CString strDropFileName;
    count = DragQueryFile(hDropInfo, 0xFFFFFFFF, NULL, 0);
    if(count > 0)
    {
        DragQueryFile(hDropInfo, 0, strDropFileName.GetBuffer(256), 256);
        ImageType imgType = GetBinaryDataType(strDropFileName);
        if(imgType == ImageType_Jpg || imgType == ImageType_Png)
        {
            LoadNormalImageFromDisk(strDropFileName);
        }
        else if(imgType == ImageType_Dat)
        {
            int iErr = 0;
            m_datParser.Init(iErr, strDropFileName);
            SetCurShowPic(0);
            ShowOneBinaryDataInDatByIndex(m_curShowPic);
        }
    }
    DragFinish(hDropInfo);
    CDialog::OnDropFiles(hDropInfo);
}

void CDialogSingleDatView::SetCurShowPic(short iIdx)
{
    m_curShowPic = iIdx;
    m_btnPrev.EnableWindow(FALSE);
    m_btnNext.EnableWindow(FALSE);
    if(m_curShowPic > 0)
    {
        m_btnPrev.EnableWindow(TRUE);
    }

    if(m_curShowPic < ((int)m_datParser.GetPicCount())-1)
    {
        m_btnNext.EnableWindow(TRUE);
    }
}

// show one picture parsed from dat.
void CDialogSingleDatView::ShowOneBinaryDataInDatByIndex(short iIdx)
{
    int iErr = 0;
    char* pTemp = NULL;
    m_datParser.GetPicBufferByIndex(iErr, iIdx, &pTemp);
    DatBinType binType = m_datParser.GetPicTypeByIndex(iErr, iIdx);
    if(binType == DatBinType_Pattern || binType == DatBinType_Arrow)
    {
        m_image.Destroy();
        LoadNormalImageFromMem(iErr, (void*)pTemp, m_datParser.GetPicLengthByIndex(iErr, iIdx));
        if(iErr == DAT_SUCCESS)
        {
            CWnd* pTheWnd = AfxGetMainWnd();
            int winWidth = m_image.GetWidth();
            int winHeight = m_image.GetHeight();
            pTheWnd->SetWindowPos(NULL, 0, 0, winWidth + 33, winHeight+168, SWP_NOMOVE);
            Invalidate();
        }
        delete pTemp;
        pTemp = NULL;
    }
    else if(binType == DatBinType_Pointlist)
    {
        m_image.Destroy();
    }


    CString strCurPicInfo = m_datParser.GetPicInfoByIndex(iErr, iIdx);
    GetDlgItem(IDC_STATIC_PICINFO)->SetWindowText(strCurPicInfo);
    CString strJpgInfo = this->GetPictureInfo();
    GetDlgItem(IDC_STATIC_PICINFO_DETAIL)->SetWindowText(strJpgInfo);
}


void CDialogSingleDatView::OnBnClickedBtnPrevdat()
{
}

void CDialogSingleDatView::OnBnClickedBtnNextdat()
{
}

CString CDialogSingleDatView::GetPictureInfo()
{
    CString strPictureInfo;
    strPictureInfo.Format(_T("size: %dx%d"), m_image.GetWidth(), m_image.GetHeight());
    return strPictureInfo;
}

void CDialogSingleDatView::LoadNormalImageFromDisk(CString strFilePath)
{
    m_image.Destroy();
    HRESULT hResult = m_image.Load(strFilePath);
    if(FAILED(hResult))
    {
        MessageBox(_T("load image from disk failed!"));
        return;
    }
    CWnd* pTheWnd = AfxGetMainWnd();
    pTheWnd->SetWindowPos(NULL, 0, 0, m_image.GetWidth() + 33, m_image.GetHeight()+150, SWP_NOMOVE);
    GetDlgItem(IDC_STATIC_PICINFO)->SetWindowText(_T("a normal picture."));
    CString strJpgInfo = this->GetPictureInfo();
    GetDlgItem(IDC_STATIC_PICINFO_DETAIL)->SetWindowText(strJpgInfo);
    Invalidate();
}

// load image into m_image by memory buffer.
void CDialogSingleDatView::LoadNormalImageFromMem(int& iErr, void* pMemData, long len)
{
    HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, len);
    void* pData = GlobalLock(hGlobal);
    memcpy(pData, pMemData, len);
    GlobalUnlock(hGlobal);
    IStream* pStream = NULL;
    if(CreateStreamOnHGlobal(hGlobal, TRUE, &pStream) == S_OK)
    {
        if(SUCCEEDED(m_image.Load(pStream)))
        {
            iErr = DAT_SUCCESS;
            return;
        }
        pStream -> Release();
    }
    GlobalFree(hGlobal);
}
