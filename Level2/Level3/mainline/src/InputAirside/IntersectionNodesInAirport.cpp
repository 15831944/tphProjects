#include "StdAfx.h"
#include "./IntersectionNodesInAirport.h"
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
#include "CircleStretch.h"

const static DistanceUnit MinDistBetweenNodes = 500;

/************************************************************************/
/*                                                                      */
/************************************************************************/
void IntersectionNodesInAirport::ReadData( int nAirportID )
{
	m_vNodeList.clear();
	IntersectionNode::ReadNodeList(nAirportID,m_vNodeList);
}

void IntersectionNodesInAirport::GetIntersectionNodes( Runway * pRunway, Taxiway * pTaxiway, std::vector<IntersectionNode>& reslts )
{
	CPath2008 runwayPath = pRunway->GetPath();
	CPath2008 taxiwayPath = pTaxiway->GetPath();
	IntersectPathPath2D intersectPath;
	int intersectCount = intersectPath.Intersects(runwayPath,taxiwayPath,5);

	for(int i =0 ; i< intersectCount;i ++)
	{
		double dPos1 = runwayPath.GetIndexDist((float)intersectPath.m_vIntersectPtIndexInPath1.at(i));
		double dPos2 = taxiwayPath.GetIndexDist((float)intersectPath.m_vIntersectPtIndexInPath2.at(i));
		CPoint2008 pos = runwayPath.GetDistPoint(dPos1);
		IntersectionNode newNode;

		RunwayIntersectItem rwItem;
		rwItem.SetRunway(pRunway);
		rwItem.SetDistInItem(dPos1);
		TaxiwayIntersectItem taxiItem;
		taxiItem.SetTaxiway(pTaxiway);
		taxiItem.SetDistInItem(dPos2);

		newNode.AddItem(&rwItem);
		newNode.AddItem(&taxiItem);

		newNode.SetPosition(pos);
		newNode.SetIndex(i);
		reslts.push_back(newNode);	
	}
}

void IntersectionNodesInAirport::GetIntersectionNodes( Taxiway * pTaxiway1, Taxiway * pTaxiway2, std::vector<IntersectionNode>& reslts )
{
	CPath2008 taxi1Path = pTaxiway1->GetPath();
	CPath2008 taxi2Path = pTaxiway2->GetPath();
	IntersectPathPath2D intersectPath;
	int intersectCount = intersectPath.Intersects(taxi1Path,taxi2Path,5);

	for(int i =0 ; i< intersectCount;i ++)
	{
		double dPos1 = taxi1Path.GetIndexDist((float)intersectPath.m_vIntersectPtIndexInPath1.at(i));
		double dPos2 = taxi2Path.GetIndexDist((float)intersectPath.m_vIntersectPtIndexInPath2.at(i));
		CPoint2008 pos = taxi1Path.GetDistPoint(dPos1);
		IntersectionNode newNode;

		TaxiwayIntersectItem taxi1Item;
		TaxiwayIntersectItem taxi2Item;

		taxi1Item.SetTaxiway(pTaxiway1);
		taxi1Item.SetDistInItem(dPos1);
		taxi2Item.SetTaxiway(pTaxiway2);
		taxi2Item.SetDistInItem(dPos2);

		newNode.AddItem(&taxi1Item);
		newNode.AddItem(&taxi2Item);

		newNode.SetPosition(pos);
		newNode.SetIndex(i);
		reslts.push_back(newNode);	
	}
}

