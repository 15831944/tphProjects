// TrainTrafficGraph.cpp: implementation of the TrainTrafficGraph class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TrainTrafficGraph.h"
#include "RailwayInfo.h"
#include "proclist.h"
#include "..\inputs\RailWayData.h"
#include "engine\terminal.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#include <algorithm>
using namespace std;

TrainTrafficGraph::TrainTrafficGraph()
{
	
}
TrainTrafficGraph::TrainTrafficGraph(const TrainTrafficGraph& _anotherGraph)
{

	AdjacentStation* pStation=NULL;
	for(STATIONANDRALLWAY::const_iterator iter=_anotherGraph.m_trafficGraph.begin();iter!=_anotherGraph.m_trafficGraph.end();++iter)
	{
		pStation=new AdjacentStation(**iter);
		m_trafficGraph.push_back(pStation);
	}

	//because AdjacentStation copy constructor only copy all railway owned,
	// so we must rebuild all its un-owned railway
	DoRemainThings();
}
TrainTrafficGraph::~TrainTrafficGraph()
{
	for( STATIONANDRALLWAY::iterator iter=m_trafficGraph.begin();iter!=m_trafficGraph.end();++iter)
	{
		delete (*iter);
	}
	m_trafficGraph.clear();
}

TrainTrafficGraph& TrainTrafficGraph::operator = (const TrainTrafficGraph _anotherGraph )
{

	if(this!=&_anotherGraph)
	{
		for( STATIONANDRALLWAY::const_iterator iter=m_trafficGraph.begin();iter!=m_trafficGraph.end();++iter)
		{
			delete (*iter);
		}
		m_trafficGraph.clear();

		AdjacentStation* pStation=NULL;
		for(iter=_anotherGraph.m_trafficGraph.begin();iter!=_anotherGraph.m_trafficGraph.end();++iter)
		{
			pStation=new AdjacentStation(**iter);
			m_trafficGraph.push_back(pStation);
		}

		//because AdjacentStation copy constructor only copy all railway owned,
		// so we must rebuild all its un-owned railway
		DoRemainThings();

	}

	return *this;
}
//add railway's pointer to the AdjacentStation which linked with the railway but is not the railway's owner
void TrainTrafficGraph::DoRemainThings()
{
	RAILWAY OwnedRailWayVector;
	for(STATIONANDRALLWAY::const_iterator iter=m_trafficGraph.begin();iter!=m_trafficGraph.end();++iter)
	{
		(*iter)->GetOwnedRailWay(OwnedRailWayVector);
		for(RAILWAY::const_iterator iter=OwnedRailWayVector.begin();iter!=OwnedRailWayVector.end();++iter)
		{
			int iSecondStationIndex=(*iter)->GetSecondStationIndex();
			m_trafficGraph[iSecondStationIndex]->AddAdjacentRailWay(*iter,false);
		}
		
	}

}

// get all stations from proclist add put into vector m_trafficGraph ,at the same 
//time add all railway 
int TrainTrafficGraph::Init(Terminal* _terminal)
{
	
	AdjacentStation* pTemp;
	int iProcessCount=_terminal->procList->getProcessorCount();
	for( int i=0; i<iProcessCount; ++i)
	{
		if(_terminal->procList->getProcessor(i)->getProcessorType() == IntegratedStationProc)
		{
			pTemp = new AdjacentStation();
			pTemp->SetStation((IntegratedStation* )_terminal->procList->getProcessor(i));
			m_trafficGraph.push_back(pTemp);
		}

	}

	RAILWAY allRailWay=_terminal->pRailWay->GetAllRailWayInfo();
	RAILWAY::const_iterator iter=allRailWay.begin();
	RAILWAY::const_iterator iterLast=allRailWay.end();

	for(;iter!=iterLast;++iter)
	{
		int iFirstStationIndex=(*iter)->GetFirstStationIndex();
		int iSecondStationIndex=(*iter)->GetSecondStationIndex();
		int iOwnedStationIndex=(*iter)->GetOwnerIndex();
		m_trafficGraph[iFirstStationIndex]->AddAdjacentRailWay(*iter,iFirstStationIndex == iOwnedStationIndex);
		m_trafficGraph[iSecondStationIndex]->AddAdjacentRailWay(*iter,iSecondStationIndex == iOwnedStationIndex);

	}

	return TRUE;
}

