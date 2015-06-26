#include "stdafx.h"

#include "airsideaircraft.h"
#include "../Database/ARCportDatabaseConnection.h"

using namespace ADODB;

//--------------------------------------------------------------------------
CAACategory::CAACategory()
{
	SetValue(-1, -1, "", "");
}

CAACategory::CAACategory(int nIdxInTable, int nIdx, const CString& strName, const CString& strACCat)
{
	SetValue(nIdxInTable, nIdx, strName, strACCat);
}

CAACategory::CAACategory(const CAACategory& _other)
{
	m_nIdxInTable = _other.GetIDInTable();
	m_nIdx = _other.GetID();
	m_strName = _other.GetName();
	m_strCat = _other.GetCat();
}

CAACategory::~CAACategory()
{
}

void CAACategory::SetValue(int nIdxInTable, int nIdx, const CString& strName, const CString& strACCat)
{
	m_nIdxInTable = nIdxInTable;
	m_nIdx = nIdx;
	m_strName = strName;
	m_strCat = strACCat;
}

void CAACategory::GetValue(int& nIdxInTable, int& nIdx, CString& strName, CString& strACCat) const
{
	nIdxInTable = m_nIdxInTable;
	nIdx = m_nIdx;
	strName = m_strName;
	strACCat = m_strCat;
}

//------------------------------------------------------------------------------

CAACategoryManager::CAACategoryManager(categoryType catType)
{
	m_catType = catType;
	m_wcList.clear();
}

CAACategoryManager::~CAACategoryManager()
{
	ClearRecords();
}

void CAACategoryManager::ClearRecords()
{
	for(int i = 0; i < (int)m_wcList.size(); i++)
	{
		delete m_wcList[i];
	}
	m_wcList.clear();
}

int CAACategoryManager::ReadData()
{
	CString strSQL;
	strSQL.Format("SELECT * FROM AirsideAircraftCat WHERE PROJID = %d AND Type = %d ORDER BY Idx",
		m_nProjID, m_catType);

	ClearRecords();

	try
	{
		_variant_t varCount;
		_RecordsetPtr rs =  DATABASECONNECTION.GetConnection()->Execute((_bstr_t)strSQL,&varCount,adOptionUnspecified );
		while (!rs->GetadoEOF())
		{
			AddRecord(rs);

			rs->MoveNext();
		}

		if (rs)
			if (rs->State == adStateOpen)
				rs->Close();
	}
	catch (_com_error &e)
	{
		CString strError = e.ErrorMessage();
		return -1;
	}

	return static_cast<int>(m_wcList.size());
}

int CAACategoryManager::AddRecord(_RecordsetPtr& rs)
{
	if(rs->GetadoEOF())
		return static_cast<int>(m_wcList.size());

	_variant_t var;

	CAACategory* pcat = new CAACategory();

	var = rs->GetCollect(L"ID");
	if (var.vt != NULL )
		pcat->SetIDInTable((int)var.intVal);
	
	var = rs->GetCollect(L"Idx");
	if (var.vt != NULL )
		pcat->SetID((int)var.intVal);

	var = rs->GetCollect(L"Name");
	if(var.vt != NULL)
		pcat->SetName( CString( (char*)_bstr_t(var) ) );

	var = rs->GetCollect(L"CatOne");
	if(var.vt != NULL)
		pcat->SetCat( CString( (char*)_bstr_t(var) ) );

	m_wcList.push_back(pcat);
	return static_cast<int>(m_wcList.size());
}

int CAACategoryManager::AddRecord(const CString& strName, const CString& strCat)
{
	int nIdx = static_cast<int>(m_wcList.size());
	
	return AddRecord( CAACategory(-1, nIdx, strName, strCat) );
}

int CAACategoryManager::AddRecord(CAACategory& cat)
{
	CString strSQL;
	strSQL.Format("INSERT INTO AirsideAircraftCat(ProjID, Type, Idx) \
				  VALUES (%d, %d, %d)", 
				  m_nProjID, m_catType, cat.GetID());

	try
	{
		_variant_t varCount;

		DATABASECONNECTION.GetConnection()->Execute((_bstr_t)strSQL,&varCount,adOptionUnspecified );

		//add idx in table
		CString strSQL;
		strSQL.Format("SELECT * FROM AirsideAircraftCat WHERE PROJID = %d AND Type = %d AND Idx = %d",
			m_nProjID, m_catType, cat.GetID());

		_RecordsetPtr rs =  DATABASECONNECTION.GetConnection()->Execute((_bstr_t)strSQL,&varCount,adOptionUnspecified );
		if(!rs->GetadoEOF())
		{
			_variant_t var;
			var = rs->GetCollect(L"ID");
			if (var.vt != NULL )
				cat.SetIDInTable((int)var.intVal);
		}

	}
	catch (_com_error &e)
	{
		CString strError = e.ErrorMessage();
		return static_cast<int>(m_wcList.size());
	}

	CAACategory* pcat = new CAACategory(cat);
	m_wcList.push_back(pcat);
	return static_cast<int>(m_wcList.size());
}

