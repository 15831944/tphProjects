// ACTypesManager.cpp: implementation of the CACTypesManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "..\main\resource.h"
#include "ACTypesManager.h"
#include "projectmanager.h"
#include "common\airportdatabase.h"
#include "Common/AirportDatabaseList.h"
#include "../Database/ARCportADODatabase.h"
//#include "engine\terminal.h"
#include "assert.h"
#include <algorithm>
#include <functional>
#include <common/ARCUnit.h>
#include "AircraftAliasManager.h"
#include <math.h>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// define
static const CString _strACFileName			= "acdata.acd";
static const CString _strACCatFileName		= "category.cat";
#include "SuperTypeRelationData.h"
void CACTypesManager::ReSetAllID()
{
	for(unsigned i=0; i<m_vACTypes.size(); i++)
	{
		 m_vACTypes[i]->SetID(-1);
		 m_vACTypes[i]->ResetDoorID() ;
	}
	for(unsigned j=0; j<m_vACCategories.size(); j++)
		 m_vACCategories[j]->SetID(-1);
}
void CACTypesManager::DeleteAllDataFromDB(int airportID,DATABASESOURCE_TYPE type)
{
   m_ACTypeDataSet.DeleteAllData(airportID,type) ;
   CACType::DeleteAllDataFromDB(airportID,type) ;

}
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CACTypesManager::CACTypesManager(CAirportDatabase* pAirportDatabase):m_RelationShip(NULL),m_AirportID(-1)
,m_pAirportDatabase(pAirportDatabase)
{

}
void CACTypesManager::InitRelationShip()
{
	m_RelationShip = new SuperTypeRelationData(this) ;
	m_RelationShip->ReadDataFromDB() ;
}
SuperTypeRelationData* CACTypesManager::GetActypeRelationShip()
{
	return NULL;

	//return m_RelationShip ;
}
CACTypesManager::~CACTypesManager()
{
	for(unsigned i=0; i<m_vACTypes.size(); i++)
		delete m_vACTypes[i];
	m_vACTypes.clear();

	for(unsigned j=0; j<m_vACCategories.size(); j++)
		delete m_vACCategories[j];
	m_vACCategories.clear();

	if(m_RelationShip != NULL)
	{
		delete m_RelationShip ;
		m_RelationShip = NULL ;
	}
}
void CACTypesManager::Clear()
{
	for(unsigned i=0; i<m_vACTypes.size(); i++)
		delete m_vACTypes[i];
	m_vACTypes.clear();

	for(unsigned j=0; j<m_vACCategories.size(); j++)
		delete m_vACCategories[j];
	m_vACCategories.clear();
	ClearDeleteActype() ;
	ClearDelCategories() ;
}
void CACTypesManager::RemoveActype(CACType* _actype)
{
	if(_actype == NULL)
		return ;
	CACTYPELIST::iterator iter = m_vACTypes.begin() ;
	for ( ; iter != m_vACTypes.end() ;iter++)
	{
		if(*iter == _actype)
		{
			m_DelActype.push_back(*iter) ;
			m_vACTypes.erase(iter) ;
			break ;
		}
	}
}
void CACTypesManager::RemoveCategories(CACCategory* _categories)
{
	if(_categories == NULL)
		return ;
	CACCATEGORYLIST::iterator iter = m_vACCategories.begin() ;
	for ( ; iter != m_vACCategories.end() ;iter++)
	{
		if(*iter == _categories)
		{
			m_DelCaccate.push_back(*iter) ;
			m_vACCategories.erase(iter) ;
			break ;
		}
	}
}
void CACTypesManager::ClearDelCategories()
{
	CACCATEGORYLIST::iterator iter = m_DelCaccate.begin() ;
	for ( ; iter != m_DelCaccate.end() ;iter++)
	{
		delete *iter ;
	}
	m_DelCaccate.clear() ;
}
void CACTypesManager::ClearDeleteActype()
{
	CACTYPELIST::iterator iter = m_DelActype.begin() ;
	for ( ; iter != m_DelActype.end() ;iter++)
	{
		delete *iter ;
	}
	m_DelActype.clear() ;
}

