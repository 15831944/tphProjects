#include "StdAfx.h"
#include ".\altairport.h"
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
#include "AirsidePaxBusParkSpot.h"


ALTAirport::ALTAirport(void)
:m_guid(CGuid::Create())
{
	m_strName = "Airport_New";
	m_refpoint = CPoint2008(0.0,0.0,0.0);
	m_lLongitude = 0;
	m_lLatitude = 0;
	m_dElevation = 0.0;
	m_nAirportID = -1;
}

ALTAirport::ALTAirport( const ALTAirport& otherARP )
{
	//memcpy(this,&otherARP,sizeof(ALTAirport));
	*this = otherARP;
}
ALTAirport::~ALTAirport(void)
{
}
//void ALTAirport::setLongtitude(const CString& strLong)
//{
//
//}

CString ALTAirport::getLongtitude() const
{
	long lLongitude = m_lLongitude;
	CString strLongtitude = _T("");
	CString nDir = _T("");
	if(m_lLongitude >=0)
		nDir += "E";
	else
	{
		nDir +="W";
		lLongitude = abs(m_lLongitude);
	}

	int nDgree = static_cast<int>(lLongitude/(60*60));
	int nleft = lLongitude-nDgree*60*60;
	
	int nMin = static_cast<int>(nleft/60);	
	int nSec = nleft - nMin*60;

	strLongtitude.Format(_T("%s %d:%d:%d"),nDir,nDgree,nMin,nSec);

	return strLongtitude;

}
//void ALTAirport::setLatitude(const CString& strLat)
//{
//
//}

CString ALTAirport::getLatitude() const
{
	long lLatitude = m_lLatitude;

	CString strLatitude = _T("");
	CString nDir = _T("");

	if(m_lLatitude >=0)
		nDir += "N";
	else
	{
		nDir +="S";
		lLatitude = abs(m_lLatitude);
	}

	int nDgree = static_cast<int>(lLatitude/(60*60));
	int nleft = lLatitude-nDgree*60*60;

	int nMin = static_cast<int>(nleft/60);

	int nSec = nleft - nMin*60;

	strLatitude.Format(_T("%s %d:%d:%d"),nDir,nDgree,nMin,nSec);

	return strLatitude;

}

void ALTAirport::NewAirport(int nProjID)
{
	SaveAirport(nProjID);
}
//PROJID, NAME, LONGITUDE, LATITUTE, ELEVATION, REFPOINTID, POINTCOUNT, \
//REFPOINT, TSTAMP
void ALTAirport::ReadAirport(int nAirportID)
{
	CADORecordset adoRecordset;
	long nRecordCount = -1;
	CADODatabase::ExecuteSQLStatement(GetSelectScript(nAirportID),nRecordCount,adoRecordset);

	if (!adoRecordset.IsEOF())
	{
		m_nAirportID = nAirportID;
		ReadRecord(adoRecordset);
	}

}

void ALTAirport::ReadRecord(CADORecordset& adoRecordset)
{
	adoRecordset.GetFieldValue(_T("NAME"),m_strName);
	adoRecordset.GetFieldValue(_T("LONGITUDE"),m_lLongitude);
	adoRecordset.GetFieldValue(_T("LATITUTE"),m_lLatitude);
	adoRecordset.GetFieldValue(_T("ELEVATION"),m_dElevation);
	double dX,dY,dZ;
	adoRecordset.GetFieldValue(_T("REFPOINTX"),dX);
	adoRecordset.GetFieldValue(_T("REFPOINTY"),dY);
	adoRecordset.GetFieldValue(_T("REFPOINTZ"),dZ);
	m_refpoint.setPoint(dX,dY,dZ);
	adoRecordset.GetFieldValue(_T("MAGVARVAL"),m_magVar.m_dval);
	adoRecordset.GetFieldValue(_T("MAGVARDIR"),m_magVar.m_iEast_West);
}
void ALTAirport::UpdateAirport(int nAirportID)
{
	CADODatabase::ExecuteSQLStatement(GetUpdateScript(nAirportID));

}
void ALTAirport::SaveAirport(int nProjID)
{

	//CADODatabase::ExecuteSQLStatement(GetInsertScript(nProjID));
	try
	{	
		long nRecordCount = -1;
		
		CADODatabase::ExecuteSQLStatement(GetInsertScript(nProjID));
	
		CADORecordset idRecordset;	
		CString strSQLID = _T("SELECT @@Identity AS id");
		CADODatabase::ExecuteSQLStatement(strSQLID,nRecordCount,idRecordset);

		if (!idRecordset.IsEOF())
		{
			ALTAirport airport;
			idRecordset.GetFieldValue(_T("ID"),m_nAirportID);
		}
	}
	catch (CADOException& e)
	{
		e.ErrorMessage();
	}


}
//void ALTAirport::DeleteAirport(int nAirportID)
//{	
//	CADODatabase::ExecuteSQLStatement(GetDeleteScript(nAirportID));
//}
void ALTAirport::DeleteAirport(int nAirportID)
{
	//delete object
	ALTAirport::DeleteAllObject(nAirportID);

	CString strSQL = _T("");
	strSQL.Format(_T("DELETE FROM ALTAIRPORT\
					 WHERE ID = %d"),nAirportID);

	CADODatabase::ExecuteSQLStatement(strSQL);
}
CString ALTAirport::GetSelectScript(int nAirportID) const
{
	CString strSQL = _T("");
	strSQL.Format(_T("SELECT NAME, LONGITUDE, LATITUTE, ELEVATION, REFPOINTX, REFPOINTY,REFPOINTZ,MAGVARVAL,MAGVARDIR\
		FROM ALTAIRPORT\
		WHERE ID = %d"),nAirportID);
	return strSQL;

}
CString ALTAirport::GetInsertScript(int nProjID) const
{
	CString strSQL = _T("");
	strSQL.Format(_T("INSERT INTO ALTAIRPORT\
		(PROJID, NAME, LONGITUDE, LATITUTE, ELEVATION, REFPOINTX,REFPOINTY,REFPOINTZ,MAGVARVAL,MAGVARDIR)\
		VALUES (%d,'%s',%d,%d,%f,%f,%f,%f,%f,%d)"),
		nProjID,m_strName.c_str(),m_lLongitude,m_lLatitude,m_dElevation,
		m_refpoint.getX(),m_refpoint.getY(),m_refpoint.getZ(), m_magVar.m_dval,m_magVar.m_iEast_West);
	return strSQL;
}
CString ALTAirport::GetUpdateScript(int nAirportID) const
{
	CString strSQL = _T("");
	strSQL.Format(_T("UPDATE ALTAIRPORT \
					 SET NAME = '%s', LONGITUDE = %d, LATITUTE =%d, ELEVATION =%f,\
					 REFPOINTX =%f, REFPOINTY = %f ,REFPOINTZ = %f, MAGVARVAL = %f, MAGVARDIR = %d\
					 WHERE ID = %d"),
		m_strName.c_str(),m_lLongitude,m_lLatitude,m_dElevation,
		m_refpoint.getX(),m_refpoint.getY(),m_refpoint.getZ(),m_magVar.m_dval, m_magVar.m_iEast_West, nAirportID);
	return strSQL;
}
CString ALTAirport::GetDeleteScript(int nAirportID) const
{

	CString strSQL = _T("");
	strSQL.Format(_T("DELETE FROM ALTAIRPORT\
		WHERE ID = %d"),nAirportID);

	return strSQL;
}
void ALTAirport::UpdateName(const CString& strName, int nAirportID)
{
	CString strSQL = _T("");
	strSQL.Format(_T("UPDATE ALTAIRPORT SET NAME = '%s' WHERE ID = %d"),strName,nAirportID);
	CADODatabase::ExecuteSQLStatement(strSQL);
}

