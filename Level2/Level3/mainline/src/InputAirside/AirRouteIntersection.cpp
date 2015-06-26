#include "StdAfx.h"
#include ".\airrouteintersection.h"
#include "../Database/ADODatabase.h"
#include "AirRoute.h"
#include "InputAirside/AirsideImportExportManager.h"
#include "../Common/DistanceLineLine.h"
#include "../Common/point.h"
#include "../Common/line.h"


IntersectedRouteSegment::IntersectedRouteSegment()
{
	m_nID = -1;
	m_nIntersectionID = -1;
	m_nWaypoint1ID = -1;
	m_nWaypoint2ID = -1;
	m_nRouteID = -1;
}

IntersectedRouteSegment::IntersectedRouteSegment(int nIntersectionID)
{
	m_nID = -1;
	m_nIntersectionID = nIntersectionID;
	m_nWaypoint1ID = -1;
	m_nWaypoint2ID = -1;
	m_nRouteID = -1;
}

IntersectedRouteSegment::~IntersectedRouteSegment()
{

}

IntersectedRouteSegment& IntersectedRouteSegment::operator = (const IntersectedRouteSegment& routeSeg)
{
	m_nID = routeSeg.m_nID;
	m_nIntersectionID = routeSeg.m_nIntersectionID;
	m_nRouteID = routeSeg.m_nRouteID;
	m_nWaypoint1ID = routeSeg.m_nWaypoint1ID;
	m_nWaypoint2ID = routeSeg.m_nWaypoint2ID;

	return *this;
}

void IntersectedRouteSegment::DeleteData()
{
	if (m_nID < 0 )
		return;

	CString strSQL;
	strSQL.Format(_T("DELETE FROM AIRROUTEINTERSECTDETAIL WHERE (ID = %d)"), m_nID);
	CADODatabase::ExecuteSQLStatement(strSQL);
}

void IntersectedRouteSegment::UpdateData()
{
	if (m_nID <0)
		return;

	CString strSQL;
	strSQL.Format(_T("UPDATE AIRROUTEINTERSECTDETAIL SET INTERSECTIONID = %d, AIRROUTE_ID = %d, ARWAYPOINT1_ID = %d, ARWAYPOINT2_ID = %d WHERE ID = %d"),
		m_nIntersectionID, m_nRouteID, m_nWaypoint1ID,m_nWaypoint2ID, m_nID);
	CADODatabase::ExecuteSQLStatement(strSQL);
}

void IntersectedRouteSegment::SaveData()
{
	CString strSQL;
	if (m_nID < 0)
	{
		strSQL.Format(_T("INSERT INTO AIRROUTEINTERSECTDETAIL (INTERSECTIONID, AIRROUTE_ID, ARWAYPOINT1_ID, ARWAYPOINT2_ID) VALUES (%d,%d,%d,%d)"),
			m_nIntersectionID, m_nRouteID, m_nWaypoint1ID,m_nWaypoint2ID);
		m_nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
	}
	else
	{
		UpdateData();
	}
}

void IntersectedRouteSegment::ImportRouteSeg(CAirsideImportFile& importFile)
{
	int nOldID = -1;
	importFile.getFile().getInteger(nOldID);
	int nOldIntersectionID = -1;
	importFile.getFile().getInteger(nOldIntersectionID);

	int nAirRouteID = -1;
	importFile.getFile().getInteger(nAirRouteID);
	setAirRouteID(importFile.GetAirRouteNewIndex(nAirRouteID));

	int nWaypointID = -1;
	importFile.getFile().getInteger(nWaypointID);
	setWaypoint1ID(importFile.GetAirRouteARWaypointNewIndex(nWaypointID));

	nWaypointID = -1;
	importFile.getFile().getInteger(nWaypointID);
	setWaypoint2ID(importFile.GetAirRouteARWaypointNewIndex(nWaypointID));

	SaveData();
	importFile.getFile().getLine();
}