// insert a new railway path which link two station
BOOL TrainTrafficGraph::InsertRailWay(IntegratedStation* _firstStation,IntegratedStation* _secondStation,int _typeFromFirst,int _pointCount,const Point* _pointList)
{
	int iFirstIndexInTrafficGraph=-1;
	int iSecondIndexInTrafficGraph=-1;

	for ( unsigned int i=0; i<m_trafficGraph.size();++i)
	{
		if(m_trafficGraph[i]->GetStation() == _firstStation)
		{
			iFirstIndexInTrafficGraph=i;
		}
		else if (m_trafficGraph[i]->GetStation() == _secondStation)
		{
			iSecondIndexInTrafficGraph=i;
		}
	}

	if( iFirstIndexInTrafficGraph == -1 || iSecondIndexInTrafficGraph == -1)
	{
		AfxMessageBox("Can not find linked Integrated Stations!");
		return false;
	}

	InsertRailWay(iFirstIndexInTrafficGraph,iSecondIndexInTrafficGraph,_typeFromFirst,_pointCount,_pointList);


	return true;
}

// insert a new railway path which link two station
BOOL TrainTrafficGraph::InsertRailWay(int _firstStationIndex,int  _secondStationIndex,int _typeFromFirst,int _pointCount,const Point* _pointList)
{
	int iStationCount=m_trafficGraph.size();
	if( _firstStationIndex <0 || _firstStationIndex >=iStationCount || _secondStationIndex<0 || _secondStationIndex >= iStationCount)
	{
		AfxMessageBox("Can not find linked Integrated Stations! ");
		return false;
	}

	RailwayInfo* pInfo=new RailwayInfo();
	pInfo->SetFirstStationIndex(_firstStationIndex);
	pInfo->SetSecondStationIndex(_secondStationIndex);
	pInfo->SetOwnerIndex(_firstStationIndex);
	pInfo->SetRailWay(_pointCount,_pointList);
	pInfo->SetTypeFromFirstViewPoint(_typeFromFirst);

	if(_typeFromFirst == PORT1_PORT2)
	{	
		pInfo->SetTypeFromSecondViewPoint(PORT2_PORT1);
	}
	else if(_typeFromFirst == PORT2_PORT1)
	{
		pInfo->SetTypeFromSecondViewPoint(PORT1_PORT2);
	}
	else
		pInfo->SetTypeFromSecondViewPoint(_typeFromFirst);

	//since two station may have the same one railway,so at least one should have 
	// the responsibility to delete railway
	//true = delete ,
	m_trafficGraph[_firstStationIndex]->AddAdjacentRailWay(pInfo,true);
	m_trafficGraph[_secondStationIndex]->AddAdjacentRailWay(pInfo,false);

	return true;
	
}

// delete railway which link two station
BOOL TrainTrafficGraph::DeleteRailWay(int _firstStationIndex,int  _secondStationIndex,int _typeFromFirst)
{
	int iStationCount=m_trafficGraph.size();
	if( _firstStationIndex <0 || _firstStationIndex >=iStationCount || _secondStationIndex<0 || _secondStationIndex >= iStationCount)
	{
		AfxMessageBox("Can not find linked Integrated Stations! ");
		return FALSE;
	}

	if( !m_trafficGraph[_firstStationIndex]->DeleteRailWay(_secondStationIndex,_typeFromFirst) )
		return FALSE;

	if( _typeFromFirst== PORT1_PORT2 )
		_typeFromFirst= PORT2_PORT1;
	else if ( _typeFromFirst== PORT2_PORT1 )
		_typeFromFirst= PORT1_PORT2;

	if( !m_trafficGraph[_secondStationIndex]->DeleteRailWay(_firstStationIndex,_typeFromFirst) )
		return FALSE;

	return TRUE;
	
}




