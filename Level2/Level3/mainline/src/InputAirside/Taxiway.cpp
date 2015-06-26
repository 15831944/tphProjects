#include "StdAfx.h"
#include ".\taxiway.h"
#include "../Common/line.h"
#include "..\Common\ARCVector.h"
#include "../Common/DistanceLineLine.h"
#include <algorithm>
#include "Runway.h"
#include "stand.h"
#include "AirsideImportExportManager.h"
#include "ALTAirport.h"
#include "DeicePad.h"
#include "ALTObjectDisplayProp.h"
#include "HoldPosition.h"
#include "IntersectionNodesInAirport.h"
#include "FiletTaxiway.h"

Taxiway::Taxiway(void)
{
	m_path.init(0,NULL);
	m_dWidth = 2500;
	m_sMarking = _T("Marking"); 
	m_fMarkingPos= 0.0;
	m_bShow = FALSE;
}


Taxiway::~Taxiway(void)
{
}

void Taxiway::SetWidth( double width )
{
	m_dWidth = width;
}	

double Taxiway::GetWidth()
{
	return m_dWidth;
}

void Taxiway::SetMarking( const std::string& _sMarking )
{
	m_sMarking =  _sMarking;
}

const std::string& Taxiway::GetMarking() const
{
	return m_sMarking;
}

const CPath2008& Taxiway::GetPath() const
{
	return m_path;
}

void Taxiway::SetPath( const CPath2008& _path )
{
	m_path = _path;
}

CPath2008& Taxiway::getPath()
{
	return m_path;
}

void Taxiway::SetMarkingPos( double fpos )
{
	m_fMarkingPos = fpos;
}

double Taxiway::GetMarkingPos() const
{
	return m_fMarkingPos;
}
CString Taxiway::GetSelectScript(int nObjID) const
{	
	CString strSQL = _T("");
	strSQL.Format(_T("SELECT APTID, NAME_L1, NAME_L2, NAME_L3, NAME_L4, LOCKED, PATHID, POINTCOUNT, PATH, WIDTH,SHOW, \
		MARKING,MARKINGPOS, TSTAMP\
		FROM TAXIWAY_VIEW\
		WHERE ID = %d"),nObjID);
	return strSQL;
}
CString Taxiway::GetInsertScript( int nObjID) const
{
	CString strSQL = _T("");

	strSQL.Format(_T("INSERT INTO TAXIWAYPROP(OBJID, PATH, WIDTH, MARKING,MARKINGPOS,SHOW) \
		VALUES (%d ,%d,%f,'%s',%f,%d)"),nObjID,m_nPathID,m_dWidth,m_sMarking.c_str(),m_fMarkingPos,(int)m_bShow);

	return strSQL;
}
CString Taxiway::GetUpdateScript(int nObjID) const
{	
	CString strSQL = _T("");
	strSQL.Format(_T("UPDATE TAXIWAYPROP SET WIDTH = %f, MARKING ='%s',MARKINGPOS = %f ,SHOW=%d\
		WHERE OBJID = %d"),m_dWidth,m_sMarking.c_str(),m_fMarkingPos,m_bShow,nObjID);
	return strSQL;
}
CString Taxiway::GetDeleteScript(int nObjID) const
{
	CString strSQL = _T("");
	strSQL.Format(_T("DELETE FROM TAXIWAYPROP \
		WHERE OBJID = %d"),nObjID);

	return strSQL;
}

//NAME_L1, NAME_L2, NAME_L3, NAME_L4, PATHID, POINTCOUNT, PATH, WIDTH, \
//MARKING, TSTAMP
void Taxiway::ReadObject(int nObjID)
{
	m_nObjID = nObjID;
	CADORecordset adoRecordset;
	long nRecordCount = -1;
	CADODatabase::ExecuteSQLStatement(GetSelectScript(nObjID),nRecordCount,adoRecordset);

	if (!adoRecordset.IsEOF())
	{	


		ReadRecord(adoRecordset);
		ReadSurfaceData();
	}
	
	//m_vAllHoldPositions.clear();
	//HoldPosition::ReadHoldPositionList(nObjID, m_vAllHoldPositions);
	
}

