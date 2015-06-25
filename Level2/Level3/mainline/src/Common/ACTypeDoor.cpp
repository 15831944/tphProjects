#include "StdAfx.h"
#include ".\actypedoor.h"
#include "AirportDatabase.h"
#include "Aircraft.h"
#include "ACTypesManager.h"
#include "../Database/ARCportADODatabase.h"
ACTypeDoor::ACTypeDoor(void)
{
	 m_dNoseDist = 0 ;
	 m_dSillHeight = 0;
	 m_dHeight = 0;
	 m_dWidth = 0;
	 m_enumDoorDir = LeftHand;
	  m_nID = -1;
	  m_nACID = -1;
	  m_strACName = _T("");
}

ACTypeDoor::~ACTypeDoor(void)
{
}

int ACTypeDoor::GetID()
{
	return m_nID;
}

int ACTypeDoor::GetACTypeID()
{
	return m_nACID;
}
////////new version database read and save/////////////////////////////////////////////////
bool ACTypeDoor::loadDatabase(CADORecordset& aodRecordset)
{
	if (!aodRecordset.IsEOF())
	{
		int nValue = 0;
		aodRecordset.GetFieldValue(_T("ID"),m_nID);
		aodRecordset.GetFieldValue(_T("ACNAME"),m_strACName);
		aodRecordset.GetFieldValue(_T("DOORNAME"),m_strName);
		aodRecordset.GetFieldValue(_T("TONOSEDIST"),m_dNoseDist);
		aodRecordset.GetFieldValue(_T("SILLHEIGHT"),m_dSillHeight);
		CString sDoorDir(_T(""));
		aodRecordset.GetFieldValue(_T("DOORDIR"),sDoorDir);
		m_enumDoorDir = GetDoorDirByString(sDoorDir);
		aodRecordset.GetFieldValue(_T("WIDTH"),m_dWidth);
		aodRecordset.GetFieldValue(_T("HEIGHT"),m_dHeight);
	}
	return true;
}

bool ACTypeDoor::saveDatabase(CAirportDatabase* pAirportDatabase)
{
	ASSERT(pAirportDatabase);

	if(m_nID != -1)
		updateDatabase(pAirportDatabase);
	else
	{
		CString strSQL(_T(""));
		strSQL.Format(_T("INSERT INTO TB_AIRPORTDB_ACTYPEDOOR (ACNAME,DOORNAME,TONOSEDIST,SILLHEIGHT,DOORDIR,WIDTH,HEIGHT )		\
					  VALUES('%s','%s',%f,%f,'%s',%f,%f)"),
					  m_strACName,
					  m_strName,
					  m_dNoseDist,
					  m_dSillHeight,
					  GetStringByHand(m_enumDoorDir),
					  m_dWidth,
					  m_dHeight);
		m_nID = CDatabaseADOConnetion::ExecuteSQLStatementAndReturnScopeID(strSQL,pAirportDatabase->GetAirportConnection());
	}
	return true;
}

bool ACTypeDoor::deleteDatabase(CAirportDatabase* pAirportDatabase)
{
	CString strSQL(_T(""));
	strSQL.Format(_T("DELETE * FROM TB_AIRPORTDB_ACTYPEDOOR WHERE ID = %d"),m_nID);
	CDatabaseADOConnetion::ExecuteSQLStatement(strSQL,pAirportDatabase->GetAirportConnection());
	return true;
}

