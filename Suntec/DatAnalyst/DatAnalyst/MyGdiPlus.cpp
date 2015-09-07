#include "StdAfx.h"
#include "MyGdiPlus.h"
#include "MyError.h"

void MyGdiPlus::LoadBitmapFromMemory(int& iErr, void* pMemData, long len, Gdiplus::Bitmap** ppBmp)
{
    HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE | GMEM_NODISCARD, len);
    char *pData = reinterpret_cast<char*>(GlobalLock(hGlobal));
    memcpy(pData, pMemData, len);
    GlobalUnlock(hGlobal);
    IStream* pStream = NULL;
    if(CreateStreamOnHGlobal(hGlobal, TRUE, &pStream) != S_OK)
    {
        iErr = xxxxxx10;
        *ppBmp = NULL;
        return;
    }
    else
    {
        iErr = DAT_SUCCESS;
        *ppBmp = new Gdiplus::Bitmap(pStream);
        pStream->Release();
        return;
    }
}

void MyGdiPlus::LoadBitmapFromFile(int& iErr, std::string szFileName, Gdiplus::Bitmap** ppBmp)
{
    HANDLE hFile = ::CreateFile(szFileName.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, 
        OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
    if(hFile == INVALID_HANDLE_VALUE)
    {
        iErr = xxxxxx14;
        return;
    }

    DWORD dwSize;
    dwSize = ::GetFileSize(hFile, NULL);
    HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE | GMEM_NODISCARD, dwSize);
    if (!hGlobal)
    {
        iErr = xxxxxx13;
        return;
    }

    char *pData = reinterpret_cast<char*>(GlobalLock(hGlobal));
    if ( !pData )
    {
        iErr = xxxxxx13;
        return;
    }

    try
    {
        DWORD dwReadBytes = 0;
        ::ReadFile(hFile, pData, dwSize, &dwReadBytes, NULL);
    }
    catch(...)
    {
        iErr = xxxxxx11;
        GlobalFree(hGlobal);
        ::CloseHandle(hFile);
        return;
    }

    GlobalUnlock(hGlobal);
    ::CloseHandle(hFile);

    IStream *pStream = NULL;
    if (CreateStreamOnHGlobal(hGlobal, TRUE, &pStream ) != S_OK)
    {
        iErr = xxxxxx12;
        return;
    }

    *ppBmp = new Gdiplus::Bitmap(pStream);
    pStream->Release();
    iErr = DAT_SUCCESS;
    return;
}