int CAACategoryManager::DeleteRecord(int nIndex)
{
	ASSERT( 0 <= nIndex && nIndex < (int)m_wcList.size());

	CString strSQL;
	strSQL.Format("DELETE FROM AirsideAircraftCat WHERE ProjID = %d AND Type = %d AND Idx = %d",
		m_nProjID, m_catType, nIndex);

	try
	{
		_variant_t varCount;
		DATABASECONNECTION.GetConnection()->Execute((_bstr_t)strSQL,&varCount,adOptionUnspecified );
	}
	catch (_com_error &e)
	{
		CString strError = e.ErrorMessage();
		return static_cast<int>(m_wcList.size());
	}	
	
	m_wcList.erase(m_wcList.begin() + nIndex);

	return static_cast<int>(m_wcList.size());
}

int CAACategoryManager::ModifyRecordIdx(int nIndex, int nNew)
{
	ASSERT( 0 <= nIndex && nIndex < (int)m_wcList.size());
	
	int nIdx = m_wcList[nIndex]->GetID();

	CString strSQL;
	strSQL.Format("UPDATE AirsideAircraftCat \
				  SET Idx = %d \
				  WHERE ProjID = %d AND Type = %d AND Idx = %d",
				  nNew, m_nProjID, m_catType, nIdx);
	try
	{
		_variant_t varCount;
		DATABASECONNECTION.GetConnection()->Execute((_bstr_t)strSQL,&varCount,adOptionUnspecified );
	}
	catch (_com_error &e)
	{
		CString strError = e.ErrorMessage();
		return -1;
	}	

	m_wcList[nIndex]->SetID(nNew);
	return nIdx;
}

CString CAACategoryManager::ModifyRecordName(int nIndex, const CString& strName)
{
	ASSERT( 0 <= nIndex && nIndex < (int)m_wcList.size());

	CString strSQL;
	strSQL.Format("UPDATE AirsideAircraftCat \
				  SET Name = '%s' \
				  WHERE ProjID = %d AND Type = %d AND Idx = %d",
				  strName, m_nProjID, m_catType, nIndex);
	try
	{
		_variant_t varCount;
		DATABASECONNECTION.GetConnection()->Execute((_bstr_t)strSQL,&varCount,adOptionUnspecified );
	}
	catch (_com_error &e)
	{
		CString strError = e.ErrorMessage();
		return _T("");
	}	

	CString str = m_wcList[nIndex]->GetName();
	m_wcList[nIndex]->SetName(strName);
	return str;
}

CString CAACategoryManager::ModifyRecordCat(int nIndex, const CString& strCat)
{
	ASSERT( 0 <= nIndex && nIndex < (int)m_wcList.size());

	CString strSQL;
	strSQL.Format("UPDATE AirsideAircraftCat \
				  SET CatOne = '%s' \
				  WHERE ProjID = %d AND Type = %d AND Idx = %d",
				  strCat, m_nProjID, m_catType, nIndex);
	try
	{
		_variant_t varCount;
		DATABASECONNECTION.GetConnection()->Execute((_bstr_t)strSQL,&varCount,adOptionUnspecified );
	}
	catch (_com_error &e)
	{
		CString strError = e.ErrorMessage();
		return _T("");
	}	

	CString str = m_wcList[nIndex]->GetCat();
	m_wcList[nIndex]->SetCat(strCat);
	return str;
}

