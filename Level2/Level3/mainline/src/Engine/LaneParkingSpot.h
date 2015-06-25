#pragma once
#include "LandsideResourceInSim.h"
#include "common/ARCMathCommon.h"
#include "common/ARCVector.h"
#include "Common/Point2008.h"
class LandsideStretchLaneInSim;
class LandsideResourceInSim;
class LandsideLaneNodeList;
class LandsideLinkStretchObject;
class LaneSegInSim;
class OutLaneParkingSpot;
class LaneParkingSpot;

#define TODERIVE_DECL(dclass, func) virtual  dclass * func(){ return NULL; }  virtual const dclass * func()const{ return NULL; }
#define TODERIVE_IMPL(dclass, func) virtual dclass * func(){ return this; } virtual  const dclass * func()const{ return this; }

class IParkingSpotInSim :public LandsideResourceInSim
{
public:	
	IParkingSpotInSim();
	IParkingSpotInSim* toLaneSpot(){ return this; }

	TODERIVE_DECL(OutLaneParkingSpot, toOutLanePark);
	TODERIVE_DECL(LaneParkingSpot, toInLanePark);


	LandsideLayoutObjectInSim* getLayoutObject() const;
	virtual CString print()const{ return m_pParent->print(); }

	void SetPreOccupy(LandsideVehicleInSim* pVehicle){ mpOrderVehicle = pVehicle; }
	LandsideVehicleInSim* GetPreOccupy()const{ return mpOrderVehicle; }
	const CPoint2008& GetParkingPos()const{ return m_pos; }
	const ARCVector3& GetParkingPosDir()const{ return m_dir; }
	void SetPosDir( const ARCPoint3& pos ,const ARCVector3& dir );

	virtual void OnVehicleExit(LandsideVehicleInSim* pVehicle,const ElapsedTime& t);

	
protected:
	LandsideVehicleInSim* mpOrderVehicle;
	CPoint2008 m_pos;
	ARCVector3 m_dir;
	LandsideResourceInSim* m_pParent;
};


//outside stretch parking spaces
class OutLaneParkingSpot : public IParkingSpotInSim
{
public:
	TODERIVE_IMPL(OutLaneParkingSpot, toOutLanePark);
	OutLaneParkingSpot(LandsideLaneInSim* pLinkLane, LandsideResourceInSim* parent);
	DistanceUnit getEntryDistInLane()const{ return m_entryDistInLane; }
	DistanceUnit getExitDistInLane()const{ return m_exitDistInlane; }
	double  getEntryIndexInlane()const{ return m_dIndexEntry; }
	double getExitIndexInlane()const{ return m_dIndexExit; }

	LandsideLaneInSim* getLinkLane()const { return m_pLinkLane; }

	void SetLinkLane(LandsideLaneInSim* pLinkLane){ m_pLinkLane = pLinkLane; }
	void SetEntryExitDist(DistanceUnit entryDist, DistanceUnit exitDist); 
protected:
	LandsideLaneInSim* m_pLinkLane;
	DistanceUnit m_entryDistInLane;
	DistanceUnit m_exitDistInlane;
	double m_dIndexEntry;
	double m_dIndexExit;
	
};

class ParkingSpace;
class LandsideStretchInSim;
class OutLaneParkingSpotGroup
{
public:	
	void Init(LandsideLinkStretchObject * pLinkStretch,LandsideStretchInSim* pStretch,LandsideResourceInSim* parent);
		
	void getMinEntryExit(double& dentrydistinStertch, double& dexitinStretch)const;

	OutLaneParkingSpot* FindParkingSpot(LandsideVehicleInSim* pVehicle, LandsideLaneNodeList& followPath)const;
protected:
	std::vector<OutLaneParkingSpot*> m_vSpots;
	void addParkingSpace(ParkingSpace & space, LandsideStretchInSim* pLinkStretch,double dHeight, LandsideResourceInSim* parent );
	bool _getParkspotPath( LandsideLaneInSim* pFromlane, const CPoint2008& dFromPos, OutLaneParkingSpot* pSpot, LandsideLaneNodeList& followPath )const;
	OutLaneParkingSpot* _findParkingPos(LandsideVehicleInSim* pVehicle, LandsideLaneInSim* patLane, LandsideLaneNodeList& followPath)const;
};


//inside stretch parking spots
class LaneParkingSpot : public IParkingSpotInSim
{
	friend class InLaneParkingSpotsGroup;
public:
	TODERIVE_IMPL(LaneParkingSpot, toInLanePark);

	LaneParkingSpot(LandsideLaneInSim * plane, DistanceUnit distf, DistanceUnit distT, LandsideResourceInSim* pParent);


	const CPoint2008& GetEndPos()const{ return m_startPos; }
	const CPoint2008& GetStartPos()const{ return m_endPos; }



	LandsideLaneInSim* GetLane()const{ return mpOnLane; }
	DistanceUnit GetDistInLane()const{ return (mdistF+mdistT)*0.5; }

