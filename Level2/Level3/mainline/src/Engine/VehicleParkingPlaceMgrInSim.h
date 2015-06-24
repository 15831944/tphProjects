#pragma once
#include "../Common/Path2008.h"
#include "../Common/Point2008.h"
#include <deque>
class AirsideVehicleInSim;
class CFlightServiceRoute;
////////////////////////////////////////////////////////////////////////////////////////////////
class VehicleParkingPlaceInSim
{
public:
	VehicleParkingPlaceInSim();
	virtual ~VehicleParkingPlaceInSim();

	struct VehicleParkingPosition
	{
		VehicleParkingPosition()
			:m_dRowHeight(0.0)
			,m_dColWidth(0.0)
		{

		}
		AirsideVehicleInSim* m_pVehicleInsim;
		CPoint2008 m_lcation;
		CPoint2008 m_vDir;
		double m_dRowHeight;
		double m_dColWidth;
	};

	void Clear();
	void HoldVehicle(AirsideVehicleInSim* pVehicleInSim);
	bool GetVehicleDirection(AirsideVehicleInSim* pVehicleInSim,CPoint2008& point)const;
	bool GetVehiclePosition(AirsideVehicleInSim* pVehicleInSim,CPoint2008& point)const;
	virtual void InitResource(const CPath2008& path,const CPoint2008& point,const CPoint2008& vDir);
	void WakeupParkingPlaceVehicle(CFlightServiceRoute* pFlightServiceRoute,const ElapsedTime &eTime);

protected:
	double GetMaxHeight()const;

	void InitDirection(const CPoint2008& vDir);
	virtual void InitParkingPlaceDirection(const CPoint2008& vDir) = 0;

	CPoint2008 AdjustDirection()const;
	virtual CPoint2008 AdjustParkingPlaceDirection()const = 0; 

protected:
	CPoint2008 m_location;
	double m_dWidth;//default value 20m
	CPoint2008 m_vDir;//parking place direction
	std::deque<VehicleParkingPosition> m_vVehicleInSim;
};

///////////////////////////////////////////////////////////////////////////////////////////////
class LeftVehilceParkingPlaceInSim : public VehicleParkingPlaceInSim
{
public:
	LeftVehilceParkingPlaceInSim();
	~LeftVehilceParkingPlaceInSim();

public:
	virtual void InitParkingPlaceDirection(const CPoint2008& vDir);
	virtual CPoint2008 AdjustParkingPlaceDirection()const;
};

///////////////////////////////////////////////////////////////////////////////////////////////
class RightVehicleParkingPlaceInSim : public VehicleParkingPlaceInSim
{
public:
	RightVehicleParkingPlaceInSim();
	~RightVehicleParkingPlaceInSim();

public:
	virtual void InitParkingPlaceDirection(const CPoint2008& vDir);
	virtual CPoint2008 AdjustParkingPlaceDirection()const;
};

///////////////////////////////////////////////////////////////////////////////////////////////
class VehicleParkingPlaceMgrInSim
{
public:
	VehicleParkingPlaceMgrInSim();
	~VehicleParkingPlaceMgrInSim();

public:
	void InitResource(const CPath2008& path,const CPoint2008& point,const CPoint2008& vDir);
	void EnterParkingPlace(AirsideVehicleInSim* pVehicleInSim);
	void ExitParkingPlace(CFlightServiceRoute* pFlightServiceRoute,const ElapsedTime &eTime);
	bool GetVehiclePosition(AirsideVehicleInSim* pVehicleInSim,CPoint2008& point);
	bool GetVehicleDirection(AirsideVehicleInSim* pVehicleInSim,CPoint2008& point);
	void Clear();

protected:
	bool m_bSideParkingPlace;//left or right side parking assign
	VehicleParkingPlaceInSim* m_leftVehicleParkingPlace;
	VehicleParkingPlaceInSim* m_RightVehicleParkingPlace;
};
