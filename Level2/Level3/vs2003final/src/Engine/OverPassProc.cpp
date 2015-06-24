#include "StdAfx.h"
#include ".\overpassproc.h"
const CString OverPassProc::m_strTypeName = "OverPass";

OverPassProc::OverPassProc(void)
{
	m_dclearance = 3 * SCALE_FACTOR;   //3m;
}

OverPassProc::~OverPassProc(void)
{
}

int OverPassProc::readSpecialField(ArctermFile& procFile)
{
return TRUE;
}

int OverPassProc::writeSpecialField(ArctermFile& procFile)const
{

return TRUE;
}


bool OverPassProc::HasProperty(ProcessorProperty propType)
{
	if(propType == LandfieldProcessor::PropClearance )
		return true;
	return false;
}
