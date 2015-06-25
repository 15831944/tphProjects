#include "stdafx.h"
#include "common\fsstream.h"
#include <ctype.h>
#include "inputs\flight.h"
#include "common\term_bin.h"
#include "common\termfile.h"
#include "common\elaptime.h"
#include "common\exeption.h"
#include "Common\FlightManager.h"
#include "..\common\actypesmanager.h"
#include "..\common\airlinemanager.h"
#include "..\common\airportsmanager.h"
#include "engine\FlightArrDepEvent.h"
#include "common\states.h"
#include "engine\terminal.h"
#include "inputs\flt_db.h"
#include "inputs\fltdata.h"
#include "inputs\probab.h"
#include "..\InputAirside\ALTObject.h"
#include "..\InputAirside\ALTObjectGroup.h"
#include "../InputAirside/stand.h"
#include "common\ArrFlightOperation.h"
#include "common\DepFlightOperation.h"
#include "../InputAirside/TowOffStandAssignmentDataList.h"
#include "../InputAirside/TowOffStandAssignmentData.h"

#include <Common/ProbabilityDistribution.h>

int Flight::sortBy = DepTimeField;
int Flight::m_nNextUniqueID = 0;


//Modify by adam 2007-04-24
//#define FLIGHT_FIELDS   12
#define FLIGHT_FIELDS   11
//End modify by adam 2007-04-24


char * far flightHeaders[] =
{
	"Airline",
	"Arr ID",
	"Origin",
	"Arr Stopover",
	"Arr Time",
	"Dep ID",
	"Dep Stopover",
	"Destination",	
	"Dep Time",	
	"AC Type",
	"Arr Stand",
	"Dep Stand",
	"Intermidate Stand",
	"Stay Time",
	"Arr Parking Time",
	"Dep Parking Time",
	"Int Parking Time",
	"Arr Gate",
	"Dep Gate",
	"BagDevice",
	"Arr Loadfactor",
	"Dep Loadfactor",
	"Capacity",
	"ATA",
	"ATD",
	"T Tow off",
	"T EX Int Stand",
};
#define ALL_FIELDS sizeof(flightHeaders)/sizeof(char*)


// make sure give unique _nKey
Flight::Flight( Terminal* _pTerm )
: m_pTerm( _pTerm )
,m_bagFirstBirthTimeInReclaim(0l)
,m_bagLastLeaveTimeInReclaim(0l)
{
	m_nUniqueID = m_nNextUniqueID++;

	m_arrloadFactor = -1.0;
	m_deploadFactor = -1.0;
	m_capacity = -1;

	m_arrLFInput = -1.0;
	m_depLFInput = -1.0;
	m_capacityInput = -1;

	m_bagCarosel = 0;

	m_nDepTransit = 0;		
	m_nArrTransit = 0;

	m_nDepTransfer = 0;		
	m_nArrTransfer = 0;	

	m_nArrGateIdx = -1;
	m_nDepGateIdx = -1;

	m_ataTime = -1;
	m_atdTime = -1;

	//m_arrLoad = 0;
	//m_depLoad = 0;
    curPax = 0;
	baggageDevice.SetStrDict(_pTerm->inStrDict);
	m_bNeedCheckRealDepTime = false;
	m_bAirsideDepartured = false;
	
}
Flight::Flight(const Flight& _flight):ARCFlight(_flight)
{
	*this = _flight;
}

Flight::~Flight()
{

}
const Flight& Flight::operator = (const Flight& aFlight)
{
	ARCFlight::operator =(aFlight);
    curPax = aFlight.curPax;
	m_mapLastCallOfEveryStage = aFlight.m_mapLastCallOfEveryStage;
	lastCall = aFlight.lastCall;
	baggageDevice = aFlight.baggageDevice;
	m_pTerm = aFlight.m_pTerm;
	

	m_arrStopover = aFlight.m_arrStopover;
	m_depStopover = aFlight.m_depStopover;


	m_bagFirstBirthTimeInReclaim = aFlight.m_bagFirstBirthTimeInReclaim;
	m_bagLastLeaveTimeInReclaim = aFlight.m_bagLastLeaveTimeInReclaim;

	m_arrloadFactor = aFlight.m_arrloadFactor;
	m_deploadFactor = aFlight.m_deploadFactor;
	m_capacity = aFlight.m_capacity;

	m_arrLFInput = aFlight.m_arrLFInput ;
	m_depLFInput = aFlight.m_depLFInput ;
	m_capacityInput = aFlight.m_capacityInput ;

	m_bagCarosel = aFlight.m_bagCarosel;

	m_nDepTransit = aFlight.m_nDepTransit;		
	m_nArrTransit = aFlight.m_nArrTransit;

	m_nDepTransfer = aFlight.m_nDepTransfer;		
	m_nArrTransfer = aFlight.m_nArrTransfer;

	m_ArrGate = aFlight.m_ArrGate;
	m_DepGate = aFlight.m_DepGate;

	m_nArrGateIdx = aFlight.m_nArrGateIdx;
	m_nDepGateIdx = aFlight.m_nDepGateIdx;
	m_bNeedCheckRealDepTime = aFlight.m_bNeedCheckRealDepTime;

	m_ataTime = aFlight.m_ataTime;//ATA 
	m_atdTime = aFlight.m_atdTime;//ATD

	m_bAirsideDepartured = aFlight.m_bAirsideDepartured;

    return *this;
}

int Flight::operator < (const Flight& p_flight) const
{
    switch (sortBy)
    {
        case AirlineField:
            return _strcmpi (m_Airline, p_flight.m_Airline) < 0;
        case ArrIDField:
            if (!_strcmpi (m_Airline, p_flight.m_Airline))
                return getArrID() < p_flight.getArrID() ;
            else
                return _strcmpi (m_Airline, p_flight.m_Airline) < 0;
        case OrigField:
			return _strcmpi (getOrigin(), p_flight.getOrigin()) < 0;
		case ArrStopoverField:
			return _strcmpi (m_arrStopover, p_flight.m_arrStopover) < 0;
        case ArrTimeField:
            return getArrTime() < p_flight.getArrTime();
        case DepIDField:
            if (!_strcmpi (m_Airline, p_flight.m_Airline))
                return getDepID() < p_flight.getDepID() ;
            else
                return _strcmpi (m_Airline, p_flight.m_Airline) < 0;
        case DepStopoverField:
			return _strcmpi (m_depStopover, p_flight.m_depStopover) < 0;
		case DestField:
			return _strcmpi (getDestination(), p_flight.getDestination()) < 0;
        case DepTimeField:
            return getDepTime() < p_flight.getDepTime();
		case GateTimeField:
			return getGateTime() < p_flight.getGateTime();
        case ACTypeField: default:
            return _strcmpi (m_AcType, p_flight.m_AcType) < 0;
		
    }
}




