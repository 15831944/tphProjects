#include "StdAfx.h"
#include ".\intersectedstretch.h"
#include "Stretch.h"
#include "IntersectionNodesInAirport.h"
#include "InputAirside.h"
#include "AirsideImportExportManager.h"
#include "IntersectItem.h"
#include "CircleStretch.h"
#include "../Common/Path2008.h"

/*****************************************/
/*         IntersectedStretchPoint       */
/*****************************************/

IntersectedStretchPoint::IntersectedStretchPoint(void)
{
	m_dDistToIntersected = 0.0;
    m_dDisToUser = 0.0;
	m_nIntersectItemID = -1;
}

IntersectedStretchPoint::IntersectedStretchPoint(int nItemID, DistanceUnit distToIntersect)
{
	m_dDistToIntersected = distToIntersect;
	m_dDisToUser = 0.0;
	m_nIntersectItemID = nItemID;
}

DistanceUnit IntersectedStretchPoint::GetDistToIntersect() const
{
	return m_dDistToIntersected;
}

void IntersectedStretchPoint::SetDistToIntersect(DistanceUnit disToIntersect)
{
	m_dDistToIntersected = disToIntersect;
}

int IntersectedStretchPoint::GetIntersectItemID() const
{
	return m_nIntersectItemID;
}

void IntersectedStretchPoint::SetIntersectItemID(int nItemID)
{
	m_nIntersectItemID = nItemID;
}

DistanceUnit IntersectedStretchPoint::GetDistToUser()const
{
	return m_dDisToUser;
}

void IntersectedStretchPoint::SetDisToUser(const DistanceUnit disToUser)
{
	m_dDisToUser = disToUser;
}

bool IntersectedStretchPoint::IsAtSameSide(const IntersectedStretchPoint& otherStretch)const
{
	return (m_dDistToIntersected<=0) == (otherStretch.m_dDistToIntersected<=0);
}

/************************************************************************/
/*                 IntersectedStretch                                   */
/************************************************************************/

IntersectedStretch::IntersectedStretch(void)
{
	m_nID = -1;
}

IntersectedStretch::IntersectedStretch( const IntersectionNode& node )
{
	m_nID = -1;
	m_intersectioNode = node;
}

IntersectedStretch::IntersectedStretch( const IntersectedStretch& otherStretch )
{
	*this = otherStretch;
}

IntersectedStretch::~IntersectedStretch(void)
{
}

//void IntersectedStretch::ReadData(CADORecordset& Recordset)
//{
//
//}

void IntersectedStretch::GetSelectSQL(int nAirportID,CString& strSQL)const
{
	strSQL.Format(_T("SELECT * FROM INTERSECTEDSTRETCH WHERE (AIRPORTID = %d)"),nAirportID);
}

void IntersectedStretch::GetDeleteSQL(CString& strSQL)const
{
	strSQL.Format(_T("DELETE FROM INTERSECTEDSTRETCH WHERE (ID = %d)"),m_nID);
}

void IntersectedStretch::GetInsertSQL(int nParentID,CString& strSQL)const
{
	strSQL.Format(_T("INSERT INTO INTERSECTEDSTRETCH (INTERSECTNODEID, DISTTOINTERSECTNODE1, DISTTOINTERSECTNODE2, AIRPORTID, INTERSECTITEMID1, INTERSECTITEMID2, USERDIST1, USERDIST2) VALUES (%d, %f, %f, %d, %d, %d, %f, %f)"),m_intersectioNode.GetID(),m_fristPoint.GetDistToIntersect(),m_secondPoint.GetDistToIntersect(),nParentID,m_fristPoint.GetIntersectItemID(),m_secondPoint.GetIntersectItemID(),  m_fristPoint.m_dDisToUser, m_secondPoint.m_dDisToUser);
}

void IntersectedStretch::GetUpdateSQL(CString& strSQL)const
{
	strSQL.Format(_T("UPDATE INTERSECTEDSTRETCH SET INTERSECTNODEID = %d, DISTTOINTERSECTNODE1 = %f, DISTTOINTERSECTNODE2 = %f, INTERSECTITEMID1 = %d, INTERSECTITEMID2 = %d,  USERDIST1 = %f, USERDIST2 = %f WHERE (ID = %d)"), m_intersectioNode.GetID(),m_fristPoint.GetDistToIntersect(), m_secondPoint.GetDistToIntersect(), m_fristPoint.GetIntersectItemID(), m_secondPoint.GetIntersectItemID(), m_fristPoint.m_dDisToUser, m_secondPoint.m_dDisToUser,m_nID);
}

void IntersectedStretch::SetFristIntersectedStretchPoint(const IntersectedStretchPoint& point)
{
	m_fristPoint = point;
}

void IntersectedStretch::SetSecondIntersectedStretchPoint(const IntersectedStretchPoint& point)
{
	m_secondPoint = point;
}

void IntersectedStretch::SetIntersectionNode(const IntersectionNode& node)
{
	m_intersectioNode = node;
}

IntersectedStretchPoint& IntersectedStretch::GetFristIntersectedStretchPoint()
{
	return m_fristPoint;
}

IntersectedStretchPoint& IntersectedStretch::GetSecondIntersectedStretchPoint()
{
	return m_secondPoint;
}

IntersectionNode& IntersectedStretch::GetIntersectionNode()
{
	return m_intersectioNode;
}

