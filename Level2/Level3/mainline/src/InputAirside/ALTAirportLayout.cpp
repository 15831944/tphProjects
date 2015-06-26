#include "StdAfx.h"
#include ".\altairportlayout.h"
#include "../Database/ADODatabase.h"
#include "../Common/path.h"

#include "../Common/LatLongConvert.h"
#include "../InputAirside/AirsideImportExportManager.h"
#include "GroundRoute.h"
#include "GroundRoutes.h"
#include "VehicleSpecifications.h"
#include "IntersectionNode.h"
#include "../Common/ARCUnit.h"
#include "../InputAirside/Heliport.h"
#include "./IntersectionNodesInAirport.h"
#include "InputAirside.h"
#include "../InputAirside/CircleStretch.h"
#include "../Common/CARCUnit.h"
#include "..\Database\DBElementCollection_Impl.h"
#include "Runway.h"
#include "Taxiway.h"
#include "ReportingArea.h"
#include "AirSector.h"
#include "MeetingPoint.h"
#include "StartPosition.h"




void ALTAirportLayout::LoadData()
{
	ClearData();

	LoadRunway();
	LoadTaxiway();
	LoadHeliport();
	LoadStand();
	LoadDeicePad();
	LoadStretch();
	LoadRoadIntersection();
	LoadPoolParking();
	LoadTrafficLight();
	LoadTollGate();
	LoadStopSign();
	LoadYieldSign();
	LoadCircleStretch();
	LoadStartPosition();
	LoadMeetingPoint();
	m_nodelist.ReadData(getID());
	m_vFilletTaxiways.ReadData(getID(),m_nodelist);

}

void ALTAirportLayout::LoadTaxiway()
{
	CString strSQL = _T("");
	strSQL.Format("SELECT ID, APTID, NAME_L1, NAME_L2, NAME_L3, NAME_L4, LOCKED, PATHID, POINTCOUNT, PATH, WIDTH,SHOW, \
				  MARKING,MARKINGPOS, TSTAMP FROM TAXIWAY_VIEW WHERE APTID = %d",m_nAirportID);

	try
	{
		long nRecordCount;
		CADORecordset adoRs;
		CADODatabase::ExecuteSQLStatement(strSQL,nRecordCount,adoRs);

		while (!adoRs.IsEOF())
		{
			int nID = -1;
			adoRs.GetFieldValue(_T("ID"),nID);
			
			Taxiway *pTaxiway = new Taxiway;
			pTaxiway->setID(nID);
			pTaxiway->ReadRecord(adoRs);
			adoRs.MoveNextData();
			m_vTaxiway.push_back(pTaxiway);
		}
	}
	catch (CADOException& e)
	{
		throw e;
	}
}

void ALTAirportLayout::LoadRunway()
{

	CString strSQL = _T("");
	strSQL.Format("SELECT ID,APTID, NAME_L1, NAME_L2, NAME_L3, NAME_L4, LOCKED, PATHID, PATH, POINTCOUNT, WIDTH, SHOW1,SHOW2,\
				  MARKING1, MARKING2, TSTAMP,MARK1LTHRESHOLD,MARK1TTHRESHOLD,MARK2LTHRESHOLD,MARK2TTHRESHOLD FROM RUNWAY_VIEW WHERE APTID = %d",getID());

	try
	{
		long nRecordCount;
		CADORecordset adoRs;
		CADODatabase::ExecuteSQLStatement(strSQL,nRecordCount,adoRs);

		while (!adoRs.IsEOF())
		{
			int nID = -1;
			adoRs.GetFieldValue(_T("ID"),nID);

			Runway *pRunway = new Runway;
			pRunway->setID(nID);

			pRunway->ReadRecord(adoRs);
			adoRs.MoveNextData();

			m_vRunway.push_back(pRunway);
		}
	}
	catch (CADOException& e)
	{
		throw e;
	}
}

void ALTAirportLayout::ClearData()
{	
	m_vRunway.clear();
	m_vTaxiway.clear();
}

Runway * ALTAirportLayout::GetRunwayByID( int nRunwayID )
{
	Runway *pRetRunway  = NULL;

	std::vector<Runway::RefPtr >::iterator iterRunway = m_vRunway.begin();
	for (;iterRunway != m_vRunway.end(); ++ iterRunway)
	{
		if((*iterRunway).get()->getID() == nRunwayID)
		{
			pRetRunway = (*iterRunway).get();
			break;
		}
	}

	return pRetRunway;
}