void Flight::getFieldTitles (char *titleLine, int gateField)
{
    strcpy (titleLine, flightHeaders[0]);

    for (int i = 1; i < FLIGHT_FIELDS; i++)
    {
        strcat (titleLine, ",");
        strcat (titleLine, flightHeaders[i]);
    }

    if (gateField)
    {
		for(int i=FLIGHT_FIELDS;i<ALL_FIELDS;i++){
			strcat (titleLine, ",");
			strcat (titleLine, flightHeaders[i]);
		} 
    }
}
void Flight::readVersion2_2 (ArctermFile& p_file ,StringDictionary* _pStrDict )
{
	ElapsedTime aTime;
	FlightDescStruct logEntry;

	char szBuffer[1024];
	memset(szBuffer, 0, sizeof(szBuffer) / sizeof(char));
	strcpy(szBuffer, logEntry.airline);
    //p_file.getField (logEntry.airline, AIRLINE_LEN);
	p_file.getField(szBuffer, AIRLINE_LEN);
	logEntry.airline = szBuffer;

	CString csStr = logEntry.airline;
	_g_GetActiveAirlineMan( m_pTerm->m_pAirportDB )->FindOrAddEntry( csStr );

    // arrival info
    if (p_file.isBlankField())
    {
        logEntry.arrID = 0;
        p_file.skipField(1);
    }
    else
	{
		short s = 0;
        p_file.getInteger (s);
		char str[FLIGHT_LEN];
		sprintf( str, "%d", s );
		logEntry.arrID = str;
	}

    memset(szBuffer, 0, sizeof(szBuffer) / sizeof(char));
	strcpy(szBuffer, logEntry.origin);
    //p_file.getField (logEntry.fromAirport, AIRPORT_LEN);
	p_file.getField (szBuffer, AIRPORT_LEN);
	logEntry.origin = szBuffer;

    p_file.getTime (aTime);
    logEntry.arrTime = (long)aTime;

	csStr = logEntry.origin;
	_g_GetActiveAirportMan( m_pTerm->m_pAirportDB )->FindOrAddEntry( csStr );

    // depature info
    if (p_file.isBlankField())
    {
        logEntry.depID = 0;
        p_file.skipField(1);
    }
    else
	{
		short s = 0;
        p_file.getInteger (s);
		char str[FLIGHT_LEN];
		sprintf( str, "%d", s );
		logEntry.depID = str;	}

	memset(szBuffer, 0, sizeof(szBuffer) / sizeof(char));
	strcpy(szBuffer, logEntry.destination);
    //p_file.getField (logEntry.toAirport, AIRPORT_LEN);
	p_file.getField (szBuffer, AIRPORT_LEN);
	logEntry.destination = szBuffer;

    p_file.getTime (aTime);
    logEntry.depTime = (long)aTime;

	csStr = logEntry.destination;
	_g_GetActiveAirportMan( m_pTerm->m_pAirportDB )->FindOrAddEntry( csStr );

	memset(szBuffer, 0, sizeof(szBuffer) / sizeof(char));
	strcpy(szBuffer, logEntry.acType);
    //p_file.getField (logEntry.acType, AC_TYPE_LEN);
	p_file.getField (szBuffer, AC_TYPE_LEN);
	logEntry.acType = szBuffer;
	
	csStr = logEntry.acType;
	_g_GetActiveACMan( m_pTerm->m_pAirportDB )->FindOrAddEntry( csStr );

	InitFlightOperationsFromLogEntry(logEntry);
	//standGate.SetStrDict( _pStrDict );
 //   standGate.readProcessorID ( p_file );
	ALTObjectID standID;
	standID.readALTObjectID(p_file);

	if (isArriving())
		m_pArrFlightOp->SetArrStand(standID);

	if (isDeparting())
	{
		m_pDepFlightOp->SetDepStand(standID);
	}
	//arrGate = standGate;
	//depGate = standGate;
    p_file.getTime (serviceTime, TRUE);
}

void Flight::readFlightParameter(ArctermFile& p_file, StringDictionary* _pStrDict, BOOL bHasDate , CStartDate* psDate , EXTRAAIRPORTTYPE _enumExtraAirportType /* = NO_EXTRA_AIRPORT */ )
{
	char szDate[16];
	int nNum[3];
	int i = 0;
	COleDateTime dtStart, dtEnd;

	ElapsedTime aTime;
	ElapsedTime t;

	FlightDescStruct logEntry;

	char szBuffer[1024];
	memset(szBuffer, 0, sizeof(szBuffer) / sizeof(char));
	strcpy(szBuffer, logEntry.airline);
	//p_file.getField (logEntry.airline, AIRLINE_LEN);
	p_file.getField (szBuffer, AIRLINE_LEN);
	logEntry.airline = szBuffer;
	CString csStr = logEntry.airline;
	_g_GetActiveAirlineMan( m_pTerm->m_pAirportDB )->FindOrAddEntry( csStr );

	// arrival info
	if (p_file.isBlankField())
	{
		logEntry.arrID = 0;
		p_file.skipField(1);
	}
	else
	{
		memset(szBuffer, 0, sizeof(szBuffer) / sizeof(char));
		// p_file.getInteger (s);
		p_file.getField(szBuffer, FLIGHT_LEN);
		logEntry.arrID = szBuffer;
	}

	switch( _enumExtraAirportType )
	{
	case NO_EXTRA_AIRPORT:
		// only read to origin
		memset(szBuffer, 0, sizeof(szBuffer) / sizeof(char));
		strcpy(szBuffer, logEntry.origin);				//??????? don't understand this
		p_file.getField (szBuffer, AIRPORT_LEN);
		logEntry.origin = szBuffer;
		logEntry.arrStopover = NULL;
		break;
	case WITH_FROMTO:
		// if has orig field,origField ==> origin, fromField ==> arrStopover
		// else fromField ==> origin, reset the arrStopover
		memset(szBuffer, 0, sizeof(szBuffer) / sizeof(char));
		strcpy(szBuffer, logEntry.origin);
		p_file.getField (szBuffer, AIRPORT_LEN);
		if( strlen( szBuffer ) > 0 )
		{
			// origField ==> origin
			logEntry.origin = szBuffer;

			// fromField ==> arrStopover
			memset(szBuffer, 0, sizeof(szBuffer) / sizeof(char));
			strcpy(szBuffer, logEntry.arrStopover);
			p_file.getField (szBuffer, AIRPORT_LEN);
			logEntry.arrStopover = szBuffer;
		}
		else
		{
			// fromField ==> origin
			memset(szBuffer, 0, sizeof(szBuffer) / sizeof(char));
			strcpy(szBuffer, logEntry.origin);
			p_file.getField (szBuffer, AIRPORT_LEN);
			logEntry.origin = szBuffer;
			logEntry.arrStopover = NULL;
		}
		break;
	case WITH_STOPOVER:
		// read to origin
		memset(szBuffer, 0, sizeof(szBuffer) / sizeof(char));
		strcpy(szBuffer, logEntry.origin);
		p_file.getField (szBuffer, AIRPORT_LEN);
		logEntry.origin = szBuffer;

		// read to arrStopover
		memset(szBuffer, 0, sizeof(szBuffer) / sizeof(char));
		strcpy(szBuffer, logEntry.arrStopover);
		p_file.getField (szBuffer, AIRPORT_LEN);
		logEntry.arrStopover = szBuffer;
		break;
	}

	if (bHasDate)
	{
		if (!p_file.isBlankField())
		{
			memset(szDate, 0, sizeof(szDate) / sizeof(char));
			p_file.getField(szDate, 10);
			CString strDate = szDate;
			if (!strDate.IsEmpty())
			{
				int nPrevPos = 0;
				int nPos = 0;
				while (((nPos = strDate.Find('-', nPos + 1)) != -1) && (i < 3))
				{
					//CString strTemp = strDate.Mid(nPrevPos, nPos - nPrevPos); 
					nNum[i++] = atoi(strDate.Mid(nPrevPos, nPos - nPrevPos));
					nPrevPos = nPos + 1;
				}

				//CString strTemp = strDate.Mid(nPrevPos, strDate.GetLength() - nPrevPos); 
				nNum[i] = atoi(strDate.Mid(nPrevPos, strDate.GetLength() - nPrevPos));


				if (psDate && !psDate->IsAbsoluteDate())
				{
					psDate->SetAbsoluteDate( true );
					psDate->SetDate(nNum[0], nNum[1], nNum[2]);
				}
				dtStart = COleDateTime(nNum[0], nNum[1], nNum[2], 0, 0, 0);
			}
		}
		else
			p_file.skipField(1);
	}

	p_file.getTime (aTime);
	logEntry.arrTime = (long)aTime;

	if (psDate && psDate->IsAbsoluteDate() )
	{
		psDate->GetRelativeTime(dtStart, aTime, t);
		logEntry.arrTime = (long)t;
	}

	csStr = logEntry.origin;
	_g_GetActiveAirportMan( m_pTerm->m_pAirportDB )->FindOrAddEntry( csStr );
	csStr = logEntry.arrStopover;
	_g_GetActiveAirportMan( m_pTerm->m_pAirportDB )->FindOrAddEntry( csStr );

	// depature info
	if (p_file.isBlankField())
	{
		logEntry.depID = 0;
		p_file.skipField(1);
	}
	else
	{
		memset(szBuffer, 0, sizeof(szBuffer) / sizeof(char));
		//p_file.getInteger (s);
		p_file.getField(szBuffer, FLIGHT_LEN);
		logEntry.depID = szBuffer;
	}

	switch( _enumExtraAirportType )
	{
	case NO_EXTRA_AIRPORT:
		// only read to destination
		memset(szBuffer, 0, sizeof(szBuffer) / sizeof(char));
		strcpy(szBuffer, logEntry.destination);				//??????? don't understand this
		p_file.getField (szBuffer, AIRPORT_LEN);
		logEntry.destination = szBuffer;
		logEntry.depStopover = NULL;
		break;
	case WITH_FROMTO:
		// read toField
		p_file.getField (szBuffer, AIRPORT_LEN);

		char szBuf2[1024];
		p_file.getField (szBuf2, AIRPORT_LEN);
		if( strlen( szBuf2 ) > 0 )
		{
			//toField ==> depStopover
			logEntry.depStopover = szBuffer;

			// destField ==> destination
			logEntry.destination = szBuf2;

		}
		else
		{
			// toField ==> destination, reset the depStopover
			logEntry.destination = szBuffer;
			logEntry.depStopover = NULL;
		}
		break;
	case WITH_STOPOVER:
		// read to depStopover
		memset(szBuffer, 0, sizeof(szBuffer) / sizeof(char));
		strcpy(szBuffer, logEntry.depStopover);
		p_file.getField (szBuffer, AIRPORT_LEN);
		logEntry.depStopover = szBuffer;

		// read to destination
		memset(szBuffer, 0, sizeof(szBuffer) / sizeof(char));
		strcpy(szBuffer, logEntry.destination);
		p_file.getField (szBuffer, AIRPORT_LEN);
		logEntry.destination = szBuffer;
		break;
	}

	i = 0;
	if (bHasDate)
	{
		if (!p_file.isBlankField())
		{
			memset(szDate, 0, sizeof(szDate) / sizeof(char));
			p_file.getField(szDate, 10);
			CString strDate = szDate;
			if (!strDate.IsEmpty())
			{
				int nPrevPos = 0;
				int nPos = 0;
				while (((nPos = strDate.Find('-', nPos + 1)) != -1) && (i < 3))
				{
					//CString strTemp = strDate.Mid(nPrevPos, nPos - nPrevPos); 
					nNum[i++] = atoi(strDate.Mid(nPrevPos, nPos - nPrevPos));
					nPrevPos = nPos + 1;
				}

				//CString strTemp = strDate.Mid(nPrevPos, strDate.GetLength() - nPrevPos); 
				nNum[i] = atoi(strDate.Mid(nPrevPos, strDate.GetLength() - nPrevPos));

				if (psDate && !psDate->IsAbsoluteDate())
				{
					psDate->SetAbsoluteDate( true );
					psDate->SetDate(nNum[0], nNum[1], nNum[2]);
				}

				dtEnd = COleDateTime(nNum[0], nNum[1], nNum[2], 0, 0, 0);
			}
		}
		else
			p_file.skipField(1);
	}

	p_file.getTime (aTime);
	logEntry.depTime = (long)aTime;

	if (psDate && psDate->IsAbsoluteDate())
	{
		psDate->GetRelativeTime(dtEnd, aTime, t);
		logEntry.depTime = (long)t;
	}


	csStr = logEntry.destination;
	_g_GetActiveAirportMan( m_pTerm->m_pAirportDB )->FindOrAddEntry( csStr );
	csStr = logEntry.depStopover;
	_g_GetActiveAirportMan( m_pTerm->m_pAirportDB )->FindOrAddEntry( csStr );

	memset(szBuffer, 0, sizeof(szBuffer) / sizeof(char));
	strcpy(szBuffer, logEntry.acType);
	//p_file.getField (logEntry.acType, AC_TYPE_LEN);
	p_file.getField (szBuffer, AC_TYPE_LEN);
	logEntry.acType = szBuffer;


	csStr = logEntry.acType;
	_g_GetActiveACMan( m_pTerm->m_pAirportDB )->FindOrAddEntry( csStr );

	/*ProcessorID procid;
	procid.SetStrDict(_pStrDict);
	procid.readProcessorID(p_file);*/
	//standGate.SetStrDict( _pStrDict );
	//standGate.readProcessorID ( p_file );
	InitFlightOperationsFromLogEntry(logEntry);
}

