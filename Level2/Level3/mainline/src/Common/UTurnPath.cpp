#include "StdAfx.h"
#include ".\uturnpath.h"
#include "./BizierCurve.h"


UTurnPath::UTurnPath( const CPoint2008& pStart, const CPoint2008& pCenter, const CPoint2008& pEnd, DistanceUnit dRad )
{
	m_pCenter = pCenter;
	m_pStart = pStart;
	m_pEnd = pEnd;
	m_dRad = dRad;
}
UTurnPath::~UTurnPath(void)
{
}

CPath2008 UTurnPath::GetPath() const
{

	std::vector<CPoint2008> vCtrlPts;
	vCtrlPts.resize(5);

	ARCVector3 vDir = m_pCenter - m_pStart + m_pCenter - m_pEnd;
	vDir.SetLength(m_dRad);	
	vCtrlPts[0] = m_pStart;	
	vDir.RotateXY(90); 
	vCtrlPts[1] = m_pCenter + vDir;
	vDir.RotateXY(-90);
	vCtrlPts[2] = m_pCenter + vDir;
	vDir.RotateXY(-90);
	vCtrlPts[3] = m_pCenter + vDir;	
	vCtrlPts[4] = m_pEnd;

	std::vector<CPoint2008> vOutPts;
	BizierCurve::GenCurvePoints(vCtrlPts, vOutPts, 15);
	
	CPath2008 retPath;
	retPath.init(vOutPts.size(), &(*vOutPts.begin()) );
	return retPath;
}