CPoint2008 IntersectedStretch::GetFristIntersectedStretchPointPos()
{
	IntersectItem * pItem = m_intersectioNode.GetItemByID(m_fristPoint.GetIntersectItemID());
	DistanceUnit disTance = m_fristPoint.GetDistToIntersect()/* + m_fristPoint.GetDistToUser() */+ pItem->GetDistInItem();
	ALTObject * pObject = pItem->GetObject();
	if(pObject && pObject->GetType() == ALT_STRETCH)
	{
		return ((Stretch*)pObject)->GetPath().GetDistPoint(disTance);
	}
	if(pObject && pObject->GetType() == ALT_CIRCLESTRETCH)
	{
		CPath2008 realPath;
		((CircleStretch*)pObject)->getPath().PathSupplement(realPath);
		return realPath.GetDistPoint(disTance);
	}
	ASSERT(false);
	return m_intersectioNode.GetPosition();
}

CPoint2008 IntersectedStretch::GetSecondIntersectedStretchPointPos()
{
	IntersectItem * pItem = m_intersectioNode.GetItemByID(m_secondPoint.GetIntersectItemID());
	DistanceUnit disTance = m_secondPoint.GetDistToIntersect()/* + m_secondPoint.GetDistToUser()*/ + pItem->GetDistInItem();
	ALTObject * pObject = pItem->GetObject();
	if(pObject && pObject->GetType() == ALT_STRETCH)
	{
		return ((Stretch* )pObject)->GetPath().GetDistPoint(disTance);
	}
	if(pObject && pObject->GetType() == ALT_CIRCLESTRETCH)
	{
		CPath2008 realPath;
		((CircleStretch*)pObject)->getPath().PathSupplement(realPath);
		return realPath.GetDistPoint(disTance);
	}
	ASSERT(false);
	return  m_intersectioNode.GetPosition();
}

DistanceUnit IntersectedStretch::GetFristPointDisInPath()
{
	IntersectItem * pItem = m_intersectioNode.GetItemByID(m_fristPoint.GetIntersectItemID());
	ASSERT(pItem);
	DistanceUnit disTance = m_fristPoint.GetDistToIntersect()/* + m_fristPoint.GetDistToUser()*/ + pItem->GetDistInItem();
	return disTance;
}

DistanceUnit IntersectedStretch::GetSecondPointDisInPath()
{
	IntersectItem *pItem = m_intersectioNode.GetItemByID(m_secondPoint.GetIntersectItemID());
	ASSERT(pItem);
	DistanceUnit disTance = m_secondPoint.GetDistToIntersect()/* + m_secondPoint.GetDistToUser() */+ pItem->GetDistInItem();
	return disTance;
}

CPath2008 IntersectedStretch::GetStretchPathOfFirstPoint()
{
	IntersectItem * pItem = m_intersectioNode.GetItemByID(m_fristPoint.GetIntersectItemID());
	if(pItem)
	{
		ALTObject * pObject = pItem->GetObject();
		if (pObject && pObject->GetType() == ALT_STRETCH)
		{
			return ((Stretch* )pObject)->GetPath();
		}
		if (pObject && pObject->GetType() == ALT_CIRCLESTRETCH)
		{
			CPath2008 realPath;
			((CircleStretch* )pObject)->getPath().PathSupplement(realPath);
			return realPath;
		}
	}
	ASSERT(false);
	return CPath2008();
}

CPath2008 IntersectedStretch::GetStretchPathOfSecondPoint()
{
	IntersectItem * pItem = m_intersectioNode.GetItemByID(m_secondPoint.GetIntersectItemID());
	if(pItem)
	{
		ALTObject * pObject = pItem->GetObject();
		if (pObject && pObject->GetType() == ALT_STRETCH)
		{
			return ((Stretch *)pObject)->GetPath();
		}
		if (pObject && pObject->GetType() == ALT_CIRCLESTRETCH)
		{
			CPath2008 realPath;
			((CircleStretch *)pObject)->getPath().PathSupplement(realPath);
		    return realPath;
		}
	}
	ASSERT(false);
	return CPath2008();
}

CPoint2008 IntersectedStretch::GetExtentFristPoint()
{
	IntersectItem * pItem = m_intersectioNode.GetItemByID(m_fristPoint.GetIntersectItemID());
	ALTObject * pObject = pItem->GetObject();
	if(pObject && pObject->GetType() == ALT_STRETCH)
	{
		return ((Stretch* )pObject)->GetPath().GetDistPoint(GetFristPointDisInPath()+((m_fristPoint.GetDistToIntersect()>0)?1:-1));
	}
	if(pObject && pObject->GetType() == ALT_CIRCLESTRETCH)
	{
		CPath2008 realPath;
		((CircleStretch* )pObject)->getPath().PathSupplement(realPath);
		return realPath.GetDistPoint(GetFristPointDisInPath()+((m_fristPoint.GetDistToIntersect()>0)?1:-1));
	}
	CPoint2008 vDir = CPoint2008(GetFristIntersectedStretchPointPos() - m_intersectioNode.GetPosition());
	vDir.Normalize();
	return GetFristIntersectedStretchPointPos() + vDir;
	
}

