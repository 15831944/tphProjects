#include "stdafx.h"

// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved
//+-----------------------------------------------------------------------------
//
//
//  $Description:
//      Sample DirectWrite Application
//
//  $ENDTAG
//
//------------------------------------------------------------------------------

#include "BrushTypesSample.h"

/******************************************************************
*                                                                 *
*  Initialize members.                                            *
*                                                                 *
******************************************************************/

BrushTypesSample::BrushTypesSample() :
m_hwnd(NULL),
    m_pD2DFactory(NULL),
    m_pWICFactory(NULL),
    m_pBitmap(NULL),
    m_pRenderTarget(NULL),
    m_pBlackBrush(NULL),
    m_pYellowGreenBrush(NULL),
    m_pLinearGradientBrush(NULL),
    m_pRadialGradientBrush(NULL),
    m_pBitmapBrush(NULL),
    m_pDWriteFactory(NULL),
    m_pTextFormat(NULL)
{
}

/******************************************************************
*                                                                 *
*  Release resources.                                             *
*                                                                 *
******************************************************************/

BrushTypesSample::~BrushTypesSample()
{
    SafeRelease(&m_pD2DFactory);
    SafeRelease(&m_pWICFactory);
    SafeRelease(&m_pBitmap);
    SafeRelease(&m_pRenderTarget);
    SafeRelease(&m_pBlackBrush);
    SafeRelease(&m_pYellowGreenBrush);
    SafeRelease(&m_pLinearGradientBrush);
    SafeRelease(&m_pRadialGradientBrush);
    SafeRelease(&m_pBitmapBrush);
    SafeRelease(&m_pDWriteFactory);
    SafeRelease(&m_pTextFormat);
}

/*******************************************************************
*                                                                  *
*  Create the application window and device-independent resources. *
*                                                                  *
*******************************************************************/

