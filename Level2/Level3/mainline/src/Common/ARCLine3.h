#ifndef ARCLINE3__H
#define ARCLINE3__H

#include "ARCMathCommon.h"
#include "ARCVector.h"

class ARCLine3
{
protected:
	ARCPoint3 m_pt1;
	ARCPoint3 m_pt2;

public:
	// Construction
	ARCLine3();
	ARCLine3(const ARCPoint3& _pt1, const ARCPoint3& _pt2);
	ARCLine3(const ARCLine3& _l);
	
	// Destruction
	virtual ~ARCLine3();

	ARCPoint3 Intersection(const ARCLine3& _l) const;
	int Intersects(const ARCLine3& _l) const;
	int Intersects(const ARCPoint3& _p) const;
	int MayIntersect(const ARCLine3& _l) const;
	int Contains(const ARCPoint3& _p) const;
	int IsCornerPoint(const ARCPoint3& _p) const;

	ARCPoint3 Nearest(const ARCPoint3& _p) const;
	ARCPoint3 GetMinPoint() const;
	ARCPoint3 GetMaxPoint() const;
	double GetLineLength() const;

	int GetIntersectionPoint(const ARCPoint3& _p, ARCPoint3& _result, double& _dLength) const;
	//friend ostream& operator << (ostream& _s, const ARCLine3& _l);
	int Intersects(const ARCLine3& _l, ARCPoint3& _result) const;
	void GetCenterPoint(ARCPoint3& _result) const;
	double GetZ() const;
};


#endif