// TrainTrafficGraph.h: interface for the TrainTrafficGraph class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TRAINTRAFFICGRAPH_H__4BB6C632_6972_4189_A85A_000F497C0A53__INCLUDED_)
#define AFX_TRAINTRAFFICGRAPH_H__4BB6C632_6972_4189_A85A_000F497C0A53__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "AdjacentStation.h"
//#include "terminal.h"
#include "..\common\point.h"
typedef std::vector<AdjacentStation*> STATIONANDRALLWAY;
#include <vector>


class TrainTrafficGraph  
{
private:
	
	//all stations and its railway
	STATIONANDRALLWAY m_trafficGraph;
	
	//add railway's pointer to the AdjacentStation which linked with the railway but is not the railway's owner
	void DoRemainThings();
	
public:
	TrainTrafficGraph();
	TrainTrafficGraph(const TrainTrafficGraph& _anotherGraph);


	virtual ~TrainTrafficGraph();
	
	// get all stations from proclist add put into vector m_trafficGraph ,at the same 
	//time add all railway 
	int Init(Terminal* _terminal);
	
	// insert a new railway path which link two station
	BOOL InsertRailWay(IntegratedStation* _firstStation,IntegratedStation* _secondStation,int _typeFromFirst,int _pointCount,const Point* _pointList);
	BOOL InsertRailWay(int _firstStationIndex,int  _secondStationIndex,int _typeFromFirst,int _pointCount,const Point* _pointList);

	// delete railway which link two station
	BOOL DeleteRailWay(int _firstStationIndex,int  _secondStationIndex,int _typeFromFirst);

	TrainTrafficGraph& operator = (const TrainTrafficGraph _anotherGraph );

	const STATIONANDRALLWAY& GetTrafficGraph() const { return m_trafficGraph;	}
	
	//_iPortType=0,return all railway linked with station 
	//_iPortType=1,return railway linked with station port1
	//_iPortType=2,return railway linked with station port2
	void  GetStationLinkedRailWay(int _iStationIndex, int _iPortType,RAILWAY& _railWayVector) const;
	
	void  GetStationLinkedRailWay(int _iFirstStationIndex, int _iSecondStationIndex, int _iLinkedType,RAILWAY& _railWayVector) const;
	void  GetStationLinkedRailWay(IntegratedStation*  _pFirstStation,IntegratedStation*  _pSecondStation, int _iLinkedType,RAILWAY& _railWayVector) const;

	//get railway which link first station with second station by the type of _linkedType
	const RailwayInfo*  GetStationLinkedRailWay(int _iFirstStationIndex, int _secondStationIndex,int _linkedType) const;

	//get railway which link first station with second station by the type of _linkedType
	const RailwayInfo*  GetStationLinkedRailWay(IntegratedStation*  _pFirstStation,IntegratedStation*  _pSecondStation,int _linkedType) const;

	int GetStationIndexInTraffic( IntegratedStation* _pStation ) const;

	// check if two station linked with each other and the linked type from first station's viewpoint is _linkedType 
	BOOL  IsTwoStationDirectLinked(int _firstStationIndex,int  _secondStationIndex,int _linkedType) const;

	// check if two station linked with each other and the linked type from first station's viewpoint is _linkedType 
	BOOL  IsTwoStationDirectLinked(IntegratedStation*  _pFirstStation,IntegratedStation*  _pSecondStation,int _linkedType) const;

	// check if two station linked with each other 
	BOOL  IsTwoStationDirectLinked(IntegratedStation*  _pFirstStation,IntegratedStation*  _pSecondStation) const;
	BOOL  IsTwoStationDirectLinked(int _firstStationIndex,int  _secondStationIndex) const;


	//update railway which link station _firstStationIndex and station _secondStationIndex by the type of _typeFromFirst
	BOOL  UpdateRailWayInfo(int _firstStationIndex,int  _secondStationIndex,int _typeFromFirst,int _pointCount,const Point* _pointList);
	
	//get all railway 
	void GetAllRailWayInfo(std::vector<RailwayInfo*>& _railWayVector) const;

	//add a new integrated station into traffic system
	void AddNewStation(IntegratedStation* _newStation);
	
	//check if all railway has defined its path correctly
	bool IsAllRailWayHasDefinedPath() const;

	int GetLinkedType( IntegratedStation* _pStation, RailwayInfo* _pRailWay ) const;

	BOOL AdjustLinkedRailWay( IntegratedStation* _pStation );
	BOOL AdjustAllLinkedRailWay( );

	int DeleteStation( IntegratedStation* _pStation );

	

};

#endif // !defined(AFX_TRAINTRAFFICGRAPH_H__4BB6C632_6972_4189_A85A_000F497C0A53__INCLUDED_)
