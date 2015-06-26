#include "StdAfx.h"
#include ".\filettaxiway.h"
#include "Runway.h"
#include "Taxiway.h"
#include "AirsideImportExportManager.h"
#include "InputAirside.h"
#include "Common/BizierCurve.h"
#include "Common/ARCPath.h"
#include "IntersectionNodesInAirport.h"
#include <cmath>
/************************************************************************/
/*      FilletPoint
*/
/************************************************************************/
FilletPoint::FilletPoint()
{
	m_dDistToIntersect = 0;
	m_dUserDist = 0;
	m_nIntersectItemID = -1;
}

FilletPoint::FilletPoint( int nItemID,const DistanceUnit& dist )
{
	m_dDistToIntersect = dist;
	m_dUserDist = 0;
	m_nIntersectItemID = nItemID;
}


DistanceUnit FilletPoint::GetDistToIntersect() const
{
	return m_dDistToIntersect;
}

void FilletPoint::SetDistToInterect( const DistanceUnit& dist )
{
	m_dDistToIntersect = dist;
}

bool FilletPoint::IsAtSameSide( const FilletPoint& otherFillet ) const
{
	return (m_dDistToIntersect<=0) == (otherFillet.m_dDistToIntersect<=0);
}

void FilletPoint::SetUsrDist( const DistanceUnit& dist )
{
	m_dUserDist = dist;
}

DistanceUnit FilletPoint::GetUsrDist() const
{
	return m_dUserDist;
}

FilletPoint::~FilletPoint()
{

}

DistanceUnit FilletPoint::GetRealDist() const
{
	return GetDistToIntersect()+GetUsrDist();
}
/************************************************************************/
/*        FilletTaxiway
*/
/************************************************************************/
FilletTaxiway::FilletTaxiway()
{
	m_bActive = true;
	m_nID  = -1;
}
FilletTaxiway::FilletTaxiway( const IntersectionNode& node )
{
	m_IntersectNode = node;
	m_bActive = true;
	m_nID  = -1;

}


void FilletTaxiway::SetFilletPoint1( const FilletPoint& filtPt )
{
	m_filetPt1 = filtPt;
}

void FilletTaxiway::SetFilletPoint2( const FilletPoint& filtPt )
{
	m_filetPt2 = filtPt;
}

IntersectionNode& FilletTaxiway::GetIntersectNode()
{
	return m_IntersectNode;
}

FilletPoint& FilletTaxiway::GetFilletPoint2()
{
	return m_filetPt2;
}

FilletPoint& FilletTaxiway::GetFilletPoint1()
{
	return m_filetPt1;
}

bool FilletTaxiway::IsOverLapWith( const FilletTaxiway& otherWay ) const
{
	if( !m_IntersectNode.IsIdentical(otherWay.m_IntersectNode) )
		return false;

	if( m_filetPt1.GetIntersectItemID() == otherWay.m_filetPt1.GetIntersectItemID() 
		&& m_filetPt2.GetIntersectItemID() == otherWay.m_filetPt2.GetIntersectItemID() )
	{
		if( m_filetPt1.IsAtSameSide(otherWay.m_filetPt1) 
			&& m_filetPt2.IsAtSameSide(otherWay.m_filetPt2) )
			return true;
	}

	if( m_filetPt2.GetIntersectItemID() == otherWay.m_filetPt1.GetIntersectItemID() 
		&& m_filetPt1.GetIntersectItemID() == otherWay.m_filetPt2.GetIntersectItemID() )
	{
		if( m_filetPt1.IsAtSameSide(otherWay.m_filetPt2)
			&& m_filetPt2.IsAtSameSide(otherWay.m_filetPt1) )
			return true;
	}
	
	return false;
}

void FilletTaxiway::SetActive( bool b )
{
	m_bActive = b;
}

bool FilletTaxiway::IsActive() const
{
	return m_bActive;
}

CPoint2008 FilletTaxiway::GetFilletPoint1Pos() 
{
	IntersectItem * pItem = m_IntersectNode.GetItemByID( m_filetPt1.GetIntersectItemID() );
	ASSERT( pItem);

	DistanceUnit distInObj  = m_filetPt1.GetDistToIntersect() + m_filetPt1.GetUsrDist() + pItem->GetDistInItem();
	ALTObject * pObj = pItem->GetObject();
	if(pObj && pObj->GetType() == ALT_RUNWAY)
	{
		return ((Runway*)pObj)->GetPath().GetDistPoint(distInObj);
	}
	if( pObj && pObj->GetType() == ALT_TAXIWAY )
	{
		return ((Taxiway*)pObj)->GetPath().GetDistPoint(distInObj);
	}
	ASSERT(false);
	return m_IntersectNode.GetPosition();

}

CPoint2008 FilletTaxiway::GetFilletPoint2Pos() 
{
	IntersectItem * pItem = m_IntersectNode.GetItemByID( m_filetPt2.GetIntersectItemID() );
	ASSERT( pItem);

	DistanceUnit distInObj  = m_filetPt2.GetDistToIntersect() + m_filetPt2.GetUsrDist() + pItem->GetDistInItem();
	ALTObject * pObj = pItem->GetObject();
	if(pObj && pObj->GetType() == ALT_RUNWAY)
	{
		return ((Runway*)pObj)->GetPath().GetDistPoint(distInObj);
	}
	if( pObj && pObj->GetType() == ALT_TAXIWAY )
	{
		return ((Taxiway*)pObj)->GetPath().GetDistPoint(distInObj);
	}
	ASSERT(false);
	return m_IntersectNode.GetPosition();
}

