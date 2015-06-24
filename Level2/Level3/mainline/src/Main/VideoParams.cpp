// VideoParams.cpp: implementation of the CVideoParams class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "termplan.h"
#include "VideoParams.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CVideoParams::CVideoParams() : DataSet( VideoParamsFile, 2.3f )
{
	m_nFPS = 20;
	m_nQuality = 100;
	m_nWidth = 640;
	m_nHeight = 480;
	m_bAutoFileName = TRUE;
	m_sFileName = "AUTO";
}

CVideoParams::~CVideoParams()
{
}

void CVideoParams::clear()
{
}

void CVideoParams::readObsoleteData(ArctermFile& p_file)
{
	m_nFPS = 20;
	m_nQuality = 100;
	m_nWidth = 640;
	m_nHeight = 480;
	m_bAutoFileName = TRUE;
	m_sFileName = "AUTO";
}

void CVideoParams::readData (ArctermFile& p_file)
{
	int i;
	char buf[256];
	p_file.getLine();
	p_file.getInteger(i);
	m_bAutoFileName = (BOOL) i;
	p_file.getField(buf, 255);
	m_sFileName = buf;
	p_file.getInteger(m_nWidth);
	p_file.getInteger(m_nHeight);
	p_file.getInteger(m_nFPS);
	p_file.getInteger(m_nQuality);

}

void CVideoParams::writeData (ArctermFile& p_file) const
{
	p_file.writeInt((int) m_bAutoFileName);
	p_file.writeField(m_sFileName);
	p_file.writeInt(m_nWidth);
	p_file.writeInt(m_nHeight);
	p_file.writeInt(m_nFPS);
	p_file.writeInt(m_nQuality);
	p_file.writeLine();
}