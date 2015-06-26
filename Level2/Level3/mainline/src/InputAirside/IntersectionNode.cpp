#include "StdAfx.h"
#include "../Database/ADODatabase.h"
#include "AirsideImportExportManager.h"

#include ".\intersectionnode.h"
#include "../Common/GetIntersection.h"
#include "../Common/DistanceLineLine.h"

#include "Runway.h"
#include "Taxiway.h"
#include "Stand.h"
#include "DeicePad.h"
#include "Stretch.h"



IntersectionNode::IntersectionNode( void )
{
	m_nID = -1;	
	m_nAirportID = -1;	
	m_nidx = 0;
}

IntersectionNode::IntersectionNode( const IntersectionNode& node)
{
	*this = node;
}

bool IntersectionNode::operator==(const IntersectionNode& other )const
{
	if (m_nID == other.m_nID)
		return true;
	return false;
}


bool IsIdentIntersectItem( IntersectItem* pItem1, IntersectItem* pItem2)
{
	if( pItem1 == pItem2 )
		return true;
	if( pItem1&& pItem2)
		return pItem2->IsIdent(pItem1);
	return false;
}


IntersectionNode::~IntersectionNode( void )
{
	ClearItemList();
}

void IntersectionNode::SaveData(int nAirportID)
{
	m_nAirportID = nAirportID; 
	if (m_nID >=0)
	{
		UpdateData();
	}
	else
	{
		CString strSQL;
		strSQL.Format(_T("INSERT INTO INTERSECTIONNODE (NAME, AIRPORTID, NODEINDEX, POS_X, POS_Y, POS_Z) VALUES ('%s',%d,%d,%f,%f,%f)"),
			m_strName,nAirportID,m_nidx,m_pos.getX(),m_pos.getY(),m_pos.getZ());
		m_nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
	}
	SaveItems();
}

BOOL IntersectionNode::IsIdentical( const IntersectionNode& node )const
{
	std::vector<IntersectItem* >vItems =  GetItemList();
	std::vector<IntersectItem* >vOtherItems = node.GetItemList();
	
	if( m_nidx != node.m_nidx )
		return FALSE;

	if( vItems.size() == vOtherItems.size() )
	{
		for(int i=0;i<(int)vItems.size();i++)
		{
			bool bIdentItem = false;
			for(int j=0;j<(int) vOtherItems.size();j++)
			{
				if( IsIdentIntersectItem(vItems[i], vOtherItems[j]) )
				{
					bIdentItem  = true;
					break;
				}
			}

			if(!bIdentItem)
				return FALSE;
		}
	}
	else 
		return FALSE;

	return TRUE;
}


int IntersectionNode::GetID() const
{
	return m_nID;
}

CPoint2008 IntersectionNode::GetPosition() const
{
	return m_pos;
}

void IntersectionNode::UpdateData()
{
	CString updateSQL;
	updateSQL.Format(_T("UPDATE INTERSECTIONNODE SET NAME ='%s', NODEINDEX =%d, POS_X =%f, POS_Y =%f, POS_Z =%f	 WHERE (ID = %d)"),
					 m_strName,m_nidx,m_pos.getX(),m_pos.getY(),m_pos.getZ(),m_nID);
	CADODatabase::ExecuteSQLStatement(updateSQL);
}

void IntersectionNode::DeleteData()
{
	DeleteItems();
	CString strSQL;
	strSQL.Format(_T("DELETE FROM INTERSECTIONNODE WHERE (ID = %d)"),m_nID);
	CADODatabase::ExecuteSQLStatement(strSQL);
}

void IntersectionNode::ReadData(CADORecordset& adoRecordset)
{
	adoRecordset.GetFieldValue(_T("ID"),m_nID);
	adoRecordset.GetFieldValue(_T("NAME"),m_strName);
	adoRecordset.GetFieldValue(_T("AIRPORTID"),m_nAirportID);
	adoRecordset.GetFieldValue(_T("NODEINDEX"),m_nidx);
	DistanceUnit dx,dy,dz;
	adoRecordset.GetFieldValue(_T("POS_X"),dx);
	adoRecordset.GetFieldValue(_T("POS_Y"),dy);
	adoRecordset.GetFieldValue(_T("POS_Z"),dz);
	m_pos.setPoint(dx,dy,dz);
	ReadItems();
}

