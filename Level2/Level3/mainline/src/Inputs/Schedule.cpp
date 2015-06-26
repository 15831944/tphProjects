#include "stdafx.h"
#include "inputs\flight.h"
#include "inputs\schedule.h"
#include "inputs\in_term.h"
#include "inputs\fltdata.h"
#include "inputs\bagdata.h"
#include "common\category.h"
#include "inputs\acdata.h"
#include "engine\proclist.h"
#include "engine\process.h"
#include "results\fltlog.h"
#include "common\term_bin.h"
#include "common\termfile.h"
#include "common\exeption.h"
#include "..\common\AirportsManager.h"
#include "..\common\ACTypesManager.h"
#include "../Common/Aircraft.h"
#include "../InputAirside/ALTAirport.h"
#include "../InputAirside/ALTObject.h"
#include "../InputAirside/stand.h"
#include "../InputAirside/TowOffStandAssignmentData.h"
#include "../InputAirside/TowOffStandAssignmentDataList.h"
#include "../Common/ARCTracker.h"
#include "FLT_DB.H"
#include "InputAirside/TowoffStandStrategyContainer.h"

#define FLIGHT_SCHED_FILE_VER   200

// C based function wrappers to provide access to Delphi code
FlightSchedule::FlightSchedule() : UnsortedContainer<Flight> (32, 0, 32),DataSet (FlightScheduleFile, 3.5f ) 
{ 
	ownsElements(1); 
	m_pTowingData = new CTowOffStandAssignmentDataList();

}
//End Modify by adam 2007-04-24
FlightSchedule::~FlightSchedule() 
{
	if (m_pTowingData != NULL)
	{
		delete m_pTowingData;
		m_pTowingData = NULL;
	}

}

CTowOffStandAssignmentDataList* FlightSchedule::getTowOffDataList()
{
	return m_pTowingData;
}

void FlightSchedule::readData (ArctermFile& p_file)
{
	m_pTowingData->CleanData();
	m_pTowingData->SetAirportDatabase(m_pInTerm->m_pAirportDB);
	m_pTowingData->ReadData(m_pInTerm->m_nProjID);

	m_fileVersion = p_file.getVersion();
    Flight *newFlight;
	Flight::ResetUniqueID() ;
    while (p_file.getLine())
    {
        newFlight = new Flight( (Terminal*)m_pInTerm );
		assert( m_pInTerm );
		//Modify by adam 2007-04-24
        //newFlight->readFlight2_8 (p_file, m_pInTerm->inStrDict, &m_startDate );
		//Modify by emily 2008-5-23
		//newFlight->readFlight2_9(p_file, m_pInTerm->inStrDict, NULL );
		newFlight->readFligt3_5(p_file, m_pInTerm->inStrDict, NULL );
		
		addItem (newFlight);
    }
}

