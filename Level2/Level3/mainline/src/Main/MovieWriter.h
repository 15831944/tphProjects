// MovieWriter.h: interface for the CMovieWriter class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MOVIEWRITER_H__F75336A8_2FDA_44EB_9A11_6EE04FA5C801__INCLUDED_)
#define AFX_MOVIEWRITER_H__F75336A8_2FDA_44EB_9A11_6EE04FA5C801__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef _NOWMVCORE
#include "WMSDK9\wmsdk.h"
#endif


#define THROW_STDEXCEPTION(s) throw std::exception(s);

struct FrameInfo
{
	INSSBuffer* pINSBuffer;
	BYTE* pByteBuffer;
	DWORD bufferSize;
	int nFrameIdx;
	HRESULT hr; //result
};

class CMovieWriter
{
public:
	CMovieWriter();
	virtual ~CMovieWriter();

	HRESULT InitializeMovie(LPCSTR szFileName, int nWidth, int nHeight, int fps, int quality);
	void EndMovie();

	//write frame in on function
	void WriteFrame(UINT nFrameIdx, const BYTE* pBits);

	//begin frame write ,return the buffer to write
	void BegineFrameWrite(UINT nFrameIdx,FrameInfo& info);
	void EndFrameWrite(const FrameInfo& info);

	int GetWidth()const{ return m_dwWidth; }
	int GetHeight()const{ return m_dwHeight; }

protected:
#ifndef _NOWMVCORE
	HRESULT CreateProfile( IWMProfile ** ppIWMProfile );
	IWMWriter* m_pWMWriter;
#endif

	BOOL m_bIsWriting;

	DWORD m_dwHeight;
	DWORD m_dwWidth;
	DWORD m_dwBPP;
	DWORD m_dwFPS;
	QWORD m_dwNSPerFrame;
	DWORD m_dwQuality;

};

#endif // !defined(AFX_MOVIEWRITER_H__F75336A8_2FDA_44EB_9A11_6EE04FA5C801__INCLUDED_)