//////////////////////////////////////////////////////////////////////////
//static function
void ALTAirport::GetRunwaysIDs(int nAirportID, std::vector<int>& vRunwayID)
{
	CString strSQL = _T("");
	strSQL.Format("SELECT ID FROM RUNWAY_VIEW WHERE APTID = %d",nAirportID);

	try
	{
		long nRecordCount;
		CADORecordset adoRs;
		CADODatabase::ExecuteSQLStatement(strSQL,nRecordCount,adoRs);

		while (!adoRs.IsEOF())
		{
			int nID = -1;
			adoRs.GetFieldValue(_T("ID"),nID);
			vRunwayID.push_back(nID);

			adoRs.MoveNextData();
		}
	}
	catch (CADOException& e)
	{
		e.ErrorMessage();
	}
}

void ALTAirport::GetContoursIDs(int nAirportID, std::vector<int>& vContourID)
{
	CString strSQL = _T("");
	strSQL.Format("SELECT ID FROM CONTOUR_VIEW WHERE APTID = %d",nAirportID);

	try
	{
		long nRecordCount;
		CADORecordset adoRs;
		CADODatabase::ExecuteSQLStatement(strSQL,nRecordCount,adoRs);

		while (!adoRs.IsEOF())
		{
			int nID = -1;
			adoRs.GetFieldValue(_T("ID"),nID);
			vContourID.push_back(nID);

			adoRs.MoveNextData();
		}
	}
	catch (CADOException& e)
	{
		e.ErrorMessage();
	}
}

//get the root contour ID
void ALTAirport::GetRootContoursIDs(int nAirportID, std::vector<int>& vContourID)
{
	CString strSQL = _T("");
	strSQL.Format("SELECT ID FROM CONTOUR_VIEW WHERE APTID = %d AND PARENTID = %d",nAirportID, -1);

	try
	{
		long nRecordCount;
		CADORecordset adoRs;
		CADODatabase::ExecuteSQLStatement(strSQL,nRecordCount,adoRs);

		while (!adoRs.IsEOF())
		{
			int nID = -1;
			adoRs.GetFieldValue(_T("ID"),nID);
			vContourID.push_back(nID);

			adoRs.MoveNextData();
		}
	}
	catch (CADOException& e)
	{
		e.ErrorMessage();
	}
}

void ALTAirport::GetGroundRouteIDs(int nAirportID, std::vector<int>& vGroundRouteID)
{
	CString strSQL = _T("");
	strSQL.Format("SELECT ID FROM ALTOBJECT WHERE APTID = %d AND TYPE = %d",nAirportID, (int)ALT_GROUNDROUTE);

	try
	{
		long nRecordCount;
		CADORecordset adoRs;
		CADODatabase::ExecuteSQLStatement(strSQL,nRecordCount,adoRs);

		while (!adoRs.IsEOF())
		{
			int nID = -1;
			adoRs.GetFieldValue(_T("ID"),nID);
			vGroundRouteID.push_back(nID);

			adoRs.MoveNextData();
		}
	}
	catch (CADOException& e)
	{
		e.ErrorMessage();
	}
}

void ALTAirport::GetAirportList(int nProjID,std::vector<ALTAirport>& vAirport)
{
	CString strSQL = _T("");
	strSQL.Format(_T("SELECT ID, NAME, LONGITUDE, LATITUTE, ELEVATION, REFPOINTX, REFPOINTY, \
		REFPOINTZ, TSTAMP, MAGVARVAL, MAGVARDIR\
		FROM ALTAIRPORT\
		WHERE PROJID = %d"),nProjID);

	//try
	{
		long nRecordCount;
		CADORecordset adoRs;
		CADODatabase::ExecuteSQLStatement(strSQL,nRecordCount,adoRs);

		while (!adoRs.IsEOF())
		{
			ALTAirport airport;
			int nID = -1;
			adoRs.GetFieldValue(_T("ID"),nID);
			airport.setID(nID);
			airport.ReadRecord(adoRs);

			vAirport.push_back(airport);
			adoRs.MoveNextData();
		}
	}
	//catch (CADOException& e)
	//{
	//	e.ErrorMessage();
	//}

}
void ALTAirport::GetDeicePadsIDs( int nAirportID, std::vector<int>& vDeicePadList )
{
	CString strSQL = _T("");
	strSQL.Format("SELECT ID FROM DEICEPAD_VIEW WHERE APTID = %d",nAirportID);

	try
	{
		long nRecordCount;
		CADORecordset adoRs;
		CADODatabase::ExecuteSQLStatement(strSQL,nRecordCount,adoRs);

		while (!adoRs.IsEOF())
		{
			int nID = -1;
			adoRs.GetFieldValue(_T("ID"),nID);
			vDeicePadList.push_back(nID);

			adoRs.MoveNextData();
		}
	}
	catch (CADOException& e)
	{
		e.ErrorMessage();
	}
}

void ALTAirport::GetDeicePadsAsStandsIDs(int nAirportID, std::vector<int>& vDeicePadAsStandList)
{
	CString strSQL = _T("");
	bool bAsStand = true;
	strSQL.Format("SELECT ID FROM DEICEPAD_VIEW WHERE (APTID = %d) AND (ASSTAND = %d)",nAirportID, bAsStand?1:0);

	try
	{
		long nRecordCount;
		CADORecordset adoRs;
		CADODatabase::ExecuteSQLStatement(strSQL,nRecordCount,adoRs);

		while (!adoRs.IsEOF())
		{
			int nID = -1;
			adoRs.GetFieldValue(_T("ID"),nID);
			vDeicePadAsStandList.push_back(nID);

			adoRs.MoveNextData();
		}
	}
	catch (CADOException& e)
	{
		e.ErrorMessage();
	}
}

