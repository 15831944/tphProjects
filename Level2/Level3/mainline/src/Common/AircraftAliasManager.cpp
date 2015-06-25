#include "StdAfx.h"
#include "AircraftAliasManager.h"
#include "ProjectManager.h"
#include "AirportDatabase.h"
#include <algorithm>
#include <Shlwapi.h>
#include <map>
#include <vector>
#include "ACTypesManager.h"
#include "Aircraft.h"
#include "../Database/ARCportADODatabase.h"
using namespace std;
#define DEFAULT_SHAPE_NAME _T("defaultShape")
#define DEFAULT_AC_NAME _T("defaultAC")

//-------------------------------------------------------------------------
//AircraftAliasItem
AircraftAliasItem::AircraftAliasItem()
:m_strShapeName(DEFAULT_SHAPE_NAME)
{
	m_vectAssocItem.push_back(DEFAULT_AC_NAME);
}

AircraftAliasItem::~AircraftAliasItem(void)
{
}

void AircraftAliasItem::setSharpName(CString strShapeName)
{
	m_strShapeName = strShapeName;
}

CString AircraftAliasItem::getSharpName()
{
	return m_strShapeName;
}

bool AircraftAliasItem::addAssocItem(CString strName)
{
	AssocAircraftIter iter = std::find(m_vectAssocItem.begin(),
		m_vectAssocItem.end(), DEFAULT_AC_NAME);
	if(iter != m_vectAssocItem.end())
		m_vectAssocItem.erase(iter);

	m_vectAssocItem.push_back(strName);

	return true;
}

bool AircraftAliasItem::deleteAssocItem(CString strName)
{
	AssocAircraftIter iter = std::find(m_vectAssocItem.begin(),
		m_vectAssocItem.end(), strName);

	if(iter != m_vectAssocItem.end())
		m_vectAssocItem.erase((iter));

	if(m_vectAssocItem.size() == 0)
		m_vectAssocItem.push_back(DEFAULT_AC_NAME);
	return true;
}

bool AircraftAliasItem::fit(CString& strName)
{
	if(strName.CompareNoCase(m_strShapeName) == 0)
		return true;

	AssocAircraftIter iter = std::find(m_vectAssocItem.begin(),
		m_vectAssocItem.end(), strName);
	return (iter != m_vectAssocItem.end());
}

CString AircraftAliasItem::getRecordString()
{
	CString strRecord = m_strShapeName;

	for(AssocAircraftIter iter = m_vectAssocItem.begin();
		iter != m_vectAssocItem.end(); iter++)
	{
		CString strAssocName = *iter;
		CString strTemp;
		strTemp.Format("%s,%s",strRecord,strAssocName);	
		strRecord = strTemp;
	}

	return strRecord + _T("\r\n");
}

int	AircraftAliasItem::getCount()
{
	return (int)m_vectAssocItem.size();
}

CString AircraftAliasItem::getItem(int nIndex)
{
	ASSERT(nIndex >=0 && nIndex < (int)m_vectAssocItem.size());
	return m_vectAssocItem[nIndex];
}



//-------------------------------------------------------------------------
//AircraftAliasManager
AircraftAliasManager::AircraftAliasManager(void)
:m_bIsDataLoad(false)
{

}

AircraftAliasManager::~AircraftAliasManager(void)
{
	reset();
}

CString AircraftAliasManager::getOglShapeName(CString& strACName)
{
	/*if(!m_bIsDataLoad)
		readData(pAirportDatabase);*/
	
	/*for(AircraftAliasItemIter iter = m_vectAircraftAliasItem.begin();
		iter != m_vectAircraftAliasItem.end(); iter++)
	{
		if( (*iter)->fit(strACName) )
			return (*iter)->getSharpName();
	}*/
    
	airCraftshapeiter iter=AirCraftShapeList.find(strACName);
	if (iter!=AirCraftShapeList.end())
	{
		return iter->second;
	}
	
	/*if(iter!=AirCraftShapeList.end())
	{

		vector<CACType*> pACList=GetCACTypeManager()->GetACTypeList();
		for (int i=0; i<(int)pACList->size(); i++)
		{
			CACType* pACType = (*pACList)[i];
			if(pACType->GetIATACode().CompareNoCase(strACName)==0)
			{
				return *iter->
			}
		}
	}
	*/

	return DEFAULT_AC_NAME;
}

