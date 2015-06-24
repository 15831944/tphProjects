#ifndef SERVICE_ELEMENT_H
#define SERVICE_ELEMENT_H

//#include "common\termfile.h"
//#include "common\elaptime.h"
#include "reports\rep_pax.h"

class ServiceElement : public ReportPaxEntry
{
protected:
    ElapsedTime totalTime;
    int procCount;

public:
    // distance travelled calculations
    void calculateServiceTime (ElapsedTime p_start, ElapsedTime p_end);
    ElapsedTime getTotalTime (void) const { return totalTime; }
    ElapsedTime getAverageTime (void) const { return totalTime / (long)procCount; }
    int getProcCount (void) const { return procCount; }

    void writeEntry (ArctermFile& p_file) const;
};

#endif
