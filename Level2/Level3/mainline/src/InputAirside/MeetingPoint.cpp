#include "StdAfx.h"
#include ".\MeetingPoint.h"

CMeetingPoint::CMeetingPoint(void)
	: m_nTaxiwayID(-1)
	, m_fTaxiwayDivision(0.5)
	, m_fRadius(50)
	, m_strMarking(_T("M"))
	,m_PathID(-1)
{
}

CMeetingPoint::~CMeetingPoint(void)
{
}

CMeetingPoint::CMeetingPoint(const CMeetingPoint& sp)
	: ALTObject(sp)
	, m_nTaxiwayID(sp.m_nTaxiwayID)
	, m_fTaxiwayDivision(sp.m_fTaxiwayDivision)
	, m_fRadius(sp.m_fRadius)
	, m_strMarking(sp.m_strMarking)
{
}

void CMeetingPoint::SetPath( const CPath2008& _path )
{
	m_Path=_path;
}
void CMeetingPoint::SetMarking(const std::string& marking)
{
	m_strMarking = marking;
}
const std::string& CMeetingPoint::GetMarking() const
{
	return m_strMarking;
}
ALTObject* CMeetingPoint::NewCopy(void)
{
	return new CMeetingPoint(*this);
}
bool CMeetingPoint::CopyData(const ALTObject* pObj)
{
	ASSERT(pObj);
	if (this == pObj)
	{
		return true;
	}
	if (GetType() == pObj->GetType())
	{
		// ??? why not copy ALTObject
		CMeetingPoint* pMP = (CMeetingPoint*)pObj;
		m_nTaxiwayID = pMP->m_nTaxiwayID;
		m_fTaxiwayDivision = pMP->m_fTaxiwayDivision;
		m_fRadius = pMP->m_fRadius;
		m_strMarking = pMP->m_strMarking;
		m_Path=pMP->m_Path;
		m_bLocked = pMP->GetLocked();
		return true;
	}
	return false;
}

void CMeetingPoint::ReadObject(int nObjID)
{
	m_nObjID = nObjID;

	CADORecordset adoRecordset;
	long nRecordCount = -1;
	CADODatabase::ExecuteSQLStatement(GetSelectScript(nObjID),nRecordCount,adoRecordset);

	if (!adoRecordset.IsEOF())
	{	
		ReadObject(adoRecordset);
	}
	CADODatabase::ReadPath2008FromDatabase(m_PathID,m_Path) ;
}
int CMeetingPoint::SaveObject(int nAirportID)
{
	if(m_PathID == -1)
		m_PathID = CADODatabase::SavePath2008IntoDatabase(m_Path) ;
	else
		CADODatabase::UpdatePath2008InDatabase(m_Path,m_PathID) ;
	int nObjID = ALTObject::SaveObject(nAirportID,ALT_MEETINGPOINT);

	CString strSQL = GetInsertScript(nObjID);
	CADODatabase::ExecuteSQLStatement(strSQL);

	return nObjID;
}

void CMeetingPoint::UpdateObject(int nObjID)
{
	ALTObject::UpdateObject(nObjID);

	if(m_PathID == -1)
		m_PathID = CADODatabase::SavePath2008IntoDatabase(m_Path) ;
	else
		CADODatabase::UpdatePath2008InDatabase(m_Path,m_PathID) ;

	// To do: add our own update
	CString strUpdateScript = GetUpdateScript(nObjID);
	CADODatabase::ExecuteSQLStatement(strUpdateScript);

}

void CMeetingPoint::DeleteObject(int nObjID)
{	
	// must delete ALTObject first, to test whether this Object can be deleted!
	ALTObject::DeleteObject(nObjID);
	if(m_PathID != -1)
	{
		CADODatabase::DeletePath2008FromDatabase(m_PathID) ;
		m_PathID = -1 ;
	}
	// To do: add our own delete
	CString strDeleteScript = GetDeleteScript(nObjID);
	CADODatabase::ExecuteSQLStatement(strDeleteScript);

}

void CMeetingPoint::SetTaxiwayID(int taxiwayID)
{
	m_nTaxiwayID = taxiwayID;
}
int  CMeetingPoint::GetTaxiwayID() const
{
	return m_nTaxiwayID;
}