CPoint2008 IntersectedStretch::GetExtentSecondPoint()
{
	IntersectItem * pItem = m_intersectioNode.GetItemByID(m_secondPoint.GetIntersectItemID());
	ALTObject * pObject = pItem->GetObject();
	if(pObject && pObject->GetType() == ALT_STRETCH)
	{
		return ((Stretch* )pObject)->getPath().GetDistPoint(GetSecondPointDisInPath()+((m_secondPoint.GetDistToIntersect()>0)?1:-1));
	}
	if(pObject && pObject->GetType() == ALT_CIRCLESTRETCH)
	{
		CPath2008 realPath;
		((CircleStretch* )pObject)->getPath().PathSupplement(realPath);
		return realPath.GetDistPoint(GetSecondPointDisInPath()+((m_secondPoint.GetDistToIntersect()>0)?1:-1));
	}
	CPoint2008 vDir = CPoint2008(GetSecondIntersectedStretchPointPos() - m_intersectioNode.GetPosition());
	vDir.Normalize();
	return GetSecondIntersectedStretchPointPos() + vDir;

}


ARCVector3 IntersectedStretch::GetFristStretchDir()
{
	IntersectItem * pItem = m_intersectioNode.GetItemByID( m_fristPoint.GetIntersectItemID() );
	ASSERT( pItem);

	DistanceUnit distInObj  = m_fristPoint.GetDistToIntersect()/* + m_fristPoint.GetDistToUser() */+ pItem->GetDistInItem();
	ALTObject * pObj = pItem->GetObject();
	if(pObj && pObj->GetType() == ALT_STRETCH)
	{
		return ((Stretch*)pObj)->GetPath().GetDistPoint(distInObj +1) - ((Stretch*)pObj)->GetPath().GetDistPoint(distInObj);
	}
	if(pObj && pObj->GetType() == ALT_CIRCLESTRETCH)
	{
		CPath2008 realPath;
        ((CircleStretch*)pObj)->getPath().PathSupplement(realPath);
		return realPath.GetDistPoint(distInObj +1) - realPath.GetDistPoint(distInObj);
	}

	ASSERT(false);
	return CPoint2008(0,0,0);
}


ARCVector3 IntersectedStretch::GetSecondStretchDir()
{
	IntersectItem * pItem = m_intersectioNode.GetItemByID( m_secondPoint.GetIntersectItemID() );
	ASSERT( pItem);

	DistanceUnit distInObj  = m_secondPoint.GetDistToIntersect()/* + m_secondPoint.GetDistToUser()*/ + pItem->GetDistInItem();
	ALTObject * pObj = pItem->GetObject();
	if(pObj && pObj->GetType() == ALT_STRETCH)
	{
		return ((Stretch*)pObj)->GetPath().GetDistPoint(distInObj +1) - ((Stretch*)pObj)->GetPath().GetDistPoint(distInObj);
	}
	if(pObj && pObj->GetType() == ALT_CIRCLESTRETCH)
	{
		CPath2008 realPath;
        ((CircleStretch*)pObj)->getPath().PathSupplement(realPath);
		return realPath.GetDistPoint(distInObj +1) - realPath.GetDistPoint(distInObj);
	}
	ASSERT(false);
	return ARCVector3::ZERO;
}


BOOL IntersectedStretch::GetControlPoint(std::vector<CPoint2008>& vPoint)
{
	vPoint.clear();
	DistanceUnit disFristPoint = GetFristPointDisInPath();
	DistanceUnit disSecondPoint = GetSecondPointDisInPath();

	DistanceUnit disIntersectFrist = m_intersectioNode.GetItemByID(m_fristPoint.GetIntersectItemID())->GetDistInItem();
	DistanceUnit disIntersectSceond = m_intersectioNode.GetItemByID(m_secondPoint.GetIntersectItemID())->GetDistInItem();

	CPath2008 fristPath = GetStretchPathOfFirstPoint();
	CPath2008 secondPath = GetStretchPathOfSecondPoint();
	
	std::vector<CPoint2008> vPointFrist;
	std::vector<CPoint2008> vPointSecond;

	fristPath.GetInPathPoints(disFristPoint,disIntersectFrist,vPointFrist);
	secondPath.GetInPathPoints(disIntersectSceond,disSecondPoint,vPointSecond);
    
	vPoint.push_back(fristPath.GetDistPoint(disFristPoint));
	if (vPointFrist.size() )
	{
		vPoint.push_back(*vPointFrist.begin());
	}
		
	vPoint.push_back(m_intersectioNode.GetPosition());		
		
	if(vPointSecond.size())
	{
		vPoint.push_back(*vPointSecond.rbegin());
	}
	vPoint.push_back(secondPath.GetDistPoint(disSecondPoint));

	if (vPoint.size())
	{
		return TRUE;
	}
	else
		return FALSE;
}

DistanceUnit IntersectedStretch::GetFristWidth()
{
	IntersectItem * pItem = m_intersectioNode.GetItemByID(m_fristPoint.GetIntersectItemID());
	ALTObject * pObject = pItem->GetObject();
	if (pObject && pObject->GetType() == ALT_STRETCH)
	{
		return ((Stretch*)pObject)->GetLaneWidth()*((Stretch*)pObject)->GetLaneNumber();
	}
	if (pObject && pObject->GetType() == ALT_CIRCLESTRETCH)
	{
		return ((CircleStretch*)pObject)->GetLaneWidth()*((CircleStretch*)pObject)->GetLaneNumber();
	}
	ASSERT(false);
	return 0;
}

