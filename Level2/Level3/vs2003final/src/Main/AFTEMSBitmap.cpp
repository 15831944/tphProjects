// AFTEMSBitmap.cpp: implementation of the CAFTEMSBitmap class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AFTEMSBitmap.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// DDBToDIB		- Creates a DIB from a DDB
// bitmap		- Device dependent bitmap
// dwCompression	- Type of compression - see BITMAPINFOHEADER
// pPal			- Logical palette
HANDLE CAFTEMSBitmap::DDBToDIB( CBitmap& bitmap, DWORD dwCompression, CPalette* pPal ) 
{
	BITMAP			bm;
	BITMAPINFOHEADER	bi;
	LPBITMAPINFOHEADER 	lpbi;
	DWORD			dwLen;
	HANDLE			hDIB;
	HANDLE			handle;
	HDC 			hDC;
	HPALETTE		hPal;
	
	
	ASSERT( bitmap.GetSafeHandle() );
	
	// The function has no arg for bitfields
	if( dwCompression == BI_BITFIELDS )
		return NULL;
	
	// If a palette has not been supplied use defaul palette
	hPal = (HPALETTE) pPal->GetSafeHandle();
	if (hPal==NULL)
		hPal = (HPALETTE) GetStockObject(DEFAULT_PALETTE);
	
	// Get bitmap information
	bitmap.GetObject(sizeof(bm),(LPSTR)&bm);
	
	// Initialize the bitmapinfoheader
	bi.biSize		= sizeof(BITMAPINFOHEADER);
	bi.biWidth		= bm.bmWidth;
	bi.biHeight 		= bm.bmHeight;
	bi.biPlanes 		= 1;
	bi.biBitCount		= bm.bmPlanes * bm.bmBitsPixel;
	bi.biCompression	= dwCompression;
	bi.biSizeImage		= 0;
	bi.biXPelsPerMeter	= 0;
	bi.biYPelsPerMeter	= 0;
	bi.biClrUsed		= 0;
	bi.biClrImportant	= 0;
	
	// Compute the size of the  infoheader and the color table
	int nColors = (1 << bi.biBitCount);
	if( nColors > 256 ) 
		nColors = 0;
	dwLen  = bi.biSize + nColors * sizeof(RGBQUAD);
	
	// We need a device context to get the DIB from
	hDC = GetDC(NULL);
	hPal = SelectPalette(hDC,hPal,FALSE);
	RealizePalette(hDC);
	
	// Allocate enough memory to hold bitmapinfoheader and color table
	hDIB = GlobalAlloc(GMEM_FIXED,dwLen);
	
	if (!hDIB){
		SelectPalette(hDC,hPal,FALSE);
		ReleaseDC(NULL,hDC);
		return NULL;
	}
	
	lpbi = (LPBITMAPINFOHEADER)hDIB;
	
	*lpbi = bi;
	
	// Call GetDIBits with a NULL lpBits param, so the device driver 
	// will calculate the biSizeImage field 
	GetDIBits(hDC, (HBITMAP)bitmap.GetSafeHandle(), 0L, (DWORD)bi.biHeight,
		(LPBYTE)NULL, (LPBITMAPINFO)lpbi, (DWORD)DIB_RGB_COLORS);
	
	bi = *lpbi;
	
	// If the driver did not fill in the biSizeImage field, then compute it
	// Each scan line of the image is aligned on a DWORD (32bit) boundary
	if (bi.biSizeImage == 0){
		bi.biSizeImage = ((((bi.biWidth * bi.biBitCount) + 31) & ~31) / 8) 
			* bi.biHeight;
		
		// If a compression scheme is used the result may infact be larger
		// Increase the size to account for this.
		if (dwCompression != BI_RGB)
			bi.biSizeImage = (bi.biSizeImage * 3) / 2;
	}
	
	// Realloc the buffer so that it can hold all the bits
	dwLen += bi.biSizeImage;
	if (handle = GlobalReAlloc(hDIB, dwLen, GMEM_MOVEABLE))
		hDIB = handle;
	else{
		GlobalFree(hDIB);
		
		// Reselect the original palette
		SelectPalette(hDC,hPal,FALSE);
		ReleaseDC(NULL,hDC);
		return NULL;
	}
	
	// Get the bitmap bits
	lpbi = (LPBITMAPINFOHEADER)hDIB;
	
	// FINALLY get the DIB
	BOOL bGotBits = GetDIBits( hDC, (HBITMAP)bitmap.GetSafeHandle(),
		0L,				// Start scan line
		(DWORD)bi.biHeight,		// # of scan lines
		(LPBYTE)lpbi 			// address for bitmap bits
		+ (bi.biSize + nColors * sizeof(RGBQUAD)),
		(LPBITMAPINFO)lpbi,		// address of bitmapinfo
		(DWORD)DIB_RGB_COLORS);		// Use RGB for color table
	
	if( !bGotBits )
	{
		GlobalFree(hDIB);
		
		SelectPalette(hDC,hPal,FALSE);
		ReleaseDC(NULL,hDC);
		return NULL;
	}
	
	SelectPalette(hDC,hPal,FALSE);
	ReleaseDC(NULL,hDC);
	return hDIB;
}

