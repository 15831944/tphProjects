#include "StdAfx.h"
#include "LandsideVehicleInSim.h"
#include "ARCportEngine.h"
#include "MobElementsDiagnosis.h"
#include "Common\FileOutPut.h"
#include "LandsideSimulation.h"
#include "Landside\VehicleGroupProperty.h"
#include "Common\ARCUnit.h"
#include "LandsideBaseBehavior.h"
#include "Landside\SpeedControlData.h"
#include "Common\ProDistrubutionData.h"
#include "core\SAgent.h"
#include "LandsideExternalNodeInSim.h"
#include "core\SGoal.h"
#include "LandsideSimErrorShown.h"
#include "LandsideParkingLotInSim.h"
#include "Landside\State_InParkingLot.h"
#include "Landside\State_InCurbside.h"
#include "Landside\State_InBusStation.h"
#include "Landside\InputLandside.h"
#include "Common\ALTObjectID.h"
#include "Landside\LandsideCurbsideStrategy.h"
#include "LandsideStrategyStateInSim.h"
#include "LandsideCurbsideInSim.h"
#include "LandsideResourceManager.h"

#define VEHICLE0LOG _T("D:\\LandsideDebug\\Vehicle0.log")

//void LandsideVehicleInSim::OnTerminate(CARCportEngine*pEngine)
//{
//	DeActive();
//	Kill(pEngine->GetLandsideSimulation(),curTime());
//}

//write state to log
int LandsideVehicleInSim::WriteLog(const MobileState& state)
{
	LandsideVehicleEventStruct curEvent;
	memset(&curEvent,0,sizeof(LandsideVehicleEventStruct));

	curEvent.time = state.time.getPrecisely();
	//curEvent.eventNum = Event::getCurEventNum();
	const CPoint2008& pt = state.pos;
	curEvent.x = static_cast<float>(pt.x);
	curEvent.y = static_cast<float>(pt.y);
	curEvent.z = static_cast<float>(pt.z);	
	curEvent.state = m_curMode;
	curEvent.mode = m_pWaitVeh?m_pWaitVeh->getID():-1;
	curEvent.speed = static_cast<float>(state.dSpeed);
	curEvent.wantspeed = static_cast<float>(m_pairResourceSpeed.dSpeed);
	curEvent.eventNum = Event::getCurEventNum();

	if(state.getLandsideRes() && state.getLandsideRes()->getLayoutObject() )
		curEvent.resid = state.getLandsideRes()->getLayoutObject()->getID();
	curEvent.dist = static_cast<float>(state.distInRes);

	m_logEntry.addEvent(curEvent);
	int nEventNum = m_logEntry.getEventList().size();
	//debug
	//VehicleDebugLog::LogEvent(this);
	if( MobElementsDiagnosis()->isInList(m_id) )
	{
		CFileOutput outfile(VEHICLE0LOG);
		if(outfile)
		{
			outfile.LogItem(nEventNum);
			outfile.LogItem((long)state.time).LogItem(state.time);
			CString spos; 
			spos.Format(_T("(%.2f,%.2f,%.2f)"),state.pos.x ,state.pos.y,state.pos.z);
			outfile.LogItem(spos);
			outfile.LogItem(state.distInRes);

			if(LandsideResourceInSim* pRes = state.getLandsideRes())
			{  
				outfile.LogItem(pRes->print().GetString());
			}
			outfile.LogItem(state.dSpeed);
			outfile.Line();
		}			
	}

	
	return nEventNum;
}


void LandsideVehicleInSim::AddOnPax( LandsideBaseBehavior* plandsidePax )
{
	bool bAddAlraedy = std::find(m_vOnVehiclePax.begin(),m_vOnVehiclePax.end(),plandsidePax)!=m_vOnVehiclePax.end();
	if(bAddAlraedy)
	{
		//ASSERT(FALSE);
		return;
	}

	m_vOnVehiclePax.push_back(plandsidePax);
	int& paxload = m_logEntry.GetVehicleDesc().paxcount;
	paxload++;
}

void LandsideVehicleInSim::RemoveOnPax( LandsideBaseBehavior* plandsidePax ,const ElapsedTime& t)
{
	std::vector<LandsideBaseBehavior*>::iterator itr = std::find(m_vOnVehiclePax.begin(),m_vOnVehiclePax.end(),plandsidePax);
	if(itr!=m_vOnVehiclePax.end())
		m_vOnVehiclePax.erase(itr);
}



CPoint2008 LandsideVehicleInSim::getOffVehiclePos(const CPoint2008& nextPt) const
{
	const MobileState& lastState = getLastState();
	ARCVector3  vdir = nextPt -  lastState.pos;
	
    ARCVector2 dir = lastState.dir.xy().PerpendicularLCopy();
	if( vdir.xy().cross( lastState.dir.xy() ) >0 )
		dir = -dir;
	dir.SetLength(200);
	return  lastState.pos + ARCVector3(dir,0);
}


