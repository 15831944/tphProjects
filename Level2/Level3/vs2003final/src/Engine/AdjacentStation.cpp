// AdjacentStation.cpp: implementation of the AdjacentStation class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AdjacentStation.h"
#include <algorithm>
using namespace std;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

AdjacentStation::AdjacentStation()
{

}

AdjacentStation::~AdjacentStation()
{
	for (UINT i=0;i<m_adjacentRailWay.size();++i)
	{
		if(m_deleteRailWayFlag[i] )// I have the responsibility to delete this railway 
			delete m_adjacentRailWay[i];
	}
	m_adjacentRailWay.clear();
	m_deleteRailWayFlag.clear();
}

AdjacentStation::AdjacentStation(AdjacentStation& _anotherStation)
{
	m_station=_anotherStation.m_station;

	int i=0;
	RailwayInfo* pRail=NULL;
	for(RAILWAY::iterator iter=_anotherStation.m_adjacentRailWay.begin();iter!=_anotherStation.m_adjacentRailWay.end();++iter,++i)
	{
		if(_anotherStation.m_deleteRailWayFlag[i])
		{
			m_deleteRailWayFlag.push_back(true);
			pRail=new RailwayInfo(**iter);//call RailwayInfo copy constructor
			m_adjacentRailWay.push_back(pRail);	
		}
		
		
	}
}

AdjacentStation& AdjacentStation::operator = (const AdjacentStation& _anotherStation)
{
	if(this!=&_anotherStation)
	{

		m_station=_anotherStation.m_station;

		for (UINT i=0;i<m_adjacentRailWay.size();++i)
		{
			if(m_deleteRailWayFlag[i] )
				delete m_adjacentRailWay[i];
		}
		m_adjacentRailWay.clear();
		m_deleteRailWayFlag.clear();

		i=0;
		RailwayInfo* pRail=NULL;
		for(RAILWAY::const_iterator iter=_anotherStation.m_adjacentRailWay.begin();iter!=_anotherStation.m_adjacentRailWay.end();++iter,++i)
		{
			if(_anotherStation.m_deleteRailWayFlag[i])
			{
				m_deleteRailWayFlag.push_back(true);
				pRail=new RailwayInfo(**iter);//call RailwayInfo copy constructor
				m_adjacentRailWay.push_back(pRail);	
			}
			
		}

	}

	return *this;
}


// get railway which linked with my port(_iPortType)
//_iPortType=0,get all railway linked with me 
//_iPortType=1,get railway linked with my port1
//_iPortType=2,get railway linked with my port2
void  AdjacentStation::GetLinkedRailWay(int _iPortType,RAILWAY& _railWayVector) const
{
	_railWayVector.clear();
	switch ( _iPortType )
	{
		case 0:
			{
				copy(m_adjacentRailWay.begin(),m_adjacentRailWay.end(),back_inserter(_railWayVector));
				break;;
			}
		case 1 :
			{
				RAILWAY::const_iterator iter=m_adjacentRailWay.begin();
				RAILWAY::const_iterator iterEdn=m_adjacentRailWay.end();

				for(int i=0;iter!=iterEdn;++iter,++i)
				{
					if(m_deleteRailWayFlag[i])
					{
						if((*iter)->GetTypeFromFirstViewPoint()==PORT1_PORT1 || 
							(*iter)->GetTypeFromFirstViewPoint()==PORT1_PORT2)
									
								_railWayVector.push_back(*iter);
					}
					else
					{
						if((*iter)->GetTypeFromSecondViewPoint()==PORT1_PORT2 || 
							(*iter)->GetTypeFromSecondViewPoint()==PORT1_PORT1)
									
								_railWayVector.push_back(*iter);

					}
					
				}
				break;
			}
		case 2:
			{
				RAILWAY::const_iterator iter=m_adjacentRailWay.begin();
				RAILWAY::const_iterator iterEdn=m_adjacentRailWay.end();

				for(int i=0;iter!=iterEdn;++iter,++i)
				{
					if(m_deleteRailWayFlag[i])
					{
						if((*iter)->GetTypeFromFirstViewPoint()==PORT2_PORT1 || 
								(*iter)->GetTypeFromFirstViewPoint()==PORT2_PORT2)

								_railWayVector.push_back(*iter);
					}
					else
					{
						if((*iter)->GetTypeFromSecondViewPoint()==PORT2_PORT1 || 
								(*iter)->GetTypeFromSecondViewPoint()==PORT2_PORT2)

								_railWayVector.push_back(*iter);
					}
					
				}
				break;
			}
	}
}

