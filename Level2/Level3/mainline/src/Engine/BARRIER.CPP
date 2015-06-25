#include "stdafx.h"
#include "engine\barrier.h"
#include "common\pollygon.h"
#include "common\containr.h"
#include "common\exeption.h"



void Barrier::initServiceLocation (int pathCount, const Point *pointList)
{
    if (pathCount < 2)
        throw new StringError ("Barriers must have at least 2 points");

    m_location.init (pathCount, pointList);
    m_barrier.init (pathCount, pointList);
    pointCount = m_barrier.getCount();
	UpdateMinMax();
}


void Barrier::scale (double sx, double sy, double sz )
{
	m_barrier.scale( sx, sy, sz );
	UpdateMinMax();
}
void Barrier::initInConstraint (int pathCount, const Point *)
{
    if (pathCount)
        throw new StringError ("Barriers may not have in constraints");
	UpdateMinMax();
}

 
void Barrier::initQueue (int, int pathCount, const Point *)
{
    if (pathCount)
        throw new StringError ("Barriers may not have queues");
}

 
void Barrier::initOutConstraint (int pathCount, const Point *)
{
    if (pathCount)
        throw new StringError ("Barriers may not have out constraints");
	UpdateMinMax();
}

#define ROTATION 10.0

//Within this function, some methods are used to guide passenger around barrier.
Point Barrier::getNextPoint (const Point& orig, const Point& dest) const
{
    int usedPoints[65];
    memset ((void *)usedPoints, '\0', sizeof (int) * pointCount);
    usedPoints[pointCount] = -1;

    RoutingOption leftRoute (orig), rightRoute (orig);
    if (!getRoute (leftRoute, dest, usedPoints, -ROTATION))
        leftRoute.distance = 99999999.0;

    memset ((void *)usedPoints, '\0', sizeof (int) * pointCount);
    usedPoints[pointCount] = -1;
    if (!getRoute (rightRoute, dest, usedPoints, ROTATION))
        rightRoute.distance = 99999999.0;

    if (leftRoute.distance == 99999999.0 &&
        rightRoute.distance == 99999999.0)
            throw new StringError ("No path around barrier");

    return (leftRoute.distance < rightRoute.distance)?
        leftRoute.result: rightRoute.result;
}

//return leftRoute.result and rightRoute.result
Point Barrier::getNextWorsePoint(const Point&orig,const Point& dest) const
{
	int usedPoints[65];
    memset ((void *)usedPoints, '\0', sizeof (int) * pointCount);
    usedPoints[pointCount] = -1;
	
    RoutingOption leftRoute (orig), rightRoute (orig);
    if (!getRoute (leftRoute, dest, usedPoints, -ROTATION))
        leftRoute.distance = 0.0;
	
    memset ((void *)usedPoints, '\0', sizeof (int) * pointCount);
    usedPoints[pointCount] = -1;
    if (!getRoute (rightRoute, dest, usedPoints, ROTATION))
        rightRoute.distance = 0.0;
	
    if (leftRoute.distance == 0.0 &&
        rightRoute.distance == 0.0)
		throw new StringError ("No path around barrier");
	
	return (leftRoute.distance > rightRoute.distance)?
        leftRoute.result: rightRoute.result;

}
#define DISTANCE 100.0

/*
p_route is passed with the current point in origPoint, and is expected to be returned with the best option in result if getRoute returns true, as well as the total distance required to get to p_dest by following that route. 
If getRoute returns false, there is no way to get to p_dest using the direction indicated by p_rotation.
p_used is used to track where the route goes around the barrier so that endless looping can be avoided.
*/
int Barrier::getRoute (RoutingOption &p_route, const Point& p_dest, int *p_used, float p_rotation) const
{
    TArrayAsVector <RoutingOption> optionList (16, 0, 16);
    RoutingOption option;

    Point flowPoint, tempPoint, offset, orig (p_route.origPoint);
    int usedPoints[65];
    memcpy ((void *)usedPoints, (void *)p_used, sizeof(int)*(pointCount+1));

#if 0
    char origStr[32];
    char destStr[32];
    if (env->getParameter ("-BARR") != NULL)
    {
        p_route.origPoint.printPoint (origStr, 1);
        p_dest.printPoint (destStr, 1);
    }
#endif

    // fill optionList with intermediary points that may divert pax
    // around the barrier.
    for (int i = 0; i < pointCount; i++)
    {
        // skip all used points except near corner
        if (usedPoints[i] && i != usedPoints[pointCount])
            continue;

        flowPoint.initVector (orig, m_barrier.getPoint(i));
        flowPoint.scale (0.999, 0.999, 1.0);
        if (m_barrier.intersects (orig, flowPoint + orig))
            continue;

        flowPoint += DISTANCE;
        tempPoint = flowPoint;
        tempPoint.rotate (p_rotation);
        offset.initVector (flowPoint, tempPoint);
        offset.length (DISTANCE);
        option.origPoint = orig + flowPoint + offset;
        if (!m_barrier.intersects (orig, option.origPoint))
        {
            option.corner = i;
            option.distance = orig.distance (option.origPoint);
            optionList.Add (option);
        }
    }

    int optionCount = optionList.GetItemsInContainer();
    if (!optionCount)
        return 0;

    TSArrayAsVector <RoutingOption> sOptionList (16, 0, 16);
    for (int ii = 0; ii < optionCount; ii++)
    {
        option = optionList[ii];

        // intermediary point avoids barrier, add to distance and exit
        if (!m_barrier.intersects (option.origPoint, p_dest))
            option.distance += option.origPoint.distance (p_dest);
        else
        {
            // if there are other options, and near corner route does
            // not have a direct path to p_dest, now discard near point
            if (usedPoints[option.corner] && optionCount > 1)
                continue;

            usedPoints[option.corner] = 1;
            usedPoints[m_barrier.getCount()] = option.corner;
            if (!getRoute (option, p_dest, usedPoints, p_rotation))
                continue;
        }
        sOptionList.Add (option);
    }
    if (sOptionList.GetItemsInContainer() == 0)
        return 0;

    p_route.result = sOptionList[0].origPoint;
    p_route.distance += sOptionList[0].distance;
    return 1;
}


// do offset for all the position related data. be careful with the derived class.
void Barrier::DoOffset( DistanceUnit _xOffset, DistanceUnit _yOffset )
{
	Processor::DoOffset( _xOffset, _yOffset );
	m_barrier.DoOffset( _xOffset, _yOffset, 0.0 );
}

Processor* Barrier::CreateNewProc()
{
	Processor* pProc = new Barrier;
	return pProc;
}

void Barrier::UpdateMinMax()
{
	Processor::UpdateMinMax();
	GetMinMax(m_min,m_max,m_barrier);
}

