#include "StdAfx.h"
#include ".\landsideintersectioninsim.h"
#include "LandsideResourceManager.h"

#include "LandsideVehicleInSim.h"
#include "CrossWalkInSim.h"
#include "ARCportEngine.h"
#include "LandsideSimulation.h"

void LandsideIntersectionInSim::InitRelateWithOtherObject( LandsideResourceManager* allRes )
{
	LandsideIntersectionNode* mpNode = (LandsideIntersectionNode*)getInput();
	//double dLaneWidth = 350;
	for(int i=0;i<mpNode->getLinageCount();i++)
	{
		LaneLinkage& link = mpNode->getLinkage(i);
		LandsideLaneNodeInSim* pLaneEntry = allRes->GetlaneNode(link.mLaneEntry);
		LandsideLaneNodeInSim* pLaneExit = allRes->GetlaneNode(link.mLaneExit);

		if(pLaneEntry && pLaneExit )
		{
			LandsideLaneEntry* pEntry =pLaneEntry->toEntry();
			LandsideLaneExit* pExit = pLaneExit->toExit();
			CPath2008 path;
			if(pExit && pEntry && link.GetPath(path) )
			{
				LandsideIntersectLaneLinkInSim* pnewLink = new LandsideIntersectLaneLinkInSim(this,pExit,pEntry,link.getGroupID());
				pEntry->SetFromRes(this);
				pExit->SetToRes(this);		
				pnewLink->SetPath(path);
				m_vLinkages.push_back(pnewLink);

				//dLaneWidth = MAX(pEntry->mpLane->getWidth(),dLaneWidth);
			}
			//need to get link path
		}
	}

	/*for(size_t i=0;i<m_vLinkages.size();i++)
	{
		m_vLinkages[i]->InitConflict(dLaneWidth);
	}*/
}

LandsideIntersectionInSim::LandsideIntersectionInSim( LandsideIntersectionNode* pNode )
:LandsideLayoutObjectInSim(pNode)
{
	//mpNode = pNode;	
	m_bKilled = false;
}

//const ARCPath3* LandsideIntersectionInSim::GetMovePath( LandsideLaneExit* pExit,LandsideLaneEntry* pEntry )const
//{
//	if(LandsideIntersectLaneLinkInSim* pLink = GetLinkage(pExit,pEntry) )
//	{
//		return &pLink->GetPath();
//	}
//	return NULL;
//}