void IntersectionNodesInAirport::GetIntersectionNodes( Runway *pRunway1, Runway * pRunway2,std::vector<IntersectionNode>& reslts )
{
	CPath2008 runway1Path = pRunway1->GetPath();
	CPath2008 runway2Path = pRunway2->GetPath();
	IntersectPathPath2D intersectPath;
	int intersectCount = intersectPath.Intersects(runway1Path,runway2Path,5);

	for(int i =0 ; i< intersectCount;i ++)
	{
		double dPos1 = runway1Path.GetIndexDist((float)intersectPath.m_vIntersectPtIndexInPath1.at(i));
		double dPos2 = runway2Path.GetIndexDist((float)intersectPath.m_vIntersectPtIndexInPath2.at(i));
		CPoint2008 pos = runway1Path.GetDistPoint(dPos1);
		IntersectionNode newNode;
		RunwayIntersectItem rwItem1;
		RunwayIntersectItem rwItem2;

		rwItem1.SetRunway(pRunway1);
		rwItem2.SetRunway(pRunway2);
		rwItem1.SetDistInItem(dPos1);
		rwItem2.SetDistInItem(dPos2);		

		newNode.AddItem(&rwItem1);
		newNode.AddItem(&rwItem2);

		newNode.SetIndex(i);
		newNode.SetPosition(pos);
		reslts.push_back(newNode);	
	}
}

void IntersectionNodesInAirport::GetIntersectionNodes( Stand * pStand , Taxiway * pTaxiway,std::vector<IntersectionNode>& reslts )
{
	//lead in lines
	const CPath2008& taxiPath = pTaxiway->GetPath();

	for(int i=0;i< (int)pStand->m_vLeadInLines.GetItemCount();i++)
	{
		StandLeadInLine& leadInLine = pStand->GetLeadInLineList().ItemAt(i);
		const CPath2008& instandpath = leadInLine.getPath();
		IntersectPathPath2D intersectPath;
		int intersectCountIn = intersectPath.Intersects(instandpath, taxiPath,5);

		for(int j =0 ; j< intersectCountIn;j++)
		{
			double dPos1 = instandpath.GetIndexDist(intersectPath.m_vIntersectPtIndexInPath1.at(j));
			double dPos2 = taxiPath.GetIndexDist(intersectPath.m_vIntersectPtIndexInPath2.at(j));
			CPoint2008 pos = instandpath.GetDistPoint(dPos1);
			IntersectionNode newNode;

			StandLeadInLineIntersectItem standLeadInItem;
			TaxiwayIntersectItem taxiItem;

			standLeadInItem.SetStand(pStand);
			standLeadInItem.SetLeadInLineID(leadInLine.GetID());
			standLeadInItem.SetDistInItem(dPos1);

			taxiItem.SetTaxiway(pTaxiway);
			taxiItem.SetDistInItem(dPos2);

			newNode.AddItem(&taxiItem);
			newNode.AddItem(&standLeadInItem);
			newNode.SetPosition(pos);

			reslts.push_back(newNode);
			//				
		}
	}

	for(int i=0;i<(int)pStand->m_vLeadOutLines.GetItemCount();i++)
	{
		StandLeadOutLine& leadOutLine = pStand->GetLeadOutLineList().ItemAt(i);
		const CPath2008& outstandPath = leadOutLine.getPath();

		IntersectPathPath2D intersectPath;
		int intersectCountOut = intersectPath.Intersects(outstandPath,taxiPath,5);

		for(int j =0 ; j< intersectCountOut;j++)
		{
			double dPos1 = outstandPath.GetIndexDist(intersectPath.m_vIntersectPtIndexInPath1.at(j));
			double dPos2 = taxiPath.GetIndexDist(intersectPath.m_vIntersectPtIndexInPath2.at(j));
			CPoint2008 pos = outstandPath.GetDistPoint(dPos1);
			IntersectionNode newNode;

			TaxiwayIntersectItem taxiItem;
			StandLeadOutLineIntersectItem standOutItem;

			standOutItem.SetStand(pStand);
			standOutItem.SetLeadOutLineID(leadOutLine.GetID());
			standOutItem.SetDistInItem(dPos1);

			taxiItem.SetTaxiway(pTaxiway);
			taxiItem.SetDistInItem(dPos2);

			newNode.AddItem(&taxiItem);
			newNode.AddItem(&standOutItem);			
			newNode.SetPosition(pos);

			reslts.push_back(newNode);
		}
	}	

}

