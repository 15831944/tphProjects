#pragma once
#include "..\..\Common\Point2008.h"

class LandsideResourceInSim; 
class CPath2008;
class ARCVector3;

class LandsidePosition
{
public:
	CPoint2008 pos;
	LandsideResourceInSim* pRes;
	DistanceUnit distInRes;	
};

class LandsidePath
{
public:
	int getPtCount()const{ return m_poslist.size(); }
	const LandsidePosition& PositionAt(int idx)const{  return m_poslist[idx]; }

	double getDistIndex(DistanceUnit dist)const;
	DistanceUnit getIndexDist(double dIndex)const;
	DistanceUnit getIndexDist(int iIndex)const;	

	LandsidePosition getIndexPosition(double dIdx)const;
	LandsidePosition getDistPosition(DistanceUnit dist)const{ return getIndexPosition(getDistIndex(dist)); }

	ARCVector3 getDistDir(DistanceUnit dist)const;
	ARCVector3 getIndexDir(double dIndex)const;

	LandsidePosition& addPostion(const LandsidePosition& pos);

	void getPath(DistanceUnit distF, DistanceUnit distT, CPath2008& path);

	void addPath(LandsideResourceInSim* pRes, const CPath2008* path,DistanceUnit distFrom);

	DistanceUnit getAllLength()const; 

	void Clear(){ m_distlist.clear(); m_poslist.clear(); }

	bool ExistResourcePoint(int iIdx,LandsideResourceInSim* pRes)const;
protected:
	std::vector<LandsidePosition> m_poslist;
	std::vector<DistanceUnit> m_distlist;
	typedef std::vector<DistanceUnit>::const_iterator _DistConstIter;
};