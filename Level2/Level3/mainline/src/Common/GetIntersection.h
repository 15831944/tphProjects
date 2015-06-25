#pragma once


class CPath2008;
class CPoint2008;

struct HoldPositionInfo 
{
	int intersectidx;
	DistanceUnit first;   //center pos
	DistanceUnit second;   // intersect pos

	bool IsFirst()const{ return first > second; }
	bool IsSameHold(const HoldPositionInfo& otherHoldPos )const{ return (intersectidx==otherHoldPos.intersectidx) && (IsFirst() == otherHoldPos.IsFirst()); }
	bool operator ==(const HoldPositionInfo& otherHoldPos)const{ return IsSameHold(otherHoldPos); }
};

typedef std::vector< HoldPositionInfo > HoldPositionInfoList;

void GetHoldPositionsInfo(const CPath2008& selfPath, DistanceUnit selfWith,const CPath2008& otherPath, DistanceUnit dwidth, HoldPositionInfoList& intersectList,bool bwithOffset);

CPath2008 GetSubPath(const CPath2008& pathsrc , DistanceUnit from ,DistanceUnit to );
CPath2008 GetSubPathRelate(const CPath2008& pathsrc, float relatfrom ,float relatTo);

//get the point dist in the path
//double GetPointDistInPath(const CPath2008& path,const CPoint2008& pt);
