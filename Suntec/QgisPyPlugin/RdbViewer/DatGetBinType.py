# -*- coding: utf-8 -*-

ImageType_Invalid = -1
ImageType_Bmp = 0
ImageType_Jpg = 1
ImageType_Gif = 2
ImageType_Png = 3
ImageType_Dat = 4

def GetBinaryDataTypeByBuffer(pBuff):
    # bmp header
    if(pBuff[:2] == "BM"):
        return ImageType_Bmp

    # JPG,JPEG header
    if(pBuff[0]=="\377"):
        return ImageType_Jpg

    # GIF header
    if(pBuff[:6] in ["GIF87a", "GIF89a"]):
        return ImageType_Gif

    # PNG header
    if(pBuff[:8] == "\211PNG\r\n\032\n"):
        return ImageType_Png

    # DAT header
    if(pBuff[0]==0xfe and pBuff[1]==0xfe):
        return ImageType_Dat

    return ImageType_Invalid

def GetBinaryDataType(errMsg, path):
    try:
        with open(path, 'rb') as iFStream:
            pBuff = iFStream.read()
        return GetBinaryDataTypeByBuffer(pBuff)
    except Exception, ex:
        errMsg[0] = ex.message
        return ImageType_Invalid
    
def GetImageSize(pBuff):
    imageType = GetBinaryDataTypeByBuffer(pBuff)
    if imageType == ImageType_Jpg:
        return GetSizeJpeg(pBuff)
    elif imageType == ImageType_Png:
        return GetSizePng(pBuff)
    else:
        return (0,0)

# big endian.
def GetInt16BigEndian(pBuff):
    return ord(pBuff[1]) + (ord(pBuff[0])<<8)
# big endian 
def GetInt32BigEndian(pBuff):
    if len(pBuff) < 4:
        return -1
    return ord(pBuff[3]) + (ord(pBuff[2])<<8) + (ord(pBuff[1])<<16) + (ord(pBuff[0])<<24)

def GetSizeJpeg(pBuff):
    totalLen = len(pBuff)
    if totalLen<1024:
        return (0,0)
    for i in range(0, totalLen-3):
        if GetInt16BigEndian(pBuff[i:i+2]) == 0xFFC0:
            x = GetInt16BigEndian(pBuff[i+5:i+7])
            y = GetInt16BigEndian(pBuff[i+7:i+9])
            return (y, x)
    return (0,0)


def GetSizePng(pBuff):
    totalLen = len(pBuff)
    if totalLen<1024:
        return (0,0)
    
    for i in range(0, totalLen-3):
        if pBuff[i:i+4] == """IHDR""":
            x = GetInt32BigEndian(pBuff[i+4:i+8])
            y = GetInt32BigEndian(pBuff[i+8:i+12])
            return (x, y)
    return (0,0)
