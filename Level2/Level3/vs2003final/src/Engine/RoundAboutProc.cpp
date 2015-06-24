#include "StdAfx.h"
#include ".\roundaboutproc.h"
#include "../Main/RoundAboutRenderer.h"
#include "../Engine/StretchProc.h"
#include <Common/ShapeGenerator.h>
#include "./Common/pollygon.h"

const CString RoundAboutProc::m_strTypeName = "RoundAbout";

RoundAboutProc::RoundAboutProc(void)
{
	m_dLaneWidth = 3*SCALE_FACTOR;
	m_nLaneNum = 1;
	
}

RoundAboutProc::~RoundAboutProc(void)
{
}

int RoundAboutProc::readSpecialField(ArctermFile& procFile)
{
	
	int _integer; double _float; 
	char str[256];
	//get lane number
	procFile.getInteger(_integer);
	SetLaneNum(_integer);
	//get lane width
	procFile.getFloat(_float);
	SetLaneWidth(_float);
	//get linking processors
	procFile.getInteger(_integer);
	int m_nCount = _integer;
	m_linkedProcs.reserve(m_nCount);
	for(int i=0;i<m_nCount;i++ )
	{
		LandProcPart newPart;
		procFile.getField(str,256);
		newPart.procName = str;
		procFile.getInteger(_integer);
		newPart.part = _integer;
		m_linkedProcs.push_back( newPart );
	}
	return TRUE;
}

int RoundAboutProc::writeSpecialField(ArctermFile& procFile)const
{
	//write lanenum;
	procFile.writeInt( GetLaneNum() );
	//write lane width;
	procFile.writeFloat( (float)GetLaneWidth() );
	//write linking processors
	int nLinkprocCnt =(int) m_linkedProcs.size();
	procFile.writeInt(nLinkprocCnt);

	for(int i=0;i<nLinkprocCnt;i++)
	{
		procFile.writeField( m_linkedProcs[i].pProc->getIDName() );
		procFile.writeInt( m_linkedProcs[i].part);
	}
	return TRUE;
}

bool RoundAboutProc::HasProperty(ProcessorProperty propType)const
{
	if(propType == LandfieldProcessor::PropLaneNumber 
		|| propType == LandfieldProcessor::PropLaneWidth
		|| propType == LandfieldProcessor::PropLinkingProces)
	return true;
	return false;
}

