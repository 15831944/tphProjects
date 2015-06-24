// NodeProc.h: interface for the NodeProc class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NODEPROC_H__842E90F6_6A2D_48FF_94CF_520B22DFCC8D__INCLUDED_)
#define AFX_NODEPROC_H__842E90F6_6A2D_48FF_94CF_520B22DFCC8D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AirfieldProcessor.h"

class NodeProc : public AirfieldProcessor  
{
public:
	NodeProc();
	virtual ~NodeProc();
	//processor type
	virtual int getProcessorType (void) const { return NodeProcessor; }
	//processor name
	virtual const char *getProcessorName (void) const { return "NODE"; }
	//processor i/o
	virtual int readSpecialField(ArctermFile& procFile);
	virtual int writeSpecialField(ArctermFile& procFile) const;

	//number
	void SetNumber(int _nNumber) { m_nNumber = _nNumber; }
	int GetNumber() const { return m_nNumber; }
private:
	int m_nNumber;
};

#endif // !defined(AFX_NODEPROC_H__842E90F6_6A2D_48FF_94CF_520B22DFCC8D__INCLUDED_)
