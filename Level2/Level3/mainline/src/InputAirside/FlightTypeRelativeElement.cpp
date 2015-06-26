#include "StdAfx.h"
#include "FlightTypeRelativeElement.h"
#include "../Database/ADODatabase.h"
#include "AirsideImportExportManager.h"
#include "AirsideImportExportManager.h"

FlightTypeRelativeElement::FlightTypeRelativeElement(void)
:m_nID(-1)
,m_nTypeID(-1)
,m_nFlightTypeDetailsItemID(-1)
{
}

FlightTypeRelativeElement::~FlightTypeRelativeElement(void)
{
}

void FlightTypeRelativeElement::SetID(int nID)
{
	m_nID = nID;
}

int FlightTypeRelativeElement::GetID()
{
	return m_nID;
}

void FlightTypeRelativeElement::SetTypeID(int nTypeID)
{
	m_nTypeID = nTypeID;
}

int FlightTypeRelativeElement::GetTypeID()
{
	return m_nTypeID;
}

void FlightTypeRelativeElement::SetServiceLocation(CPoint2008& point)
{
	m_location = point;
}

CPoint2008 FlightTypeRelativeElement::GetServiceLocation()
{
	return m_location;
}


void FlightTypeRelativeElement::ConvertPointIntoLocalCoordinate(const CPoint2008& pt,const double& dDegree)
{
	//meter ->centimeter

	m_location.setY(m_location.getY()*(-1));
	m_location.rotate(dDegree);

	m_location *= 100;
	m_location.DoOffset(pt.getX(),pt.getY(),pt.getZ());

}
void FlightTypeRelativeElement::SetFlightTypeDetailsItemID(int nID)
{
	m_nFlightTypeDetailsItemID = nID;
}

void FlightTypeRelativeElement::SaveData(int nFDID)
{
	CString strSQL;

	strSQL.Format(_T("INSERT INTO FlightTypeRelativeElement \
					 (FlightTypeDetailsItemID, ElementTypeID, location_x, location_y, location_z) \
					 VALUES (%d,%d,%f,%f,%f)"),
					 nFDID, m_nTypeID, m_location.getX(), m_location.getY(), m_location.getZ());

	m_nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);

	return;

}

void FlightTypeRelativeElement::UpdateData(int nFDID)
{
	CString strSQL = _T("");
	strSQL.Format(_T("UPDATE FlightTypeRelativeElement\
					 SET FlightTypeDetailsItemID =%d, ElementTypeID=%d,\
					 location_x=%f, location_y=%f,location_z=%f\
					 WHERE (ID = %d)"), nFDID, m_nTypeID, 
					 m_location.getX(), m_location.getY(), m_location.getZ(),m_nID);

	CADODatabase::ExecuteSQLStatement(strSQL);

}

void FlightTypeRelativeElement::DeleteData()
{
	CString strSQL = _T("");
	strSQL.Format(_T("DELETE  FROM FlightTypeRelativeElement\
					 WHERE (ID = %d)"), m_nID);

	CADODatabase::ExecuteSQLStatement(strSQL);
}


void FlightTypeRelativeElement::ExportData(CAirsideExportFile& exportFile, int nFltTypeDetailsItemID)
{
	exportFile.getFile().writeInt(m_nID);
	exportFile.getFile().writeInt(nFltTypeDetailsItemID);
	exportFile.getFile().writeInt(m_nTypeID);

	exportFile.getFile().writePoint2008(m_location);

}

void FlightTypeRelativeElement::ImportData(CAirsideImportFile& importFile,int nFltTypeDetailsItemID)
{
	int nOldID = -1;
	importFile.getFile().getInteger(nOldID);
	importFile.getFile().getInteger(m_nFlightTypeDetailsItemID);
	m_nFlightTypeDetailsItemID = nFltTypeDetailsItemID;

	importFile.getFile().getInteger(m_nTypeID);

	importFile.getFile().getPoint2008(m_location);
	
	m_nTypeID = importFile.GetVehicleDataSpecificationNewID(m_nTypeID);

	SaveData(nFltTypeDetailsItemID);
}









