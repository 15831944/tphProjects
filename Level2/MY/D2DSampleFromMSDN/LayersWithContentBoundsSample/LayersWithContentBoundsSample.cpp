
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved

//
//  $Description:
//      Sample Direct2D Application
//
//  $ENDTAG
//
//------------------------------------------------------------------------------
#include "stdafx.h"
#include "LayersWithContentBoundsSample.h"

/******************************************************************
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
*  Initialize member data.                                        *
*                                                                 *
******************************************************************/

DemoApp::DemoApp() :
m_hwnd(NULL),
    m_pD2DFactory(NULL),
    m_pDWriteFactory(NULL),
    m_pRenderTarget(NULL),
    m_pPathGeometry(NULL),
    m_pWICFactory(NULL),
    m_pWaterBitmap(NULL),
    m_pBambooBitmap(NULL),
    m_pLeafBitmap(NULL),
    m_pOriginalBitmapBrush(NULL),
    m_pRadialGradientBrush(NULL),
    m_pSolidColorBrush(NULL)
{
}

/******************************************************************
*                                                                 *
*  Release resources.                                             *
*                                                                 *
******************************************************************/

DemoApp::~DemoApp()
{
    SafeRelease(&m_pPathGeometry);
    SafeRelease(&m_pWaterBitmap);
    SafeRelease(&m_pBambooBitmap);
    SafeRelease(&m_pLeafBitmap);
    SafeRelease(&m_pOriginalBitmapBrush);
    SafeRelease(&m_pRenderTarget);
    SafeRelease(&m_pWICFactory);
    SafeRelease(&m_pD2DFactory);
    SafeRelease(&m_pDWriteFactory);
    SafeRelease(&m_pRadialGradientBrush);
    SafeRelease(&m_pSolidColorBrush);
}

