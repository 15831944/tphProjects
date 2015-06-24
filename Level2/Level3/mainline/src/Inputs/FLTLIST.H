#ifndef LIST_OF_FLIGHTS_H
#define LIST_OF_FLIGHTS_H

#include "common\strdict.h"

class ListOfFlights
{
public:
    ListOfFlights () {};
    virtual ~ListOfFlights () {};

    // methods used to fill out FlightSelectionTool lists
    virtual void getFlightList (StringList& p_strDict,
        const FlightConstraint &p_type) = 0;
};

#endif