HRESULT BrushTypesSample::Initialize()
{
    HRESULT hr;

    hr = CreateDeviceIndependentResources();
    if (SUCCEEDED(hr))
    {
        // Register the window class.
        WNDCLASSEX wcex = { sizeof(WNDCLASSEX) };
        wcex.style         = CS_HREDRAW | CS_VREDRAW;
        wcex.lpfnWndProc   = BrushTypesSample::WndProc;
        wcex.cbClsExtra    = 0;
        wcex.cbWndExtra    = sizeof(LONG_PTR);
        wcex.hInstance     = HINST_THISCOMPONENT;
        wcex.hbrBackground = NULL;
        wcex.lpszMenuName  = NULL;
        wcex.hCursor       = LoadCursor(NULL, IDC_ARROW);
        wcex.lpszClassName = L"D2DBrushTypesSample";

        RegisterClassEx(&wcex);
        hr = GetLastError();
        // Create the application window.
        //
        // Because the CreateWindow function takes its size in pixels, we
        // obtain the system DPI and use it to scale the window size.
        FLOAT dpiX, dpiY;
        m_pD2DFactory->GetDesktopDpi(&dpiX, &dpiY);

        m_hwnd = CreateWindow(
            L"D2DBrushTypesSample",
            L"Direct2D Brush Types Sample",
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
        hr = GetLastError();
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
*  This method is used to create resources which are not bound    *
*  to any device. Their lifetime effectively extends for the      *
*  duration of the app.                                           *
*                                                                 *
******************************************************************/
HRESULT BrushTypesSample::CreateDeviceIndependentResources()
{
    HRESULT hr;

    // Create a WIC factory.
    hr = CoCreateInstance(
        CLSID_WICImagingFactory,
        NULL,
        CLSCTX_INPROC_SERVER,
        IID_PPV_ARGS(&m_pWICFactory)
        );

    if (SUCCEEDED(hr))
    {
        // Create a Direct2D factory.
        hr = D2D1CreateFactory(
            D2D1_FACTORY_TYPE_SINGLE_THREADED,
            &m_pD2DFactory
            );
    }

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
            L"Verdana", //font family name
            NULL, //font collection (NULL sets it to use the system font collection)
            DWRITE_FONT_WEIGHT_BOLD,
            DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL,
            10.5f,
            L"en-us",
            &m_pTextFormat
            );
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
HRESULT BrushTypesSample::CreateDeviceResources()
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
                D2D1::ColorF(D2D1::ColorF::Black, 1.0f),
                &m_pBlackBrush
                );
        }

        // Create a solid color brush with its rgb value 0x9ACD32.
        if (SUCCEEDED(hr))
        {
            hr = m_pRenderTarget->CreateSolidColorBrush(
                D2D1::ColorF(D2D1::ColorF(0x9ACD32, 1.0f)),  
                &m_pYellowGreenBrush
                );
        }


        if (SUCCEEDED(hr))
        {
            // Create an array of gradient stops to put in the gradient stop
            // collection that will be used in the gradient brush.
            ID2D1GradientStopCollection *pGradientStops = NULL;

            D2D1_GRADIENT_STOP gradientStops[3];
            gradientStops[0].color = D2D1::ColorF(D2D1::ColorF::Yellow, 1);
            gradientStops[0].position = 0.0f;
            gradientStops[1].color = D2D1::ColorF(D2D1::ColorF::ForestGreen, 1);
            gradientStops[1].position = 1.0f;
            // Create the ID2D1GradientStopCollection from a previously
            // declared array of D2D1_GRADIENT_STOP structs.
            hr = m_pRenderTarget->CreateGradientStopCollection(
                gradientStops,
                2,
                D2D1_GAMMA_2_2,
                D2D1_EXTEND_MODE_CLAMP,
                &pGradientStops
                );

            // The line that determines the direction of the gradient starts at
            // the upper-left corner of the square and ends at the lower-right corner.

            if (SUCCEEDED(hr))
            {
                hr = m_pRenderTarget->CreateLinearGradientBrush(
                    D2D1::LinearGradientBrushProperties(
                    D2D1::Point2F(0, 0),
                    D2D1::Point2F(150, 150)),
                    pGradientStops,
                    &m_pLinearGradientBrush
                    );
            }

            // The center of the gradient is in the center of the box.
            // The gradient origin offset was set to zero(0, 0) or center in this case.
            if (SUCCEEDED(hr))
            {
                hr = m_pRenderTarget->CreateRadialGradientBrush(
                    D2D1::RadialGradientBrushProperties(
                    D2D1::Point2F(75, 75),
                    D2D1::Point2F(0, 0),
                    75,
                    75),
                    pGradientStops,
                    &m_pRadialGradientBrush
                    );
            }

            SafeRelease(&pGradientStops);
        }

        // Create the bitmap to be used by the bitmap brush.
        if (SUCCEEDED(hr))
        {
            hr = LoadResourceBitmap(
                m_pRenderTarget,
                m_pWICFactory,
                L"FERN",
                L"Image",
                &m_pBitmap
                );
        }
        if (SUCCEEDED(hr))
        {
            hr = m_pRenderTarget->CreateBitmapBrush(
                m_pBitmap,
                &m_pBitmapBrush
                );
        }
    }
    return hr;
}

/******************************************************************
*                                                                 *
*  Discard device-specific resources which need to be recreated   *
*  when a Direct3D device is lost.                                *
*                                                                 *
******************************************************************/

void BrushTypesSample::DiscardDeviceResources()
{
    SafeRelease(&m_pRenderTarget);
    SafeRelease(&m_pBlackBrush);
    SafeRelease(&m_pYellowGreenBrush);
    SafeRelease(&m_pLinearGradientBrush);
    SafeRelease(&m_pRadialGradientBrush);
    SafeRelease(&m_pBitmapBrush);

}

/******************************************************************
*                                                                 *
*  The main window message loop.                                  *
*                                                                 *
******************************************************************/