const LandsideVehicleInSim* LandsideVehicleInSim::FindEndMovablePax( const LandsideVehicleInSim* pBreakPax ) const
{
	if(m_pWaitVeh == pBreakPax)
		return pBreakPax;
	if(m_pWaitVeh )
		return m_pWaitVeh->FindEndMovablePax(pBreakPax);	
	return NULL;
}




//class VehicleMoveSig : public SSignal{
//public:
//	CString getDesc(){ return _T("Vehicle Move"); }
//};

void LandsideVehicleInSim::StartMove(const ElapsedTime&t )
{
	if(m_lastState.time < t) //write start moving log
	{
		//WriteLog(m_lastState);
		m_lastState.time = t;
		WriteLog(m_lastState);
	}
	//WaitForVehicle(NULL);
	NotifyObservers();

	//if(HaveListeners())
	//	SendSignal(new VehicleMoveSig());	
}




bool LandsideVehicleInSim::bCanWaitFor( const LandsideVehicleInSim* pOtherPax )
{
	if(pOtherPax)
	{			
		return pOtherPax->FindEndMovablePax(this)!=this;	
	}
	return true;
}




bool LandsideVehicleInSim::IsTerminated() const
{
	return m_curMode ==  _terminate;
}



void LandsideVehicleInSim::OnTerminate( CARCportEngine*pEngine )
{
	if(m_curMode == _terminate)
	{
		return ;
	}


	//add end state
	m_curMode = _terminate;	

	ElapsedTime t = curTime();


	MobileState endstate = getLastState();
	endstate.time = t;
	endstate.pRes = NULL;
	UpdateState(endstate);
	flushState(t);

	FlushLogEntry(pEngine->GetLandsideSimulation(),t);
	FlushOnVehiclePaxLog(t);
	if(pEngine->GetLandsideSimulation()->IsAllVehicleTerminate())
	{
		pEngine->GetLandsideSimulation()->DeactiveTrafficCtrl(t);
	}

}

int LandsideVehicleInSim::getID() const
{
	return m_logEntry.GetVehicleDesc().vehicleID;
}




int LandsideVehicleInSim::nLastVehicleId =0;

void LandsideVehicleInSim::initState( const MobileState& s )
{
	m_lastState = s;
	UpdateState(m_lastState);
	m_bInit =true;
}


void LandsideVehicleInSim::WaitForVehicle( LandsideVehicleInSim* pOtherPax )
{
	if(m_pWaitVeh)
	{
		m_pWaitVeh->RemoveObserver(this);
	}

	if(bCanWaitFor(pOtherPax))
	{
		m_pWaitVeh = pOtherPax;		
	}
	else
		ASSERT(FALSE);

	if(m_pWaitVeh)
		m_pWaitVeh->AddObserver(this);
}

LandsideVehicleInSim::LandsideVehicleInSim()
{
	m_bInit = false;
	m_id = nLastVehicleId++;
	m_pWaitVeh=NULL;	
	m_curMode = _onBirth;
	m_bLoadingPax = false;

	if(MobElementsDiagnosis()->isInList(m_id))
	{
		CFileOutput::Clear(VEHICLE0LOG);
	}
	m_pCurbStratgy = NULL;
	m_pParklotStratgy= NULL;
}

//bool LandsideVehicleInSim::GoalProcessToEnd( CARCportEngine *_pEngine )
//{
//	//_pEngine->GetLandsideSimulation()->StartTrafficCtrl(curTime());
//	flushState(curTime());
//	try
//	{
//		if(m_pGoal)
//		{
//			if(m_pGoal->isComplete() || m_pGoal->isFailed())
//			{
//				return true;
//			}
//			m_pGoal->Process(_pEngine);	
//			//update bbox
//			//updateBBox();
//			//
//			if(!isBlocked()){
//				ElapsedTime nextT;
//				nextT = getLastState().time;
//				if(nextT <= curTime()){ 
//					nextT  = curTime()+LandsideSimulation::tAwarenessTime;
//				}				
//				Activate(nextT);
//			}				
//			return false;
//		}	
//	}	
//	catch (SGoalException& e)
//	{
//		ShowError( e.getDesc(), "Runtime Error" );
//		OnTerminate(_pEngine);
//	}
//	return true;
//}
//
void LandsideVehicleInSim::SetProperty( LandsideVehicleProperty* pProperty )
{
	LandsideVehicleDescStruct& desc = m_logEntry.GetVehicleDesc();
	double dAcc = 20;
	double dDec = 40;
	m_dLength = 500;
	m_dWidth = 300;
	m_dHeight = 120;
	m_nCapacity = 0;

	if(pProperty)
	{
		dAcc = pProperty->GetAcc();
		dDec = pProperty->GetDec();
		m_dLength =  pProperty->GetLenght();
		m_dWidth = pProperty->GetWidth();
		m_dHeight = pProperty->GetHeight();
		m_nCapacity = pProperty->GetCapacity();
	}

	desc.dlength =  static_cast<float>(m_dLength);
	desc.dwidth = static_cast<float>(m_dWidth);
	desc.dheight = static_cast<float>(m_dHeight);

	//init performance
	mPerform.mAccSpd = ARCUnit::ConvertLength(dAcc,ARCUnit::M,ARCUnit::CM);
	mPerform.mDecSpd = ARCUnit::ConvertLength(dDec,ARCUnit::M,ARCUnit::CM); 
	mPerform.mNoramlSpd = mPerform.mMaxSpd = ARCUnit::ConvertVelocity(40,ARCUnit::KMpHR,ARCUnit::CMpS );
	m_opTable.InitFromPerform(mPerform);
}

