#include "StdAfx.h"
#include "ConflictResolution.h"
#include "..\InputAirside\ConflictAtIntersectionOnRight.h"
#include "..\InputAirside\ConflictFirstInATaxiway.h"
#include "..\InputAirside\ConflictOnSpecificTaxiways.h"
#include "AirsideImportExportManager.h"
#include "..\common\AirportDatabase.h"
#include "..\Database\DBElementCollection_Impl.h"
#include "Common\SqlScriptMaker.h"
#include "Common\CARCUnit.h"


CConflictResolution::CConflictResolution()
:m_nID(-1)
,m_nRadiusOfConcern(2000)
,m_enumAtIntersectionOnRight(Yes)
,m_enumFirstInATaxiway(Yes)
,m_enumOnSpecificTaxiways(No)
,m_nRunwayCrossBuffer(60)
{
	m_pAtIntersectionOnRightList = new CAtIntersectionOnRightList();
	m_pFirstInATaxiwayList = new CFirstInATaxiwayList();
	m_pOnSpecificTaxiwaysList = new COnSpecificTaxiwaysList();

	setRunwayAsTaxiwayApproachDistNM(10);
	m_tRunwayAsTaxiwayNoApporachTime = ElapsedTime(3600L);
	m_tRuwnayAsTaxiwayNoTakeoffTime = ElapsedTime(3600L);
}

CConflictResolution::~CConflictResolution(void)
{
	delete m_pAtIntersectionOnRightList;
	delete m_pFirstInATaxiwayList;
	delete m_pOnSpecificTaxiwaysList;
}
void CConflictResolution::SetRunwayCrossBuffer(int nSeconds)
{	
	ASSERT(nSeconds >= 0);
	m_nRunwayCrossBuffer = nSeconds;
}
int  CConflictResolution::GetRunwayCrossBuffer()
{
	return m_nRunwayCrossBuffer;
}
void CConflictResolution::ReadData(int nProjID)
{
	SqlSelectScriptMaker selectSql(TB_CONFLICTRESOLUTION);
	selectSql.SetCondition( SqlScriptColumn(COL_PROJID, nProjID).EqualCondition() );
	
	
	long lRecordCount = 0;
	CADORecordset adoRecordset;

	CADODatabase::ExecuteSQLStatement(selectSql.GetScript(),lRecordCount,adoRecordset);
	while(!adoRecordset.IsEOF())
	{		
		adoRecordset.GetFieldValue(COL_ID,m_nID);
		adoRecordset.GetFieldValue(COL_RADIUSOFCONCERN,m_nRadiusOfConcern);
		int nAtIntersectionOnRight;
		adoRecordset.GetFieldValue(COL_ENUMATINTERSECTIONONRIGHT,nAtIntersectionOnRight);
		m_enumAtIntersectionOnRight = (YesOrNo)nAtIntersectionOnRight;
		int nFirstInATaxiway;
		adoRecordset.GetFieldValue(COL_ENUMFIRSTINATAXIWAY,nFirstInATaxiway);
		m_enumFirstInATaxiway = (YesOrNo)nFirstInATaxiway;
		int nOnSpecificTaxiways;
		adoRecordset.GetFieldValue(COL_ENUMONSPECIFICTAXIWAYS,nOnSpecificTaxiways);
		m_enumOnSpecificTaxiways = (YesOrNo)nOnSpecificTaxiways;
		
		adoRecordset.GetFieldValue(COL_RUNWAYCROSSBUFFERTIME, m_nRunwayCrossBuffer);

		long nTime;
		adoRecordset.GetFieldValue(COL_RUNAYASTAXIWAY_NOAPPROACHDIST, m_dRunwayAsTaxiwayNoApporachDistance);
		adoRecordset.GetFieldValue(COL_RUNAYASTAXIWAY_NOAPPROACHTIME, nTime); m_tRunwayAsTaxiwayNoApporachTime.setPrecisely(nTime);
		adoRecordset.GetFieldValue(COL_RUNWAYASTAXIWAY_NOTAKEOFFTIME, nTime); m_tRuwnayAsTaxiwayNoTakeoffTime.setPrecisely(nTime);

		adoRecordset.MoveNextData();
	}
	m_pAtIntersectionOnRightList->SetAirportDB(m_pAirportDB);
	m_pAtIntersectionOnRightList->ReadData(m_nID);
	m_pFirstInATaxiwayList->SetAirportDB(m_pAirportDB);
	m_pFirstInATaxiwayList->ReadData(m_nID);
	m_pOnSpecificTaxiwaysList->SetAirportDB(m_pAirportDB);
	m_pOnSpecificTaxiwaysList->ReadData(m_nID);
	m_vCrossBuffers.SetAirportDB(m_pAirportDB);
	m_vCrossBuffers.ReadData(m_nID);

}

