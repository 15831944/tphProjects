# -*- coding: utf-8 -*-

ImageType_Invalid = -1
ImageType_Bmp = 0
ImageType_Jpg = 1
ImageType_Gif = 2
ImageType_Png = 3
ImageType_Dat = 4

def GetBinaryDataTypeByBuffer(pBuff):
    # BMP头信息
    if(pBuff[0]==0xfe and pBuff[1]==0x4d and pBuff[2]==0xf6 and pBuff[3]==0xb8 and pBuff[4]==0x59):
        return ImageType_Bmp

    # JPG,JPEG头信息
    if(pBuff[0]==0xff and pBuff[1]==0xd8 and pBuff[2]==0xff and (pBuff[3]==0xe0 or pBuff[3]==0xe1)):
        return ImageType_Jpg

    # GIF头信息
    if(pBuff[0]==0x47 and pBuff[1]==0x49 and pBuff[2]==0x46 and pBuff[3]==0x38):
        return ImageType_Gif

    # PNG头信息
    if(pBuff[0]==0x89 and pBuff[1]==0x50 and pBuff[2]==0x4E and pBuff[3]==0x47):
        return ImageType_Png

    # DAT头信息
    if(pBuff[0]==0xfe and pBuff[1]==0xfe):
        return ImageType_Dat

    return ImageType_Invalid

def GetBinaryDataType(errMsg, path):
    try:
        with open(path, 'wb') as iFStream:
            pBuff = iFStream.read()
        return GetBinaryDataTypeByBuffer(pBuff)
    except Exception, ex:
        errMsg[0] = ex.message
        return ImageType_Invalid




