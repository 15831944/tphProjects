// Aircraft.cpp: implementation of the CAircraft class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "ACTypesManager.h"
#include "Aircraft.h"
#include "fileman.h"
#include "template.h"
#include "assert.h"
#include <algorithm>
#include "AirportDatabase.h"
#include "../Database/ARCportADODatabase.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// CACType Construction/Destruction
//////////////////////////////////////////////////////////////////////

CACType::CACType():m_ID(-1)
{
	m_sIATACode     = _T("");
	m_sDescription  = _T("");
	m_sFileName		= _T("");
	m_fHeight       = 0.0f;
	m_fLen          = 0.0f;
	m_fSpan         = 0.0f;
	m_fMTOW         = 0.0f;
	m_fOEW          = 0.0f;
	m_fMLW          = 0.0f;
	m_fMZFW         = 0.0f;
	m_iCapacity     = 0;
    m_sICAOCode     = _T("");
    m_sOtherCodes   = _T("");
	m_bLoad			= false;
	m_HasEdit       = FALSE ;
	m_dCabinWidth = 0 ;
	memset(m_hBmp,0,ACTYPEBMP_COUNT*sizeof(HBITMAP));

	m_sCompany = _T("");
	m_sICAOGroup = _T("");
	m_sWakeVortexCategory = _T("");
	m_sWeightCategory = _T("");
	m_fMinTurnRad = 0.0f;
	m_bNeedUpdate = true;
}

CACType::CACType( const CACType& acType )
{
	m_sIATACode = acType.m_sIATACode;
	m_sDescription = acType.m_sDescription;
	m_sFileName = acType.m_sFileName;
	m_fHeight = acType.m_fHeight;
	m_fLen = acType.m_fLen;
	m_fSpan = acType.m_fSpan;
	m_fMTOW = acType.m_fMTOW;
	m_fOEW = acType.m_fOEW;
	m_fMLW = acType.m_fMLW;
	m_fMZFW = acType.m_fMZFW;
	m_iCapacity = acType.m_iCapacity;
	m_sICAOCode = acType.m_sICAOCode;
	m_sOtherCodes = acType.m_sOtherCodes;
	m_bLoad = acType.m_bLoad;
	m_HasEdit = acType.m_HasEdit;
	m_dCabinWidth = acType.m_dCabinWidth;
	memcpy(m_hBmp,acType.m_hBmp,ACTYPEBMP_COUNT*sizeof(HBITMAP));
	m_sCompany = acType.m_sCompany;
	m_sICAOGroup = acType.m_sICAOGroup;
	m_sWakeVortexCategory = acType.m_sWakeVortexCategory;
	m_sWeightCategory = acType.m_sWeightCategory;
	m_fMinTurnRad = acType.m_fMinTurnRad;
	m_bNeedUpdate = acType.m_bNeedUpdate;

	for (size_t i = 0; i < m_vACDoorList.size(); i++)
	{
		ACTypeDoor* pDoor = new ACTypeDoor(*m_vACDoorList[i]);
		m_vACDoorList.push_back(pDoor);
	}
}

CACType::~CACType()
{
	ClearBMPHandle();
	clearAllDoorData() ;
}

void CACType::ClearBMPHandle()
{
	m_hBmp[0] = NULL;
	m_hBmp[1] = NULL;
	m_hBmp[2] = NULL;
	m_hBmp[3] = NULL;
}

void CACType::LoadAllBMPHandle(const CString& strDBPath)
{
	LoadBMPHandle(m_hBmp[0], getThumbZPath(strDBPath) );
	LoadBMPHandle(m_hBmp[1],getThumbYPath(strDBPath) );
	LoadBMPHandle(m_hBmp[2],getThumbXPath(strDBPath) );
	LoadBMPHandle(m_hBmp[3],getThumbISOPath(strDBPath) );

	m_bLoad = true;
}

void CACType::RemoveFiles(const CString& strDBPath)
{
	FileManager::DeleteFile(getThumbXPath(strDBPath));
	FileManager::DeleteFile(getThumbYPath(strDBPath));
	FileManager::DeleteFile(getThumbZPath(strDBPath));
	FileManager::DeleteFile(getThumbISOPath(strDBPath));

	ClearBMPHandle();
}

void CACType::LoadBMPHandle(HBITMAP& _hBmp,const CString& _strPicPath)
{
	_hBmp = (HBITMAP)::LoadImage(AfxGetInstanceHandle(), 
		_strPicPath, 
		IMAGE_BITMAP, 
		0, 
		0, 
		LR_LOADFROMFILE | LR_CREATEDIBSECTION | LR_DEFAULTSIZE); 
}

CString CACType::getThumbXPath( const CString& strDBPath ) const
{
	CString strPicPath;
	strPicPath.Format(_T("%s\\%sx.bmp"), strDBPath.GetString(), m_sIATACode);
	return strPicPath;
}

CString CACType::getThumbYPath( const CString& strDBPath ) const
{
	CString strPicPath;
	strPicPath.Format(_T("%s\\%sy.bmp"), strDBPath.GetString(), m_sIATACode);
	return strPicPath;

}

CString CACType::getThumbZPath( const CString& strDBPath ) const
{
	CString strPicPath;
	strPicPath.Format(_T("%s\\%sz.bmp"), strDBPath.GetString(), m_sIATACode);
	return strPicPath;

}

CString CACType::getThumbISOPath( const CString& strDBPath ) const
{
	CString strPicPath;
	strPicPath.Format(_T("%s\\%siso.bmp"), strDBPath.GetString(), m_sIATACode);
	return strPicPath;
}

bool CACType::loadDoorDatabase(CAirportDatabase* pAirportDatabase)
{
	ASSERT(pAirportDatabase);

	CString strSQL(_T(""));
	strSQL.Format(_T("SELECT * FROM TB_AIRPORTDB_ACTYPEDOOR WHERE ACNAME = '%s'" ),m_sIATACode);
	CADORecordset adoRecordset;
	long lCount = 0;
	try
	{
		CDatabaseADOConnetion::ExecuteSQLStatement(strSQL,lCount,adoRecordset,pAirportDatabase->GetAirportConnection()) ;
	}
	catch (CADOException e)
	{
		e.ErrorMessage() ;
		return false;
	}

	while(!adoRecordset.IsEOF())
	{
		ACTypeDoor* pDoor = new ACTypeDoor();
		pDoor->loadDatabase(adoRecordset);
		AddDoor(pDoor);
		adoRecordset.MoveNextData();
	}
	return true;
}

bool CACType::saveDoorDatabase(CAirportDatabase* pAirportDatabase)
{
	for (int i = 0; i < (int)m_vACDoorList.size(); i++)
	{
		ACTypeDoor* pDoor = m_vACDoorList.at(i);
		pDoor->m_strACName = m_sIATACode;
		pDoor->saveDatabase(pAirportDatabase);
	}

	for (int j = 0; j < (int)m_DelDoors.size(); j++)
	{
		ACTypeDoor* pDelDoor = m_DelDoors.at(j);
		pDelDoor->m_strACName = m_sIATACode;
		pDelDoor->deleteDatabase(pAirportDatabase);
	}
	
	return true;
}

