#include "StdAfx.h"
#include ".\rotationseries.h"

#include <algorithm>
using namespace Ogre;
RotationSeries::RotationSeries( size_t nReservedSeriesCount )
{
	if (nReservedSeriesCount)
	{
		m_rots.reserve(nReservedSeriesCount);
	}
}

RotationSeries::~RotationSeries(void)
{
}

void RotationSeries::AddRotation( const Vector3& axis, Radian angle, Node::TransformSpace relativeTo /*= Node::TS_LOCAL*/ )
{
	m_rots.push_back(RotationItem(axis, angle, relativeTo));
}

void RotationSeries::ApplyRotation( Node* pNode ) const
{
	pNode->resetOrientation();
	std::for_each(m_rots.begin(), m_rots.end(), RotationApplier(pNode));
}


RotationSeries::RotationItem::RotationItem( const Vector3& _axis, Radian _angle, Node::TransformSpace _relativeTo /*= Node::TS_LOCAL*/ ) : axis(_axis)
, angle(_angle)
, relativeTo(_relativeTo)
{

}