//void CACTypesManager::ConvertCategoryDB()
//{
//	for (int ndx = 0 ; ndx < (int)m_vACCategories.size() ;ndx++)
//	{
//		CACCategory* pAcCategory = m_vACCategories[ndx] ;
//		std::vector<CACType*> m_outData ;
//		std::vector<CString> _FindData ;
//		for (int i = 0  ; i < (int)pAcCategory->m_vDirectACTypes.size() ;i++)
//		{
//			
// 			if(FindActypeByIATACodeOrICAOCode(pAcCategory->m_vDirectACTypes.at(i),m_outData))
//				_FindData.push_back(pAcCategory->m_vDirectACTypes.at(i)) ;
//		}
//		//remove find data 
//		for (int i = 0 ; i < (int)_FindData.size() ;i++)
//		{
//			pAcCategory->m_vDirectACTypes.erase(std::find(pAcCategory->m_vDirectACTypes.begin(),pAcCategory->m_vDirectACTypes.end(),_FindData[i])) ;
//		}
//		//add new data
//		for (int i = 0 ; i < (int)m_outData.size() ;i++)
//		{
//			pAcCategory->m_vDirectACTypes.push_back(m_outData[i]->GetIATACode()) ;
//		}
//	}
//}
//BOOL CACTypesManager::FindAndRemoveFromDefaultActype(std::vector<CACType*>& _OutData ,  std::vector<CACType*> _inOut, CACType* _ActypeItem)
//{
//	for (int i = 0 ; i < (int)_inOut.size() ;i++)
//	{
//		if(_inOut[i]->FitActype(_ActypeItem->GetIATACode()) && std::find(_OutData.begin(),_OutData.end(),_inOut[i]) == _OutData.end())
//			_OutData.push_back(_inOut[i]) ;
//	}
//	if((int)_OutData.size() > 0)
//		return TRUE ;
//	else
//		return FALSE ;
//}

////////new version database read and save/////////////////////////////////////////////////
bool CACTypesManager::loadDatabase(CAirportDatabase* pAirportDatabase)
{
	ASSERT(pAirportDatabase);

	Clear();
	m_ACTypeDataSet.SetACCategoryList(&m_vACCategories);
	m_ACTypeDataSet.SetDelACCategoryList(&m_DelCaccate);
	m_ACTypeDataSet.SetACTypesManager(this);
	m_ACTypeDataSet.loadDatabase(pAirportDatabase);

	CString strSQL(_T(""));
	strSQL.Format(_T("SELECT * FROM TB_AIRPORTDB_ACTYPE"));
	CADORecordset adoRecordset;
	long lCount = 0;
	try
	{
		CDatabaseADOConnetion::ExecuteSQLStatement(strSQL,lCount,adoRecordset,pAirportDatabase->GetAirportConnection());
	}
	catch (CADOException& e)
	{
		e.ErrorMessage();
		return false;
	}
	while(!adoRecordset.IsEOF())
	{
		CACType* pACType = new CACType();
		pACType->loadDatabase(pAirportDatabase,adoRecordset);
		m_vACTypes.push_back(pACType);
		adoRecordset.MoveNextData();
	}
	
	
	//ClacCategoryACTypeList();
	return true;
}

//read common data
bool CACTypesManager::loadARCDatabase(CAirportDatabase* pAirportDatabase)
{
	Clear();
	CString strSQL(_T(""));
	strSQL.Format(_T("SELECT * FROM TB_AIRPORTDB_ACTYPE"));
	CADORecordset adoRecordset;
	long lCount = 0;
	try
	{
		CDatabaseADOConnetion::ExecuteSQLStatement(strSQL,lCount,adoRecordset,pAirportDatabase->GetAirportConnection());
	}
	catch (CADOException& e)
	{
		e.ErrorMessage();
		return false;
	}
	while(!adoRecordset.IsEOF())
	{
		CACType* pACType = new CACType();
		pACType->loadDatabase(pAirportDatabase,adoRecordset);
		m_vACTypes.push_back(pACType);
		adoRecordset.MoveNextData();
	}
	/*ACALIASMANAGER->SetCACTypeManager(this);*/
	return true;
}

bool CACTypesManager::saveDatabase(CAirportDatabase* pAirportDatabase)
{
	m_ACTypeDataSet.SetACCategoryList(&m_vACCategories);
	m_ACTypeDataSet.SetDelACCategoryList(&m_DelCaccate) ;
	m_ACTypeDataSet.saveDatabase(pAirportDatabase);

	for (int i = 0; i < (int)m_vACTypes.size(); i++)
	{
		CACType* pACType = m_vACTypes.at(i);
		if (!pACType->IsEdit())
		{
			pACType->saveDatabase(pAirportDatabase);
		}
		
	}

	for (int j = 0; j < (int)m_DelActype.size(); j++)
	{
		CACType* pDelACType = m_DelActype.at(j);
		pDelACType->deleteDatabase(pAirportDatabase);
	}
	return true;
}

