#include "StdAfx.h"
#include ".\paxonboardfreemovinglogic.h"
#include "SteppableSchedule.h"
#include "ARCportEngine.h"
#include "PaxOnboardBaseBehavior.h"
#include "OnboardFlightInSim.h"
#include <common/Range.h>
#include <iosfwd>
#include <common/DistanceLineLine.h>
#include "ScheduleStepEvent.h"
#include "EntryPointInSim.h"
#include "OnboardSeatInSim.h"
#include "PAX.H"

const static  ElapsedTime Interval= ElapsedTime(0.2); 
#define PI 3.141592653589793f
//
//bool bConflictWithPax(const CPoint2008& pt, const std::vector<Person*>& vPaxList)const
//{
//	bool bRet = false;
//	for(int i=0;i<(int)vPaxList.size();i++)
//	{
//		Person* pPax = vPaxList.at(i);
//
//	}
//}

void PaxOnboardFreeMovingLogic::Repeat( CARCportEngine& engine, const ElapsedTime& nextTime )
{
	SteppableSchedule::Key key;
	key.time = nextTime;
	ScheduleStepEvent* pnext  = (new ScheduleStepEvent(this,nextTime));
	pnext->addEvent();
	//engine.m_stepSchedule.scheduleOnce(key,this);	
}

void PaxOnboardFreeMovingLogic::step( CARCportEngine& state )
{
	switch(m_curState)
	{
	case _movingInSeg:
		OnMovingInPath(state);
		break;
	case _holdIn:
		OnHoldInPath(state);
		break;
	case _endofRoute:
		OnEndofPath(state);
		break;
	case _movingToSeg:
		OnMovingToPointInSeg(state);
		break;
	}
	//ASSERT(FALSE);
	//Repeat(state,m_curTime+ElapsedTime(1L));
}



void PaxOnboardFreeMovingLogic::ResetRoute( PaxOnboardRoute& route )
{
	m_Route = route;
	m_dCurDistInSeg = 0;
	m_iCurrItemIdx = 0;
	m_dCurSpanInSeg = 0;
}



void PaxOnboardFreeMovingLogic::OnMovingInPath(CARCportEngine& engine)
{
 	
	if( m_iCurrItemIdx>=m_Route.GetCount())
	{
		SetState(_endofRoute);
		return Repeat(engine,m_curTime);		
	}	

	const PaxOnBoardRouteSeg& curItem = m_Route.ItemAt(m_iCurrItemIdx);
	CPoint2008 curPos = getCurPos();

	//DistanceUnit dist;//
	//ASSERT(curItem.PointInSeg(getCurPos(),dist));
	if(curItem.m_pElement->GetType() == COnboardCorridor::TypeString) //step in corridor
	{
		CPoint2008 pL,pR;
		DistanceUnit stepDist = getStepDist();
		DistanceUnit nextDistInSeg = m_dCurDistInSeg+stepDist;	
		DistanceUnit nextSpanInSeg = m_dCurSpanInSeg;
		CPoint2008 nextPos;
		if( ResolveConflictInCorridor(stepDist,nextPos,nextDistInSeg,nextSpanInSeg)) //( curItem.GetSpan(nextDistInSeg,pL,pR) )
		{
			//CLine2008 line(pL,pR);
			//DistancePointLine3D distPtLine(curPos,line);
			//distPtLine.GetSquared();
			//CPoint2008 nextPos = (pL+pR)*0.5;//distPtLine.m_clostPoint;
			nextPos.z =  curItem.GetHeight(nextDistInSeg);

			DistanceUnit dLen = nextPos.distance(curPos);
			ElapsedTime timeToNext  = ElapsedTime(dLen/stepDist*(double)Interval);
			ElapsedTime nextTime = m_curTime + timeToNext;
			ElapsedTime nextIntervalTime = m_curTime + Interval;

			if(dLen>stepDist)
			{
				double dr = stepDist/dLen;
				CPoint2008 pos = curPos*(1-dr) + nextPos*dr;
				DistanceUnit dist = m_dCurDistInSeg*(1-dr)+nextDistInSeg*dr;
				DistanceUnit span = m_dCurSpanInSeg*(1-dr)+nextSpanInSeg*dr;
				StepTo(pos,nextIntervalTime);
				StepTo(m_iCurrItemIdx,dist,span);
			}
			else
			{
				StepTo(nextPos,nextIntervalTime);
				StepTo(m_iCurrItemIdx,nextDistInSeg,nextSpanInSeg);
			}			
			//UpdateNextPos(nextPos);	
		}
		else
		{
			StepTo(m_iCurrItemIdx+1,0,0);				
		}
	}	
	else //steps in stair /escalator
	{		

		CPath2008 path;
		if( curItem.GetPath(path) )
		{
			for(int i=0;i<path.getCount();i++)
			{
				const CPoint2008& nextPos = path[i];
				DistanceUnit dLen = nextPos.distance(curPos);
				ElapsedTime nextTime = m_curTime + ElapsedTime(dLen/getSpeed());
				StepTo(nextPos,nextTime);
			}
			
		}	

		StepTo(m_iCurrItemIdx+1,0,0);
	}	

	Repeat(engine,m_curTime);	
}

