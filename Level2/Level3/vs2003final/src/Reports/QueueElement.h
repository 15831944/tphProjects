// QueueElement.h: interface for the QueueElement class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_QUEUEELEMENT_H__355D92F2_D017_41E0_ACB7_EAC6D2090F39__INCLUDED_)
#define AFX_QUEUEELEMENT_H__355D92F2_D017_41E0_ACB7_EAC6D2090F39__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "REP_PAX.H"

class QueueElement : public ReportPaxEntry  
{
protected:
	ElapsedTime totalTime;
	int procCount;

public:
	void writeEntry (ArctermFile& p_file);
	void calculateQueueTime (Terminal* pTerminal,ElapsedTime p_start, ElapsedTime p_end);
	QueueElement();
	virtual ~QueueElement();
	ElapsedTime getTotalTime (void) { return totalTime; }
	ElapsedTime getAverageTime (void) { return totalTime / (long)procCount; }
	int getProcCount (void) const { return procCount; }

	void calculateConveyorWaitQueueTime(const ElapsedTime p_start, const ElapsedTime p_end);
protected:
	bool calculateDependentSourceServiceTime(Terminal* pTerminal,int nProcIndex,int nEvent,const PaxEvent& track,ElapsedTime& serviceTime);
	bool isDependSource(Terminal* pTerminal,int nProcIndex);

};

#endif // !defined(AFX_QUEUEELEMENT_H__355D92F2_D017_41E0_ACB7_EAC6D2090F39__INCLUDED_)
