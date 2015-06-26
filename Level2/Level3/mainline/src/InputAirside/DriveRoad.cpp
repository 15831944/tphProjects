#include "StdAfx.h"
#include ".\driveroad.h"

#include "../common/ARCUnit.h"
#include "../Common/pollygon2008.h"
#include "Intersections.h"
#include "../InputAirside/IntersectionNode.h"

DriveRoad::DriveRoad(int nParentID)
:m_nParentID(nParentID)
{
	m_dLaneWidth = 400;
	m_nLaneNum = 1;
	m_nPathID  = -1;
	m_enumDirection = IN_Dir;
	m_path.clear();
}

DriveRoad::~DriveRoad(void)
{
}

void DriveRoad::ReadObject(int nObjID)
{
	m_nObjID = nObjID;

	CADORecordset adoRecordset;
	long nRecordCount = -1;

	CADODatabase::ExecuteSQLStatement(ALTObject::GetSelectScript(nObjID),nRecordCount,adoRecordset);
	if (!adoRecordset.IsEOF())
		ALTObject::ReadObject(adoRecordset);

	CADODatabase::ExecuteSQLStatement(GetSelectScript(nObjID),nRecordCount,adoRecordset);
	if (!adoRecordset.IsEOF())
	{	
		adoRecordset.GetFieldValue(_T("PATHID"),m_nPathID );
		m_path.clear();
		//CADODatabase::ReadPathFromDatabase(m_nPathID,m_path);
		CADODatabase::ReadPath2008FromDatabase(m_nPathID,m_path);

		int nDir = -1;
		adoRecordset.GetFieldValue(_T("DIRECTION"),nDir);
		m_enumDirection = nDir ==0? IN_Dir: OUT_Dir;

		adoRecordset.GetFieldValue(_T("LANENUMBER"),m_nLaneNum); 

		adoRecordset.GetFieldValue(_T("LANEWIDTH"),m_dLaneWidth);
		m_dLaneWidth = ARCUnit::ConvertLength(m_dLaneWidth,ARCUnit::CM,ARCUnit::M);

	}
}

int DriveRoad::SaveObject(int nAirportID)
{
	int nObjID = ALTObject::SaveObject(nAirportID,ALT_DRIVEROAD);

	m_nPathID = CADODatabase::SavePath2008IntoDatabase(m_path);

	CString strSQL = GetInsertScript(nObjID);

	CADODatabase::ExecuteSQLStatement(strSQL);

	return nObjID;
}
void DriveRoad::UpdateObject(int nObjID)
{
	ALTObject::UpdateObject(nObjID);

	CADODatabase::UpdatePath2008InDatabase(m_path,m_nPathID);
	CString strUpdateScript = GetUpdateScript(nObjID);
	CADODatabase::ExecuteSQLStatement(strUpdateScript);
}

void DriveRoad::DeleteObject(int nObjID)
{
	ALTObject::DeleteObject(nObjID);

	CADODatabase::DeletePathFromDatabase(m_nPathID);
	CString strDeleteScript = GetDeleteScript(nObjID);

	CADODatabase::ExecuteSQLStatement(strDeleteScript);
}

CString DriveRoad::GetInsertScript(int nObjID) const
{
	CString strSQL = _T("");
	strSQL.Format(_T("INSERT INTO DRIVEROAD (OBJID, PATHID, DIRECTION, LANENUMBER, LANEWIDTH)  \
					 VALUES (%d,%d,%d)"), \
					 nObjID, m_nPathID, (int)m_enumDirection, m_nLaneNum, (int)ARCUnit::ConvertLength(m_dLaneWidth,ARCUnit::M,ARCUnit::CM));

	return strSQL;
}
CString DriveRoad::GetUpdateScript(int nObjID) const
{
	CString strSQL = _T("");
	strSQL.Format(_T("UPDATE DRIVEROAD SET PATHID = %d, DIRECTION= %d, LANENUMBER = %d,LANEWIDTH = %d  \
					 WHERE OBJID = %d"),\
					 m_nPathID, (int)m_enumDirection, m_nLaneNum, (int)ARCUnit::ConvertLength(m_dLaneWidth,ARCUnit::M,ARCUnit::CM), nObjID);

	return strSQL;
}
CString DriveRoad::GetDeleteScript(int nObjID) const
{
	CString strSQL = _T("");
	strSQL.Format(_T("DELETE FROM DRIVEROAD WHERE OBJID = %d"),nObjID);

	return strSQL;
}
CString DriveRoad::GetSelectScript(int nObjID) const
{
	CString strSQL = _T("");
	strSQL.Format(_T("SELECT * FROM DRIVEROAD WHERE OBJID = %d "),
		nObjID,m_nParentID);

	return strSQL;
}
const CPath2008& DriveRoad::GetPath()const
{
	return m_path;
}