void PaxOnboardFreeMovingLogic::StartMove(const ElapsedTime& t, PaxOnboardRoute& route,CARCportEngine& state )
{
	
	m_curTime = t;	
	m_iCurrItemIdx = 0;
	ResetRoute(route);
	m_curState = _movingInSeg;	

	//if(m_Parent->GetOnElement()

	step(state);
}

void PaxOnboardFreeMovingLogic::OnHoldInPath( CARCportEngine& state )
{

}

void PaxOnboardFreeMovingLogic::OnEndofPath( CARCportEngine& state )
{
	CPoint2008 curPos = getCurPos();
	CPoint2008 dir =  CPoint2008(m_Route.GetEndPoint() - curPos);
	dir.z =0;
	DistanceUnit dLenLeft = dir.length();
	dir.length(1);

	const double dSpeed = getSpeed();
	double dStep =  dSpeed*(double)Interval;
	ElapsedTime nextTime = m_curTime + Interval;

	/*std::vector<Person*> vPaxConcern;
	std::vector<Person*> vBlockPax;

	GetPaxInConcern(vPaxConcern,vBlockPax);

	double dFarDist;
	if( CheckConflictInDir(dir,vPaxConcern,dFarDist) )
	{
		dStep = min(dFarDist,dStep);
		dStep = max(0,dStep);
	}*/

	CPoint2008 nextPos;
	if(dLenLeft<=dStep)
	{
		nextPos = m_Route.GetEndPoint();
		StepTo(nextPos,nextTime);
		EndMove(nextTime);
	}
	else
	{
		nextPos =dir*dStep+curPos;
		StepTo(nextPos,nextTime);	
		Repeat(state,nextTime);
	}	
}



double PaxOnboardFreeMovingLogic::getSpeed() const
{
	return m_Parent->m_pPerson->getSpeed();
}

CPoint2008 PaxOnboardFreeMovingLogic::getCurPos() const
{
	return CPoint2008(m_Parent->getPoint().x,m_Parent->getPoint().y,m_Parent->getPoint().z);
}

void PaxOnboardFreeMovingLogic::EndMove( const ElapsedTime& t )
{
	SetState(_null);//move l
	m_Parent->m_pPerson->setState(OnboardFreeMoveEnd);
	m_Parent->GenerateEvent(t);
}

void PaxOnboardFreeMovingLogic::SetState( MoveState s ,bool bForce)
{
	if(s!=m_curState && !bForce)
	{
		m_lastStateTime = m_curTime;
		m_curState = s;
	}
}