void IntersectionNode::ReadData( int nID )
{
	CString strSQL =_T("");
	strSQL.Format(_T("SELECT * FROM INTERSECTIONNODE WHERE (ID = %d)"),nID);

	CADORecordset adoRecordset;
	long nItemCount =-1;
	CADODatabase::ExecuteSQLStatement(strSQL,nItemCount,adoRecordset);

	if (!adoRecordset.IsEOF())
	{
		ReadData(adoRecordset);
	}
	//ReadItems();
}

IntersectionNode& IntersectionNode::operator=( const IntersectionNode& node)
{
	m_nID = node.m_nID;
	m_strName = node.m_strName;
	
	m_nidx = node.m_nidx;
	m_pos = node.m_pos;
	m_nAirportID = node.m_nAirportID;

	m_vItems = node.CloneItems();
	return *this;
}



void IntersectionNode::ExportIntersectionNode(CAirsideExportFile& exportFile)
{
	exportFile.getFile().writeInt(m_nID);
	exportFile.getFile().writeField(m_strName);
	exportFile.getFile().writeInt(m_nidx);
	exportFile.getFile().writeDouble(m_pos.getX());
	exportFile.getFile().writeDouble(m_pos.getY());
	exportFile.getFile().writeDouble(m_pos.getZ());
	exportFile.getFile().writeInt(m_nAirportID);
	std::vector<IntersectItem*> vAllItems = GetItemList();
	exportFile.getFile().writeInt((int)vAllItems.size());
	for (int i=0;i<(int)vAllItems.size();i++)
	{
		IntersectItem * pitem = vAllItems.at(i);
		exportFile.getFile().writeInt(pitem->GetUID());
		exportFile.getFile().writeInt(pitem->GetType());
		exportFile.getFile().writeInt(pitem->GetObjectID());
		exportFile.getFile().writeDouble(pitem->GetDistInItem());		
		exportFile.getFile().writeInt(pitem->GetStandLeadInOutLineID());
	}

}
void IntersectionNode::ImportIntersectionNode(CAirsideImportFile& importFile)
{
	int nOldID = -1;
	importFile.getFile().getInteger(nOldID);
	importFile.getFile().getField(m_strName.GetBuffer(1024),1024);
	m_strName.ReleaseBuffer();

	importFile.getFile().getInteger(m_nidx);
	double dx,dy,dz;
	importFile.getFile().getFloat(dx);
	importFile.getFile().getFloat(dy);
	importFile.getFile().getFloat(dz);

	m_pos.setPoint(dx,dy,dz);
	int nOldAptID =-1;
	importFile.getFile().getInteger(nOldAptID);

	m_nAirportID = importFile.GetAirportNewIndex(nOldAptID);

	if(importFile.getVersion()  <1038 )
	{
		return ;
	}

	int nItemCnt = 0;
	importFile.getFile().getInteger(nItemCnt);
	std::vector<int> vOldItemIDs;
	vOldItemIDs.resize(nItemCnt);
	for(int i=0;i<nItemCnt;i++)
	{
		int nOldId;
		importFile.getFile().getInteger(nOldId);
		vOldItemIDs[i] = nOldId;

		int nType;
		importFile.getFile().getInteger(nType);
		int nObjID = -1;
		importFile.getFile().getInteger(nObjID);
		nObjID = importFile.GetObjectNewID(nObjID);
		double distInItem;
		importFile.getFile().getFloat(distInItem);
		int leadinOutIdx;
		importFile.getFile().getInteger(leadinOutIdx);
		
		
		
	} 

	SaveData(m_nAirportID);
	
	importFile.AddIntersecctionIndexMap(nOldID,m_nID);
//add intersectItem index map
	std::vector<IntersectItem*> vItemList = GetItemList();
	for(int i=0;i< (int)vItemList.size();i++)
	{
		IntersectItem*pItem = vItemList.at(i);
		importFile.AddIntersectItemIndexMap(vOldItemIDs[i], pItem->GetUID());
	}
}

