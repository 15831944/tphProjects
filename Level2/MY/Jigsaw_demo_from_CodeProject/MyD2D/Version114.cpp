#include "stdafx.h"
#include <windows.h>
#include <d2d1.h>
#include <atlstr.h>
#include <dwrite.h>
#include <winnt.h>
#include <wincodec.h> // IWICImagingFactory

#pragma comment(lib, "C:\\Program Files (x86)\\Microsoft DirectX SDK (June 2010)\\Lib\\x86\\d2d1.lib")
#pragma comment(lib, "C:\\Program Files (x86)\\Microsoft DirectX SDK (June 2010)\\Lib\\x86\\dwrite.lib")
#pragma comment(lib, "windowscodecs.lib")

ID2D1Factory* g_pD2DFactory = NULL;
ID2D1HwndRenderTarget* g_pRenderTarget = NULL;
ID2D1SolidColorBrush* g_pSceneBrush = NULL;

IDWriteFactory* g_pDWriteFactory = NULL;
IDWriteTextFormat* g_pTextFormat = NULL;

ID2D1PathGeometry* g_pPathGeometry = NULL;

ID2D1Bitmap* g_pBitmap = NULL;
ID2D1BitmapBrush *g_pBitmapBrush = NULL;

VOID CreateD2DResource(HWND hWnd)
{
    if(g_pRenderTarget == NULL)
    {
        HRESULT hr;
        hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &g_pD2DFactory);

        RECT rc;
        GetClientRect(hWnd, &rc);
        hr = g_pD2DFactory->CreateHwndRenderTarget(
            D2D1::RenderTargetProperties(),
            D2D1::HwndRenderTargetProperties(hWnd, D2D1::SizeU(rc.right-rc.left, rc.bottom-rc.top)),
            &g_pRenderTarget
            );

        hr = g_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &g_pSceneBrush);
    }
}

void OnResize(UINT width, UINT height)
{
    if (g_pRenderTarget)
    {
        D2D1_SIZE_U size;
        size.width = width;
        size.height = height;
        g_pRenderTarget->Resize(size);
    }
}

VOID CreateDWriteResource(HWND hWnd)
{
    if(g_pDWriteFactory == NULL)
    {
        HRESULT hr;
        hr = DWriteCreateFactory(
            DWRITE_FACTORY_TYPE_SHARED,
            __uuidof(IDWriteFactory),
            reinterpret_cast<IUnknown**>(&g_pDWriteFactory));

        hr = g_pDWriteFactory->CreateTextFormat(
            L"Gabriola",
            NULL,
            DWRITE_FONT_WEIGHT_REGULAR,
            DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL,
            50.0f,
            L"en-us",
            &g_pTextFormat
            );


        hr = g_pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
        hr = g_pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
    }
}

VOID CreateGeometryResource(HWND hWnd)
{
    HRESULT hr;
    if(g_pPathGeometry == NULL)
        hr = g_pD2DFactory->CreatePathGeometry(&g_pPathGeometry);
    ID2D1GeometrySink* pSink = NULL;
    hr = g_pPathGeometry->Open(&pSink);
    pSink->SetFillMode(D2D1_FILL_MODE::D2D1_FILL_MODE_WINDING);
    pSink->BeginFigure(D2D1::Point2F(346, 255), D2D1_FIGURE_BEGIN_FILLED);
    D2D1_POINT_2F points[5] = 
    {
        D2D1::Point2F(267, 177),
        D2D1::Point2F(236, 192),
        D2D1::Point2F(212, 160),
        D2D1::Point2F(156, 255),
        D2D1::Point2F(346, 255), 
    };
    pSink->AddLines(points, ARRAYSIZE(points));
    pSink->EndFigure(D2D1_FIGURE_END_CLOSED);
    pSink->Close() ;
}

