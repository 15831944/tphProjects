#include "StdAfx.h"
#include ".\stopsignproc.h"
#include "../Main/ControlDeviceRenderer.h"

const CString StopSignProc::m_strTypeName = "StopSign";

StopSignProc::StopSignProc(void)
{
	
}

StopSignProc::~StopSignProc(void)
{
	
}
LandfieldProcessor * StopSignProc::GetCopy()
{
	LandfieldProcessor * newProc = new StopSignProc;
	newProc->SetPath(GetPath());
	return newProc;
}
