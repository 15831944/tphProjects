// VideoParams.h: interface for the CVideoParams class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VIDEOPARAMS_H__D9620BA6_451F_4E61_8E59_B97FCEF13EDD__INCLUDED_)
#define AFX_VIDEOPARAMS_H__D9620BA6_451F_4E61_8E59_B97FCEF13EDD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "common\DataSet.h"

class CVideoParams : public DataSet 
{
public:
	CVideoParams();
	virtual ~CVideoParams();

	virtual void clear();

	virtual void readData (ArctermFile& p_file);
	virtual void writeData (ArctermFile& p_file) const;
	virtual void readObsoleteData(ArctermFile& p_file);

	virtual const char *getTitle (void) const { return "Video Params Data"; }
    virtual const char *getHeaders (void) const{  return ".,..,..,,,"; }

	int GetFPS() const { return m_nFPS; }
	int GetQuality() const { return m_nQuality; }
	const CString& GetFileName() const { return m_sFileName; }
	BOOL GetAutoFileName() const { return m_bAutoFileName; }
	int GetFrameWidth() const { return m_nWidth; }
	int GetFrameHeight() const { return m_nHeight; }
	
	void SetFPS(int _fps) { m_nFPS = _fps; }
	void SetQuality(int _quality) { m_nQuality = _quality; }
	void SetFileName(const CString& _filename) { m_sFileName = _filename; }
	void SetAutoFileName(BOOL _b) { m_bAutoFileName = _b; }
	void SetFrameWidth(int _n) { m_nWidth=_n; }
	void SetFrameHeight(int _n) { m_nHeight=_n; }	

protected:
	int m_nFPS;
	int m_nQuality;
	BOOL m_bAutoFileName;
	CString m_sFileName;
	int m_nWidth;
	int m_nHeight;

};

#endif // !defined(AFX_VIDEOPARAMS_H__D9620BA6_451F_4E61_8E59_B97FCEF13EDD__INCLUDED_)
