#include "StdAfx.h"
 
#include "AirRoute.h"
#include "AirsideImportExportManager.h"
#include "ALTObjectDisplayProp.h"
#include "../Common/ARCVector.h"
#include "../common/UnitsManager.h"
#include "../Common/line2008.h"
#include "Runway.h"
#include "InputAirside.h"
#include "AirRouteIntersection.h"
#include "../Common/latlong.h"


const char *  CAirRoute::RouteTypeStr[] = 
{
	"STAR",
	"SID",
	"ENROUTE",
	"MISSEDAPCH",	
};



//////////////////////////////////////////////////////////////////////////
//class AirRoute
CAirRoute::CAirRoute(int nAirRouteID)
{
	m_strName = _T("");
	m_nObjID = nAirRouteID;
//	m_nID = m_nObjID;
	m_enumType = STAR;
	m_nRunwayMarkIndex = 0;
	//m_pRunway = NULL;

}

CAirRoute::CAirRoute(const CAirRoute& otherAirRoute)
{
	*this = otherAirRoute;
	m_vARWaypoint.clear();

	std::vector<ARWaypoint *>::const_iterator iter = otherAirRoute.m_vARWaypoint.begin();
	std::vector<ARWaypoint *>::const_iterator iterEnd = otherAirRoute.m_vARWaypoint.end();
	for (; iter != iterEnd; ++iter)
	{
		m_vARWaypoint.push_back(new ARWaypoint(**iter));
	}
	
	m_vConnectRunway.assign(otherAirRoute.m_vConnectRunway.begin(),otherAirRoute.m_vConnectRunway.end());

}

CAirRoute::~CAirRoute()
{
	ClearData();
}
void CAirRoute::ReadDataByARWaypointID(int nArwpID)
{
	CString strSQL = _T("");
	strSQL.Format(_T("SELECT AIRROUTEID\
		FROM ARWAYPOINTPROP\
		WHERE (ID = %d)"),nArwpID);

	long nRecordCount = -1;
	CADORecordset adoRecordset;
	CADODatabase::ExecuteSQLStatement(strSQL,nRecordCount,adoRecordset);

	if (!adoRecordset.IsEOF())
	{
		int nAirRouteID = -1;
		adoRecordset.GetFieldValue(_T("AIRROUTEID"),nAirRouteID);
		if (nAirRouteID == -1)
		{
			return;
		}
		ReadData(nAirRouteID);
	}
}

void CAirRoute::ReadData(int nAirRouteID)
{
	m_nObjID = nAirRouteID;
	ClearData();

	CString strSQL = _T("");
	strSQL.Format(_T("SELECT *\
					 FROM AIRROUTEPROP\
					 WHERE (ID = %d)"),nAirRouteID);
	long nRecordCount = -1;
	CADORecordset adoRecordset;
	CADODatabase::ExecuteSQLStatement(strSQL,nRecordCount,adoRecordset);

	if (!adoRecordset.IsEOF())
	{

//		m_nID = nAirRouteID;
		//name
		CString strName = _T("");
		adoRecordset.GetFieldValue(_T("NAME"),strName);
		setName(strName);

		//type
		int nAirRouteType = 0;
		adoRecordset.GetFieldValue(_T("TYPE"),nAirRouteType);

		setRouteType((CAirRoute::RouteType)nAirRouteType);


		int nAptID = -1;
		adoRecordset.GetFieldValue(_T("APTID"),nAptID);
		setAptID(nAptID);

		//runway
		int nObjID = -1;
		adoRecordset.GetFieldValue(_T("RUNWAYID"),nObjID);

		int nMarkIndex = 0;
		adoRecordset.GetFieldValue(_T("MARKINDEX"),nMarkIndex);

		//setRunwayID(nObjID,nMarkIndex);
		ReadLogicRunways();
		if (nObjID >= 0 && m_vConnectRunway.empty())
		{
			m_vConnectRunway.push_back(LogicRunway_Pair(nObjID,nMarkIndex));
		}

		ReadARWaypoint();
		
		ReadAirportInfo();
		CalWaypointExtentPoint();
		
	}
}