void IntersectedRouteSegment::ExportRouteSeg(CAirsideExportFile& exportFile)
{
	exportFile.getFile().writeInt(m_nID);
	exportFile.getFile().writeInt(m_nIntersectionID);
	exportFile.getFile().writeInt(m_nRouteID);
	exportFile.getFile().writeInt(m_nWaypoint1ID);
	exportFile.getFile().writeInt(m_nWaypoint2ID);
	exportFile.getFile().writeLine();
}
///////////////CAirRouteIntersection/////////////////////
CAirRouteIntersection::CAirRouteIntersection(void)
{
	m_nID = -1;
	m_nAirspaceID = -1;
	m_dAltHigh = -1;
	m_dAltLow = -1;
	m_vIntersectedRoutes.clear();
	m_vIntersectedRoutesDel.clear();
}

CAirRouteIntersection::~CAirRouteIntersection(void)
{
	m_vIntersectedRoutes.clear();
	m_vIntersectedRoutesDel.clear();
}

CAirRouteIntersection& CAirRouteIntersection::operator = (const CAirRouteIntersection& intersection)
{
	m_nID = intersection.m_nID;
	m_nAirspaceID = intersection.m_nAirspaceID;
	m_pos = intersection.m_pos;
	m_dAltLow = intersection.m_dAltLow;
	m_dAltHigh = intersection.m_dAltHigh;
	m_vIntersectedRoutes = intersection.m_vIntersectedRoutes;
	m_vIntersectedRoutesDel = intersection.m_vIntersectedRoutesDel;

	return *this;
}

void CAirRouteIntersection::ReadData()
{
	CString strSQL = _T("");
	strSQL.Format(_T("SELECT ID, INTERSECTIONID, AIRROUTE_ID, ARWAYPOINT1_ID,ARWAYPOINT2_ID\
					 FROM AIRROUTEINTERSECTDETAIL\
					 WHERE (INTERSECTIONID = %d)"),m_nID);
	long nRecordCount = -1;
	CADORecordset adoRecordset;
	CADODatabase::ExecuteSQLStatement(strSQL,nRecordCount,adoRecordset);

	while (!adoRecordset.IsEOF())
	{

		int  nIntersectionID = -1;
		adoRecordset.GetFieldValue(_T("INTERSECTIONID"),nIntersectionID);
		IntersectedRouteSegment routeSeg(nIntersectionID);

		int nID = -1;
		adoRecordset.GetFieldValue(_T("ID"),nID);
		routeSeg.setID(nID);

		int nRouteID = -1;
		adoRecordset.GetFieldValue(_T("AIRROUTE_ID"),nRouteID);
		routeSeg.setAirRouteID(nRouteID);

		int nWaypointID = -1;
		adoRecordset.GetFieldValue(_T("ARWAYPOINT1_ID"),nWaypointID);
		routeSeg.setWaypoint1ID(nWaypointID);

		nWaypointID = -1;
		adoRecordset.GetFieldValue(_T("ARWAYPOINT2_ID"),nWaypointID);
		routeSeg.setWaypoint2ID(nWaypointID);

		m_vIntersectedRoutes.push_back(routeSeg);

		//move to next data
		adoRecordset.MoveNextData();
	}
}

void CAirRouteIntersection::DeleteData()
{
	if (m_nID < 0 )
		return;

	DeleteIntersectedRouteList();

	CString strSQL;
	strSQL.Format(_T("DELETE FROM AIRROUTEINTERSECTIONPROP WHERE (ID = %d)"), m_nID);
	CADODatabase::ExecuteSQLStatement(strSQL);
}

