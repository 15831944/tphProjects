#include "StdAfx.h"
#include ".\airsidereportingareainsim.h"
#include "../../Common/Pollygon2008.h"
#include "../../Common/Point2008.h"

AirsideReportingAreaInSim::AirsideReportingAreaInSim( CReportingArea* pArea )
{
	m_pAreaInput = pArea;
}

AirsideReportingAreaInSim::~AirsideReportingAreaInSim(void)
{
}

bool AirsideReportingAreaInSim::IsInReportingArea(const CPoint2008& point)
{
	const CPoint2008* pointList = m_pAreaInput->GetPath().getPointList();
	int nCount = m_pAreaInput->GetPath().getCount();
	CPollygon2008 polygen;
	polygen.init(nCount, pointList);

	CPoint2008 _point = point; 
	_point.setZ(0);

	bool bIn = polygen.contains(point) >0? true:false;

	return bIn;
}