bool  IntersectionNode::GetNodeName(int ObjID1,int objID2,int idx,CString & nodeName)
{
	
	CString strSQL=_T("");
	strSQL.Format(_T("SELECT NAME FROM INTERSECTIONNODE WHERE ID \
					 IN(SELECT INTERSECTIONNODEID FROM INTERSECTIONNODEITEM WHERE INTERSECTIONNODEID \
					IN (select INTERSECTIONNODEID FROM INTERSECTIONNODEITEM WHERE OBJECTID=%d) and OBJECTID=%d)and NODEINDEX=%d "),ObjID1,objID2,idx);
	CADORecordset adoRecordset;
	long nItemCount =-1;
	CADODatabase::ExecuteSQLStatement(strSQL,nItemCount,adoRecordset);
	if (!adoRecordset.IsEOF())
	{
		adoRecordset.GetFieldValue(_T("NAME"),nodeName);
		return true;
	}
	else 
	{
		return FALSE;
	}

}
void IntersectionNode::ReadNodeList( int nAirportID, std::vector<IntersectionNode>& nodelist )
{
	CString strSQL =_T("");
	strSQL.Format(_T("SELECT * FROM INTERSECTIONNODE WHERE (AIRPORTID = %d)"),nAirportID);

	CADORecordset adoRecordset;
	long nItemCount =-1;
	CADODatabase::ExecuteSQLStatement(strSQL,nItemCount,adoRecordset);

	while (!adoRecordset.IsEOF())
	{
		IntersectionNode itersecNode;
		itersecNode.ReadData(adoRecordset);
		if( itersecNode.IsValid() )
			nodelist.push_back(itersecNode);
		else
			itersecNode.DeleteData();

		adoRecordset.MoveNextData();
	}
}

void IntersectionNode::ReadIntersecNodeListWithObjType( int thisObjId,ALTOBJECT_TYPE OtherobjType, std::vector<IntersectionNode>& lstIntersecNode )
{
	std::vector<IntersectionNode> vNodes;
	ReadIntersecNodeList(thisObjId, vNodes);
	for(int i=0;i<(int)vNodes.size();i++)
	{
		if( vNodes[i].HasOtherObjectType(thisObjId, OtherobjType) )
		{
			lstIntersecNode.push_back(vNodes[i]);
		}
	}
}

void IntersectionNode::ReadIntersecNodeListWithObjType( int thisObjId,std::vector<ALTOBJECT_TYPE> vOtherobjType, std::vector<IntersectionNode>& lstIntersecNode )
{
	std::vector<IntersectionNode> vNodes;
	ReadIntersecNodeList(thisObjId, vNodes);
	for(int i=0;i<(int)vNodes.size();i++)
	{
		for(int j =0; j < (int)vOtherobjType.size(); ++j)
		{
			if( vNodes[i].HasOtherObjectType(thisObjId, vOtherobjType[j]) )
			{
				lstIntersecNode.push_back(vNodes[i]);
				break;
			}
		}
	}
}

//void IntersectionNode::ReadIntersecNodeList(std::vector<IntersectionNode>& lstIntersecNode)
//{
//	CString strSQL;
//	strSQL.Format(_T("SELECT * FROM INTERSECTIONNODEITEM"));
//	CADORecordset adoRecordset;
//	long nItemCount =-1;
//	CADODatabase::ExecuteSQLStatement(strSQL,nItemCount,adoRecordset);
//
//	std::vector<int> vNodeIDs;
//	while (!adoRecordset.IsEOF())
//	{
//		int nNodeID = -1;
//		adoRecordset.GetFieldValue("INTERSECTIONNODEID", nNodeID );
//		if( vNodeIDs.end() == std::find(vNodeIDs.begin(),vNodeIDs.end(),nNodeID) )
//		{
//			vNodeIDs.push_back(nNodeID);
//		}
//		adoRecordset.MoveNextData();
//	}	
//
//	int iBeginIdx = (int)lstIntersecNode.size();
//	lstIntersecNode.resize(iBeginIdx+vNodeIDs.size());
//	for(int i=iBeginIdx;i<(int)lstIntersecNode.size();i++)
//	{
//		lstIntersecNode[i].ReadData(vNodeIDs.at(i-iBeginIdx));
//	}
//}
void IntersectionNode::ReadIntersecNodeList( int objID,std::vector<IntersectionNode>& lstIntersecNode )
{
	CString strSQL;
	strSQL.Format(_T("SELECT * FROM INTERSECTIONNODEITEM WHERE (OBJECTID = %d)"), objID);
	CADORecordset adoRecordset;
	long nItemCount =-1;
	CADODatabase::ExecuteSQLStatement(strSQL,nItemCount,adoRecordset);

	std::vector<int> vNodeIDs;
	while (!adoRecordset.IsEOF())
	{
		int nNodeID = -1;
		adoRecordset.GetFieldValue("INTERSECTIONNODEID", nNodeID );
		if( vNodeIDs.end() == std::find(vNodeIDs.begin(),vNodeIDs.end(),nNodeID) )
		{
			vNodeIDs.push_back(nNodeID);
		}
		adoRecordset.MoveNextData();
	}	

	int iBeginIdx = (int)lstIntersecNode.size();
	lstIntersecNode.resize(iBeginIdx+vNodeIDs.size());
	for(int i=iBeginIdx;i<(int)lstIntersecNode.size();i++)
	{
		lstIntersecNode[i].ReadData(vNodeIDs.at(i-iBeginIdx));
	}
	
}