DistanceUnit FilletTaxiway::GetWidth() 
{
	DistanceUnit obj1Width =0 ;
	DistanceUnit obj2Width =0;
	IntersectItem * pItem1 = m_IntersectNode.GetItemByID( m_filetPt1.GetIntersectItemID() );
	IntersectItem * pItem2 = m_IntersectNode.GetItemByID( m_filetPt2.GetIntersectItemID() );

	ALTObject * pObj1= pItem1->GetObject();
	ALTObject * pObj2 = pItem2->GetObject();

	if(pObj1->GetType() == ALT_TAXIWAY) 	obj1Width = ((Taxiway*)pObj1)->GetWidth();
	else if(pObj1->GetType() == ALT_RUNWAY) obj1Width = ((Runway*)pObj1)->GetWidth();

	if(pObj2->GetType() == ALT_TAXIWAY)  obj2Width = ((Taxiway*)pObj2)->GetWidth();
	else if(pObj2->GetType() == ALT_RUNWAY) obj2Width = ((Runway*)pObj2)->GetWidth();

	return min(obj1Width,obj2Width);
}

ARCVector3 FilletTaxiway::GetFilletPoint1Dir()
{
	IntersectItem * pItem = m_IntersectNode.GetItemByID( m_filetPt1.GetIntersectItemID() );
	ASSERT( pItem);

	DistanceUnit distInObj  = m_filetPt1.GetDistToIntersect() + m_filetPt1.GetUsrDist() + pItem->GetDistInItem();
	ALTObject * pObj = pItem->GetObject();
	if(pObj && pObj->GetType() == ALT_RUNWAY)
	{
		return ((Runway*)pObj)->GetPath().GetDistPoint(distInObj +1) - ((Runway*)pObj)->GetPath().GetDistPoint(distInObj);
	}
	if( pObj && pObj->GetType() == ALT_TAXIWAY )
	{
		return ((Taxiway*)pObj)->GetPath().GetDistPoint(distInObj+1) - ((Taxiway*)pObj)->GetPath().GetDistPoint(distInObj);
	}
	ASSERT(false);
	return CPoint2008(0,0,0);

}

ARCVector3 FilletTaxiway::GetFilletPoint2Dir()
{
	IntersectItem * pItem = m_IntersectNode.GetItemByID( m_filetPt2.GetIntersectItemID() );
	ASSERT( pItem);

	DistanceUnit distInObj  = m_filetPt2.GetDistToIntersect() + m_filetPt2.GetUsrDist() + pItem->GetDistInItem();
	ALTObject * pObj = pItem->GetObject();
	if(pObj && pObj->GetType() == ALT_RUNWAY)
	{
		return ((Runway*)pObj)->GetPath().GetDistPoint(distInObj+1)- ((Runway*)pObj)->GetPath().GetDistPoint(distInObj);
	}
	if( pObj && pObj->GetType() == ALT_TAXIWAY )
	{
		return ((Taxiway*)pObj)->GetPath().GetDistPoint(distInObj+1)- ((Taxiway*)pObj)->GetPath().GetDistPoint(distInObj);
	}
	ASSERT(false);
	return CPoint2008(0,0,0);
}

void FilletTaxiway::InitFromDBRecordset( CADORecordset& recordset )
{
	recordset.GetFieldValue(_T("ID"),m_nID);
	int nodeID;
	recordset.GetFieldValue(_T("INTERSECNODEID"),nodeID);
	m_IntersectNode.ReadData(nodeID);

	int ItemID1;
	recordset.GetFieldValue(_T("INTERSECTITEMID1"),ItemID1);
	int ItemID2;
	recordset.GetFieldValue(_T("INTERSECTITEMID2"),ItemID2);
	m_filetPt1.SetIntersectItemID(ItemID1);
	m_filetPt2.SetIntersectItemID(ItemID2);

	int bActive;
	recordset.GetFieldValue(_T("ACTIVE"),bActive);
	m_bActive = (bActive != 0);

	recordset.GetFieldValue(_T("DISTTOINTERSECNODE1"), m_filetPt1.m_dDistToIntersect);
	recordset.GetFieldValue(_T("DISTTOINTERSECNODE2"), m_filetPt2.m_dDistToIntersect);
	recordset.GetFieldValue(_T("USERDIST1"), m_filetPt1.m_dUserDist);
	recordset.GetFieldValue(_T("USERDIST2"), m_filetPt2.m_dUserDist);
}

void FilletTaxiway::GetInsertSQL( int nParentID,CString& strSQL ) const
{	
	strSQL.Format(_T("INSERT INTO  FILLETTAXIWAY \
					 ( INTERSECNODEID \
					 , ACTIVE \
					 , DISTTOINTERSECNODE1 \
					 , DISTTOINTERSECNODE2 \
					 , USERDIST1 \
					 , USERDIST2 \
					 , INTERSECTITEMID1 \
					 , INTERSECTITEMID2 \
					 , AIRPORTID )\
					 VALUES\
					 (%d, %d, %f,%f,%f,%f,%d,%d, %d)"),
					 m_IntersectNode.GetID(),(int)m_bActive,m_filetPt1.m_dDistToIntersect, m_filetPt2.m_dDistToIntersect,m_filetPt1.m_dUserDist,m_filetPt2.m_dUserDist,m_filetPt1.m_nIntersectItemID, m_filetPt2.m_nIntersectItemID, nParentID);
}

