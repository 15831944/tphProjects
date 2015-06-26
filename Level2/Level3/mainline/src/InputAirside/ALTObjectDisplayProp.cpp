#include "StdAfx.h"
#include ".\altobjectdisplayprop.h"
#include "AirsideImportExportManager.h"



const static ARCColor4 ShapeColor(55, 145, 170,255);
const static ARCColor4 InConstrainColor(255, 100, 0 ,255);
const static ARCColor4 OutConstrainColor = ARCColor4 (230, 210, 25,255);
const static ARCColor4 NameColor = ARCColor4(0,0,0,255);
const static ARCColor4 DeicePadColor = ARCColor4 ( 64, 210, 64,255);

const static DisplayProp NameDisplay(true,NameColor);
const static DisplayProp ShapeDisplay(true, ShapeColor);
const static DisplayProp InConstrainDisplay(true, InConstrainColor);
const static DisplayProp OutConstrainDisplay(true, OutConstrainColor);


//m_colorRef[AreasColor] = ARCColor3( 170, 25, 25 );
//m_colorRef[PortalsColor] = ARCColor3( 235, 185, 30 );
//m_colorRef[ProcShapeColor] = ARCColor3( 55, 145, 170 );
//m_colorRef[ProcServLocColor] = ARCColor3( 0, 100, 200 );
//m_colorRef[ProcQueueColor] = ARCColor3( 64, 210, 64 );
//m_colorRef[ProcInConstraintColor] = ARCColor3( 255, 100, 0 );
//m_colorRef[ProcOutConstraintColor] = ARCColor3( 230, 210, 25 );
//m_colorRef[ProcNameColor] = ARCColor3( 0, 0, 0 );

//////////////////////////////////////////////////////////////////////////
//struct DisplayProp
void DisplayProp::ReadData()
{
	if (m_nID == -1)
		return;
	bDefault = false;

	CString strSQL =_T("");
	strSQL.Format(_T("SELECT BON, COLOR, ALPHA\
		FROM ALTOBJECTDSPPROP\
		WHERE (ID = %d)"),m_nID);

	CADORecordset adoRecordset;
	long nRecordCount = -1;
	CADODatabase::ExecuteSQLStatement(strSQL,nRecordCount,adoRecordset);

	if (!adoRecordset.IsEOF())
	{
		ReadData(adoRecordset);
	}

}
void DisplayProp::ReadData(CADORecordset& adoRecordset)
{
	adoRecordset.GetFieldValue(_T("BON"),bOn);

	unsigned long ulColor = 0L;
	adoRecordset.GetFieldValue(_T("COLOR"),ulColor);
	cColor= ARCColor4((COLORREF)(DWORD)ulColor);

	int nAlpha = 0;
	adoRecordset.GetFieldValue(_T("ALPHA"),nAlpha);

	cColor[3] = nAlpha;
}
void DisplayProp::SaveData()
{
	//if (m_nID != -1)//if data exists,update
	//{
	//	return UpdateData();
	//}
	//save data
	CString strSQL = _T("");
	strSQL.Format(_T("INSERT INTO ALTOBJECTDSPPROP\
		(BON, COLOR, ALPHA)\
		VALUES (%d,%d,%d)"),bOn,(unsigned long)(DWORD)(COLORREF)cColor,cColor[3]);

	m_nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
}
void DisplayProp::UpdateData()
{
	if (m_nID == -1)
	 return;
	CString strSQL = _T("");
	strSQL.Format(_T("UPDATE ALTOBJECTDSPPROP\
		SET BON =%d, COLOR =%d, ALPHA =%d\
		WHERE (ID = %d)"),bOn,(unsigned long)(DWORD)(COLORREF)cColor,cColor[3],m_nID);

	CADODatabase::ExecuteSQLStatement(strSQL);

}
void DisplayProp::DeleteData()
{
	if (m_nID == -1)
		return;

	CString strSQL = _T("");
	strSQL.Format(_T("DELETE FROM ALTOBJECTDSPPROP\
		WHERE (ID = %d)"),m_nID);
	CADODatabase::ExecuteSQLStatement(strSQL);
}
void DisplayProp::ExportDisplayProp(CAirsideExportFile& exportFile)
{

	exportFile.getFile().writeInt(m_nID);
	if (m_nID != -1)//default one
	{
		exportFile.getFile().writeInt(bDefault?1:0);
		exportFile.getFile().writeInt(bOn?1:0);

		exportFile.getFile().writeInt((long)(unsigned long) (DWORD)(COLORREF)cColor);
		exportFile.getFile().writeInt(cColor[3]);
	}


	//exportFile.getFile().writeLine();
}

void DisplayProp::ImportDisplayProp(CAirsideImportFile& importFile)
{
	int nOldID = -2;
	importFile.getFile().getInteger(nOldID);
	if (nOldID != -1) //default
	{
		
		int nDefault = 0;
		importFile.getFile().getInteger(nDefault);
		bDefault = (nDefault > 0)?true:false;


		int nOn = 0;
		importFile.getFile().getInteger(nOn);
		bOn = (nOn > 0)?true:false;


		//color
		long ulColor;
		importFile.getFile().getInteger(ulColor);
		cColor= ARCColor4((COLORREF)(DWORD)ulColor);
		
		//alpha
		int nAlpha = 0;
		importFile.getFile().getInteger(nAlpha);
		cColor[3] = nAlpha;

		SaveData();
	}
//	importFile.getFile().getLine();

}