bool CACType::deleteDoorDatabase(CAirportDatabase* pAirportDatabase)
{
	CString strSQL(_T(""));
	strSQL.Format(_T("DELETE FROM TB_AIRPORTDB_ACTYPEDOOR WHERE ACNAME = '%s'"),m_sIATACode);
	CDatabaseADOConnetion::ExecuteSQLStatement(strSQL,pAirportDatabase->GetAirportConnection());
	return true;
}

bool CACType::deleteAllDatabase(CAirportDatabase* pAirportDatabase)
{
	ASSERT(pAirportDatabase);

	CString strSQL(_T(""));
	strSQL.Format(_T("DELETE FROM TB_AIRPORTDB_ACTYPE"));
	try
	{
		CDatabaseADOConnetion::ExecuteSQLStatement(strSQL,pAirportDatabase->GetAirportConnection());
	}
	catch (CADOException& e)
	{
		e.ErrorMessage();
		return false;
	}
	return true;
}

bool CACType::loadDatabase(CAirportDatabase* pAirportDatabase,CADORecordset& aodRecordset)
{
	ASSERT(pAirportDatabase);
	if (!aodRecordset.IsEOF())
	{
		double dValue = 0.0f;
		aodRecordset.GetFieldValue(_T("ID"),m_ID);
		aodRecordset.GetFieldValue(_T("NAME"),m_sIATACode);
		aodRecordset.GetFieldValue(_T("DESCRIPTION"),m_sDescription);
		aodRecordset.GetFieldValue(_T("HEIGHT"),dValue);
		m_fHeight = static_cast<float>(dValue);
		aodRecordset.GetFieldValue(_T("LEN"),dValue);
		m_fLen = static_cast<float>(dValue);
		aodRecordset.GetFieldValue(_T("SPAN"),dValue);
		m_fSpan = static_cast<float>(dValue);
		aodRecordset.GetFieldValue(_T("MZFW"),dValue);
		m_fMZFW = static_cast<float>(dValue);
		aodRecordset.GetFieldValue(_T("OEW"),dValue);
		m_fOEW = static_cast<float>(dValue);
		aodRecordset.GetFieldValue(_T("MTOW"),dValue);
		m_fMTOW = static_cast<float>(dValue);
		aodRecordset.GetFieldValue(_T("MLW"),dValue);
		m_fMLW = static_cast<float>(dValue);
		aodRecordset.GetFieldValue(_T("CAPACITY"),m_iCapacity);
		aodRecordset.GetFieldValue(_T("CABINWIDTH"),m_dCabinWidth);
		aodRecordset.GetFieldValue(_T("COMPANY"),m_sCompany);
		aodRecordset.GetFieldValue(_T("ICAOGROUP"),m_sICAOGroup);
		aodRecordset.GetFieldValue(_T("WAKE_VORTEX_CATEGORY"),m_sWakeVortexCategory);
		aodRecordset.GetFieldValue(_T("WEIGHT_CATEGORY"),m_sWeightCategory);
		aodRecordset.GetFieldValue(_T("MIN_TURN_RAD"),dValue);
		m_fMinTurnRad = static_cast<float>(dValue);

		loadDoorDatabase(pAirportDatabase);
		//CACType::LoadAllBMPHandleFromDB(this,type);
	}

	m_bNeedUpdate = false;
	return true;
}

bool CACType::saveDatabase(CAirportDatabase* pAirportDatabase)
{
	ASSERT(pAirportDatabase);

	//judge actype whether update database
	if (m_bNeedUpdate == false)
		return true;

	m_sIATACode.Replace("\'" ,"\'\'");;
	m_sDescription.Replace("\'" ,"\'\'");
	m_sOtherCodes.Replace("\'" ,"\'\'");
	m_sICAOCode.Replace("\'" ,"\'\'");
		
	if(m_ID!= -1)
		updateDatabase(pAirportDatabase);
	else
	{
		CString strSQL(_T(""));
		strSQL.Format(_T("INSERT INTO TB_AIRPORTDB_ACTYPE (NAME,DESCRIPTION,ICAOGROUP,HEIGHT,LEN,SPAN,MZFW,OEW,MTOW,MLW,CAPACITY,CABINWIDTH,COMPANY,WAKE_VORTEX_CATEGORY,WEIGHT_CATEGORY,MIN_TURN_RAD)\
						 VALUES('%s','%s','%s',%f,%f,%f,%f,%f,%f,%f,%d, %0.2f,'%s','%s','%s',%.2f)"),
			m_sIATACode,
			m_sDescription,
			m_sICAOGroup,
			m_fHeight,
			m_fLen,
			m_fSpan,
			m_fMZFW,
			m_fOEW,
			m_fMTOW,
			m_fMLW,
			m_iCapacity,
			m_dCabinWidth,
			m_sCompany,
			m_sWakeVortexCategory,
			m_sWeightCategory,
			m_fMinTurnRad) ;
		m_ID = CDatabaseADOConnetion::ExecuteSQLStatementAndReturnScopeID(strSQL,pAirportDatabase->GetAirportConnection());

	}

	saveDoorDatabase(pAirportDatabase);
	return true;
}

bool CACType::deleteDatabase(CAirportDatabase* pAirportDatabase)
{
	ASSERT(pAirportDatabase);
	deleteDoorDatabase(pAirportDatabase);

	CString strSQL(_T(""));
	strSQL.Format(_T("DELETE FROM TB_AIRPORTDB_ACTYPE WHERE ID = %d"),m_ID);
	CDatabaseADOConnetion::ExecuteSQLStatement(strSQL,pAirportDatabase->GetAirportConnection());
	return true;
}

bool CACType::updateDatabase(CAirportDatabase* pAirportDatabase)
{
	ASSERT(pAirportDatabase);

	CString strSQL(_T(""));
	strSQL.Format(_T("UPDATE TB_AIRPORTDB_ACTYPE SET NAME = '%s',DESCRIPTION = '%s',ICAOGROUP = '%s',HEIGHT = %f,LEN = %f ,SPAN = %f ,MZFW = %f ,OEW = %f ,\
					 MTOW = %f ,MLW = %f ,CAPACITY = %d, CABINWIDTH = %0.2f,COMPANY = '%s',WAKE_VORTEX_CATEGORY = '%s',WEIGHT_CATEGORY = '%s',MIN_TURN_RAD = %.2f WHERE \
					ID = %d"),
					m_sIATACode,
					m_sDescription,
					m_sICAOGroup,
					m_fHeight,
					m_fLen,
					m_fSpan,
					m_fMZFW,
					m_fOEW,
					m_fMTOW,
					m_fMLW,
					m_iCapacity,
					m_dCabinWidth,
					m_sCompany,
					m_sWakeVortexCategory,
					m_sWeightCategory,
					m_fMinTurnRad,
					m_ID) ;

	CDatabaseADOConnetion::ExecuteSQLStatement(strSQL,pAirportDatabase->GetAirportConnection());
	return true;
}
//////////////////////////////////////////////////////////////////////
// CACCategory Construction/Destruction
//////////////////////////////////////////////////////////////////////

