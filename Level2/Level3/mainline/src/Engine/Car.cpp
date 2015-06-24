// Car.cpp: implementation of the CCar class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Car.h"
#include "..\common\termfile.h"
#include "hold.h"
//#include "common\path.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#define ENTRY_DOOR_QUEUE_OFFSET 150.0

CCar::CCar():m_iCapacity(100),m_carArea(NULL)//,m_preBoardingArea(NULL)
{
	m_carArea=new HoldingArea();
//m_preBoardingArea=new HoldingArea();
	m_pTerm = NULL;
}
CCar::CCar(const CCar & _car)
{
	m_carArea=new HoldingArea( *(_car.m_carArea) );
	DOOR::const_iterator iter=_car.m_entryDoors.begin();
	DOOR::const_iterator iterLast=_car.m_entryDoors.end();
	for (; iter!=iterLast; ++iter)
	{
		//since processor has no copy constructor ,doing this may cause some error
		Processor* pNewDoor=new Processor(**iter);
		m_entryDoors.push_back(pNewDoor);

	}

	iter=_car.m_exitDoors.begin();
	iterLast=_car.m_exitDoors.end();
	for(; iter!=iterLast; ++iter)
	{
		Processor* pNewDoor=new Processor(**iter);
		m_exitDoors.push_back(pNewDoor);
	}

	m_iCapacity=_car.m_iCapacity;
	m_pTerm=_car.m_pTerm;
	m_nPaxOnBoard = 0;

}
CCar::~CCar()
{
	//delete m_preBoardingArea;
	delete m_carArea;

	DOOR ::iterator iter;
	for(iter=m_entryDoors.begin();iter!=m_entryDoors.end();++iter)
		delete (*iter);
	m_entryDoors.clear();

	for(iter=m_exitDoors.begin();iter!=m_exitDoors.end();++iter)
		delete (*iter);
	m_exitDoors.clear();

}
BOOL CCar::IsAValidCar()
{
	if(m_carArea->serviceLocationLength()<3)
		return false;

//	if(m_preBoardingArea->serviceLocationLength()<3)
//		return false;

	DOOR::iterator iter=m_entryDoors.begin();
	DOOR::iterator iterLast=m_entryDoors.end();
	bool bHaveOne=false;
	for(;iter != iterLast; ++ iter)
	{
		//if((*iter)->queueLength() < 2 || (*iter)->outConstraintLength() < 2)
		if((*iter)->queueLength() < 2 )
			return false;
		bHaveOne=true;
	}
	if(!bHaveOne)
		return false;


	iter=m_exitDoors.begin();
	iterLast=m_exitDoors.end();
	bHaveOne=false;
	for(;iter != iterLast; ++ iter)
	{
		if( (*iter)->outConstraintLength() < 2)
			return false;
		bHaveOne=true;
	}
	if(!bHaveOne)
		return false;
	return true;
} 
//serilized CCar 's info
int CCar::WriteAllProcessorInfo(ArctermFile& procFile) const
{
	assert( m_pTerm );
	m_carArea->SetTerminal( m_pTerm );
	m_carArea->writeProcessor(procFile);
	procFile.writeLine();



//	m_preBoardingArea->writeProcessor(procFile);

	procFile.writeInt((int)m_entryDoors.size());
	procFile.writeLine();

	DOOR ::const_iterator iter;
	for(iter= m_entryDoors.begin();iter!=m_entryDoors.end();++iter)
	{
		(*iter)->writeProcessor(procFile);
		procFile.writeLine();
	}

	procFile.writeInt((int)m_exitDoors.size());
	procFile.writeLine();
	
	for(iter=m_exitDoors.begin();iter!=m_exitDoors.end();++iter)
	{
		(*iter)->writeProcessor(procFile);
		procFile.writeLine();
	}

	procFile.writeInt( m_iCapacity );
	procFile.writeLine();

	return TRUE;
}

