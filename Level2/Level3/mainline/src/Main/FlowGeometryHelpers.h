#ifndef __FLOWGEOMETRYHELPERS_H__
#define __FLOWGEOMETRYHELPERS_H__

#include "common\ARCPath.h"
#include "common\template.h"
#include "engine\MovingSideWalk.h"
#include "engine\PROCLIST.h"
#include "inputs\SubFlow.h"
#include "inputs\SinglePaxTypeFlow.h"
#include <Inputs/IN_TERM.H>

static const double dHalfWidth = 100.0;
static const int nFloorIdxScale = static_cast<int>(SCALE_FACTOR);

static void CreatePointToPointGeometry(const ARCPoint2& _p1, DistanceUnit _alt1, const ARCPoint2& _p2, DistanceUnit _alt2)
{
	//get perpendicular vector
	ARCVector2 vperp(1.0, -(_p2[VX]-_p1[VX])/(_p2[VY]-_p1[VY]));
	vperp.Normalize();
	vperp*=dHalfWidth; //set magnitude of vperp to dHalfWidth
	glBegin(GL_QUADS);
	glVertex3d(_p1[VX]-vperp[VX], _p1[VY]-vperp[VY],_alt1);
	glVertex3d(_p2[VX]-vperp[VX], _p2[VY]-vperp[VY],_alt2);
	glVertex3d(_p2[VX]+vperp[VX], _p2[VY]+vperp[VY],_alt2);
	glVertex3d(_p1[VX]+vperp[VX], _p1[VY]+vperp[VY],_alt1);
	glEnd();
}

static void CreatePointToPointGeometry(const ARCPath& _p1, DistanceUnit _alt1, const ARCPath& _p2, DistanceUnit _alt2)
{
	ASSERT(_p1.size() == 1);
	ASSERT(_p2.size() == 1);
	const ARCPoint2& pt1 = _p1[0];
	const ARCPoint2& pt2 = _p2[0];
	CreatePointToPointGeometry(pt1, _alt1, pt2, _alt2);
}

static void CreatePointToAreaGeometry(const ARCPoint2& _pt1, DistanceUnit _alt1, const ARCPath& _area, DistanceUnit _alt2)
{
	ASSERT(_area.size() > 1);

	//we want to find the extreeme edges of the area from p1
	//construct vectors between p1 and each point in area polygon
	//choose the 2 points that make vectors with biggest angle (smallest cosine)
	double dSmallestCosine = HUGE_VAL;
	int indicies[2];
	int c = _area.size();
	for(int i=0; i<c; i++) {
		const ARCPoint2& p1 = _area[i];
		ARCVector2 v1(p1 - _pt1);
		v1.Normalize();
		for(int j=0; j<c; j++) {
			if(i==j)
				continue;
			const ARCPoint2& p2 = _area[j];
			ARCVector2 v2(p2 - _pt1);
			v2.Normalize();
			double dDP = v1*v2;
			if(dDP < dSmallestCosine) {
				dSmallestCosine = dDP;
				indicies[0] = i;
				indicies[1] = j;
			}
		}	
	}

	int idxNext = (indicies[0]+1)%c;
	int idxPrev = (indicies[0]-1)%c;
	const ARCPoint2& p1 = _area[ indicies[0] ];
	const ARCPoint2& pN = _area[ idxNext ];
	const ARCPoint2& pP = _area[ idxPrev ];

	ARCVector2 v(p1 - _pt1);
	ARCVector2 vN(pN - p1);
	ARCVector2 vP(pP - p1);
	v.Normalize(); vN.Normalize(); vP.Normalize();

	const ARCPoint2& l1 = _area[ indicies[0] ];
	const ARCPoint2& l2 = _area[ indicies[1] ];
	ARCVector2 vline(l2 - l1);
	vline.Normalize();
	vline*=dHalfWidth;
	glBegin(GL_POLYGON);
	glVertex3d(_pt1[VX]-vline[VX], _pt1[VY]-vline[VY], _alt1);

	int sign = -1;
	if(v*vN < v*vP) { //angle between v and vN is smaller than v and vP -> go forward
		sign = 1;
	}

	for(int i=indicies[0]; i!=indicies[1]; ) {
		glVertex3d(_area[i][VX], _area[i][VY], _alt2);
		i=((i+sign)%c);
		if(i<0)
			i=c+i;
	}

	glVertex3d(_area[ indicies[1] ][VX], _area[ indicies[1] ][VY], _alt2);
	glVertex3d(_pt1[VX]+vline[VX], _pt1[VY]+vline[VY], _alt1);
	glEnd();
}

