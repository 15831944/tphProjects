// HubbingDatabase.cpp: implementation of the CHubbingDatabase class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "HubbingDatabase.h"
#include "probab.h"
#include "fltdist.h"
#include "fltdist.h"
#include "schedule.h"
#include "flight.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHubbingDatabase::CHubbingDatabase()
	:SortedContainer<CHubbingData>(16, 0, 16), DataSet( HubbingDataFile, (float)2.3 )
{

}

CHubbingDatabase::~CHubbingDatabase()
{

}


// add a default CHubbingData with the passed passenger type
CHubbingData* CHubbingDatabase::AddDefaultEntry( const CPassengerConstraint& _paxConstr )
{
	CHubbingData* pHubbingData = new CHubbingData;
	pHubbingData->SetConstraint( _paxConstr );
    ProbabilityDistribution* pDefTransitingDist = new ConstantDistribution (10);
	pHubbingData->SetTransiting( pDefTransitingDist );
    ProbabilityDistribution* pDefTransferingDist = new ConstantDistribution (20);
	pHubbingData->SetTransferring( pDefTransferingDist );
    FlightTypeDistribution* pDefTransferDestDist = new FlightTypeDistribution;
	pHubbingData->SetTransferDest( pDefTransferDestDist );
	addItem( pHubbingData );
	return pHubbingData;
}



// used by DataSet for set up the default value if there is no file
void CHubbingDatabase::initDefaultValues()
{
	clear();
	CHubbingData* pHubbingData = new CHubbingData;
	CPassengerConstraint paxConstr(m_pInTerm);	// default passenger constraint
	AddDefaultEntry( paxConstr );
}



// 
void CHubbingDatabase::clear()
{
	SortedContainer<CHubbingData>::clear();
}


void CHubbingDatabase::readData (ArctermFile& _file)
{
	clear();
	_file.reset();
	_file.skipLines (3);

	//read the count of the Hub data
	if( !_file.getLine() )
		return;
	int nHubDataCount;
	_file.getInteger(nHubDataCount);

	for (int i = 0; i < nHubDataCount; i++)
	{
		if( !_file.getLine() )
			return;

		//read the count of the destination data
		int nDisCount;
		_file.getInteger(nDisCount);

		CHubbingData* pHubbingData = new CHubbingData;

		if (nDisCount == 0)//only read the transit and transfer if there is no destination
		{
			if( !ReadTransiting( _file, pHubbingData ) )
			{
				delete pHubbingData;
				return;
			}

			if( !ReadTransfering( _file, pHubbingData ) )
			{
				delete pHubbingData;
				return;
			}
			addItem( pHubbingData );//add the Hub data to HubDataBase
		}
		else if (nDisCount > 0)//read all if destination's count > 0
		{
			if( !ReadTransiting( _file, pHubbingData ) )
			{
				delete pHubbingData;
				return;
			}

			if( !ReadTransfering( _file, pHubbingData ) )
			{
				delete pHubbingData;
				return;
			}

			if ( !ReadTransferDest( _file, pHubbingData ) )
			{
				delete pHubbingData;
				return;
			}

			for( int j = 0; j < nDisCount; j++ )
			{
				if( !ReadMinTransferTime( _file, pHubbingData, j ) )
				{
					delete pHubbingData;
					return;
				}
				if( !ReadTransferWindow( _file, pHubbingData, j ) )
				{
					delete pHubbingData;
					return;
				}
			}

		addItem( pHubbingData );//add the Hub data to HubDataBase
		}

	}

//	WipeIntrinsic();

	// fill the empty item with default value
	FillEmptyWithDefault();
}