void  AdjacentStation::GetLinkedRailWay(int _anotherStationIndex, int _iPortType,RAILWAY& _railWayVector) const
{
	_railWayVector.clear();

	RAILWAY::const_iterator iter=m_adjacentRailWay.begin();
	RAILWAY::const_iterator iterEdn=m_adjacentRailWay.end();

	switch ( _iPortType )
	{
		case 0:
			{
				for(int i=0;iter!=iterEdn;++iter,++i)
				{
					if( m_deleteRailWayFlag [i] )
					{
						if( (*iter)->GetSecondStationIndex() == _anotherStationIndex )
							_railWayVector.push_back( *iter );
					}
					else
					{
						if( (*iter)->GetFirstStationIndex() == _anotherStationIndex )
							_railWayVector.push_back( *iter );
					}
				}
				break;;
			}
		case 1 :
			{
				for(int i=0;iter!=iterEdn;++iter,++i)
				{
					if(m_deleteRailWayFlag[i])
					{
						if( (*iter)->GetSecondStationIndex() == _anotherStationIndex &&

							( (*iter)->GetTypeFromFirstViewPoint()==PORT1_PORT1 || 
							  (*iter)->GetTypeFromFirstViewPoint()==PORT1_PORT2 )
							 ) 
									
							_railWayVector.push_back(*iter);
					}
					else
					{
						if( (*iter)->GetFirstStationIndex() == _anotherStationIndex &&

							( (*iter)->GetTypeFromSecondViewPoint()==PORT1_PORT2 || 
							 (*iter)->GetTypeFromSecondViewPoint()==PORT1_PORT1)
							 )
									
						_railWayVector.push_back(*iter);

					}
					
				}
				break;
			}
		case 2:
			{

				for(int i=0;iter!=iterEdn;++iter,++i)
				{
					if(m_deleteRailWayFlag[i])
					{
						if( (*iter)->GetSecondStationIndex() == _anotherStationIndex &&
							( (*iter)->GetTypeFromFirstViewPoint()==PORT2_PORT1 || 
								(*iter)->GetTypeFromFirstViewPoint()==PORT2_PORT2 )
							)

						_railWayVector.push_back(*iter);
					}
					else
					{
						if( (*iter)->GetFirstStationIndex() == _anotherStationIndex &&
							( (*iter)->GetTypeFromSecondViewPoint()==PORT2_PORT1 || 
								(*iter)->GetTypeFromSecondViewPoint()==PORT2_PORT2)
							)
						_railWayVector.push_back(*iter);
					}
					
				}
				break;
			}
	}
}
//get railway which link me with another station by the type of _linkedTypeFromMyView
const RailwayInfo* AdjacentStation::GetLinkedRailWay(int _anotherStationIndex, int _linkedTypeFromMyView) const
{
	RAILWAY::const_iterator iter=m_adjacentRailWay.begin();
	RAILWAY::const_iterator iterEnd=m_adjacentRailWay.end();


	for(int i=0;iter!=iterEnd;++iter,++i)
	{
		if(m_deleteRailWayFlag[i])
		{
			if((*iter)->GetSecondStationIndex()==_anotherStationIndex 
				&& (*iter)->GetTypeFromFirstViewPoint()==_linkedTypeFromMyView)
					return *iter;			
		}
		else
		{
			if((*iter)->GetFirstStationIndex()==_anotherStationIndex 
				&& (*iter)->GetTypeFromSecondViewPoint()==_linkedTypeFromMyView)
					return *iter;			
		}
		
	}

	return NULL;
}