void IntersectionNodesInAirport::GetIntersectionNodes( DeicePad * pDeicepad, Taxiway * pTaxiway, std::vector<IntersectionNode>& reslts )
{
	//in constrain
	CPath2008 instandpath = pDeicepad->GetInContrain();
	CPath2008 taxiPath = pTaxiway->GetPath();
	IntersectPathPath2D intersectPath;
	int intersectCountIn = intersectPath.Intersects(instandpath,taxiPath,5);
	
	for(int i =0 ; i< intersectCountIn;i ++)
	{
		double dPos1 = instandpath.GetIndexDist(intersectPath.m_vIntersectPtIndexInPath1.at(i));
		double dPos2 = taxiPath.GetIndexDist(intersectPath.m_vIntersectPtIndexInPath2.at(i));
		CPoint2008 pos = instandpath.GetDistPoint(dPos1);
		
		IntersectionNode newNode;
		
		TaxiwayIntersectItem taxiItem;
		DeicePadInContrainItem deicepadItem;

		deicepadItem.SetDeicePad(pDeicepad);
		deicepadItem.SetDistInItem(dPos1);

		taxiItem.SetTaxiway(pTaxiway);
		taxiItem.SetDistInItem(dPos2);

		newNode.AddItem(&deicepadItem);
		newNode.AddItem(&taxiItem);

		newNode.SetPosition(pos);

		reslts.push_back(newNode);		
		//				
	}

	//out constrain
	if(!pDeicepad->IsUsePushBack())
	{
		CPath2008 outstandPath = pDeicepad->GetOutConstrain();

		IntersectPathPath2D intersectPath;
		int intersectCountOut = intersectPath.Intersects(outstandPath,taxiPath,5);
		
		for(int i =0 ; i< intersectCountOut;i ++)
		{
			double dPos1 = outstandPath.GetIndexDist(intersectPath.m_vIntersectPtIndexInPath1.at(i));
			double dPos2 = taxiPath.GetIndexDist(intersectPath.m_vIntersectPtIndexInPath2.at(i));
			CPoint2008 pos = outstandPath.GetDistPoint(dPos1);
			IntersectionNode newNode;

			TaxiwayIntersectItem taxiItem;
			DeicePadOutConstrainItem deicepadItem;

			deicepadItem.SetDeicePad(pDeicepad);
			deicepadItem.SetDistInItem(dPos1);

			taxiItem.SetTaxiway(pTaxiway);
			taxiItem.SetDistInItem(dPos2);

			newNode.AddItem(&deicepadItem);
			newNode.AddItem(&taxiItem);

			newNode.SetPosition(pos);

			reslts.push_back(newNode);	
		}
	}
}

void IntersectionNodesInAirport::GetIntersectionNodes(Stretch* pStretch1, Stretch* pStretch2, std::vector<IntersectionNode>& reslts)
{
	CPath2008 stretch1Path = pStretch1->GetPath();
	CPath2008 stretch2Path = pStretch2->GetPath();
	IntersectPathPath2D intersectPath;
	int intersectCount = intersectPath.Intersects(stretch1Path,stretch2Path,5);
    if (stretch1Path.IsSameAs(stretch2Path))
		return;
	for(int i =0 ; i< intersectCount;i ++)
	{
		double dPos1 = stretch1Path.GetIndexDist(intersectPath.m_vIntersectPtIndexInPath1.at(i));
		double dPos2 = stretch2Path.GetIndexDist(intersectPath.m_vIntersectPtIndexInPath2.at(i));
		CPoint2008 pos = stretch1Path.GetDistPoint(dPos1);
		CPoint2008 pos2 = stretch2Path.GetDistPoint(dPos2);
		if ((int)pos.getZ()!=(int)pos2.getZ())
			continue;

		IntersectionNode newNode;

		StretchIntersectItem stretch1Item;
		StretchIntersectItem stretch2Item;

		stretch1Item.SetStretch(pStretch1);
		stretch1Item.SetDistInItem(dPos1);
		stretch2Item.SetStretch(pStretch2);
		stretch2Item.SetDistInItem(dPos2);

		newNode.AddItem(&stretch1Item);
		newNode.AddItem(&stretch2Item);

		newNode.SetPosition(pos);
		newNode.SetIndex(i);
		reslts.push_back(newNode);	
	}
}