//read all cars data from files
int CCar::ReadAllProcessorInfo( ArctermFile& procFile )
{
	m_carArea->SetTerminal(m_pTerm);

	procFile.getLine();
	m_carArea->ReadProcessor(procFile);

//	m_preBoardingArea->SetTerminal(m_pTerm);
//	m_preBoardingArea->ReadProcessor(procFile);
	
	int iDoorNum=0;
	procFile.getLine();
	procFile.getInteger(iDoorNum);
	

	for(int i=0; i< iDoorNum; ++i)
	{
		Processor* pNewDoor=new Processor();
		pNewDoor->SetTerminal(m_pTerm);
		procFile.getLine();
		pNewDoor->ReadProcessor(procFile);
		m_entryDoors.push_back(pNewDoor);
	}

	procFile.getLine();
	procFile.getInteger(iDoorNum);
	

	for(int ii=0; ii< iDoorNum; ++ii)
	{
		Processor* pNewDoor=new Processor();
		pNewDoor->SetTerminal(m_pTerm);
		procFile.getLine();
		pNewDoor->ReadProcessor(procFile);
		m_exitDoors.push_back(pNewDoor);
	}

	procFile.getLine();
	procFile.getInteger( m_iCapacity );
	return TRUE;
}


HoldingArea* CCar::GetCarArea()
{
	return m_carArea;
}


void CCar::DoOffest( DistanceUnit _xOffset, DistanceUnit _yOffset)
{
	m_carArea->DoOffset( _xOffset, _yOffset );

	DOOR::iterator iter=m_entryDoors.begin();
	DOOR::iterator iterLast=m_entryDoors.end();
	for(; iter!=iterLast; ++iter)
	{
		(*iter)->DoOffset( _xOffset, _yOffset );
	}

	iter=m_exitDoors.begin();
	iterLast=m_exitDoors.end();
	for(; iter!=iterLast; ++iter)
	{
		(*iter)->DoOffset( _xOffset, _yOffset );
	}

}

void CCar::Mirror(const Path* _p)
{
	if(m_carArea)
		m_carArea->Mirror(_p);
	DOOR::iterator iter;
	for(iter = m_entryDoors.begin(); iter != m_entryDoors.end(); ++iter) {
		(*iter)->Mirror(_p);
	}

	for(iter=m_exitDoors.begin(); iter!=m_exitDoors.end(); ++iter) {
		(*iter)->Mirror(_p);
	}
}

void CCar::Rotate (DistanceUnit degrees)
{
	m_carArea->Rotate( degrees );

	DOOR::iterator iter=m_entryDoors.begin();
	DOOR::iterator iterLast=m_entryDoors.end();
	for(; iter!=iterLast; ++iter)
	{
		(*iter)->Rotate( degrees );
	}

	iter=m_exitDoors.begin();
	iterLast=m_exitDoors.end();
	for(; iter!=iterLast; ++iter)
	{
		(*iter)->Rotate( degrees );
	}
}
Processor* CCar::GetEntryDoor( int _nIdx )
{ 
	return m_entryDoors[_nIdx]; 
}


int CCar::GetEntryDoorCount() const 
{ 
	return m_entryDoors.size(); 
}


Processor* CCar::GetExitDoor( int _nIdx )
{ 
	return m_exitDoors[_nIdx]; 
}


int CCar::GetExitDoorCount() const
{ 
	return m_exitDoors.size(); 
}

void CCar::LeaveStation()
{
	m_nPaxOnBoard = 0;
}

void CCar::AddPax(int _nGroupCount)
{
	m_nPaxOnBoard += _nGroupCount;
}

void CCar::DeletePax(int _nGroupCount)
{
	m_nPaxOnBoard -= _nGroupCount;
}

int CCar::GetPaxCount()
{
	return m_nPaxOnBoard;
}

void CCar::DeleteEntryDoor( int _nIdx )
{
	ASSERT( _nIdx>=0 && (UINT)_nIdx<m_entryDoors.size() );	
	DOOR::iterator iter=m_entryDoors.begin() + _nIdx;
	if(*iter)
		delete *iter;
	m_entryDoors.erase( iter );
	

}