static void CreatePointToAreaGeometry(const ARCPath& _p, DistanceUnit _alt1, const ARCPath& _area, DistanceUnit _alt2)
{
	ASSERT(_p.size() == 1);
	CreatePointToAreaGeometry(_p[0], _alt1, _area, _alt2);
}

static void CreateAreaToAreaGeomtery(const ARCPath& _area1, DistanceUnit _alt1, const ARCPath& _area2, DistanceUnit _alt2)
{
	int i;
	//1) find centroid of area1 & area2 -> c1, c2
	ARCPoint2 ct1(_area1.Centroid());
	ARCPoint2 ct2(_area2.Centroid());
	//2) from c1, find the index of extreme vertices of area2 -> indicies2[2]
	double dSmallestCosine = HUGE_VAL;
	int indicies2[2];
	int c1 = _area1.size();
	int c2 = _area2.size();
	for(i=0; i<c2; i++) {
		const ARCPoint2& p1 = _area2[i];
		ARCVector2 v1(p1 - ct1);
		v1.Normalize();
		for(int j=0; j<c2; j++) {
			if(i==j)
				continue;
			const ARCPoint2& p2 = _area2[j];
			ARCVector2 v2(p2 - ct1);
			v2.Normalize();
			double dDP = v1*v2;
			if(dDP < dSmallestCosine) {
				dSmallestCosine = dDP;
				indicies2[0] = i;
				indicies2[1] = j;
			}
		}	
	}
	//3) from c2, find the extreme vertoces of area1 -> indicies1[2]
	dSmallestCosine = HUGE_VAL;
	int indicies1[2];
	c1 = _area1.size();
	for(i=0; i<c1; i++) {
		const ARCPoint2& p1 = _area1[i];
		ARCVector2 v1(p1 - ct2);
		v1.Normalize();
		for(int j=0; j<c1; j++) {
			if(i==j)
				continue;
			const ARCPoint2& p2 = _area1[j];
			ARCVector2 v2(p2 - ct2);
			v2.Normalize();
			double dDP = v1*v2;
			if(dDP < dSmallestCosine) {
				dSmallestCosine = dDP;
				indicies1[0] = i;
				indicies1[1] = j;
			}
		}	
	}
	//4) now we start drawing the polygon starting at indicies1[0] going towards indicies1[1]
	//4a) first we determine which way we traverse vertioces (back or forward)
	int idxNext = (indicies1[0]+1)%c1;
	int idxPrev = (indicies1[0]-1)%c1;
	ARCVector2 v(_area1[ indicies1[0] ] - ct2);
	ARCVector2 vN(_area1[ idxNext ] - _area1[ indicies1[0] ]);
	ARCVector2 vP(_area1[ idxPrev ] - _area1[ indicies1[0] ]);
	v.Normalize(); vN.Normalize(); vP.Normalize();

	int sign = -1;
	if(v*vN < v*vP) { //angle between v and vN is smaller than v and vP -> go forward
		sign = 1;
	}
	//4b) draw from indicies1[0] to indicies1[1]
	glBegin(GL_POLYGON);
	for(i=indicies1[0]; i!=indicies1[1]; ) {
		glVertex3d(_area1[i][VX], _area1[i][VY], _alt1);
		i=((i+sign)%c1);
		if(i<0)
			i=c1+i;
	}
	glVertex3d(_area1[ indicies1[1] ][VX], _area1[ indicies1[1] ][VY], _alt1);

	//5) now we need to check for the next appropriate vertex
	//	 is it indicies2[0] or indicies2[1]???
	//   Choose x (0 or 1) such that angle between v(area1->getPoint(indicies1[1]), c1) and v(area1->getPoint(indicies1[1]), area2->getPoint(indicies2[x])) is largest
	int x,y;
	ARCVector2 vv(ct1 - _area1[ indicies1[1] ]);
	ARCVector2 v0(_area2[ indicies2[0] ] - _area1[ indicies1[1] ]);
	ARCVector2 v1(_area2[ indicies2[1] ] - _area1[ indicies1[1] ]);
	if(vv.GetCosOfTwoVector(v0) < vv.GetCosOfTwoVector(v1)) { //angle between vv and v0 is bigger
		x=0;
		y=1;
	}
	else {
		x=1;
		y=0;
	}
	
	//6) now we dtermine the traversing direction for area2
	idxNext = (indicies2[x]+1)%c2;
	idxPrev = (indicies2[x]-1)%c2;

	v = ARCVector2(_area2[ indicies2[x] ] - ct1);
	vN = ARCVector2(_area2[ idxNext ] - _area2[ indicies2[x] ]);
	vP = ARCVector2(_area2[ idxPrev ] - _area2[ indicies2[x] ]);
	v.Normalize(); vN.Normalize(); vP.Normalize();

	sign = -1;
	if(v*vN < v*vP) { //angle between v and vN is smaller than v and vP -> go forward
		sign = 1;
	}

	//7) now draw from indicies2[x] to indicies2[(x+1)%2]
	for(i=indicies2[x]; i!=indicies2[y]; ) {
		glVertex3d(_area2[i][VX], _area2[i][VY], _alt2);
		i=((i+sign)%c2);
		if(i<0)
			i=c2+i;
	}
	glVertex3d(_area2[ indicies2[y] ][VX], _area2[ indicies2[y] ][VY], _alt2);

	glEnd();
}