void LandsideVehicleInSim::FlushLogEntry( LandsideSimulation*pEngine,const ElapsedTime&t )
{
	//flushState(t);
	if(m_lastState.time < t) 
	{
		WriteLog(m_lastState);
		m_lastState.time = t;
		WriteLog(m_lastState);
	}
	if( m_logEntry.saveEvents() )
	{
		OutputLandside* pOutput = pEngine->getOutput();
		pOutput->m_SimLogs.getVehicleLog().addItem(m_logEntry);
	};		
}

bool LandsideVehicleInSim::IsPaxOnVehicle( int nId ) const
{
	for(size_t i =0;i<m_vOnVehiclePax.size();i++)
	{
		if( m_vOnVehiclePax[i]->GetPersonID() == nId)
			return true;
	}
	return false;
}

void LandsideVehicleInSim::FlushOnVehiclePaxLog( const ElapsedTime& t )
{
	for(size_t i=0;i<m_vOnVehiclePax.size();++i)
	{
		LandsideBaseBehavior* pLandsidepax = m_vOnVehiclePax[i];
		pLandsidepax->terminate(t);
	}
	m_vOnVehiclePax.clear();
}

void LandsideVehicleInSim::addBlock( BlockTag* block )
{
	if(!hasBlock(block) )
	{
		m_vBlockTags.push_back(block);
	}
}

bool LandsideVehicleInSim::isBlocked() const
{
	return !m_vBlockTags.empty();
}

bool LandsideVehicleInSim::removeBlock( BlockTag* block,const ElapsedTime& t )
{
	bool bDel = false;
	std::vector<BlockTag*>::iterator itr = std::find(m_vBlockTags.begin(),m_vBlockTags.end(),block);
	if(itr!=m_vBlockTags.end()){
		m_vBlockTags.erase(itr);	
		bDel = true;
	}
	//if(!isBlocked()){
	//	Activate(t);
	//}
	return bDel;
}




void LandsideVehicleInSim::MoveToPos( MobileState state )
{
	MobileState laststate = getLastState();
	if( laststate.time < curTime() ){
		laststate.time = curTime();
		UpdateState(laststate);
	}
	ElapsedTime mvt = laststate.moveTime(state);
	state.time = laststate.time + mvt;
	UpdateState(state);	
}



double LandsideVehicleInSim::getSpeed( LandsideResourceInSim* pRes,double dist /*= 0*/ )
{
	if(pRes)
	{ 
		if(StretchSpeedControlData* pcontrol = pRes->GetSpeedControl(dist) )
		{
			if(pRes == m_pairResourceSpeed.pRes && pcontrol == m_pairResourceSpeed.pSpdCtrl )//same resource, keep the same speed
				return m_pairResourceSpeed.dSpeed;

			//resource changed, calculate the new speed
			double dSpeed = pcontrol->GetStretchSpeed()*100;
			if(ProbabilityDistribution * pDist =  pcontrol->GetStretchProDistribution().GetProbDistribution())
				dSpeed += pDist->getRandomValue() * 100;

			if(dSpeed <= 0)
			{
				CString strWarn;
				strWarn.Format(_T("Speed Control is too Low in %s"),pcontrol->GetStretchName().GetString() );
				ShowError(strWarn , "Setting Error");
				dSpeed = ARCUnit::ConvertVelocity(40,ARCUnit::KMpHR,ARCUnit::CMpS);
			}

			m_pairResourceSpeed.pRes = pRes;
			m_pairResourceSpeed.dSpeed = dSpeed;
			m_pairResourceSpeed.pSpdCtrl = pcontrol;
			
			return dSpeed;
		}
	}		
	return  ARCUnit::ConvertVelocity(40,ARCUnit::KMpHR,ARCUnit::CMpS);
}