PaxOnboardFreeMovingLogic::PaxOnboardFreeMovingLogic( void )
{
	m_Parent = NULL;
	m_curState = _null;
	m_dRadius = PAXRADIUS*2;
	m_dMinRad = PAXRADIUS;
	m_iCurrItemIdx = 0;
}

void PaxOnboardFreeMovingLogic::UpdateNextPos( const CPoint2008& nextPos )
{
	//get the distance in route
	bool bInNextItem = false;
	if(m_iCurrItemIdx+1<m_Route.GetCount())
	{
		PaxOnBoardRouteSeg& nextItem = m_Route.ItemAt(m_iCurrItemIdx+1);
		DistanceUnit distInSeg;
		if(nextItem.PointInSeg(nextPos,distInSeg))
		{
			m_iCurrItemIdx++;
			m_dCurDistInSeg = max(0,distInSeg);
			bInNextItem = true;
		}
	}
	if(!bInNextItem)
	{
		const PaxOnBoardRouteSeg& curItem = m_Route.ItemAt(m_iCurrItemIdx);

		double distInSeg;
		double extra;
		if(curItem.PointInSeg(nextPos,distInSeg) && !curItem.IsOutofSeg(distInSeg,extra) ) //point still in the current item
		{			
			m_dCurDistInSeg = distInSeg;
		}
		else
		{
			m_iCurrItemIdx++;
			m_dCurDistInSeg= 0;
			if(m_iCurrItemIdx<m_Route.GetCount())
			{
				m_nextPointInSeg  = m_Route.ItemAt(m_iCurrItemIdx).GetFirstPointInSeg();
				SetState(_movingToSeg);
			}
			else
			{
				SetState(_endofRoute);
			}
		}
		
	}	
}

void PaxOnboardFreeMovingLogic::OnMovingToPointInSeg( CARCportEngine& state )
{
	CPoint2008 curPos = getCurPos();
	CPoint2008 dir = CPoint2008(m_nextPointInSeg - curPos);
	dir.setZ(0);
	DistanceUnit dLenLeft = dir.length();
	dir.length(1);

	const double dSpeed = getSpeed();
	double dStep =  dSpeed*(double)Interval;

	
	CPoint2008 nextPos;
	if(dLenLeft<dStep)
	{
		nextPos = m_nextPointInSeg;
		SetState(_movingInSeg);
	}
	else
		nextPos =dir*dStep+curPos;

	ElapsedTime nextTime = m_curTime + Interval;
	StepTo(nextPos,nextTime);	
	Repeat(state,nextTime);
}
//
//static bool IsBlockPax(Person* pPax)
//{
//	if(pPax->getOnboardBehavior())
//	{
//		PaxOnboardFreeMovingLogic* pOtherFreeMoving = pPax->getOnboardBehavior()->GetFreeMovinglogic();
//		if(pOtherFreeMoving)
//		{
//			if(pOtherFreeMoving->getCurState() == PaxOnboardFreeMovingLogic::_movingInSeg )
//			{
//
//			}			
//		}
//	}
//	return true;
//}
//
//void PaxOnboardFreeMovingLogic::GetPaxInConcern( const CPoint2008& dir, std::vector<Person*>& vPaxList , std::vector<Person*>& vblockpax)
//{
//	const ARCVector3& pos = m_Parent->getPoint();
//	vPaxList.clear();
//	std::vector<Person*> paxOnFlight;
//	m_Parent->GetOnBoardFlight()->GetPaxListInFlight(paxOnFlight);
//	for(int i=0;i<(int)paxOnFlight.size();i++)
//	{
//		Person* pPax = paxOnFlight[i];
//		if(pPax == m_Parent->m_pPerson)
//			continue;
//		ARCVector3 pt;
//		if( pPax->getOnboardPoint(pt) )
//		{			
//			if(pt.DistanceTo(pos)<200)
//			{
//				if( (pt-pos).dot(dir) > 0.2 )
//				{
//					vPaxList.push_back(pPax);
//					if(IsBlockPax(pPax))
//						vblockpax.push_back(pPax);		
//
//				}
//			}		
//		}
//	}
//	
//}
//
//bool PaxOnboardFreeMovingLogic::CheckConflictWithPax(const CPoint2008& dir,Person* person, CPoint2008& dirNext, DistanceUnit& distToPax ) const
//{
//	//double dR = m_dRadius*2; //need to add the other person's radius
//	//const ARCVector3& pos = m_Parent->getPoint();
//	//ARCVector3 otherPos;
//	//person->getOnboardPoint(otherPos);
//
//	//ARCVector2 vdir =(otherPos-pos).xy();
//	//double dAngle = vdir.AngleFromCoorndinateX();
//	//double dDist = vdir.Length();
//	//if(dDist<=0)
//	//{
//	//	dirAnlgeRange.m_minor =0;
//	//	dirAnlgeRange.m_max = 360;
//	//}
//	//else
//	//{
//	//	double dCosA = min(1,dR/dDist);
//	//	dCosA = max(-1,dCosA);
//	//	double dOffsetAngle = ARCMath::RadiansToDegrees(acos(dCosA));
//	//	dirAnlgeRange.m_minor = dAngle+dOffsetAngle;
//	//	dirAnlgeRange.m_max = dAngle-dOffsetAngle+360;
//	//}
//	return false;	
//}

