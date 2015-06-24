
#include "stdafx.h"
#include"MovingTrace.h"
#include "common\DynamicMovement.h"
#include "Common\ARCVector.h"

void MovingTrace::WaitAtLastPosToTime( const ElapsedTime&t )
{
	ASSERT(!IsEmpty());
	

	MobileState ptinfo;
	if(GetEndPoint(ptinfo))
	{
		MovingTrace other;
		other.AddPoint(ptinfo);
		ptinfo.time = t;
		other.AddPoint(ptinfo);
		*this  = other;
		return ;
	}
}

void MovingTrace::ResetBeginTime( const ElapsedTime& t )
{
	if(IsEmpty())
		return;

	ElapsedTime dt = t - m_path.rbegin()->time;	
	for(size_t i=0;i<m_path.size();i++){
		m_path[i].time += dt;
	}
}

void MovingTrace::Split( DistanceUnit dS, MovingTrace& t1, MovingTrace& t2 ) const
{

	double dTotalLen = GetLength();

	if(dS<dTotalLen){
		double dIndex = GetDistIndex(dS);
			
		//t1
		MobileState ptinfo;
		for(int i=0;i<=dIndex;i++){	
			ptinfo = m_path[i];
			t1.AddPoint(ptinfo);
		}
		//mid point
		double dmid = dIndex - (int)dIndex;
		if(dmid>0){
			const MobileState& p1 = m_path[(int)dIndex];
			const MobileState& p2 = m_path[(int)dIndex+1];
			InterpolateByDist(p1,p2,dmid,ptinfo);			
			t1.AddPoint(ptinfo);		
		}

		//t2
        t2.AddPoint(ptinfo);		
		for(size_t i=(int)dIndex+1;i<m_path.size();i++){
			t2.AddPoint(m_path[i]);			
		}
		return;
	}
	else
	{
		t1 = *this;
	}
}




void MovingTrace::AddPoint( const MobileState& pt )
{
	if(m_path.empty()){
		m_path.push_back(pt);
		m_distlist.push_back(0);
	}
	else{
		DistanceUnit d = m_path.back().pos.distance3D(pt.pos);
		m_path.push_back(pt);
		m_distlist.push_back(m_distlist.back()+d);
	}	
}

ElapsedTime MovingTrace::RawCalcualteTime( const ElapsedTime& tstart, double dSpd )
{
	if(IsEmpty())
		return tstart;

	ElapsedTime t = tstart;
	MobileState p = *m_path.rbegin();
	p.time=tstart;

	for(size_t i=1;i<m_path.size();i++){
		MobileState p2 = m_path.at(i);
		double dt = p2.pos.distance3D(p.pos)/dSpd;
		t+= ElapsedTime(dt);
		m_path[i].time = t;

		p = p2;
	}
	return t;
}

bool MovingTrace::GetEndPoint( MobileState& ptInfo ) const
{
	if(!IsEmpty()){
		ptInfo = m_path.back();
		return true;
	}
	return false;
}

//
bool MovingTrace::GetTimePos( const ElapsedTime&t, MobileState& ptInfo ) const
{
	for(int i=0;i<GetCount()-1;i++)
	{
		const MobileState& ptinfo1 = m_path[i];
		const MobileState& ptinfo2 = m_path[i+1];

		if(ptinfo1.time == t){
			ptInfo = ptinfo1;
			return true;
		}
		else if(ptinfo1.time<t && t<= ptinfo2.time){
			InterpolateByTime(ptinfo1,ptinfo2, t,ptInfo);
			return true;
		}		
	}
	return false;
}



bool MovingTrace::GetDistPos( DistanceUnit ds , MobileState& ptInfo ) const
{
	double dIndex = GetDistIndex(ds);
	int iIndex = static_cast<int>(dIndex);
	double dIndexOff = dIndex - iIndex;

	if(dIndex < GetCount()-1 && dIndex >= 0)
	{
		InterpolateByDist(m_path[iIndex],m_path[iIndex+1],dIndexOff,ptInfo);
		return true;		
	}
	return false;
}

double MovingTrace::GetDistIndex( DistanceUnit dS ) const
{
	double dIndex = -1;
	if(dS < m_distlist.front())
		return -1;
	
	for(size_t i=0;i<m_distlist.size()-1;i++){
		DistanceUnit d1= m_distlist[i];
		DistanceUnit d2 = m_distlist[i+1];
		if(d1 == dS )
			return i;
		if(d1<dS && dS<d2){
			return i+(dS -d1)/(d2-d1);
		}
		if(d2 == dS){
			return i+1;
		}
	}

	return m_distlist.size();

}

