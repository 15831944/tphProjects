#include "StdAfx.h"
#include ".\structure.h"
#include "ALTObjectDisplayProp.h"

#define	FACE_SEL		3 
#define USERDATA_SEL	3

Structure::Structure(void)
{
	m_nPathID = -1;
}

Structure::Structure( const Structure& otherStrurct ):ALTObject(otherStrurct)
{	
	DeepCopy(otherStrurct);
}

Structure::~Structure(void)
{
	std::vector<UserInputData*>::iterator iter = m_vUserData.begin();
	for (; iter != m_vUserData.end(); ++iter)
	{
		delete *iter;
	}

 	std::vector<StructFace*>::iterator it = m_vStructFace.begin();
 	for (; it != m_vStructFace.end(); ++it)
 	{
 		delete *it;
 	}
}

//NAME_L1, NAME_L2, NAME_L3, NAME_L4, LOCATIONID, POINTCOUNT, \
//TSTAMP
void Structure::ReadObject(int nObjID)
{
	m_nObjID = nObjID;
	clear();
	CADORecordset adoRecordset;
	long nRecordCount = -1;
	CADODatabase::ExecuteSQLStatement(GetSelectScript(nObjID),nRecordCount,adoRecordset);

	if (!adoRecordset.IsEOF())
	{	

		ALTObject::ReadObject(adoRecordset);
		
		adoRecordset.GetFieldValue(_T("PATHID"),m_nPathID);
		int nPointCount = -1;
		adoRecordset.GetFieldValue(_T("POINTCOUNT"),nPointCount);
		adoRecordset.GetFieldValue(_T("PATH"),nPointCount,m_path);
		CString strData = _T("");
		adoRecordset.GetFieldValue(_T("USERDATA"),strData);
		ConveyToUserData(strData);
		
		strData = _T("");
		adoRecordset.GetFieldValue(_T("STRUCTFACE"),strData);
		ConveyToStructFace(strData);
	}

}
int Structure::SaveObject(int nAirportID)
{
	int nObjID = ALTObject::SaveObject(nAirportID,ALT_STRUCTURE);

	m_nPathID = CADODatabase::SavePath2008IntoDatabase(m_path);

	CADODatabase::ExecuteSQLStatement(GetInsertScript(nObjID));

	return nObjID;
}
void Structure::UpdateObject(int nObjID)
{
	ALTObject::UpdateObject(nObjID);

	CADODatabase::UpdatePath2008InDatabase(m_path,m_nPathID);
	CADODatabase::ExecuteSQLStatement(GetUpdateScript(nObjID));
}
void Structure::DeleteObject(int nObjID)
{
	ALTObject::DeleteObject(nObjID);

	CADODatabase::DeletePathFromDatabase(m_nPathID);

	CADODatabase::ExecuteSQLStatement(GetDeleteScript(nObjID));
}
CString Structure::GetSelectScript(int nObjID) const
{
	CString strSQL = _T("");
	strSQL.Format(_T("SELECT APTID, NAME_L1, NAME_L2, NAME_L3, NAME_L4, LOCKED, PATHID,POINTCOUNT,\
		PATH,TSTAMP,USERDATA,STRUCTFACE\
		FROM STRUCTURE_VIEW\
		WHERE ID = %d"),nObjID);
	return strSQL;

}
CString Structure::GetInsertScript(int nObjID)
{
	CString strSQL = _T("");
	CString strUserData =_T("");
	CString strStructFace = _T("");
	strStructFace = FormatStructFace(m_vStructFace);
	strUserData = FormatUserData(m_vUserData);
	strSQL.Format(_T("INSERT INTO STRUCTUREPROP\
		(OBJID, PATH, USERDATA, STRUCTFACE)\
		VALUES (%d,%d,'%s','%s')"),
		nObjID,m_nPathID,strUserData,strStructFace);
	return strSQL;

}
CString Structure::GetUpdateScript(int nObjID)
{
	CString strSQL = _T("");
	CString strUserData =_T("");
	CString strStructFace = _T("");
	strStructFace = FormatStructFace(m_vStructFace);
	strUserData = FormatUserData(m_vUserData);
	strSQL.Format(_T("UPDATE STRUCTUREPROP\
		SET PATH = %d,USERDATA = '%s',STRUCTFACE = '%s'\
		WHERE OBJID = %d"),
		m_nPathID,strUserData,strStructFace,nObjID);
	return strSQL;
}
CString Structure::GetDeleteScript(int nObjID) const
{
	CString strSQL = _T("");
	strSQL.Format(_T("DELETE FROM STRUCTUREPROP\
		WHERE OBJID = %d"),nObjID);
	return strSQL;
}


CString Structure::FormatStructFace(std::vector<StructFace*>&vStructFace)
{
	CString strStructFace = _T("");
	if (vStructFace.size()>0)
	{
		StructFace* pStructFace = NULL;
		pStructFace = vStructFace.at(0);
		strStructFace.Format(_T("%d,%s,%s"),(int)pStructFace->bShow,pStructFace->strFaceName,pStructFace->strPicPath);
		for (int i =1; i < (int)vStructFace.size();i++)
		{
			CString strTemp = _T("");
			pStructFace = vStructFace.at(i);
			strTemp.Format(_T("%d,%s,%s"),(int)pStructFace->bShow,pStructFace->strFaceName,pStructFace->strPicPath);
			strStructFace += "," + strTemp;
		}
	}
	return strStructFace;
}