void CAirRouteIntersection::SaveData()
{
	CString strSQL;
	if (m_nID < 0)
	{
		strSQL.Format(_T("INSERT INTO AIRROUTEINTERSECTIONPROP (PRJID, POS_X, POS_Y, ALT_HIGH, ALT_LOW, NAME) VALUES (%d,%f,%f,%f,%f,'%s')"),
			m_nAirspaceID, m_pos.getX(), m_pos.getY(), m_dAltHigh, m_dAltLow,m_strName);
		m_nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
	}
	else
	{
		strSQL.Format(_T("UPDATE AIRROUTEINTERSECTIONPROP SET PRJID = %d, POS_X = %f, POS_Y = %f, ALT_HIGH = %f, ALT_LOW = %f, NAME = '%s' WHERE ID = %d"),
			m_nAirspaceID, m_pos.getX(), m_pos.getY(), m_dAltHigh, m_dAltLow,m_strName, m_nID);
		CADODatabase::ExecuteSQLStatement(strSQL);
	}

	for (int i = 0; i < int(m_vIntersectedRoutes.size()); i++)
	{
		m_vIntersectedRoutes[i].setIntersectionID(m_nID);
		m_vIntersectedRoutes[i].SaveData();
	}

	for (int i = 0; i < int(m_vIntersectedRoutesDel.size()); i++)
	{
		m_vIntersectedRoutesDel[i].DeleteData();
	}
	m_vIntersectedRoutesDel.clear();
}

void CAirRouteIntersection::ImportIntersection(CAirsideImportFile& importFile)
{
	int nOldID = -1;
	importFile.getFile().getInteger(nOldID);
	int nOldAirspaceID = -1;
	importFile.getFile().getInteger(nOldAirspaceID);
	setAirspaceID(importFile.getNewAirspaceID());
	double dPos = 0.0;
	importFile.getFile().getFloat(dPos);
	m_pos.setX(dPos);
	importFile.getFile().getFloat(dPos);
	m_pos.setY(dPos);

	importFile.getFile().getFloat(m_dAltHigh);
	importFile.getFile().getFloat(m_dAltLow);
	importFile.getFile().getField(m_strName.GetBuffer(1024),1024);

	int nAirRouteCount = 0;

	importFile.getFile().getInteger(nAirRouteCount);
	SaveData();
	importFile.getFile().getLine();

	for (int i =0; i< nAirRouteCount; ++i)
	{
		IntersectedRouteSegment routeSeg(m_nID);
		routeSeg.ImportRouteSeg(importFile);
	}

}

void CAirRouteIntersection::ExportIntersection(CAirsideExportFile& exportFile)
{
	exportFile.getFile().writeInt(m_nID);
	exportFile.getFile().writeInt(m_nAirspaceID);
	exportFile.getFile().writeDouble(m_pos.getX());
	exportFile.getFile().writeDouble(m_pos.getY());
	exportFile.getFile().writeDouble(m_dAltHigh);
	exportFile.getFile().writeDouble(m_dAltLow);
	exportFile.getFile().writeField(m_strName);

	int nAirRouteCount = getIntersectedRouteCount();

	exportFile.getFile().writeInt(nAirRouteCount);
	exportFile.getFile().writeLine();

	for (int i= 0; i < nAirRouteCount; ++ i)
	{
		getIntersectedRouteSegByIdx(i).ExportRouteSeg(exportFile);
	}
}

IntersectedRouteSegment CAirRouteIntersection::getIntersectedRouteSeg(CAirRoute* pRoute)
{
	IntersectedRouteSegment routeSeg;
	int nRouteID = pRoute->getID();

	for (int i = 0; i < int(m_vIntersectedRoutes.size()); i++)
	{
		if (m_vIntersectedRoutes[i].getAirRouteID() == nRouteID)
			routeSeg = m_vIntersectedRoutes[i];
	}
	return routeSeg;
}

bool CAirRouteIntersection::IsIntersectionInRoute(CAirRoute* pRoute)
{
	for (int i = 0; i < int(m_vIntersectedRoutes.size()); i++)
	{
		if (m_vIntersectedRoutes[i].getAirRouteID() == pRoute->getID())
			return true;
	}
	return false;
}

