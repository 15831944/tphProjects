#include "StdAfx.h"
#include ".\Goal_MoveInRoad.h"
#include "Goal_MoveInCurbside.h"
#include "..\LandsideResourceManager.h"
#include "..\ARCportEngine.h"
#include "..\LandsideSimulation.h"
#include "BlockTag.h"
#include "..\LandsideTaxiQueueInSim.h"

class ExceptionCannotFindParkingPos : public SGoalException
{
public:
	ExceptionCannotFindParkingPos(LandsideCurbSideInSim* pCurbSide){
		sDesc = _T("Can not Find Parking Pos to Curbside");
		m_pCurbSide = pCurbSide;
	}
	LandsideCurbSideInSim* m_pCurbSide;
};

//
void Goal_ParkingCurbside::Activate( CARCportEngine* pEngine )
{
	//RemoveAllSubgoals();

	mPath.DeleteClear();
	if(mpDest = mpCurb->getParkingSpot().FindParkingPosRandom(m_pOwner,mPath))
	{		
		m_iStatus = gs_active;
		mpDest->SetPreOccupy(m_pOwner);
		mpSubGoal =  new Goal_MoveInRoad(m_pOwner,mpCurb,mPath,true,false);
		mpCurb->RemoveListener(m_pOwner);	

		if (m_pOwner->WaitInResourceQueue(mpCurb))
		{
			LandsideResourceQueueItem* pQueue = new LandsideResourceQueueItem;
			LandsideResourceInSim* pRes = mpCurb;
			pQueue->m_nVehicleID = m_pOwner->getID();
			pQueue->m_strVehicleType = m_pOwner->getName().toString();
			pQueue->m_nResourceID = pRes->getLayoutObject()->getInput()->getID();
			pQueue->m_strResName = pRes->getLayoutObject()->getInput()->getName().GetIDString();
			pQueue->m_eTime = m_pOwner->curTime();
			pQueue->m_enumOperation = LandsideResourceQueueItem::QO_Exit;
			m_pOwner->LeaveResourceQueue(mpCurb->getLayoutObject());
		
			pEngine->GetLandsideSimulation()->getOutput()->m_SimLogs.getResourceQueueLog().AddItem(pQueue);
		}
	}
	else
	{
		
		MobileState lastState = m_pOwner->getLastState();
		if( lastState.dSpeed >0 )
		{
			lastState.dSpeed = 0;
			m_pOwner->MoveToPos(lastState);
		}

		//check vehicle can go to curbside and set resource queue
		if (m_pOwner->WaitInResourceQueue(mpCurb) == false)
		{
			LandsideResourceQueueItem* pQueue = new LandsideResourceQueueItem;
			LandsideResourceInSim* pRes = mpCurb;
			pQueue->m_nVehicleID = m_pOwner->getID();
			pQueue->m_strVehicleType = m_pOwner->getName().toString();
			pQueue->m_nResourceID = pRes->getLayoutObject()->getInput()->getID();
			pQueue->m_strResName = pRes->getLayoutObject()->getInput()->getName().GetIDString();
			pQueue->m_eTime = m_pOwner->curTime();
			pQueue->m_enumOperation = LandsideResourceQueueItem::QO_Entry;


			m_pOwner->EnterResourceQueue(pRes->getLayoutObject());

			pEngine->GetLandsideSimulation()->getOutput()->m_SimLogs.getResourceQueueLog().AddItem(pQueue);
		}
		
	}
}




void Goal_ParkingCurbside::PostProcessSubgoals(CARCportEngine* pEngine)
{
	if(mpDest)
	{
		MobileState lastmvPt = m_pOwner->getLastState();
		MobileState nextState = lastmvPt;

		
		nextState.pRes = mpDest;
		nextState.distInRes = 0;//mpDest->GetDistInLane();
		nextState.dSpeed = 0;
		nextState.pos = mpDest->GetParkingPos();	
		
		nextState.time = lastmvPt.time + lastmvPt.moveTime(nextState);

		m_pOwner->UpdateState(nextState);		
		nextState.pos += mpDest->GetParkingPosDir().Normalize();
		m_pOwner->UpdateState(nextState);

	}
}

