// SideMoveWalkPaxDist.h: interface for the CSideMoveWalkPaxDist class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SIDEMOVEWALKPAXDIST_H__315AC629_5067_4AB6_AD82_5BBA43E3F670__INCLUDED_)
#define AFX_SIDEMOVEWALKPAXDIST_H__315AC629_5067_4AB6_AD82_5BBA43E3F670__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "PROCHRCH.H"

class CSideMoveWalkPaxDist : public ProcessorHierarchy  
{
public:
	CSideMoveWalkPaxDist();
	virtual ~CSideMoveWalkPaxDist();

	void deletePaxType (int p_level, int p_index);
	    virtual const char *getTitle (void) const
        { return "Moving side walk  database"; }
    virtual const char *getHeaders (void) const
        { return "Processor,Passenger Type,probalities"; }

	virtual void readData (ArctermFile& p_file);
	virtual void readObsoleteData(ArctermFile& p_file);

};

#endif // !defined(AFX_SIDEMOVEWALKPAXDIST_H__315AC629_5067_4AB6_AD82_5BBA43E3F670__INCLUDED_)