void DriveRoad::SetPath(const CPath2008& _path)
{
	m_path.clear();
	m_path = _path;
}
double DriveRoad::GetLaneWidth(void)
{
	return m_dLaneWidth;
}
void DriveRoad::SetLaneWidth(double dLaneWidth)
{
	m_dLaneWidth = dLaneWidth;
}
int DriveRoad::GetLaneNumber(void)
{
	return m_nLaneNum;
}
void DriveRoad::SetLaneNumber(int nLaneNum)
{
	m_nLaneNum = nLaneNum;
}

ALTObject * DriveRoad::NewCopy()
{
	DriveRoad * pStretch  = new DriveRoad(m_nParentID);
	*pStretch = *this;
	return pStretch;
}

RoadDirection DriveRoad::GetRoadDir()
{
	return m_enumDirection;
}

void DriveRoad::SetRoadDir(RoadDirection _direction)
{
	m_enumDirection = _direction;
}

std::vector<IntersectionNode> DriveRoad::GetIntersectNodes() const
{
	std::vector<IntersectionNode> vIntersectNode;
	IntersectionNode::ReadIntersecNodeList(m_nObjID, vIntersectNode);
	return vIntersectNode;
}

void DriveRoad::UpdatePath()
{
	CADODatabase::UpdatePath2008InDatabase(m_path,m_nPathID);
}

bool DriveRoad::CopyData(const  ALTObject* pObj )
{
	if (this == pObj)
		return true;

	if( pObj->GetType() == GetType() )
	{
		DriveRoad * pOtherStretch = (DriveRoad*)pObj;
		m_path = pOtherStretch->GetPath();
		m_dLaneWidth = pOtherStretch->GetLaneWidth();
		m_nLaneNum = pOtherStretch->GetLaneNumber();

		m_bLocked = pOtherStretch->GetLocked();
		return true;
	}
	return false;
}


/////////////////////////////////////////DriveRoadList////////////////////////////////
DriveRoadList::DriveRoadList(int nParentID)
{
	m_nParentID = nParentID;
	m_vDelDriveRoadList.clear();
	m_vDriveRoadList.clear();
}

DriveRoadList::~DriveRoadList()
{

}

void DriveRoadList::ClearList()
{
	int nCount = (int)m_vDriveRoadList.size();
	for (int i =0; i < nCount; i++)
	{
		DriveRoad* pRoad = m_vDriveRoadList.at(i);
		if (pRoad)
		{
			delete pRoad;
			pRoad = NULL;
		}
	}
	m_vDriveRoadList.clear();

	nCount = (int)m_vDelDriveRoadList.size();
	for (int i =0; i < nCount; i++)
	{
		DriveRoad* pRoad = m_vDelDriveRoadList.at(i);
		if (pRoad)
		{
			delete pRoad;
			pRoad = NULL;
		}
	}
	m_vDelDriveRoadList.clear();
}

void DriveRoadList::AddDriveRoad(DriveRoad* pRoad)
{
	if (pRoad)
		m_vDriveRoadList.push_back(pRoad);
}

int DriveRoadList::GetRoadCount()
{
	return (int)m_vDriveRoadList.size();
}

DriveRoad* DriveRoadList::GetRoad(int nIdx)
{
	if (nIdx >=0 && nIdx < GetRoadCount())
	{
		return m_vDriveRoadList.at(nIdx);
	}
	return NULL;
}