void FilletTaxiway::GetUpdateSQL( CString& strSQL ) const
{
	strSQL.Format(_T("UPDATE FILLETTAXIWAY\
					 SET INTERSECNODEID = '%d', \
					 ACTIVE = %d,\
					 DISTTOINTERSECNODE1 = %f,\
					 DISTTOINTERSECNODE2 = %f, \
					 USERDIST1 = %f, \
					 USERDIST2 = %f, \
					 INTERSECTITEMID1 = %d, \
					 INTERSECTITEMID2 = %d \
					 WHERE (ID = %d)"),
					 m_IntersectNode.GetID(),
					 (int)m_bActive,
					 m_filetPt1.m_dDistToIntersect,
					 m_filetPt2.m_dDistToIntersect,
					 m_filetPt1.m_dUserDist,
					 m_filetPt2.m_dUserDist,
					 m_filetPt1.m_nIntersectItemID,
					 m_filetPt2.m_nIntersectItemID,
					 m_nID				
				  );
}


void FilletTaxiway::GetDeleteSQL( CString& strSQL ) const
{
	strSQL.Format(_T("DELETE FROM FILLETTAXIWAY WHERE (ID = %d)"),m_nID);
}

void FilletTaxiway::GetSelectSQL( int nARPID,CString& strSQL ) const
{
	strSQL.Format(_T("SELECT * FROM FILLETTAXIWAY WHERE (AIRPORTID = %d)"), nARPID);
}

void FilletTaxiway::GetSelectSQLByIntersectNode( int nInterctNode, CString& strSQL ) const
{
	strSQL.Format(_T("SELECT * FROM FILLETTAXIWAY WHERE (INTERSECNODEID = %d)"), nInterctNode);
}

void FilletTaxiway::ReadAirportFillets( int nARPId ,const IntersectionNodesInAirport& vnodes,std::vector<FilletTaxiway>& vFillets )
{
	//read intersection Node
	
	long nItemCount =0;
	CADORecordset adoRecordset;
	FilletTaxiway newFillet;
	CString selectSQL;
	newFillet.GetSelectSQL(nARPId,selectSQL);
	CADODatabase::ExecuteSQLStatement(selectSQL,nItemCount,adoRecordset);
	while (!adoRecordset.IsEOF())
	{	
		newFillet.ReadData(adoRecordset,vnodes);
		if(newFillet.GetIntersectNode().GetID()>=0)
			vFillets.push_back(newFillet);
		else 
			newFillet.DeleteData();

		adoRecordset.MoveNextData();
	}
}

void FilletTaxiway::ReadData( CADORecordset& recordset,const IntersectionNodesInAirport& vnodes )
{
	recordset.GetFieldValue(_T("ID"),m_nID);
	int nodeID;
	recordset.GetFieldValue(_T("INTERSECNODEID"),nodeID);

	m_IntersectNode = vnodes.GetNodeByID(nodeID);
	
	int ItemID1;
	recordset.GetFieldValue(_T("INTERSECTITEMID1"),ItemID1);
	int ItemID2;
	recordset.GetFieldValue(_T("INTERSECTITEMID2"),ItemID2);
	m_filetPt1.SetIntersectItemID(ItemID1);
	m_filetPt2.SetIntersectItemID(ItemID2);

	int bActive;
	recordset.GetFieldValue(_T("ACTIVE"),bActive);
	m_bActive = (bActive != 0);

	recordset.GetFieldValue(_T("DISTTOINTERSECNODE1"), m_filetPt1.m_dDistToIntersect);
	recordset.GetFieldValue(_T("DISTTOINTERSECNODE2"), m_filetPt2.m_dDistToIntersect);
	recordset.GetFieldValue(_T("USERDIST1"), m_filetPt1.m_dUserDist);
	recordset.GetFieldValue(_T("USERDIST2"), m_filetPt2.m_dUserDist);
}

void FilletTaxiway::ExportData( CAirsideExportFile& exportFile )
{
	exportFile.getFile().writeInt(m_IntersectNode.GetID());
	exportFile.getFile().writeInt((int)m_bActive);
	exportFile.getFile().writeFloat((float)GetFilletPoint1().m_dDistToIntersect);
	exportFile.getFile().writeFloat((float)GetFilletPoint2().m_dDistToIntersect);
	exportFile.getFile().writeFloat((float)GetFilletPoint1().m_dUserDist);
	exportFile.getFile().writeFloat((float)GetFilletPoint2().m_dUserDist);
	exportFile.getFile().writeInt( GetFilletPoint1().GetIntersectItemID() );
	exportFile.getFile().writeInt( GetFilletPoint2().GetIntersectItemID() );
	exportFile.getFile().writeInt(m_IntersectNode.GetAirportID());
	exportFile.getFile().writeLine();
}

void FilletTaxiway::ImportData( CAirsideImportFile& importFile )
{
	int nNodeID ;
	importFile.getFile().getInteger(nNodeID);	
	m_IntersectNode.SetID( importFile.GetIntersectionIndexMap(nNodeID) );
	
	int bActive;
	importFile.getFile().getInteger(bActive);
	m_bActive = (bActive!=0);

	importFile.getFile().getFloat(m_filetPt1.m_dDistToIntersect);
	importFile.getFile().getFloat(m_filetPt2.m_dDistToIntersect);
	importFile.getFile().getFloat(m_filetPt1.m_dUserDist);
	importFile.getFile().getFloat(m_filetPt2.m_dUserDist);

	int nItem1ID;
	importFile.getFile().getInteger(nItem1ID);
	nItem1ID = importFile.GetIntersectItemNewID(nItem1ID);
	m_filetPt1.SetIntersectItemID(nItem1ID); 

	int nItem2ID;
	importFile.getFile().getInteger(nItem2ID);
	nItem2ID = importFile.GetIntersectItemNewID(nItem2ID);
	m_filetPt2.SetIntersectItemID(nItem2ID);

	int nARPID;
	importFile.getFile().getInteger(nARPID);	
	nARPID = importFile.GetAirportNewIndex(nARPID);
	m_IntersectNode.SetAirportID(nARPID);

	importFile.getFile().getLine();

	SaveDataToDB();
}