bool PaxOnboardFreeMovingLogic::CheckConflictWithPax( const CPoint2008& dir,Person* person,bool bLeft, CPoint2008& dirNext, DistanceUnit& distToPax ) const
{
	return false;	

}


void PaxOnboardFreeMovingLogic::StepTo( const CPoint2008& pos, const ElapsedTime&t )
{
		//CPoint2008 curPos = getCurPos();
	//if(curPos.distance3D(pos)>ARCMath::EPSILON)
	{
		//m_Parent->writeLog(t,false);
		m_Parent->setLocation(pos);
		m_Parent->writeLog(t,false);		
	}
	m_curTime = t;
	//DebugLog();
}

void PaxOnboardFreeMovingLogic::StepTo( int item, DistanceUnit distInitem,DistanceUnit spanInitem )
{
	m_iCurrItemIdx = item;
	m_dCurDistInSeg = distInitem;	
	m_dCurSpanInSeg = spanInitem;
	if(m_iCurrItemIdx<m_Route.GetCount() && m_Parent )
	{
		//PaxOnboardBaseBehavior* pPaxOnboard = m_Parent;
		PaxOnBoardRouteSeg& item =m_Route.ItemAt(m_iCurrItemIdx);
		//m_Parent->SetOnElement( item.m_pElement, item.DistInElement(m_dCurDistInSeg) );
	}
}

static CString strState[]=
{
	_T("_holdIn"),      //hold in route
	_T("_movingInSeg"), //moving along the path
	_T("_movingToSeg"), //moving directly to a dest point
	_T("_endofRoute"),		//
	_T("_null"), //not active
};

void PaxOnboardFreeMovingLogic::DebugLog()
{
	CString debugfileName;
	debugfileName.Format( "c:\\onboardpax\\%d.txt",m_Parent->m_pPerson->getID());

	//log to flight file
	std::ofstream outfile;
	outfile.open(debugfileName,std::ios::app);
	outfile << m_curTime <<"," << long(m_curTime) << '\t';

	CPoint2008 curPos = getCurPos();
	CString sPoint;
	sPoint.Format(_T("(%.2f,%.2f,%.2f)"),curPos.x,curPos.y,curPos.z);
	outfile << sPoint<< '\t';

	outfile<< strState[m_curState]<< '\t';

	//if(m_Parent&&m_Parent->GetOnElement() && m_Parent->GetOnElement()->GetType()==COnboardCorridor::TypeString)
	//{
	//	OnboardCorridorInSim* pCoridor = (OnboardCorridorInSim*)m_Parent->GetOnElement();
	//	CString strname = pCoridor->m_pCorridor->GetIDName().GetIDString();
	//	outfile<<strname<<"  Span:"<<m_dCurSpanInSeg<<"  Dist:"<<m_Parent->GetDistOnElement()<<'\t';
	//}
	//else
	//{

	//}

	outfile<<std::endl;
	outfile.close();
}