Taxiway* ALTAirportLayout::GetTaxiwayByID( int nTaxiwayID )
{
	Taxiway *pRetTaxiway  = NULL;

	std::vector<Taxiway::RefPtr >::iterator iterTaxiway = m_vTaxiway.begin();
	for (;iterTaxiway != m_vTaxiway.end(); ++ iterTaxiway)
	{
		if((*iterTaxiway).get()->getID() == nTaxiwayID)
		{
			pRetTaxiway = (*iterTaxiway).get();
			break;
		}
	}

	return pRetTaxiway;
}

ALTObject * ALTAirportLayout::GetObjectByID( int nObjectID )
{
	ALTObject *pObject = NULL;

	pObject = GetRunwayByID(nObjectID);
	if(pObject != NULL)
		return pObject;

	pObject = GetTaxiwayByID(nObjectID);
	if(pObject != NULL)
		return pObject;


	return pObject;
}



//void ALTAirport::GetStartPositionIDs(int nAirportID, std::vector<int>& vStartPositionID)
//{
//	CString strSQL = _T("");
//	strSQL.Format("SELECT ID FROM ALTOBJECT WHERE APTID = %d AND TYPE = %d",
//		nAirportID, ALT_STARTPOSITION);
//
//	try
//	{
//		long nRecordCount;
//		CADORecordset adoRs;
//		CADODatabase::ExecuteSQLStatement(strSQL,nRecordCount,adoRs);
//
//		while (!adoRs.IsEOF())
//		{
//			int nID = -1;
//			adoRs.GetFieldValue(_T("ID"),nID);
//			vStartPositionID.push_back(nID);
//
//			adoRs.MoveNextData();
//		}
//	}
//	catch (CADOException& e)
//	{
//		e.ErrorMessage();
//	}
//}
//
//void ALTAirport::GetStartPositionList( int nAirportID, ALTObjectList& vObjectList )
//{
//	std::vector<int> vStartPosIDs;
//	GetStartPositionIDs(nAirportID,vStartPosIDs);
//	size_t nCount = vStartPosIDs.size();
//	for(size_t i =0 ;i < nCount; ++i)
//	{
//		CStartPosition* pStartPosition = new CStartPosition;
//		pStartPosition->ReadObject(vStartPosIDs.at(i));
//		vObjectList.push_back(pStartPosition);
//	}
//}
//
// void ALTAirport::GetAirsideResourceObject(int nAirportID, ALTObjectList& vObjectList,Airsdie_Resource_ID _ResourceTy) 
//{
//	switch(_ResourceTy)
//	{
//	case AIRSIDE_DEICEPAD:
//		GetDeicePadList(nAirportID,vObjectList) ;
//		break ;
//	case AIRSIDE_REPORTING_AREA:
//		GetReportingAreaList(nAirportID,vObjectList) ;
//		break ;
//	case AIRSIDE_RUNWAY:
//		GetRunwayList(nAirportID,vObjectList) ;
//		break ;
//	case AIRSIDE_STAND:
//		GetStandList(nAirportID,vObjectList) ;
//		break ;
//	case AIRSIDE_TAXIWAY:
//		GetTaxiwayList(nAirportID,vObjectList) ;
//		break ;
//	default :
//		break ;
//	}
//}
//
//void ALTAirport::GetMeetingPointIDs(int nAirportID, std::vector<int>& vMeetingPointID)
//{
//	CString strSQL = _T("");
//	strSQL.Format("SELECT ID FROM ALTOBJECT WHERE APTID = %d AND TYPE = %d",
//		nAirportID, ALT_MEETINGPOINT);
//
//	try
//	{
//		long nRecordCount;
//		CADORecordset adoRs;
//		CADODatabase::ExecuteSQLStatement(strSQL,nRecordCount,adoRs);
//
//		while (!adoRs.IsEOF())
//		{
//			int nID = -1;
//			adoRs.GetFieldValue(_T("ID"),nID);
//			vMeetingPointID.push_back(nID);
//
//			adoRs.MoveNextData();
//		}
//	}
//	catch (CADOException& e)
//	{
//		e.ErrorMessage();
//	}
//}
//
//void ALTAirport::GetMeetingPointList( int nAirportID, ALTObjectList& vObjectList )
//{
//	std::vector<int> vMeetingPoints;
//	GetMeetingPointIDs(nAirportID,vMeetingPoints);
//	int nCount = (int)vMeetingPoints.size();
//	for(int i =0 ;i < nCount; ++i)
//	{
//		CMeetingPoint * pMeetingPoint = new CMeetingPoint;
//		pMeetingPoint->ReadObject(vMeetingPoints.at(i));
//		vObjectList.push_back(pMeetingPoint);
//	}
//}
//
//const GUID& ALTAirport::getTypeGUID()
//{
//	// {1941EC43-A1AB-49d3-A7FA-D2819ECA25FA}
//	static const GUID name = 
//	{ 0x1941ec43, 0xa1ab, 0x49d3, { 0xa7, 0xfa, 0xd2, 0x81, 0x9e, 0xca, 0x25, 0xfa } };
//	
//	return name;
//}
//
//const CGuid& ALTAirport::getGuid() const
//{
//	return m_guid;
//}
//
//