MobileState& LandsideVehicleInSim::getState( const ElapsedTime&t )
{
	return m_lastState;
	/*if(t < m_lastState.time)
		return m_lastState;*/

	//if(t != m_tState.time){
	//	MobileState statePre= m_lastState;
	//	for(int i=0;i< m_nextSteps.GetCount();i++){
	//		const MobileState& stateNext = m_nextSteps.at(i);
	//		if(stateNext.time>t){
	//			MovingTrace::InterpolateByTime(statePre,stateNext,t,m_tState);
	//			break;
	//		}
	//		statePre = stateNext;
	//	}		
	//}	
	//m_tState.time = t;
	//return m_tState;
}

MobileState& LandsideVehicleInSim::getState()
{
	return getState(curTime());
}

//ARCPolygon& LandsideVehicleInSim::getBBox( const ElapsedTime& t )
//{
//	/*if(t != m_tBBoxTime){
//		m_tBBoxTime = t;
//
//		MobileState& tstate = getState(t);
//		DistanceUnit minDecDist = mPerform.minDecDist(tstate.dSpeed);
//		minDecDist = MAX(minDecDist,100);
//		m_tBBox.Build( m_tState.pos , m_tState.dir ,m_dLength*0.5, m_dLength*0.5, m_dWidth);
//	}*/
//	return m_tBBox;
//}
//


void LandsideVehicleInSim::flushState( const ElapsedTime& t )
{
	//ASSERT(m_bInit);
	if(!m_bInit) 
		return;
	if(m_lastState.time<t)
	{
		m_lastState.time = t;		
	}	

	//m_pastSteps.Clear();
	//m_pastSteps.AddPoint(m_lastState);
	//
	//m_nextSteps.Clear();

	//int nStateCnt = m_nextSteps.GetCount();

	//for(int i=0;i<nStateCnt;i++)
	//{
	//	const MobileState& stateThis = m_nextSteps.at(i);
	//	if(stateThis.time > t)
	//	{
	//		MobileState midState;
	//		MovingTrace::InterpolateByTime(m_lastState,stateThis,t, midState);
	//		UpdateState(midState);
	//		break;
	//	}
	//	UpdateState(m_nextSteps.at(i));		
	//}
}

const MobileState& LandsideVehicleInSim::getLastState() const
{	
	return m_lastState;
}

void LandsideVehicleInSim::UpdateState( const MobileState& state )
{
//	ASSERT(state.getLandsideRes());

	bool bNeedLog = false;

	if( state.dAcc!= m_lastState.dAcc )
		bNeedLog = true;
	if(state.isConerPt)
		bNeedLog = true;

	if(state.pRes!= m_lastState.pRes )
	{		
		WriteLog(m_lastState);
		if(m_lastState.pRes)
		{
			m_lastState.getLandsideRes()->OnVehicleExit(this,state.time);
		}	
		if(state.pRes)
		{
			state.getLandsideRes()->OnVehicleEnter(this,state.distInRes,state.time);
		}
		bNeedLog = true;
	}	
	
	//need add a start moving log
	if(m_lastState.dSpeed == 0 && state.dSpeed !=0 )
	{
		WriteLog(m_lastState);
	}
	
	//update state
	ARCVector3 dir = state.pos - m_lastState.pos;
	if(dir.Length()<ARCMath::EPSILON)
		dir = m_lastState.dir;
	m_lastState = state;
	m_lastState.dir = dir;
	
	if(bNeedLog)
		WriteLog(state);
}

//void LandsideVehicleInSim::updateBBox()
//{
//	DistanceUnit minDecDist = mPerform.minDecDist(m_lastState.dSpeed);
//	minDecDist = MAX(minDecDist,100);
//	m_tBBox.Build( m_lastState.pos , m_lastState.dir ,m_dLength*0.5+minDecDist, m_dLength*0.5, m_dWidth);		
//}

bool LandsideVehicleInSim::hasBlock( BlockTag* block ) const
{
	std::vector<BlockTag*>::const_iterator itr = std::find(m_vBlockTags.begin(),m_vBlockTags.end(),block);
	return itr!=m_vBlockTags.end();
}

//bool LandsideVehicleInSim::DefaultHandleSignal( SSignal*pmsg )
//{
//	if(IsTerminated())
//	{
//		if( pmsg->getSource() )
//			pmsg->getSource()->RemoveListener(this);
//		return true;
//	}
//	Activate(pmsg->curTime());
//	return true;
//}

//bool LandsideVehicleInSim::EndPaxService( CARCportEngine*pEngine, const ElapsedTime& eEventTime )
//{
//	return false;
//}


void LandsideVehicleInSim::NotifyPaxMoveInto(CARCportEngine*pEngine,  LandsideBaseBehavior* pPassenger, const ElapsedTime& eTime )
{
	int nState = pPassenger->getState();
	ASSERT(0);
}

