// AirfieldProcessor.h: interface for the AirfieldProcessor class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AIRFIELDPROCESSOR_H__40A6C31D_4610_4A50_B3F8_A6248708C096__INCLUDED_)
#define AFX_AIRFIELDPROCESSOR_H__40A6C31D_4610_4A50_B3F8_A6248708C096__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../common\containr.h"
#include "../engine\process.h"
#include "proclist.h"
#include "../common\Line.h"

class AirfieldProcessor : public Processor  
{
public:
	AirfieldProcessor();
	virtual ~AirfieldProcessor();

	//processor type
	virtual int getProcessorType (void) const =0;
	//processor name
	virtual const char *getProcessorName (void) const =0;
	virtual void autoSnap(ProcessorList * proclist){NULL;}
};
////return the snap point to the path if in the width
inline static Point GetSnapPoint(const Path* path, double dwidth,Point p)
{
	int nPathPtCount=0;
	nPathPtCount=path->getCount();
	Point restl=p;
	for(int i=0;i<nPathPtCount-1;++i)
	{
		Point p1=path->getPoint(i);
		Point p2=path->getPoint(i+1);
		//Distance to The line (pi,pi+1)
		DistanceUnit _dist=abs((p2.getY()-p1.getY())*(p.getX()-p1.getX())-(p.getY()-p1.getY())*(p2.getX()-p1.getX()))/p1.getDistanceTo(p2);
		if(_dist<dwidth*0.5)
		{
			Point prjPt=Line(p1,p2).getProjectPoint(p);
			if(prjPt.within(p1,p2)){
				restl=prjPt;
				break;
			}
		}
	}
	return restl;
}
#endif // !defined(AFX_AIRFIELDPROCESSOR_H__40A6C31D_4610_4A50_B3F8_A6248708C096__INCLUDED_)
