// AllCarSchedule.cpp: implementation of the CAllCarSchedule class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AllCarSchedule.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAllCarSchedule::CAllCarSchedule():DataSet(AllCarsScheduleFile, (float)2.3)
{
	m_pSystemRailWay=NULL;
	m_pSystemProcessorlist = NULL;
}
CAllCarSchedule::CAllCarSchedule(const CAllCarSchedule& _anotherSchedule):DataSet(AllCarsScheduleFile)
{
	ALLCARSCHEDULE::const_iterator iter=_anotherSchedule.m_allCarSchedule.begin();
	ALLCARSCHEDULE::const_iterator iterLast=_anotherSchedule.m_allCarSchedule.end();
	for(; iter!=iterLast; ++iter )
	{
		CSingleCarSchedule* pSingleSchedule=new CSingleCarSchedule();
		*pSingleSchedule=**iter;
		m_allCarSchedule.push_back(pSingleSchedule);
	}

}
CAllCarSchedule& CAllCarSchedule::operator = (const CAllCarSchedule& _anotherSchedule)
{
	if( this!= &_anotherSchedule)
	{
		ALLCARSCHEDULE::const_iterator iter=m_allCarSchedule.begin();
		ALLCARSCHEDULE::const_iterator iterLast=m_allCarSchedule.end();
		for(; iter!=iterLast; ++iter )
		{
			delete (*iter);
		}
		m_allCarSchedule.clear();

		iter=_anotherSchedule.m_allCarSchedule.begin();
		iterLast=_anotherSchedule.m_allCarSchedule.end();
		for(; iter!=iterLast; ++iter )
		{
			CSingleCarSchedule* pSingleSchedule=new CSingleCarSchedule();
			*pSingleSchedule=**iter;
			m_allCarSchedule.push_back(pSingleSchedule);
		}
	}

	return *this;
}
CAllCarSchedule::~CAllCarSchedule()
{
	ClearCarSchedule();	
}

bool CAllCarSchedule::IsCarAlreadyExist(CString _sCarName)
{
	ALLCARSCHEDULE::const_iterator iter=m_allCarSchedule.begin();
	ALLCARSCHEDULE::const_iterator iterLast=m_allCarSchedule.end();
	for(; iter!=iterLast; ++iter)
	{
		if( _sCarName == (*iter)->GetCarName() )
			return true;
	}

	return false;
}

//check if all car flow is valid
bool CAllCarSchedule::IsAllCarFlowValid(TrainTrafficGraph* _pTrafficGraph) const
{
	ALLCARSCHEDULE::const_iterator iter=m_allCarSchedule.begin();
	ALLCARSCHEDULE::const_iterator iterLast=m_allCarSchedule.end();
	for(; iter!=iterLast; ++iter)
	{
		if( !(*iter)->IsAValidCarFlow(_pTrafficGraph) )
			return false;
	}

	return true;
}

void CAllCarSchedule::readData (ArctermFile& p_file)
{
	p_file.getLine();
	int iCount=0;
	p_file.getInteger( iCount );

	m_allCarSchedule.clear();
	for( int i=0; i<iCount; ++i )
	{
		CSingleCarSchedule* pSingleSchedule=new CSingleCarSchedule();
		pSingleSchedule->SetSystemProcessorList( m_pSystemProcessorlist );
		pSingleSchedule->SetSystemRailWay( m_pSystemRailWay );
		pSingleSchedule->ReadData( p_file );

		m_allCarSchedule.push_back( pSingleSchedule );
	}
}

void CAllCarSchedule::readObsoleteData ( ArctermFile& p_file )
{
	p_file.getLine();
	int iCount=0;
	p_file.getInteger( iCount );

	m_allCarSchedule.clear();
	for( int i=0; i<iCount; ++i )
	{
		CSingleCarSchedule* pSingleSchedule=new CSingleCarSchedule();
		pSingleSchedule->SetSystemProcessorList( m_pSystemProcessorlist );
		pSingleSchedule->SetSystemRailWay( m_pSystemRailWay );
		pSingleSchedule->readObsoleteData( p_file );

		m_allCarSchedule.push_back( pSingleSchedule );
	}
}

void CAllCarSchedule::writeData (ArctermFile& p_file) const
{
	p_file.writeInt( (int)m_allCarSchedule.size() );
	p_file.writeLine();
	ALLCARSCHEDULE::const_iterator iter=m_allCarSchedule.begin();
	ALLCARSCHEDULE::const_iterator iterLast=m_allCarSchedule.end();
	for(; iter!=iterLast; ++iter)
	{
		(*iter)->WriteData( p_file );
	}
}

void CAllCarSchedule::clear (void)
{
//	m_
}

