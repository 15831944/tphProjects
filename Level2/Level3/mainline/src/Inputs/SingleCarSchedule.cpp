// SingleCarSchedule.cpp: implementation of the CSingleCarSchedule class.
//
//////////////////////////////////////////////////////////////////////
//#define DeltaV 300
#include "stdafx.h"
#include "SingleCarSchedule.h"
#include "..\engine\proclist.h"
#include "RailWayData.h"
#include "../Common/EchoSystem.h"

#include "common\distancepath.h"
#include "common\states.h"
#include "in_term.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#include <algorithm>
using namespace std;
CSingleCarSchedule::CSingleCarSchedule()
{
	m_iTrainNumber = 0 ;
	m_turnArroundTime = 0l ;
	m_headWayTime = 0l ;
	m_dNormalSpeed = 0;
	m_dAccelerateSpeed = 0;
	m_dDecelerateSpeed = 0;
	m_pSystemProcessorList= NULL;
	m_iOneDayCount= 0 ;
}

CSingleCarSchedule::~CSingleCarSchedule()
{

}

//add _preStation's next station to the car flow list,if _preStation is null, add into list head
bool CSingleCarSchedule::AddNextStation( IntegratedStation* _preStation,  IntegratedStation* _nextStation, bool _pushToBack)
{
	if(!_preStation)
	{
		m_carFlow.push_front( _nextStation );
//		m_linkedRailWayFlag.push_front(false);
		return true;
	}
	else
	{
		if( _pushToBack )
		{
			m_carFlow.push_back( _nextStation );
			return true;
		}
		else
		{
			CARFLOW::iterator iter= std::find( m_carFlow.begin(), m_carFlow.end(),_preStation);
			if( iter!=m_carFlow.end() )//find it
			{
				m_carFlow.insert(++iter,_nextStation);
				return true;
			}
			else
				return false;
		}
	}
}

//check if defined car flow is valid
bool CSingleCarSchedule::IsAValidCarFlow(TrainTrafficGraph* _pTrafficGraph) const
{
	if(m_carFlow.size() < 3)
		return false;

	CARFLOW::const_iterator iterNext,iter=m_carFlow.begin();
	CARFLOW::const_iterator iterLast=m_carFlow.end();
	
	if( iter!=iterLast )
	{
		--iterLast;
		if( (*iter) != (*iterLast) )
		{
			AfxMessageBox(" Start Station must be equal with End Station!");
			return false;
		}
		iterLast = m_carFlow.end();
	}

	RAILWAY::const_iterator iterRailway = m_allRailWay.begin();
	RAILWAY::const_iterator iterRailwayEnd = m_allRailWay.end();

//	char stationName [32];
	for(; iter!=iterLast; ++iter,++iterRailway)
	{
		iterNext=iter;
		++iterNext;
		if(iterNext!=iterLast)
		{
//			(*iter)->getID()->printID( stationName );
//			// TRACE("\n first station = %s\n", stationName );
//			(*iterNext)->getID()->printID( stationName );
//			// TRACE("\n second station = %s\n", stationName );
			if(! _pTrafficGraph->IsTwoStationDirectLinked( *iter, *iterNext))
			{
				return false;
			}
			else				
			{
				if( iterRailway != iterRailwayEnd )
				{
					int iLinkedType = (*iterRailway)->GetTypeFromFirstViewPoint();
					if ( ! _pTrafficGraph->GetStationLinkedRailWay( *iter, *iterNext, iLinkedType ) )
					{
						iLinkedType = (*iterRailway)->GetTypeFromSecondViewPoint();
						if ( ! _pTrafficGraph->GetStationLinkedRailWay( *iter, *iterNext, iLinkedType ) )
							return false;
					}
				}
			}
		}
		else
		{
			return true;
		}
	}

	return true;
}

