#pragma once
#include "AirsideResource.h"
#include "../../Common/Point2008.h"
#include "../../InputAirside/ReportingArea.h"

class ENGINE_TRANSFER AirsideReportingAreaInSim
{
public:
	typedef ref_ptr<AirsideReportingAreaInSim> RefPtr;

	AirsideReportingAreaInSim( CReportingArea* pArea );
	~AirsideReportingAreaInSim(void);

	int GetAreaID()const{ return m_pAreaInput->getID(); }
	CReportingArea::RefPtr GetAreaInput(){ return m_pAreaInput; }

	bool IsInReportingArea(const CPoint2008& point);

private:
	CReportingArea::RefPtr m_pAreaInput;
};


///////////////////////////////////////////////////////////