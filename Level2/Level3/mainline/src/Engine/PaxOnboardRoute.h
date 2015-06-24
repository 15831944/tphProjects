#pragma once
#include <common/Point2008.h>
#include "CorridorGraphMgr.h"
#include <common/Range.h>

class OnboardAircraftElementInSim;

class PaxOnBoardRouteSeg
{
public:
	PaxOnBoardRouteSeg();
	PaxOnBoardRouteSeg(OnboardAircraftElementInSim* pElm, double dDistF,double dDistT)
		:m_dDistFrom(dDistF),m_dDistTo(dDistT),m_pElement(pElm)
	{

	}
	PaxOnBoardRouteSeg(OnboardAircraftElementInSim* pElm, const ARCVector3& pF,const ARCVector3& pT);


	double m_dDistFrom;
	double m_dDistTo;
	OnboardAircraftElementInSim* m_pElement;
	CPoint2008 m_ptFrom;
	CPoint2008 m_ptTo;

	void InitPtToDist();
	bool GetPath(CPath2008& path)const;

	CPoint2008 GetDir(double dDistInItem)const;
	CPoint2008 GetDistancePointInSeg(double dDistance) const;
	CPoint2008 GetDistancePoint(double dDistance) const;

	//bool GetRouteSpan(double dDistInItem,CPoint2008& pC, CPoint2008& pL,CPoint2008& pR)const;
	bool IsOutofSeg(double dDistInSeg,double& dextra)const;
	double DistInElement(double distInSeg)const; //map the dist in seg to the element
	double DistInSeg(double distInElm)const; //map the dist in element to the seg
	double GetDistInRouteItem(const CPoint2008& pt)const;

	bool PointInSeg(const CPoint2008& pt, DistanceUnit& distInSeg)const;

	//the force avoid the pax stuck in path
	CPoint2008 GetBoundForce(const CPoint2008& pos)const;

	//get the closet bound dir
	bool GetFollowSideDir(const CPoint2008& pos, CPoint2008& dir, CPoint2008& closetPt);

	CPoint2008 GetFirstPointInSeg()const;
	//
	//CPoint2008 GetClosetPoint(const CPoint2008& pt)const;
	bool RayIntersect(const CPoint2008& pos, const CPoint2008& dir, CPoint2008& intersectpt,CNumRange& dirAnlgeRange  );

	bool GetSpan(double distInSeg, CPoint2008& pC, CPoint2008& pL, CPoint2008& pR)const;
	bool GetNextSpan(double distInSeg, CPoint2008& pL, CPoint2008& pR, double& nextDistInSeg)const;	

	DistanceUnit GetHeight(double distInSeg)const;

	bool IsPositive()const{ return m_dDistFrom< m_dDistTo; }

	//////////////////////////////////////////////////////////////////////////

};

class PaxOnboardRoute
{
public:
	PaxOnboardRoute(void);
	~PaxOnboardRoute(void);
	void SetDirectRoute(CorridorGraphVertexList& vertexList); 
	void SetRoute(CorridorGraphVertexList& vertexList); 

	PaxOnBoardRouteSeg& ItemAt(int idx){ return m_vItems[idx]; }
	const PaxOnBoardRouteSeg& ItemAt(int idx)const{ return m_vItems[idx]; }
	int GetCount()const{ return (int)m_vItems.size(); }

	void SetEndPoint(ARCVector3& endPoint);

	void ClearRoute(){ m_vItems.clear();}

	//only could be call to get the point which away from the end point 
	//dDsiatnce could be + or -
	//- Ahead of the end point
	//+ Behind the end point
	CPoint2008 GetPointOfRoute(double dDistance);

	CPoint2008 GetEndPoint(){return m_endPoint;}

	//only called by find route to seat
	void CutToCheckingPoint(double& dChekingPointDistInCorridor);


protected:
	CPoint2008 m_endPoint;
	std::vector<PaxOnBoardRouteSeg>  m_vItems;
};