CACCategory::CACCategory( CACTypesManager* _pAcMan) : m_pAcMan( _pAcMan ),m_ID(-1)
{
}

CACCategory::~CACCategory()
{
}
void AddToList(CACTYPELIST& ACTList, CACType* pACT)
{
	for(unsigned i=0; i<ACTList.size(); i++) {
		if(ACTList[i]->GetIATACode().CompareNoCase(pACT->GetIATACode()) == 0) { //found the ac type
			return;
		}
	}
	ACTList.push_back(pACT);
}

void CACCategory::GetACTypeList(CACTYPELIST& ACTList)
{
/*	for(int i=0; i<m_vACTypeFilters.size(); i++) {
		CACTYPEFILTER actFilter = m_vACTypeFilters[i];
		CACTYPELIST TempList;
		assert( m_pAcMan );
		m_pAcMan->GetACTypesByFilter(actFilter, TempList);
		for(int j=0; j<TempList.size(); j++)
			AddToList(ACTList, TempList[j]);
		TempList.clear();
	}
*/
	ACTList.assign(m_vACTList.begin(),m_vACTList.end());
}

void CACCategory::AddFilter(CACTYPEFILTER actFilter)
{
	m_vACTypeFilters.push_back(actFilter);
}


int CACCategory::contains (const char *p_acType) const
{
  //  char str[AC_TYPE_LEN];
  //  for (unsigned i = 0; i < m_vACTypeFilters.size(); i++)
  //  {
  //      strcpy( str, m_vACTypeFilters[i].code );
  //      
		//// replace wildcards
  //      for (int j = 0; str[j] && p_acType[j]; j++)
  //      {
  //          if (str[j] == '?')
  //              str[j] = p_acType[j];
  //          else if (str[j] == '*')
  //              strcpy (str + j, p_acType + j);
  //      }
  //      if (!strcmpi (p_acType, str))
  //          return 1;
  //  }
  //  return 0;
	CACTYPELIST::const_iterator iter = m_vACTList.begin();
	CACTYPELIST::const_iterator iterEnd = m_vACTList.end();
	for (;iter != iterEnd; ++iter)
	{
		if ((*iter)->GetIATACode().CompareNoCase(p_acType) == 0)
		  return 1;
	}
	return 0;
}



void CACCategory::ClacACTypeList()
{
	//clear obsolete data
	//CACTYPELIST::iterator iter = m_vACTList.begin();
	//CACTYPELIST::iterator iterEnd = m_vACTList.end();
	//for (;iter != iterEnd; ++iter)
	//{
	//	delete *iter;
	//}
	
	m_vACTList.clear();

	//GetACTypeList(m_vACTList);
	ASSERT(m_pAcMan != NULL);
	if (!m_bHasScript)
	{
		CACTYPELIST* pACList = m_pAcMan->GetACTypeList();
		for (CACTYPELIST::const_iterator iterAC =  pACList->begin();
			iterAC != pACList->end(); iterAC++)
		{
			for (std::vector<CString>::const_iterator iterString = m_vDirectACTypes.begin();
				iterString != m_vDirectACTypes.end(); iterString++)
			{
				if (*iterString == (*iterAC)->GetIATACode())
				{
					m_vACTList.push_back((*iterAC));
				}
			}
		}
	}
	else
	{
		for (std::vector<CACTYPEFILTER>::const_iterator iter = m_vACTypeFilters.begin();
			iter != m_vACTypeFilters.end(); iter++)
		{
			CString strFilter(iter->code);
			TCHAR cLogical = strFilter.GetAt(0);
			CString strFilterScript = strFilter.Right(strFilter.GetLength() - 1);

			strFilterScript.Remove(_T('{'));
			strFilterScript.Remove(_T('}'));
			CACTYPELIST TempList1;
			m_pAcMan->GetACTypesByFilter(strFilterScript, TempList1);
			std::sort(TempList1.begin(),TempList1.end());
			CACTYPELIST TempList2(m_vACTList);

			if (iter == m_vACTypeFilters.begin() && cLogical == '&')
			{
				m_vACTList = TempList1;
				continue;
			}

			switch(cLogical)
			{
			case '&': // logical and
				{
 					m_vACTList.assign(m_pAcMan->getACTypeCount(), NULL);
 					CACTYPELIST::iterator iterResult = std::set_intersection(TempList1.begin(), TempList1.end(), 
 						TempList2.begin(), TempList2.end(), m_vACTList.begin());
 					m_vACTList.erase(iterResult, m_vACTList.end());
				}
				break;
			case '|':// logical or
				{
					m_vACTList.assign(m_pAcMan->getACTypeCount(), NULL);
					CACTYPELIST::iterator iterResult = std::set_union(TempList1.begin(), TempList1.end(), 
						TempList2.begin(), TempList2.end(), m_vACTList.begin());
					m_vACTList.erase(iterResult, m_vACTList.end());
				}
				break;
				//default:
			}
		}
	}
}
////////////////////////////////////////////////////////////////////////////////
// class CACCategoryDataSet

CACCategoryDataSet::CACCategoryDataSet()
	: DataSet(AircraftCategoryFile, 2.1f)
{
	m_pACCategoryList = NULL;
	m_pAcMan = NULL;
}

CACCategoryDataSet::~CACCategoryDataSet()
{
}

void CACCategoryDataSet::clear()
{
}

void CACCategoryDataSet::readData(ArctermFile& p_file)
{
	ASSERT(m_pACCategoryList != NULL && m_pAcMan != NULL);
	char buf[256];

	while (p_file.getLine() == 1)
	{
		// read the ACCategory name;
		if (p_file.getField(buf, 255) == 0)
			return;
		CACCategory* pACC = new CACCategory(m_pAcMan);
		pACC->m_sName = buf;

		// read the value indicate the type of filter
		int anInt;
		if (p_file.getInteger(anInt) == 0)
			return;
		
		if (anInt) // has script
		{
			pACC->m_bHasScript = true;
			while (p_file.getField(buf, 255) != 0)
			{
				CACTYPEFILTER actFilter;
				actFilter.code = buf;
				
				///////////////////////////////////////////
				int iLeftPos = actFilter.code.Find('{');
				if (iLeftPos == -1) // can not find the '{'
				{
					int iLogicalOperatorPos = actFilter.code.FindOneOf("><=!");
					if (iLogicalOperatorPos > 0)
					{
						int iRightPos = iLogicalOperatorPos + 1;
						if (actFilter.code.GetAt(iLogicalOperatorPos + 1) == '=')
							iRightPos++;

						actFilter.code.Insert(iLogicalOperatorPos, '{');
						actFilter.code.Insert(iRightPos + 1, '}');
					}
				}
				///////////////////////////////////////////
				pACC->AddFilter(actFilter);
			}
		}
		else // has not script
		{
			pACC->m_bHasScript = false;
			while (p_file.getField(buf, 255) != 0)
			{
				pACC->m_vDirectACTypes.push_back(CString(buf));
			}
		}
		m_pACCategoryList->push_back(pACC);
	}
}