#include "Common/DistanceLineLine.h"
//LandsideVehicleInSim* LandsideIntersectionInSim::GetAheadVehicle(LandsideVehicleInSim* mpVehicle, const CPath2008& pathInode, const CPoint2008& curPos, 
//																 double& dDistToAhead )const
//{
//	LandsideVehicleInSim* pAheadVehicle = NULL;
//	double mMidDist = 0;
//	double conflictDist = mpVehicle->GetLength()*2; 
//	int nVehicleIndex = GetVehicleIndex(mpVehicle);
//
//	for(int i=0;i<GetInResVehicleCount();i++)
//	{
//		LandsideVehicleInSim* pVehicle = GetInResVehicle(i);		
//		if(pVehicle==mpVehicle)
//			break;		
//
//		//get vehicle state
//		//double dCurDistInPath = pathInode.GetPointDist(curPos);		
//		
//		MobileState& dstate = pVehicle->getState();		
//		{				
//			//DistancePointPath3D distPoint(midState.pos,pathInode);
//			//if( distPoint.GetSquared()  < conflictDist*conflictDist ) //in conflict area
//			{				
//				//int otherIndex = GetVehicleIndex(pVehicle);
//				double distToOther = curPos.distance3D(dstate.pos);
//
//				//bool bIsHead = otherIndex < nVehicleIndex;
//				//if(bIsHead){
//					if(!pAheadVehicle) //init ahead
//					{
//						mMidDist = distToOther;
//						pAheadVehicle = pVehicle;
//						
//					}
//					else if(distToOther < mMidDist) //find the nearest vehicle
//					{
//						mMidDist = distToOther;
//						pAheadVehicle = pVehicle;
//						
//					}
//				//}		
//
//			}			
//		}
//	}
//
//	//clear not lane vehicles	
//	dDistToAhead = mMidDist;
//	return pAheadVehicle;
//}
//
LandsideIntersectLaneLinkInSim* LandsideIntersectionInSim::GetLinkage( LandsideLaneExit* pExit,LandsideLaneEntry* pEntry ) const
{
	for(int i=0;i<GetLinkageCount();i++)
	{
		LandsideIntersectLaneLinkInSim* pLink = GetLinkage(i);
		if(pLink->getToNode() == pEntry && pLink->getFromNode() == pExit)
		{//
			return pLink;
		}
	}
	return NULL;
}
typedef std::vector<LandsideIntersectLaneLinkInSim *> LinkList;
typedef std::map<int,LinkList> GroupMap;
void LandsideIntersectionInSim::GenerateCtrlEvents(CIntersectionTrafficControlIntersection *pTrafficCtrlNode,LandsideResourceManager* pResManager,OutputLandside* pOutput,ElapsedTime startTime,ElapsedTime endTime/*=0*/)
{
	//Generate traffic control events for every linkage group.
	//Maybe not all the group id in the default page of setting
	//dlg exist in intersection.
	LandsideIntersectionNode* mpNode = (LandsideIntersectionNode*)getInput();
	if(mpNode->getNodeType()==LandsideIntersectionNode::_Unsignalized)
		return;
	GroupMap m_mapGroup;

	for (int i=0;i<GetLinkageCount();i++)
	{
		LandsideIntersectLaneLinkInSim *pLinkInSim=GetLinkage(i);			
		int nGroupID=pLinkInSim->GetGroupID();
		m_mapGroup[nGroupID].push_back(pLinkInSim);
	}

	//calculate the ctrl time of link in same group
	int nCycleTime=0;//the time of one ctrl cycle
	GroupMap::iterator iter;
	for (iter=m_mapGroup.begin();iter!=m_mapGroup.end();iter++)
	{
		CString groupName = mpNode->GetGroupName(iter->first);
		TrafficCtrlTime ctrlTime=pTrafficCtrlNode->GetGroupCtrlTime(groupName);
		nCycleTime+=ctrlTime.activeTime;
		nCycleTime+=ctrlTime.bufferTime;
	}

	int nFirstCloseTime=0;
	for (int i=0;i<(int)m_vLinkGroups.size();i++)
	{
		delete m_vLinkGroups.at(i);
	}
	m_vLinkGroups.clear();

	for (int i=0;i<pTrafficCtrlNode->GetItemCount();i++)
	{
		CString nGroupNum=pTrafficCtrlNode->GetItem(i)->GetLinkGroupName();
		int groupID = mpNode->GetGroupID(nGroupNum);

		iter=m_mapGroup.find(groupID);
		if (iter!=m_mapGroup.end())
		{
			TrafficCtrlTime ctrlTime=pTrafficCtrlNode->GetGroupCtrlTime(nGroupNum);			
			ctrlTime.closeTime=nCycleTime-ctrlTime.activeTime-ctrlTime.bufferTime;
			LandsideIntersectLinkGroupInSim *pLinkGroupInSim=new LandsideIntersectLinkGroupInSim(this,iter->first);
			LandsideIntersectionNode *pIntersection=(LandsideIntersectionNode*)m_pInput;
			if (pResManager && pIntersection)
			{
				//add cross walk in sim
				for (int k=0;k<pResManager->m_vTrafficObjectList.getCount();k++)
				{
					if( CCrossWalkInSim* pCross =pResManager->m_vTrafficObjectList.getItem(k)->toCrossWalk())
					{
						if (pCross->GetLinkIntersectionID()==pIntersection->getID() && pCross->GetLinkGroup()==groupID /*nGroupNum*/)
						{
							pLinkGroupInSim->AddLinkCrossWalk(pCross);
							//pCross->InitLogEntry(pOutput);
						}
					}
				}				
			}
			pLinkGroupInSim->InitLogEntry(pOutput);
			pLinkGroupInSim->SetState(LS_ACTIVE);
			pLinkGroupInSim->SetCtrlTime(ctrlTime);
			pLinkGroupInSim->SetFirstCloseTime(nFirstCloseTime);
			pLinkGroupInSim->SetEndTime(endTime);
			pLinkGroupInSim->Activate(startTime+(long)pTrafficCtrlNode->GetOffsetSimTime());
			pLinkGroupInSim->WriteLog(startTime);
			m_vLinkGroups.push_back(pLinkGroupInSim);
			nFirstCloseTime+=ctrlTime.activeTime+ctrlTime.bufferTime;
		}
	}
}


