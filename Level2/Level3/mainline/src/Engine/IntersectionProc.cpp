#include "StdAfx.h"
#include ".\intersectionproc.h"
#include "../Main/IntersectionRenderer.h"

const CString IntersectionProc::m_strTypeName = "Intersection";

IntersectionProc::IntersectionProc(void)
{
	
}

IntersectionProc::~IntersectionProc(void)
{
}
int IntersectionProc::readSpecialField(ArctermFile& procFile)
{
	int _integer; 
	char str[256];
	
	procFile.getInteger(_integer);
	int m_nCount = _integer;
	m_linkedProcs.reserve(m_nCount);
	for(int i=0;i<m_nCount;i++ )
	{
		LandProcPart newPart;
		procFile.getField(str,256);
		newPart.procName = str;
		procFile.getInteger(_integer);
		newPart.part = _integer;
		m_linkedProcs.push_back( newPart );
	}
	
	return TRUE;
}
int IntersectionProc::writeSpecialField(ArctermFile& procFile) const
{
	int nLinkprocCnt =(int) m_linkedProcs.size();
	procFile.writeInt(nLinkprocCnt);
	
	for(int i=0;i<nLinkprocCnt;i++)
	{
		procFile.writeField( m_linkedProcs[i].pProc->getIDName() );
		procFile.writeInt( m_linkedProcs[i].part);
	}
	return TRUE;
}

bool IntersectionProc::HasProperty(ProcessorProperty propType)const
{
	if( propType == LandfieldProcessor::PropLinkingProces)
		return true;
	return false;
}