void IntersectionNodesInAirport::GetIntersectionNodes(CircleStretch* pCirStretch, Stretch* pStretch, std::vector<IntersectionNode>& reslts)
{
	CPath2008 circleTempPath = pCirStretch->GetPath();
	CPath2008 circleStretchPath;
	circleTempPath.PathSupplement(circleStretchPath);
	CPath2008 stretchPath = pStretch->GetPath();
	IntersectPathPath2D intersectPath;
	int intersectCount = intersectPath.Intersects(circleStretchPath,stretchPath,5);
	for (int i=0; i<intersectCount; i++)
	{
		double dPosCir = circleStretchPath.GetIndexDist(intersectPath.m_vIntersectPtIndexInPath1.at(i));
		double dPosDir = stretchPath.GetIndexDist(intersectPath.m_vIntersectPtIndexInPath2.at(i));
		CPoint2008 posCir = circleStretchPath.GetDistPoint(dPosCir);
		CPoint2008 posDir = stretchPath.GetDistPoint(dPosDir);

		if ((int)posCir.getZ()!=(int)posDir.getZ())
			continue;

		IntersectionNode newNode;
		CircleStretchIntersectItem circleItem;
		StretchIntersectItem directItem;

		circleItem.SetCircleStretch(pCirStretch);
		circleItem.SetDistInItem(dPosCir);
		directItem.SetStretch(pStretch);
		directItem.SetDistInItem(dPosDir);
        
		newNode.AddItem(&circleItem);
		newNode.AddItem(&directItem);

		newNode.SetPosition(posCir);
		newNode.SetIndex(i);
		reslts.push_back(newNode);
	}
}