void CAirRouteIntersection::DeleteIntersectedRoute(CAirRoute* pRoute)
{
	for (int i = 0; i < int(m_vIntersectedRoutes.size()); i++)
	{
		if (m_vIntersectedRoutes[i].getAirRouteID() == pRoute->getID())
		{
			m_vIntersectedRoutesDel.push_back(m_vIntersectedRoutes[i]);
			m_vIntersectedRoutes.erase(m_vIntersectedRoutes.begin()+i);
		}
	}
}

void CAirRouteIntersection::DeleteIntersectedRouteList()
{
	for (int i = 0; i < int(m_vIntersectedRoutes.size()); i++)
	{
		m_vIntersectedRoutes[i].DeleteData();
	}
	for (int j = 0; j < int(m_vIntersectedRoutesDel.size()); j++)
	{
		m_vIntersectedRoutesDel[j].DeleteData();
	}
}

bool CAirRouteIntersection::IsSamePoint(CPoint2008 pointPos, double dAlthigh,double dAltlow)
{
	if (pointPos == m_pos)
	{
		if (m_dAltHigh < dAltlow || m_dAltLow > dAlthigh)
			return false;

		return true;
	}
	return false;
}
///////////////AirRouteIntersectionList////////////////////
AirRouteIntersectionList::AirRouteIntersectionList()
{
	m_vAirRouteIntersectionList.clear();
	m_vAirRouteIntersectionDelList.clear();
}

AirRouteIntersectionList::~AirRouteIntersectionList()
{
	RemoveAllIntersections();
}

std::vector<CAirRouteIntersection*> AirRouteIntersectionList::GetAirRouteIntersection(CAirRoute* pRoute)
{
	std::vector<CAirRouteIntersection*> vIntersections;
	vIntersections.clear();

	for (int i = 0; i < GetAllAirRouteIntersectionCount(); i++)
	{
		CAirRouteIntersection* pIntersection = GetAirRouteIntersectionByIdx(i);
		if (pIntersection->IsIntersectionInRoute(pRoute))
			vIntersections.push_back(pIntersection);
	}

	return vIntersections;
}

void AirRouteIntersectionList::RemoveAllIntersections()
{
	std::vector<CAirRouteIntersection*>::iterator iter;
	std::vector<CAirRouteIntersection*>::iterator iterDel;
	for (iter = m_vAirRouteIntersectionList.begin(); iter != m_vAirRouteIntersectionList.end(); iter++)
	{
		delete (*iter);
		(*iter) = NULL;
	}
	for (iterDel = m_vAirRouteIntersectionDelList.begin(); iterDel != m_vAirRouteIntersectionDelList.end(); iterDel++)
	{
		delete (*iterDel);
		(*iterDel) = NULL;
	}
	m_vAirRouteIntersectionList.clear();
	m_vAirRouteIntersectionDelList.clear();
}

