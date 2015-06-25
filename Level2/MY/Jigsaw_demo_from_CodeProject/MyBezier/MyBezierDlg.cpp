
// MyBezierDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MyBezier.h"
#include "MyBezierDlg.h"
#include "afxdialogex.h"
#include <complex>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMyBezierDlg dialog




CMyBezierDlg::CMyBezierDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMyBezierDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMyBezierDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMyBezierDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_DRAW_BEZIER, &CMyBezierDlg::OnBnClickedDrawBezier)
    ON_BN_CLICKED(IDC_BUTTON1, &CMyBezierDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// CMyBezierDlg message handlers

BOOL CMyBezierDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CMyBezierDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

HCURSOR CMyBezierDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CMyBezierDlg::OnBnClickedDrawBezier()
{
    POINT oriPoints[18] = {
        {100,100},
        {135,115},
        {137,105},
        {137,105},
        {140,100},
        {138,95},
        {138,95},
        {120,80},
        {150,80},
        {150,80},
        {180,80},
        {162,95},
        {162,95},
        {160,100},
        {163,105},
        {163,105},
        {165,115},
        {200,100}
    };
    POINT bPoints[50] = {0};
    for(int i=0; i<6; i++)
    {
        for(int nPercent=1; nPercent<=50; nPercent++)
        {
            bPoints[nPercent-1] = GetBezierPoint(oriPoints+i*3, 3, nPercent*10);
        }

        CDC *pdc=GetDC();
        for(int nPercent=0; nPercent<10; nPercent++)
        {
            pdc->MoveTo(bPoints[nPercent]);
            pdc->LineTo(bPoints[nPercent+1]);
        }
        ReleaseDC(pdc);
    }
    return;
}

POINT CMyBezierDlg::GetBezierPoint(POINT* oriPoints, int nPtCount, int nPercent)
{
    double berzierCoe = 0.0f, dX = 0.0f, dY = 0.0f;
    POINT pt = {0, 0};
    for(int i=0; i<nPtCount; i++)
    {
        berzierCoe = GetBezierCoefficient(nPtCount, i, nPercent);
        TRACE(L"%d, %d, %.8f\r\n", i, nPercent, berzierCoe);
        dX += (berzierCoe * oriPoints[i].x);
        dY += (berzierCoe * oriPoints[i].y);
    }
    pt.x = dX;
    pt.y = dY;
    return pt;
}

double CMyBezierDlg::GetBezierCoefficient(int nPtCount, int i, int nPercent)
{
    double dResult = 0.0f;
    int nC = Factorial(nPtCount-1) / Factorial(i) / Factorial(nPtCount-i-1);
    double d1 = pow((double)nPercent/100.0f, i);
    double d2 = pow(1.0f-(double)nPercent/100.0f, nPtCount-i-1);
    dResult = (double)nC * d1 * d2;
    return dResult;
}

// return n!
int CMyBezierDlg::Factorial(int n)
{
    int nResult = 1;
    for(int i=1; i<=n; i++)
    {
        nResult *= i;
    }
    return nResult;
}



void CMyBezierDlg::OnBnClickedButton1()
{
//     HRESULT DemoApp::CreateGridPatternBrush(
//         ID2D1RenderTarget *pRenderTarget,
//         ID2D1BitmapBrush **ppBitmapBrush
//         )
//     {
        // Create a compatible render target.
        ID2D1BitmapRenderTarget *pCompatibleRenderTarget = NULL;
        HRESULT hr = pRenderTarget->CreateCompatibleRenderTarget(
            D2D1::SizeF(10.0f, 10.0f),
            &pCompatibleRenderTarget
            );
        if (SUCCEEDED(hr))
        {
            // Draw a pattern.
            ID2D1SolidColorBrush *pGridBrush = NULL;
            hr = pCompatibleRenderTarget->CreateSolidColorBrush(
                D2D1::ColorF(D2D1::ColorF(0.93f, 0.94f, 0.96f, 1.0f)),
                &pGridBrush
                );
            if (SUCCEEDED(hr))
            {
                pCompatibleRenderTarget->BeginDraw();
                pCompatibleRenderTarget->FillRectangle(D2D1::RectF(0.0f, 0.0f, 10.0f, 1.0f), pGridBrush);
                pCompatibleRenderTarget->FillRectangle(D2D1::RectF(0.0f, 0.1f, 1.0f, 10.0f), pGridBrush);
                pCompatibleRenderTarget->EndDraw();

                // Retrieve the bitmap from the render target.
                ID2D1Bitmap *pGridBitmap = NULL;
                hr = pCompatibleRenderTarget->GetBitmap(&pGridBitmap);
                if (SUCCEEDED(hr))
                {
                    // Choose the tiling mode for the bitmap brush.
                    D2D1_BITMAP_BRUSH_PROPERTIES brushProperties =
                        D2D1::BitmapBrushProperties(D2D1_EXTEND_MODE_WRAP, D2D1_EXTEND_MODE_WRAP);

                    // Create the bitmap brush.
                    hr = m_pRenderTarget->CreateBitmapBrush(pGridBitmap, brushProperties, ppBitmapBrush);

                    pGridBitmap->Release();
                }

                pGridBrush->Release();
            }

            pCompatibleRenderTarget->Release();
        }

//         return hr;
//     }
}