void Taxiway::ReadSurfaceData()
{
	m_ObjTaxiSur.ReadData(m_nObjID,TRUE,ALT_TAXIWAY,No_Type);
}

int Taxiway::SaveObject(int AirportID)
{
	int nTaxiwayID = ALTObject::SaveObject(AirportID,ALT_TAXIWAY);

	m_nPathID =  CADODatabase::SavePath2008IntoDatabase(m_path);

	CString strInsertScript = GetInsertScript(nTaxiwayID);
	
	CADODatabase::ExecuteSQLStatement(strInsertScript);
	
	SaveSurfaceData();

	return nTaxiwayID;
}

int Taxiway::SaveSurfaceData()
{
	m_ObjTaxiSur.SaveData(m_nObjID,TRUE,ALT_TAXIWAY,No_Type);
	return TRUE;
}

void Taxiway::UpdatePath()
{
	CADODatabase::UpdatePath2008InDatabase(m_path,m_nPathID);
}
void Taxiway::UpdateObject(int nObjID)
{
	ALTObject::UpdateObject(nObjID);

	CString strUpdateScript = GetUpdateScript(nObjID);
	
	CADODatabase::ExecuteSQLStatement(strUpdateScript);

	UpdateSurfaceData();
}

void Taxiway::UpdateSurfaceData()
{
	m_ObjTaxiSur.UpdateData();
}

void Taxiway::DeleteObject(int nObjID)
{
	ALTObject::DeleteObject(nObjID);
	
	CADODatabase::DeletePathFromDatabase(m_nPathID);

	CString strDeleteScript = GetDeleteScript(nObjID);
	CADODatabase::ExecuteSQLStatement(strDeleteScript);
	
	DeleteSurfaceData();
}

void Taxiway::DeleteSurfaceData()
{
	m_ObjTaxiSur.DeleteData();
}

void Taxiway::GetMarkingPosition( CPoint2008 & startPos, double & rotateAngle )
{	
	
	double readMarkpos = m_fMarkingPos / 100;
	int m_imarkPosAfterPtID = (int) ( readMarkpos );
	double m_dmarkPosRelatePos = readMarkpos - m_imarkPosAfterPtID;
	
	if(m_imarkPosAfterPtID >m_path.getCount()-2) m_imarkPosAfterPtID=m_path.getCount()-2;

	if(m_imarkPosAfterPtID < 0) return;

	CLine2008 _line( m_path.getPoint(m_imarkPosAfterPtID),m_path.getPoint(m_imarkPosAfterPtID+1));
	startPos= _line.getInlinePoint(m_dmarkPosRelatePos);
	ARCVector2 k;
	k.reset(m_path.getPoint(m_imarkPosAfterPtID),m_path.getPoint(m_imarkPosAfterPtID+1));
	rotateAngle = k.AngleFromCoorndinateX();
	// move to the center
	//k.Rotate(90);k.SetLength(GetWidth()*0.5);
	//startPos += CPoint2008(k[VX],k[VY],0);
	
}

ALTObject * Taxiway::NewCopy()
{
	Taxiway * reslt = new Taxiway;
	reslt->CopyData(this);
	return reslt;
}


//find the min max fillet dist to the node in the taxiway
void GetMinMaxFilletDistToNode(const FilletTaxiwaysInAirport& fillets, const IntersectionNode& node, Taxiway* pTaxiway, DistanceUnit& minDist, DistanceUnit& maxDist )
{
	DistanceUnit dMin = 0; DistanceUnit dMax= 0;
	
	const int nObjID = pTaxiway->getID();
	for(int i=0;i< fillets.GetCount();i++)
	{
		FilletTaxiway theFillet = fillets.ItemAt(i);
		if( theFillet.GetIntersectNode().GetID() == node.GetID() )
		{
			FilletPoint filetPt;
			if( theFillet.GetObject1ID() == nObjID )
			{
				filetPt = theFillet.GetFilletPoint1();
			}
			if( theFillet.GetObject2ID() == nObjID )
			{
				filetPt = theFillet.GetFilletPoint2();
			}
			
			DistanceUnit distToNode = filetPt.GetDistToIntersect();
			if( distToNode < 0 && distToNode < dMin )
			{
				dMin = distToNode;
			}
			if( distToNode > 0 && distToNode > dMax )
			{
				dMax = distToNode;
			}
		}
	}
	minDist = dMin;
	maxDist = dMax;
}