CString LandsideIntersectionInSim::print() const
{
	return GetIntersectionName().GetIDString();
}

LandsideIntersectLinkGroupInSim* LandsideIntersectionInSim::GetLinkGroupById( int idx ) const
{
	for(int i=0;i<(int)m_vLinkGroups.size();i++)
	{
		LandsideIntersectLinkGroupInSim* pGroup = m_vLinkGroups[i];
		if(pGroup->GetGroupID() == idx)
			return pGroup;
	}
	return NULL;
}

void LandsideIntersectionInSim::Clear()
{
	for(size_t i=0;i<m_vLinkages.size();i++)
		delete m_vLinkages[i];
	m_vLinkages.clear();

	for(size_t i=0;i<m_vLinkGroups.size();i++)
		delete m_vLinkGroups[i];
	m_vLinkGroups.clear();
}

void LandsideIntersectionInSim::InitGraphLink( LandsideRouteGraph* pGraph ) const
{
	for( int i=0;i< GetLinkageCount();i++)
	{
		const LandsideIntersectLaneLinkInSim* linkage = GetLinkage(i);
		pGraph->AddEdge(linkage->getFromNode(),linkage->getToNode(),linkage->getDistance() );		
	}
}


LandsideLayoutObjectInSim* LandsideIntersectionInSim::getLayoutObject() const
{
	return const_cast<LandsideIntersectionInSim*>(this);
}



void LandsideIntersectionInSim::Kill( OutputLandside* pOutput, const ElapsedTime& t )
{
	if(m_bKilled)
		return ;
	//do kill
	//OutputLandside* pOutput = pEngine->GetLandsideSimulation()->getOutput();
	for (size_t k=0;k<m_vLinkGroups.size();k++)
	{
		LandsideIntersectLinkGroupInSim *linkGroupInSim=(LandsideIntersectLinkGroupInSim *)m_vLinkGroups.at(k);		
		linkGroupInSim->End(t);

		pOutput->m_SimLogs.GetLinkGroupLog().addItem(linkGroupInSim->m_logEntry);
	}
	m_bKilled = true;
}

void LandsideIntersectionInSim::Deactive()
{
	for (size_t k=0;k<m_vLinkGroups.size();k++)
	{
		LandsideIntersectLinkGroupInSim *linkGroupInSim=(LandsideIntersectLinkGroupInSim *)m_vLinkGroups.at(k);		
		linkGroupInSim->DeActive();	
	}
}


//LandsideVehicleInSim* LandsideIntersectLaneLinkInSim::GetAheadVehicle( LandsideVehicleInSim* pVehicle, LandsideIntersectLaneLinkInSim* pOtherLink, double distInLink, 
//																	  double& sep )const
//{
//	LandsideVehicleInSim* pConflictV = NULL;
//	double dDistToEnd = getDistance()-distInLink;
//
//	int IndexInNode = mpParent->GetVehicleIndex(pVehicle);
//
//	for(int i=0;i<pOtherLink->GetInResVehicleCount();i++)
//	{
//		LandsideVehicleInSim* pOtherVehicle = pOtherLink->GetInResVehicle(i);
//		if(pOtherVehicle==pVehicle)
//			continue;
//
//		int OtherIndexInNode  = mpParent->GetVehicleIndex(pOtherVehicle);
//		double dOtherVehicleToEnd = pOtherLink->getDistance()-pOtherVehicle->getState().distInRes;
//		//check whether the vehicle in the range of conflict
//		bool bConflict = OtherIndexInNode < IndexInNode;	//other come first
//		double dDistToHead = dDistToEnd - dOtherVehicleToEnd;
//		if(bConflict)
//		{	
//			if(!pConflictV ||  dDistToHead< sep)
//			{
//				pConflictV = pOtherVehicle;
//				sep = dDistToHead;
//			}
//		}
//	}
//	return pConflictV;
//}