void Flight::readObsoleteFlight (ArctermFile& p_file, StringDictionary* _pStrDict, BOOL bHasDate, CStartDate* psDate, EXTRAAIRPORTTYPE _enumExtraAirportType /*= NO_EXTRA_AIRPORT*/ )
{
	readFlightParameter(p_file,_pStrDict,bHasDate,psDate,_enumExtraAirportType);

	ALTObjectID standID;
	if( !standID.readALTObjectID(p_file) ) 
		p_file.skipField(1);
	else 
	{
		if (isArriving())
			m_pArrFlightOp->SetArrStand(standID);
		if (isDeparting())
			m_pDepFlightOp->SetDepStand(standID);
	}
	
//	// // TRACE("\n%s\n",standGate.GetIDString());
    //p_file.getTime (serviceTime, TRUE);
	p_file.getTime (serviceTime);

	ProcessorID arrGate;
	arrGate.SetStrDict( _pStrDict );
	arrGate.readProcessorID( p_file );
	m_ArrGate = arrGate;

	ProcessorID depGate;
	depGate.SetStrDict( _pStrDict );
	depGate.readProcessorID( p_file );
	m_DepGate = depGate;


}
void Flight::readFlight2_4(ArctermFile& p_file,StringDictionary* _pStrDict)
{
	baggageDevice.SetStrDict(_pStrDict);
	readObsoleteFlight(p_file,_pStrDict);
	baggageDevice.readProcessorID(p_file);
}

void Flight::readFlight2_5(ArctermFile& p_file,StringDictionary* _pStrDict, CStartDate* _psDate)
{
	baggageDevice.SetStrDict(_pStrDict);

	readObsoleteFlight(p_file,_pStrDict, TRUE, _psDate);

	baggageDevice.readProcessorID(p_file);
}

void Flight::readFlight2_6(ArctermFile& p_file,StringDictionary* _pStrDict, CStartDate* _psDate)
{
	baggageDevice.SetStrDict(_pStrDict);

	readObsoleteFlight(p_file,_pStrDict, TRUE, _psDate, WITH_FROMTO);

	baggageDevice.readProcessorID(p_file);
}
void Flight::readFlight2_7(ArctermFile& p_file,StringDictionary* _pStrDict, CStartDate* _psDate)
{
	baggageDevice.SetStrDict(_pStrDict);

	readObsoleteFlight(p_file,_pStrDict, TRUE, _psDate, WITH_FROMTO);

	baggageDevice.readProcessorID(p_file);

	//read star and SID
	char szBuffer[1024];
	memset(szBuffer, 0, sizeof(szBuffer) / sizeof(char));
	p_file.getField (szBuffer, 1024);
	CString m_strStar = CString(szBuffer);

	memset(szBuffer, 0, sizeof(szBuffer) / sizeof(char));
	p_file.getField (szBuffer, 1024);
	CString m_strSID = CString(szBuffer);

}
void Flight::readFlight2_8(ArctermFile& p_file,StringDictionary* _pStrDict, CStartDate* _psDate)
{
	baggageDevice.SetStrDict(_pStrDict);

	readObsoleteFlight(p_file,_pStrDict, TRUE, _psDate, WITH_FROMTO);

	baggageDevice.readProcessorID(p_file);
}

void Flight::readFlight2_9(ArctermFile& p_file,StringDictionary* _pStrDict, CStartDate* _psDate)
{
	baggageDevice.SetStrDict(_pStrDict);

	readObsoleteFlight(p_file,_pStrDict, FALSE, _psDate, WITH_FROMTO);

	baggageDevice.readProcessorID(p_file);
}

void Flight::readFlight3_0(ArctermFile& p_file,StringDictionary* _pStrDict, CStartDate* _psDate)
{
	baggageDevice.SetStrDict(_pStrDict);

	readFlightParameter(p_file,_pStrDict, FALSE, _psDate, WITH_FROMTO);

	ALTObjectID standID1;
	if( !standID1.readALTObjectID(p_file) ) 
		p_file.skipField(1);
	else 
	{
		m_pArrFlightOp->SetArrStand(standID1);
	}

	ALTObjectID standID2;
	if( !standID2.readALTObjectID(p_file) ) 
		p_file.skipField(1);
	else 
	{
		m_pDepFlightOp->SetDepStand(standID2);
	}

	p_file.getTime (serviceTime);

	ProcessorID arrGate;
	arrGate.SetStrDict( _pStrDict );
	arrGate.readProcessorID( p_file );
	m_ArrGate = arrGate;

	ProcessorID depGate;
	depGate.SetStrDict( _pStrDict );
	depGate.readProcessorID( p_file );
	m_DepGate = depGate;
	baggageDevice.readProcessorID(p_file);
}

