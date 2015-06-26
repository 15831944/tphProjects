#include "StdAfx.h"
#include "Stretch.h"
#include "../common/ARCUnit.h"
#include "../Common/pollygon2008.h"
#include "AirsideImportExportManager.h"
#include "ALTAirport.h"
#include "Intersections.h"
#include "VehiclePoolParking.h"
#include "../InputAirside/IntersectionNode.h"
#include "ALTObjectDisplayProp.h"


Stretch::Stretch(void)
{
	m_dLaneWidth = 400;
	m_iLaneNum = 1;
	m_nPathID  = -1;
}

Stretch::Stretch( const Stretch& other)
{
	*this = other;
}
Stretch::~Stretch(void)
{
	m_path.clear();
}

void Stretch::ReadObject(int nObjID)
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

		adoRecordset.GetFieldValue(_T("LANEWIDTH"),m_dLaneWidth);
		m_dLaneWidth = ARCUnit::ConvertLength(m_dLaneWidth,ARCUnit::CM,ARCUnit::M);

		adoRecordset.GetFieldValue(_T("LANENUM"),m_iLaneNum); 
	}
}

int Stretch::SaveObject(int nAirportID)
{
	int nObjID = ALTObject::SaveObject(nAirportID,ALT_STRETCH);

	m_nPathID = CADODatabase::SavePath2008IntoDatabase(m_path);
	
	CString strSQL = GetInsertScript(nObjID);

	CADODatabase::ExecuteSQLStatement(strSQL);

	return nObjID;
}
void Stretch::UpdateObject(int nObjID)
{
	ALTObject::UpdateObject(nObjID);

	CADODatabase::UpdatePath2008InDatabase(m_path,m_nPathID);
	CString strUpdateScript = GetUpdateScript(nObjID);
	CADODatabase::ExecuteSQLStatement(strUpdateScript);
}

void Stretch::DeleteObject(int nObjID)
{
	ALTObject::DeleteObject(nObjID);
	
	CADODatabase::DeletePathFromDatabase(m_nPathID);
	CString strDeleteScript = GetDeleteScript(nObjID);

	CADODatabase::ExecuteSQLStatement(strDeleteScript);
}

CString Stretch::GetInsertScript(int nObjID) const
{
	CString strSQL = _T("");
	strSQL.Format(_T("INSERT INTO STRETCH (OBJID,LANEWIDTH , LANENUM, PATHID ) \
		VALUES (%d,%f,%d,%d)"), \
		nObjID,ARCUnit::ConvertLength(m_dLaneWidth,ARCUnit::M,ARCUnit::CM),m_iLaneNum,m_nPathID);

	return strSQL;
}
CString Stretch::GetUpdateScript(int nObjID) const
{
	CString strSQL = _T("");
	strSQL.Format(_T("UPDATE STRETCH SET LANEWIDTH = %f, LANENUM = %d, PATHID = %d \
		 WHERE OBJID = %d"),ARCUnit::ConvertLength(m_dLaneWidth,ARCUnit::M,ARCUnit::CM),m_iLaneNum,m_nPathID,nObjID);

	return strSQL;
}
CString Stretch::GetDeleteScript(int nObjID) const
{
	CString strSQL = _T("");
	strSQL.Format(_T("DELETE FROM STRETCH WHERE OBJID = %d"),nObjID);

	return strSQL;
}
CString Stretch::GetSelectScript(int nObjID) const
{
	CString strSQL = _T("");
	strSQL.Format(_T("SELECT * FROM STRETCH WHERE OBJID = %d"),
		nObjID);

	return strSQL;
}
const CPath2008& Stretch::GetPath()const
{
	return (m_path);
}

CPath2008& Stretch::getPath()
{
	return m_path;
}
void Stretch::SetPath(const CPath2008& _path)
{
	m_path.clear();
	m_path = _path;
}
double Stretch::GetLaneWidth(void)const
{
	return (m_dLaneWidth);
}
void Stretch::SetLaneWidth(double dLaneWidth)
{
	m_dLaneWidth = dLaneWidth;
}
int Stretch::GetLaneNumber(void)const
{
	return (m_iLaneNum);
}
void Stretch::SetLaneNumber(int iLaneNum)
{
	m_iLaneNum = iLaneNum;
}

