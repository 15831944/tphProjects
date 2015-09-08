#include "stdafx.h"
#include "DatAnalyst.h"
#include "DialogMultiPics.h"
#include "MyImageType.h"
#include "MyGdiPlus.h"
#include "MyError.h"
#include "DatParser.h"

IMPLEMENT_DYNAMIC(CDialogMultiPics, CDialog)

CDialogMultiPics::CDialogMultiPics(CWnd* pParent)
: CDialog(CDialogMultiPics::IDD, pParent)
{
    m_pGdiplusBitmap = NULL;
}

CDialogMultiPics::~CDialogMultiPics()
{
}

void CDialogMultiPics::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDialogMultiPics, CDialog)
    ON_WM_SIZE()
    ON_WM_PAINT()
    ON_WM_DROPFILES()
    ON_BN_CLICKED(IDC_BTN_RESET, &CDialogMultiPics::OnBnClickedBtnReset)
    ON_BN_CLICKED(IDC_BTN_FITSIZE_MULPIC, &CDialogMultiPics::OnBnClickedBtnFitsizeMulpic)
END_MESSAGE_MAP()

BOOL CDialogMultiPics::OnInitDialog()
{
    CDialog::OnInitDialog();
    GetDlgItem(IDC_BTN_FITSIZE_MULPIC)->EnableWindow(FALSE);
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
    LayoutControl(GetDlgItem(IDC_STATIC_PICINFO_DETAIL_MULTIPIC), TopRight, BottomRight, cx, cy);

    if(nType != SIZE_MINIMIZED)
    {
        m_oldCx = cx;
        m_oldCy = cy;
    }
    InvalidateRect(NULL);
}

void CDialogMultiPics::OnPaint()
{
    CPaintDC dc(this);
    CRect destRect;
    GetDlgItem(IDC_PICTURE_MULPIC)->GetWindowRect(&destRect);
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
        ImageType imgType = GetBinaryDataType(strDropFile);
        if(imgType == ImageType_Jpg || 
            imgType == ImageType_Png ||
            imgType == ImageType_Dat)
        {
            m_vecCurFilePaths.push_back(strDropFile);
            GetDlgItem(IDC_BTN_FITSIZE_MULPIC)->EnableWindow(TRUE);
        }
    }
    DragFinish(hDropInfo);
    ShowAllPictures();
    CString strImgNames = GetPictureNameListString();
    GetDlgItem(IDC_STATIC_PICINFO_DETAIL_MULTIPIC)->SetWindowText(strImgNames);
    CDialog::OnDropFiles(hDropInfo);
    return;
}

void CDialogMultiPics::OnBnClickedBtnReset()
{
    m_vecCurFilePaths.clear();
    m_pGdiplusBitmap = NULL;
    GetDlgItem(IDC_BTN_FITSIZE_MULPIC)->EnableWindow(FALSE);
    GetDlgItem(IDC_STATIC_PICINFO_DETAIL_MULTIPIC)->SetWindowText(_T(""));
    Invalidate();
}

void CDialogMultiPics::OnBnClickedBtnFitsizeMulpic()
{
    ResizeWindowToFitImage();
}

