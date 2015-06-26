// AircraftDispPropItem.cpp: implementation of the CAircraftDispPropItem class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "VehicleDispPropItem.h"
#include "../Database/ADODatabase.h"
#include "..\Database\DBElementCollection_Impl.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CVehicleDispPropItem::CVehicleDispPropItem(int nProjID):DBElement()
{	
	m_nProjID = nProjID;
	m_nParentID = -1;
	m_bVisible = TRUE;
	m_shape =  std::make_pair(6, 0);//set 2 is vehicle ,see plugins project
	m_linetype = ENUM_LINE_1PTSOLID;
	m_colorRef = RGB( 32, 32, 32 );
	m_bIsLeaveTrail = TRUE;
	m_dDensity = 1.0; 
}

CVehicleDispPropItem::~CVehicleDispPropItem()
{

}

void CVehicleDispPropItem::InitFromDBRecordset(CADORecordset& adoRecordset)
{
	adoRecordset.GetFieldValue(_T("ID"),m_nID);
	adoRecordset.GetFieldValue(_T("PROJID"),m_nProjID);
	adoRecordset.GetFieldValue(_T("PARENTID"),m_nParentID);

	int nValue = -1;
	adoRecordset.GetFieldValue(_T("VEHICLEID"),nValue);
	m_vehicleItem.ReadData(nValue);

	adoRecordset.GetFieldValue(_T("VISIBLE"),nValue);
	m_bVisible = (BOOL)nValue;

	int nFirst = 0;
	adoRecordset.GetFieldValue(_T("SHAPEFIRST"),nFirst);
	int nSecond = 0;
	adoRecordset.GetFieldValue(_T("SHAPESECOND"),nSecond);
	m_shape = std::make_pair(nFirst,nSecond);

	adoRecordset.GetFieldValue(_T("LINETYPE"),nValue);
	m_linetype = (ENUM_LINE_TYPE)nValue;

	adoRecordset.GetFieldValue(_T("COLOR"),nValue);
	m_colorRef = (COLORREF)nValue;

	adoRecordset.GetFieldValue(_T("ISLEAVETRAIL"),nValue);
	m_bIsLeaveTrail = (BOOL)nValue;

	adoRecordset.GetFieldValue(_T("DENSITY"),m_dDensity);
}

void CVehicleDispPropItem::GetInsertSQL(int nParentID,CString& strSQL) const
{
	strSQL.Format(_T("INSERT INTO VEHICLEDISPLAYPROPERTYITEM \
					 (PROJID,PARENTID, VEHICLEID, VISIBLE, SHAPEFIRST,SHAPESECOND, LINETYPE, COLOR, ISLEAVETRAIL, DENSITY) \
					 VALUES (%d,%d,%d,%d,%d,%d,%d,%d,%d,%f);"),
					 m_nProjID,nParentID,m_vehicleItem.GetID(),(int)m_bVisible,m_shape.first,m_shape.second ,(int)m_linetype,(DWORD)m_colorRef,(int)m_bIsLeaveTrail,m_dDensity);
}

void CVehicleDispPropItem::GetUpdateSQL(CString& strSQL) const
{
	strSQL.Format(_T("UPDATE VEHICLEDISPLAYPROPERTYITEM SET \
				  PROJID = %d,PARENTID = %d, VEHICLEID = %d, VISIBLE = %d, SHAPEFIRST = %d,SHAPESECOND = %d , \
				  LINETYPE = %d, COLOR = %d , ISLEAVETRAIL = %d, DENSITY = %f WHERE (ID = %d);"),
				  m_nProjID,m_nParentID,m_vehicleItem.GetID(),(int)m_bVisible,m_shape.first,m_shape.second ,(int)m_linetype,(DWORD)m_colorRef,(int)m_bIsLeaveTrail,m_dDensity,m_nID);
}

void CVehicleDispPropItem::GetDeleteSQL(CString& strSQL) const
{
	strSQL.Format(_T("DELETE FROM VEHICLEDISPLAYPROPERTYITEM WHERE (ID = %d);"),m_nID);
}

void CVehicleDispPropItem::GetSelectSQL(int nID,CString& strSQL)const
{
	strSQL.Format(_T("SELECT * FROM VEHICLEDISPLAYPROPERTYITEM WHERE (ID = %d);"),nID);
}

void CVehicleDispPropItem::GetVehicleDispPropItemIDListByParentID(int nParentID,std::vector<int> & vrIDList)
{
	CString strSQL = _T("");
	strSQL.Format(_T("SELECT * FROM VEHICLEDISPLAYPROPERTYITEM WHERE (PARENTID = %d);"),nParentID);
	long nRecordCount = -1;
	CADORecordset adoRecordset;
	CADODatabase::ExecuteSQLStatement(strSQL, nRecordCount, adoRecordset);

	vrIDList.clear();
	int nIDValue = -1;
	while (!adoRecordset.IsEOF()) {
		adoRecordset.GetFieldValue(_T("ID"),nIDValue);
		vrIDList.push_back(nIDValue);
		adoRecordset.MoveNextData();
	}

	return;
}

