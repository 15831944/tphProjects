#include "StdAfx.h"
#include ".\noseinparkingproc.h"
#include "./StretchProc.h"

const CString NoseInParkingProc::m_strTypeName = "NoseInParking";

NoseInParkingProc::NoseInParkingProc(void)
{
	m_nSpotNum = 5 ;
	m_dSpotWidth = 1.5 * SCALE_FACTOR;
	m_dSpotLen = 3 * SCALE_FACTOR;
	m_dSpotSlopAngle = 0;
}

NoseInParkingProc::~NoseInParkingProc(void)
{
}

bool NoseInParkingProc::HasProperty(ProcessorProperty propType) const
{
	if( propType == LandfieldProcessor::PropLinkingProces
		|| propType == LandfieldProcessor::PropControlPoints
		|| propType == LandfieldProcessor::PropSpotNumber
		|| propType == LandfieldProcessor::PropSpotLength
		|| propType == LandfieldProcessor::PropSpotSlopAngle)
		return true;
	return false;

}

LandfieldProcessor * NoseInParkingProc::GetCopy()
{
	LandfieldProcessor * newProc = new NoseInParkingProc;
	return newProc;
}

void NoseInParkingProc::GetParkingPath( ARCPath3& closeStretchSide, ARCPath3& farToStretchSide ) const
{
	LandProcPartArray linkProcs = GetLinkedProcs();
	if(linkProcs.size() <2) return;
	LandProcPart stretchposFrom = linkProcs[0];
	LandProcPart stretchposTo = linkProcs[linkProcs.size()-1];

	if( stretchposFrom.pProc != stretchposTo.pProc )return;

	LandfieldProcessor * pProc  = (LandfieldProcessor*)stretchposFrom.pProc;
	
	int stretchPartFrom = stretchposFrom.part;
	int stretchPartTo = stretchposTo.part;

	ARCPath3 stretchPath;

	if( stretchPartFrom% 2 ==0 )
		stretchPath = ((StretchProc*)pProc)->getStretchPipePath().leftpath;
	else
		stretchPath = ((StretchProc*)pProc)->getStretchPipePath().rightpath;

	if(stretchPath.size()<3)
	{
		stretchPartFrom %= 2;
		stretchPartTo %= 2;
	}	

	ARCVector3 v1,v2;Point intertPtFrom,intertPtTo;
	ARCVector3 pFrom,pTo;

	ARCPath3 parkingPath = GetPath();
	pFrom = parkingPath[0];
	pTo = parkingPath[parkingPath.size()-1];

	v1 = stretchPath[stretchPartFrom/2];
	v2 = stretchPath[stretchPartFrom/2+1];

	intertPtFrom = Line( Point(v1[VX],v1[VY],v1[VZ]),Point(v2[VX],v2[VY],v2[VZ]) ).getProjectPoint(Point(pFrom[VX],pFrom[VY],pFrom[VZ]) );

	v1 = stretchPath[stretchPartTo/2];
	v2 = stretchPath[stretchPartTo/2+1];

	intertPtTo = Line( Point(v1[VX],v1[VY],v1[VZ]),Point(v2[VX],v2[VY],v2[VZ]) ).getProjectPoint(Point(pTo[VX],pTo[VY],pTo[VZ]) );

	double parkingFromDist = stretchPath.getPointDistance(ARCVector3(intertPtFrom));
	double parkingToDist = stretchPath.getPointDistance(ARCVector3(intertPtTo)) ;
	double parkinglen = parkingToDist - parkingFromDist ;
	int parkingNum = GetSpotNumber();
	double spotLen = parkinglen/parkingNum;



	ARCPath3 parkingLine;



	//get the control points
	for(int i=0;i<parkingNum+1;i++)
	{
		parkingLine.push_back( stretchPath.getDistancePoint(parkingFromDist + i * spotLen ));
	}	


	ShapeGenerator::WidthPipePath widePath;
	ShapeGenerator::GenWidthPipePath(parkingLine, GetSpotLength() * 2,widePath );
	ARCPath3 sidePath; sidePath.reserve(parkingLine.size());

	double rotateAngle = ARCMath::DegreesToRadians( GetSpotSlopAnlge() );

	if( (spotLen>0)^(stretchPartFrom%2 == 0) )
		sidePath = widePath.rightpath;
	else
		sidePath = widePath.leftpath;

	ARCVector3 v;
	for(int i=0;i<(int)parkingLine.size();i++)
	{
		v = sidePath[i] - parkingLine[i];
		v = ARCVector3( v[VX]*cos(rotateAngle) - v[VY]*sin(rotateAngle), v[VY]*cos(rotateAngle)+v[VX]*sin(rotateAngle),0);
		sidePath[i] = parkingLine[i] + v;
	}

	farToStretchSide = sidePath;
	closeStretchSide = parkingLine;

}

void NoseInParkingProc::GetParkingSpot( std::vector<ParkingSpot>& spots ) 
{
	ARCPath3 closePath, farPath;
	GetParkingPath(closePath,farPath);
	ASSERT( closePath.size() == farPath.size() );
	int nPtCnt = (int) closePath.size();
	
	LandProcPartArray linkProcs = GetLinkedProcs();
	if(linkProcs.size() <2) return;
	LandfieldProcessor * pProc = (LandfieldProcessor*)linkProcs[0].pProc;
	ASSERT(pProc->getProcessorType() == StretchProcessor );
	double laneWidth = pProc->GetLaneWidth();
	

	for(int i=0;i<nPtCnt-1;i++){
		ARCVector3 v1 = farPath[i] - closePath[i];
		ARCVector3 v2 = farPath[i+1] - closePath[i+1];
		ARCVector3 v3 = farPath[i+1] - farPath[i];
		ARCVector3 v4 = closePath[i+1]  - closePath[i];

		ParkingSpot spot;
		spot.pos = (farPath[i] + closePath[i] + farPath[i+1] + closePath[i+1]) * 0.25;
		spot.dir = ARCVector2(v1[VX]+v2[VX],v1[VY]+v2[VY]).Normalize();
		spot.linkProc = pProc;
		spot.width =  0.5 * ( v3.Length() + v4.Length() );
		spot.length  = 0.5 * ( v1.Length() + v2.Length() );
		spot.inPos = - ARCVector3(spot.dir[VX],spot.dir[VY],0) * laneWidth    + closePath[i] ;
		spot.outPos = spot.inPos;
		spots.push_back(spot);
	}
}

StretchProc * NoseInParkingProc::GetLinkProc()
{	
	if( m_linkedProcs.size() <2 ) return NULL;
	return (m_linkedProcs[0].pProc->getProcessorType() == StretchProcessor)  ? (StretchProc*)m_linkedProcs[0].pProc: NULL;
}