	virtual void OnVehicleEnter(LandsideVehicleInSim* pVehicle, DistanceUnit dist,const ElapsedTime& t);
	virtual void OnVehicleExit(LandsideVehicleInSim* pVehicle,const ElapsedTime& t);

protected:
	LandsideLaneInSim* mpOnLane;
	DistanceUnit mdistF;
	DistanceUnit mdistT;

	CPoint2008 m_startPos;
	CPoint2008 m_endPos;

	int m_index; // index in the lane


	LandsideResourceInSim* pHead;
	LandsideResourceInSim* pLeft;
	LandsideResourceInSim* pRight;
	LandsideResourceInSim* pBehind;
};



class InLaneParkingSpotsGroup
{
public:
	//LandsideLaneNodeList
	LaneParkingSpot* FindParkingPos( LandsideVehicleInSim* pVehicle, LandsideLaneNodeList& followPath )const;
	LaneParkingSpot* FindParkingPos( LandsideVehicleInSim* pVehicle, LandsideLaneInSim* pLane , LandsideLaneNodeList& followPath ) const;
	LaneParkingSpot* FindParkingPosRandom( LandsideVehicleInSim* pVehicle, LandsideLaneNodeList& followPath )const;
	LaneParkingSpot* FindParkingPosRandom( LandsideVehicleInSim* pVehicle, LandsideLaneInSim* pLane , LandsideLaneNodeList& followPath)const;

	//bool FindLeavePath( LandsideVehicleInSim* pVehicle, LandsideLaneNodeList& followPath );
	bool FindLeavePath( LandsideVehicleInSim* pVehicle, LaneParkingSpot* pSpot, LandsideLaneNodeList& followPath );
	void ReleaseParkingPos( LandsideVehicleInSim* pVehicle ,const ElapsedTime& t);

	void Init(LandsideLinkStretchObject * pLinkStretch,LandsideStretchInSim* pStretch,LandsideResourceInSim* parent);


	LandsideResourceInSim* getLeft(LaneParkingSpot* pthis)const;
	LandsideResourceInSim* getRight(LaneParkingSpot* pthis)const;
	LandsideResourceInSim* getHead(LaneParkingSpot* pthis)const;
	LandsideResourceInSim* getBehind(LaneParkingSpot* pthis)const;

	LaneParkingSpot* getSpot(LandsideLaneInSim* pLane, int idx)const;
	
	LaneParkingSpot* getSpot(LandsideVehicleInSim* pVehicle)const;
	int getSpotCount()const { return mvParkingSpots.size(); }

	~InLaneParkingSpotsGroup();

	void SetLeftDrive(bool b){ m_bLeftDrive = b; }

	void addLane(LandsideStretchLaneInSim* pLane, DistanceUnit distF, DistanceUnit distT,DistanceUnit spotlength,LandsideResourceInSim* pParent);
	void InitSpotRelations();

protected:
	std::vector<LaneParkingSpot*> mvParkingSpots;
	bool FindParkspotPath(LandsideLaneInSim* pFromlane, const CPoint2008& dFromPos, LaneParkingSpot* pSpot, LandsideLaneNodeList& followPath)const;

	//find the entry point can go to the spot
	LandsideLaneInSim* FindPosEntrySpot(LaneParkingSpot* pSpot, CPoint2008& pos)const;
	LandsideLaneInSim* FindPosEntrySpotR(LaneParkingSpot* pSpot, CPoint2008& pos)const;
	LandsideLaneInSim* FindPosEntrySpotL(LaneParkingSpot* pSpot, CPoint2008& pos)const;

	//find the exit point can exit the spot
	LandsideLaneInSim* FindPosExitToLane(LaneParkingSpot* pSpot,CPoint2008& pos)const;
	LandsideLaneInSim* FindPosExitToLaneR(LaneParkingSpot* pSpot,CPoint2008& pos)const;
	LandsideLaneInSim* FindPosExitToLaneL(LaneParkingSpot* pSpot,CPoint2008& pos)const;

	bool m_bLeftDrive;
	std::vector<LaneSegInSim*> m_vLaneOccupy; //replace with 

};

//all the parking spots in the bus station/taxi queue/curbside
class LandsidePosition;
class LandsideInterfaceParkingSpotsGroup
{
public:
	void Init(LandsideLinkStretchObject * pLinkStretch,LandsideResourceManager* allRes,LandsideResourceInSim* parent);
	IParkingSpotInSim * FindParkingPos( LandsideVehicleInSim* pVehicle, LandsideLaneNodeList& followPath )const;
	bool FindLeavePosition( LandsideVehicleInSim* pVehicle, IParkingSpotInSim* pSpot, LandsidePosition& pos );	
	InLaneParkingSpotsGroup& getInStretchSpots(){ return m_inlaneGroup; }

	double GetEntryIndexInStrech()const { return m_dEntryIndexInStretch; }
	double GetExitIndexInStretch()const{ return m_dExitIndexInStretch; }
protected:
	InLaneParkingSpotsGroup m_inlaneGroup;
	OutLaneParkingSpotGroup m_outLaneGroup;
	double m_dEntryIndexInStretch;
	double m_dExitIndexInStretch;

};