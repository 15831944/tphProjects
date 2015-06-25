#pragma once
#include "ARCVector.h"


class  ARCBoundingSphere
{
public:
	ARCBoundingSphere();
	ARCBoundingSphere(const ARCVector3& vLocation,const DistanceUnit& rad );
	ARCBoundingSphere& SetLocation(const ARCVector3& vLocation);
	ARCBoundingSphere& SetRadius(const DistanceUnit& rad);

	ARCVector3 GetLocation()const;
	DistanceUnit GetRadius()const;

	ARCBoundingSphere& operator += (const ARCBoundingSphere& bs);

friend ARCBoundingSphere Merge(const ARCBoundingSphere& bs1, const ARCBoundingSphere& bs2);

protected:
	ARCVector3 m_vWorldLoaction;
	DistanceUnit m_dRadius;
};