bool AircraftAliasManager::isAssociated(CString& strACName)
{
	/*if(!m_bIsDataLoad)
		readData(pAirportDatabase);*/
   airCraftshapeiter iter=AirCraftShapeList.find(strACName);
   if (iter!=AirCraftShapeList.end())
	   
   {
	   return true;
   }
	//for(AircraftAliasItemIter iter = m_vectAircraftAliasItem.begin();
	//	iter != m_vectAircraftAliasItem.end(); iter++)
	//{
	//	if( (*iter)->fit(strACName) )
	//		return true;
	//}

	return false;
}

bool AircraftAliasManager::isAssociatedShapeName(CString &strShapeName,int nIdex)
{
	CString strACName=getItem(nIdex);
	CString shapeName=AirCraftShapeList[strACName];
	if (shapeName.CompareNoCase(strShapeName)==0)
	{
		return true;
	}
	return false;
}

void AircraftAliasManager::addItem(CString airCraft,CString ShapeName)
{
	/*m_vectAircraftAliasItem.push_back(pNewItem);*/
	AirCraftShapeList.insert(make_pair(airCraft,ShapeName));
}

int	AircraftAliasManager::getCount()
{
	return (int)AirCraftShapeList.size();
}

CString  AircraftAliasManager::getItem(int nIndex)
{
	/*ASSERT(nIndex >=0 && nIndex < (int)m_vectAircraftAliasItem.size());
	return m_vectAircraftAliasItem[nIndex];*/
	/*vector<CACType*> ACTypeList= m_pACManager->GetACTypeList();
		return ACTypeList[nIndex];*/
	airCraftshapeiter iter=AirCraftShapeList.begin();
	int i=0;
	CString AircraftName;
	for (iter;iter!=AirCraftShapeList.end();iter++)
	{
		if (i==nIndex)
		{
		  AircraftName=iter->first;
		}
		i++;
	}


	std::vector<CACType*> *ACTypeList=m_pACManager->GetACTypeList();
	for(i=0;i<(int)ACTypeList->size();i++)
	{
		CACType* pACType = (*ACTypeList)[i];
		if (AircraftName.CompareNoCase(pACType->GetIATACode())==0)
		{
			return pACType->GetIATACode();
		}
	}
	return DEFAULT_AC_NAME;
}

void AircraftAliasManager::reset()
{
	//for(AircraftAliasItemIter iter = m_vectAircraftAliasItem.begin();
	//	iter != m_vectAircraftAliasItem.end(); iter++)
	//{
	//	delete *iter;
	//}
	//m_vectAircraftAliasItem.clear();
	AirCraftShapeList.clear();
}

