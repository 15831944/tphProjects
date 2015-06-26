#include "stdafx.h"
#include "GoAroundCriteriaDataProcess.h"
#include "../Database/ADODatabase.h"

CGoAroundCriteriaDataProcess::CGoAroundCriteriaDataProcess(void)
{
	m_nProjID = -1;
	m_dLBLSRWDIS = 100000.0;
	m_dLBLSRWADIS = 500000.0;
	m_nLBLIRWFPTDIFF = 120;
	m_dLBLIRWFPDDIFF = 200000.0;
	m_nLBLIRWRWTDIFF = 120;
	m_dLBLIRWRWDDIFF = 200000.0;
	m_dLBTSRWDIS = 270000.0;
	m_nLBTIRWTDIFF = 120;
}

CGoAroundCriteriaDataProcess::CGoAroundCriteriaDataProcess(int nProjID)
{
	m_nProjID = nProjID;
	m_dLBLSRWDIS = 100000.0;
	m_dLBLSRWADIS = 500000.0;
	m_nLBLIRWFPTDIFF = 120;
	m_dLBLIRWFPDDIFF = 200000.0;
	m_nLBLIRWRWTDIFF = 120;
	m_dLBLIRWRWDDIFF = 200000.0;
	m_dLBTSRWDIS = 270000.0;
	m_nLBTIRWTDIFF = 120;
}

CGoAroundCriteriaDataProcess::~CGoAroundCriteriaDataProcess(void)
{
}

void CGoAroundCriteriaDataProcess::ReadData(int nProjID)
{
	CString strSQL;
	strSQL.Format(_T("SELECT * FROM GOAROUNDCRITERIA WHERE PROJID = %d;"),nProjID);		
	CADORecordset adoRecordset;
	long nRecordAfflect = 0L;
	CADODatabase::ExecuteSQLStatement(strSQL,nRecordAfflect,adoRecordset);
    double  d_val = 0 ;
	int n_val = 0 ;
	double  zero = 0 ; 
	while (!adoRecordset.IsEOF())
	{
		adoRecordset.GetFieldValue(_T("PROJID"),m_nProjID);

		adoRecordset.GetFieldValue(_T("LBLSRWDIS"),d_val);
        if(d_val != zero )
            m_dLBLSRWDIS = d_val ;
		adoRecordset.GetFieldValue(_T("LBLSRWADIS"),d_val);
		if(d_val != zero )
			m_dLBLSRWADIS = d_val ;
		adoRecordset.GetFieldValue(_T("LBLIRWFPTDIFF"),n_val);
		if(n_val != zero )
			m_nLBLIRWFPTDIFF =n_val ;
		adoRecordset.GetFieldValue(_T("LBLIRWFPDDIFF"),d_val);
		if(d_val != zero )
			m_dLBLIRWFPDDIFF = d_val ;
		adoRecordset.GetFieldValue(_T("LBLIRWRWTDIFF"),n_val);
		if(n_val != zero )
			m_nLBLIRWRWTDIFF = n_val ;
		adoRecordset.GetFieldValue(_T("LBLIRWRWDDIFF"),d_val);
		if(d_val != zero )
			m_dLBLIRWRWDDIFF = d_val ;
		adoRecordset.GetFieldValue(_T("LBTSRWDIS"),d_val);
		if(d_val != zero )
			m_dLBTSRWDIS = d_val ;
		adoRecordset.GetFieldValue(_T("LBTIRWTDIFF"),n_val);
		if(n_val != zero )
			m_nLBTIRWTDIFF = n_val ;
		adoRecordset.MoveNextData();
	}
}

void CGoAroundCriteriaDataProcess::SaveData(void)
{
	if(m_nProjID == -1)
		return;
	CString strSQL;
	strSQL.Format(_T("DELETE FROM GOAROUNDCRITERIA WHERE PROJID = %d;"),m_nProjID);
	CADODatabase::ExecuteSQLStatement(strSQL);
	strSQL.Format(_T("INSERT INTO GOAROUNDCRITERIA (PROJID, LBLSRWDIS, LBLSRWADIS, LBLIRWFPTDIFF, LBLIRWFPDDIFF,LBLIRWRWTDIFF,LBLIRWRWDDIFF,LBTSRWDIS,LBTIRWTDIFF) \
					 VALUES (%d,%f,%f,%d,%f,%d,%f,%f,%d);"),m_nProjID,m_dLBLSRWDIS,m_dLBLSRWADIS,m_nLBLIRWFPTDIFF,m_dLBLIRWFPDDIFF,m_nLBLIRWRWTDIFF,m_dLBLIRWRWDDIFF,m_dLBTSRWDIS,m_nLBTIRWTDIFF);
	CADODatabase::ExecuteSQLStatement(strSQL);
}

void CGoAroundCriteriaDataProcess::ExportGoAroundCriteriaData(CAirsideExportFile& exportFile)
{
	CGoAroundCriteriaDataProcess goaroundCriteria(exportFile.GetProjectID());	
	goaroundCriteria.ReadData(exportFile.GetProjectID());	
	goaroundCriteria.ExportData(exportFile);
}

void CGoAroundCriteriaDataProcess::ImportGoAroundCriteriaData(CAirsideImportFile& importFile)
{
	CGoAroundCriteriaDataProcess goaroundCriteria;	
	while(!importFile.getFile().isBlank())
	{
		goaroundCriteria.ImportData(importFile);
	}
	goaroundCriteria.SaveData();
}