static void CreatePathGeometry(const ARCPath& _path, DistanceUnit _alt)
{
	int c = _path.size();
	if(c < 2)
		return;
	for(int i=0; i<c-1; i++) {
		CreatePointToPointGeometry(_path[i], _alt, _path[i+1], _alt);
	}
}

static void CreateAreaGeometry(const ARCPath& _area, DistanceUnit _alt)
{
	ASSERT(_area.size() > 2);
	
	int c = _area.size();
	glBegin(GL_POLYGON);
	for(int i=0; i<c; i++) {
		glVertex3d(_area[i][VX], _area[i][VY], _alt);
	}
	glEnd();
}

static void FillPollygon( int _iPointNum, Point* _pPointList, DistanceUnit _alt )
{
	Pollygon tempPollygon;
	tempPollygon.init( _iPointNum, _pPointList );
	std::vector<Pollygon> vPollygons;
	tempPollygon.DividAsConvexPollygons( vPollygons );
	int iCount = vPollygons.size();
	for( int i=0; i<iCount; ++i )
	{
		glBegin(GL_POLYGON);
		int iPointCount = vPollygons[i].getCount();		
		for( int j=0; j<iPointCount ; ++j )
		{					
			glVertex3d( vPollygons[i].getPoint(j).getX(),vPollygons[i].getPoint(j).getY(), _alt );					
			
		}
		glEnd();
		
	}
}

