#include "StdAfx.h"
#include "TaxiwayConstraint.h"
//#include "../Inputs/Flt_cnst.h"
#include "..\Database\ADODatabase.h"
#include "AirsideImportExportManager.h"

//////////////////////////////////////////////////////////////////////////
// class CTaxiwayCommonData

CTaxiwayConstraintData::CTaxiwayConstraintData()
: m_nID(-1)
,m_nFromID(-1)
,m_nToID(-1)
,m_lMaxNumber(0)
{
}
CTaxiwayConstraintData::~CTaxiwayConstraintData()
{
}

void CTaxiwayConstraintData::SetFromID(int nFromID)
{ 
	m_nFromID = nFromID;
}
int	 CTaxiwayConstraintData::GetFromID()
{
	return m_nFromID;
}

void CTaxiwayConstraintData::SetToID(int nToID)
{ 
	m_nToID = nToID; 
}
int	 CTaxiwayConstraintData::GetToID()
{	
	return m_nToID;
}

void CTaxiwayConstraintData::SetMaxNumber(long lMaxNumber)
{
	m_lMaxNumber = lMaxNumber;
}
long CTaxiwayConstraintData::GetMaxNumber()
{
	return m_lMaxNumber;
}


void CTaxiwayConstraintData::SetConstraintType(TaxiwayConstraintType emType)
{
	m_emType = emType;    
}

TaxiwayConstraintType CTaxiwayConstraintData::GetConstraintType()
{
	return m_emType;
}

void CTaxiwayConstraintData::ReadData(CADORecordset& adoRecordset)
{
	adoRecordset.GetFieldValue(_T("ID"),m_nID);

	adoRecordset.GetFieldValue(_T("FROMNODE"),m_nFromID);
	adoRecordset.GetFieldValue(_T("TONODE"),m_nToID);
	adoRecordset.GetFieldValue(_T("MAXVALUE"),m_lMaxNumber);
}
void CTaxiwayConstraintData::SaveData(int nProjID,TaxiwayConstraintType consType, int nTaxiwayID)
{
	if (m_nID != -1)
	{
		return UpdateData();
	}

	CString strSQL = _T("");
	strSQL.Format(_T("INSERT INTO IN_TAXIWAYCONSTRAINT\
					 (PROJID, TYPE, TAXIWAYID, FROMNODE, TONODE, MAXVALUE)\
					 VALUES (%d,%d,%d,%d,%d,%d)"),
					 nProjID, (int)consType,nTaxiwayID, m_nFromID,m_nToID,m_lMaxNumber);


	m_nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
}
void CTaxiwayConstraintData::DeleteData()
{
	CString strSQL;
	strSQL.Format(_T("DELETE FROM IN_TAXIWAYCONSTRAINT\
					 WHERE (ID = %d)"), m_nID);

	CADODatabase::ExecuteSQLStatement(strSQL);
}
void CTaxiwayConstraintData::UpdateData()
{
	CString strSQL;
	strSQL.Format(_T("UPDATE IN_TAXIWAYCONSTRAINT\
					 SET FROMNODE =%d, TONODE =%d, MAXVALUE =%d\
					 WHERE (ID = %d)"), m_nFromID,m_nToID,m_lMaxNumber,m_nID);

	CADODatabase::ExecuteSQLStatement(strSQL);
}

