#include "stdafx.h"
#include "fltdata.h"
#include "in_term.h"
#include "schedule.h"
#include "acdata.h"
#include "flight.h"
#include "../Common/ProbabilityDistribution.h"
#include "../Common/elaptime.h"
#include "../Common/termfile.h"
#include "../engine/boardcal.h"
#include "../Common/exeption.h"
#include "../Common/actypesmanager.h"


char szStageLabel[] = "BOARDING_CALL_STAGE_SIZE";


FlightData::~FlightData ()
{
	int nSize = m_vCalls.size();
	for( int i=0; i<nSize; i++ )
	{
		delete m_vCalls[i];
		m_vCalls[i] =  NULL;
	}
	m_vCalls.clear(); 
}


void FlightData::initDefaultValues (void)
{
    ProbabilityDistribution *defaultDist;
    ConstraintEntry *anEntry;

    // init default delay distribution
    defaultDist = new ConstantDistribution (0);
    anEntry = new ConstraintEntry;
    anEntry->initialize (new FlightConstraint, defaultDist);
    delays.addEntry (anEntry);

    // init default load distribution
    defaultDist = new ConstantDistribution (100);
    anEntry = new ConstraintEntry;
    anEntry->initialize (new FlightConstraint, defaultDist);
    loads.addEntry (anEntry);

    // init default capacity distribution
    //defaultDist = new ConstantDistribution (0);
    //anEntry = new ConstraintEntry;
    //anEntry->initialize (new FlightConstraint, defaultDist);
    //capacity.addEntry (anEntry);

    // init default boarding calls distribution
	AddStage();
}

void FlightData::readData (ArctermFile& p_file)
{
	assert( m_pInTerm );
	int nStageNumber = ReadBoardingCallStageNumber( p_file );

	delays.readDatabase (p_file, "DELAY", m_pInTerm );
	loads.readDatabase (p_file, "LOAD", m_pInTerm );
	capacity.readDatabase (p_file, "CAPACITY", m_pInTerm );

	// read boarding call
	int nSize = m_vBoardingCallStages.size();
	if( nStageNumber == 0 && nSize == 0 )
	{
		AddStage();
		return;
	}
	std::vector<FlightConWithProcIDDatabase*> vCalls;
	for( int i=0; i<nStageNumber; i++ )
	{
		if( vCalls.size() <= (unsigned)i )
		{
			// need to new 
			FlightConWithProcIDDatabase* pStage = new FlightConWithProcIDDatabase();
			vCalls.push_back( pStage );
		}
		char szLabel[128];
		sprintf( szLabel, "BOARDING_CALLS_STAGE%d", i+1 );
		vCalls[i]->readDatabase (p_file, szLabel, m_pInTerm );
	}
}