void FilletTaxiway::SetID( int nID )
{
	m_nID = nID;
}

void FilletTaxiway::SaveDataToDB()
{	
	SaveData(m_IntersectNode.GetAirportID());	
}


void FilletTaxiway::ReadIntersectionFillets( const IntersectionNode& intersectNode, std::vector<FilletTaxiway>& vFillets )
{
	long nItemCount =0;
	CADORecordset adoRecordset;
	FilletTaxiway newFillet;
	CString selectSQL;
	newFillet.GetSelectSQLByIntersectNode(intersectNode.GetID(),selectSQL);
	CADODatabase::ExecuteSQLStatement(selectSQL,nItemCount,adoRecordset);
	while (!adoRecordset.IsEOF())
	{	
		newFillet.InitFromDBRecordset(adoRecordset);
		newFillet.SetIntersectNode(intersectNode);
		vFillets.push_back(newFillet);
		adoRecordset.MoveNextData();
	}
}

DistanceUnit FilletTaxiway::GetFilletPt1Dist()const
{
	IntersectItem * pItem = m_IntersectNode.GetItemByID( m_filetPt1.GetIntersectItemID() );
	ASSERT( pItem);

	return m_filetPt1.GetDistToIntersect() + m_filetPt1.GetUsrDist() + pItem->GetDistInItem();
}

DistanceUnit FilletTaxiway::GetFilletPt2Dist()const
{
	IntersectItem * pItem = m_IntersectNode.GetItemByID( m_filetPt2.GetIntersectItemID() );
	ASSERT( pItem);

	return m_filetPt2.GetDistToIntersect() + m_filetPt2.GetUsrDist() + pItem->GetDistInItem();
}

CPoint2008 FilletTaxiway::GetFilletPoint1Ext()
{
	IntersectItem * pItem = m_IntersectNode.GetItemByID( m_filetPt1.GetIntersectItemID() );
	ASSERT( pItem);

	DistanceUnit distInObj  = m_filetPt1.GetDistToIntersect() + m_filetPt1.GetUsrDist() + pItem->GetDistInItem();
	ALTObject * pObj = pItem->GetObject();
	if(pObj && pObj->GetType() == ALT_RUNWAY)
	{
		return ((Runway*)pObj)->GetPath().GetDistPoint( distInObj + ((m_filetPt1.GetDistToIntersect()>0)?1:-1) );
	}
	if( pObj && pObj->GetType() == ALT_TAXIWAY )
	{
		return ((Taxiway*)pObj)->GetPath().GetDistPoint( distInObj + ((m_filetPt1.GetDistToIntersect()>0)?1:-1) ) ;
	}
	ARCVector3 vDir = (GetFilletPoint1Pos() - m_IntersectNode.GetPosition());
	vDir.Normalize();
	return GetFilletPoint1Pos() + vDir;
}

CPoint2008 FilletTaxiway::GetFilletPoint2Ext()
{
	IntersectItem * pItem = m_IntersectNode.GetItemByID( m_filetPt2.GetIntersectItemID() );
	ASSERT( pItem);

	DistanceUnit distInObj  = m_filetPt2.GetDistToIntersect() + m_filetPt2.GetUsrDist() + pItem->GetDistInItem();
	ALTObject * pObj = pItem->GetObject();
	if(pObj && pObj->GetType() == ALT_RUNWAY)
	{
		return ((Runway*)pObj)->GetPath().GetDistPoint( distInObj + ((m_filetPt2.GetDistToIntersect()>0)?1:-1) ) ;
	}
	if( pObj && pObj->GetType() == ALT_TAXIWAY )
	{
		return ((Taxiway*)pObj)->GetPath().GetDistPoint( distInObj + ((m_filetPt2.GetDistToIntersect()>0)?1:-1) ) ;
	}
	ARCVector3 vDir = (GetFilletPoint2Pos() - m_IntersectNode.GetPosition());
	vDir.Normalize();
	return GetFilletPoint2Pos() + vDir;
}

FilletTaxiway::~FilletTaxiway()
{

}