void CAACategoryManager::MoveRecord(int nIndex, int nNew)
{
	ASSERT( 0 <= nIndex && nIndex < (int)m_wcList.size());
	ASSERT( 0 <= nNew && nNew < (int)m_wcList.size());

	CAACategory* wc1 = GetRecord(nIndex);
	CAACategory* wc2 = GetRecord(nNew);

	CString strSQL1, strSQL2;
	strSQL1.Format("UPDATE AirsideAircraftCat \
				  SET Idx = %d WHERE ID = %d",
				  nNew, wc1->GetIDInTable());

	strSQL2.Format("UPDATE AirsideAircraftCat \
				   SET Idx = %d WHERE ID = %d",
				   nIndex, wc2->GetIDInTable());
	try
	{
		_variant_t varCount;
		DATABASECONNECTION.GetConnection()->Execute((_bstr_t)strSQL1,&varCount,adOptionUnspecified );
		DATABASECONNECTION.GetConnection()->Execute((_bstr_t)strSQL2,&varCount,adOptionUnspecified );
	}
	catch (_com_error &e)
	{
		CString strError = e.ErrorMessage();
		return ;
	}	

	int nIdxInTable = m_wcList[nIndex]->GetIDInTable();
	CString strName = m_wcList[nIndex]->GetName();
	CString strCat = m_wcList[nIndex]->GetCat();

	m_wcList[nIndex]->SetIDInTable(m_wcList[nNew]->GetIDInTable());
	m_wcList[nIndex]->SetName(m_wcList[nNew]->GetName());
	m_wcList[nIndex]->SetCat(m_wcList[nNew]->GetCat());

	m_wcList[nNew]->SetIDInTable(nIdxInTable);
	m_wcList[nNew]->SetName(strName);
	m_wcList[nNew]->SetCat(strCat);
}
CAACategory* CAACategoryManager::GetRecord(int nIndex)
{
	ASSERT( 0 <= nIndex && nIndex < (int)m_wcList.size());

	return m_wcList[nIndex];
}

//----------------------------------------------------------------
CAACompCatManager::CAACompCatManager()
{
	m_catType = compositeType;
	ClearRecords();
}

CAACompCatManager::~CAACompCatManager()
{
	ClearRecords();
}

void CAACompCatManager::ClearRecords()
{
	m_wcList.clear();
}


void CAACompCatManager::ReadData()
{
	CString strSQL;
	strSQL.Format("SELECT * FROM AirsideAircraftCat WHERE PROJID = %d AND Type = %d ORDER BY Idx",
		m_nProjID, m_catType);

	ClearRecords();

	try
	{
		_variant_t varCount;
		_RecordsetPtr rs =  DATABASECONNECTION.GetConnection()->Execute((_bstr_t)strSQL,&varCount,adOptionUnspecified );
		while (!rs->GetadoEOF())
		{
			AddRecord(rs);

			rs->MoveNext();
		}

		if (rs)
			if (rs->State == adStateOpen)
				rs->Close();
	}
	catch (_com_error &e)
	{
		CString strError = e.ErrorMessage();
		return ;
	}
}