bool CACTypesManager::deleteDatabase(CAirportDatabase* pAirportDatabase)
{
	m_ACTypeDataSet.deleteDatabase(pAirportDatabase);
	CACType::deleteAllDatabase(pAirportDatabase);
	return true;
}
/////////////////old code version,no more use//////////////////////////////////////////////
void CACTypesManager::ReadDataFromDB(int airportID,DATABASESOURCE_TYPE type )
{
	Clear() ;
	m_ACTypeDataSet.SetACCategoryList(&m_vACCategories);
	m_ACTypeDataSet.SetDelACCategoryList(&m_DelCaccate) ;
	m_ACTypeDataSet.SetACTypesManager(this);
	m_ACTypeDataSet.ReadDataFromDB(airportID,type) ;
	CACType::ReadDataFromDB(m_vACTypes,airportID,type) ;
	
	if((int)m_vACTypes.size() < 105) 
	   ReadCommonActype(m_strCommonDBPath) ;
	ClacCategoryACTypeList();
}
void CACTypesManager::WriteDataToDB(int airportID,DATABASESOURCE_TYPE type )
{

	m_ACTypeDataSet.SetACCategoryList(&m_vACCategories);
	m_ACTypeDataSet.SetDelACCategoryList(&m_DelCaccate) ;
	m_ACTypeDataSet.SaveDataToDB(airportID,type) ;

	CACType::WriteDataToDB(m_vACTypes,airportID,type) ;
	CACType::DeleteDataFromDB(m_DelActype,airportID,type) ;

   ClearDelCategories() ;
   ClearDeleteActype() ;
}
BOOL CACTypesManager::LoadDataFromOtherFile(const CString& _strDBPath)
{
	CString _currentStr ;
	_currentStr = m_strDBPath ;
	BOOL res = LoadData(_strDBPath);
	m_strDBPath = _currentStr ;
	return res ;
}
BOOL CACTypesManager::ReadCommonActype(const CString& _Path)
{
	BOOL bRet = TRUE;
	CFile* pFile = NULL;
	CString sFileName = _Path + "\\" + _strACFileName;

	// read ac 
	try
	{
		pFile = new CFile(sFileName, CFile::modeRead | CFile::shareDenyNone);
		CArchive ar(pFile, CArchive::load);
		char line[513];
		//skip a line
		CString csLine;
		ar.ReadString( csLine );
		//get version 
		char* version = NULL ;
		version = (char*)strchr(csLine,',') ;
		int fileVersion = 0 ;
		if(version)
			fileVersion = atoi(version+1) ;


		//read line 2
		ar.ReadString( csLine );
		csLine.MakeUpper();
		strcpy( line, csLine );
		if(_stricmp(line, "AIRCRAFT DATABASE") == 0)
		{
			//skip the column names
			ar.ReadString( csLine );
			//read the values
			//read a line
			ar.ReadString( csLine );
			csLine.MakeUpper();
			strcpy( line, csLine );
			while( *line != '\0')
			{
				CACType* pACType = new CACType();
				char* b = line;
				char* p = NULL;
				int c = 1;
				while((p = strchr(b, ',')) != NULL) {
					*p = '\0';
					switch(c)
					{
					case 1:
						pACType->setIATACode(b);
						break;
						// ICAO code...
					case 2: //Desc
						pACType->m_sDescription = b;
						break;
					case 3: //height
						pACType->m_fHeight = (float) atof(b);
						break;
					case 4: //length
						pACType->m_fLen = (float) atof(b);
						break;
					case 5: //span
						pACType->m_fSpan = (float) atof(b);
						break;
					case 6: //mzfw
						pACType->m_fMZFW = (float) atof(b);
						break;
					case 7: //oew
						pACType->m_fOEW = (float) atof(b);
						break;
					case 8: //mtow
						pACType->m_fMTOW = (float) atof(b);
						break;
					case 9: //mlw
						pACType->m_fMLW = (float) atof(b);
						break;
					case 10: //capacity
						pACType->m_iCapacity = (int) atoi(b);
						break;
					case 11:
						pACType->m_dCabinWidth = atof(b) ;
						break ;
					}
					b = ++p;
					c++;
				}
				if(b!=NULL) {// the last column did not have a comma after it
					switch(c)
					{
					case 1:
						pACType->setIATACode(b);
						break;
						// ICAO code...
					case 2: //Desc
						pACType->m_sDescription = b;
						break;
					case 3: //height
						pACType->m_fHeight = (float) atof(b);
						break;
					case 4: //length
						pACType->m_fLen = (float) atof(b);
						break;
					case 5: //span
						pACType->m_fSpan = (float) atof(b);
						break;
					case 6: //mzfw
						pACType->m_fMZFW = (float) atof(b);
						break;
					case 7: //oew
						pACType->m_fOEW = (float) atof(b);
						break;
					case 8: //mtow
						pACType->m_fMTOW = (float) atof(b);
						break;
					case 9: //mlw
						pACType->m_fMLW = (float) atof(b);
						break;
					case 10: //capacity
						pACType->m_iCapacity = (int) atoi(b);
						break;
					case 11:
						pACType->m_dCabinWidth = atof(b) ;
						break ;
					}
				}
				if(findACTypeItem(pACType) == INT_MAX)
					m_vACTypes.push_back(pACType);
				else
					delete pACType ;
				ar.ReadString( csLine );
				csLine.MakeUpper();
				strcpy( line, csLine );
			}
		}
		else {
			bRet = FALSE;
		}
		ar.Close();
		delete pFile;
	}
	catch(CException* e)
	{
		AfxMessageBox("Error reading Aircraft Types file: " + sFileName, MB_ICONEXCLAMATION | MB_OK);
		e->Delete();
		if(pFile != NULL)
			delete pFile;
		bRet = FALSE;
	}
	return bRet ;
}
BOOL CACTypesManager::LoadData( const CString& _strDBPath )
{
	Clear();
	m_ACTypeDataSet.SetACCategoryList(&m_vACCategories);
	m_ACTypeDataSet.SetACTypesManager(this);
	m_ACTypeDataSet.loadDataSet(_strDBPath);


	m_strDBPath = _strDBPath;
	
	//because the actype format has changed ,version which before 2.1 is not used any more 
	//when this function call ,it's version is sure less than 2.1 ,so do not need read the old data again 
	//just read the data from common data.
   return ReadCommonActype(m_strCommonDBPath) ;

}