Goal_ParkingCurbside::Goal_ParkingCurbside( LandsideVehicleInSim* pEnt, LandsideCurbSideInSim* pDest )
:LandsideVehicleGoal(pEnt)
{
	mpCurb = pDest;
	m_sDesc = "Goal_ParkingCurbside";
	mpDest = NULL;
	mpSubGoal = NULL;
}

void Goal_ParkingCurbside::Process( CARCportEngine* pEngine )
{
	ActivateIfInactive(pEngine);
	if(isActive())
	{
		ASSERT(mpSubGoal);
		if(mpSubGoal)
		{
			mpSubGoal->Process(pEngine);
			if(mpSubGoal->isComplete())
			{
				PostProcessSubgoals(pEngine);
				cpputil::autoPtrReset(mpSubGoal);
				setComplete();
			}	
			else if (mpSubGoal->isFailed())
			{
				setFailed();
				cpputil::autoPtrReset(mpSubGoal);
			}
		}		
	}

}

Goal_ParkingCurbside::~Goal_ParkingCurbside()
{
	mPath.DeleteClear();
	delete mpSubGoal;
}

void Goal_ExitCurbside::Activate( CARCportEngine* pEngine )
{
	RemoveAllSubgoals();

	mPath.DeleteClear();	
	if(mpCurb->getParkingSpot().FindLeavePath(m_pOwner,mPath))
	{		
		m_iStatus = gs_active;		
		AddSubgoalFront(new Goal_MoveInRoad(m_pOwner,NULL,mPath,false,false));
		mpCurb->RemoveListener(m_pOwner);
	}
	else
	{
		mpCurb->AddListener(m_pOwner);
	}
}

Goal_ExitCurbside::Goal_ExitCurbside( LandsideVehicleInSim* pEnt,LandsideCurbSideInSim* pCurb )
:LandsideVehicleSequenceGoal(pEnt)
{
	mpCurb=pCurb;
	m_sDesc = "Goal_LeaveCurbside";
}


Goal_ExitCurbside::~Goal_ExitCurbside(){ mPath.DeleteClear(); }



bool Goal_ExitCurbside::FindOtherLaneOut(LandsideLaneNodeList& path)
{
	LandsideResourceInSim* pAtRes = m_pOwner->getLastState().getLandsideRes();
	if(!pAtRes) return false;
	LandsideLaneInSim* pAtLane = pAtRes->toLane();
	if(!pAtLane) return false;
	CPoint2008 dAtPos = m_pOwner->getLastState().pos;	


	ASSERT(FALSE);
	LandsideLaneInSim* pOtherLane = NULL;// mpCurb->GetNotAtlane(pAtLane);	
	if(pOtherLane) // on the lane that the curb at 
	{
		//LandsideLaneEntry* pLaneEntry = new LandsideLaneEntry();
		//pLaneEntry->SetPosition(pAtLane,dAtPos);
		//pLaneEntry->SetFromRes(pAtLane);
		//path.push_back(pLaneEntry);

		////find a lane that the curb is not at
		//LandsideLaneExit* pLaneExit = new LandsideLaneExit();
		//pLaneExit->SetPosition(pAtLane, pLaneEntry->m_distInlane+m_pOwner->GetLength()*0.5);
		//pLaneExit->SetToRes(pOtherLane);
		//path.push_back(pLaneExit);	

		//
		LandsideLaneEntry * pOtherLaneEntry= new LandsideLaneEntry();
		pOtherLaneEntry->SetPosition(pOtherLane,dAtPos);
		pOtherLaneEntry->SetFromRes(pAtLane);
		path.push_back(pOtherLaneEntry);

		LandsideLaneExit* pOtherLaneExit = new LandsideLaneExit();
		pOtherLaneExit->SetPosition(pOtherLane, pOtherLaneEntry->m_distInlane + m_pOwner->GetLength());
		pOtherLaneExit->SetToRes(pOtherLane);
		path.push_back(pOtherLaneExit);

		return true;
	}
	return false;
}


