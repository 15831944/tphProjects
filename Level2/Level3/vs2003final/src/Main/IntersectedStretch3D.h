#pragma once
#include "selectionmap.h"
#include "../InputAirside/IntersectedStretch.h"
#include "AirsideThickness3D.h"

class IntersectionNodesInAirport;
class IntersectedStretchPoint;
class IntersectedStretch;
class C3DView;


class CIntersectedStretch3DPoint : public Selectable
{
public:
	typedef ref_ptr<CIntersectedStretch3DPoint> RefPtr;
	CIntersectedStretch3DPoint(IntersectedStretch& point, int mid);
	~CIntersectedStretch3DPoint();
	void DrawOGL(C3DView* pView);
    virtual ARCPoint3 GetLocation()const;
	virtual SelectType GetSelectType()const;
	virtual void OnMove(DistanceUnit dx, DistanceUnit dy,DistanceUnit dz = 0);
	virtual void AfterMove();
    IntersectedStretchPoint& GetIntersectedStretchPt();
	IntersectionNode& GetIntersectionNode();
    DistanceUnit GetDistInObj() const;
protected:
	IntersectedStretch& m_stretch3D;
	int m_index;
};

/************************************************************************/
/*                  CIntersectedStretch3D                               */
/************************************************************************/
class CIntersectedStretch3D : public Selectable
{

public:
	typedef ref_ptr<CIntersectedStretch3D> RefPtr;
	CIntersectedStretch3D(IntersectedStretch& interStretch);
	~CIntersectedStretch3D(void);
	void DrawSelect(C3DView* pView);
	void DrawOGL(C3DView* pView);
	void DrawEditPoint(C3DView* pView,bool bSelect = false);
	void DrawLine(C3DView* pView,bool bEdit = false);
	void RenderOneStencil();
	virtual SelectType GetSelectType()const;
	virtual ARCPoint3 GetLocation() const;
	void GenSmoothStretchPath();

public:
	CIntersectedStretch3DPoint::RefPtr m_fristPoint;
	CIntersectedStretch3DPoint::RefPtr m_secondPoint;
	IntersectedStretch& m_interStretch;

protected:
	ARCPath3 m_vPath;
	CAirsideThickness3D airsideThick;
};


/************************************************************************/
/*                    CIntersectedStretch3DInAirport                    */
/************************************************************************/

class CIntersectedStretch3DInAirport
{
public:
	typedef std::vector<int> Int;
	typedef std::vector<IntersectedStretchPoint> PInterSt;

	CIntersectedStretch3DInAirport();
	~CIntersectedStretch3DInAirport();
	void DrawOGL(C3DView* pView);
	void DrawSelect(C3DView* pView);
	void DrawLine(C3DView* pView);
	void DrawLane(C3DView* pView);
	void RenderStencil();

	bool IsInEdit()const{return m_bInEdit;}
	void SetEdit(bool b){ m_bInEdit = b; }
	void SetInEditFillet(int id){ m_iInEdit = id; }
	CIntersectedStretch3D* GetInEditFillet();
	void GetPositionOfLane();
public:
	void UpdateIntersectedStretchInAirport(const IntersectionNodesInAirport& nodeList, const std::vector<int>& vChangeNodeIndexs);
	void InitIntersectedStrechInAirport(int nAirport,const IntersectionNodesInAirport& nodeList);
	int GetCount()const{ return (int)m_vintersectedStretch3D.size(); }
	CIntersectedStretch3D* GetStretch3D(int idx)const{ return (CIntersectedStretch3D* )m_vintersectedStretch3D.at(idx).get(); }
	IntersectedStretchInAirport GetIntersectedStretchInAirport()const;
protected:
	IntersectedStretchInAirport m_intersectedStretchInAirport;
	std::vector<CIntersectedStretch3D::RefPtr> m_vintersectedStretch3D;
	std::pair<Int, PInterSt> vLane;
	int m_nAirport;
	bool m_bInEdit;
	int m_iInEdit;
};