void FlightData::readObsoleteData (ArctermFile& p_file)
{	
	assert( m_pInTerm );

	float fVersion = p_file.getVersion();
	if( fVersion < (float)(2.2 - 0.00001) )
	{
		clear();
		initDefaultValues();
		delays.readDatabase (p_file, "DELAY", m_pInTerm );
		capacity.readDatabase (p_file, "LOAD", m_pInTerm);
		if( p_file.getVersion() >= 2.0 )
			capacity.readDatabase (p_file, "CAPACITY", m_pInTerm );
		std::vector<FlightConWithProcIDDatabase*> vCalls;
		vCalls[0]->readObsoleteDatabase( p_file, "BOARDING_CALLS", m_pInTerm );
	}
	else if((float)(2.3-0.00001) < fVersion && fVersion < (float)(2.3+0.00001)) // this is a error version
	{
		clear();
		initDefaultValues();
		int nStageNumber = ReadBoardingCallStageNumber( p_file );

		delays.readErrorDatabase (p_file, "DELAY", m_pInTerm );
		loads.readErrorDatabase (p_file, "LOAD", m_pInTerm );
		capacity.readErrorDatabase (p_file, "CAPACITY", m_pInTerm );

		FlightConWithProcIDDatabase pStage;
		for(int i=0; i<nStageNumber; i++)
		{
			char szLabel[128];
			sprintf(szLabel, "BOARDING_CALLS_STAGE%d", i+1);
			pStage.readErrorDatabase (p_file, szLabel, m_pInTerm);
			AddStage();
			m_vBoardingCallStages[i]->
		}
	}
	else if(fVersion < (float)(2.4+0.00001))		//2.20 , 2.40
	{
		clear();
		initDefaultValues();
		int nStageNumber = ReadBoardingCallStageNumber( p_file );
		
		delays.readDatabase (p_file, "DELAY", m_pInTerm );
		loads.readDatabase (p_file, "LOAD", m_pInTerm );
		capacity.readDatabase (p_file, "CAPACITY", m_pInTerm );
		
		// read boarding call
		int nSize = m_vBoardingCallStages.size();
		if( nStageNumber == 0 && nSize == 0 )
		{
			AddStage();
			return;
		}
		
		std::vector<FlightConWithProcIDDatabase> vStages;
		for( int i=0; i<nStageNumber; i++ )
		{
			FlightConWithProcIDDatabase stage;
			vStages.push_back(stage);
			char szLabel[128];
			sprintf( szLabel, "BOARDING_CALLS_STAGE%d", i+1 );
			vStages[i].readObsoleteDatabase(p_file, szLabel, m_pInTerm );
		}
	}
	else if ((float)(2.5-0.00001) < fVersion && fVersion<(float)(2.5+0.00001))
	{
		int nStageNumber = ReadBoardingCallStageNumber( p_file );

		delays.readDatabase (p_file, "DELAY", m_pInTerm );
		loads.readObsoleteDatabase (p_file, "LOAD", m_pInTerm );
		capacity.readObsoleteDatabase (p_file, "CAPACITY", m_pInTerm );

		// read boarding call
		int nSize = m_vBoardingCallStages.size();
		if( nStageNumber == 0 && nSize == 0 )
		{
			AddStage();
			return;
		}
		std::vector<FlightConWithProcIDDatabase*> vCalls;
		for( int i=0; i<nStageNumber; i++ )
		{
			if( vCalls.size() <= (unsigned)i )
			{
				// need to new 
				FlightConWithProcIDDatabase* pStage = new FlightConWithProcIDDatabase();
				vCalls.push_back( pStage );
			}
			char szLabel[128];
			sprintf( szLabel, "BOARDING_CALLS_STAGE%d", i+1 );
			vCalls[i]->readDatabase (p_file, szLabel, m_pInTerm );
		}
	}
	else if ((float)(2.6-0.00001) < fVersion && fVersion<(float)(2.6+0.00001))
	{
		assert( m_pInTerm );
		int nStageNumber = ReadBoardingCallStageNumber( p_file );

		delays.readDatabase (p_file, "DELAY", m_pInTerm );
		loads.readDatabase (p_file, "LOAD", m_pInTerm );
		capacity.readDatabase (p_file, "CAPACITY", m_pInTerm );

		// read boarding call
		int nSize = m_vBoardingCallStages.size();
		if( nStageNumber == 0 && nSize == 0 )
		{
			AddStage();
			return;
		}
		std::vector<FlightConWithProcIDDatabase*> vCalls;
		for( int i=0; i<nStageNumber; i++ )
		{
			if( vCalls.size() <= (unsigned)i )
			{
				// need to new 
				FlightConWithProcIDDatabase* pStage = new FlightConWithProcIDDatabase();
				vCalls.push_back( pStage );
			}
			char szLabel[128];
			sprintf( szLabel, "BOARDING_CALLS_STAGE%d", i+1 );
			vCalls[i]->readDatabase (p_file, szLabel, m_pInTerm );
		}
	}
	else
	{
		readData( p_file );
	}
}

void FlightData::writeData (ArctermFile& p_file) const
{
	assert( m_pInTerm );
	int nSize = m_vCalls.size();
	WriteBoardingCallStageNumber( p_file, nSize );
    delays.writeDatabase (p_file, "DELAY", m_pInTerm);
    loads.writeDatabase (p_file, "LOAD", m_pInTerm);
    capacity.writeDatabase (p_file, "CAPACITY", m_pInTerm);
	// write boarding call
	for( int i=0; i<nSize; i++ )
	{
		char szLabel[128];
		sprintf( szLabel, "BOARDING_CALLS_STAGE%d", i+1 );
	    m_vCalls[i]->writeDatabase( p_file, szLabel, m_pInTerm );
	}
}

