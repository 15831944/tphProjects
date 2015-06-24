#include "stdafx.h"
#include "paxdata.h"
#include "pax_db.h"
#include "in_term.h"
#include "flightconwithprociddatabase.h"
#include "MobileElemTypeStrDB.h"
#include "../Common/ProbabilityDistribution.h"
#include "../Common/termfile.h"
#include "../Common/helplist.h"
#include "../Common/strdict.h"


char * far PassengerData::paxDataName[] =
{
    //"BAG_COUNT",
    //"CART_COUNT",
    "GROUP_SIZE",
    "IN_STEP",
    //"LEAD_LAG_TIME",
    "SIDE_STEP",
    "SPEED",
    //"VISITORS",
    "VISIT_TIME",
	"RESPONSE_TIME",
	"SPEED_IMPACT",
	"PUSH_PROPENSITY",
	"IMPACT_SPEED",
	"IMPACT_INSTEP",
	"IMPACT_SIDESTEP",  //ADD by adam 2007-05-14
	"VISITOR_STA_TRIGGER",
    NULL
};

char * far PassengerData::units[] =
{
    //"COUNT",
    //"COUNT",
    "COUNT",
    "METERS",
    //"MIN",
    "METERS",
    "METERS/SEC",
    //"COUNT",
    "MIN",
	"SECONDS",
	"METERS/SEC",
	"PERCENT",
	"METERS",
	"METERS/SEC",
	"METERS", //ADD by adam 2007-05-14
	"MIN",
    NULL
};


PassengerData::PassengerData() : DataSet (PassengerDataFile,(float)2.80)
{
    for (int i = 0; i < PAX_DATA_COUNT; i++)
    {
        paxData[i] = new CMobileElemConstraintDatabase;
        paxData[i]->setUnits (units[i]);
    }
	pLeadLagData = new CMobileConWithProcIDDatabase;
	pLeadLagData->setUnits("MIN");

	nopaxData = NULL;
	iNopaxCount = 0; 
}

PassengerData::~PassengerData()
{
    for (int i = 0; i < PAX_DATA_COUNT; i++)
        delete (paxData[i]);
	delete pLeadLagData;
	delNoPaxData();
}
/*
int PassengerData::getHelp (int p_ndx) const
{
    switch (p_ndx)
    {
        case 0:
            return BAGCOUN1_HLP;
        case 1:
            return CRTCOUN1_HLP;
        case 2:
            return GROUPSI1_HLP;
        case 3:
            return INSTE1_HLP;
        case 4:
            return LEADLA1_HLP;
        case 5:
            return SIDESTE1_HLP;
        case 6:
            return SPEE1_HLP;
        case 7:
            return VISITOR1_HLP;
        case 8:
				return VISITIM1_HLP;
    }
    return 0;
}

char PassengerData::getMode (int p_ndx) const
{
	 return (p_ndx >= 9)? 'D': '\0';
}
*/
// delete all references to the passed index at p_level
void PassengerData::deletePaxType (int p_level, int p_index)
{
    for (int i = 0; i < PAX_DATA_COUNT; i++)
        paxData[i]->deletePaxType (p_level, p_index);
	
	pLeadLagData->deletePaxType( p_level, p_index );
	if(nopaxData)
	{
		for(int j =0; j<iNopaxCount; j++)	
			nopaxData[j]->deletePaxType(p_level, p_index);
	}
}