//void LandsideVehicleInSim::NewServiceTimeEndEvent( const ElapsedTime& endT )
//{
//	if(m_pServiceEndEvt)
//	{
//		m_pServiceEndEvt->CloseEvent(true);
//	}
//	m_pServiceEndEvt=new LandsideVehicleServiceEndEvent(this, endT) ;
//	m_pServiceEndEvt->addEvent();
//}
//
//bool LandsideVehicleInSim::IsServiceTimeEnd() const
//{
//	if(m_pServiceEndEvt)
//	{
//		return m_pServiceEndEvt->IsEventClosed();
//	}
//	return true;
//}

//
//void LandsideVehicleInSim::OnServiceTimeEnd( CARCportEngine* pEngine, const ElapsedTime& eEventTime )
//{
//
//}

DistanceUnit LandsideVehicleInSim::GetSafeDistToFront() const
{
	DistanceUnit minDecDist = mPerform.minDecDist(m_lastState.dSpeed)+100;
	//minDecDist = MAX(minDecDist,100);
	return GetLength()*0.5 + minDecDist;
}

bool LandsideVehicleInSim::WaitInResourceQueue(LandsideLayoutObjectInSim* pRes) const
{
	int nCount = (int)m_repResourceQueue.size();
	for (int i = 0; i < nCount; i++)
	{
		landsideResourceQueue* pQueue = m_repResourceQueue[i];
		if (pQueue->WaitInResourceQueue(pRes))
		{
			return true;
		}
	}
	return false;
}

bool LandsideVehicleInSim::WaitInCrosswalkQueue(CCrossWalkInSim* pCrosswalk) const
{
	int nCount = (int)m_repResourceQueue.size();
	for (int i = 0; i < nCount; i++)
	{
		landsideResourceQueue* pQueue = m_repResourceQueue[i];
		if (pQueue->WaitInCrosswalkQueue(pCrosswalk))
		{
			return true;
		}
	}
	return false;
}

void LandsideVehicleInSim::EnterResourceQueue( LandsideLayoutObjectInSim* pRes )
{
	if (WaitInResourceQueue(pRes))//check vehicle whether wait in resource queue
		return;
	
	landsideResourceQueue* pQueue = new landsideResourceQueue;
	pQueue->SetWaitInQueueFlag(true);
	pQueue->SetlandsideRes(pRes);
	m_repResourceQueue.push_back(pQueue);
}

void LandsideVehicleInSim::LeaveResourceQueue(LandsideLayoutObjectInSim* pRes)
{
	int nCount = (int)m_repResourceQueue.size();
	for (int i = 0; i < nCount; i++)
	{
		landsideResourceQueue* pQueue = m_repResourceQueue[i];
		if (pQueue->GetLandsideRes() == pRes)
		{
			m_repResourceQueue.erase(m_repResourceQueue.begin() + i);
			break;
		}
	}
}

void LandsideVehicleInSim::EnterCrosswalkQueue( CCrossWalkInSim* pCrosswalk )
{
	if (WaitInCrosswalkQueue(pCrosswalk))//check vehicle whether wait in resource queue
		return;

	landsideResourceQueue* pQueue = new landsideResourceQueue;
	pQueue->SetWaitInQueueFlag(true);
	pQueue->SetCrosswalk(pCrosswalk);
	m_repResourceQueue.push_back(pQueue);
}

void LandsideVehicleInSim::LeaveCrosswalkQueue(CCrossWalkInSim* pCrosswalk)
{
	int nCount = (int)m_repResourceQueue.size();
	for (int i = 0; i < nCount; i++)
	{
		landsideResourceQueue* pQueue = m_repResourceQueue[i];
		if (pQueue->GetCrosswalk() == pCrosswalk)
		{
			m_repResourceQueue.erase(m_repResourceQueue.begin() + i);
			break;
		}
	}
}

LandsideVehicleInSim::~LandsideVehicleInSim()
{
	ClearResourceQueue();
	cpputil::autoPtrReset(m_pCurbStratgy);
	cpputil::autoPtrReset(m_pParklotStratgy);
}

void LandsideVehicleInSim::ClearResourceQueue()
{
	int nCount = (int)m_repResourceQueue.size();
	for (int i = 0; i < nCount; i++)
	{
		landsideResourceQueue* pQueue = m_repResourceQueue[i];
		if (pQueue)
		{
			delete pQueue;
		}
	}

	m_repResourceQueue.clear();
}

bool LandsideVehicleInSim::GetWaitResourceQueueFlag() const
{
	return m_repResourceQueue.empty() ? false : true;
}

landsideResourceQueue* LandsideVehicleInSim::GetLastResourceQueue() const
{
	if (m_repResourceQueue.empty())
	{
		return NULL;
	}

	return m_repResourceQueue.back();
}




