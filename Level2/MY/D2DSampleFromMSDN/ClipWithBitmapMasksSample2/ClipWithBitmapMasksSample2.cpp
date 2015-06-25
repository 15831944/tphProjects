// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved

#include "stdafx.h"
#include "ClipWithBitmapMasksSample2.h"

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
    // Ignore the return value because we want to continue running even in the
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
    m_pDirect2DFactory(NULL),
    m_pRenderTarget(NULL),
    m_pBlackBrush(NULL),
    m_pWICFactory(NULL),
    m_pBitmapMask(NULL),
    m_pLinearGradientBrush(NULL),
    m_pRadialGradientBrush(NULL),
    m_pBitmapMaskBrush(NULL),
    m_pDirectWriteFactory(NULL),
    m_pRectGeo(NULL),
    m_pFernBitmapBrush(NULL),
    m_pLinearFadeFlowersBitmapBrush(NULL),
    m_pRadialFadeFlowersBitmapBrush(NULL),
    m_pFernBitmap(NULL),
    m_pLinearFadeFlowersBitmap(NULL),
    m_pRadialFadeFlowersBitmap(NULL)
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
    SafeRelease(&m_pDirect2DFactory);
    SafeRelease(&m_pRenderTarget);
    SafeRelease(&m_pBlackBrush);
    SafeRelease(&m_pWICFactory);
    SafeRelease(&m_pBitmapMask);
    SafeRelease(&m_pLinearGradientBrush);
    SafeRelease(&m_pRadialGradientBrush);
    SafeRelease(&m_pBitmapMaskBrush);
    SafeRelease(&m_pDirectWriteFactory);
    SafeRelease(&m_pRectGeo);
    SafeRelease(&m_pFernBitmapBrush);
    SafeRelease(&m_pLinearFadeFlowersBitmapBrush);
    SafeRelease(&m_pRadialFadeFlowersBitmapBrush);
    SafeRelease(&m_pFernBitmap);
    SafeRelease(&m_pLinearFadeFlowersBitmap);
    SafeRelease(&m_pRadialFadeFlowersBitmap);
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

    // Initialize device-independent resources, such
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
        m_pDirect2DFactory->GetDesktopDpi(&dpiX, &dpiY);

        m_hwnd = CreateWindow(
            L"D2DDemoApp",
            L"Direct2D Demo Application",
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            static_cast<UINT>(ceil(640.f * dpiX / 96.f)),
            static_cast<UINT>(ceil(640.f * dpiY / 96.f)),
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
    HRESULT hr = S_OK;

    // Create a Direct2D factory.
    hr = D2D1CreateFactory(
        D2D1_FACTORY_TYPE_SINGLE_THREADED,
        &m_pDirect2DFactory
        );

    if (SUCCEEDED(hr))
    {
        // Create a WIC factory.
        hr = CoCreateInstance(
            CLSID_WICImagingFactory,
            NULL,
            CLSCTX_INPROC_SERVER,
            IID_PPV_ARGS(&m_pWICFactory)
            );
    }

    if (SUCCEEDED(hr))
    {
        hr = m_pDirect2DFactory->CreatePathGeometry(&m_pRectGeo);
        if(SUCCEEDED(hr))
        {
            ID2D1GeometrySink* pSink = NULL;
            hr = m_pRectGeo->Open(&pSink);
            if(SUCCEEDED(hr))
            {
                pSink->BeginFigure(
                    D2D1::Point2F(0.0f, 0.0f),
                    D2D1_FIGURE_BEGIN_FILLED
                    );

                pSink->AddLine(D2D1::Point2F(200, 0));
                pSink->AddBezier(
                    D2D1::BezierSegment(
                    D2D1::Point2F(150, 50),
                    D2D1::Point2F(150, 150),
                    D2D1::Point2F(200, 200))
                    );

                pSink->AddLine(D2D1::Point2F(0, 200));
                pSink->AddBezier(
                    D2D1::BezierSegment(
                    D2D1::Point2F(50, 150),
                    D2D1::Point2F(50, 50),
                    D2D1::Point2F(0, 0))
                    );

                pSink->EndFigure(D2D1_FIGURE_END_CLOSED);
                hr = pSink->Close();
            }
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
        hr = m_pDirect2DFactory->CreateHwndRenderTarget(
            D2D1::RenderTargetProperties(),
            D2D1::HwndRenderTargetProperties(m_hwnd, size),
            &m_pRenderTarget
            );

        if (SUCCEEDED(hr))
        {
            hr = m_pRenderTarget->CreateSolidColorBrush(
                D2D1::ColorF(D2D1::ColorF::Black),
                &m_pBlackBrush
                );

            if (SUCCEEDED(hr))
            {
                // Create the bitmap to be used by the bitmap brush.
                hr = LoadResourceBitmap(
                    m_pRenderTarget,
                    m_pWICFactory,
                    L"Fern",
                    L"Image",
                    &m_pFernBitmap
                    );
            }

            if (SUCCEEDED(hr))
            {
                // Create the bitmap to be used by the bitmap brush.
                hr = LoadResourceBitmap(
                    m_pRenderTarget,
                    m_pWICFactory,
                    L"LinearFadeFlowers",
                    L"Image",
                    &m_pLinearFadeFlowersBitmap
                    );
            }
            if (SUCCEEDED(hr))
            {
                // Create the bitmap to be used by the bitmap brush.
                hr = LoadResourceBitmap(
                    m_pRenderTarget,
                    m_pWICFactory,
                    L"RadialFadeFlowers",
                    L"Image",
                    &m_pRadialFadeFlowersBitmap
                    );
            }
            if (SUCCEEDED(hr))
            {
                hr = LoadResourceBitmap(
                    m_pRenderTarget,
                    m_pWICFactory,
                    L"BitmapMask",
                    L"Image",
                    &m_pBitmapMask
                    );
            }
            if (SUCCEEDED(hr))
            {
                D2D1_BITMAP_BRUSH_PROPERTIES propertiesXClampYClamp = 
                    D2D1::BitmapBrushProperties(
                    D2D1_EXTEND_MODE_CLAMP,
                    D2D1_EXTEND_MODE_CLAMP,
                    D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR
                    );
                hr = m_pRenderTarget->CreateBitmapBrush(
                    m_pFernBitmap,
                    propertiesXClampYClamp,
                    &m_pFernBitmapBrush
                    );
                if (SUCCEEDED(hr))
                {
                    hr = m_pRenderTarget->CreateBitmapBrush(
                        m_pLinearFadeFlowersBitmap,
                        propertiesXClampYClamp,
                        &m_pLinearFadeFlowersBitmapBrush
                        );
                }
                if (SUCCEEDED(hr))
                {
                    hr = m_pRenderTarget->CreateBitmapBrush(
                        m_pRadialFadeFlowersBitmap,
                        propertiesXClampYClamp,
                        &m_pRadialFadeFlowersBitmapBrush
                        );
                }

                if (SUCCEEDED(hr))
                {
                    hr = m_pRenderTarget->CreateBitmapBrush(
                        m_pBitmapMask,
                        propertiesXClampYClamp,
                        &m_pBitmapMaskBrush
                        );
                }
            }


            if (SUCCEEDED(hr))
            {
                ID2D1GradientStopCollection *pGradientStops = NULL;

                static const D2D1_GRADIENT_STOP gradientStops[] =
                {
                    {   0.f,  D2D1::ColorF(D2D1::ColorF::Black, 1.0f)  },
                    {   1.f,  D2D1::ColorF(D2D1::ColorF::White, 0.0f)  },
                };

                hr = m_pRenderTarget->CreateGradientStopCollection(
                    gradientStops,
                    2,
                    &pGradientStops);


                if (SUCCEEDED(hr))
                {
                    hr = m_pRenderTarget->CreateLinearGradientBrush(
                        D2D1::LinearGradientBrushProperties(
                        D2D1::Point2F(0, 0),
                        D2D1::Point2F(150, 150)),
                        pGradientStops,
                        &m_pLinearGradientBrush);
                }

                if (SUCCEEDED(hr))
                {
                    hr = m_pRenderTarget->CreateRadialGradientBrush(
                        D2D1::RadialGradientBrushProperties(
                        D2D1::Point2F(75, 75),
                        D2D1::Point2F(0, 0),
                        75,
                        75),
                        pGradientStops,
                        &m_pRadialGradientBrush);
                }
                pGradientStops->Release();
            }
        }
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
    SafeRelease(&m_pBlackBrush);
    SafeRelease(&m_pLinearGradientBrush);
    SafeRelease(&m_pRadialGradientBrush);
    SafeRelease(&m_pBitmapMaskBrush);
    SafeRelease(&m_pFernBitmapBrush);
    SafeRelease(&m_pLinearFadeFlowersBitmapBrush);
    SafeRelease(&m_pRadialFadeFlowersBitmapBrush);
    SafeRelease(&m_pFernBitmap);
    SafeRelease(&m_pLinearFadeFlowersBitmap);
    SafeRelease(&m_pRadialFadeFlowersBitmap);
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

    while (GetMessage(&msg, NULL, 0, 0)>0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

/******************************************************************
*                                                                 *
*  DemoApp::OnRender                                              *
*                                                                 *
*  This method draws blocks demonstrating different opacity       *
*  masks.                                                         *
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

        // Rectangle that holds the shape of rectangles to be filled with brushes.
        D2D1_RECT_F rcBrushRect = D2D1::RectF(5, 5, 155, 155);

        // A group of images for a linear gradient brush.
        // A Bitmap of flowers that are used with a linear gradient brush.
        m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Translation(5, 5));
        m_pRenderTarget->FillRectangle(
            rcBrushRect, 
            m_pLinearFadeFlowersBitmapBrush
            );
        m_pRenderTarget->DrawRectangle(rcBrushRect, m_pBlackBrush, 1.f);

        // Fill a rectangle with a linear gradient brush. 
        m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Translation(205, 5));
        m_pRenderTarget->FillRectangle(rcBrushRect, m_pLinearGradientBrush);
        m_pRenderTarget->DrawRectangle(rcBrushRect, m_pBlackBrush, 1.f);


        // Use a linear gradient brush as an opacity mask.
        m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Translation(405, 5));
        m_pRenderTarget->FillGeometry(
            m_pRectGeo, 
            m_pLinearFadeFlowersBitmapBrush, 
            m_pLinearGradientBrush
            );
        m_pRenderTarget->DrawRectangle(rcBrushRect, m_pBlackBrush, 1.f);

        // A group of images for a radial gradient brush.     
        m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Translation(5, 205));
        m_pRenderTarget->FillRectangle(
            rcBrushRect, 
            m_pRadialFadeFlowersBitmapBrush
            );
        m_pRenderTarget->DrawRectangle(rcBrushRect, m_pBlackBrush, 1.f);

        // Fill a rectangle with a radial gradient brush.
        m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Translation(205, 205));
        m_pRenderTarget->FillRectangle(rcBrushRect, m_pRadialGradientBrush);
        m_pRenderTarget->DrawRectangle(rcBrushRect, m_pBlackBrush, 1.f);

        // Use a radial gradient brush as an opacity mask.
        m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Translation(405, 205));
        m_pRenderTarget->FillGeometry(
            m_pRectGeo,
            m_pRadialFadeFlowersBitmapBrush, 
            m_pRadialGradientBrush
            );
        m_pRenderTarget->DrawRectangle(rcBrushRect, m_pBlackBrush, 1.f);

        // A group for a bitmap mask brush.
        m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Translation(5, 405));
        m_pRenderTarget->FillRectangle(rcBrushRect, m_pFernBitmapBrush);
        m_pRenderTarget->DrawRectangle(rcBrushRect, m_pBlackBrush, 1.f);

        // Fill a rectangle with a bitmap brush.
        m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Translation(205, 405));
        m_pRenderTarget->FillRectangle(rcBrushRect, m_pBitmapMaskBrush);
        m_pRenderTarget->DrawRectangle(rcBrushRect, m_pBlackBrush, 1.f);

        // Use a bitmap as an opacity mask.
        m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Translation(405, 405));

        // D2D1_ANTIALIAS_MODE_ALIASED must be set for FillOpacityMask to function properly
        m_pRenderTarget->SetAntialiasMode(D2D1_ANTIALIAS_MODE_ALIASED);
        m_pRenderTarget->FillOpacityMask(
            m_pBitmapMask,
            m_pFernBitmapBrush,
            D2D1_OPACITY_MASK_CONTENT_GRAPHICS,
            &rcBrushRect
            );
        m_pRenderTarget->SetAntialiasMode(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
        m_pRenderTarget->DrawRectangle(&rcBrushRect, m_pBlackBrush, 1.f);


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
*  DemoApp::LoadResourceBitmap                                    *
*                                                                 *
*  This method will create a Direct2D bitmap from an application  *
*  resource.                                                      *
*                                                                 *
******************************************************************/

HRESULT DemoApp::LoadResourceBitmap(
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