void CACTypesManager::GetACTypesByFilter(LPCSTR strFilterScript, CACTYPELIST& list)
{
/*	CString sFilter = actFilter.code;
	int nIdx = sFilter.Find('*');
	if(nIdx != -1)
		sFilter = sFilter.Left(nIdx);
	for(int i=0; i<m_vACTypes.size(); i++) {
		CACType* pACT = m_vACTypes[i];
		if(pACT->m_sCode.Left(sFilter.GetLength()).CompareNoCase(sFilter) == 0)
			list.push_back(pACT);
	}
*/
	using std::remove;
	using std::remove_copy_if;

	int iFieldIndex = -1;
	LogicalOperator LO;
	CString strValue;
	if (ParseFilterString(strFilterScript, iFieldIndex, LO, strValue))
	{
		list.assign(m_vACTypes.size(), NULL);
		remove_copy_if(m_vACTypes.begin(), m_vACTypes.end(), list.begin(), ACTypeRemove(iFieldIndex, LO, strValue));
		CACTYPELIST::iterator iterRemove = remove(list.begin(), list.end(), (CACType*)NULL);
		list.erase(iterRemove, list.end());
	}
}

bool CACTypesManager::ParseFilterString(const CString& strFilter, 
										int& iFieldIndex, 
										LogicalOperator& LO, 
										CString& strValue)
{
	if (strFilter.IsEmpty())
		return false;

	int iLogicalOperatorIndex = strFilter.FindOneOf(_T("><=!"));
	if (iLogicalOperatorIndex <= 0) // if there is no logical operator or field string
		return false;

	CString colnames;
	colnames.LoadString(IDS_ATD_COLNAMES);
	std::vector<CString> vNameSplit;
	// fill the container with each colname
	// split the colnames into parts seperated by commas
	int first = -1, next = -1;
	while(TRUE)
	{
		next = colnames.Find(',', first+1);
		if(next != -1)
			vNameSplit.push_back(colnames.Mid(first+1,next-first-1));
		else
		{
			vNameSplit.push_back(colnames.Mid(first+1));
			break;
		}
		first = next;
	}

	// 1. find the field
	iFieldIndex = -1;
	CString strField = strFilter.Left(iLogicalOperatorIndex);
	for (unsigned i = 0; i < vNameSplit.size(); i++)
	{
		if (strField.CompareNoCase(vNameSplit[i]) == 0)
		{
			iFieldIndex = i + 1;
			break;
		}
	}
	if (iFieldIndex == -1) // there is no field to fit the string.
		return false;

	// 2. find the logical operator
	//ASSERT(strFilter.GetLength() > iLogicalOperatorIndex + 2);
	TCHAR cOperator1 = strFilter.GetAt(iLogicalOperatorIndex);
	TCHAR cOperator2 = strFilter.GetAt(iLogicalOperatorIndex + 1);
	switch(cOperator1)
	{
	case _T('='):
		LO = EQUAL;
		break;

	case _T('>'):
		{
			if (cOperator2 == _T('='))
				LO = GRATER_EQUAL;
			else
				LO = GRATER;
		}
		break;

	case _T('<'):
		{
			if (cOperator2 == _T('='))
				LO = LESS_EQUAL;
			else
				LO = LESS;
		}
		break;

	case _T('!'):
		{
			if (cOperator2 == _T('='))
				LO = NOT_EQUAL;
			else
				return false;
		}

	default:
		return false;
	}

	// 3. get the value;
	int iValueStringLen = strFilter.GetLength() - iLogicalOperatorIndex 
												- (cOperator2 == _T('=') ? 2 : 1);
	strValue = strFilter.Right(iValueStringLen);

	return true;
}

