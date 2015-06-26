// QueueElement.cpp: implementation of the QueueElement class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Reports.h"
#include "QueueElement.h"
#include "results\paxentry.h"
#include "results\fltlog.h"
#include "common\termfile.h"
#include "common\states.h"
#include "common\point.h"
#include "Engine\process.h"
#include "Engine\terminal.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

QueueElement::QueueElement()
{

}

QueueElement::~QueueElement()
{

}

bool QueueElement::isDependSource(Terminal* pTerminal,int nProcIndex)
{
	ASSERT(pTerminal);
	if (pTerminal == NULL)
		return false;

	Processor* pProc = pTerminal->procList->getProcessor(nProcIndex);
	if (pProc == NULL || pProc->getProcessorType() != DepSourceProc)
		return false;

	return true;
}

bool QueueElement::calculateDependentSourceServiceTime(Terminal* pTerminal,int nProcIndex,int nEvent,const PaxEvent& track,ElapsedTime& serviceTime)
{
	if (!isDependSource(pTerminal,nProcIndex))//check processor type is dependent source
		return false;

	int trackCount = getCount();

	//find arrival at server event
	PaxEvent arrivalEvent;
	arrivalEvent = track;
	while(arrivalEvent.getState() != ArriveAtServer && nEvent < trackCount)
	{
		arrivalEvent.init(getEvent(nEvent++));
	}

	//if did not find
	if (arrivalEvent.getState() != ArriveAtServer || arrivalEvent.getProc() != nProcIndex)
		return false;


	//find leave server event
	PaxEvent LeaveEvent;
	LeaveEvent = arrivalEvent;
	while(LeaveEvent.getState() != LeaveServer && nEvent < trackCount)
	{
		LeaveEvent.init(getEvent(nEvent++));
	}

	//if did not find
	if (LeaveEvent.getState() != LeaveServer || LeaveEvent.getProc() != nProcIndex)
		return false;

	//calculate service time
	serviceTime =LeaveEvent.getTime() - arrivalEvent.getTime();

	return true;
}

void QueueElement::calculateQueueTime(Terminal* pTerminal,ElapsedTime p_start, ElapsedTime p_end)
{
	totalTime = 0l;
	procCount = 0;

	loadEvents();
	if (usesSelectedProc (p_start, p_end))
	{
		PaxEvent track1, track2;
		track2.init (getEvent(0));
		Point point1, point2;
		int trackCount = getCount();

		for (int i = 1; i < trackCount; i++)
		{
			track1 = track2;
			track2.init (getEvent(i));

			if (track1.getState() == WaitInQueue &&
				isValidProc (track1.getProc()))
			{
				ElapsedTime serviceTime;
				calculateDependentSourceServiceTime(pTerminal,track1.getProc(),i - 1,track1,serviceTime);

				while (track2.getState() != LeaveQueue && (i < trackCount-1) &&
					track2.getState() != JumpQueue)
				{
					i++;
					track2.init (getEvent(i));
				}

				ElapsedTime time1 = track1.getTime();
				ElapsedTime time2 = track2.getTime();
				ElapsedTime mytotalTime = time2 - time1;
				mytotalTime += serviceTime;
				totalTime += mytotalTime;
				procCount++;
			}
		}
	}
	clearLog();

	if(totalTime<ElapsedTime(0L))
	{
		CString strMsg;
		strMsg.Format("Time log is invalid for passenger ID %d,  Process terminated.",getID());
		char msg[255];
		strcpy(msg,strMsg.GetBuffer(0));
		strMsg.ReleaseBuffer();
		throw new ARCInvalidTimeError(msg);
	}


}

void QueueElement::writeEntry(ArctermFile &p_file)
{
	if (!procCount)
		return;

	p_file.writeInt (item.id);
	p_file.writeTime (totalTime, TRUE);
	p_file.writeTime (totalTime / (long)procCount, TRUE);
	p_file.writeInt (procCount);

	char str[MAX_PAX_TYPE_LEN];
	printFullType (str);
	p_file.writeField (str);
	p_file.writeLine();
}
void QueueElement::calculateConveyorWaitQueueTime(const ElapsedTime p_start, const ElapsedTime p_end)
{
	totalTime = 0l;
	procCount = 0;

	loadEvents();
	PaxEvent track1, track2;
	track2.init (getEvent(0));
	Point point1, point2;
	int trackCount = getCount();
	int iPreProcIdx=-1;
	for (int i = 1; i < trackCount; i++)
	{
		track1 = track2;
		track2.init (getEvent(i));

		if (track1.getTime() > p_end)
			break;

		if (track1.getTime() < p_start)
			continue;

		if( track1.getReason() <0 )
			continue;
		if (track1.getState() == ProcessorStateStop && isValidProc (track1.getReason() ) )
		{
			while (track2.getState() != ProcessorStateRun && i < trackCount-1 )
			{
				i++;
				track2.init (getEvent(i));
			}	
			if( iPreProcIdx != track1.getReason() )
			{
				procCount++;
				iPreProcIdx = track1.getReason();
			}

			totalTime += track2.getTime() - track1.getTime();            
		}
	}
	clearLog();
}