//check if I linked with another station (_anotherStationIndex) by the type of _linkedTypeFromMyView
BOOL AdjacentStation::IsDirectLinkedWithAnotherStation(int _anotherStationIndex, int _linkedTypeFromMyView)
{
	RAILWAY::iterator iter=m_adjacentRailWay.begin();
	RAILWAY::iterator iterEnd=m_adjacentRailWay.end();


	for(int i=0;iter!=iterEnd;++iter,++i)
	{
		if(m_deleteRailWayFlag[i])
		{
			if((*iter)->GetSecondStationIndex()==_anotherStationIndex 
				&& (*iter)->GetTypeFromFirstViewPoint()==_linkedTypeFromMyView)
					return TRUE;			
		}
		else
		{
			if((*iter)->GetFirstStationIndex()==_anotherStationIndex 
				&& (*iter)->GetTypeFromSecondViewPoint()==_linkedTypeFromMyView)
					return TRUE;			
		}
		
	}

	return FALSE;
}

//check if I linked with another station (_anotherStationIndex) with any type
BOOL AdjacentStation::IsDirectLinkedWithAnotherStation(int _anotherStationIndex)
{
	RAILWAY::iterator iter=m_adjacentRailWay.begin();
	RAILWAY::iterator iterEnd=m_adjacentRailWay.end();

	for(int i=0;iter!=iterEnd;++iter,++i)
	{
		if(m_deleteRailWayFlag[i])
		{
			if((*iter)->GetSecondStationIndex()==_anotherStationIndex )
					return TRUE;			
		}
		else
		{
			if((*iter)->GetFirstStationIndex()==_anotherStationIndex )
					return TRUE;			
		}
		
	}

	return FALSE;

}
	
//update railway which link another station (_anotherStationIndex) with me by the type of _linkedTypeFromMyView
BOOL AdjacentStation::UpdateRailWayInfo(int  _anotherStationIndex,int _linkedTypeFromMyView,int _pointCount,const Point* _pointList)
{

	RAILWAY::iterator iter=m_adjacentRailWay.begin();
	RAILWAY::iterator iterEnd=m_adjacentRailWay.end();

	for(int i=0;iter!=iterEnd;++iter,++i)
	{
		if(m_deleteRailWayFlag[i])
		{
			if((*iter)->GetSecondStationIndex()==_anotherStationIndex && (*iter)->GetTypeFromFirstViewPoint()==_linkedTypeFromMyView)
			{
				(*iter)->SetRailWay(_pointCount,_pointList);
				return TRUE;			
			}
				
		}
		else
		{
			if((*iter)->GetFirstStationIndex()==_anotherStationIndex && (*iter)->GetTypeFromSecondViewPoint()==_linkedTypeFromMyView)
			{
				(*iter)->SetRailWay(_pointCount,_pointList);
				return TRUE;			
			}
				
		}
		
	}

	return FALSE;
}


// get all railway I owned
void AdjacentStation::GetOwnedRailWay(RAILWAY& _railWayVector) const
{
	_railWayVector.clear();
	RAILWAY::const_iterator iter=m_adjacentRailWay.begin();
	RAILWAY::const_iterator iterEdn=m_adjacentRailWay.end();
	for(int i=0;iter!=iterEdn;++iter,++i)
	{
		if(m_deleteRailWayFlag[i])
		{
			_railWayVector.push_back(*iter);
		}
	}
}


// delete railway which link me with station _anotherStationIndex
// and the link type from my point view is _linkedTypeFromMyView
BOOL AdjacentStation::DeleteRailWay(int  _anotherStationIndex,int _linkedTypeFromMyView)
{
	RAILWAY::iterator iter=m_adjacentRailWay.begin();
	RAILWAY::iterator iterEdn=m_adjacentRailWay.end();
	for(int i=0;iter!=iterEdn;++iter,++i)
	{
		if((*iter)->GetOwnerIndex() != _anotherStationIndex)// I owned this railway
		{
			if( (*iter)->GetSecondStationIndex() == _anotherStationIndex
				&& (*iter)->GetTypeFromFirstViewPoint() == _linkedTypeFromMyView )
			{
				//delete (*iter);
				EraseDeleteFlag(i);
				m_adjacentRailWay.erase(iter);
				return TRUE;
			}
		}
		else
		{
			if((*iter)->GetFirstStationIndex() == _anotherStationIndex
				&& (*iter)->GetTypeFromSecondViewPoint() == _linkedTypeFromMyView )
			{
				EraseDeleteFlag(i);
				m_adjacentRailWay.erase(iter);
				return TRUE;
			}
		}
	}

	return FALSE;
}