void CAirRoute::ReadARWaypoint()
{
	CString strSQL  = _T("");
	strSQL.Format(_T("SELECT ID, WAYPOINTID, NAME_L1, NAME_L2, NAME_L3, NAME_L4, LOCKED, MINSPEED, MAXSPEED, MINHEIGHT, MAXHEIGHT,DEPARTFOR,HEADINGDEGREE,HEADINGTYPE,VISDISTANCE, SEQUENCENUM\
		FROM ARWAYPOINT_VIEW\
		WHERE (AIRROUTEID = %d) order by SEQUENCENUM"),m_nObjID);

	long nRecordCount = -1;
	CADORecordset adoRecordset;
	CADODatabase::ExecuteSQLStatement(strSQL,nRecordCount,adoRecordset);

	while (!adoRecordset.IsEOF())
	{
		int nARwaypointID = -1;
		adoRecordset.GetFieldValue(_T("ID"),nARwaypointID);
		ARWaypoint *pARwayPoint = new ARWaypoint(nARwaypointID);


		//way point
		int nObjID = -1;
		CString strName1 = _T("");
		CString strName2 = _T("");
		CString strName3 = _T("");
		CString strName4 = _T("");

		//ALTObjectID objName;
		adoRecordset.GetFieldValue(_T("WAYPOINTID"),nObjID);

		/*adoRecordset.GetFieldValue(_T("NAME_L1"),strName1);
		objName.at(0) = strName1;

		adoRecordset.GetFieldValue(_T("NAME_L2"),strName2);
		objName.at(1) = strName2;

		adoRecordset.GetFieldValue(_T("NAME_L3"),strName3);
		objName.at(2) = strName3;	

		adoRecordset.GetFieldValue(_T("NAME_L4"),strName4);
		objName.at(3) = strName4;*/

		pARwayPoint->getWaypoint().ReadObject(nObjID);
		

		//speed
		double lMinSpeed = -1;
		double lMaxSpeed = -1;

		adoRecordset.GetFieldValue(_T("MINSPEED"),lMinSpeed);
		adoRecordset.GetFieldValue(_T("MAXSPEED"),lMaxSpeed);

		pARwayPoint->setMinSpeed(lMinSpeed);
		pARwayPoint->setMaxSpeed(lMaxSpeed);

		//height
		double lMinHeight = -1;
		double lMaxHeight = -1;
		adoRecordset.GetFieldValue(_T("MINHEIGHT"),lMinHeight);
		adoRecordset.GetFieldValue(_T("MAXHEIGHT"),lMaxHeight);
		
		pARwayPoint->setMinHeight(lMinHeight);
		pARwayPoint->setMaxHeight(lMaxHeight);

		//depart info
		int nDepartType = 0;
		long lDegree = 0;
		int nHeadingType = 0;
		double lVisDistance = 0;
		int nSequenceNum = 0;

		adoRecordset.GetFieldValue(_T("DEPARTFOR"),nDepartType);
		adoRecordset.GetFieldValue(_T("HEADINGDEGREE"),lDegree);
		adoRecordset.GetFieldValue(_T("HEADINGTYPE"),nHeadingType);
		adoRecordset.GetFieldValue(_T("VISDISTANCE"),lVisDistance);
		adoRecordset.GetFieldValue(_T("SEQUENCENUM"),nSequenceNum);

		pARwayPoint->setDepartType(ARWaypoint::DepartType(nDepartType));
		pARwayPoint->setDegrees(lDegree);
		pARwayPoint->setHeadingType(ARWaypoint::HeadingType(nHeadingType));
		pARwayPoint->setVisDistance(lVisDistance);
		pARwayPoint->SetSequenceNum(nSequenceNum);
		pARwayPoint->setExtentPoint(pARwayPoint->getWaypoint().GetLocation(m_airportInfo));

		m_vARWaypoint.push_back(pARwayPoint);


		//move to next data
		adoRecordset.MoveNextData();
	}
	InitPath();
}

void CAirRoute::ReadLogicRunways()
{
	CString strSQL  = _T("");
	strSQL.Format(_T("SELECT ID, RUNWAYID, PORTMARK FROM AIRROUTERUNWAYPORTS WHERE (AIRROUTEID = %d)"),m_nObjID);

	long nRecordCount = -1;
	CADORecordset adoRecordset;
	CADODatabase::ExecuteSQLStatement(strSQL,nRecordCount,adoRecordset);

	m_vConnectRunway.clear();
	while (!adoRecordset.IsEOF())
	{
		int nID = -1;
		adoRecordset.GetFieldValue(_T("ID"),nID);
		int nRwyID = -1;
		int nPort = -1;
		adoRecordset.GetFieldValue(_T("RUNWAYID"), nRwyID);
		adoRecordset.GetFieldValue(_T("PORTMARK"), nPort);

		if (IsRunwayExist(nRwyID))
		{
			adoRecordset.MoveNextData();
			continue;
		}
		m_vConnectRunway.push_back(LogicRunway_Pair(nRwyID,nPort));
		adoRecordset.MoveNextData();
	}
}