void PassengerData::initDefaultValues (void)
{
    ProbabilityDistribution *defaultDist;
    ConstraintEntry *anEntry;

    // init default bag count distribution
    double vals[3] = {0, 1, 2};
    unsigned char probs[3] = {20, 70, 100};
    //defaultDist = new EmpiricalDistribution (3, vals, probs);
    //anEntry = new ConstraintEntry;
    //anEntry->initialize (new CPassengerConstraint, defaultDist);
    //paxData[BagCountDatabase]->addEntry (anEntry);

    // init default cart count distribution
    //defaultDist = new BernoulliDistribution (0, 1, 0.4);
    //anEntry = new ConstraintEntry;
    //anEntry->initialize (new CPassengerConstraint, defaultDist);
    //paxData[CartCountDatabase]->addEntry (anEntry);

    // init default group size distribution
    defaultDist = new BernoulliDistribution (1.0, 2.0, 0.65);
    anEntry = new ConstraintEntry;
    anEntry->initialize (new CMobileElemConstraint(m_pInTerm), defaultDist);
    paxData[GroupSizeDatabase]->addEntry (anEntry);

    // init default in step distribution
    defaultDist = new UniformDistribution (0.65, 0.75);
    anEntry = new ConstraintEntry;
    anEntry->initialize (new CMobileElemConstraint(m_pInTerm), defaultDist);
    paxData[InStepDatabase]->addEntry (anEntry);

    // init default side step distribution
    defaultDist = new UniformDistribution (-0.3, 0.3);
    anEntry = new ConstraintEntry;
    anEntry->initialize (new CMobileElemConstraint(m_pInTerm), defaultDist);
    paxData[SideStepDatabase]->addEntry (anEntry);

    // init default walking speed distribution
    defaultDist = new UniformDistribution (0.6, 1.0);
    anEntry = new ConstraintEntry;
    anEntry->initialize (new CMobileElemConstraint(m_pInTerm), defaultDist);
    paxData[SpeedDatabase]->addEntry (anEntry);

    // init default visitor count distribution
    //defaultDist = new ConstantDistribution (0);
    //anEntry = new ConstraintEntry;
    //anEntry->initialize (new CPassengerConstraint, defaultDist);
    //paxData[VisitorCountDatabase]->addEntry (anEntry);

    // init default visitor time distribution
    //defaultDist = new UniformDistribution (10, 30);
    //anEntry = new ConstraintEntry;
    //anEntry->initialize (new CMobileElemConstraint(m_pInTerm), defaultDist);
    //paxData[VisitTimeDatabase]->addEntry (anEntry);

    // init default lead lag time distribution
    defaultDist = new UniformDistribution (5, 15);
    ConstraintWithProcIDEntry* pWithProcEntry = new ConstraintWithProcIDEntry;
	ProcessorID defaultid;
	defaultid.SetStrDict( m_pInTerm->inStrDict );
    pWithProcEntry->initialize (new CMobileElemConstraint(m_pInTerm), defaultDist,defaultid);
    //paxData[LeadLagTimeDatabase]->addEntry (anEntry);
	pLeadLagData->addEntry(pWithProcEntry);

    // init lead lag time distribution for departures
    defaultDist = new UniformDistribution (-90, -40);
    CMobileElemConstraint *defaultConst = new CMobileElemConstraint(m_pInTerm);
	assert( m_pInTerm );
    defaultConst->setIntrinsicType (m_pInTerm->inStrDict->findString ("DEPARTURE"));
    pWithProcEntry = new ConstraintWithProcIDEntry;
    pWithProcEntry->initialize (defaultConst, defaultDist,defaultid);
    pLeadLagData->addEntry (pWithProcEntry);
	
	// init response time distribution
	defaultDist = new ConstantDistribution( 10 );
	anEntry = new ConstraintEntry;
	anEntry->initialize( new CMobileElemConstraint(m_pInTerm), defaultDist );
	paxData[ResponseTimeDatabase]->addEntry( anEntry );

	//init speed impact distribution
	defaultDist = new ConstantDistribution(4);
	anEntry = new ConstraintEntry;
	anEntry->initialize( new CMobileElemConstraint(m_pInTerm), defaultDist );
	paxData[ImpactEmerDatabase]->addEntry( anEntry);

	// init push Propensity distribution
	defaultDist = new ConstantDistribution(50);
	anEntry = new ConstraintEntry;
	anEntry->initialize( new CMobileElemConstraint(m_pInTerm), defaultDist );
	paxData[PushPropensityDatabase]->addEntry( anEntry );

	//init the impact speed
	defaultDist = new ConstantDistribution(0);
	anEntry = new ConstraintEntry;
	anEntry->initialize( new CMobileElemConstraint(m_pInTerm),defaultDist );
	paxData[ImpactSpeedDatabase]->addEntry(anEntry);

	//init the impact in-step
	defaultDist = new ConstantDistribution(0);
	anEntry = new ConstraintEntry;
	anEntry->initialize( new CMobileElemConstraint(m_pInTerm),defaultDist );
	paxData[ImpactInStepDatabase]->addEntry(anEntry);

	//init the impact side-step
	defaultDist = new ConstantDistribution(0);
	anEntry = new ConstraintEntry;
	anEntry->initialize( new CMobileElemConstraint(m_pInTerm),defaultDist );
	paxData[ImpactSideStepDatabase]->addEntry(anEntry);

	//init visitor sta trigger
	defaultDist = new ConstantDistribution(0);
	anEntry = new ConstraintEntry;
	anEntry->initialize( new CMobileElemConstraint(m_pInTerm),defaultDist );
	paxData[VisitorSTATrigger]->addEntry(anEntry);

	//init non_passenger count distribution
	initNoPaxData();
	for(int i=0; i<iNopaxCount; i++)
	{
		defaultDist = new ConstantDistribution( 0.0 );
		ConstraintEntryEx* anEntryEx = new ConstraintEntryEx;
		anEntryEx->initialize (new CMobileElemConstraint(m_pInTerm), defaultDist);
		nopaxData[i]->addEntry (anEntryEx);
	}
}

