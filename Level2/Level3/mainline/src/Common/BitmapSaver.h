#pragma once


// this class is designed to help saving HBITMAP

// 1. analyze an HBITMAP, retrieve data
// 2. save file
// 3. construct an HBITMAP from data buffer

class BitmapSaver
{
public:
	BitmapSaver();
	~BitmapSaver();


	BOOL Init(HBITMAP hBitmap);

	LPBYTE GetBitmapData(DWORD& nBitsSize);

	BOOL SaveFile(const CString& strFileName);

	static HBITMAP FromFile(const CString fileName);
	static HBITMAP FromBuffer(LPVOID lpData, DWORD dwDataSize);
	static HBITMAP ScaleBitmap(HBITMAP hBmp, WORD wNewWidth, WORD wNewHeight);
private:
	HBITMAP m_hBitmap;
	DWORD m_dwDataSize;
	LPBYTE m_pData; // GlobalAlloc, GlobalFree
};