HBITMAP CAFTEMSBitmap::DIBToDDB( HANDLE hDIB )
{
	LPBITMAPINFOHEADER	lpbi;
	HBITMAP 		hbm;
	CPalette		pal;
	CPalette*		pOldPal;
	CClientDC		dc(NULL);
	
	if (hDIB == NULL)
		return NULL;
	
	lpbi = (LPBITMAPINFOHEADER)hDIB;
	
	int nColors = lpbi->biClrUsed ? lpbi->biClrUsed : 
	1 << lpbi->biBitCount;
	
	BITMAPINFO &bmInfo = *(LPBITMAPINFO)hDIB ;
	LPVOID lpDIBBits;
	if( bmInfo.bmiHeader.biBitCount > 8 )
		lpDIBBits = (LPVOID)((LPDWORD)(bmInfo.bmiColors + 
		bmInfo.bmiHeader.biClrUsed) + 
		((bmInfo.bmiHeader.biCompression == BI_BITFIELDS) ? 3 : 0));
	else
		lpDIBBits = (LPVOID)(bmInfo.bmiColors + nColors);
	
	// Create and select a logical palette if needed
	if( nColors <= 256 && dc.GetDeviceCaps(RASTERCAPS) & RC_PALETTE)
	{
		UINT nSize = sizeof(LOGPALETTE) + (sizeof(PALETTEENTRY) * nColors);
		LOGPALETTE *pLP = (LOGPALETTE *) new BYTE[nSize];
		
		pLP->palVersion = 0x300;
		pLP->palNumEntries = nColors;
		
		for( int i=0; i < nColors; i++)
		{
			pLP->palPalEntry[i].peRed = bmInfo.bmiColors[i].rgbRed;
			pLP->palPalEntry[i].peGreen = bmInfo.bmiColors[i].rgbGreen;
			pLP->palPalEntry[i].peBlue = bmInfo.bmiColors[i].rgbBlue;
			pLP->palPalEntry[i].peFlags = 0;
		}
		
		pal.CreatePalette( pLP );
		
		delete[] pLP;
		
		// Select and realize the palette
		pOldPal = dc.SelectPalette( &pal, FALSE );
		dc.RealizePalette();
	}
	
	
	hbm = CreateDIBitmap(dc.GetSafeHdc(),		// handle to device context
		(LPBITMAPINFOHEADER)lpbi,	// pointer to bitmap info header 
		(LONG)CBM_INIT,			// initialization flag
		lpDIBBits,			// pointer to initialization data 
		(LPBITMAPINFO)lpbi,		// pointer to bitmap info
		DIB_RGB_COLORS );		// color-data usage 
	
	if (pal.GetSafeHandle())
		dc.SelectPalette(pOldPal,FALSE);
	
	return hbm;
}

// DrawDIB	- Draws a DIB onto a device// pDC		- Pointer to a device context
// hDIB		- Handle of the device-independent bitmap
// pPal		- Pointer to a palette associated with the DIB
// xDest	- x-coordinate of the upper-left corner of the destination rect
// yDest	- y-coordinate of the upper-left corner of the destination rect
void CAFTEMSBitmap::DrawDIB( CDC* pDC, HGLOBAL hDIB, CPalette *pPal, int xDest, int yDest )
{
	LPVOID  lpDIBBits;              // Pointer to DIB bits
	BOOL    bSuccess=FALSE;         // Success/fail flag	
	BITMAPINFO &bmInfo = *(LPBITMAPINFO)hDIB ;
	int nColors = bmInfo.bmiHeader.biClrUsed ? bmInfo.bmiHeader.biClrUsed : 
	1 << bmInfo.bmiHeader.biBitCount;		// Compute the address of the bitmap bits
	
	if( bmInfo.bmiHeader.biBitCount > 8 )
		lpDIBBits = (LPVOID)((LPDWORD)(bmInfo.bmiColors + 
		bmInfo.bmiHeader.biClrUsed) + 
		((bmInfo.bmiHeader.biCompression == BI_BITFIELDS) ? 3 : 0));	
	else
		lpDIBBits = (LPVOID)(bmInfo.bmiColors + nColors);	
	
	// Select and realize the palette if one supplied and if device supports it
	if( pPal && (pDC->GetDeviceCaps(RASTERCAPS) & RC_PALETTE) )	
	{
		pDC->SelectPalette(pPal, FALSE);		
		pDC->RealizePalette();	
	}	
	
	::SetDIBitsToDevice(pDC->m_hDC, 		 // hDC		
		xDest,					 // DestX
		yDest,					 // DestY		
		bmInfo.bmiHeader.biWidth,		 // nDestWidth
		bmInfo.bmiHeader.biHeight,		 // nDestHeight		
		0,					 // SrcX		
		0,					 // SrcY
		0,					 // nStartScan		
		bmInfo.bmiHeader.biHeight,		 // nNumScans
		lpDIBBits,				 // lpBits		
		(LPBITMAPINFO)hDIB, 			 // lpBitsInfo
		DIB_RGB_COLORS);			 // wUsage
}