void PassengerData::clear (void)
{
    for (int i = 0; i < PAX_DATA_COUNT; i++)
        paxData[i]->clear();
	pLeadLagData->clear();
	delNoPaxData();
}

void PassengerData::readData (ArctermFile& p_file)
{
	assert( m_pInTerm );
    for (int i = 0; i < PAX_DATA_COUNT; i++)
        paxData[i]->readDatabase (p_file, paxDataName[i], m_pInTerm );
	pLeadLagData->readDatabase( p_file,"LEAD_LAG_TIME",m_pInTerm );

	readNoPaxData( p_file );
}

void PassengerData::readObsoleteData ( ArctermFile& p_file )
{
	assert( m_pInTerm );
	float fVersion = p_file.getVersion();

	if( fVersion<(float)2.30 )
	{
		for (int i = 0; i < PAX_DATA_COUNT - 1; i++)
			paxData[i]->readDatabase (p_file, paxDataName[i], m_pInTerm );
		pLeadLagData->readObsoleteDatabase( p_file, "LEAD_LAG_TIME", m_pInTerm );

		readObsoleteNoPaxData( p_file, fVersion );
	}

	else if( fVersion == (float)2.40 ) // this is a error version
	{
		for (int i = 0; i < PAX_DATA_COUNT - 1; i++)
			paxData[i]->readErrorDatabase (p_file, paxDataName[i], m_pInTerm );
		pLeadLagData->readErrorDatabase( p_file, "LEAD_LAG_TIME", m_pInTerm );

		readErrorNoPaxData( p_file, fVersion );
	}

	else if( fVersion < (float)2.60 ) //2.3, 2.5
	{
		for (int i = 0; i < PAX_DATA_COUNT - 1; i++)
			paxData[i]->readDatabase (p_file, paxDataName[i], m_pInTerm );
		pLeadLagData->readObsoleteDatabase( p_file,"LEAD_LAG_TIME",m_pInTerm );

		readNoPaxData( p_file );
	}
	else if(fVersion < (float)2.70)
	{
		assert( m_pInTerm );
		for (int i = 0; i < PAX_DATA_COUNT - 1; i++)
			paxData[i]->readDatabase(p_file, paxDataName[i], m_pInTerm );
		pLeadLagData->readDatabase( p_file,"LEAD_LAG_TIME",m_pInTerm );

		readNoPaxData( p_file );
	}

	if(fVersion < (float)2.70)
	{
		//enum PaxDataTypes
		//{
		//	//BagCountDatabase,
		//	//CartCountDatabase,
		//	GroupSizeDatabase,//0
		//	InStepDatabase,//1
		//	//LeadLagTimeDatabase,
		//	SideStepDatabase,//2
		//	SpeedDatabase,//3
		//	//VisitorCountDatabase,
		//	VisitTimeDatabase,//4
		//	ResponseTimeDatabase,//5
		//		ImpactEmerDatabase,//6
		//	PushPropensityDatabase,//7
		//		ImpactSpeedDatabase,//8
		//		ImpactInStepDatabase,//9
		//		ImpactSideStepDatabase //10           //add by adam 2007-05-14
		//};

		//change he mobile element dialog
		//the visitor, ImpactEmerDatabase,ImpactSpeedDatabase,ImpactInStepDatabase,ImpactSideStepDatabase
		
		int i =0;
		int nEmerCount = getImpactEmer()->getCount();
		for (; i< nEmerCount; ++i)
		{
			Constraint *pConstraint = getImpactEmer()->getItem(i)->getConstraint();
			if(pConstraint)
			{
				CMobileElemConstraint *pMobileElement = (CMobileElemConstraint *)pConstraint;
				if (pMobileElement != NULL)
				{
					if(pMobileElement->GetMobileElementType() == enum_MobileElementType_ALL)
					{
						pMobileElement->SetMobileElementType(enum_MobileElementType_PAX);
						pMobileElement->SetTypeIndex(0);
					}
				}
			}
		}
	
		//ImpactSpeed
		int nImpactSpeedCount = getImpactSpeed()->getCount();
		for (i =0; i< nImpactSpeedCount; ++i)
		{
			Constraint *pConstraint = getImpactSpeed()->getItem(i)->getConstraint();
			if(pConstraint)
			{
				CMobileElemConstraint *pMobileElement = (CMobileElemConstraint *)pConstraint;
				if (pMobileElement != NULL)
				{
					if(pMobileElement->GetMobileElementType() == enum_MobileElementType_ALL)
					{
						pMobileElement->SetMobileElementType(enum_MobileElementType_PAX);
						pMobileElement->SetTypeIndex(0);
					}
				}
			}
		}

		//ImpactSideStep
		int nImpactSideCount = getImpactSideStep()->getCount();
		for (i =0; i< nImpactSideCount; ++i)
		{
			Constraint *pConstraint = getImpactSideStep()->getItem(i)->getConstraint();
			if(pConstraint)
			{
				CMobileElemConstraint *pMobileElement = (CMobileElemConstraint *)pConstraint;
				if (pMobileElement != NULL)
				{
					if(pMobileElement->GetMobileElementType() == enum_MobileElementType_ALL)
					{
						pMobileElement->SetMobileElementType(enum_MobileElementType_PAX);
						pMobileElement->SetTypeIndex(0);
					}
				}
			}
		}
		//getImpactInStep
		int nImpactInStepCount = getImpactInStep()->getCount();
		for (i =0; i< nImpactInStepCount; ++i)
		{
			Constraint *pConstraint = getImpactInStep()->getItem(i)->getConstraint();
			if(pConstraint)
			{
				CMobileElemConstraint *pMobileElement = (CMobileElemConstraint *)pConstraint;
				if (pMobileElement != NULL)
				{
					if(pMobileElement->GetMobileElementType() == enum_MobileElementType_ALL)
					{
						pMobileElement->SetMobileElementType(enum_MobileElementType_PAX);
						pMobileElement->SetTypeIndex(0);
					}
				}
			}
		}

		//Non_passenger
		for( int nPax=0; nPax<iNopaxCount; nPax++ )
		{
			int nNonPaxCount =nopaxData[nPax]->getCount();
			for ( i = 0; i < nNonPaxCount; ++i)
			{
				Constraint *pConstraint = nopaxData[nPax]->getItem(i)->getConstraint();
				if(pConstraint)
				{
					CMobileElemConstraint *pMobileElement = (CMobileElemConstraint *)pConstraint;
					if (pMobileElement != NULL)
					{
						if(pMobileElement->GetMobileElementType() == enum_MobileElementType_ALL)
						{
							pMobileElement->SetMobileElementType(enum_MobileElementType_PAX);
							pMobileElement->SetTypeIndex(0);
						}
					}
				}
			}

		}

	}

	if (fVersion < (float)2.80)
	{
		for (int i = 0; i < PAX_DATA_COUNT - 1; i++)
			paxData[i]->readDatabase (p_file, paxDataName[i], m_pInTerm );
		pLeadLagData->readDatabase( p_file,"LEAD_LAG_TIME",m_pInTerm );

		readNoPaxData( p_file );
	}

	if (fVersion < (float)2.90)
	{
		for (int i = 0; i < PAX_DATA_COUNT; i++)
			paxData[i]->readDatabase (p_file, paxDataName[i], m_pInTerm );
		pLeadLagData->readDatabase( p_file,"LEAD_LAG_TIME",m_pInTerm );

		readNoPaxData( p_file );
	}
}