// load new pictures set in 'm_vecCurFilePaths' and show them.
void CDialogMultiPics::ShowAllPictures()
{
    if(m_vecCurFilePaths.empty())
    {
        return;
    }

    int iErr = DAT_SUCCESS;
    ImageType imgType = ImageType_Invalid;
    imgType = GetBinaryDataType(m_vecCurFilePaths[0]);
    // draw the 1st file's content.
    if(imgType == ImageType_Jpg || imgType == ImageType_Png)
    {
        MyGdiPlus::LoadBitmapFromFile(iErr, std::string(m_vecCurFilePaths[0]), &m_pGdiplusBitmap);
    }
    else if(imgType == ImageType_Dat)
    {
        DatParser datParser;
        datParser.Init(iErr, m_vecCurFilePaths[0]);
        char* pTemp = NULL;
        datParser.GetPicBufferByIndex(iErr, 0, &pTemp);
        MyGdiPlus::LoadBitmapFromMemory(iErr, pTemp, datParser.GetPicLengthByIndex(iErr, 0), &m_pGdiplusBitmap);
        delete pTemp;
        pTemp = NULL;
        if(datParser.HasPointlist() == true)
        {
            int iIdx = datParser.GetPointlistIndex();
            std::vector<short> vecCoor = datParser.GetPointCoordinateListByIndex(iErr, iIdx);
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
        }
    }
    else
    {
        return;
    }

    // then append the left files' content.
    for(int i=1; i<(int)m_vecCurFilePaths.size(); i++)
    {
        imgType = GetBinaryDataType(m_vecCurFilePaths[i]);
        if(imgType == ImageType_Jpg || imgType == ImageType_Png)
        {
            Gdiplus::Bitmap* pNewBmp = NULL;
            MyGdiPlus::LoadBitmapFromFile(iErr, std::string(m_vecCurFilePaths[i]), &pNewBmp);
            if(iErr != DAT_SUCCESS)
            {
                MessageBox(_T("hehehe, some error, you'd better find what's wrong."));
                return;
            }
            Gdiplus::Graphics* pNewG = Gdiplus::Graphics::FromImage(m_pGdiplusBitmap);
            pNewG->DrawImage(pNewBmp, 0, 0, pNewBmp->GetWidth(), pNewBmp->GetHeight());
        }
        else if(imgType == ImageType_Dat)
        {
            DatParser datParser;
            datParser.Init(iErr, m_vecCurFilePaths[i]);
            char* pTemp = NULL;
            datParser.GetPicBufferByIndex(iErr, 0, &pTemp);
            Gdiplus::Bitmap* pNewBmp = NULL;
            MyGdiPlus::LoadBitmapFromMemory(iErr, pTemp, datParser.GetPicLengthByIndex(iErr, 0), &pNewBmp);
            delete pTemp;
            pTemp = NULL;
            if(iErr != DAT_SUCCESS)
            {
                MessageBox(_T("hehehe, some error, you'd better find what's wrong."));
                return;
            }
            Gdiplus::Graphics* pNewG = Gdiplus::Graphics::FromImage(m_pGdiplusBitmap);
            pNewG->DrawImage(pNewBmp, 0, 0, pNewBmp->GetWidth(), pNewBmp->GetHeight());

            if(datParser.HasPointlist() == true)
            {
                int iIdx = datParser.GetPointlistIndex();
                std::vector<short> vecCoor = datParser.GetPointCoordinateListByIndex(iErr, iIdx);
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
            }
        }
        else
        {
            // do nothing.
        }
    }
    Invalidate();
}

// reset window size to fit the new image.
void CDialogMultiPics::ResizeWindowToFitImage()
{
    CWnd* pTheWnd = AfxGetMainWnd();
    CRect winRect, picRect;
    pTheWnd->GetWindowRect(&winRect);
    GetDlgItem(IDC_PICTURE_MULPIC)->GetWindowRect(&picRect);
    int deltaWidth = winRect.Width() - picRect.Width();
    int deltaHeight = winRect.Height() - picRect.Height();
    int x1 = m_pGdiplusBitmap->GetWidth() + deltaWidth;
    int x2 = m_pGdiplusBitmap->GetHeight() + deltaHeight;
    pTheWnd->SetWindowPos(NULL, 0, 0, x1, x2, SWP_NOMOVE);
    return;
}

CString CDialogMultiPics::GetPictureNameListString()
{
    CString strResult;
    for(size_t i=0; i<m_vecCurFilePaths.size(); i++)
    {
        CString strCurFilePath = m_vecCurFilePaths[i];
        strCurFilePath.ReleaseBuffer();
        CString strName = strCurFilePath.Right(strCurFilePath.GetLength()-1-strCurFilePath.ReverseFind('\\'));
        CString strTemp;
        strTemp.Format(_T("%d: %s\n"), i+1, strName);
        strResult += strTemp;
    }
    return strResult;
}

