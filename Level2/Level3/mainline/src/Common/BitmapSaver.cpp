
#include "StdAfx.h"
#include "BitmapSaver.h"


//------------------------------------------------------------
// 

static LPBYTE CreateBMPData(HBITMAP hBitmap, DWORD& dwDataSize) 
{ 
	HDC hDC; 
	int iBits; 
	WORD wBitCount = 0; 
	DWORD dwPaletteSize=0, dwBmBitsSize, dwDIBSize;
	BITMAP Bitmap; 
	BITMAPFILEHEADER bmfHdr; 
	BITMAPINFOHEADER bi; 
	LPBITMAPINFOHEADER lpbi;
	HANDLE hPal, hOldPal=NULL;
	LPBYTE pData;

	::GetObject(hBitmap, sizeof(BITMAP), &Bitmap); 

	iBits = Bitmap.bmBitsPixel;
	if(iBits <= 1)   
		wBitCount = 1;   
	else if(iBits <= 4)   
		wBitCount = 4;   
	else if(iBits <= 8)   
		wBitCount = 8;   
	else 
		wBitCount = 24;

	if(wBitCount <= 8)   
		dwPaletteSize = (1 << wBitCount) * sizeof(RGBQUAD);   

	bi.biSize = sizeof(BITMAPINFOHEADER);   
	bi.biWidth = Bitmap.bmWidth;   
	bi.biHeight = Bitmap.bmHeight;   
	bi.biPlanes = 1;   
	bi.biBitCount = wBitCount;   
	bi.biCompression = BI_RGB;   
	bi.biSizeImage = 0;   
	bi.biXPelsPerMeter = 0;   
	bi.biYPelsPerMeter = 0;   
	bi.biClrUsed = 0;   
	bi.biClrImportant = 0;   

	dwBmBitsSize = ((Bitmap.bmWidth * wBitCount+31)/32) * 4 * Bitmap.bmHeight;   

	pData = new BYTE[dwBmBitsSize+dwPaletteSize+sizeof(BITMAPINFOHEADER)];   
	lpbi = (LPBITMAPINFOHEADER)pData;   
	*lpbi = bi;   

	hPal  =  GetStockObject(DEFAULT_PALETTE);   
	if(hPal)   
	{   
		hDC = ::GetDC(NULL);
		hOldPal = ::SelectPalette(hDC, (HPALETTE)hPal, FALSE);   
		RealizePalette(hDC);   
	}   


	GetDIBits(hDC, hBitmap, 0, (UINT)Bitmap.bmHeight,   
		(BYTE*)lpbi + sizeof(BITMAPINFOHEADER)   
		+dwPaletteSize, (LPBITMAPINFO)lpbi, DIB_RGB_COLORS);

	if(hOldPal)   
	{   
		SelectPalette(hDC, (HPALETTE)hOldPal, TRUE);   
		RealizePalette(hDC);   
		::ReleaseDC(NULL,   hDC);   
	}   

	bmfHdr.bfType = 0x4D42;
	dwDIBSize = sizeof(BITMAPFILEHEADER) +  sizeof(BITMAPINFOHEADER)   
		+  dwPaletteSize + dwBmBitsSize;       
	bmfHdr.bfSize = dwDIBSize;   
	bmfHdr.bfReserved1 = 0;   
	bmfHdr.bfReserved2 = 0;   
	bmfHdr.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER)     
		+ (DWORD)sizeof(BITMAPINFOHEADER)   
		+ dwPaletteSize;   

	pData = (LPBYTE)GlobalAlloc(GMEM_FIXED, dwDIBSize);
	memcpy(pData, &bmfHdr, sizeof(BITMAPFILEHEADER));   
	memcpy(pData+sizeof(BITMAPFILEHEADER), lpbi, dwDIBSize-sizeof(BITMAPFILEHEADER));

	dwDataSize = dwDIBSize;
	return   pData;   
}
//------------------------------------------------------------



BitmapSaver::BitmapSaver()
	: m_hBitmap(NULL)
	, m_dwDataSize(0)
	, m_pData(NULL)
{
	;
}

BitmapSaver::~BitmapSaver()
{
	if (m_pData)
	{
		GlobalFree((HGLOBAL)m_pData);
	}

}

BOOL BitmapSaver::Init( HBITMAP hBitmap )
{
	m_hBitmap = hBitmap;
	if (m_pData)
	{
		GlobalFree((HGLOBAL)m_pData);
	}

	m_pData = CreateBMPData(m_hBitmap, m_dwDataSize);
	return NULL != m_pData;
}

