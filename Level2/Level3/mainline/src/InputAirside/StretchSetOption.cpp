#include "StdAfx.h"
#include "StretchSetOption.h"
#include "..\Database\ADODatabase.h"

CStretchSetOption::CStretchSetOption()
{
	m_nID = -1;
	m_nProjID = -1;
	m_strDrive = 1;
	m_dLaneWidth = 4;
	m_iLaneNumber = 2;
	
}

CStretchSetOption::CStretchSetOption(int nProjID)
{
	m_nID = -1;
	m_nProjID = nProjID;
	m_strDrive = 1;
	m_dLaneWidth = 4;
	m_iLaneNumber = 2;
}

CStretchSetOption::~CStretchSetOption()
{
}

void CStretchSetOption::InitFromDBRecordset(CADORecordset& recordset)
{
	if (!recordset.IsEOF())
	{
		recordset.GetFieldValue(_T("ID"), m_nID);
        recordset.GetFieldValue(_T("PROJID"), m_nProjID);
		recordset.GetFieldValue(_T("DRIVE"),m_strDrive);
		recordset.GetFieldValue(_T("LANEWIDTH"),m_dLaneWidth);
		recordset.GetFieldValue(_T("NUMBEROFLANES"),m_iLaneNumber);
	}
}

void CStretchSetOption::GetInsertSQL(CString& strSQL) const
{	
		strSQL.Format(_T("INSERT INTO STRETCHOPTION(PROJID,DRIVE, LANEWIDTH, NUMBEROFLANES) VALUES (%d,%d,%f,%d)"),m_nProjID,m_strDrive,m_dLaneWidth,m_iLaneNumber);
	
}

void  CStretchSetOption::GetUpdateSQL(CString& strSQL) const
{
	strSQL.Format(_T("UPDATE STRETCHOPTION SET PROJID = %d,DRIVE = %d,LANEWIDTH = %f,NUMBEROFLANES = %d\
					 WHERE ID= %d"),m_nProjID,m_strDrive,m_dLaneWidth,m_iLaneNumber,m_nID);

}

void  CStretchSetOption::GetDeleteSQL(CString& strSQL) const
{
	strSQL.Format(_T("DELETE FROM  STRETCHOPTION \
					 WHERE (ID = %d);"),m_nID);

}

void  CStretchSetOption::GetSelectSQL(int nID,CString& strSQL)const
{
	strSQL.Format(_T("SELECT * FROM  STRETCHOPTION \
					 WHERE (PROJID = %d);"),nID);

}

void CStretchSetOption::ImportData(CAirsideImportFile& importFile)
{
	m_nProjID = importFile.getNewProjectID();
	int nOldProjID = -1;
	importFile.getFile().getInteger(m_nID);
	importFile.getFile().getInteger(nOldProjID);
	importFile.getFile().getInteger(m_strDrive);
	importFile.getFile().getFloat(m_dLaneWidth);
	importFile.getFile().getInteger(m_iLaneNumber);
	importFile.getFile().getLine();
}

void CStretchSetOption::ExportData(CAirsideExportFile& exportFile)
{
	exportFile.getFile().writeField(_T("ID,PROJID,DRIVE,LANEWIDTH,NUMBEROFLANES"));
	exportFile.getFile().writeLine();
    exportFile.getFile().writeInt(m_nID);
	exportFile.getFile().writeInt(m_nProjID);
	exportFile.getFile().writeInt(m_strDrive);
	exportFile.getFile().writeDouble(m_dLaneWidth);
	exportFile.getFile().writeInt(m_iLaneNumber);	
	exportFile.getFile().writeLine();
	exportFile.getFile().endFile();	
}

void CStretchSetOption::ExportStretchSetOption(CAirsideExportFile& exportFile)
{
	CStretchSetOption stretchsetOption(exportFile.GetProjectID());	
	stretchsetOption.ReadData(exportFile.GetProjectID());	
	stretchsetOption.ExportData(exportFile);
}

void CStretchSetOption::ImportStretchSetOption(CAirsideImportFile& importFile)
{
	CStretchSetOption stretchsetOption;	
	while(!importFile.getFile().isBlank())
	{
		stretchsetOption.ImportData(importFile);
	}
	stretchsetOption.SaveDatas();
}

void CStretchSetOption::ReadData(int nID)
{
	CString strSelectSQL;
	GetSelectSQL(nID,strSelectSQL);
	strSelectSQL.Trim();
	if (strSelectSQL.IsEmpty())
	{
		CString strSQL;
		strSQL.Format(_T("INSERT INTO STRETCHOPTION(PROJID,DRIVE, LANEWIDTH, NUMBEROFLANES) VALUES (%d,%d,%f,%d)"),m_nProjID,1,4,2);
		return;
	}

	long nRecordCount = -1;
	CADORecordset adoRecordset;
	CADODatabase::ExecuteSQLStatement(strSelectSQL, nRecordCount, adoRecordset);

	if (!adoRecordset.IsEOF())
	{
		InitFromDBRecordset(adoRecordset);	
	}

}

void CStretchSetOption::SaveDatas()
{

	CString strSQL;
	try
	{
		CADODatabase::BeginTransaction() ;
		if (m_nID < 0)
		{
			GetInsertSQL(strSQL);

			if (strSQL.IsEmpty())
				return;

			m_nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
		}
		else
		{
			GetUpdateSQL(strSQL);
			if (strSQL.IsEmpty())
				return;

			CADODatabase::ExecuteSQLStatement(strSQL);
		}
		CADODatabase::CommitTransaction() ;
	}
	catch (CADOException e)
	{
		CADODatabase::RollBackTransation() ;
	}
	
}



int CStretchSetOption::GetProjID(void) const
{
	return (m_nProjID);
}

void CStretchSetOption::SetProjID(int nProjID)
{
	m_nProjID = nProjID;
}


void CStretchSetOption::SetDrive(int strDrive)
{
	m_strDrive = strDrive;
}

int CStretchSetOption::GetDrive(void) const
{
	return m_strDrive;
}

void CStretchSetOption::SetLaneWidth(double dLaneWidth)
{
	m_dLaneWidth = dLaneWidth;
}

double CStretchSetOption::GetLaneWidth(void) const
{
	return m_dLaneWidth;
}

void CStretchSetOption::SetLaneNumber(int iLaneNumber)
{
	m_iLaneNumber = iLaneNumber;
}

int CStretchSetOption::GetLaneNumber(void) const
{
	return m_iLaneNumber;
}
