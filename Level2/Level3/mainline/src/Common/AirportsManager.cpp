// AirportsManager.cpp: implementation of the CAirportsManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "..\main\resource.h"
#include "AirportsManager.h"
#include "ProjectManager.h"
#include "common\airportdatabase.h"
#include "assert.h"
#include "latlong.h"
#include "../Database/ARCportADODatabase.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// define
static const CString _strAirportFileName	= "Airports.dat";
static const CString _strSectorFileName		= "Sector.sec";

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAirportsManager::CAirportsManager(CAirportDatabase* pAirportDatabase):m_AirportID(-1)
,m_pAirportDatabase(pAirportDatabase)
{

}

CAirportsManager::~CAirportsManager()
{
	for(unsigned i=0; i<m_vAirports.size(); i++)
		delete m_vAirports[i];
	m_vAirports.clear();

	for(unsigned j=0; j<m_vSectors.size(); j++)
		delete m_vSectors[j];
	m_vSectors.clear();

	ClearDelAirport() ;
	ClearDelSectors() ;
}
void CAirportsManager::Clear()
{
	for(unsigned i=0; i<m_vAirports.size(); i++)
		delete m_vAirports[i];
	m_vAirports.clear();

	for(unsigned j=0; j<m_vSectors.size(); j++)
		delete m_vSectors[j];
	m_vSectors.clear();

	ClearDelAirport() ;
	ClearDelSectors() ;
}
BOOL CAirportsManager::LoadDataFromOtherFile(const CString& _strDBPath)
{
      CString currentPath ;
	  currentPath = m_strDBPath ;
	  m_strDBPath = _strDBPath ;
	  BOOL res = LoadData(_strDBPath) ;
	  m_strDBPath = currentPath ;
	  return res;

}
BOOL CAirportsManager::ReadCommonAirports(const CString& _path)
{
	CFile* pFile = NULL;
	CString sFileName = _path + "\\" + _strAirportFileName;
	BOOL bRet = TRUE;
	// airport file
	try
	{
		pFile = new CFile(sFileName, CFile::modeRead | CFile::shareDenyNone);
		CArchive ar(pFile, CArchive::load);
		char line[1024];
		//skip a line
		CString csLine;
		ar.ReadString( csLine );
		//read line 2
		ar.ReadString( csLine );
		csLine.MakeUpper();
		strcpy( line, csLine );
		if(_stricmp(line, "AIRPORTS DATABASE") == 0)
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
				CAirport* pAP = new CAirport();
				char* b = line;
				char* p = NULL;
				int c = 1;
				while((p = strchr(b, ',')) != NULL)
				{
					*p = '\0';
					switch(c)
					{
					case 1: //IATA code
						pAP->m_sIATACode = b;
						break;
					case 2: //Desc
						pAP->m_sLongName = b;
						break;
					case 3: // ICAO code...
						pAP->m_sICAOCode = b;
						break;
					case 4: // ARP coordinates...
						pAP->m_sARPCoordinates = b;
						break;
					case 5: // City...
						pAP->m_sCity = b;
						break;
					case 6: // Country...
						pAP->m_sCountry = b;
						break;
					case 7: // Country code...
						pAP->m_sCountryCode = b;
						break;
					case 8: // Elevation...
						pAP->m_sElevation = b;
						break;
					case 9: // Number of runways...
						pAP->m_sNumberOfRunways = b;
						break;
					case 10: // Maximum runway length...
						pAP->m_sMaximumRunwayLength = b;
						break;
					case 11: // Alternate airport ICAO code...
						pAP->m_sAlternateAirportICAOCode = b;
						break;
					default:
						CString tmp = b;
						break;
					}
					b = ++p;
					c++;
				}
				if(b!= NULL)
				{
					pAP->m_sAlternateAirportICAOCode = b;
				}
				if(!IsExist(pAP))
					m_vAirports.push_back(pAP);
				else
					delete pAP ;
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
		AfxMessageBox("Error reading Airports file: " + sFileName, MB_ICONEXCLAMATION | MB_OK);
		e->Delete();
		if(pFile != NULL)
			delete pFile;
		bRet = FALSE;
	}
	return bRet ;
}
BOOL CAirportsManager::LoadData( const CString& _strDBPath )
{
	Clear();
	m_strDBPath = _strDBPath;
//	ReadCommonAirports(m_strCommonDBPath) ;
	CFile* pFile = NULL;
	CString sFileName = _strDBPath + "\\" + _strAirportFileName;
	BOOL bRet = TRUE;
	// airport file
	try
	{
		pFile = new CFile(sFileName, CFile::modeRead | CFile::shareDenyNone);
		CArchive ar(pFile, CArchive::load);
		char line[1024];
		//skip a line
		CString csLine;
		ar.ReadString( csLine );
		//read line 2
		ar.ReadString( csLine );
		csLine.MakeUpper();
		strcpy( line, csLine );
		if(_stricmp(line, "AIRPORTS DATABASE") == 0)
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
				CAirport* pAP = new CAirport();
				char* b = line;
				char* p = NULL;
				int c = 1;
				while((p = strchr(b, ',')) != NULL)
				{
					*p = '\0';
					switch(c)
					{
					case 1: //IATA code
						pAP->m_sIATACode = b;
						break;
					case 2: //Desc
						pAP->m_sLongName = b;
						break;
                    case 3: // ICAO code...
                        pAP->m_sICAOCode = b;
                        break;
                    case 4: // ARP coordinates...
                        pAP->m_sARPCoordinates = b;
                        break;
                    case 5: // City...
                        pAP->m_sCity = b;
                        break;
                    case 6: // Country...
                        pAP->m_sCountry = b;
                        break;
                    case 7: // Country code...
                        pAP->m_sCountryCode = b;
                        break;
                    case 8: // Elevation...
                        pAP->m_sElevation = b;
                        break;
                    case 9: // Number of runways...
                        pAP->m_sNumberOfRunways = b;
                        break;
                    case 10: // Maximum runway length...
                        pAP->m_sMaximumRunwayLength = b;
                        break;
                    case 11: // Alternate airport ICAO code...
                        pAP->m_sAlternateAirportICAOCode = b;
                        break;
					default:
						CString tmp = b;
						break;
					}
					b = ++p;
					c++;
				}
				if(b!= NULL)
				{
                    pAP->m_sAlternateAirportICAOCode = b;
				}
				if(!IsExist(pAP))
				  m_vAirports.push_back(pAP);
				else
					delete pAP ;
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
		AfxMessageBox("Error reading Airports file: " + sFileName, MB_ICONEXCLAMATION | MB_OK);
		e->Delete();
		if(pFile != NULL)
			delete pFile;
		bRet = FALSE;
	}

	// sector file
	pFile = NULL;
	sFileName = _strDBPath + "\\" + _strSectorFileName;
	try
	{
		pFile = new CFile(sFileName, CFile::modeRead | CFile::shareDenyNone);
		CArchive ar(pFile, CArchive::load);
		char line[2048];
		CString csLine;
		ar.ReadString( csLine );
		//read line 2
		ar.ReadString( csLine );
		csLine.MakeUpper();
		strcpy( line, csLine );
		if(_stricmp(line, "SECTORS DATABASE") == 0)
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
				//CSector* pS = new CSector();
				CString sSectorName;
				CString sAirport;
				char* b = line;
				char* p = NULL;
				int c = 1;
				while((p = strchr(b, ',')) != NULL)
				{
					*p = '\0';
					switch(c)
					{
					case 1: //name
						sSectorName = b;
						break;
					case 2: //airport(s)
						sAirport = b;
						break;
					default: //more airports
						CString tmp = b;
						sAirport = sAirport + "," + tmp;
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
						sSectorName = b;
						break;
					case 2: //airport(s)
						sAirport = b;
						break;
					default: //more airports
						CString tmp = b;
						sAirport = sAirport + "," + tmp;
						break;
					}
				}
				CAIRPORTFILTER apFilter;
				while(TRUE) {
					int nIdx = sAirport.Find(',');
					if(nIdx != -1) { // found a comma
						apFilter.sIATACode = sAirport.Left(nIdx);
						AddSector(sSectorName, apFilter);
						sAirport = sAirport.Right(sAirport.GetLength()-nIdx-1);
					}
					else if(!sAirport.IsEmpty()){ // no comma, but string not empty
						apFilter.sIATACode = sAirport;
						AddSector(sSectorName, apFilter);
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
		AfxMessageBox("Error reading Sectors file: " + sFileName, MB_ICONEXCLAMATION | MB_OK);
		e->Delete();
		if(pFile != NULL)
			delete pFile;
		bRet = FALSE;
	}
	return bRet;
}

void CAirportsManager::AddSector(CString sSectorName, CAIRPORTFILTER airportFilter)
{
	for(unsigned i=0; i<m_vSectors.size(); i++) {
		CSector* pSector = m_vSectors[i];
		if(pSector->m_sName.Compare(sSectorName) == 0) { //found sector
			pSector->AddFilter(airportFilter);
			return;
		}
	}
	CSector* pSector = new CSector( this );
	pSector->m_sName = sSectorName;
	pSector->AddFilter(airportFilter);
	m_vSectors.push_back(pSector);
}

void CAirportsManager::GetAirportsByFilter(CAIRPORTFILTER airportFilter, CAIRPORTLIST& list)
{
	CString sFilter = airportFilter.sIATACode;
	int nIdx = sFilter.Find('*');
	if(nIdx != -1)
	{
		sFilter = sFilter.Left(nIdx);
		for(unsigned i=0; i<m_vAirports.size(); i++) 
		{
			CAirport* pAP = m_vAirports[i];
			if(pAP->m_sIATACode.Left(sFilter.GetLength()).CompareNoCase(sFilter) == 0)
				list.push_back(pAP);
		}

	}
	else
	{
		for(unsigned i=0; i<m_vAirports.size(); i++) 
		{
			CAirport* pAP = m_vAirports[i];
			if(pAP->m_sIATACode.CompareNoCase(sFilter) == 0)
				list.push_back(pAP);
		}
	}


}


// return index in the list.
int CAirportsManager::findSectItem( CSector* _pSector )
{
	int nCount = m_vSectors.size();
	for( int i=0; i<nCount; i++ )
	{
		if( *m_vSectors[i] == *_pSector )
			return i;
	}
	return INT_MAX;
}

CAirport* CAirportsManager::GetAiportByName(const CString& sAirport)
{
	int nCount = m_vAirports.size();
	for( int i=0; i<nCount; i++ )
	{
		if( m_vAirports[i]->m_sIATACode == sAirport )
			return m_vAirports[i];
	}
	return NULL;
}

// find the entry and return the index
// if not find add and return the index
// return -1 for invalid.
int CAirportsManager::FindOrAddEntry( CString _csAirport)
{
	int nCount = m_vAirports.size();
	for( int i=0; i<nCount; i++ )
	{
		if( m_vAirports[i]->m_sIATACode == _csAirport )
			return i;
	}

	if( _csAirport.GetLength() < 1 || !IsStringAllAlpha( _csAirport ) || m_pAirportDatabase == NULL)
		return -1;		

	CAirport* pAirport = new CAirport;
	pAirport->m_sIATACode = _csAirport;
	BOOL bRET = pAirport->m_sLongName.LoadString( IDS_READ_FROM_FILE );
	ASSERT(bRET);
	m_vAirports.push_back( pAirport );

	try
	{
		CDatabaseADOConnetion::BeginTransaction(m_pAirportDatabase->GetAirportConnection()) ;
		pAirport->saveDatabase(m_pAirportDatabase);
		CDatabaseADOConnetion::CommitTransaction(m_pAirportDatabase->GetAirportConnection()) ;
	}
	catch (CADOException& e)
	{
		CDatabaseADOConnetion::RollBackTransation(m_pAirportDatabase->GetAirportConnection()) ;
		e.ErrorMessage() ;
	}
	return m_vAirports.size() - 1;	
}

void CAirportsManager::GetAirportCoordinates(const CString strAirport, CLatitude& tlat, CLongitude& tlong )
{
	int iAirportIndex = FindOrAddEntry(strAirport);

	CString strCorrd;
	if(iAirportIndex >= 0 )
	{
		CAirport * pAirport =  GetAirportList()->at(iAirportIndex);
		ASSERT(pAirport);
		if(pAirport)
			strCorrd = pAirport->m_sARPCoordinates;
	}
	//get lat long form str;
	if( strCorrd.CompareNoCase("") !=0 )
	{
		CString strLongitude;
		CString strLatitude;
		int nIndex = strCorrd.Find("-");
		strLongitude = strCorrd.Left(nIndex);
		tlong.SetValue(strLongitude);
		strLatitude  = strCorrd.Right(strCorrd.GetLength() - nIndex-1);
		tlat.SetValue(strLatitude);
	}

}

bool CAirportsManager::IsStringAllAlpha(const CString &_str)
{
	int nStrLen = _str.GetLength();
	for (int i = 0; i < nStrLen; i++)
	{
		if( !isalpha(_str[i]))
		{
			return false;
		}
	}
	return true;
}
//////new version database read and save
bool CAirportsManager::loadDatabase(CAirportDatabase* pAirportDatabase)
{
	ASSERT(pAirportDatabase);
	if(!pAirportDatabase)
		return false;

	Clear();

	CString strSQL(_T(""));
	CADORecordset adoAirportRecordset;
	long lCount = 0 ;
	strSQL.Format(_T("SELECT * FROM TB_AIRPORTDB_AIRPORT"));
	try
	{
		CDatabaseADOConnetion::ExecuteSQLStatement(strSQL,lCount,adoAirportRecordset,pAirportDatabase->GetAirportConnection()) ;
	}
	catch (CADOException& e)
	{
		e.ErrorMessage();
		return false;
	}

	while(!adoAirportRecordset.IsEOF())
	{
		 CAirport* pAirport = new CAirport;
		 pAirport->loadDatabase(adoAirportRecordset);
		 m_vAirports.push_back(pAirport);
		 adoAirportRecordset.MoveNextData();
	}

	//read all sector
	CADORecordset adoSectorRecordset;
	strSQL.Format(_T("SELECT * FROM TB_AIRPORTDB_SECTOR"));
	try
	{
		CDatabaseADOConnetion::ExecuteSQLStatement(strSQL,lCount,adoSectorRecordset,pAirportDatabase->GetAirportConnection());
	}
	catch (CADOException& e)
	{
		e.ErrorMessage();
		return false;
	}
	
	while(!adoSectorRecordset.IsEOF())
	{
		CSector* pSector = new CSector(this);
		pSector->loadDatabase(adoSectorRecordset,pAirportDatabase);
		m_vSectors.push_back(pSector);
		adoSectorRecordset.MoveNextData();
	}

	return true;
}

//read common data
bool CAirportsManager::loadARCDatabase(CAirportDatabase* pAirportDatabase)
{
	CString strSQL(_T(""));
	CADORecordset adoAirportRecordset;
	long lCount = 0 ;
	strSQL.Format(_T("SELECT * FROM TB_AIRPORTDB_AIRPORT"));
	try
	{
		CDatabaseADOConnetion::ExecuteSQLStatement(strSQL,lCount,adoAirportRecordset,pAirportDatabase->GetAirportConnection()) ;
	}
	catch (CADOException& e)
	{
		e.ErrorMessage();
		return false;
	}

	while(!adoAirportRecordset.IsEOF())
	{
		CAirport* pAirport = new CAirport;
		pAirport->loadDatabase(adoAirportRecordset);
		m_vAirports.push_back(pAirport);
		adoAirportRecordset.MoveNextData();
	}
	return true;
}

bool CAirportsManager::saveDatabase(CAirportDatabase* pAirportDatabase)
{
	//save all airport
	CAIRPORTLIST::iterator airportIter = m_vAirports.begin();
	for (; airportIter != m_vAirports.end(); airportIter ++)
	{
		if (!(*airportIter)->IsEdit())
		{
			(*airportIter)->saveDatabase(pAirportDatabase);
		}
	}

	airportIter = m_DelAirport.begin();
	for (; airportIter != m_DelAirport.end(); ++airportIter)
	{
		(*airportIter)->deleteDatabase(pAirportDatabase);
	}
	//save all sector
	CSECTORLIST::iterator sectorIter = m_vSectors.begin();
	for (; sectorIter != m_vSectors.end(); ++sectorIter)
	{
		(*sectorIter)->saveDatabase(pAirportDatabase);
	}

	sectorIter = m_DelSectors.begin();
	for (; sectorIter != m_DelSectors.end(); ++sectorIter)
	{
		(*sectorIter)->deleteDatabase(pAirportDatabase);
	}

	ClearDelAirport();
	ClearDelSectors();
	return true;
}

bool CAirportsManager::deleteDatabase(CAirportDatabase* pAirportDatabase)
{
	//delete airport from database
	CAirport::deleteAllDatabase(pAirportDatabase);

	//delete sector from database
	CSector::deleteAllDatabase(pAirportDatabase);
	CSECTORLIST::iterator iter = m_vSectors.begin();
	for (; iter != m_vSectors.end(); ++iter)
	{
		(*iter)->deleteSectorFilter(pAirportDatabase);
	}
	return true;
}
//////////////////////////////old database read and save, no more use/////////////////
//global function
CAirportsManager* _g_GetActiveAirportMan( CAirportDatabase* _pAirportDB )
{
	assert( _pAirportDB );
	if(!_pAirportDB)
		return NULL;
	return _pAirportDB->getAirportMan();
}
void CAirportsManager::ReadDataFromDB(int _airportDBID,DATABASESOURCE_TYPE type )
{
	Clear() ;
		
	CAirport::ReadDataFromDB(m_vAirports,_airportDBID,type) ;
	CSector::ReadDataFromDB(m_vSectors,this,_airportDBID,type) ;

	if((int)m_vAirports.size() < 1120) 
		ReadCommonAirports(m_strCommonDBPath) ;
}
void CAirportsManager::WriteDataToDB(int _airportDBID,DATABASESOURCE_TYPE type)
{

		CAirport::WriteDataToDB(m_vAirports,_airportDBID,type) ;
		CAirport::DeleteDataFromDB(m_DelAirport,type) ;


		CSector::WriteDataToDB(m_vSectors,_airportDBID,type) ;
		CSector::DeleteDataFromDB(m_DelSectors,type);

	ClearDelAirport() ;
	ClearDelSectors() ;
}
void CAirportsManager::RemoveSector(CSector* _sector)
{
	CSECTORLIST::iterator iter = m_vSectors.begin() ;
	for ( ; iter != m_vSectors.end() ;iter++)
	{
		if(*iter == _sector)
		{
			m_DelSectors.push_back(*iter) ;
			m_vSectors.erase(iter) ;
			break ;
		}
	}
}
void CAirportsManager::RemveAirport(CAirport* _airport)
{
	CAIRPORTLIST::iterator iter = m_vAirports.begin() ;
	for ( ; iter != m_vAirports.end() ;iter++)
	{
		if(*iter == _airport)
		{
			m_DelAirport.push_back(*iter) ;
			m_vAirports.erase(iter) ;
			break ;
		}
	}
}
void CAirportsManager::ClearDelAirport()
{
	CAIRPORTLIST::iterator iter = m_DelAirport.begin() ;
	for ( ; iter != m_DelAirport.end() ;iter++)
	{
		delete *iter ;
	}
	m_DelAirport.clear() ;
}
void CAirportsManager::ClearDelSectors()
{
	CSECTORLIST::iterator iter = m_DelSectors.begin() ;
	for ( ; iter != m_DelSectors.end() ;iter++)
	{
		delete *iter ;
	}
	m_DelSectors.clear() ;
}
void CAirportsManager::ReSetAllID()
{
	unsigned i=0;
	for( i=0; i<m_vAirports.size(); i++)
		 m_vAirports[i]->SetID(-1);
	

	for(i=0; i<m_vSectors.size(); i++)
		 m_vSectors[i]->SetID(-1);
}
void CAirportsManager::DeleteAllDataFromDB(int _airportDBID,DATABASESOURCE_TYPE type /* = DATABASESOURCE_TYPE_ACCESS */)
{
	CAirport::DeleteAllDataFromDB(_airportDBID,type) ;
	CSector::DeleteAllDataFromDB(_airportDBID,type) ;
}
BOOL CAirportsManager::IsExist(CAirport* _airport)
{
	if(_airport == NULL)
		return FALSE ;
	for (int i = 0 ; i < (int)m_vAirports.size() ;i++)
	{
		if(strcmp(m_vAirports[i]->m_sIATACode ,_airport->m_sIATACode) == 0)
			return TRUE ;
	}
	return FALSE ;
}