void AirRouteIntersectionList::CalIntersectionInAirRoutes(CAirRoute* pRoute,CAirRoute*pOtherRoute)				//calculate the intersection of two air route
{
	std::vector<CPoint2008> pointList;
	GetIntersectedPoints(pRoute,pOtherRoute,pointList);

	for (int i = 0; i < GetAllAirRouteIntersectionCount(); i++ )
	{
		if (GetAirRouteIntersectionByIdx(i)->IsIntersectionInRoute(pRoute) && GetAirRouteIntersectionByIdx(i)->IsIntersectionInRoute(pOtherRoute))
		{
			int nSegIdx = -1;
			CPoint2008 IntersectPos = GetAirRouteIntersectionByIdx(i)->getPos();
			for (int j =0; j <int(pointList.size());j++)
			{
				CPoint2008 pos = pointList[j];
				if ((int(IntersectPos.getX()/10) == int(pos.getX()/10)) && (int(IntersectPos.getY()/10) == int(pos.getY()/10)))
				{
					nSegIdx = j;
					break;
				}
			}

			if (nSegIdx < 0)
			{
				GetAirRouteIntersectionByIdx(i)->DeleteIntersectedRoute(pRoute);

				if (GetAirRouteIntersectionByIdx(i)->getIntersectedRouteCount() < 2)
					DeleteData(GetAirRouteIntersectionByIdx(i));
			}
		}
	}

	for (int i = 0; i < int(pointList.size());i++)
	{
		CPoint2008 pointPos = pointList[i];
		ARWaypoint* pForeWaypoint1 = pRoute->getFrontWaypoint(pointPos);
		ARWaypoint* pAfterWaypoint1 = pRoute->getAfterWaypoint(pForeWaypoint1);
		if (pForeWaypoint1 == NULL || pAfterWaypoint1 == NULL)
			continue;

		ARWaypoint* pForeWaypoint2 = pOtherRoute->getFrontWaypoint(pointPos);
		ARWaypoint* pAfterWaypoint2 = pOtherRoute->getAfterWaypoint(pForeWaypoint2);
		if (pForeWaypoint2 == NULL || pAfterWaypoint2 == NULL)
			continue;

		if (pForeWaypoint1->getWaypoint().getID() == pForeWaypoint2->getWaypoint().getID() 
			&& pAfterWaypoint1->getWaypoint().getID() == pAfterWaypoint2->getWaypoint().getID())		//same segment
			continue;

		double AltHigh1 = GetPointAltHigh(pForeWaypoint1, pAfterWaypoint1, pointPos);
		double AltLow1 = GetPointAltLow(pForeWaypoint1,pAfterWaypoint1,pointPos);
		
		double AltHigh2 = GetPointAltHigh(pForeWaypoint2, pAfterWaypoint2, pointPos);
		double AltLow2 = GetPointAltLow(pForeWaypoint2,pAfterWaypoint2,pointPos);

		if ((AltHigh1 < AltLow2) || (AltHigh2 < AltLow1))
			continue;

		AltHigh1 = AltHigh1 > AltHigh2 ? AltHigh1: AltHigh2;
		AltLow1 = AltLow1 < AltLow2 ? AltLow1 : AltLow2;

		CAirRouteIntersection* pIntersection = NULL;
		
		int j =0;
		for(; j < GetAllAirRouteIntersectionCount(); j ++)
		{
			if (GetAirRouteIntersectionByIdx(j)->IsSamePoint(pointPos,AltHigh1,AltLow1))
			{	
				pIntersection = GetAirRouteIntersectionByIdx(j);
				break;
			}
		}
		
		if (pIntersection == NULL)
		{
			pIntersection = new CAirRouteIntersection;
			pIntersection->setAirspaceID(pRoute->getAptID());
			pIntersection->setPos(pointPos);
			AddAirRouteIntersection(pIntersection);
		}
		
		if (!pIntersection->IsIntersectionInRoute(pRoute))
		{
			IntersectedRouteSegment seg(GetAirRouteIntersectionByIdx(j)->getID());
			seg.setAirRouteID( pRoute->getID());
			seg.setWaypoint1ID( pForeWaypoint1->getWaypoint().getID());
			seg.setWaypoint2ID( pAfterWaypoint1->getWaypoint().getID());
			pIntersection->AddIntersectedRouteSeg(seg);
		}

		if (!pIntersection->IsIntersectionInRoute(pOtherRoute))
		{
			IntersectedRouteSegment seg2(GetAirRouteIntersectionByIdx(j)->getID());
			seg2.setAirRouteID( pOtherRoute->getID());
			seg2.setWaypoint1ID( pForeWaypoint2->getWaypoint().getID());
			seg2.setWaypoint2ID( pAfterWaypoint2->getWaypoint().getID());
			pIntersection->AddIntersectedRouteSeg(seg2);
		}

		if (pIntersection->getAltHigh() < AltHigh1)
			pIntersection->setAltHigh(AltHigh1);
		if (pIntersection->getAltLow() < 0 ||pIntersection->getAltLow() > AltLow1)
			pIntersection->setAltLow(AltLow1);
	}


}