//_iPortType=0,get all railway linked with station 
//_iPortType=1,get railway linked with station port1
//_iPortType=2,get railway linked with station port2
void TrainTrafficGraph::GetStationLinkedRailWay(int _iStationIndex, int _iPortType,RAILWAY& _railWayVector) const
{
	if(_iStationIndex <0 || (unsigned int)_iStationIndex >= m_trafficGraph.size())
	{
		AfxMessageBox("Error : no such station index");
		return;
	}

	m_trafficGraph[_iStationIndex]->GetLinkedRailWay(_iPortType,_railWayVector);
}

//get railway which link first station with second station by the type of _linkedType
const RailwayInfo* TrainTrafficGraph::GetStationLinkedRailWay(int _iFirstStationIndex, int _secondStationIndex,int _linkedType) const
{
	if(_iFirstStationIndex <0 || (UINT)_iFirstStationIndex >= m_trafficGraph.size())
	{
		return NULL;
	}

	return m_trafficGraph[_iFirstStationIndex]->GetLinkedRailWay(_secondStationIndex,_linkedType);
}

void  TrainTrafficGraph::GetStationLinkedRailWay(int _iFirstStationIndex, int _iSecondStationIndex, int _iLinkedType,RAILWAY& _railWayVector) const
{
	ASSERT( _iFirstStationIndex>=0 && (UINT)_iFirstStationIndex< m_trafficGraph.size() );
	ASSERT( _iSecondStationIndex>=0 && (UINT)_iSecondStationIndex< m_trafficGraph.size() );
	m_trafficGraph[_iFirstStationIndex]->GetLinkedRailWay( _iSecondStationIndex, _iLinkedType,_railWayVector );
}

void  TrainTrafficGraph::GetStationLinkedRailWay(IntegratedStation*  _pFirstStation,IntegratedStation*  _pSecondStation, int _iLinkedType,RAILWAY& _railWayVector) const
{
	STATIONANDRALLWAY::const_iterator iter=m_trafficGraph.begin();
	STATIONANDRALLWAY::const_iterator iterLast=m_trafficGraph.end();
	int iFirstStationIndex=-1;
	int iSecondStationIndex=-1;
	for(int i=0; iter!=iterLast; ++iter,++i)
	{
		if( (*iter)->GetStation()== _pFirstStation )
			iFirstStationIndex=i;
		else if( (*iter)->GetStation()== _pSecondStation )
			iSecondStationIndex=i;
		
		if(iFirstStationIndex!=-1 && iSecondStationIndex!=-1)
		{
			m_trafficGraph[iFirstStationIndex]->GetLinkedRailWay( iSecondStationIndex, _iLinkedType,_railWayVector );
			break;
		}
	}

}
	//get railway which link first station with second station by the type of _linkedType
const RailwayInfo* TrainTrafficGraph::GetStationLinkedRailWay(IntegratedStation*  _pFirstStation,IntegratedStation*  _pSecondStation,int _linkedType) const
{

	STATIONANDRALLWAY::const_iterator iter=m_trafficGraph.begin();
	STATIONANDRALLWAY::const_iterator iterLast=m_trafficGraph.end();
	int iFirstStationIndex=-1;
	int iSecondStationIndex=-1;
	for(int i=0; iter!=iterLast; ++iter,++i)
	{
		if( (*iter)->GetStation()== _pFirstStation )
			iFirstStationIndex=i;
		else if( (*iter)->GetStation()== _pSecondStation )
			iSecondStationIndex=i;
		
		if(iFirstStationIndex!=-1 && iSecondStationIndex!=-1)
			return m_trafficGraph[iFirstStationIndex]->GetLinkedRailWay(iSecondStationIndex,_linkedType);
	}

	return NULL;
}




// check if two station linked with each other and the linked type from first station's viewpoint is _linkedType 
BOOL TrainTrafficGraph::IsTwoStationDirectLinked(int _firstStationIndex,int  _secondStationIndex,int _typeFromFirst) const
{
	int iStationCount=m_trafficGraph.size();
	if( _firstStationIndex <0 || _firstStationIndex >=iStationCount || _secondStationIndex<0 || _secondStationIndex >= iStationCount)
	{
		AfxMessageBox("Can not find linked Integrated Stations! ");
		return false;
	}

	return m_trafficGraph[_firstStationIndex]->IsDirectLinkedWithAnotherStation(_secondStationIndex,_typeFromFirst);
}