void ALTAirport::GetDeicePadsNotAsStandsIDs(int nAirportID, std::vector<int>& vDeicePadList)
{
	CString strSQL = _T("");
	bool bAsStand = false;
	strSQL.Format("SELECT ID FROM DEICEPAD_VIEW WHERE (APTID = %d) AND (ASSTAND = %d)",nAirportID, bAsStand?1:0);

	try
	{
		long nRecordCount;
		CADORecordset adoRs;
		CADODatabase::ExecuteSQLStatement(strSQL,nRecordCount,adoRs);

		while (!adoRs.IsEOF())
		{
			int nID = -1;
			adoRs.GetFieldValue(_T("ID"),nID);
			vDeicePadList.push_back(nID);

			adoRs.MoveNextData();
		}
	}
	catch (CADOException& e)
	{
		e.ErrorMessage();
	}
}

void ALTAirport::GetTaxiwaysIDs( int nAirportID, std::vector<int>& vTaxiwayID )
{
	CString strSQL = _T("");
	strSQL.Format("SELECT ID FROM TAXIWAY_VIEW WHERE APTID = %d",nAirportID);

	try
	{
		long nRecordCount;
		CADORecordset adoRs;
		CADODatabase::ExecuteSQLStatement(strSQL,nRecordCount,adoRs);

		while (!adoRs.IsEOF())
		{
			int nID = -1;
			adoRs.GetFieldValue(_T("ID"),nID);
			vTaxiwayID.push_back(nID);

			adoRs.MoveNextData();
		}
	}
	catch (CADOException& e)
	{
		e.ErrorMessage();
	}
}

void ALTAirport::GetNoseInParkingsIDs(int nAirportID,std::vector<int>& vrNoseInParkingID)
{
	CString strSQL = _T("");
	strSQL.Format("SELECT ID FROM ALTOBJECT WHERE APTID = %d AND TYPE = %d",nAirportID,(int)ALT_NOSEINPARKING);

	try
	{
		long nRecordCount;
		CADORecordset adoRs;
		CADODatabase::ExecuteSQLStatement(strSQL,nRecordCount,adoRs);

		while (!adoRs.IsEOF())
		{
			int nID = -1;
			adoRs.GetFieldValue(_T("ID"),nID);
			vrNoseInParkingID.push_back(nID);

			adoRs.MoveNextData();
		}
	}
	catch (CADOException& e)
	{
		e.ErrorMessage();
	}
}

void ALTAirport::GetYieldSignsIDs(int nAirportID,std::vector<int>& vrYieldSignID)
{
	CString strSQL = _T("");
	strSQL.Format("SELECT ID FROM ALTOBJECT WHERE APTID = %d AND TYPE = %d",nAirportID,(int)ALT_YIELDSIGN);

	try
	{
		long nRecordCount;
		CADORecordset adoRs;
		CADODatabase::ExecuteSQLStatement(strSQL,nRecordCount,adoRs);

		while (!adoRs.IsEOF())
		{
			int nID = -1;
			adoRs.GetFieldValue(_T("ID"),nID);
			vrYieldSignID.push_back(nID);

			adoRs.MoveNextData();
		}
	}
	catch (CADOException& e)
	{
		e.ErrorMessage();
	}
}

void ALTAirport::GetStopSignsIDs(int nAirportID,std::vector<int>& vrStopSignID)
{
	CString strSQL = _T("");
	strSQL.Format("SELECT ID FROM ALTOBJECT WHERE APTID = %d AND TYPE = %d",nAirportID,(int)ALT_STOPSIGN);

	try
	{
		long nRecordCount;
		CADORecordset adoRs;
		CADODatabase::ExecuteSQLStatement(strSQL,nRecordCount,adoRs);

		while (!adoRs.IsEOF())
		{
			int nID = -1;
			adoRs.GetFieldValue(_T("ID"),nID);
			vrStopSignID.push_back(nID);

			adoRs.MoveNextData();
		}
	}
	catch (CADOException& e)
	{
		e.ErrorMessage();
	}
}

void ALTAirport::GetTollGatesIDs(int nAirportID,std::vector<int>& vrTollGateID)
{
	CString strSQL = _T("");
	strSQL.Format("SELECT ID FROM ALTOBJECT WHERE APTID = %d AND TYPE = %d",nAirportID,(int)ALT_TOLLGATE);

	try
	{
		long nRecordCount;
		CADORecordset adoRs;
		CADODatabase::ExecuteSQLStatement(strSQL,nRecordCount,adoRs);

		while (!adoRs.IsEOF())
		{
			int nID = -1;
			adoRs.GetFieldValue(_T("ID"),nID);
			vrTollGateID.push_back(nID);

			adoRs.MoveNextData();
		}
	}
	catch (CADOException& e)
	{
		e.ErrorMessage();
	}
}

void ALTAirport::GetTrafficLightsIDs(int nAirportID,std::vector<int>& vrTrafficLightID)
{
	CString strSQL = _T("");
	strSQL.Format("SELECT ID FROM ALTOBJECT WHERE APTID = %d AND TYPE = %d",nAirportID,(int)ALT_TRAFFICLIGHT);

	try
	{
		long nRecordCount;
		CADORecordset adoRs;
		CADODatabase::ExecuteSQLStatement(strSQL,nRecordCount,adoRs);

		while (!adoRs.IsEOF())
		{
			int nID = -1;
			adoRs.GetFieldValue(_T("ID"),nID);
			vrTrafficLightID.push_back(nID);

			adoRs.MoveNextData();
		}
	}
	catch (CADOException& e)
	{
		e.ErrorMessage();
	}
}

void ALTAirport::GetLineParkingsIDs(int nAirportID,std::vector<int>& vrLineParkingID)
{
	CString strSQL = _T("");
	strSQL.Format("SELECT ID FROM ALTOBJECT WHERE APTID = %d AND TYPE = %d",nAirportID,(int)ALT_LINEPARKING);

	try
	{
		long nRecordCount;
		CADORecordset adoRs;
		CADODatabase::ExecuteSQLStatement(strSQL,nRecordCount,adoRs);

		while (!adoRs.IsEOF())
		{
			int nID = -1;
			adoRs.GetFieldValue(_T("ID"),nID);
			vrLineParkingID.push_back(nID);

			adoRs.MoveNextData();
		}
	}
	catch (CADOException& e)
	{
		e.ErrorMessage();
	}
}