void CMeetingPoint::SetTaxiwayDivision(DistanceUnit fDivision)
{
	m_fTaxiwayDivision = fDivision;
}
DistanceUnit CMeetingPoint::GetTaxiwayDivision()
{
	return m_fTaxiwayDivision;
}
void CMeetingPoint::SetRadius(DistanceUnit radius)
{
	m_fRadius = radius;
}
DistanceUnit CMeetingPoint::GetRadius()
{
	return m_fRadius;
}
bool CMeetingPoint::GetServicePoint(CPoint2008& pt)
{
	if (-1 != m_nTaxiwayID)    // check id
	{
		ALTObject* pObject = NULL;
		// get the object globally
		pObject = ALTObject::ReadObjectByID(m_nTaxiwayID);  // look into
		if (pObject)
		{
			if (ALT_TAXIWAY == pObject->GetType())   //check type
			{
				Taxiway* pTaxiway = (Taxiway*)pObject;
				CPath2008 path = pTaxiway->GetPath();
				delete pObject;  // here don't forget to release the memory
				return path.GetIndexPoint((float)m_fTaxiwayDivision, pt);
			}
			delete pObject;
		}
	}

	return false;
}


CString CMeetingPoint::GetSelectScript(int nObjID) const
{
	CString strSQL = _T("");
	strSQL.Format(_T("SELECT APTID, NAME_L1, NAME_L2, NAME_L3, NAME_L4, LOCKED, \
					 TAXIWAY_ID, DIVISION, RADIUS, MARKING ,PATHID\
					 FROM ALTOBJECT, MEETINGPOINT_PROP \
					 WHERE ALTOBJECT.ID =  %d \
					 AND ALTOBJECT.ID = MEETINGPOINT_PROP.OBJID"),
					 nObjID);

	return strSQL;
}
//CString CMeetingPoint::GetSelectScriptForPath2008(int nObjID) const
//{
//
//}
CString CMeetingPoint::GetInsertScript(int nObjID) const
{
	CString strSQL = _T("");
	strSQL.Format(_T("INSERT INTO MEETINGPOINT_PROP(OBJID,TAXIWAY_ID,DIVISION,RADIUS,MARKING,PATHID) \
					 VALUES (%d,%d,%f,%f,'%s',%d)"),
					 nObjID,m_nTaxiwayID, m_fTaxiwayDivision, m_fRadius, m_strMarking.c_str(),m_PathID);

	return strSQL;
}
CString CMeetingPoint::GetUpdateScript(int nObjID) const
{
	CString strSQL = _T("");
	strSQL.Format(_T("UPDATE MEETINGPOINT_PROP	SET TAXIWAY_ID = %d, DIVISION = %f, RADIUS = %f, MARKING = '%s',PATHID = %d \
					 WHERE OBJID = %d"),
					 m_nTaxiwayID, m_fTaxiwayDivision, m_fRadius, m_strMarking.c_str(), m_PathID,
					 nObjID);

	return strSQL;
}
CString CMeetingPoint::GetDeleteScript(int nObjID) const
{
	CString strSQL = _T("");
	strSQL.Format(_T("DELETE FROM MEETINGPOINT_PROP WHERE OBJID = %d"),nObjID);

	return strSQL;
}
// load data from SQL-result to member variables
void CMeetingPoint::ReadObject(CADORecordset& adoRecordset)
{
	ALTObject::ReadObject(adoRecordset);

	adoRecordset.GetFieldValue(_T("TAXIWAY_ID"),m_nTaxiwayID);
	adoRecordset.GetFieldValue(_T("DIVISION"),m_fTaxiwayDivision);
	adoRecordset.GetFieldValue(_T("RADIUS"),m_fRadius);
	adoRecordset.GetFieldValue(_T("MARKING"),m_strMarking);
	adoRecordset.GetFieldValue(_T("PATHID"),m_PathID) ;

}


void CMeetingPoint::getMarkingPos( const CPath2008& path, ARCPoint3& pos, double& dAngle, double& dScale )
{
	int markCnt  = (int)m_strMarking.size();
	double dsquareRat = markCnt* 1.4 /2.0;   //square width/height;
	CPoint2008 vDir = CPoint2008(path.GetIndexDir((float)GetTaxiwayDivision()));
	ARCVector2 vdir(vDir.getX(),vDir.getY());
	dAngle = vdir.AngleFromCoorndinateX()-90;
	vdir.Rotate(-90 - ARCMath::RadiansToDegrees(atan(1.0/dsquareRat)) );//
	vdir.SetLength(GetRadius());
	ARCPoint3 pt = path.GetIndexPoint((float)GetTaxiwayDivision());
	pos = pt + ARCPoint3(vdir[VX],vdir[VY],0);
    dScale = GetRadius()*2.0*1.4/sqrt(1+dsquareRat*dsquareRat);

}

CPath2008* CMeetingPoint::GetPath()
{
	return &m_Path ;
}

const GUID& CMeetingPoint::getTypeGUID()
{
	// {5A70E6EE-DE42-4669-A8B4-500191F5BD29}
	static const GUID name = 
	{ 0x5a70e6ee, 0xde42, 0x4669, { 0xa8, 0xb4, 0x50, 0x1, 0x91, 0xf5, 0xbd, 0x29 } };
	return name;
}