// DrawBitmap	- Draws a bitmap (DDB & DIB section) onto a device
// pDC		- Pointer to a device context// hBitmap	- Handle of the bitmap
// hPal		- Handle of a logical palette associated with the bitmap
// xDest	- x-coordinate of the upper-left corner of the destination rect
// yDest	- y-coordinate of the upper-left corner of the destination rect
void CAFTEMSBitmap::DrawBitmap( CDC *pDC, 
							   HBITMAP hBitmap, 
							   HPALETTE hPal, 
							   int xDest, 
							   int yDest )
{	// Get logical coordinates	
	BITMAP bm;
	::GetObject( hBitmap, sizeof( bm ), &bm );
	CPoint size( bm.bmWidth, bm.bmHeight );	
	
	pDC->DPtoLP(&size);	
	CPoint org(0,0);
	pDC->DPtoLP(&org);	
	
	// Create a memory DC compatible with the destination DC
	CDC memDC;	
	memDC.CreateCompatibleDC( pDC );
	memDC.SetMapMode( pDC->GetMapMode() );		
	
	//memDC.SelectObject( &bitmap );
	HBITMAP hBmOld = (HBITMAP)::SelectObject( memDC.m_hDC, hBitmap );		
	
	// Select and realize the palette
	if( hPal && pDC->GetDeviceCaps(RASTERCAPS) & RC_PALETTE)	
	{
		SelectPalette( pDC->GetSafeHdc(), hPal, FALSE );		
		pDC->RealizePalette();	
	}
	
	pDC->BitBlt(xDest, yDest, size.x, size.y, &memDC, org.x, org.y, SRCCOPY);
	::SelectObject( memDC.m_hDC, hBmOld );
	memDC.DeleteDC();
}

BOOL CAFTEMSBitmap::LoadBMP( LPCTSTR sBMPFile, HGLOBAL *phDIB, CPalette *pPal )
{
	CFile file;
	if( !file.Open( sBMPFile, CFile::modeRead) )
		return FALSE;
	
	BITMAPFILEHEADER bmfHeader;
	long nFileLen;
	
	nFileLen = static_cast<long>(file.GetLength());
	
	// Read file header
	if (file.Read((LPSTR)&bmfHeader, sizeof(bmfHeader)) != sizeof(bmfHeader))
		return FALSE;
	
	// File type should be 'BM'
	if (bmfHeader.bfType != ((WORD) ('M' << 8) | 'B'))
		return FALSE;
	
	HGLOBAL hDIB = ::GlobalAlloc(GMEM_FIXED, nFileLen);
	if (hDIB == 0)
		return FALSE;
	
	// Read the remainder of the bitmap file.
	if (file.Read((LPSTR)hDIB, nFileLen - sizeof(BITMAPFILEHEADER)) !=
		nFileLen - sizeof(BITMAPFILEHEADER) )
	{
		::GlobalFree(hDIB);
		return FALSE;
	}
	
	BITMAPINFO &bmInfo = *(LPBITMAPINFO)hDIB ;
	
	int nColors = bmInfo.bmiHeader.biClrUsed ? bmInfo.bmiHeader.biClrUsed : 
	1 << bmInfo.bmiHeader.biBitCount;
	
	// Create the palette
	if( nColors <= 256 )
	{
		UINT nSize = sizeof(LOGPALETTE) + (sizeof(PALETTEENTRY) * nColors);
		LOGPALETTE *pLP = (LOGPALETTE *) new BYTE[nSize];
		
		pLP->palVersion = 0x300;
		pLP->palNumEntries = nColors;
		
		for( int i=0; i < nColors; i++)
		{
			pLP->palPalEntry[i].peRed = bmInfo.bmiColors[i].rgbRed;
			pLP->palPalEntry[i].peGreen = bmInfo.bmiColors[i].rgbGreen;
			pLP->palPalEntry[i].peBlue = bmInfo.bmiColors[i].rgbBlue;
			pLP->palPalEntry[i].peFlags = 0;
		}
		
		pPal->CreatePalette( pLP );
		delete[] pLP;
	}
	
	*phDIB = hDIB;
	return TRUE;
}

