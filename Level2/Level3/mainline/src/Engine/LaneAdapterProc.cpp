#include "StdAfx.h"
#include ".\laneadapterproc.h"
//#include "../Main/LaneAdapterRenderer.h"

const CString LaneAdapterProc::m_strTypeName = "LaneAdapter";

LaneAdapterProc::LaneAdapterProc(void)
{
	
}

LaneAdapterProc::~LaneAdapterProc(void)
{
}

//int LaneAdapterProc::readSpecialField(ArctermFile& procFile)
//{
//	return TRUE;
//}
//
//int LaneAdapterProc::writeSpecialField(ArctermFile& procFile)const
//{
//	return TRUE;
//}

bool LaneAdapterProc::HasProperty(ProcessorProperty propType)const
{
	if(propType == LandfieldProcessor::PropLinkingProces)
	{
		return true;
	}
	return false;
}