bool CAirRoute::IsWaypointExists(int nWaypointID)
{
	std::vector<ARWaypoint *>::const_iterator iter = m_vARWaypoint.begin();
	std::vector<ARWaypoint *>::const_iterator iterEnd = m_vARWaypoint.end();
	for (; iter != iterEnd; ++iter)
	{
		if((*iter)->getWaypoint().getID() == nWaypointID)
			return true;
	}

	return false;
}

void CAirRoute::SaveData(int nAirspaceID)
{
	CString strSQL = _T("");
	strSQL.Format(_T("INSERT INTO AIRROUTEPROP\
		(APTID, NAME, TYPE,RUNWAYID,MARKINDEX)\
		VALUES (%d,'%s',%d, %d, %d)"),
		nAirspaceID,m_strName,(int)m_enumType,-1,-1);

	m_nObjID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);


	UpdateARWaypoint();
	UpdateLogicRunways();
}

void CAirRoute::UpdateData()
{
	CString strSQL = _T("");
	strSQL.Format(_T("UPDATE AIRROUTEPROP\
					 SET NAME ='%s', TYPE =%d,RUNWAYID =%d,MARKINDEX = %d\
					 WHERE (ID = %d)"),
					 m_strName,(int)m_enumType,-1,-1,m_nObjID);
	CADODatabase::ExecuteSQLStatement(strSQL);
	
	UpdateARWaypoint();
	UpdateLogicRunways();
}

void CAirRoute::UpdateARWaypoint()
{
	std::vector<ARWaypoint *>::iterator iter = m_vARWaypoint.begin();
	std::vector<ARWaypoint *>::iterator iterEnd = m_vARWaypoint.end();

	for (int i=0; i<(int)m_vARWaypoint.size(); i++)
	{
		m_vARWaypoint[i]->SetSequenceNum(i);
	}

	for (;iter != iterEnd; ++iter)
	{
		if ((*iter)->getID() == -1)
		{
			(*iter)->SaveData(m_nObjID);
		}
		else
		{
			(*iter)->UpdateData();
		}
	}
}

void CAirRoute::UpdateLogicRunways()
{
	DeleteLogicRunways();
	for (int i  = 0; i < (int)m_vConnectRunway.size(); i++)
	{
		LogicRunway_Pair logicrunway = m_vConnectRunway.at(i);

		CString strSQL = _T("");
		strSQL.Format(_T("INSERT INTO AIRROUTERUNWAYPORTS\
						 (AIRROUTEID, RUNWAYID, PORTMARK)\
						 VALUES (%d,%d,%d)"),
						 m_nObjID, logicrunway.first, logicrunway.second);

		int nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
	}

}

void CAirRoute::DeleteData()
{
	DeleteLogicRunways();

	CString strSQL = _T("");
	strSQL.Format(_T("DELETE FROM AIRROUTEPROP	WHERE (ID = %d)"),m_nObjID);
	CADODatabase::ExecuteSQLStatement(strSQL);
	
	DeleteARwayPoint();
}
void CAirRoute::DeleteARwayPoint()
{
	CString strSQL = _T("");
	strSQL.Format(_T("DELETE FROM ARWAYPOINTPROP\
		WHERE (AIRROUTEID = %d) "),m_nObjID);
	
	CADODatabase::ExecuteSQLStatement(strSQL);
}

void CAirRoute::DeleteLogicRunways()
{
	CString strSQL = _T("");
	strSQL.Format(_T("DELETE FROM AIRROUTERUNWAYPORTS\
					 WHERE (AIRROUTEID = %d) "),m_nObjID);

	CADODatabase::ExecuteSQLStatement(strSQL);
}
//const ALTObject& CAirRoute::getRunwayObj() const
//{
//	return m_runway;
//}