void FlightSchedule::sortFlights (void)
{
    SortedContainer<Flight> sortedList;
    for (int i = 0; i < getCount(); i++)
        sortedList.addItem (getItem (i));

    ownsElements (0);
    clear();
    ownsElements (1);
    for (int ii = 0; ii < sortedList.getCount(); ii++)
        addItem (sortedList.getItem (ii));
}
void FlightSchedule::sortFlights(SortedContainer<Flight>&flightSortSchedule)
{
	int *piOldSortBy = new int[getCount()];
	for (int i = 0; i < getCount(); i++)
	{
		piOldSortBy[i] =getItem(i)->getSortBy();
		getItem(i)->setSortBy(ArrTimeField);
        flightSortSchedule.addItem (getItem (i));
	}

    for (int ii = 0; ii <getCount(); ii++)
        getItem(ii)->setSortBy(piOldSortBy[ii]);
	delete []piOldSortBy;
}
void FlightSchedule::readObsoleteData (ArctermFile& p_file)
{
	m_pTowingData->CleanData();
// 	m_pTowingData->SetAirportDatabase(m_pInTerm->m_pAirportDB);
// 	m_pTowingData->ReadData(m_pInTerm->m_nProjID);
	TowoffStandStrategyContainer towoffStrategyCon;
	towoffStrategyCon.SetAirportDatabase(m_pInTerm->m_pAirportDB);
	towoffStrategyCon.ReadData(-1);
	TowoffStandStrategy* pTowoffStrategy = towoffStrategyCon.GetActiveStrategy();

	m_fileVersion = p_file.getVersion();
	if( p_file.getVersion() < 2.0 )
	{
	    SortedContainer<Flight> sortedList;
	    sortedList.ownsElements(0);

	    Flight *newFlight;
	    while (p_file.getLine())
	    {
	        newFlight = new Flight( (Terminal*)m_pInTerm );
	        newFlight->readVersion1_0 (p_file);
	        sortedList.addItem (newFlight);
	    }
	    for (int i = 0; i < sortedList.getCount(); i++)
	        addItem (sortedList.getItem (i));
	}
	else if(p_file.getVersion() <= 2.20001)
	{
		Flight *newFlight;
	    while (p_file.getLine())
	    {
	        newFlight = new Flight( (Terminal*)m_pInTerm );
			assert( m_pInTerm );
	        newFlight->readVersion2_2 ( p_file, m_pInTerm->inStrDict );
	        addItem (newFlight);
		}
	}
	else if (p_file.getVersion() <= 2.30001) //2.3
	{
		Flight *newFlight;
		while (p_file.getLine())
		{
			newFlight = new Flight( (Terminal*)m_pInTerm );
			assert( m_pInTerm );
			newFlight->readObsoleteFlight ( p_file, m_pInTerm->inStrDict );
			addItem (newFlight);
		}
	}
	else if (p_file.getVersion() <= 2.40001)
	{
		Flight*newFlight;
		while (p_file.getLine())
		{
			newFlight = new Flight( (Terminal*)m_pInTerm );
			assert( m_pInTerm );
			newFlight->readFlight2_4 ( p_file, m_pInTerm->inStrDict );
			addItem (newFlight);
		}
	}
	else if (p_file.getVersion() <= 2.50001)
	{
		Flight*newFlight;
		while (p_file.getLine())
		{
			newFlight = new Flight( (Terminal*)m_pInTerm );
			assert( m_pInTerm );
			newFlight->readFlight2_5 ( p_file, m_pInTerm->inStrDict );
			addItem (newFlight);
		}
	}
	else if (p_file.getVersion() <= 2.60001)
	{
		Flight*newFlight;
		while (p_file.getLine())
		{
			newFlight = new Flight( (Terminal*)m_pInTerm );
			assert( m_pInTerm );
			newFlight->readFlight2_6 (p_file, m_pInTerm->inStrDict, &m_startDate  );
			addItem (newFlight);
		}
	}
	else if (p_file.getVersion() < 2.700001)
	{
		Flight*newFlight;
		while (p_file.getLine())
		{
			newFlight = new Flight( (Terminal*)m_pInTerm );
			assert( m_pInTerm );
			newFlight->readFlight2_7 (p_file, m_pInTerm->inStrDict, &m_startDate  );
			addItem (newFlight);
		}
	}
	else if (p_file.getVersion() < 2.800001)
	{
		Flight *newFlight;
		while (p_file.getLine())
		{
			newFlight = new Flight( (Terminal*)m_pInTerm );
			assert( m_pInTerm );
			newFlight->readFlight2_8 (p_file, m_pInTerm->inStrDict, &m_startDate );
			addItem (newFlight);
		}
	}
	else if (p_file.getVersion() < 2.900001)
	{
		Flight *newFlight;
		while (p_file.getLine())
		{
			newFlight = new Flight( (Terminal*)m_pInTerm );
			assert( m_pInTerm );
			newFlight->readFlight2_9(p_file, m_pInTerm->inStrDict, NULL );
			addItem (newFlight);
		}
	}
	else if (p_file.getVersion() < 3.000001)
	{
		Flight *newFlight;
		while (p_file.getLine())
		{
			newFlight = new Flight( (Terminal*)m_pInTerm );
			assert( m_pInTerm );
			newFlight->readFlight3_0(p_file, m_pInTerm->inStrDict, NULL );
			addItem (newFlight);
		}
	}
	else if( p_file.getVersion() < 3.2f)
	{
		Flight* newFlight;
		while(p_file.getLine())
		{
			newFlight = new Flight( (Terminal*)m_pInTerm );
			assert( m_pInTerm );
			newFlight->readFlight3_1(p_file, m_pInTerm->inStrDict, NULL );

			if (newFlight->isTurnaround())
			{
				if (newFlight->getIntermediateStand().IsBlank())
					newFlight->setArrParkingTime(newFlight->getDepTime()-newFlight->getArrTime());
				else
				{
					newFlight->setArrParkingTime(newFlight->getDeplaneTime());
					newFlight->setDepParkingTime(newFlight->getEnplaneTime());
					newFlight->setIntParkingTime(newFlight->getDepTime()- newFlight->getArrTime() - newFlight->getDeplaneTime() - newFlight->getEnplaneTime());
				}
			}
			else if (newFlight->isArriving())
				newFlight->setArrParkingTime(newFlight->getServiceTime());
			else
				newFlight->setDepParkingTime(newFlight->getServiceTime());				

			addItem (newFlight); 
		}
	}
	else if (p_file.getVersion() < 3.3f)
	{
		Flight *newFlight;
		while (p_file.getLine())
		{
			newFlight = new Flight( (Terminal*)m_pInTerm );
			assert( m_pInTerm );
			newFlight->readFlight3_2(p_file, m_pInTerm->inStrDict, NULL );
			addItem (newFlight);
		}
	}
	else if (p_file.getVersion() < 3.4f)
	{
		Flight *newFlight;
		while (p_file.getLine())
		{
			newFlight = new Flight( (Terminal*)m_pInTerm );
			assert( m_pInTerm );
			newFlight->readFlight3_3(p_file, m_pInTerm->inStrDict, NULL );
			addItem (newFlight);
		}
	}
	else if (p_file.getVersion() < 3.5f)
	{
		Flight *newFlight;
		if (pTowoffStrategy == NULL)
			return;
		CTowOffStandAssignmentDataList* pTowoffStandDataList = pTowoffStrategy->GetTowoffStandDataList();
		if (pTowoffStandDataList == NULL)
			return;

		while (p_file.getLine())
		{
			newFlight = new Flight( (Terminal*)m_pInTerm );
			assert( m_pInTerm );
			newFlight->readFligt3_4(p_file, m_pInTerm->inStrDict, NULL );
			if (!newFlight->getIntermediateStand().IsBlank() && newFlight->GetTowoffTime() == -1l && newFlight->GetExIntStandTime() == -1l)
			{
				CTowOffStandAssignmentData* pFltData = pTowoffStandDataList->GetFlightTowoffData(newFlight,newFlight->getStand());
				if (pFltData)
				{
					ElapsedTime startTime;
					ElapsedTime endTime;
					if (newFlight->isArriving())
						startTime = newFlight->getArrTime();
					else
						startTime = newFlight->getDepTime() - newFlight->getServiceTime(); 

					startTime += ElapsedTime(pFltData->GetDeplanementTime()*60L);

					if (newFlight->isDeparting())
						endTime = newFlight->getDepTime();
					else
						endTime = newFlight->getArrTime() + newFlight->getServiceTime();

					endTime -=  ElapsedTime(pFltData->GetEnplanementTime()*60L);

					newFlight->SetTowoffTime(startTime.getPrecisely());
					newFlight->SetExIntStandTime(endTime.getPrecisely());
				}
			}
			addItem (newFlight);
		}
	}
}

void FlightSchedule::writeData (ArctermFile& p_file) const
{
    Flight *aFlight;
	assert( m_pInTerm );
    int flightCount = getCount();
    for (int i = 0; i < flightCount; i++)
    {
        aFlight = getItem (i);
        aFlight->writeFlight ( p_file, m_pInTerm->inStrDict, 1);
    }
}

const char* FlightSchedule::getHeaders (void) const
{
	static char titles[512] = {0};
    Flight::getFieldTitles(titles, TRUE);
    return titles;
}

void FlightSchedule::echoFlightSchedule (const char *p_filename) const
{
    ofsstream echoFile (p_filename);
    if (echoFile.bad())
        throw new FileOpenError (p_filename);

    char string[512];
    Flight::getFieldTitles (string);
    echoFile << "Arcterm Flight List\n\n" << string << '\n';

    Flight *aFlight;
    int flightCount = getCount();
	assert( m_pInTerm );
    for (int i = 0; i < flightCount; i++)
    {
        aFlight = getItem (i);

		//Modify by adam 2007-04-24
        //aFlight->printFlight (string, 0, m_pInTerm->inStrDict, &m_startDate );
		aFlight->printFlight (string, 0, m_pInTerm->inStrDict);
		//End Modify by adam 2007-04-24

        echoFile << string << '\n';
    }
    echoFile.close();
}

void FlightSchedule::writeBinarySchedule (FlightLog *p_list) const
{
	PLACE_METHOD_TRACK_STRING();
    Flight *flight;
    int flightCount = getCount();
    for (int i = 0; i < flightCount; i++)
    {
        flight = getItem (i);
        p_list->addItem (flight->getLogEntry());
    }
}

void FlightSchedule::swapFlights (int firstIndex, int secondIndex)
{
    Flight tempFlight = *(getItem (firstIndex));
    *(getItem (firstIndex)) = *(getItem (secondIndex));
    *(getItem (secondIndex)) = tempFlight;
}

