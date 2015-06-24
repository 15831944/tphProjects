#include "stdafx.h"
#include "areaintersectpoint.h"
#include "areasportals.h"


bool sortByXValueLess( const AreaIntersectPoint& _pt1, const AreaIntersectPoint& _pt2 ) 
{
	return _pt1.point.getX() < _pt2.point.getX();
}

bool sortByXValueMore( const AreaIntersectPoint& _pt1, const AreaIntersectPoint& _pt2 ) 
{
	return _pt1.point.getX() > _pt2.point.getX();
}

bool sortByYValueLess( const AreaIntersectPoint& _pt1, const AreaIntersectPoint& _pt2 ) 
{
	return _pt1.point.getY() < _pt2.point.getY();
}

bool sortByYValueMore( const AreaIntersectPoint& _pt1, const AreaIntersectPoint& _pt2 ) 
{
	return _pt1.point.getY() > _pt2.point.getY();
}
 