void AirRouteIntersectionList::GetIntersectedPoints(CAirRoute* pRoute,CAirRoute* pOtherRoute,std::vector<CPoint2008>& PosList)
{
	CPath2008 route = pRoute->getPath();
	CPath2008 otherRoute = pOtherRoute->getPath();
	IntersectPathPath2D intersectPath;
	int intersectCount = intersectPath.Intersects(route,otherRoute,0);

	for(int i =0 ; i< intersectCount;i ++)
	{
		double dPos1 = route.GetIndexDist((float)intersectPath.m_vIntersectPtIndexInPath1.at(i));
		double dPos2 = otherRoute.GetIndexDist((float)intersectPath.m_vIntersectPtIndexInPath2.at(i));
		CPoint2008 pos = route.GetDistPoint(dPos1);
		CPoint2008 pos2 = otherRoute.GetDistPoint(dPos2);
			PosList.push_back(pos);	
	}
}

double AirRouteIntersectionList::GetPointAltHigh(ARWaypoint* pForeWaypoint, ARWaypoint* pAfterWaypoint,CPoint2008 pointPos)
{
	CPoint2008 fromPos = pForeWaypoint->getWaypoint().GetServicePoint();
	CPoint2008 toPos = pAfterWaypoint->getWaypoint().GetServicePoint();

	//double dAltFront = (double)pForeWaypoint->getMaxHeight();
	//if (dAltFront < 0)
		double dAltFront = pForeWaypoint->getWaypoint().GetHighLimit();

	//double dAltAfter = (double)pAfterWaypoint->getMaxHeight();
	//if (dAltAfter < 0)
		double dAltAfter = pAfterWaypoint->getWaypoint().GetHighLimit();

	double dratio = (pointPos.distance(fromPos))/(toPos.distance(fromPos));

	return dAltAfter * dratio + dAltFront * (1 - dratio);
}

double AirRouteIntersectionList::GetPointAltLow(ARWaypoint* pForeWaypoint, ARWaypoint* pAfterWaypoint,CPoint2008 pointPos)
{
	CPoint2008 fromPos = pForeWaypoint->getWaypoint().GetServicePoint();
	CPoint2008 toPos = pAfterWaypoint->getWaypoint().GetServicePoint();

	//double dAltFront = (double)pForeWaypoint->getMinHeight();
	//if (dAltFront < 0)
		double dAltFront = pForeWaypoint->getWaypoint().GetLowLimit();

	//double dAltAfter = (double)pAfterWaypoint->getMinHeight();
	//if (dAltAfter < 0)
		double dAltAfter = pAfterWaypoint->getWaypoint().GetLowLimit();

	double dratio = (pointPos.distance(fromPos))/(toPos.distance(fromPos));

	return dAltAfter * dratio + dAltFront * (1 - dratio);
}

void AirRouteIntersectionList::UpdateIntersectionList()
{
	for (int i = 0; i < int(m_vAirRouteIntersectionList.size()); i++)
	{
		if( GetAirRouteIntersectionByIdx(i)->getIntersectedRouteCount() < 2 )
		{
			m_vAirRouteIntersectionDelList.push_back(m_vAirRouteIntersectionList.at(i));
			m_vAirRouteIntersectionList.erase(m_vAirRouteIntersectionList.begin() + i);
		}
	}
}