void DriveRoadList::DelDriveRoad(DriveRoad* pRoad)
{
	if (pRoad == NULL)
		return;

	int nCount = (int)m_vDriveRoadList.size();
	for (int i =0; i < nCount; i++)
	{
		if (pRoad == m_vDriveRoadList.at(i))
		{
			m_vDelDriveRoadList.push_back(pRoad);
			m_vDriveRoadList.erase(m_vDriveRoadList.begin()+i);
			return;
		}
	}

}

void DriveRoadList::ReadDriveRoadList()
{
	std::vector<int> vObjID;
	CString strSQL = _T("");
	strSQL.Format(_T("SELECT * FROM OBJ_DEPENDENCE WHERE PAR_OBJID = %d AND OBJ_TYPE = %d"),m_nParentID, (int)ALT_DRIVEROAD);

	long nRecordCount = -1;
	CADORecordset adoRecordset;
	CADODatabase::ExecuteSQLStatement(strSQL, nRecordCount, adoRecordset);

	while (!adoRecordset.IsEOF())
	{
		int nObjID = -1;
		adoRecordset.GetFieldValue(_T("OBJID"),nObjID);
		vObjID.push_back(nObjID);
		adoRecordset.MoveNextData();
	} 

	int nCount = (int)vObjID.size();
	for (int i =0; i < nCount; i++)
	{
		int nObjID = vObjID.at(i);
		DriveRoad* pData = new DriveRoad(m_nParentID);
		pData->ReadObject(nObjID);
		m_vDriveRoadList.push_back(pData);
	}

}

void DriveRoadList::SaveDriveRoadList(int nAirportID)
{
	CString strSQL = _T("");
	strSQL.Format(_T("DELETE * FROM OBJ_DEPENDENCE WHERE PAR_OBJID = %d AND OBJ_TYPE = %d"),m_nParentID, (int)ALT_DRIVEROAD);
	CADODatabase::ExecuteSQLStatement(strSQL);

	int nCount = (int)m_vDelDriveRoadList.size();
	for (int i =0; i < nCount; i++)
	{
		DriveRoad* pData = m_vDelDriveRoadList.at(i);
		pData->DeleteObject(pData->getID());
	}

	std::vector<int> vObjID;
	nCount =(int) m_vDriveRoadList.size();
	for (int i =0; i < nCount; i++)
	{
		DriveRoad* pData = m_vDriveRoadList.at(i);
		int nObjID = pData->getID();
		if (nObjID < 0)
			nObjID = pData->SaveObject(nAirportID);
		else
			pData->UpdateObject(nObjID);

		vObjID.push_back(nObjID);
	}

	nCount =(int) vObjID.size();
	for (int i =0; i < nCount; i++)
	{
		strSQL.Format(("INSERT INTO OBJ_DEPENDENCE \
					   (OBJID, PAR_OBJID, OBJ_TYPE) \
					   VALUES (%d,%d,%d)"),vObjID.at(i),m_nParentID,(int)ALT_DRIVEROAD);

		CADODatabase::ExecuteSQLStatement(strSQL);
	}

}

void DriveRoadList::DeleteDriveRoadList()
{
	CString strSQL = _T("");
	strSQL.Format(_T("DELETE * FROM OBJ_DEPENDENCE WHERE PAR_OBJID = %d AND OBJ_TYPE = %d"),m_nParentID, (int)ALT_DRIVEROAD);

	CADODatabase::ExecuteSQLStatement(strSQL);

	int nCount = (int)m_vDriveRoadList.size();
	for (int i =0; i < nCount; i++)
	{
		DriveRoad* pData = m_vDriveRoadList.at(i);
		pData->DeleteObject(pData->getID());
	}

	nCount = (int)m_vDelDriveRoadList.size();
	for (int i =0; i < nCount; i++)
	{
		DriveRoad* pData = m_vDelDriveRoadList.at(i);
		pData->DeleteObject(pData->getID());
	}

}

void DriveRoadList::CopyData(DriveRoadList& otherRoadList)
{
	ClearList();
	int nCount = otherRoadList.GetRoadCount();
	m_vDriveRoadList.resize(nCount);
	for(int i= 0;i< nCount;i++)
	{
		DriveRoad* pData = new DriveRoad(m_nParentID);
		DriveRoad* pOtherData = otherRoadList.GetRoad(i) ;
		*pData = *pOtherData;
		m_vDriveRoadList.push_back(pData);
	}
}