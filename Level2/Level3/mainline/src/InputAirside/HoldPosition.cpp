#include "StdAfx.h"
#include ".\holdposition.h"
#include "../Database/ADODatabase.h"
#include "AirsideImportExportManager.h"
#include "../Common/line2008.h"
#include "../Common/Path2008.h"
#include "common/Point2008.h"
#include "Common/ARCVector.h"

void HoldPosition::ExportHoldPositions(CAirsideExportFile& exportFile)
{
	exportFile.getFile().writeField(_T("ID, OBJID, SIGN, INTERSECNODEID, DISTOINTERSEC, INTERSECPOS"));
	exportFile.getFile().writeLine();

	int nProjectID = exportFile.GetProjectID();

	CString strSQL = _T("");
	strSQL.Format(_T("SELECT ID, OBJID, SIGN, INTERSECNODEID, DISTOINTERSEC, INTERSECPOS\
		FROM HOLDPOSITION\
		WHERE (OBJID IN\
		(SELECT ID\
		FROM ALTOBJECT\
		WHERE TYPE = %d AND APTID IN\
		(SELECT ID\
		FROM ALTAIRPORT\
		WHERE PROJID = %d)))"),(int)ALT_TAXIWAY,nProjectID);

	long nItemCount =0;
	CADORecordset adoRecordset;

	CADODatabase::ExecuteSQLStatement(strSQL,nItemCount,adoRecordset);


	while (!adoRecordset.IsEOF())
	{
		int nObjectID = -1;
		adoRecordset.GetFieldValue(_T("OBJID"),nObjectID);

		HoldPosition holdPosition;
		holdPosition.ReadData(adoRecordset);

		exportFile.getFile().writeInt(holdPosition.m_nUniqueID);
		exportFile.getFile().writeInt(nObjectID);
		exportFile.getFile().writeField(holdPosition.m_strSign);
		exportFile.getFile().writeInt(holdPosition.m_intersectNode.GetID());
		exportFile.getFile().writeDouble(holdPosition.m_distToIntersect);
		exportFile.getFile().writeDouble(holdPosition.m_intersectPos);
		exportFile.getFile().writeLine();
		
		adoRecordset.MoveNextData();
	}

	exportFile.getFile().endFile();
}
void HoldPosition::ImportHoldPositions(CAirsideImportFile& importFile)
{
	if( importFile.getVersion() < 1038 )
	{
		return;
	}
	
	while (!importFile.getFile().isBlank())
	{	
		HoldPosition holdPosition;

		int nOldID = -1;
		importFile.getFile().getInteger(nOldID);

		int nOldObjectID = -1;
		importFile.getFile().getInteger(nOldObjectID);

		importFile.getFile().getField(holdPosition.m_strSign.GetBuffer(1024),1024);
		
		int nOldIntersectionNodeID = -1;
		importFile.getFile().getInteger(nOldIntersectionNodeID);

		importFile.getFile().getFloat(holdPosition.m_distToIntersect);
		importFile.getFile().getFloat(holdPosition.m_intersectPos);


		holdPosition.m_intersectNode.SetID( importFile.GetIntersectionIndexMap(nOldIntersectionNodeID) );

		holdPosition.SaveData(importFile.GetObjectNewID(nOldObjectID));

		importFile.getFile().getLine();

	}

}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////


void HoldPosition::ReadHoldPositionList(const int nObjectID,std::vector<HoldPosition>& vHoldPosition)
{
	CString strSQL = _T("");
	strSQL.Format(_T("SELECT ID,OBJID, SIGN, INTERSECNODEID, DISTOINTERSEC, INTERSECPOS\
		FROM HOLDPOSITION\
		WHERE (OBJID = %d)"),nObjectID);
	long nItemCount =0;
	CADORecordset adoRecordset;

	CADODatabase::ExecuteSQLStatement(strSQL,nItemCount,adoRecordset);

	while (!adoRecordset.IsEOF())
	{
		HoldPosition holdPosition;
		holdPosition.ReadData(adoRecordset);
		
		vHoldPosition.push_back(holdPosition);

		adoRecordset.MoveNextData();
	}
}
void HoldPosition::ReadData(CADORecordset& adoRecordset)
{
	adoRecordset.GetFieldValue(_T("ID"),m_nUniqueID);
	adoRecordset.GetFieldValue(_T("OBJID"),m_nObjectID);
	adoRecordset.GetFieldValue(_T("SIGN"),m_strSign);
	int nNodeID;
	adoRecordset.GetFieldValue(_T("INTERSECNODEID"),nNodeID);
	m_intersectNode.ReadData(nNodeID);
	adoRecordset.GetFieldValue(_T("DISTOINTERSEC"),m_distToIntersect);
	adoRecordset.GetFieldValue(_T("INTERSECPOS"),m_intersectPos);
}
void HoldPosition::SaveData(int nObjectID)
{
	
	m_nObjectID = nObjectID;
	if (m_nUniqueID >= 0)
		return UpdateData();
	
	CString strSQL = _T("");
	strSQL.Format(_T("INSERT INTO HOLDPOSITION\
		(OBJID, SIGN, INTERSECNODEID, DISTOINTERSEC, INTERSECPOS)\
		VALUES (%d,'%s',%d,%f,%f)"),nObjectID,m_strSign,m_intersectNode.GetID(),m_distToIntersect,m_intersectPos );

	m_nUniqueID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);


}
void HoldPosition::DeleteData()
{
	CString strSQl = _T("");
	strSQl.Format(_T("DELETE FROM HOLDPOSITION\
		WHERE (ID = %d)"),m_nUniqueID);

	CADODatabase::ExecuteSQLStatement(strSQl);

}
void HoldPosition::UpdateData()
{
	CString strSQL;
	strSQL.Format(_T("UPDATE HOLDPOSITION\
		SET SIGN ='%s', INTERSECNODEID =%d, DISTOINTERSEC =%f, INTERSECPOS =%f\
		WHERE (ID = %d)"),m_strSign,m_intersectNode.GetID(),m_distToIntersect,m_intersectPos,m_nUniqueID );

	CADODatabase::ExecuteSQLStatement(strSQL);
}

