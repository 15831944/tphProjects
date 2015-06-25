#pragma once
#include "OnboardAircraftElementInSim.h"
#include "EntryPointInSim.h"

class COnboardCorridor;
class ARCVector3;
class CPath2008;
class CPoint2008;
class OnboardCorridorInSim;
class OnboardFlightInSim;

#include <common/Point2008.h>

class CorridorPoint 
{
public:
	CorridorPoint()
		:m_dDistanceFromStart(0)
		,m_nSegment(-1)
		,m_pIntersectCorridorInSim(NULL)
		,m_dInterCorridorDist(0.0)
		,m_pEntryPointInSim(NULL)
	{

	}
	~CorridorPoint()
	{

	}

	void SetDistance(double dDistance){m_dDistanceFromStart = dDistance;}
	double GetDistance()const {return m_dDistanceFromStart;}

	void SetIntersectionCorridor(OnboardCorridorInSim* pCorridorInSim){m_pIntersectCorridorInSim = pCorridorInSim;}
	OnboardCorridorInSim* GetIntersectionCorridor()const {return m_pIntersectCorridorInSim;}

	void SetInterCorridorDist(double dDistance){m_dInterCorridorDist = dDistance;}
	double GetInterCorridorDist()const {return m_dInterCorridorDist;}

	void SetEntryPointInSim(EntryPointInSim* pEntryPointInSim){m_pEntryPointInSim = pEntryPointInSim;}
	EntryPointInSim* GetEntryPointInSim()const {return m_pEntryPointInSim;}

	void SetSegment(int nSegment){m_nSegment = nSegment;}
	int GetSegment()const {return m_nSegment;}	
private:
	double m_dDistanceFromStart;
	int m_nSegment;
	OnboardCorridorInSim* m_pIntersectCorridorInSim;
	double m_dInterCorridorDist;
	EntryPointInSim* m_pEntryPointInSim;
};

typedef std::vector<CorridorPoint> CorridorPointList;


class OnboardCorridorInSim : public OnboardAircraftElementInSim
{
public:
	OnboardCorridorInSim(COnboardCorridor *pCorridor,OnboardFlightInSim* pFlightInSim);
	~OnboardCorridorInSim(void);

	virtual CString GetType()const;
	virtual int GetResourceID() const;

	struct  CoridorSpanItem
	{
		double dIndexInCenter;
		CPoint2008 ptSide;
		CPoint2008 ptCenter;
	};

	class CoridorSpans
	{
	public:
		std::vector<CoridorSpanItem> m_leftSpans;
		std::vector<CoridorSpanItem> m_rightSpans;
		void Clear(){ m_leftSpans.resize(0); m_rightSpans.resize(0); }
		bool GetSpan(double cindex,CPoint2008& pL, CPoint2008& pR)const;		
		bool GetNextSpan(double cindex, CPoint2008& pL, CPoint2008& pR, double& nextIndex)const;
		bool GetNextSpanReverse(double cindex,CPoint2008& pL, CPoint2008& pR, double& nextIndex)const;
	};

public:
	int GetIntersectionOnSegment(int nSegment,CorridorPointList& ptList);
	//calculate the corridor intersection with other corridor
	void CalculateIntersection(OnboardFlightInSim* pOnboarFlightInSim);
	double GetShortestDistancePoint(const CPoint2008& pt,CPoint2008& destPt,const CPath2008& path,int& nSegment,double& dRealDistance);
	void InsertIntersectionNode(int nSegment, double dRealDistance,EntryPointInSim* pEntryPoint = NULL);


	bool bRayIntesect(const CPoint2008& pt, const CPoint2008& dir,DistanceUnit& dLen, CPoint2008& avoiddir )const;

	CPoint2008 GetDistancePoint(double dDistance);
private:
	void CalculateIntersectionWithSingleCorridor(OnboardCorridorInSim* pCorridorInSim,CorridorPointList& IntersectionPointList);
	int IntersectionPath(OnboardCorridorInSim* pCorridorInSim,const CPath2008& path1, const CPath2008& path2,CorridorPointList& IntersectionPointList);
	double ReflectIntersectionPointOnCorridor(const CPath2008& path,const CPoint2008& pt)const;
	int IntersectionCenterPath(OnboardCorridorInSim* pCorridorInSim,const CPath2008& path1, const CPath2008& path2,CorridorPointList& IntersectionPointList)const;

	void CaculateSpans();
	virtual void CalculateSpace();

public:
	COnboardCorridor* m_pCorridor;
	CoridorSpans m_spans;
	// all the intersection with corridor path
	std::vector<CorridorPointList> m_centerIntersectionPoint;
};