void CAFTEMSBitmap::DrawDIB( CDC* pDC, HGLOBAL hDIB, CPalette *pPal )
{
	LPVOID	lpDIBBits;		// Pointer to DIB bits
	BOOL	bSuccess=FALSE; 	// Success/fail flag
	
	BITMAPINFO &bmInfo = *(LPBITMAPINFO)hDIB ;
	int nColors = bmInfo.bmiHeader.biClrUsed ? bmInfo.bmiHeader.biClrUsed : 
	1 << bmInfo.bmiHeader.biBitCount;
	
	if( bmInfo.bmiHeader.biBitCount > 8 )
		lpDIBBits = (LPVOID)((LPDWORD)(bmInfo.bmiColors +
		bmInfo.bmiHeader.biClrUsed) +
		((bmInfo.bmiHeader.biCompression == BI_BITFIELDS) ? 3 : 0));
	else
		lpDIBBits = (LPVOID)(bmInfo.bmiColors + nColors);
	
	if( pPal && (pDC->GetDeviceCaps(RASTERCAPS) & RC_PALETTE) )
	{
		pDC->SelectPalette(pPal, FALSE);
		pDC->RealizePalette();
	}
	
	::SetDIBitsToDevice(pDC->m_hDC, 				// hDC
		0,					// DestX
		0,					// DestY
		bmInfo.bmiHeader.biWidth,		// nDestWidth
		bmInfo.bmiHeader.biHeight,		// nDestHeight
		0,					// SrcX
		0,					// SrcY
		0,					// nStartScan
		bmInfo.bmiHeader.biHeight,		// nNumScans
		lpDIBBits,				// lpBits
		(LPBITMAPINFO)hDIB,			// lpBitsInfo
		DIB_RGB_COLORS); 			// wUsage
}

// LoadBMPImage	- Loads a BMP file and creates a bitmap GDI object
//		  also creates logical palette for it.
// Returns	- TRUE for success
// sBMPFile	- Full path of the BMP file
// bitmap	- The bitmap object to initialize
// pPal		- Will hold the logical palette. Can be NULL
BOOL CAFTEMSBitmap::LoadBMPImage( LPCTSTR sBMPFile, CBitmap& bitmap, CPalette *pPal )
{
	CFile file;
	if( !file.Open( sBMPFile, CFile::modeRead) )
		return FALSE;
	
	BITMAPFILEHEADER bmfHeader;
	
	// Read file header
	if (file.Read((LPSTR)&bmfHeader, sizeof(bmfHeader)) != sizeof(bmfHeader))
		return FALSE;
	
	// File type should be 'BM'
	if (bmfHeader.bfType != ((WORD) ('M' << 8) | 'B'))
		return FALSE;
	
	// Get length of the remainder of the file and allocate memory
	DWORD nPackedDIBLen = static_cast<DWORD>(file.GetLength()) - sizeof(BITMAPFILEHEADER);
	HGLOBAL hDIB = ::GlobalAlloc(GMEM_FIXED, nPackedDIBLen);
	if (hDIB == 0)
		return FALSE;
	
	// Read the remainder of the bitmap file.
	if (file.Read((LPSTR)hDIB, nPackedDIBLen) != nPackedDIBLen )
	{
		::GlobalFree(hDIB);
		return FALSE;
	}
	
	
	BITMAPINFOHEADER &bmiHeader = *(LPBITMAPINFOHEADER)hDIB ;
	BITMAPINFO &bmInfo = *(LPBITMAPINFO)hDIB ;
	
	// If bmiHeader.biClrUsed is zero we have to infer the number
	// of colors from the number of bits used to specify it.
	int nColors = bmiHeader.biClrUsed ? bmiHeader.biClrUsed : 
	1 << bmiHeader.biBitCount;
	
	LPVOID lpDIBBits;
	if( bmInfo.bmiHeader.biBitCount > 8 )
		lpDIBBits = (LPVOID)((LPDWORD)(bmInfo.bmiColors + bmInfo.bmiHeader.biClrUsed) + 
		((bmInfo.bmiHeader.biCompression == BI_BITFIELDS) ? 3 : 0));
	else
		lpDIBBits = (LPVOID)(bmInfo.bmiColors + nColors);
	
	// Create the logical palette
	if( pPal != NULL )
	{
		// Create the palette
		if( nColors <= 256 )
		{
			UINT nSize = sizeof(LOGPALETTE) + (sizeof(PALETTEENTRY) * nColors);
			LOGPALETTE *pLP = (LOGPALETTE *) new BYTE[nSize];
			
			pLP->palVersion = 0x300;
			pLP->palNumEntries = nColors;
			
			for( int i=0; i < nColors; i++)
			{
				pLP->palPalEntry[i].peRed = bmInfo.bmiColors[i].rgbRed;
				pLP->palPalEntry[i].peGreen = bmInfo.bmiColors[i].rgbGreen;
				pLP->palPalEntry[i].peBlue = bmInfo.bmiColors[i].rgbBlue;
				pLP->palPalEntry[i].peFlags = 0;
			}
			
			pPal->CreatePalette( pLP );
			
			delete[] pLP;
		}
	}
	
	CClientDC dc(NULL);
	CPalette* pOldPalette = NULL;
	if( pPal )
	{
		pOldPalette = dc.SelectPalette( pPal, FALSE );
		dc.RealizePalette();
	}
	
	HBITMAP hBmp = CreateDIBitmap( dc.m_hDC,		// handle to device context 
		&bmiHeader,	// pointer to bitmap size and format data 
		CBM_INIT,	// initialization flag 
		lpDIBBits,	// pointer to initialization data 
		&bmInfo,	// pointer to bitmap color-format data 
		DIB_RGB_COLORS);		// color-data usage 
	
	bitmap.DeleteObject();
	bitmap.Attach( hBmp );
	
	if( pOldPalette )
		dc.SelectPalette( pOldPalette, FALSE );
	
	::GlobalFree(hDIB);
	return TRUE;
}

