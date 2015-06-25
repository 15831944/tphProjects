#pragma once
#include <Common/Point2008.h>
#include <common/Path2008.h>

class COMMON_TRANSFER UTurnPath
{
public:
	UTurnPath(const CPoint2008& pStart, const CPoint2008& pCenter, const CPoint2008& pEnd, DistanceUnit dRad);
	~UTurnPath(void);

	CPath2008 GetPath()const;

	CPoint2008 m_pStart;
	CPoint2008 m_pEnd;
	CPoint2008 m_pCenter;
	DistanceUnit m_dRad;
};