void CVehicleDispPropItem::ExportData(CAirsideExportFile& exportFile)
{
	exportFile.getFile().writeInt(m_nID);
	exportFile.getFile().writeInt(m_nProjID);
	exportFile.getFile().writeInt(m_nParentID);
	exportFile.getFile().writeInt(m_vehicleItem.GetID());
	exportFile.getFile().writeInt((int)m_bVisible);
	exportFile.getFile().writeInt(m_shape.first);
	exportFile.getFile().writeInt(m_shape.second);
	exportFile.getFile().writeInt((int)m_linetype);
	exportFile.getFile().writeInt(GetRValue(m_colorRef));
	exportFile.getFile().writeInt(GetGValue(m_colorRef));
	exportFile.getFile().writeInt(GetBValue(m_colorRef));
	exportFile.getFile().writeInt((int)m_bIsLeaveTrail);
	exportFile.getFile().writeDouble(m_dDensity);
	exportFile.getFile().writeLine();
}

void CVehicleDispPropItem::ImportData(CAirsideImportFile& importFile)
{ 
	importFile.getFile().getInteger(m_nID); 
	m_nID = -1;
	importFile.getFile().getInteger(m_nProjID);
	m_nProjID = importFile.getNewProjectID();

	importFile.getFile().getInteger(m_nParentID);
	int nValue = -1;
	importFile.getFile().getInteger(nValue);
	nValue = importFile.GetVehicleDataSpecificationNewID(nValue);
	m_vehicleItem.ReadData(nValue);

	importFile.getFile().getInteger(nValue);
	m_bVisible = (BOOL)nValue;

	int nFirst = 0 , nSecond = 0;
	importFile.getFile().getInteger(nFirst);
	importFile.getFile().getInteger(nSecond);
	m_shape = std::make_pair(nFirst,nSecond);

	importFile.getFile().getInteger(nValue);
	m_linetype = (ENUM_LINE_TYPE)nValue;

	int nRed = -1;
	int nGreen = -1;
	int nBlue = -1;
	importFile.getFile().getInteger(nRed);
	importFile.getFile().getInteger(nGreen);
	importFile.getFile().getInteger(nBlue);
	m_colorRef = RGB(nRed,nGreen,nBlue);

	importFile.getFile().getInteger(nValue);
	m_bIsLeaveTrail = (BOOL)nValue;

	importFile.getFile().getFloat(m_dDensity);
	importFile.getFile().getLine();
}


const CVehicleSpecificationItem * CVehicleDispPropItem::GetVehicleType(void) const
{
	return &m_vehicleItem; 
}

BOOL CVehicleDispPropItem::IsVisible() const
{ 
	return m_bVisible; 
}

const std::pair<int, int>& CVehicleDispPropItem::GetShape() const
{
	return m_shape; 
}

COLORREF CVehicleDispPropItem::GetColor() const 
{ 
	return m_colorRef; 
}

BOOL CVehicleDispPropItem::IsLeaveTrail() const 
{ 
	return m_bIsLeaveTrail;
}

CVehicleDispPropItem::ENUM_LINE_TYPE CVehicleDispPropItem::GetLineType() const
{ 
	return m_linetype;
}

double CVehicleDispPropItem::GetDensity()const 
{ 
	return m_dDensity;
}    //for test 

void CVehicleDispPropItem::SetDensity(double density_)
{
	m_dDensity = density_;
}

void CVehicleDispPropItem::SetVehicleType( const CVehicleSpecificationItem& _vehicleItem )
{ 
	m_vehicleItem = _vehicleItem;
}

void CVehicleDispPropItem::SetVisible( BOOL _isVisible )
{
	m_bVisible = _isVisible;
}

void CVehicleDispPropItem::SetShape( const std::pair<int, int>& _shape )
{
	m_shape = _shape;
}

void CVehicleDispPropItem::SetColor( COLORREF _colorRef )
{ 
	m_colorRef = _colorRef; 
}

void CVehicleDispPropItem::SetLeaveTrail( BOOL _isLeaveTrail )
{ 
	m_bIsLeaveTrail = _isLeaveTrail; 
}

void CVehicleDispPropItem::SetLineType( CVehicleDispPropItem::ENUM_LINE_TYPE _linetype )
{
	m_linetype = _linetype;
}

bool operator == (const CVehicleDispPropItem & itemVehicle1,const CVehicleDispPropItem & itemVehicle2)
{
	if(itemVehicle1.GetVehicleType()->getName() == itemVehicle2.GetVehicleType()->getName() )
		return (true);
	else
		return (false);
}

int CVehicleDispPropItem::GetProjectID(void)
{
	return (m_nProjID);
}

void CVehicleDispPropItem::SetParentID(int nParentID)
{
	m_nParentID = nParentID;
}

int CVehicleDispPropItem::GetParentID(void)
{
	return (m_nParentID);
}