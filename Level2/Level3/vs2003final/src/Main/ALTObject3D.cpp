#include "StdAfx.h"
#include ".\altobject3d.h"
#include "..\InputAirside\ALTObject.h"
#include "..\InputAirside\ALTObjectDisplayProp.h"

#include "Runway3D.h"
#include "Taxiway3D.h"
#include "Gate3D.h"
#include "./DeicePad3D.h"
#include "./AirRoute3D.h"
#include "./Hold3D.h"
#include "./Sector3D.h"
#include "./WayPoint3D.h"
#include "./Surface3D.h"
#include "./Structure3D.h"
#include "./WallShape3D.h"
#include "./Contour3D.h"
#include "./Stretch3D.h"
#include "./RoadIntersection3D.h"
#include "VehiclePoolParking3D.h"
#include "./TrafficLight3D.h"
#include "TollGate3D.h"
#include "StopSign3D.h"
#include "YieldSign3D.h"
#include "./Heliport3D.h"
#include "./CircleStretch3D.h"
#include "./ReportingArea3D.h"
#include "./StartPosition3D.h"
#include "./MeetingPoint3D.h"

ALTObject3D::ALTObject3D( int _Id ) : m_nID(_Id)
{
	m_bInEdit = false;
}

ALTObject3D::~ALTObject3D(void)
{
	
}

Selectable::SelectType ALTObject3D::GetSelectType() const
{
	return ALT_OBJECT;
}

void ALTObject3D::Update()
{
	try
	{
		if(m_pObj.get())
			m_pObj->ReadObject(m_nID);
		//Read display prop
		if(ALTObjectDisplayProp* pDispProp = GetDisplayProp() )
			pDispProp->ReadData(m_nID);
		//	
	}
	catch (CADOException& e)
	{
		CString strErr = e.ErrorMessage(); 
	}	
	SyncDateType::InValidate();
}

void ALTObject3D::DoSync()
{	
	SyncDateType::DoSync();
}

void ALTObject3D::FlushChange()
{
	try {
		m_pObj->UpdateObject(m_nID);
	}
	catch (CADOException& e){
		CString strErr = e.ErrorMessage();

	}

}

