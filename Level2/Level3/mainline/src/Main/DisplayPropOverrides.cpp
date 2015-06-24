// DisplayPropOverrides.cpp: implementation of the CDisplayPropOverrides class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "termplan.h"
#include "DisplayPropOverrides.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDisplayPropOverrides::CDisplayPropOverrides() :
	DataSet(DisplayPropOverridesFile,2.3f)
{
	memset(m_pbProcDisplayOverrides, 0, 7*sizeof(BOOL));
}


CDisplayPropOverrides::~CDisplayPropOverrides()
{
}

void CDisplayPropOverrides::clear()
{
	memset(m_pbProcDisplayOverrides, 0, 7*sizeof(BOOL));
}

void CDisplayPropOverrides::readData(ArctermFile& p_file)
{
	p_file.getLine();
	for(int i=0; i<7; i++) {
		p_file.getInteger(m_pbProcDisplayOverrides[i]);
	}
}

//void CDisplayPropOverrides::readObsoleteData(ArctermFile& p_file)
//{
//}

void CDisplayPropOverrides::writeData(ArctermFile& p_file) const
{
	for(int i=0; i<7; i++) {
		p_file.writeInt(m_pbProcDisplayOverrides[i]);
	}
	p_file.writeLine();
}

void CDisplayPropOverrides::readObsoleteData( ArctermFile& p_file )
{
	for(int i=0; i<6; i++) {
		p_file.writeInt(m_pbProcDisplayOverrides[i]);
	}
	p_file.writeLine();
}