void CACTypesManager::AddACCategory(CString sCatName, CACTYPEFILTER actFilter)
{
	for(unsigned i=0; i<m_vACCategories.size(); i++) {
		CACCategory* pACC = m_vACCategories[i];
		if(pACC->m_sName.CompareNoCase(sCatName) == 0) { //found ac cat
			pACC->AddFilter(actFilter);
			return;
		}
	}
	CACCategory* pACC = new CACCategory( this );
	pACC->m_sName = sCatName;
	pACC->AddFilter(actFilter);
	m_vACCategories.push_back(pACC);
}

int CACTypesManager::findCatItem( CACCategory* _pACCat )
{
	int nCount = m_vACCategories.size();
	for( int i=0; i<nCount; i++ )
	{
		if( *m_vACCategories[i] == *_pACCat )
			return i;
	}
	return INT_MAX;	
}


int CACTypesManager::findACTypeItem( CACType* _pACType )
{
	int nCount = m_vACTypes.size();
	for( int i=0; i<nCount; i++ )
	{
		if( *m_vACTypes[i] == *_pACType )
			return i;
	}
	return INT_MAX;	
}


// find the entry and return the index
// if not find add and return the index
// return -1 for invalid.
int CACTypesManager::FindOrAddEntry(CString _csACType)
{
	CACType* pACType = new CACType();
	pACType->setIATACode( _csACType );
	CString s;
	s.LoadString( IDS_READ_FROM_FILE );
	pACType->m_sDescription = s;
	int nRet = findACTypeItem( pACType );
	if( nRet != INT_MAX )
	{
		delete pACType;
		return nRet;
	}else
	{
		CACType* FindACType = FindActypeByIATACodeOrICAOCode(_csACType) ;
		if(FindACType != NULL)
			return findACTypeItem(FindACType) ;
	}
	if(GetActypeRelationShip() != NULL)
	{
		CACType* pRetACType = GetActypeRelationShip()->FindActypeBySuperTypeRelation(_csACType) ;
		if(pRetACType != NULL)
		{
			nRet = findACTypeItem( pRetACType );
			if( nRet != INT_MAX )
			{
				delete pACType;
				return nRet;
			}
		}
	}
	// now add into database
	if( _csACType.IsEmpty() || m_pAirportDatabase == NULL)
	{
		delete pACType;
		return -1;
	}

	m_vACTypes.push_back( pACType );
	std::vector<CACType*> _AcTypes ;
	_AcTypes.push_back(pACType) ;
	try
	{
		CDatabaseADOConnetion::BeginTransaction(m_pAirportDatabase->GetAirportConnection()) ;
		saveDatabase(m_pAirportDatabase);
		CDatabaseADOConnetion::CommitTransaction(m_pAirportDatabase->GetAirportConnection()) ;
	}
	catch (CADOException& e)
	{
		CDatabaseADOConnetion::RollBackTransation(m_pAirportDatabase->GetAirportConnection()) ;
		e.ErrorMessage() ;
	}

	return getACTypeCount()-1;	
}
void CACTypesManager::ClacCategoryACTypeList()
{
	CACCATEGORYLIST::iterator iter = m_vACCategories.begin();
	CACCATEGORYLIST::iterator iterEnd = m_vACCategories.end();
	for (;iter != iterEnd; ++iter)
	{
		(*iter)->ClacACTypeList();
	}

}
//////////////////////////////////////////////////////////////////////////
CACType* CACTypesManager::FindActypeByIATACodeOrICAOCode(const CString& _str) 
{
	int nCount = m_vACTypes.size();
	CACType* Pactype = NULL ;
	for( int i=0; i<nCount; i++ )
	{
		Pactype = m_vACTypes[i] ;
		if( Pactype->GetIATACode().CompareNoCase(_str) == 0|| Pactype->GetICAOCode().CompareNoCase(_str) == 0)
			return Pactype;
	}
	return NULL ;
}
BOOL CACTypesManager::FindActypeByIATACodeOrICAOCode(const CString& _str,std::vector<CACType*>& _Actypelist)
{
	int nCount = m_vACTypes.size();
	long  res = _Actypelist.size() ;
	CACType* Pactype = NULL ;
	for( int i=0; i<nCount; i++ )
	{
		Pactype = m_vACTypes[i] ;
		if( (Pactype->GetIATACode().CompareNoCase(_str) == 0 || Pactype->GetICAOCode().CompareNoCase(_str) == 0) && std::find(_Actypelist.begin(),_Actypelist.end(),Pactype) == _Actypelist.end())
			_Actypelist.push_back(Pactype);
	}
	if((int)_Actypelist.size() > res)
		return TRUE ;
	else
		return FALSE ;
}
//////////////////////////////////////////////////////////////////////////
CACType *CACTypesManager::getACTypeItem(const CString& _csACType)
{
	CACType *pRetACType = NULL;
	if( !_csACType.IsEmpty() )
	{
		CACType* pACType = new CACType();
		pACType->setIATACode( _csACType );
		CString s;
		s.LoadString( IDS_READ_FROM_FILE );
		pACType->m_sDescription = s;
		int nRet = findACTypeItem( pACType );
		if( nRet != INT_MAX )
		{
			delete pACType;
			pRetACType =  getACTypeItem(nRet);
			return pRetACType ;
		}
	

		if(GetActypeRelationShip() != NULL)
		{
			CACType* pRetACType = GetActypeRelationShip()->FindActypeBySuperTypeRelation(pACType->GetIATACode()) ;
			return pRetACType ;
		}

	}
	return pRetACType;
}
// global function
//CACTypesManager* _g_GetActiveACMan( Terminal* _pTerm )
//{
//	return _g_GetActiveACMan( (InputTerminal*)_pTerm );
//}
//
//CACTypesManager* _g_GetActiveACMan( InputTerminal* _pInTerm )
//{
//	assert( _pInTerm );
//	assert( _pInTerm->m_pAirportDB );
//
//	return _pInTerm->m_pAirportDB->getAcMan();
//}