LPBYTE BitmapSaver::GetBitmapData( DWORD& nBitsSize )
{
	nBitsSize = m_dwDataSize;
	return m_pData;
}

BOOL BitmapSaver::SaveFile( const CString& strFileName )
{
	CFile file;
	if (file.Open(strFileName, CFile::modeCreate | CFile::modeWrite))
	{
		file.Write(m_pData, m_dwDataSize);
		return TRUE;
	}
	return FALSE;
}

#pragma warning(push)
#pragma warning(disable:4996)

HBITMAP BitmapSaver::FromBuffer( LPVOID lpData, DWORD /*dwDataSize*/ )
{
	BYTE*				hDIB;
	BYTE*				lpBuffer = (BYTE*)lpData;
	LPVOID				lpDIBBits;
	BITMAPFILEHEADER	bmfHeader;
	DWORD				bmfHeaderLen;
	HBITMAP             hBitmap;

	bmfHeaderLen = sizeof(BITMAPFILEHEADER);
	strncpy((LPSTR)&bmfHeader,(LPSTR)lpBuffer,bmfHeaderLen);

	if (bmfHeader.bfType != ((WORD) ('M' << 8) | 'B')) 
		return NULL;

	hDIB = lpBuffer + bmfHeaderLen;

	BITMAPINFOHEADER bmiHeader = *(LPBITMAPINFOHEADER)hDIB ;
	BITMAPINFO bmInfo;
	memset((void*)&bmInfo, 0, sizeof(BITMAPINFO));
	bmInfo.bmiHeader = bmiHeader;

	DWORD dwPaletteSize = 0;
	if(bmiHeader.biBitCount <= 8)
		dwPaletteSize = (1 << bmiHeader.biBitCount) * sizeof(RGBQUAD);  

	int nOffSize = (DWORD)sizeof(BITMAPFILEHEADER)     
		+ (DWORD)sizeof(BITMAPINFOHEADER)   
		+ dwPaletteSize; 

	lpDIBBits = lpBuffer + nOffSize;

	bmfHeader.bfOffBits = nOffSize; 


	HDC hDC = ::GetDC(NULL); 
	hBitmap = CreateDIBitmap(hDC,&bmiHeader,CBM_INIT,lpDIBBits,&bmInfo,DIB_RGB_COLORS);
	::ReleaseDC(NULL, hDC);

	return hBitmap;
}


#pragma warning(pop)

HBITMAP BitmapSaver::FromFile( const CString fileName )
{
	CFile file;
	if (file.Open(fileName, CFile::modeRead))
	{
		DWORD dwBytes = (DWORD)file.GetLength();
		LPVOID lpData = (LPVOID)::GlobalAlloc(GMEM_FIXED, dwBytes);
		if (lpData)
		{
			file.Read(lpData, dwBytes);
			HBITMAP hBitmap = FromBuffer(lpData, dwBytes);
			::GlobalFree((HGLOBAL)lpData);
			return hBitmap;
		}
	}
	return NULL;
}


//------------------------------------------------------------
// Code from net
//------------------------------------------------------------
//
// Smooth bitmap resize
//
// Ivaylo Byalkov, November 16, 2000
//

#include <math.h>

///////////////////////////////////////////////////////////

// helper function prototypes
static BITMAPINFO *PrepareRGBBitmapInfo(WORD wWidth, WORD wHeight);
static void ShrinkData(BYTE *pInBuff, WORD wWidth, WORD wHeight,
					   BYTE *pOutBuff, WORD wNewWidth, WORD wNewHeight);
static void EnlargeData(BYTE *pInBuff, WORD wWidth, WORD wHeight,
						BYTE *pOutBuff, WORD wNewWidth, WORD wNewHeight);

///////////////////////////////////////////////////////////
// Main resize function