void IntersectionNodesInAirport::GetIntersectionNodes( ALTObject *pObj1, ALTObject * pObj2,std::vector<IntersectionNode>& reslts )
{
	ASSERT(pObj1 != pObj2);

	if(pObj1->GetType() == ALT_TAXIWAY && pObj2->GetType() == ALT_TAXIWAY)
	{
		return GetIntersectionNodes((Taxiway*)pObj1,(Taxiway*)pObj2,reslts);
	}
	if(pObj1->GetType() == ALT_RUNWAY && pObj2->GetType() == ALT_RUNWAY )
	{
		return GetIntersectionNodes((Runway*)pObj1,(Runway*)pObj2, reslts);
	}
	if (pObj1->GetType() == ALT_STRETCH && pObj2->GetType() == ALT_STRETCH)
	{
		return GetIntersectionNodes((Stretch*)pObj1,(Stretch*)pObj2,reslts);
	}

	Runway * pRunway = NULL;
	Taxiway * pTaxiway = NULL;
	Stand * pStand = NULL;
	DeicePad * pDeicePad = NULL;
	Stretch * pStretch = NULL;
	CircleStretch * pCirStretch = NULL;

	if(pObj1->GetType() == ALT_RUNWAY) pRunway = (Runway*)pObj1;
	if(pObj1->GetType() == ALT_TAXIWAY) pTaxiway = (Taxiway*)pObj1;
	if(pObj1->GetType() == ALT_STAND) pStand = (Stand*)pObj1;
	if(pObj1->GetType() == ALT_DEICEBAY) pDeicePad = (DeicePad*)pObj1;
	if (pObj1->GetType() == ALT_STRETCH) pStretch = (Stretch*)pObj1;
	if (pObj1->GetType() == ALT_CIRCLESTRETCH) pCirStretch = (CircleStretch*)pObj1;

	if(pObj2->GetType() == ALT_RUNWAY) pRunway = (Runway*)pObj2;
	if(pObj2->GetType() == ALT_TAXIWAY) pTaxiway = (Taxiway*)pObj2;
	if(pObj2->GetType() == ALT_STAND) pStand = (Stand*)pObj2;
	if(pObj2->GetType() == ALT_DEICEBAY) pDeicePad = (DeicePad*)pObj2;
	if (pObj2->GetType() == ALT_STRETCH) pStretch = (Stretch*)pObj2;
	if (pObj2->GetType() == ALT_CIRCLESTRETCH) pCirStretch = (CircleStretch*)pObj2;

	if(pTaxiway)
	{
		if(pRunway)
			return GetIntersectionNodes(pRunway,pTaxiway,reslts);
		if(pStand)
			return GetIntersectionNodes(pStand,pTaxiway,reslts);
		if(pDeicePad)
			return GetIntersectionNodes(pDeicePad,pTaxiway,reslts);
	}
	if (pStretch && pCirStretch)
	{
		return GetIntersectionNodes(pCirStretch,pStretch,reslts);
	}


}
//
void IntersectionNodesInAirport::CaculateIntersectionNodes( ALTObjectList objList )
{
	m_vNodeList.clear();
	for(int i=0 ; i< (int) objList.size();i ++)
	{
		for(int j=i +1; j  < (int)objList.size();j++)
		{
			ALTObject * pobj1 = objList.at(i).get();
			ALTObject * pobj2 = objList.at(j).get();
			std::vector<IntersectionNode> reslts;
			GetIntersectionNodes(pobj1,pobj2,reslts);
			AddNodes(reslts);
		}
	}
}

void IntersectionNodesInAirport::AddNode( const IntersectionNode& newNode )
{

	for(int i=0;i<(int)m_vNodeList.size();i++)
	{
		IntersectionNode& theNode = m_vNodeList[i];
		if( theNode.GetPosition().distance3D(newNode.GetPosition()) < MinDistBetweenNodes )
		{
			theNode.Merge( newNode );
			return ;
		};
	}

	m_vNodeList.push_back(newNode);
}

void IntersectionNodesInAirport::AddNodes( const std::vector<IntersectionNode> newNodes )
{
	for(int i=0;i <(int)newNodes.size();i++)
	{
		AddNode(newNodes[i]);
	}
}

void IntersectionNodesInAirport::GetIntersectionNodeOf( int nObjID, std::vector<IntersectionNode>& reslts )const
{
	for (int i=0;i<(int)m_vNodeList.size();i++)
	{
		IntersectionNode node = m_vNodeList.at(i);
		if( node.HasObject(nObjID))
			reslts.push_back(node);
	}
}

int IntersectionNodesInAirport::GetCount() const
{
	return (int)m_vNodeList.size();
}

IntersectionNode& IntersectionNodesInAirport::NodeAt( int idx )
{
	return m_vNodeList[idx];
}

const IntersectionNode& IntersectionNodesInAirport::NodeAt( int idx ) const
{
	return m_vNodeList[idx];
}

IntersectionNodesInAirport::IntersectionNodesInAirport()
{
}

IntersectionNodesInAirport::IntersectionNodesInAirport( const IntersectionNodesInAirport& nodes)
{
	m_vNodeList = nodes.m_vNodeList;
}