void FlightData::setFlightDelay (Flight *p_flight) const
{
    FlightConstraint fltType;
	static int flag = 0;

	if(p_flight->isArriving() && !p_flight->isDeparting())
	{
		fltType = p_flight->getType('A');
	}
	else if(!p_flight->isArriving() && p_flight->isDeparting())
	{
		fltType = p_flight->getType('D');
	}
	else
	{
		if(flag == 0)
		{
			fltType = p_flight->getType('A');
			flag = 1;
		}
		else
		{
			fltType = p_flight->getType('D');
			flag = 0;
		}
	}

    const ProbabilityDistribution *prob = delays.lookup (fltType);
    p_flight->setDelay ((prob)? (ElapsedTime) (prob->getRandomValue()
        * 60.0): (ElapsedTime) 0l);
	
	if(p_flight->isArriving() && !p_flight->isDeparting())
	{
		p_flight->setArrDelay((prob)? (ElapsedTime) (prob->getRandomValue() 
			* 60.0): (ElapsedTime) 0l);
		p_flight->setDepDelay(0l);
	}
	else if(!p_flight->isArriving() && p_flight->isDeparting())
	{
		p_flight->setArrDelay(0l);
		p_flight->setDepDelay((prob)? (ElapsedTime) (prob->getRandomValue() 
			* 60.0): (ElapsedTime) 0l);
	}
	else
	{
		if(flag == 1)//next time,dep,current ,arr
			p_flight->setArrDelay((prob)? (ElapsedTime) (prob->getRandomValue() 
			* 60.0): (ElapsedTime) 0l);
		if(flag == 0)//next time,arr, current,dep
			p_flight->setDepDelay((prob)? (ElapsedTime) (prob->getRandomValue()
			* 60.0): (ElapsedTime) 0l);
	}
}

// "LOAD" evaluates to a percentage of the capacity
void FlightData::setFlightLoadFactor (Flight *p_flight, char p_mode) const
{
    FlightConstraint fltType = p_flight->getType (p_mode);
    const ProbabilityDistribution *prob = loads.lookup (fltType);

    //int cap = getCapacity (p_flight);

    float percent = (float)((prob)? (prob->getRandomValue()/100.0): 1.0);
	if (p_mode == 'A')
		p_flight->setArrLoadFactor( percent);
	if (p_mode == 'D')
		p_flight->setDepLoadFactor( percent); 
    //p_flight->setLoad ((int)(percent * (float)cap), p_mode);
	//p_flight->setCapacity(cap) ;			
	//
}

int FlightData::getCapacity (Flight *p_flight) const
{
	assert( m_pInTerm );

	char p_mode;
	if(p_flight->isArriving())
		p_mode = 'A';
	else
		p_mode = 'D';

    FlightConstraint fltType = p_flight->getType(p_mode);
    const ProbabilityDistribution *prob = capacity.lookup (fltType);
    if (!prob)
    {
        char str[AC_TYPE_LEN];
        p_flight->getACType (str);
        CACType acType, *anAircraft;
        acType.setIATACode(str);
        int ndx = _g_GetActiveACMan( m_pInTerm->m_pAirportDB )->findACTypeItem( &acType );
        if (ndx == INT_MAX)
        {
//            cerr << "\nNo capacity defined for ac type " << str;
            return 0;
        }

        anAircraft = _g_GetActiveACMan( m_pInTerm->m_pAirportDB )->getACTypeItem (ndx);
        return anAircraft->getDefaultCapacity();
    }
    else
        return (int)prob->getRandomValue();
}

const HistogramDistribution *FlightData::getBoardingCalls
    (const FlightConstraint &p_type,  const ProcessorID& _id, int _nStageNumber ) const
{
    const ProbabilityDistribution *prob = m_vCalls[_nStageNumber-1]->lookup_with_procid (p_type, _id);
	if(prob)
	{
	 
	 if(prob->getProbabilityType() == HISTOGRAM || prob->getProbabilityType() == EMPIRICAL )
	 {
		 return (const HistogramDistribution *)prob;
	 }
	}
	return NULL;
	// return ( prob && (prob->getProbabilityType() == HISTOGRAM || prob->getProbabilityType() == EMPIRICAL )) ? (const HistogramDistribution *)prob : NULL;

}



// go through all the file get the size of boardina call stage size;
int FlightData::ReadBoardingCallStageNumber( ArctermFile& _file )
{
	_file.reset();
	_file.skipLines (3);
	
	char str[64];
	while( _file.getLine() )
	{
		_file.getField (str, SMALL);
        if( strcmp ( str, szStageLabel ) == 0 )
		{
			int nNumStages;
			_file.getInteger( nNumStages );
			return nNumStages;
		}
	}
	return 0;
}


void FlightData::WriteBoardingCallStageNumber( ArctermFile& _file, int _nNumStages ) const
{
	_file.writeField( szStageLabel );
	_file.writeInt( _nNumStages );
	_file.writeLine();
}