void CSingleCarSchedule::readObsoleteData ( ArctermFile& p_file )
{
	p_file.getLine();
	char carName[256];
	p_file.getField( carName,256 );
	m_carName=carName;


	p_file.getTime( m_scheduleStartTime, TRUE );
	p_file.getTime( m_scheduleEndTime, TRUE );


	p_file.getFloat( m_dNormalSpeed );
	m_dNormalSpeed *= SCALE_FACTOR;
	p_file.getFloat(m_dAccelerateSpeed);
	m_dAccelerateSpeed *= SCALE_FACTOR;
	p_file.getFloat(m_dDecelerateSpeed);
	m_dDecelerateSpeed *= SCALE_FACTOR;

	p_file.getInteger(m_iTrainNumber);
	p_file.getTime( m_turnArroundTime, TRUE );
	p_file.getTime(m_headWayTime, TRUE);
	//m_headWayTime /= 60;
	p_file.getInteger(m_iOneDayCount);

	// car flow
	p_file.getLine();
	
	int iCount=0;
	p_file.getInteger( iCount );
	m_carFlow.clear();
	//if( p_f)
	for( int i=0; i<iCount; ++i )
	{
		int iProcIndex=-1;
		p_file.getInteger( iProcIndex );
		ASSERT( iProcIndex > -1 );
		m_carFlow.push_back( (IntegratedStation *)m_pSystemProcessorList->getProcessor( iProcIndex ) );
	}

	// move path
	p_file.getLine();
	p_file.getInteger( iCount );
	m_ViehcleMovePath.clear();
	for(int ii=0; ii<iCount; ++ii )
	{
		CViehcleMovePath movePath;
		movePath.ReadData( p_file );
		m_ViehcleMovePath.push_back( movePath );
	}	
	p_file.getLine();
	p_file.getTime( m_circleTime, TRUE );
	


	p_file.getLine();
	
	p_file.getInteger( iCount );

	for( int ii=0; ii<iCount; ++ii )
	{
		RailwayInfo tempRail;
		tempRail.ReadDataFromFile( p_file );

		RailwayInfoKey key ;
		key.SetFirstStationIndex( tempRail.GetFirstStationIndex() );
		key.SetSecondStationIndex( tempRail.GetSecondStationIndex() );
		key.SetTypeFromFirstViewPoint( tempRail.GetTypeFromFirstViewPoint() );

		
		RailwayInfo* pRailWay = m_pSystemRailWay->FindRailWayPointer( key );
		ASSERT( pRailWay );
		m_allRailWay.push_back( pRailWay );
		m_allRailWayKeys.push_back( key );
	}

	p_file.getLine();
	p_file.getInteger( iCount );
	int iStayTime=0;
	p_file.getLine();
	for( int ii=0; ii < iCount; ++ ii )
	{
		p_file.getInteger( iStayTime );
		m_stayStationTime.push_back(iStayTime);
	}
}
bool CSingleCarSchedule::ReadData ( ArctermFile& p_file )
{
	p_file.getLine();
	char carName[256];
	p_file.getField( carName,256 );
	m_carName=carName;


	p_file.getTime( m_scheduleStartTime, TRUE );
	p_file.getTime( m_scheduleEndTime, TRUE );


	p_file.getFloat( m_dNormalSpeed );
	m_dNormalSpeed *= SCALE_FACTOR;
	p_file.getFloat(m_dAccelerateSpeed);
	m_dAccelerateSpeed *= SCALE_FACTOR;
	p_file.getFloat(m_dDecelerateSpeed);
	m_dDecelerateSpeed *= SCALE_FACTOR;

	p_file.getInteger(m_iTrainNumber);
	p_file.getTime( m_turnArroundTime, TRUE );
	p_file.getTime(m_headWayTime, TRUE);
	//m_headWayTime /= 60;
	p_file.getInteger(m_iOneDayCount);

	// car flow
	p_file.getLine();
	int iCount=0;
	p_file.getInteger( iCount );
	
	m_carFlow.clear();
	for( int i=0; i<iCount; ++i )
	{
		//int iProcIndex=-1;
		p_file.getLine();
		ProcessorID temp;
		temp.SetStrDict(m_pSystemProcessorList->GetInputTerminal()->inStrDict );
		temp.readProcessorID( p_file );
		Processor* pProc = m_pSystemProcessorList->getProcessor( temp.GetIDString() );
		if (!pProc)
		{
			return false;
		}
		m_carFlow.push_back( (IntegratedStation *)pProc );
	}

	// move path
	p_file.getLine();
	p_file.getInteger( iCount );
	m_ViehcleMovePath.clear();
	for(int ii=0; ii < iCount; ++ii )
	{
		CViehcleMovePath movePath;
		movePath.ReadData( p_file );
		m_ViehcleMovePath.push_back( movePath );
	}	
	p_file.getLine();
	p_file.getTime( m_circleTime, TRUE );
	


	p_file.getLine();
	p_file.getInteger( iCount );

	for(int ii=0; ii<iCount; ++ii )
	{
		RailwayInfo tempRail;
		tempRail.ReadDataFromFile( p_file );

		RailwayInfoKey key ;
		key.SetFirstStationIndex( tempRail.GetFirstStationIndex() );
		key.SetSecondStationIndex( tempRail.GetSecondStationIndex() );
		key.SetTypeFromFirstViewPoint( tempRail.GetTypeFromFirstViewPoint() );

		
		RailwayInfo* pRailWay = m_pSystemRailWay->FindRailWayPointer( key );
		ASSERT( pRailWay );
		if(pRailWay)
		{
			m_allRailWay.push_back( pRailWay );
			m_allRailWayKeys.push_back( key );
		}
	}

	p_file.getLine();
	p_file.getInteger( iCount );
	int iStayTime=0;
	p_file.getLine();
	for(int ii=0; ii<iCount; ++ii )
	{
		p_file.getInteger( iStayTime );
		m_stayStationTime.push_back(iStayTime);
	}
	return TRUE;
}

