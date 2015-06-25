#include "StdAfx.h"
#include ".\fileindb.h"
#include <Shlwapi.h>
#include <database/ARCportADODatabase.h>
#include <common/TmpFileManager.h>
#include <common\ZipInfo.h>
#include <common\Guid.h>
#include <common/AirportDatabase.h>
#include <common\tinyxml/tinyxml.h>
//




int CFileInDB::Save( int id, AirportDatabaseConnection* pConn )
{
	if (!FileExists())
		return -1;
	if(id>=0)
	{
		CDatabaseADOConnetion::UpdateFileInDatabase(m_sFilePath,id,pConn);
		return id;
	}	
	return CDatabaseADOConnetion::SaveFileIntoDatabase(m_sFilePath,pConn);	
}

void CFileInDB::Read( int id,AirportDatabaseConnection* pConn )
{
	//m_ID = id;
	m_sFilePath = CDatabaseADOConnetion::ReadFileFromDatabase(id,pConn);
}

CFileInDB::CFileInDB()
{
	//m_ID = -1;
}

CFileInDB::CFileInDB( int id, AirportDatabaseConnection* pConn )
{
	Read(id,pConn);
}
void CFileInDB::Delete( int id, AirportDatabaseConnection* pConn )
{
	if(id>=0)
	{
		CDatabaseADOConnetion::DeleteFileFromDatabase(id,pConn);
	}
}

bool CFileInDB::FileExists() const
{
	return !m_sFilePath.IsEmpty() && PathFileExists(m_sFilePath);
}

void CFileInDB::MakeSureFileValid( CString ext )
{
	if(FileExists())
		return;
	m_sFilePath = CTmpFileManager::GetInstance().GetTmpFileName(ext);
}



CFileInDB::~CFileInDB()
{
	::DeleteFile(m_sFilePath);
}
/////////////////////////////////////////////////////////////////////////
CBaseDBModel::CBaseDBModel()
:m_nID(-1)
{
	m_nFileID = -1;
}

CBaseDBModel::~CBaseDBModel()
{
	CloseFile();
}


bool CBaseDBModel::IsFileOpen() const
{
	return PathFileExists(msTmpWorkDir)==TRUE;
}


void CBaseDBModel::CloseFile()
{
	if(!msTmpWorkDir.IsEmpty())
		CTmpFileManager::DeleteDirectory1(msTmpWorkDir);
}

bool CBaseDBModel::MakeSureFileOpen()
{
	if(IsFileOpen())return true;
	return OpenFile();
}

bool CBaseDBModel::OpenFile()
{
	CloseFile();
	msTmpWorkDir = CTmpFileManager::GetInstance().GetTmpDirectory();
	if(m_nFileID>=0){
		CFileInDB fileInDB(m_nFileID,GetConnection());
		ExtractFile(fileInDB.m_sFilePath);
	}
	return true;
}

void CBaseDBModel::ZipToFile( CString fielPath )
{
	ASSERT(IsFileOpen());
	CZipInfo zipFile;
	zipFile.AddDirectory(fielPath.GetString(),msTmpWorkDir.GetString());
}

void CBaseDBModel::ExtractFile( CString filePath )
{
	ASSERT(IsFileOpen());
	CZipInfo zipinfo;
	zipinfo.ExtractFiles(filePath.GetString(), msTmpWorkDir.GetString() );
}

void CBaseDBModel::SaveFile()
{
	CFileInDB mFile;
	mFile.MakeSureFileValid(GetFileExtnsion());
	ZipToFile(mFile.m_sFilePath);
	m_nFileID = mFile.Save(m_nFileID,GetConnection());
}


//////////////////////////////////////////////////////////////////////////
CModel::CModel()
{
	CGuid::Create(mguid);
	//msMeshFileName = (CString)mguid + _T(".mesh");
	mbExternalMesh = false;
}

CModel::~CModel()
{
}




void CModel::ReadContent()
{	
	if(!MakeSureFileOpen())return;
	///
	CString dir = msTmpWorkDir;
	CString tmpxmlFile = dir + STR_MODEL_FILE;
	TiXmlDocument xmldoc;
	xmldoc.LoadFile(tmpxmlFile.GetString());
	TiXmlElement* pModelElm = xmldoc.FirstChildElement(GetFileName());
	if(pModelElm)
	{
		int nVersion = GetCurrentVersion();
		pModelElm->Attribute(STR_VERSION,&nVersion);
		serializeFrom(pModelElm,nVersion);
	}
}

void CModel::SaveContent()
{
	CString dir = msTmpWorkDir;
	TiXmlDocument xmlDoc;
	TiXmlDeclaration xmlDeclare("1.0","gb2312", "yes");
	xmlDoc.InsertEndChild(xmlDeclare);
	TiXmlElement* pRoot = new TiXmlElement(GetFileName());
	pRoot->SetAttribute(STR_VERSION,GetCurrentVersion());
	xmlDoc.LinkEndChild( serializeTo(pRoot,GetCurrentVersion()) );
	xmlDoc.SaveFile( (dir + STR_MODEL_FILE).GetString() );	
}

TiXmlElement * CModel::serializeTo( TiXmlElement *pmodelElm, int nVersion ) const
{
	if(pmodelElm)	
	{
		pmodelElm->SetAttribute(STR_IDNAME,((CString)mguid).GetString());
		pmodelElm->SetAttribute(STR_CRTOR,GetCreator());
		pmodelElm->SetAttribute(STR_CRTIME,(int)GetCreateTime().GetTime());
		pmodelElm->SetAttribute(STR_MDTIME,(int)GetLastModifiedTime().GetTime());
		//pmodelElm->SetAttribute(STR_MESH_FILENAME,msMeshFileName.GetString());
		pmodelElm->SetAttribute(STR_BEXTMODEL,mbExternalMesh);
		//export camera
		pmodelElm->LinkEndChild( mActiveCam.serializeTo( new TiXmlElement(STR_MOD_CAM),nVersion ) );
		//grid
		pmodelElm->LinkEndChild( mGrid.serializeTo( new TiXmlElement(STR_MOD_GRID),nVersion ) );
	}	
	return pmodelElm;
}

void CModel::serializeFrom( const TiXmlElement* pElement, int nVersion )
{
	Clear();
	int tmpInt=0;
	const char* tempString=NULL;

	if(tempString = pElement->Attribute(STR_CRTOR))
		SetCreator(tempString);

	if(pElement->Attribute(STR_CRTIME,&tmpInt))
		m_tCreateTime = CTime(tmpInt);
	if(pElement->Attribute(STR_MDTIME,&tmpInt))
		m_tModifiedTime = CTime(tmpInt);

	if( tempString = pElement->Attribute(STR_IDNAME) )
	{
		if(strlen(tempString))
		{
			mguid = CGuid(tempString);
		}	
	}

	if(pElement->Attribute(STR_BEXTMODEL,&tmpInt)){		mbExternalMesh = (tmpInt!=0);	}

	const TiXmlElement* camElm = pElement->FirstChildElement(STR_MOD_CAM);
	if(camElm)
		mActiveCam.serializeFrom(camElm,GetCurrentVersion());
	const TiXmlElement* gridElm = pElement->FirstChildElement(STR_MOD_GRID);
	if(gridElm)
		mGrid.serializeFrom(gridElm,GetCurrentVersion());
}

void CModel::Clear()
{

}

CString CModel::GetMeshFileName() const
{
	return (CString)mguid+_T(".mesh");
}