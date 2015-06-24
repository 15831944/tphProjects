// ActivityElements.cpp: implementation of the ActivityElements class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Reports.h"
#include "ActivityElements.h"
#include "..\common\states.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CActivityElements::CActivityElements()
{

}

CActivityElements::~CActivityElements()
{

}


void CActivityElements::calculateActivities (void)
{
    freeMovingTime = queueWaitTime = bagWaitTime =
        holdAreaWaitTime = serviceTime = 0L;

    setToFirst();
    PaxEvent track;
    track.init (getNextEvent());
    int trackCount = getCount();
    int processor = track.getProc();
    ElapsedTime curTime = 0L, prevTime = track.getTime();
	ElapsedTime etInterval=0L;

    int state = FreeMoving, trackState;
    for (int i = 1; i < trackCount; i++)
    {
        track.init (getNextEvent());
        trackState = track.getState();
        curTime = track.getTime();
		etInterval=curTime - prevTime;
//	marked by aivin.
//		if(prevTime>ElapsedTime(WholeDay)||prevTime<ElapsedTime(-WholeDay)||curTime>ElapsedTime(WholeDay)||curTime<ElapsedTime(-WholeDay))
//		{
//			CString strMsg;
//			strMsg.Format("Time log is invalid for passenger ID %d,  Process terminated.",getID());
//			char msg[255];
//			strcpy(msg,strMsg.GetBuffer(0));
//			strMsg.ReleaseBuffer();
//			throw new ARCInvalidTimeError(msg);
//		}
        switch (state)
        {
            case FreeMoving:
                if (trackState == WaitInQueue ||
                    trackState == ArriveAtServer ||
                    trackState == WaitForBaggage)
                {
					etInterval=curTime - prevTime;
//					if(etInterval <ElapsedTime( 0L))
//						etInterval+=WholeDay;

                    freeMovingTime += etInterval;
                    state = trackState;
                    prevTime = curTime;
                    processor = track.getProc();
                }
				break;

            case WaitInQueue:
                if (trackState == LeaveQueue ||
                    trackState == JumpQueue)
				{
					etInterval=curTime - prevTime;
//					if(etInterval < ElapsedTime(0L))
//						etInterval+=WholeDay;

                    queueWaitTime += etInterval;
                    state = FreeMoving;
                    prevTime = curTime;
                }
				break;

            case ArriveAtServer:
                if (trackState == WaitForBaggage)
                    state = trackState;
                else
                {
                    if (report->getProcType (processor) == HoldAreaProc)
					{
						etInterval=curTime - prevTime;
//						if(etInterval < ElapsedTime(0L))
//							etInterval+=WholeDay;

                        holdAreaWaitTime += etInterval;
					}
                    else
					{
						etInterval=curTime - prevTime;
//						if(etInterval < ElapsedTime(0L))
//							etInterval+=WholeDay;

                        serviceTime += etInterval;
					}
                    state = FreeMoving;
                    prevTime = curTime;
                    processor = track.getProc();
                }
				break;

            case WaitForBaggage:
				etInterval=curTime - prevTime;
//				if(etInterval < ElapsedTime(0L))
//					etInterval+=WholeDay;
                bagWaitTime += etInterval;

                state = FreeMoving;
                prevTime = curTime;
                break;
		  }
	 }
}

void CActivityElements::writeEntry (ArctermFile& p_file) const
{
    p_file.writeInt (item.id);
    p_file.writeTime (freeMovingTime, TRUE);
    p_file.writeTime (queueWaitTime, TRUE);
    p_file.writeTime (bagWaitTime, TRUE);
    p_file.writeTime (holdAreaWaitTime, TRUE);
    p_file.writeTime (serviceTime, TRUE);

    char str[MAX_PAX_TYPE_LEN];
    printFullType (str);
    p_file.writeField (str);
    p_file.writeLine();
}                         