IntersectionNodesInAirport::~IntersectionNodesInAirport()
{

}
//
int IntersectionNodesInAirport::FindNodeIndex( int nObjID1, DistanceUnit distofObj1, int nObjID2 ) const
{
	DistanceUnit minDist = ARCMath::DISTANCE_INFINITE;
	int nFitNodeId = -1;
	for (int i=0;i<GetCount();i++)
	{
		IntersectionNode theNode = NodeAt(i);
		if( theNode.HasObject(nObjID1) && theNode.HasObject(nObjID2) ){
			DistanceUnit theDist = theNode.GetDistance(nObjID1);
			DistanceUnit offsetDist = abs(theDist - distofObj1);
			if( offsetDist< minDist)
			{
				minDist = offsetDist;
				nFitNodeId = i;
			}				
		}
	}
	return nFitNodeId;
}


IntersectionNode IntersectionNodesInAirport::GetNodeByID(int id) const
{
	IntersectionNode node;
	for (int i=0;i<GetCount();i++)
	{
		if (NodeAt(i).GetID() == id)
		{
			node = NodeAt(i);
			break;
		}
	}

	return node;

}

void IntersectionNodesInAirport::ExportIntersectionNodes(CAirsideExportFile& exportFile)
{
	exportFile.getFile().writeField(_T("ID, NAME, AIRPORTID, OBJECTID1, OBJ1DISTPOS, OBJECTID2, OBJ2DISTPOS, NODEINDEX, POS_X, POS_Y, POS_Z"));
	exportFile.getFile().writeLine();

	CString strSQL =_T("");
	strSQL.Format(_T("SELECT * FROM INTERSECTIONNODE WHERE AIRPORTID IN (SELECT ID FROM ALTAIRPORT WHERE PROJID = %d) "),exportFile.GetProjectID());

	CADORecordset adoRecordset;
	long nItemCount =-1;
	CADODatabase::ExecuteSQLStatement(strSQL,nItemCount,adoRecordset);

	while (!adoRecordset.IsEOF())
	{
		IntersectionNode itersecNode;
		itersecNode.ReadData(adoRecordset);
		itersecNode.ExportIntersectionNode(exportFile);
		exportFile.getFile().writeLine();
		adoRecordset.MoveNextData();
	}
	exportFile.getFile().endFile();
}

void IntersectionNodesInAirport::ImportIntersectionNodes(CAirsideImportFile& importFile)
{
	if(importFile.getVersion() < 1038)
		return;

	while (!importFile.getFile().isBlank())
	{
		IntersectionNode intersecNode;
		intersecNode.ImportIntersectionNode(importFile);
		importFile.getFile().getLine();
	}

}


//set node ref obj to the object in the object list
void IntersectionNodesInAirport::Init( int nAirportID, ALTObjectList objList )
{
	m_nAirportID = nAirportID;
	Clear();
	ReadData(nAirportID);	
	for(int i=0;i<(int)m_vNodeList.size();i++)
	{
		IntersectionNode& theNode = m_vNodeList[i];
		theNode.SetObject(objList);		
	}	
}

void RemoveIntersectionOfObject( int nObjID, IntersectionNodeList& nodeList)
{
	for(int i=0;i< (int) nodeList.size();i++)
	{
		nodeList[i].RemoveObjectItem(nObjID);
	}

	IntersectionNodeList oldList = nodeList;
	nodeList.clear();
	for(int i=0;i<(int)oldList.size();i++)
	{
		if( oldList[i].IsValid() )
		{
			nodeList.push_back( oldList[i] );
		}
	}
}

void UpdateNodesIdx(IntersectionNodesInAirport& nodesInAirport, const std::vector<int>& vNodeIndexs)
{
	for(int i=0;i< (int)vNodeIndexs.size();i++)
	{
		IntersectionNode& theNode = nodesInAirport.NodeAt(vNodeIndexs[i]);
		theNode.SetIndex(0);
	}

	for(int i=0;i<(int)vNodeIndexs.size()-1;i++ )
	{
		IntersectionNode& nodeOrig = nodesInAirport.NodeAt(vNodeIndexs[i]);
		
		for(int j=i+1;j<(int)vNodeIndexs.size();j++)
		{
			IntersectionNode& nodeCmp = nodesInAirport.NodeAt(vNodeIndexs[j]);
			if( nodeCmp.IsIdenticalWithOutIndex(nodeOrig) ){
				nodeCmp.SetIndex( nodeOrig.GetIndex()+1);
				break;
			}

		}
	}

}