void AirRouteIntersectionList::ReadData(int nAirspaceID)
{

	CString strSQL = _T("");
	strSQL.Format(_T("SELECT ID, PRJID, POS_X, POS_Y,ALT_HIGH,ALT_LOW,NAME\
					 FROM AIRROUTEINTERSECTIONPROP\
					 WHERE (PRJID = %d)"),nAirspaceID);
	long nRecordCount = -1;
	CADORecordset adoRecordset;
	CADODatabase::ExecuteSQLStatement(strSQL,nRecordCount,adoRecordset);

	while (!adoRecordset.IsEOF())
	{

		int nID = -1;
		adoRecordset.GetFieldValue(_T("ID"),nID);
		CAirRouteIntersection *pIntersection = new CAirRouteIntersection;
		pIntersection->setID(nID);
		pIntersection->setAirspaceID(nAirspaceID);

		CPoint2008 pos;
		double dVal;
		adoRecordset.GetFieldValue(_T("POS_X"),dVal);
		pos.setX(dVal);
		adoRecordset.GetFieldValue(_T("POS_Y"),dVal);
		pos.setY(dVal);
		pIntersection->setPos(pos);

		double dAlt = -1.0;
		adoRecordset.GetFieldValue(_T("ALT_HIGH"),dAlt);
		pIntersection->setAltHigh(dAlt);
		adoRecordset.GetFieldValue(_T("ALT_LOW"),dAlt);
		pIntersection->setAltLow(dAlt);


		//name
		CString strName = _T("");
		adoRecordset.GetFieldValue(_T("NAME"),strName);
		pIntersection->setName(strName);


		pIntersection->ReadData();

		m_vAirRouteIntersectionList.push_back(pIntersection);


		//move to next data
		adoRecordset.MoveNextData();
	}
}

void AirRouteIntersectionList::DeleteData(CAirRouteIntersection* pIntersection)
{
	size_t nIdx = -1;
	size_t nCount = m_vAirRouteIntersectionList.size();
	for (size_t i = 0; i < nCount; ++i)
	{
		if (pIntersection == m_vAirRouteIntersectionList.at(i))
		{
			nIdx = i;
			break;
		}
	}

	if (nIdx >= 0)
	{
		m_vAirRouteIntersectionDelList.push_back(m_vAirRouteIntersectionList.at(nIdx));
		m_vAirRouteIntersectionList.erase(m_vAirRouteIntersectionList.begin() + nIdx);

	}
}

void AirRouteIntersectionList::SaveData()
{
	std::vector<CAirRouteIntersection*>::iterator iter;
	std::vector<CAirRouteIntersection*>::iterator iterDel;
	try
	{
		CADODatabase::BeginTransaction() ;
		for (iterDel = m_vAirRouteIntersectionDelList.begin();iterDel != m_vAirRouteIntersectionDelList.end(); ++iterDel)
		{
			(*iterDel)->DeleteData();
		}

		for (iter = m_vAirRouteIntersectionList.begin();iter != m_vAirRouteIntersectionList.end(); ++iter)
		{
			(*iter)->SaveData();
		}
		CADODatabase::CommitTransaction() ;
	}
	catch (CADOException e)
	{
		CADODatabase::RollBackTransation() ;
	}

}

void AirRouteIntersectionList::DeleteData()
{
	for (int i = 0; i < GetAllAirRouteIntersectionCount(); i++)
	{
		GetAirRouteIntersectionByIdx(i)->DeleteData();
	}
}

void AirRouteIntersectionList::ImportAirRouteIntersections(CAirsideImportFile& importFile)
{
	while (!importFile.getFile().isBlank())
	{
		CAirRouteIntersection intersection;
		intersection.setAirspaceID(importFile.getNewProjectID());
		intersection.ImportIntersection(importFile);
	}

}

void AirRouteIntersectionList::ExportAirRouteIntersections(CAirsideExportFile& exportFile)
{
	exportFile.getFile().writeField(_T("AirrouteIntersection"));
	exportFile.getFile().writeLine();

	AirRouteIntersectionList intersectionlist;
	intersectionlist.ReadData(exportFile.GetProjectID());

	int nCount = intersectionlist.GetAllAirRouteIntersectionCount() ;
	for (int i =0; i < nCount; ++i)
	{
		intersectionlist.GetAirRouteIntersectionByIdx(i)->ExportIntersection(exportFile);
	}
	exportFile.getFile().endFile();
}


