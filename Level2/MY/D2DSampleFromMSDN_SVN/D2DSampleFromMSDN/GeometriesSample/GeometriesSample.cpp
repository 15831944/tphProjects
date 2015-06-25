
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved
#include "stdafx.h"
#include "GeometriesSample.h"

/******************************************************************
*                                                                 *
*  WinMain                                                        *
*                                                                 *
*  Application entry point.                                       *
*                                                                 *
******************************************************************/

int WINAPI WinMain(
    HINSTANCE /* hInstance */,
    HINSTANCE /* hPrevInstance */,
    LPSTR /* lpCmdLine */,
    int /* nCmdShow */
    )
{
    // Ignore the return value because we want to run the program even in the
    // unlikely event that HeapSetInformation fails.
    HeapSetInformation(NULL, HeapEnableTerminationOnCorruption, NULL, 0);

    if (SUCCEEDED(CoInitialize(NULL)))
    {
        {
            DemoApp app;

            if (SUCCEEDED(app.Initialize()))
            {
                app.RunMessageLoop();
            }
        }
        CoUninitialize();
    }

    return 0;
}

/******************************************************************
*                                                                 *
*  DemoApp::DemoApp constructor                                   *
*                                                                 *
*  Initialize member data.                                        *
*                                                                 *
******************************************************************/

DemoApp::DemoApp() :
m_hwnd(NULL),
    m_pD2DFactory(NULL),
    m_pRenderTarget(NULL),
    m_pSceneBrush(NULL),
    m_pGridPatternBitmapBrush(NULL),
    m_pLeftMountainGeometry(NULL),
    m_pRightMountainGeometry(NULL),
    m_pSunGeometry(NULL),
    m_pRiverGeometry(NULL),
    m_pRadialGradientBrush(NULL)
{
}

/******************************************************************
*                                                                 *
*  DemoApp::~DemoApp destructor                                   *
*                                                                 *
*  Release resources.                                             *
*                                                                 *
******************************************************************/

DemoApp::~DemoApp()
{
    SafeRelease(&m_pD2DFactory);
    SafeRelease(&m_pRenderTarget);
    SafeRelease(&m_pSceneBrush);
    SafeRelease(&m_pGridPatternBitmapBrush);
    SafeRelease(&m_pLeftMountainGeometry);
    SafeRelease(&m_pRightMountainGeometry);
    SafeRelease(&m_pSunGeometry);
    SafeRelease(&m_pRiverGeometry);
    SafeRelease(&m_pRadialGradientBrush);
}

/******************************************************************
*                                                                 *
*  DemoApp::Initialize                                            *
*                                                                 *
*  Create application window and device-independent resources.    *
*                                                                 *
******************************************************************/

HRESULT DemoApp::Initialize()
{
    HRESULT hr;

    // Initialize device-indpendent resources, such
    // as the Direct2D factory.
    hr = CreateDeviceIndependentResources();
    if (SUCCEEDED(hr))
    {
        // Register the window class.
        WNDCLASSEX wcex = { sizeof(WNDCLASSEX) };
        wcex.style         = CS_HREDRAW | CS_VREDRAW;
        wcex.lpfnWndProc   = DemoApp::WndProc;
        wcex.cbClsExtra    = 0;
        wcex.cbWndExtra    = sizeof(LONG_PTR);
        wcex.hInstance     = HINST_THISCOMPONENT;
        wcex.hbrBackground = NULL;
        wcex.lpszMenuName  = NULL;
        wcex.hCursor       = LoadCursor(NULL, IDC_ARROW);
        wcex.lpszClassName = L"D2DDemoApp";

        RegisterClassEx(&wcex);

        // Create the application window.
        //
        // Because the CreateWindow function takes its size in pixels, we
        // obtain the system DPI and use it to scale the window size.
        FLOAT dpiX, dpiY;
        m_pD2DFactory->GetDesktopDpi(&dpiX, &dpiY);

        m_hwnd = CreateWindow(
            L"D2DDemoApp",
            L"Direct2D Geometries Examples",
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            static_cast<UINT>(ceil(840.f * dpiX / 96.f)),
            static_cast<UINT>(ceil(700.f * dpiY / 96.f)),
            NULL,
            NULL,
            HINST_THISCOMPONENT,
            this
            );

        hr = m_hwnd ? S_OK : E_FAIL;
        if (SUCCEEDED(hr))
        {
            ShowWindow(m_hwnd, SW_SHOWNORMAL);
            UpdateWindow(m_hwnd);
        }
    }

    return hr;
}