CACTypesManager * _g_GetActiveACMan( CAirportDatabase * pAirportDB )
{
	assert(pAirportDB);
	return pAirportDB->getAcMan();
}

// class ACTypeRemove
ACTypeRemove::ACTypeRemove(const int iFieldIndex, 
						   const CACTypesManager::LogicalOperator LO, 
						   LPCSTR strValue)
{
	m_iFieldIndex = iFieldIndex;
	m_LO = LO;
	m_strValue = strValue;
	
}

bool ACTypeRemove::operator() (CACType* pACType)
{
	bool bRet = true;
	
	switch(m_iFieldIndex)
	{
	case 1: // IATA Code	
		{
			if (m_LO != CACTypesManager::EQUAL)
				return true;

			CString strValue(m_strValue);
			int nIdx = strValue.Find('*');
			if(nIdx != -1)
				strValue = strValue.Left(nIdx);
			bRet = (pACType->GetIATACode().Left(strValue.GetLength()).CompareNoCase(strValue) != 0);
		}
		break;
	case 2: // Company
		{
			if (m_LO != CACTypesManager::EQUAL)
				return true;

			CString strValue(m_strValue);
			int nIdx = strValue.Find('*');
			if(nIdx != -1)
				strValue = strValue.Left(nIdx);
			bRet = (pACType->m_sCompany.Left(strValue.GetLength()).CompareNoCase(strValue) != 0);
		}
		break;
	case 3 : //Description
		{
			if (m_LO != CACTypesManager::EQUAL)
				return true;

			CString strValue(m_strValue);
			int nIdx = strValue.Find('*');
			if(nIdx != -1)
				strValue = strValue.Left(nIdx);
			bRet = (pACType->m_sDescription.Left(strValue.GetLength()).CompareNoCase(strValue) != 0);
		}
		break;
	case 4 : //ICAO Group
		{
			if (m_LO != CACTypesManager::EQUAL)
				return true;

			CString strValue(m_strValue);
			int nIdx = strValue.Find('*');
			if(nIdx != -1)
				strValue = strValue.Left(nIdx);
			bRet = (pACType->m_sICAOGroup.Left(strValue.GetLength()).CompareNoCase(strValue) != 0);
		}
		break;
	case 5: // Height
		{
			double dValue = atof(m_strValue);
			bRet = !CompareValue(pACType->m_fHeight, m_LO, dValue);
		}
		break;
	case 6: // Len
		{
			double dValue = atof(m_strValue);
			bRet = !CompareValue(pACType->m_fLen, m_LO, dValue);
		}
		break;
	case 7: // Span
		{
			double dValue = atof(m_strValue);
			bRet = !CompareValue(pACType->m_fSpan, m_LO, dValue);
		}
		break;
	case 8: //wake wotex category
		{
			if (m_LO != CACTypesManager::EQUAL)
				return true;

			CString strValue(m_strValue);
			int nIdx = strValue.Find('*');
			if(nIdx != -1)
				strValue = strValue.Left(nIdx);
			bRet = (pACType->m_sWakeVortexCategory.Left(strValue.GetLength()).CompareNoCase(strValue) != 0);
		}
		break;
	case 9: // MZFW
		{
			double dValue = ARCUnit::LBSToKG(atof(m_strValue));
			bRet = !CompareValue(pACType->m_fMZFW, m_LO, dValue);
		}
		break;
	case 10: // OEW
		{
			double dValue = ARCUnit::LBSToKG(atof(m_strValue));
			bRet = !CompareValue(pACType->m_fOEW, m_LO, dValue);
		}
		break;
	case 11: // MTOW
		{
			double dValue = ARCUnit::LBSToKG(atof(m_strValue));
			bRet = !CompareValue(pACType->m_fMTOW, m_LO, dValue);
		}
		break;
	case 12: // MLW
		{
			double dValue = ARCUnit::LBSToKG(atof(m_strValue));
			bRet = !CompareValue(pACType->m_fMLW, m_LO, dValue);
		}
		break;
	case 13: //weight  category
		{
			if (m_LO != CACTypesManager::EQUAL)
				return true;

			CString strValue(m_strValue);
			int nIdx = strValue.Find('*');
			if(nIdx != -1)
				strValue = strValue.Left(nIdx);
			bRet = (pACType->m_sWeightCategory.Left(strValue.GetLength()).CompareNoCase(strValue) != 0);
		}
		break;
	case 14: // Capacity
		{
			int iValue = atoi(m_strValue);
			bRet = !CompareValue(pACType->m_iCapacity, m_LO, iValue);
		}
		break;
	case 15: //min turning radius
		{
			double dValue = atof(m_strValue);
			bRet = !CompareValue(pACType->m_fMinTurnRad, m_LO, dValue);
		}
		break;
	case 16: //cabin width
		{
			double dValue = atof(m_strValue);
			bRet = !CompareValue(pACType->m_dCabinWidth, m_LO, dValue);
		}
		break;

    //case 15: // ICAO Code...
	//case 16: // Other codes...
	//	{
	//		if (m_LO != CACTypesManager::EQUAL)
	//			return true;

	//		CString strValue(m_strValue);
	//		int nIdx = strValue.Find('*');
	//		if(nIdx != -1)
	//			strValue = strValue.Left(nIdx);
	//		bRet = (pACType->GetIATACode().Left(strValue.GetLength()).CompareNoCase(strValue) != 0);
	//	}
	//	break;

	default:
		bRet = true;
	}

	return bRet;
}

