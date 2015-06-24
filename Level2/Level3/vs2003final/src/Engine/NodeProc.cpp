// NodeProc.cpp: implementation of the NodeProc class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "NodeProc.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

NodeProc::NodeProc()
{
	m_nNumber = -1;
}

NodeProc::~NodeProc()
{

}

int NodeProc::readSpecialField(ArctermFile& procFile)
{
	//number
	procFile.getInteger( m_nNumber );
	return TRUE;
}
int NodeProc::writeSpecialField(ArctermFile& procFile) const
{
	//number
	procFile.writeInt( m_nNumber );
	return TRUE;
}