DistanceUnit IntersectedStretch::GetSecondWidth()
{
	IntersectItem * pItem = m_intersectioNode.GetItemByID(m_secondPoint.GetIntersectItemID());
	ALTObject * pObject = pItem->GetObject();
	if (pObject && pObject->GetType() == ALT_STRETCH)
	{
		return ((Stretch*)pObject)->GetLaneWidth()*((Stretch*)pObject)->GetLaneNumber();
	}
	if (pObject && pObject->GetType() == ALT_CIRCLESTRETCH)
	{
		return ((CircleStretch*)pObject)->GetLaneWidth() * ((CircleStretch*)pObject)->GetLaneNumber();
	}
	ASSERT(false);
	return 0;
}

DistanceUnit IntersectedStretch::GetMinWidth()
{
	IntersectItem * pItem1 = m_intersectioNode.GetItemByID(m_fristPoint.GetIntersectItemID());
	IntersectItem * pItem2 = m_intersectioNode.GetItemByID(m_secondPoint.GetIntersectItemID());
	ALTObject * pObject1 = pItem1->GetObject();
	ALTObject * pObject2 = pItem2->GetObject();
	if (pObject1&&pObject2&&pObject1->GetType() == ALT_STRETCH&&pObject2->GetType() == ALT_STRETCH)
	{
		DistanceUnit disTance1 = ((Stretch*)pObject1)->GetLaneWidth()*((Stretch*)pObject1)->GetLaneNumber();
		DistanceUnit disTance2 = ((Stretch*)pObject2)->GetLaneWidth()*((Stretch*)pObject2)->GetLaneNumber();
		if (disTance1<disTance2)
			return disTance1;
		else
			return disTance2;
	}
	if (pObject1&&pObject2&&(pObject1->GetType() == ALT_CIRCLESTRETCH&&pObject2->GetType() == ALT_STRETCH))
	{
		DistanceUnit disTance1 = ((CircleStretch*)pObject1)->GetLaneWidth()*((CircleStretch*)pObject1)->GetLaneNumber();
		DistanceUnit disTance2 = ((Stretch*)pObject2)->GetLaneWidth()*((Stretch*)pObject2)->GetLaneNumber();
		if (disTance1<disTance2)
			return disTance1;
		else
			return disTance2;
	}
	if (pObject1&&pObject2&&(pObject1->GetType() == ALT_STRETCH&&pObject2->GetType() == ALT_CIRCLESTRETCH))
	{
		DistanceUnit disTance1 = ((Stretch*)pObject1)->GetLaneWidth()*((Stretch*)pObject1)->GetLaneNumber();
		DistanceUnit disTance2 = ((CircleStretch*)pObject2)->GetLaneWidth()*((CircleStretch*)pObject2)->GetLaneNumber();
		if (disTance1<disTance2)
			return disTance1;
		else
			return disTance2;
	}
	return 0.0;
}

bool IntersectedStretch::IsOverLapWith(const IntersectedStretch& otherWay )const 
{
	if(!m_intersectioNode.IsIdentical(otherWay.m_intersectioNode))
		return false;

	if (m_fristPoint.GetIntersectItemID() == otherWay.m_fristPoint.GetIntersectItemID()
		&& m_secondPoint.GetIntersectItemID() == otherWay.m_secondPoint.GetIntersectItemID())
	{
		if (m_fristPoint.IsAtSameSide(otherWay.m_fristPoint)
			&& m_secondPoint.IsAtSameSide(otherWay.m_secondPoint))
			return true;
	}
	if (m_secondPoint.GetIntersectItemID() == otherWay.m_fristPoint.GetIntersectItemID()
		&&m_fristPoint.GetIntersectItemID() == otherWay.m_secondPoint.GetIntersectItemID())
	{
		if (m_fristPoint.IsAtSameSide(otherWay.m_secondPoint)
			&& m_secondPoint.IsAtSameSide(otherWay.m_fristPoint))
		    return true;
	} 
	return false;
}

int IntersectedStretch::GetObject1ID()
{
	IntersectItem * pItem = m_intersectioNode.GetItemByID( m_fristPoint.GetIntersectItemID() );
	ASSERT( pItem);
	return pItem->GetObjectID();
}

int IntersectedStretch::GetObject2ID()
{
	IntersectItem * pItem = m_intersectioNode.GetItemByID( m_secondPoint.GetIntersectItemID() );
	ASSERT( pItem);
	return pItem->GetObjectID();
}



void IntersectedStretch::ExportData( CAirsideExportFile& exportFile )
{
	exportFile.getFile().writeInt(m_intersectioNode.GetID());
//	exportFile.getFile().writeInt((int)m_bActive);
	exportFile.getFile().writeFloat((float)GetFristIntersectedStretchPoint().m_dDistToIntersected);
	exportFile.getFile().writeFloat((float)GetSecondIntersectedStretchPoint().m_dDistToIntersected);
	exportFile.getFile().writeFloat((float)GetFristIntersectedStretchPoint().m_dDisToUser);
	exportFile.getFile().writeFloat((float)GetSecondIntersectedStretchPoint().m_dDisToUser);
	exportFile.getFile().writeInt( GetFristIntersectedStretchPoint().m_nIntersectItemID );
	exportFile.getFile().writeInt( GetSecondIntersectedStretchPoint().m_nIntersectItemID);
	exportFile.getFile().writeInt(0);
	exportFile.getFile().writeLine();
}

