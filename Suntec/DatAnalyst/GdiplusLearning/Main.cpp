#include <windows.h>
#include <gdiplus.h>
#include <string>

#pragma comment(lib,"gdiplus")

using namespace Gdiplus;

//根据图片的宽度和高度更新窗口客户区的大小
void set_window_size(HWND hWnd,int width,int height)
{
    RECT rcWindow,rcClient;
    int border_width,border_height;
    GetWindowRect(hWnd,&rcWindow);
    GetClientRect(hWnd,&rcClient);
    border_width = (rcWindow.right-rcWindow.left) - (rcClient.right-rcClient.left);
    border_height = (rcWindow.bottom-rcWindow.top) - (rcClient.bottom-rcClient.top);
    SetWindowPos(hWnd,0,0,0,border_width+width,border_height+height,SWP_NOMOVE|SWP_NOZORDER);
}

void ReadFileAndConvertToIStream(HWND hwnd, std::string szFileName, Bitmap** ppBmp)
{
    TCHAR achErrInfo[512] = {0};
    // 将文件读到流内存中
    HANDLE hFile = ::CreateFile(szFileName.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, 
        OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if(hFile == INVALID_HANDLE_VALUE)
    {
        MessageBox(hwnd, (std::string("failed to open file: ") + szFileName).c_str(), NULL, MB_OK);
        return;
    }

    DWORD dwSize;
    dwSize = ::GetFileSize(hFile, NULL);
    char* pTemp = new char[dwSize];
    try
    {
        DWORD dwReadBytes = 0;
        ::ReadFile(hFile, pTemp, dwSize, &dwReadBytes, NULL);
    }
    catch(...)
    {
        MessageBox(hwnd, "Load (file): An exception occured while reading the file %s\n", NULL, MB_OK);
        return;
    }

    HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE | GMEM_NODISCARD, dwSize);
    if (!hGlobal)
    {
        MessageBox(hwnd, "Load (file): Error allocating memory\n", NULL, MB_OK);
        return;
    };

    char *pData = reinterpret_cast<char*>(GlobalLock(hGlobal));
    if (!pData)
    {
        MessageBox(hwnd, "Load (file): Error locking memory\n", NULL, MB_OK);
        return;
    };

    memcpy(pData, pTemp, dwSize);

    GlobalUnlock(hGlobal);
    ::CloseHandle(hFile);

    IStream *pStream = NULL;
    if(CreateStreamOnHGlobal(hGlobal, TRUE, &pStream) != S_OK)
    {
        return;
    }

    *ppBmp = new Bitmap(pStream);
    pStream->Release();
}

void draw_image(HWND hWnd,std::string file)
{
    HDC hdc;
    int width,height;

    //加载图像
    Bitmap* image = NULL;
    ReadFileAndConvertToIStream(hWnd, file, &image);
    if(image->GetLastStatus() != Status::Ok)
    {
        MessageBox(hWnd, "hehehe", NULL, MB_OK);
        return;
    }

    //取得宽度和高度
    width = image->GetWidth();
    height = image->GetHeight();

    //更新窗口大小
    set_window_size(hWnd,width,height);
    hdc = GetDC(hWnd);

    //绘图
    Graphics graphics(hdc);
    graphics.DrawImage(image, 0, 0, width, height);
    ReleaseDC(hWnd,hdc);
    return;
}

LRESULT CALLBACK WndProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
    switch(uMsg)
    {
    case WM_DROPFILES://拖动图片文件时进行图像显示
        {
            char file[MAX_PATH];
            DragQueryFile((HDROP)wParam,0,file,sizeof(file)/sizeof(*file));
            draw_image(hWnd, std::string(file));
            DragFinish((HDROP)wParam);
            return 0;
        }
    case WM_LBUTTONDOWN://左键单击时拖动窗体
        SendMessage(hWnd,WM_NCLBUTTONDOWN,HTCAPTION,0);
        return 0;
    case WM_CREATE:
        return 0;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hWnd,uMsg,wParam,lParam);
}

int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nShowCmd)
{
    MSG msg;
    WNDCLASSEX wce={0};
    HWND hWnd;

    wce.cbSize = sizeof(wce);
    wce.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wce.hCursor = LoadCursor(NULL,IDC_ARROW);
    wce.hIcon = LoadIcon(NULL,IDI_APPLICATION);
    wce.hInstance = hInstance;
    wce.lpfnWndProc = WndProc;
    wce.lpszClassName = "MyClassName";
    wce.style = CS_HREDRAW|CS_VREDRAW;
    if(!RegisterClassEx(&wce))
        return 1;

    char* title = "Win32SDK GDI+ 图像显示示例程序(拖动图片文件到窗口以显示)";
    hWnd = CreateWindowEx(0,"MyClassName",title,WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,
        NULL,NULL,hInstance,NULL);
    if(hWnd == NULL)
        return 1;

    //GdiPlus初始化
    ULONG_PTR gdiplusToken;
    GdiplusStartupInput gdiplusInput;
    GdiplusStartup(&gdiplusToken,&gdiplusInput,NULL);

    //窗口接收文件拖放
    DragAcceptFiles(hWnd,TRUE);

    ShowWindow(hWnd,nShowCmd);
    UpdateWindow(hWnd);

    while(GetMessage(&msg,NULL,0,0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    //GdiPlus 取消初始化
    GdiplusShutdown(gdiplusToken);

    return msg.wParam;
}