bool PaxOnboardFreeMovingLogic::CheckConflictInDir( const CPoint2008& dir, Person*person, double& dfardist ) const
{
	double dR = m_dRadius*2; //need to add the other person's radius
	const ARCVector3& pos = m_Parent->getPoint();
	ARCVector3 otherPos;
	person->getOnboardPoint(otherPos);

	ARCVector2 vdistdir =(otherPos-pos).xy();	
	double dDist = vdistdir.Length();
	vdistdir.SetLength(1);

	if(dDist<=0)
	{
		return false;
	}
	else
	{
		
		double dSinA = min(1,dR/dDist);
		dSinA = max(-1,dSinA);
		ARCVector2 vdir(dir.x,dir.y);
		vdir.SetLength(1);

		if(vdistdir.dot(vdir)<0)
			return false;
		else if( dSinA <= abs(vdistdir.cross(vdir)) )
		{
			return false;
		}
		else
		{
			dfardist = dDist - dR;
			return true;
		}
	
	}
	return false;	
}

bool PaxOnboardFreeMovingLogic::CheckConflictInDir( const CPoint2008& dir, const std::vector<Person*>& vPaxList, double& dfardist ) const
{
	bool bConf = false;
	for(int i=0;i<(int)vPaxList.size();i++)
	{
		double ddist;
		if( CheckConflictInDir(dir,vPaxList.at(i),ddist))
		{
			if(!bConf)
			{
				dfardist = ddist;
				bConf =true;
			}
			else
			{
				dfardist = min(dfardist,ddist);
			}			
		}
	}
	return bConf;
}
void PaxOnboardFreeMovingLogic::HoldForStep( CARCportEngine& engine )
{
	m_curTime += Interval;
}

DistanceUnit PaxOnboardFreeMovingLogic::getStepDist() const
{
	return getSpeed()* (double)Interval;
}

bool PaxOnboardFreeMovingLogic::FixDir( const CPoint2008& orginDir, CPoint2008& destDir, DistanceUnit& distLen )const
{	
	if(AvoidBound(orginDir,destDir,distLen))
		return true;
	return false;
}

bool PaxOnboardFreeMovingLogic::AvoidBound( const CPoint2008& orginDir, CPoint2008& destDir, DistanceUnit& distLen ) const
{
	const PaxOnBoardRouteSeg& curItem = m_Route.ItemAt(m_iCurrItemIdx);
	OnboardCorridorInSim* pCoridor = (OnboardCorridorInSim*)curItem.m_pElement;
	CPoint2008 curPos = getCurPos();
	DistanceUnit distStep = getStepDist();

	CPoint2008 dirIter = orginDir;	
	do
	{		
		DistanceUnit dist;
		CPoint2008 nextDir;
		if( pCoridor->bRayIntesect(curPos,dirIter,dist,nextDir) )
		{			
			if(dist>m_dRadius )
			{
				if( dist<m_dRadius+distStep)
				{
					distLen = dist - m_dRadius;
					destDir = nextDir;
					return true;
				}
				else
					return false; //no need to avoid bound
			}
			else
			{
				dirIter = nextDir; //need loop
			}
		}
		else
		{
			return false;
		}

	}while(true);
	return false;

}

OnboardAircraftElementInSim* PaxOnboardFreeMovingLogic::getCurElement() const
{
	if(m_iCurrItemIdx<m_Route.GetCount() && m_iCurrItemIdx>=0)
	{
		return m_Route.ItemAt(m_iCurrItemIdx).m_pElement;
	}
	return NULL;
}