ARCPath3 RoundAboutProc::GetPath() const
{ 
	LandProcPartArray linkProcs  = GetLinkedProcs();
	ARCPath3 smoothCtrlPts; //smoothCtrlPts.reserve( (smoothCnt + 2 ) * ctrlpoints.size() );

	if(linkProcs.size()<2) return smoothCtrlPts;
	LandProcPartArray::iterator itr;	
	//check to see the linked procs are in a counterclockwise order
	int ncount = (int)linkProcs.size();
	Pollygon poly;
	Point * pts = new Point[linkProcs.size()];	

	for(int i = 0;i< ncount;i++)
	{
		LandfieldProcessor* pStretch  = (LandfieldProcessor*) linkProcs[i].pProc;
		const ARCPath3& stretchpath = pStretch->GetPath();

		if( linkProcs[i].part / 2 >= 1 )
		{
			int pcount = (int) stretchpath.size();
			pts[i] = Point( stretchpath[pcount-1][VX],stretchpath[pcount-1][VY],stretchpath[pcount-1][VZ] );
		}
		else 
		{
			pts[i] = Point ( stretchpath[0][VX],stretchpath[0][VY],stretchpath[0][VZ]);
		}  
	}
	poly.init(linkProcs.size(),pts);
	delete[] pts;
	bool bcountclk= poly.IsCountClock();
	if(!bcountclk)
	{	
		std::reverse(linkProcs.begin(),linkProcs.end());
	}
	//get control points
	double rbhalfwidth = 0.5 * GetLaneNum() * GetLaneWidth();
	//ARCVector3 reservePt;
	ARCPath3 ctrlpoints;
	ctrlpoints.reserve( linkProcs.size() * 2 );	

	for( itr = linkProcs.begin();itr!=linkProcs.end();itr++ )
	{
		StretchProc * pStretchProc = (StretchProc* ) (*itr).pProc;

		if(!pStretchProc)continue;
		const ARCPath3& stretchpath = pStretchProc->GetPath();
		ARCVector3 orien, startPt;
		if( (*itr).part / 2 >= 1 )
		{
			int pcount = (int) stretchpath.size();
			orien = ARCVector3( stretchpath[pcount-2] ,stretchpath[pcount-1] );
			startPt = stretchpath[pcount-1];
		}
		else 
		{
			orien = ARCVector3(stretchpath[1],stretchpath[0]);
			startPt = stretchpath[0];
		}
		orien.Normalize();	

		double Stretchhalfwidth =0.5* pStretchProc->GetLaneWidth() * pStretchProc->GetLaneNum();

		//startPt += orien * rbhalfwidth;

		ARCVector3 orienWidth = ARCVector3( orien[VY],-orien[VX], 0 );
		orien[VZ]=0;	

		ctrlpoints.push_back( orien*rbhalfwidth + startPt -  orienWidth * Stretchhalfwidth ); 
		ctrlpoints.push_back( orien*rbhalfwidth + startPt +  Stretchhalfwidth * orienWidth ); 
	}

	const static int smoothCnt = 10;
	int ctrptsCnt =(int) ctrlpoints.size();
	smoothCtrlPts.push_back(ctrlpoints[0]);
	for(int i=0;i<ctrptsCnt / 2;i++ )
	{
		ARCVector3 v1,v2,v3,p1,p2;

		p1 = ctrlpoints[2*i+1];
		if(i<ctrptsCnt / 2-1)
			p2 = ctrlpoints[2*i+2];
		else 
			p2 = ctrlpoints[0];

		v1 = ARCVector3(ctrlpoints[i*2],ctrlpoints[i*2+1]); v1.Normalize();			

		if(i<ctrptsCnt / 2-1)
			v2 = ARCVector3(ctrlpoints[i*2+3],ctrlpoints[i*2+2]);
		else 
			v2 = ARCVector3(ctrlpoints[1],ctrlpoints[0]);
		v2.Normalize();

		//v1 = ARCVector3(ctrlpoints[i*2],ctrlpoints[i*2+1]); v2 = ARCVector3(ctrlpoints[i*2+3],ctrlpoints[i*2+2]);
		v3 = ARCVector3(p1,p2);

		double ratio = v1.GetCosOfTwoVector(v2);    
		ratio =sqrt( 1 - ratio * ratio ) ;
		if(ratio <1 )ratio =1;
		ratio = v3.Magnitude() / ratio;               // length/sin(a)


		double d1,d2;
		d1 = v2.GetCosOfTwoVector(v3);    //cos(a)
		d1 = sqrt(1- d1*d1);              //sin(a)
		d1 = d1 * ratio;

		d2 = v1.GetCosOfTwoVector(v3);
		d2 = sqrt(1-d2*d2);
		d2 = d2 * ratio;

		v1 *= d1; v2 *= d2;		

		std::vector<ARCVector3> _inpoint;
		std::vector<ARCVector3> _outpoint;
		_inpoint.reserve(3);_outpoint.reserve(smoothCnt);
		_inpoint.push_back( p1 ); _inpoint.push_back( (p1+v1+p2+v2)*0.5 );
		_inpoint.push_back( p2 );

		//for(int j=0;j<3;j++)
		//smoothCtrlPts.push_back(_inpoint[j]);
		SHAPEGEN.GenBezierPath(_inpoint,_outpoint,smoothCnt);
		for(int j=0;j<smoothCnt;j++)
			smoothCtrlPts.push_back( _outpoint[j] ); 
	}
	return smoothCtrlPts;
}