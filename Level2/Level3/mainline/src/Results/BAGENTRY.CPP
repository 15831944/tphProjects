#include "stdafx.h"
#include "results\bagentry.h"
#include "results\fltlog.h"
#include "results\out_term.h"


void BagLogEntry::echo (ofsstream& p_stream, const CString& _csProjPath) const
{
    p_stream << item.id << ',' << item.paxID << ',';
    char str[16];
	assert( m_pOutTerm );
    m_pOutTerm->flightLog->getFlightID (str, item.flight);
    p_stream << str << ',' << getEntryTime() << ',' << getExitTime()
        << ',' << item.procNumber << '\n';
}

ElapsedTime BagLogEntry::getEntryTime (void) const
{
    ElapsedTime t;
    t.setPrecisely (item.startTime);
    return t;
}

ElapsedTime BagLogEntry::getExitTime (void) const
{
    ElapsedTime t;
    t.setPrecisely (item.endTime);
    return t;
}

