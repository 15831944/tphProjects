// AFTEMSBitmap.h: interface for the CAFTEMSBitmap class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AFTEMSBITMAP_H__63D78F9A_37E1_11D2_8681_00A024A40DE2__INCLUDED_)
#define AFX_AFTEMSBITMAP_H__63D78F9A_37E1_11D2_8681_00A024A40DE2__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CAFTEMSBitmap  
{
private:
	static BOOL SaveBitmapFile(HDC, HBITMAP, PCSTR); 

public:
	// DDBToDIB		- Creates a DIB from a DDB
	// bitmap		- Device dependent bitmap
	// dwCompression	- Type of compression - see BITMAPINFOHEADER
	// pPal			- Logical palette
	static HANDLE DDBToDIB( CBitmap& bitmap, DWORD dwCompression, CPalette* pPal ) ;

	static HBITMAP DIBToDDB( HANDLE hDIB );

	//The DrawDIB() function uses the function SetDIBitsToDevice() for to render the image. 
	//When using this function, you should be aware that the BMP file is arranged such 
	//that the first scan line (first row of pixels) is the bottom most scan line. You 
	//could actually have the top most scan line be the first if the height of the bitmap
	//is specified as a negative value but this is rarely used. So if you want only the 
	//top half of the bitmap drawn then the nStartScan value should be half of nNumScans. 
	
	// DrawDIB	- Draws a DIB onto a device// pDC		- Pointer to a device context
	// hDIB		- Handle of the device-independent bitmap
	// pPal		- Pointer to a palette associated with the DIB
	// xDest	- x-coordinate of the upper-left corner of the destination rect
	// yDest	- y-coordinate of the upper-left corner of the destination rect
	static void DrawDIB( CDC* pDC, HGLOBAL hDIB, CPalette *pPal, int xDest, int yDest );

	//The function given below is just an example of how to use the loaded bitmap to 
	//draw on the screen. It uses the function SetDIBitsToDevice() for this purpose. 
	//When using this function, you should be aware that the BMP file is arranged such 
	//that the first scan line (first row of pixels) is the bottom most scan line. 
	//So if you want only the top half of the bitmap drawn then the nStartScan value
	//should be half of nNumScans. 
	static void DrawDIB( CDC* pDC, HGLOBAL hDIB, CPalette *pPal );

	//The DrawBitmap() function can be used with both a DDB and a DIB section. This 
	//function does take into account that the destination device context might have 
	//a map mode other than MM_TEXT. This might not work always or the effect may not 
	//be what you want. 

	// DrawBitmap	- Draws a bitmap (DDB & DIB section) onto a device
	// pDC		- Pointer to a device context// hBitmap	- Handle of the bitmap
	// hPal		- Handle of a logical palette associated with the bitmap
	// xDest	- x-coordinate of the upper-left corner of the destination rect
	// yDest	- y-coordinate of the upper-left corner of the destination rect
	static void DrawBitmap( CDC *pDC, HBITMAP hBitmap, HPALETTE hPal, int xDest, int yDest );

	//To draw the bitmap we need the information in the last 3 sections of the file. 
	//That is, the BITMAPINFOHEADER onwards. We allocate enough memory to hold this 
	//information and then read in from the bitmap file. Based on this information, 
	//a logical palette is also created. 
	// LoadBMP		- Loads a BMP file and creates a logical palette for it.
	// Returns		- TRUE for success
	// sBMPFile		- Full path of the BMP file
	// phDIB		- Pointer to a HGLOBAL variable to hold the loaded bitmap
	//			  Memory is allocated by this function but should be 
	//			  released by the caller.
	// pPal			- Will hold the logical palette
	static BOOL LoadBMP( LPCTSTR sBMPFile, HGLOBAL *phDIB, CPalette *pPal );

	// LoadBMPImage	- Loads a BMP file and creates a bitmap GDI object
	//		  also creates logical palette for it.
	// Returns	- TRUE for success
	// sBMPFile	- Full path of the BMP file
	// bitmap	- The bitmap object to initialize
	// pPal		- Will hold the logical palette. Can be NULL
	static BOOL LoadBMPImage( LPCTSTR sBMPFile, CBitmap& bitmap, CPalette *pPal );
	static BOOL LoadBMPImage( CArchive& ar, CBitmap& bitmap, CPalette *pPal );

	//The DrawBitmap() function given below is a very simple function that will 
	//draw the given bitmap to the top left corner of the device context. It will 
	//also use a logical palette to draw the image if you supply one. You can easily 
	//extend this function to specify where to draw the bitmap or even whether to 
	//tile the bitmap or extend it to cover the device context. 
	static void DrawBitmap(CDC* pDC, CBitmap& bitmap, CPalette *pPal );

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
	static BOOL WriteDIB( LPCTSTR szFile, HANDLE hDIB);
	static BOOL WriteDIB( CArchive& ar, HANDLE hDIB);

	static BOOL WriteWindowToDIB( LPCTSTR szFile, CWnd *pWnd );

	static BOOL SaveWndToBmp( CBitmap& bmp, CWnd *pWnd );
};

#endif // !defined(AFX_AFTEMSBITMAP_H__63D78F9A_37E1_11D2_8681_00A024A40DE2__INCLUDED_)
