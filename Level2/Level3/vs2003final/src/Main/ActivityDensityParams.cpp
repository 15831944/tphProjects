// ActivityDensityParams.cpp: implementation of the CActivityDensityParams class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "termplan.h"
#include <math.h>
#include "ActivityDensityParams.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

COLORREF GetDefaultColor(int n, int max)
{
	const int cMax[] = {9,0,178};
	const int cMid[] = {255,252,0};
	const int cMin[] = {255,2,0};
	int cRet[3];
	double d = ((double)n)/max;
	if(d<0.5) {
		d = d*2;
		cRet[0] = static_cast<int>((1-d)*cMin[0]+d*cMid[0]);
		cRet[1] = static_cast<int>((1-d)*cMin[1]+d*cMid[1]);
		cRet[2] = static_cast<int>((1-d)*cMin[2]+d*cMid[2]);
	}
	else if(d>0.5) {
		d = 2*(d-0.5);
		cRet[0] = static_cast<int>((1-d)*cMid[0]+d*cMax[0]);
		cRet[1] = static_cast<int>((1-d)*cMid[1]+d*cMax[1]);
		cRet[2] = static_cast<int>((1-d)*cMid[2]+d*cMax[2]);
	}
	else {
		cRet[0] = cMid[0];
		cRet[1] = cMid[1];
		cRet[2] = cMid[2];
	}

	return RGB(cRet[0],cRet[1],cRet[2]);
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CActivityDensityParams::CActivityDensityParams() : 
	DataSet(ActivityDensityFile),
	m_AOICenter(0.0,0.0),
	m_AOISize(50000.0,50000.0),
	m_AOISubdivs(100.0,100.0),
	m_nAOIFloor(0),
	m_dLOSMin(0.5),
	m_dLOSInterval(0.5),
	m_nLOSLevels(25)
{
	initDefaultValues();
}

CActivityDensityParams::~CActivityDensityParams()
{

}

void CActivityDensityParams::clear()
{
	m_vLOSColors.clear();
}

void CActivityDensityParams::initDefaultValues()
{
	m_vLOSColors.resize(m_nLOSLevels);
	for(int i=0; i<m_nLOSLevels; i++) {
		m_vLOSColors[i] = GetDefaultColor(i, m_nLOSLevels);
	}

}

void CActivityDensityParams::readData (ArctermFile& p_file)
{
	int n;
	long l;
	double d;
	p_file.getLine();
	p_file.getInteger(n);
	m_nAOIFloor = n;
	p_file.getFloat(d);
	m_AOICenter[VX] = d;
	p_file.getFloat(d);
	m_AOICenter[VY] = d;
	p_file.getFloat(d);
	m_AOISize[VX] = d;
	p_file.getFloat(d);
	m_AOISize[VY] = d;
	p_file.getFloat(d);
	m_AOISubdivs[VX] = d;
	p_file.getFloat(d);
	m_AOISubdivs[VY] = d;
	p_file.getFloat(d);
	m_dLOSMin = d;
	p_file.getFloat(d);
	m_dLOSInterval = d;
	p_file.getInteger(n);
	m_nLOSLevels = n;
	m_vLOSColors.resize(m_nLOSLevels);
	for(int i=0; i<m_nLOSLevels; i++) {
		p_file.getInteger(l);
		m_vLOSColors[i] = (COLORREF)l;
	}

}

void CActivityDensityParams::writeData (ArctermFile& p_file) const
{
	p_file.writeInt(m_nAOIFloor);
	p_file.writeFloat(static_cast<float>(m_AOICenter[VX]));
	p_file.writeFloat(static_cast<float>(m_AOICenter[VY]));
	p_file.writeFloat(static_cast<float>(m_AOISize[VX]));
	p_file.writeFloat(static_cast<float>(m_AOISize[VY]));
	p_file.writeFloat(static_cast<float>(m_AOISubdivs[VX]));
	p_file.writeFloat(static_cast<float>(m_AOISubdivs[VY]));
	p_file.writeFloat(static_cast<float>(m_dLOSMin));
	p_file.writeFloat(static_cast<float>(m_dLOSInterval));
	p_file.writeInt(m_nLOSLevels);
	for(int i=0; i<m_nLOSLevels; i++) {
		p_file.writeInt(static_cast<long>(m_vLOSColors[i]));
	}

	p_file.writeLine();
}

// Description: Load from default file
// Exception:	FileVersionTooNewError
void CActivityDensityParams::loadInputs( const CString& _csProjPath, InputTerminal* _pInTerm )
{
	this->SetInputTerminal( _pInTerm );
	this->loadDataSet( _csProjPath );
}

void CActivityDensityParams::saveInputs( const CString& _csProjPath, bool _bUndo )
{
	this->saveDataSet(_csProjPath, _bUndo);
}

COLORREF CActivityDensityParams::GetLOSColor(int level) const
{
	ASSERT(level >= 0 && level < static_cast<int>(m_vLOSColors.size()));
	return m_vLOSColors[level];
}

COLORREF CActivityDensityParams::GetLOSColor(double dLOS) const
{
	ASSERT(m_nLOSLevels > 1);
	if(dLOS < m_dLOSMin)
		return m_vLOSColors[0];
	if(dLOS > m_dLOSMin+(m_nLOSLevels-2)*m_dLOSInterval)
		return m_vLOSColors[m_nLOSLevels-1];

	dLOS = dLOS - m_dLOSMin;
	long idx = static_cast<long>(ceil(dLOS/m_dLOSInterval));
	if(idx >= m_nLOSLevels)
		idx = m_nLOSLevels-1;
	return m_vLOSColors[idx];
}

void CActivityDensityParams::SetAOISize(double x, double y)
{
	m_AOISize[VX] = x; m_AOISize[VY] = y;
}

void CActivityDensityParams::SetAOISubdivs(double x, double y)
{
	m_AOISubdivs[VX] = x; m_AOISubdivs[VY] = y;
}

void CActivityDensityParams::SetAOICenter(double x, double y)
{
	m_AOICenter[VX] = x; m_AOICenter[VY] = y;
}

void CActivityDensityParams::SetLOSMinimum(double min)
{
	ASSERT(min >= 0);
	m_dLOSMin = min;
}

void CActivityDensityParams::SetLOSInterval(double interval)
{
	ASSERT(interval >= 0);
	m_dLOSInterval = interval;
}

void CActivityDensityParams::SetLOSLevels(int levels)
{
	ASSERT(levels > 0);
	int nOldLevels = m_vLOSColors.size();
	if(levels > static_cast<int>(m_vLOSColors.size())) {
		m_vLOSColors.reserve(levels);
		for(int i=0; i<levels; i++) {
			m_vLOSColors.push_back(0);
		}
	}
	else {
		m_vLOSColors.erase(m_vLOSColors.begin()+levels, m_vLOSColors.end());
	}
	m_nLOSLevels = levels;
}

void CActivityDensityParams::SetLOSColor(int level, COLORREF col)
{
	ASSERT(level >= 0 && level < static_cast<int>(m_vLOSColors.size()));
	m_vLOSColors[level] = col;
}

void CActivityDensityParams::SetAOIFloor(int nFloor)
{
	m_nAOIFloor = nFloor;
}

const CString CActivityDensityParams::GetLOSLevelDescription(int nLevel) const
{
	CString s;
	if(nLevel == 0) {
		s.Format("< %.2f sq.m/p", m_dLOSMin);
	}
	else if(nLevel == m_nLOSLevels-1) {
		s.Format("> %.2f sq.m/p", m_dLOSMin+(m_nLOSLevels-2)*m_dLOSInterval);
	}
	else {
		s.Format("%.2f - %.2f sq.m/p", m_dLOSMin+(nLevel-1)*m_dLOSInterval,m_dLOSMin+nLevel*m_dLOSInterval);
	}
	return s;
}