void CConflictResolution::SaveData(int nProjID)
{
	if(m_nID < 0)
	{
		SqlInsertScriptMaker insertScript(TB_CONFLICTRESOLUTION);
		insertScript.AddColumn(COL_PROJID, nProjID);
		insertScript.AddColumn(COL_RADIUSOFCONCERN, m_nRadiusOfConcern);
		insertScript.AddColumn(COL_ENUMATINTERSECTIONONRIGHT, (int)m_enumAtIntersectionOnRight);
		insertScript.AddColumn(COL_ENUMFIRSTINATAXIWAY, (int)m_enumFirstInATaxiway);
		insertScript.AddColumn(COL_ENUMONSPECIFICTAXIWAYS, (int)m_enumOnSpecificTaxiways);
		insertScript.AddColumn(COL_RUNWAYCROSSBUFFERTIME, m_nRunwayCrossBuffer);

		insertScript.AddColumn(COL_RUNAYASTAXIWAY_NOAPPROACHTIME, m_tRunwayAsTaxiwayNoApporachTime.getPrecisely() );
		insertScript.AddColumn(COL_RUNAYASTAXIWAY_NOAPPROACHDIST,  m_dRunwayAsTaxiwayNoApporachDistance );
		insertScript.AddColumn(COL_RUNWAYASTAXIWAY_NOTAKEOFFTIME, m_tRuwnayAsTaxiwayNoTakeoffTime.getPrecisely() );

		m_nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(insertScript.GetScript());

	}
	else
	{
		SqlUpdateScriptMaker updateScript(TB_CONFLICTRESOLUTION);
		
		updateScript.AddColumn(COL_RADIUSOFCONCERN, m_nRadiusOfConcern);
		updateScript.AddColumn(COL_ENUMATINTERSECTIONONRIGHT, (int)m_enumAtIntersectionOnRight);
		updateScript.AddColumn(COL_ENUMFIRSTINATAXIWAY, (int)m_enumFirstInATaxiway);
		updateScript.AddColumn(COL_ENUMONSPECIFICTAXIWAYS, (int)m_enumOnSpecificTaxiways);
		updateScript.AddColumn(COL_RUNWAYCROSSBUFFERTIME, m_nRunwayCrossBuffer);


		updateScript.AddColumn(COL_RUNAYASTAXIWAY_NOAPPROACHTIME, m_tRunwayAsTaxiwayNoApporachTime.getPrecisely() );
		updateScript.AddColumn(COL_RUNAYASTAXIWAY_NOAPPROACHDIST,  m_dRunwayAsTaxiwayNoApporachDistance );
		updateScript.AddColumn(COL_RUNWAYASTAXIWAY_NOTAKEOFFTIME, m_tRuwnayAsTaxiwayNoTakeoffTime.getPrecisely() );


		CString strCond = SqlScriptColumn(COL_ID, m_nID).EqualCondition();
		updateScript.SetCondition(strCond);

		CADODatabase::ExecuteSQLStatement(updateScript.GetScript());
		

	}
	m_pAtIntersectionOnRightList->SaveData(m_nID);
	m_pFirstInATaxiwayList->SaveData(m_nID);
	m_pOnSpecificTaxiwaysList->SaveData(m_nID);
	m_vCrossBuffers.SaveData(m_nID);
}

