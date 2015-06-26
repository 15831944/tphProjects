#include "StdAfx.h"
#include ".\parkingplace.h"
#include "Common\ARCVector.h"
#include "Common\ARCMathCommon.h"

ParkingPlace::ParkingPlace(int nParentID)
:m_nParentID(nParentID)
,m_nPathID(-1)
{
}

ParkingPlace::~ParkingPlace(void)
{
	m_SpotLine.clear();
}

void ParkingPlace::SetSpotLine(const CPath2008& _path)
{
	m_SpotLine = _path;
}

const CPath2008& ParkingPlace::GetSpotLine() const
{
	return m_SpotLine;
}

void ParkingPlace::SetSpotLength(DistanceUnit dSpotLength)
{
	m_dSpotLength = dSpotLength;
}

DistanceUnit ParkingPlace::GetSpotLength()
{
	return m_dSpotLength;
}

void ParkingPlace::SetSpotWidth(DistanceUnit dSpotWidth)
{
	m_dSpotWidth = dSpotWidth;
}

DistanceUnit ParkingPlace::GetSpotWidth()
{
	return m_dSpotWidth;
}

void ParkingPlace::SetSpotAngle(int nSpotAngle)
{
	m_nSpotAngle = nSpotAngle;
}

int ParkingPlace::GetSpotAngle()
{
	return m_nSpotAngle;
}

void ParkingPlace::ReadObject(int nObjID)
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
		adoRecordset.GetFieldValue(_T("SPOTSLINEID"),m_nPathID);
		m_SpotLine.clear();
		CADODatabase::ReadPath2008FromDatabase(m_nPathID,m_SpotLine);
	}

}
int ParkingPlace::SaveObject(int nAirportID)
{
	int nObjID = ALTObject::SaveObject(nAirportID,ALT_PARKINGPLACE);
	
	m_nPathID = CADODatabase::SavePath2008IntoDatabase(m_SpotLine);

	CADODatabase::ExecuteSQLStatement(GetInsertScript(nObjID));

	return nObjID;

}

void ParkingPlace::UpdateObject(int nObjID)
{

	ALTObject::UpdateObject(nObjID);
	CADODatabase::UpdatePath2008InDatabase(m_SpotLine,m_nPathID);
	CADODatabase::ExecuteSQLStatement(GetUpdateScript(nObjID));
}

void ParkingPlace::DeleteObject(int nObjID)
{

	ALTObject::DeleteObject(nObjID);
	CADODatabase::DeletePathFromDatabase(m_nPathID);
	CADODatabase::ExecuteSQLStatement(GetDeleteScript(nObjID));

}

CString ParkingPlace::GetSelectScript(int nObjID) const
{
	CString strSQL = _T("");
	strSQL.Format(_T("SELECT * FROM PARKINGPLACE WHERE OBJID = %d "),nObjID);

	return strSQL;
}

CString ParkingPlace::GetInsertScript(int nObjID) const
{
	CString strSQL = _T("");

	strSQL.Format(_T("INSERT INTO PARKINGPLACE(OBJID,SPOTSLINEID,SPOTLENGTH,SPOTWIDTH,SPOTANGLE,PARKINGTYPE) \
					 VALUES (%d,%d,%d,%d,%d,%d)"),\
					 nObjID,m_nPathID,(int)m_dSpotLength,(int)m_dSpotWidth,m_nSpotAngle,(int)m_enumParkingType);

	return strSQL;


}
CString ParkingPlace::GetUpdateScript(int nObjID) const
{
	CString strSQL = _T("");

	strSQL.Format(_T("UPDATE PARKINGPLACE SET SPOTSLINEID =%d,SPOTLENGTH =%d, \
		SPOTWIDTH =%d,SPOTANGLE =%d,PARKINGTYPE =%d \
					 WHERE OBJID = %d"),
					 nObjID,m_nPathID,(int)m_dSpotLength,(int)m_dSpotWidth,m_nSpotAngle,(int)m_enumParkingType);

	return strSQL;
}
CString ParkingPlace::GetDeleteScript(int nObjID) const
{
	CString strSQL = _T("");

	strSQL.Format(_T("DELETE FROM PARKINGPLACE	WHERE OBJID = %d"),nObjID);

	return strSQL;

}

ALTObject * ParkingPlace::NewCopy()
{
	ParkingPlace * reslt = new ParkingPlace(m_nParentID);
	*reslt = *this;

	return reslt;
}

void ParkingPlace::OnRotate( DistanceUnit dx,const ARCVector3& vCenter )
{
	m_SpotLine.DoOffset(-vCenter[VX],-vCenter[VY],-vCenter[VZ]);
	m_SpotLine.Rotate(dx);
	m_SpotLine.DoOffset(vCenter[VX],vCenter[VY],vCenter[VZ]);
}

void ParkingPlace::DoOffset( DistanceUnit dx, DistanceUnit dy, DistanceUnit dz )
{
	m_SpotLine.DoOffset(dx,dy,dz);
}

bool ParkingPlace::CopyData(const  ALTObject* pObj )
{
	if (this == pObj)
		return true;

	if( pObj->GetType() == GetType() )
	{
		ParkingPlace * otherParkingPlace = (ParkingPlace*)pObj;
		m_SpotLine = otherParkingPlace->GetSpotLine();
		m_dSpotLength = otherParkingPlace->GetSpotLength();
		m_dSpotWidth = otherParkingPlace->GetSpotWidth();
		m_nSpotAngle = otherParkingPlace->GetSpotAngle();

		m_bLocked = otherParkingPlace->GetLocked();
		return true;
	}
	ASSERT(FALSE);
	return false;
}

void ParkingPlace::SetParkingType(ParkingType _type)
{
	m_enumParkingType = _type;
}