void PassengerData::writeData (ArctermFile& p_file) const
{
	assert( m_pInTerm );
    for (int i = 0; i < PAX_DATA_COUNT; i++)
        paxData[i]->writeDatabase (p_file, paxDataName[i], m_pInTerm);
	pLeadLagData->writeDatabase( p_file, "LEAD_LAG_TIME", m_pInTerm);
	writeNoPaxData( p_file );
}


void PassengerData::readNoPaxData(ArctermFile &p_file)
{
	ProbabilityDistribution *defaultDist;
    ConstraintEntryEx *anEntryEx;
	
	assert( m_pInTerm );
	
	//delNoPaxData();
	initNoPaxData();

	//read nonpassenger data from file
	for( int i=0; i<iNopaxCount; i++ )
	{
		CString strNonPaxName = m_pInTerm->m_pMobElemTypeStrDB->GetString( i);
		nopaxData[i]->readDatabase( p_file, (LPCSTR)strNonPaxName, m_pInTerm );
		//if the item has not been initiated,initiated it;
		if(  nopaxData[i]->getCount()<1 )	//initated item
		{
			defaultDist = new ConstantDistribution( 0.0 );
			anEntryEx = new ConstraintEntryEx;
			CMobileElemConstraint* pMob = new CMobileElemConstraint(m_pInTerm);
			pMob->SetInputTerminal( m_pInTerm );
			anEntryEx->initialize ( pMob , defaultDist);
			nopaxData[i]->addEntry (anEntryEx);			
		}
	}
}

