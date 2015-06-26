#include "StdAfx.h"
#include "../Database/DBElementCollection_Impl.h"
#include "DirectionalityConstraints.h"
#include "..\Database\ADODatabase.h"
#include "AirsideImportExportManager.h"

//----------------------------------------------------------------------------------
//DirectionalityConstraintItem
DirectionalityConstraintItem::DirectionalityConstraintItem(void)
:m_nAirportID(-1)
,m_nTaxiwayID(-1)
,m_nSegFromID(-1)
,m_nSegToID(-1)
,m_dBearingDegree(0.0)
,m_emType(Direction_Bearing)
{
}

DirectionalityConstraintItem::~DirectionalityConstraintItem(void)
{

}

int DirectionalityConstraintItem::GetTaxiwayID()
{
	return m_nTaxiwayID;
}

void DirectionalityConstraintItem::SetTaxiwayID(int nID)
{
	m_nTaxiwayID = nID;
}

int DirectionalityConstraintItem::GetSegFromID()
{
	return m_nSegFromID;
}

void DirectionalityConstraintItem::SetSegFromID(int nFromID)
{
	m_nSegFromID = nFromID;
}

int DirectionalityConstraintItem::GetSegToID()
{
	return m_nSegToID;
}

void DirectionalityConstraintItem::SetSegToID(int nToID)
{
	m_nSegToID = nToID;
}

double DirectionalityConstraintItem::GetBearingDegree()
{
	return m_dBearingDegree;
}

void DirectionalityConstraintItem::SetBearingDegree(double degree)
{
	m_dBearingDegree = degree;
}


void DirectionalityConstraintItem::SetDirConsType(DirectionalityConstraintType emType)
{
	m_emType = emType;
}

DirectionalityConstraintType DirectionalityConstraintItem::GetDirConsType()
{
	return m_emType;
}

void DirectionalityConstraintItem::InitFromDBRecordset(CADORecordset& adoRecordset)
{
	adoRecordset.GetFieldValue(_T("ID"), m_nID);
	adoRecordset.GetFieldValue(_T("AIRPORTID"), m_nAirportID);

	adoRecordset.GetFieldValue(_T("TAXIWAYID"), m_nTaxiwayID);
	adoRecordset.GetFieldValue(_T("SEGFROMID"), m_nSegFromID);
	adoRecordset.GetFieldValue(_T("SEGTOID"), m_nSegToID);
	adoRecordset.GetFieldValue(_T("BEARING"), m_dBearingDegree);
	int nType;
	adoRecordset.GetFieldValue(_T("TYPE"), nType);

	if (nType > 1)
		m_emType = Direction_Bearing;
	else
		m_emType = (DirectionalityConstraintType)nType;
}

void DirectionalityConstraintItem::GetInsertSQL(int nParentID, CString& strSQL) const
{
	strSQL.Empty();
	strSQL.Format(_T("INSERT INTO IN_DERECTIONALITYCONSTRAINTS\
					 (AIRPORTID, TAXIWAYID, TYPE, SEGFROMID, SEGTOID, BEARING)\
					 VALUES(%d,%d,%d, %d,%d,%f)"),
					 nParentID, m_nTaxiwayID, (int)m_emType, m_nSegFromID, m_nSegToID, m_dBearingDegree);
}

void DirectionalityConstraintItem::GetUpdateSQL(CString& strSQL) const
{
	strSQL.Empty();
	strSQL.Format(_T("UPDATE IN_DERECTIONALITYCONSTRAINTS\
					 SET  AIRPORTID=%d, TAXIWAYID=%d, TYPE=%d, SEGFROMID=%d,SEGTOID=%d,BEARING=%f\
					 WHERE (ID = %d)"),
					 m_nAirportID, m_nTaxiwayID, (int)m_emType, m_nSegFromID, m_nSegToID, m_dBearingDegree, m_nID);
}