HRESULT LoadBitmapFromFile( ID2D1RenderTarget* pRenderTarget, 
    IWICImagingFactory* pIWICFactory,
    PCWSTR uri,
    UINT destinationWidth,
    UINT destinationHeight,
    ID2D1Bitmap **ppBitmap)
{
    HRESULT hr = S_OK;

    IWICBitmapDecoder* pDecoder = NULL;
    IWICBitmapFrameDecode* pSource = NULL;
    IWICStream* pStream = NULL;
    IWICFormatConverter* pConverter = NULL;
    IWICBitmapScaler* pScaler = NULL;

    hr = pIWICFactory->CreateDecoderFromFilename( 
        uri,
        NULL,
        GENERIC_READ,
        WICDecodeMetadataCacheOnLoad,
        &pDecoder
        );

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

    // If a new width or height was specified, create an 
    // IWICBitmapScaler and use it to resize the image.
    if(destinationWidth != 0 || destinationHeight != 0)
    {
        UINT originalWidth, originalHeight;
        hr = pSource->GetSize(&originalWidth, &originalHeight);
        if(SUCCEEDED(hr))
        {
            if(destinationWidth == 0)
            {
                FLOAT scaler = static_cast<FLOAT>(destinationHeight) / 
                    static_cast<FLOAT>(originalHeight);
                destinationHeight = static_cast<UINT>(scaler * static_cast<FLOAT>(originalHeight));
            }
            else if(destinationHeight == 0)
            {
                FLOAT scaler = static_cast<FLOAT>(destinationWidth) / 
                    static_cast<FLOAT>(originalWidth);
                destinationWidth = static_cast<UINT>(scaler * static_cast<FLOAT>(originalWidth));
            }

            hr = pIWICFactory->CreateBitmapScaler(&pScaler);
            if(SUCCEEDED(hr))
            {
                hr = pScaler->Initialize( pSource,
                    destinationWidth,
                    destinationHeight,
                    WICBitmapInterpolationModeCubic);
            }
            if(SUCCEEDED(hr))
            {
                hr = pConverter->Initialize( 
                    pScaler,
                    GUID_WICPixelFormat32bppPBGRA,
                    WICBitmapDitherTypeNone,
                    NULL,
                    0.f,
                    WICBitmapPaletteTypeMedianCut
                    );
            }
        }
    }
    else
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

    if(SUCCEEDED(hr))
    {
        hr = pRenderTarget->CreateBitmapFromWicBitmap(pConverter, NULL, ppBitmap);
    }

    if(pDecoder){ pDecoder->Release(); pDecoder = NULL; }
    if(pSource){ pSource->Release(); pSource = NULL; }
    if(pStream){ pStream->Release(); pStream = NULL; }
    if(pConverter){ pConverter->Release(); pConverter = NULL; }
    if(pScaler){ pScaler->Release(); pScaler = NULL; }
    return hr;
}

VOID CreateBitmapResource(HWND hWnd)
{
    HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
    IWICImagingFactory* pWICFactory = NULL;
    hr = CoCreateInstance( CLSID_WICImagingFactory, 
        NULL, 
        CLSCTX_INPROC_SERVER, 
        IID_IWICImagingFactory, 
        (LPVOID*)&pWICFactory); 

    if(SUCCEEDED(hr)) 
    {
        hr = LoadBitmapFromFile( g_pRenderTarget, 
            pWICFactory, 
            L"D:\\1.png", 
            100, 
            100, 
            &g_pBitmap);
    }

    hr = g_pRenderTarget->CreateBitmapBrush(g_pBitmap, &g_pBitmapBrush);

    CoUninitialize();
}

VOID WriteHelloWorld(HWND hWnd)
{
    g_pRenderTarget->SetTransform(D2D1::IdentityMatrix());
    g_pSceneBrush->SetColor(D2D1::ColorF(D2D1::ColorF::Yellow, 1.f));

    WCHAR wszText_[] = L"Hello World! Direct2D!";
    RECT rc;
    GetClientRect(hWnd, &rc);
    D2D1_RECT_F layoutRect = D2D1::RectF(
        static_cast<FLOAT>(rc.left) ,
        static_cast<FLOAT>(rc.top),
        static_cast<FLOAT>(rc.right - rc.left),
        static_cast<FLOAT>(rc.bottom - rc.top));

    int len = ARRAYSIZE(wszText_)-1;
    g_pRenderTarget->DrawText(
        wszText_,
        len,
        g_pTextFormat,
        layoutRect,
        g_pSceneBrush);
}