LandsideVehicleInSim* LandsideIntersectLaneLinkInSim::GetConflictVehicle( LandsideVehicleInSim* pVehicle, double distInLink,
																		 double& dDistToConflict )const
{
	const static int dMinBuffer = 200;
	LandsideVehicleInSim* pconflictVehicle = NULL;
	int IndexInNode = mpParent->GetVehicleIndex(pVehicle);

	for(ConflitMap::const_iterator itr= mConflictMap.begin(); itr!=mConflictMap.end(); itr++)
	{
		double cfdist= itr->dist;
		const IntersectWithOtherLinkPos& otherlinkpos = *itr;
		
		if(cfdist< distInLink)
			continue;

		const double dDistToCfPos = cfdist - distInLink;

		for(int i=0;i<otherlinkpos.pOtherLink->GetInResVehicleCount();i++)
		{
			LandsideVehicleInSim* pOtherVehicle = otherlinkpos.pOtherLink->GetInResVehicle(i);
			if(pOtherVehicle==pVehicle)
				continue;

			int OtherIndexInNode  = mpParent->GetVehicleIndex(pOtherVehicle);
			double otherVehiclePosInOtherLink = pOtherVehicle->getState().distInRes;
			//check whether the vehicle in the range of conflict
			bool bConflict = false;
			if(otherVehiclePosInOtherLink < otherlinkpos.distInOtherLink )
			{
				bConflict = OtherIndexInNode < IndexInNode;
				/*double doffset = otherlinkpos.distInOtherLink - otherVehiclePosInOtherLink;
				if(doffset < pOtherVehicle->GetLength()*0.5 + pOtherVehicle->GetDecDist()  + dMinBuffer + pVehicle->GetWidth() )
				{					
				}*/
			}
			else{
				double dOffset = otherVehiclePosInOtherLink - otherlinkpos.distInOtherLink;
				if(dOffset < pOtherVehicle->GetLength()*0.5 + dMinBuffer)
				{
					bConflict = true;
				}
			}

			if(bConflict)
			{	
				if(!pconflictVehicle || dDistToCfPos < dDistToConflict)
				{
					pconflictVehicle = pOtherVehicle;
					dDistToConflict = dDistToCfPos;
				}
			}

		}
	}
	return pconflictVehicle;
}

//void LandsideIntersectLaneLinkInSim::InitConflict(double dTolerane)
//{
//	/*for(int i=0;i<mpParent->GetLinkageCount();i++)
//	{
//		LandsideIntersectLaneLinkInSim* pOtherlink = mpParent->GetLinkage(i);
//		if(pOtherlink==this)
//			continue;
//
//		IntersectPathPath2D intersect(m_path, pOtherlink->GetPath());		
//		int nCount = intersect.Intersects(0);
//		for(int  j=0;j<nCount;j++)
//		{
//			double dIndex = intersect.m_vpath1Parameters[j];
//			double dIndexOther = intersect.m_vpath2Parameters[j];
//			
//			double dDist = m_path.GetIndexDist((float)dIndex);
//			IntersectWithOtherLinkPos otherPos;
//
//			otherPos.dist = dDist;
//			otherPos.pOtherLink = pOtherlink;
//			otherPos.distInOtherLink = pOtherlink->GetPath().GetIndexDist((float)dIndexOther);
//
//			mConflictMap.push_back(otherPos);			
//		}
//	}*/
//}
//
void LandsideIntersectLaneLinkInSim::OnVehicleEnter( LandsideVehicleInSim* pVehicle, DistanceUnit dist,const ElapsedTime& t )
{
	__super::OnVehicleEnter(pVehicle,dist, t);
	mpParent->OnVehicleEnter(pVehicle,dist, t);
}
void LandsideIntersectLaneLinkInSim::OnVehicleExit( LandsideVehicleInSim* pVehicle,const ElapsedTime& t )
{
	__super::OnVehicleExit(pVehicle,t);
	mpParent->OnVehicleExit(pVehicle,t);
}

bool LandsideIntersectLaneLinkInSim::IsClosed() const
{
	if( LandsideIntersectLinkGroupInSim* pGroup = mpParent->GetLinkGroupById(m_nGroupID) )
	{
		if(pGroup->GetState() != LS_ACTIVE)
		{
			return true;
		}
	}
	return false;
}