//erase delete flag at _index
void AdjacentStation::EraseDeleteFlag(int _index)
{
	typedef std::vector<bool> BOOLVECTOR;
	BOOLVECTOR::iterator iter=m_deleteRailWayFlag.begin();
	
	for(int i=0;i< _index;++i,++iter){};

	m_deleteRailWayFlag.erase(iter);

}
void AdjacentStation::AdjustRailWayPort( int _iMyIndexInTrafficSystem )
{
	Point portPoint = m_station->GetPort1Path()->getPoint( 0 );
	
	RAILWAY linkedRailWay;
	GetLinkedRailWay( 1, linkedRailWay );

	RAILWAY::const_iterator iter=linkedRailWay.begin();
	RAILWAY::const_iterator iterEdn=linkedRailWay.end();
	for(;iter!=iterEdn;++iter)
	{
		int iPointCount = (*iter)->GetRailWayPath().getCount();
		if ( (*iter)->GetFirstStationIndex() == _iMyIndexInTrafficSystem )
		{
			//(*iter)->GetRailWayPath().getPointList()->setPoint( portPoint.getX(), portPoint.getY(), 0 );
			(*iter)->GetRailWayPath().getPointList()->setPoint( portPoint.getX(), portPoint.getY(), portPoint.getZ() );
		}
		else
		{
			Point* pList = (*iter)->GetRailWayPath().getPointList();
			//pList[ iPointCount-1 ].setPoint( portPoint.getX(), portPoint.getY(), 0 );
			pList[ iPointCount-1 ].setPoint( portPoint.getX(), portPoint.getY(), portPoint.getZ() );
		}
	}


	portPoint = m_station->GetPort2Path()->getPoint( 0 );
	GetLinkedRailWay( 2, linkedRailWay );	
	iter=linkedRailWay.begin();
	iterEdn=linkedRailWay.end();
	for(;iter!=iterEdn;++iter)
	{
		int iPointCount = (*iter)->GetRailWayPath().getCount();
		if ( (*iter)->GetFirstStationIndex() == _iMyIndexInTrafficSystem )
		{
			//(*iter)->GetRailWayPath().getPointList()->setPoint( portPoint.getX(), portPoint.getY(), 0 );
			(*iter)->GetRailWayPath().getPointList()->setPoint( portPoint.getX(), portPoint.getY(), portPoint.getZ() );
		}
		else
		{
			Point* pList = (*iter)->GetRailWayPath().getPointList();
			//pList[ iPointCount-1 ].setPoint( portPoint.getX(), portPoint.getY(), 0);
			pList[ iPointCount-1 ].setPoint( portPoint.getX(), portPoint.getY(), portPoint.getZ());
		}
	}
	
}

// since one station may be deleted,then the index of the station in the traffic system
// has been changed too,
// so all the railway liked with me  should be updated by seting the new station index as old index -1
void AdjacentStation::AdjustStationIndex( int _iMyOldIndexInTrafficSystem )
{
	RAILWAY::iterator iter=m_adjacentRailWay.begin();
	RAILWAY::iterator iterEdn=m_adjacentRailWay.end();
	for( ; iter != iterEdn; ++iter )
	{
		if ( (*iter)->GetFirstStationIndex() == _iMyOldIndexInTrafficSystem )
		{
			(*iter)->SetFirstStationIndex( _iMyOldIndexInTrafficSystem -1 );
			(*iter)->SetOwnerIndex( _iMyOldIndexInTrafficSystem -1 );
		}
		else
		{
			(*iter)->SetSecondStationIndex( _iMyOldIndexInTrafficSystem -1 );
		}
	}
}