/******************************************************************
*                                                                 *
*  DemoApp::CreateDeviceIndependentResources                      *
*                                                                 *
*  This method is used to create resources which are not bound    *
*  to any device. Their lifetime effectively extends for the      *
*  duration of the app.                                           *
*                                                                 *
******************************************************************/

HRESULT DemoApp::CreateDeviceIndependentResources()
{

    // Create a Direct2D factory.
    HRESULT hr = D2D1CreateFactory(
        D2D1_FACTORY_TYPE_SINGLE_THREADED,
        &m_pD2DFactory
        );

    if(SUCCEEDED(hr))
    {
        hr = m_pD2DFactory->CreatePathGeometry(&m_pLeftMountainGeometry);
        if(SUCCEEDED(hr))
        {
            ID2D1GeometrySink *pSink = NULL;
            hr = m_pLeftMountainGeometry->Open(&pSink);
            if (SUCCEEDED(hr))
            {
                pSink->SetFillMode(D2D1_FILL_MODE_WINDING);

                pSink->BeginFigure(
                    D2D1::Point2F(346,255),
                    D2D1_FIGURE_BEGIN_FILLED
                    );
                D2D1_POINT_2F points[5] = {
                    D2D1::Point2F(267, 177),
                    D2D1::Point2F(236, 192),
                    D2D1::Point2F(212, 160),
                    D2D1::Point2F(156, 255),
                    D2D1::Point2F(346, 255), 
                };
                pSink->AddLines(points, ARRAYSIZE(points));
                pSink->EndFigure(D2D1_FIGURE_END_CLOSED);

            }
            hr = pSink->Close();

            SafeRelease(&pSink);
        }
    }

    if(SUCCEEDED(hr))
    {
        hr = m_pD2DFactory->CreatePathGeometry(&m_pRightMountainGeometry);
        if(SUCCEEDED(hr))
        {
            ID2D1GeometrySink *pSink = NULL;

            hr = m_pRightMountainGeometry->Open(&pSink);
            if (SUCCEEDED(hr))
            {
                pSink->SetFillMode(D2D1_FILL_MODE_WINDING);

                pSink->BeginFigure(
                    D2D1::Point2F(575,263),
                    D2D1_FIGURE_BEGIN_FILLED
                    );
                D2D1_POINT_2F points[] = {
                    D2D1::Point2F(481, 146),
                    D2D1::Point2F(449, 181),
                    D2D1::Point2F(433, 159),
                    D2D1::Point2F(401, 214),
                    D2D1::Point2F(381, 199), 
                    D2D1::Point2F(323, 263), 
                    D2D1::Point2F(575, 263)
                };
                pSink->AddLines(points, ARRAYSIZE(points));
                pSink->EndFigure(D2D1_FIGURE_END_CLOSED);
            }
            hr = pSink->Close();

            SafeRelease(&pSink);
        }
    }

    if(SUCCEEDED(hr))
    {

        hr = m_pD2DFactory->CreatePathGeometry(&m_pSunGeometry);
        if(SUCCEEDED(hr))
        {
            ID2D1GeometrySink *pSink = NULL;

            hr = m_pSunGeometry->Open(&pSink);
            if (SUCCEEDED(hr))
            {
                pSink->SetFillMode(D2D1_FILL_MODE_WINDING);

                pSink->BeginFigure(
                    D2D1::Point2F(270, 255),
                    D2D1_FIGURE_BEGIN_FILLED
                    );
                pSink->AddArc(
                    D2D1::ArcSegment(
                    D2D1::Point2F(440, 255), // end point
                    D2D1::SizeF(85, 85),
                    0.0f, // rotation angle
                    D2D1_SWEEP_DIRECTION_CLOCKWISE,
                    D2D1_ARC_SIZE_SMALL
                    ));            
                pSink->EndFigure(D2D1_FIGURE_END_CLOSED);

                pSink->BeginFigure(
                    D2D1::Point2F(299, 182),
                    D2D1_FIGURE_BEGIN_HOLLOW
                    );
                pSink->AddBezier(
                    D2D1::BezierSegment(
                    D2D1::Point2F(299, 182),
                    D2D1::Point2F(294, 176),
                    D2D1::Point2F(285, 178)
                    ));
                pSink->AddBezier(
                    D2D1::BezierSegment(
                    D2D1::Point2F(276, 179),
                    D2D1::Point2F(272, 173),
                    D2D1::Point2F(272, 173)
                    ));
                pSink->EndFigure(D2D1_FIGURE_END_OPEN);

                pSink->BeginFigure(
                    D2D1::Point2F(354, 156),
                    D2D1_FIGURE_BEGIN_HOLLOW
                    );
                pSink->AddBezier(
                    D2D1::BezierSegment(
                    D2D1::Point2F(354, 156),
                    D2D1::Point2F(358, 149),
                    D2D1::Point2F(354, 142)
                    ));
                pSink->AddBezier(
                    D2D1::BezierSegment(
                    D2D1::Point2F(349, 134),
                    D2D1::Point2F(354, 127),
                    D2D1::Point2F(354, 127)
                    ));
                pSink->EndFigure(D2D1_FIGURE_END_OPEN);

                pSink->BeginFigure(
                    D2D1::Point2F(322,164),
                    D2D1_FIGURE_BEGIN_HOLLOW
                    );
                pSink->AddBezier(
                    D2D1::BezierSegment(
                    D2D1::Point2F(322, 164),
                    D2D1::Point2F(322, 156),
                    D2D1::Point2F(314, 152)
                    ));
                pSink->AddBezier(
                    D2D1::BezierSegment(
                    D2D1::Point2F(306, 149),
                    D2D1::Point2F(305, 141),
                    D2D1::Point2F(305, 141)
                    ));              
                pSink->EndFigure(D2D1_FIGURE_END_OPEN);

                pSink->BeginFigure(
                    D2D1::Point2F(385, 164),
                    D2D1_FIGURE_BEGIN_HOLLOW
                    );
                pSink->AddBezier(
                    D2D1::BezierSegment(
                    D2D1::Point2F(385,164),
                    D2D1::Point2F(392,161),
                    D2D1::Point2F(394,152)
                    ));
                pSink->AddBezier(
                    D2D1::BezierSegment(
                    D2D1::Point2F(395,144),
                    D2D1::Point2F(402,141),
                    D2D1::Point2F(402,142)
                    ));                
                pSink->EndFigure(D2D1_FIGURE_END_OPEN);

                pSink->BeginFigure(
                    D2D1::Point2F(408,182),
                    D2D1_FIGURE_BEGIN_HOLLOW
                    );
                pSink->AddBezier(
                    D2D1::BezierSegment(
                    D2D1::Point2F(408,182),
                    D2D1::Point2F(416,184),
                    D2D1::Point2F(422,178)
                    ));
                pSink->AddBezier(
                    D2D1::BezierSegment(
                    D2D1::Point2F(428,171),
                    D2D1::Point2F(435,173),
                    D2D1::Point2F(435,173)
                    ));
                pSink->EndFigure(D2D1_FIGURE_END_OPEN);
            }
            hr = pSink->Close();

            SafeRelease(&pSink);
        }
    }

    if(SUCCEEDED(hr))
    {
        hr = m_pD2DFactory->CreatePathGeometry(&m_pRiverGeometry);

        if(SUCCEEDED(hr))
        {
            ID2D1GeometrySink *pSink = NULL;

            hr = m_pRiverGeometry->Open(&pSink);
            if (SUCCEEDED(hr))
            {
                pSink->SetFillMode(D2D1_FILL_MODE_WINDING);
                pSink->BeginFigure(
                    D2D1::Point2F(183, 392),
                    D2D1_FIGURE_BEGIN_FILLED
                    );
                pSink->AddBezier(
                    D2D1::BezierSegment(
                    D2D1::Point2F(238, 284),
                    D2D1::Point2F(472, 345),
                    D2D1::Point2F(356, 303)
                    ));
                pSink->AddBezier(
                    D2D1::BezierSegment(
                    D2D1::Point2F(237, 261),
                    D2D1::Point2F(333, 256),
                    D2D1::Point2F(333, 256)
                    ));
                pSink->AddBezier(
                    D2D1::BezierSegment(
                    D2D1::Point2F(335, 257),
                    D2D1::Point2F(241, 261),
                    D2D1::Point2F(411, 306)
                    ));
                pSink->AddBezier(
                    D2D1::BezierSegment(
                    D2D1::Point2F(574, 350),
                    D2D1::Point2F(288, 324),
                    D2D1::Point2F(296, 392)
                    ));
                pSink->EndFigure(D2D1_FIGURE_END_OPEN);
            }
            hr = pSink->Close();
            SafeRelease(&pSink);
        }
    }
    return hr;
}