void Goal_ExitCurbside::PostProcessSubgoals( CARCportEngine* pEngine )
{
	mpCurb->getParkingSpot().ReleaseParkingPos(m_pOwner, m_pOwner->curTime());
}



void Goal_SkipParkCurbside::Activate( CARCportEngine* pEngine )
{
	RemoveAllSubgoals();

	mPath.DeleteClear();
	if(FindSkipPath(mPath))
	{		
		m_iStatus = gs_active;		
		AddSubgoalFront(new Goal_MoveInRoad(m_pOwner,NULL,mPath,false,false));
		mpCurb->RemoveListener(m_pOwner);
	}
	else
	{
		mpCurb->AddListener(m_pOwner);
	}
}

Goal_SkipParkCurbside::Goal_SkipParkCurbside( LandsideVehicleInSim* pEnt,LandsideCurbSideInSim* pCurb ) :LandsideVehicleSequenceGoal(pEnt)
{
	mpCurb = pCurb;
	m_sDesc = "Goal_SkipParkCurbside";
}

bool Goal_SkipParkCurbside::FindSkipPath(LandsideLaneNodeList& path)
{
	LandsideResourceInSim* pAtRes = m_pOwner->getLastState().getLandsideRes();
	if(!pAtRes) return false;
	LandsideLaneInSim* pAtLane = pAtRes->toLane();
	if(!pAtLane) return false;
	CPoint2008 dAtPos = m_pOwner->getLastState().pos;	


	//ASSERT(FALSE);
	LandsideLaneInSim* pOtherLane = mpCurb->GetNotAtlane(pAtLane);	
	if(pOtherLane) // on the lane that the curb at 
	{
		LandsideLaneEntry* pLaneEntry = new LandsideLaneEntry();
		pLaneEntry->SetPosition(pAtLane,dAtPos);
		pLaneEntry->SetFromRes(pAtLane);
		path.push_back(pLaneEntry);

		//find a lane that the curb is not at
		LandsideLaneExit* pLaneExit = new LandsideLaneExit();
		pLaneExit->SetPosition(pAtLane, pLaneEntry->m_distInlane+m_pOwner->GetLength()*0.5);
		pLaneExit->SetToRes(pOtherLane);
		path.push_back(pLaneExit);	

		//
		LandsideLaneEntry * pOtherLaneEntry= new LandsideLaneEntry();
		pOtherLaneEntry->SetPosition(pOtherLane,pLaneExit->GetPosition() );
		pOtherLaneEntry->SetFromRes(pAtLane);
		path.push_back(pOtherLaneEntry);

		LandsideLaneExit* pOtherLaneExit = new LandsideLaneExit();
		pOtherLaneExit->SetPosition(pOtherLane, pOtherLaneEntry->m_distInlane + m_pOwner->GetLength());
		pOtherLaneExit->SetToRes(pOtherLane);
		path.push_back(pOtherLaneExit);

		return true;
	}
	return false;

}


//////////////////////////////////////////////////////////////////////////
void Goal_ParkingCurbside2::PostProcessSubgoals( CARCportEngine* pEngine )
{
	if(!mPath.empty())
	{
		LandsideLaneNodeInSim* pNode = mPath.back();
		MobileState lastmvPt = m_pOwner->getLastState();
		MobileState nextState = lastmvPt;


		nextState.pRes = pNode->mpLane;
		nextState.distInRes = pNode->m_distInlane;
		nextState.dSpeed = 0;
		nextState.pos = pNode->m_pos;	

		nextState.time = lastmvPt.time + lastmvPt.moveTime(nextState);

		m_pOwner->UpdateState(nextState);		
		nextState.pos += pNode->m_dir.GetLengtDir(1);
		m_pOwner->UpdateState(nextState);

	}
}

Goal_ParkingCurbside2::~Goal_ParkingCurbside2()
{
	mPath.DeleteClear();
}