void MovingTrace::InterpolateByTime( const MobileState& ptinfo1, const MobileState& ptinfo2,const ElapsedTime&t, MobileState& ptInfo )
{
	double rt = (t-ptinfo1.time).getPrecisely()/(double)(ptinfo2.time - ptinfo1.time).getPrecisely();
	if(rt<=0){
		ptInfo = ptinfo1;
		return;
	}
	if(rt>=1)
	{
		ptInfo = ptinfo2;
		return;
	}

	double rs = rt;
	if(ptinfo1.dSpeed+ptinfo2.dSpeed>0){
		rs = rt*(2*ptinfo1.dSpeed+ (ptinfo2.dSpeed-ptinfo1.dSpeed)*rt)/(ptinfo1.dSpeed+ptinfo2.dSpeed);
	}


	ptInfo = ptinfo1;
	ptInfo.pos= ptinfo1.pos*(1-rs)+ptinfo2.pos*rs;
	ptInfo.dir = (ptinfo2.pos-ptinfo1.pos);
	ptInfo.dSpeed = ptinfo1.dSpeed*(1-rt)+ptinfo2.dSpeed*rt;
	ptInfo.time = t;
	ptInfo.isConerPt = false;
	if(ptinfo1.pRes == ptinfo2.pRes)
		ptInfo.distInRes = ptinfo1.distInRes*(1-rs)+ptinfo2.distInRes*rs;
}

void MovingTrace::InterpolateByDist( const MobileState& ptinfo1, const MobileState& ptinfo2,double rs, MobileState& ptInfo )
{
	if(rs<=0)
	{
		ptInfo= ptinfo1;
		return;
	}
	if(rs>=1){
		ptInfo = ptinfo2;
		return;
	}

	DynamicMovement dymv(ptinfo1.dSpeed,ptinfo2.dSpeed,1 );
	double rt = dymv.getTrate(rs);

	ptInfo = ptinfo1;
	ptInfo.pos= ptinfo1.pos*(1-rs)+ptinfo2.pos*rs;
	ptInfo.dir = (ptinfo2.pos-ptinfo1.pos);
	ptInfo.dSpeed = ptinfo1.dSpeed*(1-rt)+ptinfo2.dSpeed*rt;
	ptInfo.time.setPrecisely( static_cast<long>(ptinfo1.time.getPrecisely() *(1-rt) + ptinfo2.time.getPrecisely()*rt) );
	ptInfo.isConerPt = false;
	if(ptinfo1.pRes == ptinfo2.pRes)
		ptInfo.distInRes = ptinfo1.distInRes*(1-rs)+ptinfo2.distInRes*rs;
}

void MovingTrace::CalculateTime( const ElapsedTime& tStart, const ElapsedTime& tEnd, double dSpdStart, double dSpdEnd )
{
	if(GetCount()==0)
		return;

	DynamicMovement dymv(dSpdStart,dSpdEnd,GetLength());

	m_path.front().dSpeed =  dSpdStart;
	m_path.front().time = tStart;

	for(int i=1;i<GetCount()-1;i++)
	{
		double rt = dymv.getTrate(m_distlist[i]);
		m_path[i].dSpeed =  dSpdStart*(1-rt)+dSpdEnd*rt;
		m_path[i].time  = tStart*(1-rt)+tEnd*rt;
	}

	m_path.back().dSpeed = dSpdEnd;
	m_path.back().time  = tEnd;
}

DistanceUnit MovingTrace::GetLength() const
{
	if(!m_distlist.empty())
		return m_distlist.back();
	return 0;
}


ElapsedTime MobileState::moveTime( const MobileState& nextPt) const
{
	double avgSpd = (dSpeed + nextPt.dSpeed)*0.5;
	double dist =0;
	if(pRes==nextPt.pRes){
		dist = abs(nextPt.distInRes - distInRes);
	}
	else{
		dist = pos.distance3D(nextPt.pos);
	}
	if(avgSpd>0){
		return ElapsedTime(dist/avgSpd);
	}
	return ElapsedTime(0L);
}

MobileState::MobileState()
{
	pRes = 0;
	dSpeed = 0;
	distInRes = 0;
	isConerPt = true;
	pathIndex = 0;
	dAcc = 0;
}
