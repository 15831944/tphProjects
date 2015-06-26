#include "StdAfx.h"

#include "../Common/line.h"
#include ".\runway.h"
#include ".\Taxiway.h"
#include "../Common/DistanceLineLine.h"

#include "AirsideImportExportManager.h"
#include "ALTAirport.h"
#include "../Common/GetIntersection.h"
#include "ALTObjectDisplayProp.h"
#include "IntersectionNodesInAirport.h"
#include "../Common/Point2008.h"

Runway::Runway(void)
{
	m_path.init(0,NULL);
	m_dWidth= 6500;
	m_sMarking1 = _T("Marking1");
	m_sMarking2 = _T("Marking2");
	m_dMark1LandingThreshold = 0.0;
	m_dMark1TakeOffThreshold = 0.0;

	m_dMark2LandingThreshold = 0.0;
	m_dMark2TakeOffThreshold = 0.0;
		
	m_bSurface1Show = FALSE;
	m_bSurface2Show = FALSE;
}

Runway::Runway( const Runway& rw):ALTObject(rw)
{
	*this = rw;
}
Runway::~Runway(void)
{
}


void Runway::SetPath( const CPath2008& _path )
{
	m_path = _path;
}

void Runway::SetWidth( double width )
{
	m_dWidth = width;
}

double Runway::GetWidth()const
{
	return m_dWidth;
}

void Runway::SetMarking1( const std::string& _sMarking )
{
	m_sMarking1 = _sMarking;
}

void Runway::SetMarking2( const std::string& _sMarking )
{
	m_sMarking2 = _sMarking;
}

const std::string& Runway::GetMarking1() const
{
	return m_sMarking1;
}

const std::string& Runway::GetMarking2() const
{
	return m_sMarking2;
}

void Runway::SetMark1LandingThreshold(double dThreshold)
{
	m_dMark1LandingThreshold = dThreshold;
}
double Runway::GetMark1LandingThreshold()const
{
	return m_dMark1LandingThreshold;	
}
void Runway::SetMark1TakeOffThreshold(double dThreshold)
{
	m_dMark1TakeOffThreshold = dThreshold;
}
double Runway::GetMark1TakeOffThreshold()const
{
	return m_dMark1TakeOffThreshold;
}

void Runway::SetMark2LandingThreshold(double dThreshold)
{
	m_dMark2LandingThreshold = dThreshold;
}
double Runway::GetMark2LandingThreshold()const
{
	return m_dMark2LandingThreshold;	
}
void Runway::SetMark2TakeOffThreshold(double dThreshold)
{
	m_dMark2TakeOffThreshold = dThreshold;
}
double Runway::GetMark2TakeOffThreshold()const
{
	return m_dMark2TakeOffThreshold;
}




// NAME_L1, NAME_L2, NAME_L3, NAME_L4, PATHID, PATH, POINTCOUNT, WIDTH, \
//MARKING1, MARKING2, TSTAMP
void Runway::ReadObject(int nObjID)
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

}

void Runway::ReadSurfaceData()
{
	m_ObjSurface1.EndOfRunway.ReadData(m_nObjID,TRUE,ALT_RUNWAY,EndOfRunway_Type);
	m_ObjSurface1.Lateral.ReadData(m_nObjID,TRUE,ALT_RUNWAY,Lateral_Type);
	m_ObjSurface1.ThresHold.ReadData(m_nObjID,TRUE,ALT_RUNWAY,ThresHold_Type);

	m_ObjSurface2.EndOfRunway.ReadData(m_nObjID,FALSE,ALT_RUNWAY,EndOfRunway_Type);
	m_ObjSurface2.Lateral.ReadData(m_nObjID,FALSE,ALT_RUNWAY,Lateral_Type);
	m_ObjSurface2.ThresHold.ReadData(m_nObjID,FALSE,ALT_RUNWAY,ThresHold_Type);
}

int Runway::SaveObject(int nAirportID)
{
	int nObjID = ALTObject::SaveObject(nAirportID,ALT_RUNWAY);

	m_nPathID = CADODatabase::SavePath2008IntoDatabase(m_path);
	
	CString strSQL = GetInsertScript(nObjID);

	CADODatabase::ExecuteSQLStatement(strSQL);

	SaveSurfaceData();

	return nObjID;
}

int Runway::SaveSurfaceData()
{
	m_ObjSurface1.ThresHold.SaveData(m_nObjID,TRUE,ALT_RUNWAY,ThresHold_Type);
	m_ObjSurface1.Lateral.SaveData(m_nObjID,TRUE,ALT_RUNWAY,Lateral_Type);
	m_ObjSurface1.EndOfRunway.SaveData(m_nObjID,TRUE,ALT_RUNWAY,EndOfRunway_Type);

	m_ObjSurface2.ThresHold.SaveData(m_nObjID,FALSE,ALT_RUNWAY,ThresHold_Type);
	m_ObjSurface2.Lateral.SaveData(m_nObjID,FALSE,ALT_RUNWAY,Lateral_Type);
	m_ObjSurface2.EndOfRunway.SaveData(m_nObjID,FALSE,ALT_RUNWAY,EndOfRunway_Type);

	return TRUE;
}
void Runway::UpdatePath()
{
	CADODatabase::UpdatePath2008InDatabase(m_path,m_nPathID);
}
void Runway::UpdateObject(int nObjID)
{
	ALTObject::UpdateObject(nObjID);

	CString strUpdateScript = GetUpdateScript(nObjID);
	CADODatabase::ExecuteSQLStatement(strUpdateScript);

	UpdataSurfaceData();
}

