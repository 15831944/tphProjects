#include "StdAfx.h"
#include ".\stretchproc.h"
#include "../main/StretchRenderer.h"
#include "../Common/math/tCubicSpline.h"
#include <Common/ShapeGenerator.h>
/*
The path store the path relative to the floor it belongs to 

*/

const CString StretchProc::m_strTypeName = "Stretch";
const  double StretchProc::lanesidewidth = 100;
const  double StretchProc::lanethickness = 75;
const int StretchProc::interpoltCnt = 9;

StretchProc::StretchProc(void)
{
	//m_pRenderer = new StretchRenderer(this);
	m_dLaneWidth = 5*SCALE_FACTOR;
	m_nLaneNum = 1;
}

StretchProc::~StretchProc(void)
{
}

//ARCPath3 StretchProc::GetSidePath(bool bleft)
//{
//			
//}

LandfieldProcessor* StretchProc::GetCopy()
{
	LandfieldProcessor * newProc = new StretchProc;
	newProc->SetPath(GetPath());
	newProc->SetLaneNum(GetLaneNum());
	newProc->SetLaneWidth(GetLaneWidth());
	return newProc;
}

bool StretchProc::HasProperty(ProcessorProperty propType)const
{
	if(   propType == LandfieldProcessor::PropControlPoints 
		|| propType == LandfieldProcessor::PropLaneNumber
		|| propType == LandfieldProcessor::PropLaneWidth
		|| propType == LandfieldProcessor::PropLaneDir)
	return true;
	return false;
}

ShapeGenerator::WidthPipePath StretchProc::getStretchPipePath()
{
	//ASSERT( laneNumber< GetLaneNum() );
	if(IsNeedSync())DoSync();
	return m_widthPipePath;	
}

ARCPath3 StretchProc::getLanePath(int n){
	ASSERT(n<GetLaneNum());
	ShapeGenerator::WidthPipePath stretchpipepath = getStretchPipePath();
	double len =  GetLaneNum() * GetLaneWidth();
	//SHAPEGEN.GenWidthPipePath(GetPath(),GetLaneNum()*GetLaneWidth(),stretchpipepath);
	ARCPath3 ret;
	int nptCnt = (int)stretchpipepath.leftpath.size();
	ret.resize(stretchpipepath.leftpath.size());
	for(int i=0;i<nptCnt;i++){
		double t = (double(n)  +0.5)/GetLaneNum();
		ret[i] = stretchpipepath.leftpath[i] *(1-t) + stretchpipepath.rightpath[i] * t;
	}

	return ret;
	
}
void StretchProc::DoSync(){
	ShapeGenerator::WidthPipePath stretchpipepath;

	double len =  GetLaneNum() * GetLaneWidth();

	//SHAPEGEN.GenWidthPipePath(GetPath(),GetLaneNum()*GetLaneWidth(),stretchpipepath);

	typedef tCubicSpline<ARCPath3::value_type> CubicSpline;

	CubicSpline splineC;

	ARCPath3 path = GetPath();
	splineC.SetControlPoints(path.begin(),path.end());

	//CubicSpline splineL ,splineR;
	//splineL.SetControlPoints(stretchpipepath.leftpath.begin(),stretchpipepath.leftpath.end());
	//splineR.SetControlPoints(stretchpipepath.rightpath.begin(),stretchpipepath.rightpath.end());


	//ShapeGenerator::WidthPipePath lanepipepath;
	//int nPtCnt = stretchpipepath.leftpath.size(); 
	int nPtCnt =(int) path.size();
	int nPtCntNew = interpoltCnt * (nPtCnt-1) +1;
	//lanepipepath.leftpath.resize(nPtCntNew); lanepipepath.rightpath.resize(nPtCntNew);
	path.resize(nPtCntNew);
	for(int i=0;i<nPtCntNew;i++){
		float t = (float)i/interpoltCnt;
		path[i] = splineC.eval(t);
		//lanepipepath.leftpath[i] =(splineL.eval(t));
		//lanepipepath.rightpath[i] = (splineR.eval(t));		
	}	

	/*
	for(int i =0 ;i<nPtCntNew;++i){
	ARCVector3 v  = lanepipepath.leftpath[i] - lanepipepath.rightpath[i];
	ARCVector3 center = 0.5 * ( lanepipepath.leftpath[i] + lanepipepath.rightpath[i]);
	v.length(len);
	lanepipepath.leftpath[i] = center - 0.5 * v;
	lanepipepath.rightpath[i] = center + 0.5 * v;
	}*/
	SHAPEGEN.GenWidthPipePath(path,GetLaneNum()*GetLaneWidth(),stretchpipepath);

	m_widthPipePath = stretchpipepath;
	SyncDateType::DoSync();
}