int CAACompCatManager::AddRecord(ADODB::_RecordsetPtr& rs)
{
	if(rs->GetadoEOF())
		return static_cast<int>(m_wcList.size());

	_variant_t var;

	int nID = -1;
	int nIdx = -1;
	CString strName = _T("");
	CATVECT vACCat;

	var = rs->GetCollect(L"ID");
	if (var.vt != NULL )
		nID = (int)var.intVal;

	var = rs->GetCollect(L"Idx");
	if (var.vt != NULL )
		nIdx = (int)var.intVal;

	var = rs->GetCollect(L"Name");
	if(var.vt != NULL)
		strName = (char*)_bstr_t(var);

	var = rs->GetCollect(L"CatOne");
	if(var.vt != NULL)
	{
		CString str = (char*)_bstr_t(var);
		vACCat.push_back(str);
	}

	var = rs->GetCollect(L"CatTwo");
	if(var.vt != NULL)
	{
		CString str = (char*)_bstr_t(var);
		vACCat.push_back(str);
	}

	CAACompositeCat cat(nID, nIdx, strName, vACCat);
	m_wcList.push_back(cat);
	return static_cast<int>(m_wcList.size());
}
int CAACompCatManager::AddRecord(const CString& strName, const CATVECT& vACCat)
{
	int nIdx = static_cast<int>(m_wcList.size());
	CAACompositeCat cat(-1, nIdx, strName, vACCat);
	return AddRecord(cat);
}
int CAACompCatManager::AddRecord(CAACompositeCat& cat)
{
	CString strSQL;
	strSQL.Format("INSERT INTO AirsideAircraftCat(ProjID, Type, Idx) \
				  VALUES (%d, %d, %d)", 
				  m_nProjID, m_catType, cat.GetID());

	try
	{
		_variant_t varCount;
		DATABASECONNECTION.GetConnection()->Execute((_bstr_t)strSQL,&varCount,adOptionUnspecified );
		
		//add idx in table
		CString strSQL;
		strSQL.Format("SELECT * FROM AirsideAircraftCat WHERE PROJID = %d AND Type = %d AND Idx = %d",
			m_nProjID, m_catType, cat.GetID());
		
		_RecordsetPtr rs =  DATABASECONNECTION.GetConnection()->Execute((_bstr_t)strSQL,&varCount,adOptionUnspecified );
		if(!rs->GetadoEOF())
		{
			_variant_t var;
			var = rs->GetCollect(L"ID");
			if (var.vt != NULL )
				cat.SetIDInTable((int)var.intVal);
		}

		if (rs)
			if (rs->State == adStateOpen)
				rs->Close();

	}
	catch (_com_error &e)
	{
		CString strError = e.ErrorMessage();
		return static_cast<int>(m_wcList.size());
	}

	m_wcList.push_back(cat);
	return static_cast<int>(m_wcList.size());
}
int CAACompCatManager::DeleteRecord(int nIndex)
{
	ASSERT( 0 <= nIndex && nIndex < (int)m_wcList.size());

	CString strSQL;
	strSQL.Format("DELETE FROM AirsideAircraftCat WHERE ProjID = %d AND Type = %d AND Idx = %d",
		m_nProjID, m_catType, nIndex);

	try
	{
		_variant_t varCount;
		DATABASECONNECTION.GetConnection()->Execute((_bstr_t)strSQL,&varCount,adOptionUnspecified );
	}
	catch (_com_error &e)
	{
		CString strError = e.ErrorMessage();
		return static_cast<int>(m_wcList.size());
	}

	m_wcList.erase(m_wcList.begin() + nIndex);
	return static_cast<int>(m_wcList.size());
}

int CAACompCatManager::ModifyRecordIdx(int nIndex, int nNew)
{
	ASSERT( 0 <= nIndex && nIndex < (int)m_wcList.size());

	int nIdx = m_wcList[nIndex].GetID();

	CString strSQL;
	strSQL.Format("UPDATE AirsideAircraftCat \
				  SET Idx = %d \
				  WHERE ProjID = %d AND Type = %d AND Idx = %d",
				  nNew, m_nProjID, m_catType, nIdx);
	try
	{
		_variant_t varCount;
		DATABASECONNECTION.GetConnection()->Execute((_bstr_t)strSQL,&varCount,adOptionUnspecified );
	}
	catch (_com_error &e)
	{
		CString strError = e.ErrorMessage();
		return -1;
	}	

	m_wcList[nIndex].SetID(nNew);
	return nIdx;
}

CString CAACompCatManager::ModifyRecordName(int nIndex, const CString& strName)
{
	ASSERT( 0 <= nIndex && nIndex < (int)m_wcList.size());

	CString strSQL;
	strSQL.Format("UPDATE AirsideAircraftCat \
				  SET Name = '%s' \
				  WHERE ProjID = %d AND Type = %d AND Idx = %d",
				  strName, m_nProjID, m_catType, nIndex);
	try
	{
		_variant_t varCount;
		DATABASECONNECTION.GetConnection()->Execute((_bstr_t)strSQL,&varCount,adOptionUnspecified );
	}
	catch (_com_error &e)
	{
		CString strError = e.ErrorMessage();
		return _T("");
	}	

	CString str = m_wcList[nIndex].GetName();
	m_wcList[nIndex].SetName(strName);
	return str;
}
CString CAACompCatManager::ModifyRecordCat(int nIndex, const CString& strCat, int nSubIndex)
{
	ASSERT( 0 <= nIndex && nIndex < (int)m_wcList.size());


	CString ColName = _T("");
	if(nSubIndex == 0)
		ColName = _T("CatOne");
	else if(nSubIndex == 1)
		ColName = _T("CatTwo");

	CString strSQL;
	strSQL.Format("UPDATE AirsideAircraftCat \
				  SET %s = '%s' \
				  WHERE ProjID = %d AND Type = %d AND Idx = %d",
				  ColName, strCat, m_nProjID, m_catType, nIndex);
	try
	{
		_variant_t varCount;
		DATABASECONNECTION.GetConnection()->Execute((_bstr_t)strSQL,&varCount,adOptionUnspecified );
	}
	catch (_com_error &e)
	{
		CString strError = e.ErrorMessage();
		return _T("");
	}

	CString str = m_wcList[nIndex].GetCat(nSubIndex);
	m_wcList[nIndex].SetCat(strCat, nSubIndex);
	return str;
}