void Runway::UpdataSurfaceData()
{
	m_ObjSurface1.ThresHold.UpdateData();
	m_ObjSurface1.EndOfRunway.UpdateData();
	m_ObjSurface1.Lateral.UpdateData();

	m_ObjSurface2.ThresHold.UpdateData();
	m_ObjSurface2.EndOfRunway.UpdateData();
	m_ObjSurface2.Lateral.UpdateData();
}
void Runway::DeleteObject(int nObjID)
{
	DeleteSurfaceData();
	ALTObject::DeleteObject(nObjID);
	
	CADODatabase::DeletePathFromDatabase(m_nPathID);
	CString strDeleteScript = GetDeleteScript(nObjID);

	CADODatabase::ExecuteSQLStatement(strDeleteScript);
}

void Runway::DeleteSurfaceData()
{
	m_ObjSurface1.EndOfRunway.DeleteData();
	m_ObjSurface1.Lateral.DeleteData();
	m_ObjSurface1.ThresHold.DeleteData();

	m_ObjSurface2.EndOfRunway.DeleteData();
	m_ObjSurface2.Lateral.DeleteData();
	m_ObjSurface2.ThresHold.DeleteData();
}
CString Runway::GetSelectScript(int nObjID) const
{
	CString strSQL = _T("");
	strSQL.Format(_T("SELECT APTID, NAME_L1, NAME_L2, NAME_L3, NAME_L4, LOCKED, PATHID, PATH, POINTCOUNT, WIDTH, SHOW1,SHOW2,\
		MARKING1, MARKING2, TSTAMP,MARK1LTHRESHOLD,MARK1TTHRESHOLD,MARK2LTHRESHOLD,MARK2TTHRESHOLD\
		FROM RUNWAY_VIEW\
		WHERE ID = %d"),
		nObjID);

	return strSQL;
}

CString Runway::GetSelectScriptForPath2008(int nObjID) const
{
	CString strSQL = _T("");
	strSQL.Format(_T("SELECT ALTOBJECT.ID, ALTOBJECT.APTID, ALTOBJECT.NAME_L1, ALTOBJECT.NAME_L2, ALTOBJECT.NAME_L3, \
						ALTOBJECT.NAME_L4, ALTOBJECT.LOCKED, RUNWAYPROP.PATH AS PATHID, OBJPATH2008.PATH, OBJPATH2008.POINTCOUNT, RUNWAYPROP.WIDTH, \
						RUNWAYPROP.MARKING1, RUNWAYPROP.MARKING2, ALTOBJECT.TSTAMP, RUNWAYPROP.MARK1LTHRESHOLD, \
						RUNWAYPROP.MARK1TTHRESHOLD, RUNWAYPROP.MARK2LTHRESHOLD, RUNWAYPROP.MARK2TTHRESHOLD \
						FROM  RUNWAYPROP,ALTOBJECT,OBJPATH2008 \
						WHERE RUNWAYPROP.PATH = OBJPATH.ID \
						AND RUNWAYPROP.OBJID = ALTOBJECT.ID \
						AND ALTOBJECT.ID = %d"),
						nObjID);

	return strSQL;
}

CString Runway::GetInsertScript(int nObjID) const
{

	CString strSQL = _T("");
	strSQL.Format(_T("INSERT INTO RUNWAYPROP(OBJID, PATH, WIDTH, MARKING1, MARKING2,MARK1LTHRESHOLD,MARK1TTHRESHOLD,MARK2LTHRESHOLD,MARK2TTHRESHOLD,SHOW1,SHOW2) \
		VALUES (%d,%d,%f,'%s','%s',%f ,%f ,%f ,%f,%d,%d )"),
		nObjID,m_nPathID,m_dWidth,m_sMarking1.c_str(),m_sMarking2.c_str(),m_dMark1LandingThreshold,m_dMark1TakeOffThreshold,m_dMark2LandingThreshold,m_dMark2TakeOffThreshold,\
		(int)m_bSurface1Show,(int)m_bSurface2Show);

	return strSQL;
}
CString Runway::GetUpdateScript(int nObjID) const
{
	CString strSQL = _T("");
	strSQL.Format(_T("UPDATE RUNWAYPROP	SET WIDTH =%f, MARKING1 = '%s', MARKING2 = '%s' ,MARK1LTHRESHOLD=%f,MARK1TTHRESHOLD=%f,MARK2LTHRESHOLD=%f,MARK2TTHRESHOLD=%f,SHOW1=%d,SHOW2=%d\
		WHERE OBJID = %d"),m_dWidth,m_sMarking1.c_str(),m_sMarking2.c_str(),m_dMark1LandingThreshold,m_dMark1TakeOffThreshold,m_dMark2LandingThreshold,m_dMark2TakeOffThreshold,(int)m_bSurface1Show,(int)m_bSurface2Show,nObjID);

	return strSQL;


}
CString Runway::GetDeleteScript(int nObjID) const
{
	CString strSQL = _T("");
	strSQL.Format(_T("DELETE FROM RUNWAYPROP WHERE OBJID = %d"),nObjID);

	return strSQL;
}

ALTObject * Runway::NewCopy()
{
	Runway * newRw = new Runway;
	
	newRw->CopyData(this);

	return newRw;
		
}

void Runway::GetIntersectTaxiway( const ALTObjectList& vTaxiway, ALTObjectList& reslt ) const
{
	for(int i = 0 ;i< (int)vTaxiway.size();++i){

		Taxiway * pTaxi = (Taxiway*)vTaxiway.at(i).get();
		if(pTaxi){
			if( m_path.intersects( &pTaxi->getPath() ) ){
				reslt.push_back(pTaxi);
			}
		}
	}
}

bool Runway::Intersect( const Taxiway& taixway ) const
{
	return m_path.intersects(&taixway.GetPath());
}