void CAirRoute::SetConnectRunway(std::vector<LogicRunway_Pair>& vRunways)
{
	if (vRunways.empty())
		return;

	m_vConnectRunway.assign(vRunways.begin(),vRunways.end());
}

int CAirRoute::GetConnectRunwayCount()
{ 
	return (int)m_vConnectRunway.size();
}

LogicRunway_Pair CAirRoute::getLogicRunwayByIdx(int nIdx)
{
	return m_vConnectRunway.at(nIdx);
}

bool CAirRoute::IsConnectRunway(int nRwyID, int nRwyMark)
{
	int nRwy = GetConnectRunwayCount();
	for (int i =0; i < nRwy; i++)
	{
		LogicRunway_Pair logicrunway = m_vConnectRunway.at(i);
		if (logicrunway.first == nRwyID && logicrunway.second == nRwyMark)
			return true;
	}
	return false;
}
const CString& CAirRoute::getName() const
{
	return m_strName;
}

CAirRoute::RouteType CAirRoute::getRouteType() const
{
	return m_enumType;
}

const std::vector<ARWaypoint *>& CAirRoute::getARWaypoint() const
{
	return m_vARWaypoint;
}

std::vector<ARWaypoint *>& CAirRoute::getARWaypoint()
{
	return m_vARWaypoint;
}

ARWaypoint *CAirRoute::getARWaypoint( int nArwpID)
{
	std::vector<ARWaypoint *>::iterator iter = m_vARWaypoint.begin();
	std::vector<ARWaypoint *>::iterator iterEnd = m_vARWaypoint.end();

	for (;iter != iterEnd; ++iter)
	{
		if((*iter)->getID() == nArwpID)
			return *iter;
	}

	return NULL;
}

//int CAirRoute::getRunwayID()
//{
//	return m_runway.getID();
//}
//
//int CAirRoute::getRunwayMarkIndex()
//{
//	return m_nRunwayMarkIndex;
//}
//
//void CAirRoute::setRunwayID( int nRunwayID,int nMarkIndex )
//{
//	m_runway.setID(nRunwayID);
//	m_nRunwayMarkIndex = nMarkIndex;
//}

void CAirRoute::ExportAirRoutes(CAirsideExportFile& exportFile)
{
	//AirRouteList airouteList;
	//airouteList.ReadData(exportFile.GetAirspaceID());
	//int nAirRouteCount = airouteList.GetAirRouteCount();
	//for (int i =0; i< nAirRouteCount;++i)
	//{
	//	airouteList.GetAirRoute(i)->ExportAirRoute(exportFile);
	//}
}
void CAirRoute::ExportAirRoute(CAirsideExportFile& exportFile)
{
	//ALTObject::ExportObject(exportFile);
	//airspace id
	//exportFile.getFile().writeInt(GetType());
	//exportFile.getFile().writeInt(m_nObjID);
	//exportFile.getFile().writeInt(m_nAptID);

	//exportFile.getFile().writeField(m_strName);
	////route type
	//exportFile.getFile().writeInt(m_enumType);

	////
	//exportFile.getFile().writeInt(m_runway.getID());
	//exportFile.getFile().writeInt(m_nRunwayMarkIndex);
	//
	////arwaypoint count
	//int nARWaypointCount = static_cast<int>(m_vARWaypoint.size());
	//exportFile.getFile().writeInt(nARWaypointCount);
	//exportFile.getFile().writeLine();

	////write arwaypoint info

	//std::vector<ARWaypoint *>::size_type i = 0;
	//for (;i < m_vARWaypoint.size(); ++i)
	//{
	//	m_vARWaypoint[i]->ExportObject(exportFile);
	//}

	////export display property
	//AirRouteDisplayProp dspProp;
	//dspProp.ReadData(m_nObjID);
	//dspProp.ExportDspProp(exportFile);

}
void CAirRoute::ImportObject(CAirsideImportFile& importFile)
{
	//import
//	int nOldObjeID = -1;
//	importFile.getFile().getInteger(nOldObjeID);
//	int nOldAirspaceID = -1;
//	importFile.getFile().getInteger(nOldAirspaceID);
//	m_nAptID = importFile.getNewAirspaceID();
//
//	char chName[512];
//	importFile.getFile().getField(chName,512);
//	m_strName = chName;
//
//	int nRouteType = 0;
//	importFile.getFile().getInteger(nRouteType);
//	m_enumType = (RouteType)nRouteType;
//
//	int nOldRunwayID = -1;
//	importFile.getFile().getInteger(nOldRunwayID);
//	int nNewRunwayID = importFile.GetObjectNewID(nOldRunwayID);
//	m_runway.setID(nNewRunwayID);
//
//	importFile.getFile().getInteger(m_nRunwayMarkIndex);
//	
//
//	int nARWaypointCount = 0;
//	importFile.getFile().getInteger(nARWaypointCount);
//	importFile.getFile().getLine();
//
////	int nOldObjectID = m_nObjID;
//	//int nNewObjID =
//		SaveData(m_nAptID);
//	importFile.AddAirRouteIndexMap(nOldObjeID,m_nObjID);
//
//	
//	//read arwaypoint
//	for (int i = 0; i < nARWaypointCount;++i )
//	{
//		ARWaypoint arwaypointObj;
//		arwaypointObj.ImportObject(importFile,m_nObjID);
//	}
//
//	AirRouteDisplayProp dspProp;
//	dspProp.ImportDspProp(importFile,m_nObjID);

}