void FilletTaxiway::GetControlPoints( std::vector<CPoint2008>& vPts )
{
	DistanceUnit distInObj1 = GetFilletPt1Dist();
	DistanceUnit distInobj2 = GetFilletPt2Dist();
	IntersectItem * pItem1 = m_IntersectNode.GetItemByID( m_filetPt1.GetIntersectItemID() );
	IntersectItem * pItem2 = m_IntersectNode.GetItemByID( m_filetPt2.GetIntersectItemID() );
	
	DistanceUnit intersectDist1 = pItem1->GetDistInItem();
	DistanceUnit intersectDist2 = pItem2->GetDistInItem();
	CPath2008 path1 = GetFilletPt1AtPath();
	CPath2008 path2 = GetFilletPt2AtPath();
	
	std::vector<CPoint2008> vPtsA;
	path1.GetInPathPoints(distInObj1, intersectDist1, vPtsA);
	std::vector<CPoint2008> vPtsB;
	path2.GetInPathPoints(intersectDist2, distInobj2, vPtsB);


	//CPoint2008 fp1 = path1.GetDistPoint(distInObj1);
	//CPoint2008 vDir1;
	//if( vPtsA.size() )
	//{
	//	vDir1 = (*vPtsA.begin()) - fp1;
	//}else
	//{
	//	vDir1 = m_IntersectNode.GetPosition() - fp1;
	//}

	//CPoint2008 fp2 = path2.GetDistPoint(distInobj2);
	//CPoint2008 vDir2;
	//if( vPtsB.size() )
	//{
	//	vDir2 = (*vPtsB.begin())-fp2;
	//}
	//else
	//{
	//	vDir2 = m_IntersectNode.GetPosition() - fp2;
	//}

	//vDir1.Normalize();
	//vDir2.Normalize();
	//double cosdAnlge1 = (fp2 - fp1).GetCosOfTwoVector(vDir1);
	//double cosdAngle2 = (fp1 - fp2).GetCosOfTwoVector(vDir2);
	//

	//vPts.push_back(fp1);
	//if(acos(cosdAnlge1) + acos(cosdAngle2) < ARCMath::DegreesToRadians(180) )
	//{
	//	//double cosZeda = vDir1.GetCosOfTwoVector(vDir2);	
	//	
	//	DistanceUnit distP1P2 = fp1.distance(fp2);
	//	vDir1.length(distP1P2*0.5 );
	//	vDir2.length(distP1P2*0.5 );
	//	vPts.push_back(fp1+vDir1);
	//	vPts.push_back(fp2+vDir2);
	//}
	//vPts.push_back(fp2);


	vPts.push_back( path1.GetDistPoint(distInObj1) );
	if(vPtsA.size())
	{
		vPts.push_back(*vPtsA.begin());
		//vPts.push_back( path1.GetDistPoint( (distInObj1 + intersectDist1) *0.5 ) );
	}
	if( vPtsA.size() + vPtsB.size()< 1  && abs(distInObj1-intersectDist1)>ARCMath::EPSILON && abs(distInobj2-intersectDist2)>ARCMath::EPSILON )
	{
		vPts.push_back( m_IntersectNode.GetPosition() );
	}
	if(vPtsB.size())
	{
		vPts.push_back( *vPtsB.begin() );
		//vPts.push_back( path2.GetDistPoint( (distInobj2 + intersectDist2)*0.5 ));
	}
	vPts.push_back( path2.GetDistPoint(distInobj2) );
}

inline CPath2008 FilletTaxiway::GetFilletPt1AtPath() 
{
	IntersectItem * pItem = m_IntersectNode.GetItemByID( m_filetPt1.GetIntersectItemID() );
	ASSERT( pItem);

	ALTObject * pObj = pItem->GetObject();//m_IntersectNode.GetObject(m_filetPt1.GetObjectID());
	if(pObj && pObj->GetType() == ALT_RUNWAY)
	{
		return ((Runway*)pObj)->GetPath();
	}
	if( pObj && pObj->GetType() == ALT_TAXIWAY )
	{
		return ((Taxiway*)pObj)->GetPath();
	}
	return CPath2008();
}

CPath2008 FilletTaxiway::GetFilletPt2AtPath() 
{
	IntersectItem * pItem = m_IntersectNode.GetItemByID( m_filetPt2.GetIntersectItemID() );
	ASSERT( pItem);

	ALTObject * pObj = pItem->GetObject();//m_IntersectNode.GetObject(m_filetPt2.GetObjectID());
	if(pObj && pObj->GetType() == ALT_RUNWAY)
	{
		return ((Runway*)pObj)->GetPath();
	}
	if( pObj && pObj->GetType() == ALT_TAXIWAY )
	{
		return ((Taxiway*)pObj)->GetPath();
	}
	return CPath2008();
}

void FilletTaxiway::CalculateRoutePath()
{
	std::vector<ARCVector3> _inPts;
	std::vector<CPoint2008> vCtrlPts;
	ARCPath3 vPath;

	GetControlPoints(vCtrlPts);	

	const int nCtrlCnt = (int)vCtrlPts.size();
	if(nCtrlCnt < 2 )
		return ;

	if(nCtrlCnt==2)
	{	
		vPath.push_back(vCtrlPts[0]);
		vPath.push_back(vCtrlPts[1]);
	}
	else
	{
		_inPts.reserve(4);
		_inPts.push_back(vCtrlPts[0]);
		ARCVector3 vBegin = vCtrlPts[1] - vCtrlPts[0];
		ARCVector3 vEnd = vCtrlPts[nCtrlCnt-2] - vCtrlPts[nCtrlCnt-1];
		DistanceUnit dist = vCtrlPts[0].distance3D( vCtrlPts[nCtrlCnt-1] );
		vBegin.SetLength(dist*0.5);
		vEnd.SetLength(dist*0.5);
		_inPts.push_back(vCtrlPts[0] + vBegin);
		_inPts.push_back(vCtrlPts[nCtrlCnt-1] + vEnd);
		_inPts.push_back(vCtrlPts[nCtrlCnt-1] );

		vPath.clear();vPath.reserve(10);
		BizierCurve::GenCurvePoints(_inPts,vPath,10);
	}


	m_routePath.clear();
	m_routePath = vPath.getPath2008();
}

CPath2008 FilletTaxiway::GetRoutePath(bool needReverse)
{
	if(m_routePath.getCount() <= 0)
		CalculateRoutePath();

	if(!needReverse)
		return m_routePath;

	CPath2008 invertPath = m_routePath;
	invertPath.invertList();
	return  invertPath;
}

int FilletTaxiway::GetObject1ID()const
{
	IntersectItem * pItem = m_IntersectNode.GetItemByID( m_filetPt1.GetIntersectItemID() );
	ASSERT( pItem);
	return pItem->GetObjectID();
}

