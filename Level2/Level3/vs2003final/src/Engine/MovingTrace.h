#pragma once
#include "common\Point2008.h"
#include "common\elaptime.h"
#include "Common\Path2008.h"
#include "Common\ARCVector.h"
#include "Landside\LandsidePath.h"

class LandsideResourceInSim;
class MobileState : public LandsidePosition
{
public:
	//basic info
	double dAcc;  //acc speed  in cm/s^2
	double dSpeed; //speed
	ElapsedTime time;  //time
 
	//extra info
	ARCVector3 dir;
	bool isConerPt;
	int pathIndex;

	MobileState();
	LandsideResourceInSim* getLandsideRes()const{ return pRes; }
	ElapsedTime moveTime(const MobileState& other)const;
};


//Mobile State List

class MovingTrace
{
public:
	inline int GetCount()const { return (int)m_path.size(); } 
	void Clear(){	m_path.clear();m_distlist.clear(); }
	//add new point
	void AddPoint(const MobileState& pt);

	//return the pos index at the time
	bool GetTimePos(const ElapsedTime& t,  MobileState& ptInfo)const;

	//return the pos
	bool GetDistPos(DistanceUnit ds , MobileState& ptInfo)const;

	//is empty
	bool IsEmpty()const{ return m_path.empty(); }

	MobileState& back(){ return m_path.back(); }
	const MobileState& back()const{ return m_path.back(); }

	MobileState& front(){ return m_path.front(); }
	const MobileState& front()const{ return m_path.front(); }

	//get content of last moving point
	bool GetEndPoint( MobileState& ptinfo)const;

	//line 
	DistanceUnit GetLength()const;

	//
	ElapsedTime RawCalcualteTime(const ElapsedTime& tstart, double dSpd); //calculate the times

	void CalculateTime( const ElapsedTime& tStart, const ElapsedTime& tEnd,double dSpdStart, double dSpdEnd);
	

	//split this trace to two in the middle path
	void Split(DistanceUnit dS, MovingTrace& t1, MovingTrace& t2)const;

	//offset time
	void ResetBeginTime(const ElapsedTime& t);
	//clear all except the last one
	void WaitAtLastPosToTime(const ElapsedTime&t);
	
	//
	double GetDistIndex(DistanceUnit dS)const;



	static void InterpolateByTime(const MobileState& ptinfo1, const MobileState& ptinfo2,const ElapsedTime&t, MobileState& ptInfo);

	static void InterpolateByDist(const MobileState& ptinfo1, const MobileState& ptinfo2,double rs, MobileState& ptInfo);



	MobileState & at(int idx){ return m_path.at(idx); }
	const MobileState& at(int idx)const { return m_path.at(idx); }
public:
	void UpdateDistlist();
	std::vector<MobileState> m_path;  //points info of the path
	std::vector<DistanceUnit> m_distlist; //dist list of 
};