/******************************************************************
*                                                                 *
*  DemoApp::CreateDeviceResources                                 *
*                                                                 *
*  This method creates resources which are bound to a particular  *
*  D3D device. It's all centralized here, in case the resources   *
*  need to be recreated in case of D3D device loss (eg. display   *
*  change, remoting, removal of video card, etc).                 *
*                                                                 *
******************************************************************/

HRESULT DemoApp::CreateDeviceResources()
{
    HRESULT hr = S_OK;

    if (!m_pRenderTarget)
    {
        RECT rc;
        GetClientRect(m_hwnd, &rc);

        D2D1_SIZE_U size = D2D1::SizeU(
            rc.right - rc.left,
            rc.bottom - rc.top
            );

        // Create a Direct2D render target.
        hr = m_pD2DFactory->CreateHwndRenderTarget(
            D2D1::RenderTargetProperties(),
            D2D1::HwndRenderTargetProperties(m_hwnd, size),
            &m_pRenderTarget
            );

        if (SUCCEEDED(hr))
        {
            hr = m_pRenderTarget->CreateSolidColorBrush(
                D2D1::ColorF(D2D1::ColorF::Black, 1.f),
                &m_pSceneBrush
                );
        }

        if (SUCCEEDED(hr))
        {
            // Create an array of gradient stops to put in the gradient stop
            // collection that will be used in the gradient brush.
            ID2D1GradientStopCollection *pGradientStops = NULL;

            D2D1_GRADIENT_STOP gradientStops[3];
            gradientStops[0].color = D2D1::ColorF(D2D1::ColorF::Gold, 1);
            gradientStops[0].position = 0.0f;
            gradientStops[1].color = D2D1::ColorF(D2D1::ColorF::Orange, 0.8);
            gradientStops[1].position = 0.85f;
            gradientStops[2].color = D2D1::ColorF(D2D1::ColorF::OrangeRed, 0.7);
            gradientStops[2].position = 1.0f;

            // Create the ID2D1GradientStopCollection from a previously
            // declared array of D2D1_GRADIENT_STOP structs.
            hr = m_pRenderTarget->CreateGradientStopCollection(
                gradientStops,
                3,
                D2D1_GAMMA_2_2,
                D2D1_EXTEND_MODE_CLAMP,
                &pGradientStops
                );

            // The center of the gradient is in the center of the box.
            // The gradient origin offset was set to zero(0, 0) or center in this case.
            if (SUCCEEDED(hr))
            {
                hr = m_pRenderTarget->CreateRadialGradientBrush(
                    D2D1::RadialGradientBrushProperties(
                    D2D1::Point2F(330, 330),
                    D2D1::Point2F(140, 140),
                    140,
                    140),
                    pGradientStops,
                    &m_pRadialGradientBrush
                    );
            }
        }

        if (SUCCEEDED(hr))
        {
            hr = CreateGridPatternBrush(m_pRenderTarget, &m_pGridPatternBitmapBrush);
        }

    }
    return hr;
}