ARWaypoint * CAirRoute::GetWayPointByIdx( int idx )
{
	return m_vARWaypoint[idx];
}

int CAirRoute::GetWayPointCount() const
{
	return (int)m_vARWaypoint.size();
}

int CAirRoute::HasIntersection(CAirRoute * pAirRoute)
{
	if (m_nObjID == pAirRoute->getID())
		return -1;

	const std::vector<ARWaypoint *> vArwayPoint = pAirRoute->getARWaypoint();
	for (size_t i =0;i < vArwayPoint.size(); ++i)
	{
		for (size_t j =0; j < m_vARWaypoint.size(); ++j)
		{
			if( vArwayPoint[i]->getWaypoint().getID() == m_vARWaypoint[j]->getWaypoint().getID() )
			{
				return vArwayPoint[i]->getWaypoint().getID();
			}

		}
	}

	return -1;

}

void CAirRoute::CalWaypointExtentPoint()
{
	CPoint2008 vnorth(0,1,0);
	for(int i =0; i <int(m_vARWaypoint.size()); i++)
	{
		ARWaypoint* pWaypoint = m_vARWaypoint[i];
		if (pWaypoint->getDepartType() == ARWaypoint::Heading)
		{

			CPoint2008 pos = pWaypoint->getWaypoint().GetLocation(m_airportInfo);
			double degree = double(pWaypoint->getDegrees());
			double distance = pWaypoint->getVisDistance();

			degree += m_airportInfo.GetMagnectVariation().GetRealVariation();
			vnorth.rotate(degree);
			vnorth.length(distance);
			CPoint2008 extend = pos + vnorth;
			m_vARWaypoint[i]->setExtentPoint(extend);
		}
	}

}

void CAirRoute::ReadAirportInfo()
{
	int nPrjId = getAptID();
	std::vector<int> vAirportIDs;
	InputAirside::GetAirportList(nPrjId,vAirportIDs);
	ASSERT(vAirportIDs.size());
	int nAirportID = vAirportIDs.at(0);
	m_airportInfo.ReadAirport(nAirportID);

	//m_pRunway = (Runway*)ALTObject::ReadObjectByID(getRunwayID());
}

double CAirRoute::GetAngleCosValueWithARPCoordinateBaseOnRefPoint(const CLatitude& latitude , const CLongitude& longtitude )
{
	if (m_vARWaypoint.empty())
		return 0;

	CString strlat = m_airportInfo.getLatitude();
	CString strLong = m_airportInfo.getLongtitude();

	CPoint2008 ARPpos = m_airportInfo.GetLatLongPos(latitude,longtitude);

	ARWaypoint* pWaypoint = NULL;
	if (m_enumType == SID)
	{
		int nCount = (int)m_vARWaypoint.size();
		pWaypoint = m_vARWaypoint.at(nCount-1);
	}
	else
		pWaypoint = m_vARWaypoint.at(0);

	CPoint2008 WapointPos = pWaypoint->getWaypoint().GetLocation(m_airportInfo);

	CPoint2008 Refpont = m_airportInfo.getRefPoint();

	CPoint2008 dirPos1 = CPoint2008(WapointPos - Refpont);
	CPoint2008 dirPos2 = CPoint2008(ARPpos - Refpont);

	CPoint2008 Pos0(0,0,0);
	if (dirPos1 == Pos0 || dirPos2 == Pos0)
		return 1;

	return dirPos2.GetCosOfTwoVector(dirPos1);

}