void IntersectedStretch::ImportData( CAirsideImportFile& importFile )
{
	int nNodeID ;
	importFile.getFile().getInteger(nNodeID);	
	int nID = importFile.GetIntersectionIndexMap(nNodeID);
	m_intersectioNode.SetID(nID);
	//int bActive;
	//importFile.getFile().getInteger(bActive);
	//m_bActive = (bActive!=0);

	importFile.getFile().getFloat(m_fristPoint.m_dDistToIntersected);
	importFile.getFile().getFloat(m_secondPoint.m_dDistToIntersected);
	importFile.getFile().getFloat(m_fristPoint.m_dDisToUser);
	importFile.getFile().getFloat(m_secondPoint.m_dDisToUser);

	int nItem1ID;
	importFile.getFile().getInteger(nItem1ID);
	nItem1ID = importFile.GetIntersectItemNewID(nItem1ID);
	m_fristPoint.SetIntersectItemID(nItem1ID); 

	int nItem2ID;
	importFile.getFile().getInteger(nItem2ID);
	nItem2ID = importFile.GetIntersectItemNewID(nItem2ID);
	m_secondPoint.SetIntersectItemID(nItem2ID);

	int nARPID;
	importFile.getFile().getInteger(nARPID);	
	nARPID = importFile.GetAirportNewIndex(nARPID);
	m_intersectioNode.SetAirportID( nARPID );

	importFile.getFile().getLine();

	if(nItem1ID >=0 && nItem2ID >=0 )
		SaveDataToDB();
}

void IntersectedStretch::SaveDataToDB()
{
	SaveData(m_intersectioNode.GetAirportID());
}

void IntersectedStretch::ReadAirportStretchs( int nAirportID ,const IntersectionNodesInAirport& vnodes,std::vector<IntersectedStretch>& vStretches)
{
	//read intersection Node

	long nItemCount =0;
	CADORecordset adoRecordset;
	IntersectedStretch newStretch;
	CString selectSQL;
	newStretch.GetSelectSQL(nAirportID,selectSQL);
	CADODatabase::ExecuteSQLStatement(selectSQL,nItemCount,adoRecordset);
	while (!adoRecordset.IsEOF())
	{	
		newStretch.ReadData(adoRecordset,vnodes);
		if(newStretch.GetIntersectionNode().GetID()>=0)
			vStretches.push_back(newStretch);
		else 
			newStretch.DeleteData();

		adoRecordset.MoveNextData();
	}
}

void IntersectedStretch::ReadData(CADORecordset& recordset,const IntersectionNodesInAirport& vnodes )
{
	recordset.GetFieldValue(_T("ID"),m_nID);

	int nodeID;
	recordset.GetFieldValue(_T("INTERSECTNODEID"),nodeID);
	m_intersectioNode = vnodes.GetNodeByID(nodeID);

	int itemID1;
	recordset.GetFieldValue(_T("INTERSECTITEMID1"),itemID1);
	m_fristPoint.SetIntersectItemID(itemID1);

    int itemID2;
	recordset.GetFieldValue(_T("INTERSECTITEMID2"),itemID2);
	m_secondPoint.SetIntersectItemID(itemID2);

	//float dis1;
	recordset.GetFieldValue(_T("DISTTOINTERSECTNODE1"),m_fristPoint.m_dDistToIntersected);
	//m_fristPoint.SetDistToIntersect(dis1);
   
	//float dis2;
	recordset.GetFieldValue(_T("DISTTOINTERSECTNODE2"),m_secondPoint.m_dDistToIntersected);
	//m_secondPoint.SetDistToIntersect(dis2);

	recordset.GetFieldValue(_T("USERDIST1"),m_fristPoint.m_dDisToUser);

	recordset.GetFieldValue(_T("USERDIST2"),m_secondPoint.m_dDisToUser);
}


void IntersectedStretch::InitFromDBRecordset( CADORecordset& recordset )
{
	recordset.GetFieldValue(_T("ID"),m_nID);
	int nodeID;
	recordset.GetFieldValue(_T("INTERSECTNODEID"),nodeID);
	m_intersectioNode.ReadData(nodeID);

	int ItemID1;
	recordset.GetFieldValue(_T("INTERSECTITEMID1"),ItemID1);
	int ItemID2;
	recordset.GetFieldValue(_T("INTERSECTITEMID2"),ItemID2);
	m_fristPoint.SetIntersectItemID(ItemID1);
	m_secondPoint.SetIntersectItemID(ItemID2);

	//int bActive;
	//recordset.GetFieldValue(_T("ACTIVE"),bActive);
	//m_bActive = (bActive != 0);

	recordset.GetFieldValue(_T("DISTTOINTERSECTNODE1"), m_fristPoint.m_dDistToIntersected);
	recordset.GetFieldValue(_T("DISTTOINTERSECTNODE2"), m_secondPoint.m_dDistToIntersected);
	recordset.GetFieldValue(_T("USERDIST1"), m_fristPoint.m_dDisToUser);
	recordset.GetFieldValue(_T("USERDIST2"), m_secondPoint.m_dDisToUser);
}

/************************************************************************/
/*                   IntersectedStretchInAirport                        */
/************************************************************************/

