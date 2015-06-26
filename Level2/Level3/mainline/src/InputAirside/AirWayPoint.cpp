#include "StdAfx.h"
#include ".\Airwaypoint.h"
#include "ALTAirspace.h"
#include "AirsideImportExportManager.h"
#include "../Common/line2008.h"
#include "ALTAirport.h"
#include "ALTObjectDisplayProp.h"


//const GUID AirWayPoint::class_guid = 
//{ 0x72f8e5f4, 0xf38d, 0x4340, { 0xba, 0xec, 0x1a, 0xcf, 0x9d, 0x61, 0x7f, 0x29 } };


AirWayPoint::AirWayPoint(void)
{
	//share
	m_nflag = 0;

	////method0 WayPointByPosition
	//CPoint2008 m_serviceLocation;
	//method1 WayPointByWayPointBearingDistance
	m_nOtherWayPointID = 0;
	m_dBearing = 0;
	m_dDistance = 0;
	//method2 WayPointByWayPointBearingAndWayPointBearing
	m_nOtherWayPoint1ID = 0;
	m_dBearing1 = 0;
	m_nOtherWayPoint2ID = 0;
	m_dBearing2 = 0;

	m_dlowlimit = 30000;
	m_dhightlimit = 40000.0;

//	m_nAptID = this->getAptID();
//
////	this->GetObjectList(m_nAptID,m_vWaypoint);
//	ALTObject::GetObjectList(ALT_WAYPOINT,m_nAptID,m_vWaypoint);
}

AirWayPoint::~AirWayPoint(void)
{
}
void AirWayPoint::SetServicePoint( const CPoint2008& _pt )
{
	m_serviceLocation = _pt;
}

CPoint2008 AirWayPoint::GetServicePoint()
{
	ALTAirport _ARP;
	_ARP.ReadAirport(m_nAptID);

	return GetLocation(_ARP);;
}
void AirWayPoint::WayPointBearingDistanceClear()
{
	m_nOtherWayPointID = 0;
	m_dBearing = 0;
	m_dDistance = 0;
}
void AirWayPoint::WayPointBearingAndWayPointBearingClear()
{
	m_nOtherWayPoint1ID = 0;
	m_dBearing1 = 0;
	m_nOtherWayPoint2ID = 0;
	m_dBearing2 = 0;
}
//database operation