void Stretch::ExportObject(CAirsideExportFile& exportFile)
{
	ALTObject::ExportObject(exportFile);
	exportFile.getFile().writeInt(m_nAptID);
	exportFile.getFile().writeDouble(ARCUnit::ConvertLength(m_dLaneWidth,ARCUnit::M,ARCUnit::CM));
	exportFile.getFile().writeInt(m_iLaneNum);	
	exportFile.getFile().writeInt(m_nPathID);

	//path
	int nPathCount = m_path.getCount();	
	exportFile.getFile().writeInt(nPathCount);
	for (int i = 0; i < nPathCount; i++)
	{
		exportFile.getFile().writePoint2008(m_path.getPoint(i));
	}

	exportFile.getFile().writeLine();
}

void Stretch::ExportStretch(int nAirportID,CAirsideExportFile& exportFile)
{
	std::vector<int> vStretchID;
	ALTAirport::GetStretchsIDs(nAirportID,vStretchID);
	int nStretchCount = static_cast<int>(vStretchID.size());
	for (int i =0; i < nStretchCount;++i)
	{
		Stretch stretchObj;
		stretchObj.ReadObject(vStretchID[i]);
		stretchObj.ExportObject(exportFile);
	}
}

void Stretch::ImportObject(CAirsideImportFile& importFile)
{
	ALTObject::ImportObject(importFile);
	int nOldAirportID = -1;
	importFile.getFile().getInteger(nOldAirportID);
	m_nAptID = importFile.GetAirportNewIndex(nOldAirportID);

	importFile.getFile().getFloat(m_dLaneWidth);
	m_dLaneWidth = ARCUnit::ConvertLength(m_dLaneWidth,ARCUnit::CM,ARCUnit::M);
	importFile.getFile().getInteger(m_iLaneNum);
	importFile.getFile().getInteger(m_nPathID);

	int nPointCount = 0;
	importFile.getFile().getInteger(nPointCount);
	m_path.init(nPointCount);
	CPoint2008 *pPoint =  m_path.getPointList();
	for (int i = 0; i < nPointCount; i++)
	{
		importFile.getFile().getPoint2008(pPoint[i]);
	}

	importFile.getFile().getLine();

	int nOldObjectID = m_nObjID;
	int nNewObjID = SaveObject(m_nAptID);
	importFile.AddObjectIndexMap(nOldObjectID,nNewObjID);
}

ALTObject * Stretch::NewCopy()
{
	Stretch * pStretch  = new Stretch;
	pStretch->CopyData(this);
	return pStretch;
}

int Stretch::GetLaneDir( int idx ) const
{
	if(idx < ( m_iLaneNum -1) * 0.5 )
	{
		return NegativeDirection;
	}
	else 
		return PositiveDirection; 
}

bool Stretch::IsAInLaneToIntersection( int laneNo, const Intersections& intersection ) const
{
	const InsecObjectPartVector& objPartsVector = intersection.GetIntersectionsPart();
	DistanceUnit strethDist = GetPath().GetTotalLength();
	for(int i=0;i<(int) objPartsVector.size();i++)
	{
		InsecObjectPart objPart = objPartsVector.at(i);
		if( objPart.GetObjectID() == getID() )
		{
			if ( objPart.pos * 2 > GetPath().getCount()-1 )
			{
				if( GetLaneDir(laneNo) == PositiveDirection ){
					return true;
				}
			}else
			{
				if( GetLaneDir(laneNo) == NegativeDirection ){
					return true;
				}
			}
			
		}
	}
	return false;
}