// read transit data
bool CHubbingDatabase::ReadTransiting( ArctermFile& _file, CHubbingData* _pHubbingData )
{
	if( !_file.getLine() )
		return false;

	// type of data
	char szDataType[64];
	_file.getField( szDataType, SMALL );
	if( strcmp( szDataType, "TRANSITING" ) != 0 )
		return false;

	// pax type
	CPassengerConstraint aPaxConst(m_pInTerm);
	aPaxConst.SetInputTerminal( m_pInTerm );
	aPaxConst.readConstraint (_file);

	int nEqualIndex = FindEqual( aPaxConst );
	if( nEqualIndex == -1 )
	{
		_pHubbingData->SetConstraint( aPaxConst );
	}

	// unit
	char szUnit[64];
	_file.getField( szUnit, SMALL );

	// prob dist
	ProbabilityDistribution* pProbDist = GetTerminalRelateProbDistribution (_file,m_pInTerm);
	_pHubbingData->	SetTransiting( pProbDist );

	return true;
}

//read the transfer data
bool CHubbingDatabase::ReadTransfering( ArctermFile& _file, CHubbingData* _pHubbingData )
{
	if( !_file.getLine() )
		return false;

	// type of data
	char szDataType[64];
	_file.getField( szDataType, SMALL );
	if( strcmp( szDataType, "TRANSFERRING" ) != 0 )
		return false;

	// pax type
	CPassengerConstraint aPaxConst(m_pInTerm);
	//aPaxConst.SetInputTerminal( m_pInTerm );
	aPaxConst.readConstraint (_file);

	int nEqualIndex = FindEqual( aPaxConst );
	if( nEqualIndex == -1 )
	{
		_pHubbingData->SetConstraint( aPaxConst );
	}
	// unit
	char szUnit[64];
	_file.getField( szUnit, SMALL );

	// prob dist
	ProbabilityDistribution* pProbDist = GetTerminalRelateProbDistribution (_file,m_pInTerm);
	_pHubbingData->SetTransferring(pProbDist);

	return true;
}

//read transfer destination
bool CHubbingDatabase::ReadTransferDest( ArctermFile& _file, CHubbingData* _pHubbingData )
{
	if( !_file.getLine() )
		return false;

	// type of data
	char szDataType[64];
	_file.getField( szDataType, SMALL );
	if( strcmp( szDataType, "TRANSFER_DESTINATIONS" ) != 0 )
		return false;

	// pax type
	CPassengerConstraint aPaxConst(m_pInTerm);
	aPaxConst.SetInputTerminal( m_pInTerm );
	aPaxConst.readConstraint (_file);

	int nEqualIndex = FindEqual( aPaxConst );
	if( nEqualIndex == -1 )
	{
		_pHubbingData->SetConstraint( aPaxConst );
	}

	// unit
	char szUnit[64];
	_file.getField( szUnit, SMALL );

	// prob dist
	ProbabilityDistribution* pProbDist = GetTerminalRelateProbDistribution (_file,m_pInTerm);
	FlightTypeDistribution* pFlightType = (FlightTypeDistribution*)pProbDist;
	_pHubbingData->SetTransferDest(pFlightType);

	return true;
}

//read the min transfer time
bool CHubbingDatabase::ReadMinTransferTime( ArctermFile& _file, CHubbingData* _pHubbingData, int nIndex )
{
	if( !_file.getLine() )
		return false;

	// type of data
	char szDataType[64];
	_file.getField( szDataType, SMALL );
	if( strcmp( szDataType, "MIN_TRANSFER_TIME" ) != 0 )
		return false;

	// pax type
	CPassengerConstraint aPaxConst(m_pInTerm);
	aPaxConst.SetInputTerminal( m_pInTerm );
	aPaxConst.readConstraint (_file);

	//_pHubbingData->SetConstraint( aPaxConst );
	int nEqualIndex = FindEqual( aPaxConst );
	if( nEqualIndex == -1 )
	{
		_pHubbingData->SetConstraint( aPaxConst );
	}

	// unit
	char szUnit[64];
	_file.getField( szUnit, SMALL );

	// prob dist
	ProbabilityDistribution* pProbDist = GetTerminalRelateProbDistribution (_file,m_pInTerm);
	_pHubbingData->GetTransferDest()->getGroup(nIndex)->setMinTransferTime(pProbDist);

	return true;
}