ALTObject3D * ALTObject3D::NewALTObject3D(ALTObject * pObj){
	ALTObject3D * reslt = NULL;
	switch(pObj->GetType())
	{
	case ALT_RUNWAY : 
		reslt = new CRunway3D(-1);
		reslt->m_pObj = pObj;
		break;
	case ALT_HELIPORT:
		reslt = new CHeliport3D(-1);
		reslt->m_pObj = pObj;
		break;
	case ALT_TAXIWAY :
		reslt = new CTaxiway3D(-1);
		reslt->m_pObj  = pObj;
		break;
	case ALT_STAND :
		reslt  = new CStand3D(-1);
		reslt->m_pObj=  pObj;
		break;
	case ALT_DEICEBAY :
		reslt = new CDeicePad3D(-1);
		reslt->m_pObj = pObj;
		break;
	case ALT_WAYPOINT : 
		reslt = new CWayPoint3D(-1);
		reslt->m_pObj = pObj;
		break;
	case ALT_SECTOR :
		reslt = new CAirSector3D(-1);
		reslt->m_pObj = pObj;
		break;
	case ALT_HOLD :
		reslt = new CHold3D(-1);
		reslt->m_pObj = pObj;
		break;
	case ALT_AIRROUTE :
		reslt = new CAirRoute3D(-1);
		reslt->m_pObj = pObj;
		break;
	case ALT_SURFACE:
		reslt = new CSurface3D(-1);
		reslt->m_pObj = pObj;
		break;
	case ALT_STRUCTURE :
		reslt = new CStructure3D(-1);
		reslt->m_pObj = pObj;
		break;
	case ALT_WALLSHAPE :
		reslt = new CWallShape3D(-1);
		reslt->m_pObj = pObj;
		break;
	case ALT_CONTOUR :
		reslt = new CContour3D(-1);
		reslt->m_pObj = pObj;
		break;
	case ALT_STRETCH :
		reslt = new CStretch3D(-1);
		reslt->m_pObj = pObj;
		break;
	case ALT_INTERSECTIONS :
		reslt = new CRoadIntersection3D(-1);
		reslt->m_pObj = pObj;
		break;
	case ALT_VEHICLEPOOLPARKING:
		reslt = new CVehiclePoolParking3D(-1);
		reslt->m_pObj = pObj;
		break;
	case ALT_TRAFFICLIGHT:
		reslt = new CTrafficLight3D(-1);
		reslt->m_pObj = pObj;
		break;
	case ALT_TOLLGATE:
		reslt = new CTollGate3D(-1);
		reslt->m_pObj = pObj;
		break;
	case ALT_STOPSIGN:
		reslt = new CStopSign3D(-1);
		reslt->m_pObj = pObj;
		break;
	case ALT_YIELDSIGN:
		reslt = new CYieldSign3D(-1);
		reslt->m_pObj = pObj;
		break;
	case ALT_CIRCLESTRETCH:
		reslt = new CCircleStretch3D(-1);
		reslt->m_pObj = pObj;
		break;
	case ALT_REPORTINGAREA:
		reslt = new CReportingArea3D(-1);
		reslt->m_pObj = pObj;
		break;
	case ALT_STARTPOSITION:
		reslt = new CStartPosition3D(-1);
		reslt->m_pObj = pObj;
		break;
	case ALT_MEETINGPOINT:
		reslt = new CMeetingPoint3D(-1);
		reslt->m_pObj = pObj;
		break;
	default :
		ASSERT(false);
	}
	
	return reslt;
}

ARCPoint3 ALTObject3D::GetLocation() const
{
	ASSERT(FALSE);
	return ARCPoint3(0,0,0);
}

ARCPoint3 ALTObject3D::GetSubLocation(int index) const
{
	ASSERT(FALSE);
	return ARCPoint3(0,0,0);
}

ALTObject* ALTObject3D::GetObject()const
{
	return (ALTObject*)m_pObj.get();
}

CString ALTObject3D::GetDisplayName()const
{
	return GetObject()->GetObjNameString();
}

void ALTObject3D::OnMove( DistanceUnit dx, DistanceUnit dy,DistanceUnit dz /*= 0*/ )
{
	Selectable::OnMove(dx,dy,dz);
}

void ALTObject3D::AfterMove()
{
	Selectable::AfterMove();

	FlushChange();
}

ALTObjectDisplayProp * ALTObject3D::GetDisplayProp()
{
	return m_pObj.get()?m_pObj->getDisplayProp():NULL;//.get();
}

const ALTObjectDisplayProp * ALTObject3D::GetDisplayProp() const
{	
	ALTObject3D* pObj3D = const_cast<ALTObject3D*>(this);
	return pObj3D->GetDisplayProp();
}


ALTObject3D * ALTObject3DList::GetALTObject3D( int objID )
{
	for(int i=0; i< (int)size();i++ ){
		ALTObject3D * pObj3D = at(i).get();
		ASSERT(pObj3D->GetObject());
		if(pObj3D->GetObject()->getID() == objID)
		{
			return pObj3D;
		}
	}
	return NULL;
}

ALTObjectList ALTObject3DList::GetObjectList() const
{
	ALTObjectList vReslt;
	for(const_iterator itr = begin();itr!=end();itr++)
		vReslt.push_back( (*itr)->GetObject() );
	return vReslt;
}

bool ALTObject3DList::HasType( ALTOBJECT_TYPE objType ) const
{
	for(size_t i=0;i<size();i++)
	{
		const ALTObject3D * pObj3D =  at(i).get();
		if( pObj3D->GetObjectType() == objType )
			return true;
	}
	return false;
}