LandsideIntersectLaneLinkInSim::LandsideIntersectLaneLinkInSim( LandsideIntersectionInSim* pIntersect,LandsideLaneExit* pLaneExit, LandsideLaneEntry* pLaneEntry ,int nGroupID )
{
	mpParent = pIntersect;
	mpEntry = pLaneEntry;
	mpExit = pLaneExit;
	m_nGroupID=nGroupID;
}

LandsideIntersectLinkGroupInSim* LandsideIntersectLaneLinkInSim::getGroup() const
{
	return mpParent->GetLinkGroupById(GetGroupID());
}

LandsideLayoutObjectInSim* LandsideIntersectLaneLinkInSim::getLayoutObject() const
{
	return mpParent;
}

LandsideVehicleInSim* LandsideIntersectLaneLinkInSim::GetAheadVehicle( LandsideVehicleInSim* mpVehicle, DistanceUnit distInLane, DistanceUnit& aheadDist )
{
	return GetAheadVehicleByDistInRes(mpVehicle,distInLane,aheadDist);
}

//LandsideVehicleInSim* LandsideIntersectLaneLinkInSim::CheckSpaceConflict(LandsideVehicleInSim* pVeh,  const ARCPolygon& poly )
//{
//	LandsideIntersectionInSim* pIntersection = getIntersection();
//	if(pIntersection)
//	{
//		for(int i=0;i<pIntersection->GetLinkageCount();i++)
//		{
//			LandsideIntersectLaneLinkInSim* pOtherLink = pIntersection->GetLinkage(i);
//			if(pOtherLink==this)
//				continue;
//
//			for(int iV = 0; iV < pOtherLink->GetInResVehicleCount();iV++)
//			{
//				LandsideVehicleInSim* pOtherV = pOtherLink->GetInResVehicle(iV);
//				if(pOtherV == pVeh )
//					continue;
//
//				if( !pVeh->bCanWaitFor(pOtherV) )
//					continue;
//
//				if( poly.IsOverlapWithOtherPolygon(pOtherV->getBBox(pVeh->curTime()) ) )
//					return pOtherV;
//			}
//		}
//	}		
//	return NULL;
//}

LandsideVehicleInSim* LandsideIntersectLaneLinkInSim::CheckPathConflict( LandsideVehicleInSim* pVeh, const CPath2008& path,DistanceUnit& minDist )
{
	LandsideVehicleInSim* pConflict = NULL;

	LandsideIntersectionInSim* pIntersection = getIntersection();
	if(pIntersection)
	{
		for(int i=0;i<pIntersection->GetLinkageCount();i++)
		{
			LandsideIntersectLaneLinkInSim* pOtherLink = pIntersection->GetLinkage(i);
			if(pOtherLink==this)
				continue;

			for(int iV = 0; iV < pOtherLink->GetInResVehicleCount();iV++)
			{
				LandsideVehicleInSim* pOtherV = pOtherLink->GetInResVehicle(iV);
				if(pOtherV == pVeh )
					continue;

				if( !pVeh->bCanWaitFor(pOtherV) )
					continue;

				IntersectPathPath2D intersect;
				double dWidth = (pVeh->GetWidth()+pOtherV->GetWidth())*0.5;
				if(intersect.Intersects(path, pOtherV->getSpanPath(),dWidth)>0)
				{
					double dIntserctIndex = intersect.m_vIntersectPtIndexInPath1.front();
					double dOtherInterIndex = intersect.m_vIntersectPtIndexInPath2.front();

					double dist = path.GetIndexDist(dIntserctIndex);
					double dOtherDist = path.GetIndexDist(dOtherInterIndex);
					if(dist < dOtherDist)
						continue;

					if(!pConflict)
					{
						minDist = dist;
						pConflict = pOtherV;
					}
					else if(minDist > dist)
					{
						minDist = dist;
						pConflict = pOtherV;						
					}					
				}
			}
		}
	}		
	return pConflict;
}