void CCar::DeleteExitDoor( int _nIdx )
{
	ASSERT( _nIdx>=0 && (UINT)_nIdx<m_exitDoors.size() );	
	DOOR::iterator iter=m_exitDoors.begin() + _nIdx;

	if(*iter)
		delete *iter;
	m_exitDoors.erase( iter );
}
void CCar::CorrectZ( DistanceUnit _dZ )
{
	int iCarSeviceArea = m_carArea->serviceLocationLength();
	Point* pCarService = m_carArea->serviceLocationPath()->getPointList();
	for( int i=0; i<iCarSeviceArea; ++i )
	{
		pCarService[i].setZ( _dZ );
	}
	
	//for( int i=0;)
	DOOR::iterator iter=m_entryDoors.begin();
	DOOR::iterator iterLast=m_entryDoors.end();
	for(; iter!=iterLast; ++iter)
	{
		(*iter)->serviceLocationPath()->getPointList()->setZ( _dZ );
		Point* pQueuePtList = (*iter)->queuePath()->getPointList();
		int iQueuePtCount = (*iter)->queuePath()->getCount();
		for( int i=0; i<iQueuePtCount; ++i )
		{
			pQueuePtList[ i ].setZ( _dZ );
		}
	}

	iter=m_exitDoors.begin();
	iterLast=m_exitDoors.end();
	for(; iter!=iterLast; ++iter)
	{
		(*iter)->serviceLocationPath()->getPointList()->setZ( _dZ );
		Point* pOutConPtList = (*iter)->outConstraint()->getPointList(); 
		int iOutConPtCount = (*iter)->outConstraint()->getCount();
		for( int i=0; i<iOutConPtCount; ++i )
		{
			pOutConPtList[ i ].setZ( _dZ );
		}
	}
}
bool CCar::ComputeDoorInfo(int _nEntryDoorNum, int _nExitDoorNum, int _nExitDoorType , double _dCarLength, double _dPlatFormWidth )
{
	int nDoorNumDelta = _nEntryDoorNum - m_entryDoors.size();
	if( nDoorNumDelta >0 )
	{
		for( int i=1; i<=nDoorNumDelta; ++i )
		{
			Processor *pNewProcessor = new Processor();
			pNewProcessor->SetTerminal(m_pTerm);
			m_entryDoors.push_back( pNewProcessor );
		}
	}
	else if ( nDoorNumDelta < 0 )
	{
		int nLowIndex = m_entryDoors.size()+nDoorNumDelta;
		for( int i=m_entryDoors.size()-1;i>=nLowIndex; --i)
		{
			delete m_entryDoors[i];
			m_entryDoors.erase( m_entryDoors.begin() + i );
		}
	}

	nDoorNumDelta = _nExitDoorNum - m_exitDoors.size();

	if( nDoorNumDelta >0 )
	{
		for( int i=1; i<=nDoorNumDelta; ++i )
		{
			Processor *pNewProcessor = new Processor();
			pNewProcessor->SetTerminal(m_pTerm);
			m_exitDoors.push_back( pNewProcessor );
		}
	}
	else if ( nDoorNumDelta < 0 )
	{
		int nLowIndex = m_exitDoors.size()+nDoorNumDelta;
		for( int i=m_exitDoors.size()-1;i>=nLowIndex; --i)
		{
			delete m_exitDoors[i];
			m_exitDoors.erase( m_exitDoors.begin() + i );
		}
	}

	//platform side
	if( _nExitDoorType )
	{
		double dSinglDoorDistance =  _dCarLength /( _nEntryDoorNum + _nExitDoorNum +1 );
		Point basePoint = m_carArea->serviceLocationPath()->getPoint( 3 );
		Point doorLocationAndQueue [2];
		doorLocationAndQueue[0]=basePoint;
		doorLocationAndQueue[1]=basePoint;
		double _y=basePoint.getY() - PLATFORM_CAR_DISTANCE - _dPlatFormWidth/3 ;
		doorLocationAndQueue[1].setY( _y );
		
		for( int i=0;i<_nEntryDoorNum ;++i )
		{
			doorLocationAndQueue[0].DoOffset( dSinglDoorDistance,0,0 );
			doorLocationAndQueue[1].DoOffset( dSinglDoorDistance,0,0 );
			m_entryDoors[i]->initServiceLocation(1,doorLocationAndQueue);

			doorLocationAndQueue[0].setY( basePoint.getY() - PLATFORM_CAR_DISTANCE - ENTRY_DOOR_QUEUE_OFFSET );
			m_entryDoors[i]->initQueue(0, 2, doorLocationAndQueue );
			doorLocationAndQueue[0].setY( basePoint.getY() );
		}

		for(int ii=0;ii<_nExitDoorNum; ++ii )
		{
			doorLocationAndQueue[0].DoOffset( dSinglDoorDistance,0,0 );
			doorLocationAndQueue[1].DoOffset( dSinglDoorDistance,0,0 );
			m_exitDoors[ii]->initServiceLocation(1,doorLocationAndQueue);
			doorLocationAndQueue[0].setY( basePoint.getY() - PLATFORM_CAR_DISTANCE - ENTRY_DOOR_QUEUE_OFFSET );
			m_exitDoors[ii]->initOutConstraint( 2, doorLocationAndQueue );
			doorLocationAndQueue[0].setY( basePoint.getY() );
			
		}
	}
	else
	{
		double dSinglDoorDistance =  _dCarLength /( _nEntryDoorNum +1 );
		Point basePoint = m_carArea->serviceLocationPath()->getPoint( 3 );
		Point doorLocationAndQueue [2];
		doorLocationAndQueue[0]=basePoint;
		doorLocationAndQueue[1]=basePoint;
		double _y=basePoint.getY() - PLATFORM_CAR_DISTANCE - _dPlatFormWidth/3 ;
		doorLocationAndQueue[1].setY( _y );
		
		for( int i=0;i<_nEntryDoorNum ;++i )
		{
			doorLocationAndQueue[0].DoOffset( dSinglDoorDistance,0,0 );
			doorLocationAndQueue[1].DoOffset( dSinglDoorDistance,0,0 );
			m_entryDoors[i]->initServiceLocation(1,doorLocationAndQueue);
			doorLocationAndQueue[0].setY( basePoint.getY() - PLATFORM_CAR_DISTANCE - ENTRY_DOOR_QUEUE_OFFSET );
			m_entryDoors[i]->initQueue(0, 2, doorLocationAndQueue );
			doorLocationAndQueue[0].setY( basePoint.getY() );
		}



		dSinglDoorDistance = _dCarLength / ( _nExitDoorNum +1 );
		basePoint = m_carArea->serviceLocationPath()->getPoint( 0 );
		doorLocationAndQueue[0]=basePoint;
		doorLocationAndQueue[1]=basePoint;	
		_y=basePoint.getY() +PLATFORM_CAR_DISTANCE + _dPlatFormWidth/3 ;
		doorLocationAndQueue[1].setY( _y);

		for(int ii=0;ii<_nExitDoorNum; ++ii )
		{
			doorLocationAndQueue[0].DoOffset( dSinglDoorDistance,0,0 );
			doorLocationAndQueue[1].DoOffset( dSinglDoorDistance,0,0 );
			m_exitDoors[ii]->initServiceLocation(1,doorLocationAndQueue);
			m_exitDoors[ii]->initOutConstraint( 2, doorLocationAndQueue );
		}
	}

	return true;
}