bool ACTypeRemove::IsDoubleValueEqual(double ldValue, double rdValue)
{
	return fabs(ldValue - rdValue) < 0.001;
}
bool ACTypeRemove::IsDoubleValueGraterEqual(double ldValue, double rdValue)
{
	return fabs(ldValue - rdValue) < 0.001 || ldValue > rdValue;
}

bool ACTypeRemove::IsDoubleValueLessEqual(double ldValue, double rdValue)
{
	return fabs(ldValue - rdValue) < 0.001 || ldValue < rdValue;
}

bool ACTypeRemove::CompareValue(const double d1stValue, 
								const CACTypesManager::LogicalOperator LO, 
								const double d2ndValue)
{
	bool bRet = false;
	switch(LO)
	{
	case CACTypesManager::EQUAL:
		bRet = IsDoubleValueEqual(d1stValue, d2ndValue);
		break;
	case CACTypesManager::NOT_EQUAL:
		bRet = (d1stValue != d2ndValue);
		break;
	case CACTypesManager::LESS:
		bRet = (d1stValue < d2ndValue);
		break;
	case CACTypesManager::LESS_EQUAL:
		bRet = IsDoubleValueLessEqual(d1stValue, d2ndValue);
		break;
	case CACTypesManager::GRATER:
		bRet = (d1stValue > d2ndValue);
		break;
	case CACTypesManager::GRATER_EQUAL:
		bRet = IsDoubleValueGraterEqual(d1stValue, d2ndValue);
		break;
	}

	return bRet;
}
bool ACTypeRemove::CompareValue(const int i1stValue, 
								const CACTypesManager::LogicalOperator LO, 
								const int i2ndValue)
{
	bool bRet = false;
	switch(LO)
	{
	case CACTypesManager::EQUAL:
		bRet = (i1stValue == i2ndValue);
		break;
	case CACTypesManager::NOT_EQUAL:
		bRet = (i1stValue != i2ndValue);
		break;
	case CACTypesManager::LESS:
		bRet = (i1stValue < i2ndValue);
		break;
	case CACTypesManager::LESS_EQUAL:
		bRet = (i1stValue <= i2ndValue);
		break;
	case CACTypesManager::GRATER:
		bRet = (i1stValue > i2ndValue);
		break;
	case CACTypesManager::GRATER_EQUAL:
		bRet = (i1stValue >= i2ndValue);
		break;
	}

	return bRet;
}
CACType* CACTypesManager::FilterActypeByStrings(std::vector<CString>& _ActypeNames) 
{
	int nCount = m_vACTypes.size();
	for( int i=0; i<nCount; i++ )
	{
		if(std::find(_ActypeNames.begin() , _ActypeNames.end() ,m_vACTypes[i]->GetIATACode()) != _ActypeNames.end())
			return m_vACTypes[i] ;
	}
	return NULL;
}