static void CreateServLocToServLocGeometry(Processor* pSource, Processor* pDest, double* dAlt)
{
	int srcType = pSource->getProcessorType();
	int dstType = pDest->getProcessorType();
	//point -> point
	if ((srcType == PointProc || srcType == DepSourceProc || srcType == DepSinkProc || srcType == GateProc || srcType == FloorChangeProc) &&
		(dstType == PointProc || dstType == DepSourceProc || dstType == DepSinkProc || dstType == GateProc || dstType == FloorChangeProc)) {
		CreatePointToPointGeometry(ARCPath(pSource->serviceLocationPath()), dAlt[pSource->getFloor()/nFloorIdxScale], ARCPath(pDest->serviceLocationPath()), dAlt[pDest->getFloor()/nFloorIdxScale]);
	}
	//point -> line | area
	else if((srcType == PointProc || srcType == DepSourceProc || srcType == DepSinkProc || srcType == GateProc || srcType == FloorChangeProc) &&
		(dstType == LineProc || dstType == BarrierProc || dstType == HoldAreaProc || dstType == BaggageProc)) {
		CreatePointToAreaGeometry(ARCPath(pSource->serviceLocationPath()), dAlt[pSource->getFloor()/nFloorIdxScale], ARCPath(pDest->serviceLocationPath()), dAlt[pDest->getFloor()/nFloorIdxScale]);
		if( dstType == HoldAreaProc )
		{
			FillPollygon( pDest->serviceLocationLength(), pDest->serviceLocationPath()->getPointList(), dAlt[pDest->getFloor()/nFloorIdxScale] );
		}
	}
	//point -> MS
	else if((srcType == PointProc || srcType == DepSourceProc || srcType == DepSinkProc || srcType == GateProc || srcType == FloorChangeProc) &&
		(dstType == MovingSideWalkProc)) {
		CreatePointToPointGeometry(ARCPoint2(pSource->serviceLocationPath()->getPoint(0)), dAlt[pSource->getFloor()/nFloorIdxScale], ARCPoint2(pDest->serviceLocationPath()->getPoint(0)), dAlt[pDest->getFloor()/nFloorIdxScale]);
		POLLYGONVECTOR vCoveredArea;
		((MovingSideWalk*)pDest)->GetSideWalkCoveredRegion( vCoveredArea );
		int iQUADCount = vCoveredArea.size();
		DistanceUnit dZ = dAlt[(int) pDest->serviceLocationPath()->getPoint(0).getZ () /nFloorIdxScale];
		for( int i=0; i<iQUADCount ; ++i )
		{
			//vCoveredArea[i].
			glBegin(GL_QUADS);
			glVertex3d( vCoveredArea[i].getPoint(0).getX(),vCoveredArea[i].getPoint(0).getY(),dZ );
			glVertex3d( vCoveredArea[i].getPoint(1).getX(),vCoveredArea[i].getPoint(1).getY(),dZ );
			glVertex3d( vCoveredArea[i].getPoint(2).getX(),vCoveredArea[i].getPoint(2).getY(),dZ );
			glVertex3d( vCoveredArea[i].getPoint(3).getX(),vCoveredArea[i].getPoint(3).getY(),dZ );
			glEnd();
		}
	}
	//MS -> point
	else if((srcType == MovingSideWalkProc) &&
		(dstType == PointProc || dstType == DepSourceProc || dstType == DepSinkProc || dstType == GateProc || dstType == FloorChangeProc)) {
		int c = pSource->serviceLocationLength();
		CreatePointToPointGeometry(ARCPoint2(pSource->serviceLocationPath()->getPoint(c-1)), dAlt[pSource->getFloor()/nFloorIdxScale], ARCPoint2(pDest->serviceLocationPath()->getPoint(0)), dAlt[pSource->getFloor()/nFloorIdxScale]);
	}
	//MS -> area | line
	else if((srcType == MovingSideWalkProc) &&
		(dstType == LineProc || dstType == BarrierProc || dstType == HoldAreaProc || dstType == BaggageProc)) {
		int c = pSource->serviceLocationLength();
		CreatePointToAreaGeometry(ARCPoint2(pSource->serviceLocationPath()->getPoint(c-1)), dAlt[pSource->getFloor()/nFloorIdxScale], ARCPath(pDest->serviceLocationPath()), dAlt[pDest->getFloor()/nFloorIdxScale]);
		if( dstType == HoldAreaProc )
		{
			FillPollygon( pDest->serviceLocationLength(), pDest->serviceLocationPath()->getPointList(), dAlt[pDest->getFloor()/nFloorIdxScale] );
		}
	}
	//MS -> MS
	else if((srcType == MovingSideWalkProc) &&
		(dstType == MovingSideWalkProc)) {
		int c = pSource->serviceLocationLength();
		CreatePointToPointGeometry(ARCPoint2(pSource->serviceLocationPath()->getPoint(c-1)), dAlt[pSource->getFloor()/nFloorIdxScale], ARCPoint2(pDest->serviceLocationPath()->getPoint(0)), dAlt[pDest->getFloor()/nFloorIdxScale]);
		POLLYGONVECTOR vCoveredArea;
		((MovingSideWalk*)pDest)->GetSideWalkCoveredRegion( vCoveredArea );
		int iQUADCount = vCoveredArea.size();
		DistanceUnit dZ = dAlt[(int) pDest->serviceLocationPath()->getPoint(0).getZ () /nFloorIdxScale];
		for( int i=0; i<iQUADCount ; ++i )
		{
			//vCoveredArea[i].
			glBegin(GL_QUADS);
			glVertex3d( vCoveredArea[i].getPoint(0).getX(),vCoveredArea[i].getPoint(0).getY(),dZ );
			glVertex3d( vCoveredArea[i].getPoint(1).getX(),vCoveredArea[i].getPoint(1).getY(),dZ );
			glVertex3d( vCoveredArea[i].getPoint(2).getX(),vCoveredArea[i].getPoint(2).getY(),dZ );
			glVertex3d( vCoveredArea[i].getPoint(3).getX(),vCoveredArea[i].getPoint(3).getY(),dZ );
			glEnd();
		}
	}
	//area | line -> point
	else if((srcType == LineProc || srcType == BarrierProc || srcType == HoldAreaProc || srcType == BaggageProc) &&
		(dstType == PointProc || dstType == DepSourceProc || dstType == DepSinkProc || dstType == GateProc || dstType == FloorChangeProc)) {
		CreatePointToAreaGeometry(ARCPath(pDest->serviceLocationPath()), dAlt[pDest->getFloor()/nFloorIdxScale], ARCPath(pSource->serviceLocationPath()), dAlt[pSource->getFloor()/nFloorIdxScale]);
		if( srcType == HoldAreaProc )
		{
			FillPollygon( pSource->serviceLocationLength(), pSource->serviceLocationPath()->getPointList(), dAlt[pSource->getFloor()/nFloorIdxScale] );
		}
	}
	//area | line -> area | line
	else if((srcType == HoldAreaProc || srcType == BaggageProc || srcType == LineProc || srcType == BarrierProc) &&
		(dstType == HoldAreaProc || dstType == BaggageProc || dstType == LineProc || dstType == BarrierProc)) {
		CreateAreaToAreaGeomtery(ARCPath(pSource->serviceLocationPath()), dAlt[pSource->getFloor()/nFloorIdxScale], ARCPath(pDest->serviceLocationPath()), dAlt[pDest->getFloor()/nFloorIdxScale]);
		if( srcType == HoldAreaProc )
		{
			FillPollygon( pSource->serviceLocationLength(), pSource->serviceLocationPath()->getPointList(), dAlt[pSource->getFloor()/nFloorIdxScale] );
		}
		if( dstType == HoldAreaProc )
		{
			FillPollygon( pDest->serviceLocationLength(), pDest->serviceLocationPath()->getPointList(), dAlt[pDest->getFloor()/nFloorIdxScale] );
		}
	}
	//area | line -> MS
	else if((srcType == LineProc || srcType == BarrierProc || srcType == HoldAreaProc || srcType == BaggageProc) &&
		(dstType == MovingSideWalkProc)) {
		CreatePointToAreaGeometry(ARCPoint2(pDest->serviceLocationPath()->getPoint(0)), dAlt[pDest->getFloor()/nFloorIdxScale], ARCPath(pSource->serviceLocationPath()), dAlt[pSource->getFloor()/nFloorIdxScale]);
		if( srcType == HoldAreaProc )
		{
			FillPollygon( pSource->serviceLocationLength(), pSource->serviceLocationPath()->getPointList(), dAlt[pSource->getFloor()/nFloorIdxScale] );
		}
		
	}
}