void ALTAirportLayout::LoadHeliport()
{	
	std::vector<int> vObjectID;
	ALTAirport::GetHeliportsIDs(m_nAirportID, vObjectID);
	for (size_t i=0; i<vObjectID.size();i++)
	{
		Heliport* pObject = new Heliport();
		pObject->ReadObject(vObjectID.at(i));
		m_vHeliport.push_back(pObject);
	}
}
void ALTAirportLayout::LoadStand()
{
	std::vector<int> vObjectID;
	ALTAirport::GetStandsIDs(m_nAirportID, vObjectID);
	for (size_t i=0; i<vObjectID.size();i++)
	{
		Stand* pObject = new Stand();
		pObject->ReadObject(vObjectID.at(i));
		m_vStand.push_back(pObject);
	}
}
void ALTAirportLayout::LoadDeicePad()
{
	std::vector<int> vObjectID;
	ALTAirport::GetDeicePadsIDs(m_nAirportID, vObjectID);
	for (size_t i=0; i<vObjectID.size();i++)
	{
		DeicePad* pObject = new DeicePad();
		pObject->ReadObject(vObjectID.at(i));
		m_vDeicePad.push_back(pObject);
	}
}
void ALTAirportLayout::LoadStretch()
{
	std::vector<int> vObjectID;
	ALTAirport::GetStretchsIDs(m_nAirportID, vObjectID);
	for (size_t i=0; i<vObjectID.size();i++)
	{
		Stretch* pObject = new Stretch();
		pObject->ReadObject(vObjectID.at(i));
		m_vStretch.push_back(pObject);
	}
}
void ALTAirportLayout::LoadRoadIntersection()
{
	std::vector<int> vObjectID;
	ALTAirport::GetRoadIntersectionsIDs(m_nAirportID, vObjectID);
	for (size_t i=0; i<vObjectID.size();i++)
	{
		Intersections* pObject = new Intersections();
		pObject->ReadObject(vObjectID.at(i));
		m_vRoadIntersection.push_back(pObject);
	}
}
void ALTAirportLayout::LoadPoolParking()
{
	std::vector<int> vObjectID;
	ALTAirport::GetVehiclePoolParkingIDs(m_nAirportID, vObjectID);
	for (size_t i=0; i<vObjectID.size();i++)
	{
		VehiclePoolParking* pObject = new VehiclePoolParking();
		pObject->ReadObject(vObjectID.at(i));
		m_vPoolParking.push_back(pObject);
	}
}
void ALTAirportLayout::LoadTrafficLight()
{
	std::vector<int> vObjectID;
	ALTAirport::GetTrafficLightsIDs(m_nAirportID, vObjectID);
	for (size_t i=0; i<vObjectID.size();i++)
	{
		TrafficLight* pObject = new TrafficLight();
		pObject->ReadObject(vObjectID.at(i));
		m_vTrafficLight.push_back(pObject);
	}
}
void ALTAirportLayout::LoadTollGate()
{
	std::vector<int> vObjectID;
	ALTAirport::GetTollGatesIDs(m_nAirportID, vObjectID);
	for (size_t i=0; i<vObjectID.size();i++)
	{
		TollGate* pObject = new TollGate();
		pObject->ReadObject(vObjectID.at(i));
		m_vTollGate.push_back(pObject);
	}
}
void ALTAirportLayout::LoadStopSign()
{
	std::vector<int> vObjectID;
	ALTAirport::GetStopSignsIDs(m_nAirportID, vObjectID);
	for (size_t i=0; i<vObjectID.size();i++)
	{
		StopSign* pObject = new StopSign();
		pObject->ReadObject(vObjectID.at(i));
		m_vStopSign.push_back(pObject);
	}
}
void ALTAirportLayout::LoadYieldSign()
{
	std::vector<int> vObjectID;
	ALTAirport::GetYieldSignsIDs(m_nAirportID, vObjectID);
	for (size_t i=0; i<vObjectID.size();i++)
	{
		YieldSign* pObject = new YieldSign();
		pObject->ReadObject(vObjectID.at(i));
		m_vYieldSign.push_back(pObject);
	}
}
void ALTAirportLayout::LoadCircleStretch()
{
	std::vector<int> vObjectID;
	ALTAirport::GetCircleStretchsIDs(m_nAirportID, vObjectID);
	for (size_t i=0; i<vObjectID.size();i++)
	{
		CircleStretch* pObject = new CircleStretch();
		pObject->ReadObject(vObjectID.at(i));
		m_vCircleStretch.push_back(pObject);
	}
}
void ALTAirportLayout::LoadStartPosition()
{
	std::vector<int> vObjectID;
	ALTAirport::GetStartPositionIDs(m_nAirportID, vObjectID);
	for (size_t i=0; i<vObjectID.size();i++)
	{
		CStartPosition* pObject = new CStartPosition();
		pObject->ReadObject(vObjectID.at(i));
		m_vStartPosition.push_back(pObject);
	}
}
void ALTAirportLayout::LoadMeetingPoint()
{
	std::vector<int> vObjectID;
	ALTAirport::GetMeetingPointIDs(m_nAirportID, vObjectID);
	for (size_t i=0; i<vObjectID.size();i++)
	{
		CMeetingPoint* pObject = new CMeetingPoint();
		pObject->ReadObject(vObjectID.at(i));
		m_vMeetingPoint.push_back(pObject);
	}
}
ALTObject * ALTAirportLayout::GetObject( const CGuid& guid, const GUID& class_guid )
{
	if (class_guid == Runway::getTypeGUID())
	{
		return TemplateGetObject<Runway>(guid,m_vRunway);
	}
	else if(class_guid == Heliport::getTypeGUID())
	{
		return TemplateGetObject<Heliport>(guid,m_vHeliport);
	}
	else if(class_guid == Taxiway::getTypeGUID())
	{
		return TemplateGetObject<Taxiway >(guid,m_vTaxiway);
	}
	else if(class_guid == Stand::getTypeGUID())
	{
		return TemplateGetObject<Stand >(guid,m_vStand);
	}
	else if(class_guid == DeicePad::getTypeGUID())
	{
		return TemplateGetObject<DeicePad >(guid,m_vDeicePad);
	}
	else if(class_guid == Stretch::getTypeGUID())
	{
		return TemplateGetObject<Stretch >(guid,m_vStretch);
	}
	else if(class_guid == Intersections::getTypeGUID())
	{
		return TemplateGetObject<Intersections >(guid,m_vRoadIntersection);
	}
	else if(class_guid == VehiclePoolParking::getTypeGUID())
	{
		return TemplateGetObject<VehiclePoolParking >(guid,m_vPoolParking);
	}
	else if(class_guid == TrafficLight::getTypeGUID())
	{
		return TemplateGetObject<TrafficLight >(guid,m_vTrafficLight);
	}
	else if(class_guid == TollGate::getTypeGUID())
	{
		return TemplateGetObject<TollGate >(guid,m_vTollGate);
	}
	else if(class_guid == StopSign::getTypeGUID())
	{
		return TemplateGetObject<StopSign >(guid,m_vStopSign);
	}
	else if(class_guid == YieldSign::getTypeGUID())
	{
		return TemplateGetObject<YieldSign >(guid,m_vYieldSign);
	}
	else if(class_guid == CircleStretch::getTypeGUID())
	{
		return TemplateGetObject<CircleStretch >(guid,m_vCircleStretch);
	}
	else if(class_guid == CStartPosition::getTypeGUID())
	{
		return TemplateGetObject<CStartPosition >(guid,m_vStartPosition);
	}
	else if(class_guid == CMeetingPoint::getTypeGUID())
	{
		return TemplateGetObject<CMeetingPoint >(guid,m_vMeetingPoint);
	}


	return NULL;

}