ParkingType ParkingPlace::GetParingType()
{
	return m_enumParkingType;
}

///////////////////////////////////ParkingPlaceList/////////////////////////////////////
ParkingPlaceList::ParkingPlaceList(int nParentID)
{
	m_nParentID = nParentID;
	m_vDelParkingPlaceList.clear();
	m_vParkingPlaceList.clear();
}

ParkingPlaceList::~ParkingPlaceList()
{
	ClearList();
}

void ParkingPlaceList::ClearList()
{
	int nCount = (int) m_vParkingPlaceList.size();
	for (int i =0; i < nCount; i++)
	{
		ParkingPlace* pParking = m_vParkingPlaceList.at(i);
		if (pParking)
		{
			delete pParking;
			pParking = NULL;
		}
	}
	m_vParkingPlaceList.clear();

	nCount = (int)m_vDelParkingPlaceList.size();
	for (int i =0; i < nCount; i++)
	{
		ParkingPlace* pParking = m_vDelParkingPlaceList.at(i);
		if (pParking)
		{
			delete pParking;
			pParking = NULL;
		}
	}
	m_vDelParkingPlaceList.clear();
}

void ParkingPlaceList::AddParkingPlace(ParkingPlace* pParkingPlace)
{
	if (pParkingPlace)
		m_vParkingPlaceList.push_back(pParkingPlace);
}

int ParkingPlaceList::GetParkingPlaceCount()
{
	return (int)m_vParkingPlaceList.size();
}

ParkingPlace* ParkingPlaceList::GetParkingPlace(int nIdx)
{
	if (nIdx >=0 && nIdx < GetParkingPlaceCount())
	{
		return m_vParkingPlaceList.at(nIdx);
	}

	return NULL;
}

void ParkingPlaceList::DelParkingPlace(ParkingPlace* pParkingPlace)
{
	if (pParkingPlace == NULL)
		return;

	int nCount = (int)m_vParkingPlaceList.size();
	for (int i =0; i < nCount; i++)
	{
		if (pParkingPlace == m_vParkingPlaceList.at(i))
		{
			m_vDelParkingPlaceList.push_back(pParkingPlace);
			m_vParkingPlaceList.erase(m_vParkingPlaceList.begin()+ i);
			return;
		}
	}

}

void ParkingPlaceList::ReadParkingPlaceList()
{
	std::vector<int> vObjID;
	CString strSQL = _T("");
	strSQL.Format(_T("SELECT * FROM OBJ_DEPENDENCE WHERE PAR_OBJID = %d AND OBJ_TYPE = %d"),m_nParentID, (int)ALT_PARKINGPLACE);

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
		ParkingPlace* pData = new ParkingPlace(m_nParentID);
		pData->ReadObject(nObjID);
		m_vParkingPlaceList.push_back(pData);
	}

}

void ParkingPlaceList::SaveParkingPlaceList(int nAirportID)
{
	CString strSQL = _T("");
	strSQL.Format(_T("DELETE * FROM OBJ_DEPENDENCE WHERE PAR_OBJID = %d AND OBJ_TYPE = %d"),m_nParentID, (int)ALT_PARKINGPLACE);
	CADODatabase::ExecuteSQLStatement(strSQL);



	std::vector<int> vObjID;
	int nCount = (int)m_vParkingPlaceList.size();
	for (int i =0; i < nCount; i++)
	{
		ParkingPlace* pData = m_vParkingPlaceList.at(i);
		int nObjID = pData->getID();
		if (nObjID < 0)
			nObjID = pData->SaveObject(nAirportID);
		else
			pData->UpdateObject(nObjID);

		vObjID.push_back(nObjID);
	}

	nCount = (int)m_vDelParkingPlaceList.size();
	for (int i =0; i < nCount; i++)
	{
		ParkingPlace* pData = m_vDelParkingPlaceList.at(i);
		pData->DeleteObject(pData->getID());
	}

	nCount = (int)vObjID.size();
	for (int i =0; i < nCount; i++)
	{
		strSQL.Format(("INSERT INTO OBJ_DEPENDENCE \
			(OBJID, PAR_OBJID, OBJ_TYPE) \
			VALUES (%d,%d,%d)"),vObjID.at(i),m_nParentID,(int)ALT_PARKINGPLACE);

		CADODatabase::ExecuteSQLStatement(strSQL);
	}

}

void ParkingPlaceList::DeleteParkingPlaceList()
{
	CString strSQL = _T("");
	strSQL.Format(_T("DELETE * FROM OBJ_DEPENDENCE WHERE PAR_OBJID = %d AND OBJ_TYPE = %d"),m_nParentID, (int)ALT_PARKINGPLACE);

	CADODatabase::ExecuteSQLStatement(strSQL);

	int nCount = (int)m_vParkingPlaceList.size();
	for (int i =0; i < nCount; i++)
	{
		ParkingPlace* pData = m_vParkingPlaceList.at(i);
		pData->DeleteObject(pData->getID());
	}

	nCount = (int)m_vDelParkingPlaceList.size();
	for (int i =0; i < nCount; i++)
	{
		ParkingPlace* pData = m_vDelParkingPlaceList.at(i);
		pData->DeleteObject(pData->getID());
	}
	
}

void ParkingPlaceList::CopyData(ParkingPlaceList& pOtherParkingList)
{
	ClearList();
	int nCount = pOtherParkingList.GetParkingPlaceCount();
	m_vParkingPlaceList.resize(nCount);
	for(int i= 0;i< nCount;i++)
	{
		ParkingPlace* pData = new ParkingPlace(m_nParentID);
		ParkingPlace* pOtherData = pOtherParkingList.GetParkingPlace(i) ;
		*pData = *pOtherData;
		m_vParkingPlaceList.push_back(pData);
	}
}