void DisplayProp::CopyData( const DisplayProp& prop )
{
	bDefault = prop.bDefault;
	bOn = prop.bOn;
	cColor = prop.cColor;
}
//////////////////////////////////////////////////////////////////////////
//class ALTObjectDisplayProp
ALTObjectDisplayProp::ALTObjectDisplayProp(void)
{
	//m_dpName.strName = "Object Name";
	//m_dpName.cColor = ARCColor4(ARCColor4::BLACK);
	//m_dpShape.strName  = "Object Shape";
	//m_dpShape.cColor = ARCColor4( 55, 145, 170,255 );
	m_dpName = NameDisplay;
	m_dpShape = ShapeDisplay;
	m_nID = -1;
}


ALTObjectDisplayProp::~ALTObjectDisplayProp(void)
{
}


ALTOBJECT_TYPE ALTObjectDisplayProp::GetType()
{
	return ALT_UNKNOWN;
}
void ALTObjectDisplayProp::ReadBaseRecord(CADORecordset& adoRecordset)
{	
	adoRecordset.GetFieldValue(_T("ID"),m_nID);
	adoRecordset.GetFieldValue(_T("DPNAME"),m_dpName.m_nID);
	adoRecordset.GetFieldValue(_T("DPSHAPE"),m_dpShape.m_nID);

	m_dpName.ReadData();
	m_dpShape.ReadData();
}
void ALTObjectDisplayProp::UpdateBaseRecord()
{
	m_dpName.UpdateData();
	m_dpShape.UpdateData();
}
void ALTObjectDisplayProp::DeleteBaseRecord()
{
	m_dpName.DeleteData();
	m_dpShape.DeleteData();
}
void ALTObjectDisplayProp::SaveBaseRecord()
{
	m_dpName.SaveData();
	m_dpShape.SaveData();

}
void ALTObjectDisplayProp::ExportDspProp(CAirsideExportFile& exportFile)
{
	exportFile.getFile().writeInt(m_nID);
	if (m_nID == -1)
	{	
		exportFile.getFile().writeLine();
		return;
	}

	m_dpName.ExportDisplayProp(exportFile);
	m_dpShape.ExportDisplayProp(exportFile);
	ExportOtherDspProp(exportFile);
	exportFile.getFile().writeLine();

}
void ALTObjectDisplayProp::ImportDspProp(CAirsideImportFile& importFile,int nObjID)
{

	if (importFile.getVersion() < 1021)
		return;


	int nOldID = -1;
	importFile.getFile().getInteger(nOldID);

	if (nOldID == -1)
	{	
		importFile.getFile().getLine();
		return;
	}

	m_dpName.ImportDisplayProp(importFile);
	m_dpShape.ImportDisplayProp(importFile);
	ImportOtherDspProp(importFile);
	importFile.getFile().getLine();

	SaveData(nObjID);

}
void ALTObjectDisplayProp::ExportOtherDspProp(CAirsideExportFile& exportFile)
{

}
void ALTObjectDisplayProp::ImportOtherDspProp(CAirsideImportFile& importFile)
{

}
void ALTObjectDisplayProp::ReadData(int nObjectID)
{

}
void ALTObjectDisplayProp::UpdateData(int nObjectID)
{

}
void ALTObjectDisplayProp::DeleteData(int nObjectID)
{

}
void ALTObjectDisplayProp::SaveData(int nObjectID)
{

}

void ALTObjectDisplayProp::CopyData( const ALTObjectDisplayProp& prop)
{
	m_dpShape.CopyData(prop.m_dpShape);
	m_dpName.CopyData(prop.m_dpName);
}

//////////////////////////////////////////////////////////////////////////
//class TaxiwayDisplayProp

