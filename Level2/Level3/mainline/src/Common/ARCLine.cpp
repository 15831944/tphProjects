
#include "StdAfx.h"
#include <limits>
#include "ARCLine.h"

bool ARCLine::Intersects(const ARCLine& _l, ARCPoint2* _pResPt) const
{
	ARCPoint2 d1(m_pt2 - m_pt1);
	ARCPoint2 d1perp((m_pt2 - m_pt1).PerpendicularLCopy());
	ARCPoint2 d2(_l.m_pt2 - _l.m_pt1);
	ARCPoint2 d2perp(d2.PerpendicularLCopy());

	DistanceUnit s = ((_l.m_pt1 - m_pt1)*d2perp)/(d1*d2perp);
	if(s < 0.0 || s > 1.0)
		return false;
	DistanceUnit t = ((m_pt1 - _l.m_pt1)*d1perp)/(d2*d1perp);
	if(t < 0.0 || s > 1.0)
		return false;

	if(_pResPt)
		(*_pResPt) = _l.m_pt1 + t*d2;
	return true;	
}

bool ARCLine::MayIntersect(const ARCLine& _l) const
{
	if (m_pt1 == _l.m_pt1 || m_pt1 == _l.m_pt2 || m_pt2 == _l.m_pt1 || m_pt2 == _l.m_pt2)
        return false;
    return (GetMaxPoint() > _l.GetMinPoint() && GetMinPoint() < _l.GetMaxPoint());
}
   
bool ARCLine::Contains(const ARCPoint2& _p) const
{
	//this line can be expressed as m_pt1 + s*(m_pt2 - m_pt1) where 0 >= s >= 1
	DistanceUnit s = (_p[VX] - m_pt1[VX])/(m_pt2[VX]-m_pt1[VX]);
	if(s < 0.0 || s > 1.0)
		return false;
	return true;
}

const ARCPoint2& ARCLine::Nearest(const ARCPoint2& _p) const
{
	if(m_pt1.DistanceTo(_p) < m_pt2.DistanceTo(_p))
		return m_pt1;
	else
		return m_pt2;
}

bool ARCLine::GetIntersectPoint(const ARCPoint2& _p, ARCPoint2& _resPoint, DistanceUnit& _resLength) const
{
	// check if the point will have intersect perp line.
	ARCVector2 thisVector( m_pt2 - m_pt1 );
	ARCVector2 thatVector( _p - m_pt1 );
	DistanceUnit dCosVal = thisVector.GetCosOfTwoVector( thatVector );
	if( dCosVal < 0 )
		return false;

	DistanceUnit dPtLenth = m_pt1.DistanceTo( _p );
	_resLength = dPtLenth * dCosVal;
	if( _resLength > Length() )
		return false;

	thisVector.SetLength( _resLength );
	_resPoint = m_pt1 + thisVector;
	return true;
}

void ARCLine::GetCenterPoint(ARCPoint2& _ptCtrPoint) const
{
	_ptCtrPoint[VX] = m_pt1[VX] + (m_pt2[VX] - m_pt1[VX]) / 2.0;
	_ptCtrPoint[VY] = m_pt1[VY] + (m_pt2[VY] - m_pt1[VY]) / 2.0;
}



void TwoNearestPoint( const ARCLine3& line1, const ARCLine3& line2, float& clampf1, float& clamf2 )
{

}

bool ARCLine3::Intersects( const ARCLine3& line ) const
{
	
	float f1, f2;
	TwoNearestPoint(*this, line, f1, f2);
	ARCVector3 v1 = (1-f1) * m_pt1 + f1 * m_pt2;
	ARCVector3 v2 = (1-f2) * line.m_pt1  + f2 * line.m_pt2;

	return v1.DistanceTo(v2) < 0.0001;

}