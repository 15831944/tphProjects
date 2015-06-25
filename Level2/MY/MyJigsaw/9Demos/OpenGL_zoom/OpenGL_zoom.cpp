// OpenGL_zoom.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <windows.h>
#include <stdio.h>

#define FXZOOMRATIO 1.5
#define FYZOOMRATIO 1.5
unsigned char *pBmpBuf;
unsigned char *pNewBmpBuf;
int        bmpWidth;
int        bmpHeight;
RGBQUAD    *pColorTable;
int        biBitCount;
long       newBmpWidth;
long       newBmpHeight;
long       newLineByte;


static bool readBmp(const char* bmpName)
{
    FILE *fp=fopen(bmpName,"rb");
    if(fp==0)
    {
        printf("cannot open file");
        return 0;
    }
    fseek(fp,sizeof(BITMAPFILEHEADER),0);
    BITMAPINFOHEADER head;
    fread(&head,sizeof(BITMAPINFOHEADER),1,fp);
    bmpWidth = head.biWidth;
    bmpHeight = head.biHeight;
    biBitCount = head.biBitCount;
    int lineByte = (bmpWidth *biBitCount/8+3)/4*4;
    if(biBitCount == 8)
    {
        pColorTable = new RGBQUAD[256];
        fread(pColorTable,sizeof(RGBQUAD),256,fp);
    }
    pBmpBuf = new unsigned char [lineByte *bmpHeight];
    fread(pBmpBuf,1,lineByte *bmpHeight,fp);
    fclose(fp);
    return 1;

}

static bool saveBmp(const char* bmpName,unsigned char *imgBuf,int width,int height,int biBitCount,RGBQUAD *pColorTable)
{
    if(!imgBuf)
        return 0;
    int colorTablesize = 0;
    if(biBitCount == 8)
        colorTablesize =1024;
    int lineByte = (width * biBitCount/8+3)/4*4;
    FILE *fp = fopen(bmpName,"wb");
    if(fp == 0) return 0;
    BITMAPFILEHEADER fileHead;
    fileHead.bfType= 0x4d42;
    fileHead.bfSize = sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER) + colorTablesize + lineByte *height;
    fileHead.bfReserved1 = 0;
    fileHead.bfReserved2 = 0;
    fileHead.bfOffBits = 54 +colorTablesize;
    fwrite(&fileHead,sizeof(BITMAPFILEHEADER),1,fp);
    BITMAPINFOHEADER head;
    head.biBitCount = biBitCount;
    head.biClrImportant = 0;
    head.biClrUsed = 0;
    head.biCompression = 0;
    head.biHeight = height;
    head.biPlanes =1;
    head.biSize = 40;
    head.biSizeImage = lineByte *height;
    head.biWidth = width;
    head.biXPelsPerMeter = 0;
    head.biYPelsPerMeter = 0;
    fwrite(&head,sizeof(BITMAPINFOHEADER),1,fp);
    if(biBitCount == 8)
        fwrite(pColorTable,sizeof(RGBQUAD),256,fp);
    fwrite(imgBuf,height * lineByte,1,fp);
    fclose(fp);
    return 1;

}

static bool bmpzoom(const char* szSrcBmp, const char* szDstBmp)
{


    readBmp(szSrcBmp);
    newBmpWidth = (long) ((bmpWidth * FXZOOMRATIO) +0.5);

    newBmpHeight = (long) (bmpHeight * FYZOOMRATIO +0.5);

    newLineByte = (newBmpWidth * biBitCount/8+3)/4*4;

    pNewBmpBuf = new unsigned char [newLineByte * newBmpHeight];


    //printf("width = %d, height = %d,biBitCount = %d\n",bmpWidth,bmpHeight,biBitCount);
    //printf("newwidth = %d, newheight = %d,biBitCount = %d\n",newBmpWidth,newBmpHeight,biBitCount);
    long i,j,k;
    long i0,j0;
    int lineByte =(bmpWidth*biBitCount/8+3)/4*4;
    if(biBitCount==8)
    {
        for(i = 0;i < bmpHeight/2;i++)
        {
            for(j = 0;j<bmpWidth/2;j++)
                *(pBmpBuf+i*lineByte+j) = 0;

        }
    }

    if(biBitCount == 24)
    {
        for(i = 0;i < newBmpHeight;i++)
        {
            for(j = 0; j<newBmpWidth;j++)
                for(k=0;k<3;k++) 

                {   
                    i0 = (long)(i/FYZOOMRATIO+0.5); 
                    j0 = (long)(j/FXZOOMRATIO+0.5);
                    if((j0 >=0) && (j0 < bmpWidth) && (i0 >=0)&& (i0 <bmpHeight))
                    {

                        *(pNewBmpBuf+i*newLineByte+j*3+k) = *(pBmpBuf+i0*lineByte+j0*3+k);
                    }
                    else
                    {
                        *(pNewBmpBuf+i*newLineByte+j*3+k)=255;
                    }

                }
        }
    }
    saveBmp(szDstBmp,pNewBmpBuf,newBmpWidth,newBmpHeight,biBitCount,pColorTable);
    delete []pNewBmpBuf;
    if(biBitCount == 8)
        delete []pColorTable;
    return 1;
}



int _tmain(int argc, _TCHAR* argv[])
{
    char str1[80]; 
    char str2[80];
    bmpzoom("d:\\1.bmp", "d:\\2.bmp");
}