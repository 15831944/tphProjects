#include "StdAfx.h"

#include "reports\distelem.h"
#include "results\paxentry.h"
#include "results\fltlog.h"
#include "common\termfile.h"
#include "common\point.h"
#include "common\states.h"
#include "FromToProcCaculation.h"
#include "engine\proclist.h"
#include "engine\terminal.h"

void DistanceElement::calculateDistance (CFromToProcCaculation& _values )
{
    totalDistance = 0.0;
	
    loadEvents();
    PaxEvent track1, track2;
    track1.init (getEvent(0));
    track2.init (getEvent(1));
    Point point1, point2;
    int trackCount = getCount();
	
	_values.AddValue( track1.getProc(), 0.0, track1.getState());	
	
    for (int i = 2; i < trackCount; i++)
    {
        track1 = track2;
        track2.init (getEvent(i));
		
        point1 = track1.getPoint();
        point2 = track2.getPoint();
		
		if( track1.getState() != BoardVehicle)
		{
			if( point1 != point2 || point1.getZ() != point2.getZ() )
			{
				double dDistance = point1.distance (point2);
				totalDistance += dDistance;
				_values.AddValue( track1.getProc(), dDistance, track1.getState() );
			}
			else
			{
				_values.AddValue( track1.getProc(), 0.0,  track1.getState());
			}

		}    
    }
    clearLog();
}

void DistanceElement::writeEntry (ArctermFile& p_file, CFromToProcCaculation& _values ,Terminal* _pTerm) const
{
    
    char str[MAX_PAX_TYPE_LEN];
    printFullType (str);
    
	std::vector<CFromToProcCaculation::FROM_TO_PROC>& values = _values.GetValues();
	for( unsigned i=0; i<values.size(); ++i )
	{
		if( values[i].m_enFlag == CFromToProcCaculation::END_COMPUTE || values[i].m_iToProc == END_PROCESSOR_INDEX )
		{
			p_file.writeInt (item.id);
			p_file.writeFloat ((float)(values[i].m_dValue / 100.0), 2);
			p_file.writeField (str);

			Processor* pProc = _pTerm->procList->getProcessor( values[i].m_iFromProc );
			p_file.writeField( pProc->getID()->GetIDString() );
			
			pProc = _pTerm->procList->getProcessor( values[i].m_iToProc );
			p_file.writeField( pProc->getID()->GetIDString() );			

			p_file.writeLine();
		}
	}    
}

