#include "stdafx.h"
#include <string.h>
#include "results\fltentry.h"



void FlightLogEntry::echo (ofsstream& p_stream, const CString& _csProjPath) const
{
    p_stream << item.airline << ',' << item.arrID << ',' << item.origin << ',' << item.arrStopover << ','
         << ',' << getArrTime() << ',' << item.arrLoad << ',' <<
		 item.depID << ',' << item.depStopover << ',' << item.destination << ',' << getDepTime() << ','
         << item.depLoad << ',' << item.acType << '\n';
}
                                 ;

void FlightLogEntry::getFlightID (char *p_str, char p_mode) const
{
    strcpy (p_str, item.airline);
    if (p_mode == 'D')
        //itoa (item.depID, p_str + strlen (p_str), 10);
		strcat(p_str, item.depID);
    else if (p_mode == 'A')
        //itoa (item.arrID, p_str + strlen (p_str), 10);
		strcat(p_str, item.arrID);
    else if (isDeparting())
        //itoa (item.depID, p_str + strlen (p_str), 10);
		strcat(p_str,item.depID);
    else
        //itoa (item.arrID, p_str + strlen (p_str), 10);
		strcat(p_str, item.arrID);
}

ElapsedTime FlightLogEntry::getGateTime (void) const
{
    return (item.depID)? getDepTime(): getArrTime();
}

ElapsedTime FlightLogEntry::getArrTime (void) const
{
    ElapsedTime t;
    t.setPrecisely (item.arrTime);
    return t;
}

ElapsedTime FlightLogEntry::getDepTime (void) const
{
    ElapsedTime t;
    t.setPrecisely (item.depTime);
    return t;
}