BOOL CAFTEMSBitmap::LoadBMPImage(  CArchive& ar, CBitmap& bitmap, CPalette *pPal )
{
	if (ar.IsStoring())
		return false;
	
	BITMAPFILEHEADER bmfHeader;
	
	// Read file header
	if (ar.Read((LPSTR)&bmfHeader, sizeof(bmfHeader)) != sizeof(bmfHeader))
		return FALSE;
	
	// File type should be 'BM'
	if (bmfHeader.bfType != ((WORD) ('M' << 8) | 'B'))
		return FALSE;
	
	// Get length of the remainder of the file and allocate memory
	DWORD nPackedDIBLen = bmfHeader.bfSize - sizeof(bmfHeader);
	HGLOBAL hDIB = ::GlobalAlloc(GMEM_FIXED, nPackedDIBLen);
	if (hDIB == 0)
		return FALSE;
	
	// Read the remainder of the bitmap file.
	if (ar.Read((LPSTR)hDIB, nPackedDIBLen) != nPackedDIBLen )
	{
		::GlobalFree(hDIB);
		return FALSE;
	}
	
	
	BITMAPINFOHEADER &bmiHeader = *(LPBITMAPINFOHEADER)hDIB ;
	BITMAPINFO &bmInfo = *(LPBITMAPINFO)hDIB ;
	
	// If bmiHeader.biClrUsed is zero we have to infer the number
	// of colors from the number of bits used to specify it.
	int nColors = bmiHeader.biClrUsed ? bmiHeader.biClrUsed : 
	1 << bmiHeader.biBitCount;
	
	LPVOID lpDIBBits;
	if( bmInfo.bmiHeader.biBitCount > 8 )
		lpDIBBits = (LPVOID)((LPDWORD)(bmInfo.bmiColors + bmInfo.bmiHeader.biClrUsed) + 
		((bmInfo.bmiHeader.biCompression == BI_BITFIELDS) ? 3 : 0));
	else
		lpDIBBits = (LPVOID)(bmInfo.bmiColors + nColors);
	
	// Create the logical palette
	if( pPal != NULL )
	{
		// Create the palette
		if( nColors <= 256 )
		{
			UINT nSize = sizeof(LOGPALETTE) + (sizeof(PALETTEENTRY) * nColors);
			LOGPALETTE *pLP = (LOGPALETTE *) new BYTE[nSize];
			
			pLP->palVersion = 0x300;
			pLP->palNumEntries = nColors;
			
			for( int i=0; i < nColors; i++)
			{
				pLP->palPalEntry[i].peRed = bmInfo.bmiColors[i].rgbRed;
				pLP->palPalEntry[i].peGreen = bmInfo.bmiColors[i].rgbGreen;
				pLP->palPalEntry[i].peBlue = bmInfo.bmiColors[i].rgbBlue;
				pLP->palPalEntry[i].peFlags = 0;
			}
			
			pPal->CreatePalette( pLP );
			
			delete[] pLP;
		}
	}
	
	CClientDC dc(NULL);
	CPalette* pOldPalette = NULL;
	if( pPal )
	{
		pOldPalette = dc.SelectPalette( pPal, FALSE );
		dc.RealizePalette();
	}
	
	HBITMAP hBmp = CreateDIBitmap( dc.m_hDC,		// handle to device context 
		&bmiHeader,	// pointer to bitmap size and format data 
		CBM_INIT,	// initialization flag 
		lpDIBBits,	// pointer to initialization data 
		&bmInfo,	// pointer to bitmap color-format data 
		DIB_RGB_COLORS);		// color-data usage 
	
	if (!hBmp)
	{
		// TRACE("Error code: %d", GetLastError()); 
		return false;
	}

	bitmap.DeleteObject();
	if (!bitmap.Attach( hBmp ))
	{
		return false;
	}
	
	if( pOldPalette )
		dc.SelectPalette( pOldPalette, FALSE );
	
	::GlobalFree(hDIB);
	return TRUE;
}