void Flight::readFlight3_1(ArctermFile& p_file,StringDictionary* _pStrDict, CStartDate* _psDate)
{
	baggageDevice.SetStrDict(_pStrDict);

	readFlightParameter(p_file,_pStrDict, FALSE, _psDate, WITH_FROMTO);

	ALTObjectID standID1;
	if( !standID1.readALTObjectID(p_file) ) 
		p_file.skipField(1);
	else 
	{
		m_pArrFlightOp->SetArrStand(standID1);
	}

	ALTObjectID standID2;
	if( !standID2.readALTObjectID(p_file) ) 
		p_file.skipField(1);
	else 
	{
		m_pDepFlightOp->SetDepStand(standID2);
	}

	ALTObjectID standID3;
	if( !standID3.readALTObjectID(p_file) ) 
		p_file.skipField(1);
	else 
	{
		if (isTurnaround())
			m_IntermediateStand = standID3;
	}

	p_file.getTime (serviceTime);

	ProcessorID arrGate;
	arrGate.SetStrDict( _pStrDict );
	arrGate.readProcessorID( p_file );
	m_ArrGate = arrGate;

	ProcessorID depGate;
	depGate.SetStrDict( _pStrDict );
	depGate.readProcessorID( p_file );
	m_DepGate = depGate;
	baggageDevice.readProcessorID(p_file);
}

void Flight::readVersion1_0 (ArctermFile& p_file)
{
    char str[128];
    char mode;

    p_file.setToField (4);
    p_file.getChar (mode);
    p_file.setToField (0);

    // Flight identification field
    p_file.getField (str, 16);


	FlightDescStruct logEntry;

    int index = 0;
    /*while (isalpha (str[index]) && index < AIRLINE_LEN-1)
        logEntry.airline[index] = str[index++];
    logEntry.airline[index] = '\0';*/
	char* pszBuffer = new char[AIRLINE_LEN];
	while (isalpha (str[index]) && index < AIRLINE_LEN-1)
        pszBuffer[index] = str[index++];
    pszBuffer[index] = '\0';
	logEntry.airline = pszBuffer;


    while (!isdigit (str[index]))
        index++;
    setFlightID (str + index, mode);

    // AC-Type field
	memset(pszBuffer, 0, sizeof(pszBuffer) / sizeof(char));
	strcpy(pszBuffer, logEntry.acType);
    p_file.getField (pszBuffer, 7);
	logEntry.acType = pszBuffer;
	delete[] pszBuffer;

    // Airport field
    p_file.getField (str, 5);
    setAirport (str, mode);

    // Sched field: scheduled time of departure
    ElapsedTime gateTime;
    p_file.getTime (gateTime);
    setGateTime (gateTime, mode);

    // Mode field: Arrival or Departure
    p_file.getChar (mode);

	InitFlightOperationsFromLogEntry(logEntry);
    // Gate Field
    if (!p_file.eol())
	{
		//standGate.readProcessorID (p_file);
		//arrGate = standGate;
		//depGate = standGate;
		ALTObjectID standID;
		standID.readALTObjectID(p_file);
		if (isArriving())
		{
			m_pArrFlightOp->SetArrStand(standID);
		}
		if (isDeparting())
		{
			m_pDepFlightOp->SetDepStand(standID);
		}
	}

    // Load / Unload time
    if (!p_file.eol())
        p_file.getTime (serviceTime, TRUE);
}

void Flight::writeFlight (ArctermFile& p_file, StringDictionary* _pStrDict, int p_flush)
{
    char str[512];
    printFlight (str, 1, _pStrDict);
    p_file.writeField (str);
    if (p_flush)
        p_file.writeLine();
}

void Flight::printFlight (char *p_str, int p_printGate, StringDictionary* _pStrDict)
{
	//CStartDate sDate, outDate;
	//ElapsedTime outTime;

    strcpy (p_str, m_Airline);
    strcat (p_str, ",");
    if (isArriving())
    {
        //itoa (logEntry.arrID, p_str + strlen (p_str), 10);
		strcat(p_str, m_pArrFlightOp->GetArrFlightID());
		strcat (p_str, ",");
		strcat (p_str, m_pArrFlightOp->GetOriginAirport());
        strcat (p_str, ",");
		strcat (p_str, m_arrStopover);
        strcat (p_str, ",");
		//strcat (p_str, logEntry.arrDate.PrintDate());

		//Modify by adam 2007-04-24
		//bool bAbsDate;
		//COleDateTime date;
		//int nDtIdx;
		//COleDateTime time;
		//sDate.GetDateTime( getArrTime(), bAbsDate, date, nDtIdx, time );
		//if( bAbsDate )
		//	strcat(p_str, date.Format(_T("%Y-%m-%d")) );
		//strcat (p_str, ",");

		ElapsedTime etTime;
		etTime = getArrTime();
		strcat( p_str, etTime.printTime());
        strcat (p_str, ",");
		//End modify by adam 2007-04-24
    }
    else
        strcat (p_str, ",,,0:00,");

    if (isDeparting())
    {
        //itoa (logEntry.depID, p_str + strlen (p_str), 10);
		strcat(p_str, m_pDepFlightOp->GetDepFlightID());
        strcat (p_str, ",");
		strcat (p_str, m_depStopover);
        strcat (p_str, ",");
		strcat (p_str, m_pDepFlightOp->GetDestinationAirport());
		strcat (p_str, ",");

		//Modify by adam 2007-04-24
		//bool bAbsDate;
		//COleDateTime date;
		//int nDtIdx;
		//COleDateTime time;
		//sDate.GetDateTime( getDepTime(), bAbsDate, date, nDtIdx, time );
		//if( bAbsDate )
		//	strcat(p_str, date.Format(_T("%Y-%m-%d")) );
		//strcat (p_str, ",");
		ElapsedTime etTime;
		etTime = getDepTime();
		strcat( p_str, etTime.printTime());
        strcat (p_str, ",");
		//End Modify by adam 2007-04-24
    }
    else
        strcat (p_str, ",,,0:00,");

    strcat (p_str, m_AcType);

    if (p_printGate)
    {
        strcat (p_str, ",");
		//standGate.SetStrDict( _pStrDict );
		if(isArriving())
		{
			ALTObjectID standID(m_pArrFlightOp->GetArrStand());
			standID.printID (p_str + strlen (p_str));
		}		
        strcat (p_str, ",");

		if(isDeparting())
		{
			ALTObjectID standID(m_pDepFlightOp->GetDepStand());
			standID.printID (p_str + strlen (p_str));
		}

		strcat (p_str, ",");
		m_IntermediateStand.printID (p_str + strlen (p_str));
		

        strcat (p_str, ",");
		ElapsedTime stayTime;
		if (isTurnaround())
		{
			stayTime = getDepTime() - getArrTime();
		}
		else
		{
			stayTime = serviceTime;
		}
        stayTime.printTime (p_str + strlen (p_str), 0);

		strcat(p_str,",");
		getArrParkingTime().printTime(p_str+strlen(p_str),0);

		strcat(p_str,",");
		getDepParkingTime().printTime(p_str+strlen(p_str),0);

		strcat(p_str,",");
		getIntParkingTime().printTime(p_str+strlen(p_str),0);

        strcat (p_str, ",");
		ProcessorID arrGate = m_ArrGate;
		arrGate.SetStrDict( _pStrDict );
		arrGate.printID( p_str + strlen (p_str) );

		strcat (p_str, ",");
		ProcessorID depGate = m_DepGate;
		depGate.SetStrDict( _pStrDict );
		depGate.printID( p_str + strlen (p_str) );
    }
	
	strcat(p_str,",");
	baggageDevice.SetStrDict(_pStrDict);
	baggageDevice.printID(p_str + strlen(p_str));
	strcat (p_str, ",");

	CString strload = "";
	if (m_arrLFInput >0)
		strload.Format("%.2f", (m_arrLFInput*100));
	strcat( p_str, strload);
	strcat (p_str, ",");

	strload = "";
	if (m_depLFInput >0)
		strload.Format("%.2f", (m_depLFInput*100));
	strcat( p_str, strload);
	strcat (p_str, ",");

	CString strCap = "";
	if (m_capacityInput >0)
		strCap.Format("%d", m_capacityInput);
	strcat( p_str, strCap);

	CString strValue;
	strValue.Format(_T("%d"),m_ataTime);
	strcat(p_str,",");
	strcat( p_str, strValue);

	strValue.Format(_T("%d"),m_atdTime);
	strcat(p_str,",");
	strcat( p_str, strValue);

	strValue.Format(_T("%d"),m_towoffTime);
	strcat(p_str,",");
	strcat( p_str, strValue);

	strValue.Format(_T("%d"),m_exIntStandTime);
	strcat(p_str,",");
	strcat( p_str, strValue);
	
	//star ,sid
	//strcat(p_str,",");
	//strcat(p_str,m_strStar.GetBuffer());
	//strcat(p_str,",");
	//strcat(p_str,m_strSID.GetBuffer());
}