void ALTAirport::GetLaneAdaptersIDs(int nAirportID,std::vector<int>& vrLaneAdapterID)
{
	CString strSQL = _T("");
	strSQL.Format("SELECT ID FROM ALTOBJECT WHERE APTID = %d AND TYPE = %d",nAirportID,(int)ALT_LANEADAPTER);

	try
	{
		long nRecordCount;
		CADORecordset adoRs;
		CADODatabase::ExecuteSQLStatement(strSQL,nRecordCount,adoRs);

		while (!adoRs.IsEOF())
		{
			int nID = -1;
			adoRs.GetFieldValue(_T("ID"),nID);
			vrLaneAdapterID.push_back(nID);

			adoRs.MoveNextData();
		}
	}
	catch (CADOException& e)
	{
		e.ErrorMessage();
	}
}

void ALTAirport::GetTurnoffsIDs(int nAirportID,std::vector<int>& vrTurnoffID)
{
	CString strSQL = _T("");
	strSQL.Format("SELECT ID FROM ALTOBJECT WHERE APTID = %d AND TYPE = %d",nAirportID,(int)ALT_TURNOFF);

	try
	{
		long nRecordCount;
		CADORecordset adoRs;
		CADODatabase::ExecuteSQLStatement(strSQL,nRecordCount,adoRs);

		while (!adoRs.IsEOF())
		{
			int nID = -1;
			adoRs.GetFieldValue(_T("ID"),nID);
			vrTurnoffID.push_back(nID);

			adoRs.MoveNextData();
		}
	}
	catch (CADOException& e)
	{
		e.ErrorMessage();
	}
}

void ALTAirport::GetRoundaboutsIDs(int nAirportID,std::vector<int>& vrRoundaboutID)
{
	CString strSQL = _T("");
	strSQL.Format("SELECT ID FROM ALTOBJECT WHERE APTID = %d AND TYPE = %d",nAirportID,(int)ALT_ROUNDABOUT);

	try
	{
		long nRecordCount;
		CADORecordset adoRs;
		CADODatabase::ExecuteSQLStatement(strSQL,nRecordCount,adoRs);

		while (!adoRs.IsEOF())
		{
			int nID = -1;
			adoRs.GetFieldValue(_T("ID"),nID);
			vrRoundaboutID.push_back(nID);

			adoRs.MoveNextData();
		}
	}
	catch (CADOException& e)
	{
		e.ErrorMessage();
	}
}

void ALTAirport::GetRoadIntersectionsIDs(int nAirportID,std::vector<int>& vrRoadIntersectionsID)
{
	CString strSQL = _T("");
	strSQL.Format("SELECT ID FROM ALTOBJECT WHERE APTID = %d AND TYPE = %d",nAirportID,(int)ALT_INTERSECTIONS);

	try
	{
		long nRecordCount;
		CADORecordset adoRs;
		CADODatabase::ExecuteSQLStatement(strSQL,nRecordCount,adoRs);

		while (!adoRs.IsEOF())
		{
			int nID = -1;
			adoRs.GetFieldValue(_T("ID"),nID);
			vrRoadIntersectionsID.push_back(nID);

			adoRs.MoveNextData();
		}
	}
	catch (CADOException& e)
	{
		e.ErrorMessage();
	}
}

void ALTAirport::GetHeliportsIDs(int nAirportID,std::vector<int>& vrHeliport)
{
	CString strSQL = _T("");
	strSQL.Format("SELECT ID FROM ALTOBJECT WHERE APTID = %d AND TYPE = %d",nAirportID,(int)ALT_HELIPORT);

	try
	{
		long nRecordCount;
		CADORecordset adoRs;
		CADODatabase::ExecuteSQLStatement(strSQL,nRecordCount,adoRs);
		while (!adoRs.IsEOF())
		{
			int nID = -1;
			adoRs.GetFieldValue(_T("ID"),nID);
			vrHeliport.push_back(nID);
			adoRs.MoveNextData();
		}
	}
	catch (CADOException& e)
	{
		e.ErrorMessage();
	}
}

void ALTAirport::GetStretchsIDs(int nAirportID,std::vector<int>& vrStretchID)
{
	CString strSQL = _T("");
	strSQL.Format("SELECT ID FROM ALTOBJECT WHERE APTID = %d AND TYPE = %d",nAirportID,(int)ALT_STRETCH);

	try
	{
		long nRecordCount;
		CADORecordset adoRs;
		CADODatabase::ExecuteSQLStatement(strSQL,nRecordCount,adoRs);

		while (!adoRs.IsEOF())
		{
			int nID = -1;
			adoRs.GetFieldValue(_T("ID"),nID);
			vrStretchID.push_back(nID);

			adoRs.MoveNextData();
		}
	}
	catch (CADOException& e)
	{
		e.ErrorMessage();
	}
}

void ALTAirport::GetCircleStretchsIDs(int nAirportID,std::vector<int>& vrCircleStretchID)
{
	CString strSQL = _T("");
	strSQL.Format("SELECT ID FROM ALTOBJECT WHERE APTID = %d AND TYPE = %d",nAirportID,(int)ALT_CIRCLESTRETCH);

	try
	{
		long nRecordCount;
		CADORecordset adoRs;
		CADODatabase::ExecuteSQLStatement(strSQL,nRecordCount,adoRs);

		while (!adoRs.IsEOF())
		{
			int nID = -1;
			adoRs.GetFieldValue(_T("ID"),nID);
			vrCircleStretchID.push_back(nID);

			adoRs.MoveNextData();
		}
	}
	catch (CADOException& e)
	{
		e.ErrorMessage();
	}
}

void ALTAirport::GetReportingAreaList(int nAirportID, ALTObjectList& vObjectList)
{
	std::vector<int> vAreas;
	GetReportingAreaIDs(nAirportID,vAreas);
	int nCount = (int)vAreas.size();
	for(int i =0 ;i < nCount; ++i)
	{
		CReportingArea * pArea = new CReportingArea();
		pArea->ReadObject(vAreas.at(i));
		vObjectList.push_back(pArea);
	}
}

void ALTAirport::GetReportingAreaIDs(int nAirportID, std::vector<int>& vAreaID)
{
	CString strSQL = _T("");
	strSQL.Format("SELECT ID FROM ALTOBJECT WHERE APTID = %d AND TYPE = %d",nAirportID,(int)ALT_REPORTINGAREA);

	try
	{
		long nRecordCount;
		CADORecordset adoRs;
		CADODatabase::ExecuteSQLStatement(strSQL,nRecordCount,adoRs);

		while (!adoRs.IsEOF())
		{
			int nID = -1;
			adoRs.GetFieldValue(_T("ID"),nID);
			vAreaID.push_back(nID);

			adoRs.MoveNextData();
		}
	}
	catch (CADOException& e)
	{
		e.ErrorMessage();
	}
}