void CSingleCarSchedule::WriteData( ArctermFile& p_file ) const
{
	p_file.writeField(m_carName);


	p_file.writeTime( m_scheduleStartTime, TRUE );
	p_file.writeTime( m_scheduleEndTime, TRUE );

	
	//Normal speed 
	p_file.writeFloat( (float) (m_dNormalSpeed / SCALE_FACTOR) );

	//Acceleration speed
	p_file.writeFloat( (float) (m_dAccelerateSpeed / SCALE_FACTOR) );

	//Deceleration  speed
	p_file.writeFloat( (float) (m_dDecelerateSpeed / SCALE_FACTOR) );
	
	p_file.writeInt(m_iTrainNumber);

	p_file.writeTime( m_turnArroundTime, TRUE );

	p_file.writeTime( m_headWayTime, TRUE );
	
	p_file.writeInt(m_iOneDayCount);
	
	p_file.writeLine();

	//all stations I will pass
	CARFLOW::const_iterator iter=m_carFlow.begin();
	CARFLOW::const_iterator iterEnd=m_carFlow.end();
	p_file.writeInt( (int)m_carFlow.size() );
	p_file.writeLine();
	for(; iter!=iterEnd; ++iter )
	{
		(*iter)->getID()->writeProcessorID( p_file );
		p_file.writeLine();
	}
	
	

	// move path
	int nCount = m_ViehcleMovePath.size();
	p_file.writeInt( nCount );
	p_file.writeLine();
	for( int i=0; i<nCount; i++ )
	{
		m_ViehcleMovePath[i].WriteData( p_file );
	}
	p_file.writeTime( m_circleTime, TRUE );
	p_file.writeLine();

	// railway

	RAILWAY::const_iterator iterRail = m_allRailWay.begin();
	RAILWAY::const_iterator iterRailEnd = m_allRailWay.end();
	p_file.writeInt( (int)m_allRailWay.size() );
	p_file.writeLine();

	for(; iterRail!=iterRailEnd; ++iterRail )
	{
		(*iterRail)->SaveDataToFile( p_file );
	}

	std::vector<int>::const_iterator iterTimeOnSt = m_stayStationTime.begin();
	std::vector<int>::const_iterator iterTimeOnStEnd = m_stayStationTime.end();
	p_file.writeInt( (int)m_stayStationTime.size());
	p_file.writeLine();
	for (; iterTimeOnSt!=iterTimeOnStEnd; ++iterTimeOnSt )
	{
		p_file.writeInt( *iterTimeOnSt );
	}



	//RAILWAY::cons m_allRailWay;
	// we can write railwat's first station index,second statio index and 
	// ower station index ,type from first view type from second..
	//then when read these data from file's ,find it in railway system to get the 
	//pointer of railway!

	//std::list<int> m_stayStationTime;
	
	p_file.writeLine();
	
}


bool CSingleCarSchedule::AddLinkedRailWay (  IntegratedStation* _pStartStation, RailwayInfo* _pLinkedRailWay , bool _pushToBack)
{
	if( _pushToBack )
	{
		m_allRailWay.push_back( _pLinkedRailWay );

		RailwayInfoKey key ;
		key.SetFirstStationIndex( _pLinkedRailWay->GetFirstStationIndex() );
		key.SetSecondStationIndex( _pLinkedRailWay->GetSecondStationIndex() );
		key.SetTypeFromFirstViewPoint( _pLinkedRailWay->GetTypeFromFirstViewPoint() );
		m_allRailWayKeys.push_back( key );
		return true;
	}
	else
	{
		CARFLOW::const_iterator iter = m_carFlow.begin();
		CARFLOW::const_iterator iterEnd = m_carFlow.end();
		
		RAILWAY::iterator iterRailWay = m_allRailWay.begin();
		RAILWAY::iterator iterRailWayEnd = m_allRailWay.end();
		for(; iter!=iterEnd; ++iter,++iterRailWay )
		{
			if( (*iter) == _pStartStation )
			{
				m_allRailWay.insert( iterRailWay, _pLinkedRailWay );
				//m_allRailWayKeys should also insert
				return true;
			}
		}

		return false;
	}
}

bool CSingleCarSchedule::AddStayOnStationTime (  IntegratedStation* _pStation, int _iStayOnStationMini , bool _pushBack )
{
	if( !_pStation )
	{
		m_stayStationTime.insert( m_stayStationTime.begin(), _iStayOnStationMini );
		return true;
	}
	if( _pushBack )
	{
		m_stayStationTime.push_back( _iStayOnStationMini );
		return true;
	}
	else
	{
		CARFLOW::const_iterator iter = m_carFlow.begin();
		CARFLOW::const_iterator iterEnd = m_carFlow.end();
		
		std::vector<int>::iterator iterTime = m_stayStationTime.begin();
		std::vector<int>::iterator iterTimeEnd = m_stayStationTime.end();
		for(; iter!=iterEnd; ++iter,++iterTime )
		{
			if( (*iter) == _pStation )
			{
				m_stayStationTime.insert( iterTime, _iStayOnStationMini );
				return true;
			}
		}

		return false;
	}
}


