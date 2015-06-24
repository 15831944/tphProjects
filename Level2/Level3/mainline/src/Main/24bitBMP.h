// 24bitBMP.h: interface for the C24bitBMP class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_24BITBMP_H__4A1F8138_6297_483D_BC70_E56756F9E9ED__INCLUDED_)
#define AFX_24BITBMP_H__4A1F8138_6297_483D_BC70_E56756F9E9ED__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class C24bitBMP  
{
protected:
	C24bitBMP();
	virtual ~C24bitBMP();

public:
	//saves the bits at _pBits as a BMP file with name _sFileName
	static BOOL SaveBMP(LPCTSTR _sFileName, int _nWidth, int _nHeight, void* _pBits);
	//loads the BMP file named _sFileName into _pBits
	static BOOL LoadBMP(LPCTSTR _sFileName, int _nWidth, int _nHeight, void* _pBits);

};

#endif // !defined(AFX_24BITBMP_H__4A1F8138_6297_483D_BC70_E56756F9E9ED__INCLUDED_)