void CConflictResolution::ExportConflictResolution(CAirsideExportFile& exportFile,CAirportDatabase* pAirportDB)
{
	CConflictResolution conflictResolution;
	conflictResolution.SetAirportDB(pAirportDB);
	conflictResolution.ReadData(exportFile.GetProjectID());
	conflictResolution.ExportData(exportFile);

	exportFile.getFile().endFile();

}
void CConflictResolution::ExportData(CAirsideExportFile& exportFile)
{
	exportFile.getFile().writeField("ConflictResolution");
	exportFile.getFile().writeLine();

	exportFile.getFile().writeInt(m_nID);
	exportFile.getFile().writeInt(exportFile.GetProjectID());
	exportFile.getFile().writeInt(m_nRadiusOfConcern);
	exportFile.getFile().writeInt((int)m_enumAtIntersectionOnRight);
	exportFile.getFile().writeInt((int)m_enumFirstInATaxiway);
	exportFile.getFile().writeInt((int)m_enumOnSpecificTaxiways);
	exportFile.getFile().writeLine();

	m_pAtIntersectionOnRightList->ExportCAtIntersectionOnRights(exportFile);
	m_pFirstInATaxiwayList->ExportCFirstInATaxiways(exportFile);
	m_pOnSpecificTaxiwaysList->ExportCOnSpecificTaxiwayss(exportFile);
}
void CConflictResolution::ImportConflictResolution(CAirsideImportFile& importFile)
{
	CConflictResolution conflictResolution;
	conflictResolution.ImportData(importFile);
}
void CConflictResolution::ImportData(CAirsideImportFile& importFile)
{
	int nOldID = -1;
	importFile.getFile().getInteger(nOldID);
	int nOldProjID = -1;
	importFile.getFile().getInteger(nOldProjID);

	importFile.getFile().getInteger(m_nRadiusOfConcern);

	int nAtIntersectionOnRight;
	importFile.getFile().getInteger(nAtIntersectionOnRight);
	m_enumAtIntersectionOnRight = (YesOrNo)nAtIntersectionOnRight;

	int nFirstInATaxiway;
	importFile.getFile().getInteger(nFirstInATaxiway);
	m_enumFirstInATaxiway = (YesOrNo)nFirstInATaxiway;

	int nOnSpecificTaxiways;
	importFile.getFile().getInteger(nOnSpecificTaxiways);
	m_enumOnSpecificTaxiways = (YesOrNo)nOnSpecificTaxiways;

	importFile.getFile().getLine();

	SaveData(importFile.getNewProjectID());
	m_pAtIntersectionOnRightList->SetConflictResolutionID(m_nID);
	m_pAtIntersectionOnRightList->ImportCAtIntersectionOnRights(importFile);

	m_pFirstInATaxiwayList->SetConflictResolutionID(m_nID);
	m_pFirstInATaxiwayList->ImportCFirstInATaxiways(importFile);

	m_pOnSpecificTaxiwaysList->SetConflictResolutionID(m_nID);
	m_pOnSpecificTaxiwaysList->ImportCOnSpecificTaxiwayss(importFile);
}

DistanceUnit CConflictResolution::getRunwayAsTaxiwayApproachDistNM() const
{
	return CARCLengthUnit::ConvertLength( DEFAULT_DATABASE_LENGTH_UNIT,AU_LENGTH_NAUTICALMILE, m_dRunwayAsTaxiwayNoApporachDistance);
}

void CConflictResolution::setRunwayAsTaxiwayApproachDistNM( DistanceUnit d )
{
	m_dRunwayAsTaxiwayNoApporachDistance  = CARCLengthUnit::ConvertLength( AU_LENGTH_NAUTICALMILE,DEFAULT_DATABASE_LENGTH_UNIT, d);
}

