#pragma once

#include "LandsideStretchInSim.h"
#include "Landside\LandsideParkingLot.h"
//////////////////////////////////////////////////////////////////////////
class LandsideParkingLotInSim;
class LandsideParkingSpaceInSim;
class LandsideParkingSpotConstraints;
class PaxLandsideBehavior;
class LandsideSimulation;

//class ParkingLotStateChange : public SSignal
//{
//public:
//	CString getDesc(){ return _T("Parking Lot State Change"); }
//};

class LandsideParkingSpotInSim : public LandsideResourceInSim
{
	
public:
	LandsideParkingSpotInSim(LandsideParkingSpaceInSim* pSpaceInSim,int idx, int nParkLotLevel);
	virtual CString print()const;
	virtual LandsideLayoutObjectInSim* getLayoutObject()const;
	LandsideParkingSpotInSim* toParkLotSpot(){ return this; }

	virtual void OnVehicleEnter(LandsideVehicleInSim* pVehicle,DistanceUnit dist, const ElapsedTime& t);
	virtual void OnVehicleExit(LandsideVehicleInSim* pVehicle,const ElapsedTime& t);

	
	const CPoint2008& getPos()const{ return m_pos; }
	const ARCVector3& getDir()const{ return m_dir; }
	void setPosDir(const ARCVector3& pos ,const ARCVector3& dir);
	CPoint2008 getEntryPos()const{ return m_entryPos[0]; }
	CPoint2008 getEntryPos2()const{ return m_entryPos[1]; }
	void setEntryPos(const CPoint2008& p1){ m_entryPos[0]=p1; }
	void setEntryPos2(const CPoint2008& p2){ m_entryPos[1] = p2;}
	
	//if is parallel it will have 2 entry pos
	bool Is_parallel()const;
	
	LandsideParkingLotInSim* getParkingLot()const;	
	LandsideVehicleInSim* getOccupyVehicle()const;
	void setOccupyVehicle(LandsideVehicleInSim*pVehicle){ AddInResVehicle(pVehicle); }

	//level 0,1,2,3...
	int getParkingLotLevelIndex() const;
	void setParkingLotLevelIndex(int nLevel);

	//database id
	int GetParkingLotLevelID();
	int m_idx;

	CPoint2008 GetPaxWaitPos()const;
public:
	virtual void PassengerMoveInto(PaxLandsideBehavior *pPaxBehvior, ElapsedTime eTime);
	virtual void PassengerMoveOut(PaxLandsideBehavior *pPaxBehvior, ElapsedTime eTime);

protected:
	LandsideParkingSpaceInSim* mpSpace;	
	CPoint2008 m_pos;
	ARCVector3 m_dir;
	CPoint2008 m_entryPos[2];

	int m_nParkinglotLevel;
};


//////////////////////////////////////////////////////////////////////////
class LandsideParkingLotDoorInSim : public LandsideResourceInSim
{
friend class LandsideParkingLotLevelInSim;
public:
	LandsideParkingLotDoorInSim(LandsideParkingLotLevelInSim* pLot,ParkLotEntryExit& door);	
	LandsideParkingLotDoorInSim * toParkLotDoor(){ return this; }
	virtual void OnVehicleExit(LandsideVehicleInSim* pVehicle,const ElapsedTime& t);
	virtual CString print()const;
	virtual LandsideLayoutObjectInSim* getLayoutObject()const;
	//
	LandsideParkingLotInSim* getParkingLot()const;


	CPoint2008 getPos()const;
	CPoint2008 getPortPos(int idx)const;
	int getPortCount()const{ return m_door.m_nInOutPort; }

	std::vector<LandsideLaneEntry*> m_vLaneEntries;
	std::vector<LandsideLaneExit*> m_vLaneExits;	
	const ParkLotEntryExit& getInput()const{ return m_door; }
	
	int getVehiclePortIndex(LandsideVehicleInSim* pVeh)const;
	int allocatVehicleToPort(LandsideVehicleInSim* pVeh);
protected:
	LandsideParkingLotLevelInSim* mpLotLevel;	
	ParkLotEntryExit m_door;
	std::vector<LandsideVehicleInSim*> mVehicleOrder;
};
//////////////////////////////////////////////////////////////////////////
//a group of spots
class LandsideParkingSpaceInSim
{
public:
	friend class LandsideParkingLotLevelInSim;
	friend class LandsideParkingSpotInSim;

	LandsideParkingSpaceInSim(LandsideParkingLotLevelInSim* pLot,ParkingSpace& space,int idx);
	~LandsideParkingSpaceInSim(){ Clear(); }

	int getSpotCount()const{ return m_vSpots.size(); }
	LandsideParkingSpotInSim* getSpot(int idx){ return m_vSpots[idx]; }


