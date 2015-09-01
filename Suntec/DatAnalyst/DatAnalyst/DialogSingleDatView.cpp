#include "stdafx.h"
#include "DatAnalyst.h"
#include "DialogSingleDatView.h"
#include "MyImageType.h"

#define DAT_SUCCESS 0
/************************************************************************/
IMPLEMENT_DYNAMIC(CDialogSingleDatView, CDialog)

CDialogSingleDatView::CDialogSingleDatView(CWnd* pParent)
: CDialog(CDialogSingleDatView::IDD, pParent)
, m_pGdiplusBitmap(NULL)
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
    ON_BN_CLICKED(IDC_BTN_RESETSIZE, &CDialogSingleDatView::OnBnClickedBtnResetsize)
END_MESSAGE_MAP()

BOOL CDialogSingleDatView::OnInitDialog()
{
    CDialog::OnInitDialog();
    this->SetCurShowPic(0);
    GetDlgItem(IDC_BTN_PREVDAT)->ShowWindow(FALSE);
    GetDlgItem(IDC_BTN_NEXTDAT)->ShowWindow(FALSE);
    GetDlgItem(IDC_BTN_RESETSIZE)->EnableWindow(FALSE);
    return TRUE;
}

void CDialogSingleDatView::OnPaint()
{
    CPaintDC dc(this);
    CRect destRect;
    GetDlgItem(IDC_PICTURE)->GetWindowRect(&destRect);
    ScreenToClient(&destRect);
    if(m_pGdiplusBitmap != NULL)
    {
        Gdiplus::Graphics graphDst(dc.m_hDC);
        graphDst.SetInterpolationMode(Gdiplus::InterpolationModeDefault);
        graphDst.DrawImage(m_pGdiplusBitmap,
            Gdiplus::RectF( destRect.left,
                            destRect.top,
                            destRect.Width(),
                            destRect.Height()),
            0,
            0,
            m_pGdiplusBitmap->GetWidth(),
            m_pGdiplusBitmap->GetHeight(),
            Gdiplus::UnitPixel);
    }
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
    LayoutControl(GetDlgItem(IDC_STATIC_PICINFO), TopRight, BottomRight, cx, cy);
    LayoutControl(GetDlgItem(IDC_STATIC_PICINFO_DETAIL), BottomLeft, BottomRight, cx, cy);
    LayoutControl(GetDlgItem(IDC_PICTURE), TopLeft, BottomRight, cx, cy);

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
        CStringW str2 = CT2CW(strDropFileName);
        m_pGdiplusBitmap = new Gdiplus::Bitmap(str2.GetBuffer());
        GetDlgItem(IDC_BTN_RESETSIZE)->EnableWindow(TRUE);
        GetDlgItem(IDC_STATIC_PICINFO)->SetWindowText(_T("a normal picture."));
        CString strJpgInfo = this->GetPictureInfo();
        GetDlgItem(IDC_STATIC_PICINFO_DETAIL)->SetWindowText(strJpgInfo);
        Invalidate();
    }
    else if(GetBinaryDataType(strDropFileName) == ImageType_Dat)
    {
        int iErr = 0;
        m_datParser.Init(iErr, strDropFileName);
        SetCurShowPic(0);
        LoadBmpFromDatDataByIndex(m_curShowPic);
    }
    return;
}

void CDialogSingleDatView::OnBnClickedBtnPrev()
{
    SetCurShowPic(m_curShowPic - 1);
    LoadBmpFromDatDataByIndex(m_curShowPic);
    UpdateInfoOfCurrentShowingDat();
}