// sets passenger loads and flight delays base on the random number
void FlightSchedule::updateFlights (const FlightData *data)
{
	PLACE_METHOD_TRACK_STRING();
    Flight *aFlight;
	FlightData* pFltData = const_cast<FlightData*>(data);
    int flightCount = getCount();
    for (int i = 0; i < flightCount; i++)
    {
        aFlight = getItem (i);
		FlightConstraintDatabaseWithSchedData* pData = (FlightConstraintDatabaseWithSchedData*)pFltData->getCapacity();
		if (aFlight->getCapacityInput() < 0 || pData->IsNeglectSchedData())
		{
			int nCap = data->getCapacity(aFlight);
			if (nCap >=0)
				aFlight->setCapacity(nCap);
			else
			{
				if(m_pInTerm)
				{
					char str[256];
					aFlight->getACType(str);
					CACType* pType = m_pInTerm->m_pAirportDB->getAcMan()->getACTypeItem(str);

					if (pType && pType->m_iCapacity >=0)
						aFlight->setCapacity(pType->m_iCapacity);
				}		
			}
		}
		else
			aFlight->setCapacity(aFlight->getCapacityInput());


		pData = (FlightConstraintDatabaseWithSchedData*)pFltData->getLoads();
        if (aFlight->isArriving())       
		{
			if (aFlight->getArrLFInput() <0 || pData->IsNeglectSchedData())
				data->setFlightLoadFactor (aFlight, 'A');
			else
				aFlight->setArrLoadFactor(aFlight->getArrLFInput());

            data->setFlightDelay (aFlight);
        }
        if (aFlight->isDeparting())		
		{
			if (aFlight->getDepLFInput() <0 || pData->IsNeglectSchedData())
				data->setFlightLoadFactor (aFlight, 'D');
			else
				aFlight->setDepLoadFactor(aFlight->getDepLFInput());

			data->setFlightDelay(aFlight);
		}


		//aFlight->getLogEntry().gateOccupancy = aFlight->getServiceTime();		//need modify
		//aFlight->getLogEntry().nArrTransit = 0;
		//aFlight->getLogEntry().nArrTransfer = 0;
		//aFlight->getLogEntry().nDepTransit = 0;
		//aFlight->getLogEntry().nDepTransfer = 0;
    }
}

void FlightSchedule::assignBaggageCarosels (const BaggageData *bagData,InputTerminal* _inTerm)
{
    Flight *aFlight;
    int flightCount = getCount();
    for (int i = 0; i < flightCount; i++)
    {
        aFlight = getItem (i);
		//set setBaggageCarosel
		if(!aFlight->getBaggageDevice().isBlank())
		{
			GroupIndex Index = m_pInTerm->procList->getGroupIndex(aFlight->getBaggageDevice());
			if(Index.start==Index.end)
				aFlight->setBaggageCarosel(Index.start);
			
		}
       // if (aFlight->isArriving())
       //            aFlight->setBaggageCarosel (bagData->getProcessor(i));
       
    }
}

void FlightSchedule::getFlightID (char *p_id, int p_ndx, char _flightMode ) const
{
    Flight *aFlight = getItem (p_ndx);
    aFlight->getFullID (p_id, _flightMode);
}

int FlightSchedule::getFlightIndex (const char *p_flight, char _flightMode ) const
{
    Flight *aFlight;
    char fullID[FULL_ID_LEN];
    int flightCount = getCount();
    for (int i = 0; i < flightCount; i++)
	{
        aFlight = getItem (i);

        if (_flightMode != 'D')
        {
            aFlight->getAirline (fullID);
            aFlight->getArrID (fullID + strlen (fullID));
            if (!_stricmp (fullID, p_flight))
                return i;
        }
        if (_flightMode != 'A')
        {
            aFlight->getAirline (fullID);
            aFlight->getDepID (fullID + strlen (fullID));
            if (!_stricmp (fullID, p_flight))
                return i;
        }
	}
	return -1;
}
Flight* FlightSchedule::GetFlightByFlightIndex(int Fli_Index)
{
	Flight *aFlight;
	int flightCount = getCount();
	for (int i = 0; i < flightCount; i++)
	{
		aFlight = getItem (i);
        if(aFlight->getFlightIndex() == Fli_Index)
			return aFlight ;
	}
	return NULL ;
}
void FlightSchedule::setFlight (const Flight *aFlight, int flightIndex)
{
    Flight *oldFlight = getItem(flightIndex);
    *oldFlight = *aFlight;
}

int FlightSchedule::getNextFlightSameCarrier (int index) const
{
    Flight *aFlight = getItem (index);
    char carrier[8], nextCarrier[8];
    aFlight->getAirline (carrier);

    Flight *nextFlight;
    int flightCount = getCount();
    for (int i = index+1; i < flightCount; i++)
    {
        nextFlight = getItem (i);
        nextFlight->getAirline (nextCarrier);

        if (!_stricmp (carrier, nextCarrier))
            return i;
    }
    return -1;
}

int FlightSchedule::getAllCarriersFlights (const char *carrier,
    int *indexes, ElapsedTime start, ElapsedTime end) const
{
    char nextCarrier[8];
    int count = 0;

    Flight *nextFlight;
    int flightCount = getCount();
    for (int i = 0; i < flightCount; i++)
    {
        nextFlight = getItem (i);
        nextFlight->getAirline (nextCarrier);

        if (!_stricmp (carrier, nextCarrier))
            if (nextFlight->getDepTime() >= start &&
                nextFlight->getArrTime() <= end)
                    indexes[count++] = i;
    }
    return count;
}

Flight *FlightSchedule::getRandomDeparture (ElapsedTime p_start, ElapsedTime p_end,int nArrFlightIndex) const
{
    UnsortedContainer<Flight> destList (64, 0, 64);
    Flight *aFlight;

    int flightCount = getCount();
    for (int i = 0; i < flightCount; i++)
    {
		if(i == nArrFlightIndex)//skip the given flight 
			continue;

        aFlight = getItem (i);
        if (aFlight->getDepTime() < p_end && aFlight->isDeparting() &&
            aFlight->getDepTime() > p_start && aFlight->getSpace ('D') > 0 )
                destList.addItem (aFlight);
    }

    int count = destList.getCount();
    return (count)? destList.getItem (random (count)): NULL;
}

Flight *FlightSchedule::getDeparture (ElapsedTime p_start, ElapsedTime p_end, const FlightConstraint &p_type,int nArrFlightIndex) const
{
    UnsortedContainer<Flight> destList (64, 0, 64);
    FlightConstraint flightType;
    Flight *aFlight;

    int flightCount = getCount();
    for (int i = 0; i < flightCount; i++)
    {
		if( i == nArrFlightIndex)//skip the given flight 
			continue;

        aFlight = getItem (i);

        if (aFlight->getDepTime() > p_end || !aFlight->isDeparting() ||
            aFlight->getDepTime() < p_start || aFlight->getSpace ('D') <= 0 )
                continue; //Jan98 //break;

        flightType = aFlight->getType ('D');
        if (p_type.fits (flightType))
            destList.addItem (aFlight);
    }

    int count = destList.getCount();
    return (count)? destList.getItem (random (count)): NULL;
}

