#pragma once
#include "LandsideBaseParam.h"
#include "..\Common\Point2008.h"
class LANDSIDEREPORT_API LSVehicleThroughputParam : public LandsideBaseParam
{
public:
	LSVehicleThroughputParam(void);
	~LSVehicleThroughputParam(void);

	virtual CString GetReportParamName();
	//write the parameter data
	virtual void WriteParameter(ArctermFile& _file);
	virtual void ReadParameter(ArctermFile& _file);

	const CString& GetPortalName()const;
	void SetPortalName(const CString& sPortalName);

	void SetStartPoint(const CPoint2008& ptPoint);
	CPoint2008 GetStartPoint()const;

	void SetEndPoint(const CPoint2008& ptPoint);
	CPoint2008 GetEndPoint()const;
private:
	CPoint2008 m_pPointList[2];
	CString m_sPortalName;
};