//SELECT NAME_L1, NAME_L2, NAME_L3, NAME_L4, LOWLMT, HIGHLMT, SVCLOCTID,
//POINTCOUNT, SVCLOCT, TSTAMP
void AirWayPoint::ReadObject(int nObjID)
{m_nObjID = nObjID;
	CADORecordset adoRecordset;
	long nRecordCount = -1;
	CADODatabase::ExecuteSQLStatement(GetSelectScript(nObjID),nRecordCount,adoRecordset);

	if (!adoRecordset.IsEOF())
	{	
		ALTObject::ReadObject(adoRecordset);

		double ptx,pty,ptz;
		adoRecordset.GetFieldValue(_T("LCTX"),ptx);

		adoRecordset.GetFieldValue(_T("LCTY"),pty);

		adoRecordset.GetFieldValue(_T("LCTZ"),ptz);
		m_serviceLocation.setPoint(ptx,pty,ptz);

		adoRecordset.GetFieldValue(_T("FLAG"),m_nflag);
		if(m_nflag < 0 || m_nflag > 2)
			m_nflag = 0;
//		int nOtherWayPointID;
		adoRecordset.GetFieldValue(_T("OTHERWAYPOINTID"),m_nOtherWayPointID);
		////-------
		//for (int i = 0; i < static_cast<int>(m_vWaypoint.size()); ++ i)
		//{
		//	if(m_vWaypoint[i].getID() == nOtherWayPointID)
		//		m_pOtherWayPoint = (AirWayPoint*)(&m_vWaypoint[i]);
		//}
		////----------
		adoRecordset.GetFieldValue(_T("BEARING"),m_dBearing);
		adoRecordset.GetFieldValue(_T("DISTANCE"),m_dDistance);

	//	int nOtherWayPoint1ID;
		adoRecordset.GetFieldValue(_T("OTHERWAYPOINT1ID"),m_nOtherWayPoint1ID);
	//	m_pOtherWayPoint1->setID(nOtherWayPoint1ID);

		adoRecordset.GetFieldValue(_T("BEARING1"),m_dBearing1);
	//	int nOtherWayPoint2ID;
		adoRecordset.GetFieldValue(_T("OTHERWAYPOINT2ID"),m_nOtherWayPoint2ID);
	//	m_pOtherWayPoint2->setID(nOtherWayPoint2ID);
		adoRecordset.GetFieldValue(_T("BEARING2"),m_dBearing2);

		adoRecordset.GetFieldValue(_T("LOWLMT"),m_dlowlimit);
		adoRecordset.GetFieldValue(_T("HIGHLMT"),m_dhightlimit);
	}

}
int AirWayPoint::SaveObject(int nAirportID)
{
	int nObjID = ALTObject::SaveObject(nAirportID,ALT_WAYPOINT);

	//Path tempPath;
	//tempPath.init(1,&m_serviceLocation);
	//m_nSvcptID = CADODatabase::SavePathIntoDatabase(tempPath);

	CString strSQL = GetInsertScript(nObjID);

	CADODatabase::ExecuteSQLStatement(strSQL);

	return nObjID;

}
void AirWayPoint::UpdateObject(int nObjID)
{
	ALTObject::UpdateObject(nObjID);

	//Path tempPath;
	//tempPath.init(1,&m_serviceLocation);
	//CADODatabase::UpdatePathInDatabase(tempPath,m_nSvcptID);

	CString strUpdateScript = GetUpdateScript(nObjID);
	CADODatabase::ExecuteSQLStatement(strUpdateScript);
}
void AirWayPoint::DeleteObject(int nObjID)
{
	ALTObject::DeleteObject(nObjID);

	//CADODatabase::DeletePathFromDatabase(m_nSvcptID);
	CString strDeleteScript = GetDeleteScript(nObjID);

	CADODatabase::ExecuteSQLStatement(strDeleteScript);

}
CString AirWayPoint::GetSelectScript(int nObjID) const
{
	CString strSQL = _T("");
	strSQL.Format(_T("SELECT APTID, NAME_L1, NAME_L2, NAME_L3, NAME_L4, LOCKED, LCTX, LCTY, LCTZ, \
		FLAG, OTHERWAYPOINTID, BEARING, DISTANCE, OTHERWAYPOINT1ID, BEARING1, OTHERWAYPOINT2ID, BEARING2, \
		LOWLMT,HIGHLMT, TSTAMP\
		FROM WAYPOINT_VIEW\
		WHERE ID = %d"),nObjID);
	
	return strSQL;
}
CString AirWayPoint::GetInsertScript(int nObjID) const
{
	CString strSQL = _T("");
	strSQL.Format(_T("INSERT INTO WAYPOINTPROP \
		(OBJID,LCTX, LCTY, LCTZ , FLAG, OTHERWAYPOINTID, BEARING, DISTANCE, OTHERWAYPOINT1ID, BEARING1, OTHERWAYPOINT2ID, BEARING2, LOWLMT, HIGHLMT) \
		VALUES (%d,%f,%f,%f,%d,%d,%f,%f,%d,%f,%d,%f,%f,%f)"),
		nObjID,m_serviceLocation.getX(),m_serviceLocation.getY(),m_serviceLocation.getZ(),m_nflag,m_nOtherWayPointID,m_dBearing,m_dDistance,m_nOtherWayPoint1ID,m_dBearing1,m_nOtherWayPoint2ID,m_dBearing2,m_dlowlimit,m_dhightlimit);

	return strSQL;
}
CString AirWayPoint::GetUpdateScript(int nObjID) const
{
	CString strSQL = _T("");
	strSQL.Format(_T("UPDATE WAYPOINTPROP \
					 SET LCTX = %f, LCTY = %f, LCTZ = %f, FLAG = %d, OTHERWAYPOINTID = %d, BEARING = %f, DISTANCE = %f,\
					 OTHERWAYPOINT1ID = %d, BEARING1 = %f, OTHERWAYPOINT2ID = %d, BEARING2 = %f, LOWLMT = %f, HIGHLMT = %f \
					 WHERE OBJID = %d"),
					 m_serviceLocation.getX(),m_serviceLocation.getY(),m_serviceLocation.getZ(),m_nflag,m_nOtherWayPointID,m_dBearing,m_dDistance,m_nOtherWayPoint1ID,m_dBearing1,m_nOtherWayPoint2ID,m_dBearing2,m_dlowlimit,m_dhightlimit,nObjID);

	return strSQL;
}
CString AirWayPoint::GetDeleteScript(int nObjID) const
{
	CString strSQL = _T("");
	strSQL.Format(_T("DELETE FROM WAYPOINTPROP \
		WHERE OBJID = %d"),nObjID);

	return strSQL;
}