void CTaxiwayConstraintData::ExportData(CAirsideExportFile& exportFile,TaxiwayConstraintType enumConsType,int nTaxiwayID)
{	
	exportFile.getFile().writeInt(m_nID);
	exportFile.getFile().writeInt(exportFile.GetProjectID());
	exportFile.getFile().writeInt((int)enumConsType);
	exportFile.getFile().writeInt(nTaxiwayID);
	exportFile.getFile().writeInt(m_nFromID);
	exportFile.getFile().writeInt(m_nToID);
	exportFile.getFile().writeInt(m_lMaxNumber);
	exportFile.getFile().writeLine();

}
void CTaxiwayConstraintData::ImportData(CAirsideImportFile& importFile)
{
	int nOldTaxiwayID = -1;
	int nenumType  = 0;
	int nOldID = -1;
	int nOldprojID = -1;
	importFile.getFile().getInteger(nOldID);
	importFile.getFile().getInteger(nOldprojID);
	
	importFile.getFile().getInteger(nenumType);
	importFile.getFile().getInteger(nOldTaxiwayID);
	
	int nOldFromID = -1;
	importFile.getFile().getInteger(nOldFromID);
	m_nFromID = importFile.GetIntersectionIndexMap(nOldFromID);

	int nOldToID = -1;
	importFile.getFile().getInteger(nOldToID);
	m_nToID = importFile.GetIntersectionIndexMap(nOldToID);

	importFile.getFile().getInteger(m_lMaxNumber);
	importFile.getFile().getLine();
	if (nenumType > TaxiwayConstraint_unknown && nenumType < TaxiwayConstraint_Count)
	{
		SaveData(importFile.getNewProjectID(),(TaxiwayConstraintType)nenumType,importFile.GetObjectNewID(nOldTaxiwayID));
	}



}

//////////////////////////////////////////////////////////////////////////
// class CTaxiwayConstraint

CTaxiwayConstraint::CTaxiwayConstraint()
:m_nTaxiwayID(-1)
{
}
CTaxiwayConstraint::~CTaxiwayConstraint()
{
}

void CTaxiwayConstraint::SetTaxiwayID(int nTaxiwayID)
{
	m_nTaxiwayID = nTaxiwayID;
}
int	 CTaxiwayConstraint::GetTaxiwayID()
{ 
	return m_nTaxiwayID; 
}

void CTaxiwayConstraint::AddItem(CTaxiwayConstraintData* pNewItem)
{
	m_vConstraintData.push_back(pNewItem);
}
void CTaxiwayConstraint::DelItem(CTaxiwayConstraintData* pDelItem)
{
	std::vector<CTaxiwayConstraintData *>::iterator iter = std::find(m_vConstraintData.begin(), m_vConstraintData.end(), pDelItem);
	if(iter == m_vConstraintData.end())
		return;

	m_vConstraintDataNeedDel.push_back(pDelItem);
	m_vConstraintData.erase(iter);
}

CTaxiwayConstraintData* CTaxiwayConstraint::GetItem(int nIndex)
{
	ASSERT(nIndex < (int)m_vConstraintData.size());
	return m_vConstraintData.at(nIndex);
}
int CTaxiwayConstraint::GetItemCount()
{
	return static_cast<int>( m_vConstraintData.size() ); 
}
void CTaxiwayConstraint::ReadData(CADORecordset& adoRecordset)
{

}
void CTaxiwayConstraint::SaveData(int nProjID,TaxiwayConstraintType consType)
{
	std::vector<CTaxiwayConstraintData *>::iterator iter = m_vConstraintData.begin();
	for (;iter != m_vConstraintData.end();++iter)
	{
		(*iter)->SaveData(nProjID,consType,m_nTaxiwayID);
	}

	iter = m_vConstraintDataNeedDel.begin();
	for (;iter!= m_vConstraintDataNeedDel.end();++iter)
	{
		(*iter)->DeleteData();
		delete *iter;
	}
	m_vConstraintDataNeedDel.clear();

}
void CTaxiwayConstraint::DeleteData(int nProjID,TaxiwayConstraintType consType)
{
	CString strSQL;
	strSQL.Format(_T("DELETE FROM IN_TAXIWAYCONSTRAINT\
					 WHERE (PROJID = %d) AND (TYPE = %d) AND (TAXIWAYID = %d)"),
					 nProjID,(int)consType,m_nTaxiwayID);

	CADODatabase::ExecuteSQLStatement(strSQL);

}
void CTaxiwayConstraint::UpdateData()
{

}
void CTaxiwayConstraint::ExportData(CAirsideExportFile& exportFile,TaxiwayConstraintType enumConsType)
{
	std::vector<CTaxiwayConstraintData *>::iterator iter = m_vConstraintData.begin();
	for (;iter != m_vConstraintData.end();++iter)
	{
		(*iter)->ExportData(exportFile,enumConsType,m_nTaxiwayID);
	}
}