//read transfer window
bool CHubbingDatabase::ReadTransferWindow( ArctermFile& _file, CHubbingData* _pHubbingData, int nIndex )
{
	if( !_file.getLine() )
		return false;

	// type of data
	char szDataType[64];
	_file.getField( szDataType, SMALL );
	if( strcmp( szDataType, "TRANSFER_WINDOW" ) != 0 )
		return false;

	// pax type
	CPassengerConstraint aPaxConst(m_pInTerm);
	aPaxConst.SetInputTerminal( m_pInTerm );
	aPaxConst.readConstraint (_file);

	int nEqualIndex = FindEqual( aPaxConst );
	if( nEqualIndex == -1 )
	{
		_pHubbingData->SetConstraint( aPaxConst );
	}

	// unit
	char szUnit[64];
	_file.getField( szUnit, SMALL );

	// prob dist
	ProbabilityDistribution* pProbDist = GetTerminalRelateProbDistribution (_file,m_pInTerm);
	_pHubbingData->GetTransferDest()->getGroup(nIndex)->setTransferWindow(pProbDist);

	return true;

}

//read old file data
void CHubbingDatabase::readObsoleteData (ArctermFile& p_file)
{

	clear();
	p_file.reset();
	p_file.skipLines (3);

	ProbabilityDistribution* pMinTime;//save the min_Time
	ProbabilityDistribution* pTransferWindow;//save the transfer_Window

	while (p_file.getLine())
	{
		// type of data
		char szDataType[64];
		p_file.getField( szDataType, SMALL );

		// pax type
		CPassengerConstraint aPaxConst(m_pInTerm);
		aPaxConst.SetInputTerminal( m_pInTerm );
		aPaxConst.readConstraint (p_file);

		// unit
		char szUnit[64];
		p_file.getField( szUnit, SMALL );

		// prob dist
		ProbabilityDistribution* pProbDist =GetTerminalRelateProbDistribution (p_file,m_pInTerm);


		// find the pax type
		int nEqualIndex = FindEqual( aPaxConst );
		if( nEqualIndex == -1 )
		{
			// new entry
			CHubbingData* pHubbingData = new CHubbingData;
			pHubbingData->SetConstraint( aPaxConst );
			addItem( pHubbingData );
		}		
		nEqualIndex = FindEqual( aPaxConst );
		assert( nEqualIndex >= 0 );

		// now set the specific value.
		if( strcmp( szDataType, "TRANSITING" ) == 0 )
			getItem( nEqualIndex )->SetTransiting( pProbDist );
		else if( strcmp( szDataType, "TRANSFERRING" ) == 0 )
			getItem( nEqualIndex )->SetTransferring( pProbDist );
		else if( strcmp( szDataType, "MIN_TRANSFER_TIME" ) == 0 )
		{
			pMinTime = pProbDist;
		}
		else if( strcmp( szDataType, "TRANSFER_WINDOW" ) == 0 )
		{
			pTransferWindow = pProbDist;
		}
		else if( strcmp( szDataType, "TRANSFER_DESTINATIONS" ) == 0 )
		{
			getItem( nEqualIndex )->SetTransferDest( (FlightTypeDistribution*)pProbDist );
			// put the old min_transfer_time & transfer_window into each destination
			int nCount = getItem(nEqualIndex)->GetTransferDest()->getCount();
			for (int i = 0; i < nCount; i++)
			{
				getItem(nEqualIndex)->GetTransferDest()->getGroup(i)->setMinTransferTime(pMinTime);
				getItem(nEqualIndex)->GetTransferDest()->getGroup(i)->setMinTransferTime(pTransferWindow);
			}
		}
	}

	WipeIntrinsic();

	// fill the empty item with default value
	FillEmptyWithDefault();
}



