// AirlineManager.cpp: implementation of the CAirlineManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "..\main\resource.h"
#include "AirlineManager.h"
#include "ProjectManager.h"
#include "common\airportdatabase.h"
#include "assert.h"
#include "../Database/ARCportADODatabase.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// define
static const CString _strAirlineFileName	= "Airlines.dat";
static const CString _strSubAirlineFileName	= "subairlines.sal";
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAirlinesManager::CAirlinesManager(CAirportDatabase* pAirportDatabase):m_AirportID(-1)
,m_pAirportDatabase(pAirportDatabase)
{

}

CAirlinesManager::~CAirlinesManager()
{
	for(unsigned i=0; i<m_vAirlines.size(); i++)
		delete m_vAirlines[i];
	m_vAirlines.clear();

	for(unsigned j =0; j<m_vSubairlines.size(); j++)
		delete m_vSubairlines[j];
	m_vSubairlines.clear();

	ClearDelAirlines() ;
	ClearDelSubAirlines() ;
}
void CAirlinesManager::Clear()
{
	for(unsigned i=0; i<m_vAirlines.size(); i++)
		delete m_vAirlines[i];
	m_vAirlines.clear();

	for(unsigned j=0; j<m_vSubairlines.size(); j++)
		delete m_vSubairlines[j];
	m_vSubairlines.clear();

	ClearDelAirlines() ;
	ClearDelSubAirlines() ;
}
BOOL CAirlinesManager::LoadDataFromOtherFile( const CString& _strDBPath )
{

   Clear();
	CString _currentStr ;
	_currentStr = m_strDBPath ;
	BOOL res = LoadData(_strDBPath) ;
	m_strDBPath = _currentStr ;
	return res ;
}
BOOL CAirlinesManager::ReadCommonAirlineData(const CString& _Path) 
{	
	CFile* pFile = NULL;
	CString sFileName = _Path + "\\" + _strAirlineFileName;
	BOOL bRet = TRUE;
	// read  airline
	try
	{
		pFile = new CFile(sFileName, CFile::modeRead | CFile::shareDenyNone);
		CArchive ar(pFile, CArchive::load);
		char line[513];
		//skip a line
		CString csLine;
		ar.ReadString( csLine );
		//read line 2
		ar.ReadString( csLine );
		csLine.MakeUpper();
		strcpy( line, csLine );
		if(_stricmp(line, "AIRLINES DATABASE") == 0)
		{
			//skip the column names
			ar.ReadString( csLine );
			//read the values
			//read a line
			ar.ReadString( csLine );
			csLine.MakeUpper();
			strcpy( line, csLine );
			while( *line != '\0' )
			{
				CAirline* pAL = new CAirline();
				char* b = line;
				char* p = NULL;
				int c = 1;
				while((p = strchr(b, ',')) != NULL)
				{
					*p = '\0';
					switch(c)
					{
					case 1: // Airline IATA code
						pAL->m_sAirlineIATACode = b;
						break;
					case 2: //Desc
						pAL->m_sLongName = b;
						break;
					case 3: // ICAO code...
						pAL->m_sICAOCode = b;
						break;
					case 4: // Other codes...
						pAL->m_sOtherCodes = b;
						break;
					case 5: // Street address...
						pAL->m_sStreetAddress = b;
						break;
					case 6: // City...
						pAL->m_sCity = b;
						break;
					case 7: // Country...
						pAL->m_sCountry = b;
						break;
					case 8: // HQ zip code...
						pAL->m_sHQZipCode = b;
						break;
					case 9: // Continent...
						pAL->m_sContinent = b;
						break;
					case 10: // Telephone...
						pAL->m_sTelephone = b;
						break;
					case 11: // Fax...
						pAL->m_sFax = b;
						break;
					case 12: // E-Mail...
						pAL->m_sEMail = b;
						break;
					case 13: // Director...
						pAL->m_sDirector = b;
						break;
					case 14: // Fleet...
						pAL->m_sFleet = b;
						break;
					default:
						CString tmp = b;
						pAL->m_sLongName = pAL->m_sLongName + "," + tmp;
						break;
					}
					b = ++p;
					c++;
				}
				if(b!=NULL) // the last column did not have a comma after it
				{
					switch(c)
					{
					case 1: // Airline IATA code
						pAL->m_sAirlineIATACode = b;
						break;
					case 2: //Desc
						pAL->m_sLongName = b;
						break;
					case 3: // ICAO code...
						pAL->m_sICAOCode = b;
						break;
					case 4: // Other codes...
						pAL->m_sOtherCodes = b;
						break;
					case 5: // Street address...
						pAL->m_sStreetAddress = b;
						break;
					case 6: // City...
						pAL->m_sCity = b;
						break;
					case 7: // Country...
						pAL->m_sCountry = b;
						break;
					case 8: // HQ zip code...
						pAL->m_sHQZipCode = b;
						break;
					case 9: // Continent...
						pAL->m_sContinent = b;
						break;
					case 10: // Telephone...
						pAL->m_sTelephone = b;
						break;
					case 11: // Fax...
						pAL->m_sFax = b;
						break;
					case 12: // E-Mail...
						pAL->m_sEMail = b;
						break;
					case 13: // Director...
						pAL->m_sDirector = b;
						break;
					case 14: // Fleet...
						pAL->m_sFleet = b;
						break;
					default:
						CString tmp = b;
						pAL->m_sLongName = pAL->m_sLongName + "," + tmp;
						break;
					}
				}
				if( !IfExist(pAL->m_sAirlineIATACode))
					m_vAirlines.push_back(pAL);
				else
					delete pAL ;
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
		AfxMessageBox("Error reading Airlines file: " + sFileName, MB_ICONEXCLAMATION | MB_OK);
		e->Delete();
		if(pFile != NULL)
			delete pFile;
		bRet = FALSE;
	}
	return bRet;
}
BOOL CAirlinesManager::LoadData( const CString& _strDBPath )
{
	Clear();
//	ReadCommonAirlineData(m_strCommonDBPath) ;
	m_strDBPath = _strDBPath;
	CFile* pFile = NULL;
	CString sFileName = _strDBPath + "\\" + _strAirlineFileName;
	BOOL bRet = TRUE;
	// read  airline
	try
	{
		pFile = new CFile(sFileName, CFile::modeRead | CFile::shareDenyNone);
		CArchive ar(pFile, CArchive::load);
		char line[513];
		//skip a line
		CString csLine;
		ar.ReadString( csLine );
		//read line 2
		ar.ReadString( csLine );
		csLine.MakeUpper();
		strcpy( line, csLine );
		if(_stricmp(line, "AIRLINES DATABASE") == 0)
		{
			//skip the column names
			ar.ReadString( csLine );
			//read the values
			//read a line
			ar.ReadString( csLine );
			csLine.MakeUpper();
			strcpy( line, csLine );
			while( *line != '\0' )
			{
				CAirline* pAL = new CAirline();
				char* b = line;
				char* p = NULL;
				int c = 1;
				while((p = strchr(b, ',')) != NULL)
				{
					*p = '\0';
					switch(c)
					{
					case 1: // Airline IATA code
						pAL->m_sAirlineIATACode = b;
						break;
					case 2: //Desc
						pAL->m_sLongName = b;
						break;
                    case 3: // ICAO code...
                        pAL->m_sICAOCode = b;
                        break;
                    case 4: // Other codes...
                        pAL->m_sOtherCodes = b;
                        break;
                    case 5: // Street address...
                        pAL->m_sStreetAddress = b;
                        break;
                    case 6: // City...
                        pAL->m_sCity = b;
                        break;
                    case 7: // Country...
                        pAL->m_sCountry = b;
                        break;
                    case 8: // HQ zip code...
                        pAL->m_sHQZipCode = b;
                        break;
                    case 9: // Continent...
                        pAL->m_sContinent = b;
                        break;
                    case 10: // Telephone...
                        pAL->m_sTelephone = b;
                        break;
                    case 11: // Fax...
                        pAL->m_sFax = b;
                        break;
                    case 12: // E-Mail...
                        pAL->m_sEMail = b;
                        break;
                    case 13: // Director...
                        pAL->m_sDirector = b;
                        break;
                    case 14: // Fleet...
                        pAL->m_sFleet = b;
                        break;
					default:
						CString tmp = b;
						pAL->m_sLongName = pAL->m_sLongName + "," + tmp;
						break;
					}
					b = ++p;
					c++;
				}
				if(b!=NULL) // the last column did not have a comma after it
				{
					switch(c)
					{
					case 1: // Airline IATA code
						pAL->m_sAirlineIATACode = b;
						break;
					case 2: //Desc
						pAL->m_sLongName = b;
						break;
                    case 3: // ICAO code...
                        pAL->m_sICAOCode = b;
                        break;
                    case 4: // Other codes...
                        pAL->m_sOtherCodes = b;
                        break;
                    case 5: // Street address...
                        pAL->m_sStreetAddress = b;
                        break;
                    case 6: // City...
                        pAL->m_sCity = b;
                        break;
                    case 7: // Country...
                        pAL->m_sCountry = b;
                        break;
                    case 8: // HQ zip code...
                        pAL->m_sHQZipCode = b;
                        break;
                    case 9: // Continent...
                        pAL->m_sContinent = b;
                        break;
                    case 10: // Telephone...
                        pAL->m_sTelephone = b;
                        break;
                    case 11: // Fax...
                        pAL->m_sFax = b;
                        break;
                    case 12: // E-Mail...
                        pAL->m_sEMail = b;
                        break;
                    case 13: // Director...
                        pAL->m_sDirector = b;
                        break;
                    case 14: // Fleet...
                        pAL->m_sFleet = b;
                        break;
					default:
						CString tmp = b;
						pAL->m_sLongName = pAL->m_sLongName + "," + tmp;
						break;
					}
				}
				if( !IfExist(pAL->m_sAirlineIATACode))
				   m_vAirlines.push_back(pAL);
				else
					delete pAL ;
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
		AfxMessageBox("Error reading Airlines file: " + sFileName, MB_ICONEXCLAMATION | MB_OK);
		e->Delete();
		if(pFile != NULL)
			delete pFile;
		bRet = FALSE;
	}

	// read sub airline
	pFile = NULL;
	sFileName = _strDBPath + "\\" + _strSubAirlineFileName;
	try
	{
		pFile = new CFile(sFileName, CFile::modeRead | CFile::shareDenyNone);
		CArchive ar(pFile, CArchive::load);
		char line[513];
		CString csLine;
		ar.ReadString( csLine );
		//read line 2
		ar.ReadString( csLine );
		csLine.MakeUpper();
		strcpy( line, csLine );
		if(_stricmp(line, "SUBAIRLINES DATABASE") == 0)
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
				//CSubairline* pS = new CSubairline();
				CString sSubairlineName;
				CString sAirline;
				char* b = line;
				char* p = NULL;
				int c = 1;
				while((p = strchr(b, ',')) != NULL)
				{
					*p = '\0';
					switch(c)
					{
					case 1: //name
						sSubairlineName = b;
						break;
					case 2: //airline(s)
						sAirline = b;
						break;
					default: //more airlines
						CString tmp = b;
						sAirline = sAirline + "," + tmp;
						break;
					}
					b = ++p;
					c++;
				}
				if(b!=NULL) // the last column did not have a comma after it
				{
					switch(c)
					{
					case 1: //name
						sSubairlineName = b;
						break;
					case 2: //airline(s)
						sAirline = b;
						break;
					default: //more airlines
						CString tmp = b;
						sAirline = sAirline + "," + tmp;
						break;
					}
				}
				CAIRLINEFILTER alFilter;
				while(TRUE) {
					int nIdx = sAirline.Find(',');
					if(nIdx != -1) { // found a comma
						alFilter.code = sAirline.Left(nIdx);
						AddSubairline(sSubairlineName, alFilter);
						sAirline = sAirline.Right(sAirline.GetLength()-nIdx-1);
					}
					else if(!sAirline.IsEmpty()){ // no comma, but string not empty
						alFilter.code = sAirline;
						AddSubairline(sSubairlineName, alFilter);
						break;
					}
					else { // no comma, string empty
						break;
					}
				}
				ar.ReadString( csLine );
				csLine.MakeUpper();
				strcpy( line, csLine );
			}
		}
		else
			bRet = FALSE;
		ar.Close();
		delete pFile;
	}
	catch(CException* e) {
		AfxMessageBox("Error reading Subairlines file: " + sFileName, MB_ICONEXCLAMATION | MB_OK);
		e->Delete();
		if(pFile != NULL)
			delete pFile;
		bRet = FALSE;
	}
	return bRet;
}

void CAirlinesManager::AddSubairline(CString sSubairlineName, CAIRLINEFILTER airlineFilter)
{
	for(unsigned i=0; i<m_vSubairlines.size(); i++) 
	{
		CSubairline* pSubairline = m_vSubairlines[i];
		if(pSubairline->m_sName.Compare(sSubairlineName) == 0) { //found subairline
			pSubairline->AddFilter(airlineFilter);
			return;
		}
	}
	CSubairline* pSubairline = new CSubairline( this );
	pSubairline->m_sName = sSubairlineName;
	pSubairline->AddFilter(airlineFilter);
	m_vSubairlines.push_back(pSubairline);
}

void CAirlinesManager::GetAirlinesByFilter(CAIRLINEFILTER airlineFilter, CAIRLINELIST& list)
{
	CString sFilter = airlineFilter.code;
	int nIdx = sFilter.Find('*');
	if(nIdx != -1)
	{
		sFilter = sFilter.Left(nIdx);
	    for(unsigned i=0; i<m_vAirlines.size(); i++) {
		    CAirline* pAL = m_vAirlines[i];
		    if(pAL->m_sAirlineIATACode.Left(sFilter.GetLength()).CompareNoCase(sFilter) == 0)
			list.push_back(pAL);
		}
	}
	else
	{
		for(unsigned i=0; i<m_vAirlines.size(); i++)
		{
			CAirline* pAL = m_vAirlines[i];
			if(pAL->m_sAirlineIATACode.CompareNoCase(sFilter) == 0)
			{
				list.push_back(pAL);
			}
		}
	}
}

// return index in the list.
int CAirlinesManager::findSubairlineItem( CSubairline* _pSubairline )
{
	int nCount = m_vSubairlines.size();
	for( int i=0; i<nCount; i++ )
	{
		if( *m_vSubairlines[i] == *_pSubairline )
			return i;
	}
	return INT_MAX;
}


// find the entry and return the index
// if not find add and return the index
// return -1 for invalid.
int CAirlinesManager::FindOrAddEntry( CString _csAirline)
{
	int nCount = m_vAirlines.size();
	for( int i=0; i<nCount; i++ )
	{
		if( m_vAirlines[i]->m_sAirlineIATACode.CompareNoCase(_csAirline) ==  0)
			return i;
	}

	///if( _csAirline.GetLength() < 2 || !isalpha( _csAirline[0] ) )
	if( _csAirline.GetLength() < 2 || m_pAirportDatabase == NULL)// modified by tutu because of the task 330 in pvcs
		return -1;		

	CAirline* pAirline = new CAirline;
	pAirline->m_sAirlineIATACode = _csAirline;
	pAirline->m_sLongName.LoadString( IDS_READ_FROM_FILE );
	m_vAirlines.push_back( pAirline );

	std::vector<CAirline*> _Airlines ;
	_Airlines.push_back(pAirline) ;
	try
	{
		CDatabaseADOConnetion::BeginTransaction(m_pAirportDatabase->GetAirportConnection()) ;
		saveDatabase(m_pAirportDatabase);
		CDatabaseADOConnetion::CommitTransaction(m_pAirportDatabase->GetAirportConnection()) ;
	}
	catch (CADOException& e)
	{
		CDatabaseADOConnetion::RollBackTransation(m_pAirportDatabase->GetAirportConnection()) ;
		e.ErrorMessage();
	}

	return m_vAirlines.size() - 1;	
}

bool CAirlinesManager::IfExist( CString _csAirline )
{
	int nCount = m_vAirlines.size();
	for( int i=0; i<nCount; i++ )
	{
		if(  _csAirline.CompareNoCase(m_vAirlines[i]->m_sAirlineIATACode)  == 0)
			return true;
	}

	return false;
}

CAirline* CAirlinesManager::GetAirlineByName(const CString& sAirline)
{
	int nCount = m_vAirlines.size();
	for( int i=0; i<nCount; i++ )
	{
		if(  sAirline.CompareNoCase(m_vAirlines[i]->m_sAirlineIATACode)  == 0)
			return m_vAirlines[i];
	}
	return NULL;
}
//global function
CAirlinesManager*  _g_GetActiveAirlineMan( CAirportDatabase* _pAirportDB )
{
	assert(_pAirportDB);
	
	return _pAirportDB->getAirlineMan();
}
long CAirlinesManager::GetCode(CString codeName){
	char cstr[AIRLINE_LEN]={0,};
	std::string str(codeName);
	memcpy(cstr,str.c_str(),str.size());
	long *ret = (long*) cstr;
	return  *ret;
}
CString CAirlinesManager::GetCodeStr(long code){
	char str[AIRLINE_LEN] = {0};
	 memcpy(str,&code,sizeof(str));
	// str[4] = 0;
	 return str;
}
////////////new version database read and save//////////////////////////////////////////////
bool CAirlinesManager::loadDatabase(CAirportDatabase* pAirportDatabase)
{
	Clear();

	//read all airline
	CString strSQL(_T(""));
	strSQL.Format(_T("SELECT * FROM TB_AIRPORTDB_AIRLINE"));
	CADORecordset adoAirlineRecordData ;
	long lCount = 0;
	try
	{
		CDatabaseADOConnetion::ExecuteSQLStatement(strSQL,lCount,adoAirlineRecordData,pAirportDatabase->GetAirportConnection());
	}
	catch (CADOException e)
	{
		e.ErrorMessage() ;
		return false;
	}
	while(!adoAirlineRecordData.IsEOF())
	{
		CAirline* pAirline = new CAirline;
		pAirline->loadDatabase(adoAirlineRecordData);
		m_vAirlines.push_back(pAirline);
		adoAirlineRecordData.MoveNextData();
	}

	//read subline data
	strSQL.Format(_T("SELECT * FROM TB_AIRPORTDB_SUBAIRLINE"));
	CADORecordset adoSublineRecordset ;
	try
	{
		CDatabaseADOConnetion::ExecuteSQLStatement(strSQL,lCount,adoSublineRecordset,pAirportDatabase->GetAirportConnection()); 
	}
	catch (CADOException e)
	{
		e.ErrorMessage() ;
		return false;
	}

	while (!adoSublineRecordset.IsEOF())
	{
		CSubairline* pSubline = new CSubairline(this);
		pSubline->loadDatabase(adoSublineRecordset,pAirportDatabase);
		m_vSubairlines.push_back(pSubline);
		adoSublineRecordset.MoveNextData();
	}

	return true;
}

//read common data
bool CAirlinesManager::loadARCDatabase(CAirportDatabase* pAirportDatabase)
{
	//read all airline
	CString strSQL(_T(""));
	strSQL.Format(_T("SELECT * FROM TB_AIRPORTDB_AIRLINE"));
	CADORecordset adoAirlineRecordData ;
	long lCount = 0;
	try
	{
		CDatabaseADOConnetion::ExecuteSQLStatement(strSQL,lCount,adoAirlineRecordData,pAirportDatabase->GetAirportConnection());
	}
	catch (CADOException e)
	{
		e.ErrorMessage() ;
		return false;
	}
	while(!adoAirlineRecordData.IsEOF())
	{
		CAirline* pAirline = new CAirline;
		pAirline->loadDatabase(adoAirlineRecordData);
		m_vAirlines.push_back(pAirline);
		adoAirlineRecordData.MoveNextData();
	}
	return true;
}

bool CAirlinesManager::saveDatabase(CAirportDatabase* pAirportDatabase)
{
	CAIRLINELIST::iterator airlineIter = m_vAirlines.begin();
	for (; airlineIter != m_vAirlines.end(); ++airlineIter)
	{
		if (!(*airlineIter)->IsEdit())
		{
			(*airlineIter)->saveDatabase(pAirportDatabase);
		}
	}
	
	airlineIter = m_DelAirlines.begin();
	for (; airlineIter != m_DelAirlines.end(); ++airlineIter)
	{
		(*airlineIter)->deleteDatabase(pAirportDatabase);
	}

	ClearDelAirlines();

	CSUBAIRLINELIST::iterator sublineIter = m_vSubairlines.begin();
	for (; sublineIter != m_vSubairlines.end(); ++sublineIter)
	{
		(*sublineIter)->saveDatabase(pAirportDatabase);
	}

	sublineIter = m_DelSubAirline.begin();
	for (; sublineIter != m_DelSubAirline.end(); ++sublineIter)
	{
		(*sublineIter)->deleteDatabase(pAirportDatabase);
	}

	ClearDelSubAirlines();
	return true;
}

bool CAirlinesManager::deleteDatabase(CAirportDatabase* pAirportDatabase)
{
	CAirline::deleteAllDatabase(pAirportDatabase);

	CSubairline::deleteAllDatabase(pAirportDatabase);
	CSUBAIRLINELIST::iterator iter = m_vSubairlines.begin();
	for (; iter != m_vSubairlines.end(); ++iter)
	{
		(*iter)->deleteAirlineFilter(pAirportDatabase);
	}
	return true;
}
/////old code, no more use//////////////////////////////////////////////////////////////////
void CAirlinesManager::ReadDataFromDB(int _airportID,DATABASESOURCE_TYPE type )
{
	Clear() ;

	CAirline::ReadDataFromDB(m_vAirlines,_airportID,type) ;
	CSubairline::ReadDataFromDB(m_vSubairlines,this,_airportID,type) ;
	if((int)m_vAirlines.size() < 1120)
		ReadCommonAirlineData(m_strCommonDBPath) ;
}
void CAirlinesManager::WriteDataToDB(int _airportID,DATABASESOURCE_TYPE type)
{

		
		CAirline::WriteDataToDB(m_vAirlines,_airportID,type) ;
		CAirline::DeleteDataFromDB(m_DelAirlines,type) ;

		CSubairline::WriteDataToDB(m_vSubairlines,_airportID,type) ;
		CSubairline::DeleteDataFromDB(m_DelSubAirline,type) ;

	ClearDelAirlines() ;
	ClearDelSubAirlines() ;

	
}
void CAirlinesManager::RemoveAirline(CAirline* _airline)
{
	if(_airline == NULL)
		return ;
	CAIRLINELIST::iterator iter = m_vAirlines.begin() ;
	for ( ; iter != m_vAirlines.end() ;iter++)
	{
		if (*iter == _airline)
		{
			m_DelAirlines.push_back(*iter) ;
			m_vAirlines.erase(iter) ;
			break ;
		}
	}
}
void CAirlinesManager::RemoveSubAirline(CSubairline* _subairline)
{
	if(_subairline == NULL)
		return ;
	CSUBAIRLINELIST::iterator iter =  m_vSubairlines.begin() ;
	for ( ; iter != m_vSubairlines.end() ; iter++)
	{
		if(*iter == _subairline)
		{
			m_DelSubAirline.push_back(*iter) ;
			m_vSubairlines.erase(iter) ;
			break ;
		}
	}
}
void CAirlinesManager::ClearDelAirlines()
{
   CAIRLINELIST::iterator iter = m_DelAirlines.begin() ; 
   for ( ; iter!= m_DelAirlines.end() ;iter++)
   {
	   delete *iter ;
   }
   m_DelAirlines.clear() ;
}
void CAirlinesManager::ClearDelSubAirlines()
{
	CSUBAIRLINELIST::iterator iter = m_DelSubAirline.begin() ; 
	for ( ; iter!= m_DelSubAirline.end() ;iter++)
	{
		delete *iter ;
	}
	m_DelSubAirline.clear() ;
}
void CAirlinesManager::ReSetAllID()
{
	for(unsigned i=0; i<m_vAirlines.size(); i++)
		 m_vAirlines[i]->SetID(-1);
	

	for(unsigned j=0; j<m_vSubairlines.size(); j++)
		 m_vSubairlines[j]->SetID(-1);

}
void CAirlinesManager::DeleteAllDataFromDB(int _airportID,DATABASESOURCE_TYPE type /* = DATABASESOURCE_TYPE_ACCESS */ )
{
	CAirline::DeleteAllData(_airportID,type) ;
	CSubairline::DeleteAllData(_airportID,type) ;
}