bool ALTAirportLayout::GetObjectList( const GUID& class_guid,std::vector<void*>& objectList )
{
	if (class_guid == Runway::getTypeGUID())
	{
		TemplateGetObjectList<Runway>(objectList,m_vRunway);
	}
	else if(class_guid == Heliport::getTypeGUID())
	{
		TemplateGetObjectList<Heliport>(objectList,m_vHeliport);
	}
	else if(class_guid == Taxiway::getTypeGUID())
	{
		TemplateGetObjectList<Taxiway >(objectList,m_vTaxiway);
	}
	else if(class_guid == Stand::getTypeGUID())
	{
		TemplateGetObjectList<Stand >(objectList,m_vStand);
	}
	else if(class_guid == DeicePad::getTypeGUID())
	{
		TemplateGetObjectList<DeicePad >(objectList,m_vDeicePad);
	}
	else if(class_guid == Stretch::getTypeGUID())
	{
		TemplateGetObjectList<Stretch >(objectList,m_vStretch);
	}
	else if(class_guid == Intersections::getTypeGUID())
	{
		TemplateGetObjectList<Intersections >(objectList,m_vRoadIntersection);
	}
	else if(class_guid == VehiclePoolParking::getTypeGUID())
	{
		TemplateGetObjectList<VehiclePoolParking >(objectList,m_vPoolParking);
	}
	else if(class_guid == TrafficLight::getTypeGUID())
	{
		TemplateGetObjectList<TrafficLight >(objectList,m_vTrafficLight);
	}
	else if(class_guid == TollGate::getTypeGUID())
	{
		TemplateGetObjectList<TollGate >(objectList,m_vTollGate);
	}
	else if(class_guid == StopSign::getTypeGUID())
	{
		TemplateGetObjectList<StopSign >(objectList,m_vStopSign);
	}
	else if(class_guid == YieldSign::getTypeGUID())
	{
		TemplateGetObjectList<YieldSign >(objectList,m_vYieldSign);
	}
	else if(class_guid == CircleStretch::getTypeGUID())
	{
		TemplateGetObjectList<CircleStretch >(objectList,m_vCircleStretch);
	}
	else if(class_guid == CStartPosition::getTypeGUID())
	{
		TemplateGetObjectList<CStartPosition >(objectList,m_vStartPosition);
	}
	else if(class_guid == CMeetingPoint::getTypeGUID())
	{
		TemplateGetObjectList<CMeetingPoint >(objectList,m_vMeetingPoint);
	}
	else
		return false;


	return true;
}


