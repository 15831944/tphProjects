#include "stdafx.h"
#include <fstream>

typedef enum
{
    ImageType_Invalid = -1,
    ImageType_Bmp = 0,
    ImageType_Jpg,
    ImageType_Gif,
    ImageType_Png,
    ImageType_Dat
}ImageType;

static ImageType GetBinaryDataTypeByBuffer(unsigned char * ch)
{
    //BMP头信息
    if(ch[0]==0xfe && ch[1]==0x4d && ch[2]==0xf6 && ch[3]==0xb8 && ch[4]==0x59)
    {
        return ImageType_Bmp;
    }

    //JPG,JPEG头信息
    if(ch[0]==0xff && ch[1]==0xd8 && ch[2]==0xff && (ch[3]==0xe0 || ch[3]==0xe1))
    {
        return ImageType_Jpg;
    }

    //GIF头信息
    if(ch[0]==0x47 && ch[1]==0x49 && ch[2]==0x46 && ch[3]==0x38)
    {
        return ImageType_Gif;
    }

    //PNG头信息
    if(ch[0]==0x89 && ch[1]==0x50 && ch[2]==0x4E && ch[3]==0x47)
    {
        return ImageType_Png;
    }

    //DAT头信息
    if(ch[0]==0xfe && ch[1]==0xfe)
    {
        return ImageType_Dat;
    }

    return ImageType_Invalid;
}

static ImageType GetBinaryDataType(CString path)
{
    std::ifstream readf(path, std::ios::binary);
    if(!readf.is_open())
        return ImageType_Invalid;
    unsigned char ch[8];
    for(int i=0;i<8;i++)
        readf>>ch[i];
    return GetBinaryDataTypeByBuffer(ch);
}