bool ACTypeDoor::updateDatabase(CAirportDatabase* pAirportDatabase)
{
	ASSERT(pAirportDatabase);

	CString strSQL(_T(""));
	strSQL.Format(_T("UPDATE TB_AIRPORTDB_ACTYPEDOOR SET ACNAME = '%s' ,DOORNAME = '%s',TONOSEDIST = %f,SILLHEIGHT = %f ,DOORDIR = '%s' ,WIDTH = %f ,HEIGHT = %f WHERE \
				  ID = %d"),
				  m_strACName,
				  m_strName,
				  m_dNoseDist,
				  m_dSillHeight,
				  GetStringByHand(m_enumDoorDir),
				  m_dWidth,
				  m_dHeight,
				  m_nID) ;

	CDatabaseADOConnetion::ExecuteSQLStatement(strSQL,pAirportDatabase->GetAirportConnection());
	return true;
}
/////////old code version,no more use///////////////////////////////////////////////////////
void ACTypeDoor::ReadDataFromDB( std::vector<ACTypeDoor*>& _Doors , int _airportID , DATABASESOURCE_TYPE type )
{
	if(_airportID == -1 )
		return ;

	CString SQL ;
	SQL.Format(_T("SELECT * FROM TB_AIRPORTDB_ACTYPEDOOR WHERE AIRPORTDB = %d " ),_airportID) ;
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
	ACTypeDoor* pAcDoor = NULL ;
	int nvalue ; 
	while (!dataset.IsEOF())
	{
		pAcDoor = new ACTypeDoor ;
		dataset.GetFieldValue(_T("ID"),id) ;
		pAcDoor->SetID(id) ;

		dataset.GetFieldValue(_T("ACID"),id) ;
		pAcDoor->SetACID(id);

		dataset.GetFieldValue(_T("DOORNAME"),strval) ;
		pAcDoor->m_strName = strval ;

		dataset.GetFieldValue(_T("TONOSEDIST"),double_val) ;
		pAcDoor->m_dNoseDist = double_val ;

		dataset.GetFieldValue(_T("SILLHEIGHT"),double_val) ;
		pAcDoor->m_dSillHeight = double_val ;

		dataset.GetFieldValue(_T("DOORDIR"),nvalue) ;
		pAcDoor->m_enumDoorDir = (ACTypeDoor::DoorDir)nvalue ;

		dataset.GetFieldValue(_T("WIDTH"),double_val) ;
		pAcDoor->m_dWidth = (float)double_val ;

		dataset.GetFieldValue(_T("HEIGHT"),double_val) ;
		pAcDoor->m_dHeight = (float)double_val ;

		_Doors.push_back(pAcDoor) ;

		dataset.MoveNextData() ;
	}
}

void ACTypeDoor::WriteDataToDB( std::vector<ACTypeDoor*>& _Doors , int _airportID ,  DATABASESOURCE_TYPE type )
{
	if(_airportID == -1)
		return ;

	std::vector<ACTypeDoor*>::iterator iter = _Doors.begin() ;

	for ( ; iter != _Doors.end() ; iter++)
	{
		(*iter)->m_strName.Replace("\'" ,"\'\'") ;

		ACTypeDoor::WriteACDoorToDB(*iter,_airportID,type) ;
	}
}

void ACTypeDoor::DeleteDataFromDB( std::vector<ACTypeDoor*>& _Doors , int _AcTypeID, DATABASESOURCE_TYPE type )
{
	std::vector<ACTypeDoor*>::iterator iter = _Doors.begin() ;
	for ( ;iter != _Doors.end() ;iter++)
	{
		ACTypeDoor::DeleteACDoorFromDB(*iter,type) ;
	}
}