void IntersectionNode::ReadIntersectNodeList( int objID1, int objID2, std::vector<IntersectionNode>& lstIntersecNode )
{
	CString strSQL;
	strSQL.Format(_T("SELECT * FROM INTERSECTIONNODEITEM WHERE (OBJECTID = %d)"), objID1);
	CADORecordset adoRecordset;
	long nItemCount =-1;
	CADODatabase::ExecuteSQLStatement(strSQL,nItemCount,adoRecordset);

	std::vector<int> vNodeIDList1;
	while (!adoRecordset.IsEOF())
	{
		int nNodeID = -1;
		adoRecordset.GetFieldValue("INTERSECTIONNODEID", nNodeID );
		if( vNodeIDList1.end() == std::find(vNodeIDList1.begin(),vNodeIDList1.end(),nNodeID) )
		{
			vNodeIDList1.push_back(nNodeID);
		}
		adoRecordset.MoveNextData();
	}	

	strSQL.Format(_T("SELECT * FROM INTERSECTIONNODEITEM WHERE (OBJECTID = %d)"), objID2);
	CADODatabase::ExecuteSQLStatement(strSQL,nItemCount,adoRecordset);

	std::vector<int> vNodeIDList;
	while (!adoRecordset.IsEOF())
	{
		int nNodeID = -1;
		adoRecordset.GetFieldValue("INTERSECTIONNODEID", nNodeID );
		if( vNodeIDList1.end() != std::find(vNodeIDList1.begin(),vNodeIDList1.end(),nNodeID) ) // find id in list1
		{
			vNodeIDList.push_back(nNodeID);
		}
		adoRecordset.MoveNextData();
	}	

	int iBeginIdx = (int)lstIntersecNode.size();
	lstIntersecNode.resize(iBeginIdx+vNodeIDList.size());
	for(int i=iBeginIdx;i<(int)lstIntersecNode.size();i++)
	{
		lstIntersecNode[i].ReadData(vNodeIDList.at(i-iBeginIdx));
	}

}

int IntersectionNode::GetIndex() const
{
	return m_nidx;
}



void IntersectionNode::AddItem( IntersectItem* newItem )
{
	m_vItems.push_back(newItem->Clone());
}

bool IntersectionNode::HasObject( int nObjID )const
{
	std::vector<IntersectItem*> vAllItems = GetItemList();
	for(int i=0;i< (int)vAllItems.size();i++)
	{
		if( vAllItems.at(i)->GetObjectID() == nObjID )
			return true;
	}
	return false;
}

bool IntersectionNode::HasObjectIn( const ALTObjectUIDList& vObjIDs )const
{
	std::vector<IntersectItem*> vAllItems = GetItemList();
	for(int i=0;i< (int)vAllItems.size();i++)
	{
		if( vObjIDs.end()!= std::find(vObjIDs.begin(),vObjIDs.end(), vAllItems.at(i)->GetObjectID() ) )
			return true;
	}
	return false;
}

std::vector<ALTObject*> IntersectionNode::GetOtherObjectList( int nObjID )
{
	std::vector<IntersectItem*> vAllItems = GetItemList();
	std::vector<ALTObject*> vReslt;
	for(size_t i=0;i<vAllItems.size();i++)
	{
		IntersectItem* theItem = vAllItems[i];
		if (vReslt.end() == std::find(vReslt.begin(),vReslt.end(),theItem->GetObject()) && theItem->GetObjectID()!= nObjID )
		{
			vReslt.push_back(theItem->GetObject());
		}
	}	
	return vReslt;
}

std::vector<int> IntersectionNode::GetOtherObjectIDList( int nObjID )
{
	std::vector<int> vReslt;
	std::vector<IntersectItem*> vAllItems = GetItemList();
	for(int i=0;i<(int)vAllItems.size();++i)
	{
		IntersectItem *theItem = vAllItems.at(i);
		if( theItem->GetObjectID() != nObjID)
			vReslt.push_back(theItem->GetObjectID());
	}
	return vReslt;
}