// -----------------------------------------
// input:
		
// output: 
//		std::vector<CViehcleMovePath> m_ViehcleMovePath;
// -----------------------------------------


void CSingleCarSchedule::GenerateRealSchedule( TrainTrafficGraph* _pTraffic )
{
// 	if(m_allRailWay.size() != m_carFlow.size() )
// 		return;
	
	CARFLOW::const_iterator iterStart = m_carFlow.begin();
	CARFLOW::const_iterator iterNext = iterStart;
	CARFLOW::const_iterator iterLast = m_carFlow.end();

	//all source is the first station??? it is not right!!
	int nCount = m_carFlow.size();
	IntegratedStation* pSourceStation = *iterNext;
	
	ElapsedTime arrTime = 0l;
	//long lStartTime = m_scheduleStartTime.GetHour()*3600 + m_scheduleStartTime.GetMinute()*60 + m_scheduleStartTime.GetSecond();
//	arrTime.set( m_scheduleStartTime.GetHour(),m_scheduleStartTime.GetMinute(),m_scheduleStartTime.GetSecond() );
	iterNext++;
	std::vector<int>::iterator iterStayTime = m_stayStationTime.begin();
	
	m_ViehcleMovePath.clear();
	for( int i=0 ; iterNext!=iterLast&& i<(int)m_allRailWay.size(); i++ )
	{
		IntegratedStation* pDestStation = *iterNext;
		RailwayInfo* pRailwayInfo = m_allRailWay[i];	// need to check if size of m_allRailWay is the same as m_carFlow
		int nDwellTime = *iterStayTime;
		ElapsedTime depTime = arrTime;
		depTime += (long)nDwellTime;
		CViehcleMovePath viehcleMovePath;
		int nLinkedType = _pTraffic->GetLinkedType( pSourceStation, pRailwayInfo );
		ElapsedTime endTime = GenerateViehcleMovePathBetweenTwoStation( pSourceStation, pDestStation, nLinkedType, pRailwayInfo, arrTime, depTime, viehcleMovePath );
		m_ViehcleMovePath.push_back( viehcleMovePath );
		arrTime = endTime;
		pSourceStation = *iterNext;// add by tutu
		iterNext++;
		iterStayTime++;
	}
	m_circleTime = arrTime;
}


bool CSingleCarSchedule::SelectBestSchedule( IntegratedStation* _pCurrentStation, IntegratedStation* _pDesStation, ElapsedTime arriveCurStationTime, std::vector<CViehcleMovePath>& _bestMovePath )
{
	// if car flow is A->B->C->D->C->B->A
	//and we call this f with parameter B, A,,,
	//then return false, but you know that is not right!
	CARFLOW::const_iterator iter = m_carFlow.begin();
	CARFLOW::const_iterator iterEnd = m_carFlow.end();
	int nIdx = 0;
	int nSourceIdx = -1;
	int nDestIdx = -1;
/*	for(; iter!=iterEnd; ++iter )
	{
		if( (*iter) == _pCurrentStation )
			nSourceIdx = nIdx;
		if( (*iter) == _pDesStation )
		{
			nDestIdx = nIdx;
			break;
		}

		nIdx++;
	}
*/
	for(; iter!=iterEnd; ++iter )
	{
		if( (*iter) == _pCurrentStation )
			nSourceIdx = nIdx;
		if( (*iter) == _pDesStation )
		{
			nDestIdx = nIdx;
		}
		if( nSourceIdx != -1 && nDestIdx != -1 && nSourceIdx< nDestIdx )
			break;
		nIdx++;
	}

	if( nSourceIdx < 0 || nDestIdx < 0 || nSourceIdx >= nDestIdx )
		return false;

	return GetLocationAndTime( nSourceIdx, nDestIdx-1, arriveCurStationTime, _bestMovePath );
}

bool CSingleCarSchedule::GetLocationAndTime( int _iSourceStationIndex, int _iDestStationIndex, ElapsedTime arriveCurStationTime, std::vector<CViehcleMovePath>& _bestMovePath )
{
	//CViehcleMovePath movePath = m_ViehcleMovePath[_iSourceStationIndex];
	ElapsedTime arriveTime = m_ViehcleMovePath[_iSourceStationIndex].GetArrTime();
	arriveCurStationTime -= arriveTime;
	for( int i=_iSourceStationIndex; i<=_iDestStationIndex; i++ )
	{
		CViehcleMovePath movePath = m_ViehcleMovePath[i];
		movePath.DoTimeOffset( arriveCurStationTime );
		_bestMovePath.push_back( movePath );
	}
	return true;
	
}