//void Runway::UpdateHoldPositions( ALTObjectList& objList )
//{
//	HoldPositionList newHoldList;
//	for(int i=0 ;i< (int)objList.size();i++)
//	{
//		ALTObject * pObj = objList.at(i).get();
//		if(pObj->getID() == getID()) continue;
//
//		if(pObj->GetType() == ALT_TAXIWAY){
//			CalculateHoldPosition((Taxiway*)pObj,newHoldList);
//		}
//		if(pObj->GetType() == ALT_RUNWAY){
//			CalculateHoldPosition((Runway*)pObj,newHoldList);
//		}		
//	}
//	for(int i=0;i<(int)newHoldList.size();i++)
//	{
//		const HoldPosition& newHold = newHoldList[i];
//		int idx = GetHoldPositionsIndex( newHold.m_intersectNode , newHold.IsFirst() );
//		if(idx < 0 ){
//			m_vAllHoldPositions.push_back(newHold);		
//		}else{
//			HoldPosition& hold = m_vAllHoldPositions[idx];
//			hold.m_intersectNode = newHold.m_intersectNode;
//			hold.m_intersectPos = newHold.m_intersectPos;
//		}
//	}
//
//
//	HoldPositionList::iterator newEnd = std::remove_if( m_vAllHoldPositions.begin(),m_vAllHoldPositions.end(), UnExsitHoldPredicate(newHoldList) );
//	m_vAllHoldPositions.erase(newEnd,m_vAllHoldPositions.end());	
//
//
//	IntersectNodeList vReslt;
//	HoldPositionFunc::CalculateIntersectionNodes(m_vAllHoldPositions,vReslt);
//	UpdateIntersectionList(vReslt);
//	UpdateHodePositionLists(vReslt);
//}
//
//
//void Runway::CalculateHoldPosition( Runway* pRunway, HoldPositionList& list )
//{
//	int rwId = pRunway->getID();
//	ASSERT(pRunway->getPath().getCount()>1);	
//
//	HoldPositionInfoList vIntersections; // center intersect, side intersect
//	GetHoldPositionsInfo(m_path,GetWidth(),pRunway->GetPath(),pRunway->GetWidth(),vIntersections,true);
//
//	for( int i=0 ;i < (int)vIntersections.size() ; i++ )
//	{
//		HoldPositionInfo holdinfo  = vIntersections.at(i);
//		IntersectNode node(pRunway, holdinfo.intersectidx, holdinfo.first);
//		HoldPosition newHold(node, holdinfo.second);		
//		list.push_back(newHold);			
//	}
//}
//void Runway::CalculateHoldPosition( Taxiway* pTaxiway, HoldPositionList& list )
//{
//	int taxiId = pTaxiway->getID();
//	ASSERT(pTaxiway->getPath().getCount()>1);	
//
//	HoldPositionInfoList vIntersections; // center intersect, side intersect
//	GetHoldPositionsInfo(m_path,GetWidth(),pTaxiway->GetPath(),pTaxiway->GetWidth(),vIntersections,false);
//
//	for( int i=0 ;i < (int)vIntersections.size() ; i++ )
//	{
//		HoldPositionInfo holdinfo  = vIntersections.at(i);
//		IntersectNode node(pTaxiway, holdinfo.intersectidx, holdinfo.first);
//		HoldPosition newHold(node, holdinfo.second);		
//		list.push_back(newHold);			
//	}
//}
//
//
// read form database 
//IntersectNodeList Runway::GetIntersectNodes() const
//{	
//	IntersectNodeList vIntersectNode;
//	IntersectNode::ReadIntersecNodeList(m_nObjID,vIntersectNode);
//	return vIntersectNode;
//}