void Taxiway::UpdateHoldPositions(const ALTAirport& airport, std::vector<Runway*>vRunways,const FilletTaxiwaysInAirport& fillets, const IntersectionNodesInAirport& nodelist)
{
	HoldPositionList newHoldList;	
	//	
	for(int i=0;i<(int)vRunways.size();++i)
	{
		CalculateHoldPosition(vRunways.at(i), airport, newHoldList, nodelist);
	}

	//
	HoldPositionList fileHoldPositions;
	for(int i=0;i<nodelist.GetCount();i++)
	{
		IntersectionNode theNode = nodelist.NodeAt(i);
		if(theNode.GetRunwayIntersectItemList().size() && theNode.HasObject(getID()) )
		{
			TaxiwayIntersectItem* pTaxiItem = theNode.GetTaxiwayItem(getID()) ;
			DistanceUnit distToNode = 0;
			DistanceUnit dMin = 0; DistanceUnit dMax = 0;
			GetMinMaxFilletDistToNode(fillets, theNode, this, dMin, dMax );
			if(dMin < 0)
			{
				fileHoldPositions.push_back(HoldPosition(theNode, dMin + pTaxiItem->GetDistInItem(), getID()) );
			}
			if( dMax > 0)
			{
				fileHoldPositions.push_back(HoldPosition(theNode,dMax + pTaxiItem->GetDistInItem(),getID()));
			}
		} 
	}
	
	//adjust the hold position behind the fillet
	for(int i=0; i< (int)newHoldList.size();++i)
	{
		HoldPosition& theHold = newHoldList[i];
		for(int j=0;j<(int)fileHoldPositions.size();++j)
		{
			HoldPosition& filetHold= fileHoldPositions[j];
			if(filetHold.GetIntersectNode().GetID() == theHold.GetIntersectNode().GetID() && filetHold.IsFirst() == theHold.IsFirst() )
			{
				if(theHold.IsFirst() && theHold.m_intersectPos > filetHold.m_intersectPos )
				{
					theHold.m_intersectPos = filetHold.m_intersectPos;
				} 
				if( !theHold.IsFirst() && theHold.m_intersectPos < filetHold.m_intersectPos )
				{
					theHold.m_intersectPos = filetHold.m_intersectPos;
				}
				break;
			}
		}
	}


	for(int i=0;i<(int)newHoldList.size();i++)
	{
		HoldPosition& theNewHold = newHoldList[i];
		
		int idx = GetHoldPositionsIndex( theNewHold.m_intersectNode , theNewHold.IsFirst() );
		if(idx>=0)
		{
			HoldPosition& hold = m_vAllHoldPositions[idx];
			hold.m_intersectNode = theNewHold.m_intersectNode;
			hold.m_intersectPos = theNewHold.m_intersectPos;
			hold.m_strSign = theNewHold.m_strSign;
			theNewHold = hold;
		}
	}
	m_vAllHoldPositions = newHoldList;
	
	UpdateHoldPositionsToDB();
}

void Taxiway::GetAllHoldPositions( HoldPositionList& holdlist ) const
{
	holdlist = m_vAllHoldPositions;
}


