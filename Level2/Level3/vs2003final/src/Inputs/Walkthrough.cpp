// Walkthrough.cpp: implementation of the CWalkthrough class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Walkthrough.h"
#include "common\ARCVector.h"
#include "common\TERMFILE.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

static const ARCVector3 TERMINAL_OFFSETS[] = {
	ARCVector3(0.0,0.0,200.0),
	ARCVector3(-5000.0,-5000.0,2000.0),
};

static const ARCVector3 TERMINAL_DIRECTIONS[] = {
	ARCVector3(100.0,100.0,-20.0),
	ARCVector3(100.0,100.0,-35.0),
};

static const ARCVector3 AIRSIDE_OFFSETS[] = {
	ARCVector3(1000.0,0.0,2000.0),
	ARCVector3(-10000.0,0.0,3000.0),
};

static const ARCVector3 AIRSIDE_DIRECTIONS[] = {
	ARCVector3(100.0,100.0,-20.0),
	ARCVector3(100.0,100.0,-20.0),
};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CWalkthrough::CWalkthrough()
{
	m_nPaxID = 0;
	m_viewpoint = THIRDPERSON;
	m_nStartTime = 0;
	m_nEndTime = 0;
	m_eEnvMode = EnvMode_Unknow;
}

CWalkthrough::CWalkthrough(EnvironmentMode envmode)
{
	m_nPaxID = 0;
	m_viewpoint = THIRDPERSON;
	m_nStartTime = 0;
	m_nEndTime = 0;
	m_eEnvMode = envmode;
	switch(envmode) 
	{
		case EnvMode_Terminal:
			m_sPaxIDDesc = _T("NO PAX SELECTED");
			break;
		case EnvMode_AirSide:
			m_sPaxIDDesc = _T("NO FLIGHT SELECTED");
			break;
		case EnvMode_LandSide:
			m_sPaxIDDesc = _T("NO VEHICLE SELECTED");
			break;
		default:
			{
				ASSERT(0);
				break;
			}
	}
}

CWalkthrough::~CWalkthrough()
{

}

CWalkthrough* CWalkthrough::CreateFromFile(ArctermFile& _file)
{
	CWalkthrough* pWT = new CWalkthrough();
	char s[256];
	long l;
	int i;
	_file.getLine();
	_file.getInteger(l);
	pWT->m_nPaxID = l;
	_file.getField(s,255);
	pWT->m_sPaxIDDesc = s;
	pWT->m_sPaxIDDesc.Replace("\\./", ",");
	_file.getInteger(i);
	pWT->m_viewpoint = (VIEWPOINT)i;
	_file.getInteger(i);
	pWT->m_eEnvMode = (EnvironmentMode)i;
	_file.getInteger(pWT->m_nStartTime);
	_file.getInteger(pWT->m_nEndTime);
	return pWT;
}

CWalkthrough* CWalkthrough::CreateFromObsoleteFile(ArctermFile& _file)
{
	CWalkthrough* pWT = new CWalkthrough();
	char s[256];
	long l;
	int i;
	_file.getLine();
	_file.getInteger(l);
	pWT->m_nPaxID = l;
	_file.getField(s,255);
	pWT->m_sPaxIDDesc = s;
	pWT->m_sPaxIDDesc.Replace("\\./", ",");
	_file.getInteger(i);
	pWT->m_viewpoint = (VIEWPOINT)i;
	pWT->m_eEnvMode = EnvMode_Terminal;
	_file.getInteger(pWT->m_nStartTime);
	_file.getInteger(pWT->m_nEndTime);
	return pWT;
}

void CWalkthrough::WriteToFile(const CWalkthrough& _wt, ArctermFile& _file)
{
	_file.writeInt((long)_wt.m_nPaxID);
	CString temp(_wt.m_sPaxIDDesc);
	temp.Replace(",", "\\./");
	_file.writeField(temp);
	_file.writeInt((int)_wt.m_viewpoint);
	_file.writeInt((int)_wt.m_eEnvMode);
	_file.writeInt(_wt.m_nStartTime);
	_file.writeInt(_wt.m_nEndTime);
	_file.writeLine();
}

const ARCVector3& CWalkthrough::GetViewpointOffset() const
{
	if(EnvMode_Terminal == m_eEnvMode)
		return TERMINAL_OFFSETS[m_viewpoint];
	else if(EnvMode_AirSide == m_eEnvMode)
		return AIRSIDE_OFFSETS[m_viewpoint];
	else 
	{
		ASSERT(FALSE);
		return TERMINAL_OFFSETS[m_viewpoint];
	}
}

const ARCVector3& CWalkthrough::GetViewpointDirection() const
{
	if(EnvMode_Terminal == m_eEnvMode)
		return TERMINAL_DIRECTIONS[m_viewpoint];
	else if(EnvMode_AirSide == m_eEnvMode)
		return AIRSIDE_DIRECTIONS[m_viewpoint];
	else 
	{
		ASSERT(FALSE);
		return TERMINAL_DIRECTIONS[m_viewpoint];
	}

}

UINT CWalkthrough::GetPaxID() const
{
	return m_nPaxID;
}