void CACCategoryDataSet::readObsoleteData (ArctermFile& p_file)
{
	ASSERT(m_pACCategoryList != NULL && m_pAcMan != NULL);

	char buf[256];
	while (p_file.getLine() == 1)
	{
		if (p_file.getField(buf, 255) == 0)
			return;

		CACCategory* pACC = new CACCategory(m_pAcMan);
		pACC->m_sName = buf;
		pACC->m_bHasScript = true;

		while (p_file.getField(buf, 255) != 0)
		{
			CACTYPEFILTER actFilter;
			CString strFilter("|Code{=}");
			strFilter += buf;
			actFilter.code = strFilter;
			pACC->AddFilter(actFilter);
		}
		m_pACCategoryList->push_back(pACC);
	}

}

void CACCategoryDataSet::writeData(ArctermFile& p_file) const
{
	ASSERT(m_pACCategoryList != NULL);

	for (CACCATEGORYLIST::const_iterator iterCat = m_pACCategoryList->begin();
		 iterCat != m_pACCategoryList->end(); iterCat++)
	{
		CACCategory* pACC = *iterCat;
		p_file.writeField(pACC->m_sName);
		p_file.writeInt(pACC->m_bHasScript);
		if (pACC->m_bHasScript)
		{
			for (std::vector<CACTYPEFILTER>::const_iterator iterFilter = pACC->m_vACTypeFilters.begin();
				 iterFilter != pACC->m_vACTypeFilters.end(); iterFilter++)
			{
				p_file.writeField(iterFilter->code);
			}
		}
		else
		{
			for (std::vector<CString>::const_iterator iter = pACC->m_vDirectACTypes.begin();
				 iter != pACC->m_vDirectACTypes.end(); iter++)
			{
				p_file.writeField(*iter);
			}

		}
		p_file.writeLine();
	}
}

void CACCategoryDataSet::SetACCategoryList(CACCATEGORYLIST* pACCategoryList)
{
	ASSERT(pACCategoryList != NULL);
	m_pACCategoryList = pACCategoryList;
}

void CACCategoryDataSet::SetACTypesManager(CACTypesManager* pAcMan)
{
	ASSERT(pAcMan != NULL);
	m_pAcMan = pAcMan;
}

//////////////////////////////////////////////////////////////////////////
//the interface for actype Database

void CACType::ReadDataFromDB(std::vector<CACType*>& _AcTypes , int _airportDBID,DATABASESOURCE_TYPE type )
{
	if(_airportDBID == -1)
		return ;
    CString SQL ;
	SQL.Format(_T("SELECT * FROM TB_AIRPORTDB_ACTYPE WHERE AIRPORTDB_ID = %d"),_airportDBID) ;
	CADORecordset dataset ;
	long count = 0 ;
	try
	{
		CADODatabase::ExecuteSQLStatement(SQL,count,dataset,type) ;
	}
	catch (CADOException e)
	{
		e.ErrorMessage() ;
		return ;
	}
	int id  = -1 ;
	double double_val  = 0 ;
	CString strval ;
	CACType* pAcType = NULL ;
	int capacity ; 
	while (!dataset.IsEOF())
	{
		pAcType = new CACType ;
       dataset.GetFieldValue(_T("ID"),id) ;
	   pAcType->SetID(id) ;

	   dataset.GetFieldValue(_T("IATACODE"),strval) ;
	   pAcType->setIATACode(strval) ;

	   dataset.GetFieldValue(_T("DESCRIPTION"),strval) ;
	   pAcType->m_sDescription = strval ;

	   dataset.GetFieldValue(_T("FILENAME"),strval) ;
	   pAcType->m_sFileName = strval ;

	   dataset.GetFieldValue(_T("HEIGHT"),double_val) ;
	   pAcType->m_fHeight = (float)double_val ;

	   dataset.GetFieldValue(_T("LEN"),double_val) ;
	   pAcType->m_fLen = (float)double_val ;

	   dataset.GetFieldValue(_T("SPAN"),double_val) ;
	   pAcType->m_fSpan = (float)double_val ;

	   dataset.GetFieldValue(_T("MZFW"),double_val) ;
	   pAcType->m_fMZFW = (float)double_val ;

	   dataset.GetFieldValue(_T("OEW"),double_val) ;
	   pAcType->m_fOEW = (float)double_val ;

	   dataset.GetFieldValue(_T("MTOW"),double_val) ;
	   pAcType->m_fMTOW = (float)double_val ;

	   dataset.GetFieldValue(_T("MLW"),double_val) ;
	   pAcType->m_fMLW = (float)double_val ;

	   dataset.GetFieldValue(_T("CAPACITY"),capacity) ;
	   pAcType->m_iCapacity = capacity ;

	   dataset.GetFieldValue(_T("ICAOCODE"),strval) ;
	   pAcType->m_sICAOCode = strval ;

	   dataset.GetFieldValue(_T("OTHERCODE"),strval) ;
	   pAcType->m_sOtherCodes = strval ;

	   dataset.GetFieldValue(_T("CABINWIDTH"),double_val) ;
	   pAcType->m_dCabinWidth = double_val ;

	   CACType::LoadAllBMPHandleFromDB(pAcType,type) ;
       _AcTypes.push_back(pAcType) ;

	   dataset.MoveNextData() ;
	}
}