/******************************************************************
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
        m_pD2DFactory->GetDesktopDpi(&dpiX, &dpiY);

        m_hwnd = CreateWindow(
            L"D2DDemoApp",
            L"Direct2D Clip with Layers",
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            static_cast<UINT>(ceil(550.f * dpiX / 96.f)),
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
*  This method is used to create resources which are not bound    *
*  to any device. Their lifetime effectively extends for the      *
*  duration of the app.                                           *
*                                                                 *
******************************************************************/
HRESULT DemoApp::CreateDeviceIndependentResources()
{
    HRESULT hr = CoCreateInstance(
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

    if(SUCCEEDED(hr))
    {
        hr = m_pD2DFactory->CreatePathGeometry(&m_pPathGeometry);

        if(SUCCEEDED(hr))
        {
            ID2D1GeometrySink *pSink = NULL;
            // Write to the path geometry using the geometry sink.
            hr = m_pPathGeometry->Open(&pSink);

            if (SUCCEEDED(hr))
            {
                pSink->SetFillMode(D2D1_FILL_MODE_WINDING);
                pSink->BeginFigure(
                    D2D1::Point2F(0, 90),
                    D2D1_FIGURE_BEGIN_FILLED
                    );

                D2D1_POINT_2F points[7] = {
                    D2D1::Point2F(35, 30),
                    D2D1::Point2F(50, 50),
                    D2D1::Point2F(70, 45),
                    D2D1::Point2F(105, 90),
                    D2D1::Point2F(130, 90),
                    D2D1::Point2F(150, 60),
                    D2D1::Point2F(170, 90)
                };

                pSink->AddLines(points, 7);
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
            hr = LoadResourceBitmap(
                m_pRenderTarget,
                m_pWICFactory,
                L"Dam",
                L"Image",
                &m_pWaterBitmap
                );
        }

        if (SUCCEEDED(hr))
        {
            hr = LoadResourceBitmap(
                m_pRenderTarget,
                m_pWICFactory,
                L"Bamboo",
                L"Image",
                &m_pBambooBitmap
                );
        }
        if (SUCCEEDED(hr))
        {
            hr = LoadResourceBitmap(
                m_pRenderTarget,
                m_pWICFactory,
                L"Leaf",
                L"Image",
                &m_pLeafBitmap
                );
        }

        if (SUCCEEDED(hr))
        {
            hr = m_pRenderTarget->CreateSolidColorBrush(
                D2D1::ColorF(D2D1::ColorF::YellowGreen),
                &m_pSolidColorBrush
                );
            if (SUCCEEDED(hr))
            {
                m_pSolidColorBrush->SetOpacity(0.75f);    

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
                D2D1_GAMMA_2_2,
                D2D1_EXTEND_MODE_CLAMP,
                &pGradientStops);

            if (SUCCEEDED(hr))
            {
                hr = m_pRenderTarget->CreateRadialGradientBrush(
                    D2D1::RadialGradientBrushProperties(
                    D2D1::Point2F(50, 60),
                    D2D1::Point2F(0, 0),
                    50,
                    50),
                    pGradientStops,
                    &m_pRadialGradientBrush);
            }
            pGradientStops->Release();
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

void DemoApp::DiscardDeviceResources()
{
    SafeRelease(&m_pRenderTarget);
    SafeRelease(&m_pWaterBitmap);
    SafeRelease(&m_pLeafBitmap);
    SafeRelease(&m_pBambooBitmap);
    SafeRelease(&m_pOriginalBitmapBrush);
    SafeRelease(&m_pRadialGradientBrush);
    SafeRelease(&m_pSolidColorBrush);
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

        m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Translation(5, 5));
        m_pRenderTarget->DrawBitmap(m_pWaterBitmap, D2D1::RectF(0, 0, 128, 192));
        hr = RenderWithLayerWithContentBounds(m_pRenderTarget);

        // Move 600 pixels down along the y-axis.
        if (SUCCEEDED(hr))
        {
            m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Translation(5, 250));

            m_pRenderTarget->DrawBitmap(m_pBambooBitmap, D2D1::RectF(0, 0, 190, 127));
            m_pRenderTarget->FillRectangle(
                D2D1::RectF(25.f, 25.f, 50.f, 50.f), 
                m_pSolidColorBrush
                );
            m_pRenderTarget->FillRectangle(
                D2D1::RectF(50.f, 50.f, 75.f, 75.f),
                m_pSolidColorBrush
                ); 
            m_pRenderTarget->FillRectangle(
                D2D1::RectF(75.f, 75.f, 100.f, 100.f),
                m_pSolidColorBrush
                );    

            hr = RenderWithLayerWithOpacityMask(m_pRenderTarget);

            if (SUCCEEDED(hr))
            {
                m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Translation(5, 450));

                m_pRenderTarget->DrawBitmap(m_pLeafBitmap, D2D1::RectF(0, 0, 198, 132));
                m_pRenderTarget->FillRectangle(
                    D2D1::RectF(50.f, 50.f, 75.f, 75.f), 
                    m_pSolidColorBrush
                    ); 
                m_pRenderTarget->FillRectangle(
                    D2D1::RectF(75.f, 75.f, 100.f, 100.f),
                    m_pSolidColorBrush
                    );        

                hr = RenderWithLayerWithGeometricMask(m_pRenderTarget);
                if (SUCCEEDED(hr))
                {
                    hr = m_pRenderTarget->EndDraw();

                    if (hr == D2DERR_RECREATE_TARGET)
                    {
                        hr = S_OK;
                        DiscardDeviceResources();
                    }
                }
            }
        }
    }
    return S_OK;
}


/******************************************************************
*                                                                 *
* Demonstrates the layerParameters with the content bounds.       *
*                                                                 *
******************************************************************/
HRESULT DemoApp::RenderWithLayerWithContentBounds(ID2D1RenderTarget *pRT)
{

    HRESULT hr = S_OK;

    // Create a layer.
    ID2D1Layer *pLayer = NULL;
    hr = pRT->CreateLayer(NULL, &pLayer);

    if (SUCCEEDED(hr))
    {
        pRT->SetTransform(D2D1::Matrix3x2F::Translation(300, 0));

        // Push the layer with the content bounds.
        pRT->PushLayer(
            D2D1::LayerParameters(D2D1::RectF(10, 108, 121, 177)),
            pLayer
            );

        pRT->DrawBitmap(m_pWaterBitmap, D2D1::RectF(0, 0, 128, 192));
        pRT->PopLayer();
    }

    SafeRelease(&pLayer);

    return hr;

}

/******************************************************************
*                                                                 *
* Demonstrates the layerParameters with the geometric mask.       *
*                                                                 *
******************************************************************/
HRESULT DemoApp::RenderWithLayerWithGeometricMask(ID2D1RenderTarget *pRT)
{

    HRESULT hr;

    // Create a layer.
    ID2D1Layer *pLayer = NULL;
    hr = pRT->CreateLayer(NULL, &pLayer);

    if (SUCCEEDED(hr))
    {
        pRT->SetTransform(D2D1::Matrix3x2F::Translation(300, 450));

        pRT->PushLayer(
            D2D1::LayerParameters(D2D1::InfiniteRect(), m_pPathGeometry),
            pLayer
            );

        pRT->DrawBitmap(m_pLeafBitmap, D2D1::RectF(0, 0, 198, 132));

        m_pRenderTarget->FillRectangle(
            D2D1::RectF(50.f, 50.f, 75.f, 75.f), 
            m_pSolidColorBrush
            ); 
        m_pRenderTarget->FillRectangle(
            D2D1::RectF(75.f, 75.f, 100.f, 100.f),
            m_pSolidColorBrush
            );        

        pRT->PopLayer();
    }

    SafeRelease(&pLayer);

    return hr;

}

/******************************************************************
*                                                                 *
* Demonstrates the layerParameters with the opacity mask.         *
*                                                                 *
******************************************************************/
// Use a radial gradient brush as an opacity mask
HRESULT DemoApp::RenderWithLayerWithOpacityMask(ID2D1RenderTarget *pRT)
{   

    HRESULT hr = S_OK;

    // Create a layer.
    ID2D1Layer *pLayer = NULL;
    hr = pRT->CreateLayer(NULL, &pLayer);

    if (SUCCEEDED(hr))
    {
        pRT->SetTransform(D2D1::Matrix3x2F::Translation(300, 250));

        // Push the layer with the content bounds.
        pRT->PushLayer(
            D2D1::LayerParameters(
            D2D1::InfiniteRect(),
            NULL,
            D2D1_ANTIALIAS_MODE_PER_PRIMITIVE,
            D2D1::IdentityMatrix(),
            1.0,
            m_pRadialGradientBrush,
            D2D1_LAYER_OPTIONS_NONE),
            pLayer
            );

        pRT->DrawBitmap(m_pBambooBitmap, D2D1::RectF(0, 0, 190, 127));

        m_pRenderTarget->FillRectangle(
            D2D1::RectF(25.f, 25.f, 50.f, 50.f), 
            m_pSolidColorBrush
            );
        m_pRenderTarget->FillRectangle(
            D2D1::RectF(50.f, 50.f, 75.f, 75.f),
            m_pSolidColorBrush
            ); 
        m_pRenderTarget->FillRectangle(
            D2D1::RectF(75.f, 75.f, 100.f, 100.f),
            m_pSolidColorBrush
            );    

        m_pRenderTarget->PopLayer();
    }
    SafeRelease(&pLayer);

    return hr;

}

/******************************************************************
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

/******************************************************************
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