void BrushTypesSample::RunMessageLoop()
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
*  Called whenever the application needs to display the client    *
*  window.                                                        *
*                                                                 *
*  Note that this function will automatically discard             *
*  device-specific resources if the Direct3D device disappears    *
*  during execution, and will recreate the resources the          *
*  next time it's invoked.                                        *
*                                                                 *
******************************************************************/
HRESULT BrushTypesSample::OnRender()
{

    HRESULT hr = CreateDeviceResources();

    if (SUCCEEDED(hr))
    {
        static const WCHAR szSolidBrushText[] = L"ID2D1SolidColorBrush";
        static const WCHAR szBitmapBrushText[] = L"ID2D1BitmapBrush";
        static const WCHAR szLinearGradientText[] = L"ID2D1LinearGradientBrush";
        static const WCHAR szRadialGradientText[] = L"ID2D1RadialGradientBrush";

        m_pRenderTarget->BeginDraw();

        m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());

        // Define the shape of rectangles to be filled with brushes. 
        D2D1_RECT_F rcBrushRect = D2D1::RectF(5, 0, 150, 150);

        // Define the area where captions are drawn.
        D2D1_RECT_F rcTextRect = D2D1::RectF(5, 165, 175, 200);

        // Start with a white background.
        m_pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));

        // Translate for the solid color brush.
        m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Translation(D2D1::SizeF(5, 5)));

        // Demonstrate a solid color brush.
        m_pRenderTarget->FillRectangle(&rcBrushRect, m_pYellowGreenBrush);
        m_pRenderTarget->DrawRectangle(&rcBrushRect, m_pBlackBrush, 1, NULL);

        // Draw a caption.
        m_pRenderTarget->DrawText(
            szSolidBrushText,
            ARRAYSIZE(szSolidBrushText) - 1,
            m_pTextFormat,
            &rcTextRect,
            m_pBlackBrush
            );

        // Translate for the linear gradient brush.
        m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Translation(D2D1::SizeF(200, 5)));

        // Demonstrate a linear gradient brush.
        m_pRenderTarget->FillRectangle(&rcBrushRect, m_pLinearGradientBrush);
        m_pRenderTarget->DrawRectangle(&rcBrushRect, m_pBlackBrush, 1, NULL);

        // Draw a caption.
        m_pRenderTarget->DrawText(
            szLinearGradientText,
            ARRAYSIZE(szLinearGradientText) - 1,
            m_pTextFormat,
            &rcTextRect,
            m_pBlackBrush
            );

        m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());

        // Translate for the radial gradient brush.
        m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Translation(D2D1::SizeF(5, 200)));

        // Draw a caption.
        m_pRenderTarget->DrawText(
            szRadialGradientText,
            ARRAYSIZE(szRadialGradientText) - 1,
            m_pTextFormat,
            &rcTextRect,
            m_pBlackBrush
            );

        D2D1_ELLIPSE   ellipse = D2D1::Ellipse(D2D1::Point2F(75.f, 75.f), 75.f, 75.f);

        m_pRenderTarget->FillEllipse(ellipse, m_pRadialGradientBrush);
        m_pRenderTarget->DrawEllipse(ellipse, m_pBlackBrush, 1, NULL);

        // Translate for the bitmap brush.
        m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Translation(D2D1::SizeF(200, 200)));

        // Draw a caption.
        m_pRenderTarget->DrawText(
            szBitmapBrushText,
            ARRAYSIZE(szBitmapBrushText) - 1,
            m_pTextFormat,
            &rcTextRect,
            m_pBlackBrush
            );

        // Demonstrate a bitmap brush.
        m_pRenderTarget->FillRectangle(&rcBrushRect, m_pBitmapBrush);
        m_pRenderTarget->DrawRectangle(&rcBrushRect, m_pBlackBrush, 1, NULL);

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
*  If the application receives a WM_SIZE message, this method     *
*  resizes the render target appropriately.                       *
*                                                                 *
******************************************************************/

void BrushTypesSample::OnResize(UINT width, UINT height)
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
*                                                                 *
*  The window message handler.                                    *
*                                                                 *
******************************************************************/

