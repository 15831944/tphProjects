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
	int nSize = m_vCalls.size();
	if( nStageNumber == 0 && nSize == 0 )
	{
		AddStage();
		return;
	}

	for( int i=0; i<nStageNumber; i++ )
	{
		if( m_vCalls.size() <= (unsigned)i )
		{
			// need to new 
			FlightConWithProcIDDatabase* pStage = new FlightConWithProcIDDatabase();
			m_vCalls.push_back( pStage );
		}
		char szLabel[128];
		sprintf( szLabel, "BOARDING_CALLS_STAGE%d", i+1 );
	    m_vCalls[i]->readDatabase (p_file, szLabel, m_pInTerm );
	}
}

void FlightData::readObsoleteData (ArctermFile& p_file)
{	
	assert( m_pInTerm );

	float fVersion = p_file.getVersion();
	if( fVersion < (float)2.2 )
	{
		clear();
		initDefaultValues();
		delays.readDatabase (p_file, "DELAY", m_pInTerm );
		capacity.readDatabase (p_file, "LOAD", m_pInTerm);
		if( p_file.getVersion() >= 2.0 )
			capacity.readDatabase (p_file, "CAPACITY", m_pInTerm );
		m_vCalls[0]->readObsoleteDatabase( p_file, "BOARDING_CALLS", m_pInTerm );
	}
	else if( fVersion == (float)2.3 ) // this is a error version
	{
		clear();
		initDefaultValues();
		int nStageNumber = ReadBoardingCallStageNumber( p_file );

		delays.readErrorDatabase (p_file, "DELAY", m_pInTerm );
		loads.readErrorDatabase (p_file, "LOAD", m_pInTerm );
		capacity.readErrorDatabase (p_file, "CAPACITY", m_pInTerm );

		int nSize = m_vCalls.size();
		if( nStageNumber == 0 && nSize == 0 )
		{
			AddStage();
			return;
		}
		
		for( int i=0; i<nStageNumber; i++ )
		{
			if( m_vCalls.size() <= (unsigned)i )
			{
				// need to new 
				FlightConWithProcIDDatabase* pStage = new FlightConWithProcIDDatabase();
				m_vCalls.push_back( pStage );
			}
			char szLabel[128];
			sprintf( szLabel, "BOARDING_CALLS_STAGE%d", i+1 );
			m_vCalls[i]->readErrorDatabase (p_file, szLabel, m_pInTerm );
		}
	}
	else if( fVersion <= (float) 2.4)		//2.20 , 2.40
	{
		clear();
		initDefaultValues();
		int nStageNumber = ReadBoardingCallStageNumber( p_file );
		
		delays.readDatabase (p_file, "DELAY", m_pInTerm );
		loads.readDatabase (p_file, "LOAD", m_pInTerm );
		capacity.readDatabase (p_file, "CAPACITY", m_pInTerm );
		
		// read boarding call
		int nSize = m_vCalls.size();
		if( nStageNumber == 0 && nSize == 0 )
		{
			AddStage();
			return;
		}
		
		for( int i=0; i<nStageNumber; i++ )
		{
			if( m_vCalls.size() <= (unsigned)i )
			{
				// need to new 
				FlightConWithProcIDDatabase* pStage = new FlightConWithProcIDDatabase();
				m_vCalls.push_back( pStage );
			}
			char szLabel[128];
			sprintf( szLabel, "BOARDING_CALLS_STAGE%d", i+1 );
			m_vCalls[i]->readObsoleteDatabase(p_file, szLabel, m_pInTerm );
		}
	}
	else if (fVersion == (float)2.5)
	{
		int nStageNumber = ReadBoardingCallStageNumber( p_file );

		delays.readDatabase (p_file, "DELAY", m_pInTerm );
		loads.readObsoleteDatabase (p_file, "LOAD", m_pInTerm );
		capacity.readObsoleteDatabase (p_file, "CAPACITY", m_pInTerm );

		// read boarding call
		int nSize = m_vCalls.size();
		if( nStageNumber == 0 && nSize == 0 )
		{
			AddStage();
			return;
		}

		for( int i=0; i<nStageNumber; i++ )
		{
			if( m_vCalls.size() <= (unsigned)i )
			{
				// need to new 
				FlightConWithProcIDDatabase* pStage = new FlightConWithProcIDDatabase();
				m_vCalls.push_back( pStage );
			}
			char szLabel[128];
			sprintf( szLabel, "BOARDING_CALLS_STAGE%d", i+1 );
			m_vCalls[i]->readDatabase (p_file, szLabel, m_pInTerm );
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
	int nSize = m_vCalls.size();
	for( int i=0; i<nSize; i++ )
	{
		delete m_vCalls[i];
		m_vCalls[i] =  NULL;
	}
	m_vCalls.clear(); 
}


// Add one more stages with default value
void FlightData::AddStage()
{
    // init default boarding calls distribution
    double vals[3] = {-1200, -1000, -600};
    double probs[3] = {20, 90, 100};
    ProbabilityDistribution *defaultDist = new HistogramDistribution (3, vals, probs);
    ConstraintWithProcIDEntry *pEntry = new ConstraintWithProcIDEntry;
	ProcessorID id;
	id.SetStrDict( m_pInTerm->inStrDict );

    pEntry->initialize (new FlightConstraint, defaultDist, id );
	FlightConWithProcIDDatabase* pNewStage = new FlightConWithProcIDDatabase();
    pNewStage->addEntry (pEntry);
	m_vCalls.push_back( pNewStage );	
}

// remove one stage data
void FlightData::DeleteOneStage( int _nStageNumber )
{
	assert( _nStageNumber < static_cast<int>(m_vCalls.size()) );
	m_vCalls.erase( m_vCalls.begin() + _nStageNumber );
}

void FlightData::DeleteOneStage(ConstraintDatabase *pFCDB)
{
	int nCount=m_vCalls.size();
	for(int i=0;i<nCount;i++)
	{
		if(m_vCalls[i]==pFCDB)
		{
			m_vCalls.erase(m_vCalls.begin()+i);
			break;
		}
	}
}

void FlightData::resortBoardingCallDB()
{
	for( unsigned i=0; i<m_vCalls.size(); i++ )
	{
		m_vCalls[i]->resortDatebase();
	}
}

void FlightData::InitDemoStageData()
{
	BoardingCallFlightTypeDatabase* pFlightDB = new BoardingCallFlightTypeDatabase();
	m_vBoardingCallStages.push_back(pFlightDB);
	BoardingCallFlightTypeEntry* pFlightEntry = new BoardingCallFlightTypeEntry();
	FlightConstraint* pFlightConstraint = new FlightConstraint();
	pFlightConstraint->initDefault();
	pFlightEntry->initialize(pFlightConstraint, NULL);
	pFlightDB->push_back(pFlightEntry);
	BoardingCallStandDatabase* pStandDB = pFlightEntry->GetStandDatabase();
	BoardingCallStandEntry* pStandEntry = new BoardingCallStandEntry();
	ProcessorID* pStandProcID = new ProcessorID();
	pStandProcID->SetStrDict(m_pInTerm->inStrDict);
	pStandProcID->setID("StandProcID_XX");
	pStandEntry->changeID(*pStandProcID);
	pStandDB->push_back(pStandEntry);
	BoardingCallPaxTypeDatabase* pPaxDB = pStandEntry->GetPaxTypeDatabase();
	BoardingCallPaxTypeEntry* pPaxEntry = new BoardingCallPaxTypeEntry();
	CMobileElemConstraint* pMBConst = new CMobileElemConstraint();
	pMBConst->SetMobileElementType(enum_MobileElementType_ALL);
	pPaxEntry->initialize(pMBConst, NULL);
	pPaxDB->push_back(pPaxEntry);
	std::vector<BoardingCallTrigger>* pTriggers = pPaxEntry->GetTriggersDatabase();
	BoardingCallTrigger trigger1;
	trigger1.SetTriggerTime(10000L);
	trigger1.SetTriggerProportion(20.0);
	pTriggers->push_back(trigger1);
	trigger1.SetTriggerTime(20000L);
	trigger1.SetTriggerProportion(20.0);
	pTriggers->push_back(trigger1);
	trigger1.SetTriggerTime(30000L);
	trigger1.SetTriggerProportion(20.0);
	pTriggers->push_back(trigger1);
}
