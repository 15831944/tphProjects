// ARCLine.h
//
// interface for the ARCLine class
//
// this class represents a 2D Line (2 2D points)
//////////////////////////////////////////////////////////////////////
#ifndef ARCLINE__H
#define ARCLINE__H

#include "ARCVector.h"
#include "CommonDll.h"

class COMMON_TRANSFER ARCLine
{
protected:
	ARCPoint2 m_pt1;
	ARCPoint2 m_pt2;

public:
	ARCLine() {}
	ARCLine(const ARCLine& _rhs)
		: m_pt1(_rhs.m_pt1), m_pt2(_rhs.m_pt2) {}
	ARCLine(const ARCPoint2& _pt1, const ARCPoint2& _pt2)
		: m_pt1(_pt1), m_pt2(_pt2) {}
		~ARCLine(){}; //not virtual since we will not inherit from this class


	//ARCPoint2 Intersection(const ARCLine& _l) const;	//intersection between this line and _l
    bool Intersects(const ARCLine& _l, ARCPoint2* _pResPt=NULL) const;//returns true if _l intersects this line
    bool Intersects(const ARCPoint2& _p) const			//returns true if _p intersects this line
		{ return Contains(_p); }
	//bool Intersects(const ARCLine& _l, ARCPoint2& _resPt) const;	//returns true if _l intersects this line
																	//when returning true, _resPt holds the intersection point
    bool MayIntersect(const ARCLine& _l) const;
    bool Contains(const ARCPoint2& _p) const;
    bool IsCornerPoint(const ARCPoint2& _p) const
        { return _p.Compare(m_pt1) || _p.Compare(m_pt2); }
	
    const ARCPoint2& Nearest(const ARCPoint2& _p) const;	//returns the nearest point (m_pt1 or m_pt2) to _p
    ARCPoint2 GetMinPoint() const
		{ return ARCPoint2( min(m_pt1[VX],m_pt2[VX]), min(m_pt1[VY],m_pt2[VY]) ); }
    ARCPoint2 GetMaxPoint() const
		{ return ARCPoint2( max(m_pt1[VX],m_pt2[VX]), max(m_pt1[VY],m_pt2[VY]) ); }
	DistanceUnit Length() const						//returns the length of this line
		{ return m_pt1.DistanceTo(m_pt2); }
	// make a 90 perp vector from _point to this line
	// if the perp vector intersect with this line, return the intersection point
	// return TRUE for there is a result, FALSE for no result.
	bool GetIntersectPoint(const ARCPoint2& _p, ARCPoint2& _resPoint, DistanceUnit& _resLength) const;
	void GetCenterPoint(ARCPoint2& _ptCtrPoint) const;
	void Init(const ARCPoint2& _ptFrom, const ARCPoint2& _ptTo)
		{ m_pt1 = _ptFrom; m_pt2 = _ptTo; }
	//friend ostream& operator << (ostream& _s, const ARCLineFl& _l);

};



class COMMON_TRANSFER ARCLine3{
public:
	ARCVector3 m_pt1;
	ARCVector3 m_pt2;

public:
	ARCLine3();
	ARCLine3(const ARCVector3& p1, const ARCVector3 & p2);

	bool Intersects(const ARCLine3& line)const;
	

	

	//get two nearest point of two line
	friend void TwoNearestPoint(const ARCLine3& line1, const ARCLine3& line2, float& clampf1, float& clamf2 );

};



#endif