IntersectedStretchInAirport::IntersectedStretchInAirport()
{

}

IntersectedStretchInAirport::~IntersectedStretchInAirport()
{

}


void CalculateStretchOfNode(const IntersectionNode& node, std::vector<IntersectedStretch>& newStretchInAirport,const DistanceUnit m_dSegma,IntersectItem& item1, IntersectItem& item2)
{
	CPath2008 item1Path = item1.GetPath();
	CPath2008 item2Path = item2.GetPath();

	if (item2.GetType() == IntersectItem::CIRCLESTRETCH_INTERSECT)
	{
		CPath2008 realPath;
		item2Path.PathSupplement(realPath);
		item2Path = realPath;
	}
	
	DistanceUnit lenPath1 = item1Path.GetTotalLength();
	DistanceUnit lenPath2 = item2Path.GetTotalLength();

	DistanceUnit distInItem1 = item1.GetDistInItem();
	DistanceUnit distInItem2 = item2.GetDistInItem();

	CPoint2008 pt1 = item1Path.GetDistPoint(distInItem1);
	CPoint2008 pt2 = item2Path.GetDistPoint(distInItem2);

	if( distInItem1 > 1 )
	{
		CPoint2008 vDir1 = CPoint2008(item1Path.GetDistPoint(distInItem1 -1 ) - pt1);
		if( distInItem2 > 1)
		{
			CPoint2008 vDir2 = CPoint2008(item2Path.GetDistPoint(distInItem2-1) - pt2);
			double cosZeda  = ( vDir1.GetCosOfTwoVector(vDir2) );
			double ctanhalfZeda = sqrt( (1.0+cosZeda)/(1.0-cosZeda));
			double sinZeda = sqrt( (1.0+cosZeda)*(1.0-cosZeda) );		
			double dAlaph = min(0.8/sinZeda, 2.0);

			DistanceUnit absDistToIntersect = m_dSegma*dAlaph;//m_dSegma * ctanhalfZeda;

			if( distInItem1 - absDistToIntersect > 0 && distInItem2 - absDistToIntersect > 0 && absDistToIntersect < m_dSegma*2.0)
			{
				IntersectedStretch newStretch;
				newStretch.SetFristIntersectedStretchPoint(IntersectedStretchPoint(item1.GetUID(),-absDistToIntersect));
				newStretch.SetSecondIntersectedStretchPoint(IntersectedStretchPoint(item2.GetUID(),-absDistToIntersect));
				newStretch.SetIntersectionNode(node);
				newStretchInAirport.push_back(newStretch);

			}

		}

		if( distInItem2 < lenPath2 -1)
		{
			CPoint2008 vDir2 = CPoint2008(item2Path.GetDistPoint(distInItem2+1) - pt2);
			double cosZeda  = ( vDir1.GetCosOfTwoVector(vDir2) );
			double ctanhalfZeda = sqrt( (1.0+cosZeda)/(1.0-cosZeda));
			double sinZeda = sqrt( (1.0+cosZeda)*(1.0-cosZeda) );
			double dAlaph = min(0.8/sinZeda, 2.0);

			DistanceUnit absDistToIntersect = m_dSegma*dAlaph;//m_dSegma * ctanhalfZeda;

			if( distInItem1-absDistToIntersect > 0 && distInItem2 + absDistToIntersect < lenPath2 && absDistToIntersect < m_dSegma*2.0)
			{
				IntersectedStretch newStretch;
				newStretch.SetFristIntersectedStretchPoint(IntersectedStretchPoint(item1.GetUID(),-absDistToIntersect));
				newStretch.SetSecondIntersectedStretchPoint(IntersectedStretchPoint(item2.GetUID(),absDistToIntersect));
				newStretch.SetIntersectionNode(node);
				newStretchInAirport.push_back(newStretch);
			}
		}
	}
	if( distInItem1 < lenPath1 -1 )
	{
		CPoint2008 vDir1 = CPoint2008(item1Path.GetDistPoint(distInItem1+1) - pt1);
		if( distInItem2 > 1)
		{
			CPoint2008 vDir2 = CPoint2008(item2Path.GetDistPoint(distInItem2-1) - pt2);
			double cosZeda  = ( vDir1.GetCosOfTwoVector(vDir2) );
			double ctanhalfZeda = sqrt( (1.0+cosZeda)/(1.0-cosZeda));
			double sinZeda = sqrt( (1.0+cosZeda)*(1.0-cosZeda) );
			double dAlaph = min(0.8/sinZeda, 2.0);

			DistanceUnit absDistToIntersect = m_dSegma*dAlaph;//m_dSegma * ctanhalfZeda;

			if(distInItem1+absDistToIntersect< lenPath1 && distInItem2 - absDistToIntersect > 0 && absDistToIntersect < m_dSegma*2.0)
			{
				IntersectedStretch newStretch;
				newStretch.SetFristIntersectedStretchPoint(IntersectedStretchPoint(item1.GetUID(),absDistToIntersect));
				newStretch.SetSecondIntersectedStretchPoint(IntersectedStretchPoint(item2.GetUID(),-absDistToIntersect));
				newStretch.SetIntersectionNode(node);
				newStretchInAirport.push_back(newStretch);
			}
		}

		if( distInItem2 < lenPath2 -1)
		{
			CPoint2008 vDir2 = CPoint2008(item2Path.GetDistPoint(distInItem2+1) - pt2);
			double cosZeda  = ( vDir1.GetCosOfTwoVector(vDir2) );
			double ctanhalfZeda = sqrt( (1.0+cosZeda)/(1.0-cosZeda));
			double sinZeda = sqrt( (1.0+cosZeda)*(1.0-cosZeda) );

			double dAlaph = min(0.8/sinZeda, 2.0);
			
			DistanceUnit absDistToIntersect =  m_dSegma * dAlaph;//m_dSegma * ctanhalfZeda;

			if(distInItem1+absDistToIntersect<lenPath1 && distInItem2+absDistToIntersect<lenPath2 && absDistToIntersect < m_dSegma*2.0)
			{
				IntersectedStretch newStretch;
				newStretch.SetFristIntersectedStretchPoint(IntersectedStretchPoint(item1.GetUID(),absDistToIntersect));
				newStretch.SetSecondIntersectedStretchPoint(IntersectedStretchPoint(item2.GetUID(),absDistToIntersect));
				newStretch.SetIntersectionNode(node);
				newStretchInAirport.push_back(newStretch);
			}
		}
	}
}

