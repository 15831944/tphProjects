#include "stdafx.h"
#include "VehicleParkingPlaceMgrInSim.h"
#include "../Common/RayIntersectPath.h"
#include "Airside/AirsideVehicleInSim.h"
#include "Airside/FlightServiceRoute.h"
#include <limits>

VehicleParkingPlaceInSim::VehicleParkingPlaceInSim()
:m_dWidth(2000.0)
{

}

VehicleParkingPlaceInSim::~VehicleParkingPlaceInSim()
{

}

void VehicleParkingPlaceInSim::InitResource(const CPath2008& path,const CPoint2008& point,const CPoint2008& vDir)
{
	InitDirection(vDir);

	CRayIntersectPath2D rayPath(point,m_vDir,path);

	if (rayPath.Get())
	{
		m_location = path.GetIndexPoint((float)rayPath.m_Out.begin()->m_indexInpath);
		m_location += m_vDir*500.0;
	}
}

void VehicleParkingPlaceInSim::Clear()
{
	m_vVehicleInSim.clear();
}

void VehicleParkingPlaceInSim::InitDirection(const CPoint2008& vDir)
{
	InitParkingPlaceDirection(vDir);
}

CPoint2008 VehicleParkingPlaceInSim::AdjustDirection()const
{
	return AdjustParkingPlaceDirection();
}
/*
		col
		|<---------20m ------------------------->|
		-----------------------------------------
	row	||	Vehicle  |<-1m->| Vehicle |			|
		|					|		  |			|
		|					-					|
		|					|1m					|
		|					-					|
		|| Vehicle|<-1m->| Vehicle |			|
		|										|
		|										|
		|										|
		------------------------------------------

*/

void VehicleParkingPlaceInSim::HoldVehicle(AirsideVehicleInSim* pVehicleInSim)
{
	double dColWidth = 0.0;
	double dRowHeight = 0.0;

	if (!m_vVehicleInSim.empty())
	{
		VehicleParkingPosition& backPosition = m_vVehicleInSim.back();
		dColWidth = backPosition.m_dColWidth;
		double totalWidth = dColWidth + pVehicleInSim->GetVehicleWidth() + 100.0;
		/*if (totalWidth > m_dWidth)
		{
			dColWidth = 0.0;
			dRowHeight = GetMaxHeight() + 100.0;
		}
		else*/
		{
			dColWidth += 100.0;//add gap
			dRowHeight = backPosition.m_dRowHeight;
		}
	}

	CPoint2008 vDir = m_vDir;
	CPoint2008 offsetPt1 = vDir*(pVehicleInSim->GetVehicleWidth()/2 + dColWidth);
	vDir = AdjustDirection();
	CPoint2008 offsetPt2 = vDir*(pVehicleInSim->GetVehicleLength()/2 + dRowHeight);

	CPoint2008 vehiclePt;
	vehiclePt = m_location + offsetPt2 + offsetPt1;
	dColWidth += pVehicleInSim->GetVehicleWidth();

	VehicleParkingPosition vehiclePosition;
	vehiclePosition.m_pVehicleInsim = pVehicleInSim;
	vehiclePosition.m_lcation = vehiclePt;
	vehiclePosition.m_vDir = vehiclePt + vDir * 600;
	vehiclePosition.m_dColWidth = dColWidth;
	vehiclePosition.m_dRowHeight = dRowHeight;
	m_vVehicleInSim.push_back(vehiclePosition);
}

bool VehicleParkingPlaceInSim::GetVehiclePosition(AirsideVehicleInSim* pVehicleInSim,CPoint2008& point)const
{	
	for (int i = 0; i < (int)m_vVehicleInSim.size(); i++)
	{
		VehicleParkingPosition vehiclePos = m_vVehicleInSim[i];
		if (vehiclePos.m_pVehicleInsim == pVehicleInSim)
		{
			point = vehiclePos.m_lcation;
			return true;
		}
	}
	return false;
}

bool VehicleParkingPlaceInSim::GetVehicleDirection(AirsideVehicleInSim* pVehicleInSim,CPoint2008& point)const
{	
	for (int i = 0; i < (int)m_vVehicleInSim.size(); i++)
	{
		const VehicleParkingPosition& vehiclePos = m_vVehicleInSim[i];
		if (vehiclePos.m_pVehicleInsim == pVehicleInSim)
		{
			point = vehiclePos.m_vDir;
			return true;
		}
	}
	return false;
}
void VehicleParkingPlaceInSim::WakeupParkingPlaceVehicle(CFlightServiceRoute* pFlightServiceRoute,const ElapsedTime &eTime)
{
	for (int i = 0; i < (int)m_vVehicleInSim.size(); i++)
	{
		VehicleParkingPosition vehiclePos = m_vVehicleInSim[i];
		AirsideVehicleInSim* pVehicleInSim = vehiclePos.m_pVehicleInsim;
		if (pVehicleInSim->GetTime() >= eTime)
		{
	//		pVehicleInSim->WirteLog(pVehicleInSim->GetPosition(), pVehicleInSim->GetSpeed(), pVehicleInSim->GetTime(),false);
			pVehicleInSim->SetMode(OnVehicleMoveOutTempParking);
			pFlightServiceRoute->GenerateNextEvent(pVehicleInSim,pVehicleInSim->GetTime() + ElapsedTime(0.01 * i));
		}
		else
		{
	//		pVehicleInSim->WirteLog(pVehicleInSim->GetPosition(),pVehicleInSim->GetSpeed(),eTime,false);
			pVehicleInSim->SetMode(OnVehicleMoveOutTempParking);
			pFlightServiceRoute->GenerateNextEvent(pVehicleInSim,eTime +ElapsedTime(0.01 * i));
		}
	}
	Clear();
}