	LandsideParkingLotLevelInSim* getParkingLotLevel() const;
	LandsideParkingLotInSim* getParkingLot()const;	 

	int GetIndex()const{ return m_index; }
protected:
	int m_index;
	std::vector<LandsideParkingSpotInSim*> m_vSpots;
	ParkingSpace mspace;
	void Clear();
	LandsideParkingLotLevelInSim* mpLotLevel;
};

class LandsideParkingLotDrivePipeInSim;

/////intersection with walkway
class CWalkwayInSim;
struct  IntersectWalkway{
	CWalkwayInSim* pWalkway;
	DistanceUnit distInWalkway;
	DistanceUnit distInLane;

	DistanceUnit distHoldInLane;
	DistanceUnit distHoldExitInLane;
	
	bool operator<(const IntersectWalkway& other)const{
		return distHoldInLane < other.distHoldInLane;
	}
};

class LandsideParkinglotDriveLaneInSim : public LandsideResourceInSim
{
	friend class LandsideParkingLotDrivePipeInSim;
public:
	LandsideParkinglotDriveLaneInSim(LandsideParkingLotDrivePipeInSim* pPipe,bool otherdir);

	virtual CString print()const{ return "ParkingLot Drive Lane"; }
	const CPath2008& getPath()const{ return m_path; }
	const ARCPath3& getARCPath()const{ return m_arcpath; }
		
	virtual LandsideLayoutObjectInSim* getLayoutObject()const;
	virtual LandsideParkinglotDriveLaneInSim* toParkDriveLane(){ return this; }
	
	void InitRelateWithOtherRes(LandsideResourceManager* allRes);

	int GetIntersetWalks(DistanceUnit distF , DistanceUnit distT,std::vector<IntersectWalkway>& ret )const;

	virtual LandsideVehicleInSim* GetAheadVehicle(LandsideVehicleInSim* mpVehicle, DistanceUnit distInLane, DistanceUnit& aheadDist);

	virtual void  OnVehicleEnter(LandsideVehicleInSim* pVehicle, DistanceUnit dist, const ElapsedTime& t);
protected:
	ARCPath3 m_arcpath;
	CPath2008 m_path;
	LandsideParkingLotDrivePipeInSim* m_pPipe;
	bool m_botherdir;
	std::vector<IntersectWalkway> m_vIntersectWalkway;
	void CaculateIntesectWithWalkway( CWalkwayInSim* pRes );
};
//class 


class LandsideParkingLotDrivePipeInSim : public LandsideResourceInSim
{
public:
	LandsideParkingLotDrivePipeInSim(LandsideParkingLotLevelInSim* pLot,const ParkingDrivePipe& lane,bool bLeftDrive );


	virtual LandsideParkingLotDrivePipeInSim* toParkDrivePipe(){ return this; }

	double getWidth()const{ return m_drivelane.m_width; }	
	virtual CString print()const{ return "ParkingLot Drive Pipe"; }
	virtual LandsideLayoutObjectInSim* getLayoutObject()const;
	const CPath2008& getPath()const{ return m_path; }

	bool IsBiDirection()const;

	LandsideParkinglotDriveLaneInSim* getLane(bool breverse);

	void InitRelateWithOtherRes(LandsideResourceManager* allRes);

protected:
	//const CPath2008& getPath()const{ return m_drivelane.m_ctrlPath; }
	std::vector<LandsideParkinglotDriveLaneInSim*> m_vLanes;
	LandsideParkingLotLevelInSim* mpLotLevel;
	ParkingDrivePipe m_drivelane;
	CPath2008 m_path;
};
//////////////////////////////////////////////////////////////////////////
//parking lot level
class LandsideParkingLotGraph;
class LandsideParkingLotLevelInSim
{
public:
	//enum level_type{ _base, _other,  };
	LandsideParkingLotLevelInSim(LandsideParkingLotInSim* plot , /*level_type t,*/ int idx);
	~LandsideParkingLotLevelInSim(){ Clear(); }

	virtual void Init(bool bLeftDrive);

	LandsideParkingLotInSim* getLot()const{ return m_pLot; }
	DistanceUnit getHeight()const;


	void Clear();
	void InitRelation(LandsideResourceManager* pAllRes);

	bool IsSpotLess() const;

	LandsideParkingLotDoorInSim* getExitDoor( LandsideVehicleInSim* pVehicle );

	LandsideParkingSpotInSim* getFreeSpot( LandsideVehicleInSim* pVehicle );

	int GetRowCount()const { return (int)m_vParkSpaces.size();}
	LandsideParkingSpaceInSim* GetRowParkingSpace(int idx) const {return m_vParkSpaces[idx];}

	//LandsideParkingLotDoorInSim* getEntryDoor( LandsideVehicleInSim* pVehicle );