void PassengerData::readObsoleteNoPaxData( ArctermFile& p_file, float _fVersion ) 
{
	ProbabilityDistribution *defaultDist;
    ConstraintEntryEx *anEntryEx;
	
	assert( m_pInTerm );
	
	//delNoPaxData();
	initNoPaxData();

	//read nonpassenger data from file
	for( int i=0; i<iNopaxCount; i++ )
	{
		CString strNonPaxName = m_pInTerm->m_pMobElemTypeStrDB->GetString( i);
		nopaxData[i]->readOldDatabase( p_file, (LPCSTR)strNonPaxName, m_pInTerm );
		//if the item has not been initiated,initiated it;
		if(  nopaxData[i]->getCount()<1 )	//initated item
		{
			defaultDist = new ConstantDistribution( 0.0 );
			anEntryEx = new ConstraintEntryEx;
			CMobileElemConstraint* pMob = new CMobileElemConstraint(m_pInTerm);
			pMob->SetInputTerminal( m_pInTerm );
			anEntryEx->initialize ( pMob, defaultDist);
			nopaxData[i]->addEntry (anEntryEx);			
		}
	}
}

void PassengerData::readErrorNoPaxData( ArctermFile& p_file, float _fVersion  )
{
	ProbabilityDistribution *defaultDist;
    ConstraintEntryEx *anEntryEx;
	
	assert( m_pInTerm );
	
	//delNoPaxData();
	initNoPaxData();
	
	//read nonpassenger data from file
	for( int i=0; i<iNopaxCount; i++ )
	{
		CString strNonPaxName = m_pInTerm->m_pMobElemTypeStrDB->GetString( i);
		nopaxData[i]->readErrorDatabase( p_file, (LPCSTR)strNonPaxName, m_pInTerm );
		//if the item has not been initiated,initiated it;
		if(  nopaxData[i]->getCount()<1 )	//initated item
		{
			defaultDist = new ConstantDistribution( 0.0 );
			anEntryEx = new ConstraintEntryEx;
			CMobileElemConstraint* pMob = new CMobileElemConstraint(m_pInTerm);
			pMob->SetInputTerminal( m_pInTerm );
			anEntryEx->initialize ( pMob , defaultDist);
			nopaxData[i]->addEntry (anEntryEx);			
		}
	}
}

void PassengerData::delNoPaxData()
{
//	assert( m_pInTerm );

	//iNopaxCount = m_pInTerm->m_pMobElemTypeStrDB->GetCount();
	if( nopaxData )
	{
		for(int i=0; i<iNopaxCount; i++ )
			delete nopaxData[i];
	}

	delete []nopaxData;
}


void PassengerData::initNoPaxData( )
{
	assert( m_pInTerm );

	iNopaxCount	= m_pInTerm->m_pMobElemTypeStrDB->GetCount();
	nopaxData = new CNoPaxConstraintCoutDatabasePointer[ iNopaxCount];
	for(int i =0; i<iNopaxCount; i++)	
	{
		nopaxData[i] = new CNoPaxConstraintCoutDatabase;
        nopaxData[i]->setUnits( "COUNT" );
	}
}


CMobileElemConstraintDatabase* PassengerData::getNopaxData(int ndx) const
{
	assert( m_pInTerm );

	if( ndx >=0 && ndx< iNopaxCount )
		return nopaxData[ ndx ];
	else
		return NULL;
}

void PassengerData::writeNoPaxData(ArctermFile &p_file) const
{
	assert( m_pInTerm );
	//assert( nopaxData );

	//iNopaxCount = m_pInTerm->m_pMobElemTypeStrDB->GetCount();
    for (int i = 0; i < iNopaxCount; i++)
	{
		CString strNopaxName = m_pInTerm->m_pMobElemTypeStrDB->GetString( i); 
		nopaxData[i]->writeDatabase(p_file,(LPCSTR)strNopaxName, m_pInTerm);
	}
}