//The DrawBitmap() function given below is a very simple function that will 
//draw the given bitmap to the top left corner of the device context. It will 
//also use a logical palette to draw the image if you supply one. You can easily 
//extend this function to specify where to draw the bitmap or even whether to 
//tile the bitmap or extend it to cover the device context. 
void CAFTEMSBitmap::DrawBitmap(CDC* pDC, CBitmap& bitmap, CPalette *pPal )
{
	// Create a compatible memory DC
	CDC memDC;
	memDC.CreateCompatibleDC( pDC );
	memDC.SelectObject( &bitmap );
	
	// Select and realize the palette
	if( pPal != NULL && pDC->GetDeviceCaps(RASTERCAPS) & RC_PALETTE )
	{
		pDC->SelectPalette( pPal, FALSE );
		pDC->RealizePalette();
	}
	
	BITMAP bm;
	bitmap.GetBitmap( &bm );
	
	pDC->BitBlt(0, 0, bm.bmWidth, bm.bmHeight, &memDC, 0, 0,SRCCOPY);
	memDC.DeleteDC();
}

//Writing a bitmap to a BMP file is fairly simple if we have a handle 
//to device-independent bitmap. We simply write BITMAPINFOHEADER 
//information followed by the contents of the bitmap. The three fields 
//that we have to set in the BITMAPINFOHEADER are the bfType which should
//always be "BM", the bfSize which is the size of the bitmap including 
//the infoheader and the bfOffBits which is the offset to the bitmap bits 
//from the start of the file. 
//If you have a device-dependent bitmap to begin with, you have to 
//first create a DIB from it. Creating a DIB from a DDB has already been 
//covered in another section. 

// WriteDIB		- Writes a DIB to file
// Returns		- TRUE on success
// szFile		- Name of file to write to
// hDIB			- Handle of the DIB
BOOL CAFTEMSBitmap::WriteDIB( LPCTSTR szFile, HANDLE hDIB)
{
	BITMAPFILEHEADER	hdr;
	LPBITMAPINFOHEADER	lpbi;
	
	if (!hDIB)
		return FALSE;
	
	CFile file;
	if( !file.Open( szFile, CFile::modeWrite|CFile::modeCreate) )
		return FALSE;
	
	lpbi = (LPBITMAPINFOHEADER)hDIB;
	
	int nColors = 1 << lpbi->biBitCount;
	
	// Fill in the fields of the file header 
	hdr.bfType		= ((WORD) ('M' << 8) | 'B');	// is always "BM"
	hdr.bfSize		= GlobalSize (hDIB) + sizeof( hdr );
	hdr.bfReserved1 	= 0;
	hdr.bfReserved2 	= 0;
	hdr.bfOffBits		= (DWORD) (sizeof( hdr ) + lpbi->biSize +
		nColors * sizeof(RGBQUAD));
	
	// Write the file header 
	file.Write( &hdr, sizeof(hdr) );
	
	// Write the DIB header and the bits 
	file.Write( lpbi, GlobalSize(hDIB) );
	
	return TRUE;
}


BOOL CAFTEMSBitmap::WriteDIB( CArchive& ar, HANDLE hDIB)
{
	BITMAPFILEHEADER	hdr;
	LPBITMAPINFOHEADER	lpbi;
	
	if (!ar.IsStoring())
	{
		return false;
	}
	
	if (!hDIB)
		return FALSE;
	
	lpbi = (LPBITMAPINFOHEADER)hDIB;
	
	int nColors = 1 << lpbi->biBitCount;
	
	// Fill in the fields of the file header 
	hdr.bfType		= ((WORD) ('M' << 8) | 'B');	// is always "BM"
	hdr.bfSize		= GlobalSize (hDIB) + sizeof( hdr );
	hdr.bfReserved1 	= 0;
	hdr.bfReserved2 	= 0;
	hdr.bfOffBits		= (DWORD) (sizeof( hdr ) + lpbi->biSize +
		nColors * sizeof(RGBQUAD));
	
	// Write the file header 
	ar.Write( &hdr, sizeof(hdr) );
	
	// Write the DIB header and the bits
	ar.Write( lpbi, GlobalSize(hDIB) );
	
	return TRUE;
}

