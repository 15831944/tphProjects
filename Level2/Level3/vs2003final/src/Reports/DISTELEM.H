#ifndef DISTANCE_ELEMENT_H
#define DISTANCE_ELEMENT_H


#include "reports\rep_pax.h"
#include "common\termfile.h"
class CFromToProcCaculation;
class DistanceElement : public ReportPaxEntry
{
protected:

    double totalDistance;

public:

    // distance travelled calculations
    void calculateDistance (CFromToProcCaculation& _values );
    double getDistance (void) const { return totalDistance; };
    
    void writeEntry (ArctermFile& p_file, CFromToProcCaculation& _values ,Terminal* _pTerm) const;
};


#endif