VOID DrawGeometry(HWND hWnd)
{
    HRESULT hr;

    g_pSceneBrush->SetColor(D2D1::ColorF(D2D1::ColorF::Black, 1.f));
    g_pRenderTarget->DrawGeometry(g_pPathGeometry, g_pSceneBrush, 1.f);

    g_pSceneBrush->SetColor(D2D1::ColorF(D2D1::ColorF::OliveDrab, 1.f));
    g_pRenderTarget->FillGeometry(g_pPathGeometry, g_pBitmapBrush);
    return;
}

void DrawBitmap()
{
    D2D1_SIZE_F size = g_pBitmap->GetSize();
    D2D1_POINT_2F upperLeftCorner = D2D1::Point2F(0.f, 0.f);
    g_pRenderTarget->DrawBitmap( g_pBitmap,
        D2D1::RectF( upperLeftCorner.x,
        upperLeftCorner.y,
        upperLeftCorner.x + size.width,
        upperLeftCorner.y + size.height));
}

VOID DrawAll(HWND hWnd)
{
    HRESULT hr;
    g_pRenderTarget->BeginDraw() ;
    g_pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));


    WriteHelloWorld(hWnd);
    DrawGeometry(hWnd);
    DrawBitmap();

    hr = g_pRenderTarget->EndDraw() ;
}

VOID Cleanup()
{
    if(g_pRenderTarget){g_pRenderTarget->Release(); g_pRenderTarget=NULL;}
    if(g_pSceneBrush){g_pSceneBrush->Release(); g_pSceneBrush=NULL;}
    if(g_pD2DFactory){g_pD2DFactory->Release(); g_pD2DFactory=NULL;}
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch(message)
    {
    case WM_CREATE:
        {
            CreateD2DResource(hWnd);
            CreateDWriteResource(hWnd);
            CreateGeometryResource(hWnd);
            CreateBitmapResource(hWnd);
        }
        return 0;
    case WM_SIZE:
        {
            UINT width = LOWORD(lParam);
            UINT height = HIWORD(lParam);
            OnResize(width, height);
        }
        return 0;
    case WM_PAINT:
        DrawAll(hWnd);
        return 0;
    case WM_KEYDOWN:
        {
            switch(wParam)
            {
            case VK_ESCAPE:
                SendMessage(hWnd, WM_CLOSE, 0, 0);
                break;
            default:
                break;
            }
        }
        break;
    case WM_DESTROY:
        Cleanup();
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hWnd, message, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
    WNDCLASSEX ex;
    ex.cbClsExtra       = NULL;
    ex.cbSize           = sizeof(WNDCLASSEX);
    ex.cbWndExtra       = NULL;
    ex.hbrBackground    = NULL;
    ex.hCursor          = LoadCursor(NULL, IDC_ARROW);
    ex.hIcon            = LoadIcon(NULL, IDI_APPLICATION);
    ex.hIconSm          = LoadIcon(NULL, IDI_APPLICATION);
    ex.hInstance        = hInstance;
    ex.lpfnWndProc      = WndProc;
    ex.lpszClassName    = L"Direct2D";
    ex.lpszMenuName     = NULL;
    ex.style            = CS_HREDRAW | CS_VREDRAW;

    if(!RegisterClassEx(&ex))
    {
        CString strErr;
        strErr.Format(L"RegisterClass Failed! Error: %d", GetLastError());
        MessageBox(NULL, strErr, L"Error", MB_OK);
        return -1;
    }

    HWND hWnd = CreateWindowEx(NULL, 
        L"Direct2D",
        L"Draw Rectangle",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        400,
        200,
        NULL,
        NULL,
        hInstance,
        NULL);

    ShowWindow(hWnd, iCmdShow);
    UpdateWindow(hWnd);

    MSG msg;
    ZeroMemory(&msg, sizeof(msg));

    while(GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return msg.wParam;
}