void ALTAirport::GetVehiclePoolParkingIDs(int nAirportID,std::vector<int>& vrVehiclePoolParkingID)
{
	CString strSQL = _T("");
	strSQL.Format("SELECT ID FROM ALTOBJECT WHERE APTID = %d AND TYPE = %d",nAirportID,(int)ALT_VEHICLEPOOLPARKING);

	try
	{
		long nRecordCount;
		CADORecordset adoRs;
		CADODatabase::ExecuteSQLStatement(strSQL,nRecordCount,adoRs);

		while (!adoRs.IsEOF())
		{
			int nID = -1;
			adoRs.GetFieldValue(_T("ID"),nID);
			vrVehiclePoolParkingID.push_back(nID);

			adoRs.MoveNextData();
		}
	}
	catch (CADOException& e)
	{
		e.ErrorMessage();
	}
}

void ALTAirport::GetParkingPlaceIDs(int nAirportID,std::vector<int>& vParkingPlaceIDs)
{
	CString strSQL = _T("");
	strSQL.Format("SELECT ID FROM ALTOBJECT WHERE APTID = %d AND TYPE = %d",nAirportID,(int)ALT_PARKINGPLACE);

	try
	{
		long nRecordCount;
		CADORecordset adoRs;
		CADODatabase::ExecuteSQLStatement(strSQL,nRecordCount,adoRs);

		while (!adoRs.IsEOF())
		{
			int nID = -1;
			adoRs.GetFieldValue(_T("ID"),nID);
			vParkingPlaceIDs.push_back(nID);

			adoRs.MoveNextData();
		}
	}
	catch (CADOException& e)
	{
		e.ErrorMessage();
	}
}

void ALTAirport::GetStandsIDs( int nAirportID, std::vector<int>& vStandList )
{
	CString strSQL = _T("");
	strSQL.Format("SELECT ID FROM STAND_VIEW WHERE APTID = %d",nAirportID);

	try
	{
		long nRecordCount;
		CADORecordset adoRs;
		CADODatabase::ExecuteSQLStatement(strSQL,nRecordCount,adoRs);

		while (!adoRs.IsEOF())
		{
			int nID = -1;
			adoRs.GetFieldValue(_T("ID"),nID);
			vStandList.push_back(nID);

			adoRs.MoveNextData();
		}
	}
	catch (CADOException& e)
	{
		e.ErrorMessage();
	}
}
int ALTAirport::GetProjectIDByName(const CString& strModelName)
{

	int nProjID = -1;

	CString strSQL = _T("");

	strSQL.Format(_T("SELECT ID	FROM PROJECT WHERE (NAME = '%s')"),strModelName);
	try
	{
		long nRecordCount;
		CADORecordset adoRs;
		CADODatabase::ExecuteSQLStatement(strSQL,nRecordCount,adoRs);

		if (!adoRs.IsEOF())
		{
			ALTAirport airport;
			adoRs.GetFieldValue(_T("ID"),nProjID);
		}
	}
	catch (CADOException& e)
	{
		e.ErrorMessage();
	}

	return nProjID;
}

int ALTAirport::InitializeAirside(const CString& strModelName, int& nAirportID)
{
	int nProjID = GetProjectIDByName(strModelName);
	if (nProjID == -1) //add project into database
	{
		nProjID = InputAirside::GetProjectID(strModelName);
		//Insert project info
		//nProjID = NewProject(strModelName);
		//insert airport (airport 1)
		if(nProjID == -1)
			return 0;
		nAirportID = InitializeDefaultAirport(nProjID);

		return 1;
		//insert Floor information
	}
	else
	{
		std::vector<ALTAirport> vectAirport;
		ALTAirport::GetAirportList(nProjID, vectAirport);
		if(vectAirport.size() > 0)
			nAirportID = vectAirport[0].getID();
		else
		{
		   nAirportID = InitializeDefaultAirport(nProjID);
		   return 1;
		}
	}

	return 0;
}

int ALTAirport::NewProject(const CString& strModelName)
{
	int nProjID = -1;


	CString strSQL = _T("");
	CString strDesc = _T("");

	strSQL.Format(_T("INSERT INTO PROJECT(NAME, DESCRIPTION)\
		VALUES ('%s','%s')"),strModelName,strDesc);
//SCOPE_IDENTITY_SQLSCRIPT
	long nRecordCount = -1;

	CADODatabase::ExecuteSQLStatement(strSQL);

	CString strSQLID =_T("SELECT @@Identity AS id");
	CADORecordset idRecordset;
	CADODatabase::ExecuteSQLStatement(strSQLID,nRecordCount,idRecordset);

	//adoRecordset.NextRecordset(idRecordset);

	idRecordset.GetFieldValue(_T("ID"),nProjID);

	return nProjID;

}

int ALTAirport::InitializeDefaultAirport(int nProjID)
{
	ALTAirport airport;
	airport.SaveAirport(nProjID);

	return airport.GetAirportID();
}
void ALTAirport::DeleteAllObject(int nAirportID)
{
	//delete runway
	std::vector<int> vRunwayID;
	ALTAirport::GetRunwaysIDs(nAirportID,vRunwayID);
	Runway runway;
	for (int i = 0; i < static_cast<int>(vRunwayID.size()); ++i)
	{
		runway.ReadObject(vRunwayID[i]);
		runway.DeleteObject(vRunwayID[i]);
	}

	//delete taxiway
	std::vector<int> vTaxiwayID;
	ALTAirport::GetTaxiwaysIDs(nAirportID,vTaxiwayID);
	Taxiway _taxiway;
	for (int i = 0; i < static_cast<int>(vTaxiwayID.size()); ++i)
	{
		_taxiway.ReadObject(vRunwayID[i]);
		_taxiway.DeleteObject(vTaxiwayID[i]);
	}

	//delete Deice pad
	std::vector<int> vDeicePadID;
	ALTAirport::GetDeicePadsIDs(nAirportID,vDeicePadID);
	DeicePad deicepad;
	for (int i = 0; i < static_cast<int>(vDeicePadID.size()); ++i)
	{
		deicepad.ReadObject(vRunwayID[i]);
		deicepad.DeleteObject(vDeicePadID[i]);
	}

	//delete stand
	std::vector<int> vStandID;
	ALTAirport::GetStandsIDs(nAirportID,vStandID);
	Stand stand;
	for (int i = 0; i < static_cast<int>(vStandID.size()); ++i)
	{
		stand.ReadObject(vRunwayID[i]);
		stand.DeleteObject(vStandID[i]);
	}
}