HRESULT DemoApp::CreateGridPatternBrush(
    ID2D1RenderTarget *pRenderTarget,
    ID2D1BitmapBrush **ppBitmapBrush
    )
{
    HRESULT hr = S_OK;

    // Create a compatible render target.
    ID2D1BitmapRenderTarget *pCompatibleRenderTarget = NULL;
    hr = pRenderTarget->CreateCompatibleRenderTarget(
        D2D1::SizeF(10, 10),
        &pCompatibleRenderTarget
        );

    if (SUCCEEDED(hr))
    {
        // Draw a pattern.
        ID2D1SolidColorBrush *pGridBrush = NULL;
        hr = pCompatibleRenderTarget->CreateSolidColorBrush(
            D2D1::ColorF(D2D1::ColorF(0.93f, 0.94f, 0.96f, 1)),
            &pGridBrush
            );

        if (SUCCEEDED(hr))
        {
            pCompatibleRenderTarget->BeginDraw();
            pCompatibleRenderTarget->FillRectangle(D2D1::RectF(0, 0, 10, 1), pGridBrush);
            pCompatibleRenderTarget->FillRectangle(D2D1::RectF(0, 0, 1, 10), pGridBrush);
            hr = pCompatibleRenderTarget->EndDraw();

            if (hr == D2DERR_RECREATE_TARGET)
            {
                // Discard all of the domain resources on this error.
                DiscardDeviceResources();
            }
            if (SUCCEEDED(hr))
            {
                // Retrieve the bitmap from the render target.
                ID2D1Bitmap *pGridBitmap = NULL;
                hr = pCompatibleRenderTarget->GetBitmap(&pGridBitmap);

                if (SUCCEEDED(hr))
                {
                    // Create the bitmap brush.
                    hr = m_pRenderTarget->CreateBitmapBrush(
                        pGridBitmap,
                        D2D1::BitmapBrushProperties(
                        D2D1_EXTEND_MODE_WRAP,
                        D2D1_EXTEND_MODE_WRAP),
                        ppBitmapBrush
                        );

                    pGridBitmap->Release();
                }
            }

            pGridBrush->Release();
        }

        pCompatibleRenderTarget->Release();
    }

    return hr;
}

