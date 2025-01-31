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
	int count = m_vFltTypeDB.size();
	for( int i=0; i<count; i++ )
	{
		delete m_vFltTypeDB[i];
		m_vFltTypeDB[i] =  NULL;
	}
	m_vFltTypeDB.clear(); 
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
	delays.readDatabase (p_file, "DELAY", m_pInTerm );
	loads.readDatabase (p_file, "LOAD", m_pInTerm );
	capacity.readDatabase (p_file, "CAPACITY", m_pInTerm );

	readBoardingCallData(p_file);
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
		FlightConWithProcIDDatabase oldStage;
		oldStage.readObsoleteDatabase( p_file, "BOARDING_CALLS", m_pInTerm );
		AddStageFor260OrOlder(&oldStage);
	}
	else if((float)(2.3-0.00001) < fVersion && fVersion < (float)(2.3+0.00001)) // this is a error version
	{
		clear();
		initDefaultValues();
		int nStageNumber = ReadBoardingCallStageCount( p_file );

		delays.readErrorDatabase (p_file, "DELAY", m_pInTerm );
		loads.readErrorDatabase (p_file, "LOAD", m_pInTerm );
		capacity.readErrorDatabase (p_file, "CAPACITY", m_pInTerm );

		FlightConWithProcIDDatabase oldStage;
		for(int i=0; i<nStageNumber; i++)
		{
			char szLabel[128];
			sprintf(szLabel, "BOARDING_CALLS_STAGE%d", i+1);
			oldStage.readErrorDatabase (p_file, szLabel, m_pInTerm);
			AddStageFor260OrOlder(&oldStage);
		}
	}
	else if(fVersion < (float)(2.4+0.00001))		//2.20 , 2.40
	{
		clear();
		initDefaultValues();
		int nStageNumber = ReadBoardingCallStageCount( p_file );
		
		delays.readDatabase (p_file, "DELAY", m_pInTerm );
		loads.readDatabase (p_file, "LOAD", m_pInTerm );
		capacity.readDatabase (p_file, "CAPACITY", m_pInTerm );

		for( int i=0; i<nStageNumber; i++ )
		{
			FlightConWithProcIDDatabase oldStage;
			char szLabel[128];
			sprintf( szLabel, "BOARDING_CALLS_STAGE%d", i+1 );
			oldStage.readObsoleteDatabase(p_file, szLabel, m_pInTerm );
			AddStageFor260OrOlder(&oldStage);
		}
	}
	else if ((float)(2.5-0.00001) < fVersion && fVersion<(float)(2.5+0.00001))
	{
		int nStageNumber = ReadBoardingCallStageCount( p_file );

		delays.readDatabase (p_file, "DELAY", m_pInTerm );
		loads.readObsoleteDatabase (p_file, "LOAD", m_pInTerm );
		capacity.readObsoleteDatabase (p_file, "CAPACITY", m_pInTerm );

		for( int i=0; i<nStageNumber; i++ )
		{
			FlightConWithProcIDDatabase oldStage;
			char szLabel[128];
			sprintf( szLabel, "BOARDING_CALLS_STAGE%d", i+1 );
			oldStage.readDatabase (p_file, szLabel, m_pInTerm );
			AddStageFor260OrOlder(&oldStage);
		}
	}
	else if ((float)(2.6-0.00001) < fVersion && fVersion<(float)(2.6+0.00001))
	{
		assert( m_pInTerm );
		int nStageNumber = ReadBoardingCallStageCount( p_file );

		delays.readDatabase (p_file, "DELAY", m_pInTerm );
		loads.readDatabase (p_file, "LOAD", m_pInTerm );
		capacity.readDatabase (p_file, "CAPACITY", m_pInTerm );

		for( int i=0; i<nStageNumber; i++ )
		{
			FlightConWithProcIDDatabase oldStage;
			char szLabel[128];
			sprintf( szLabel, "BOARDING_CALLS_STAGE%d", i+1 );
			oldStage.readDatabase (p_file, szLabel, m_pInTerm );
			AddStageFor260OrOlder(&oldStage);
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
    delays.writeDatabase (p_file, "DELAY", m_pInTerm);
    loads.writeDatabase (p_file, "LOAD", m_pInTerm);
    capacity.writeDatabase (p_file, "CAPACITY", m_pInTerm);

	writeBoardingCallData(p_file);
}

void FlightData::readBoardingCallData( ArctermFile &p_file )
{
	p_file.reset();
	char str[64];
	while (p_file.getLine())
	{
		p_file.getField (str, SMALL);
		if (_stricmp (str, "BOARDING_CALL_DATA_BEGIN") == 0)
			break;
	}
	p_file.getLine();
	p_file.skipField(1);
	int stageCount;
	p_file.getInteger(stageCount);
	for(int i=0; i<stageCount; i++)
	{
		AddStage();
		p_file.getLine();
		p_file.skipField(1);
		int fltTypeCount;
		p_file.getInteger(fltTypeCount);
		m_vFltTypeDB[i]->readDatabase(p_file, m_pInTerm, fltTypeCount);
	}
}

void FlightData::writeBoardingCallData( ArctermFile &_file ) const
{
	_file.writeField("BOARDING_CALL_DATA_BEGIN");
	_file.writeLine();
	int stageCount = m_vFltTypeDB.size();

	_file.writeField(szStageLabel);
	_file.writeInt(stageCount);
	_file.writeLine();
	// write boarding call
	char szLabel[128];
	for( int i=0; i<stageCount; i++ )
	{
		sprintf(szLabel, "BOARDING_CALLS_STAGE%d", i);
		_file.writeField(szLabel);
		int flightCount = m_vFltTypeDB[i]->getCount();
		_file.writeInt(flightCount);
		_file.writeLine();
		m_vFltTypeDB[i]->writeDatabase(_file);
	}
	_file.writeField("BOARDING_CALL_DATA_END");
	_file.writeLine();
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

// go through all the file get the size of boardina call stage size;
int FlightData::ReadBoardingCallStageCount( ArctermFile& _file )
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

void FlightData::clear() 
{ 
	delays.clear(); 
	loads.clear();
	capacity.clear(); 
	int nSize = m_vFltTypeDB.size();
	for( int i=0; i<nSize; i++ )
	{
		delete m_vFltTypeDB[i];
		m_vFltTypeDB[i] =  NULL;
	}
	m_vFltTypeDB.clear(); 
}


/*  Add one stage with default value:

    Flight Type: DEPARTING
          |
          | Stand group: All Stand
                 |			
                 |Passenger Type : DEFAULT
                        Number of triggers: 4
                                 |
                                 |Trigger1
                                 |   |Time:-1200
                                 |   |Prop:20
                                 |
                                 |Trigger2
								 |   |Time:-1000
								 |   |Prop:70
								 |
								 |Trigger3
								 |   |Time:-600
								 |   |Prop:10
								 |
								 |Trigger4
								     |Time:-500
								     |Prop:Residual
*/
void FlightData::AddStage()
{
	BoardingCallFlightTypeDatabase* pFlightTypeDB = new BoardingCallFlightTypeDatabase();
	pFlightTypeDB->AddFlightType(NULL, m_pInTerm);
	m_vFltTypeDB.push_back(pFlightTypeDB);
	return;
}


void FlightData::AddStageFor260OrOlder(FlightConWithProcIDDatabase* pOldStage)
{
	if(!pOldStage)
		return;

	BoardingCallFlightTypeDatabase* pFlightTypeDB = new BoardingCallFlightTypeDatabase();
	pFlightTypeDB->AddFlightFor260OrOlder(pOldStage, m_pInTerm);
	m_vFltTypeDB.push_back(pFlightTypeDB);
	return;
}

// remove one stage data
void FlightData::DeleteOneStageByIndex(int _stageIndex)
{
	assert( _stageIndex < static_cast<int>(m_vFltTypeDB.size()) );
	delete m_vFltTypeDB[_stageIndex];
	m_vFltTypeDB.erase(m_vFltTypeDB.begin() + _stageIndex);
}

BOOL FlightData::DeleteOneStageByFlightConstDB(ConstraintDatabase *pFCDB)
{
	int nCount=m_vFltTypeDB.size();
	for(int i=0;i<nCount;i++)
	{
		if(m_vFltTypeDB[i]==pFCDB)
		{
			delete m_vFltTypeDB[i];
			m_vFltTypeDB.erase(m_vFltTypeDB.begin()+i);
			return TRUE;
		}
	}
	return FALSE;
}


