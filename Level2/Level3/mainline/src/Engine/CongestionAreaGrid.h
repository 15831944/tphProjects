#pragma once

#include ".\congestionareacell.h"
#include "..\inputs\areasportals.h"
#include "..\Inputs\Pipe.h"
//#include "..\common\dataset.h"
//#include "..\common\exeption.h"
#include "..\common\point.h"
#include "..\common\line.h"
#ifdef _DEBUG
class Person;
#endif

#define	CELLWIDTH	1
#define	CELLHEIGHT	1


#ifdef _DEBUG
static bool g_debugfileflg = false;
#endif


class CongestionCellCentrePoint : public CPoint
{
public:
	CongestionCellCentrePoint( void ) {};
	CongestionCellCentrePoint(Point ptt);

	bool operator==(const CongestionCellCentrePoint& _other)const 
	{
		return ( (x == _other.x) && (y == _other.y) );
	}
};

struct CellMapCmp
{
	bool operator()(const CongestionCellCentrePoint p1, const CongestionCellCentrePoint p2) const
	{
		if( p1.x < p2.x)
		{
			return true;
		}
		else if( p1.x == p2.x )
		{
			if( p1.y < p2.y)
			{
				return true;
			}
		}

		return false;
	}
};
 
class CCongestionAreaGrid
{
public:
	CCongestionAreaGrid(void);
	virtual ~CCongestionAreaGrid(void);

protected:
	std::map<CongestionCellCentrePoint, CCongestionAreaCell, CellMapCmp>  m_vCells;//

	std::vector<Point> m_AllPtVec ;
	std::vector<Pollygon> m_AllPolVec;
	RECT m_MaxRect;

public:
	void InitGrid(CAreas* pAreas, const std::vector<CPipe*>& vpPipes);
	Point GetNearestPoint(const Point& pCurrentPt,const Point& pDestPt);
#ifdef _DEBUG
	CCongestionAreaCell* GetCell(const Point pt, Person *pPerson = NULL);
#else
	CCongestionAreaCell* GetCell(const Point pt);
#endif
	bool GetInterSectPoint(const Point pts, const Point pte, Point& interpoint);
	bool IsValidCongestionArea() ;

private:
	void GetAreasInfo(CAreas* pAreas); // get the pollygon in area, and get the all points in pollygon.
	void GetPipeInfo(const std::vector<CPipe*>& pPipes); // get the all pollygons in pipe, and get the all points in pollygons.
	bool GetMaxRectByPoint( RECT& maxrc, const std::vector<Point>& ptvec);
	long GetUsableCell( );
	bool IsNeedRect(RECT& rc, double curfloor);
	bool GetLineToPollygonIntersection(const Pollygon& pol, const Point pts, const Point pte, Point& intersectpoint);
	void GetRectByLeftTopPoint(long lf, long tp, RECT& rc);

private:
	double m_Curfloor;

};