// check if two station linked with each other and the linked type from first station's viewpoint is _linkedType 
BOOL  TrainTrafficGraph::IsTwoStationDirectLinked(IntegratedStation*  _pFirstStation,IntegratedStation*  _pSecondStation,int _linkedType) const
{
	STATIONANDRALLWAY::const_iterator iter=m_trafficGraph.begin();
	STATIONANDRALLWAY::const_iterator iterLast=m_trafficGraph.end();
	int iFirstStationIndex=-1;
	int iSecondStationIndex=-1;
	for(int i=0; iter!=iterLast; ++iter,++i)
	{
		if( (*iter)->GetStation()== _pFirstStation )
			iFirstStationIndex=i;
		else if( (*iter)->GetStation()== _pSecondStation )
			iSecondStationIndex=i;
		
		if(iFirstStationIndex!=-1 && iSecondStationIndex!=-1)
			return m_trafficGraph[iFirstStationIndex]->IsDirectLinkedWithAnotherStation(iSecondStationIndex,_linkedType);
	}

	return false;

}

// check if two station linked with each other
BOOL  TrainTrafficGraph::IsTwoStationDirectLinked(IntegratedStation*  _pFirstStation,IntegratedStation*  _pSecondStation) const
{
	STATIONANDRALLWAY::const_iterator iter=m_trafficGraph.begin();
	STATIONANDRALLWAY::const_iterator iterLast=m_trafficGraph.end();
	int iFirstStationIndex=-1;
	int iSecondStationIndex=-1;
	for(int i=0; iter!=iterLast; ++iter,++i)
	{
		if( (*iter)->GetStation()== _pFirstStation )
			iFirstStationIndex=i;
		else if( (*iter)->GetStation()== _pSecondStation )
			iSecondStationIndex=i;
		
		if(iFirstStationIndex!=-1 && iSecondStationIndex!=-1)
			return m_trafficGraph[iFirstStationIndex]->IsDirectLinkedWithAnotherStation(iSecondStationIndex);
	}

	return false;

}
BOOL  TrainTrafficGraph::IsTwoStationDirectLinked(int _firstStationIndex,int  _secondStationIndex) const
{
	ASSERT( _firstStationIndex>=0 && (UINT)_firstStationIndex< m_trafficGraph.size() );
	ASSERT( _secondStationIndex>=0 && (UINT)_secondStationIndex< m_trafficGraph.size() );
	return m_trafficGraph[_firstStationIndex]->IsDirectLinkedWithAnotherStation(_secondStationIndex);
}
//update railway which link station _firstStationIndex and station _secondStationIndex by the type of _typeFromFirst
BOOL TrainTrafficGraph::UpdateRailWayInfo(int _firstStationIndex,int  _secondStationIndex,int _typeFromFirst,int _pointCount,const Point* _pointList)
{
	if(!IsTwoStationDirectLinked(_firstStationIndex,_secondStationIndex,_typeFromFirst))
	{
		AfxMessageBox("Two station don not link by this type!");
		return FALSE;
	}
	return m_trafficGraph[_firstStationIndex]->UpdateRailWayInfo(_secondStationIndex,_typeFromFirst,_pointCount,_pointList);

}

//get all railway 
void TrainTrafficGraph::GetAllRailWayInfo(std::vector<RailwayInfo*>& _railWayVector) const
{
	STATIONANDRALLWAY::const_iterator iter=m_trafficGraph.begin();
	STATIONANDRALLWAY::const_iterator iterLast=m_trafficGraph.end();

	_railWayVector.clear();
	RAILWAY OwnedRailWayVector;
	for(;iter!=iterLast;++iter)
	{
		OwnedRailWayVector.clear();
		(*iter)->GetOwnedRailWay(OwnedRailWayVector);

		copy(OwnedRailWayVector.begin(),OwnedRailWayVector.end(),back_inserter(_railWayVector));
	}

}

//add a new integrated station into traffic system
void TrainTrafficGraph::AddNewStation(IntegratedStation* _newStation)
{
	AdjacentStation*pTemp = new AdjacentStation();
	pTemp->SetStation(_newStation);
	m_trafficGraph.push_back(pTemp);
}

