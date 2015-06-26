#include "StdAfx.h"
#include ".\lsvehiclethroughputparam.h"
#include "..\Common\TERMFILE.H"

LSVehicleThroughputParam::LSVehicleThroughputParam(void)
{
}

LSVehicleThroughputParam::~LSVehicleThroughputParam(void)
{
}

CString LSVehicleThroughputParam::GetReportParamName()
{
	return  _T("\\LandsideVehicleThroughput\\Param.pac");
}

const CString& LSVehicleThroughputParam::GetPortalName() const
{
	return m_sPortalName;
}

void LSVehicleThroughputParam::SetPortalName( const CString& sPortalName )
{
	m_sPortalName = sPortalName;
}

void LSVehicleThroughputParam::SetStartPoint( const CPoint2008& ptPoint )
{
	m_pPointList[0] = ptPoint;
}

CPoint2008 LSVehicleThroughputParam::GetStartPoint() const
{
	return m_pPointList[0];
}

void LSVehicleThroughputParam::SetEndPoint( const CPoint2008& ptPoint )
{
	m_pPointList[1] = ptPoint;
}

CPoint2008 LSVehicleThroughputParam::GetEndPoint() const
{
	return m_pPointList[1];
}

void LSVehicleThroughputParam::WriteParameter( ArctermFile& _file )
{
	LandsideBaseParam::WriteParameter(_file);

	_file.writeField(m_sPortalName);
	_file.writePoint2008(m_pPointList[0]);
	_file.writePoint2008(m_pPointList[1]);
}

void LSVehicleThroughputParam::ReadParameter( ArctermFile& _file )
{
	LandsideBaseParam::ReadParameter(_file);

	_file.getField(m_sPortalName.GetBuffer(),255);
	m_sPortalName.ReleaseBuffer();

	_file.getPoint2008(m_pPointList[0]);
	_file.getPoint2008(m_pPointList[1]);
}