//default values	
TaxiwayDisplayProp::TaxiwayDisplayProp()
{
	m_dpDir = DisplayProp(false, ARCColor4(ARCColor4::WHITE) );
	m_dpCenterLine = DisplayProp(true,ARCColor4(255,255,0,255) );
	m_dpMarking = DisplayProp(true, ARCColor4(255,255,255,150) );

}
TaxiwayDisplayProp::~TaxiwayDisplayProp()
{
}
void TaxiwayDisplayProp::ReadData(int nTaxiwayID)
{
	if (nTaxiwayID == -1)
	 return;

	CString strSQL = _T("");
	strSQL.Format(_T("SELECT ID,DPNAME, DPSHAPE, DPDIR, DPCENTERLINE, DPMARK\
		FROM TAXIWAYDSPPROP\
		WHERE (TAXIWAYID = %d)"),nTaxiwayID);

	CADORecordset adoRecordset;
	long nRecordCount = -1;
	CADODatabase::ExecuteSQLStatement(strSQL,nRecordCount,adoRecordset);

	if (!adoRecordset.IsEOF())
	{
		ReadBaseRecord(adoRecordset);
		adoRecordset.GetFieldValue(_T("DPDIR"),m_dpDir.m_nID);
		adoRecordset.GetFieldValue(_T("DPCENTERLINE"),m_dpCenterLine.m_nID);
		adoRecordset.GetFieldValue(_T("DPMARK"),m_dpMarking.m_nID);
		
		m_dpDir.ReadData();
		m_dpCenterLine.ReadData();
		m_dpMarking.ReadData();
	}
}
void TaxiwayDisplayProp::UpdateData(int nTaxiwayID)
{
	UpdateBaseRecord();
	m_dpDir.SaveData();
	m_dpCenterLine.SaveData();
	m_dpMarking.SaveData();
	CString strSQL = _T("");
	strSQL.Format(_T("UPDATE TAXIWAYDSPPROP\
		SET DPNAME =%d, DPSHAPE =%d, DPDIR =%d, DPCENTERLINE =%d, DPMARK =%d\
		WHERE (TAXIWAYID = %d)"),
		m_dpName.m_nID,m_dpShape.m_nID,m_dpDir.m_nID,m_dpCenterLine.m_nID,m_dpMarking.m_nID,
		nTaxiwayID);

	CADODatabase::ExecuteSQLStatement(strSQL);

}
void TaxiwayDisplayProp::DeleteData(int nTaxiwayID)
{
	DeleteBaseRecord();
	m_dpDir.DeleteData();
	m_dpCenterLine.DeleteData();
	m_dpMarking.DeleteData();
	CString strSQL = _T("");
	strSQL.Format(_T("DELETE FROM TAXIWAYDSPPROP\
		WHERE (TAXIWAYID = %d)"),nTaxiwayID);

	CADODatabase::ExecuteSQLStatement(strSQL);
}
void TaxiwayDisplayProp::SaveData(int nTaxiwayID)
{
	if (m_nID != -1)
	{
		return UpdateData(nTaxiwayID);
	}
	SaveBaseRecord();
	m_dpDir.SaveData();
	m_dpCenterLine.SaveData();
	m_dpMarking.SaveData();
	CString strSQL = _T("");
	strSQL.Format(_T("INSERT INTO TAXIWAYDSPPROP\
		(TAXIWAYID, DPNAME, DPSHAPE, DPDIR, DPCENTERLINE, DPMARK)\
		VALUES (%d,%d,%d,%d,%d,%d)"),
		nTaxiwayID,m_dpName.m_nID,m_dpShape.m_nID,m_dpDir.m_nID,m_dpCenterLine.m_nID,m_dpMarking.m_nID);

	m_nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
}
void TaxiwayDisplayProp::ExportOtherDspProp(CAirsideExportFile& exportFile)
{
	m_dpDir.ExportDisplayProp(exportFile);
	m_dpCenterLine.ExportDisplayProp(exportFile);
	m_dpMarking.ExportDisplayProp(exportFile);

}
void TaxiwayDisplayProp::ImportOtherDspProp(CAirsideImportFile& importFile)
{
	m_dpDir.ImportDisplayProp(importFile);
	m_dpCenterLine.ImportDisplayProp(importFile);
	m_dpMarking.ImportDisplayProp(importFile);

}

void TaxiwayDisplayProp::CopyData( const TaxiwayDisplayProp& prop )
{
	ALTObjectDisplayProp::CopyData(prop);
	m_dpCenterLine.CopyData(prop.m_dpCenterLine);
	m_dpDir.CopyData(prop.m_dpDir);
	m_dpMarking.CopyData(prop.m_dpMarking);
}
//////////////////////////////////////////////////////////////////////////
//RunwayDisplayProp
RunwayDisplayProp::RunwayDisplayProp()
{
}
RunwayDisplayProp::~RunwayDisplayProp()
{
}

void RunwayDisplayProp::ReadData( int nRunwayID)
{
	CString strSQL = _T("");
	strSQL.Format(_T("SELECT ID, DPNAME, DPSHAPE\
		FROM RUNWAYDSPPROP\
		WHERE (RUNWAYID = %d)"),nRunwayID);

	CADORecordset adoRecordset;
	long nRecordCount = -1;
	CADODatabase::ExecuteSQLStatement(strSQL,nRecordCount,adoRecordset);
	if (!adoRecordset.IsEOF())
	{
		ReadBaseRecord(adoRecordset);
	}
}
void RunwayDisplayProp::UpdateData(int nRunwayID)
{	
	UpdateBaseRecord();
	CString strSQL = _T("");
	strSQL.Format(_T("UPDATE RUNWAYDSPPROP\
		SET DPNAME =%d, DPSHAPE =%d\
		WHERE (RUNWAYID = %d)"),m_dpName.m_nID,m_dpShape.m_nID,nRunwayID);
}
void RunwayDisplayProp::DeleteData(int nRunwayID)
{	
	DeleteBaseRecord();
	CString strSQL = _T("");
	strSQL.Format(_T("DELETE FROM RUNWAYDSPPROP\
		WHERE (RUNWAYID = %d)"),nRunwayID);

	CADODatabase::ExecuteSQLStatement(strSQL);
}
void RunwayDisplayProp::SaveData(int nRunwayID)
{	
	if (m_nID != -1)
	{
		return UpdateData(nRunwayID);
	}
	SaveBaseRecord();
	CString strSQL = _T("");
	strSQL.Format(_T("INSERT INTO RUNWAYDSPPROP\
		(RUNWAYID, DPNAME, DPSHAPE)\
		VALUES (%d,%d,%d)"),nRunwayID,m_dpName.m_nID,m_dpShape.m_nID);
	m_nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
}