void CalculateIntersectedStretchesOfNode(const IntersectionNode& node, std::vector<IntersectedStretch>& newStretchInAirport)
{
	IntersectionNode m_node = node;


	std::vector<StretchIntersectItem*> vIntersectItem = m_node.GetStretchIntersectItem();
	int elementCount = static_cast<int>(vIntersectItem.size());
	for (int i=0; i<elementCount-1; i++)
	{
		DistanceUnit width1 = vIntersectItem[i]->GetStretch()->GetLaneWidth()* vIntersectItem[i]->GetStretch()->GetLaneNumber();
		for(int j=i+1; j<elementCount; j++)
		{
			DistanceUnit width2 = vIntersectItem[j]->GetStretch()->GetLaneWidth()* vIntersectItem[j]->GetStretch()->GetLaneNumber();
			{
				DistanceUnit width = (width1+width2)*0.8;
				CalculateStretchOfNode(node, newStretchInAirport,width,*vIntersectItem[i],*vIntersectItem[j]);
			}
		}
	}


	std::vector<CircleStretchIntersectItem*> vcsIntersectItem = m_node.GetCircleStretchIntersectItemList();
	int nCount = static_cast<int>(vcsIntersectItem.size());
	for (int i=0; i<elementCount;i++)
	{
		StretchIntersectItem* pStretchItem = vIntersectItem[i];
		if(pStretchItem && pStretchItem->GetStretch() )
		{
			DistanceUnit width1 = pStretchItem->GetStretch()->GetLaneWidth() * pStretchItem->GetStretch()->GetLaneNumber();
			for (int j=0;j<nCount; j++)
			{
				CircleStretchIntersectItem* pCsItem = vcsIntersectItem[j];
				if(pCsItem && pCsItem->GetCircleStretch())
				{
					DistanceUnit width2 = pCsItem->GetCircleStretch()->GetLaneNumber() * pCsItem->GetCircleStretch()->GetLaneWidth();
					{
						DistanceUnit width = (width1+width2)*0.8;
						CalculateStretchOfNode(node,newStretchInAirport,width,*pStretchItem,*pCsItem );
					}
				}
				
			
			}
		}		
	}
}

void IntersectedStretchInAirport::CalculateIntersectedStretches(const IntersectionNodesInAirport& nodelist, IntersectedStretchInAirport& newStretchInAirport)
{
	for (int i=0; i<nodelist.GetCount(); i++)
	{
		//CalculateIntersectedStretchesOfNode(nodelist.NodeAt(i),newStretchInAirport);
	}
}





void IntersectedStretchInAirport::AddItem(const IntersectedStretch& intersectedStretch)
{
	int nOverLapItem = FindOverLapStretch(intersectedStretch);
	if (nOverLapItem<0)
	{
		m_vIntersectedStretches.push_back(intersectedStretch);
	}
	else
	{
		m_vIntersectedStretches[nOverLapItem] = intersectedStretch;
	}
}

void IntersectedStretchInAirport::SaveData(int nAirportID)
{
	for (int i=0; i<static_cast<int>(m_vIntersectedStretches.size()); i++)
	{
		m_vIntersectedStretches[i].SaveData(nAirportID);
	}
}

void IntersectedStretch::SetID(int id)
{
	m_nID = id;
}

int IntersectedStretch::GetID() const
{
	return m_nID;
}

void IntersectedStretch::DeleteData()
{
	CString strSQL;
	GetDeleteSQL(strSQL);
	if (strSQL.IsEmpty())
		return;

	CADODatabase::ExecuteSQLStatement(strSQL);
}

void IntersectedStretch::SaveData( int nParentID )
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

