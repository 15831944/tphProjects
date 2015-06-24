// FlightArrDepEvent.cpp: implementation of the FlightArrDepEvent class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FlightArrDepEvent.h"
#include "engine\process.h"
#include "engine\proclist.h"
#include "common\states.h"		// for out debug information
#include "common\CodeTimeTest.h"
#include "../Common/ARCTracker.h"
#include "Engine/ARCportEngine.h"
#include "terminal.h"
// to stand gate, write a FlightArrivalAirport/FlightDepartureAirport Log
int FlightArrDepEvent::process (CARCportEngine* pEngine)
{
	PLACE_METHOD_TRACK_STRING();
	//assert( _pInTerm );
	
	Processor* p_standGate = pEngine->getTerminal()->procList->getProcessor( m_iStandGateIndex );
	assert( p_standGate && p_standGate->getProcessorType() == GateProc );

	p_standGate->writeLogEvent(NULL, time, m_iFlightState );

	//////////////////////////////////////////////////////////////////////////
	// debug	
	#ifdef _DEBUG
	char csSTATELABEL[][64] = 
	{
		"FlightArrivalAtAirport",
		"FlightDepartureAirport"
	};
	int iStateIndex = m_iFlightState == FlightArrivalAtAirport ? 0 : 1;

	CString strProcID = p_standGate->getID()->GetIDString();
	char szProcID[256];
	strcpy( szProcID,strProcID );

	char szTime[64];
	time.printTime(szTime);
	
	ofsstream debugFile("FlightEvent.log",stdios::app);
	debugFile<<"\r\nOne Flight "
			 <<csSTATELABEL[iStateIndex]
			 <<" at time: "
			 <<time<<"	("<<szTime<<")	"
			 <<" at gate: "
			 << szProcID
			 <<"/r/n";
	debugFile.close();
	#endif
	//////////////////////////////////////////////////////////////////////////

	return TRUE;
}