int FilletTaxiway::GetObject2ID()const
{
	IntersectItem * pItem = m_IntersectNode.GetItemByID( m_filetPt2.GetIntersectItemID() );
	ASSERT( pItem);
	return pItem->GetObjectID();
}

void FilletTaxiway::DeleteData()
{
	CString strSQL;
	GetDeleteSQL(strSQL);
	if (strSQL.IsEmpty())
		return;

	CADODatabase::ExecuteSQLStatement(strSQL);
}

int FilletTaxiway::GetID() const
{
	return m_nID;
}

void FilletTaxiway::SaveData( int nParentID )
{
	CString strSQL;

	if (m_nID < 0)
	{
		GetInsertSQL(nParentID,strSQL);		
		if (strSQL.IsEmpty())
			return;

		m_nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
	}
	else
	{
		GetUpdateSQL(strSQL);
		if (strSQL.IsEmpty())
			return;

		CADODatabase::ExecuteSQLStatement(strSQL);
	}	
}

/************************************************************************/
/*          Fillet   List                                               */
/************************************************************************/
void FilletTaxiwayList::AddItem( const FilletTaxiway& filtTaxiway )
{
	int nOvelapItem = FindOverLapFillet(filtTaxiway) ;
	if( nOvelapItem < 0)
	{
		m_vData.push_back(filtTaxiway);
	}
	else
	{
		m_vData[nOvelapItem] = filtTaxiway;
	}
}

int FilletTaxiwayList::FindOverLapFillet( const FilletTaxiway& fillet )
{
	for(int i =0 ; i< (int)m_vData.size() ;i++)
	{
		FilletTaxiway& theItem = m_vData[i];
		if( theItem.IsOverLapWith(fillet) )
		{
			return i;
		}
	}

	return -1;
}

int FilletTaxiwayList::GetCount() const
{
	return (int)m_vData.size();
}

FilletTaxiway& FilletTaxiwayList::FilletAt( int idx )
{
	ASSERT(idx < (int)m_vData.size() && idx>=0);
	return m_vData[idx];
}

const FilletTaxiway& FilletTaxiwayList::FilletAt( int idx ) const
{
	ASSERT(idx < (int)m_vData.size() && idx>=0);
	return m_vData[idx];
}

bool FilletTaxiwayList::Remove( int idx )
{
	if(idx>=0 && idx< GetCount())
	{
		m_vData.erase( m_vData.begin() + idx);
		return true;
	}
	return false;
}
/************************************************************************/
/*         FilletTaxiwaysInAirport                                                             */
/************************************************************************/
#include "Taxiway.h"
#include "Runway.h"
#include "IntersectionNode.h"

int FilletTaxiwaysInAirport::GetCount() const
{
	return (int) m_vFilletWays.m_vData.size();
}

FilletTaxiway& FilletTaxiwaysInAirport::ItemAt( int idx )
{
	ASSERT(idx>=0 && idx< GetCount());
	return m_vFilletWays.m_vData[idx];
}

const FilletTaxiway& FilletTaxiwaysInAirport::ItemAt( int idx ) const
{
	ASSERT(idx>=0 && idx< GetCount());
	return m_vFilletWays.m_vData[idx];
}

//check to see the fillet is valid
bool IsValidFilletTaxiway(FilletTaxiway& filet)
{
	DistanceUnit path1Len = filet.GetFilletPt1AtPath().GetTotalLength();
	DistanceUnit path2Len = filet.GetFilletPt2AtPath().GetTotalLength();
	DistanceUnit pt1AtDist = filet.GetFilletPt1Dist();
	DistanceUnit pt2AtDist = filet.GetFilletPt2Dist();

	if( pt1AtDist < 0 || pt1AtDist > path1Len )
		return false;

	if( pt2AtDist < 0 || pt2AtDist > path2Len )
		return false;

	return true;
}


void FilletTaxiwaysInAirport::SaveData( int nARPID )
{
	
	for(int i=0;i< GetCount();i++)
	{		
		ItemAt(i).SaveData(nARPID);
	}
}

void FilletTaxiwaysInAirport::ReadData( int nARPID, const IntersectionNodesInAirport& vnodes )
{
	m_vFilletWays.m_vData.clear();
	FilletTaxiway::ReadAirportFillets(nARPID,vnodes,m_vFilletWays.m_vData);
}

void FilletTaxiwaysInAirport::ReadData( int nARPId )
{
	m_vFilletWays.m_vData.clear();
	long nItemCount =0;
	CADORecordset adoRecordset;
	FilletTaxiway newFillet;
	CString selectSQL;
	newFillet.GetSelectSQL(nARPId,selectSQL);
	CADODatabase::ExecuteSQLStatement(selectSQL,nItemCount,adoRecordset);
	while (!adoRecordset.IsEOF())
	{	
		newFillet.InitFromDBRecordset(adoRecordset);
		m_vFilletWays.AddItem(newFillet);
		adoRecordset.MoveNextData();
	}
}

void FilletTaxiwaysInAirport::ExportData( CAirsideExportFile& exportFile )
{
	//fillets.ReadData()
	exportFile.getFile().writeField("FilletTaxiway");
	exportFile.getFile().writeLine();

	int nPrjID = exportFile.GetProjectID();
	std::vector<int> vAirportIDs;
	InputAirside::GetAirportList(nPrjID,vAirportIDs);

	for(int i=0;i< (int)vAirportIDs.size();i++)
	{
		int nAirportID = vAirportIDs.at(i);
		FilletTaxiwaysInAirport fillets;
		fillets.ReadData(nAirportID);

		for(int j=0;j< fillets.GetCount();j++)
		{
			fillets.ItemAt(j).ExportData(exportFile);
		}
	}

	exportFile.getFile().endFile();
}