void LandsideVehicleInSim::Continue()
{
	ElapsedTime tTime = MAX(getLastState().time, curTime() );
	Activate(tTime);
}


void LandsideVehicleInSim::StepTime( CARCportEngine* pEngine )
{
	Activate(curTime()+ LandsideSimulation::tAwarenessTime);
}

DistanceUnit LandsideVehicleInSim::getDecDist( double dSpeed )
{
	double dendDecDist = mPerform.minDecDist(dSpeed);
	dendDecDist += getIDModel().gap;
	dendDecDist += getIDModel().T * dSpeed;
	return dendDecDist;
}

void LandsideVehicleInSim::OnStateChanged()
{
	//log the state 
	if(m_curState && MobElementsDiagnosis()->isInList(m_id) )
	{	
		ElapsedTime curT = curTime();
		CFileOutput outfile(VEHICLE0LOG);
		outfile.Line();
		outfile.Log("<<<<<");
		outfile.LogItem(m_curState->getDesc());		
		outfile.LogItem((long)curT).LogItem(curT);
		outfile.Line();
	}
}

void LandsideVehicleInSim::MoveToExtNode( LandsideExternalNodeInSim* pNode )
{
	MobileState deadState;
	deadState.distInRes = 0;
	deadState.time = curTime();
	deadState.dSpeed = getSpeed(pNode,0);
	deadState.pRes = pNode;
	deadState.pos = pNode->getPos();
	UpdateState(deadState);
}

void LandsideVehicleInSim::OnActive( CARCportEngine*pEngine )
{	
	try
	{
		ExecuteCurState(pEngine);
	}
	catch(SGoalException& e)
	{
		LandsideSimErrorShown::VehicleSimWarning(this, e.getDesc(),"RunTimeError",curTime());
		OnTerminate(pEngine);
	}
}

bool LandsideVehicleInSim::InitBirthInLayoutObject( LandsideLayoutObjectInSim* pObj,CARCportEngine* pEngine )
{
	if(!pObj)
	{
		return false;
	}
	if(LandsideParkingLotInSim* plot = pObj->toParkLot())
	{
		if(!plot->IsSpotLess() ) 
		{
			LandsideParkingSpotInSim* pSpot = plot->getFreeSpot(this);
			if(pSpot)//got a free spot stay in there
			{
				//got to next state parking in lot			
				MobileState birthState;
				birthState.distInRes = 0;
				birthState.time = curTime();
				birthState.dSpeed = 0;
				birthState.pRes = pSpot;
				birthState.pos = pSpot->getPos();
				initState(birthState);
				birthState.pos+=pSpot->getDir();
				UpdateState(birthState);	
				SuccessBirthInLotSpot(pSpot,pEngine);
				return true;
			}			
		}
	}
	else if(LandsideExternalNodeInSim* pExtNode = pObj->toExtNode())
	{
		MobileState birthState;
		birthState.distInRes = 0;
		birthState.time = curTime();
		birthState.dSpeed = getSpeed(pExtNode,0);
		birthState.pRes = pExtNode;
		birthState.pos = pExtNode->getPos();
		initState(birthState);
		SuccessProcessAtLayoutObject(pObj,pExtNode,pEngine);
		return true;
	}
	CString sError;
	sError.Format(_T("%s can not birth on %s"), m_TypeName.toString().GetString(), pObj->getName().GetIDString().GetString() );
	ShowError(sError,"Definition Error");
	return false;
}



void MoveOperationTable::InitFromPerform( const MobileGroundPerform& perfom )
{	
	this->resize(_OpMoveTypeCount);
	(*this)[_MAXBREAK] = -perfom.mMaxBreak;
	(*this)[_SLOWDOWN] = -perfom.mDecSpd;
	(*this)[_UNIFORM] = 0;
	(*this)[_SLOWACC] = perfom.mAccSpd*0.5;
	(*this)[_MAXACC] = perfom.mAccSpd;
}




void LandsideVehicleInSim::LeaveLayoutObject( LandsideLayoutObjectInSim* pObj, LandsideResourceInSim* pDetailRes, CARCportEngine* pEngine )
{

	if(LandsideParkingLotInSim* plot = pObj->toParkLot() )
	{
		ChangeState(new State_LeaveParkingLot(this,plot), pEngine);
		return;
	}
	else if(LandsideCurbSideInSim* pCurb = pObj->toCurbSide())
	{
		IParkingSpotInSim* laneSpot = pDetailRes->toLaneSpot();
		if(laneSpot)
		{
			ChangeState(new State_LeaveCurbside(this,pCurb,laneSpot),pEngine);
			return;
		}
		ASSERT(FALSE);
	}
	else if(LandsideBusStationInSim* pStation = pObj->toBusSation() )
	{
		IParkingSpotInSim* laneSpot = pDetailRes->toLaneSpot();
		if(laneSpot)
		{
			ChangeState(new State_LeaveBusStation(this,pStation,laneSpot),pEngine);
			return;
		}
		ASSERT(FALSE);
	}	
	ChangeStateMoveToDest(pEngine);	
	return ;
}