LRESULT CALLBACK BrushTypesSample::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    LRESULT result = 0;

    if (message == WM_CREATE)
    {
        LPCREATESTRUCT pcs = (LPCREATESTRUCT)lParam;
        BrushTypesSample *pDemoApp = (BrushTypesSample *)pcs->lpCreateParams;

        ::SetWindowLongPtrW(
            hwnd,
            GWLP_USERDATA,
            PtrToUlong(pDemoApp)
            );

        result = 1;
    }
    else
    {
        BrushTypesSample *pDemoApp = reinterpret_cast<BrushTypesSample *>(static_cast<LONG_PTR>(
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
                result = 0;
                wasHandled = true;
                break;

            case WM_DISPLAYCHANGE:
                {
                    InvalidateRect(hwnd, NULL, FALSE);
                }
                result = 0;
                wasHandled = true;
                break;

            case WM_PAINT:
                {
                    pDemoApp->OnRender();
                    ValidateRect(hwnd, NULL);
                }
                result = 0;
                wasHandled = true;
                break;

            case WM_DESTROY:
                {
                    PostQuitMessage(0);
                }
                result = 1;
                wasHandled = true;
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


/******************************************************************
*                                                                 *
*                                                                 *
*  This method will create a Direct2D bitmap from a resource in   *
*  this DLL.                                                      *
*                                                                 *
******************************************************************/

HRESULT BrushTypesSample::LoadResourceBitmap(
    ID2D1RenderTarget *pRenderTarget,
    IWICImagingFactory *pIWICFactory,
    PCWSTR resourceName,
    PCWSTR resourceType,
    ID2D1Bitmap **ppBitmap
    )
{
    HRESULT hr = S_OK;
    IWICBitmapDecoder *pDecoder = NULL;
    IWICBitmapFrameDecode *pSource = NULL;
    IWICStream *pStream = NULL;
    IWICFormatConverter *pConverter = NULL;

    HRSRC imageResHandle = NULL;
    HGLOBAL imageResDataHandle = NULL;
    void *pImageFile = NULL;
    DWORD imageFileSize = 0;

    // Locate the resource.
    imageResHandle = FindResourceW(HINST_THISCOMPONENT, resourceName, resourceType);

    hr = imageResHandle ? S_OK : E_FAIL;
    if (SUCCEEDED(hr))
    {
        // Load the resource.
        imageResDataHandle = LoadResource(HINST_THISCOMPONENT, imageResHandle);

        hr = imageResDataHandle ? S_OK : E_FAIL;
    }
    if (SUCCEEDED(hr))
    {
        // Lock it to get a system memory pointer.
        pImageFile = LockResource(imageResDataHandle);

        hr = pImageFile ? S_OK : E_FAIL;
    }
    if (SUCCEEDED(hr))
    {
        // Calculate the size.
        imageFileSize = SizeofResource(HINST_THISCOMPONENT, imageResHandle);

        hr = imageFileSize ? S_OK : E_FAIL;

    }
    if (SUCCEEDED(hr))
    {
        // Create a WIC stream to map onto the memory.
        hr = pIWICFactory->CreateStream(&pStream);
    }
    if (SUCCEEDED(hr))
    {
        // Initialize the stream with the memory pointer and size.
        hr = pStream->InitializeFromMemory(
            reinterpret_cast<BYTE*>(pImageFile),
            imageFileSize
            );
    }
    if (SUCCEEDED(hr))
    {
        // Create a decoder for the stream.
        hr = pIWICFactory->CreateDecoderFromStream(
            pStream,
            NULL,
            WICDecodeMetadataCacheOnLoad,
            &pDecoder
            );
    }
    if (SUCCEEDED(hr))
    {
        // Create the initial frame.
        hr = pDecoder->GetFrame(0, &pSource);
    }
    if (SUCCEEDED(hr))
    {
        // Convert the image format to 32bppPBGRA
        // (DXGI_FORMAT_B8G8R8A8_UNORM + D2D1_ALPHA_MODE_PREMULTIPLIED).
        hr = pIWICFactory->CreateFormatConverter(&pConverter);
    }
    if (SUCCEEDED(hr))
    {
        hr = pConverter->Initialize(
            pSource,
            GUID_WICPixelFormat32bppPBGRA,
            WICBitmapDitherTypeNone,
            NULL,
            0.f,
            WICBitmapPaletteTypeMedianCut
            );
    }
    if (SUCCEEDED(hr))
    {
        // Create a Direct2D bitmap from the WIC bitmap.
        hr = pRenderTarget->CreateBitmapFromWicBitmap(
            pConverter,
            NULL,
            ppBitmap
            );
    }

    SafeRelease(&pDecoder);
    SafeRelease(&pSource);
    SafeRelease(&pStream);
    SafeRelease(&pConverter);

    return hr;
}