//void Runway::UpdateIntersectionList(IntersectNodeList& lstIntersecNodeReturn) const
//{
//
//	HoldPositionFunc::UpdateIntersectionList(m_nObjID,lstIntersecNodeReturn);
//	return;
//}
//
//void Runway::UpdateHodePositionLists(IntersectNodeList& lstIntersecNode)const
//{
//	std::vector<HoldPosition> vDBHoldPosition;
//	HoldPosition::ReadHoldPositionList(m_nObjID,vDBHoldPosition);
//
//	UpdateHodePositionList(m_vAllHoldPositions,vDBHoldPosition,lstIntersecNode);
//
//	for (int i =0; i < static_cast<int>(vDBHoldPosition.size()); ++i)
//	{
//		vDBHoldPosition[i].DeleteData();
//	}
//}
//void Runway::UpdateHodePositionList(HoldPositionList& holdPositionList,HoldPositionList& dbHoldPositionList,IntersectNodeList& lstIntersecNode)const
//{
//
//	for (std::vector <HoldPosition *>::size_type i = 0; i < holdPositionList.size(); ++i)
//	{
//		bool bNewHoldPos = true;
//		for (HoldPositionList::size_type j = 0; j < dbHoldPositionList.size(); ++j )
//		{	
//		
//			int nIntNode = 0;
//			int nIntNodeCount =static_cast<int>(lstIntersecNode.size());
//			for (; nIntNode < nIntNodeCount;++nIntNode)
//			{
//				if(lstIntersecNode[nIntNode].m_nUniqueID == dbHoldPositionList[j].m_intersectNode.m_nUniqueID)
//					break;
//
//			}
//
//			if(nIntNode != nIntNodeCount)
//			{
//				if(holdPositionList[i].m_intersectNode.GetOtherObj()->getID() == lstIntersecNode[nIntNode].GetOtherObj()->getID() &&
//					holdPositionList[i].m_intersectNode.m_idx == lstIntersecNode[nIntNode].m_idx)
//				{
//					holdPositionList[i].m_intersectNode.m_nUniqueID = lstIntersecNode[nIntNode].m_nUniqueID;
//					holdPositionList[i].m_nUniqueID = dbHoldPositionList[j].m_nUniqueID;
//					dbHoldPositionList.erase(dbHoldPositionList.begin() + j);	
//					bNewHoldPos = false;	
//					break;
//				}
//			}
//
//
//		}
//
//		if (bNewHoldPos)
//		{
//			int nIntNode = 0;
//			int nIntNodeCount =static_cast<int>(lstIntersecNode.size());
//			for (; nIntNode < nIntNodeCount;++nIntNode)
//			{
//				if(holdPositionList[i].m_intersectNode.GetOtherObj()->getID() == lstIntersecNode[nIntNode].GetOtherObj()->getID()  &&
//					holdPositionList[i].m_intersectNode.m_idx == lstIntersecNode[nIntNode].m_idx)
//				{
//					holdPositionList[i].m_intersectNode.m_nUniqueID = lstIntersecNode[nIntNode].m_nUniqueID;
//					break;
//				}
//
//			}
//			if (nIntNode != nIntNodeCount)
//			{
//				holdPositionList[i].SaveData(m_nObjID);
//			}
//
//		}
//	}
//
//}
//
//
//
void Runway::ExportRunways(int nAirportID,CAirsideExportFile& exportFile)
{	
	//exportFile.getFile().writeField(_T("Runway ID, Name, CPath2008, dWidth, Marking1, Marking2"));
	//exportFile.getFile().writeLine();

	std::vector<int> vRunwayID;
	ALTAirport::GetRunwaysIDs(nAirportID,vRunwayID);
	int nRunwayCount = static_cast<int>(vRunwayID.size());
	////write info;
	//exportFile.getFile().writeField(_T("Airport"));
	//exportFile.getFile().writeInt(*iterApt);
	//exportFile.getFile().writeField("RunwayCount");
	//exportFile.getFile().writeInt(nRunwayCount);
	//exportFile.getFile().writeLine();

	//export runway
	for (int i =0; i < nRunwayCount;++i)
	{
		Runway runway;
		runway.ReadObject(vRunwayID[i]);
		runway.ExportRunway(exportFile);
	}
	
}
void Runway::ExportRunway(CAirsideExportFile& exportFile)
{
	ALTObject::ExportObject(exportFile);
	//apt id
	exportFile.getFile().writeInt(m_nAptID);
	//path
	int nPathCount = m_path.getCount();	
	exportFile.getFile().writeInt(nPathCount);
	for (int i =0; i < nPathCount; ++i)
	{
		exportFile.getFile().writePoint2008(m_path.getPoint(i));
	}

	exportFile.getFile().writeDouble(m_dWidth);
	exportFile.getFile().writeField(m_sMarking1.c_str());
	exportFile.getFile().writeField(m_sMarking2.c_str());

	exportFile.getFile().writeDouble(m_dMark1LandingThreshold);
	exportFile.getFile().writeDouble(m_dMark1TakeOffThreshold);
	exportFile.getFile().writeDouble(m_dMark2LandingThreshold);
	exportFile.getFile().writeDouble(m_dMark2TakeOffThreshold);




	exportFile.getFile().writeLine();


	RunwayDisplayProp dspProp;
	dspProp.ReadData(m_nObjID);
	dspProp.ExportDspProp(exportFile);

}

void Runway::ImportObject(CAirsideImportFile& importFile)
{
	ALTObject::ImportObject(importFile);
	int nOldAirportID = -1;
	importFile.getFile().getInteger(nOldAirportID);
	m_nAptID = importFile.GetAirportNewIndex(nOldAirportID);
	
	//path
	//m_path.a

	int nPointCount = 0;
	importFile.getFile().getInteger(nPointCount);
	m_path.init(nPointCount);
	CPoint2008 *pPoint =  m_path.getPointList();

	for (int i = 0; i < nPointCount; ++i)
	{
		importFile.getFile().getPoint2008(pPoint[i]);
	}

	importFile.getFile().getFloat(m_dWidth);
	char chMark[512];
	memset(chMark,0,512);
	importFile.getFile().getField(chMark,512);

	m_sMarking1 = chMark;
	memset(chMark,0,512);
	
	importFile.getFile().getField(chMark,512);
	m_sMarking2 = chMark;

	if (importFile.getVersion() > 1030 )
	{
		importFile.getFile().getFloat(m_dMark1LandingThreshold );
		importFile.getFile().getFloat(m_dMark1TakeOffThreshold);
		importFile.getFile().getFloat(m_dMark2LandingThreshold);
		importFile.getFile().getFloat(m_dMark2TakeOffThreshold);
	}

	int nOldObjectID = m_nObjID;
	int nNewObjID = SaveObject(m_nAptID);
	importFile.AddObjectIndexMap(nOldObjectID,nNewObjID);

	importFile.getFile().getLine();


	RunwayDisplayProp dspProp;
	dspProp.ImportDspProp(importFile,m_nObjID);
}

const CPath2008& Runway::GetPath() const
{
	return m_path;
}


CPath2008& Runway::getPath()
{
	return m_path;
}

//bool Runway::GetHoldPositionLine( int idx, Line& line ) const
//{
//	return HoldPositionFunc::CaculateHoldPositionLine(m_path,m_dWidth,m_vAllHoldPositions,idx,line);
//}

int Runway::GetExitList( RUNWAY_MARK rwMark, RunwayExitList& exitlist ) const
{	
	RunwayExit::ReadExitsList(getID(),rwMark,exitlist);
	return (int)exitlist.size();	
}

