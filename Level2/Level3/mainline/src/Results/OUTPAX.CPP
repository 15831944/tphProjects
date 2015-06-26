#include "stdafx.h"
#include "results\outpax.h"
#include "inputs\pax_cnst.h"
#include "results\fltlog.h"
#include "results\out_term.h"
#include "common\point.h"
#include "common\strdict.h"
#include "inputs\MobileElemTypeStrDB.h"
#include "engine\terminal.h"
/*****
*
*   General query functions
*
*****/

int OutputPaxEntry::fits (const CMultiMobConstraint& multiConst) const
{
    CMobileElemConstraint passengerType(NULL);
	if(  item.intrinsic == 4 )
	{
		// dep part
		int nDepFltIndex = getDepFlight();
		if( nDepFltIndex == -1 )
			return 0;
		
		FlightLogEntry flightLogDep;
		m_pOutTerm->flightLog->getItem( flightLogDep, nDepFltIndex );
		FlightDescStruct flightStructDep;
        flightLogDep.initStruct (flightStructDep);
        passengerType.initFlightType (flightStructDep, 'D');
		
		// arr part
		int nArrFltIndex = getArrFlight();
		if( nArrFltIndex == -1 )
			return 0; 
		
		FlightLogEntry flightLogArr;
		m_pOutTerm->flightLog->getItem( flightLogArr, nArrFltIndex );			
		FlightDescStruct flightStructArr;
		flightLogArr.initStruct( flightStructArr );
		passengerType.initOtherFlightType( flightStructArr ); // init arr part of the TRANSFER.
        passengerType.initMobType (item);
		
		if(this->getID() == 20143)
		{
			int i =0;
		
		}



        if (multiConst.isMatched (passengerType))
            return 1;
		
		//CString strMob;
		//passengerType.screenPrint(strMob);
		//std::ofstream file("C:\\mob.txt",std::ios::app);
		//file<<strMob<<"\n";
		//file.close();	
		
		return 0;
	}
	

    FlightLogEntry flightLog;
    FlightDescStruct flightStruct;
    int ndx = getArrFlight();

	assert( m_pOutTerm );
    if (ndx != -1)
    {
        m_pOutTerm->flightLog->getItem (flightLog, ndx);
		char fltID[256];
		flightLog.getFlightID(fltID);
		if(strcmp(fltID, "S7510") == 0)
		{
			int i =0;
		}
        flightLog.initStruct (flightStruct);
        passengerType.initFlightType (flightStruct, 'A');
        passengerType.initMobType (item);

		if(this->getID() == 20143)
		{
			int i =0;

		}
        if (multiConst.isMatched (passengerType))
            return 1;
	
		//CString strMob;
		//passengerType.screenPrint(strMob);
		//std::ofstream file("C:\\mob.txt",std::ios::app);
		//file<<strMob<<"\n";
		//file.close();
    }
    ndx = getDepFlight();
    if (ndx != -1)
    {
        m_pOutTerm->flightLog->getItem (flightLog, ndx);
		char fltID[256];
		flightLog.getFlightID(fltID);
		if(strcmp(fltID, "S7510") == 0)
		{
			int i =0;
		}
        flightLog.initStruct (flightStruct);
        passengerType.initFlightType (flightStruct, 'D');
        passengerType.initMobType (item);
        if (multiConst.isMatched (passengerType))
            return 1;
    }
    return 0;
}

int OutputPaxEntry::fits (const CMobileElemConstraint& aConst) const
{
    CMobileElemConstraint passengerType(NULL);

	if(  item.intrinsic == 4 )
	{
		// dep part
		int nDepFltIndex = getDepFlight();
		if( nDepFltIndex == -1 )
			return 0;

		FlightLogEntry flightLogDep;
		m_pOutTerm->flightLog->getItem( flightLogDep, nDepFltIndex );
		FlightDescStruct flightStructDep;
        flightLogDep.initStruct (flightStructDep);
        passengerType.initFlightType (flightStructDep, 'D');

		// arr part
		int nArrFltIndex = getArrFlight();
		if( nArrFltIndex == -1 )
			return 0; 

		FlightLogEntry flightLogArr;
		m_pOutTerm->flightLog->getItem( flightLogArr, nArrFltIndex );			
		FlightDescStruct flightStructArr;
		flightLogArr.initStruct( flightStructArr );
		passengerType.initOtherFlightType( flightStructArr ); // init arr part of the TRANSFER.

        passengerType.initMobType (item);
        if (aConst.fits (passengerType))
            return 1;
	
		return 0;
	}


    FlightLogEntry flightLog;
    FlightDescStruct flightStruct;
	
    int ndx = getArrFlight();

	assert( m_pOutTerm );
    if (ndx != -1)
    {
        m_pOutTerm->flightLog->getItem (flightLog, ndx);
        flightLog.initStruct (flightStruct);
        passengerType.initFlightType (flightStruct, 'A');
        passengerType.initMobType (item);
        if (aConst.fits (passengerType))
            return 1;
    }
    ndx = getDepFlight();
    if (ndx != -1)
    {
        m_pOutTerm->flightLog->getItem (flightLog, ndx);
        flightLog.initStruct (flightStruct);
        passengerType.initFlightType (flightStruct, 'D');
        passengerType.initMobType (item);
        if (aConst.fits (passengerType))
            return 1;
    }
    return 0;
}

void OutputPaxEntry::printMobile(char *p_str) const
{
	assert( m_pOutTerm );
	Terminal* pTerm = (Terminal*)m_pOutTerm;
	int iMobileType = item.mobileType;
	CString strType = pTerm->m_pMobElemTypeStrDB->GetString( iMobileType );
	strcpy( p_str, (LPCSTR) strType );	
}

void OutputPaxEntry::printType (char *p_str) const
{
	assert( m_pOutTerm );
    strcpy (p_str, m_pOutTerm->outStrDict->getString (item.intrinsic));
    for (int i = 0; i < MAX_PAX_TYPES && item.paxType[i] != EMPTY_ID; i++)
    {
        strcat (p_str, "-");
        strcat (p_str, m_pOutTerm->outStrDict->getString (item.paxType[i]));
    }
}

void OutputPaxEntry::printFullType (char *p_str) const
{
	assert( m_pOutTerm );
	//change by aivin
	if(1 == item.intrinsic)
	{
		m_pOutTerm->flightLog->getFlightID (p_str, getFlightIndex(), 'A');
	}
	else if( 2 == item.intrinsic)
	{
		m_pOutTerm->flightLog->getFlightID (p_str, getFlightIndex(), 'D');
	}
	else
	{
		m_pOutTerm->flightLog->getFlightID (p_str, getFlightIndex());
	}

    strcat (p_str, "(");
    printType (p_str + strlen (p_str));
    strcat (p_str, ")");
	////////////////////////
	strcat (p_str, "{");
	printMobile( p_str + strlen( p_str) );
	strcat (p_str, "}");
}

/*****
*
*   Element Track interface methods
*
*****/

Point PaxEvent::getPoint (void) const
{
    Point pnt;
    pnt.setPoint (event.x, event.y, event.z);
    return pnt;
}

ElapsedTime PaxEvent::getTime (void) const
{
    ElapsedTime t;
    t.setPrecisely (event.time);
    return t;
}