// input: station1, station2, start time, railway
// output: viehcleMovePath, lastHitTime
ElapsedTime CSingleCarSchedule::GenerateViehcleMovePathBetweenTwoStation( IntegratedStation* _pSourceStation, IntegratedStation* _pDestStation, int _nLinkedType, RailwayInfo* _pRailwayInfo, ElapsedTime _arrTime, ElapsedTime _depTime, CViehcleMovePath& _viehcleMovePath )
{
	// Add point into distancePath
	CDistancePath origPath;
	const Path pathRailway = _pRailwayInfo->GetRailWayPath();
	int nRailWayCount = pathRailway.getCount();
	Point* pPointList = new Point[nRailWayCount + 2];
	pPointList[0] = _pSourceStation->getServicePoint( 0 );
	if( _nLinkedType == PORT1_PORT1 || _nLinkedType == PORT1_PORT2 )
		pPointList[1] = _pSourceStation->GetPort1Path()->getPoint( 0 );
	else
		pPointList[1] = _pSourceStation->GetPort2Path()->getPoint( 0 );

	pPointList[nRailWayCount + 1] = _pDestStation->getServicePoint( 0 );
	if( _nLinkedType == PORT1_PORT1 || _nLinkedType == PORT2_PORT1 )
		pPointList[nRailWayCount] = _pDestStation->GetPort1Path()->getPoint( 0 );
	else
		pPointList[nRailWayCount] = _pDestStation->GetPort2Path()->getPoint( 0 );

		Point selectedPoint = _pSourceStation->getServicePoint( 0 );
		Point firstPathPoint = pathRailway.getPoint( 0 ) ;
		Point secondPathPoint = pathRailway.getPoint( nRailWayCount-1 ) ;
		if ( selectedPoint.distance(firstPathPoint) < selectedPoint.distance( secondPathPoint ) )
		{
			for( int i=1; i<nRailWayCount-1; i++)
			{
				pPointList[i+1] = pathRailway.getPoint(i);
			}
		}
		else
		{
			for( int i=1, j=nRailWayCount-2; j>=1; i++,j--)
			{
				pPointList[i+1] = pathRailway.getPoint(j);
			}
		}


	origPath.init( nRailWayCount + 2, pPointList );
	delete [] pPointList;

	origPath.SetSpeed( m_dAccelerateSpeed, m_dDecelerateSpeed, m_dNormalSpeed );

	origPath.Calculate();

	std::vector<CStateTimePoint>& stateTimePoints =  origPath.GetResultList();

	_viehcleMovePath.Clear();

	_viehcleMovePath.SetArrTime( _arrTime );
	_viehcleMovePath.SetDepTime( _depTime );
	// calculate all t in acc sergement and insert into 
	int nCount = stateTimePoints.size();
	CStateTimePoint item;
	for( int i=0; i<nCount; i++ )
	{
		item = stateTimePoints[i];
		item.SetTime( item.GetTime() + _depTime );
		_viehcleMovePath.AddItem( item );		
	}
	return item.GetTime();
}
bool CSingleCarSchedule::DeleteLinkedRailWay( int iCountFromTail ) 
{

	if( (UINT)iCountFromTail > m_carFlow.size() )
		return false;
	//iCountFromTail--;
	while( iCountFromTail-- )
	{
		m_carFlow.pop_back();
		m_stayStationTime.pop_back();
		m_allRailWay.pop_back();
		m_allRailWayKeys.pop_back();
	}
//	m_carFlow.pop_back();
//	m_stayStationTime.pop_back();

	//if (ECHOLOG->IsEnable() && ECHOLOG->IsLogEnable(RailWayLog))
	{
		CString strLog = _T("");
		strLog = _T("DeleteLinkedRailWay in Car Schedule;");
		ECHOLOG->Log(RailWayLog,strLog);
	}
	return true;
}
bool CSingleCarSchedule::DeleteLinkedRailWay( IntegratedStation* _pStartStation, RailwayInfo* _pLinkedRailWay )
{
	CARFLOW::iterator iter = m_carFlow.begin();
	CARFLOW::iterator iterEnd = m_carFlow.end();
	--iterEnd;
	//CARFLOW::iterator iterFind = find ( iter, iterEnd, _pStartStation);
	std::vector<int>::iterator iterTime = m_stayStationTime.begin();
	for( int i=0; iter!=iterEnd;  ++i)
	{
		if( (*iter) == _pStartStation && m_allRailWay[i] == _pLinkedRailWay)
		{

			m_allRailWay.erase( m_allRailWay.begin() + i );
			m_allRailWayKeys.erase( m_allRailWayKeys.begin() + i );
			++iter;			
			m_carFlow.erase( iter );
			m_stayStationTime.erase( iterTime );
			++iterTime;

			//if (ECHOLOG->IsEnable() && ECHOLOG->IsLogEnable(RailWayLog))
			{
				CString strLog = _T("");
				strLog = _T("DeleteLinkedRailWay in Car Schedule;");
				ECHOLOG->Log(RailWayLog,strLog);
			}

			return true;
		}
		else
		{
			++iter;
			++iterTime;
		}
	}

	return false;

}