void SetRunwayExitContent(RunwayExit& rwExit,Runway* pRunway, RUNWAY_MARK rwmark, Taxiway* pTaxiway,const ARCVector3& exitVec  )
{
	CString strRwMark = (rwmark==RUNWAYMARK_FIRST)?pRunway->GetMarking1().c_str() : pRunway->GetMarking2().c_str();
	rwExit.SetRunwayMark( rwmark );

	double dAngle = pRunway->GetAngle(rwmark, exitVec);
	RunwayExit::ExitSide side = dAngle>=0?RunwayExit::EXIT_LEFT:RunwayExit::EXIT_RIGHT;
	rwExit.SetExitSide(side);
	rwExit.SetAngle( abs(dAngle) );
	CString exitName;
	exitName.Format("%s & %s (%s)", strRwMark.GetString(), pTaxiway->GetMarking().c_str() , (rwExit.GetSideType() == RunwayExit::EXIT_LEFT)?"LEFT":"RIGHT" );
	rwExit.SetName(exitName);	
}



void Runway::UpdateRunwayExits( const IntersectionNodesInAirport& vNodes )
{	
	DistanceUnit rwHalfWidth = GetWidth()*0.5;
	RunwayExitList newExits;
	//save to database	
	std::vector<IntersectionNode> nodelist;
	vNodes.GetIntersectionNodeOf(getID(),nodelist);
	for(int i=0;i< (int)nodelist.size();i++)
	{
		IntersectionNode& theNode = nodelist[i];
		//std::vector<TaxiwayIntersectItem*> vTaxiItemList = theNode.GetTaxiwayIntersectItemList();
		TRACE("Node Index:%d", i);

		std::vector<IntersectItem *> vIntersectItem = theNode.GetItemList();
		for(int iItemIdx = 0; iItemIdx <(int)vIntersectItem.size();iItemIdx++)
		{
			IntersectItem* intersectItem = vIntersectItem[iItemIdx];
			if (intersectItem->GetType() == IntersectItem::TAXIWAY_INTERSECT)
			{
				TaxiwayIntersectItem* pTaxiwayItem = (TaxiwayIntersectItem *)intersectItem;
				if(pTaxiwayItem == NULL)
					continue;
				Taxiway* pTaxiway = pTaxiwayItem->GetTaxiway();
				if(pTaxiway)	
				{
					DistanceUnit taxiLen = pTaxiway->GetPath().GetTotalLength();
					DistanceUnit nodeDistInTaxiway = pTaxiwayItem->GetDistInItem();
					//ASSERT(nodeDistInTaxiway<=taxiLen);
					//ASSERT(nodeDistInTaxiway>=0);

					if( nodeDistInTaxiway > rwHalfWidth )  //
					{
						RunwayExit rwExit;
						rwExit.SetRunwayID(getID());
						rwExit.SetIntersectRunwayOrTaxiwayID(pTaxiway->getID());
						rwExit.SetIntersectNode(theNode);	

						//CPoint2008 exitPos = pTaxiway->GetPath().GetDistPoint( );
						ARCVector3 exitVec =  -pTaxiway->GetPath().GetDistDir(nodeDistInTaxiway - rwHalfWidth);//exitPos - pTaxiway->GetPath().GetDistPoint(nodeDistInTaxiway);						

						SetRunwayExitContent(rwExit, this, RUNWAYMARK_FIRST, pTaxiway, exitVec);
						newExits.push_back(rwExit);

						SetRunwayExitContent(rwExit, this, RUNWAYMARK_SECOND, pTaxiway, exitVec);
						newExits.push_back(rwExit);							
					}
					if( nodeDistInTaxiway < taxiLen - rwHalfWidth)
					{
						RunwayExit rwExit;
						rwExit.SetRunwayID(getID());
						rwExit.SetIntersectRunwayOrTaxiwayID(pTaxiway->getID());
						rwExit.SetIntersectNode(theNode);	
						//CPoint2008 exitPos = pTaxiway->GetPath().GetDistPoint(nodeDistInTaxiway + rwHalfWidth);
						ARCVector3 exitVec =  pTaxiway->GetPath().GetDistDir(nodeDistInTaxiway + rwHalfWidth);//exitPos-pTaxiway->GetPath().GetDistPoint(nodeDistInTaxiway);
						//first runway
						SetRunwayExitContent(rwExit, this, RUNWAYMARK_FIRST, pTaxiway, exitVec);
						newExits.push_back(rwExit);

						SetRunwayExitContent(rwExit, this, RUNWAYMARK_SECOND, pTaxiway, exitVec);
						newExits.push_back(rwExit);		
					}
				}	
			}
			else if(intersectItem->GetType() == IntersectItem::RUNWAY_INTESECT)	//calculate runway exits of cross runway
			{
				//get runway items
				RunwayIntersectItem* pRunwayItem = (RunwayIntersectItem *)intersectItem;
				if(pRunwayItem == NULL)
					continue;
				Runway* pRunway = pRunwayItem->GetRunway();
				if(pRunway == NULL)
					continue;
				if(pRunway->getID() == getID())//if the intersect runway item is itself, continue cycle
					continue;


				DistanceUnit taxiLen = pRunway->GetPath().GetTotalLength();
				DistanceUnit nodeDistInTaxiway = pRunwayItem->GetDistInItem();
				if( nodeDistInTaxiway > 100 )  //
				{
					RunwayExit rwExit;
					rwExit.SetRunwayID(getID());
					rwExit.SetIntersectRunwayOrTaxiwayID(pRunway->getID());
					rwExit.SetIntersectNode(theNode);	
					CPoint2008 exitPos = pRunway->GetPath().GetDistPoint(nodeDistInTaxiway -100);
					ARCVector3 exitVec =  exitPos - pRunway->GetPath().GetDistPoint(nodeDistInTaxiway);
					//first runway
					{
						rwExit.SetRunwayMark( RUNWAYMARK_FIRST );
						double dAngle = GetAngle(RUNWAYMARK_FIRST, exitVec);
						RunwayExit::ExitSide side = dAngle>=0?RunwayExit::EXIT_LEFT:RunwayExit::EXIT_RIGHT;
						rwExit.SetExitSide(side);
						rwExit.SetAngle( abs(dAngle) );	
						
						CString exitName;
						exitName.Format("%s & %s (%s)", GetMarking1().c_str(), pRunway->GetMarking2().c_str() , (rwExit.GetSideType() == RunwayExit::EXIT_LEFT)?"LEFT":"RIGHT" );
						rwExit.SetName(exitName);
						newExits.push_back(rwExit);
					}
					//second runway
					{
						rwExit.SetRunwayMark(RUNWAYMARK_SECOND);
						double dAngle = GetAngle(RUNWAYMARK_SECOND, exitVec);
						RunwayExit::ExitSide side = dAngle>=0?RunwayExit::EXIT_LEFT:RunwayExit::EXIT_RIGHT;
						rwExit.SetExitSide(side);
						rwExit.SetAngle( abs(dAngle) );

						CString exitName;
						exitName.Format("%s & %s (%s)", GetMarking2().c_str(), pRunway->GetMarking2().c_str() , (rwExit.GetSideType() == RunwayExit::EXIT_LEFT)?"LEFT":"RIGHT" );
						rwExit.SetName(exitName);
						newExits.push_back(rwExit);
					}
				}
				if( nodeDistInTaxiway < taxiLen - 100)
				{
					RunwayExit rwExit;
					rwExit.SetRunwayID(getID());
					rwExit.SetIntersectRunwayOrTaxiwayID(pRunway->getID());
					rwExit.SetIntersectNode(theNode);	
					CPoint2008 exitPos = pRunway->GetPath().GetDistPoint(nodeDistInTaxiway + 100);
					ARCVector3 exitVec =  exitPos-pRunway->GetPath().GetDistPoint(nodeDistInTaxiway);
					
					{//first runway
						rwExit.SetRunwayMark( RUNWAYMARK_FIRST);
						double dAngle = GetAngle(RUNWAYMARK_FIRST, exitVec);
						RunwayExit::ExitSide side = dAngle>=0?RunwayExit::EXIT_LEFT:RunwayExit::EXIT_RIGHT;
						rwExit.SetExitSide(side);
						rwExit.SetAngle( abs(dAngle) );

						CString exitName;
						exitName.Format("%s & %s (%s)", GetMarking1().c_str(), pRunway->GetMarking1().c_str() , (rwExit.GetSideType() == RunwayExit::EXIT_LEFT)?"LEFT":"RIGHT" );
						rwExit.SetName(exitName);				
						newExits.push_back(rwExit);
					}
					
					{//second runway
						rwExit.SetRunwayMark( RUNWAYMARK_SECOND );
						double dAngle = GetAngle(RUNWAYMARK_SECOND, exitVec);
						RunwayExit::ExitSide side = dAngle>=0?RunwayExit::EXIT_LEFT:RunwayExit::EXIT_RIGHT;
						rwExit.SetExitSide(side);
						rwExit.SetAngle( abs(dAngle) );

						CString exitName;
						exitName.Format("%s & %s (%s)", GetMarking2().c_str(), pRunway->GetMarking1().c_str() , (rwExit.GetSideType() == RunwayExit::EXIT_LEFT)?"LEFT":"RIGHT" );
						rwExit.SetName( exitName );
						newExits.push_back(rwExit);
					}
				}
			}
		}
	}


	




	//update database
	RunwayExitList DbExits;
	RunwayExit::ReadExitsList(m_nObjID,DbExits);

	RunwayExitList::iterator iterClac = newExits.begin();
	for (;iterClac!= newExits.end(); ++iterClac)
	{
		RunwayExitList::iterator iterDB = DbExits.begin();
		for (;iterDB != DbExits.end(); ++iterDB)
		{
			if( iterClac->IsIdent(*iterDB) )
			{
				iterClac->SetID( iterDB->GetID() );
				DbExits.erase(iterDB);
				break;
			}
		}
		(*iterClac).SaveData(m_nObjID);
	}

	RunwayExitList::iterator iterDB = DbExits.begin();
	for (;iterDB != DbExits.end(); ++iterDB)
	{
		(*iterDB).DeleteData();
	}

	//m_vExits = newExits;
}