//////////////////////////////////////////////////////////////////////////
//stand 
StandDisplayProp::StandDisplayProp()
{
	m_dpInConstr = DisplayProp(true,ARCColor4(255, 120, 20 ,255));
	m_dpOutConstr =  DisplayProp(true, ARCColor4(255,255,0,255));
}


void StandDisplayProp::ReadData( int nStandID)
{
	CString strSQL = _T("");
	strSQL.Format(_T("SELECT ID, DPNAME, DPSHAPE, DPINCONS, DPOUTCONS\
		FROM STANDDSPPROP\
					 WHERE (STANDID = %d)"),nStandID);

	CADORecordset adoRecordset;
	long nRecordCount = -1;
	CADODatabase::ExecuteSQLStatement(strSQL,nRecordCount,adoRecordset);
	if (!adoRecordset.IsEOF())
	{
		ReadBaseRecord(adoRecordset);
		adoRecordset.GetFieldValue(_T("DPINCONS"),m_dpInConstr.m_nID);
		adoRecordset.GetFieldValue(_T("DPOUTCONS"),m_dpOutConstr.m_nID);

		m_dpInConstr.ReadData();
		m_dpOutConstr.ReadData();
	}
}
void StandDisplayProp::UpdateData(int nStandID)
{	
	UpdateBaseRecord();
	m_dpInConstr.SaveData();
	m_dpOutConstr.SaveData();
	CString strSQL = _T("");
	strSQL.Format(_T("UPDATE STANDDSPPROP\
		SET DPNAME =%d, DPSHAPE =%d, DPINCONS =%d, DPOUTCONS =%d\
					 WHERE (STANDID = %d)"),
					 m_dpName.m_nID,m_dpShape.m_nID,m_dpInConstr.m_nID,m_dpOutConstr.m_nID,nStandID);
}
void StandDisplayProp::DeleteData(int nStandID)
{	
	DeleteBaseRecord();
	m_dpInConstr.DeleteData();
	m_dpOutConstr.DeleteData();
	CString strSQL = _T("");
	strSQL.Format(_T("DELETE FROM STANDDSPPROP\
					 WHERE (STANDID = %d)"),nStandID);

	CADODatabase::ExecuteSQLStatement(strSQL);
}
void StandDisplayProp::SaveData(int nStandID)
{	
	if (m_nID != -1)
	{
		return UpdateData(nStandID);
	}
	SaveBaseRecord();
	m_dpInConstr.SaveData();
	m_dpOutConstr.SaveData();
	CString strSQL = _T("");
	strSQL.Format(_T("INSERT INTO STANDDSPPROP\
		(STANDID, DPNAME, DPSHAPE, DPINCONS, DPOUTCONS)\
		VALUES (%d,%d,%d,%d,%d)"),
		nStandID,m_dpName.m_nID,m_dpShape.m_nID,m_dpInConstr.m_nID,m_dpOutConstr.m_nID);

	m_nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
}
void StandDisplayProp::ExportOtherDspProp(CAirsideExportFile& exportFile)
{

	m_dpInConstr.ExportDisplayProp(exportFile);
	m_dpOutConstr.ExportDisplayProp(exportFile);

}
void StandDisplayProp::ImportOtherDspProp(CAirsideImportFile& importFile)
{

	m_dpInConstr.ImportDisplayProp(importFile);
	m_dpOutConstr.ImportDisplayProp(importFile);
}