//database interface
void AircraftAliasManager::GetAllDefaultAirCraftShape()
{
 
	
	CString strDefaultFile;
	strDefaultFile.Format(_T("%s\\AircraftAlias_Default.dat"), PROJMANAGER->m_csAirportPath);
	if (PathFileExists(strDefaultFile) == FALSE )
	{
		AfxMessageBox(_T("Error reading Aircraft Alias databases file, Please update the resource to version 3.5 or later"), MB_ICONEXCLAMATION | MB_OK);
	}
	

	std::vector<CACType*> *ACTypeList=m_pACManager->GetACTypeList();

	CFile* pFile = NULL;
	
	try
	{
		pFile = new CFile(strDefaultFile, CFile::modeRead | CFile::shareDenyNone);
		CArchive ar( pFile, CArchive::load);
		
		char line[513];
		CString csLine = _T("");

		ar.ReadString(csLine);
		ar.ReadString(csLine);
		ar.ReadString(csLine);

		ar.ReadString(csLine);
		strcpy( line, csLine );
		CString  AircrafName;
		CString ShapeName;

		
		while (*line != '\0')
		{
			char* b = line;
			char* p = NULL;
			int c = 1;
			while ( (p = strchr(b, ',')) != NULL)
			{
				*p = '\0';
				switch(c)
				{
				case 1:
					//pNewItem->setSharpName(b);
					ShapeName=b;
					break;
				default:
					for (int i=0;i<(int)ACTypeList->size();i++)

					{
						CACType *acType=(*ACTypeList)[i];
					    AircrafName=b;
					    if(AircrafName.CompareNoCase(acType->GetIATACode()))	
						{
							AirCraftShapeList.insert(make_pair(AircrafName,ShapeName));
							break;
						}
					}
					
				    break;
				}

				b = ++p;
				c++;
			}
			/*if( b!= NULL)
				pNewItem->addAssocItem(b);*/

		/*	m_vectAircraftAliasItem.push_back(pNewItem);*/
			ar.ReadString( csLine );
			strcpy( line, csLine);
		}

		ar.Close();
		delete pFile;
	}
	catch (CException* e)
	{
		AfxMessageBox(_T("Error reading Aircraft Alias databases file"), MB_ICONEXCLAMATION | MB_OK);
		e->Delete();
		if(pFile != NULL)
			delete pFile;
	}

}
void AircraftAliasManager::GetAllNonDefaultAirCraftShape(CAirportDatabase*pAirportDataBase)
{
	CString strSQL;
	strSQL.Format("select * from Shape_aircrafttype");
	long recordCount;
	CADORecordset adoRecordset;
	CDatabaseADOConnetion::ExecuteSQLStatement(strSQL,recordCount,adoRecordset,pAirportDataBase->GetAirportConnection());
	CString ShapeName;
	CString AirCraftName;
	std::vector<CACType*>*ACTypeList=m_pACManager->GetACTypeList();
	
	while(!adoRecordset.IsEOF())
	{

		adoRecordset.GetFieldValue(_T("ShapeName"),ShapeName);
		adoRecordset.GetFieldValue(_T("aircraftTypeName"),AirCraftName);
		for (int i=0;i<(int)ACTypeList->size();i++)
		{
			CACType* pACType = (*ACTypeList)[i];
			if (AirCraftName.CompareNoCase(pACType->GetIATACode())==0)
			{
				AirCraftShapeList.insert(make_pair(AirCraftName,ShapeName));
				break;
			}
		}
		adoRecordset.MoveNextData();
		
	}

}
bool AircraftAliasManager::readData(CAirportDatabase *pAirportDatabase)
{
	reset();
	GetAllNonDefaultAirCraftShape(pAirportDatabase);
	
	CString StrSQL;
	StrSQL.Format("select * from Single_projectInfo where AirCraftAdd>0");
    long lCount;
	CADORecordset adoRecordSet;
	CDatabaseADOConnetion::ExecuteSQLStatement(StrSQL,lCount,adoRecordSet,pAirportDatabase->GetAirportConnection());
	if (!adoRecordSet.IsEOF())
	{

		return true;
	}
	GetAllDefaultAirCraftShape();
	CString strAliasFilePath;
	strAliasFilePath.Format(_T("%s\\AircraftAlias.dat"), PROJMANAGER->m_csAirportPath);
	
	if (PathFileExists(strAliasFilePath) == FALSE )// copy default file 
	{
		/*CString strDefaultFile;
		strDefaultFile.Format(_T("%s\\AircraftAlias_Default.dat"), PROJMANAGER->m_csAirportPath);
		if (PathFileExists(strDefaultFile) == FALSE )
		{
			AfxMessageBox(_T("Error reading Aircraft Alias databases file, Please update the resource to version 3.5 or later"), MB_ICONEXCLAMATION | MB_OK);
			m_bIsDataLoad = true;
			return false;
		}
		CopyFile(strDefaultFile,strAliasFilePath,TRUE);*/
		return true ;

	}
	std::vector<CACType*> *ACTypeList=m_pACManager->GetACTypeList();

	CFile* pFile = NULL;
	
	try
	{
		pFile = new CFile(strAliasFilePath, CFile::modeRead | CFile::shareDenyNone);
		CArchive ar( pFile, CArchive::load);
		
		char line[513];
		CString csLine = _T("");

		ar.ReadString(csLine);
		ar.ReadString(csLine);
		ar.ReadString(csLine);

		ar.ReadString(csLine);
		strcpy( line, csLine );
		CString  AircrafName;
		CString ShapeName;

		
		while (*line != '\0')
		{
			
			char* b = line;
			char* p = NULL;
			int c = 1;
			while ( (p = strchr(b, ',')) != NULL)
			{
				*p = '\0';
				switch(c)
				{
				case 1:
					//pNewItem->setSharpName(b);
					ShapeName=b;
					break;
				default:
					for (int i=0;i<(int)ACTypeList->size();i++)

					{
						CACType *acType=(*ACTypeList)[i];
					    AircrafName=b;
					    if(AircrafName.CompareNoCase(acType->GetIATACode()))	
						{
							AirCraftShapeList.insert(make_pair(AircrafName,ShapeName));
							break;
						}
					}
					
				    break;
				}

				b = ++p;
				c++;
			}
			/*if( b!= NULL)
				pNewItem->addAssocItem(b);*/

		/*	m_vectAircraftAliasItem.push_back(pNewItem);*/
			ar.ReadString( csLine );
			strcpy( line, csLine);
		}

		ar.Close();
		delete pFile;
	}
	catch (CException* e)
	{
		AfxMessageBox(_T("Error reading Aircraft Alias databases file"), MB_ICONEXCLAMATION | MB_OK);
		e->Delete();
		if(pFile != NULL)
			delete pFile;
		m_bIsDataLoad = true;
		return false;
	}
 
	m_bIsDataLoad = true;
	return true;
}