//std::vector<Runway*> Runway::GetIntersectRunways()
//{
//	std::vector<Runway*> reslt;	
//	IntersectionNodeList vIntersectNodes;
//	IntersectionNode::ReadIntersecNodeList(getID(),vIntersectNodes);
//	for(int i=0;i< (int)vIntersectNodes.size();i++)
//	{
//		IntersectionNode& theNode = vIntersectNodes[i];
//		std::vector<RunwayIntersectItem*> vRwItems = theNode.GetRunwayIntersectItemList();
//		for(int irwIdx=0; irwIdx<(int)vRwItems.size(); irwIdx++ )
//		{
//			RunwayIntersectItem* rwItem = vRwItems[irwIdx];
//			Runway * pRunway = rwItem->GetRunway();
//			if(pRunway && pRunway->getID()!= getID() )
//			{
//				if( reslt.end() == std::find(reslt.begin(),reslt.end(),pRunway) )
//					reslt.push_back( pRunway );
//			}
//		}
//
//	}
//	return reslt;
//}

std::vector<int> Runway::GetIntersectRunways() const
{
	std::set<int> vRetIds;
	IntersectionNodeList vIntersectNodes;
	IntersectionNode::ReadIntersecNodeList(getID(),vIntersectNodes);
	for(int i=0;i< (int)vIntersectNodes.size();i++)
	{
		IntersectionNode& theNode = vIntersectNodes[i];
		std::vector<RunwayIntersectItem*> vRwItems = theNode.GetRunwayIntersectItemList();
		for(int irwIdx=0; irwIdx<(int)vRwItems.size(); irwIdx++ )
		{
			RunwayIntersectItem* rwItem = vRwItems[irwIdx];
			vRetIds.insert(rwItem->GetObjectID());			
		}

	}
	return std::vector<int>(vRetIds.begin(),vRetIds.end());
}
bool Runway::IsPointLeft(RUNWAY_MARK rwMark, const CPoint2008& pt)const
{
	ARCVector3 vRwDir;
	ARCVector3 vPtDir;
	if(rwMark == RUNWAYMARK_FIRST)
	{
		vRwDir = GetPath().getPoint(1) - GetPath().getPoint(0);
		vPtDir = pt - GetPath().getPoint(0);
	}
	else
	{
		vRwDir = GetPath().getPoint(0) - GetPath().getPoint(1);
		vPtDir = pt - GetPath().getPoint(1);
	}

	ARCVector3 vX = vRwDir ^ vPtDir;
	return vX[VZ] > 0;

}