void CACType::WriteDataToDB(std::vector<CACType*>& _AcTypes , int _airportDBID,DATABASESOURCE_TYPE type)
{
	if(_airportDBID == -1)
		return ;
	std::vector<CACType*>::iterator iter = _AcTypes.begin() ;

	for ( ; iter != _AcTypes.end() ; iter++)
		{
			(*iter)->m_sIATACode.Replace("\'" ,"\'\'") ;
			(*iter)->m_sDescription.Replace("\'" ,"\'\'") ;
			(*iter)->m_sOtherCodes.Replace("\'" ,"\'\'") ;
			(*iter)->m_sICAOCode.Replace("\'" ,"\'\'") ;
			CACType::WriteActypeToDB(*iter,_airportDBID,type) ;
			CACType::WriteAllBMPToDB(*iter,type) ;
		}
	
}
void CACType::DeleteDataFromDB(std::vector<CACType*>& _AcTypes , int _airportDBID,DATABASESOURCE_TYPE type)
{
	std::vector<CACType*>::iterator iter = _AcTypes.begin() ;
	for ( ;iter != _AcTypes.end() ;iter++)
	{
	
		CACType::DeleteActypeFromDB(*iter,type) ;
	}
}
void CACType::WriteActypeToDB(CACType* _actype ,int _airportDBID,DATABASESOURCE_TYPE type)
{
  if(_actype == NULL || _airportDBID == -1)
	  return ;
  if(_actype->GetID() != -1)
	  CACType::UpDateActypeToDB(_actype , _airportDBID,type) ;
  else
  {
	  int id = -1 ;
	  CString SQL ;
	  SQL.Format(_T("INSERT INTO TB_AIRPORTDB_ACTYPE (IATACODE,DESCRIPTION,FILENAME,HEIGHT,LEN,SPAN,MZFW,OEW,MTOW,MLW,CAPACITY,ICAOCODE,OTHERCODE,AIRPORTDB_ID,CABINWIDTH) VALUES('%s','%s','%s',%f,%f,%f,%f,%f,%f,%f,%d,'%s','%s',%d, %0.2f)"),
		  _actype->GetIATACode(),
		  _actype->m_sDescription,
		  _actype->m_sFileName,
		  _actype->m_fHeight,
		  _actype->m_fLen,
		  _actype->m_fSpan,
		  _actype->m_fMZFW,
		  _actype->m_fOEW,
		  _actype->m_fMTOW,
		  _actype->m_fMLW,
		  _actype->m_iCapacity,
		  _actype->GetICAOCode(),
		  _actype->m_sOtherCodes,
		  _airportDBID,
		  _actype->m_dCabinWidth) ;
		  id = CADODatabase::ExecuteSQLStatementAndReturnScopeID(SQL,type) ;
		  _actype->SetID(id) ;
	 
  }
}
void CACType::UpDateActypeToDB(CACType* _actype ,int _airportDBID,DATABASESOURCE_TYPE type)
{
     if(_actype == NULL || _airportDBID == -1 || !_actype->IsEdit())
		 return ;
	 if(_actype->GetID() == -1)
		 WriteActypeToDB(_actype,_airportDBID,type);
	 else
	 {
		 CString SQL ;
		 SQL.Format(_T("UPDATE TB_AIRPORTDB_ACTYPE SET IATACODE = '%s',DESCRIPTION = '%s' ,FILENAME = '%s' ,HEIGHT = %f,LEN = %f ,SPAN = %f ,MZFW = %f ,OEW = %f ,MTOW = %f ,MLW = %f ,CAPACITY = %d,ICAOCODE = '%s',OTHERCODE ='%s',AIRPORTDB_ID =%d, CABINWIDTH = %0.2f WHERE \
					   ID = %d"),
					   _actype->GetIATACode(),
					   _actype->m_sDescription,
					   _actype->m_sFileName,
					   _actype->m_fHeight,
					   _actype->m_fLen,
					   _actype->m_fSpan,
					   _actype->m_fMZFW,
					   _actype->m_fOEW,
					   _actype->m_fMTOW,
					   _actype->m_fMLW,
					   _actype->m_iCapacity,
					   _actype->GetICAOCode(),
					   _actype->m_sOtherCodes,
					   _airportDBID,
					   _actype->m_dCabinWidth,
					   _actype->GetID()) ;

			 CADODatabase::ExecuteSQLStatement(SQL,type) ;
	 }
    
}
void CACType::DeleteActypeFromDB(CACType* _actype,DATABASESOURCE_TYPE type)
{
	if(_actype == NULL || _actype->GetID() == -1)
		return ;
	CString SQL ;
	SQL.Format(_T("DELETE FROM TB_AIRPORTDB_ACTYPE WHERE ID = %d") ,_actype->GetID()) ;
	CADODatabase::ExecuteSQLStatement(SQL,type) ;
}
void CACType::DeleteAllDataFromDB(int _airportDBID,DATABASESOURCE_TYPE type )
{
	CString SQL ;
	SQL.Format(_T("DELETE FROM TB_AIRPORTDB_ACTYPE WHERE AIRPORTDB_ID = %d") ,_airportDBID) ;
	try
	{
		CADODatabase::BeginTransaction(type) ;
		CADODatabase::ExecuteSQLStatement(SQL,type) ;
		CADODatabase::CommitTransaction(type) ;
	}
	catch (CADOException e)
	{
		e.ErrorMessage();
		CADODatabase::RollBackTransation(type) ;
		return ;
	}
}
void CACType::LoadAllBMPHandleFromDB(CACType* _actype,DATABASESOURCE_TYPE type)
{

}
void CACType::WriteAllBMPToDB(CACType* _actype,DATABASESOURCE_TYPE type)
{

}

BOOL CACType::FitActype( const CString& _val )
{
		return GetIATACode().CompareNoCase(_val)==0 ;
}

BOOL CACType::FitActype( const TCHAR* _val )
{
  CString val ;
  val.Format(_T("%s"),_val) ;
  return FitActype(val) ;
}

void CACType::AddDoor( ACTypeDoor* _door )
{
	if(std::find(m_vACDoorList.begin() , m_vACDoorList.end(),_door) == m_vACDoorList.end())
		m_vACDoorList.push_back(_door) ;
	else
		delete _door;
}

void CACType::RemoveDoor( ACTypeDoor* _door )
{
	ACTYPEDOORLIST_ITER iter = std::find(m_vACDoorList.begin() , m_vACDoorList.end(),_door) ;
	if(iter != m_vACDoorList.end())
		m_vACDoorList.erase(iter) ;

	m_DelDoors.push_back(_door) ;
}



void CACType::DeleteDoorData(  DATABASESOURCE_TYPE type /*= DATABASESOURCE_TYPE_ACCESS*/ )
{
	ACTypeDoor::DeleteDataFromDB(m_DelDoors,GetID(),type) ;
	for (int i = 0 ; i < (int)m_DelDoors.size() ;i++)
	{
		delete m_DelDoors[i] ;
	}
	m_DelDoors.clear() ;
}

void CACType::clearAllDoorData()
{
	for (int i = 0 ; i < (int)m_vACDoorList.size() ;i++)
	{
		delete m_vACDoorList[i] ;
	}
	m_vACDoorList.clear() ;
}

