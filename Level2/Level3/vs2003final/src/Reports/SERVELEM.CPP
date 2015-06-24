
#include "StdAfx.h"
#include "reports\servelem.h"
#include "results\paxentry.h"
#include "results\fltlog.h"
#include "common\termfile.h"
#include "common\states.h"

void ServiceElement::calculateServiceTime (ElapsedTime p_start,
    ElapsedTime p_end)
{
    totalTime = 0l;
    procCount = 0;

	ElapsedTime etInterval;

    loadEvents();
    if (usesSelectedProc (p_start, p_end))
    {
        PaxEvent track1, track2,track3;
        track2.init (getEvent(0));
        int trackCount = getCount();

        for (int i = 1; i < trackCount; i++)
        {
            track1 = track2;
            track2.init (getEvent(i));
			if (i != trackCount - 1)
			{
				track3.init(getEvent(i+1));
			}

            if (track1.getState() == ArriveAtServer &&
                ///report->getProcType (track1.getProc()) != HoldAreaProc &&
                isValidProc (track1.getProc()))
            {
				if (i != trackCount - 1)
				{
					if (track3.getState() == LeaveServer)
					{
						track2 = track3;
						i++;
					}
				}
		
                etInterval=track2.getTime() - track1.getTime();
//				if(etInterval<ElapsedTime(0L))
//					etInterval+=WholeDay;

				totalTime += etInterval;
                procCount++;
            }
        }
    }
    clearLog();
}

void ServiceElement::writeEntry (ArctermFile& p_file) const
{
    if (!procCount)
        return;

    p_file.writeInt (item.id);
    p_file.writeTime (totalTime, TRUE);
    p_file.writeTime (totalTime / (long)item.groupSize, TRUE);
    p_file.writeInt (procCount);

    char str[MAX_PAX_TYPE_LEN];
    printFullType (str);
    p_file.writeField (str);
    p_file.writeLine();
}