/******************************************************************
*                                                                 *
*  DemoApp::DiscardDeviceResources                                *
*                                                                 *
*  Discard device-specific resources which need to be recreated   *
*  when a Direct3D device is lost.                                *
*                                                                 *
******************************************************************/

void DemoApp::DiscardDeviceResources()
{
    SafeRelease(&m_pRenderTarget);
    SafeRelease(&m_pSceneBrush);
    SafeRelease(&m_pGridPatternBitmapBrush);
    SafeRelease(&m_pRadialGradientBrush);
}

/******************************************************************
*                                                                 *
*  DemoApp::RunMessageLoop                                        *
*                                                                 *
*  Main window message loop                                       *
*                                                                 *
******************************************************************/

void DemoApp::RunMessageLoop()
{
    MSG msg;

    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

/******************************************************************
*                                                                 *
*  DemoApp::OnRender                                              *
*                                                                 *
*  Called whenever the application needs to display the client    *
*  window.                                                        *
*                                                                 *
*  Note that this function will automatically discard             *
*  device-specific resources if the Direct3D device disappears    *
*  during execution, and will recreate the resources the          *
*  next time it's invoked.                                        *
*                                                                 *
******************************************************************/

HRESULT DemoApp::OnRender()
{
    HRESULT hr = CreateDeviceResources();

    if (SUCCEEDED(hr))
    {
        m_pRenderTarget->BeginDraw();

        m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());

        m_pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));

        D2D1_SIZE_F rtSize = m_pRenderTarget->GetSize();
        m_pRenderTarget->FillRectangle(
            D2D1::RectF(0, 0, rtSize.width, rtSize.height),
            m_pGridPatternBitmapBrush
            );

        m_pRenderTarget->FillGeometry(m_pSunGeometry, m_pRadialGradientBrush);

        m_pSceneBrush->SetColor(D2D1::ColorF(D2D1::ColorF::Black, 1.f));
        m_pRenderTarget->DrawGeometry(m_pSunGeometry, m_pSceneBrush, 1.f);

        m_pSceneBrush->SetColor(D2D1::ColorF(D2D1::ColorF::OliveDrab, 1.f));
        m_pRenderTarget->FillGeometry(m_pLeftMountainGeometry, m_pSceneBrush);

        m_pSceneBrush->SetColor(D2D1::ColorF(D2D1::ColorF::Black, 1.f));
        m_pRenderTarget->DrawGeometry(m_pLeftMountainGeometry, m_pSceneBrush, 1.f);

        m_pSceneBrush->SetColor(D2D1::ColorF(D2D1::ColorF::LightSkyBlue, 1.f));
        m_pRenderTarget->FillGeometry(m_pRiverGeometry, m_pSceneBrush);

        m_pSceneBrush->SetColor(D2D1::ColorF(D2D1::ColorF::Black, 1.f));
        m_pRenderTarget->DrawGeometry(m_pRiverGeometry, m_pSceneBrush, 1.f);

        m_pSceneBrush->SetColor(D2D1::ColorF(D2D1::ColorF::YellowGreen, 1.f));
        m_pRenderTarget->FillGeometry(m_pRightMountainGeometry, m_pSceneBrush);

        m_pSceneBrush->SetColor(D2D1::ColorF(D2D1::ColorF::Black, 1.f));
        m_pRenderTarget->DrawGeometry(m_pRightMountainGeometry, m_pSceneBrush, 1.f);


        hr = m_pRenderTarget->EndDraw();

    }

    if (hr == D2DERR_RECREATE_TARGET)
    {
        hr = S_OK;
        DiscardDeviceResources();
    }
    return hr;
}