static void CreateServLocToPointGeometry(Processor* _pSource, double _altSrc, const ARCPoint2& _pt, double _altPt)
{
	int srcType = _pSource->getProcessorType();
	if(srcType == PointProc || srcType == DepSourceProc || srcType == DepSinkProc || srcType == GateProc || srcType == FloorChangeProc) {
		CreatePointToPointGeometry( ARCPoint2(_pSource->serviceLocationPath()->getPoint(0)), _altSrc, _pt, _altPt);
	}
	else if(srcType == LineProc || srcType == BarrierProc || srcType == HoldAreaProc || srcType == BaggageProc) {
		CreatePointToAreaGeometry(_pt, _altPt, ARCPath(_pSource->serviceLocationPath()), _altSrc);
		if( srcType == HoldAreaProc )
		{
			FillPollygon( _pSource->serviceLocationLength(), _pSource->serviceLocationPath()->getPointList(), _altSrc );
		}
		
	}
	else if(srcType == MovingSideWalkProc) {
		int c = _pSource->serviceLocationLength();
		CreatePointToPointGeometry(ARCPoint2(_pSource->serviceLocationPath()->getPoint(c-1)), _altSrc, _pt, _altPt);
	}
	else if(srcType == IntegratedStationProc) {
	}
}

//static void CreateSourceToDestGeometry(Processor* pSource, Processor* pDest, double* dAlt)
void CreateSourceToDestGeometry(Processor* pSource, Processor* pDest, double* dAlt)
{
	int srcType, dstType;
	srcType = pSource->getProcessorType();
	dstType = pDest->getProcessorType();
	int c;
	Point lastPt;
	BOOL bUseLastPoint = FALSE;
	c = pSource->outConstraintLength();
	if(c > 0) { //if src has out constraint
		CreateServLocToPointGeometry(pSource, dAlt[pSource->getFloor()/nFloorIdxScale], ARCVector2(pSource->outConstraint()->getPoint(0)), dAlt[pSource->getFloor()/nFloorIdxScale]);
		CreatePathGeometry(pSource->outConstraint(), dAlt[pSource->getFloor()/nFloorIdxScale]);
		lastPt = pSource->outConstraint()->getPoint(c-1);
		bUseLastPoint = TRUE;
	}
	c = pDest->inConstraintLength();
	if(c > 0) { //if dest has in constraint
		if(bUseLastPoint)
			CreatePointToPointGeometry(ARCPoint2(lastPt), dAlt[(int)lastPt.getZ()/nFloorIdxScale], ARCVector2(pDest->inConstraint()->getPoint(0)), dAlt[pDest->getFloor()/nFloorIdxScale]); //go from lastPt to first pt in inC of dest
		else
			CreateServLocToPointGeometry(pSource, dAlt[pSource->getFloor()/nFloorIdxScale],ARCVector2(pDest->inConstraint()->getPoint(0)), dAlt[pDest->getFloor()/nFloorIdxScale]);
		CreatePathGeometry(pDest->inConstraint(), dAlt[pDest->getFloor()/nFloorIdxScale]); //
		lastPt = pDest->inConstraint()->getPoint(c-1);
		bUseLastPoint = TRUE;
	}
	c = pDest->queueLength();
	if(c > 0) { //if dest has Q
		if(bUseLastPoint)
			CreatePointToPointGeometry(ARCVector2(lastPt), dAlt[(int)lastPt.getZ()/nFloorIdxScale], ARCVector2(pDest->queuePath()->getPoint(c-1)), dAlt[pDest->getFloor()/nFloorIdxScale]); //go from lastPt to first pt in inC of dest
		else
			CreateServLocToPointGeometry(pSource, dAlt[pSource->getFloor()/nFloorIdxScale], ARCVector2(pDest->queuePath()->getPoint(c-1)), dAlt[pDest->getFloor()/nFloorIdxScale]);
		CreatePathGeometry(pDest->queuePath(), dAlt[pDest->getFloor()/nFloorIdxScale]); //
		lastPt = pDest->queuePath()->getPoint(0);
		bUseLastPoint = TRUE;
	}
	if(bUseLastPoint)
		CreateServLocToPointGeometry(pDest, dAlt[pDest->getFloor()/nFloorIdxScale], ARCVector2(lastPt), dAlt[(int)lastPt.getZ()/nFloorIdxScale]);
	else
		CreateServLocToServLocGeometry(pSource, pDest, dAlt);
}