	void InitGraph(LandsideParkingLotGraph* );
	LandsideParkingLotDrivePipeInSim* getClosestLane( const ARCPoint3& cpt,	DistanceUnit& distInLane ,DistanceUnit& minDisTo) const;

	int getLevelIndex()const; //base : -1 , other index
	int GetLevelID();

	int getDoorCount()const{ return (int)m_vDoors.size(); }
	LandsideParkingLotDoorInSim* getDoor(int idx)const{ return m_vDoors[idx]; };
protected:
	LandsideParkingLotInSim* m_pLot;
	//level_type m_type;
	int m_idx;
	std::vector<LandsideParkingSpaceInSim*> m_vParkSpaces;
	std::vector<LandsideParkingLotDoorInSim*> m_vDoors;
	std::vector<LandsideParkingLotDrivePipeInSim*> m_vDriveLanes;

};

class LandsideParkingLot;
class LandsideParkingLotGraph;
class LandsideParkingLotInSim :  public LandsideLayoutObjectInSim, public LandsideResourceInSim
{
public:
	LandsideParkingLotInSim(LandsideParkingLot* pInput,bool bLeftDrive);
	~LandsideParkingLotInSim(void);
	virtual CString print()const;
	virtual LandsideLayoutObjectInSim* getLayoutObject()const;
	LandsideResourceInSim* IsResource(){ return this; }
	LandsideParkingLotInSim* toParkLot(){ return this; }

	bool IsCellPhone()const;

	virtual void InitRelateWithOtherObject(LandsideResourceManager* pRes);
	virtual void InitGraphNodes(LandsideRouteGraph* pGraph)const{};
	virtual void InitGraphLink(LandsideRouteGraph* pGraph)const{};

	int getLevelCount()const{ return (int)m_vLevels.size(); }
	LandsideParkingLotLevelInSim* getLevel(int idx)const{ return m_vLevels[idx]; }
	DistanceUnit getLevelIndexHeight(double indexZ)const;
	DistanceUnit getLevelHeight(int idx)const;

	//no spots ?
	bool IsSpotLess()const;
	LandsideParkingSpotInSim* getFreeSpot(LandsideVehicleInSim* pVehicle);	
	//find a door can exit lot
	LandsideParkingLotDoorInSim* getExitDoor(LandsideVehicleInSim* pVehicle);
	//find door can enter lot
	//LandsideParkingLotDoorInSim* getEntryDoor(LandsideVehicleInSim* pVehicle);


	LandsideParkingLotDoorInSim* getNearestEntryDoor(LandsideResourceManager* pRes ,LandsideVehicleInSim* pVehicle);

	LandsideParkingLotDoorInSim* getDoorToSpot(LandsideVehicleInSim* pVehicle, LandsideParkingSpotInSim*pSpot);

	//------------------------------------------------------------------------------
	//Summary:
	//		intersect with area and select the shortest walkway
	//Parameters:
	//		pt[in]: current point to calculate the distance to select best walkway
	//		LandsideResourceManager[in]: get walkway list to select best walkway
	//Return:
	//		retrieve the best walkway
	//------------------------------------------------------------------------------
	CLandsideWalkTrafficObjectInSim* getBestTrafficObjectInSim(const CPoint2008& pt,LandsideResourceManager* allRes);

	//LandsideParkingLot* mpLot;

	//get closest drive lane
	LandsideParkingLotDrivePipeInSim* getClosestLane(const ARCPoint3& pt, DistanceUnit& distInLane, DistanceUnit& minDisTo)const;

	LandsideParkingLotGraph* getDriveGraph(){ return m_pDriveGraph; }
	LandsideParkingLot* getLotInput()const{ return (LandsideParkingLot*)getInput(); }

	void SetParkingLotConstraint(LandsideParkingSpotConstraints* pParkingLotCon);

	void ClearEmbedBusStation();
	void AddEmbedBusStation(LandsideBusStationInSim * pStation);
	int GetEmbedBusStationCount() const;
	LandsideBusStationInSim * GetEmbedBusStation(int nIndex);

	LandsideBusStationInSim* GetBestBusStationGetOff(LandsideSimulation *pSimulation,PaxLandsideBehavior* pLandsideBehavior);
private:
	//--------------------------------------------------------------------------
	//Summary:
	//		parking lot intersect with traffic object
	//--------------------------------------------------------------------------
	void InitTrafficObjectOverlap(LandsideResourceManager* allRes);
protected:
	void InitGraph();
	
	
	std::vector<LandsideParkingLotLevelInSim*> m_vLevels;
	LandsideParkingLotGraph* m_pDriveGraph;
	bool	m_bInit;//whether init traffic object

	LandsideParkingSpotConstraints* m_pLandsideParkingSpotCon;

	std::vector<LandsideBusStationInSim *> m_vEmbedBusStationInSim;

	void Clear();
};