DistanceUnit IntersectionNode::GetDistance( int nObjID ) const
{
	std::vector<IntersectItem*> vAllItems = GetItemList();
	for(int i=0;i<(int)vAllItems.size();++i)
	{
		IntersectItem *theItem = vAllItems.at(i);
		if( theItem->GetObjectID() == nObjID)
			return theItem->GetDistInItem();
	}
	ASSERT(false);
	return -1;
}

ALTObject * IntersectionNode::GetObject( int nObjID )
{	
	std::vector<IntersectItem*> vAllItems = GetItemList();

	for(size_t i =0 ;i< vAllItems.size();i++)
	{
		if( vAllItems.at(i)->GetObjectID() == nObjID) 
			return vAllItems.at(i)->GetObject();
	}
	return NULL;
}


std::vector<IntersectItem* > IntersectionNode::GetItemList()const
{
	return m_vItems;	
}

void IntersectionNode::SaveItems()
{
	std::vector<IntersectItem*> vItemsDB = IntersectItem::ReadData(GetID());
	std::vector<IntersectItem*> vItemsNow = GetItemList();
	
	//delete useless items 
	for(int i=0;i< (int)vItemsDB.size();i++)
	{
		bool bUseless = true;
		for(int j=0;j<(int) vItemsNow.size();j++)
		{
			if( IsIdentIntersectItem(vItemsDB[i],vItemsNow[j]) )
			{
				bUseless = false;
				vItemsNow[j]->SetUID( vItemsDB[i]->GetUID() );
				break;
			}
		}
		if(bUseless)
			vItemsDB[i]->DeleteData();
	}	
	//
	for(int i=0;i<(int)vItemsNow.size();i++)
	{
		vItemsNow[i]->SaveData(GetID());
	}	
}

void IntersectionNode::ReadItems()
{
	m_vItems = IntersectItem::ReadData(GetID());
}

void IntersectionNode::DeleteItems()
{
	std::vector<IntersectItem*> vItemsNow = GetItemList();
	for(int i=0;i<(int)vItemsNow.size();i++)
	{
		vItemsNow.at(i)->DeleteData();
	}
	ClearItemList();
}

//have otherobj
bool IntersectionNode::HasOtherObjectType( int nObjID, ALTOBJECT_TYPE OtherobjType )
{
	IntersectItem* pObjItem = GetObjectItem(nObjID);
	if(pObjItem)
	{		
		std::vector<IntersectItem*> vObjItems = GetTypeItemList(pObjItem->GetType());
		std::vector<IntersectItem*> vOtherObjItems = GetObjectTypeItems(OtherobjType);

		if( pObjItem->GetObject() && pObjItem->GetObject()->GetType() == OtherobjType )
		{
			return vOtherObjItems.size() >=2 ;
		}
		else
		{
			return vOtherObjItems.size() >=1 ;
		}
	}	
	return false;
}


void IntersectionNode::Merge( const IntersectionNode& _otherNode )
{
 	std::vector<IntersectItem* >vItems =  GetItemList();

	IntersectionNode otherNode =_otherNode;
	std::vector<IntersectItem* >vOtherItems = otherNode.GetItemList();

	std::vector<IntersectItem*> vAddItems;

	for(int i=0;i<(int)vOtherItems.size();i++)
	{
		bool toAdd = true;
		for(int j=0;j<(int) vItems.size();j++)
		{
			if( IsIdentIntersectItem(vOtherItems[i], vItems[j]) )
			{
				toAdd = false;
				break;
			}
		}
		if(toAdd)
			vAddItems.push_back( vOtherItems[i]);
	}

	for(int i=0;i<(int)vAddItems.size();i++)
	{
		AddItem(vAddItems[i]);
	}
	
	m_pos = ( GetPosition() + otherNode.GetPosition() ) * 0.5;
}

void IntersectionNode::SetObject( ALTObjectList objList )
{
	std::vector<IntersectItem*> vItems = GetItemList();
	for(int i=0;i< (int)vItems.size();i++)
	{
		int nObjID = vItems[i]->GetObjectID();
		for(int j=0;j<(int)objList.size();j++)
		{
			if( objList[j]->getID() == nObjID )
			{
				vItems[i]->SetObject(objList[j].get());
				break;
			}
		}
	}
}