void CHubbingDatabase::writeData (ArctermFile& p_file) const
{
	assert( m_pInTerm );

	int nCount = getCount();

	//write the count of Hub Data
	p_file.writeInt(nCount);
	p_file.writeField("the number of Hub data");
	p_file.writeLine();

	for( int i=0; i<nCount; i++ )
	{
		CHubbingData* pHubbingData = getItem( i );
		CPassengerConstraint aConst = pHubbingData->GetConstraint();
		aConst.SetInputTerminal( m_pInTerm );

		FlightTypeDistribution* m_pTransferDest = pHubbingData->GetTransferDest();

		//write  Hub data 's Dis count
		int nCountDis = m_pTransferDest->getCount();
		p_file.writeInt(nCountDis);

		//write the "the number of  transfer Destination"
		p_file.writeField("the number of the Transfer Destination");
		p_file.writeLine();

		//write transit data
		p_file.writeField ("TRANSITING");
		aConst.writeConstraint (p_file);
		p_file.writeField( "PERCENT" );		
		ProbabilityDistribution* pTransiting = pHubbingData->GetTransiting();
		pTransiting->writeDistribution( p_file );
		p_file.writeLine();

		//write transfer data
		p_file.writeField ("TRANSFERRING");
		aConst.writeConstraint (p_file);
		p_file.writeField( "PERCENT" );		
		ProbabilityDistribution* m_pTransferring = pHubbingData->GetTransferring();
		m_pTransferring->writeDistribution( p_file );
		p_file.writeLine();

		if( nCountDis > 0 )// write destination data if the count of destination > 0
		{
			p_file.writeField ("TRANSFER_DESTINATIONS");
			aConst.writeConstraint (p_file);
			p_file.writeField( "" );		
			m_pTransferDest->writeDistribution( p_file );
			p_file.writeLine();

			for( int j = 0; j < nCountDis; j++ )
			{
				//write min transfer time of destination 
				p_file.writeField ("MIN_TRANSFER_TIME");
				aConst.writeConstraint (p_file);
				p_file.writeField( "MINUTES" );		
				ProbabilityDistribution* m_pMinTransferTime = pHubbingData->GetTransferDest()->getGroup(j)->getMinTransferTime();
				m_pMinTransferTime->writeDistribution( p_file );
				p_file.writeLine();

				//write transfer window of the destination
				p_file.writeField ("TRANSFER_WINDOW");
				aConst.writeConstraint (p_file);
				p_file.writeField( "MINUTES" );		
				ProbabilityDistribution* m_pTransferWindow = pHubbingData->GetTransferDest()->getGroup(j)->getTransferWindow();
				m_pTransferWindow->writeDistribution( p_file );
				p_file.writeLine();
			}
		}
	}
}


// find the item with same paxtype
int CHubbingDatabase::FindEqual( const CPassengerConstraint _aPaxConst )
{
	int nCount = getCount();
	for( int i = 0; i < nCount; i++ )
    {
		CPassengerConstraint aPaxConstraint = getItem( i )->GetConstraint();
		if( aPaxConstraint.isEqual( &_aPaxConst ) )	// the aPaxConstraint can hold the _aPaxConst
			return i;
    }
    return -1;	
}



// looking for the closest match, return -1 for not found.
int CHubbingDatabase::FindBestMatch( const CPassengerConstraint& _aPaxConst ) const
{
	int nCount = getCount();
	for( int i = 0; i < nCount; i++ )
    {
		CPassengerConstraint aPaxConstraint = getItem( i )->GetConstraint();
		if( aPaxConstraint.fits( _aPaxConst ) )	// the aPaxConstraint can hold the _aPaxConst
			return i;
    }
    return -1;	
}


// removes intrinsic type from all constraints
void CHubbingDatabase::WipeIntrinsic()
{
	int nCount = getCount();
	for( int i = 0; i < nCount; i++ )
    {
		getItem( i )->WipeIntrinsic();
    }
}