double VehicleParkingPlaceInSim::GetMaxHeight()const
{
	double dMaxVaule = (std::numeric_limits<double>::min)();
	for (int i = 0; i < (int)m_vVehicleInSim.size(); i++)
	{
		VehicleParkingPosition vehiclePos = m_vVehicleInSim[i];
		double dRowHeight = vehiclePos.m_pVehicleInsim->GetVehicleLength() + vehiclePos.m_dRowHeight;
		if (dMaxVaule < dRowHeight)
		{
			dMaxVaule = dRowHeight;
		}
	}
	return dMaxVaule;
}
////////////////////////////////////////////////////////////////////////////////////////////////
LeftVehilceParkingPlaceInSim::LeftVehilceParkingPlaceInSim()
{

}

LeftVehilceParkingPlaceInSim::~LeftVehilceParkingPlaceInSim()
{

}


void LeftVehilceParkingPlaceInSim::InitParkingPlaceDirection(const CPoint2008& vDir)
{
	m_vDir = vDir;
	m_vDir.rotate(90.0);
	m_vDir.Normalize();
}

CPoint2008 LeftVehilceParkingPlaceInSim::AdjustParkingPlaceDirection()const
{
	CPoint2008 vDir = m_vDir;
	//rotate 90
	vDir.rotate(-90.0);
	vDir.Normalize();
	return vDir;
}

////////////////////////////////////////////////////////////////////////////////////////////////
RightVehicleParkingPlaceInSim::RightVehicleParkingPlaceInSim()
{

}

RightVehicleParkingPlaceInSim::~RightVehicleParkingPlaceInSim()
{

}

void RightVehicleParkingPlaceInSim::InitParkingPlaceDirection(const CPoint2008& vDir)
{
	m_vDir = vDir;
	m_vDir.rotate(-90.0);
	m_vDir.Normalize();
}

CPoint2008 RightVehicleParkingPlaceInSim::AdjustParkingPlaceDirection()const
{
	CPoint2008 vDir = m_vDir;
	//rotate 90
	vDir.rotate(90.0);
	vDir.Normalize();
	return vDir;
}

////////////////////////////////////////////////////////////////////////////////////////////////
VehicleParkingPlaceMgrInSim::VehicleParkingPlaceMgrInSim()
:m_bSideParkingPlace(false)
{
	m_leftVehicleParkingPlace = new LeftVehilceParkingPlaceInSim();
	m_RightVehicleParkingPlace = new RightVehicleParkingPlaceInSim();
}

VehicleParkingPlaceMgrInSim::~VehicleParkingPlaceMgrInSim()
{
	if (m_leftVehicleParkingPlace)
	{
		delete m_leftVehicleParkingPlace;
		m_leftVehicleParkingPlace = NULL;
	}

	if (m_RightVehicleParkingPlace)
	{
		delete m_RightVehicleParkingPlace;
		m_RightVehicleParkingPlace = NULL;
	}
}

void VehicleParkingPlaceMgrInSim::InitResource(const CPath2008& path,const CPoint2008& point,const CPoint2008& vDir)
{
	m_leftVehicleParkingPlace->InitResource(path,point,vDir);
	m_RightVehicleParkingPlace->InitResource(path,point,vDir);
}

void VehicleParkingPlaceMgrInSim::EnterParkingPlace(AirsideVehicleInSim* pVehicleInSim)
{
	if (m_bSideParkingPlace)
	{
		m_leftVehicleParkingPlace->HoldVehicle(pVehicleInSim);
		m_bSideParkingPlace = false;
	}
	else
	{
		m_RightVehicleParkingPlace->HoldVehicle(pVehicleInSim);
		m_bSideParkingPlace = true;
	}
}

void VehicleParkingPlaceMgrInSim::ExitParkingPlace(CFlightServiceRoute* pFlightServiceRoute,const ElapsedTime &eTime)
{
	m_leftVehicleParkingPlace->WakeupParkingPlaceVehicle(pFlightServiceRoute,eTime);
	m_RightVehicleParkingPlace->WakeupParkingPlaceVehicle(pFlightServiceRoute,eTime);
}

bool VehicleParkingPlaceMgrInSim::GetVehiclePosition(AirsideVehicleInSim* pVehicleInSim,CPoint2008& point)
{
	if (m_leftVehicleParkingPlace->GetVehiclePosition(pVehicleInSim,point))
		return true;
	
	if (m_RightVehicleParkingPlace->GetVehiclePosition(pVehicleInSim,point))
		return true;
	
	return false;
}

bool VehicleParkingPlaceMgrInSim::GetVehicleDirection(AirsideVehicleInSim* pVehicleInSim,CPoint2008& point)
{
	if (m_leftVehicleParkingPlace->GetVehicleDirection(pVehicleInSim,point))
		return true;

	if (m_RightVehicleParkingPlace->GetVehicleDirection(pVehicleInSim,point))
		return true;

	return false;
}

void VehicleParkingPlaceMgrInSim::Clear()
{
	m_leftVehicleParkingPlace->Clear();
	m_RightVehicleParkingPlace->Clear();
}