BOOL CAFTEMSBitmap::WriteWindowToDIB( LPCTSTR szFile, CWnd *pWnd )
{
#if 0
	CBitmap 	bitmap;
	CWindowDC	dc(pWnd);
	CDC 		memDC;
	CRect		rect;
	
	memDC.CreateCompatibleDC(&dc); 
	
	pWnd->GetWindowRect(rect);
	
	bitmap.CreateCompatibleBitmap(&dc, rect.Width(),rect.Height() );
	
	CBitmap* pOldBitmap = memDC.SelectObject(&bitmap);
	memDC.BitBlt(0, 0, rect.Width(),rect.Height(), &dc, 0, 0, SRCCOPY); 
	memDC.SelectObject(pOldBitmap);
	
	BOOL result = SaveBitmapFile(memDC.GetSafeHdc(), (HBITMAP)bitmap.GetSafeHandle(), szFile) ;
	memDC.DeleteDC();
	return result;
#endif
	CBitmap 	bitmap;
	CDC*	dc  = pWnd->GetDC();
	CDC 		memDC;
	CRect		rect;
	
	memDC.CreateCompatibleDC(dc); 
	
	pWnd->GetClientRect(rect);
	
	bitmap.CreateCompatibleBitmap(dc, rect.Width(),rect.Height() );
	
	CBitmap* pOldBitmap = memDC.SelectObject(&bitmap);
	memDC.BitBlt(0, 0, rect.Width(),rect.Height(), dc, 0, 0, SRCCOPY); 
	memDC.SelectObject(pOldBitmap);
	
	BOOL result = SaveBitmapFile(memDC.GetSafeHdc(), (HBITMAP)bitmap.GetSafeHandle(), szFile) ;
	memDC.DeleteDC();
	pWnd->ReleaseDC(dc);
	return result;
}

BOOL CAFTEMSBitmap::SaveWndToBmp( CBitmap& bitmap, CWnd *pWnd )
{
	CDC*	dc  = pWnd->GetDC();
	CDC 		memDC;
	CRect		rect;
	
	memDC.CreateCompatibleDC(dc); 
	
	pWnd->GetClientRect(rect);
	bitmap.DeleteObject();	
	bitmap.CreateCompatibleBitmap(dc, rect.Width(),rect.Height() );
	
	CBitmap* pOldBitmap = memDC.SelectObject(&bitmap);
	memDC.BitBlt(0, 0, rect.Width(),rect.Height(), dc, 0, 0, SRCCOPY); 
	memDC.SelectObject(pOldBitmap);
	memDC.DeleteDC();
	pWnd->ReleaseDC(dc);
	return true;
}

/******************************Module*Header*******************************\ 
* 
* 
* Created: 06-Jan-1992 10:59:36 
* 
* Copyright 1993 - 1998 Microsoft Corporation 
* 
* Contains the main routine, SaveBitmapFile, for saving a DDB into file 
* in DIB format. 
* 
* Dependencies: 
* 
*   (#defines) 
*   (#includes) 
*       #include <windows.h> 
*       #include "jtypes.h" 
* 
\**************************************************************************/  
/******************************Public*Routine******************************\ 
* SaveBitmapFile 
* 
* 
* Effects: Save pInfo->hBmpSaved into disk specified by pszFileName 
* 
* Warnings: assumes hBmpSaved is not selected into window's DC other than 
*           pInfo->hwnd's DC 
* 
\**************************************************************************/ 