//////////////////////////////////////////////////////////////////////////
// class CTaxiwayWeightConstraintList

CTaxiwayConstraintList::CTaxiwayConstraintList()
: m_nProjID(-1)
{
}
CTaxiwayConstraintList::~CTaxiwayConstraintList()
{
	std::vector<CTaxiwayConstraint *>::iterator iter = m_vConstraint.begin();
	for (; iter!= m_vConstraint.end(); ++iter)
	{
		delete *iter;
	}

	iter = m_vConstraintNeedDel.begin();
	for (; iter != m_vConstraintNeedDel.end(); ++iter)
	{
		delete *iter;
	}
}

void CTaxiwayConstraintList::SetConstraintType(TaxiwayConstraintType emType)
{
	m_emType = emType;    
}
TaxiwayConstraintType CTaxiwayConstraintList::GetConstraintType()
{
	return m_emType;
}

void CTaxiwayConstraintList::AddItem(CTaxiwayConstraint* pNewItem)
{
	m_vConstraint.push_back(pNewItem);
}

void CTaxiwayConstraintList::DelItem(CTaxiwayConstraint* pDelItem)
{
	std::vector<CTaxiwayConstraint *>::iterator iter = std::find(m_vConstraint.begin(), m_vConstraint.end(), pDelItem);
	if(iter == m_vConstraint.end())
		return;

	m_vConstraintNeedDel.push_back(pDelItem);
	m_vConstraint.erase(iter);
}

void CTaxiwayConstraintList::DelItem(int nIndex)
{
	ASSERT(nIndex < (int)m_vConstraint.size());

	m_vConstraintNeedDel.push_back(m_vConstraint[nIndex]);
	m_vConstraint.erase(m_vConstraint.begin() + nIndex);
}

CTaxiwayConstraint* CTaxiwayConstraintList::GetItem(int nIndex)
{
	ASSERT(nIndex < (int)m_vConstraint.size());
	return m_vConstraint.at(nIndex);
}
int  CTaxiwayConstraintList::GetItemCount()
{
	return static_cast<int>( m_vConstraint.size() );
}

void CTaxiwayConstraintList::ReadData(int nprojID)
{
	CString strSQL = _T("");
	strSQL.Format(_T("SELECT ID, TAXIWAYID, FROMNODE, TONODE, MAXVALUE\
					 FROM IN_TAXIWAYCONSTRAINT\
					 WHERE (PROJID = %d) AND (TYPE = %d)"),nprojID,(int)m_emType);

	long nRecordCount = 0;
	CADORecordset adoRecordset;
	CADODatabase::ExecuteSQLStatement(strSQL,nRecordCount,adoRecordset);

	while (!adoRecordset.IsEOF())
	{
		int nTaxiwayID = 0;
		adoRecordset.GetFieldValue(_T("TAXIWAYID"),nTaxiwayID);
		CTaxiwayConstraintData* taxiwayConstraintData = new CTaxiwayConstraintData();
		taxiwayConstraintData->ReadData(adoRecordset);
		taxiwayConstraintData->SetConstraintType(m_emType);

		AddTaxiwayConstraintDataItem(nTaxiwayID,taxiwayConstraintData);

		adoRecordset.MoveNextData();
	}
}