HBITMAP BitmapSaver::ScaleBitmap(HBITMAP hBmp, WORD wNewWidth, WORD wNewHeight)
{
	BITMAP bmp;
	::GetObject(hBmp, sizeof(BITMAP), &bmp);

	// check for valid size
	if ((bmp.bmWidth > wNewWidth && bmp.bmHeight < wNewHeight) 
		|| (bmp.bmWidth < wNewWidth && bmp.bmHeight > wNewHeight))
		return NULL;

	HDC hDC = ::GetDC(NULL);
	BITMAPINFO *pbi = PrepareRGBBitmapInfo((WORD)bmp.bmWidth, 
		(WORD)bmp.bmHeight);

	BYTE *pData = new BYTE[pbi->bmiHeader.biSizeImage];

	::GetDIBits(hDC, 
		hBmp, 
		0, 
		bmp.bmHeight, 
		pData, 
		pbi, 
		DIB_RGB_COLORS);

	delete pbi;
	pbi = PrepareRGBBitmapInfo(wNewWidth, wNewHeight);
	BYTE *pData2 = new BYTE[pbi->bmiHeader.biSizeImage];

	if(bmp.bmWidth >= wNewWidth 
		&& bmp.bmHeight >= wNewHeight)
		ShrinkData(pData, (WORD)bmp.bmWidth, (WORD)bmp.bmHeight,
		pData2, wNewWidth, wNewHeight);
	else
		EnlargeData(pData, (WORD)bmp.bmWidth, (WORD)bmp.bmHeight,
		pData2, wNewWidth, wNewHeight);

	delete[] pData;

	HBITMAP hResBmp = ::CreateCompatibleBitmap(hDC, 
		wNewWidth, 
		wNewHeight);

	::SetDIBits(hDC, 
		hResBmp, 
		0, 
		wNewHeight, 
		pData2, 
		pbi, 
		DIB_RGB_COLORS);

	::ReleaseDC(NULL, hDC);

	delete pbi;
	delete[] pData2;

	return hResBmp;
}

///////////////////////////////////////////////////////////

BITMAPINFO *PrepareRGBBitmapInfo(WORD wWidth, WORD wHeight)
{
	BITMAPINFO *pRes = new BITMAPINFO;
	::ZeroMemory(pRes, sizeof(BITMAPINFO));
	pRes->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	pRes->bmiHeader.biWidth = wWidth;
	pRes->bmiHeader.biHeight = wHeight;
	pRes->bmiHeader.biPlanes = 1;
	pRes->bmiHeader.biBitCount = 24;
	pRes->bmiHeader.biSizeImage = ((3 * wWidth + 3) & ~3) 
		* wHeight;

	return pRes;
}

///////////////////////////////////////////////////////////

static float *CreateCoeff(int nLen, int nNewLen, BOOL bShrink)
{
	int nSum = 0, nSum2;
	float *pRes = new float[2 * nLen];
	float *pCoeff = pRes;
	float fNorm = (bShrink)? (float)nNewLen / nLen : 1;
	int	  nDenom = (bShrink)? nLen : nNewLen;

	::ZeroMemory(pRes, 2 * nLen * sizeof(float));
	for(int i = 0; i < nLen; i++, pCoeff += 2)
	{
		nSum2 = nSum + nNewLen;
		if(nSum2 > nLen)
		{
			*pCoeff = (float)(nLen - nSum) / nDenom;
			pCoeff[1] = (float)(nSum2 - nLen) / nDenom;
			nSum2 -= nLen;
		}
		else
		{
			*pCoeff = fNorm;
			if(nSum2 == nLen)
			{
				pCoeff[1] = -1;
				nSum2 = 0;
			}
		}
		nSum = nSum2;
	}

	return pRes;
}

///////////////////////////////////////////////////////////

#define F_DELTA		0.0001f

