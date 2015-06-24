#ifndef BARRIER_H
#define BARRIER_H

#include "common\containr.h"
#include "common\pollygon.h"
#include "engine\process.h"

//record information of routine
class RoutingOption
{
public:
    Point origPoint;
    Point result;
    double distance;
    int corner;

    RoutingOption (const Point &p_pnt) { origPoint = p_pnt; distance = 0.0; }
    RoutingOption () { distance = 0.0; }

    int operator == (const RoutingOption& option) const
        { return result == option.result ; }
    int operator < (const RoutingOption& option) const
        { return distance < option.distance; }
};

//It stores position of a barrier and handle routine around it.
class Barrier : public Processor
{
protected:
    //position of barrier
    Pollygon m_barrier;

    //number of points of barrier
    int pointCount;

public:
    Barrier() {};

    //copy constructor
    Barrier (Barrier& aProc) { *this = aProc; };

    // It initializes service location(s).
    // It must test count according to type of processor
    virtual void initServiceLocation (int pathCount, const Point *pointList);

    //It initializes in constraint.
    void initInConstraint (int pathCount, const Point *pointList);

    //It allocates and initializes fixed or non-fixed queue
    virtual void initQueue (int fixed, int pathCount, const Point *pointList);

    //It initializes out constraint.
    void initOutConstraint (int pathCount, const Point *pointList);

    //Returns barrier processor type.
    virtual int getProcessorType (void) const { return BarrierProc; };

    //Returns barrier processor name.
    virtual const char *getProcessorName (void) const { return "BARRIER"; };

    //Returns TRUE if path from p1 to p2 intersects with barrier.
    int intersects (const Point& p1, const Point& p2){ return m_barrier.intersects (p1, p2); }

    //Gets position of floor.
    long getFloor (void) { return (long)m_barrier.getPoint(0).getZ(); }

    //Returns TRUE if the point lies within the m_barrier's polygon.
    int contains (const Point& pnt) { return m_barrier.contains (pnt); };

    Point getNextPoint (const Point& orig, const Point& dest) const;

	Point getNextWorsePoint(const Point&orig,const Point& dest) const;

    int getRoute (RoutingOption &p_route, const Point& p_dest,
        int *p_points, float p_rotation) const;
	
	// do offset for all the position related data. be careful with the derived class.
	virtual void DoOffset( DistanceUnit _xOffset, DistanceUnit _yOffset );

	void scale (double sx, double sy, double sz = 1.0);

	virtual Processor* CreateNewProc();

public:
	virtual void UpdateMinMax();

};

#endif