DistanceUnit PaxOnboardFreeMovingLogic::getCurDistInElement() const
{
	if(m_iCurrItemIdx<m_Route.GetCount()  && m_iCurrItemIdx>=0)
	{
		return m_Route.ItemAt(m_iCurrItemIdx).DistInElement(m_dCurDistInSeg);
	}
	return 0;
}

bool PaxOnboardFreeMovingLogic::IsDirPositiveInElement() const
{
	if(m_iCurrItemIdx<m_Route.GetCount() && m_iCurrItemIdx>=0)
	{
		return m_Route.ItemAt(m_iCurrItemIdx).IsPositive();
	}
	return true;
}


struct ConflictPaxInfo
{
	Person* pPax;
	int IsBlock; //0 not , 1 static , 2 conflict
	
	DistanceUnit distInSeg;
	DistanceUnit distInSpan; //dist in the span
	CPoint2008 loc;
	DistanceUnit minSpan; //left <0
	DistanceUnit maxSpan; //right >0
	CPoint2008 spanL;
	CPoint2008 spanR;

	bool operator<(const ConflictPaxInfo& other)const{ return distInSeg<other.distInSeg; }
};


bool PaxOnboardFreeMovingLogic::ResolveConflictInCorridor( double dLen, CPoint2008& nextPoint, DistanceUnit& distInSeg,DistanceUnit& spanInSeg )
{
	std::vector<ConflictPaxInfo> m_vConflictPax;

	const PaxOnBoardRouteSeg& curItem = m_Route.ItemAt(m_iCurrItemIdx);
	CPoint2008 curPos = getCurPos();
	bool haveNegDirPax = false;
	//get pax in ahead
	std::vector<Person*> paxOnFlight;
	m_Parent->GetOnBoardFlight()->GetPaxListInFlight(paxOnFlight);
	for(int i=0;i<(int)paxOnFlight.size();i++)
	{
		Person* pPax = paxOnFlight.at(i);
		if(pPax==m_Parent->m_pPerson)
			continue;
		
		ARCVector3 location;
		if( pPax &&  pPax->getOnboardPoint(location) )
		{
			CPoint2008 otherPos = CPoint2008(location.x,location.y,location.z);
			
			PaxOnboardBaseBehavior* pOtherPaxOnboard = pPax->getOnboardBehavior();
			if(!pOtherPaxOnboard)
				continue;
			//PaxOnboardFreeMovingLogic* pOtherLogic = pOtherPaxOnboard->GetFreeMovinglogic();
			//
			//if( pOtherPaxOnboard->GetOnElement() == curItem.m_pElement ) // in the same element
			//{
			//	DistanceUnit otherDistInSeg = curItem.DistInSeg(pOtherPaxOnboard->GetDistOnElement());
			//	if( abs(otherDistInSeg-distInSeg)< m_dRadius )
			//	{
			//		ConflictPaxInfo conflict;
			//		conflict.distInSeg = otherDistInSeg;
			//		conflict.pPax = pPax;
			//		conflict.loc = otherPos;

			//		if(pPax->getState()== OnboardFreeMoving)
			//		{
			//			if( curItem.IsPositive()!=pOtherLogic->IsDirPositiveInElement())
			//			{
			//				conflict.IsBlock = 2;
			//				haveNegDirPax = true;
			//			}
			//			else
			//				conflict.IsBlock = 0;
			//		}
			//		else
			//			conflict.IsBlock = 1;
			//		

			//		CPoint2008 spC,spL, spR;
			//		if(curItem.GetSpan(otherDistInSeg,spC,spL,spR))
			//		{
			//			conflict.minSpan = -spL.distance(spC);
			//			conflict.maxSpan = spR.distance(spC);
			//			DistanceUnit distToSpL = spL.distance(otherPos);
			//			DistanceUnit distToSpR = spR.distance(otherPos);
			//			double dRat = distToSpL/(distToSpL+distToSpR);
			//			conflict.distInSpan = conflict.minSpan*(1-dRat)+conflict.maxSpan*dRat;
			//			conflict.spanL = spL;
			//			conflict.spanR = spR;
			//			m_vConflictPax.push_back(conflict);
			//		}

			//	}	
			//}			
		
		}
	}


	//get conflict seat entry 
	std::vector<EntryPointInSim* >& seatEntries = m_Parent->GetOnBoardFlight()->GetSeatEntryPoint();
	for(int i=0;i<(int)seatEntries.size();i++)
	{
		EntryPointInSim* pEntry = seatEntries[i];
		if(!pEntry->IsAvailable())
			continue;

		if(curItem.m_pElement != pEntry->GetCorridor())
			continue;
	
		if(m_Parent->getSeat()->GetEntryPoint() == pEntry)
			continue;

		DistanceUnit entrydistInSeg = curItem.DistInSeg(pEntry->GetDistance());
		
		DistanceUnit exraD; 
		if( curItem.IsOutofSeg(distInSeg, exraD) )
			continue;

		ARCVector3 loc;
		if(!pEntry->GetLocation(loc))
			continue;
		CPoint2008 entrPos(loc.x,loc.y,loc.z);

		CPoint2008 spC,spL, spR;
		if(curItem.GetSpan(entrydistInSeg,spC,spL,spR))
		{
			ConflictPaxInfo conflict;
			conflict.pPax = NULL;
			conflict.distInSeg = entrydistInSeg;
			conflict.IsBlock = 1;
			conflict.minSpan = -spL.distance(spC);
			conflict.maxSpan = spR.distance(spC);
			DistanceUnit distToSpL = spL.distance(entrPos);
			DistanceUnit distToSpR = spR.distance(entrPos);
			double dRat = distToSpL/(distToSpL+distToSpR);
			conflict.distInSpan = conflict.minSpan*(1-dRat)+conflict.maxSpan*dRat;
			conflict.spanL = spL;
			conflict.spanR = spR;
			//m_vConflictPax.push_back(conflict);
		}
	}

	std::sort(m_vConflictPax.begin(),m_vConflictPax.end());
	//////////////////////////////////////method 1////////////////////////////////////
	if(1)
	{		
		ARCVector2 vPosInSeg = ARCVector2(m_dCurSpanInSeg,m_dCurDistInSeg+getStepDist());

		//conflict resolve
		ARCVector2 resolveConflict(0,0);
		for(int i=0;i<(int)m_vConflictPax.size();i++)
		{
			ConflictPaxInfo& conflict = m_vConflictPax[i];
			ARCVector2 vOtherPosInSeg  = ARCVector2(conflict.distInSpan,conflict.distInSeg);
			ARCVector2 dirToOther = vOtherPosInSeg - vPosInSeg;

			DistanceUnit distToOther = dirToOther.Length();

			if(distToOther< m_dMinRad*2)//has conflict need to resolve
			{
				dirToOther.SetLength(distToOther-m_dMinRad*2);
				resolveConflict+= dirToOther;
				//vPosInSeg+=dirToOther;
				//break;
			}					
		}

		vPosInSeg+=resolveConflict;
		//get next pos
		DistanceUnit nextDistInSeg = vPosInSeg.y;
		DistanceUnit nextSpan = vPosInSeg.x;
		CPoint2008 spC,spL, spR;
		if(curItem.GetSpan(nextDistInSeg,spC,spL,spR))
		{
			DistanceUnit minSpan = -spL.distance(spC);
			DistanceUnit maxSpan = spR.distance(spC);
			nextSpan = max(minSpan+m_dMinRad,nextSpan);
			nextSpan = min(maxSpan-m_dMinRad,nextSpan);
			double dRat = (nextSpan-minSpan)/(maxSpan-minSpan);			
			nextPoint = spL*(1-dRat)+spR*dRat;
			distInSeg = nextDistInSeg;
			spanInSeg = nextSpan;
			return true;
		}
	}
	//////////////////////////method 2////////////////////////////////////////////////
	if(0)
	{
		DistanceUnit nextSpan = 0;
		DistanceUnit nextDistInSeg = dLen+m_dCurDistInSeg;
		for(int i=0;i<(int)m_vConflictPax.size();i++)
		{
			ConflictPaxInfo& conflict = m_vConflictPax[i];
			if(conflict.distInSeg<=m_dCurDistInSeg)
				continue;
			//get free space of next dist			
			if(conflict.IsBlock==0)
			{
				/*DistanceUnit minL,maxL;
				DistanceUnit minR,maxR;
				minL = conflict.minSpan;
				maxL = conflict.distInSpan-m_dMinRad;
				minR = conflict.distInSpan+m_dMinRad;
				maxR = conflict.maxSpan;
				if(haveNegDirPax)
					maxR-= m_dMinRad;

				if(maxL-minL>m_dMinRad*2)
				{
					nextSpan = (maxL+minL)*0.5;
					nextDistInSeg = conflict.distInSeg;
					break;
				}
				else if(maxR-minR>m_dMinRad*2)
				{
					nextSpan = (maxR+minR)*0.5;
					nextDistInSeg = conflict.distInSeg;
					break;
				}
				else*/
				{
					nextSpan = conflict.distInSpan;
					nextDistInSeg = max(0,conflict.distInSeg-m_dMinRad*2);
					break;
				}
			}
			if(conflict.IsBlock==1) //no block ,follow it
			{
				DistanceUnit minL,maxL;
				DistanceUnit minR,maxR;
				minL = conflict.minSpan;
				maxL = conflict.distInSpan-m_dMinRad;
				minR = conflict.distInSpan+m_dMinRad;
				maxR = conflict.maxSpan;
				if(haveNegDirPax)
					maxR-= m_dMinRad;			

				if(maxL-minL>maxR-minR)
				{
					nextSpan = minL;
				}
				else
				{
					nextSpan = maxR;
				}				
			}
		}		
		CPoint2008 spC,spL, spR;
		if(curItem.GetSpan(nextDistInSeg,spC,spL,spR))
		{
			DistanceUnit minSpan = -spL.distance(spC);
			DistanceUnit maxSpan = spR.distance(spC);
			nextSpan = max(minSpan+m_dMinRad,nextSpan);
			nextSpan = min(maxSpan-m_dMinRad,nextSpan);
			double dRat = (nextSpan-minSpan)/(maxSpan-minSpan);			
			nextPoint = spL*(1-dRat)+spR*dRat;
			distInSeg = nextDistInSeg;
			spanInSeg = nextSpan;
			return true;
		}
	}
	return false;
}

