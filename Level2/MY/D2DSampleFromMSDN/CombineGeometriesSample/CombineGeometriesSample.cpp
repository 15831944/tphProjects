// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved

//+-----------------------------------------------------------------------------
//
//  $Description:
//      Sample Direct2D Application
//
//  $ENDTAG
//
//------------------------------------------------------------------------------

#include "stdafx.h"
#include "CombineGeometriesSample.h"
/******************************************************************
*                                                                 *
*  WinMain                                                        *
*                                                                 *
*  Application entry point                                        *
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
    m_pOutlineBrush(NULL),
    m_pTextBrush(NULL),
    m_pShapeFillBrush(NULL),
    m_pGridPatternBitmapBrush(NULL),
    m_pCircleGeometry1(NULL),
    m_pCircleGeometry2(NULL),
    m_pPathGeometryUnion(NULL),
    m_pPathGeometryIntersect(NULL),
    m_pPathGeometryXOR(NULL),
    m_pPathGeometryExclude(NULL),
    m_pDWriteFactory(NULL),
    m_pTextFormat(NULL),
    m_pStrokeStyle(NULL)
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
    SafeRelease(&m_pOutlineBrush);
    SafeRelease(&m_pTextBrush);
    SafeRelease(&m_pShapeFillBrush);
    SafeRelease(&m_pGridPatternBitmapBrush);
    SafeRelease(&m_pCircleGeometry1);
    SafeRelease(&m_pCircleGeometry2);
    SafeRelease(&m_pPathGeometryUnion);
    SafeRelease(&m_pPathGeometryIntersect);
    SafeRelease(&m_pPathGeometryXOR);
    SafeRelease(&m_pPathGeometryExclude);
    SafeRelease(&m_pDWriteFactory);
    SafeRelease(&m_pTextFormat);
    SafeRelease(&m_pStrokeStyle);
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
            L"Direct2D Combine Geometries",
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            static_cast<UINT>(ceil(640.f * dpiX / 96.f)),
            static_cast<UINT>(ceil(480.f * dpiY / 96.f)),
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
    HRESULT hr;

    // Create a Direct2D factory.
    hr = D2D1CreateFactory(
        D2D1_FACTORY_TYPE_SINGLE_THREADED,
        &m_pD2DFactory
        );

    if (SUCCEEDED(hr))
    {
        // Create a shared DirectWrite factory.
        hr = DWriteCreateFactory(
            DWRITE_FACTORY_TYPE_SHARED,
            __uuidof(IDWriteFactory),
            reinterpret_cast<IUnknown**>(&m_pDWriteFactory)
            );
    }

    if (SUCCEEDED(hr))
    {
        // Create a DirectWrite text format object.
        hr = m_pDWriteFactory->CreateTextFormat(
            L"Verdana",     // The font family name.
            NULL,           // The font collection (NULL sets it to use the system font collection).
            DWRITE_FONT_WEIGHT_REGULAR,
            DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL,
            10.0f,
            L"en-us",
            &m_pTextFormat
            );
    }

    if (SUCCEEDED(hr))
    {
        hr = CreateGeometryResources();
    }

    if (SUCCEEDED(hr))
    {
        float dashes[] = {1.f, 1.f, 2.f, 3.f, 5.f};

        m_pD2DFactory->CreateStrokeStyle(
            D2D1::StrokeStyleProperties(
            D2D1_CAP_STYLE_FLAT,
            D2D1_CAP_STYLE_FLAT,
            D2D1_CAP_STYLE_ROUND,
            D2D1_LINE_JOIN_ROUND,   // lineJoin
            10.f,   //miterLimit
            D2D1_DASH_STYLE_CUSTOM,
            0.f     //dashOffset
            ),
            dashes,
            ARRAYSIZE(dashes)-1,
            &m_pStrokeStyle
            );
    }

    return hr;
}

HRESULT DemoApp::CreateGeometryResources()
{
    HRESULT hr = S_OK;
    ID2D1GeometrySink *pGeometrySink = NULL;

    // Create the first ellipse geometry to merge.
    const D2D1_ELLIPSE circle1 = D2D1::Ellipse(
        D2D1::Point2F(75.0f, 75.0f),
        50.0f,
        50.0f
        );

    hr = m_pD2DFactory->CreateEllipseGeometry(
        circle1,
        &m_pCircleGeometry1
        );

    if (SUCCEEDED(hr))
    {
        // Create the second ellipse geometry to merge.
        const D2D1_ELLIPSE circle2 = D2D1::Ellipse(
            D2D1::Point2F(125.0f, 75.0f),
            50.0f,
            50.0f
            );

        hr = m_pD2DFactory->CreateEllipseGeometry(circle2, &m_pCircleGeometry2);
    }


    if (SUCCEEDED(hr))
    {
        //
        // Use D2D1_COMBINE_MODE_UNION to combine the geometries.
        //
        hr = m_pD2DFactory->CreatePathGeometry(&m_pPathGeometryUnion);

        if (SUCCEEDED(hr))
        {
            hr = m_pPathGeometryUnion->Open(&pGeometrySink);

            if (SUCCEEDED(hr))
            {
                hr = m_pCircleGeometry1->CombineWithGeometry(
                    m_pCircleGeometry2,
                    D2D1_COMBINE_MODE_UNION,
                    NULL,
                    NULL,
                    pGeometrySink
                    );
            }

            if (SUCCEEDED(hr))
            {
                hr = pGeometrySink->Close();
            }

            SafeRelease(&pGeometrySink);
        }
    }

    if (SUCCEEDED(hr))
    {
        //
        // Use D2D1_COMBINE_MODE_INTERSECT to combine the geometries.
        //
        hr = m_pD2DFactory->CreatePathGeometry(&m_pPathGeometryIntersect);

        if (SUCCEEDED(hr))
        {
            hr = m_pPathGeometryIntersect->Open(&pGeometrySink);

            if (SUCCEEDED(hr))
            {
                hr = m_pCircleGeometry1->CombineWithGeometry(
                    m_pCircleGeometry2,
                    D2D1_COMBINE_MODE_INTERSECT,
                    NULL,
                    NULL,
                    pGeometrySink
                    );
            }

            if (SUCCEEDED(hr))
            {
                hr = pGeometrySink->Close();
            }

            SafeRelease(&pGeometrySink);
        }
    }

    if (SUCCEEDED(hr))
    {
        //
        // Use D2D1_COMBINE_MODE_XOR to combine the geometries.
        //
        hr = m_pD2DFactory->CreatePathGeometry(&m_pPathGeometryXOR);

        if (SUCCEEDED(hr))
        {
            hr = m_pPathGeometryXOR->Open(&pGeometrySink);

            if (SUCCEEDED(hr))
            {
                hr = m_pCircleGeometry1->CombineWithGeometry(
                    m_pCircleGeometry2,
                    D2D1_COMBINE_MODE_XOR,
                    NULL,
                    NULL,
                    pGeometrySink
                    );
            }

            if (SUCCEEDED(hr))
            {
                hr = pGeometrySink->Close();
            }

            SafeRelease(&pGeometrySink);
        }
    }

    if (SUCCEEDED(hr))
    {
        //
        // Use D2D1_COMBINE_MODE_EXCLUDE to combine the geometries.
        //
        hr = m_pD2DFactory->CreatePathGeometry(&m_pPathGeometryExclude);

        if (SUCCEEDED(hr))
        {
            hr = m_pPathGeometryExclude->Open(&pGeometrySink);

            if (SUCCEEDED(hr))
            {
                hr = m_pCircleGeometry1->CombineWithGeometry(
                    m_pCircleGeometry2,
                    D2D1_COMBINE_MODE_EXCLUDE,
                    NULL,
                    NULL,
                    pGeometrySink
                    );
            }

            if (SUCCEEDED(hr))
            {
                hr = pGeometrySink->Close();
            }

            SafeRelease(&pGeometrySink);
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
                D2D1::ColorF(D2D1::ColorF::DarkSlateBlue, 1.f),
                &m_pOutlineBrush
                );
        }

        if (SUCCEEDED(hr))
        {
            hr = m_pRenderTarget->CreateSolidColorBrush(
                D2D1::ColorF(D2D1::ColorF::CornflowerBlue, 0.5f),
                &m_pShapeFillBrush
                );
        }

        if (SUCCEEDED(hr))
        {
            hr = m_pRenderTarget->CreateSolidColorBrush(
                D2D1::ColorF(D2D1::ColorF::Black, 1.f),
                &m_pTextBrush
                );
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
            pCompatibleRenderTarget->FillRectangle(D2D1::RectF(0.0f, 0.0f, 1.0f, 10.0f), pGridBrush);
            hr = pCompatibleRenderTarget->EndDraw();

            // Retrieve the bitmap from the render target.
            ID2D1Bitmap *pGridBitmap = NULL;
            hr = pCompatibleRenderTarget->GetBitmap(&pGridBitmap);
            if (SUCCEEDED(hr))
            {
                // Choose the tiling mode for the bitmap brush.
                D2D1_BITMAP_BRUSH_PROPERTIES brushProperties = D2D1::BitmapBrushProperties(
                    D2D1_EXTEND_MODE_WRAP,
                    D2D1_EXTEND_MODE_WRAP
                    );

                // Create the bitmap brush.
                hr = m_pRenderTarget->CreateBitmapBrush(pGridBitmap, brushProperties, ppBitmapBrush);

                pGridBitmap->Release();
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
    SafeRelease(&m_pOutlineBrush);
    SafeRelease(&m_pShapeFillBrush);
    SafeRelease(&m_pGridPatternBitmapBrush);
    SafeRelease(&m_pTextBrush);
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

        static const WCHAR szBeforeText[] = L"The circles before combining";
        static const WCHAR szUnionText[] = L"D2D1_COMBINE_MODE_UNION";
        static const WCHAR szIntersectText[] = L"D2D1_COMBINE_MODE_INTERSECT";
        static const WCHAR szXorText[] = L"D2D1_COMBINE_MODE_XOR";
        static const WCHAR szExcludeText[] = L"D2D1_COMBINE_MODE_EXCLUDE";

        m_pRenderTarget->BeginDraw();
        m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());

        m_pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));

        D2D1_SIZE_F renderTargetSize = m_pRenderTarget->GetSize();

        m_pRenderTarget->FillRectangle(
            D2D1::RectF(0.f, 0.f, renderTargetSize.width, renderTargetSize.height),
            m_pGridPatternBitmapBrush
            );

        // Draw the geomtries before merging.
        m_pRenderTarget->FillGeometry(m_pCircleGeometry1, m_pShapeFillBrush);
        m_pRenderTarget->DrawGeometry(m_pCircleGeometry1, m_pOutlineBrush, 1.0f);
        m_pRenderTarget->FillGeometry(m_pCircleGeometry2, m_pShapeFillBrush);
        m_pRenderTarget->DrawGeometry(m_pCircleGeometry2, m_pOutlineBrush, 1.0f);

        m_pRenderTarget->DrawText(
            szBeforeText,
            ARRAYSIZE(szBeforeText) - 1,
            m_pTextFormat,
            D2D1::RectF(25.0f, 130.0f, 200.0f, 300.0f),
            m_pTextBrush
            );

        m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Translation(200, 0));

        // Draw the geometries merged using the union combine mode.
        m_pRenderTarget->FillGeometry(m_pPathGeometryUnion, m_pShapeFillBrush, NULL);
        m_pRenderTarget->DrawGeometry(m_pPathGeometryUnion, m_pOutlineBrush, 1.0f);

        m_pRenderTarget->DrawText(
            szUnionText,
            ARRAYSIZE(szUnionText) - 1,
            m_pTextFormat,
            D2D1::RectF(25, 130, 200, 300),
            m_pTextBrush
            );

        m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Translation(400, 0));

        // Draw the geometries merged using the intersect combine mode.
        m_pRenderTarget->FillGeometry(m_pPathGeometryIntersect, m_pShapeFillBrush, NULL);
        m_pRenderTarget->DrawGeometry(m_pPathGeometryIntersect, m_pOutlineBrush, 1.0f);

        m_pRenderTarget->DrawText(
            szIntersectText,
            ARRAYSIZE(szIntersectText) - 1,
            m_pTextFormat,
            D2D1::RectF(25, 130, 400, 300),
            m_pTextBrush
            );

        m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Translation(200, 150));

        // Draw the geometries merged using the XOR combine mode.
        m_pRenderTarget->FillGeometry(m_pPathGeometryXOR, m_pShapeFillBrush, NULL);
        m_pRenderTarget->DrawGeometry(m_pPathGeometryXOR, m_pOutlineBrush, 1.0f);

        m_pRenderTarget->DrawText(
            szXorText,
            ARRAYSIZE(szXorText) - 1,
            m_pTextFormat,
            D2D1::RectF(25, 130, 200, 470),
            m_pTextBrush
            );

        m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Translation(400, 150));

        // Draw the geometries merged using the exclude combine mode.
        m_pRenderTarget->FillGeometry(m_pPathGeometryExclude, m_pShapeFillBrush, NULL);
        m_pRenderTarget->DrawGeometry(m_pPathGeometryExclude, m_pOutlineBrush, 1.0f);

        m_pRenderTarget->DrawText(
            szExcludeText,
            ARRAYSIZE(szExcludeText) - 1,
            m_pTextFormat,
            D2D1::RectF(25, 130, 400, 470),
            m_pTextBrush
            );

        // The following code demonstrates how to call various geometric operations. Depending on 
        // your needs, it lets you decide how to use those output values.
        D2D1_GEOMETRY_RELATION result = D2D1_GEOMETRY_RELATION_UNKNOWN;

        // Compare circle1 with circle2
        hr = m_pCircleGeometry1->CompareWithGeometry(
            m_pCircleGeometry2,
            D2D1::IdentityMatrix(),
            0.1f,
            &result
            );

        if (SUCCEEDED(hr))
        {
            static const WCHAR szGeometryRelation[] = L"Two circles overlap.";
            m_pRenderTarget->SetTransform(D2D1::IdentityMatrix());
            if (result == D2D1_GEOMETRY_RELATION_OVERLAP)
            {
                m_pRenderTarget->DrawText(
                    szGeometryRelation,
                    ARRAYSIZE(szGeometryRelation) - 1,
                    m_pTextFormat,
                    D2D1::RectF(25.0f, 160.0f, 200.0f, 300.0f),
                    m_pTextBrush
                    );
            }
        }

        float area;

        // Compute the area of circle1
        hr = m_pCircleGeometry1->ComputeArea(
            D2D1::IdentityMatrix(),
            &area
            );

        float length;

        // Compute the area of circle1
        hr = m_pCircleGeometry1->ComputeLength(
            D2D1::IdentityMatrix(),
            &length
            );

        if (SUCCEEDED(hr))
        {
            // Process the length of the geometry.
        }

        D2D1_POINT_2F point;
        D2D1_POINT_2F tangent;

        hr = m_pCircleGeometry1->ComputePointAtLength(
            10, 
            NULL, 
            &point, 
            &tangent); 

        if (SUCCEEDED(hr))
        {
            // Retrieve the point and tangent point.
        }

        D2D1_RECT_F bounds;

        hr = m_pCircleGeometry1->GetBounds(
            D2D1::IdentityMatrix(),
            &bounds
            );

        if (SUCCEEDED(hr))
        {
            // Retrieve the bounds.
        }

        D2D1_RECT_F bounds1;
        hr = m_pCircleGeometry1->GetWidenedBounds(
            5.0,
            m_pStrokeStyle,
            D2D1::IdentityMatrix(),
            &bounds1
            );
        if (SUCCEEDED(hr))
        {
            // Retrieve the widened bounds.
        }

        BOOL containsPoint;

        hr = m_pCircleGeometry1->StrokeContainsPoint(
            D2D1::Point2F(0,0),
            10,     // stroke width
            NULL,   // stroke style
            NULL,   // world transform
            &containsPoint
            );

        if (SUCCEEDED(hr))
        {
            // Process containsPoint.
        }

        BOOL containsPoint1;
        hr = m_pCircleGeometry1->FillContainsPoint(
            D2D1::Point2F(0,0),
            D2D1::Matrix3x2F::Identity(),
            &containsPoint1
            );

        if (SUCCEEDED(hr))
        {
            // Process containsPoint.
        }


        hr = m_pRenderTarget->EndDraw();
        if (hr == D2DERR_RECREATE_TARGET)
        {
            hr = S_OK;
            DiscardDeviceResources();
        }
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