bool CSingleCarSchedule::GetNearestSchedule( IntegratedStation* _pCurrentStation, IntegratedStation* _pDesStation, ElapsedTime _currentTime,  ElapsedTime& _arriveCurStationTime, ElapsedTime& _arriveDestStationTime )	
{
	CARFLOW::const_iterator iter = m_carFlow.begin();
	CARFLOW::const_iterator iterEnd = m_carFlow.end();
	int nIdx = 0;
	int nSourceIdx = -1;
	int nDestIdx = -1;
	for(; iter!=iterEnd; ++iter )
	{
		if( (*iter) == _pCurrentStation )
			nSourceIdx = nIdx;
		if( (*iter) == _pDesStation )
		{
			nDestIdx = nIdx;
		}
		if( nSourceIdx != -1 && nDestIdx != -1 && nSourceIdx< nDestIdx )
			break;
		nIdx++;
	}
	
	if( nSourceIdx < 0 || nDestIdx < 0 || nSourceIdx >= nDestIdx )
		return false;
	

	ElapsedTime arrTime = m_ViehcleMovePath[nSourceIdx].GetArrTime();
	ElapsedTime depTime = m_ViehcleMovePath[nSourceIdx].GetDepTime();
	arrTime += m_scheduleStartTime;
	depTime += m_scheduleStartTime;
	bool bNotFind = true;
	int nTrainIndex = 0;
	while( depTime <= _currentTime && bNotFind )
	{
	//	ElapsedTime timeOnceMore = depTime + m_circleTime;
		ElapsedTime timeNextArr = arrTime;
		ElapsedTime timeNextDep = depTime;
		for( int i=1; i<m_iTrainNumber && bNotFind; ++i )
		{
			timeNextDep += m_headWayTime;
			timeNextArr += m_headWayTime;
			if( timeNextDep > _currentTime )
			{
				bNotFind = false;
				nTrainIndex = i;
				_arriveCurStationTime = timeNextArr;
				arrTime  = _arriveCurStationTime;
			}
		}

		if( bNotFind )
		{
			depTime += m_circleTime;
			arrTime += m_circleTime;
		}
		
	}
	


	_arriveDestStationTime = m_ViehcleMovePath[nDestIdx-1].GetDestArrTime();
	_arriveDestStationTime += arrTime ;
	if( nTrainIndex == 0 )
		_arriveCurStationTime = arrTime;

	
		

	return true;
}
// test if this sechedule can take people from source station to dest station
bool CSingleCarSchedule::IfScheduleBetweenStations( IntegratedStation* _pSourceStation, IntegratedStation* _pDesStation )const
{
	CARFLOW::const_iterator iter = m_carFlow.begin();
	CARFLOW::const_iterator iterEnd = m_carFlow.end();
	int nIdx = 0;
	int nSourceIdx = -1;
	int nDestIdx = -1;
	for(; iter!=iterEnd; ++iter )
	{
		if( (*iter) == _pSourceStation )
			nSourceIdx = nIdx;
		if( (*iter) == _pDesStation )
		{
			nDestIdx = nIdx;
		}
		if( nSourceIdx != -1 && nDestIdx != -1 && nSourceIdx< nDestIdx )
			return true;	
		nIdx++;
	}
	
	return false;
	
}
bool CSingleCarSchedule::GetArrDepTimeTable( int _iCurrentStationIndex ,ArrDepTimeVector& _timeTable )
{
	_timeTable.clear();
/*	
	CARFLOW::const_iterator iter = m_carFlow.begin();
	CARFLOW::const_iterator iterEnd = m_carFlow.end();
	int nIdx = 0;
	int nSourceIdx = -1;
	int nDestIdx = -1;
	for(; iter!=iterEnd; ++iter )
	{
		if( (*iter) == _pCurrentStation )
			nSourceIdx = nIdx;
		if( (*iter) == _pDesStation )
		{
			nDestIdx = nIdx;
		}
		if( nSourceIdx != -1 && nDestIdx != -1 && nSourceIdx< nDestIdx )
			break;
		nIdx++;
	}
	
	if( nSourceIdx < 0 || nDestIdx < 0 || nSourceIdx >= nDestIdx )
		return false;
*/
	if( _iCurrentStationIndex <0 || (UINT)_iCurrentStationIndex >= m_ViehcleMovePath.size() )
		return false;
	ElapsedTime arrTime = m_ViehcleMovePath[_iCurrentStationIndex].GetArrTime();
	ElapsedTime depTime = m_ViehcleMovePath[_iCurrentStationIndex].GetDepTime();

	arrTime += m_scheduleStartTime;
	depTime += m_scheduleStartTime;
	CArrDepTime tempTimeTable( arrTime, depTime );
	_timeTable.push_back( tempTimeTable );
	
	while ( arrTime < m_scheduleEndTime )
	{
		arrTime += m_circleTime;
		depTime += m_circleTime;
		tempTimeTable.SetArrTime( arrTime );
		tempTimeTable.SetDepTime( depTime );
		_timeTable.push_back( tempTimeTable );
	}
		
	return true;
}
void CSingleCarSchedule::GetAllTimeTable( TimeTableVector& _timeTable )
{
	_timeTable.clear();
	CARFLOW::const_iterator iter = m_carFlow.begin();
	CARFLOW::const_iterator iterEnd = m_carFlow.end();
	CARFLOW::const_iterator iterNext = iter;
	
	char stationName [ 64 ];
	//CString 
	for ( int i=0; iter!=iterEnd ; /*++iterNext,*/++iter,++i )
	{
		TIMETABLE tempTimeTable;
		++iterNext;
		if (iterNext == iterEnd)
		{
			continue;
		}
		(*iter)->getID()->printID( stationName );
		tempTimeTable.m_sCurStationName = stationName;
		
		(*iterNext)->getID()->printID( stationName );
		tempTimeTable.m_sDestStationName = stationName;

		ArrDepTimeVector arrDepTimeVec;

		if( GetArrDepTimeTable( i , arrDepTimeVec ) )
		{
			char tempTime [ 16 ];
			ElapsedTime elapsedTime;
	
			for( UINT j=0; j<arrDepTimeVec.size(); ++j )
			{
				const CArrDepTime& arrDepTime = arrDepTimeVec[j];
				tempTimeTable.m_arrTime = arrDepTime.GetArrTimeString(tempTimeTable.m_arrDay);
				tempTimeTable.m_depTime = arrDepTime.GetDepTimeString(tempTimeTable.m_depDay);
				tempTimeTable.m_nTrainIndex = 0;
				_timeTable.push_back( tempTimeTable );
				CString offSetString;
				for( long i=1; i< m_iTrainNumber ; ++i)
				{
					tempTimeTable.m_nTrainIndex = i;
					elapsedTime=arrDepTime.GetArrTime() + m_headWayTime*i;
					tempTimeTable.m_arrDay = elapsedTime.GetDay();
					elapsedTime.SetDay(1);
					elapsedTime.printTime( tempTime	);
					tempTimeTable.m_arrTime = tempTime;

					elapsedTime = arrDepTime.GetDepTime() + m_headWayTime*i;
					tempTimeTable.m_depDay = elapsedTime.GetDay();
					elapsedTime.SetDay(1);
					elapsedTime.printTime( tempTime	);
					tempTimeTable.m_depTime = tempTime;
					
					_timeTable.push_back( tempTimeTable );
				}

			}
		}

	}
	
}