void FilletTaxiwaysInAirport::ImportData( CAirsideImportFile& importFile )
{
	if( importFile.getVersion() < 1038 )
	{
		return;
	}
	while (!importFile.getFile().isBlank())
	{
		FilletTaxiway fillet;
		fillet.ImportData(importFile);
	}
}


const static double dMinRadiusScale = 1.0/std::tan( ARCMath::DegreesToRadians(85)*0.5 );
const static double dMaxctanhalfAngle = 1.0/std::tan( ARCMath::DegreesToRadians(10)*0.5 );
const static double dMaxRadiusScale= 4;//
void CalNodeFillet(const IntersectionNode& node ,IntersectItem& item1, IntersectItem& item2 ,const DistanceUnit& FilletRadius, FilletTaxiwayList& vFillets )
{
	CPath2008 item1Path = item1.GetPath();
	CPath2008 item2Path = item2.GetPath();
	DistanceUnit lenPath1 = item1Path.GetTotalLength();
	DistanceUnit lenPath2 = item2Path.GetTotalLength();
	
	DistanceUnit distInItem1 = item1.GetDistInItem();
	DistanceUnit distInItem2 = item2.GetDistInItem();

	CPoint2008 pt1 = item1Path.GetDistPoint(distInItem1);
	CPoint2008 pt2 = item2Path.GetDistPoint(distInItem2);

	if( distInItem1 > 1 )
	{
		ARCVector3 vDir1 = item1Path.GetDistPoint(distInItem1 -1 ) - pt1;
		if( distInItem2 > 1)
		{
			ARCVector3 vDir2 = item2Path.GetDistPoint(distInItem2-1) - pt2;
			double cosZeda  = ( vDir1.GetCosOfTwoVector(vDir2) );
			double ctanhalfZeda = sqrt( (1.0+cosZeda)/(1.0-cosZeda));
			
			if( ctanhalfZeda < dMaxctanhalfAngle /*&& ctanhalfZeda > dMinRadiusScale */)
			{
				ctanhalfZeda  = min(ctanhalfZeda, dMaxRadiusScale);
				//ctanhalfZeda = max(ctanhalfZeda, dMinRadiusScale);
				DistanceUnit absDistToIntersect = FilletRadius * ctanhalfZeda;
				if( distInItem1 - absDistToIntersect > 0 && distInItem2 - absDistToIntersect > 0 )
				{
					FilletTaxiway newFillet;
					newFillet.SetFilletPoint1( FilletPoint(item1.GetUID(), -absDistToIntersect) );
					newFillet.SetFilletPoint2( FilletPoint(item2.GetUID(), -absDistToIntersect) );
					newFillet.SetIntersectNode( node );
					vFillets.AddItem( newFillet);
				}
			}
			

		}
		if( distInItem2 < lenPath2 -1)
		{
			ARCVector3 vDir2 = item2Path.GetDistPoint(distInItem2+1) - pt2;
			double cosZeda  = ( vDir1.GetCosOfTwoVector(vDir2) );
			double ctanhalfZeda = sqrt( (1.0+cosZeda)/(1.0-cosZeda));
			

			if( ctanhalfZeda < dMaxctanhalfAngle /*&& ctanhalfZeda > dMinRadiusScale */)
			{
				ctanhalfZeda  = min(ctanhalfZeda, dMaxRadiusScale);
				//ctanhalfZeda = max(ctanhalfZeda, dMinRadiusScale);

				DistanceUnit absDistToIntersect = FilletRadius * ctanhalfZeda;

				if( distInItem1-absDistToIntersect > 0 && distInItem2 + absDistToIntersect < lenPath2 )
				{
					FilletTaxiway newFillet;
					newFillet.SetFilletPoint1( FilletPoint(item1.GetUID(), -absDistToIntersect) );
					newFillet.SetFilletPoint2( FilletPoint(item2.GetUID(), absDistToIntersect) );
					newFillet.SetIntersectNode( node );
					vFillets.AddItem( newFillet);
				}
			}
		}
	}
	if( distInItem1 < lenPath1 -1 )
	{
		ARCVector3 vDir1 = item1Path.GetDistPoint(distInItem1+1) - pt1;
		if( distInItem2 > 1)
		{
			ARCVector3 vDir2 = item2Path.GetDistPoint(distInItem2-1) - pt2;
			double cosZeda  = ( vDir1.GetCosOfTwoVector(vDir2) );
			double ctanhalfZeda = sqrt( (1.0+cosZeda)/(1.0-cosZeda));
			
			if( ctanhalfZeda < dMaxctanhalfAngle /*&& ctanhalfZeda > dMinRadiusScale */)
			{
				ctanhalfZeda  = min(ctanhalfZeda, dMaxRadiusScale);
				//ctanhalfZeda = max(ctanhalfZeda, dMinRadiusScale);

				DistanceUnit absDistToIntersect = FilletRadius * ctanhalfZeda;

				if(distInItem1+absDistToIntersect< lenPath1 && distInItem2 - absDistToIntersect > 0 ) 
				{
					FilletTaxiway newFillet;
					newFillet.SetFilletPoint1( FilletPoint(item1.GetUID(), absDistToIntersect) );
					newFillet.SetFilletPoint2( FilletPoint(item2.GetUID(), -absDistToIntersect) );
					newFillet.SetIntersectNode( node );
					vFillets.AddItem( newFillet);
				}
			}
		}
		if( distInItem2 < lenPath2 -1)
		{
			ARCVector3 vDir2 = item2Path.GetDistPoint(distInItem2+1) - pt2;
			double cosZeda  = ( vDir1.GetCosOfTwoVector(vDir2) );
			double ctanhalfZeda = sqrt( (1.0+cosZeda)/(1.0-cosZeda));
			
			if( ctanhalfZeda < dMaxctanhalfAngle /*&& ctanhalfZeda > dMinRadiusScale */)
			{
				ctanhalfZeda  = min(ctanhalfZeda, dMaxRadiusScale);
				//ctanhalfZeda = max(ctanhalfZeda, dMinRadiusScale);

				DistanceUnit absDistToIntersect = FilletRadius * ctanhalfZeda;
				if(distInItem1+absDistToIntersect<lenPath1 && distInItem2+absDistToIntersect<lenPath2  )
				{
					FilletTaxiway newFillet;
					newFillet.SetFilletPoint1( FilletPoint(item1.GetUID(), absDistToIntersect) );
					newFillet.SetFilletPoint2( FilletPoint(item2.GetUID(), absDistToIntersect) );
					newFillet.SetIntersectNode( node );
					vFillets.AddItem( newFillet);
				}
			}
			
		}
	}
}

