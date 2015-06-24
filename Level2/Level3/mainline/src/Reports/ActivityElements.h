// ActivityElements.h: interface for the ActivityElements class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ACTIVITYELEMENTS_H__6078BC5D_1025_4352_ACB4_155662047334__INCLUDED_)
#define AFX_ACTIVITYELEMENTS_H__6078BC5D_1025_4352_ACB4_155662047334__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "rep_pax.h"

class CActivityElements : public ReportPaxEntry
{
public:
	CActivityElements();
	virtual ~CActivityElements();

    void calculateActivities (void);
    ElapsedTime getFreeMovingTime (void) const { return freeMovingTime; };
    ElapsedTime getQueueWaitTime (void) const { return queueWaitTime; };
    ElapsedTime getBagWaitTime (void) const { return bagWaitTime; };
    ElapsedTime getHoldAreaWaitTime (void) const { return holdAreaWaitTime; };
    ElapsedTime getServiceTime (void) const { return serviceTime; };

    void writeEntry (ArctermFile& p_file) const;

protected:
    ElapsedTime freeMovingTime,
                queueWaitTime,
                bagWaitTime,
                holdAreaWaitTime,
                serviceTime;

};

#endif // !defined(AFX_ACTIVITYELEMENTS_H__6078BC5D_1025_4352_ACB4_155662047334__INCLUDED_)