void LandsideVehicleInSim::ArrivalLayoutObject( LandsideLayoutObjectInSim* pObj,LandsideResourceInSim* pDetailRes,CARCportEngine* pEngine )
{
	if(LandsideParkingLotInSim* pLot = pObj->toParkLot())
	{
		if(LandsideParkingLotDoorInSim* pDoor = pDetailRes->toParkLotDoor())
		{
			ChangeState(new State_EntryLotDoor(this,pDoor),pEngine);
			return;
		}
		else 
			ASSERT(FALSE);
	}
	else if(LandsideCurbSideInSim* pCurb = pObj->toCurbSide() )
	{
		ChangeState(new State_TryParkingCurbside(this,pCurb), pEngine);
		return;		
	}
	else if(LandsideExternalNodeInSim* pNode = pObj->toExtNode())
	{
		MoveToExtNode(pNode);
	}
	else if(LandsideBusStationInSim * pStation = pObj->toBusSation())
	{
		ChangeState(new State_ParkingBusStation(this,pStation),pEngine);
		return;
	}
	SuccessProcessAtLayoutObject(pObj,pDetailRes,pEngine);
}


void LandsideVehicleInSim::SuccessProcessAtLayoutObject( LandsideLayoutObjectInSim* pObj, LandsideResourceInSim* pDetailRes,CARCportEngine* pEngine )
{	
	if(ProceedToNextFcObject(pEngine)) //get next processor
	{
		LeaveLayoutObject(pObj,pDetailRes,pEngine);
	}
	else 
	{
		OnTerminate(pEngine);
	}
}

void LandsideVehicleInSim::SetCurbStragy( CurbsideStrategyStateInSim* pS )
{
	cpputil::autoPtrReset(m_pCurbStratgy,pS);
}

void LandsideVehicleInSim::SetParkLotStragy( ParkinglotStrategyStateInSim* pS )
{
	cpputil::autoPtrReset(m_pParklotStratgy,pS);
}

CurbsideStrategyStateInSim* LandsideVehicleInSim::BeginCurbStrategy( const ALTObjectID& curbName, CARCportEngine* pEngine )
{
	InputLandside* pInputLandside = pEngine->GetLandsideSimulation()->getInput();
	LandsideCurbsideSettingData* pSetting = pInputLandside->getCurbsideStrategy()->FindMatchSetting(m_TypeName,curbName);
	if(pSetting)
	{
		SetCurbStragy(new CurbsideStrategyStateInSim(pSetting));
		return m_pCurbStratgy;
	}
	return NULL;
}

ParkinglotStrategyStateInSim* LandsideVehicleInSim::BeginLotStrategy( const ALTObjectID& lotName, CARCportEngine* pEngine )
{

	return m_pParklotStratgy;
}

void LandsideVehicleInSim::FailedParkInCurb( LandsideCurbSideInSim*pCurb,CARCportEngine* pEngine )
{
	if( CurbsideStrategyStateInSim* curbStratgy = getCurbStragy() )
	{
		if( LandsideLayoutObjectInSim* pNextDest = curbStratgy->GoNextPriority(pEngine) )
		{
			ChangeDest(pNextDest);
			ChangeStateMoveToDest(pEngine);
			if(curbStratgy->isFinalState())
				EndCurbStrategy();	
			return;
		}		
	}
	CString sError;
	sError.Format(_T("Failed Parking at Curbside %s,Define Curbside Strategy"),pCurb->getName().GetIDString().GetString() );
	ShowError(sError, "Simulation Error");

	if(ProceedToNextFcObject(pEngine))
	{
		ChangeStateMoveToDest(pEngine);
	}
	else
		Terminate(curTime());
}

void LandsideVehicleInSim::ChangeDest( LandsideLayoutObjectInSim* pdest )
{
	m_pPreviousLayoutObject = m_pCurLayoutObject; 
	m_pCurLayoutObject = pdest;
}

LandsideLayoutObjectInSim* LandsideVehicleInSim::getDestLayoutObject()
{
	return m_pCurLayoutObject;
}

void LandsideVehicleInSim::EndCurbStrategy()
{
	cpputil::autoPtrReset(m_pCurbStratgy);
}

void LandsideVehicleInSim::EndLotStrategy()
{
	cpputil::autoPtrReset(m_pParklotStratgy);
}

void LandsideVehicleInSim::ChangeVehicleState( IState* nextState )
{
	ElapsedTime tTime = MAX(getLastState().time, curTime() );
	ChangeState(nextState,tTime);
}