void FlightData::clear() 
{ 
	delays.clear(); 
	loads.clear();
	capacity.clear(); 
	int nSize = m_vBoardingCallStages.size();
	for( int i=0; i<nSize; i++ )
	{
		delete m_vBoardingCallStages[i];
		m_vBoardingCallStages[i] =  NULL;
	}
	m_vBoardingCallStages.clear(); 
}


/*  Add one stage with default value

    Flight Type: DEFAULT
          |
          | Stand group: All Processors
                 |			
                 |Passenger Type : DEFAULT
                        Number of triggers: 4
                                 |
                                 |Trigger1
                                 |   |Time:
                                 |   |Prop:
                                 |
                                 |Trigger2
								 |   |Time:
								 |   |Prop:
								 |
								 |Trigger3
								 |   |Time:
								 |   |Prop:
								 |
								 |Trigger4
								     |Time:
								     |Prop:
*/
void FlightData::AddStage()
{
	BoardingCallFlightTypeDatabase* pFlightTypeDB = new BoardingCallFlightTypeDatabase();
	m_vBoardingCallStages.push_back(pFlightTypeDB);
	AddFlight(pFlightTypeDB, NULL);/*add a default flight constraint entry.*/
	return;
}

void FlightData::AddFlight(BoardingCallFlightTypeDatabase* pFlightTypeDB, FlightConstraint* pFlight)
{
	FlightConstraint* pFlightConstraint = new FlightConstraint();
	if(pFlight)
	{
		*pFlightConstraint = *pFlight;
	}
	else
	{
		pFlightConstraint->initDefault(); /* Set 'Flight Type': DEFAULT */
	}
	BoardingCallFlightTypeEntry* flightEntry = new BoardingCallFlightTypeEntry();
	flightEntry->initialize(pFlightConstraint, NULL);
	pFlightTypeDB->addEntry(flightEntry, true);/* Replace if exists. */
	AddStand(flightEntry, NULL);
}

void FlightData::AddStand(BoardingCallFlightTypeEntry* flightEntry, char* strProc)
{
	ProcessorID* pStandProcID = new ProcessorID();
	pStandProcID->SetStrDict(m_pInTerm->inStrDict);
	if(strProc)
	{
		pStandProcID->setID(strProc);
	}
	else
	{
		pStandProcID->setID("All Stand"); /* Set 'Stand group': All Stand */
	}
	flightEntry->GetStandDatabase()->addEntry(*pStandProcID);
	BoardingCallStandEntry* pStandEntry = (BoardingCallStandEntry*)flightEntry->GetStandDatabase()->FindEntry(*pStandProcID);
	AddPax(pStandEntry, NULL);/*add a default mobile element constraint entry.*/
}

void FlightData::AddPax(BoardingCallStandEntry* pStandEntry, CMobileElemConstraint* pInputConst)
{
	CMobileElemConstraint* pMBConst = new CMobileElemConstraint();
	if(pInputConst)
	{
		*pMBConst = *pInputConst;
	}
	else
	{
		pMBConst->SetMobileElementType(enum_MobileElementType_ALL); /* Set 'Passenger Type': DEFAULT */
	}
	BoardingCallPaxTypeEntry* pPaxEntry = new BoardingCallPaxTypeEntry();
	pPaxEntry->initialize(pMBConst, NULL);
	pPaxEntry->InitTriggersDatabase();	/* InitTriggersDatabase(): The 4 initial triggers will be added here. */
	pStandEntry->GetPaxTypeDatabase()->addEntry(pPaxEntry, true);/* Replace if exists. */
}

// remove one stage data
void FlightData::DeleteOneStageByIndex(int _stageIndex)
{
	assert( _stageIndex < static_cast<int>(m_vBoardingCallStages.size()) );
	delete m_vBoardingCallStages[_stageIndex];
	m_vBoardingCallStages.erase(m_vBoardingCallStages.begin() + _stageIndex);
}

BOOL FlightData::DeleteOneStageByFlightConstDB(ConstraintDatabase *pFCDB)
{
	int nCount=m_vBoardingCallStages.size();
	for(int i=0;i<nCount;i++)
	{
		if(m_vBoardingCallStages[i]==pFCDB)
		{
			delete m_vBoardingCallStages[i];
			m_vBoardingCallStages.erase(m_vBoardingCallStages.begin()+i);
			return TRUE;
		}
	}
	return FALSE;
}

void FlightData::resortBoardingCallDB()
{
	for( unsigned i=0; i<m_vCalls.size(); i++ )
	{
		m_vCalls[i]->resortDatebase();
	}
}
