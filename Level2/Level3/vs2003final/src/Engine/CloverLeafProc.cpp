#include "StdAfx.h"
#include ".\cloverleafproc.h"
#include "./StretchProc.h"

const CString CloverLeafProc::m_strTypeName = "CloverLeaf";

CloverLeafProc::CloverLeafProc(void)
{
}

CloverLeafProc::~CloverLeafProc(void)
{
}

bool CloverLeafProc::HasProperty(ProcessorProperty propType) const
{
	if(propType == LandfieldProcessor::PropLinkingProces
		|| propType == LandfieldProcessor::PropControlPoints)
	{
		return true;
	}
	return false;
}

void CloverLeafProc::GeneratePath()
{

}