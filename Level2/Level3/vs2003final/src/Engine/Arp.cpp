// Arp.cpp: implementation of the Arp class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "Arp.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ArpProc::ArpProc()
{
	memset(m_sLatlong, 0, sizeof(m_sLatlong));
}

ArpProc::~ArpProc()
{

}

int ArpProc::readSpecialField(ArctermFile& procFile)
{
	//lat-long
	char sBuf[256] = "";
	procFile.getField( sBuf, 256);
	m_sLatlong[0] = sBuf;
	procFile.getField( sBuf, 256);
	m_sLatlong[1] = sBuf;


	return TRUE;
}
int ArpProc::writeSpecialField(ArctermFile& procFile) const
{
	//lat-long
	procFile.writeField( m_sLatlong[0].c_str());
	procFile.writeField( m_sLatlong[1].c_str());
	
	return TRUE;
}