ARCVector3 ALTAirport::GetWorldLocation( long latitude, long longtitude, double elevation ) const
{
	CLatLongConvert convent;
	CLatitude lat(latitude);
	CLongitude longti(longtitude);

	convent.Set_LatLong_Origin(&lat,&longti,0,0);

	CLatitude localLat(m_lLatitude);
	CLongitude localLong(m_lLongitude);
	
	LatLong_Structure Conventd (&localLat,&localLong);
	float x_dist, y_dist,distance;
	convent.Distance_LatLong(&Conventd,x_dist,y_dist,distance);
	
	return ARCVector3(x_dist * 1852,y_dist*1852,0);
}

void ALTAirport::GetAirportsName( int nPrjID, std::map<int, CString>& nameMap )
{
	CString strSQL = _T("");
	strSQL.Format(_T("SELECT ID, NAME FROM ALTAIRPORT	 WHERE PROJID = %d"),nPrjID);

	//try
	{
		long nRecordCount;
		CADORecordset adoRs;
		CADODatabase::ExecuteSQLStatement(strSQL,nRecordCount,adoRs);

		while (!adoRs.IsEOF())
		{
		
			int nID = -1;
			CString strName;
			strName.GetBuffer(255);
			adoRs.GetFieldValue(_T("ID"),nID);
			adoRs.GetFieldValue(_T("NAME"),strName);

			if(nID>=0) nameMap[nID] = strName;
			adoRs.MoveNextData();
		}
	}
}

void ALTAirport::GetVehiclePoolParkingList(int nAirportID, ALTObjectList& vObjectList)
{
	std::vector<int> vrVehiclePoolParkings;
	GetVehiclePoolParkingIDs(nAirportID,vrVehiclePoolParkings);
	for(int i =0 ;i < (int)vrVehiclePoolParkings.size(); ++i)
	{
		VehiclePoolParking * pVehiclePoolParking = new VehiclePoolParking();
		pVehiclePoolParking->ReadObject(vrVehiclePoolParkings.at(i));
		vObjectList.push_back(pVehiclePoolParking);
	}
}

void ALTAirport::GetTaxiwayList( int nAirportID, ALTObjectList& vObjectList )
{
	std::vector<int> vTaxiways;
	GetTaxiwaysIDs(nAirportID,vTaxiways);
	for(int i =0 ;i < (int)vTaxiways.size(); ++i)
	{
		Taxiway * pTaxiway = new Taxiway();
		pTaxiway->ReadObject(vTaxiways.at(i));
		vObjectList.push_back(pTaxiway);
	}
}

void ALTAirport::GetRunwayList( int nAirportID, ALTObjectList& vObjectList )
{
	std::vector<int> vRunways;
	GetRunwaysIDs(nAirportID,vRunways);
	for(int i =0 ;i < (int)vRunways.size(); ++i)
	{
		Runway * pRunway = new Runway;
		pRunway->ReadObject(vRunways.at(i));
		vObjectList.push_back(pRunway);
	}
}

void ALTAirport::GetStandList( int nAirportID, ALTObjectList& vObjectList )
{
	std::vector<int> vStands;
	GetStandsIDs(nAirportID,vStands);
	for(int i=0; i< (int)vStands.size();++i)
	{
		Stand * pStand = new Stand;
		pStand->ReadObject(vStands.at(i));
		vObjectList.push_back(pStand);
	}

	std::vector<int> vDeicePads;
	GetDeicePadsAsStandsIDs(nAirportID,vDeicePads);
	for(int i=0; i<(int)vDeicePads.size(); ++i)
	{
		DeicePad * pDeicepad = new DeicePad;
		pDeicepad->ReadObject(vDeicePads.at(i));
		vObjectList.push_back(pDeicepad);
	}
	sortAllStandByName(vObjectList);
}

void ALTAirport::GetDeicePadList( int nAirportID, ALTObjectList& vObjectList )
{
	std::vector<int> vDeicePads;
	GetDeicePadsNotAsStandsIDs(nAirportID,vDeicePads);
	for(int i=0; i<(int)vDeicePads.size(); ++i)
	{
		DeicePad * pDeicepad = new DeicePad;
		pDeicepad->ReadObject(vDeicePads.at(i));
		vObjectList.push_back(pDeicepad);
	}
}
void ALTAirport::ExportAirports(CAirsideExportFile& exportFile,int nProject)
{
	//exportFile.getFile().writeField(_T("Airport Data"));
	//exportFile.getFile().writeLine();
	exportFile.getFile().writeField(_T("Airport Index,Airport Name, longituder, latitude, Elevation, reference point"));
	exportFile.getFile().writeLine();
	std::vector<ALTAirport> vAirport;
	ALTAirport::GetAirportList(nProject,vAirport);
	std::vector<ALTAirport>::iterator  iter = vAirport.begin();
	for (;iter != vAirport.end(); ++iter)
	{
		(*iter).ExportAirport(exportFile);
	}
}


void ALTAirport::ImportAirports(CAirsideImportFile& importFile)
{
//	importFile.getFile().skipLine();
	while (!importFile.getFile().isBlank())
	{
		ALTAirport airport;
		airport.ImportAirport(importFile);
		importFile.getFile().getLine();
	}

}
void ALTAirport::ImportAirport(CAirsideImportFile& importFile)
{
	int nOldID = -1;
	importFile.getFile().getInteger(nOldID);
	char szName[512];
	importFile.getFile().getField(szName,512);
	m_strName = szName;

	importFile.getFile().getInteger(m_lLongitude);
	importFile.getFile().getInteger(m_lLatitude);
	importFile.getFile().getFloat(m_dElevation);
	importFile.getFile().getFloat(m_magVar.m_dval);
	importFile.getFile().getPoint2008(m_refpoint);

	SaveAirport(importFile.getNewProjectID());

	importFile.AddAirportIndexMap(nOldID,m_nAirportID);
}
//std::string m_strName;
////reference info
//long m_lLongitude;    // degrees
//long m_lLatitude;     // degrees
//double m_dElevation;    // ft;
//
//CPoint2008 m_refpoint;     // refer point to airport stuff 
void ALTAirport::ExportAirport(CAirsideExportFile& exportFile)
{
	exportFile.AddAirportID(m_nAirportID);
	exportFile.getFile().writeInt(m_nAirportID);
	exportFile.getFile().writeField(m_strName.c_str());
	exportFile.getFile().writeInt(m_lLongitude);
	exportFile.getFile().writeInt(m_lLatitude);
	exportFile.getFile().writeDouble(m_dElevation);
	exportFile.getFile().writeDouble(m_magVar.m_dval);
	exportFile.getFile().writePoint2008(m_refpoint);
	exportFile.getFile().writeLine();
}