void Flight::printFlight(char *p_str, char p_mode, int p_printGate, StringDictionary* _pStrDict)
{
	strcpy (p_str, m_Airline);
	strcat (p_str, ",");
	if (p_mode == 'A')
	{
		strcat(p_str, m_pArrFlightOp->GetArrFlightID());
		strcat (p_str, ",");
		strcat (p_str, m_pArrFlightOp->GetOriginAirport());
		strcat (p_str, ",");
		strcat (p_str, m_arrStopover);
		strcat (p_str, ",");
		ElapsedTime etTime;
		etTime = getArrTime();
		strcat( p_str, etTime.printTime());
		strcat (p_str, ",");

	}
	else
		strcat (p_str, ",,,0:00,");

	if (p_mode == 'D')
	{
		strcat(p_str, m_pDepFlightOp->GetDepFlightID());
		strcat (p_str, ",");
		strcat (p_str, m_depStopover);
		strcat (p_str, ",");
		strcat (p_str, m_pDepFlightOp->GetDestinationAirport());
		strcat (p_str, ",");
		ElapsedTime etTime;
		etTime = getDepTime();
		strcat( p_str, etTime.printTime());
		strcat (p_str, ",");
	}
	else
		strcat (p_str, ",,,0:00,");

	if (p_mode == 'T')
	{
		strcat(p_str, m_pArrFlightOp->GetArrFlightID());
		strcat (p_str, ",");
		strcat (p_str, m_pArrFlightOp->GetOriginAirport());
		strcat (p_str, ",");
		strcat (p_str, m_arrStopover);
		strcat (p_str, ",");
		ElapsedTime etTime;
		etTime = getArrTime();
		strcat( p_str, etTime.printTime());
		strcat (p_str, ",");

		strcat(p_str, m_pDepFlightOp->GetDepFlightID());
		strcat (p_str, ",");
		strcat (p_str, m_depStopover);
		strcat (p_str, ",");
		strcat (p_str, m_pDepFlightOp->GetDestinationAirport());
		strcat (p_str, ",");
		etTime = getDepTime();
		strcat( p_str, etTime.printTime());
		strcat (p_str, ",");
	}

	strcat (p_str, m_AcType);
	if (p_printGate)
	{
		strcat (p_str, ",");
		//standGate.SetStrDict( _pStrDict );
		if(isArriving())
		{
			ALTObjectID standID(m_pArrFlightOp->GetArrStand());
			standID.printID (p_str + strlen (p_str));
		}		
		strcat (p_str, ",");

		if(isDeparting())
		{
			ALTObjectID standID(m_pDepFlightOp->GetDepStand());
			standID.printID (p_str + strlen (p_str));
		}

		strcat (p_str, ",");
		m_IntermediateStand.printID (p_str + strlen (p_str));

		strcat (p_str, ",");
		serviceTime.printTime (p_str + strlen (p_str), 0);

		strcat (p_str, ",");
		ProcessorID arrGate;
		arrGate.SetStrDict( _pStrDict );
		arrGate.printID( p_str + strlen (p_str) );

		strcat (p_str, ",");
		ProcessorID depGate;
		depGate.SetStrDict( _pStrDict );
		depGate.printID( p_str + strlen (p_str) );
	}

	strcat(p_str,",");
	baggageDevice.SetStrDict(_pStrDict);
	baggageDevice.printID(p_str + strlen(p_str));
}

void Flight::getArrStopover (char *p_str) const
{ 
	strcpy( p_str, m_arrStopover); 
}

void Flight::getDepStopover (char *p_str) const
{ 
	strcpy (p_str, m_depStopover); 
}	

//void Flight::getDestination (char *p_str) const
//{
//	strcpy (p_str, getDestination()); 
//}

//void Flight::getACType (char *p_str) const 
//{
//	strcpy (p_str, m_AcType);
//}

//const ALTObjectID Flight::getStand()const 
//{ 
//	if (!isArriving())
//	{
//		return getDepStand();
//	}
//	
//	return getArrStand();
//	 
//}

//const ALTObjectID Flight::getArrStand()const 
//{ 
//	return m_pArrFlightOp->GetArrStand();
//}
//
//const ALTObjectID Flight::getDepStand()const 
//{
//
//	return m_pDepFlightOp->GetDepStand();
//}

//const ALTObjectID Flight::getIntermediateStand() const
//{
//
//	return m_IntermediateStand;
//}

const ProcessorID Flight::getArrGate (void) const 
{ 
	return m_ArrGate;
}

const ProcessorID Flight::getDepGate (void) const 
{ 
	return m_DepGate;
}

// assume p_mode = { 'A', 'D' }
FlightConstraint Flight::getType (char p_mode) const
{
    if (! isFlightMode(p_mode) || p_mode == 'T')
        throw new StringError ("Invalid mode in call to Flight::getType");

    FlightConstraint type;

    type.setAirline (m_Airline);
    type.setFlightID ((p_mode == 'D')? getDepID(): getArrID());
	type.setDay((p_mode == 'D')? getDepTime().GetDay(): getArrTime().GetDay());
	type.setTime((p_mode == 'D')? getDepTime().getPrecisely(): getArrTime().getPrecisely());
	type.SetFltConstraintMode( (p_mode=='A') ? ENUM_FLTCNSTR_MODE_ARR : ENUM_FLTCNSTR_MODE_DEP );
	type.setAirport ((p_mode == 'D')?getDestination(): getOrigin());
	type.setStopoverAirport((p_mode == 'D')? m_depStopover: m_arrStopover);
    type.setACType (m_AcType);
    return type;
}

void Flight::setStopoverAirport(const char *p_str, char p_mode)
{
	if (p_mode == 'D')
		m_depStopover = p_str;
	else if (p_mode == 'A')
		m_arrStopover = p_str;
	else if (isDeparting())
		m_depStopover = p_str;
	else
		m_arrStopover = p_str;
}


//void Flight::setLoad (int p_count, char p_mode)
//{
//    if (p_mode == 'A')
//       m_arrLoad = (short)p_count;
//    else if (p_mode == 'D')
//	{
//		curPax = 0;
//       m_depLoad =  (short)p_count;
//	}
//}
void Flight::setArrStopover (const char *p_str) 
{ 
	m_arrStopover = p_str; 
}

void Flight::setDepStopover (const char *p_str) 
{ 
	m_depStopover = p_str; 
}

void Flight::setArrGate(const ProcessorID& p_id)
{ 
	ProcessorID ArrGateID(p_id);
	ArrGateID.SetStrDict(m_pTerm->inStrDict);
	m_ArrGate = ArrGateID;
}

void Flight::setDepGate(const ProcessorID& p_id) 
{ 
	ProcessorID DepGateID(p_id);
	DepGateID.SetStrDict(m_pTerm->inStrDict);
	m_DepGate = DepGateID;
}

void Flight::setArrLoadFactor(double fFactor)
{
	m_arrloadFactor = fFactor;
}

void Flight::setCapacity(int nCapacity)
{
	m_capacity = nCapacity;
}