/*****
*
*   Methods used for filling out flight selection tool
*
*****/

    
void FlightSchedule::getFlightList (StringList& p_strDict,
    const FlightConstraint &p_type)
{
    p_strDict.clear();

    // if no carrier is selected, no flights are listed
    char str[AIRLINE_LEN];
    p_type.getAirline (str);
    if (!str[0])
        return;

    // clear flightID, as flights should not block each other
    // filter is based on carrier, airport, sector, etc.
    FlightConstraint type (p_type);
	CFlightID emptyID;
	type.setFlightID( emptyID );

    Flight *aFlight = NULL;
	printf("%d",(int)this);
    int count = getCount();
    for (int i = 0; i < count; i++)
    {
        aFlight = getItem (i);
        if (aFlight->isArriving() && type.fits (aFlight->getType ('A')))
        {
            aFlight->getArrID (str);
            p_strDict.addString (str);
        }
        if (aFlight->isDeparting() && type.fits (aFlight->getType ('D')))
        {
            aFlight->getDepID (str);
            p_strDict.addString (str);
        }
    }
}


//void FlightSchedule::clearAllGates (InputTerminal* pInput)
//{
//	/////////////////////////////////////////////////////////////
//	int i,j;
//	//get all gate proc
//	ProcessorArray processArray;
//	pInput->procList->getProcessorsOfType( GateProc, processArray );
//	int nGateCount = processArray.getCount();
//	
//    ProcessorID id;     // constructor initializes to blank ID
//    Flight *aFlight;
//    int flightCount = getCount();
//    for (i = 0; i < flightCount; i++)
//    {
//        aFlight = getItem (i);
//		const ProcessorID& arrgate = aFlight->getArrGate();
//		for( j=0; j<nGateCount; j++)
//		{
//			if( *(processArray[j]->getID()) == arrgate )
//				break;
//		}
//		if( j>=nGateCount )
//			aFlight->setArrGate (id);	
//		const ProcessorID& dptgate = aFlight->getDepGate();
//		for( j=0; j<nGateCount; j++)
//		{
//			if( *(processArray[j]->getID()) == dptgate )
//				break;
//		}
//		if( j>=nGateCount )
//			aFlight->setDepGate(id);	
//    }
//}

//void FlightSchedule::clearGates (const ProcessorID& id)
//{
//    ProcessorID blankID;     // constructor initializes to blank ID
//    Flight *aFlight;
//    int flightCount = getCount();
//    for (int i = 0; i < flightCount; i++)
//    {
//        aFlight = getItem (i);
//        if (id.idFits (aFlight->getArrGate()))
//            aFlight->setArrGate(blankID);
//		if(id.idFits(aFlight->getDepGate()))
//			aFlight->setDepGate(blankID);
//    }
//}

//void FlightSchedule::getAllGates (StringDictionary& strDict) const
//{
//    char gateName[32];
//    ProcessorID id, blankID;     // constructor initializes to blank ID
//    Flight *aFlight;
//    int flightCount = getCount();
//    for (int i = 0; i < flightCount; i++)
//    {
//        aFlight = getItem (i);
//		id = aFlight->getArrGate();
//		if (!(id == blankID))
//		{
//			id.printID (gateName);
//			strDict.addString (gateName);
//		}
//		id = aFlight->getDepGate();
//		if (!(id == blankID))
//		{
//			id.printID (gateName);
//			strDict.addString (gateName);
//		}
//		//}
//}
bool FlightSchedule::IfUsedThisGate( const ProcessorID& _gateID )const
{
	Flight *aFlight;
	ProcessorID id, blankID;
    int flightCount = getCount();
    for (int i = 0; i < flightCount; i++)
    {
        aFlight = getItem (i);
        id = aFlight->getArrGate();
        if (!(id == blankID))
        {
            if( _gateID.idFits( id ) || id.idFits( _gateID ) )
			{
				return true;
			}
        } 
		id = aFlight->getDepGate();
		if (!(id == blankID))
		{
			if( _gateID.idFits( id ) || id.idFits( _gateID ) )
			{
				return true;
			}
		} 
    }

	return false;
}
//}
//
//int FlightSchedule::assignFlightToGate (Flight *p_flight,
//    const ProcessorID& p_gate, ElapsedTime p_buffer)
//{
//    int ndx = findItem (p_flight);
//    Flight *previousFlight = NULL, *nextFlight = NULL;
//
//    for (int i = ndx - 1; i >= 0; i--)
//    {
//        previousFlight = getItem (i);
//        if (previousFlight->getGate() == p_gate)
//            break;
//        previousFlight = NULL;
//    }
//
//    int flightCount = getCount();
//    for (i = ndx + 1; i < flightCount; i++)
//    {
//        nextFlight = getItem (i);
//        if (nextFlight->getGate() == p_gate)
//            break;
//        nextFlight = NULL;
//    }
//
//    if (previousFlight && ((previousFlight->getDepTime() + p_buffer)
//        > p_flight->getArrTime()))
//            return FALSE;
//
//    if (nextFlight && ((p_flight->getDepTime() + p_buffer) >
//        nextFlight->getArrTime()))
//            return FALSE;
//
//    p_flight->setGate (p_gate);
//    return TRUE;
//}