//ALT_RUNWAY ,
//ALT_TAXIWAY, 
//ALT_STAND, 
//ALT_DEICEBAY,
//ALT_GROUNDROUTE,
//ALT_WAYPOINT,
//ALT_HOLD,
//ALT_AIRROUTE,
//ALT_SECTOR,
//ALT_SURFACE,
//ALT_STRUCTURE,
//ALT_WALLSHAPE,
//ALT_CONTOUR,
void ALTAirport::ExportObjects(int nAirportID,CAirsideExportFile& exportFile)
{

	////export runway
	Runway::ExportRunways(nAirportID,exportFile);

	////export taxiway
	Taxiway::ExportTaxiways(nAirportID,exportFile);


	//export stand
	Stand::ExportStands(nAirportID,exportFile);

	//deice pad
	DeicePad::ExportDeicePads(nAirportID,exportFile);

	//ground route
	GroundRoute::ExportGroundRoutes(nAirportID,exportFile);

	//stretch
	Stretch::ExportStretch(nAirportID,exportFile);

	//road intersection
	Intersections::ExportIntersections(nAirportID,exportFile);

	//traffic light
	TrafficLight::ExportTrafficLight(nAirportID,exportFile);

	//toll gate
	TollGate::ExportTollGate(nAirportID,exportFile);

	//stop sign
	StopSign::ExportStopSign(nAirportID,exportFile);

	//yield sign
	YieldSign::ExportYieldSign(nAirportID,exportFile);

	//Heliport sign
	Heliport::ExportHeliport(nAirportID,exportFile);
}
//such as runway exit,intersection node,hold position
void ALTAirport::ExportMiscObjects(CAirsideImportExportManager* exportManager)
{
	CAirsideExportFile exportFileIntersec(exportManager,exportManager->GetFilePath(AS_RUNWAYINTERSECTION),"",exportManager->getVersion());
	IntersectionNodesInAirport::ExportIntersectionNodes(exportFileIntersec);

	CAirsideExportFile exportFileHoldPos(exportManager,exportManager->GetFilePath(AS_HOLDPOSITION),"",exportManager->getVersion());
	HoldPosition::ExportHoldPositions(exportFileHoldPos);

	CAirsideExportFile exportFileRunwayExit(exportManager,exportManager->GetFilePath(AS_RUNWAYEXIT),"",exportManager->getVersion());
	RunwayExit::ExportRuwayExits(exportFileRunwayExit);

}
void ALTAirport::ImportMiscObjects(CAirsideImportExportManager* importManager)
{
	if(importManager->getVersion() < 1009)
		return;

	CAirsideImportFile importFileIntersec(importManager,importManager->GetFilePath(AS_RUNWAYINTERSECTION),"",importManager->getVersion());
	IntersectionNodesInAirport::ImportIntersectionNodes(importFileIntersec);

	CAirsideImportFile importFileHoldPos(importManager,importManager->GetFilePath(AS_HOLDPOSITION),"",importManager->getVersion());
	HoldPosition::ImportHoldPositions(importFileHoldPos);

	CAirsideImportFile importFileRunwayExit(importManager,importManager->GetFilePath(AS_RUNWAYEXIT),"",importManager->getVersion());
	RunwayExit::ImportRunwayExits(importFileRunwayExit);

}

int ALTAirport::getID() const
{
	return m_nAirportID;
}

MagneticVariation& ALTAirport::GetMagnectVariation()
{ 
	return m_magVar; 
}

MagneticVariation ALTAirport::GetMagnectVariation() const
{ 
	return m_magVar;
}

CPoint2008 ALTAirport::GetLatLongPos( const CLatitude& latitude , const CLongitude& longtitude ) const
{
	CLatLongConvert convent;
	CLatitude clat;
	CLongitude cLong;
	clat.SetValue(getLatitude());
	cLong.SetValue(getLongtitude());

	CPoint2008 refPoint = getRefPoint() * SCALE_FACTOR;
	convent.Set_LatLong_Origin(&clat,&cLong,refPoint.getX(),refPoint.getY());

	LatLong_Structure Origin(&clat, &cLong);
	LatLong_Structure Convented((CLatitude*)&latitude,(CLongitude*)&longtitude);

	float xdist,ydist,dist;
	convent.Distance_LatLong(&Convented,xdist,ydist,dist);

	xdist = (float)ARCUnit::ConvertLength(xdist,ARCUnit::NM,ARCUnit::CM);
	ydist =  (float)ARCUnit::ConvertLength(ydist,ARCUnit::NM,ARCUnit::CM);
	CPoint2008 pos;
	pos.setPoint(refPoint.getX() + xdist,refPoint.getY()+ydist,pos.getZ());
	return pos;
}

CPoint2008 ALTAirport::getRefPoint() const
{
	return m_refpoint;
}

double ALTAirport::getElevation() const
{

	return m_dElevation;
}


void ALTAirport::GetPosLatLong( const CPoint2008& in_pt, CLatitude& out_latitude, CLongitude& out_longtitude ) const
{
	CLatLongConvert convent;
	CLatitude clat;
	CLongitude cLong;
	clat.SetValue(getLatitude());
	cLong.SetValue(getLongtitude());	
	CPoint2008 refPoint = getRefPoint() * SCALE_FACTOR;
	convent.Set_LatLong_Origin(&clat,&cLong,0,0);

	LatLong_Structure convented(&out_latitude,&out_longtitude);

	float deltaX = (float)(in_pt.getX() - refPoint.getX());
	float deltaY = (float)(in_pt.getY() - refPoint.getY());
	deltaX = (float)CARCLengthUnit::ConvertLength(AU_LENGTH_CENTIMETER,AU_LENGTH_NAUTICALMILE,deltaX);
	deltaY = (float)CARCLengthUnit::ConvertLength(AU_LENGTH_CENTIMETER,AU_LENGTH_NAUTICALMILE,deltaY);

	convent.Point_LatLong(deltaX,deltaY,&convented);

	
}


void ALTAirport::GetStartPositionIDs(int nAirportID, std::vector<int>& vStartPositionID)
{
	CString strSQL = _T("");
	strSQL.Format("SELECT ID FROM ALTOBJECT WHERE APTID = %d AND TYPE = %d",
		nAirportID, ALT_STARTPOSITION);

	try
	{
		long nRecordCount;
		CADORecordset adoRs;
		CADODatabase::ExecuteSQLStatement(strSQL,nRecordCount,adoRs);

		while (!adoRs.IsEOF())
		{
			int nID = -1;
			adoRs.GetFieldValue(_T("ID"),nID);
			vStartPositionID.push_back(nID);

			adoRs.MoveNextData();
		}
	}
	catch (CADOException& e)
	{
		e.ErrorMessage();
	}
}