void LandsideLaneNodeList::DeleteClear()
{
	for(iterator itr = begin();itr!=end();++itr)
	{
		delete *itr;
	}
	clear();
}
void LandsideIntersectLinkGroupInSim::InitLogEntry( OutputLandside* pOutput )
{
	IntersectionLinkGroupDescStruct& desc = m_logEntry.GetLinkGroupDesc();
	desc.groupID = m_nGroupID;
	desc.intersectionID = mpParent->getInput()->getID();
	m_logEntry.SetEventLog(pOutput->m_SimLogs.getLinkGroupEventLog());
	//m_logEntry.SetEventLog();	
}
void LandsideIntersectLinkGroupInSim::WriteLog(const ElapsedTime& t)
{
	IntersectionLinkGroupEventStruct curEvent;
	curEvent.state=m_eState;
	curEvent.time=t.getPrecisely();
	m_logEntry.addEvent(curEvent);
}
void LandsideIntersectLinkGroupInSim::SetCtrlTime(TrafficCtrlTime &ctrlTime)
{
	m_ctrlTime=ctrlTime;
}
void LandsideIntersectLinkGroupInSim::SetState(LinkGroupState eState)
{
	m_eState=eState;
	for (int i=0;i<(int)m_vIinkCrossWalk.size();i++)
	{
		CCrossWalkInSim *pCrossWalk=(CCrossWalkInSim *)m_vIinkCrossWalk.at(i);
		if (pCrossWalk->getCrossType()!=Cross_Intersection)
		{
			continue;
		}
		switch(m_eState)
		{
		case LS_ACTIVE:
			{
				pCrossWalk->setPaxLightState(CROSSLIGHT_RED);
				break;
			}
		case LS_BUFFER:
			{
				pCrossWalk->setPaxLightState(CROSSLIGHT_RED);
				break;
			}
		case LS_CLOSE:
			{
				pCrossWalk->setPaxLightState(CROSSLIGHT_GREEN);
				break;
			}
		case LS_DISABLE:
			{
				pCrossWalk->setPaxLightState(CROSSLIGHT_GREEN);
				break;
			}
		default:
			{
				break;
			}
		}
		
	}
}
void LandsideIntersectLinkGroupInSim::OnActive(CARCportEngine*pEngine)
{
	ElapsedTime nextTime;
	if (!m_bFirstTimeActive)
	{
		switch (m_eState)
		{
		case LS_ACTIVE:
			{
				SetState(LS_BUFFER);
				nextTime=(curTime()+(long)m_ctrlTime.bufferTime);
				break;
			}
		case LS_BUFFER:
			{
				SetState(LS_CLOSE);
				nextTime=(curTime()+(long)m_ctrlTime.closeTime);
				break;
			}
		case LS_CLOSE:
			{
				SetState(LS_ACTIVE);
				nextTime=(curTime()+(long)m_ctrlTime.activeTime);
				break;
			}
		default:
			{
				break;
			}
		}
		
	}else
	{
		if (m_nFirstCloseTime == 0)
		{
			SetState(LS_ACTIVE);
			nextTime=(curTime()+(long)m_ctrlTime.activeTime);

		}else
		{
			SetState(LS_CLOSE);
			nextTime=(curTime()+(long)m_nFirstCloseTime);
		}
		m_bFirstTimeActive=false;
	}

	//LinkStateChangeSignal *linkStateSignal=new LinkStateChangeSignal;
	//SendSignal(linkStateSignal);
	//ClearListener();
	NotifyObservers();



	WriteLog(curTime());
	if (/*m_tEndTime==0L ||*/ nextTime<m_tEndTime)
	{
		Activate(nextTime);
	}
	else
	{
		SetState(LS_ACTIVE);
	}

}


void LandsideIntersectLinkGroupInSim::OnTerminate( CARCportEngine*pEngine ) 
{
	WriteLog(curTime());
	m_logEntry.saveEvents();
}

void LandsideIntersectLinkGroupInSim::End( const ElapsedTime& t )
{
	WriteLog(t);
	m_logEntry.saveEvents();
}

LandsideIntersectLinkGroupInSim::LandsideIntersectLinkGroupInSim( LandsideIntersectionInSim *pVehicle,int nGroupID )
{
	m_nGroupID=nGroupID;
	m_eState=LS_ACTIVE;
	m_bFirstTimeActive=true;
	m_nFirstCloseTime=0;
	mpParent=pVehicle;
}