void AircraftAliasManager::deleteItem(CString strACName)
{

	AirCraftShapeList.erase(strACName);
}
bool AircraftAliasManager::saveData(CAirportDatabase *pAirportDatabase)
{
	//CFile* pFile = NULL;
	//CString strAliasFilePath;
	//strAliasFilePath.Format(_T("%s\\AircraftAlias.dat"), PROJMANAGER->m_csAirportPath);
	//BOOL bRet = TRUE;

	/*try
	{
		pFile = new CFile(strAliasFilePath, CFile::modeCreate | CFile::modeWrite | CFile::shareExclusive);
		CArchive ar(pFile, CArchive::store);
		ar.WriteString(_T("Databases\\AircraftAlias.dat,1.00\r\nAircraft Alias Database\r\n"));
		ar.WriteString(_T("ShapeName ACType\r\n"));

		CString strSingleLine = _T("");
		for (AircraftAliasItemIter iter = m_vectAircraftAliasItem.begin();
			iter != m_vectAircraftAliasItem.end(); iter++)
		{
			strSingleLine += (*iter)->getRecordString();	
			ar.WriteString(strSingleLine);
			strSingleLine.Empty();
		}

		CTime t = CTime::GetCurrentTime();
		ar.WriteString("\r\n" + t.Format("%m/%d/%y,%H:%M:%S"));
		ar.Close();
		delete pFile;
	}
	catch (CException* e)
	{
		AfxMessageBox(_T("Error writing Aircraft Alias databases file."), MB_ICONEXCLAMATION | MB_OK);
		e->Delete();
		if(pFile != NULL)
			delete pFile;
		return false;
	}*/
	CString strSQL;
	strSQL.Format("delete from Shape_aircrafttype");
	try
	{
		long lCount;
		CDatabaseADOConnetion::BeginTransaction(pAirportDatabase->GetAirportConnection());
		CDatabaseADOConnetion::ExecuteSQLStatement(strSQL,pAirportDatabase->GetAirportConnection()) ;
		strSQL.Format("select AirCraftAdd from single_ProjectInfo where AirCraftAdd>=1 ");
		CADORecordset adoRecordSet;
		CDatabaseADOConnetion::ExecuteSQLStatement(strSQL,lCount,adoRecordSet,pAirportDatabase->GetAirportConnection());
		if (adoRecordSet.IsEOF())
		{

			strSQL.Format("Insert Into single_ProjectInfo  (AirCraftAdd) values(1)");
			CDatabaseADOConnetion::ExecuteSQLStatement(strSQL,pAirportDatabase->GetAirportConnection());
		}
	
		airCraftshapeiter iter =AirCraftShapeList.begin();
		int i=0;
		for (iter;iter!=AirCraftShapeList.end();iter++)
		{
			i++;
			strSQL.Format("insert into Shape_aircrafttype(ID,aircraftTypeName,shapeName) values(%d,'%s','%s')",i,iter->first,iter->second);
			CDatabaseADOConnetion::ExecuteSQLStatement(strSQL,pAirportDatabase->GetAirportConnection()) ;
		}
		
		CDatabaseADOConnetion::CommitTransaction(pAirportDatabase->GetAirportConnection());

	}
	catch (CException* e)
	{
		AfxMessageBox(_T("Error writing Aircraft Alias databases file."), MB_ICONEXCLAMATION | MB_OK);
		e->Delete();
		CDatabaseADOConnetion::RollBackTransation(pAirportDatabase->GetAirportConnection());
		return false;
	}
	
	
	return true;
}
bool AircraftAliasManager::loadDatabase(CAirportDatabase *pAirportDatabase)
{
	reset();

	ASSERT(pAirportDatabase);

	CString strSQL(_T(""));
	strSQL.Format(_T("SELECT * FROM Shape_aircrafttype"));
	CADORecordset adoRecordset;
	long lCount = 0;
	try
	{
		CDatabaseADOConnetion::ExecuteSQLStatement(strSQL,lCount,adoRecordset,pAirportDatabase->GetAirportConnection()) ;

		CString ShapeName;
		CString AirCraftName;
		std::vector<CACType*>*ACTypeList=m_pACManager->GetACTypeList();
		while(!adoRecordset.IsEOF())
		{

			adoRecordset.GetFieldValue(_T("ShapeName"),ShapeName);
			adoRecordset.GetFieldValue(_T("aircraftTypeName"),AirCraftName);

			for (int i=0;i<(int)ACTypeList->size();i++)
			{
				CACType* pACType = (*ACTypeList)[i];
				if (AirCraftName.CompareNoCase(pACType->GetIATACode())==0)
				{
					AirCraftShapeList.insert(make_pair(AirCraftName,ShapeName));
					break;
				}
			}
			adoRecordset.MoveNextData();
		}
	}
	catch (CADOException& e)
	{
		e.ErrorMessage();
		return false;
	}
	return true;
}
AircraftAliasManager* AircraftAliasManager::GetInstance()
{
	if(!m_pInstance)
		m_pInstance = new AircraftAliasManager();
	return m_pInstance;
}

void AircraftAliasManager::ReleaseInstance()
{
	delete m_pInstance;
	m_pInstance = NULL;
}

void AircraftAliasManager::SetInstance( AircraftAliasManager* pInst )
{
	m_pInstance = pInst;
}

AircraftAliasManager* AircraftAliasManager::m_pInstance =NULL;