void FlightSchedule::initGateIndexes (const ProcessorList *procList )
{
	ASSERT(procList);
	//ASSERT(airsideProcList);

    Flight *aFlight;
    Processor *aProc;
    ProcessorID procID;

    int i;
    GroupIndex gateIndex;


	if (!procList->getProcessorsOfType (GateProc) /*&& 
		!airsideProcList->getProcessorsOfType (StandProcessor)*/)
		return;
	


    int flightCount = getCount();
    for (i = 0; i < flightCount; i++)
    {
        aFlight = getItem (i);
//        procID = aFlight->getGate();
//        if (!procID)
//            continue;
		BOOL bUsedAirsideStand = FALSE;
//		//////////////////////////////////////////////////////////////////////////
//		// stand gate
////		// // TRACE("\n%s\n",procID.GetIDString());
//        gateIndex = procList->getGroupIndex (procID);
//
//		if (gateIndex.start != gateIndex.end ||
//			gateIndex.start < 0)
//		{
//			gateIndex = airsideProcList->getGroupIndex(procID);
//
//			if (gateIndex.start != gateIndex.end)
//				throw new StringError ("Flights must be assigned to a single gate");
//
//			if (gateIndex.start < 0)
//				throw new StringError ("Flight assigned to an unknown gate");
//			bUsedAirsideStand = TRUE;
//		}
//
//		if (bUsedAirsideStand)
//			aProc = airsideProcList->getProcessor (gateIndex.start);
//		else
//			aProc = procList->getProcessor(gateIndex.start);
//        
//
//		//char strText[200];
//		//aProc->getID()->printID(strText);
//		//// // TRACE("ProcID: %s\n", strText);
//
//
//		int nProcessorType = aProc->getProcessorType();
//		if ((bUsedAirsideStand==TRUE && nProcessorType != StandProcessor) ||
//			(bUsedAirsideStand==FALSE && nProcessorType != GateProc))
//
//			throw new StringError ("A flight's gate must be a gate processor");
//
//        aFlight->setGateIndex (gateIndex.start);  
//		float fX;
//		float fY;
//		short iZ;
//		aProc->serviceLocationPath()->getPoint(0).getPoint( fX,fY,iZ );
//		aFlight->setACStandGatePos( fX,fY,iZ );		

		//////////////////////////////////////////////////////////////////////////
		// arr gate
		//Maybe used Stand 
		procID = aFlight->getArrGate();
//		// // TRACE("ProcID: %s\n", procID.GetIDString());
		if( procID.isBlank() )	// if is All Processors
		{
			aFlight->setArrGateIndex( -1 );
		}
		else
		{
			//BOOL bUsedAirsideStand = FALSE;
			gateIndex = procList->getGroupIndex( procID );
			
			if (gateIndex.start != gateIndex.end ||
				gateIndex.start < 0)
			{
				/*gateIndex = airsideProcList->getGroupIndex(procID);
				if (gateIndex.start != gateIndex.end)
					throw new StringError ("Flights must be assigned to a single gate");

				if (gateIndex.start < 0)
				{
					static CString strErrorMsg;
					strErrorMsg = "Flight assigned to an unknown gate:" + procID.GetIDString();
					throw new StringError ( strErrorMsg );
				}

				bUsedAirsideStand = TRUE;*/
				ProcessorID procId;
				aFlight->setArrGate(procId);
			}
			else
			{
				aProc = procList->getProcessor(gateIndex.start);			

				int nProcessorType = aProc->getProcessorType();
				if (nProcessorType != GateProc)
					throw new StringError ("A flight's gate must be a gate processor");

				aFlight->setArrGateIndex( gateIndex.start );
			}
			
		}

		//////////////////////////////////////////////////////////////////////////
		// dep gate
		procID = aFlight->getDepGate();
///		// // TRACE("ProcID: %s\n", procID.GetIDString());
		if( procID.isBlank() )	// if is All Processors
		{
			aFlight->setDepGateIndex( -1 );
		}
		else
		{
			//BOOL bUsedAirsideStand = FALSE;
			gateIndex = procList->getGroupIndex( procID );

			if (gateIndex.start != gateIndex.end ||
				gateIndex.start < 0)
			{
				//gateIndex = airsideProcList->getGroupIndex(procID);

				//if (gateIndex.start != gateIndex.end)
				//	throw new StringError ("Flights must be assigned to a single gate");
	 
				//if (gateIndex.start < 0)
				//{
				//	throw new ActermException(OPENFILEERR,CString("initGateIndexes"));
				//	//throw new StringError ("Flight assigned to an unknown gate");
				//}
				//bUsedAirsideStand = TRUE;
				ProcessorID procId;
				aFlight->setDepGate(procId);
			}
			else 
			{
				aProc = procList->getProcessor(gateIndex.start);
				int nProcessorType = aProc->getProcessorType();
				if (nProcessorType != GateProc)
					throw new StringError ("A flight's gate must be a gate processor");
				aFlight->setDepGateIndex( gateIndex.start );
			}		
		}
    }
}
//return true mean want to modify  entry 
bool FlightSchedule::getUsedProcInfo(const ProcessorID&p_id,InputTerminal *_term,std::vector<CString>&_stringList)
{
	ProcessorID blankID;
    Flight *aFlight;
	bool nRet = false;
    for (int i = 0; i < getCount(); i++)
    {
        aFlight = getItem(i);
  //      if (p_id.idFits (aFlight->getGate()))
		//{
  //          CString sline=aFlight->getGate().GetIDString();
		//	sline += ": ";
		//	char fly[514];
		//	aFlight->getFullID(fly,aFlight->getFlightMode());
		//	sline += "As Gate of ";
		//	sline += fly;
		//	sline += " ;"; // flight id
		//	_stringList.push_back(sline);
  //          nRet = true;
		//}
		//else if(aFlight->getGate().idFits(p_id) && !aFlight->getGate().isBlank()) //
		//{
		//	if(!_term->HasBrother(p_id,aFlight->getGate()))
		//	{
		//		CString sline=aFlight->getGate().GetIDString();
		//		sline += ": ";
		//		char fly[514];
		//		aFlight->getFullID(fly,aFlight->getFlightMode());
		//		sline += "As Gate of ";
		//		sline += fly;
		//		sline += " ;"; // flight id
		//		_stringList.push_back(sline);
		//		nRet = true;
		//	}
		//}

		if(p_id.idFits(aFlight->getArrGate()))
		{
			CString sline=aFlight->getArrGate().GetIDString();
			sline += ": ";
			char fly[514];
			aFlight->getFullID(fly,aFlight->getFlightMode());
			sline += "As ArrivalGate of ";
			sline += fly;
			sline += " ;"; // flight id
			_stringList.push_back(sline);
            nRet = true;
		}
		else if(aFlight->getArrGate().idFits(p_id)&& !aFlight->getArrGate().isBlank()) //
		{
			if(!_term->HasBrother(p_id,aFlight->getArrGate()))
			{
				CString sline=aFlight->getArrGate().GetIDString();
				sline += ": ";
				char fly[514];
				aFlight->getFullID(fly,aFlight->getFlightMode());
				sline += "As ArrivalGate of ";
				sline += fly;
				sline += " ;"; // flight id
				_stringList.push_back(sline);
				nRet = true;
			}
		}

		
		if(p_id.idFits(aFlight->getDepGate()))
		{
			CString sline=aFlight->getDepGate().GetIDString();
			sline += ": ";
			char fly[514];
			aFlight->getFullID(fly,aFlight->getFlightMode());
			sline += "As DepartGate of ";
			sline += fly;
			sline += " ;"; // flight id
			_stringList.push_back(sline);
            nRet = true;
		}
		else if(aFlight->getDepGate().idFits(p_id)&& !aFlight->getDepGate().isBlank())
		{
			if(!_term->HasBrother(p_id,aFlight->getDepGate()))
			{
				CString sline=aFlight->getDepGate().GetIDString();
				sline += ": ";
				char fly[514];
				aFlight->getFullID(fly,aFlight->getFlightMode());
				sline += "As DepartGate of ";
				sline += fly;
				sline += " ;"; // flight id
				_stringList.push_back(sline);
				nRet = true;
			}
		}

		if(p_id.idFits(aFlight->getBaggageDevice()))
		{
			CString sline=aFlight->getBaggageDevice().GetIDString();
			sline += ": ";
			char fly[514];
			aFlight->getFullID(fly,aFlight->getFlightMode());
			sline += "As BaggageDevice of ";
			sline += fly;
			sline += " ;"; // flight id
			_stringList.push_back(sline);
            nRet = true;
		}
		else if(aFlight->getBaggageDevice().idFits(p_id)&& !aFlight->getBaggageDevice().isBlank())
		{
			if(!_term->HasBrother(p_id,aFlight->getBaggageDevice()))
			{
				CString sline=aFlight->getBaggageDevice().GetIDString();
				sline += ": ";
				char fly[514];
				aFlight->getFullID(fly,aFlight->getFlightMode());
				sline += "As BaggageDevice of ";
				sline += fly;
				sline += " ;"; // flight id
				_stringList.push_back(sline);
				nRet = true;
			}
		}
    }
	return nRet;
}

