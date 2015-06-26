#include "StdAfx.h"
#include ".\StartPosition.h"

CStartPosition::CStartPosition(void)
: m_nTaxiwayID(-1), m_fTaxiwayDivision(0.5), m_fRadius(50), m_strMarking(_T("24"))
{
}

CStartPosition::~CStartPosition(void)
{
}

CStartPosition::CStartPosition(const CStartPosition& sp)
: ALTObject(sp), m_nTaxiwayID(sp.m_nTaxiwayID), m_fTaxiwayDivision(sp.m_fTaxiwayDivision),
m_fRadius(sp.m_fRadius), m_strMarking(sp.m_strMarking)
{
}


void CStartPosition::SetMarking(const std::string& marking)
{
	m_strMarking = marking;
}
const std::string& CStartPosition::GetMarking() const
{
	return m_strMarking;
}
ALTObject* CStartPosition::NewCopy(void)
{
	return new CStartPosition(*this);
}
bool CStartPosition::CopyData(const ALTObject* pObj)
{
	ASSERT(pObj);
	if (this == pObj)
	{
		return true;
	}
	if (GetType() == pObj->GetType())
	{
		CStartPosition* pSp = (CStartPosition*)pObj;
		m_nTaxiwayID = pSp->m_nTaxiwayID;
		m_fTaxiwayDivision = pSp->m_fTaxiwayDivision;
		m_fRadius = pSp->m_fRadius;
		m_strMarking = pSp->m_strMarking;

		m_bLocked = pSp->GetLocked();
		return true;
	}
	return false;
}

void CStartPosition::ReadObject(int nObjID)
{
	m_nObjID = nObjID;

	CADORecordset adoRecordset;
	long nRecordCount = -1;
	CADODatabase::ExecuteSQLStatement(GetSelectScript(nObjID),nRecordCount,adoRecordset);

	if (!adoRecordset.IsEOF())
	{	
		ReadRecord(adoRecordset);
	}
}
int CStartPosition::SaveObject(int nAirportID)
{
	int nObjID = ALTObject::SaveObject(nAirportID,ALT_STARTPOSITION);

	CString strSQL = GetInsertScript(nObjID);
	CADODatabase::ExecuteSQLStatement(strSQL);

	return nObjID;
}

void CStartPosition::UpdateObject(int nObjID)
{
	ALTObject::UpdateObject(nObjID);

	// To do: add our own update
	CString strUpdateScript = GetUpdateScript(nObjID);
	CADODatabase::ExecuteSQLStatement(strUpdateScript);

}

void CStartPosition::DeleteObject(int nObjID)
{	
	// must delete ALTObject first, to test whether this Object can be deleted!
	ALTObject::DeleteObject(nObjID);

	// To do: add our own delete
	CString strDeleteScript = GetDeleteScript(nObjID);
	CADODatabase::ExecuteSQLStatement(strDeleteScript);

}

void CStartPosition::SetTaxiwayID(int taxiwayID)
{
	m_nTaxiwayID = taxiwayID;
}
int  CStartPosition::GetTaxiwayID() const
{
	return m_nTaxiwayID;
}

void CStartPosition::SetTaxiwayDivision(DistanceUnit fDivision)
{
	m_fTaxiwayDivision = fDivision;
}
DistanceUnit CStartPosition::GetTaxiwayDivision()
{
	return m_fTaxiwayDivision;
}
void CStartPosition::SetRadius(DistanceUnit radius)
{
	m_fRadius = radius;
}
DistanceUnit CStartPosition::GetRadius()
{
	return m_fRadius;
}
bool CStartPosition::GetServicePoint(CPoint2008& pt)
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


CString CStartPosition::GetSelectScript(int nObjID) const
{
	CString strSQL = _T("");
	strSQL.Format(_T("SELECT APTID, NAME_L1, NAME_L2, NAME_L3, NAME_L4, LOCKED, \
					 TAXIWAY_ID, DIVISION, RADIUS, MARKING \
					 FROM ALTOBJECT, STARTPOSITION_PROP \
					 WHERE ALTOBJECT.ID =  %d \
					 AND ALTOBJECT.ID = STARTPOSITION_PROP.OBJID"),
					 nObjID);

	return strSQL;
}
//CString CStartPosition::GetSelectScriptForPath2008(int nObjID) const
//{
//
//}
CString CStartPosition::GetInsertScript(int nObjID) const
{
	CString strSQL = _T("");
	strSQL.Format(_T("INSERT INTO STARTPOSITION_PROP(OBJID,TAXIWAY_ID,DIVISION,RADIUS,MARKING) \
					 VALUES (%d,%d,%f,%f,'%s')"),
					 nObjID,m_nTaxiwayID, m_fTaxiwayDivision, m_fRadius, m_strMarking.c_str());

	return strSQL;
}
CString CStartPosition::GetUpdateScript(int nObjID) const
{
	CString strSQL = _T("");
	strSQL.Format(_T("UPDATE STARTPOSITION_PROP	SET TAXIWAY_ID = %d, DIVISION = %f, RADIUS = %f, MARKING = '%s' \
					 WHERE OBJID = %d"),
					 m_nTaxiwayID, m_fTaxiwayDivision, m_fRadius, m_strMarking.c_str(), 
					 nObjID);

	return strSQL;
}
CString CStartPosition::GetDeleteScript(int nObjID) const
{
	CString strSQL = _T("");
	strSQL.Format(_T("DELETE FROM STARTPOSITION_PROP WHERE OBJID = %d"),nObjID);

	return strSQL;
}
// load data from SQL-result to member variables
void CStartPosition::ReadRecord(CADORecordset& adoRecordset)
{
	ALTObject::ReadObject(adoRecordset);

	adoRecordset.GetFieldValue(_T("TAXIWAY_ID"),m_nTaxiwayID);
	adoRecordset.GetFieldValue(_T("DIVISION"),m_fTaxiwayDivision);
	adoRecordset.GetFieldValue(_T("RADIUS"),m_fRadius);
	adoRecordset.GetFieldValue(_T("MARKING"),m_strMarking);

}


void CStartPosition::getMarkingPos( const CPath2008& path, ARCPoint3& pos, double& dAngle, double& dScale )
{
	int markCnt  = (int)m_strMarking.size();
	double dsquareRat = markCnt* 1.4 /2.0;   //square width/height;
	ARCVector3 vDir = path.GetIndexDir(GetTaxiwayDivision());
	ARCVector2 vdir = vDir.xy();
	dAngle = vdir.AngleFromCoorndinateX()-90;
	vdir.Rotate(-90 - ARCMath::RadiansToDegrees(atan(1.0/dsquareRat)) );//
	vdir.SetLength(GetRadius());
	ARCPoint3 pt = path.GetIndexPoint((float)GetTaxiwayDivision());
	pos = pt + ARCPoint3(vdir[VX],vdir[VY],0);
    dScale = GetRadius()*2.0*1.4/sqrt(1+dsquareRat*dsquareRat);

}

const GUID& CStartPosition::getTypeGUID()
{
	// {E9DCB204-D89C-47f3-93D7-A6C52DC9370E}
	static const GUID name = 
	{ 0xe9dcb204, 0xd89c, 0x47f3, { 0x93, 0xd7, 0xa6, 0xc5, 0x2d, 0xc9, 0x37, 0xe } };
	
	return name;
}