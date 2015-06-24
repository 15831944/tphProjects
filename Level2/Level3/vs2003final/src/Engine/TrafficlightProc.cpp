#include "StdAfx.h"
#include ".\trafficlightproc.h"

const CString TrafficlightProc::m_strTypeName = "TrafficLight";

TrafficlightProc::TrafficlightProc(void)
{
	
}

TrafficlightProc::~TrafficlightProc(void)
{
	
}
 LandfieldProcessor * TrafficlightProc::GetCopy()
{
	LandfieldProcessor * newProc = new TrafficlightProc;
	newProc->SetPath(GetPath());
	return newProc;
}