bool Flight::isSameGate()
{
	return (m_ArrGate == m_DepGate)?true:false ;
}
// display a flight as string " EEE555 / EEE666.ACA,5:00,ABJ,6:00,100 "
void Flight::displayFlight( char *p_str ) const
{
   strcpy (p_str, m_Airline);

   if( isTurnaround() )
   {
	   //itoa( logEntry.arrID, p_str+strlen(p_str),10 );
	   strcat(p_str, getArrID());
	   strcat( p_str, " / " );
	   strcat( p_str, m_Airline );
	   //itoa( logEntry.depID, p_str+strlen(p_str),10 );
	   strcat(p_str, getDepID());
   }
   else if( isArriving() )		// arrival
	   //itoa( logEntry.arrID, p_str+strlen(p_str),10 );
	   strcat(p_str,  getArrID());
   else if( isDeparting() )		// departure
	   //itoa( logEntry.depID, p_str+strlen(p_str),10 );
	   strcat(p_str, getDepID());

   strcat( p_str, ",");

   if (isArriving())
   {
	   strcat (p_str, getOrigin());
	   strcat (p_str, ",");
	   (getArrTime()).printTime (p_str + strlen (p_str), 0);
	   strcat (p_str, ",");
   }
   else
	   strcat (p_str, " , , , ");
   
   if (isDeparting())
   {
	   strcat (p_str, getDestination());
	   strcat (p_str, ",");
	   (getDepTime()).printTime (p_str + strlen (p_str), 0);
	   strcat (p_str, ",");
   }
   else
	   strcat (p_str, " , , , ");
   
   strcat (p_str, m_AcType);
}


// create flightarrdep event from flight data
bool Flight::createFlightEvent( Terminal* _pTerm )
{
	ElapsedTime ArrivalTime, DepartureTime;
	int iStandGateIndex = getStandID();

	// if the flight doesn't have stand gate, can't create flight event
	if( iStandGateIndex == -1 )	// no gate
		return false;

	ElapsedTime arrDelay(0l);
	ElapsedTime depDelay(0l);
	if (isArriving())
		arrDelay = getFlightDelayTime( _pTerm, 'A');
	if(isDeparting())
		depDelay = getFlightDelayTime(_pTerm, 'D');

	ElapsedTime zeroTime(0l);
	
	// get arr&dep time
	if( getFlightMode() == 'D' )		// departure
	{
		DepartureTime = max( (getDepTime() + depDelay), zeroTime );
		ArrivalTime = max( (DepartureTime - getServiceTime()),zeroTime );
	}
	else						// arrival & turnround
	{
		ArrivalTime = max( (getArrTime() + arrDelay), zeroTime );
		if( isTurnaround() )	// turnround
			DepartureTime = max( (getDepTime() + depDelay), zeroTime );
		else					// arrival	
			DepartureTime = ArrivalTime + getDeplaneTime();
	}

	// new arr& dep event
	FlightArrDepEvent* pArrEvent = new FlightArrDepEvent( getArrGateIndex(), FlightArrivalAtAirport );
	pArrEvent->setTime( ArrivalTime );
	pArrEvent->addEvent();

	FlightArrDepEvent* pDepEvent = new FlightArrDepEvent( getDepGateIndex(), FlightDepartureAirport );
	pDepEvent->setTime( DepartureTime );
	pDepEvent->addEvent();

	return true;
}


// get flight delay time
ElapsedTime Flight::getFlightDelayTime( Terminal* _pTerm , char p_mode )
{
	assert( _pTerm );
	
	ElapsedTime delay;
	FlightConstraintDatabase* pDelayDB =  _pTerm->flightData->getDelays();
	FlightConstraint flight = getType(p_mode);
	const ProbabilityDistribution *delayPD = pDelayDB->lookup( flight );
	if( !delayPD )
	{
		/*
		char str[128];
        flight.printConstraint (str);
        throw new TwoStringError ("no flight delay time for ", str);
		*/
		delay = (float)0.0;
		return delay;
	}

	delay = (float)( 60.0 * delayPD->getRandomValue() );
	
	return delay;
}

void Flight::getGateOccupancyTime(ElapsedTime& time1, ElapsedTime& time2) const
{
	time1 = getGateTime();

	if (getFlightMode() == 'D' )
		time1 = getDepTime() - getServiceTime() ;

	if (isTurnaround())
		time2 = getDepTime();
	else if (getFlightMode() == 'D' )
		time2 = getDepTime();
	else
		time2 = time1 + getServiceTime();
}

void Flight::echo(EchoLogType key) const
{
	if (ECHOLOG->IsEnable() && ECHOLOG->IsLogEnable(key))
	{
		CString strContent;
		char szInfo[100];

		memset(szInfo, 0, sizeof(szInfo));
		getAirline( szInfo );
		strContent.Append( szInfo );
		strContent.Append( "," );

		memset(szInfo, 0, sizeof(szInfo));
		getArrID( szInfo );
		strContent.Append( szInfo );
		strContent.Append( "," );

		memset(szInfo, 0, sizeof(szInfo));
		getDepID( szInfo );
		strContent.Append( szInfo );
		strContent.Append( "," );

		memset(szInfo, 0, sizeof(szInfo));
		getOrigin( szInfo );
		strContent.Append( szInfo );
		strContent.Append( "," );

		memset(szInfo, 0, sizeof(szInfo));
		getDestination( szInfo );
		strContent.Append( szInfo );
		strContent.Append( "," );

		memset(szInfo, 0, sizeof(szInfo));
		getACType( szInfo );
		strContent.Append( szInfo );

		ECHOLOG->Log(key, strContent);

	}
}

void Flight::InitFlightOperationsFromLogEntry(FlightDescStruct& logEntry)
{
	m_AcType = logEntry.acType;
	m_Airline = logEntry.airline;

	m_arrStopover = logEntry.arrStopover;
	m_depStopover = logEntry.depStopover;

	m_arrDelay = logEntry.arrDelay;
	m_depDelay = logEntry.depDelay;

	m_bagFirstBirthTimeInReclaim = logEntry.bagFirstBirthTimeInReclaim;
	m_bagLastLeaveTimeInReclaim = logEntry.bagLastLeaveTimeInReclaim;

	m_arrloadFactor = logEntry.arrloadFactor;
	m_deploadFactor = logEntry.deploadFactor;

	m_capacity = logEntry.capacity;
	m_bagCarosel = logEntry.bagCarosel;

	m_nDepTransit = logEntry.nDepTransit;		
	m_nArrTransit = logEntry.nArrTransit;

	m_nDepTransfer = logEntry.nDepTransfer;		
	m_nArrTransfer = logEntry.nArrTransfer;	

	if (logEntry.arrID.HasValue())
	{
		m_pArrFlightOp->SetArrFlightID(logEntry.arrID);
		m_pArrFlightOp->SetArrTime(logEntry.arrTime);
		m_pArrFlightOp->SetOriginAirport(logEntry.origin);
		m_pArrFlightOp->SetArrStandIdx(logEntry.gate);
		m_nArrGateIdx = logEntry.nArrGate;
	}
	if (logEntry.depID.HasValue())
	{
		m_pDepFlightOp->SetDepFlightID(logEntry.depID);
		m_pDepFlightOp->SetDepTime(logEntry.depTime);
		m_pDepFlightOp->SetDestinationAirport(logEntry.destination);
		m_pDepFlightOp->SetDepStandIdx(logEntry.gate);
		m_nDepGateIdx = logEntry.nDepGate;
	}
}

int Flight::getArrGateIndex (void) const 
{ 
	return m_nArrGateIdx; 
}

int Flight::getDepGateIndex (void) const
{ 
	return m_nDepGateIdx;
}


void Flight::setArrGateIndex (int p_ndx) 
{ 
	m_nArrGateIdx = (short)p_ndx; 
}

void Flight::setDepGateIndex (int p_ndx) 
{ 
	m_nDepGateIdx = (short)p_ndx;
}

const int Flight::getArrLoad()
{
	if (m_arrloadFactor > 0 && m_capacity >0)
		return int(m_arrloadFactor* m_capacity);

	return 0;
}

const int Flight::getDepLoad() 
{ 
	if (m_deploadFactor > 0 && m_capacity >0)
		return int(m_deploadFactor* m_capacity);

	return 0;
}

int Flight::spaceAvail()
{
	return curPax < getDepLoad(); 
}


