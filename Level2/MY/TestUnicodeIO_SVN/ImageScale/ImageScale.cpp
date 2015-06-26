#include "stdafx.h"
#include <stdio.h>
#include <string>
#include <windows.h>
#include <math.h>

enum StretchMode
{
    nearest,
    bilinear,
    third
};


double S(double x)
{
//          PI=3.1415926535897932385; 
//          S(x) = sin(PI*x)/(PI*x)
//                  ┏ 1-2*Abs(x)^2+Abs(x)^3,            0<=Abs(x)<1
//          S(x)=  ｛  4-8*Abs(x)+5*Abs(x)^2-Abs(x)^3,   1<=Abs(x)<2
//                  ┗ 0,                                Abs(x)>=2

    const double a = -0.49; //a还可以取 a=-2,-1,-0.75,-0.5等等，起到调节锐化或模糊程度的作用

    if (x<0) x=-x;
    double x2=x*x;
    double x3=x2*x;
    if(x<=1)
        return (a+2)*x3 - (a+3)*x2 + 1;
    else if(x<=2) 
        return a*x3 - (5*a)*x2 + (8*a)*x - (4*a);
    else
        return 0;
}

void Stretch(const std::string& srcFile,const std::string& desFile,int desPixelInLine,int desLineCount,StretchMode mode)
{
    BITMAPFILEHEADER bmfHeader;
    BITMAPINFOHEADER bmiHeader;

    FILE *pFile;
    fopen_s(&pFile, srcFile.c_str(),"rb");
    if (pFile == NULL)
    {
        printf("open bmp file error.");
        exit(-1);
    }
    fseek(pFile,0,SEEK_SET);
    fread(&bmfHeader,sizeof(BITMAPFILEHEADER), 1, pFile);
    fread(&bmiHeader,sizeof(BITMAPINFOHEADER), 1, pFile);
    int bitInPixel = bmiHeader.biBitCount;
    if (bitInPixel < 16)
    {
        printf("16bit bmp file not supported.");
        return;
    }
    int srcPixelInLine = bmiHeader.biWidth;
    int srcLineCount = bmiHeader.biHeight;
    int byteInPixel = bitInPixel/8;

    int srcByteInLine = byteInPixel * srcPixelInLine;
    int srcByteTotal = srcByteInLine * srcLineCount;
    BYTE* pSrcBuf = new BYTE[srcByteTotal];
    memset(pSrcBuf, 0, srcByteTotal);

    int srcPaddingByte = (4- srcByteInLine%4) % 4;
    for(int i=0; i<srcLineCount; i++)
    {
        fread(&pSrcBuf[i*srcByteInLine], srcByteInLine, 1, pFile);
        fseek(pFile, srcPaddingByte, SEEK_CUR);
    }
    fclose(pFile);

//     FILE* pFileWrite1;
//     fopen_s(&pFileWrite1, desFile.c_str(), "wb");
//     fwrite(&bmfHeader, sizeof(BITMAPFILEHEADER), 1, pFileWrite1);
//     fwrite(&bmiHeader, sizeof(BITMAPINFOHEADER), 1, pFileWrite1);
//     for(int i=0; i<srcLineCount; i++)
//     {
//         fwrite(&pSrcBuf[i*srcByteInLine], srcByteInLine, 1, pFileWrite1);
//         fwrite("0", srcPaddingByte, 1, pFileWrite1);
//     }
//     fclose(pFileWrite1);
//     return;

    int desByteInLine = byteInPixel * desPixelInLine;
    int desByteTotal = desByteInLine * desLineCount;
    BYTE* pDesBuf = new BYTE[desByteTotal];
    memset(pDesBuf, 0, desByteTotal);

    double lineRate = (double)srcLineCount / desLineCount;
    double pixelRate = (double)srcPixelInLine / desPixelInLine;
    switch(mode)
    {
    case nearest:
        for(int nCurLine=0; nCurLine<desLineCount; nCurLine++)
        {
            for(int nCurPixel=0; nCurPixel<desPixelInLine; nCurPixel++)
            {
                int oriLine = (int)(nCurLine * lineRate - 0.5);
                int oriPixel = (int)(nCurPixel * pixelRate - 0.5);
                for(int nCurByte=0; nCurByte<byteInPixel; nCurByte++)
                {
                    pDesBuf[desByteInLine*nCurLine+byteInPixel*nCurPixel+nCurByte] = 
                        pSrcBuf[srcByteInLine*oriLine+byteInPixel*oriPixel+nCurByte];
                }
            }
        }
        break;
    case bilinear:
        //f(i+u,j+v) = (1-u)(1-v)f(i,j) + (1-u)v*f(i,j+1) + u(1-v)f(i+1,j) + uv*f(i+1,j+1)
        for(int nCurLine=0; nCurLine<desLineCount; nCurLine++)
        {
            for(int nCurPixel=0; nCurPixel<desPixelInLine; nCurPixel++)
            {
                double fOriLine = nCurLine * lineRate;
                double fOriPixel = nCurPixel * pixelRate;
                double u = fOriLine - (int)fOriLine;
                double v = fOriPixel - (int)fOriPixel;
                int nline0 = ((int)fOriLine);
                int nline1 = ((int)fOriLine+1)<srcLineCount ? ((int)fOriLine+1) : srcLineCount-1;
                int npixel0 = ((int)fOriPixel);
                int npixel1 = ((int)fOriPixel+1)<srcPixelInLine ? ((int)fOriPixel+1) : srcPixelInLine-1;

                for(int nCurByte=0; nCurByte<byteInPixel; nCurByte++)
                {
                    if(u<0.000000001 && v<0.000000001)
                    {
                        pDesBuf[desByteInLine*nCurLine+byteInPixel*nCurPixel+nCurByte] = 
                            pSrcBuf[srcByteInLine*nline0+byteInPixel*npixel0+nCurByte];
                    }
                    pDesBuf[desByteInLine*nCurLine+byteInPixel*nCurPixel+nCurByte] = 
                        pSrcBuf[srcByteInLine*nline0+byteInPixel*npixel0+nCurByte] * (1-u) * (1-v)
                        + pSrcBuf[srcByteInLine*nline1+byteInPixel*npixel0+nCurByte] * u * (1-v)
                        + pSrcBuf[srcByteInLine*nline0+byteInPixel*npixel1+nCurByte] * (1-u) * v
                        + pSrcBuf[srcByteInLine*nline1+byteInPixel*npixel1+nCurByte] * u * v;
                }
            }
        }
        break;
    case third:
//         f(i+u,j+v) = [A] * [B] * [C]
//         [A]=    [S(u + 1) S(u + 0) S(u - 1) S(u - 2)]
//                 ┏ f(i-1, j-1) f(i-1, j+0) f(i-1, j+1) f(i-1, j+2) ┓
//         [B]=    ┃ f(i+0, j-1) f(i+0, j+0) f(i+0, j+1) f(i+0, j+2) ┃
//                 ┃ f(i+1, j-1) f(i+1, j+0) f(i+1, j+1) f(i+1, j+2) ┃
//                 ┗ f(i+2, j-1) f(i+2, j+0) f(i+2, j+1) f(i+2, j+2) ┛
//                 ┏ S(v + 1) ┓
//         [C]=    ┃ S(v + 0) ┃
//                 ┃ S(v - 1) ┃
//                 ┗ S(v - 2) ┛
//                 ┏ 1-2*Abs(x)^2+Abs(x)^3,            0<=Abs(x)<1
//         S(x)=  ｛  4-8*Abs(x)+5*Abs(x)^2-Abs(x)^3,   1<=Abs(x)<2
//                 ┗ 0,                                Abs(x)>=2
//         S(x) = Sin(Pi*x)/(Pi*x);

        for(int nCurLine=0; nCurLine<desLineCount; nCurLine++)
        {
            for(int nCurPixel=0; nCurPixel<desPixelInLine; nCurPixel++)
            {
                double fOriLine = nCurLine * lineRate;
                double fOriPixel = nCurPixel * pixelRate;
                double u = fOriLine - (int)fOriLine;
                double v = fOriPixel - (int)fOriPixel;
                int nline0 = ((int)fOriLine-1)>0 ? ((int)fOriLine-1) : 0;
                int nline1 = ((int)fOriLine);
                int nline2 = ((int)fOriLine+1)<srcLineCount ? ((int)fOriLine+1) : srcLineCount-1;
                int nline3 = ((int)fOriLine+2)<srcLineCount ? ((int)fOriLine+2) : srcLineCount-1;
                int npixel0 = ((int)fOriPixel-1)>0 ? ((int)fOriPixel-1) : 0;
                int npixel1 = ((int)fOriPixel);
                int npixel2 = ((int)fOriPixel+1)<srcPixelInLine ? ((int)fOriPixel+1) : srcPixelInLine-1;
                int npixel3 = ((int)fOriPixel+2)<srcPixelInLine ? ((int)fOriPixel+2) : srcPixelInLine-1;

                double su0 = S(u + 1);
                double su1 = S(u);
                double su2 = S(u - 1);
                double su3 = S(u - 2);

                double sv0 = S(v + 1);
                double sv1 = S(v);
                double sv2 = S(v - 1);
                double sv3 = S(v - 2);

                for(int nCurByte=0; nCurByte<byteInPixel; nCurByte++)
                {
                    if(u<0.000000001 && v<0.000000001)
                    {
                        pDesBuf[desByteInLine*nCurLine+byteInPixel*nCurPixel+nCurByte] = 
                            pSrcBuf[srcByteInLine*nline1+byteInPixel*npixel1+nCurByte];
                    }
                    BYTE pixel00 = pSrcBuf[srcByteInLine*nline0+byteInPixel*npixel0+nCurByte];
                    BYTE pixel10 = pSrcBuf[srcByteInLine*nline1+byteInPixel*npixel0+nCurByte];
                    BYTE pixel20 = pSrcBuf[srcByteInLine*nline2+byteInPixel*npixel0+nCurByte];
                    BYTE pixel30 = pSrcBuf[srcByteInLine*nline3+byteInPixel*npixel0+nCurByte];
                    BYTE pixel01 = pSrcBuf[srcByteInLine*nline0+byteInPixel*npixel1+nCurByte];
                    BYTE pixel11 = pSrcBuf[srcByteInLine*nline1+byteInPixel*npixel1+nCurByte];
                    BYTE pixel21 = pSrcBuf[srcByteInLine*nline2+byteInPixel*npixel1+nCurByte];
                    BYTE pixel31 = pSrcBuf[srcByteInLine*nline3+byteInPixel*npixel1+nCurByte];
                    BYTE pixel02 = pSrcBuf[srcByteInLine*nline0+byteInPixel*npixel2+nCurByte];
                    BYTE pixel12 = pSrcBuf[srcByteInLine*nline1+byteInPixel*npixel2+nCurByte];
                    BYTE pixel22 = pSrcBuf[srcByteInLine*nline2+byteInPixel*npixel2+nCurByte];
                    BYTE pixel32 = pSrcBuf[srcByteInLine*nline3+byteInPixel*npixel2+nCurByte];
                    BYTE pixel03 = pSrcBuf[srcByteInLine*nline0+byteInPixel*npixel3+nCurByte];
                    BYTE pixel13 = pSrcBuf[srcByteInLine*nline1+byteInPixel*npixel3+nCurByte];
                    BYTE pixel23 = pSrcBuf[srcByteInLine*nline2+byteInPixel*npixel3+nCurByte];
                    BYTE pixel33 = pSrcBuf[srcByteInLine*nline3+byteInPixel*npixel3+nCurByte];

                    double res0 = su0*pixel00 + su1*pixel10 + su2*pixel20 + su3*pixel30;
                    double res1 = su0*pixel01 + su1*pixel11 + su2*pixel21 + su3*pixel31;
                    double res2 = su0*pixel02 + su1*pixel12 + su2*pixel22 + su3*pixel32;
                    double res3 = su0*pixel03 + su1*pixel13 + su2*pixel23 + su3*pixel33;

                    double resBYTE = res0*sv0 + res1*sv1 + res2*sv2 + res3*sv3;
                    if(resBYTE > 255)
                        resBYTE = 255;
                    pDesBuf[desByteInLine*nCurLine+byteInPixel*nCurPixel+nCurByte] = resBYTE;
                }
            }
        }
        break;
    default:
        break;
    }

    BITMAPFILEHEADER fileHeader;
    fileHeader.bfType = 0x4D42;
    int desPaddingByte = (4-desByteInLine%4) % 4;
    fileHeader.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + desByteInLine*desLineCount;
    fileHeader.bfReserved1 = 0;
    fileHeader.bfReserved2 = 0;
    fileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

    BITMAPINFOHEADER inforHeader;
    inforHeader.biSize=sizeof(BITMAPINFOHEADER);
    inforHeader.biWidth=desPixelInLine;
    inforHeader.biHeight=desLineCount;
    inforHeader.biPlanes=1;
    inforHeader.biBitCount=bitInPixel;
    inforHeader.biCompression=BI_RGB;
    inforHeader.biSizeImage=0;
    inforHeader.biXPelsPerMeter=0;
    inforHeader.biYPelsPerMeter=0;
    inforHeader.biClrUsed=0;
    inforHeader.biClrImportant=0;

    FILE* pFileWrite;
    fopen_s(&pFileWrite, desFile.c_str(), "wb");
    fwrite(&fileHeader, sizeof(BITMAPFILEHEADER), 1, pFileWrite);
    fwrite(&inforHeader, sizeof(BITMAPINFOHEADER), 1, pFileWrite);
    for(int i=0; i<desLineCount; i++)
    {
        fwrite(&pDesBuf[i*desByteInLine], desByteInLine, 1, pFileWrite);
        fwrite("0", desPaddingByte, 1, pFileWrite);
    }
    fclose(pFileWrite);
}

int main(int argc, char* argv[])
{
    std::string srcFile("d:\\t.bmp");
    std::string desFileN("d:\\nearest.bmp");
    std::string desFileB("d:\\bilinear.bmp");
    std::string desFileT("d:\\third.bmp");

    Stretch(srcFile, desFileN, 198, 226, nearest);
    Stretch(srcFile, desFileB, 198, 226, bilinear);
    Stretch(srcFile, desFileT, 198, 226, third);
    return 0;
}

