#if !defined(_MAIN_AREAINTERSECTPOINT_H_)
#define _MAIN_AREAINTERSECTPOINT_H_ 

// inlcude
#include "..\common\point.h"
#include "..\common\elaptime.h"

// delcare
class CArea;
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

enum AreaIntersectState { STATE_UNKNOW, ENTRY_AREA, LEAVE_AREA,MOVETO_CORNER, ARRIVAL_DEST };

struct INTERSECT_INFO
{
	AreaIntersectState  state;
	CArea*				pArea;
	int					iSegment;

	INTERSECT_INFO() : pArea(NULL) 
	{
	}

	INTERSECT_INFO( AreaIntersectState _state, CArea* _pArea, int _iSeg) 
	{
		state = _state;
		pArea = _pArea;
		iSegment = _iSeg;
	}
		
	~INTERSECT_INFO()
	{	}
};

typedef std::vector<INTERSECT_INFO> INTERSECT_INFO_LIST;

class AreaIntersectPoint
{
public:
	Point			point;
	ElapsedTime		intersectTime;
	INTERSECT_INFO_LIST intersectInfo;
	
	AreaIntersectPoint() {}
	AreaIntersectPoint( const Point& _pt, const ElapsedTime& _time, const INTERSECT_INFO_LIST& _info )
	{
		point = _pt;
		intersectTime = _time;
		intersectInfo = _info;
	}

	~AreaIntersectPoint()
	{ }

	int operator==( const AreaIntersectPoint& _otherPt ) const
	{
		return _otherPt.point.preciseCompare( point );
	}

};

typedef std::vector< AreaIntersectPoint > INTERSECTPOINTLIST;

bool sortByXValueLess( const AreaIntersectPoint& _pt1, const AreaIntersectPoint& _pt2 ) ;
bool sortByXValueMore( const AreaIntersectPoint& _pt1, const AreaIntersectPoint& _pt2 ) ;
bool sortByYValueLess( const AreaIntersectPoint& _pt1, const AreaIntersectPoint& _pt2 ) ;
bool sortByYValueMore( const AreaIntersectPoint& _pt1, const AreaIntersectPoint& _pt2 ) ;

#endif	//_MAIN_AREAINTERSECTPOINT_H_ 