ALTObject * AirWayPoint::NewCopy()
{
	AirWayPoint * reslt = new AirWayPoint;
	reslt->CopyData(this);
	return reslt;
}
void AirWayPoint::ExportWaypoints(CAirsideExportFile& exportFile)
{

}
//CPoint2008 m_serviceLocation;
//double m_dlowlimit;
//double m_dhightlimit;
bool AirWayPoint::IsInwaypointListOrder(ALTObjectUIDList* pwaypointListOrder,int nID)
{
	ALTObjectUIDList waypointListOrder = *pwaypointListOrder;
	ALTObjectUIDList::iterator iter = std::find(waypointListOrder.begin(),waypointListOrder.end(),nID);
	if (iter == waypointListOrder.end())
		return false;
	return true;
}
void AirWayPoint::ExportWaypoint(CAirsideExportFile& exportFile)
{

}
void AirWayPoint::ImportObject(CAirsideImportFile& importFile)
{
	ALTObject::ImportObject(importFile);
	
	int nAirspaceID = -1;
	importFile.getFile().getInteger(nAirspaceID);
	m_nAptID = importFile.getNewAirspaceID();
	importFile.getFile().getPoint2008(m_serviceLocation);
	importFile.getFile().getFloat(m_dlowlimit);
	importFile.getFile().getFloat(m_dhightlimit);		
	importFile.getFile().getInteger(m_nflag);

	int nOldOtherWayPointID = -1;
	importFile.getFile().getInteger(nOldOtherWayPointID);
	m_nOtherWayPointID = importFile.GetObjectNewID(nOldOtherWayPointID);
	//importFile.getFile().getInteger(m_nOtherWayPointID);

	importFile.getFile().getFloat(m_dBearing);
	importFile.getFile().getFloat(m_dDistance);

	int nOldOtherWayPoint1ID = -1;
	importFile.getFile().getInteger(nOldOtherWayPoint1ID);
	m_nOtherWayPoint1ID = importFile.GetObjectNewID(nOldOtherWayPoint1ID);
	//importFile.getFile().getInteger(m_nOtherWayPoint1ID);

	importFile.getFile().getFloat(m_dBearing1);

	int nOldOtherWayPoint2ID = -1;
	importFile.getFile().getInteger(nOldOtherWayPoint2ID);
	m_nOtherWayPoint2ID = importFile.GetObjectNewID(nOldOtherWayPoint2ID);
	//importFile.getFile().getInteger(m_nOtherWayPoint2ID);

	importFile.getFile().getFloat(m_dBearing2);
	
	importFile.getFile().getLine();

	int nOldObjectID = m_nObjID;
	int nNewObjID = SaveObject(m_nAptID);
	importFile.AddObjectIndexMap(nOldObjectID,nNewObjID);


	AirWayPointDisplayProp dspProp;
	dspProp.ImportDspProp(importFile,m_nObjID); 
}

void AirWayPoint::GetWayPointList( int nAirspaceID , std::vector<AirWayPoint>& list )
{
	ALTObjectUIDList vIndexList;
	ALTAirspace::GetWaypointList(nAirspaceID,vIndexList);
	list.resize(vIndexList.size());
	for(int i=0;i< (int)vIndexList.size(); i++)
	{
		list[i].ReadObject(vIndexList.at(i));
	}
}

