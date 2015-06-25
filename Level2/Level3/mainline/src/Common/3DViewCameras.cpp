#include "StdAfx.h"
#include ".\3dviewcameras.h"
#include "tinyxml/tinyxml.h"
#include "FileInDB.h"
#include "TmpFileManager.h"
#include <Database/ARCportADODatabase.h>
#include <Inputs/IN_TERM.H>

CCameraData& C3DViewCameraDesc::GetCameraData( int nRow, int nCol )
{
	return const_cast<CCameraData&>(
		(const_cast<const C3DViewCameraDesc*>(this))->GetCameraData(nRow, nCol)
		);
}

const CCameraData& C3DViewCameraDesc::GetCameraData( int nRow, int nCol ) const
{
	if (nRow>=0 && nRow<C3DViewPaneInfo::MaxRowNum && nCol>=0 && nCol<C3DViewPaneInfo::MaxColNum)
	{
		return m_cameras[nRow][nCol];
	}
	ASSERT(FALSE);
	return m_cameras[0][0];
}

#define STR_NAME           "name"
#define STR_LEFT_WIDTH     "left_width"
#define STR_RIGHT_WIDTH    "right_width"
#define STR_TOP_HEIGHT     "top_height"
#define STR_BOTTOM_HEIGHT  "bottom_height"

TiXmlElement * C3DViewCameraDesc::serializeTo( TiXmlElement* pElm, int nVersion ) const
{
	pElm->SetAttribute(STR_NAME, m_sName.GetString());
	pElm->SetAttribute(STR_LEFT_WIDTH, m_paneInfo.nLeftWidth );
	pElm->SetAttribute(STR_RIGHT_WIDTH, m_paneInfo.nRightWidth );
	pElm->SetAttribute(STR_TOP_HEIGHT, m_paneInfo.nTopHeight );
	pElm->SetAttribute(STR_BOTTOM_HEIGHT, m_paneInfo.nBottomHeight );

	for(int i=0;i<C3DViewPaneInfo::MaxRowNum;i++)
	{
		for(int j=0;j<C3DViewPaneInfo::MaxColNum;j++)
		{
			pElm->LinkEndChild(m_cameras[i][j].serializeTo(new TiXmlElement(GetCameraXMLTag(i, j)), nVersion));
		}
	}

	return pElm;
}

void C3DViewCameraDesc::serializeFrom( const TiXmlElement* pElm, int nVersion )
{
	m_sName = pElm->Attribute(STR_NAME);
	pElm->Attribute(STR_LEFT_WIDTH, &m_paneInfo.nLeftWidth);
	pElm->Attribute(STR_RIGHT_WIDTH, &m_paneInfo.nRightWidth);
	pElm->Attribute(STR_TOP_HEIGHT, &m_paneInfo.nTopHeight);
	pElm->Attribute(STR_BOTTOM_HEIGHT, &m_paneInfo.nBottomHeight);

	for(int i=0;i<C3DViewPaneInfo::MaxRowNum;i++)
	{
		for(int j=0;j<C3DViewPaneInfo::MaxColNum;j++)
		{
			const TiXmlElement* camElm = pElm->FirstChildElement(GetCameraXMLTag(i, j));
			if(camElm)
				m_cameras[i][j].serializeFrom(camElm, nVersion);
		}
	}

}

CString C3DViewCameraDesc::GetCameraXMLTag( int nRow, int nCol )
{
	CString strTag;
	strTag.Format(_T("camera_row%d_col%d"), nRow, nCol);
	return strTag;
}

void C3DViewCameraDesc::InitCameraData( CCameraData& camData, CCameraData::PROJECTIONTYPE ept )
{
	camData.m_eProjType = ept;
	switch(ept)
	{
	case CCameraData::perspective:
		camData.m_vLocation = ARCVector3(1600,0,3600);
		camData.m_vLook = ARCVector3(-1,0,-1);
		camData.m_vUp = ARCVector3(-1,0,1);
		camData.dWorldWidth = 100;
		camData.dFovy = 30;
		break;
	case CCameraData::parallel:
		camData.m_vLocation = ARCVector3(0,0,3600);
		camData.m_vLook = ARCVector3(0,0,-1);
		camData.m_vUp = ARCVector3(-1,0,1);
		camData.dWorldWidth = 3000;
		camData.dFovy = 30;
		break;
	}

}