void ALTAirport::GetStartPositionList( int nAirportID, ALTObjectList& vObjectList )
{
	std::vector<int> vStartPosIDs;
	GetStartPositionIDs(nAirportID,vStartPosIDs);
	size_t nCount = vStartPosIDs.size();
	for(size_t i =0 ;i < nCount; ++i)
	{
		CStartPosition* pStartPosition = new CStartPosition;
		pStartPosition->ReadObject(vStartPosIDs.at(i));
		vObjectList.push_back(pStartPosition);
	}
}

 void ALTAirport::GetAirsideResourceObject(int nAirportID, ALTObjectList& vObjectList,Airsdie_Resource_ID _ResourceTy) 
{
	switch(_ResourceTy)
	{
	case AIRSIDE_DEICEPAD:
		GetDeicePadList(nAirportID,vObjectList) ;
		break ;
	case AIRSIDE_REPORTING_AREA:
		GetReportingAreaList(nAirportID,vObjectList) ;
		break ;
	case AIRSIDE_RUNWAY:
		GetRunwayList(nAirportID,vObjectList) ;
		break ;
	case AIRSIDE_STAND:
		GetStandList(nAirportID,vObjectList) ;
		break ;
	case AIRSIDE_TAXIWAY:
		GetTaxiwayList(nAirportID,vObjectList) ;
		break ;
	default :
		break ;
	}
}

void ALTAirport::GetMeetingPointIDs(int nAirportID, std::vector<int>& vMeetingPointID)
{
	CString strSQL = _T("");
	strSQL.Format("SELECT ID FROM ALTOBJECT WHERE APTID = %d AND TYPE = %d",
		nAirportID, ALT_MEETINGPOINT);

	try
	{
		long nRecordCount;
		CADORecordset adoRs;
		CADODatabase::ExecuteSQLStatement(strSQL,nRecordCount,adoRs);

		while (!adoRs.IsEOF())
		{
			int nID = -1;
			adoRs.GetFieldValue(_T("ID"),nID);
			vMeetingPointID.push_back(nID);

			adoRs.MoveNextData();
		}
	}
	catch (CADOException& e)
	{
		e.ErrorMessage();
	}
}

void ALTAirport::GetMeetingPointList( int nAirportID, ALTObjectList& vObjectList )
{
	std::vector<int> vMeetingPoints;
	GetMeetingPointIDs(nAirportID,vMeetingPoints);
	int nCount = (int)vMeetingPoints.size();
	for(int i =0 ;i < nCount; ++i)
	{
		CMeetingPoint * pMeetingPoint = new CMeetingPoint;
		pMeetingPoint->ReadObject(vMeetingPoints.at(i));
		vObjectList.push_back(pMeetingPoint);
	}
}

const GUID& ALTAirport::getTypeGUID()
{
	// {1941EC43-A1AB-49d3-A7FA-D2819ECA25FA}
	static const GUID name = 
	{ 0x1941ec43, 0xa1ab, 0x49d3, { 0xa7, 0xfa, 0xd2, 0x81, 0x9e, 0xca, 0x25, 0xfa } };
	
	return name;
}

const CGuid& ALTAirport::getGuid() const
{
	return m_guid;
}

static bool StandCompare(ALTObject::RefPtr obj1, ALTObject::RefPtr obj2 )
{
	return (obj1->GetObjectName()< obj2->GetObjectName());
}

void ALTAirport::sortAllStandByName( ALTObjectList& vObjectList )
{
	std::sort( vObjectList.begin(), vObjectList.end(), StandCompare);
	return;
}

void ALTAirport::GetBagCartParkPosIDs( int nAirportID,std::vector<int>& vrSpotID )
{

	CString strSQL = _T("");
	strSQL.Format("SELECT ID FROM ALTOBJECT WHERE APTID = %d AND TYPE = %d",nAirportID,(int)ALT_ABAGCARTSPOT);

	try
	{
		long nRecordCount;
		CADORecordset adoRs;
		CADODatabase::ExecuteSQLStatement(strSQL,nRecordCount,adoRs);

		while (!adoRs.IsEOF())
		{
			int nID = -1;
			adoRs.GetFieldValue(_T("ID"),nID);
			vrSpotID.push_back(nID);

			adoRs.MoveNextData();
		}
	}
	catch (CADOException& e)
	{
		e.ErrorMessage();
	}
}

void ALTAirport::GetPaxBusParkPosIDs( int nAirportID,std::vector<int>& vrSpotID )
{

	CString strSQL = _T("");
	strSQL.Format("SELECT ID FROM ALTOBJECT WHERE APTID = %d AND TYPE = %d",nAirportID,(int)ALT_APAXBUSSPOT);

	try
	{
		long nRecordCount;
		CADORecordset adoRs;
		CADODatabase::ExecuteSQLStatement(strSQL,nRecordCount,adoRs);

		while (!adoRs.IsEOF())
		{
			int nID = -1;
			adoRs.GetFieldValue(_T("ID"),nID);
			vrSpotID.push_back(nID);

			adoRs.MoveNextData();
		}
	}
	catch (CADOException& e)
	{
		e.ErrorMessage();
	}
}

void ALTAirport::GetPaxBusParkSpotList( int nAirportID, ALTObjectList& vObjectList )
{
	std::vector<int> vParkSpots;
	GetPaxBusParkPosIDs(nAirportID,vParkSpots);
	for(int i =0 ;i < (int)vParkSpots.size(); ++i)
	{
		AirsidePaxBusParkSpot * pParkSpot = new AirsidePaxBusParkSpot();
		pParkSpot->ReadObject(vParkSpots.at(i));
		vObjectList.push_back(pParkSpot);
	}
}

void ALTAirport::UpdateRunwayExitsNames(int nAirportID)
{

	IntersectionNodesInAirport vAirportNodes;
	ALTObjectList vRunways;
	ALTObjectList vTaxiways;
	GetTaxiwayList(nAirportID, vTaxiways);
	GetRunwayList(nAirportID, vRunways);


	ALTObjectList vObjects  = vRunways;
	vObjects.insert(vObjects.end(), vTaxiways.begin(), vTaxiways.end());
	
	vAirportNodes.Init(nAirportID, vObjects );
	

	//update runway exits
	for(size_t i=0;i<vRunways.size();i++)
	{
		ALTObject* pObj = vRunways[i].get();
		if(pObj && pObj->GetType() == ALT_RUNWAY)
		{
			Runway * pRunway = (Runway*)pObj;
			pRunway->UpdateRunwayExits(vAirportNodes);
		}
	}	
}
