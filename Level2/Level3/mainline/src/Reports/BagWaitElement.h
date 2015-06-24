// BagWaitElement.h: interface for the CBagWaitElement class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BAGWAITELEMENT_H__EFAE5410_FD5D_40E3_8BEB_B27B5C062FF5__INCLUDED_)
#define AFX_BAGWAITELEMENT_H__EFAE5410_FD5D_40E3_8BEB_B27B5C062FF5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "REP_PAX.H"
class Terminal ;
class ProcessorIDList;
class CBagWaitElement : public ReportPaxEntry  
{
public:
	CBagWaitElement();
	virtual ~CBagWaitElement();

    bool calculateBagWaitTime (ElapsedTime p_start, ElapsedTime p_end, Terminal* _pTerm , ProcessorIDList* _pIDList);
    ElapsedTime getTotalTime (void) const { return totalTime; }

    void writeEntry (ArctermFile& p_file) const;

protected:
    ElapsedTime totalTime;
    int procCount;

};

#endif // !defined(AFX_BAGWAITELEMENT_H__EFAE5410_FD5D_40E3_8BEB_B27B5C062FF5__INCLUDED_)