// removes and deallocates all entries that fit passed procID
void FlightSchedule::removeAllEntries (const ProcessorID& p_id,InputTerminal *_term)
{
    ProcessorID blankID;
    Flight *aFlight;

    for (int i = 0; i < getCount(); i++)
    {
        aFlight = getItem(i);
        /*if (p_id.idFits (aFlight->getGate()))
            aFlight->setGate (blankID);
		else if(aFlight->getGate().idFits(p_id)&& !aFlight->getGate().isBlank())
		{
			if(!_term->HasBrother(p_id,aFlight->getGate()))
			  aFlight->setGate (blankID);
		}*/
		if(p_id.idFits(aFlight->getArrGate()))
			aFlight->setArrGate(blankID);
		else if(aFlight->getArrGate().idFits(p_id)&& !aFlight->getArrGate().isBlank())
		{
			if(!_term->HasBrother(p_id,aFlight->getArrGate()))
				aFlight->setArrGate(blankID);
		}

		if(p_id.idFits(aFlight->getDepGate()))
			aFlight->setDepGate(blankID);
		else if(aFlight->getDepGate().idFits(p_id)&& !aFlight->getDepGate().isBlank())
		{
			if(!_term->HasBrother(p_id,aFlight->getDepGate()))
				aFlight->setDepGate(blankID);
		}

		if(p_id.idFits(aFlight->getBaggageDevice()))
			aFlight->setBaggageDevice(blankID);
		else if(aFlight->getBaggageDevice().idFits(p_id)&& !aFlight->getBaggageDevice().isBlank())
		{
			if(!_term->HasBrother(p_id,aFlight->getBaggageDevice()))
				aFlight->setBaggageDevice(blankID);
		}
    }
}

// finds all entries that fit passed oldID and changes them to newID
void FlightSchedule::replaceAllEntries (const ProcessorID& p_old,
    const ProcessorID& p_new,InputTerminal* _term)
{
    Flight *aFlight;

    for (int i = 0; i < getCount(); i++)
    {
        aFlight = getItem(i);
        /*if (p_old.idFits (aFlight->getGate()))
            aFlight->setGate (p_new);
		else if(aFlight->getGate().idFits(p_old)&& !aFlight->getGate().isBlank())
		{
			if(!aFlight->getGate().idFits(p_new))
			{
				if(!_term->HasBrother(p_old,aFlight->getGate()))
					aFlight->setGate (p_new);
			}
		}*/

		if(p_old.idFits(aFlight->getArrGate()))
			aFlight->setArrGate(p_new);
		else if(aFlight->getArrGate().idFits(p_old)&& !aFlight->getArrGate().isBlank())
		{
			if(!aFlight->getArrGate().idFits(p_new))
			{
				if(!_term->HasBrother(p_old,aFlight->getArrGate()))
					aFlight->setArrGate (p_new);
			}
		}
		
		if(p_old.idFits(aFlight->getDepGate()))
			aFlight->setDepGate(p_new);
		else if(aFlight->getDepGate().idFits(p_old)&& !aFlight->getDepGate().isBlank())
		{
			if(!aFlight->getDepGate().idFits(p_new))
			{
				if(!_term->HasBrother(p_old,aFlight->getDepGate()))
					aFlight->setDepGate (p_new);
			}
		}
		
		if(p_old.idFits(aFlight->getBaggageDevice()))
			aFlight->setBaggageDevice(p_new);
		else if(aFlight->getBaggageDevice().idFits(p_old)&& !aFlight->getBaggageDevice().isBlank())
		{
			if(!aFlight->getBaggageDevice().idFits(p_new))
			{
				if(!_term->HasBrother(p_old,aFlight->getBaggageDevice()))
					aFlight->setBaggageDevice (p_new);
			}
		}
    }
}

// create FlightArrDepEvent from flightschedule






//
int FlightSchedule::createFlightEvent( Terminal* _pTerm )
{
	assert( _pTerm );

	Flight *pFlight;
	int iEventNum = 0;

	for( int i=0; i<getCount(); i++ )
	{
		pFlight = getItem(i);
		if( pFlight->createFlightEvent( _pTerm ) )
			iEventNum += 2;
	}

	return iEventNum;
}

const CStartDate& FlightSchedule::GetStartDate() const
{
	return m_startDate;
}

const CStartDate& FlightSchedule::GetEndDate() const
{
	return m_EndDate;
}

void FlightSchedule::SetEndDate(const CStartDate &_date)
{
	m_EndDate = _date;
}

void FlightSchedule::SetStartDate(const CStartDate &_date)
{
	m_startDate = _date;
}

void FlightSchedule::SetUserStartTime(const ElapsedTime& _date)
{
	m_UserStartTime = _date;
}

const ElapsedTime& FlightSchedule::GetUserStartTime() const
{
	return m_UserStartTime;
}

void FlightSchedule::SetUserEndTime(const ElapsedTime& _date)
{
	m_UserEndTime = _date;
}

const ElapsedTime& FlightSchedule::GetUserEndTime() const
{
	return m_UserEndTime;
}

//add by adam 2007-05-31
void FlightSchedule::SetFlightScheduleEndTime(const ElapsedTime& estEndTime)
{
	m_estFlightScheduleEndTime = estEndTime;
}

ElapsedTime& FlightSchedule::GetFlightScheduleEndTime()
{
	return m_estFlightScheduleEndTime;
}
//End add by adam 2007-05-31