IntersectedStretch& IntersectedStretch::operator=( const IntersectedStretch& otherStretch)
{
	m_nID = otherStretch.m_nID;
	m_intersectioNode = otherStretch.m_intersectioNode;
	m_fristPoint = otherStretch.m_fristPoint;
	m_secondPoint = otherStretch.m_secondPoint;
	m_stretchPath = otherStretch.m_stretchPath;

	return *this;
}
void IntersectedStretchInAirport::UpdateStretch(int nARPID,const IntersectionNodesInAirport& vNodes, const std::vector<int>& vChangeNodeIndexs)
{	
	typedef std::vector<IntersectedStretch> IntersectedStretchList;
	
	std::vector<int> vNoChangeNodeIDs;
	for(int i=0;i<vNodes.GetCount();i++)
	{
		if( vChangeNodeIndexs.end() == std::find(vChangeNodeIndexs.begin(), vChangeNodeIndexs.end(), i) )
		{
			const IntersectionNode& theNode = vNodes.NodeAt(i);
			vNoChangeNodeIDs.push_back(theNode.GetID());
		}
	}

	std::map<int, IntersectedStretchList> vChangeStretchMap;
	//
	IntersectedStretchList vNewStretchs;
	for(int i=0;i<(int)m_vIntersectedStretches.size();i++)
	{
		IntersectedStretch& theStretch = m_vIntersectedStretches[i];
		int nNodeID = theStretch.GetIntersectionNode().GetID();
		if( vNoChangeNodeIDs.end() != std::find(vNoChangeNodeIDs.begin(), vNoChangeNodeIDs.end(),nNodeID ) )
		{
			vNewStretchs.push_back(theStretch);
		}
		else
		{
			vChangeStretchMap[nNodeID].push_back(theStretch);
		}
	}

	//calculate the new fillets with
	for(int iNodeIdx=0;iNodeIdx<(int)vChangeNodeIndexs.size();iNodeIdx++)
	{

		const IntersectionNode& theNode = vNodes.NodeAt(vChangeNodeIndexs[iNodeIdx]);		
		int nNodeID = theNode.GetID();
		IntersectedStretchList& theOldStretchList = vChangeStretchMap[nNodeID];

		IntersectedStretchList vChangedStreths;		
		CalculateIntersectedStretchesOfNode(theNode, vChangedStreths);		
		for(int i=0;i< (int)vChangedStreths.size();i++)
		{
			IntersectedStretch& theNewStretch = vChangedStreths[i];
			for(int j=0;j< (int)theOldStretchList.size();j++ )
			{
				IntersectedStretch& theOldStretch = theOldStretchList[j];
				if( theNewStretch.IsOverLapWith(theOldStretch) )
				{					
					theNewStretch.SetID( theOldStretch.GetID() );
					theOldStretchList.erase( theOldStretchList.begin()+j);
					break;
				}

			}
			theNewStretch.SaveData(nARPID);		
			vNewStretchs.push_back(theNewStretch);
		}
		for(int i=0;i< (int)theOldStretchList.size();i++)
		{
			IntersectedStretch& theOldStretch = theOldStretchList[i];
			theOldStretch.DeleteData();
		}

	}

	m_vIntersectedStretches = vNewStretchs;
}

void IntersectedStretchInAirport::ReadData( int nARPID, const IntersectionNodesInAirport& vnodes )
{
	m_vIntersectedStretches.clear();
	IntersectedStretch::ReadAirportStretchs(nARPID,vnodes,m_vIntersectedStretches);
}

int IntersectedStretchInAirport::GetCount()const
{
	return (int)m_vIntersectedStretches.size();
}

IntersectedStretch& IntersectedStretchInAirport::ItemAt(int index)
{
	return m_vIntersectedStretches[index];
}

const IntersectedStretch& IntersectedStretchInAirport::ItemAt( int index ) const
{
	return m_vIntersectedStretches[index];
}
int IntersectedStretchInAirport::FindOverLapStretch(const IntersectedStretch& stretch)
{
	for(int i =0 ; i< GetCount() ;i++)
	{
		IntersectedStretch& theItem = ItemAt(i);
		if( theItem.IsOverLapWith(stretch) )
		{
			return i;
		}
	}

	return -1;
}


void IntersectedStretchInAirport::ReadData( int nAirportID )
{
	m_vIntersectedStretches.clear();
	long nItemCount =0;
	CADORecordset adoRecordset;
	IntersectedStretch newStretch;
	CString selectSQL;
	newStretch.GetSelectSQL(nAirportID,selectSQL);
	CADODatabase::ExecuteSQLStatement(selectSQL,nItemCount,adoRecordset);
	while (!adoRecordset.IsEOF())
	{	
		newStretch.InitFromDBRecordset(adoRecordset);
		AddItem(newStretch);
		adoRecordset.MoveNextData();
	}
}

void IntersectedStretchInAirport::ExportData(CAirsideExportFile& exportFile)
{
	exportFile.getFile().writeField("IntersectedStretch");
	exportFile.getFile().writeLine();
	
	int nPrjID = exportFile.GetProjectID();
	std::vector<int> vAirportIDs;
	InputAirside::GetAirportList(nPrjID,vAirportIDs);

	for(int i=0;i< (int)vAirportIDs.size();i++)
	{
		int nAirportID = vAirportIDs.at(i);
		IntersectedStretchInAirport stretches;
		stretches.ReadData(nAirportID);

		for(int j=0;j< stretches.GetCount();j++)
		{
			stretches.ItemAt(j).ExportData(exportFile);
		}
	}

	exportFile.getFile().endFile();
}


void IntersectedStretchInAirport::ImportData( CAirsideImportFile& importFile )
{
	if( importFile.getVersion() < 1038 )
	{
		return;
	}
	while (!importFile.getFile().isBlank())
	{
		IntersectedStretch stretch;
		stretch.ImportData(importFile);
	}
}