void Taxiway::ExportTaxiways(int nAirportID,CAirsideExportFile& exportFile)
{

	std::vector<int> vTaxiwayID;
	ALTAirport::GetTaxiwaysIDs(nAirportID,vTaxiwayID);
	int nTaxiwayCount = static_cast<int>(vTaxiwayID.size());
	for (int i =0; i < nTaxiwayCount;++i)
	{
		Taxiway taxiwayObj;
		taxiwayObj.ReadObject(vTaxiwayID[i]);
		taxiwayObj.ExportTaxiway(exportFile);
	}

}
void Taxiway::ExportTaxiway(CAirsideExportFile& exportFile)
{
	ALTObject::ExportObject(exportFile);
	exportFile.getFile().writeInt(m_nAptID);
	int nPathCount = m_path.getCount();
	exportFile.getFile().writeInt(nPathCount);
	for (int i=0; i < nPathCount; ++ i)
	{
		exportFile.getFile().writePoint2008(m_path.getPoint(i));
	}

	exportFile.getFile().writeDouble(m_dWidth);
	exportFile.getFile().writeField(m_sMarking.c_str());
	exportFile.getFile().writeDouble(m_fMarkingPos);
	exportFile.getFile().writeLine();


	TaxiwayDisplayProp dspProp;
	dspProp.ReadData(m_nObjID);
	dspProp.ExportDspProp(exportFile);


}
void Taxiway::ImportObject(CAirsideImportFile& importFile)
{
	ALTObject::ImportObject(importFile);
	int nAirportID = -1;
	importFile.getFile().getInteger(nAirportID);
	m_nAptID = importFile.GetAirportNewIndex(nAirportID);

	int nPathCount = 0;
	importFile.getFile().getInteger(nPathCount);
	m_path.init(nPathCount);
	CPoint2008 *pPoint = m_path.getPointList();
	for (int i = 0; i < nPathCount; ++i)
	{
		importFile.getFile().getPoint2008(pPoint[i]);
	}
	importFile.getFile().getFloat(m_dWidth);
	char chMark[512];
	importFile.getFile().getField(chMark,512);
	m_sMarking = chMark;
	importFile.getFile().getFloat(m_fMarkingPos);

	int nOldObjectID = m_nObjID;
	int nNewObjID = SaveObject(m_nAptID);
	importFile.AddObjectIndexMap(nOldObjectID,nNewObjID);

	importFile.getFile().getLine();

	TaxiwayDisplayProp dspProp;
	dspProp.ImportDspProp(importFile,m_nObjID);
}


void Taxiway::CleanAllHoldPosition()
{
	m_vAllHoldPositions.clear();
}

IntersectionNodeList Taxiway::GetIntersectNodes() const
{
	IntersectionNodeList vIntersectNode;
	IntersectionNode::ReadIntersecNodeList(m_nObjID, vIntersectNode);
	return vIntersectNode;
}

void Taxiway::UpdateHoldPositionsToDB()
{
	std::vector<HoldPosition> vDBHoldPosition;
	HoldPosition::ReadHoldPositionList(m_nObjID,vDBHoldPosition);

	for (size_t j =0; j < m_vAllHoldPositions.size(); ++ j)
	{
		for (size_t i =0; i < vDBHoldPosition.size(); ++i)
		{
			if(vDBHoldPosition[i].IsFirst() == m_vAllHoldPositions[j].IsFirst()
				&&vDBHoldPosition[i].GetIntersectNode().GetID() == m_vAllHoldPositions[j].GetIntersectNode().GetID())
			{
                m_vAllHoldPositions[j].m_nUniqueID = vDBHoldPosition[i].m_nUniqueID; 
				m_vAllHoldPositions[j].m_distToIntersect = vDBHoldPosition[i].m_distToIntersect;

				vDBHoldPosition.erase(vDBHoldPosition.begin() + i);
				break;
			}
		}
		m_vAllHoldPositions[j].SaveData(getID());
	}


	for (size_t ndel = 0; ndel < vDBHoldPosition.size(); ++ ndel)
	{
		vDBHoldPosition[ndel].DeleteData();
	}		
}
int Taxiway::GetHoldPositionsIndex( const IntersectionNode& intersect, bool bfirst ) const
{
	for(int i =0 ;i<(int)m_vAllHoldPositions.size(); i++)
	{
		const HoldPosition& hold = m_vAllHoldPositions[i];
		if(hold.m_intersectNode == intersect && hold.IsFirst() == bfirst ) return i;
	}
	return -1;
}