// Clean all sim engine related data, for the new sim process
void CCar::CleanData()
{
	int nEntryDoorCount = GetEntryDoorCount();
	for( int i=0; i<nEntryDoorCount; i++ )
	{
		Processor* pEntryDoor = GetEntryDoor( i );
		pEntryDoor->CleanData();
	}

	int nExitDoorCount = GetExitDoorCount();
	for( int ii=0; ii<nExitDoorCount; ii++ )
	{
		Processor* pExitDoor = GetExitDoor( ii );
		pExitDoor->CleanData();
	}

	m_carArea->CleanData();
}



int CCar::clearQueue (ElapsedTime _curTime)
{
	int nResCount = 0;
	int nEntryDoorCount = GetEntryDoorCount();
	for( int i=0; i<nEntryDoorCount; i++ )
	{
		Processor* pEntryDoor = GetEntryDoor( i );
		nResCount += pEntryDoor->clearQueue( _curTime );
	}

	int nExitDoorCount = GetExitDoorCount();
	for(int ii=0; ii<nExitDoorCount; ii++ )
	{
		Processor* pExitDoor = GetExitDoor( ii );
		nResCount += pExitDoor->clearQueue( _curTime );
	}

	nResCount += m_carArea->clearQueue( _curTime );
	return nResCount;
}


void CCar::removePerson( const Person* _pPerson )
{
	int nEntryDoorCount = GetEntryDoorCount();
	for( int i=0; i<nEntryDoorCount; i++ )
	{
		Processor* pEntryDoor = GetEntryDoor( i );
		pEntryDoor->removePerson(_pPerson );
	}

	int nExitDoorCount = GetExitDoorCount();
	for(int ii=0; ii<nExitDoorCount; ii++ )
	{
		Processor* pExitDoor = GetExitDoor( ii );
		pExitDoor->removePerson( _pPerson );
	}

	m_carArea->removePerson( _pPerson );
}
