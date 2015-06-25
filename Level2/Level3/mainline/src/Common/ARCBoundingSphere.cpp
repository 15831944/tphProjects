#include "StdAfx.h"


#include "ARCBoundingSphere.h"




ARCBoundingSphere Merge( const ARCBoundingSphere& bs1, const ARCBoundingSphere& bs2 )
{
	ARCBoundingSphere reslt = bs1;
	return reslt += bs2;
}

ARCBoundingSphere& ARCBoundingSphere::SetLocation( const ARCVector3& vLocation )
{
	m_vWorldLoaction = vLocation; 
	return *this;
}

ARCBoundingSphere& ARCBoundingSphere::SetRadius( const DistanceUnit& rad )
{
	m_dRadius = rad;
	return *this;
}

ARCBoundingSphere& ARCBoundingSphere::operator+=( const ARCBoundingSphere& bs )
{
	m_dRadius += bs.m_dRadius + m_vWorldLoaction.DistanceTo(bs.m_vWorldLoaction);
	m_vWorldLoaction = 0.5 * (bs.m_vWorldLoaction + m_vWorldLoaction);
	return *this;
}

ARCBoundingSphere::ARCBoundingSphere() : m_vWorldLoaction(0,0,0)
{
	m_dRadius = 0;
}

ARCBoundingSphere::ARCBoundingSphere( const ARCVector3& vLocation,const DistanceUnit& rad ) : m_dRadius (rad), m_vWorldLoaction(vLocation)
{
	
}
ARCVector3 ARCBoundingSphere::GetLocation() const
{
	return m_vWorldLoaction;
}

DistanceUnit ARCBoundingSphere::GetRadius() const
{
	return m_dRadius;
}