bool CAllCarSchedule::SelectBestSchedule( IntegratedStation* _pCurrentStation, IntegratedStation* _pDesStation, ElapsedTime _currentTime, std::vector<CViehcleMovePath>& _bestMovePath )
{
	char temptime[16];
	_currentTime.printTime(temptime );
//	// TRACE("\n current Station =%d\n dest station=%d\n  time= %s\n", _pCurrentStation->getIndex(), _pDesStation->getIndex(),temptime );
	ALLCARSCHEDULE::const_iterator iter=m_allCarSchedule.begin();
	ALLCARSCHEDULE::const_iterator iterLast=m_allCarSchedule.end();
	ElapsedTime nearestTime;
	bool bNearestTimeNoSetyet = true;
	ElapsedTime tempTime;
	ElapsedTime arriveCurStationTime ;
	int iBestScheduleIndex=-1;

	for( int i=0 ; iter!=iterLast; ++iter,++i)
	{
		if( (*iter)->GetNearestSchedule( _pCurrentStation, _pDesStation, _currentTime, arriveCurStationTime,tempTime ) )
		{
			if( nearestTime > tempTime || bNearestTimeNoSetyet )
			{
				bNearestTimeNoSetyet = false;
				nearestTime = tempTime;
				iBestScheduleIndex = i;
			}
		}
		
	}

	arriveCurStationTime.printTime( temptime );
//	// TRACE("\n arriveCurStationTime = %s \n ",temptime );
	
	if( iBestScheduleIndex > -1)
		return m_allCarSchedule[ iBestScheduleIndex ]->SelectBestScheduleCircualtion( _pCurrentStation, _pDesStation, arriveCurStationTime, _bestMovePath );
	else
		return false;
}
bool CAllCarSchedule::DeleteCarSchedule( int _scheduleIndex )
{
	ASSERT( _scheduleIndex >=0 && _scheduleIndex < static_cast<int>(m_allCarSchedule.size()) );
	ALLCARSCHEDULE::iterator iterBegin = m_allCarSchedule.begin();
	ALLCARSCHEDULE::iterator iterDelete = iterBegin + _scheduleIndex;
	delete *iterDelete;
	m_allCarSchedule.erase( iterDelete );
	return true;
}

void CAllCarSchedule::GetStationTimeTable( IntegratedStation* _pCurrentStation, TimeTableVector& _timeTable)
{
	_timeTable.clear();
	
	ALLCARSCHEDULE::const_iterator iter=m_allCarSchedule.begin();
	ALLCARSCHEDULE::const_iterator iterLast=m_allCarSchedule.end();
	for(; iter!=iterLast; ++iter)
	{
		(*iter)->GetStationTimeTable( _pCurrentStation , _timeTable );
	}
}

void CAllCarSchedule::AdjustRailWay( TrainTrafficGraph* _pTraffic  )
{
	ALLCARSCHEDULE::const_iterator iter=m_allCarSchedule.begin();
	std::vector<int> eraseFlag;

	for(int i=0 ; iter!=m_allCarSchedule.end(); ++iter,++i )
	{
		if ( (*iter)->AdjustRailway( _pTraffic ) )
		{
			delete *iter;
			eraseFlag.push_back( i );
		}
	}

	for(int ii=eraseFlag.size() -1 ; ii>=0 ; --ii )
	{
		m_allCarSchedule.erase( m_allCarSchedule.begin() + eraseFlag[ii] );
	}
	
}

void CAllCarSchedule::SetSystemRailWay( CRailWayData* _pRailWaySys)
{
	m_pSystemRailWay=_pRailWaySys;	
	ALLCARSCHEDULE::const_iterator iter=m_allCarSchedule.begin();
	ALLCARSCHEDULE::const_iterator iterLast=m_allCarSchedule.end();
	for(; iter!=iterLast; ++iter)
	{
		(*iter)->SetSystemRailWay( _pRailWaySys );
	}
}

// delete all schedule which use this station
// update all schedule which do not use this staiton
void CAllCarSchedule::StationIsDeleted( int _iDeletedStationIdx )
{
	ALLCARSCHEDULE::const_iterator iter=m_allCarSchedule.begin();
	ALLCARSCHEDULE::const_iterator iterLast=m_allCarSchedule.end();
	std::vector<int> eraseFlag;
	for( int i=0 ; iter!=iterLast; ++iter, ++i )
	{
		if ( (*iter)->IsUseThisStation( _iDeletedStationIdx ) )
		{
			delete *iter;
			eraseFlag.push_back( i );
		}
		else
		{
			(*iter)->AdjustRailwayKeys( _iDeletedStationIdx );
		}
	}

	for( int ii=eraseFlag.size() -1 ; ii>=0 ; --ii )
	{
		m_allCarSchedule.erase( m_allCarSchedule.begin() + eraseFlag[ii] );
	}
}

// test if there is at least one sechedule can take people from source station to dest station
bool CAllCarSchedule::IfHaveScheduleBetweenStations( IntegratedStation* _pSourceStation, IntegratedStation* _pDesStation )const
{
	ALLCARSCHEDULE::const_iterator iter=m_allCarSchedule.begin();
	ALLCARSCHEDULE::const_iterator iterLast=m_allCarSchedule.end();
	std::vector<int> eraseFlag;
	for( int i=0 ; iter!=iterLast; ++iter, ++i )
	{
		if( (*iter)->IfScheduleBetweenStations( _pSourceStation, _pDesStation ) )
		{
			return true;
		}
	}
	
	return false;
}

void CAllCarSchedule::ClearCarSchedule()
{
	ALLCARSCHEDULE::const_iterator iter=m_allCarSchedule.begin();
	ALLCARSCHEDULE::const_iterator iterLast=m_allCarSchedule.end();
	for(; iter!=iterLast; ++iter )
	{
		delete (*iter);
	}
	m_allCarSchedule.clear();
}