void CACType::operator=( const CACType& p_actype )
{
	setIATACode(p_actype.m_sIATACode) ;
	m_sDescription = p_actype.m_sDescription ;
	m_sFileName = p_actype.m_sFileName ;
	m_fHeight = p_actype.m_fHeight ;
	m_fLen = p_actype.m_fLen ;
	m_fSpan = p_actype.m_fSpan ;
	m_fMZFW = p_actype.m_fMZFW ;
	m_fOEW = p_actype.m_fOEW ;
	m_fMTOW = p_actype.m_fMTOW ;
	m_fMLW = p_actype.m_fMLW;
	m_iCapacity = p_actype.m_iCapacity ;
	m_sICAOCode = p_actype.m_sICAOCode ;
	m_sOtherCodes = p_actype.m_sOtherCodes ;
	m_dCabinWidth = p_actype.m_dCabinWidth ;
	m_sCompany = p_actype.m_sCompany;
	m_sICAOGroup = p_actype.m_sICAOGroup;
	m_sWakeVortexCategory = p_actype.m_sWakeVortexCategory;
	m_sWeightCategory = p_actype.m_sWeightCategory;
	m_fMinTurnRad = p_actype.m_fMinTurnRad;

	ACTYPEDOORLIST::const_iterator iter = p_actype.m_vACDoorList.begin();
	for (; iter != p_actype.m_vACDoorList.end(); ++iter)
	{
		ACTypeDoor* pDoor = new ACTypeDoor(**iter);
		m_vACDoorList.push_back(pDoor);
	}

	for (iter = p_actype.m_DelDoors.begin(); iter != p_actype.m_DelDoors.end(); ++iter)
	{
		ACTypeDoor* pDelDoor = new ACTypeDoor(**iter);
		m_DelDoors.push_back(pDelDoor);
	}
}

void CACType::AddDoorMessager( const ACTYPEDOORLIST& _door )
{
	for (int i =0 ; i < (int)_door.size() ;i++)
	{
		ACTypeDoor* pnewDoor = new ACTypeDoor ;
		*pnewDoor = *_door[i] ;
		m_vACDoorList.push_back(pnewDoor) ;
	}
}

void CACType::ResetDoorID()
{
	for (int i =0 ; i < (int)m_vACDoorList.size() ;i++)
	{
		ACTypeDoor* pnewDoor = m_vACDoorList[i] ;
		pnewDoor->SetID(-1) ;
	}
}

//////////////////new version database read and save//////////////////////////////////////
bool CACCategory::loadDatabase(CADORecordset& aodRecordset,CAirportDatabase* pAirportDatabase)
{
	if (!aodRecordset.IsEOF())
	{
		aodRecordset.GetFieldValue(_T("STR_NAME"),m_sName);
		int bHas = 0;
		aodRecordset.GetFieldValue(_T("HAS_SCRIPT"),bHas);
		m_bHasScript = bHas?true:false;

		aodRecordset.GetFieldValue(_T("ID"),m_ID);

		if(m_bHasScript)
			loadScriptFilter(pAirportDatabase);
		else
			loadDirectFilter(pAirportDatabase);
	}
	return true;
}

bool CACCategory::saveDatabase(CAirportDatabase* pAirportDatabase)
{
	ASSERT(pAirportDatabase);

	if (m_ID == -1)
	{
		CString strSQL(_T(""));
		m_sName.Replace("\'" ,"\'\'");
		strSQL.Format(_T("INSERT INTO TB_AIRPORTDB_CACCATEGORY (STR_NAME,HAS_SCRIPT) VALUES('%s',%d)"),m_sName,m_bHasScript);
		m_ID = CDatabaseADOConnetion::ExecuteSQLStatementAndReturnScopeID(strSQL,pAirportDatabase->GetAirportConnection());
	}
	else
	{
		updateDatabase(pAirportDatabase);
	}

	if(m_bHasScript)
		saveScriptFilter(pAirportDatabase);
	else
		saveDirectFilter(pAirportDatabase);
	return true;
}

bool CACCategory::deleteDatabase(CAirportDatabase* pAirportDatabase)
{
	CString strSQL(_T(""));
	strSQL.Format(_T("DELETE FROM TB_AIRPORTDB_CACCATEGORY WHERE ID = %d"),m_ID);
	CDatabaseADOConnetion::ExecuteSQLStatement(strSQL,pAirportDatabase->GetAirportConnection());
	return true;
}

bool CACCategory::updateDatabase(CAirportDatabase* pAirportDatabase)
{
	ASSERT(pAirportDatabase);
	int nAirportID = pAirportDatabase->GetID();

	CString strSQL(_T(""));
	m_sName.Replace("\'" ,"\'\'");
	strSQL.Format(_T("UPDATE TB_AIRPORTDB_CACCATEGORY SET STR_NAME = '%s' ,HAS_SCRIPT = %d WHERE ID =%d"),m_sName,m_bHasScript,m_ID);
	CDatabaseADOConnetion::ExecuteSQLStatement(strSQL,pAirportDatabase->GetAirportConnection()) ;
	return true;
}

bool CACCategory::loadScriptFilter(CAirportDatabase* pAirportDatabase)
{
	CString strSQL(_T(""));
	strSQL.Format(_T("SELECT * FROM TB_AIRPORTDB_CACCATEGORY_ACFILTER WHERE CACCATE_ID = %d") , m_ID);
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

	CACTYPEFILTER actFilter;
	while(!adoRecordset.IsEOF())
	{
		adoRecordset.GetFieldValue(_T("STR_SCRIPT"),actFilter.code) ;
		AddFilter(actFilter);
		adoRecordset.MoveNextData();
	}
	return true;
}

bool CACCategory::saveScriptFilter(CAirportDatabase* pAirportDatabase)
{
	deleteScriptFilter(pAirportDatabase);

	CString strSQL(_T(""));
	std::vector<CACTYPEFILTER>::const_iterator iterFilter = m_vACTypeFilters.begin();
	for (;iterFilter != m_vACTypeFilters.end(); iterFilter++)
	{
		strSQL.Format(_T("INSERT INTO TB_AIRPORTDB_CACCATEGORY_ACFILTER (STR_SCRIPT,CACCATE_ID) VALUES('%s',%d) "),(*iterFilter).code,m_ID);
		CDatabaseADOConnetion::ExecuteSQLStatement(strSQL,pAirportDatabase->GetAirportConnection());
	}
	return true;
}

bool CACCategory::deleteScriptFilter(CAirportDatabase* pAirportDatabase)
{
	CString strSQL(_T(""));
	strSQL.Format(_T("DELETE FROM TB_AIRPORTDB_CACCATEGORY_ACFILTER WHERE CACCATE_ID = %d"),m_ID);
	CDatabaseADOConnetion::ExecuteSQLStatement(strSQL,pAirportDatabase->GetAirportConnection());
	return true;
}

bool CACCategory::loadDirectFilter(CAirportDatabase* pAirportDatabase)
{
	CString strSQL(_T(""));
	strSQL.Format(_T("SELECT * FROM TB_AIRPORTDB_CACCATEGORY_DIRECTACTYPE WHERE CACCATE_ID = %d"),m_ID);
	CADORecordset adoRecordset;
	long lCount = 0;
	try
	{
		CDatabaseADOConnetion::ExecuteSQLStatement(strSQL,lCount,adoRecordset,pAirportDatabase->GetAirportConnection()) ;

	}
	catch (CADOException& e)
	{
		e.ErrorMessage();
		return false;
	}

	CString strValue(_T(""));
	while (!adoRecordset.IsEOF())
	{
		adoRecordset.GetFieldValue(_T("ACTYPE"),strValue);

		m_vDirectACTypes.push_back(strValue);
		adoRecordset.MoveNextData();
	}
	return true;
}

