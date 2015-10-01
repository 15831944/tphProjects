#pragma once
#include "atlimage.h"
#include <string>

class MyGdiPlus
{
private:
    MyGdiPlus(){}
    ~MyGdiPlus(){}
public:
    static void LoadBitmapFromMemory(int& iErr, void* pMemData, long len, Gdiplus::Bitmap** ppBmp);
    static void LoadBitmapFromFile(int& iErr, std::string szFileName, Gdiplus::Bitmap** ppBmp);
};