void CSingleCarSchedule::GetStationTimeTable( IntegratedStation* _pStation, TimeTableVector& _timeTable)
{
	CARFLOW::const_iterator iter = m_carFlow.begin();
	CARFLOW::const_iterator iterEnd = m_carFlow.end();
	CARFLOW::const_iterator iterNext = iter;
	++iterNext;
	char stationName [ 64 ];
	TIMETABLE tempTimeTable;
	tempTimeTable.m_sScheduleName = m_carName;

	for(int nIdx = 0; iterNext!=iterEnd; ++iterNext,++iter,++nIdx)
	{
		if( (*iter) == _pStation )
		{

			(*iter)->getID()->printID( stationName );
			tempTimeTable.m_sCurStationName = stationName;
		
			(*iterNext)->getID()->printID( stationName );
			tempTimeTable.m_sDestStationName = stationName;

			ArrDepTimeVector arrDepTimeVec;

			if( GetArrDepTimeTable( nIdx , arrDepTimeVec ) )
			{
				char tempTime [ 16 ];
				ElapsedTime elapsedTime;
	
				for( UINT j=0; j<arrDepTimeVec.size(); ++j )
				{
					const CArrDepTime& arrDepTime = arrDepTimeVec[j];
					tempTimeTable.m_arrTime = arrDepTime.GetArrTimeString(tempTimeTable.m_arrDay);
					tempTimeTable.m_depTime = arrDepTime.GetDepTimeString(tempTimeTable.m_depDay);
					tempTimeTable.m_nTrainIndex = 0;
					_timeTable.push_back( tempTimeTable );
					for( long i=1; i< m_iTrainNumber ; ++i)
					{
						tempTimeTable.m_nTrainIndex = i;
						elapsedTime=arrDepTime.GetArrTime() + m_headWayTime*i;
						tempTimeTable.m_arrDay = elapsedTime.GetDay();
						elapsedTime.SetDay(1);
						elapsedTime.printTime( tempTime	);
						tempTimeTable.m_arrTime = tempTime;

						elapsedTime = arrDepTime.GetDepTime() + m_headWayTime*i;
						tempTimeTable.m_depDay = elapsedTime.GetDay();
						elapsedTime.SetDay(1);
						elapsedTime.printTime( tempTime );
						tempTimeTable.m_depTime = tempTime;

						_timeTable.push_back( tempTimeTable );
					}
				}
			}
		}

	}
	

}