void CGoAroundCriteriaDataProcess::ImportData(CAirsideImportFile& importFile)
{
	m_nProjID = importFile.getNewProjectID();
	int nOldProjID = -1;
	importFile.getFile().getInteger(nOldProjID);
	importFile.getFile().getFloat(m_dLBLSRWDIS);
	importFile.getFile().getFloat(m_dLBLSRWADIS);
	importFile.getFile().getInteger(m_nLBLIRWFPTDIFF);
	importFile.getFile().getFloat(m_dLBLIRWFPDDIFF);
	importFile.getFile().getInteger(m_nLBLIRWRWTDIFF);
	importFile.getFile().getFloat(m_dLBLIRWRWDDIFF);
	importFile.getFile().getFloat(m_dLBTSRWDIS);
	importFile.getFile().getInteger(m_nLBTIRWTDIFF);
	importFile.getFile().getLine();
}

void CGoAroundCriteriaDataProcess::ExportData(CAirsideExportFile& exportFile)
{	
	exportFile.getFile().writeField(_T("ID,PROJID,LBLSRWDIS,LBLSRWADIS,LBLIRWFPTDIFF,LBLIRWFPDDIFF,LBLIRWRWTDIFF,LBLIRWRWDDIFF,LBTSRWDIS,LBTIRWTDIFF"));
	exportFile.getFile().writeLine();
	exportFile.getFile().writeInt(m_nProjID);
	exportFile.getFile().writeDouble(m_dLBLSRWDIS);
	exportFile.getFile().writeDouble(m_dLBLSRWADIS);
	exportFile.getFile().writeInt(m_nLBLIRWFPTDIFF);
	exportFile.getFile().writeDouble(m_dLBLIRWFPDDIFF);
	exportFile.getFile().writeInt(m_nLBLIRWRWTDIFF);
	exportFile.getFile().writeDouble(m_dLBLIRWRWDDIFF);
	exportFile.getFile().writeDouble(m_dLBTSRWDIS);
	exportFile.getFile().writeInt(m_nLBTIRWTDIFF);	
	exportFile.getFile().writeLine();
	exportFile.getFile().endFile();	
}

int CGoAroundCriteriaDataProcess::GetProjID(void) const
{
	return (m_nProjID);
}

void CGoAroundCriteriaDataProcess::SetProjID(int nProjID)
{
	m_nProjID = nProjID;
}

double CGoAroundCriteriaDataProcess::GetLBLSRWDIS(void) const
{
	return (m_dLBLSRWDIS);
}

void CGoAroundCriteriaDataProcess::SetLBLSRWDIS(double dLBLSRWDIS)
{
	m_dLBLSRWDIS = dLBLSRWDIS;
}

double CGoAroundCriteriaDataProcess::GetLBLSRWADIS(void) const
{
	return (m_dLBLSRWADIS);
}

void CGoAroundCriteriaDataProcess::SetLBLSRWADIS(double dLBLSRWADIS)
{
	m_dLBLSRWADIS = dLBLSRWADIS;
}

int CGoAroundCriteriaDataProcess::GetLBLIRWFPTDIFF(void) const
{
	return (m_nLBLIRWFPTDIFF);
}

void CGoAroundCriteriaDataProcess::SetLBLIRWFPTDIFF(int nLBLIRWFPTDIFF)
{
	m_nLBLIRWFPTDIFF = nLBLIRWFPTDIFF;
}

double CGoAroundCriteriaDataProcess::GetLBLIRWFPDDIFF(void) const
{
	return (m_dLBLIRWFPDDIFF);
}

void CGoAroundCriteriaDataProcess::SetLBLIRWFPDDIFF(double dLBLIRWFPDDIFF)
{
	m_dLBLIRWFPDDIFF = dLBLIRWFPDDIFF;
}

int CGoAroundCriteriaDataProcess::GetLBLIRWRWTDIFF(void) const
{
	return (m_nLBLIRWRWTDIFF);
}

void CGoAroundCriteriaDataProcess::SetLBLIRWRWTDIFF(int nLBLIRWRWTDIFF)
{
	m_nLBLIRWRWTDIFF = nLBLIRWRWTDIFF;
}

double CGoAroundCriteriaDataProcess::GetLBLIRWRWDDIFF(void) const
{
	return (m_dLBLIRWRWDDIFF);
}

void CGoAroundCriteriaDataProcess::SetLBLIRWRWDDIFF(double dLBLIRWRWDDIFF)
{
	m_dLBLIRWRWDDIFF = dLBLIRWRWDDIFF;
}

double CGoAroundCriteriaDataProcess::GetLBTSRWDIS(void) const
{
	return (m_dLBTSRWDIS);
}

void CGoAroundCriteriaDataProcess::SetLBTSRWDIS(double dLBTSRWDIS)
{
	m_dLBTSRWDIS = dLBTSRWDIS;
}

int CGoAroundCriteriaDataProcess::GetLBTIRWTDIFF(void) const
{
	return (m_nLBTIRWTDIFF);
}

void CGoAroundCriteriaDataProcess::SetLBTIRWTDIFF(int nLBTIRWTDIFF)
{
	m_nLBTIRWTDIFF = nLBTIRWTDIFF;
}