int Flight::getSpace (char p_mode) 
{ 
	return (p_mode == 'A')?getArrLoad(): getDepLoad() - curPax; 
}

FlightDescStruct Flight::getLogEntry () 
{ 
	FlightDescStruct logEntry;

	logEntry.acType = m_AcType ;
	logEntry.airline = m_Airline;

	logEntry.arrStopover = m_arrStopover;
	logEntry.depStopover = m_depStopover;

	logEntry.arrDelay = m_arrDelay;
	logEntry.depDelay = m_depDelay;

	logEntry.bagFirstBirthTimeInReclaim = m_bagFirstBirthTimeInReclaim;
	logEntry.bagLastLeaveTimeInReclaim = m_bagLastLeaveTimeInReclaim;

	logEntry.arrloadFactor = static_cast<float>(m_arrloadFactor);
	logEntry.deploadFactor = static_cast<float>(m_deploadFactor);

	logEntry.capacity = m_capacity;
	logEntry.bagCarosel = m_bagCarosel;

	logEntry.nDepTransit = m_nDepTransit;		
	logEntry.nArrTransit = m_nArrTransit;

	logEntry.nDepTransfer = m_nDepTransfer;		
	logEntry.nArrTransfer = m_nArrTransfer;	

	if (isArriving())
	{
		logEntry.arrID = m_pArrFlightOp->GetArrFlightID();
		logEntry.arrLoad = short(m_arrloadFactor* m_capacity);
		logEntry.arrTime = m_pArrFlightOp->GetArrTime();
		logEntry.origin = m_pArrFlightOp->GetOriginAirport();
		logEntry.gate = m_pArrFlightOp->GetArrStandIdx();
		logEntry.nArrGate = m_nArrGateIdx;
	}
	if (isDeparting())
	{
		logEntry.depID = m_pDepFlightOp->GetDepFlightID();
		logEntry.depLoad = short(m_deploadFactor* m_capacity);
		logEntry.depTime = m_pDepFlightOp->GetDepTime();
		logEntry.destination = m_pDepFlightOp->GetDestinationAirport();
		logEntry.gate = m_pDepFlightOp->GetDepStandIdx();
		logEntry.nDepGate = m_nDepGateIdx;
	}
	logEntry.gateOccupancy = getServiceTime();
	//Stand stand;
	//stand.ReadObject( getStandID() );

	//CPoint2008 p = stand.GetServicePoint();
	//logEntry.nStandGateX = (float)p.getX();
	//logEntry.nStandGateY = (float)p.getY();
	//logEntry.nStandGateZ = (short)p.getZ();

	return logEntry; 
}

bool Flight::isBelongToGroup( FlightGroup* pFlightGroup,bool bArrival)
{
	//airline
	CString strAirline;
	getAirline(strAirline.GetBuffer(1024));
	strAirline.ReleaseBuffer();

	CString strFlightID;
	CString strDay;
	ElapsedTime etDayTime;
	if (bArrival)
	{
		getArrID(strFlightID.GetBuffer(1024));
		etDayTime = getArrTime();
		strDay.Format("%d", etDayTime.GetDay());
		
	}
	else
	{
		getDepID(strFlightID.GetBuffer(1024));
		etDayTime = getDepTime();
		strDay.Format("%d", etDayTime.GetDay());
	}
	strFlightID.ReleaseBuffer(); 
	return pFlightGroup->contain(strAirline, strFlightID,strDay,etDayTime);
	//flight id
}

void Flight::readFlight3_2( ArctermFile& p_file,StringDictionary* _pStrDict, CStartDate* _psDate /*= NULL*/ )
{
	baggageDevice.SetStrDict(_pStrDict);

	readFlightParameter(p_file,_pStrDict, FALSE, _psDate, WITH_FROMTO);

	ALTObjectID standID1;
	if( !standID1.readALTObjectID(p_file) ) 
		p_file.skipField(1);
	else 
	{
		m_pArrFlightOp->SetArrStand(standID1);
	}

	ALTObjectID standID2;
	if( !standID2.readALTObjectID(p_file) ) 
		p_file.skipField(1);
	else 
	{
		m_pDepFlightOp->SetDepStand(standID2);
	}

	ALTObjectID standID3;
	if( !standID3.readALTObjectID(p_file) ) 
		p_file.skipField(1);
	else 
	{
		//if (isTurnaround())
		m_IntermediateStand = standID3;
	}

	p_file.getTime (serviceTime);
	p_file.getTime(m_tArrParking);
	p_file.getTime(m_tDepParking);
	p_file.getTime(m_tIntParking);

	if (isTurnaround()&& m_tArrParking == 0L)
	{
		if (getIntermediateStand().IsBlank())
			setArrParkingTime(getDepTime()-getArrTime());
		else
		{
			setArrParkingTime(getDeplaneTime());
			setDepParkingTime(getEnplaneTime());
			setIntParkingTime(getDepTime()-getArrTime() - getDeplaneTime() - getEnplaneTime());
		}
	}
	else if (isArriving()&& m_tArrParking == 0L)
		setArrParkingTime(getServiceTime());
	else if (isDeparting()&& m_tDepParking == 0L)
		setDepParkingTime(getServiceTime());				

	ProcessorID arrGate;
	arrGate.SetStrDict( _pStrDict );
	arrGate.readProcessorID( p_file );
	m_ArrGate = arrGate;

	ProcessorID depGate;
	depGate.SetStrDict( _pStrDict );
	depGate.readProcessorID( p_file );
	m_DepGate = depGate;
	baggageDevice.readProcessorID(p_file);


}

void Flight::ClearArrOperation()
{
	if(m_pArrFlightOp)
		m_pArrFlightOp->ClearData();

	m_arrDelay = 0L;
	m_tArrParking = 0L;

	m_arrStopover = ""; 
	m_nArrTransit =0;
	m_nArrTransfer =0;
	ProcessorID emptyID;
	m_ArrGate = emptyID;
	m_nArrGateIdx =-1;
	m_arrloadFactor = -1.0;
	m_arrLFInput = -1.0;

}

void Flight::ClearDepOperation()
{
	if (m_pDepFlightOp)
		m_pDepFlightOp->ClearData();

	m_depDelay = 0L;
	m_tDepParking = 0L;	
	
	m_depStopover = ""; 
	m_nDepTransit =0;		
	m_nDepTransfer =0;
	ProcessorID emptyID;
	m_DepGate = emptyID;
	m_nDepGateIdx =-1;
	m_deploadFactor = -1.0;
	m_depLFInput = -1.0;

}

ElapsedTime Flight::GetRealDepTime()const
{
	if (!long(m_RealDepTime))
	{
		if (m_RealDepTime == getDepTime())
		{
			return m_RealDepTime;
		}
	}
	else if (!(m_RealDepTime == getDepTime()))
	{
		return m_RealDepTime;
	}

	 return getDepTime(); 
}

void Flight::setDepLoadFactor( double fFactor )
{
	curPax = 0;//it is very important, it is used in getSpace(), curpax is the departure pax count has been load 
	m_deploadFactor = fFactor;
}

void Flight::readFlight3_3( ArctermFile& p_file,StringDictionary* _pStrDict, CStartDate* _psDate /*= NULL*/ )
{
	baggageDevice.SetStrDict(_pStrDict);

	readFlightParameter(p_file,_pStrDict, FALSE, _psDate, WITH_FROMTO);

	ALTObjectID standID1;
	if( !standID1.readALTObjectID(p_file) ) 
		p_file.skipField(1);
	else 
	{
		m_pArrFlightOp->SetArrStand(standID1);
	}

	ALTObjectID standID2;
	if( !standID2.readALTObjectID(p_file) ) 
		p_file.skipField(1);
	else 
	{
		m_pDepFlightOp->SetDepStand(standID2);
	}

	ALTObjectID standID3;
	if( !standID3.readALTObjectID(p_file) ) 
		p_file.skipField(1);
	else 
	{
		//if (isTurnaround())
		m_IntermediateStand = standID3;
	}

	p_file.getTime (serviceTime);
	p_file.getTime(m_tArrParking);
	p_file.getTime(m_tDepParking);
	p_file.getTime(m_tIntParking);

	if (isTurnaround()&& m_tArrParking == 0L)
	{
		if (getIntermediateStand().IsBlank())
			setArrParkingTime(getDepTime()-getArrTime());
		else
		{
			setArrParkingTime(getDeplaneTime());
			setDepParkingTime(getEnplaneTime());
			setIntParkingTime(getDepTime()-getArrTime() - getDeplaneTime() - getEnplaneTime());
		}
	}
	else if (isArriving()&& m_tArrParking == 0L)
		setArrParkingTime(getServiceTime());
	else if (isDeparting()&& m_tDepParking == 0L)
		setDepParkingTime(getServiceTime());				

	ProcessorID arrGate;
	arrGate.SetStrDict( _pStrDict );
	arrGate.readProcessorID( p_file );
	m_ArrGate = arrGate;

	ProcessorID depGate;
	depGate.SetStrDict( _pStrDict );
	depGate.readProcessorID( p_file );
	m_DepGate = depGate;
	baggageDevice.readProcessorID(p_file);

	int nValue =-1;
	p_file.getInteger(nValue);
	if (nValue > 0)
		m_arrLFInput = (double)nValue/100;

	p_file.getInteger(nValue);
	if (nValue > 0)
		m_depLFInput = (double)nValue/100;

	p_file.getInteger(nValue);
	if (nValue > 0)
		m_capacityInput = nValue;


}