void Taxiway::CalculateHoldPosition( Runway * pRunway,const ALTAirport& airport, HoldPositionList& list ,const IntersectionNodesInAirport& nodelist)
{
	int Rwid = pRunway->getID();
	ASSERT(pRunway->getPath().getCount()>1);

	HoldPositionInfoList vIntersections; // center intersect, side intersect
	GetHoldPositionsInfo(m_path,GetWidth(), pRunway->GetPath(),pRunway->GetWidth(),vIntersections,true);

	
	for( int i=0 ;i < (int)vIntersections.size() ; i++ )
	{
		const HoldPositionInfo& holdinfo  = vIntersections.at(i);
		
		int intnodeidx= nodelist.FindNodeIndex(getID(),holdinfo.first,pRunway->getID());
		//ASSERT(intnodeidx >=0);
		if(intnodeidx <0)
			continue;
		const IntersectionNode& intnode = nodelist.NodeAt(intnodeidx);
		DistanceUnit dOffset = 500;
		if(holdinfo.IsFirst())
			dOffset = - dOffset;

		HoldPosition newHold(intnode, holdinfo.second + dOffset,getID());	
		EnumCardinalDirection dir = pRunway->GetPointCadDir(airport, GetPath().GetDistPoint(holdinfo.second) );		
		newHold.m_strSign.Format("%s-%s",intnode.GetName().GetString(),CardDirString[dir].GetString() );
		list.push_back(newHold);		
	}
}

//void Taxiway::CalculateHoldPosition( Taxiway * pTaxiway, HoldPositionList& list,const IntersectionNodesInAirport& nodelist )
//{
//	int taxiwayId = pTaxiway->getID();
//
//	HoldPositionInfoList vIntersections; // center intersect, side intersect
//	GetHoldPositionsInfo(m_path,GetWidth(),pTaxiway->GetPath(),pTaxiway->GetWidth(),vIntersections,false);
//
//	for( int i=0 ;i < (int)vIntersections.size() ; i++ )
//	{
//		HoldPositionInfo holdinfo  = vIntersections.at(i);
//		IntersectionNode intnode;
//		int intnodeidx= nodelist.FindNodeIndex(getID(),holdinfo.first,pTaxiway->getID());
//		if(intnodeidx <0)
//			continue;
//		intnode = nodelist.NodeAt(intnodeidx);
//		//IntersectNode node(pTaxiway, holdinfo.intersectidx, holdinfo.first);
//		HoldPosition newHold(intnode, holdinfo.second,getID());		
//		list.push_back(newHold);			
//	}
//}

//void Taxiway::CalculateHoldPosition( Stand *pStand, HoldPositionList& list ,const IntersectionNodesInAirport& nodelist)
//{
	//int standID = pStand->getID();
	//HoldPositionInfoList vIntersections; // center intersect, side intersect
	//GetHoldPositionsInfo(m_path,5,pStand->GetInContrain(),5,vIntersections,false);
	//for( int i=0 ;i < (int)vIntersections.size() ; i++ )
	//{
	//	HoldPositionInfo holdinfo  = vIntersections.at(i);
	////	IntersectNode node(pStand, holdinfo.intersectidx, holdinfo.first);
	//	IntersectionNode intnode;
	//	int intnodeidx= nodelist.FindNodeIndex(getID(),holdinfo.first,pStand->getID());
	//	//ASSERT(intnodeidx >=0);
	//	if(intnodeidx <0)
	//		continue;
	//	intnode = nodelist.NodeAt(intnodeidx);
	//	HoldPosition newHold(intnode, holdinfo.second,getID());		
	//	list.push_back(newHold);			
	//}
	//vIntersections.clear();
	//
	//if(!pStand->IsUsePushBack())
	//{
	//	vIntersections.clear();
	//	GetHoldPositionsInfo(m_path,5,pStand->GetOutConstrain(),5,vIntersections,false);
	//	
	//	for( int i=0 ;i < (int)vIntersections.size() ; i++ )
	//	{
	//		HoldPositionInfo holdinfo  = vIntersections.at(i);
	//		IntersectionNode intnode;
	//		int intnodeidx= nodelist.FindNodeIndex(getID(),holdinfo.first,pStand->getID());
	//		//ASSERT(intnodeidx >=0);

	//		if(intnodeidx <0)
	//			continue;
	//		intnode = nodelist.NodeAt(intnodeidx);

	//		//IntersectNode node(pStand, holdinfo.intersectidx +1, holdinfo.first);
	//		HoldPosition newHold(intnode, holdinfo.second,getID());		
	//		list.push_back(newHold);			
	//	}
	//}
	