template <class ALTObjectListType>
void _InsertALTObjects(ALTObjectList& objList, ALTObjectListType vSrc)
{
	size_t nCount = vSrc.size();
	for(size_t i=0;i<nCount;i++)
	{
		objList.push_back((ALTObject*)vSrc.at(i).get());
	}
}

void ALTAirportLayout::GetEntireALTObjectList( ALTObjectList& objList )
{
// 	objList.resize(m_vRunway.size() + m_vTaxiway.size() + m_vStand.size() + m_vDeicePad.size()
// 		+ m_vStretch.size() + m_vRoadIntersection.size() + m_vPoolParking.size() + m_vTrafficLight.size()
// 		+ m_vTollGate.size() + m_vStopSign.size() + m_vYieldSign.size() + m_vHeliport.size() 
// 		+ m_vCircleStretch.size()
// 		);

	_InsertALTObjects(objList, m_vRunway);
	_InsertALTObjects(objList, m_vTaxiway);
	_InsertALTObjects(objList, m_vStand);
	_InsertALTObjects(objList, m_vDeicePad);
	_InsertALTObjects(objList, m_vStretch);
	_InsertALTObjects(objList, m_vRoadIntersection);
	_InsertALTObjects(objList, m_vPoolParking);
	_InsertALTObjects(objList, m_vTrafficLight);
	_InsertALTObjects(objList, m_vTollGate);
	_InsertALTObjects(objList, m_vStopSign);
	_InsertALTObjects(objList, m_vYieldSign);
	_InsertALTObjects(objList, m_vHeliport);
	_InsertALTObjects(objList, m_vCircleStretch);
}

