AirWayPoint * AirWayPoint::GetOtherWayPoint()
{
	if(!m_pOtherWayPoint.get())
	{
		m_pOtherWayPoint = new AirWayPoint;		
		m_pOtherWayPoint->ReadObject(GetOtherWayPointID());
	}
	return m_pOtherWayPoint.get();

}

AirWayPoint * AirWayPoint::GetOtherWayPoint1()
{
	if(!m_pOtherWayPoint1.get())
	{
		m_pOtherWayPoint1 = new AirWayPoint;		
		m_pOtherWayPoint1->ReadObject(GetOtherWayPoint1ID());
	}
	return m_pOtherWayPoint1.get();
}

AirWayPoint * AirWayPoint::GetOtherWayPoint2()
{
	if(!m_pOtherWayPoint2.get())
	{
		m_pOtherWayPoint2 = new AirWayPoint;		
		m_pOtherWayPoint2->ReadObject(GetOtherWayPoint2ID());
	}
	return m_pOtherWayPoint2.get();
}

CPoint2008 AirWayPoint::GetLocation( const ALTAirport& refARP )
{
	if( m_nflag == 0)
	{
		
	}
	//for case 2
	if( m_nflag == 1)
	{
		AirWayPoint * pOtherWayPt = GetOtherWayPoint();
		ARCVector2 vDir(0,1);
		vDir.Rotate( GetBearing() + refARP.GetMagnectVariation().GetRealVariation());
		vDir.SetLength(GetDistance());
		return pOtherWayPt->GetLocation(refARP) + CPoint2008(vDir[VX],vDir[VY],0);
	}
	//for case 3
	if(m_nflag == 2)
	{
		AirWayPoint * pOtherWayPt1= GetOtherWayPoint1();
		AirWayPoint * pOtherWayPt2 = GetOtherWayPoint2();	
		double degree1 =  GetBearing1()/SCALE_FACTOR + refARP.GetMagnectVariation().GetRealVariation();
		double degree2 =  GetBearing2()/SCALE_FACTOR + refARP.GetMagnectVariation().GetRealVariation();
		CPoint2008 p1 = pOtherWayPt1->GetLocation(refARP);
		CPoint2008 p2 = pOtherWayPt2->GetLocation(refARP);
		CPoint2008 p3 = CPoint2008(0,1,0);
		CPoint2008 p4 = CPoint2008(0,1,0);
		p3.rotate(degree1);
		p4.rotate(degree2);

		
		CLine2008 line1(p1, p3 + p1);
		CLine2008 line2(p2, p4 + p2);
		return line1.intersection(line2);
	}	
	return m_serviceLocation;
	
}

double AirWayPoint::GetHighLimit() const
{
  return m_dhightlimit;
}

double AirWayPoint::GetLowLimit() const
{
	return m_dlowlimit;
}

void AirWayPoint::SetLatLong( const CLatitude& latitude , const CLongitude& longtitude )
{
	m_lat = latitude;
	m_long = longtitude;
}

bool AirWayPoint::CopyData(const  ALTObject* pObj )
{
	if(this == pObj)
		return true;

	if(pObj->GetType() == GetType() )
	{
		int nID = getID();
		GUID guid = getGuid();
		ALTObjectDisplayProp* pDisp = m_pDispProp;
			
		*this = *((AirWayPoint*)pObj);

		m_pDispProp = pDisp;
		m_guid  = getGuid();
		m_nObjID = nID;		
	}
	return __super::CopyData(pObj);
}

const GUID& AirWayPoint::getTypeGUID()
{

	static const GUID class_guid = 
	{ 0x72f8e5f4, 0xf38d, 0x4340, { 0xba, 0xec, 0x1a, 0xcf, 0x9d, 0x61, 0x7f, 0x29 } };
	return class_guid;
}

ALTObjectDisplayProp* AirWayPoint::NewDisplayProp()
{
	return new AirWayPointDisplayProp();
}