void CAirRoute::InitPath()
{
	int nCount = GetWayPointCount();
	
	m_path.init(nCount);
	for ( int i = 0; i < nCount; i++)
	{
		m_path[i] = GetWayPointByIdx(i)->getWaypoint().GetLocation(m_airportInfo) ;
	}
}

ARWaypoint* CAirRoute::getFrontWaypoint(CPoint2008 pointPos)
{
	if ( !m_path.within(pointPos))
		return NULL;

	//CPoint2008 closePoint = m_path.getClosestPoint(pointPos);

	//if ((int(closePoint.getX()/10) == int(pointPos.getX()/10)) && (int(closePoint.getY()/10) == int(pointPos.getY()/10)))
	//	return NULL;

	for (int i = 0; i < GetWayPointCount() -1; i++)
	{
		CPoint2008 pos1 = GetWayPointByIdx(i)->getWaypoint().GetLocation(m_airportInfo);
		CPoint2008 pos2 = GetWayPointByIdx(i+1)->getWaypoint().GetLocation(m_airportInfo);
		
		if ((int(pos1.getX()/10) == int(pointPos.getX()/10)) && (int(pos1.getY()/10) == int(pointPos.getY()/10)))
			return NULL;

		if ((int(pos2.getX()/10) == int(pointPos.getX()/10)) && (int(pos2.getY()/10) == int(pointPos.getY()/10)))
			return NULL;

		CLine2008 line(pos1, pos2);

		if (line.comprisePt(pointPos))
			return GetWayPointByIdx(i);
	}
	//for (int i = 0; i < GetWayPointCount() -1; i++)
	//{
	//	if (GetWayPointByIdx(i)->getWaypoint().GetLocation(m_airportInfo) == closePoint)
	//	{
	//		if (i < GetWayPointCount()-1)
	//		{
	//			if (i >0)
	//			{
	//				CPoint2008 anotherPoint = GetWayPointByIdx(i+1)->getWaypoint().GetLocation(m_airportInfo);
	//				CPath2008 pathSeg;
	//				pathSeg.init(2);
	//				pathSeg[0] = closePoint;
	//				pathSeg[1] = anotherPoint;
	//				if (pathSeg.within(pointPos))
	//					return GetWayPointByIdx(i);
	//				else
	//					return GetWayPointByIdx(i-1);
	//			}
	//			else
	//				return GetWayPointByIdx(i);
	//		}
	//		else
	//			return GetWayPointByIdx(i-1);

	//	}
	//}

	return NULL;
}

ARWaypoint* CAirRoute::getAfterWaypoint(ARWaypoint* pForeWaypoint)
{
	if ( !pForeWaypoint)
		return NULL;

	for (int i = 0; i < GetWayPointCount()-1; i++)
	{
		if ( GetWayPointByIdx(i) == pForeWaypoint)
			return GetWayPointByIdx(i+1);
	}

	return NULL;
}

void CAirRoute::ClearData()
{
	std::vector<ARWaypoint *>::iterator iter = m_vARWaypoint.begin();
	std::vector<ARWaypoint *>::iterator iterEnd = m_vARWaypoint.end();
	for (;iter != iterEnd; ++iter)
	{
		delete *iter;
	}	
	m_vARWaypoint.clear();
	//delete m_pRunway;
	//m_pRunway = NULL;
}

bool CAirRoute::IsRunwayExist( int nRwy )
{
	size_t nSize = m_vConnectRunway.size();
	for (size_t i =0; i < nSize; i++)
	{
		if (m_vConnectRunway.at(i).first == nRwy)
			return true;
	}

	return false;
}

const GUID& CAirRoute::getTypeGUID()
{// {431FE52F-3039-4709-A04B-067E86BF209A}
	static const GUID class_guid = 
	{ 0x431fe52f, 0x3039, 0x4709, { 0xa0, 0x4b, 0x6, 0x7e, 0x86, 0xbf, 0x20, 0x9a } };

	return class_guid;
}

ALTObjectDisplayProp* CAirRoute::NewDisplayProp()
{
	return new AirRouteDisplayProp();
}