void DirectionalityConstraintItem::GetDeleteSQL(CString& strSQL) const
{
	strSQL.Empty();
	strSQL.Format(_T("DELETE FROM IN_DERECTIONALITYCONSTRAINTS\
					 WHERE (ID = %d)"),m_nID);
}

void DirectionalityConstraintItem::GetSelectSQL(int nID,CString& strSQL)const
{
	strSQL.Empty();
	strSQL.Format(_T("SELECT * FROM IN_DERECTIONALITYCONSTRAINTS WHERE (ID = %d)"),nID);
}

void DirectionalityConstraintItem::ExportData(CAirsideExportFile& exportFile)
{
	exportFile.getFile().writeInt(m_nID);
	exportFile.getFile().writeInt(m_nAirportID);
	exportFile.getFile().writeInt(m_nTaxiwayID);
	exportFile.getFile().writeInt( (int)m_emType );
	exportFile.getFile().writeInt(m_nSegFromID);
	exportFile.getFile().writeInt(m_nSegToID);
	exportFile.getFile().writeDouble(m_dBearingDegree);
}

void DirectionalityConstraintItem::ImportData(CAirsideImportFile& importFile)
{
	int nID = -1;
	importFile.getFile().getInteger(nID);
	importFile.getFile().getInteger(m_nAirportID);
	
	importFile.getFile().getInteger(m_nTaxiwayID);
	m_nTaxiwayID = importFile.GetObjectNewID(m_nTaxiwayID);
	int nType = 0;
	importFile.getFile().getInteger(nType);
	m_emType = (DirectionalityConstraintType)nType;

	importFile.getFile().getInteger(m_nSegFromID);
	m_nSegFromID = importFile.GetIntersectionIndexMap(m_nSegFromID);
	importFile.getFile().getInteger(m_nSegToID);
	m_nSegToID = importFile.GetIntersectionIndexMap(m_nSegToID);

	importFile.getFile().getFloat(m_dBearingDegree);

	int nAirportID = importFile.GetAirportNewIndex(m_nAirportID);
	SaveData(nAirportID);
}


//----------------------------------------------------------------------------------
//DirectionalityConstraints
DirectionalityConstraints::DirectionalityConstraints(void)
{
}

DirectionalityConstraints::~DirectionalityConstraints(void)
{
}

void DirectionalityConstraints::ReadData(int nParentID)
{
	CString strSQL;
	GetSelectElementSQL(nParentID, strSQL);
	
	long nRecordCount = -1;
	CADORecordset adoRecordset;
	CADODatabase::ExecuteSQLStatement(strSQL, nRecordCount, adoRecordset);
	
	while (!adoRecordset.IsEOF())
	{
		DirectionalityConstraintItem* element = new DirectionalityConstraintItem;
		element->InitFromDBRecordset(adoRecordset);
		m_dataList.push_back(element);
		adoRecordset.MoveNextData();
	}

}
void DirectionalityConstraints::GetSelectElementSQL(int nParentID,CString& strSQL) const
{
	strSQL.Empty();
	strSQL.Format(_T("SELECT * FROM  IN_DERECTIONALITYCONSTRAINTS\
					 WHERE (AIRPORTID = %d)"),nParentID);
}

void DirectionalityConstraints::ExportDirectionalityConstraints(CAirsideExportFile& exportFile)
{
	exportFile.getFile().writeField(_T("DirectionalityConstraints"));
	exportFile.getFile().writeLine();
	DirectionalityConstraints dirCons;
	dirCons.ReadData(*exportFile.GetAirportIDList().begin());
	dirCons.ExportData(exportFile);

	exportFile.getFile().endFile();
}

void DirectionalityConstraints::ImportDirectionalityConstraints(CAirsideImportFile& importFile)
{
	DirectionalityConstraints dc;
	dc.ImportData(importFile);
}

void DirectionalityConstraints::DoNotCall()
{
	DirectionalityConstraints dc;
	dc.AddNewItem(NULL);
}