void ShrinkData(BYTE *pInBuff, 
				WORD wWidth, 
				WORD wHeight,
				BYTE *pOutBuff, 
				WORD wNewWidth, 
				WORD wNewHeight)
{
	BYTE  *pLine = pInBuff, *pPix;
	BYTE  *pOutLine = pOutBuff;
	DWORD dwInLn = (3 * wWidth + 3) & ~3;
	DWORD dwOutLn = (3 * wNewWidth + 3) & ~3;
	int   x, y, i, ii;
	BOOL  bCrossRow, bCrossCol;
	float *pRowCoeff = CreateCoeff(wWidth, wNewWidth, TRUE);
	float *pColCoeff = CreateCoeff(wHeight, wNewHeight, TRUE);
	float fTmp, *pXCoeff, *pYCoeff = pColCoeff;
	DWORD dwBuffLn = 3 * wNewWidth * sizeof(float);
	float *fBuff = new float[6 * wNewWidth];
	float *fCurrLn = fBuff, 
		*fCurrPix, 
		*fNextLn = fBuff + 3 * wNewWidth, 
		*fNextPix;

	::ZeroMemory(fBuff, 2 * dwBuffLn);

	y = 0;
	while(y < wNewHeight)
	{
		pPix = pLine;
		pLine += dwInLn;

		fCurrPix = fCurrLn;
		fNextPix = fNextLn;

		x = 0;
		pXCoeff = pRowCoeff;
		bCrossRow = pYCoeff[1] > F_DELTA;
		while(x < wNewWidth)
		{
			fTmp = *pXCoeff * *pYCoeff;
			for(i = 0; i < 3; i++)
				fCurrPix[i] += fTmp * pPix[i];
			bCrossCol = pXCoeff[1] > F_DELTA;
			if(bCrossCol)
			{
				fTmp = pXCoeff[1] * *pYCoeff;
				for(i = 0, ii = 3; i < 3; i++, ii++)
					fCurrPix[ii] += fTmp * pPix[i];
			}

			if(bCrossRow)
			{
				fTmp = *pXCoeff * pYCoeff[1];
				for(i = 0; i < 3; i++)
					fNextPix[i] += fTmp * pPix[i];
				if(bCrossCol)
				{
					fTmp = pXCoeff[1] * pYCoeff[1];
					for(i = 0, ii = 3; i < 3; i++, ii++)
						fNextPix[ii] += fTmp * pPix[i];
				}
			}

			if(fabs(pXCoeff[1]) > F_DELTA)
			{
				x++;
				fCurrPix += 3;
				fNextPix += 3;
			}

			pXCoeff += 2;
			pPix += 3;
		}

		if(fabs(pYCoeff[1]) > F_DELTA)
		{
			// set result line
			fCurrPix = fCurrLn;
			pPix = pOutLine;
			for(i = 3 * wNewWidth; i > 0; i--, fCurrPix++, pPix++)
				*pPix = (BYTE)*fCurrPix;

			// prepare line buffers
			fCurrPix = fNextLn;
			fNextLn = fCurrLn;
			fCurrLn = fCurrPix;
			::ZeroMemory(fNextLn, dwBuffLn);

			y++;
			pOutLine += dwOutLn;
		}
		pYCoeff += 2;
	}

	delete [] pRowCoeff;
	delete [] pColCoeff;
	delete [] fBuff;
} 

///////////////////////////////////////////////////////////

void EnlargeData(BYTE *pInBuff, 
				 WORD wWidth, 
				 WORD wHeight,
				 BYTE *pOutBuff, 
				 WORD wNewWidth, 
				 WORD wNewHeight)
{
	BYTE  *pLine = pInBuff, 
		*pPix = pLine, 
		*pPixOld, 
		*pUpPix, 
		*pUpPixOld;
	BYTE  *pOutLine = pOutBuff, *pOutPix;
	DWORD dwInLn = (3 * wWidth + 3) & ~3;
	DWORD dwOutLn = (3 * wNewWidth + 3) & ~3;
	int   x, y, i;
	BOOL  bCrossRow, bCrossCol;
	float *pRowCoeff = CreateCoeff(wNewWidth, wWidth, FALSE);
	float *pColCoeff = CreateCoeff(wNewHeight, wHeight, FALSE);
	float fTmp, fPtTmp[3], *pXCoeff, *pYCoeff = pColCoeff;

	y = 0;
	while(y < wHeight)
	{
		bCrossRow = pYCoeff[1] > F_DELTA;
		x = 0;
		pXCoeff = pRowCoeff;
		pOutPix = pOutLine;
		pOutLine += dwOutLn;
		pUpPix = pLine;
		if(fabs(pYCoeff[1]) > F_DELTA)
		{
			y++;
			pLine += dwInLn;
			pPix = pLine;
		}

		while(x < wWidth)
		{
			bCrossCol = pXCoeff[1] > F_DELTA;
			pUpPixOld = pUpPix;
			pPixOld = pPix;
			if(fabs(pXCoeff[1]) > F_DELTA)
			{
				x++;
				pUpPix += 3;
				pPix += 3;
			}
			fTmp = *pXCoeff * *pYCoeff;
			for(i = 0; i < 3; i++)
				fPtTmp[i] = fTmp * pUpPixOld[i];
			if(bCrossCol)
			{
				fTmp = pXCoeff[1] * *pYCoeff;
				for(i = 0; i < 3; i++)
					fPtTmp[i] += fTmp * pUpPix[i];
			}
			if(bCrossRow)
			{
				fTmp = *pXCoeff * pYCoeff[1];
				for(i = 0; i < 3; i++)
					fPtTmp[i] += fTmp * pPixOld[i];
				if(bCrossCol)
				{
					fTmp = pXCoeff[1] * pYCoeff[1];
					for(i = 0; i < 3; i++)
						fPtTmp[i] += fTmp * pPix[i];
				}
			}
			for(i = 0; i < 3; i++, pOutPix++)
				*pOutPix = (BYTE)fPtTmp[i];
			pXCoeff += 2;
		}
		pYCoeff += 2;
	}

	delete [] pRowCoeff;
	delete [] pColCoeff;
} 

// end src