void ACTypeDoor::DeleteAllDataFromDB( int _AcTypeID,DATABASESOURCE_TYPE type )
{
	CString SQL ;
	SQL.Format(_T("DELETE * FROM TB_AIRPORTDB_ACTYPEDOOR WHERE ACID = %d") ,_AcTypeID) ;
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

void ACTypeDoor::DeleteACDoorFromDB( ACTypeDoor* _door,DATABASESOURCE_TYPE type )
{
	if(_door == NULL || _door->GetID() == -1)
		return ;
	CString SQL ;
	SQL.Format(_T("DELETE * FROM TB_AIRPORTDB_ACTYPEDOOR WHERE ID = %d") ,_door->GetID()) ;
	CADODatabase::ExecuteSQLStatement(SQL,type) ;
}

void ACTypeDoor::UpDateACDoorToDB( ACTypeDoor* _door ,int _airportID ,DATABASESOURCE_TYPE type )
{
	if(_door == NULL || _airportID == -1||_door->GetACTypeID()== -1)
		return ;
	if(_door->GetID() == -1)
		WriteACDoorToDB(_door,_airportID,type);
	else
	{
		CString SQL ;
		SQL.Format(_T("UPDATE TB_AIRPORTDB_ACTYPEDOOR SET AIRPORTDB = %d ,ACID = %d ,DOORNAME = '%s',TONOSEDIST = %f,SILLHEIGHT = %f ,DOORDIR = %d ,WIDTH = %f ,HEIGHT = %f WHERE \
					  ID = %d"),
					  _airportID,
						_door->GetACTypeID(),
					  _door->m_strName,
					  _door->m_dNoseDist,
					  _door->m_dSillHeight,
					  (int)_door->m_enumDoorDir,
					  _door->m_dWidth,
					  _door->m_dHeight,
					  _door->GetID()) ;

		CADODatabase::ExecuteSQLStatement(SQL,type) ;
	}
}

void ACTypeDoor::WriteACDoorToDB( ACTypeDoor* _door ,int airportID ,DATABASESOURCE_TYPE type )
{
	if(_door == NULL || airportID == -1||_door->GetACTypeID() == -1)
		return ;
	if(_door->GetID() != -1)
		ACTypeDoor::UpDateACDoorToDB(_door,airportID, type) ;
	else
	{
		int id = -1 ;
		CString SQL ;
		SQL.Format(_T("INSERT INTO TB_AIRPORTDB_ACTYPEDOOR (AIRPORTDB ,ACID,DOORNAME,TONOSEDIST,SILLHEIGHT,DOORDIR,WIDTH,HEIGHT )		\
					  VALUES(%d,%d,'%s',%f,%f,%d,%f,%f)"),
					  airportID,
			_door->GetACTypeID(),
			_door->m_strName,
			_door->m_dNoseDist,
			_door->m_dSillHeight,
			(int)_door->m_enumDoorDir,
			_door->m_dWidth,
			_door->m_dHeight) ;
		id = CADODatabase::ExecuteSQLStatementAndReturnScopeID(SQL,type) ;
		_door->SetID(id) ;

	}
}

void ACTypeDoor::SetID( int nID )
{
	m_nID = nID;
}

void ACTypeDoor::SetACID( int nID )
{
	m_nACID = nID;
}

CString ACTypeDoor::GetStringByHand( DoorDir _dir )
{
	CString str ;
	switch(_dir)
	{
	case LeftHand:
		str = _T("Left") ;
		break ;
	case RightHand:
		str = _T("Right") ;
		break ;
	case BothSide:
		str = _T("Both") ;
		break ;
	default:
		str =_T("Unknown") ;
	}
	return str ;
}

ACTypeDoor::DoorDir ACTypeDoor::GetDoorDirByString(const CString& sDoorDir)
{
	if (!sDoorDir.CompareNoCase(_T("Left")))
	{
		return LeftHand;
	}
	else if(!sDoorDir.CompareNoCase(_T("Right")))
	{
		return RightHand;
	}
	else if(!sDoorDir.CompareNoCase(_T("Both")))
	{
		return BothSide;
	}
	return LeftHand;
}

void ACTypeDoor::operator=( const ACTypeDoor& p_actype )
{
	 m_strACName = p_actype.m_strACName;
	 m_strName= p_actype.m_strName;
	 m_dNoseDist= p_actype.m_dNoseDist;
	 m_dSillHeight= p_actype.m_dSillHeight;
	 m_dHeight= p_actype.m_dHeight;
	 m_dWidth= p_actype.m_dWidth;
	 m_enumDoorDir= p_actype.m_enumDoorDir;
	 m_nID = p_actype.m_nID ;
	 m_nACID = p_actype.m_nACID ;
	 m_strName = p_actype.m_strName;
}
static const CString _strACDoorFileName			= "acdoordata.acd";

ACTYPEDOORLIST* ACTypeDoorManager::GetAcTypeDoorList( const CString& sACCode )
{
	CACType* acType = m_pAcMan->getACTypeItem(sACCode);

	if (acType)
		return &acType->m_vACDoorList;

	return NULL;
}

ACTypeDoorManager * ACTypeDoorManager::_g_GetActiveACDoorMan( CAirportDatabase * pAirportDB )
{
	//assert(pAirportDB);
	return pAirportDB->getAcDoorMan();
}

//////////////new version database read and save////////////////////////////////////////////
bool ACTypeDoorManager::loadDatabase(CAirportDatabase* pAirportDatabase)
{
	ASSERT(pAirportDatabase);

	CString strSQL(_T(""));
	strSQL.Format(_T("SELECT * FROM TB_AIRPORTDB_ACTYPEDOOR" ));
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

	CACTYPELIST* actypes = m_pAcMan->GetACTypeList();
	int nCount = (int)actypes->size();
	while(!adoRecordset.IsEOF())
	{
		ACTypeDoor* pDoor = new ACTypeDoor();
		pDoor->loadDatabase(adoRecordset);
		for (int i = 0; i < nCount; i++)
		{
			CACType* pACType = actypes->at(i);
			if (pACType->GetID() == pDoor->GetACTypeID())
			{
				pACType->AddDoor(pDoor);
				break;
			}
		}
		adoRecordset.MoveNextData();
	}
	return true;
}

bool ACTypeDoorManager::saveDatabase(CAirportDatabase* pAirportDatabase)
{
	CACTYPELIST* actypeList = m_pAcMan->GetACTypeList();

	for (int i = 0; i < (int)actypeList->size(); i++)
	{
		CACType* pACType = actypeList->at(i);
		pACType->saveDoorDatabase(pAirportDatabase);
	}
	return true;
}

bool ACTypeDoorManager::deleteDatabase(CAirportDatabase* pAirportDatabase)
{
	CACTYPELIST* actypeList = m_pAcMan->GetACTypeList();

	for (int i = 0; i < (int)actypeList->size(); i++)
	{
		CACType* pACType = actypeList->at(i);
		pACType->deleteDoorDatabase(pAirportDatabase);
	}
	return true;
}
//////old code version,no more use//////////////////////////////////////////////////////////
void ACTypeDoorManager::ReadDataFromDB( int airportID,DATABASESOURCE_TYPE type /*= DATABASESOURCE_TYPE_ACCESS */ )
{
	ACTYPEDOORLIST DoorData ;
	ACTypeDoor::ReadDataFromDB(DoorData, airportID,type);

	CACTYPELIST* actypes = m_pAcMan->GetACTypeList();
	int nCount = actypes->size();
	ACTypeDoor* PDoor = NULL ;
	for (int i =0; i < nCount; i++)
	{
		CACType* actype = actypes->at(i);
		actype->clearAllDoorData() ;
		for (int ndx = 0 ; ndx < (int)DoorData.size() ;ndx++)
		{
			if(DoorData[ndx]->GetACTypeID() == actype->GetID())
			{
					PDoor = new ACTypeDoor ;
					*PDoor = *DoorData[ndx] ;
					PDoor->m_strACName = actype->GetIATACode();
					actype->AddDoor(PDoor) ;
			}
		}
	}
	for (int ndx = 0 ; ndx <(int)DoorData.size() ;ndx++)
	{
		delete DoorData[ndx] ;
	}
}

void ACTypeDoorManager::WriteDataToDB( int airportID,DATABASESOURCE_TYPE type /*= DATABASESOURCE_TYPE_ACCESS*/ )
{
	ACTYPEDOORLIST DoorData ;
	CACTYPELIST* actypes = m_pAcMan->GetACTypeList();
	int nCount = actypes->size();
	for (int i =0; i < nCount; i++)
	{
		CACType* actype = actypes->at(i);
		actype->DeleteDoorData(type) ;
		for (int ndx = 0 ; ndx < (int)actype->m_vACDoorList.size() ; ndx++)
		{
			actype->m_vACDoorList[ndx]->SetACID(actype->GetID()) ;
			DoorData.push_back(actype->m_vACDoorList[ndx]) ;
		}
	}
	ACTypeDoor::WriteDataToDB(DoorData, airportID,type);
}

ACTypeDoorManager::ACTypeDoorManager( CACTypesManager* _pAcMan )
:m_pAcMan(_pAcMan)
{

}

ACTypeDoorManager::~ACTypeDoorManager( void )
{
	m_pAcMan = NULL;
}

BOOL ACTypeDoorManager::ReadCommonACDoors( const CString& _path )
{
	BOOL bRet = TRUE;
	CFile* pFile = NULL;
	CString sFileName = _path + "\\" + _strACDoorFileName;

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
		version =(char*) strchr(csLine,',') ;
		int fileVersion = 0 ;
		if(version)
			fileVersion = atoi(version+1) ;


		//read line 2
		ar.ReadString( csLine );
		csLine.MakeUpper();
		strcpy( line, csLine );
		if(_stricmp(line, "Aircraft Door Database") == 0)
		{
			//skip the column names
			ar.ReadString( csLine );
			//read the values
			//read a line
			ar.ReadString( csLine );
			csLine.MakeUpper();
			strcpy( line, csLine );
			CString strACName;
			while( *line != '\0')
			{
				ACTypeDoor* pDoor = new ACTypeDoor();
				char* b = line;
				char* p = NULL;
				int c = 1;
				while((p = strchr(b, ',')) != NULL) {
					*p = '\0';
					switch(c)
					{
					case 1:	//ac name
						strACName = b;
						break ;
					case 2: // door name
						pDoor->m_strName = b;
						break;
					case 3:
						pDoor->m_dNoseDist = atof(b);
						break;
					case 4: 
						pDoor->m_dHeight= atof(b);
						break;
					case 5: 
						pDoor->m_dWidth = atof(b);
						break;
					case 6: 
						pDoor->m_dSillHeight = atof(b);
						break;
					case 7: 
						pDoor->m_enumDoorDir = (ACTypeDoor::DoorDir)atoi(b);
						break;
					}
					b = ++p;
					c++;
				}
				if(b!=NULL) {// the last column did not have a comma after it
					switch(c)
					{
					case 1:	//ac name
						strACName = b;
						break ;
					case 2: // door name
						pDoor->m_strName = b;
						break;
					case 3:
						pDoor->m_dNoseDist = atof(b);
						break;
					case 4: 
						pDoor->m_dHeight= atof(b);
						break;
					case 5: 
						pDoor->m_dWidth = atof(b);
						break;
					case 6: 
						pDoor->m_dSillHeight = atof(b);
						break;
					case 7: 
						pDoor->m_enumDoorDir = (ACTypeDoor::DoorDir)atoi(b);
						break;
					}
				}
				if (!AddACDoorItem(strACName, pDoor))
					delete pDoor ;

				ar.ReadString( csLine );
				csLine.MakeUpper();
				strcpy( line, csLine );
			}
		}
		else 
		{
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

bool ACTypeDoorManager::AddACDoorItem(const CString& strACName, ACTypeDoor* pDoor )
{
	CACType* pAC = m_pAcMan->getACTypeItem(strACName);
	if (pAC == NULL)
		return false;
	
	int nDoor = pAC->m_vACDoorList.size();
	for (int i =0; i < nDoor; i++)
	{
		ACTypeDoor* pACDoor = pAC->m_vACDoorList.at(i);
		if (pDoor->m_strName == pACDoor->m_strName)
			return false;
	}

	pAC->m_vACDoorList.push_back(pDoor);
	return true;

}