void IntersectionNode::UpdateName()
{
	std::vector<IntersectItem*> vItems = GetItemList();
	std::sort( vItems.begin(),vItems.end(), IntersectItem::CompareLess );
	if( vItems.size() )
	{
		m_strName = vItems[0]->GetNameString();
		for(int i=1;i<(int)vItems.size();i++)
		{
			m_strName += _T(" & ");
			m_strName += vItems[i]->GetNameString();
		}

		CString indexStr;
		if( m_nidx > 0 )
			indexStr.Format("(%d)", m_nidx);

		m_strName += indexStr;
	}
}

IntersectItem* IntersectionNode::GetItemByID( int nID )const
{	
	std::vector<IntersectItem*> vItems = GetItemList();
	for(int i=0;i< (int)vItems.size();i++)
	{
		if(vItems.at(i)->GetUID() == nID )
			return vItems.at(i);
	}
	return NULL;
}

bool IntersectionNode::HasLeadInLine(  int nInLineID )const
{
	std::vector<StandLeadInLineIntersectItem*> vItems = GetStandLeadInLineItemList();
	for(int i=0; i<(int)vItems.size();i++ )
	{
		if( vItems[i]->GetStandLeadInOutLineID() == nInLineID )
		{
			return true;
		}
	}
	return false;
}

bool IntersectionNode::HasLeadOutLine( int nOutLineID)const
{
	std::vector<StandLeadOutLineIntersectItem*> vItems = GetStandLeadOutLineItemList();
	for(int i=0; i<(int)vItems.size();i++ )
	{
		if( vItems[i]->GetStandLeadInOutLineID() == nOutLineID)
		{
			return true;
		}
	}
	return false;
}

bool IntersectionNode::IsValid() const
{
	if(m_vItems.size() >= 2)
	{
		for(int i=1; i<(int)m_vItems.size(); i++)
		{
			if(m_vItems[i]->GetObjectID() != m_vItems[i-1]->GetObjectID())
				return true;
		}
	}

	return false;
}

BOOL IntersectionNode::IsIdenticalWithOutIndex( const IntersectionNode& node ) const
{
	std::vector<IntersectItem* >vItems =  GetItemList();
	std::vector<IntersectItem* >vOtherItems = node.GetItemList();

	if( vItems.size() == vOtherItems.size() )
	{
		for(int i=0;i<(int)vItems.size();i++)
		{
			bool bIdentItem = false;
			for(int j=0;j<(int) vOtherItems.size();j++)
			{
				if( IsIdentIntersectItem(vItems[i], vOtherItems[j]) )
				{
					bIdentItem  = true;
					break;
				}
			}

			if(!bIdentItem)
				return FALSE;
		}
	}
	else 
		return FALSE;

	return TRUE;
}

CString IntersectionNode::GetName()const
{ 
	return m_strName; 
}

void IntersectionNode::ClearItemList()
{
	for(int i=0;i<(int)m_vItems.size();i++)
	{
		delete m_vItems.at(i);
	}
	m_vItems.clear();
}

IntersectionNode& IntersectionNode::SetPosition( const CPoint2008& pos )
{
	m_pos = pos; return *this;
}

IntersectionNode& IntersectionNode::SetName( const CString& strName )
{
	m_strName = strName; return *this;
}

IntersectionNode& IntersectionNode::SetIndex( int idx )
{
	m_nidx = idx;
	return *this;
}

int IntersectionNode::GetAirportID() const
{
	return m_nAirportID;
}

IntersectionNode& IntersectionNode::SetAirportID( int nID )
{
	m_nAirportID = nID;
	return *this;
}

IntersectionNode& IntersectionNode::SetID( int nID )
{
	m_nID = nID;
	return *this; 
}

std::vector<IntersectItem*> IntersectionNode::CloneItems() const
{
	std::vector<IntersectItem*> ret;
	for(size_t i=0;i< m_vItems.size();++i)
		ret.push_back( m_vItems[i]->Clone() );
	return ret;
}

std::vector<RunwayIntersectItem*> IntersectionNode::GetRunwayIntersectItemList() const
{
	std::vector<RunwayIntersectItem*> ret;
	std::vector<IntersectItem*> vItems = GetTypeItemList(IntersectItem::RUNWAY_INTESECT);
	for(size_t i=0;i<vItems.size();++i)
		ret.push_back((RunwayIntersectItem*)vItems[i]);
	return ret;
}

