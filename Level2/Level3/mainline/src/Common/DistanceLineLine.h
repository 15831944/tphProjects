#pragma once


#include "Line2008.h"
#include <limits>
#include "Path2008.h"

class DistanceLineLine2D
{
public:
	double GetLine1Parameter()const{ return m_fLine0Parameter; }
	double GetLine2Parameter()const{ return m_fLine1Parameter; }
	//DistanceUnit GetSquared(const CLine2008& line1, const CLine2008& line2);
protected:
	// Information about the closest points.
	double m_fLine0Parameter;  // closest0 = line0.origin+param*line0.direction from(0-1)
	double m_fLine1Parameter;  // closest1 = line1.origin+param*line1.direction from(0-1)
};



class DistanceLineLine3D
{
public:
	DistanceUnit GetSquared(const CLine2008& l1, const CLine2008& l2);
	double GetLine1Parameter()const{ return m_fLine0Parameter; }
	double GetLine2Parameter()const{ return m_fLine1Parameter; }
protected:
	// Information about the closest points.
	double m_fLine0Parameter;  // closest0 = line0.origin+param*line0.direction from(0-1)
	double m_fLine1Parameter;  // closest1 = line1.origin+param*line1.direction from(0-1)

};

class DistancePointLine3D
{
public:	
	DistanceUnit GetSquared(const CPoint2008& pt, const CLine2008& l1);
	double m_fLineParameter; //relate point dist, from(0 -1)
	CPoint2008 m_clostPoint;
protected:
};

class DistancePointPath3D
{
public:	
	DistanceUnit GetSquared(const CPoint2008& pt, const CPath2008& path);
	double m_fPathParameter;  //the relate point index number, from (0 - path point count)
	CPoint2008 m_clostPoint;        //closet
protected:


};


//class DistanceLinePath3D 
//{
////public:
////	DistanceLinePath3D(const CLine2008& l1 , const CPath2008& path2 );
////	double GetLine1Parameter()const{ return m_fLine0Parameter; }
////	double GetLine2Parameter()const{ return m_fLine1Parameter; }
////protected:
////	// Information about the closest points.
////	double m_fLine0Parameter;  // closest0 = line0.origin+param*line0.direction  from(0-1)
////	double m_fLine1Parameter;  // closest1 = line1.origin+param*line1.direction from(0-1)
//}; 

class IntersectLineLine3D : public DistanceLineLine3D
{
public:
	//IntersectLineLine3D();
	bool Insects(const CLine2008& l1, const CLine2008& l2, DistanceUnit dTolerance = (std::numeric_limits<DistanceUnit>::min)() );
};

class IntersectLineLine2D : public DistanceLineLine2D
{
public:
	//IntersectLineLine2D(){}
	bool Intersects(const CLine2008& l1,const CLine2008& l2,DistanceUnit dTolenrace = (std::numeric_limits<DistanceUnit>::min)() );
};


class IntersectLinePath2D 
{
public:
	//IntersectLinePath2D(){}	
	int Intersects(const CLine2008& l1, const CPath2008& _path,DistanceUnit dTolenrace = (std::numeric_limits<DistanceUnit>::min)());
	std::vector<double> m_vlineParameter;//point index
	std::vector<double> m_vpathParameter;
protected:	
};


class IntersectPathPath2D
{
public:
	int Intersects(const CPath2008& path1, const CPath2008& path2,DistanceUnit dTolenrace = (std::numeric_limits<DistanceUnit>::min)());

	std::vector<double> m_vIntersectPtIndexInPath1; //index in path
	std::vector<double> m_vIntersectPtIndexInPath2; //index in path

protected:
};


class IntersectPathPolygon2D
{
public:
	int Intersects(const CPath2008& path, const CPath2008& polygon,DistanceUnit dTolenrace = (std::numeric_limits<DistanceUnit>::min)() );

	std::vector<double> m_vpathParameters;
	std::vector<double> m_vpolyParameters;
protected:
};