void Goal_ParkingCurbside2::Activate( CARCportEngine* pEngine )
{
	RemoveAllSubgoals();

	mPath.DeleteClear();
	if( mpCurb->FindParkingPos(m_pOwner,mPath))
	{		
		m_iStatus = gs_active;
		//mpDest->SetPreOccupy(m_pOwner);
		AddSubgoalEnd(new Goal_MoveInRoad(m_pOwner,NULL,mPath,true,false));
		mpCurb->RemoveListener(m_pOwner);		
	}
	else
	{
		AddSubgoalEnd( new Goal_SkipParkCurbside(m_pOwner,mpCurb) );
		AddSubgoalEnd( new Goal_MoveToRes(m_pOwner,mpCurb) );
		AddSubgoalEnd( new Goal_ParkingCurbside2(m_pOwner,mpCurb) );
		m_iStatus = gs_active;
	}
}
//////////////////////////////////////////////////////////////////////////
void Goal_ParkingTaxiQueue::Activate( CARCportEngine* pEngine )
{
	RemoveAllSubgoals();

	mPath.DeleteClear();
	if(mpDest = mpCurb->getParkingSpot().FindParkingPos(m_pOwner,mPath))
	{		
		m_iStatus = gs_active;
		mpDest->SetPreOccupy(m_pOwner);
		AddSubgoalFront(new Goal_MoveInRoad(m_pOwner,mpCurb,mPath,true,false));
		mpCurb->RemoveListener(m_pOwner);		
	}
	else
	{
		/*AddSubgoalEnd( new Goal_SkipParkCurbside(m_pOwner,mpCurb) );
		AddSubgoalEnd( new Goal_MoveToRes(m_pOwner,mpCurb) );
		AddSubgoalEnd( new Goal_ParkingCurbside(m_pOwner,mpCurb) );
		m_iStatus = gs_active;*/
	}
}




Goal_ParkingTaxiQueue::Goal_ParkingTaxiQueue( LandsideVehicleInSim* pEnt, LandsideTaxiQueueInSim* pDest ) 
:LandsideVehicleSequenceGoal(pEnt)
{
	mpCurb = pDest;
	m_sDesc = "Goal_ParkingTaxiQueue";
	mpDest = NULL;
}

void Goal_ParkingTaxiQueue::PostProcessSubgoals( CARCportEngine* pEngine )
{

	if(mpDest)
	{
		MobileState lastmvPt = m_pOwner->getLastState();
		MobileState nextState = lastmvPt;


		nextState.pRes = mpDest->GetLane();
		nextState.distInRes = mpDest->GetDistInLane();
		nextState.dSpeed = 0;
		nextState.pos = mpDest->GetParkingPos();	

		nextState.time = lastmvPt.time + lastmvPt.moveTime(nextState);

		m_pOwner->UpdateState(nextState);		
		nextState.pos += mpDest->GetParkingPosDir().Normalize();
		m_pOwner->UpdateState(nextState);

	}
}

Goal_ExitTaxiQ::Goal_ExitTaxiQ( LandsideVehicleInSim* pEnt,LandsideTaxiQueueInSim* pCurb )
:LandsideVehicleSequenceGoal(pEnt)
{
	mpCurb = pCurb;
	m_sDesc = "Goal_ExitTaxiQueu";	
}

void Goal_ExitTaxiQ::Activate( CARCportEngine* pEngine )
{
	RemoveAllSubgoals();
	mpCurb->removeServiceVehicle(m_pOwner);
	mpCurb->DelWaitingVehicle(m_pOwner);

	mPath.DeleteClear();	
	if(mpCurb->getParkingSpot().FindLeavePath(m_pOwner,mPath))
	{		
		m_iStatus = gs_active;		
		AddSubgoalFront(new Goal_MoveInRoad(m_pOwner,NULL,mPath,false,false));
		mpCurb->RemoveListener(m_pOwner);
	}
	else
	{
		mpCurb->AddListener(m_pOwner);
	}
}

void Goal_ExitTaxiQ::PostProcessSubgoals( CARCportEngine* pEngine )
{
	mpCurb->getParkingSpot().ReleaseParkingPos(m_pOwner, m_pOwner->curTime());
}