float CHubbingDatabase::getTransitPercent (const CPassengerConstraint& _type) const
{
	int nMatchIndex = FindBestMatch( _type );
	if( nMatchIndex < 0 )
		return 0;

	CHubbingData* pHubbingData = getItem( nMatchIndex );
	const ProbabilityDistribution* pDist = pHubbingData->GetTransiting();
	return (pDist)? (float)pDist->getRandomValue(): 0;
}

float CHubbingDatabase::getTransferPercent (const CPassengerConstraint& _type) const
{
	int nMatchIndex = FindBestMatch( _type );
	if( nMatchIndex < 0 )
		return 0;

	CHubbingData* pHubbingData = getItem( nMatchIndex );
	const ProbabilityDistribution* pDist = pHubbingData->GetTransferring();
	return (pDist)? (float)pDist->getRandomValue(): 0;
}

ElapsedTime CHubbingDatabase::getMinTransferTime (const CPassengerConstraint& _type, const FlightConstraint& flight) const
{
	int nMatchIndex = FindBestMatch( _type );
	if( nMatchIndex < 0 )
		return 0l;

	CHubbingData* pHubbingData = getItem( nMatchIndex );
	int nFlightDex = pHubbingData->FindBestFlight(flight);
	const ProbabilityDistribution* pDist = pHubbingData->GetTransferDest()->getGroup(nFlightDex)->getMinTransferTime();

    ElapsedTime minXferTime = 0l;
	if( pDist )
		minXferTime = (float)(60.0 * pDist->getRandomValue());
    return minXferTime;
}

ElapsedTime CHubbingDatabase::getMinTransferTime (const CPassengerConstraint& _type) const
{
	int nMatchIndex = FindBestMatch( _type );
	if( nMatchIndex < 0 )
		return 0l;

	CHubbingData* pHubbingData = getItem( nMatchIndex );
	int nFlightDex = pHubbingData->FindBestFlight(_type.getOtherFlightConstraint());
	const ProbabilityDistribution* pDist = pHubbingData->GetTransferDest()->getGroup(nFlightDex)->getMinTransferTime();

	ElapsedTime minXferTime = 0l;
	if( pDist )
		minXferTime = (float)(60.0 * pDist->getRandomValue());
	return minXferTime;
}

ElapsedTime CHubbingDatabase::getTransferWindow (const CPassengerConstraint& _type, const FlightConstraint& flight) const
{
	int nMatchIndex = FindBestMatch( _type );
	if( nMatchIndex < 0 )
		return 0l;

	CHubbingData* pHubbingData = getItem( nMatchIndex );
	int nFlightDex = pHubbingData->FindBestFlight(flight);
	const ProbabilityDistribution* pDist = pHubbingData->GetTransferDest()->getGroup(nFlightDex)->getTransferWindow();
    ElapsedTime xferWindow = 0l;
	if( pDist )
		xferWindow = (float)( 60.0 * pDist->getRandomValue() );
    return xferWindow;
}

ElapsedTime CHubbingDatabase::getTransferWindow (const CPassengerConstraint& _type) const
{
	int nMatchIndex = FindBestMatch( _type );
	if( nMatchIndex < 0 )
		return 0l;

	CHubbingData* pHubbingData = getItem( nMatchIndex );
	int nFlightDex = pHubbingData->FindBestFlight(_type.getOtherFlightConstraint());
	const ProbabilityDistribution* pDist = pHubbingData->GetTransferDest()->getGroup(nFlightDex)->getTransferWindow();
	ElapsedTime xferWindow = 0l;
	if( pDist )
		xferWindow = (float)( 60.0 * pDist->getRandomValue() );
	return xferWindow;
}