static void DrawProcessToProcess( CSubFlow* _pFirstFlow, CSubFlow* _pSecondFlow, double* _pdAlt, BOOL* _pBOn )
{
	InputTerminal* pTerm = _pFirstFlow->GetInternalFlow()->GetInputTerm();

	std::vector<ProcessorID> vLeafNode;
	_pFirstFlow->GetInternalFlow()->GetLeafNode( vLeafNode );
	
	std::vector<ProcessorID> vRoot;
	_pSecondFlow->GetInternalFlow()->GetRootProc( vRoot );

	int iRootCount = vRoot.size();
	int iLeafCount = vLeafNode.size();
	for( int i=0; i<iLeafCount; ++i )
	{
		GroupIndex gidx = pTerm->procList->getGroupIndex( vLeafNode[ i ] );
		if(gidx.start == -1 || gidx.end == -1)
			continue;
		for( int j=gidx.start; j<=gidx.end; j++) 
		{
			if(j==START_PROCESSOR_INDEX || j==TO_GATE_PROCESS_INDEX
				|| j==FROM_GATE_PROCESS_INDEX || j == BAGGAGE_DEVICE_PROCEOR_INDEX )
				continue;
			
			Processor* procSource = pTerm->procList->getProcessor( j );
			int nIndexInFloor=procSource->getFloor()/nFloorIdxScale;
			if(!_pBOn[nIndexInFloor]) 
				continue;

			for( int iDest=0; iDest<iRootCount; ++iDest )
			{
				GroupIndex gidxDest = pTerm->procList->getGroupIndex( vRoot[ iDest ] );

				if( gidxDest.start == -1 || gidxDest.end == -1)
					continue;

				if(gidxDest.start == -1 || gidxDest.end == -1 || 
					gidxDest.start==TO_GATE_PROCESS_INDEX || 
					gidxDest.start==FROM_GATE_PROCESS_INDEX ||
					gidxDest.start==BAGGAGE_DEVICE_PROCEOR_INDEX ||
					gidxDest.end==TO_GATE_PROCESS_INDEX || 
					gidxDest.end==FROM_GATE_PROCESS_INDEX ||
					gidxDest.end==BAGGAGE_DEVICE_PROCEOR_INDEX )						
					continue;

				for( int iDestProc=gidxDest.start; iDestProc<=gidxDest.end; ++iDestProc )
				{
					Processor* procDest = pTerm->procList->getProcessor( iDestProc );
					nIndexInFloor=procDest->getFloor()/nFloorIdxScale;
					if( !_pBOn[nIndexInFloor] ) 
						continue;
					CreateSourceToDestGeometry(procSource,procDest,_pdAlt);				
				}
			}
		}
	}	
}

