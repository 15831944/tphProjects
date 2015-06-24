#pragma once
#include "LandsideResourceInSim.h"
#include "common/ARCMathCommon.h"
#include "common/ARCVector.h"

//Parking Spot at Lane
class LandsideLaneInSim;
class LaneParkingSpot : public LandsideResourceInSim
{
	friend class LaneParkingSpotsGroup;
public:
	LaneParkingSpot(LandsideLaneInSim * plane, DistanceUnit distf, DistanceUnit distT, LandsideResourceInSim* pParent);
	virtual LandsideLayoutObjectInSim* getLayoutObject()const{ return mpParent->getLayoutObject(); }
	virtual CString print()const{ return mpParent->print(); }
	virtual LaneParkingSpot* toLaneSpot(){ return this; }
	CPoint2008 GetEndPos()const;
	CPoint2008 GetStartPos()const;

	void SetPreOccupy(LandsideVehicleInSim* pVehicle){ mpOrderVehicle = pVehicle; }
	LandsideVehicleInSim* GetPreOccupy()const{ return mpOrderVehicle; }

	LandsideLaneInSim* GetLane()const{ return mpOnLane; }
	CPoint2008 GetParkingPos()const;
	ARCVector3 GetParkingPosDir()const;
	DistanceUnit GetDistInLane()const{ return (mdistF+mdistT)*0.5; }

	virtual void OnVehicleEnter(LandsideVehicleInSim* pVehicle, DistanceUnit dist,const ElapsedTime& t);
	virtual void OnVehicleExit(LandsideVehicleInSim* pVehicle,const ElapsedTime& t);

protected:
	LandsideLaneInSim* mpOnLane;
	DistanceUnit mdistF;
	DistanceUnit mdistT;

	int m_index; // index in the lane

	LandsideVehicleInSim* mpOrderVehicle;
	LandsideResourceInSim* mpParent;

	LandsideResourceInSim* pHead;
	LandsideResourceInSim* pLeft;
	LandsideResourceInSim* pRight;
	LandsideResourceInSim* pBehind;
};



class LandsideResourceInSim;
class LandsideLaneNodeList;

class LaneParkingSpotsGroup
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


	void addLane(LandsideLaneInSim* pLane, DistanceUnit distF, DistanceUnit distT,DistanceUnit spotlength,LandsideResourceInSim* pParent);
	void InitSpotRelations();

	LandsideResourceInSim* getLeft(LaneParkingSpot* pthis)const;
	LandsideResourceInSim* getRight(LaneParkingSpot* pthis)const;
	LandsideResourceInSim* getHead(LaneParkingSpot* pthis)const;
	LandsideResourceInSim* getBehind(LaneParkingSpot* pthis)const;

	LaneParkingSpot* getSpot(LandsideLaneInSim* pLane, int idx)const;
	
	LaneParkingSpot* getSpot(LandsideVehicleInSim* pVehicle)const;
	int getSpotCount()const { return mvParkingSpots.size(); }

	~LaneParkingSpotsGroup();

	void SetLeftDrive(bool b){ m_bLeftDrive = b; }
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

};