bool CAirRoute::HeadingValid(int nCurSel) const
{	
	int nWayPointCount = GetWayPointCount();
	if (nCurSel == CAirRoute::EN_ROUTE || nCurSel == CAirRoute::CIRCUIT)
	{
		if (nWayPointCount < 4)
			return false;

		if (m_vARWaypoint.at(nWayPointCount-2)->getDepartType() == ARWaypoint::Heading)
			return false;
	}
	else
	{
		if (nWayPointCount < 3)
			return false;
	}
	if (m_vARWaypoint.at(0)->getDepartType() == ARWaypoint::Heading)
		return false;

	if (m_vARWaypoint.at(nWayPointCount-1)->getDepartType() == ARWaypoint::Heading)
		return false;

	return true;
}

int CAirRoute::ExistHeading() const
{
	for (int i = 0; i < GetWayPointCount(); i++)
	{
		ARWaypoint* pWaypoint = m_vARWaypoint.at(i);
		if (pWaypoint->getDepartType() == ARWaypoint::Heading)
		{
			return i;
		}
	}
	return -1;
}

//////////////////////////////////////////////////////////////////////////
AirRouteList::AirRouteList()
{
	m_pIntersectionList = new AirRouteIntersectionList;
}
AirRouteList::~AirRouteList()
{
	std::vector<CAirRoute *>::iterator iter = m_vAirRoute.begin();
	std::vector<CAirRoute*>::iterator iterEnd = m_vAirRoute.end();

	for (;iter!=iterEnd; ++iter)
	{
		delete *iter;
	}

	delete m_pIntersectionList;
	m_pIntersectionList = NULL;

}
//class AirRouteList
void AirRouteList::ReadData(int nAirspaceID)
{
	CString strSQL = _T("");
	strSQL.Format(_T("SELECT *\
		FROM AIRROUTEPROP\
		WHERE (APTID = %d)"),nAirspaceID);
	long nRecordCount = -1;
	CADORecordset adoRecordset;
	CADODatabase::ExecuteSQLStatement(strSQL,nRecordCount,adoRecordset);

	while (!adoRecordset.IsEOF())
	{

		int nAirRouteID = -1;
		adoRecordset.GetFieldValue(_T("ID"),nAirRouteID);
		CAirRoute *pAirRoute = new CAirRoute(nAirRouteID);
		pAirRoute->ReadData(nAirRouteID);
		
//		pAirRoute->setAptID(nAirspaceID);
//		//name
//		CString strName = _T("");
//		adoRecordset.GetFieldValue(_T("NAME"),strName);
//		pAirRoute->setName(strName);
//
//		//type
//		int nAirRouteType = 0;
//		adoRecordset.GetFieldValue(_T("TYPE"),nAirRouteType);
//
//		pAirRoute->setRouteType((CAirRoute::RouteType)nAirRouteType);
//
//
//		//runway
//		int nObjID = -1;
//		//CString strName1 = _T("");
//		//CString strName2 = _T("");
//		//CString strName3 = _T("");
//		//CString strName4 = _T("");
//
//		//ALTObjectID objName;
//		adoRecordset.GetFieldValue(_T("RUNWAYID"),nObjID);
//
//		//adoRecordset.GetFieldValue(_T("NAME_L1"),strName1);
//		//objName.at(0) = strName1;
//
//		//adoRecordset.GetFieldValue(_T("NAME_L2"),strName2);
//		//objName.at(1) = strName2;
//
//		//adoRecordset.GetFieldValue(_T("NAME_L3"),strName3);
//		//objName.at(2) = strName3;	
//
//		//adoRecordset.GetFieldValue(_T("NAME_L4"),strName4);
//		//objName.at(3) = strName4;
//
////		ALTObject altObject;
//
////		altObject.setID(nObjID);
////		altObject.setObjName(objName);
//
////		pAirRoute->setRunwayObj(altObject);
//
//		int nMarkIndex = 0;
//		adoRecordset.GetFieldValue(_T("MARKINDEX"),nMarkIndex);
//		//
//		//pAirRoute->setRunwayID(nObjID,nMarkIndex);
//		pAirRoute->ReadLogicRunways();
//
//		pAirRoute->ReadARWaypoint();
//
//		pAirRoute->CalWaypointExtentPoint();
//		pAirRoute->ReadAirportInfo();

		m_vAirRoute.push_back(pAirRoute);

		//move to next data
		adoRecordset.MoveNextData();
	}
	m_pIntersectionList->ReadData(nAirspaceID);
}
void AirRouteList::DeleteData()
{
	std::vector<CAirRoute *>::iterator iter = m_vAirRoute.begin();
	std::vector<CAirRoute*>::iterator iterEnd = m_vAirRoute.end();

	for (;iter!=iterEnd; ++iter)
	{
		(*iter)->DeleteData();
		delete *iter;
	}
	m_vAirRoute.clear();
}