static void DrawProcessorToProcess( Processor* _pProc, CSubFlow* _pFlow, double* _pdAlt ,BOOL* _pBOn ,bool _bIsOneToOne)
{
	std::vector<ProcessorID> vRoot;
	_pFlow->GetInternalFlow()->GetRootProc( vRoot );
	int iRootCount = vRoot.size();
	InputTerminal* pTerm = _pFlow->GetInternalFlow()->GetInputTerm();
	for( int i=0; i<iRootCount; ++i )
	{
		GroupIndex gidxDest = pTerm->procList->getGroupIndex( vRoot[ i ] );
		if(gidxDest.start == -1 || gidxDest.end == -1 || 
			gidxDest.start==TO_GATE_PROCESS_INDEX || 
			gidxDest.start==FROM_GATE_PROCESS_INDEX ||
			gidxDest.start==BAGGAGE_DEVICE_PROCEOR_INDEX ||
			gidxDest.end==TO_GATE_PROCESS_INDEX || 
			gidxDest.end==FROM_GATE_PROCESS_INDEX ||
			gidxDest.end==BAGGAGE_DEVICE_PROCEOR_INDEX )						
		continue;

		
		for(int l = gidxDest.start; l<=gidxDest.end; l++) 
		{
			if(l==END_PROCESSOR_INDEX )
				continue;
			Processor* procDest = pTerm->procList->getProcessor(l);
			if( _bIsOneToOne )
			{			
				if( _pProc->getID()->GetLeafName() != procDest->getID()->GetLeafName() )
				{
					continue;
				}
			}
			
			
			CreateSourceToDestGeometry(_pProc,procDest,_pdAlt);
		}
	}
}