bool Stretch::IsAOutLaneFromIntersection( int laneNo, const Intersections& intersection ) const
{
	const InsecObjectPartVector& objPartsVector = intersection.GetIntersectionsPart();
	DistanceUnit strethDist = GetPath().GetTotalLength();
	for(int i=0;i<(int) objPartsVector.size();i++)
	{
		InsecObjectPart objPart = objPartsVector.at(i);
		if( objPart.GetObjectID() == getID() )
		{
			if ( objPart.pos * 2 > GetPath().getCount()-1 )
			{
				if( GetLaneDir(laneNo) == NegativeDirection ){
					return true;
				}
			}else
			{
				if( GetLaneDir(laneNo) == PositiveDirection ){
					return true;
				}
			}

		}
	}
	return false;
}

//bool Stretch::IsAInLaneToVehiclePool( int laneNo, const VehiclePoolParking& pool ) const
//{
//	ASSERT(GetPath().getCount());
//	CPollygon2008 poly;
//	poly.init(pool.GetPath().getCount(),pool.GetPath().getPointList() );
//	
//	if( poly.contains( GetPath().getPoint(0) ) )
//	{
//		if( GetLaneDir(laneNo)== NegativeDirection )
//		{
//			return true;
//		}
//
//	}
//
//	if( poly.contains( GetPath().getPoint(GetPath().getCount()-1) ) )
//	{
//		if(GetLaneDir(laneNo) == PositiveDirection )
//		{
//			return true; 
//		}
//	}
//	
//	return false;
//}
//
//bool Stretch::IsAOutLaneFromVehiclePool( int laneNo, const VehiclePoolParking& pool ) const
//{
//	ASSERT(GetPath().getCount());
//	CPath2008 poolPath = pool.GetPath();
//	for(int i=0;i<poolPath.getCount();i++)
//	{
//		poolPath[i].setZ(0);
//	}
//
//	CPollygon2008 poly;
//	poly.init(poolPath.getCount(),poolPath.getPointList() );
//
//	CPoint2008 pointBegin = GetPath().getPoint(0);pointBegin.setZ(0);
//	CPoint2008 pointEnd = GetPath().getPoint(GetPath().getCount()-1);pointEnd.setZ(0);
//
//	if( poly.contains( pointBegin ) )
//	{
//		if( GetLaneDir(laneNo)== PositiveDirection )
//		{
//			return true;
//		}
//
//	}
//
//	if( poly.contains( pointEnd ) )
//	{
//		if(GetLaneDir(laneNo) == NegativeDirection )
//		{
//			return true; 
//		}
//	}
//
//	return false;
//}

Stretch& Stretch::operator=( const Stretch& other)
{
	ALTObject::operator = (other);
	m_iLaneNum = other.m_iLaneNum;
	m_path = other.m_path;
	return *this;
}

std::vector<IntersectionNode> Stretch::GetIntersectNodes() const
{
	std::vector<IntersectionNode> vIntersectNode;
	IntersectionNode::ReadIntersecNodeList(m_nObjID, vIntersectNode);
	return vIntersectNode;
}

void Stretch::UpdatePath()
{
	CADODatabase::UpdatePath2008InDatabase(m_path,m_nPathID);
}

bool Stretch::CopyData(const  ALTObject* pObj )
{
	if (this == pObj)
		return true;

	if( pObj->GetType() == GetType() )
	{
		Stretch * pOtherStretch = (Stretch*)pObj;
		m_path = pOtherStretch->GetPath();
		m_dLaneWidth = pOtherStretch->GetLaneWidth();
		m_iLaneNum = pOtherStretch->GetLaneNumber();

	
	}
	return __super::CopyData(pObj);
}

const GUID& Stretch::getTypeGUID()
{
	// {DD89E21D-BC77-40a2-B36C-5120E4823B8F}
	static const GUID name = 
	{ 0xdd89e21d, 0xbc77, 0x40a2, { 0xb3, 0x6c, 0x51, 0x20, 0xe4, 0x82, 0x3b, 0x8f } };
	
	return name;
}

ALTObjectDisplayProp* Stretch::NewDisplayProp()
{
	return new StretchDisplayProp();
}

