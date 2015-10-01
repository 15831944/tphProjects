#include <windows.h>
#include <gdiplus.h>
#include <string>

#pragma comment(lib,"gdiplus")

using namespace Gdiplus;

//����ͼƬ�Ŀ�Ⱥ͸߶ȸ��´��ڿͻ����Ĵ�С
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
    // ���ļ��������ڴ���
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

    //����ͼ��
    Bitmap* image = NULL;
    ReadFileAndConvertToIStream(hWnd, file, &image);
    if(image->GetLastStatus() != Status::Ok)
    {
        MessageBox(hWnd, "hehehe", NULL, MB_OK);
        return;
    }

    //ȡ�ÿ�Ⱥ͸߶�
    width = image->GetWidth();
    height = image->GetHeight();

    //���´��ڴ�С
    set_window_size(hWnd,width,height);
    hdc = GetDC(hWnd);

    //��ͼ
    Graphics graphics(hdc);
    graphics.DrawImage(image, 0, 0, width, height);
    ReleaseDC(hWnd,hdc);
    return;
}

LRESULT CALLBACK WndProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
    switch(uMsg)
    {
    case WM_DROPFILES://�϶�ͼƬ�ļ�ʱ����ͼ����ʾ
        {
            char file[MAX_PATH];
            DragQueryFile((HDROP)wParam,0,file,sizeof(file)/sizeof(*file));
            draw_image(hWnd, std::string(file));
            DragFinish((HDROP)wParam);
            return 0;
        }
    case WM_LBUTTONDOWN://�������ʱ�϶�����
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

    char* title = "Win32SDK GDI+ ͼ����ʾʾ������(�϶�ͼƬ�ļ�����������ʾ)";
    hWnd = CreateWindowEx(0,"MyClassName",title,WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,
        NULL,NULL,hInstance,NULL);
    if(hWnd == NULL)
        return 1;

    //GdiPlus��ʼ��
    ULONG_PTR gdiplusToken;
    GdiplusStartupInput gdiplusInput;
    GdiplusStartup(&gdiplusToken,&gdiplusInput,NULL);

    //���ڽ����ļ��Ϸ�
    DragAcceptFiles(hWnd,TRUE);

    ShowWindow(hWnd,nShowCmd);
    UpdateWindow(hWnd);

    while(GetMessage(&msg,NULL,0,0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    //GdiPlus ȡ����ʼ��
    GdiplusShutdown(gdiplusToken);

    return msg.wParam;
}