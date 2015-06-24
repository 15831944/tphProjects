// AftemsAVIFile.h: interface for the CAftemsAVIFile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AFTEMSAVIFILE_H__1EA7FD26_F93D_11D3_80A1_00E02C06809E__INCLUDED_)
#define AFX_AFTEMSAVIFILE_H__1EA7FD26_F93D_11D3_80A1_00E02C06809E__INCLUDED_

#include "vfw.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CAftemsAVIFile  
{
public:
	CAftemsAVIFile();

	virtual ~CAftemsAVIFile();

	//stTargetFileName: file to save
	//nTotalFrame: total number of frames
	//nFrameRate: rate/second
	//bmpSize: bitmap file size, it cann't handle bmps of different size at the moment
	bool Init(CString stTargetFileName, double dbFrameRate, CSize bmpSize);

	//note, the function is static! it doesn't store filename
	static CString GetAviFileNameToOpen(CString suggestedFileName);

	//do nothing if size is different, or nTotalFrame reached
	bool AddBmpOfSameSize(CBitmap& aMap);

	//save file
	void Close();
private:
	CString m_stTargetFileName;
	double m_dbFrameRate;
	int m_nFrameCounter;

	AVISTREAMINFO m_strhdr;
	PAVIFILE m_pfile;
	PAVISTREAM m_ps, m_psCompressed, m_psText;
	AVICOMPRESSOPTIONS m_opts;

	HRESULT m_hr;
	bool m_bBusy;
private:

	/* MakeDib(hbitmap)
	**
	** Take the given bitmap and transform it into a DIB with parameters:
	**
	** BitsPerPixel:    8
	** Colors:          palette
	*/
	static LPBITMAPINFOHEADER  MakeDib(HBITMAP hbitmap, UINT bits );
};

#endif // !defined(AFX_AFTEMSAVIFILE_H__1EA7FD26_F93D_11D3_80A1_00E02C06809E__INCLUDED_)