void UpdateNodesName(IntersectionNodesInAirport& nodesInAirport,const std::vector<int>& vNodeIndexs)
{
	for(int i=0;i< (int)vNodeIndexs.size();i++)
	{
		IntersectionNode& theNode = nodesInAirport.NodeAt(vNodeIndexs[i]);
		theNode.UpdateName();
	}

}
ALTObjectUIDList GetObjectUIDList(ALTObjectList objList)
{
	ALTObjectUIDList vObjIDList;
	for(int i=0;i<(int)objList.size();i++)
	{
		vObjIDList.push_back( objList.at(i)->getID() );
	}
	return vObjIDList;
}

std::vector<int> IntersectionNodesInAirport::ReflectChangeOf( ALTObjectList changObjs,ALTObjectList objList )
{	
	
	IntersectionNodeList vOldNodes = m_vNodeList;

	std::vector<int> vChangeNodeIndexs;
	IntersectionNodeList newNodeList;		
	ALTObjectUIDList changeObjIDs = GetObjectUIDList(changObjs);

	for(int i=0;i <(int)m_vNodeList.size();i++)
	{
		IntersectionNode& theNode = m_vNodeList[i];

		bool bChanged = theNode.HasObjectIn(changeObjIDs);

		for(int j=0;j< (int)changeObjIDs.size();j++)
		{
			int changeOjbID = changeObjIDs.at(j);
			theNode.RemoveObjectItem(changeOjbID);
		}
		if(theNode.IsValid())
		{
			newNodeList.push_back(theNode);
			if(bChanged)
				vChangeNodeIndexs.push_back((int)newNodeList.size()-1);
		}
	}
	m_vNodeList = newNodeList;

	//calculate intersection nodes
	for(int i=0;i< (int) changObjs.size();i++)
	{
		ALTObject * pChangObj = changObjs[i].get();
		for(int j=0;j< (int)objList.size();j++)
		{
			ALTObject* OtherObj = objList[j].get();
			if( OtherObj->getID() != pChangObj->getID() )
			{
				std::vector<IntersectionNode> reslts;
				GetIntersectionNodes(pChangObj,OtherObj,reslts);
				AddNodes(reslts);
			}
		}

	}
	
	for(int i=0;i< (int)m_vNodeList.size();i++)
	{
		IntersectionNode& theNode = m_vNodeList[i];
		if(theNode.HasObjectIn(changeObjIDs))
		{
			if( vChangeNodeIndexs.end()== std::find(vChangeNodeIndexs.begin(), vChangeNodeIndexs.end(), i) )
			{
				vChangeNodeIndexs.push_back(i);
			}
		}
	}
	
	UpdateNodesIdx(*this, vChangeNodeIndexs);
	UpdateNodesName(*this, vChangeNodeIndexs);
	//
	for(int i=0;i< (int)m_vNodeList.size();i++)
	{
		IntersectionNode& theNewNode = m_vNodeList[i];
		for(int j=0;j<(int)vOldNodes.size();j++)
		{
			IntersectionNode& theOldNode = vOldNodes[j];
			if( theNewNode.GetID() == theOldNode.GetID()|| theNewNode.IsIdentical(theOldNode) )
			{
				theNewNode.SetID(theOldNode.GetID());
				vOldNodes.erase( vOldNodes.begin() + j);
				break;
			}
		}
	}
	for(int i=0;i< (int)vOldNodes.size();i++)
	{		
		IntersectionNode& theOldNode = vOldNodes[i];
		theOldNode.DeleteData();
	}
	UpdateDataToDB(m_nAirportID, vChangeNodeIndexs);

	return vChangeNodeIndexs;
}

