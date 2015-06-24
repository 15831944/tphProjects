// AdjacentStation.h: interface for the AdjacentStation class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ADJACENTSTATION_H__4ED74CCF_B2B2_4D70_AC23_3F3B7CD77C17__INCLUDED_)
#define AFX_ADJACENTSTATION_H__4ED74CCF_B2B2_4D70_AC23_3F3B7CD77C17__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "RailwayInfo.h"
#include "IntegratedStation.h"
typedef std::vector<RailwayInfo*> RAILWAY;
class AdjacentStation  
{
private:
	//station pointer in the processor list
	IntegratedStation* m_station;

	//all railway linked with me
	RAILWAY m_adjacentRailWay;

	//since two station may have the same one railway,so at least one should have 
	// the responsibility to delete railway
	std::vector<bool> m_deleteRailWayFlag;
	
	//erase delete flag at _index
	void EraseDeleteFlag(int _index);
	
public:
	AdjacentStation();
	//copy constructor
	AdjacentStation(AdjacentStation& _anotherSation);
	virtual ~AdjacentStation();

	void SetStation(IntegratedStation* _station){ m_station= _station;};
	IntegratedStation* GetStation() const { return m_station;	}

	// add a new railway into my vector
	void AddAdjacentRailWay(RailwayInfo* _railWay,bool _mustDelete){ m_adjacentRailWay.push_back(_railWay);
															m_deleteRailWayFlag.push_back(_mustDelete);}
	// delete railway which link me with station _anotherStationIndex
	// and the link type from my point view is _linkedTypeFromMyView
	BOOL DeleteRailWay(int  _anotherStationIndex,int _linkedTypeFromMyView);

	

	AdjacentStation& operator = (const AdjacentStation& _anotherStation);

	// get railway which linked with my port(_iPortType)
	//_iPortType=0,get all railway linked with me 
	//_iPortType=1,get railway linked with my port1
	//_iPortType=2,get railway linked with my port2
	void  GetLinkedRailWay(int _iPortType,RAILWAY& _railWayVector) const;
	void  GetLinkedRailWay(int _anotherStationIndex, int _iPortType,RAILWAY& _railWayVector) const;

	//get railway which link me with another station by the type of _linkedTypeFromMyView
	const RailwayInfo*  GetLinkedRailWay(int _anotherStationIndex, int _linkedTypeFromMyView) const;

	//check if I linked with another station (_anotherStationIndex) by the type of _linkedTypeFromMyView
	BOOL IsDirectLinkedWithAnotherStation(int _anotherStationIndex, int _linkedTypeFromMyView);

	//check if I linked with another station (_anotherStationIndex) with any type
	BOOL IsDirectLinkedWithAnotherStation(int _anotherStationIndex);

	//update railway which link another station (_anotherStationIndex) with me by the type of _linkedTypeFromMyView
	BOOL UpdateRailWayInfo(int  _anotherStationIndex,int _linkedTypeFromMyView,int _pointCount,const Point* _pointList);

	// get all railway I owned
	void GetOwnedRailWay(RAILWAY& _railWayVector) const;

	// update all railway's first or last point which linked with me.
	void AdjustRailWayPort( int _iMyIndexInTrafficSystem );

	// since one station may be deleted,then the index of the station in the traffic system
	// has been changed too,
	// so all the railway liked with me  should be updated by seting the new station index as old index -1
	void AdjustStationIndex( int _iMyOldIndexInTrafficSystem );
	

};

#endif // !defined(AFX_ADJACENTSTATION_H__4ED74CCF_B2B2_4D70_AC23_3F3B7CD77C17__INCLUDED_)