void StandDisplayProp::CopyData( const StandDisplayProp& prop )
{
	ALTObjectDisplayProp::CopyData(prop);
	m_dpInConstr.CopyData(prop.m_dpInConstr);
	m_dpOutConstr.CopyData(prop.m_dpOutConstr);
}
//////////////////////////////////////////////////////////////////////////
//deice pad
DeicePadDisplayProp::DeicePadDisplayProp()
{
	m_dpInConstr = InConstrainDisplay;
	m_dpOutConstr =  OutConstrainDisplay;
	m_dpPad.cColor = DeicePadColor;
}
void DeicePadDisplayProp::ReadData( int nDeicePadID)
{
	CString strSQL = _T("");
	strSQL.Format(_T("SELECT ID, DPNAME, DPSHAPE, DPINCONS, DPOUTCONS, DPPAD, DPTRUCK\
		FROM DEICEPADDSPPROP\
		WHERE (DEICEPADID = %d)"),nDeicePadID);

	CADORecordset adoRecordset;
	long nRecordCount = -1;
	CADODatabase::ExecuteSQLStatement(strSQL,nRecordCount,adoRecordset);
	if (!adoRecordset.IsEOF())
	{
		ReadBaseRecord(adoRecordset);
		adoRecordset.GetFieldValue(_T("DPINCONS"),m_dpInConstr.m_nID);
		adoRecordset.GetFieldValue(_T("DPOUTCONS"),m_dpOutConstr.m_nID);
		adoRecordset.GetFieldValue(_T("DPPAD"),m_dpPad.m_nID);
		adoRecordset.GetFieldValue(_T("DPTRUCK"),m_dpTruck.m_nID);

		m_dpInConstr.ReadData();
		m_dpOutConstr.ReadData();

		m_dpPad.ReadData();
		m_dpTruck.ReadData();
	}
}
void DeicePadDisplayProp::UpdateData(int nDeicePadID)
{		
	if (m_nID == -1)
	return;

	UpdateBaseRecord();
	m_dpInConstr.SaveData();
	m_dpOutConstr.SaveData();

	m_dpPad.SaveData();
	m_dpTruck.SaveData();
	CString strSQL = _T("");
	strSQL.Format(_T("UPDATE DEICEPADDSPPROP\
		SET DPNAME =%d, DPSHAPE =%d, DPINCONS =%d, DPOUTCONS =%d, DPPAD =%d, DPTRUCK =%d\
		WHERE (DEICEPADID = %d)"),
					 m_dpName.m_nID,m_dpShape.m_nID,m_dpInConstr.m_nID,m_dpOutConstr.m_nID,m_dpPad.m_nID,m_dpTruck.m_nID,nDeicePadID);
	CADODatabase::ExecuteSQLStatement(strSQL);
}
void DeicePadDisplayProp::DeleteData(int nDeicePadID)
{	
	if (m_nID == -1)
		return;

	DeleteBaseRecord();
	m_dpInConstr.DeleteData();
	m_dpOutConstr.DeleteData();

	m_dpPad.DeleteData();
	m_dpTruck.DeleteData();

	CString strSQL = _T("");
	strSQL.Format(_T("DELETE FROM DEICEPADDSPPROP\
		WHERE (DEICEPADID = %d)"),nDeicePadID);

	CADODatabase::ExecuteSQLStatement(strSQL);
}
void DeicePadDisplayProp::SaveData(int nDeicePadID)
{	
	if (m_nID != -1)
	{
		return UpdateData(nDeicePadID);
	}
	SaveBaseRecord();
	m_dpInConstr.SaveData();
	m_dpOutConstr.SaveData();

	m_dpPad.SaveData();
	m_dpTruck.SaveData();

	CString strSQL = _T("");
	strSQL.Format(_T("INSERT INTO DEICEPADDSPPROP\
		(DEICEPADID, DPNAME, DPSHAPE, DPINCONS, DPOUTCONS, DPPAD, DPTRUCK)\
					 VALUES (%d,%d,%d,%d,%d ,%d ,%d)"),
					 nDeicePadID,m_dpName.m_nID,m_dpShape.m_nID,m_dpInConstr.m_nID,m_dpOutConstr.m_nID,m_dpPad.m_nID,m_dpTruck.m_nID);

	m_nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
}
void DeicePadDisplayProp::ExportOtherDspProp(CAirsideExportFile& exportFile)
{

	m_dpInConstr.ExportDisplayProp(exportFile);
	m_dpOutConstr.ExportDisplayProp(exportFile);
	m_dpPad.ExportDisplayProp(exportFile);
	m_dpTruck.ExportDisplayProp(exportFile);

}
void DeicePadDisplayProp::ImportOtherDspProp(CAirsideImportFile& importFile)
{


	m_dpInConstr.ImportDisplayProp(importFile);
	m_dpOutConstr.ImportDisplayProp(importFile);
	m_dpPad.ImportDisplayProp(importFile);
	m_dpTruck.ImportDisplayProp(importFile);
}