void CalNodeFillet(const IntersectionNode& _node , FilletTaxiwayList& vFillets)
{
	IntersectionNode node = _node;
	std::vector<TaxiwayIntersectItem*> vTaxiItems = node.GetTaxiwayIntersectItemList();
	for(int i=0; i<(int)vTaxiItems.size()-1;i++)
	{
		DistanceUnit Width1 = vTaxiItems[i]->GetTaxiway()->GetWidth();
		for(int j=i+1; j< (int)vTaxiItems.size();j++ )
		{
			DistanceUnit Width2 = vTaxiItems[j]->GetTaxiway()->GetWidth();			
			CalNodeFillet( node , *vTaxiItems[i], *vTaxiItems[j], (Width1+Width2)*0.5, vFillets );	
		}
	}	

	std::vector<RunwayIntersectItem*> vRunwayItems = node.GetRunwayIntersectItemList();
	for(int i=0; i<(int)vTaxiItems.size();i++)
	{
		DistanceUnit Width1 = vTaxiItems[i]->GetTaxiway()->GetWidth();
		for(int j=0;j<(int)vRunwayItems.size();j++)
		{
			DistanceUnit Width2 = vRunwayItems[j]->GetRunway()->GetWidth();
			CalNodeFillet( node, *vRunwayItems[j], *vTaxiItems[i], (Width1+Width2)*0.5, vFillets );
		}
	}


}



void FilletTaxiwaysInAirport::UpdateFillets( int nARPID,const IntersectionNodesInAirport& vNodes, const std::vector<int>& vChangeNodeIndexs )
{
	
	std::vector<int> vNoChangeNodeIDs;
	for(int i=0;i<vNodes.GetCount();i++)
	{
		if( vChangeNodeIndexs.end() == std::find(vChangeNodeIndexs.begin(), vChangeNodeIndexs.end(), i) )
		{
			const IntersectionNode& theNode = vNodes.NodeAt(i);
			vNoChangeNodeIDs.push_back(theNode.GetID());
		}
	}

	std::map<int, FilletTaxiwayList> vChangeFiletMap;
	//
	FilletTaxiwayList vNewFillets;
	for(int i=0;i<m_vFilletWays.GetCount();i++)
	{
		FilletTaxiway& theFilet = m_vFilletWays.FilletAt(i);
		int nNodeID = theFilet.GetIntersectNode().GetID();
		if( vNoChangeNodeIDs.end() != std::find(vNoChangeNodeIDs.begin(), vNoChangeNodeIDs.end(),nNodeID ) )
		{
			vNewFillets.AddItem(theFilet);
		}
		else
		{
			vChangeFiletMap[nNodeID].m_vData.push_back(theFilet);
		}
	}
	
	//calculate the new fillets with
	for(int iNodeIdx=0;iNodeIdx<(int)vChangeNodeIndexs.size();iNodeIdx++)
	{
		
		const IntersectionNode& theNode = vNodes.NodeAt(vChangeNodeIndexs[iNodeIdx]);		
		int nNodeID = theNode.GetID();
		FilletTaxiwayList& theOldFiletList = vChangeFiletMap[nNodeID];

		FilletTaxiwayList vChangedFillets;		
		CalNodeFillet(theNode, vChangedFillets);		
		for(int i=0;i< vChangedFillets.GetCount();i++)
		{
			FilletTaxiway& theNewFilet = vChangedFillets.FilletAt(i);
			for(int j=0;j< theOldFiletList.GetCount();j++ )
			{
				FilletTaxiway& theOldFilet = theOldFiletList.FilletAt(j);
				if( theNewFilet.IsOverLapWith(theOldFilet) )
				{					
					theNewFilet.SetID( theOldFilet.GetID() );
					theNewFilet.GetFilletPoint1().SetUsrDist( theOldFilet.GetFilletPoint1().GetUsrDist() );
					theNewFilet.GetFilletPoint2().SetUsrDist( theOldFilet.GetFilletPoint2().GetUsrDist() );
					theOldFiletList.Remove(j);
					break;
				}
			
			}
			theNewFilet.SaveData(nARPID);		
			vNewFillets.AddItem(theNewFilet);
		}
		for(int i=0;i< theOldFiletList.GetCount();i++)
		{
			FilletTaxiway& theOldFilet = theOldFiletList.FilletAt(i);
			theOldFilet.DeleteData();
		}
		
	}

	m_vFilletWays = vNewFillets;
}