double Flight::getArrLoadfactor() const
{
	return m_arrloadFactor;
}

double Flight::getDepLoadfactor() const
{
	return m_deploadFactor;
}

int Flight::getCapacity() const
{
	return m_capacity;
}

void Flight::setArrLFInput( double fLF )
{
	m_arrLFInput = fLF;
}

void Flight::setDepLFInput( double fLF )
{
	m_depLFInput = fLF;
}

void Flight::setCapacityInput( int nCapacity )
{
	m_capacityInput = nCapacity;
}

double Flight::getArrLFInput() const
{
	return m_arrLFInput;
}

double Flight::getDepLFInput() const
{
	return m_depLFInput;
}

int Flight::getCapacityInput() const
{
	return m_capacityInput;
}

void Flight::readFligt3_5(ArctermFile& p_file,StringDictionary* _pStrDict, CStartDate* _psDate /* = NULL */)
{
	baggageDevice.SetStrDict(_pStrDict);

	readFlightParameter(p_file,_pStrDict, FALSE, _psDate, WITH_FROMTO);

	ALTObjectID standID1;
	if( !standID1.readALTObjectID(p_file) ) 
		p_file.skipField(1);
	else 
	{
		m_pArrFlightOp->SetArrStand(standID1);
	}

	ALTObjectID standID2;
	if( !standID2.readALTObjectID(p_file) ) 
		p_file.skipField(1);
	else 
	{
		m_pDepFlightOp->SetDepStand(standID2);
	}

	ALTObjectID standID3;
	if( !standID3.readALTObjectID(p_file) ) 
		p_file.skipField(1);
	else 
	{
		//if (isTurnaround())
		m_IntermediateStand = standID3;
	}

	p_file.getTime (serviceTime);
	p_file.getTime(m_tArrParking);
	p_file.getTime(m_tDepParking);
	p_file.getTime(m_tIntParking);

	if (isTurnaround()&& m_tArrParking == 0L)
	{
		if (getIntermediateStand().IsBlank())
			setArrParkingTime(getDepTime()-getArrTime());
		else
		{
			setArrParkingTime(getDeplaneTime());
			setDepParkingTime(getEnplaneTime());
			setIntParkingTime(getDepTime()-getArrTime() - getDeplaneTime() - getEnplaneTime());
		}
	}
	else if (isArriving()&& m_tArrParking == 0L)
		setArrParkingTime(getServiceTime());
	else if (isDeparting()&& m_tDepParking == 0L)
		setDepParkingTime(getServiceTime());				

	ProcessorID arrGate;
	arrGate.SetStrDict( _pStrDict );
	arrGate.readProcessorID( p_file );
	m_ArrGate = arrGate;

	ProcessorID depGate;
	depGate.SetStrDict( _pStrDict );
	depGate.readProcessorID( p_file );
	m_DepGate = depGate;
	baggageDevice.readProcessorID(p_file);

	double aValue = -1.0;
	p_file.getFloat(aValue);
	if (aValue > 0)
		m_arrLFInput = (double)aValue/100;
	else
		p_file.skipField(1);

	p_file.getFloat(aValue);
	if (aValue > 0)
		m_depLFInput = (double)aValue/100;
	else
		p_file.skipField(1);

	int nCapacity = -1;
	p_file.getInteger(nCapacity);
	if (nCapacity > 0)
		m_capacityInput = nCapacity;

	//new add member
	p_file.getInteger(m_ataTime);
	p_file.getInteger(m_atdTime);
	p_file.getInteger(m_towoffTime);
	p_file.getInteger(m_exIntStandTime);
}
//---------------------------------------------------------------------------------------------
//Summary:
//		read latest version of file
//---------------------------------------------------------------------------------------------
void Flight::readFligt3_4( ArctermFile& p_file,StringDictionary* _pStrDict, CStartDate* _psDate /*= NULL*/ )
{
	baggageDevice.SetStrDict(_pStrDict);

	readFlightParameter(p_file,_pStrDict, FALSE, _psDate, WITH_FROMTO);

	ALTObjectID standID1;
	if( !standID1.readALTObjectID(p_file) ) 
		p_file.skipField(1);
	else 
	{
		m_pArrFlightOp->SetArrStand(standID1);
	}

	ALTObjectID standID2;
	if( !standID2.readALTObjectID(p_file) ) 
		p_file.skipField(1);
	else 
	{
		m_pDepFlightOp->SetDepStand(standID2);
	}

	ALTObjectID standID3;
	if( !standID3.readALTObjectID(p_file) ) 
		p_file.skipField(1);
	else 
	{
		//if (isTurnaround())
		m_IntermediateStand = standID3;
	}

	p_file.getTime (serviceTime);
	p_file.getTime(m_tArrParking);
	p_file.getTime(m_tDepParking);
	p_file.getTime(m_tIntParking);

	if (isTurnaround()&& m_tArrParking == 0L)
	{
		if (getIntermediateStand().IsBlank())
			setArrParkingTime(getDepTime()-getArrTime());
		else
		{
			setArrParkingTime(getDeplaneTime());
			setDepParkingTime(getEnplaneTime());
			setIntParkingTime(getDepTime()-getArrTime() - getDeplaneTime() - getEnplaneTime());
		}
	}
	else if (isArriving()&& m_tArrParking == 0L)
		setArrParkingTime(getServiceTime());
	else if (isDeparting()&& m_tDepParking == 0L)
		setDepParkingTime(getServiceTime());				

	ProcessorID arrGate;
	arrGate.SetStrDict( _pStrDict );
	arrGate.readProcessorID( p_file );
	m_ArrGate = arrGate;

	ProcessorID depGate;
	depGate.SetStrDict( _pStrDict );
	depGate.readProcessorID( p_file );
	m_DepGate = depGate;
	baggageDevice.readProcessorID(p_file);

	int nValue =-1;
	p_file.getInteger(nValue);
	if (nValue > 0)
		m_arrLFInput = (double)nValue/100;

	p_file.getInteger(nValue);
	if (nValue > 0)
		m_depLFInput = (double)nValue/100;

	p_file.getInteger(nValue);
	if (nValue > 0)
		m_capacityInput = nValue;

	//new add member
	p_file.getInteger(m_ataTime);
	p_file.getInteger(m_atdTime);
	p_file.getInteger(m_towoffTime);
	p_file.getInteger(m_exIntStandTime);
}

//------------------------------------------------------------------------------
//Summary:
//		new member get and set property
//------------------------------------------------------------------------------
void Flight::SetAtaTime( long tTime )
{
	m_ataTime = tTime;
}

long Flight::GetAtaTime() const
{
	return m_ataTime;
}

void Flight::SetAtdTime( long tTime )
{
	m_atdTime = tTime;
}

long Flight::GetAtdTime() const
{
	return m_atdTime;
}

void Flight::SetAirsideFlightDepartured( bool bDeparture )
{
	m_bAirsideDepartured = bDeparture;
}

bool Flight::IsAirsideFlightDepartured() const
{
	return m_bAirsideDepartured;
}

//---------------------------------------------------------------------------