void CDialogSingleDatView::OnBnClickedBtnNext()
{
    SetCurShowPic(m_curShowPic + 1);
    LoadBmpFromDatDataByIndex(m_curShowPic);
    UpdateInfoOfCurrentShowingDat();
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
            CStringW str2 = CT2CW(strDropFileName);
            m_pGdiplusBitmap = new Gdiplus::Bitmap(str2.GetBuffer());
            Gdiplus::Status curStatus = m_pGdiplusBitmap->GetLastStatus();
            if(m_pGdiplusBitmap->GetLastStatus() != Gdiplus::Status::Ok)
            {
                CString strMsg;
                strMsg.Format(_T("Loading image failed: %s."), strDropFileName);
                MessageBox(strMsg);
                return;
            }
            GetDlgItem(IDC_BTN_RESETSIZE)->EnableWindow(TRUE);
            GetDlgItem(IDC_STATIC_PICINFO)->SetWindowText(_T("a normal picture."));
            CString strJpgSize = this->GetPictureInfo();
            GetDlgItem(IDC_STATIC_PICINFO_DETAIL)->SetWindowText(strJpgSize);
            Invalidate();
        }
        else if(imgType == ImageType_Dat)
        {
            int iErr = 0;
            m_datParser.Init(iErr, strDropFileName);
            SetCurShowPic(0);
            LoadBmpFromDatDataByIndex(m_curShowPic);
            UpdateInfoOfCurrentShowingDat();
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
void CDialogSingleDatView::LoadBmpFromDatDataByIndex(short iIdx)
{
    int iErr = 0;
    char* pTemp = NULL;
    m_datParser.GetPicBufferByIndex(iErr, iIdx, &pTemp);
    DatBinType binType = m_datParser.GetPicTypeByIndex(iErr, iIdx);
    if(binType == DatBinType_Pattern || binType == DatBinType_Arrow)
    {
        LoadBmpFromMemory(iErr, (void*)pTemp, m_datParser.GetPicLengthByIndex(iErr, iIdx));
        if(iErr == DAT_SUCCESS)
        {
            GetDlgItem(IDC_BTN_RESETSIZE)->EnableWindow(TRUE);
            Invalidate();
        }
        delete pTemp;
        pTemp = NULL;
    }
    else if(binType == DatBinType_Pointlist)
    {
        std::vector<short> vecCoor = m_datParser.GetPointCoordinateListByIndex(iErr, iIdx);
        for(size_t i=0; i<vecCoor.size(); i+=2)
        {
            short oneX = vecCoor[i];
            short oneY = vecCoor[i+1];
            m_pGdiplusBitmap->SetPixel(oneX, oneY, Gdiplus::Color::Red);
            m_pGdiplusBitmap->SetPixel(oneX+1, oneY, Gdiplus::Color::Red);
            m_pGdiplusBitmap->SetPixel(oneX, oneY+1, Gdiplus::Color::Red);
            m_pGdiplusBitmap->SetPixel(oneX-1, oneY, Gdiplus::Color::Red);
            m_pGdiplusBitmap->SetPixel(oneX, oneY-1, Gdiplus::Color::Red);
        }
        Invalidate();
    }
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
    strPictureInfo.Format(_T("size: %dx%d"),
        m_pGdiplusBitmap->GetWidth(),
        m_pGdiplusBitmap->GetHeight());
    return strPictureInfo;
}

// load image into m_image from memory buffer.
void CDialogSingleDatView::LoadBmpFromMemory(int& iErr, void* pMemData, long len)
{
    HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE | GMEM_NODISCARD, len);
    char *pData = reinterpret_cast<char*>(GlobalLock(hGlobal));
    memcpy(pData, pMemData, len);
    GlobalUnlock(hGlobal);
    IStream *pStream = NULL;
    if(CreateStreamOnHGlobal(hGlobal, TRUE, &pStream) != S_OK)
    {
        return;
    }
    m_pGdiplusBitmap = new Gdiplus::Bitmap(pStream);
    pStream->Release();
}

// reset window size to fit the new image.
void CDialogSingleDatView::ResizeWindowToFitImage()
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

void CDialogSingleDatView::OnBnClickedBtnResetsize()
{
    ResizeWindowToFitImage();
}

void CDialogSingleDatView::UpdateInfoOfCurrentShowingDat()
{
    int iErr = 0;
    CString strCurPicInfo = m_datParser.GetPicInfoByIndex(iErr, m_curShowPic);
    GetDlgItem(IDC_STATIC_PICINFO)->SetWindowText(strCurPicInfo);
    CString strJpgSize = this->GetPictureInfo();
    GetDlgItem(IDC_STATIC_PICINFO_DETAIL)->SetWindowText(strJpgSize);
    Invalidate();
}