void C3DViewCameraDesc::InitCameraData( CCameraData::PROJECTIONTYPE ept )
{
	for(int i=0;i<C3DViewPaneInfo::MaxColNum;i++)
	{
		for(int j=0;j<C3DViewPaneInfo::MaxRowNum;j++)
		{
			InitCameraData(m_cameras[i][j], ept);
		}
	}
	m_paneInfo.nRightWidth = 0;
	m_paneInfo.nBottomHeight = 0;
}

CCameraData& C3DViewCameraDesc::GetDefaultCameraData()
{
	return GetCameraData(0, 0);
}

const CCameraData& C3DViewCameraDesc::GetDefaultCameraData() const
{
	return GetCameraData(0, 0);
}

void C3DViewCameraDesc::InitCameraDataByMode( CCameraData& camData, CCameraData::PROJECTIONTYPE ept, EnvironmentMode eEnv )
{
	camData.m_eProjType = ept;
	switch(ept)
	{
	case CCameraData::perspective:
		camData.m_vLocation = ARCVector3(16000,0,8000);
		camData.m_vLook = ARCVector3(-2,0,-1);
		camData.m_vUp = ARCVector3(-1,0,1);
		camData.dWorldWidth = 100;
		camData.dFovy = 30;
		break;
	case CCameraData::parallel:
		camData.m_vLocation = ARCVector3(0,0,8000);
		camData.m_vLook = ARCVector3(0,0,-1);
		camData.m_vUp = ARCVector3(-1,0,1);
		camData.dWorldWidth = 3000;
		camData.dFovy = 30;
		break;
	}
}

void C3DViewCameraDesc::InitCameraDataByMode( CCameraData::PROJECTIONTYPE ept, EnvironmentMode eEnv )
{
	for(int i=0;i<C3DViewPaneInfo::MaxColNum;i++)
	{
		for(int j=0;j<C3DViewPaneInfo::MaxRowNum;j++)
		{
			InitCameraDataByMode(m_cameras[i][j], ept, eEnv);
		}
	}
	m_paneInfo.nRightWidth = 0;
	m_paneInfo.nBottomHeight = 0;
}
C3DViewCameras::C3DViewCameras(void)
	: m_b3DMode(true)
{
	m_userCamera3D.InitCameraData(CCameraData::perspective);
	m_userCamera2D.InitCameraData(CCameraData::parallel);
}

C3DViewCameras::~C3DViewCameras(void)
{
}

#define STR_USE_3DMODE        "use_3dmode"
#define STR_3DCAMERA          "user3d_camera"
#define STR_2DCAMERA          "user2d_camera"

TiXmlElement* C3DViewCameras::serializeTo( TiXmlElement* pElm ) const
{
	pElm->SetAttribute(STR_USE_3DMODE, int(m_b3DMode));
	pElm->LinkEndChild(m_userCamera3D.serializeTo(new TiXmlElement(STR_3DCAMERA), GetCurrentVersion()));
	pElm->LinkEndChild(m_userCamera2D.serializeTo(new TiXmlElement(STR_2DCAMERA), GetCurrentVersion()));

	C3DViewCameraDesc::List::const_iterator ite = m_savedCameras.begin();
	C3DViewCameraDesc::List::const_iterator iteEn = m_savedCameras.end();
	for (;ite!=iteEn;ite++)
	{
		pElm->LinkEndChild(ite->serializeTo(new TiXmlElement(GetSavedCameraDescXMLTag(ite - m_savedCameras.begin())), GetCurrentVersion()));
	}

	return pElm;
}