/******************************************************************
*                                                                 *
*  DemoApp::OnResize                                              *
*                                                                 *
*  If the application receives a WM_SIZE message, this method     *
*  resizes the render target appropriately.                       *
*                                                                 *
******************************************************************/

void DemoApp::OnResize(UINT width, UINT height)
{
    if (m_pRenderTarget)
    {
        // Note: This method can fail, but it's okay to ignore the
        // error here -- the error will be repeated on the next call to
        // EndDraw.
        m_pRenderTarget->Resize(D2D1::SizeU(width, height));
    }
}

/******************************************************************
*                                                                 *
*  DemoApp::WndProc                                               *
*                                                                 *
*  Window message handler                                         *
*                                                                 *
******************************************************************/

LRESULT CALLBACK DemoApp::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    LRESULT result = 0;

    if (message == WM_CREATE)
    {
        LPCREATESTRUCT pcs = (LPCREATESTRUCT)lParam;
        DemoApp *pDemoApp = (DemoApp *)pcs->lpCreateParams;

        ::SetWindowLongPtrW(
            hwnd,
            GWLP_USERDATA,
            PtrToUlong(pDemoApp)
            );

        result = 1;
    }
    else
    {
        DemoApp *pDemoApp = reinterpret_cast<DemoApp *>(static_cast<LONG_PTR>(
            ::GetWindowLongPtrW(
            hwnd,
            GWLP_USERDATA
            )));

        bool wasHandled = false;

        if (pDemoApp)
        {
            switch (message)
            {
            case WM_SIZE:
                {
                    UINT width = LOWORD(lParam);
                    UINT height = HIWORD(lParam);
                    pDemoApp->OnResize(width, height);
                }
                wasHandled = true;
                result = 0;
                break;

            case WM_DISPLAYCHANGE:
                {
                    InvalidateRect(hwnd, NULL, FALSE);
                }
                wasHandled = true;
                result = 0;
                break;

            case WM_PAINT:
                {
                    pDemoApp->OnRender();
                    ValidateRect(hwnd, NULL);
                }
                wasHandled = true;
                result = 0;
                break;

            case WM_DESTROY:
                {
                    PostQuitMessage(0);
                }
                wasHandled = true;
                result = 1;
                break;
            }
        }

        if (!wasHandled)
        {
            result = DefWindowProc(hwnd, message, wParam, lParam);
        }
    }

    return result;
}