BOOL FlightSchedule::RegenerateSchedule(const CSimGeneralPara *pSimPara, FlightSchedule& fs)
{
	PLACE_METHOD_TRACK_STRING();
	if (!pSimPara)
		return FALSE;

	//FlightSchedule* pSchedule = new FlightSchedule;
	if (!m_startDate.IsAbsoluteDate())
	{
		//	no date
		long lStart = pSimPara->GetStartTime().asSeconds() - pSimPara->GetInitPeriod().asSeconds();
		//long lStartTime = 0;
		if (lStart < 0)
		{
			//lStartTime = -WholeDay;
			lStart = 0;
		}
		//lStart -= lStartTime;

		long lEnd = pSimPara->GetEndTime().asSeconds() + pSimPara->GetFollowupPeriod().asSeconds();
		//lEnd -= lStartTime;

		//	compute days
		int nDays = ElapsedTime(lEnd - lStart).asHours() / 24;
		if ((lEnd - lStart) > (nDays * WholeDay))
			nDays++;

		//	add flights
		
		int nComposite = 0;
		for (int i = 0; i < getCount(); i++)
		{
			if (NeedSimulateFlight(getItem(i), lStart, lEnd, 0, nComposite))
			{
				Flight* pFlight = new Flight((Terminal*)m_pInTerm);
				*pFlight = *getItem(i);
				fs.addItem(pFlight);

			}
		}
	}
	else
	{
		COleDateTime dtSimStart, dtSimEnd;	// absolute date time
		COleDateTimeSpan dtsInit, dtsFollowup;
		ElapsedTime et;
		CStartDate sDate;

		bool bAbsDate;
		COleDateTime date;
		int nDateIdx;
		COleDateTime time;

		sDate = pSimPara->GetStartDate();
		sDate.GetDateTime( pSimPara->GetStartTime(), bAbsDate, date, nDateIdx, time );
		assert( bAbsDate );
		dtSimStart = date;
		dtSimStart += COleDateTimeSpan( 0, time.GetHour(), time.GetMinute(), time.GetSecond() );

		et = pSimPara->GetInitPeriod();
		dtsInit.SetDateTimeSpan(0, et.asHours(), et.asMinutes() % 60, et.asSeconds() % 60);
		dtSimStart -= dtsInit;

		sDate = pSimPara->GetStartDate();
		sDate.GetDateTime( pSimPara->GetEndTime(), bAbsDate, date, nDateIdx, time );
		assert( bAbsDate );
		dtSimEnd = date;
		dtSimEnd += COleDateTimeSpan( 0, time.GetHour(), time.GetMinute(), time.GetSecond() );
		et = pSimPara->GetFollowupPeriod();
		dtsFollowup.SetDateTimeSpan(0, et.asHours(), et.asMinutes() % 60, et.asSeconds() % 60);
		dtSimEnd += dtsFollowup;

		// set the CStartDate in simflightshedule
		COleDateTime startDate( dtSimStart.GetYear(), dtSimStart.GetMonth(), dtSimStart.GetDay(), 0, 0, 0 );
		sDate.SetDate( startDate );
		fs.SetStartDate( sDate );



		sDate = fs.GetStartDate();
		for (int i = 0; i < getCount(); i++)
		{
			
			if (NeedSimulateFlight(getItem(i), dtSimStart, dtSimEnd))
			{
				Flight* pFlight = new Flight((Terminal*)m_pInTerm);
				*pFlight = *getItem(i);
				//	Modify flight time
				if (pFlight->isArriving())
				{
					sDate.GetRelativeTime(m_startDate.GetDate(), pFlight->getArrTime(), et);
					pFlight->setArrTime(et);
				}
				
				if (pFlight->isDeparting())
				{
					sDate.GetRelativeTime(m_startDate.GetDate(), pFlight->getDepTime(), et);
					pFlight->setDepTime(et);
				}
				fs.addItem(pFlight);

				char szFlightID[16];
				pFlight->getFlightID(szFlightID);
			}
		}
	}



	return TRUE;
}

BOOL FlightSchedule::NeedSimulateFlight(const Flight *_pFlight, long _lStart, long _lEnd, int _nDayIndex,
										int& _nOutput)
{
	BOOL bRet = FALSE;
	_nOutput = 0;
	
	if (_pFlight->isArriving())
	{
		if (((_pFlight->getArrTime().asSeconds() + _nDayIndex * WholeDay) >= _lStart) &&
			((_pFlight->getArrTime().asSeconds() + _nDayIndex * WholeDay) <= _lEnd))
		{
			_nOutput |= 1;
			bRet = TRUE;
		}
	}

	if (_pFlight->isDeparting())
	{
		if (((_pFlight->getDepTime().asSeconds() + _nDayIndex * WholeDay) >= _lStart) &&
			((_pFlight->getDepTime().asSeconds() + _nDayIndex * WholeDay) <= _lEnd))
		{
			_nOutput |= 2;
			bRet = TRUE;
		}
	}

	return bRet;
}

BOOL FlightSchedule::NeedSimulateFlight(const Flight *_pFlight, const COleDateTime &_lhs, const COleDateTime &_rhs)
{
	bool bAbsDate;
	COleDateTime date;
	int nDateIdx;
	COleDateTime time;
	if (_pFlight->isArriving())
	{
		m_startDate.GetDateTime(_pFlight->getArrTime(), bAbsDate, date, nDateIdx, time);
		assert( bAbsDate );
		date += COleDateTimeSpan( 0, time.GetHour(), time.GetMinute(), time.GetSecond() );
		if (! (( date >= _lhs) && ( date <= _rhs)) )
			return FALSE;
	}

	if (_pFlight->isDeparting())
	{
		m_startDate.GetDateTime(_pFlight->getDepTime(), bAbsDate, date, nDateIdx, time);
		assert( bAbsDate );
		date += COleDateTimeSpan( 0, time.GetHour(), time.GetMinute(), time.GetSecond() );
		if (! (( date >= _lhs) && ( date <= _rhs)) )
			return FALSE;
	}

	return TRUE;
}

void FlightSchedule::loadDataSet (const CString& _pProjPath)
{
	clear();

	char filename[_MAX_PATH];
	PROJMANAGER->getFileName (_pProjPath, fileType, filename);

	ArctermFile file;
	try { file.openFile (filename, READ); }
	catch (FileOpenError *exception)
	{
		delete exception;
		initDefaultValues();
		saveDataSet(_pProjPath, false);
		return;
	}

	if (file.getVersion() < 2.800001)
	{
		CStartDate szDate ;
		bool flg = false;
		int co = 0;
		while (file.getLine())
		{
			if( GetMinStartDateFromFile(file, szDate) )
			{
				co ++;
				if(!flg)
				{
					flg = true;
					m_startDate = szDate;
					m_EndDate = szDate;
				}
				if( szDate.GetDate() < m_startDate.GetDate() )
					m_startDate = szDate;
				else if( szDate.GetDate() > m_EndDate.GetDate() )
					m_EndDate = szDate;
			}
		}	
		if( flg )
		{
			m_startDate.SetDate( m_startDate.GetDate());
			m_EndDate.SetDate( m_EndDate.GetDate());
			m_startDate.SetAbsoluteDate(true);
		}
	}
	//file version is more than 2.8
	else
	{
		CStartDate benckMarkDate;
		COleDateTime curDateTime = COleDateTime::GetCurrentTime();
		benckMarkDate.SetDate(curDateTime);
		benckMarkDate.SetAbsoluteDate(true);

		bool flag = false;
		while (file.getLine())
		{
			CStartDate szDate;
			CStartDate szEndDate;
			ElapsedTime estFltScheduleEndTime;
			
			if( GetStartDateEndDateFromFileFor2_9Versioin(file, benckMarkDate, szDate, szEndDate, estFltScheduleEndTime) )
			{

				if (!flag)
				{
					flag = true;
					m_startDate = szDate;
					m_EndDate   = szEndDate;
					m_estFlightScheduleEndTime = estFltScheduleEndTime;
				}

				//set min start date
				if( szDate.GetDate() < m_startDate.GetDate())
				{
					m_startDate = szDate;
				}		

				if(szEndDate.GetDate() < m_startDate.GetDate())
				{
					m_startDate = szEndDate;
				}

				//set max end date
				if (m_EndDate.GetDate() < szDate.GetDate())
				{
					m_EndDate = szDate;
				}
				
				if (m_EndDate.GetDate() < szEndDate.GetDate())
				{
					m_EndDate = szEndDate;
				}

				if (m_estFlightScheduleEndTime < estFltScheduleEndTime)
				{
					m_estFlightScheduleEndTime = estFltScheduleEndTime;
				}
			}
		}

		m_estFlightScheduleEndTime.SetHour(23);
		m_estFlightScheduleEndTime.SetMinute(59);
		m_estFlightScheduleEndTime.SetSecond(59);

		m_startDate.SetAbsoluteDate(false);
		m_EndDate.SetAbsoluteDate(false);
	}

	file.seekToLine(3);

	float fVersionInFile = file.getVersion();

	if( fVersionInFile < m_fVersion || fVersionInFile == 21 )
	{
		readObsoleteData( file );
		file.closeIn();
		saveDataSet(_pProjPath, false);
	}
	else if( fVersionInFile > m_fVersion )
	{
		// should stop read the file.
		file.closeIn();
		throw new FileVersionTooNewError( filename );		
	}
	else
	{
		readData (file);
		file.closeIn();
	}
}