BOOL CACTypesManager::GetAllIATACode( std::vector<CString>& _CodeList )
{
	_CodeList.clear() ;
	int nCount = m_vACTypes.size() ;
	for (int i = 0 ;i < nCount ;i++)
	{
		if(!m_vACTypes[i]->GetIATACode().IsEmpty() && std::find(_CodeList.begin(),_CodeList.end(),m_vACTypes[i]->GetIATACode()) == _CodeList.end())
			_CodeList.push_back(m_vACTypes[i]->GetIATACode()) ;
	}
	return TRUE ;
}

BOOL CACTypesManager::GetAllICAOCode( std::vector<CString>& _CodeList )
{
	_CodeList.clear() ;
	int nCount = m_vACTypes.size() ;
	for (int i = 0 ;i < nCount ;i++)
	{
		if(!m_vACTypes[i]->GetICAOCode().IsEmpty() && std::find(_CodeList.begin(),_CodeList.end(),m_vACTypes[i]->GetICAOCode()) == _CodeList.end())
			_CodeList.push_back(m_vACTypes[i]->GetICAOCode()) ;
	}
	return TRUE ;
}

BOOL CACTypesManager::FindActypeByIATACode( const CString& _str,std::vector<CACType*>& _Actypelist )
{
	_Actypelist.clear() ;
	int nCount = m_vACTypes.size();
	long  res = _Actypelist.size() ;
	CACType* Pactype = NULL ;
	for( int i=0; i<nCount; i++ )
	{
		Pactype = m_vACTypes[i] ;
		if( Pactype->GetIATACode().CompareNoCase(_str) == 0 )
			_Actypelist.push_back(Pactype);
	}
	if((int)_Actypelist.size() > res)
		return TRUE ;
	else
		return FALSE ;
}

BOOL CACTypesManager::FindActypeByICAOCode( const CString& _str,std::vector<CACType*>& _Actypelist )
{
	_Actypelist.clear() ;
	int nCount = m_vACTypes.size();
	long  res = _Actypelist.size() ;
	CACType* Pactype = NULL ;
	for( int i=0; i<nCount; i++ )
	{
		Pactype = m_vACTypes[i] ;
		if( Pactype->GetICAOCode().CompareNoCase(_str) == 0 )
			_Actypelist.push_back(Pactype);
	}
	if((int)_Actypelist.size() > res)
		return TRUE ;
	else
		return FALSE ;
}

int CACTypesManager::getACTypeCount()const
{
	return (int)m_vACTypes.size();
}