bool LandsideVehicleInSim::InitBirth( CARCportEngine *pEngine )
{
	LandsideVehicleDescStruct& desc = m_logEntry.GetVehicleDesc();
	desc.vehicleID = m_id;
	desc.paxcount = 0;
	desc.type = getType();
	strcpy(desc.sName,m_TypeName.toString().GetString());
	OutputLandside *pOutput = pEngine->GetLandsideSimulation()->getOutput();
	m_logEntry.SetEventLog(pOutput->m_SimLogs.getVehicleEventLog());	

	LandsideVehicleProperty* pProperty = pEngine->GetLandsideSimulation()->getVehicleProp(m_TypeName);
	SetProperty(pProperty);
	return true;
}

void LandsideVehicleInSim::ShowError( const CString& sError, const CString& erroType )
{
	LandsideSimErrorShown::VehicleSimWarning(this, sError, erroType,curTime() );
}


void LandsideVehicleInSim::ChangeStateMoveToDest( CARCportEngine* pEngine/*=NULL*/ )
{	
	if(!getDestLayoutObject())
		return;

	IState* pNext = new State_MoveToDest(this,getDestLayoutObject());

	if(pEngine)
	{
		ChangeState(pNext,pEngine);
	}
	else
	{
		ChangeVehicleState(pNext);
	}
}

void LandsideVehicleInSim::SuccessParkInLotSpot( LandsideParkingSpotInSim* plot )
{
	ASSERT(false);
}

void LandsideVehicleInSim::SuccessParkInCurb( LandsideCurbSideInSim*pCurb,IParkingSpotInSim* spot )
{
	ASSERT(false);
}

void LandsideVehicleInSim::SuccessParkInBusStation(LandsideBusStationInSim* pStation,IParkingSpotInSim* spot)
{
	ASSERT(false);
}

bool LandsideVehicleInSim::ProceedToNextFcObject( CARCportEngine* pEngine )
{
	ASSERT(false); return false;
}

ResidentVehicleRoute* LandsideVehicleInSim::getNextRouteItem( ResidentVehicleRouteList* flow, CARCportEngine* pEngine )
{
	std::vector<ResidentVehicleRoute*> validItems;
	int percentAll = 0;
	for(int i=0;i<flow->GetDestCount();i++)
	{
		ResidentVehicleRoute* pRoute = flow->GetDestObjectInfo(i);
		{
			int nextObject = pRoute->GetFacilityObject()->GetFacilityID();
			LandsideLayoutObjectInSim* pLayoutObj = pEngine->GetLandsideSimulation()->GetResourceMannager()->getLayoutObjectInSim(nextObject);		
			if( pLayoutObj!=NULL && !pLayoutObj->IsHaveCapacityAndFull(this) ) //next resource do not have capacity
			{				
				percentAll += pRoute->GetPercent();
				validItems.push_back(pRoute);
			}
		}		
	}

	if(percentAll<=0)
	{
		if(!validItems.empty()){
			return validItems.at(0);
		}
		return NULL;
	}

	//
	int randP = random(percentAll);
	percentAll = 0;
	for(size_t i=0;i<validItems.size();i++){
		percentAll += validItems.at(i)->GetPercent();
		if(randP<percentAll)
			return validItems.at(i);
	}
	return NULL;
}

void LandsideVehicleInSim::DoParkingToSpot(IParkingSpotInSim* mpDest)
{
	//write parking lot 
	MobileState lastmvPt = getLastState();
	DistanceUnit distMove = lastmvPt.pos.distance(mpDest->GetParkingPos());
	double dMoveSpeed = 300;
	ElapsedTime moveTime(distMove/dMoveSpeed);


	MobileState nextState = lastmvPt;

	nextState.pRes = mpDest;
	nextState.distInRes = 0;
	nextState.dSpeed = 0;
	nextState.pos = mpDest->GetParkingPos() - mpDest->GetParkingPosDir();	
	nextState.time = lastmvPt.time +moveTime;
	nextState.isConerPt = true;

	UpdateState(nextState);		
	nextState.pos = mpDest->GetParkingPos();
	UpdateState(nextState);
}

void LandsideVehicleInSim::SuccessBirthInLotSpot( LandsideParkingSpotInSim* plot, CARCportEngine* pEngine )
{
	SuccessParkInLotSpot(plot);
}

void LandsideVehicleInSim::SkipProcessLayoutObject( LandsideLayoutObjectInSim* pObj,CARCportEngine* pEngine )
{
	ASSERT(pObj==m_pCurLayoutObject);
	if(ProceedToNextFcObject(pEngine)) //get next processor
	{
		ChangeStateMoveToDest(pEngine);
	}
	else 
	{
		OnTerminate(pEngine);
	}
}