bool CACCategory::deleteDirectFilter(CAirportDatabase* pAirportDatabase)
{
	CString strSQL(_T(""));
	strSQL.Format(_T("DELETE FROM TB_AIRPORTDB_CACCATEGORY_DIRECTACTYPE WHERE CACCATE_ID = %d"),m_ID);
	CDatabaseADOConnetion::ExecuteSQLStatement(strSQL,pAirportDatabase->GetAirportConnection());
	return true;
}

bool CACCategory::saveDirectFilter(CAirportDatabase* pAirportDatabase)
{
	ASSERT(pAirportDatabase);

	deleteDirectFilter(pAirportDatabase);
	std::vector<CString>::const_iterator iter = m_vDirectACTypes.begin();
	for (;iter != m_vDirectACTypes.end(); iter++)
	{
		CString strSQL(_T(""));
		strSQL.Format(_T("INSERT INTO TB_AIRPORTDB_CACCATEGORY_DIRECTACTYPE (ACTYPE,CACCATE_ID) VALUES('%s',%d) "),*iter,m_ID) ;
		CDatabaseADOConnetion::ExecuteSQLStatement(strSQL,pAirportDatabase->GetAirportConnection());
	}
	return true;
}
////////////////old code version, no more use//////////////////////////////////////////////////////////
void CACCategory::ReadDataFromDB(std::vector<CACCategory*>& _dataSet , int _airportID,CACTypesManager* m_pAcMan,DATABASESOURCE_TYPE type)
{
	if(_airportID == -1)
		return ;
	CString SQL ;
	SQL.Format(_T("SELECT * FROM TB_AIRPORTDB_CACCATEGORY WHERE AIRPORTDB_ID = %d"),_airportID) ;
	CADORecordset recordSet ;
	long count = 0 ;
	try
	{
		CADODatabase::ExecuteSQLStatement(SQL,count,recordSet,type) ;
	}
	catch (CADOException e)
	{
		e.ErrorMessage() ;
		return ;
	}
    CACCategory*  pAcCategory = NULL ;
	CString name ;
	int hasscript = 0 ;
	int id = -1 ;
	while (!recordSet.IsEOF())
	{
         pAcCategory = new CACCategory(m_pAcMan) ;

		 recordSet.GetFieldValue(_T("STR_NAME"),name) ;
		 pAcCategory->m_sName = name ;

		 recordSet.GetFieldValue(_T("HAS_SCRIPT"),hasscript) ;
		 pAcCategory->m_bHasScript = (hasscript== 0?FALSE:TRUE) ;

         recordSet.GetFieldValue(_T("ID"),id) ;
		 pAcCategory->SetID(id) ;

		 if(hasscript)
			 CACCategory::ReadDataWithScript(pAcCategory,type) ;
		 else
			 CACCategory::ReadDataWithDirectActype(pAcCategory,type) ;
		 _dataSet.push_back(pAcCategory) ;

		 recordSet.MoveNextData() ;
	}
}
void CACCategory::ReadDataWithScript(CACCategory* _CAccategory,DATABASESOURCE_TYPE type)
{
	if(_CAccategory == NULL || _CAccategory->GetID() == -1)
		return ;
	CString SQL ;
	SQL.Format(_T("SELECT * FROM TB_AIRPORTDB_CACCATEGORY_ACFILTER WHERE CACCATE_ID = %d") , _CAccategory->GetID()) ;
	CADORecordset dataset ;
	long count = 0 ;
	try
	{
		CADODatabase::ExecuteSQLStatement(SQL,count,dataset,type) ;
	}
	catch (CADOException e)
	{
		e.ErrorMessage() ;
		return ;
	}
	CString str ;
	CACTYPEFILTER actFilter;
	while(!dataset.IsEOF())
	{
		dataset.GetFieldValue(_T("STR_SCRIPT"),str) ;
        actFilter.code = str ;

		_CAccategory->AddFilter(actFilter) ;
		dataset.MoveNextData() ;
	}
}
void CACCategory::ReadDataWithDirectActype(CACCategory* _CAccategory,DATABASESOURCE_TYPE type)
{
	if(_CAccategory == NULL || _CAccategory->GetID() == -1)
		return ;
	CString SQL ;
	SQL.Format(_T("SELECT * FROM TB_AIRPORTDB_CACCATEGORY_DIRECTACTYPE WHERE CACCATE_ID = %d"),_CAccategory->GetID()) ;
	CADORecordset dataset ;
	long count ;
	try
	{
		CADODatabase::ExecuteSQLStatement(SQL,count,dataset,type) ;
		
	}
	catch (CADOException e)
	{
		e.ErrorMessage() ;
		return ;
	}
	CString val ;
    while (!dataset.IsEOF())
    {
		dataset.GetFieldValue(_T("ACTYPE"),val) ;

		_CAccategory->m_vDirectACTypes.push_back(val) ;
		dataset.MoveNextData() ;
    }
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void CACCategory::WriteDataToDB(std::vector<CACCategory*>& _dataSet , int _airportID,DATABASESOURCE_TYPE type)
{
   if(_airportID == -1)
	   return ;
   std::vector<CACCategory*>::iterator iter = _dataSet.begin() ;
   for ( ; iter != _dataSet.end() ;iter++)
   {
	   if((*iter)->GetID() == -1)
		   CACCategory::WriteData(*iter,_airportID,type) ;
	   else
		   CACCategory::UpdateDate(*iter,_airportID,type) ;
	   if((*iter)->m_bHasScript)
		   CACCategory::WriteDataWithScript(*iter,type) ;
	   else
		   CACCategory::WriteDataWithDirectActype(*iter,type) ;
   }
}
void CACCategory::DeleteDataFromDB(std::vector<CACCategory*>& _dataSet ,DATABASESOURCE_TYPE type)
{
	std::vector<CACCategory*>::iterator iter = _dataSet.begin() ;
	for ( ; iter != _dataSet.end() ;iter++)
	{
		CACCategory::DeleteData(*iter,type) ;
	}
}
void CACCategory::DeleteData(CACCategory* _caccategory,DATABASESOURCE_TYPE type)
{
	if(_caccategory == NULL || _caccategory->GetID() == -1)
		return ;
	CString SQL ;
	SQL.Format(_T("DELETE FROM TB_AIRPORTDB_CACCATEGORY WHERE ID = %d") , _caccategory->GetID()) ;
	CADODatabase::ExecuteSQLStatement(SQL,type) ;
}
void CACCategory::WriteData(CACCategory* _caccategory,int _airportID,DATABASESOURCE_TYPE type)
{
	if(_caccategory == NULL || _airportID == -1)
		return ;
	CString SQL ;
	_caccategory->m_sName.Replace("\'" ,"\'\'") ;
	SQL.Format(_T("INSERT INTO TB_AIRPORTDB_CACCATEGORY (STR_NAME,HAS_SCRIPT,AIRPORTDB_ID) VALUES('%s',%d,%d)"),_caccategory->m_sName,_caccategory->m_bHasScript,_airportID) ;
	int id = -1;
		id = CADODatabase::ExecuteSQLStatementAndReturnScopeID(SQL,type) ;
		_caccategory->SetID(id) ;
}
void CACCategory::UpdateDate(CACCategory* _caccategory,int _airportID,DATABASESOURCE_TYPE type)
{
	if(_caccategory == NULL || _airportID == -1)
		return ;
	CString SQL ;
	_caccategory->m_sName.Replace("\'" ,"\'\'") ;
	SQL.Format(_T("UPDATE TB_AIRPORTDB_CACCATEGORY SET STR_NAME = '%s' ,HAS_SCRIPT = %d WHERE ID =%d AND AIRPORTDB_ID = %d") ,_caccategory->m_sName,_caccategory->m_bHasScript,_caccategory->GetID(),_airportID) ;
		CADODatabase::ExecuteSQLStatement(SQL,type) ;
}
void CACCategory::WriteDataWithScript(CACCategory* _CAccategory,DATABASESOURCE_TYPE type)
{
	if(_CAccategory == NULL )
		return ;
	//ASSERT(_CAccategory->GetID()==-1) ;
	CString SQL ;
	SQL.Format(_T("DELETE FROM TB_AIRPORTDB_CACCATEGORY_ACFILTER WHERE CACCATE_ID = %d"),_CAccategory->GetID()) ;
	CADODatabase::ExecuteSQLStatement(SQL,type) ;
	std::vector<CACTYPEFILTER>::const_iterator iterFilter = _CAccategory->m_vACTypeFilters.begin() ;
	for (;iterFilter != _CAccategory->m_vACTypeFilters.end(); iterFilter++)
	{
	   CString SQL ;
       SQL.Format(_T("INSERT INTO TB_AIRPORTDB_CACCATEGORY_ACFILTER (STR_SCRIPT,CACCATE_ID) VALUES('%s',%d) "),(*iterFilter).code,_CAccategory->GetID()) ;
	 CADODatabase::ExecuteSQLStatement(SQL,type) ;
	}
}
void CACCategory::WriteDataWithDirectActype(CACCategory* _CAccategory,DATABASESOURCE_TYPE type)
{
	if(_CAccategory == NULL )
		return ;
	//ASSERT(_CAccategory->GetID()==-1) ;
	CString SQL ;
	SQL.Format(_T("DELETE FROM TB_AIRPORTDB_CACCATEGORY_DIRECTACTYPE WHERE CACCATE_ID = %d"),_CAccategory->GetID()) ;
	CADODatabase::ExecuteSQLStatement(SQL,type) ;

	std::vector<CString>::const_iterator iter = _CAccategory->m_vDirectACTypes.begin();
	for (;iter != _CAccategory->m_vDirectACTypes.end(); iter++)
	{
		CString SQL ;
		SQL.Format(_T("INSERT INTO TB_AIRPORTDB_CACCATEGORY_DIRECTACTYPE (ACTYPE,CACCATE_ID) VALUES('%s',%d) "),*iter,_CAccategory->GetID()) ;
		CADODatabase::ExecuteSQLStatement(SQL,type) ;
	}
}

//////////////////new version database read and save////////////////////////////////////////
bool CACCategoryDataSet::loadDatabase(CAirportDatabase* pAirportDatabase)
{
	ASSERT(pAirportDatabase);
	CString strSQL(_T(""));
	strSQL.Format(_T("SELECT * FROM TB_AIRPORTDB_CACCATEGORY"));
	CADORecordset adoRecordSet;
	long lCount = 0;
	try
	{
		CDatabaseADOConnetion::ExecuteSQLStatement(strSQL,lCount,adoRecordSet,pAirportDatabase->GetAirportConnection());
	}
	catch (CADOException e)
	{
		e.ErrorMessage();
		return false;
	}

	while(!adoRecordSet.IsEOF())
	{
		CACCategory* pACCategory = new CACCategory(m_pAcMan);
		pACCategory->loadDatabase(adoRecordSet,pAirportDatabase);
		m_pACCategoryList->push_back(pACCategory);
		adoRecordSet.MoveNextData();
	}
	return true;
}

bool CACCategoryDataSet::saveDatabase(CAirportDatabase* pAirportDatabase)
{
	for (int i = 0; i < (int)m_pACCategoryList->size(); i++)
	{
		CACCategory* pACCategory = m_pACCategoryList->at(i);
		pACCategory->saveDatabase(pAirportDatabase);
	}

	for (int j = 0; j < (int)m_DelCategoryList->size(); j++)
	{
		CACCategory* pDelACCategory = m_DelCategoryList->at(j);
		pDelACCategory->deleteDatabase(pAirportDatabase);
	}
	return true;
}

bool CACCategoryDataSet::deleteDatabase(CAirportDatabase* pAirportDatabase)
{
	ASSERT(pAirportDatabase);

	try
	{
		CString strSQL(_T(""));
		strSQL.Format(_T("DELETE FROM TB_AIRPORTDB_CACCATEGORY"));
		CDatabaseADOConnetion::ExecuteSQLStatement(strSQL,pAirportDatabase->GetAirportConnection());
	}
	catch (CADOException& e)
	{
		e.ErrorMessage();
		return false;
	}
	return true;
}
/////////////////old code versin, no more use/////////////////////////////////////////////////////////
void CACCategoryDataSet::ReadDataFromDB(int _airportID,DATABASESOURCE_TYPE type )
{
	CACCategory::ReadDataFromDB(*m_pACCategoryList,_airportID,m_pAcMan,type) ;
}
void CACCategoryDataSet::SaveDataToDB(int _airportID,DATABASESOURCE_TYPE type )
{

		CACCategory::WriteDataToDB(*m_pACCategoryList,_airportID,type) ;
	    CACCategory::DeleteDataFromDB(*m_DelCategoryList,type) ;
}
void CACCategoryDataSet::DeleteAllData(int airportID,DATABASESOURCE_TYPE type)
{
	try
	{
		CADODatabase::BeginTransaction(type) ;
		CString SQL ;
		SQL.Format(_T("DELETE FROM TB_AIRPORTDB_CACCATEGORY WHERE AIRPORTDB_ID = %d") , airportID) ;
		CADODatabase::ExecuteSQLStatement(SQL,type) ;
		CADODatabase::CommitTransaction(type) ;
	}
	catch (CADOException e)
	{
		CADODatabase::RollBackTransation(type) ;
		
	}

}
//////////////////////////////////////////////////////////////////////////