//}

//void Taxiway::CalculateHoldPosition( DeicePad* pDeicepad ,HoldPositionList& list ,const IntersectionNodesInAirport& nodelist)
//{
//	int deiceID = pDeicepad->getID();
//	HoldPositionInfoList vIntersections; // center intersect, side intersect
//	GetHoldPositionsInfo(m_path,5,pDeicepad->GetInContrain(),5,vIntersections,false);
//	if(!pDeicepad->IsUsePushBack())
//		GetHoldPositionsInfo(m_path,5,pDeicepad->GetOutConstrain(),5,vIntersections,false);
//
//	for( int i=0 ;i < (int)vIntersections.size() ; i++ )
//	{
//		HoldPositionInfo holdinfo  = vIntersections.at(i);
//		IntersectionNode intnode;
//		int intnodeidx= nodelist.FindNodeIndex(getID(),holdinfo.first,pDeicepad->getID());
//		//ASSERT(intnodeidx >=0);
//		if(intnodeidx <0)
//			continue;
//
//		intnode = nodelist.NodeAt(intnodeidx);
//
//		//IntersectNode node(pDeicepad, holdinfo.intersectidx, holdinfo.first);
//		HoldPosition newHold(intnode, holdinfo.second,getID());		
//		list.push_back(newHold);			
//	}
//}

bool Taxiway::GetHoldPositionLine( int idx, CLine2008& line ) const
{
	return HoldPositionFunc::CaculateHoldPositionLine(m_path,m_dWidth,m_vAllHoldPositions,idx,line);
}

bool Taxiway::CopyData( const ALTObject* pObj )
{
	if (this == pObj)
		return true;

	if( pObj->GetType() == GetType() )
	{
		Taxiway * pOtherTaxi = (Taxiway*)pObj;
		m_path = pOtherTaxi->GetPath();
		m_dWidth = pOtherTaxi->GetWidth();
		m_fMarkingPos = pOtherTaxi->GetMarkingPos();
		m_sMarking = pOtherTaxi->GetMarking();
		m_bShow = pOtherTaxi->GetShow();
		m_ObjTaxiSur = pOtherTaxi->GetObjSurface();

		
	}
	return __super::CopyData(pObj);
}

//init the hold positions
void Taxiway::InitHoldPositions( const IntersectionNodesInAirport& nodelist )
{
	m_vAllHoldPositions.clear();
	HoldPosition::ReadHoldPositionList( getID(), m_vAllHoldPositions);
	for(int i=0;i< (int)m_vAllHoldPositions.size();i++)
	{
		HoldPosition& theHold = m_vAllHoldPositions[i];
		int nNodeID = theHold.m_intersectNode.GetID();
		theHold.m_intersectNode = nodelist.GetNodeByID(nNodeID);
	}
}

void Taxiway::ReadRecord( CADORecordset& adoRecordset )
{
	ALTObject::ReadObject(adoRecordset);

	adoRecordset.GetFieldValue(_T("PATHID"),m_nPathID);
	int nPointCount = -1;
	adoRecordset.GetFieldValue(_T("POINTCOUNT"),nPointCount);
	adoRecordset.GetFieldValue(_T("PATH"),nPointCount,m_path);

	adoRecordset.GetFieldValue(_T("WIDTH"),m_dWidth);
	adoRecordset.GetFieldValue(_T("MARKING"),m_sMarking);

	adoRecordset.GetFieldValue(_T("MARKINGPOS"),m_fMarkingPos);
	int bShow = 0;
	adoRecordset.GetFieldValue(_T("SHOW"),bShow);
	m_bShow = (BOOL)bShow;

	

}

const GUID& Taxiway::getTypeGUID()
{
	// {BFDFC00F-C2EC-4c24-8174-DE6783DDF8D9}
	static const GUID name = 
	{ 0xbfdfc00f, 0xc2ec, 0x4c24, { 0x81, 0x74, 0xde, 0x67, 0x83, 0xdd, 0xf8, 0xd9 } };
	return name;

}

ALTObjectDisplayProp* Taxiway::NewDisplayProp()
{
	return new TaxiwayDisplayProp();
}