BOOL CAFTEMSBitmap::SaveBitmapFile(HDC hDC, HBITMAP hBmp, PCSTR pszFileName) 
{ 
    int         hFile; 
    OFSTRUCT    ofReOpenBuff; 
    HBITMAP     hTmpBmp, hBmpOld; 
    BOOL        bSuccess; 
    BITMAPFILEHEADER    bfh; 
    PBITMAPINFO pbmi; 
    PBYTE       pBits; 
    BITMAPINFO  bmi; 
    PBYTE pjTmp, pjTmpBmi; 
    ULONG sizBMI; 
	
	
    bSuccess = TRUE; 
    // 
    // Let the graphics engine to retrieve the dimension of the bitmap for us 
    // GetDIBits uses the size to determine if it's BITMAPCOREINFO or BITMAPINFO 
    // if BitCount != 0, color table will be retrieved 
    // 
    bmi.bmiHeader.biSize = 0x28;              // GDI need this to work 
    bmi.bmiHeader.biBitCount = 0;             // don't get the color table 
    if ((GetDIBits(hDC, hBmp, 0, 0, (LPSTR)NULL, &bmi, DIB_RGB_COLORS)) == 0) 
	{ 
		return FALSE; 
    } 
	
    // 
    // Now that we know the size of the image, alloc enough memory to retrieve 
    // the actual bits 
    // 
    if ((pBits = (PBYTE)GlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT, 
		bmi.bmiHeader.biSizeImage)) == NULL) 
	{ 
        return FALSE; 
    } 
	
    // 
    // Note: 24 bits per pixel has no color table.  So, we don't have to 
    // allocate memory for retrieving that.  Otherwise, we do. 
    // 
    pbmi = &bmi; // assume no color table 
	
    switch (bmi.bmiHeader.biBitCount) 
	{ 
	case 24:                                      // has color table 
		sizBMI = sizeof(BITMAPINFOHEADER); 
		break; 
	case 16: 
	case 32: 
		sizBMI = sizeof(BITMAPINFOHEADER)+sizeof(DWORD)*3; 
		break; 
	default: 
		sizBMI = sizeof(BITMAPINFOHEADER)+sizeof(RGBQUAD)*(1<<bmi.bmiHeader.biBitCount); 
		break; 	
    } 
	
    // 
    // Allocate memory for color table if it is not 24bpp... 
    // 
    if (sizBMI != sizeof(BITMAPINFOHEADER)) 
	{ 
        ULONG       sizTmp; 
        // I need more memory for the color table 
        if ((pbmi = (PBITMAPINFO)GlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT, sizBMI )) == NULL) 
		{ 
            bSuccess = FALSE; 
            goto ErrExit1; 
        } 
        // 
        // Now that we've a bigger chunk of memory, let's copy the Bitmap 
        // info header data over 
        // 
        pjTmp = (PBYTE)pbmi; 
        pjTmpBmi = (PBYTE)&bmi; 
        sizTmp = sizeof(BITMAPINFOHEADER); 
		
        while(sizTmp--) 
        { 
            *(((PBYTE)pjTmp)++) = *((pjTmpBmi)++); 
        } 
    } 
	
    // 
    // Let's open the file and get ready for writing 
    // 
    if ((hFile = OpenFile(pszFileName, (LPOFSTRUCT)&ofReOpenBuff, OF_CREATE | OF_WRITE)) == -1) 
	{ 
        AfxMessageBox("File open failed"); 
        goto ErrExit2; 
    } 
	
    // 
    // But first, fill in the info for the BitmapFileHeader 
    // 
    bfh.bfType = 0x4D42;                            // 'BM' 
    bfh.bfSize = sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+sizBMI+ 
        pbmi->bmiHeader.biSizeImage; 
    bfh.bfReserved1 = 0;
	bfh.bfReserved2 = 0; 
    bfh.bfOffBits = sizeof(BITMAPFILEHEADER)+sizBMI; 
	
    // 
    // Write out the file header now 
    // 
    if (_lwrite(hFile, (LPSTR)&bfh, sizeof(BITMAPFILEHEADER)) == -1) 
	{ 
        bSuccess = FALSE; 
        goto ErrExit3; 
    } 
	
    // 
    // Bitmap can't be selected into a DC when calling GetDIBits 
    // Assume that the hDC is the DC where the bitmap would have been selected 
    // if indeed it has been selected 
    // 
    if (hTmpBmp = CreateCompatibleBitmap(hDC, pbmi->bmiHeader.biWidth, pbmi->bmiHeader.biHeight)) 
	{ 
        hBmpOld = (HBITMAP)SelectObject(hDC, hTmpBmp); 
        if ((GetDIBits(hDC, hBmp, 0, pbmi->bmiHeader.biHeight, (LPSTR)pBits, pbmi, DIB_RGB_COLORS))==0)
		{ 
            bSuccess = FALSE; 
            goto ErrExit4; 
        } 
    } 
	else 
	{ 
        AfxMessageBox("The bitmap file is not created"); 
        bSuccess = FALSE; 
        goto ErrExit3; 
    } 
	
    // 
    // Now write out the BitmapInfoHeader and color table, if any 
    // 
    if (_lwrite(hFile, (LPSTR)pbmi, sizBMI) == -1) 
	{ 
        bSuccess = FALSE; 
        goto ErrExit4; 
    } 
	
    // 
    // write the bits also 
    // 
    if (_lwrite(hFile, (LPSTR)pBits, pbmi->bmiHeader.biSizeImage) == -1) 
	{ 
        bSuccess = FALSE; 
        goto ErrExit4; 
    } 
	
	
ErrExit4: 
    SelectObject(hDC, hBmpOld); 
    DeleteObject(hTmpBmp); 
ErrExit3: 
    _lclose(hFile); 
ErrExit2: 
    GlobalFree(pbmi); 
ErrExit1: 
    GlobalFree(pBits); 
    return bSuccess; 
} 