HoldPosition::HoldPosition()
{
	m_distToIntersect = 0;
	m_intersectPos = 0;
	m_nUniqueID = -1;
	m_strSign = _T("");
	m_nObjectID = -1;
}

HoldPosition::HoldPosition( const IntersectionNode& node , double dintersect ,int nObjectID)
{
	m_intersectNode = node;
	m_intersectPos = dintersect;
	m_distToIntersect = 0;
	m_nObjectID = nObjectID;
	m_nUniqueID = -1;
}

HoldPosition::HoldPosition( const HoldPosition& other)
{
	m_distToIntersect = other.m_distToIntersect;
	m_intersectNode = other.m_intersectNode;
	m_intersectPos = other.m_intersectPos;
	m_nUniqueID  = other.m_nUniqueID;
	m_strSign = other.m_strSign;
	m_nObjectID = other.m_nObjectID;
}
bool HoldPosition::operator<( const HoldPosition& other ) const
{
	if( GetIntersectNode().GetDistance(m_nObjectID) < other.GetIntersectNode().GetDistance(m_nObjectID) )
		return true;
	if( other.GetIntersectNode().GetDistance(m_nObjectID) < GetIntersectNode().GetDistance(m_nObjectID) )
		return false;

	return m_distToIntersect < other.m_distToIntersect;
}

bool HoldPosition::IsFirst() const
{
	return m_intersectPos < GetIntersectNode().GetDistance(m_nObjectID);
}

double HoldPosition::GetPosition() const
{
	return m_intersectPos + m_distToIntersect;
}

IntersectionNode& HoldPosition::GetIntersectNode()
{
	return m_intersectNode;
}

const IntersectionNode& HoldPosition::GetIntersectNode() const
{
	return m_intersectNode;
}

HoldPosition& HoldPosition::operator=( const HoldPosition& other)
{
	m_distToIntersect = other.m_distToIntersect;
	m_intersectNode = other.m_intersectNode;
	m_intersectPos = other.m_intersectPos;
	m_nUniqueID  = other.m_nUniqueID;
	m_strSign = other.m_strSign;
	m_nObjectID = other.m_nObjectID;
	return *this;
}

HoldPosition::~HoldPosition()
{

}

bool HoldPosition::operator==( const HoldPosition& other ) const
{
	return (m_intersectNode == other.m_intersectNode && IsFirst() == other.IsFirst());
}

void HoldPosition::SetPosition( double pos )
{
	m_distToIntersect = pos - m_intersectPos;
}
//read hold information
//throw CADOException if failed
void HoldPosition::ReadHold( int nHoldID )
{
	CString strSQL = _T("");
	strSQL.Format(_T("SELECT ID,OBJID, SIGN, INTERSECNODEID, DISTOINTERSEC, INTERSECPOS\
					 FROM HOLDPOSITION\
					 WHERE (ID = %d)"),nHoldID);
	long nItemCount =0;
	CADORecordset adoRecordset;

	CADODatabase::ExecuteSQLStatement(strSQL,nItemCount,adoRecordset);

	if (!adoRecordset.IsEOF())
	{
		ReadData(adoRecordset);
	}
}

int HoldPosition::GetUniqueID() const
{
	return m_nUniqueID;
}

void HoldPosition::SetUniqueID( int val )
{
	m_nUniqueID = val;
}
bool HoldPositionFunc::CaculateHoldPositionLine(const CPath2008& path,const DistanceUnit& width, const HoldPositionList& holdlist, int idx, CLine2008& line)
{
	ASSERT( idx < (int)holdlist.size() );
	if(idx >(int)holdlist.size() ) return false;
	//get the real position
	double realdist = holdlist.at(idx).GetPosition();
	CPoint2008 pt; 


	if( path.GetDistPoint(realdist,pt) ){
		double relatedist = path.GetDistIndex(realdist);

		int npt = (int)relatedist;

		int nptnext;
		if(npt >= path.getCount() -1 ){
			nptnext = path.getCount() -2;
			npt = nptnext +1;
		}else {
			nptnext = npt +1;
		}

		CPoint2008 dir = CPoint2008( path.getPoint(npt) - path.getPoint(nptnext) );
		dir.setZ(0);
		dir.Normalize();
		dir.rotate(90);
		dir.length(width*0.5);
		line = CLine2008( pt + dir, pt -dir);
		return true;
	}
	return false;	
}

bool UnExsitHoldPredicate::operator()( const HoldPosition& hold ) const
{
	return m_holdInfoList.end() == std::find(m_holdInfoList.begin(),m_holdInfoList.end(),hold);
}