void RunwayCrossBuffer::InitFromDBRecordset( CADORecordset& recordset )
{
	
	long lint;
	recordset.GetFieldValue(COL_ID, m_nID);
	recordset.GetFieldValue(COL_FLTCONST, m_sfltType, false);

	recordset.GetFieldValue(COL_BUFFERDISTANCE, m_distance);
	recordset.GetFieldValue(COL_BUFFERTIME, lint );
	recordset.GetFieldValue(COL_CASE, (int&)m_case);
	
	m_time.setPrecisely(lint);
}



void RunwayCrossBuffer::Init( CAirportDatabase* p )
{
	m_fltType.SetAirportDB(p);
	m_fltType.setConstraintWithVersion(m_sfltType);
}

void RunwayCrossBuffer::GetUpdateSQL( CString& strSQL ) const
{
	SqlUpdateScriptMaker update(TB_RUNWAYCROSSBUFF);
	CString str;
	m_fltType.getSerialize(str);
	update.AddColumn(COL_FLTCONST, str);
	update.AddColumn(COL_BUFFERDISTANCE, m_distance);
	update.AddColumn(COL_BUFFERTIME, m_time.getPrecisely());
	update.AddColumn(COL_CASE, m_case);

	update.SetCondition( SqlScriptColumn(COL_ID, m_nID).EqualCondition() );
	strSQL = update.GetScript();
}

void RunwayCrossBuffer::GetInsertSQL( int nParentID,CString& strSQL ) const
{
	SqlInsertScriptMaker insert(TB_RUNWAYCROSSBUFF);
	CString str;
	m_fltType.getSerialize(str);
	insert.AddColumn(COL_FLTCONST,str);
	insert.AddColumn(COL_BUFFERDISTANCE, m_distance);
	insert.AddColumn(COL_BUFFERTIME, m_time.getPrecisely());
	insert.AddColumn(COL_PARENTID, nParentID);
	insert.AddColumn(COL_CASE, m_case);
	strSQL = insert.GetScript();
}

void RunwayCrossBuffer::GetDeleteSQL( CString& strSQL ) const
{
	SqlDeleteScriptMaker del(TB_RUNWAYCROSSBUFF);
	del.SetCondition(SqlScriptColumn(COL_ID, m_nID).EqualCondition());
	strSQL = del.GetScript();
}

void RunwayCrossBuffer::GetSelectSQL( int nID,CString& strSQL ) const
{
	SqlSelectScriptMaker sel(TB_RUNWAYCROSSBUFF);
	sel.SetCondition(SqlScriptColumn(COL_ID,nID).EqualCondition());
	strSQL = sel.GetScript();
}

RunwayCrossBuffer::RunwayCrossBuffer()
{
	m_distance = CARCLengthUnit::ConvertLength(AU_LENGTH_NAUTICALMILE, DEFAULT_DATABASE_LENGTH_UNIT, 4);
	m_time  = ElapsedTime(40L);
	m_case = _Landing;
}

DistanceUnit RunwayCrossBuffer::getDistanceNM() const
{
	return CARCLengthUnit::ConvertLength( DEFAULT_DATABASE_LENGTH_UNIT,AU_LENGTH_NAUTICALMILE, m_distance);
}

void RunwayCrossBuffer::setDistanceNM( DistanceUnit d )
{
	m_distance = CARCLengthUnit::ConvertLength( AU_LENGTH_NAUTICALMILE,DEFAULT_DATABASE_LENGTH_UNIT, d);
}

void RunwayCrossBufferList::GetSelectElementSQL( int nParentID,CString& strSQL ) const
{
	SqlSelectScriptMaker sql(TB_RUNWAYCROSSBUFF);
	sql.SetCondition( SqlScriptColumn(COL_PARENTID, nParentID).EqualCondition() );
	strSQL = sql.GetScript();
}

void RunwayCrossBufferList::ReadData( int nParentID )
{
	__super::ReadData(nParentID);
	for(size_t i=0;i< this->GetElementCount();++i)
	{
		GetItem(i)->Init(m_pAirportDB);
	}
}