void DeicePadDisplayProp::CopyData( const DeicePadDisplayProp& prop )
{
	ALTObjectDisplayProp::CopyData(prop);
	m_dpPad.CopyData(prop.m_dpPad);
	m_dpInConstr.CopyData(prop.m_dpInConstr);
	m_dpOutConstr.CopyData(prop.m_dpOutConstr);
	m_dpTruck.CopyData(prop.m_dpTruck);
}
//////////////////////////////////////////////////////////////////////////
//AirRoute
AirRouteDisplayProp::AirRouteDisplayProp()
{
	m_dpShape.cColor = ARCColor4(255, 100, 0,64);
}
void AirRouteDisplayProp::ReadData( int nAirRouteID)
{
	CString strSQL = _T("");
	strSQL.Format(_T("SELECT ID, DPNAME, DPSHAPE\
					 FROM AIRROUTEDSPPROP\
					 WHERE (AIRROUTEID = %d)"),nAirRouteID);

	CADORecordset adoRecordset;
	long nRecordCount = -1;
	CADODatabase::ExecuteSQLStatement(strSQL,nRecordCount,adoRecordset);
	if (!adoRecordset.IsEOF())
	{
		ReadBaseRecord(adoRecordset);

	}
}
void AirRouteDisplayProp::UpdateData(int nAirRouteID)
{		
	if (m_nID == -1)
		return;

	UpdateBaseRecord();
	CString strSQL = _T("");
	strSQL.Format(_T("UPDATE AIRROUTEDSPPROP\
					 SET DPNAME =%d, DPSHAPE =%d\
					 WHERE (AIRROUTEID = %d)"),
					 m_dpName.m_nID,m_dpShape.m_nID,nAirRouteID);
}
void AirRouteDisplayProp::DeleteData(int nAirRouteID)
{	
	if (m_nID == -1)
		return;

	DeleteBaseRecord();
	CString strSQL = _T("");
	strSQL.Format(_T("DELETE FROM AIRROUTEDSPPROP\
					 WHERE (AIRROUTEID = %d)"),nAirRouteID);

	CADODatabase::ExecuteSQLStatement(strSQL);
}
void AirRouteDisplayProp::SaveData(int nAirRouteID)
{	
	if (m_nID != -1)
	{
		return UpdateData(nAirRouteID);
	}
	SaveBaseRecord();
	CString strSQL = _T("");
	strSQL.Format(_T("INSERT INTO AIRROUTEDSPPROP\
					 (AIRROUTEID, DPNAME, DPSHAPE)\
					 VALUES (%d,%d,%d)"),
					 nAirRouteID,m_dpName.m_nID,m_dpShape.m_nID);

	m_nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
}


//////////////////////////////////////////////////////////////////////////
//AirRoute
AirHoldDisplayProp::AirHoldDisplayProp()
{
	m_dpShape.cColor = ARCColor4(230, 210, 25,64);
}
void AirHoldDisplayProp::ReadData( int nAirHoldID)
{
	CString strSQL = _T("");
	strSQL.Format(_T("SELECT ID, DPNAME, DPSHAPE\
					 FROM HOLDDSPPROP\
					 WHERE (AIRHOLDID = %d)"),nAirHoldID);

	CADORecordset adoRecordset;
	long nRecordCount = -1;
	CADODatabase::ExecuteSQLStatement(strSQL,nRecordCount,adoRecordset);
	if (!adoRecordset.IsEOF())
	{
		ReadBaseRecord(adoRecordset);

	}
}
void AirHoldDisplayProp::UpdateData(int nAirHoldID)
{		
	if (m_nID == -1)
		return;

	UpdateBaseRecord();
	CString strSQL = _T("");
	strSQL.Format(_T("UPDATE HOLDDSPPROP\
					 SET DPNAME =%d, DPSHAPE =%d\
					 WHERE (AIRHOLDID = %d)"),
					 m_dpName.m_nID,m_dpShape.m_nID,nAirHoldID);
}
void AirHoldDisplayProp::DeleteData(int nAirHoldID)
{	
	if (m_nID == -1)
		return;

	DeleteBaseRecord();
	CString strSQL = _T("");
	strSQL.Format(_T("DELETE FROM HOLDDSPPROP\
					 WHERE (AIRHOLDID = %d)"),nAirHoldID);

	CADODatabase::ExecuteSQLStatement(strSQL);
}
void AirHoldDisplayProp::SaveData(int nAirHoldID)
{	
	if (m_nID != -1)
	{
		return UpdateData(nAirHoldID);
	}
	SaveBaseRecord();
	CString strSQL = _T("");
	strSQL.Format(_T("INSERT INTO HOLDDSPPROP\
					 (AIRHOLDID, DPNAME, DPSHAPE)\
					 VALUES (%d,%d,%d)"),
					 nAirHoldID,m_dpName.m_nID,m_dpShape.m_nID);

	m_nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
}
//////////////////////////////////////////////////////////////////////////
// air way point 
AirWayPointDisplayProp::AirWayPointDisplayProp()
{
	
}
void AirWayPointDisplayProp::ReadData( int nWayPointID)
{
	CString strSQL = _T("");
	strSQL.Format(_T("SELECT ID, DPNAME, DPSHAPE\
					 FROM WAYPOINTDSPPROP\
					 WHERE (WAYPOINTID = %d)"),nWayPointID);

	CADORecordset adoRecordset;
	long nRecordCount = -1;
	CADODatabase::ExecuteSQLStatement(strSQL,nRecordCount,adoRecordset);
	if (!adoRecordset.IsEOF())
	{
		ReadBaseRecord(adoRecordset);

	}
}
void AirWayPointDisplayProp::UpdateData(int nWayPointID)
{		
	if (m_nID == -1)
		return;

	UpdateBaseRecord();
	CString strSQL = _T("");
	strSQL.Format(_T("UPDATE WAYPOINTDSPPROP\
					 SET DPNAME =%d, DPSHAPE =%d\
					 WHERE (WAYPOINTID = %d)"),
					 m_dpName.m_nID,m_dpShape.m_nID,nWayPointID);
}
void AirWayPointDisplayProp::DeleteData(int nWayPointID)
{	
	if (m_nID == -1)
		return;

	DeleteBaseRecord();
	CString strSQL = _T("");
	strSQL.Format(_T("DELETE FROM WAYPOINTDSPPROP\
					 WHERE (WAYPOINTID = %d)"),nWayPointID);

	CADODatabase::ExecuteSQLStatement(strSQL);
}
void AirWayPointDisplayProp::SaveData(int nWayPointID)
{	
	if (m_nID != -1)
	{
		return UpdateData(nWayPointID);
	}
	SaveBaseRecord();
	CString strSQL = _T("");
	strSQL.Format(_T("INSERT INTO WAYPOINTDSPPROP\
					 (WAYPOINTID, DPNAME, DPSHAPE)\
					 VALUES (%d,%d,%d)"),
					 nWayPointID,m_dpName.m_nID,m_dpShape.m_nID);

	m_nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
}
//////////////////////////////////////////////////////////////////////////

