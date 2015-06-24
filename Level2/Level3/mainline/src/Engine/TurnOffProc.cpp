#include "StdAfx.h"
#include ".\turnoffproc.h"
#include "../Main/TurnOffRenderer.h"
const CString TurnOffProc::m_strTypeName = "TurnOff";

TurnOffProc::TurnOffProc(void)
{
	m_dLaneWidth = 3 * SCALE_FACTOR;
	//m_pRenderer = new TurnOffRenderer(this);
}

TurnOffProc::~TurnOffProc(void)
{
}

bool TurnOffProc::HasProperty(ProcessorProperty propType) const
{
	if(propType == LandfieldProcessor::PropControlPoints 
		|| propType == LandfieldProcessor::PropLaneWidth
		|| propType == LandfieldProcessor::PropLinkingProces
		)
		return true;
	return false;
}