CString Structure::FormatUserData(std::vector<UserInputData*>&vUserData)
{
	CString strUserData = _T("");
	if (vUserData.size()>0)
	{
		UserInputData* pUserData = NULL;
		pUserData = vUserData.at(0);
		strUserData.Format(_T("%.2f,%.2f,%.2f"),pUserData->dElevation,pUserData->dObstruction,pUserData->dStructHeight);
		for (int i = 1; i < (int)vUserData.size(); i++)
		{
			CString strTemp = _T("");
			pUserData = vUserData.at(i);
			strTemp.Format(_T("%.2f,%.2f,%.2f"),pUserData->dElevation,pUserData->dObstruction,pUserData->dStructHeight);
			strUserData += "," + strTemp;
		}
	}
	return strUserData;
}

void Structure::ConveyToStructFace(CString& strStructFace)
{
	StructFace* pStructFace = NULL;
	int nPos = strStructFace.Find(',');
	int nDes = 0;
	while (nPos != -1)
	{
		switch(nDes % FACE_SEL)
		{
		case 0:
			{
				pStructFace = new StructFace;
				pStructFace->bShow = (BOOL)atoi(strStructFace.Left(nPos));
			}
			break;
		case 1:
			{
				pStructFace->strFaceName = strStructFace.Left(nPos);
			}
			break;
		case 2:
			{
				pStructFace->strPicPath = strStructFace.Left(nPos);
				m_vStructFace.push_back(pStructFace);
			}
		default:
			break;
		}
		strStructFace = strStructFace.Right(strStructFace.GetLength()-nPos-1);
		nPos = strStructFace.Find(',');
		nDes++;
	}
	// last structface
	pStructFace->strPicPath = strStructFace;
	m_vStructFace.push_back(pStructFace);
}

void Structure::ConveyToUserData(CString& strUserData)
{
	UserInputData* pUserData = NULL;
	int nPos = strUserData.Find(',');
	int nDes = 0;
	while (nPos != -1)
	{
		switch(nDes % USERDATA_SEL)
		{
		case 0:
			{
				pUserData = new UserInputData;
				pUserData->dElevation = atof(strUserData.Left(nPos));
			}
			break;
		case 1:
			{
				pUserData->dObstruction = atof(strUserData.Left(nPos));
			}
			break;
		case 2:
			{
				pUserData->dStructHeight = atof(strUserData.Left(nPos));
				m_vUserData.push_back(pUserData);
			}
			break;
		default:
			break;
		}
		strUserData = strUserData.Right(strUserData.GetLength()-nPos-1);
		nPos = strUserData.Find(',');
		nDes++;
	}
	//last userdata
	pUserData->dStructHeight = atof(strUserData);
	m_vUserData.push_back(pUserData);
}

void Structure::UpdatePath()
{
	CADODatabase::UpdatePath2008InDatabase(m_path,m_nPathID);
}

ALTObject * Structure::NewCopy()
{
	Structure*pNewCopy = new Structure();
	pNewCopy->CopyData(this);
	return pNewCopy;
}

Structure& Structure::operator=( const Structure& otherStruct )
{
	DeepCopy(otherStruct);
	return *this;
}

bool Structure::CopyData( const ALTObject* pObj )
{
	if (this == pObj)
		return true;

	if(pObj->GetType() == GetType() )
	{
		Structure* pOtherStruct = (Structure*)pObj;
// 		m_path = pOtherStruct->m_path;
// 		m_nPathID = pOtherStruct->m_nPathID;
// 		m_vStructFace = pOtherStruct->m_vStructFace;
// 		m_vUserData = pOtherStruct->m_vUserData;
		DeepCopy(*pOtherStruct);

		m_bLocked = pOtherStruct->GetLocked();
		return true;
	}
	return false;
}

void Structure::clear()
{
	std::vector<UserInputData*>::iterator iter = m_vUserData.begin();
	for (; iter != m_vUserData.end(); ++iter)
	{
		delete *iter;
	}


	std::vector<StructFace*>::iterator it = m_vStructFace.begin();
	for (; it != m_vStructFace.end(); ++it)
	{
		delete *it;
	}

	m_vStructFace.clear();
	m_vUserData.clear();
}

void Structure::DeepCopy( const Structure& otherStruct )
{
	if(this == &otherStruct)
		return;

	clear();
	m_path = otherStruct.m_path;
	for(size_t i=0;i<otherStruct.m_vStructFace.size(); ++i)
	{
		StructFace*pFace =  new StructFace();
		*pFace  = *otherStruct.m_vStructFace[i];
		m_vStructFace.push_back(pFace);
	}
	for(size_t i=0;i<otherStruct.m_vUserData.size(); ++i)
	{
		UserInputData*pFace =  new UserInputData();
		*pFace  = *otherStruct.m_vUserData[i];
		m_vUserData.push_back(pFace);
	}

}

const GUID& Structure::getTypeGUID()
{
	// {24B783A0-0025-4a79-B786-EB2D38FD08E4}
	static const GUID name = 
	{ 0x24b783a0, 0x25, 0x4a79, { 0xb7, 0x86, 0xeb, 0x2d, 0x38, 0xfd, 0x8, 0xe4 } };

	return name;
}

ALTObjectDisplayProp * Structure::NewDisplayProp()
{
	return new StructureDisplayProp();
}



StructFace::StructFace() :bShow(TRUE)
	,strFaceName(_T("TEXT"))
	,strPicPath(_T("PICTURE"))
{

}