void C3DViewCameras::serializeFrom( const TiXmlElement* pElm )
{
	int nTmp = 1;
	pElm->Attribute(STR_USE_3DMODE, &nTmp);
	m_b3DMode = nTmp == 1 ? true : false;

	const TiXmlElement* cam3DElm = pElm->FirstChildElement(STR_3DCAMERA);
	if(cam3DElm)
		m_userCamera3D.serializeFrom(cam3DElm, GetCurrentVersion());
	const TiXmlElement* cam2DElm = pElm->FirstChildElement(STR_2DCAMERA);
	if(cam2DElm)
		m_userCamera2D.serializeFrom(cam2DElm, GetCurrentVersion());

	m_savedCameras.clear();
	int nIndex = 0;
	while (true)
	{
		const TiXmlElement* camElm = pElm->FirstChildElement(GetSavedCameraDescXMLTag(nIndex));
		if(!camElm)
			break;

		nIndex++;
		m_savedCameras.resize(nIndex);
		m_savedCameras.back().serializeFrom(camElm, GetCurrentVersion());
	}

}

CString C3DViewCameras::GetSavedCameraDescXMLTag(int nIndex)
{
	CString strTag;
	strTag.Format(_T("saved_camer%d"), nIndex);
	return strTag;
}

int C3DViewCameras::GetCurrentVersion() const
{
	return 100;
}

std::string C3DViewCameras::GetCamerasXMLStirng() const
{
	std::stringstream ss;
	ss << *this;
	return ss.str();
}

void C3DViewCameras::ParseCamerasFromXMLString( const std::string& str )
{
	std::stringstream ss;
	ss << str;
	ss >> *this;
}

const C3DViewCameraDesc& C3DViewCameras::GetUserCamera() const
{
	return m_b3DMode ? m_userCamera3D : m_userCamera2D;
}

C3DViewCameraDesc& C3DViewCameras::GetUserCamera()
{
	return const_cast<C3DViewCameraDesc&>(const_cast<const C3DViewCameras*>(this)->GetUserCamera());
}

void C3DViewCameras::AddSavedCameraDesc( const C3DViewCameraDesc& cam )
{
	m_savedCameras.push_back(cam);
}

void C3DViewCameras::AddSavedCameraDesc( const CCameraData& cam, CString strName )
{
	m_savedCameras.resize(m_savedCameras.size() + 1);
	C3DViewCameraDesc& camDesc = m_savedCameras.back();
	camDesc.SetName(strName);
	camDesc.InitCameraData(cam.m_eProjType);
	camDesc.GetDefaultCameraData() = cam;
}

void C3DViewCameras::AddSavedCameraDesc(CString strName)
{
	m_savedCameras.push_back(GetUserCamera());
	m_savedCameras.back().SetName(strName);
}


#define STR_CAMERA_VERSION "format_version"
#define STR_CAMERA_DATA    "cameras_data"


bool C3DViewCameras::ReadFromDatabase( ParentObjectType eType, int nParentID )
{
	int nDataID = GetXMLDataID(eType, nParentID);
	if (nDataID>0)
	{
		CFileInDB fid(nDataID, &GetConnection());

		TiXmlDocument xmldoc;
		xmldoc.LoadFile(fid.m_sFilePath);
		TiXmlElement* pModelElm = xmldoc.FirstChildElement(STR_CAMERA_DATA);
		if(pModelElm)
		{
			int nVersion = GetCurrentVersion();
			pModelElm->Attribute(STR_CAMERA_VERSION,&nVersion);
			ASSERT(nVersion == GetCurrentVersion());
			serializeFrom(pModelElm);
			return true;
		}
	}

	return false;
}

bool C3DViewCameras::SaveIntoDatabase( ParentObjectType eType, int nParentID )
{
	CString strTmpFile = CTmpFileManager::GetInstance().GetTmpFileName(_T("xml"));
	TiXmlDocument xmlDoc;
	TiXmlDeclaration xmlDeclare("1.0","gb2312", "yes");
	xmlDoc.InsertEndChild(xmlDeclare);
	TiXmlElement* pRoot = new TiXmlElement(STR_CAMERA_DATA);
	pRoot->SetAttribute(STR_CAMERA_VERSION,GetCurrentVersion());
	xmlDoc.LinkEndChild(serializeTo(pRoot));

	if (xmlDoc.SaveFile(strTmpFile))
	{
		CFileInDB fid;
		fid.m_sFilePath = strTmpFile;
		int nDataID = GetXMLDataID(eType, nParentID);
		nDataID = fid.Save(nDataID, &GetConnection());
		ASSERT(nDataID>0);
		DeleteDBRecord(eType, nParentID);
		InsertDBRecord(eType, nParentID, nDataID);
		return true;
	}

	return false;
}

