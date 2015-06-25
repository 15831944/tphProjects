#pragma once
#include "ARCPath.h"


class ARCBoundBox
{
public:
	ARCPoint2 m_ptBL; //bounding rectangle BL
	ARCPoint2 m_ptTR; //bounding rectangle TR

	bool bContains(const ARCPoint2& pt)const
	{
		return ( m_ptBL.x < pt.x && pt.x < m_ptTR.x && m_ptBL.y < pt.y && pt.y < m_ptTR.y);
	}

	bool IsOverlap(const ARCBoundBox& otherBox)const;

	void Reset(const ARCPoint2& pt){ m_ptBL = m_ptTR = pt; }
	void Merge(const ARCPoint2& pt)
	{
		m_ptBL.x = MIN(m_ptBL.x, pt.x);
		m_ptBL.y = MIN(m_ptBL.y, pt.y);
		m_ptTR.x = MAX(m_ptTR.x ,pt.x);
		m_ptTR.y = MAX(m_ptTR.y, pt.y);
	}
	void Merge(const ARCBoundBox& otherBox)
	{
		Merge(otherBox.m_ptBL);
		Merge(otherBox.m_ptTR);
	}
};

class ARCRay2;
class ARCPolygon : public ARCPath
{
public:
	ARCPolygon() {}
	ARCPolygon(const ARCPath& _rhs) : ARCPath(_rhs) {}
	ARCPolygon(const std::vector<ARCPoint2>& _rhs) : ARCPath(_rhs) {}
	virtual ~ARCPolygon() {};

	// use even_odd rule to determine if the Polygon contains _pt or _poly
    bool Contains(const ARCPoint2& _pt) const;		//returns true if _pt inside this polygon
	bool Contains(const ARCPolygon& _poly) const;	//returns true if all point of _poly are inside this polygon

	// test Line (p1, p2) for intersection with any polygon segment
    bool Intersects(const ARCPoint2& _pt1, const ARCPoint2& _pt2) const;
	
	bool IsOverLap(const ARCPoint2& _pt1, const ARCPoint2& _pt2) const;

	bool IsOverlapWithOtherPolygon(const ARCPolygon& _poly) const;

	// test if this pollygon is a contex pollygon
	bool IsConvex() const ;

	bool IsLineInsidePolygon(const ARCPoint2& _pt1, const ARCPoint2& _pt2) const;

	void DivideAsConvexPolygons(std::vector<ARCPolygon>& _vResult) const;   

	//perform scale transformation each point
    void Scale(double _sx, double _sy);

	void Zoom(DistanceUnit _dX, DistanceUnit _dY);

	void Build(const CPoint2008& pos, const ARCVector3& dir, double dHeadLen, double dBackLen, double dWidth);
	bool UpdateBound();	
	ARCBoundBox mbBox;
protected:
};