std::vector<int> IntersectionNodesInAirport::ReflectRemoveOf( ALTObjectList objList )
{
	std::vector<int> vChangeNodeIndexs;
	//IntersectionNodeList vNodes = m_vNodeList;
	IntersectionNodeList newNodeList;	
	ALTObjectUIDList objIDList = GetObjectUIDList(objList);
	for(int i=0;i <(int)m_vNodeList.size();i++)
	{
		IntersectionNode& theNode = m_vNodeList[i];

		bool bChanged = theNode.HasObjectIn(objIDList);
		
		for(int j=0;j< (int)objIDList.size();j++)
		{
			int nObjID = objIDList.at(j);
			theNode.RemoveObjectItem(nObjID);
		}
		if(theNode.IsValid())
		{
			newNodeList.push_back(theNode);
			if(bChanged)
				vChangeNodeIndexs.push_back((int)newNodeList.size()-1);
		}
	}
	m_vNodeList = newNodeList;
	UpdateNodesIdx(*this, vChangeNodeIndexs);
	UpdateNodesName(*this, vChangeNodeIndexs);

	UpdateDataToDB(m_nAirportID, vChangeNodeIndexs);

	return vChangeNodeIndexs;
}



bool IntersectionNodesInAirport::HasNode( int nId ) const
{
	for(int i=0;i< (int)m_vNodeList.size();i++)
	{
		const IntersectionNode& theNode = m_vNodeList[i];
		if(theNode.GetID() == nId)
		{
			return  true;
		}
	}
	return false;
}



void IntersectionNodesInAirport::UpdateDataToDB( int nAirportID, const std::vector<int>& vUpdateNodeIndexs )
{
	/*IntersectionNodeList DbIntersections;
	IntersectionNode::ReadNodeList(nAirportID,DbIntersections);*/

	/*int nNodeIdx =0;
	for(IntersectionNodeList::iterator itrClac = m_vNodeList.begin();itrClac != m_vNodeList.end(); ++itrClac)
	{		
		for(IntersectionNodeList::iterator itrDB = DbIntersections.begin();itrDB != DbIntersections.end();itrDB ++)
		{
			if( itrClac->GetID() == itrDB->GetID() || (*itrClac).IsIdentical( *itrDB ) )
			{
				(*itrClac).m_nID = (*itrDB).m_nID;
				DbIntersections.erase(itrDB);
				break;
			}
		}		
	}

	for(IntersectionNodeList::iterator itrDB = DbIntersections.begin();itrDB != DbIntersections.end();itrDB ++)
	{
		(*itrDB).DeleteData();
	}*/
	
	////delete invalid nodes
	//for(IntersectionNodeList::iterator itrOld = m_vOldList.begin();itrOld != m_vOldList.end();itrOld ++)
	//{
	//	IntersectionNode& theOldNode = *itrOld;
	//	bool bNodeInNewList = false;
	//	for(IntersectionNodeList::iterator itrClac = m_vNodeList.begin();itrClac != m_vNodeList.end(); ++itrClac)
	//	{
	//		IntersectionNode& theNewNode = *itrClac;
	//		if( theNewNode.GetID() == theOldNode.GetID() || theNewNode.IsIdentical(theOldNode ) )
	//		{
	//			theNewNode.m_nID = theOldNode.m_nID;
	//			bNodeInNewList = true;
	//			break;
	//		}
	//	}
	//	if( !bNodeInNewList )
	//		theOldNode.DeleteData();
	//	
	//}
	for(int i=0;i< (int)vUpdateNodeIndexs.size();i++)
	{
		IntersectionNode& theNode = NodeAt(vUpdateNodeIndexs[i]);
		theNode.SaveData(nAirportID);
	}

	//m_vOldList = m_vNodeList;

}