double Runway::GetAngle( RUNWAY_MARK rwMark, const ARCVector3& vdir ) const
{
	ARCVector3 vrunway3 = GetDir(rwMark);
	ARCVector2 vrunway2(vrunway3[VX],vrunway3[VY]);
	return (vrunway2.GetAngleOfTwoVector( ARCVector2(vdir[VX],vdir[VY]) ));	
}

ARCVector3 Runway::GetDir( RUNWAY_MARK rwMark ) const
{
	ASSERT(m_path.getCount()>1);
	if(rwMark == RUNWAYMARK_FIRST )
		return m_path.getPoint(1) - m_path.getPoint(0);
	else
		return m_path.getPoint(0) - m_path.getPoint(1);
}

EnumCardinalDirection Runway::GetPointCadDir(const ALTAirport&aptInfo,  const CPoint2008& pt ) const
{
	ARCVector3 vnorth (0,1,0);
	double dangle =  GetAngle(RUNWAYMARK_FIRST, vnorth);
	bool bPointLeft = IsPointLeft(RUNWAYMARK_FIRST, pt);
	dangle -= aptInfo.GetMagnectVariation().GetRealVariation();	
	if(dangle <= 0 )dangle += 360;

	if(dangle>=45 && dangle<135)
	{
		if(bPointLeft)
			return NORTH;
		else 
			return SOUTH;
	}
	else if(dangle >=135 && dangle < 225)
	{
		if(bPointLeft)
			return EAST;
		else 
			return WEST;
	}
	else if(dangle >=225 && dangle < 315)
	{
		if(bPointLeft)
			return SOUTH;
		else 
			return NORTH;
	}
	else
	{
		if(bPointLeft)
			return WEST;
		else 
			return EAST;
	}
	ASSERT(FALSE);
	return NORTH;
}



IntersectionNodeList Runway::GetIntersectNodes() const
{
	IntersectionNodeList reslt;
	IntersectionNode::ReadIntersecNodeList(getID(), reslt);
	return reslt;
}

bool Runway::CopyData(const ALTObject* pObj )
{
	if(this == pObj)
		return true;

	if(pObj->GetType() == GetType() )
	{
		Runway * pOtherRunway = (Runway*)pObj;
		m_path = pOtherRunway->GetPath();
		m_dWidth = pOtherRunway->GetWidth();
		m_sMarking1 = pOtherRunway->GetMarking1();
		m_sMarking2 = pOtherRunway->GetMarking2();
		m_dMark1LandingThreshold = pOtherRunway->GetMark1LandingThreshold();
		m_dMark1TakeOffThreshold = pOtherRunway->GetMark1TakeOffThreshold();
		m_dMark2LandingThreshold = pOtherRunway->GetMark2LandingThreshold();
		m_dMark2TakeOffThreshold = pOtherRunway->GetMark2TakeOffThreshold();
		m_ObjSurface1 = pOtherRunway->getMark1RunwayStruct();
		m_ObjSurface2 = pOtherRunway->getMark2RunwayStruct();
		m_bSurface1Show = pOtherRunway->GetMark1Show();
		m_bSurface2Show = pOtherRunway->GetMark2Show();		
	}
	return __super::CopyData(pObj);
}
///////////////////////////////////////////////////////////////////////////////////////////////
ObjRuwayStruct& Runway::GetObstruction( RUNWAY_MARK mark )
{
	if(mark == RUNWAYMARK_FIRST)
		return m_ObjSurface1;
	else 
		return m_ObjSurface2;
}

void Runway::ReadRecord( CADORecordset& adoRecordset )
{
	ALTObject::ReadObject(adoRecordset);

	adoRecordset.GetFieldValue(_T("PATHID"),m_nPathID);
	int nPointCount = -1;
	adoRecordset.GetFieldValue(_T("POINTCOUNT"),nPointCount);
	adoRecordset.GetFieldValue(_T("PATH"),nPointCount,m_path);

	int bShow = 0;
	adoRecordset.GetFieldValue(_T("SHOW1"),bShow);
	m_bSurface1Show = (BOOL)bShow;

	bShow = 0;
	adoRecordset.GetFieldValue(_T("SHOW2"),bShow);
	m_bSurface2Show = (BOOL)bShow;

	adoRecordset.GetFieldValue(_T("WIDTH"),m_dWidth);
	adoRecordset.GetFieldValue(_T("MARKING1"),m_sMarking1);

	adoRecordset.GetFieldValue(_T("MARKING2"),m_sMarking2);

	adoRecordset.GetFieldValue(_T("MARK1LTHRESHOLD"),m_dMark1LandingThreshold);
	adoRecordset.GetFieldValue(_T("MARK1TTHRESHOLD"),m_dMark1TakeOffThreshold);
	adoRecordset.GetFieldValue(_T("MARK2LTHRESHOLD"),m_dMark2LandingThreshold);
	adoRecordset.GetFieldValue(_T("MARK2TTHRESHOLD"),m_dMark2TakeOffThreshold);



}


