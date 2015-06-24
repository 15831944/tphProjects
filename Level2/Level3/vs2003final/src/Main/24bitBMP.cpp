// 24bitBMP.cpp: implementation of the C24bitBMP class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "24bitBMP.h" 

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

C24bitBMP::C24bitBMP()
{

}

C24bitBMP::~C24bitBMP()
{

}


BOOL C24bitBMP::SaveBMP(LPCTSTR _sFileName, int _nWidth, int _nHeight, void* _pBits)
{
	//assume 24-bit BMP, _nWidth x _nHeight
	BITMAPFILEHEADER bfh; 
    BITMAPINFO bmi;
	CString sFileName;

	//int nExtraBytes = ((4 - (3 * _nWidth) % 4) % 4);
	//int nImageSize = (_nWidth*3+((4 - (3 * _nWidth) % 4) % 4)) * _nHeight;

	bmi.bmiHeader.biSize = 0x28;
	bmi.bmiHeader.biBitCount = 24;
	bmi.bmiHeader.biWidth = _nWidth;
	bmi.bmiHeader.biHeight = _nHeight;
	bmi.bmiHeader.biCompression = 0;
	bmi.bmiHeader.biClrUsed = 0;
	bmi.bmiHeader.biClrImportant = 0;
	bmi.bmiHeader.biXPelsPerMeter = 0;
	bmi.bmiHeader.biYPelsPerMeter = 0;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biSizeImage = (_nWidth*3+((4 - (3 * _nWidth) % 4) % 4)) * _nHeight;

	bfh.bfType = 0x4D42;// 'BM' 
    bfh.bfSize = sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+bmi.bmiHeader.biSizeImage; 
    bfh.bfReserved1 = 0;
	bfh.bfReserved2 = 0; 
    bfh.bfOffBits = sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);

	CFile outFile;
	if(!outFile.Open(_sFileName,CFile::modeCreate | CFile::modeWrite | CFile::shareExclusive)){
		return FALSE;
	}

	outFile.Write(&bfh, sizeof(BITMAPFILEHEADER));
	outFile.Write(&bmi, sizeof(BITMAPINFOHEADER));
	outFile.Write(_pBits, bmi.bmiHeader.biSizeImage);
	outFile.Close();

	return TRUE;
}

BOOL C24bitBMP::LoadBMP(LPCTSTR _sFileName, int _nWidth, int _nHeight, void* _pBits)
{
	BITMAPFILEHEADER bfh; 
    BITMAPINFO bmi;
	CFile inFile;
	if(!inFile.Open(_sFileName, CFile::modeRead | CFile::shareDenyWrite)) {
		return FALSE;
	}
	inFile.Read(&bfh, sizeof(BITMAPFILEHEADER));
	inFile.Read(&bmi, sizeof(BITMAPINFOHEADER));
	inFile.Read(_pBits, bmi.bmiHeader.biSizeImage);
	inFile.Close();
	return TRUE;
}