void CTaxiwayConstraintList::ReadWeightWingspanConsData(int nprojID)
{
	CString strSQL = _T("");
	strSQL.Format(_T("SELECT ID, TAXIWAYID, FROMNODE, TONODE, TYPE, MAXVALUE\
					 FROM IN_TAXIWAYCONSTRAINT\
					 WHERE (PROJID = %d) "),nprojID);

	long nRecordCount = 0;
	CADORecordset adoRecordset;
	CADODatabase::ExecuteSQLStatement(strSQL,nRecordCount,adoRecordset);

	while (!adoRecordset.IsEOF())
	{
		int nTaxiwayID = 0;
		adoRecordset.GetFieldValue(_T("TAXIWAYID"),nTaxiwayID);
		CTaxiwayConstraintData* taxiwayConstraintData = new CTaxiwayConstraintData();
		taxiwayConstraintData->ReadData(adoRecordset);
		int nValue = -1;
		adoRecordset.GetFieldValue(_T("TYPE"),nValue);
		m_emType = (TaxiwayConstraintType)nValue;
		taxiwayConstraintData->SetConstraintType(m_emType);

		AddTaxiwayConstraintDataItem(nTaxiwayID,taxiwayConstraintData);

		adoRecordset.MoveNextData();
	}
}

void CTaxiwayConstraintList::AddTaxiwayConstraintDataItem(int taxiwayID,CTaxiwayConstraintData *tcData)
{
	CTaxiwayConstraint *pTaxiwayConstraint = NULL;
	//find
	for (int i =0; i < static_cast<int>(m_vConstraint.size()); ++i)
	{
		if(m_vConstraint[i]->GetTaxiwayID() == taxiwayID)
		{
			pTaxiwayConstraint = m_vConstraint[i];
		}
	}
	//new
	if (pTaxiwayConstraint == NULL)
	{
		pTaxiwayConstraint = new CTaxiwayConstraint();
		pTaxiwayConstraint->SetTaxiwayID(taxiwayID);
		AddItem(pTaxiwayConstraint);
	}

	//add data
	if (pTaxiwayConstraint)
		pTaxiwayConstraint->AddItem(tcData);
}

void CTaxiwayConstraintList::SaveData(int nProjID)
{
	std::vector<CTaxiwayConstraint *>::iterator iter = m_vConstraint.begin();
	for (;iter != m_vConstraint.end();++iter)
	{
		(*iter)->SaveData(nProjID,m_emType);
	}

	iter = m_vConstraintNeedDel.begin();
	for (;iter!= m_vConstraintNeedDel.end();++iter)
	{
		(*iter)->DeleteData(nProjID,m_emType);
		delete *iter;
	}
	m_vConstraintNeedDel.clear();
}
void CTaxiwayConstraintList::ExportTaxiwayConstraint(CAirsideExportFile& exportFile)
{
	CTaxiwayConstraintList taxiwayConstraintList;
	taxiwayConstraintList.SetConstraintType(WeightConstraint);
	taxiwayConstraintList.ReadData(exportFile.GetProjectID());
	taxiwayConstraintList.ExportData(exportFile);

	//
	CTaxiwayConstraintList taxiwayConstraintList_winspan;
	taxiwayConstraintList_winspan.SetConstraintType(WingSpanConstraint);
	taxiwayConstraintList_winspan.ReadData(exportFile.GetProjectID());
	taxiwayConstraintList_winspan.ExportData(exportFile);


}
void CTaxiwayConstraintList::ImportTaxiwayConstraint(CAirsideImportFile& importFile)
{

}
void CTaxiwayConstraintList::ExportData(CAirsideExportFile& exportFile)
{
	std::vector<CTaxiwayConstraint *>::iterator iter = m_vConstraint.begin();
	for (;iter != m_vConstraint.end();++iter)
	{
		(*iter)->ExportData(exportFile,m_emType);
	}
	

}
void CTaxiwayConstraintList::ImportData(CAirsideImportFile& importFile)
{
	while (!importFile.getFile().isBlank())
	{
		CTaxiwayConstraintData taxiwayConsData;
		taxiwayConsData.ImportData(importFile);
	}
}
