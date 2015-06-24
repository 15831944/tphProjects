#include "StdAfx.h"
#include ".\yieldsignproc.h"


const CString YieldSignProc::m_strTypeName = "YieldSign";

YieldSignProc::YieldSignProc(void)
{
}

YieldSignProc::~YieldSignProc(void)
{
}
LandfieldProcessor * YieldSignProc::GetCopy() 
{
	LandfieldProcessor * newProc = new YieldSignProc;
	newProc->SetPath(GetPath());
	return newProc;
}