AirSectorDisplayProp::AirSectorDisplayProp()
{
	m_dpShape.cColor = ARCColor4(255,140,0,64);
}

void AirSectorDisplayProp::ReadData( int nSectorID)
{
	CString strSQL = _T("");
	strSQL.Format(_T("SELECT ID, DPNAME, DPSHAPE\
					 FROM SECTORDSPPROP\
					 WHERE (SECTORID = %d)"),nSectorID);

	CADORecordset adoRecordset;
	long nRecordCount = -1;
	CADODatabase::ExecuteSQLStatement(strSQL,nRecordCount,adoRecordset);
	if (!adoRecordset.IsEOF())
	{
		ReadBaseRecord(adoRecordset);

	}
}
void AirSectorDisplayProp::UpdateData(int nSectorID)
{		
	if (m_nID == -1)
		return;

	UpdateBaseRecord();
	CString strSQL = _T("");
	strSQL.Format(_T("UPDATE SECTORDSPPROP\
					 SET DPNAME =%d, DPSHAPE =%d\
					 WHERE (SECTORID = %d)"),
					 m_dpName.m_nID,m_dpShape.m_nID,nSectorID);
}
void AirSectorDisplayProp::DeleteData(int nSectorID)
{	
	if (m_nID == -1)
		return;

	DeleteBaseRecord();
	CString strSQL = _T("");
	strSQL.Format(_T("DELETE FROM SECTORDSPPROP\
					 WHERE (SECTORID = %d)"),nSectorID);

	CADODatabase::ExecuteSQLStatement(strSQL);
}
void AirSectorDisplayProp::SaveData(int nSectorID)
{	
	if (m_nID != -1)
	{
		return UpdateData(nSectorID);
	}
	SaveBaseRecord();
	CString strSQL = _T("");
	strSQL.Format(_T("INSERT INTO SECTORDSPPROP\
					 (SECTORID, DPNAME, DPSHAPE)\
					 VALUES (%d,%d,%d)"),
					 nSectorID,m_dpName.m_nID,m_dpShape.m_nID);

	m_nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
}

//////////////////////////////////////////////////////////////////////////
void StructureDisplayProp::ReadData( int nStructureID )
{
	CString strSQL = _T("");
	strSQL.Format(_T("SELECT *\
					 FROM STRUCTUREDSP\
					 WHERE (STRUCTUREID = %d)"),nStructureID);

	CADORecordset adoRecordset;
	long nRecordCount = -1;
	CADODatabase::ExecuteSQLStatement(strSQL,nRecordCount,adoRecordset);
	if (!adoRecordset.IsEOF())
	{
		ReadBaseRecord(adoRecordset);

	}
}

void StructureDisplayProp::UpdateData( int nStructureID )
{
	if (m_nID == -1)
		return;

	UpdateBaseRecord();
	CString strSQL = _T("");
	strSQL.Format(_T("UPDATE STRUCTUREDSP\
					 SET DPNAME =%d, DPSHAPE =%d\
					 WHERE (STRUCTUREID = %d)"),
					 m_dpName.m_nID,m_dpShape.m_nID,nStructureID);
}

void StructureDisplayProp::DeleteData( int nStructureID )
{
	if (m_nID == -1)
		return;

	DeleteBaseRecord();
	CString strSQL = _T("");
	strSQL.Format(_T("DELETE FROM STRUCTUREDSP\
					 WHERE (STRUCTUREID = %d)"),nStructureID);

	CADODatabase::ExecuteSQLStatement(strSQL);
}

void StructureDisplayProp::SaveData( int nStructureID )
{
	if (m_nID != -1)
	{
		return UpdateData(nStructureID);
	}
	SaveBaseRecord();
	CString strSQL = _T("");
	strSQL.Format(_T("INSERT INTO STRUCTUREDSP\
					 (STRUCTUREID, DPNAME, DPSHAPE)\
					 VALUES (%d,%d,%d)"),
					 nStructureID,m_dpName.m_nID,m_dpShape.m_nID);

	m_nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
}

/////////////////////////////////////////////////////////////////////////