//check if all railway has defined its path correctly
bool TrainTrafficGraph::IsAllRailWayHasDefinedPath() const
{
	
	RAILWAY allRailWay;
	GetAllRailWayInfo(allRailWay);
	RAILWAY::const_iterator iter=allRailWay.begin();
	RAILWAY::const_iterator iterLast=allRailWay.end();

	for(;iter!=iterLast;++iter)
	{
		if(!(*iter)->IsPathDefined())
			return false;
	}

	return true;
}

int TrainTrafficGraph::GetStationIndexInTraffic( IntegratedStation* _pStation ) const
{
	STATIONANDRALLWAY::const_iterator iter=m_trafficGraph.begin();
	STATIONANDRALLWAY::const_iterator iterLast=m_trafficGraph.end();

	for( int i=0 ;iter!=iterLast;++iter,++i )
	{
		if( (*iter)->GetStation() == _pStation )
			return i;
	}

	return -1;
}

int TrainTrafficGraph::GetLinkedType( IntegratedStation* _pStation, RailwayInfo* _pRailWay ) const
{
	int iIndex = GetStationIndexInTraffic( _pStation);
	ASSERT( iIndex > -1 );
	int iLinkedType ;
	if ( iIndex == _pRailWay->GetFirstStationIndex() )
	{
		iLinkedType = _pRailWay->GetTypeFromFirstViewPoint();
		return iLinkedType;
	}
	else if ( iIndex == _pRailWay->GetSecondStationIndex() )
	{
		iLinkedType = _pRailWay->GetTypeFromSecondViewPoint();
		return iLinkedType;
	}
	else
	{
		//ASSERT( 0 );
	}	
	return 0;
}


BOOL TrainTrafficGraph::AdjustLinkedRailWay( IntegratedStation* _pStation )
{
	int iStationIndex = GetStationIndexInTraffic( _pStation );
	if ( -1 == iStationIndex )
		return FALSE;

	m_trafficGraph[ iStationIndex ]->AdjustRailWayPort( iStationIndex );
	return TRUE;
}

BOOL TrainTrafficGraph::AdjustAllLinkedRailWay()
{
	STATIONANDRALLWAY::const_iterator iter=m_trafficGraph.begin();
	STATIONANDRALLWAY::const_iterator iterLast=m_trafficGraph.end();

	for( int i=0 ;iter!=iterLast;++iter,++i )
	{
		(*iter)->AdjustRailWayPort( i );
	}

	return TRUE;
}

int TrainTrafficGraph::DeleteStation( IntegratedStation* _pStation )
{
	int iStationIndex = GetStationIndexInTraffic( _pStation );
	ASSERT( iStationIndex > -1 );
	
	RAILWAY allRailWay;
	RAILWAY allOwnedRailWay;
	m_trafficGraph[iStationIndex]->GetLinkedRailWay( 0 , allRailWay );
	

	RAILWAY::iterator iter = allRailWay.begin();
	RAILWAY::iterator iterEnd = allRailWay.end();
	for(; iter != iterEnd; ++iter )
	{
		if( (*iter)->GetFirstStationIndex() != iStationIndex )
		{
			if( !DeleteRailWay( (*iter)->GetFirstStationIndex(), (*iter)->GetSecondStationIndex(), (*iter)->GetTypeFromFirstViewPoint() ) )
				ASSERT( 0 );
		}
		else
		{
			if( DeleteRailWay( (*iter)->GetFirstStationIndex(), (*iter)->GetSecondStationIndex(), (*iter)->GetTypeFromFirstViewPoint() ) )
			{
				delete *iter;
			}
			else
			{
				ASSERT( 0 );
			}
		}
			
	}

	for( unsigned int i=iStationIndex+1 ; i< m_trafficGraph.size(); ++i )
	{
		m_trafficGraph[i]->AdjustStationIndex( i );
	}
	
	delete m_trafficGraph[ iStationIndex ];
	m_trafficGraph.erase( m_trafficGraph.begin() + iStationIndex );
	return iStationIndex;

	
}