CPoint2008 Runway::GetLandThreshold1Point() const
{
	return m_path.GetDistPoint(m_dMark1LandingThreshold);
}

CPoint2008 Runway::GetLandThreshold2Point() const
{
	return m_path.GetDistPoint(m_path.GetTotalLength() - m_dMark2LandingThreshold);
}

const GUID& Runway::getTypeGUID()
{
	// {2F34926B-A785-4828-AD01-E91CA314782D}
	static const GUID name = 
	{ 0x2f34926b, 0xa785, 0x4828, { 0xad, 0x1, 0xe9, 0x1c, 0xa3, 0x14, 0x78, 0x2d } };
	return name;

}

ALTObjectDisplayProp* Runway::NewDisplayProp()
{
	return new RunwayDisplayProp();
}

int ObjSurface::SaveData(int nObjID,BOOL bFirst,ALTOBJECT_TYPE emALTType,SurfaceType emSurface)
{
	CString strSQL = _T("");
	CString strData = _T("");
	if(vSurface.size() == 0)
	{
		SurfaceData* pData = new SurfaceData;
		vSurface.clear();
		vSurface.push_back(pData);
	}
	strData = FormatSurface(vSurface);
	strSQL.Format(_T("INSERT INTO OBJSURFACE(OBJID,[FIRST],ALTTYPE,SURFACETYPE,SURFACE,OFFSET) VALUES(%d,%d,%d,%d,'%s',%f)"),nObjID,(int)bFirst,(int)emALTType,(int)emSurface,strData,dOffset);
	//strSQL.Format(_T("INSERT INTO OBJSURFACE(OBJID,ALTTYPE,SURFACETYPE,SURFACE,OFFSET) VALUES(%d,%d,%d,'%s',%f)"),nObjID,(int)emALTType,(int)emSurface,strData,dOffset);

	int nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
	return nID;
}

void ObjSurface::ReadData(int nObjID,BOOL bFirst,ALTOBJECT_TYPE emALTType,SurfaceType emSurface)
{
	CString strSQL = _T("");
	strSQL.Format(_T("SELECT ID,OBJID,ALTTYPE,FIRST,SURFACETYPE,SURFACE,OFFSET FROM OBJSURFACE WHERE \
					 (OBJID= %d AND FIRST = %d AND ALTTYPE = %d AND SURFACETYPE = %d)"),nObjID,(int)bFirst,\
					 (int)emALTType,(int)emSurface);
	long nRecordCount = 0;
	CADORecordset adoRecordset;
	CADODatabase::ExecuteSQLStatement(strSQL,nRecordCount,adoRecordset);
	if (!adoRecordset.IsEOF())
	{
		adoRecordset.GetFieldValue(_T("ID"),nID);
		CString strData = _T("");
		adoRecordset.GetFieldValue(_T("SURFACE"),strData);
		ConveyToSurface(strData);

		adoRecordset.GetFieldValue(_T("OFFSET"),dOffset);
	}
	else
	{
		SaveData(nObjID,bFirst,emALTType,emSurface);
	}
}

void ObjSurface::UpdateData()
{
	CString strSQL = _T("");
	CString strData = _T("");
	strData = FormatSurface(vSurface);
	strSQL.Format(_T("UPDATE OBJSURFACE SET SURFACE = '%s',OFFSET = %.2f WHERE ID = %d"),strData,dOffset,nID);
	CADODatabase::ExecuteSQLStatement(strSQL);
}

void ObjSurface::DeleteData()
{
	CString strSQL = _T("");
	strSQL.Format(_T("DELETE FROM OBJSURFACE WHERE ID = %d"),nID);
	CADODatabase::ExecuteSQLStatement(strSQL);
}

void ObjSurface::ConveyToSurface(CString& strSurface)
{
	SurfaceData* pData = NULL;
	int nPos = strSurface.Find(',');
	int nDes = 0;
	int nID = 0;
	while (nPos != -1)
	{
		if (nDes % 2 == 0)
		{
			pData = new SurfaceData;
			pData->nAngle = atoi(strSurface.Left(nPos));
		}
		else
		{	pData->nID = nID++;
			pData->dDistance = atof(strSurface.Left(nPos));
			vSurface.push_back(pData);
		}
		strSurface = strSurface.Right(strSurface.GetLength()-nPos-1);
		nPos = strSurface.Find(',');
		nDes++;
	}
	// last item
	pData->nID = nID;
	pData->dDistance = atof(strSurface);
	vSurface.push_back(pData);
}

CString ObjSurface::FormatSurface(std::vector<SurfaceData*>&vData)
{	
	CString strData = _T("");
	if (vData.size()>0)
	{
		SurfaceData* pData = NULL;
		pData = vData.at(0);
		strData.Format(_T("%d,%.2f"),pData->nAngle,pData->dDistance);
		for (int i = 1; i < (int)vData.size(); i++)
		{
			CString strTemp = _T("");
			pData = vData.at(i);
			strTemp.Format(_T("%d,%.2f"),pData->nAngle,pData->dDistance);
			strData += "," + strTemp;
		}
	}
	return strData;
}

SurfaceData* ObjSurface::findItem(int nSurfaceID)
{
	std::vector<SurfaceData*>::iterator iter = vSurface.begin();
	for (; iter != vSurface.end(); ++iter)
	{
		if (nSurfaceID == (*iter)->nID)
		{
			return (*iter);
		}
	}
	return NULL;
}