std::vector<TaxiwayIntersectItem*> IntersectionNode::GetTaxiwayIntersectItemList() const
{
	std::vector<TaxiwayIntersectItem*> ret;
	std::vector<IntersectItem*> vItems = GetTypeItemList(IntersectItem::TAXIWAY_INTERSECT);
	for(size_t i=0;i<vItems.size();++i)
		ret.push_back((TaxiwayIntersectItem*)vItems[i]);
	return ret;
}

std::vector<StandLeadInLineIntersectItem*> IntersectionNode::GetStandLeadInLineItemList() const
{
	std::vector<StandLeadInLineIntersectItem*> ret;
	std::vector<IntersectItem*> vItems = GetTypeItemList(IntersectItem::STAND_LEADINLINE_INTERSECT);
	for(size_t i=0;i<vItems.size();++i)
		ret.push_back((StandLeadInLineIntersectItem*)vItems[i]);
	return ret;
}

std::vector<StandLeadOutLineIntersectItem*> IntersectionNode::GetStandLeadOutLineItemList() const
{
	std::vector<StandLeadOutLineIntersectItem*> ret;
	std::vector<IntersectItem*> vItems = GetTypeItemList(IntersectItem::STAND_LEADOUT_LINEINTERSECT);
	for(size_t i=0;i<vItems.size();++i)
		ret.push_back((StandLeadOutLineIntersectItem*)vItems[i]);
	return ret;
}

std::vector<StretchIntersectItem*> IntersectionNode::GetStretchIntersectItem() const
{
	std::vector<StretchIntersectItem*> ret;
	std::vector<IntersectItem*> vItems = GetTypeItemList(IntersectItem::STRETCH_INTERSECT);
	for(size_t i=0;i<vItems.size();++i)
		ret.push_back((StretchIntersectItem*)vItems[i]);
	return ret;
}

std::vector<CircleStretchIntersectItem*> IntersectionNode::GetCircleStretchIntersectItemList() const
{
	std::vector<CircleStretchIntersectItem*> ret;
	std::vector<IntersectItem*> vItems = GetTypeItemList(IntersectItem::CIRCLESTRETCH_INTERSECT);
	for(size_t i=0;i<vItems.size();++i)
		ret.push_back((CircleStretchIntersectItem*)vItems[i]);
	return ret;
}

std::vector<DeicePadInContrainItem*> IntersectionNode::GetDeicePadInContrainItemList() const
{
	std::vector<DeicePadInContrainItem*> ret;
	std::vector<IntersectItem*> vItems = GetTypeItemList(IntersectItem::DEICE_INCONSTRAIN);
	for(size_t i=0;i<vItems.size();++i)
		ret.push_back((DeicePadInContrainItem*)vItems[i]);
	return ret;
}

std::vector<DeicePadOutConstrainItem*> IntersectionNode::GetDeicePadOutConstrainItemList() const
{
	std::vector<DeicePadOutConstrainItem*> ret;
	std::vector<IntersectItem*> vItems = GetTypeItemList(IntersectItem::DEICE_OUTCONSTRAIN);
	for(size_t i=0;i<vItems.size();++i)
		ret.push_back((DeicePadOutConstrainItem*)vItems[i]);
	return ret;
}

std::vector<IntersectItem*> IntersectionNode::GetTypeItemList( IntersectItem::TYPE nType ) const
{
	std::vector<IntersectItem*> ret;
	for(size_t i=0;i< m_vItems.size();++i)
	{
		IntersectItem* theItem = m_vItems[i];
		if(theItem->GetType() == nType )
		{
			ret.push_back(theItem);
		}
	}
	return ret;
}

std::vector<IntersectItem*> IntersectionNode::GetObjectTypeItems( ALTOBJECT_TYPE objType )
{
	std::vector<IntersectItem*> ret;
	for(size_t i=0;i< m_vItems.size();++i)
	{
		IntersectItem* theItem = m_vItems[i];
		if(theItem->GetObject()->GetType() == objType )
		{
			ret.push_back(theItem);
		}
	}
	return ret;

}

IntersectItem* IntersectionNode::GetObjectItem( int nObjID )const
{
	std::vector<IntersectItem*> ret;
	for(size_t i=0;i< m_vItems.size();++i)
	{
		IntersectItem* theItem = m_vItems[i];
		if(theItem->GetObjectID() == nObjID )
		{
			return theItem;
		}
	}	
	return NULL;
}