int AirRouteList::GetAirRouteCount() const
{
	return static_cast<int>(m_vAirRoute.size());

}
CAirRoute *AirRouteList::GetAirRoute(int nIndex)const
{	
	ASSERT (nIndex >= 0 && nIndex < GetAirRouteCount());
	
	return m_vAirRoute[nIndex];
}

CAirRoute* AirRouteList::GetAirRouteByID(int nAirRouteID)
{
	std::vector<CAirRoute*>::iterator iter = m_vAirRoute.begin();
	for (;iter != m_vAirRoute.end();++iter)
	{
		if ((*iter)->getID() == nAirRouteID)
		{
			return (*iter);
		}
	}
	return NULL;
}
CAirRoute * AirRouteList::DeleteAirRoute(int nIndex)
{
	ASSERT (nIndex >= 0 && nIndex < GetAirRouteCount());
	
	CAirRoute *pAirRoute = m_vAirRoute[nIndex];
	m_vAirRoute.erase(m_vAirRoute.begin() + nIndex);

	return pAirRoute;
}
void AirRouteList::AddAirRoute(CAirRoute *pAirRoute)
{
	m_vAirRoute.push_back(pAirRoute);
}
void AirRouteList::SaveData(int nAirspaceID)
{
	std::vector<CAirRoute *>::iterator iter = m_vAirRoute.begin();
	std::vector<CAirRoute*>::iterator iterEnd = m_vAirRoute.end();
	try
	{
		CADODatabase::BeginTransaction() ;
		for (;iter!=iterEnd; ++iter)
		{
			if((*iter)->getID() == -1)
			{
				(*iter)->SaveData(nAirspaceID);
			}
			else
			{
				(*iter)->UpdateData();
			}
		}
		CADODatabase::CommitTransaction() ;
	}
	catch (CADOException e)
	{
		CADODatabase::RollBackTransation() ;
	}
	
	//m_pIntersectionList->SaveData();
}
std::vector<CAirRoute *> AirRouteList::GetIntersectionAirRoute(CAirRoute * pAirRoute)
{
	std::vector<CAirRoute *> airRouteList;

	for (int i =0; i < GetAirRouteCount(); ++i)
	{
		if (GetAirRoute(i)->HasIntersection(pAirRoute))
		{
			airRouteList.push_back(GetAirRoute(i));
		}
	}

	return airRouteList;

}
AirRouteList::AirRouteList(class AirRouteList const &route)
{
	m_vAirRoute = route.m_vAirRoute;
	m_pIntersectionList = route.m_pIntersectionList;
}

void AirRouteList::CalAirRouteIntersectionListByIdx(int idx)
{
	CAirRoute* pRoute = GetAirRoute(idx);

	for (int i = idx+1; i < GetAirRouteCount(); i++)
	{
		CAirRoute* pOtherRoute = GetAirRoute(i);
		m_pIntersectionList->CalIntersectionInAirRoutes(pRoute,pOtherRoute);
	}
}

void AirRouteList::CalAirRouteIntersections()
{
	m_pIntersectionList->DeleteData();
	m_pIntersectionList->RemoveAllIntersections();
	for (int i =0; i < GetAirRouteCount()-1;i++)
	{
		CalAirRouteIntersectionListByIdx(i);
	}
	m_pIntersectionList->UpdateIntersectionList();
	m_pIntersectionList->SaveData();
}
void AirRouteList::ReCalAirRouteIntersectionsByIdx(int idx)
{
	CalAirRouteIntersectionListByIdx(idx);
	m_pIntersectionList->UpdateIntersectionList();
	m_pIntersectionList->SaveData();
}

AirRouteIntersectionList* AirRouteList::GetAirRouteIntersectionList()
{
	return m_pIntersectionList; 
}