bool PaxOnboardFreeMovingLogic::CanMoveOnCorridor( OnboardCorridorInSim* pCor, DistanceUnit distInCor, const ARCVector3& pos )
{
	std::vector<Person*> paxOnFlight;
	m_Parent->GetOnBoardFlight()->GetPaxListInFlight(paxOnFlight);
	for(int i=0;i<(int)paxOnFlight.size();i++)
	{
		Person* pPax = paxOnFlight.at(i);
		if(pPax==m_Parent->m_pPerson)
			continue;

		ARCVector3 location;
		if( pPax &&  pPax->getOnboardPoint(location) )
		{
			//CPoint2008 otherPos = CPoint2008(location.x,location.y,location.z);

			PaxOnboardBaseBehavior* pOtherPaxOnboard = pPax->getOnboardBehavior();
			if(!pOtherPaxOnboard)
				continue;
			//PaxOnboardFreeMovingLogic* pOtherLogic = pOtherPaxOnboard->GetFreeMovinglogic();

			//if( pOtherPaxOnboard->GetOnElement() == pCor ) // in the same element
			//{				
			//	if( pos.DistanceTo(location)< m_dMinRad*2 )
			//	{					
			//		return false;
			//	}	
			//}			

		}
	}
	return true;
}

ElapsedTime PaxOnboardFreeMovingLogic::GetInterval() const
{
	return Interval;
}