void IntersectionNode::RemoveObjectItem( int nObjID )
{
	std::vector<IntersectItem*> oldItems = m_vItems;
	m_vItems.clear();
	for(size_t i=0;i< oldItems.size();++i)
	{
		IntersectItem* theItem = oldItems[i];
		if(theItem->GetObjectID() != nObjID )
		{
			m_vItems.push_back(theItem);
		}
		else{
			delete theItem;
		}
	}	
}

TaxiwayIntersectItem* IntersectionNode::GetTaxiwayItem( int nTaxiID )const
{
	IntersectItem* pItem = GetObjectItem(nTaxiID);
	if(pItem&&pItem->GetType() == IntersectItem::TAXIWAY_INTERSECT)
	{
		return (TaxiwayIntersectItem*)pItem;
	}
	return NULL;
}

RunwayIntersectItem* IntersectionNode::GetRunwayItem( int nRwID )
{
	IntersectItem* pItem = GetObjectItem(nRwID);
	if(pItem&&pItem->GetType() == IntersectItem::RUNWAY_INTESECT)
	{
		return (RunwayIntersectItem*)pItem;
	}
	return NULL;
}

StretchIntersectItem* IntersectionNode::GetStretchItem( int nStretchID )
{
	IntersectItem* pItem = GetObjectItem(nStretchID);
	if(pItem&&pItem->GetType() == IntersectItem::STRETCH_INTERSECT)
	{
		return (StretchIntersectItem*)pItem;
	}
	return NULL;
}

CircleStretchIntersectItem* IntersectionNode::GetCircleStretchItem( int nCircleStretchID )
{
	IntersectItem* pItem = GetObjectItem(nCircleStretchID);
	if(pItem&&pItem->GetType() == IntersectItem::CIRCLESTRETCH_INTERSECT)
	{
		return (CircleStretchIntersectItem*)pItem;
	}
	return NULL;
}

StandLeadInLineIntersectItem* IntersectionNode::GetStandLeadInLineItem( int nStandID, int nLeadinLineIdx )
{
	for(int i=0;i< (int )m_vItems.size();++i)
	{
		IntersectItem* theItem = m_vItems[i];
		if(theItem && theItem->GetType() ==IntersectItem::STAND_LEADINLINE_INTERSECT)
		{
			StandLeadInLineIntersectItem* pStandItem = (StandLeadInLineIntersectItem* )theItem;
			if(pStandItem->GetObjectID()== nStandID && nLeadinLineIdx == pStandItem->GetStandLeadInOutLineID() )
				return pStandItem;
		}
	}

	return NULL;
}

StandLeadOutLineIntersectItem* IntersectionNode::GetStandLeadOutLineItem( int nStandID, int nLeadoutLineIdx )
{
	for(int i=0;i< (int )m_vItems.size();++i)
	{
		IntersectItem* theItem = m_vItems[i];
		if(theItem && theItem->GetType() ==IntersectItem::STAND_LEADINLINE_INTERSECT)
		{
			StandLeadOutLineIntersectItem* pStandItem = (StandLeadOutLineIntersectItem* )theItem;
			if(pStandItem->GetObjectID()== nStandID && nLeadoutLineIdx == pStandItem->GetStandLeadInOutLineID() )
				return pStandItem;
		}
	}
	return NULL;
}

bool IntersectionNode::IsRunwayIntersectionNode()const
{
	std::vector<RunwayIntersectItem*> vRunwayNode = GetRunwayIntersectItemList();
	if(vRunwayNode.size() > 0)
		return true;

	return false;
}

bool IntersectionNode::IsRunwaysIntersection() const
{
	std::vector<RunwayIntersectItem*> vRunwayNode = GetRunwayIntersectItemList();
	if(vRunwayNode.size() >= 2)			//at least two runway intersected at the node
		return true;

	return false;
}

bool IntersectionNode::HasDeiceInContrain( int nObjID ) const
{
	std::vector<DeicePadInContrainItem*> vInContrains =  GetDeicePadInContrainItemList();
	for(int i=0;i<(int)vInContrains.size();i++)
	{
		if(vInContrains.at(i)->GetObjectID() == nObjID)
			return true;
	}
	return false;
}

bool IntersectionNode::HasDeiceOutContrain( int nObjID ) const
{
	std::vector<DeicePadOutConstrainItem*> vOutContrains =  GetDeicePadOutConstrainItemList();
	for(int i=0;i<(int)vOutContrains.size();i++)
	{
		if(vOutContrains.at(i)->GetObjectID() == nObjID)
			return true;
	}
	return false;
}