void ReportingAreaDisplayProp::ReadData( int nAreaID )
{
	CString strSQL = _T("");
	strSQL.Format(_T("SELECT *\
					 FROM AS_REPORTINGAREADSP\
					 WHERE (AREAID = %d)"),nAreaID);

	CADORecordset adoRecordset;
	long nRecordCount = -1;
	CADODatabase::ExecuteSQLStatement(strSQL,nRecordCount,adoRecordset);
	if (!adoRecordset.IsEOF())
	{
		ReadBaseRecord(adoRecordset);

	}
}

void ReportingAreaDisplayProp::UpdateData( int nAreaID )
{
	if (m_nID == -1)
		return;

	UpdateBaseRecord();
	CString strSQL = _T("");
	strSQL.Format(_T("UPDATE AS_REPORTINGAREADSP\
					 SET DPNAME =%d, DPSHAPE =%d\
					 WHERE (AREAID = %d)"),
					 m_dpName.m_nID,m_dpShape.m_nID,nAreaID);
}

void ReportingAreaDisplayProp::DeleteData( int nAreaID )
{
	if (m_nID == -1)
		return;

	DeleteBaseRecord();
	CString strSQL = _T("");
	strSQL.Format(_T("DELETE FROM AS_REPORTINGAREADSP\
					 WHERE (AREAID = %d)"),nAreaID);

	CADODatabase::ExecuteSQLStatement(strSQL);
}

void ReportingAreaDisplayProp::SaveData( int nAreaID )
{
	if (m_nID != -1)
	{
		return UpdateData(nAreaID);
	}
	SaveBaseRecord();
	CString strSQL = _T("");
	strSQL.Format(_T("INSERT INTO AS_REPORTINGAREADSP\
					 (AREAID, DPNAME, DPSHAPE)\
					 VALUES (%d,%d,%d)"),
					 nAreaID,m_dpName.m_nID,m_dpShape.m_nID);

	m_nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
}
//////////////////////////////////////////////////////////////////////////
void ParkSpotDisplayProp::ReadData( int nStandID )
{
	CString strSQL = _T("");
	strSQL.Format(_T("SELECT ID, DPNAME, DPSHAPE, DPINCONS, DPOUTCONS\
					 FROM TB_PARKSPOTDSPPROP\
					 WHERE (OBJID = %d)"),nStandID);

	CADORecordset adoRecordset;
	long nRecordCount = -1;
	CADODatabase::ExecuteSQLStatement(strSQL,nRecordCount,adoRecordset);
	if (!adoRecordset.IsEOF())
	{
		ReadBaseRecord(adoRecordset);
		adoRecordset.GetFieldValue(_T("DPINCONS"),m_dpInConstr.m_nID);
		adoRecordset.GetFieldValue(_T("DPOUTCONS"),m_dpOutConstr.m_nID);

		m_dpInConstr.ReadData();
		m_dpOutConstr.ReadData();
	}
}

void ParkSpotDisplayProp::UpdateData( int nStandID )
{
	UpdateBaseRecord();
	m_dpInConstr.SaveData();
	m_dpOutConstr.SaveData();
	CString strSQL = _T("");
	strSQL.Format(_T("UPDATE TB_PARKSPOTDSPPROP\
					 SET DPNAME =%d, DPSHAPE =%d, DPINCONS =%d, DPOUTCONS =%d\
					 WHERE (OBJID = %d)"),
					 m_dpName.m_nID,m_dpShape.m_nID,m_dpInConstr.m_nID,m_dpOutConstr.m_nID,nStandID);
}

void ParkSpotDisplayProp::DeleteData( int nStandID )
{
	DeleteBaseRecord();
	m_dpInConstr.DeleteData();
	m_dpOutConstr.DeleteData();
	CString strSQL = _T("");
	strSQL.Format(_T("DELETE FROM TB_PARKSPOTDSPPROP\
					 WHERE (OBJID = %d)"),nStandID);

	CADODatabase::ExecuteSQLStatement(strSQL);
}

void ParkSpotDisplayProp::SaveData( int nStandID )
{
	if (m_nID != -1)
	{
		return UpdateData(nStandID);
	}
	SaveBaseRecord();
	m_dpInConstr.SaveData();
	m_dpOutConstr.SaveData();
	CString strSQL = _T("");
	strSQL.Format(_T("INSERT INTO TB_PARKSPOTDSPPROP\
					 (OBJID, DPNAME, DPSHAPE, DPINCONS, DPOUTCONS)\
					 VALUES (%d,%d,%d,%d,%d)"),
					 nStandID,m_dpName.m_nID,m_dpShape.m_nID,m_dpInConstr.m_nID,m_dpOutConstr.m_nID);

	m_nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
}

void ParkSpotDisplayProp::CopyData( const ParkSpotDisplayProp& prop )
{
	ALTObjectDisplayProp::CopyData(prop);
	m_dpInConstr.CopyData(prop.m_dpInConstr);
	m_dpOutConstr.CopyData(prop.m_dpOutConstr);
}

ParkSpotDisplayProp::ParkSpotDisplayProp()
{
	m_dpInConstr = DisplayProp(true,ARCColor4(255, 120, 20 ,255));
	m_dpOutConstr =  DisplayProp(true, ARCColor4(255,255,0,255));
}