static void DrawProcessToProcessor(CSubFlow* _pFlow ,  const ProcessorID& _procID, double* _pdAlt, BOOL* _pBOn, bool _bIsOneToOne )
{
	std::vector<ProcessorID> vLeafNode;
	_pFlow->GetInternalFlow()->GetLeafNode( vLeafNode );
	InputTerminal* pTerm = _pFlow->GetInternalFlow()->GetInputTerm();
	int iLeafCount = vLeafNode.size();
	for( int i=0; i<iLeafCount; ++i )
	{
		GroupIndex gidx = pTerm->procList->getGroupIndex( vLeafNode[ i ] );
		if(gidx.start == -1 || gidx.end == -1)
			continue;
		for( int j=gidx.start; j<=gidx.end; j++) 
		{
			if(j==START_PROCESSOR_INDEX || j==TO_GATE_PROCESS_INDEX
				|| j==FROM_GATE_PROCESS_INDEX || j == BAGGAGE_DEVICE_PROCEOR_INDEX 
				|| j == END_PROCESSOR_INDEX )
				continue;
			
			Processor* procSource = pTerm->procList->getProcessor( j );
			int nIndexInFloor=procSource->getFloor()/nFloorIdxScale;
			if(!_pBOn[nIndexInFloor]) 
				continue;

			GroupIndex gDestGroup = pTerm->procList->getGroupIndex( _procID );
			
			if(gDestGroup.start == -1 || gDestGroup.end == -1)
				continue;
			for( int iDest=gDestGroup.start; iDest<= gDestGroup.end; ++iDest )
			{
				if( iDest==START_PROCESSOR_INDEX || iDest==TO_GATE_PROCESS_INDEX
					|| iDest ==FROM_GATE_PROCESS_INDEX || iDest == BAGGAGE_DEVICE_PROCEOR_INDEX
					|| iDest == END_PROCESSOR_INDEX)
					continue;

				Processor* procDest = pTerm->procList->getProcessor( iDest );
				nIndexInFloor=procDest->getFloor()/nFloorIdxScale;
				if( !_pBOn[nIndexInFloor] ) 
					continue;
				if( _bIsOneToOne )
				{
					if( procSource->getID()->GetLeafName() != procDest->getID()->GetLeafName() )
					{
						continue;
					}
				}
				
				CreateSourceToDestGeometry(procSource,procDest,_pdAlt);				
			}						
		}			
	}	
}

#endif // include guard