Flight* CHubbingDatabase::getDepartingFlight (const CPassengerConstraint& _type, int _ndx, FlightSchedule* _pFlightSchedule ) const
{
	int nMatchIndex = FindBestMatch( _type );
	if( nMatchIndex < 0 )
		return getRandomDeparture( _type, _ndx, _pFlightSchedule );

	CHubbingData* pHubbingData = getItem( nMatchIndex );
	FlightTypeDistribution* pDist = (FlightTypeDistribution*)pHubbingData->GetTransferDest();
    if( pDist == NULL )
		return getRandomDeparture( _type, _ndx, _pFlightSchedule );


	assert( m_pInTerm );
    Flight *aFlight = _pFlightSchedule->getItem( _ndx );
  //  ElapsedTime startTime = aFlight->getArrTime() + getMinTransferTime( _type );
   // ElapsedTime endTime = startTime + getTransferWindow( _type );

    const FlightConstraint *destType;
    Flight *destFlight;

    // try a random series
    for (int i = 0; i < pDist->getCount(); i++)
    {
	    CTransferFlightConstraint *pTransferFlght;
        pTransferFlght = pDist->getDestinationGroup (RANDOM);
        destType = pTransferFlght->getFlightConstraint();

		ProbabilityDistribution* pMinTime;
		pMinTime = pTransferFlght->getMinTransferTime();

		ElapsedTime minXferTime = 0l;
		if( pMinTime )
			minXferTime = (float)(60.0 * pMinTime->getRandomValue());
		ElapsedTime startTime = aFlight->getArrTime() + minXferTime;

		ProbabilityDistribution* pTransferWindow;
		pTransferWindow = pTransferFlght->getTransferWindow();

		ElapsedTime xferWindow = 0l;
		if( pTransferWindow )
			xferWindow = (float)(60.0 * pTransferWindow->getRandomValue());
		ElapsedTime endTime = startTime + xferWindow;

		  destFlight = _pFlightSchedule->getDeparture
				(startTime, endTime, *destType);
        if (destFlight) //Jan98 don't know why negative :  (!destFlight)
            return destFlight;

    }
    // try a sequential series
    for (int ii = 0; ii < pDist->getCount(); ii++)
    {
		CTransferFlightConstraint *pTransferFlght;
        pTransferFlght = pDist->getDestinationGroup (SEQUENTIAL);
		destType = pTransferFlght->getFlightConstraint();

		ProbabilityDistribution* pMinTime;
		pMinTime = pTransferFlght->getMinTransferTime();

		ElapsedTime minXferTime = 0l;
		if( pMinTime )
			minXferTime = (float)(60.0 * pMinTime->getRandomValue());
		ElapsedTime startTime = aFlight->getArrTime() + minXferTime;

		ProbabilityDistribution* pTransferWindow;
		pTransferWindow = pTransferFlght->getTransferWindow();

		ElapsedTime xferWindow = 0l;
		if( pTransferWindow )
			xferWindow = (float)(60.0 * pTransferWindow->getRandomValue());
		ElapsedTime endTime = startTime + xferWindow;

        destFlight = _pFlightSchedule->getDeparture
            (startTime, endTime, *destType);
        if (destFlight)
            return destFlight;
    }
    return NULL;
}



Flight* CHubbingDatabase::getRandomDeparture (const CPassengerConstraint& _type, int _ndx, FlightSchedule* _pFlightSchedule) const
{
	assert( m_pInTerm );
	Flight *aFlight = _pFlightSchedule->getItem (_ndx);
	ElapsedTime startTime = aFlight->getArrTime() + getMinTransferTime (_type);
	ElapsedTime endTime = startTime + getTransferWindow (_type);
	return _pFlightSchedule->getRandomDeparture (startTime, endTime);
}


// delete all references to the passed index at p_level
void CHubbingDatabase::deletePaxType (int p_level, int p_index)
{
	int nCount = getCount();
	for( int i = nCount - 1; i >= 0; i-- )
	{
		if( getItem(i)->GetConstraint().contains( p_level, p_index ) )
		{
			deleteItem (i);
		}
	}
}


// fill the empty item with default value
void CHubbingDatabase::FillEmptyWithDefault()
{
	int nCount = getCount();
	for( int i=0; i<nCount; i++ )
		getItem( i )->FillEmptyWithDefault();
}