bool C3DViewCameras::DeleteFromDatabase( ParentObjectType eType, int nParentID )
{
	int nDataID = GetXMLDataID(eType, nParentID);
	if (nDataID>0)
	{
		CDatabaseADOConnetion::DeleteFileFromDatabase(nDataID, &GetConnection());
		return DeleteDBRecord(eType, nParentID);
	}
	return false;
}

int C3DViewCameras::GetXMLDataID( C3DViewCameras::ParentObjectType eType, int nParentID )
{
	CString SQL ;
	SQL.Format(_T("SELECT * FROM TB_3DVIEW_CAMERAS WHERE PARENT_TYPE = %d AND PARENTID = %d"), eType, nParentID);
	long num = 0 ;
	CADORecordset dataset ;
	try
	{
		CDatabaseADOConnetion::ExecuteSQLStatement(SQL,num,dataset,&GetConnection());
	}
	catch (CADOException e)
	{
		e.ErrorMessage() ;
		return -1;
	}

	int nDataID = -1;
	if (!dataset.IsEOF())
	{
		dataset.GetFieldValue("XML_DATA_ID", nDataID);
	}
	return nDataID;
}

bool C3DViewCameras::DeleteDBRecord( ParentObjectType eType, int nParentID )
{
	CString SQL ;
	SQL.Format(_T("DELETE * FROM TB_3DVIEW_CAMERAS WHERE PARENT_TYPE = %d AND PARENTID = %d"), eType, nParentID);
	try
	{
		CDatabaseADOConnetion::ExecuteSQLStatement(SQL,&GetConnection());
		return true;
	}
	catch (CADOException e)
	{
		e.ErrorMessage() ;
		return false;
	}
}


const TCHAR* const C3DViewCameras::m_sParentObjectTypeName[ParentObjectType_Count] =
{
	_T("Aircraft Layout"),
	_T("Environment")
};

bool C3DViewCameras::InsertDBRecord( ParentObjectType eType, int nParentID, int nDataID )
{
	CString SQL ;
	SQL.Format(_T("INSERT INTO TB_3DVIEW_CAMERAS(PARENT_TYPE, PARENT_TYPE_NAME, PARENTID, XML_DATA_ID) VALUES(%d, '%s', %d, %d)")
		, eType
		, m_sParentObjectTypeName[eType]
		, nParentID
		, nDataID
		);
	try
	{
		CDatabaseADOConnetion::ExecuteSQLStatement(SQL,&GetConnection());
		return true;
	}
	catch (CADOException e)
	{
		e.ErrorMessage() ;
		return false;
	}
}

AirportDatabaseConnection C3DViewCameras::GetConnection()
{
	AirportDatabaseConnection DBConn(DATABASECONNECTION.GetConnection(DATABASESOURCE_TYPE_ACCESS));
	return DBConn;
}

void C3DViewCameras::InitCameraData()
{
	*this = C3DViewCameras();
}


std::istream& operator>>( std::istream& is, C3DViewCameras& cams )
{
	TiXmlDocument xmldoc;
	is >> xmldoc;
	TiXmlElement* pModelElm = xmldoc.FirstChildElement(STR_CAMERA_DATA);
	if(pModelElm)
	{
		int nVersion = cams.GetCurrentVersion();
		pModelElm->Attribute(STR_CAMERA_VERSION,&nVersion);
		ASSERT(nVersion == cams.GetCurrentVersion());
		cams.serializeFrom(pModelElm);
	}
	return is;
}

std::ostream& operator<<( std::ostream& os, const C3DViewCameras& cams )
{
	TiXmlDocument xmlDoc;
	TiXmlDeclaration xmlDeclare("1.0","gb2312", "yes");
	xmlDoc.InsertEndChild(xmlDeclare);
	TiXmlElement* pRoot = new TiXmlElement(STR_CAMERA_DATA);
	pRoot->SetAttribute(STR_CAMERA_VERSION, cams.GetCurrentVersion());
	xmlDoc.LinkEndChild( cams.serializeTo(pRoot) );

	return os << xmlDoc;
}