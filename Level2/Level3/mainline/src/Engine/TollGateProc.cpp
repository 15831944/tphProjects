#include "StdAfx.h"
#include ".\tollgateproc.h"

const CString TollGateProc::m_strTypeName = "TollGate";

TollGateProc::TollGateProc(void)
{

}

TollGateProc::~TollGateProc(void)
{
	
}

LandfieldProcessor * TollGateProc::GetCopy()
{
	LandfieldProcessor * newProc = new TollGateProc;
	newProc->SetPath(GetPath());
	return newProc;
}