void FlightSchedule::updateScheduleEndDay()
{
	Flight *aFlight = NULL;
	int flightCount = getCount();
	ElapsedTime arrTime, depTime;
	m_estFlightScheduleEndTime = depTime;
	for (int i = 0; i < flightCount; i++)
	{
		aFlight = getItem (i);
		arrTime = aFlight->getArrTime();
		depTime = aFlight->getDepTime();
		depTime = max(arrTime,depTime);
		m_estFlightScheduleEndTime = max(m_estFlightScheduleEndTime,depTime);
	}
}

bool FlightSchedule::GetMinStartDateFromFile(ArctermFile& p_file, CStartDate& psDate)
{
	if( p_file.getVersion() < 2.5 )
		return false;

	char szDate[16];
	int nNum[3];
	int i = 0;

	ElapsedTime aTime;
	ElapsedTime t;

	char szBuffer[1024];
	p_file.getField (szBuffer, AIRLINE_LEN);

	// arrival info
	if (p_file.isBlankField())
	{
		p_file.skipField(1);
	}
	else
	{
		p_file.getField(szBuffer, FLIGHT_LEN);
	}

	p_file.getField (szBuffer, AIRPORT_LEN);//origin
	if(p_file.getVersion() >= 2.51)
		p_file.getField(szBuffer, AIRPORT_LEN); //from

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

			psDate.SetDate(nNum[0], nNum[1], nNum[2]);

			return true;
		}
	}
	else
		p_file.skipField(1);

	p_file.getTime (aTime);

	// depature info
	if (p_file.isBlankField())
	{		
		p_file.skipField(1);
	}
	else
	{	
		p_file.getField(szBuffer, FLIGHT_LEN);
	}

	p_file.getField (szBuffer, AIRPORT_LEN);//to
	if(p_file.getVersion() >= 2.51)
		p_file.getField (szBuffer, AIRPORT_LEN);//destination

	i = 0;
	if (!p_file.isBlankField())
	{
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

			nNum[i] = atoi(strDate.Mid(nPrevPos, strDate.GetLength() - nPrevPos));

			psDate.SetDate(nNum[0], nNum[1], nNum[2]);

			return true;
		}
	}	

	return false;
}

//add by adam 2007-05-29
bool FlightSchedule::GetStartDateEndDateFromFileFor2_9Versioin(ArctermFile& p_file, CStartDate& pBenchMarkDate, 
															   CStartDate& psDate, CStartDate& pEndDate, 
															   ElapsedTime& estFlightScheduleEndTime)
{
	//if version is small than 2.9
	if( p_file.getVersion() < 2.900000001 )
		return false;

	char szBuffer[1024];
	p_file.getField (szBuffer, AIRLINE_LEN);

	// arrival info
	if (p_file.isBlankField())
	{
		p_file.skipField(1);
	}
	else
	{
		p_file.getField(szBuffer, FLIGHT_LEN);
	}

	p_file.getField (szBuffer, AIRPORT_LEN);//origin
	if(p_file.getVersion() >= 2.51)
		p_file.getField(szBuffer, AIRPORT_LEN); //from

	ElapsedTime estArrivalTime;
	p_file.getTime (estArrivalTime);
	
	COleDateTime dateTime;
	COleDateTime errorTime;

	bool bAbsolute = true;
	int nDate = 0;
	pBenchMarkDate.GetDateTime(estArrivalTime, bAbsolute, dateTime, nDate, errorTime);
	ASSERT(bAbsolute);
	psDate.SetDate(dateTime);

	// depature info
	if (p_file.isBlankField())
	{		
		p_file.skipField(1);
	}
	else
	{	
		p_file.getField(szBuffer, FLIGHT_LEN);
	}

	p_file.getField (szBuffer, AIRPORT_LEN);//to
	if(p_file.getVersion() >= 2.51)
		p_file.getField (szBuffer, AIRPORT_LEN);//destination

	ElapsedTime estDepTime;
	p_file.getTime(estDepTime);

	if (estDepTime < estArrivalTime)
	{
		estDepTime = estArrivalTime;
	}
	estFlightScheduleEndTime = estDepTime;

	bAbsolute = true;
	nDate = 0;
	pBenchMarkDate.GetDateTime(estDepTime, bAbsolute, dateTime, nDate, errorTime);
	ASSERT(bAbsolute);
	pEndDate.SetDate(dateTime);

	return true;
}

void FlightSchedule::initStandIndexes(int nAirportID , int nPrjID)
{
	//std::vector<ALTObject> objList;
	//ALTObject::GetObjectList(ALT_STAND,nAirportID,objList);
	ALTObjectList objList;
	ALTAirport::GetStandList(nAirportID,objList);

	Flight *aFlight;
	int flightCount = getCount();
	for (int i = 0; i < flightCount; i++)
	{
		aFlight = getItem (i);

		BOOL gotArrStand = FALSE;
		BOOL gotDepStand = FALSE;
		CString strArrID = aFlight->getArrStand().GetIDString();
		CString strDepID = aFlight->getDepStand().GetIDString();

		size_t j = 0 ;
		for(; j< objList.size();++j)
		{
			if( 0==strArrID.CompareNoCase( objList[j]->GetObjNameString() ) )
			{
				gotArrStand = TRUE;
				break;
			}
		}
		if(gotArrStand)
		{
			aFlight->setArrStandID( objList[j]->getID() );	
			Stand stand;	
			stand.ReadObject(aFlight->getArrStandID());

			//CPoint2008 p = stand.GetServicePoint();
			//aFlight->setACStandGatePos( (short)p.getX(),(short)p.getY(),(short)p.getZ() );		
			//aFlight->setArrStandID( stand.getID() );			
		}
		for(j = 0 ; j< objList.size();++j)
		{
			if( 0==strDepID.CompareNoCase( objList[j]->GetObjNameString() ) )
			{
				gotDepStand = TRUE;
				break;
			}
		}
		if(gotDepStand)
		{
			aFlight->setDepStandID( objList[j]->getID() );	
			Stand stand;	
			stand.ReadObject(aFlight->getDepStandID());

			//CPoint2008 p = stand.GetServicePoint();
			//aFlight->setACStandGatePos( (short)p.getX(),(short)p.getY(),(short)p.getZ() );		
			//aFlight->setArrStandID( stand.getID() );			
		}


	}	

}

void FlightSchedule::echo(EchoLogType key) const
{
	if (ECHOLOG->IsEnable() && ECHOLOG->IsLogEnable(key))
	{
		Flight *aFlight;
		for (int i = 0; i < getCount(); i++)
		{
			aFlight = getItem (i);
			aFlight->echo( key );
		}
	}
}