bool CSingleCarSchedule::AdjustRailway( TrainTrafficGraph* _pTraffic  )
{
	m_allRailWay.clear();
	RAILWAYKEYS::const_iterator iter =  m_allRailWayKeys.begin();
	RAILWAYKEYS::const_iterator iterEnd =  m_allRailWayKeys.end();
	RailwayInfo* pRailWay = NULL;
	RailwayInfoKey key ;
	for( ; iter != iterEnd ; ++iter )
	{		
		key.SetFirstStationIndex( (*iter).GetFirstStationIndex() );
		key.SetSecondStationIndex( (*iter).GetSecondStationIndex() );
		key.SetTypeFromFirstViewPoint( (*iter).GetTypeFromFirstViewPoint() );

		pRailWay = m_pSystemRailWay->FindRailWayPointer( key );
		if( pRailWay )
		 m_allRailWay.push_back( pRailWay );
		else
		 return true;
	}

	GenerateRealSchedule( _pTraffic );
	return false;
}




// get all location and time for whole day.
void CSingleCarSchedule::GetAllLocationAndTime( int _nTrainIdx, std::vector<CViehcleMovePath>& _vMovePath )
{
	int nCount = m_ViehcleMovePath.size();
	ElapsedTime arrTime = m_scheduleStartTime + m_headWayTime * (long)_nTrainIdx;
	while( arrTime + m_circleTime < m_scheduleEndTime )
	{
		for( int i=0; i<nCount; i++ )
		{
			CViehcleMovePath movePath = m_ViehcleMovePath[i];
			movePath.DoTimeOffset( arrTime );
			_vMovePath.push_back( movePath );
		}
		arrTime += m_circleTime;
	}
	m_actualEndTime = arrTime + m_circleTime;
}

int CSingleCarSchedule::GetDwellingTime( int iPosInVector )const
{
	ASSERT( iPosInVector>=0 && (UINT)iPosInVector< m_stayStationTime.size() );
	return m_stayStationTime[iPosInVector];
}
void CSingleCarSchedule::SetDwellingTime ( int iPosInVector, int iTime )
{
	ASSERT( iPosInVector>=0 && (UINT)iPosInVector< m_stayStationTime.size() );
	m_stayStationTime.at( iPosInVector ) = iTime;
}

ElapsedTime CSingleCarSchedule::GetActualEndTime()
{
	return m_actualEndTime;
}
bool CSingleCarSchedule::IsInputDataValid() const
{
	ElapsedTime tempTime = m_headWayTime* (long)m_iTrainNumber;
	if( m_circleTime.asSeconds() <=  tempTime.asSeconds())
	{
		AfxMessageBox(" Please adjust HeadWay time or Number of Train !");
		return false;
	}
	else
		return true;
}

ElapsedTime CSingleCarSchedule::GetCircleTime()
{
	return m_circleTime;
}

void CSingleCarSchedule::AdjustRailwayKeys( int _iOldStationIndex)
{
	RAILWAYKEYS::iterator iter = m_allRailWayKeys.begin();
	RAILWAYKEYS::iterator iterEnd = m_allRailWayKeys.end();
	for( ; iter != iterEnd; ++iter )
	{
		if ( (*iter).GetFirstStationIndex() > _iOldStationIndex )
		{
			(*iter).SetFirstStationIndex( (*iter).GetFirstStationIndex() -1 );
		}

		if ( (*iter).GetSecondStationIndex() > _iOldStationIndex )
		{
			(*iter).SetSecondStationIndex( (*iter).GetSecondStationIndex() -1 );
		}

	}
}

bool CSingleCarSchedule::IsUseThisStation( int _iStationIndex )
{
	RAILWAYKEYS::iterator iter = m_allRailWayKeys.begin();
	RAILWAYKEYS::iterator iterEnd = m_allRailWayKeys.end();
	for( ; iter != iterEnd; ++iter )
	{
		if( (*iter).GetFirstStationIndex() == _iStationIndex 
			|| (*iter).GetSecondStationIndex() == _iStationIndex 
		  )

		  return true;
	}
	return false;
}

CStationLayout * CSingleCarSchedule::GetStationLayout()
{
	IntegratedStation* pFrontStation = m_carFlow.front();
	if (pFrontStation)
	{
		return pFrontStation->GetLayout();
	}
	
	return NULL;
}

/*************************************/
/*************************************/
CString CArrDepTime::GetArrTimeString() const
{
	char tempTime [ 16 ];
	m_arriveTime.printTime( tempTime );
	return CString( tempTime );
}
CString CArrDepTime::GetDepTimeString() const
{
	char tempTime [ 16 ];
	m_departureTime.printTime( tempTime );
	return CString( tempTime );
}

CString CArrDepTime::GetArrTimeString(int& day) const
{
	ElapsedTime arrTime = m_arriveTime;
	day = arrTime.GetDay();
	arrTime.SetDay(1);
	char tempTime [ 16 ];
	arrTime.printTime( tempTime );
	return CString( tempTime );
}
CString CArrDepTime::GetDepTimeString(int& day) const
{
	ElapsedTime depTime = m_departureTime;
	day = depTime.GetDay();
	depTime.SetDay(1);
	char tempTime [ 16 ];
	depTime.printTime( tempTime );
	return CString( tempTime );

}