void CAACompCatManager::MoveRecord(int nIndex, int nNew)
{
	ASSERT( 0 <= nIndex && nIndex < (int)m_wcList.size());
	ASSERT( 0 <= nNew && nNew < (int)m_wcList.size());

	const CAACompositeCat& wc1 = GetRecord(nIndex);
	const CAACompositeCat& wc2 = GetRecord(nNew);

	CString strSQL1, strSQL2;
	strSQL1.Format("UPDATE AirsideAircraftCat \
				   SET Idx = %d WHERE ID = %d ",
				   nNew, wc1.GetIDInTable());

	strSQL2.Format("UPDATE AirsideAircraftCat \
				   SET Idx = %d WHERE ID = %d ",
				   nIndex, wc2.GetIDInTable());
	try
	{
		_variant_t varCount;
		DATABASECONNECTION.GetConnection()->Execute((_bstr_t)strSQL1,&varCount,adOptionUnspecified );
		DATABASECONNECTION.GetConnection()->Execute((_bstr_t)strSQL2,&varCount,adOptionUnspecified );
	}
	catch (_com_error &e)
	{
		CString strError = e.ErrorMessage();
		return ;
	}	

	CAACompositeCat temp(wc1);
	m_wcList[nIndex].SetIDInTable(m_wcList[nNew].GetIDInTable());
	m_wcList[nIndex].SetName(m_wcList[nNew].GetName());
	m_wcList[nIndex].SetCats(m_wcList[nNew].GetCats());

	m_wcList[nNew].SetIDInTable(temp.GetIDInTable());
	m_wcList[nNew].SetName(temp.GetName());
	m_wcList[nNew].SetCats(temp.GetCats());
}
const CAACompositeCat& CAACompCatManager::GetRecord(int nIndex)
{
	ASSERT( 0 <= nIndex && nIndex < (int)m_wcList.size());

	return m_wcList[nIndex];
}

CAACompositeCat * CAACompCatManager::GetCompCatByID(int nID)
{
	for(AACompositeCatVect::iterator iter = m_wcList.begin();
		iter != m_wcList.end();
		iter++)
	{
		//// TRACE("\n%s  %s", (*iter).GetID(), (*iter).GetName() );
		if( (*iter).GetIDInTable() == nID)
			return &(*iter);
	}

	return NULL;
}
int CAACompCatManager::GetCompCatIDByName(const CString& strName)
{
	for(AACompositeCatVect::iterator iter = m_wcList.begin();
		iter != m_wcList.end();
		iter++)
	{
		//// TRACE("\n%s  %s", (*iter).GetID(), (*iter).GetName() );
		if( (*iter).GetName().CompareNoCase(strName)==0)
			return (*iter).GetIDInTable();
	}
	return -1;	
}
CAACompositeCat* CAACompCatManager::GetCompCatByName(const CString& strName)
{
	for(AACompositeCatVect::iterator iter = m_wcList.begin();
		iter != m_wcList.end();
		iter++)
	{
		if( (*iter).GetName() == strName)
			return &(*iter);
	}

	return NULL;
}


//----------------------------------------------------------------
CAirsideAircraftCat::CAirsideAircraftCat(void)
{
	m_pwcManager = new CAACategoryManager(weightType);
	m_pscManager = new CAACategoryManager(speedType);
	m_pccManager = new CAACompCatManager;
	m_pwsManager = new CAACategoryManager(wingspanType);
}

CAirsideAircraftCat::~CAirsideAircraftCat(void)
{
	delete m_pwcManager;
	delete m_pscManager;
	delete m_pccManager;
	delete m_pwsManager;
}

void CAirsideAircraftCat::ReadData()
{
	m_pwcManager->SetProjectID(m_nProjID);
	m_pwcManager->ReadData();

	m_pscManager->SetProjectID(m_nProjID);
	m_pscManager->ReadData();

	m_pccManager->SetProjectID(m_nProjID);
	m_pccManager->ReadData();

	m_pwsManager->SetProjectID(m_nProjID);
	m_pwsManager->ReadData();
}

