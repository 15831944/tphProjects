#include "stdafx.h"
#include "./AircraftModel.h"
#include "../Database/ARCportADODatabase.h"
#include "../Common/AirportDatabase.h"
#include <common\ARCStringConvert.h>
#include <common\ZipInfo.h>
#include "AircraftModelDatabase.h"
#include <common\tinyxml/tinyxml.h>
#include "common\Aircraft.h"

#define STR_TBCOL_ID _T("ID")
#define STR_TBCOL_NAME _T("NAME")
#define STR_TBCOL_FILEID _T("FILE_ID")


void ModelPro::DBReadData( CADORecordset& adoRecordset )
{
	adoRecordset.GetFieldValue(STR_TBCOL_ID, m_nID);
	adoRecordset.GetFieldValue(STR_TBCOL_NAME, m_sModelName);	
	adoRecordset.GetFieldValue(STR_TBCOL_FILEID, m_nFileID);
}

void ModelPro::DBSaveData( AirportDatabaseConnection* pConn )
{	
	//mFile.Save(pConn);
	CString strSQL;
	if (m_nID>0) // update
	{
		strSQL.Format(_T("UPDATE %s SET %s = '%s', %s = %d WHERE %s = %d")
			, STR_AC_MODL_DBTABLE
			, STR_TBCOL_NAME, m_sModelName.GetString()
			, STR_TBCOL_FILEID, m_nFileID
			, STR_TBCOL_ID, m_nID
			);
		CDatabaseADOConnetion::ExecuteSQLStatement(strSQL, pConn);
	}
	else
	{
		strSQL.Format(_T("INSERT INTO %s(%s, %s) VALUES('%s', %d)")
			, STR_AC_MODL_DBTABLE
			, STR_TBCOL_NAME, STR_TBCOL_FILEID
			, m_sModelName.GetString(), m_nFileID
			);
		m_nID = CDatabaseADOConnetion::ExecuteSQLStatementAndReturnScopeID(strSQL, pConn);
	}
}

void ModelPro::DBDeleteData( AirportDatabaseConnection* pConn )
{
	if (m_nID>0)
	{
		CString strSQL;
		strSQL.Format(_T("DELETE * FROM %s WHERE %s = %d")
			,STR_AC_MODL_DBTABLE
			,STR_TBCOL_ID,m_nID);
		CDatabaseADOConnetion::ExecuteSQLStatement(strSQL, pConn);
		CFileInDB::Delete(m_nFileID, pConn );		
	}
}

ModelPro::ModelPro()
{
}

CAircraftModel::CAircraftModel( CAircraftModelDatabase* pDB )
:mpParent(pDB)
{	
	InitDefault();
}

#define STR_MODEL _T("ACAircraftModel")
#define CurrentVersion 100
#define FILE_EXTNSION _T("aam") //arc aircraft model



int CAircraftModel::GetCurrentVersion()const
{
	return CurrentVersion;
}

const CString CAircraftModel::GetFileName()const
{
	return STR_MODEL;
}

const CString CAircraftModel::GetFileExtnsion()const
{
	return FILE_EXTNSION;
}
	


#define EditModelCamera _T("EditModelCamera")
void CAircraftModel::InitDefault()
{
	mActiveCam.InitDefault(CCameraData::perspective);
	

	mGrid.bVisibility = TRUE;
	mGrid.cAxesColor = RGB(0,0,0);
	mGrid.cLinesColor = RGB(88,88,88);
	mGrid.cSubdivsColor = RGB(140,140,140);
	mGrid.dSizeX = 10000.0;
	mGrid.dSizeY = 10000.0;
	mGrid.dSizeZ = 10000.0;
	mGrid.dLinesEvery = 200.0;
	mGrid.nSubdivs = 0;
}


void CAircraftModel::SaveData()
{	
	if(mpParent)
	{
		if(!MakeSureFileOpen())
			return;
		SaveContent();		
		SaveFile();
		SaveDataToDB();
	}
}

void CAircraftModel::DeleteData()
{
	DBDeleteData(mpParent->GetConnection());
}
void CAircraftModel::ReadData()
{
	ASSERT(mpParent);
	if(!MakeSureFileOpen())return;
	ReadContent();
}

void CAircraftModel::SaveDataToDB()
{
	DBSaveData(mpParent->GetConnection());
}

AirportDatabaseConnection* CAircraftModel::GetConnection()
{
	return mpParent->GetConnection();
}

void CAircraftModel::SetDimension( const ARCVector3& dim )
{
	if(mpRelateACType)
	{
		mpRelateACType->m_fHeight = static_cast<float>(dim[VZ]/100);
		mpRelateACType->m_fLen = static_cast<float>(dim[VX]/100);
		mpRelateACType->m_fSpan = static_cast<float>(dim[VY]/100);
	}
	__super::SetDimension(dim);
}
//
