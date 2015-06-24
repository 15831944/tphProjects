#include "StdAfx.h"
#include ".\holdproc.h"
#include ".\FixProc.h"
#include "../Common/path.h"
#include "../Common/ARCUnit.h"
#include "../Inputs/AirsideInput.h"
#include <Inputs/IN_TERM.H>

const static int NcirclePts = 6;

HoldProc::HoldProc(void)
{
	m_location.init(1);
	m_strFixname = "";
	m_inBoundDegree = 30;
	m_outBoundLegMin = 0;
	m_outBoundLegNm = 0;
	m_brightTurn = true;
	m_maxAlt = 0;
	m_minAlt = 0;
	m_pfix = NULL;
}

HoldProc::~HoldProc(void)
{
}

int HoldProc::readSpecialField(ArctermFile& procFile){
	char strtext[1024];
	int i;
	procFile.getField(strtext,1024);
	m_strFixname = strtext;
	procFile.getInteger(m_inBoundDegree);
	procFile.getFloat(m_outBoundLegNm);
	procFile.getFloat(m_outBoundLegMin);
	procFile.getInteger(i);
	m_brightTurn = i==0?true:false;
	procFile.getFloat(m_maxAlt);
	procFile.getFloat(m_minAlt);

	return TRUE;
}

int HoldProc::writeSpecialField(ArctermFile& procFile)const
{
	procFile.writeField(m_strFixname);
	procFile.writeInt(m_inBoundDegree);
	procFile.writeFloat(float(m_outBoundLegNm));
	procFile.writeFloat(float(m_outBoundLegMin));
	procFile.writeInt( m_brightTurn?0:1);
	procFile.writeFloat(float(m_maxAlt));
	procFile.writeFloat(float(m_minAlt));
	return TRUE;
}

Path HoldProc::getPath()const{
	
	Path reslt;
	if(!getFix())return reslt;
	Point ptfix = getFix()->getServicePoint(0);
	//double velocity = ARCUnit::ConvertLength(getOutBoundLegNm(),ARCUnit::NM)/ARCUnit::ConvertTime(getOutBoundLegMin(),ARCUnit::MINUS);

	//caculate R
	double radius = 200 * SCALE_FACTOR;
	Point v(0,-1.0,0);Point vhead;
	v.rotate(getInBoundDegree()-180); vhead = v;
	
	v.rotate(-90);
	v.length(radius);
	vhead.length( getOutBoundLegNm() );
	Point p1 =isRightTurn()? (ptfix - v):(ptfix+v);
	Point p2 = p1 - Point(vhead.getX(),vhead.getY(),0);

	Point vrotate = v;

	reslt.init(2*NcirclePts);
	for(int i=0;i<NcirclePts;i++){
		reslt[i] = p1 + vrotate;		
		reslt[i+NcirclePts] = p2 - vrotate;
		vrotate.rotate(180/(NcirclePts-1) );
	}
	
	
	return reslt;
	
}

void HoldProc::RefreshRelateFix(ProcessorList* proclist){
	m_pfix = (FixProc*) proclist->getProcessor(m_strFixname);
}

void HoldProc::RefreshRelatedData( InputTerminal* _pInTerm ){
	m_pfix  = (FixProc*) _pInTerm->GetAirsideInput()->GetProcessorList()->getProcessor(m_strFixname);
}