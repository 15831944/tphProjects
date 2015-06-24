// RecordedCameras.cpp: implementation of the CRecordedCameras class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "termplan.h"
#include "RecordedCameras.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRecordedCameras::CRecordedCameras() :
	m_nStartTime(0),
	m_nEndTime(0),
	m_nRecordFps(0)
{
}

CRecordedCameras::~CRecordedCameras()
{
}


void CRecordedCameras::Clear()
{
	m_vCamVec.clear();
	m_vSpeedVec.clear();
}

void CRecordedCameras::AddCamera(const MovieCameraCase& cam)
{
	if (m_vCamVec.empty())
	{
		m_vCamVec.push_back(cam);
		return;
	}

	MovieCameraCase& lastCamera = m_vCamVec.back();
	if (lastCamera.m_nStartTime > cam.m_nStartTime)
		return;

	if (lastCamera == cam)
	{
		lastCamera.m_nEndTime = cam.m_nEndTime;
	}
	else
	{
		m_vCamVec.push_back(cam);
	}
}

void CRecordedCameras::AddSpeedCase(const MovieSpeedCase& speed)
{
	if (m_vSpeedVec.empty())
	{
		m_vSpeedVec.push_back(speed);
		return;
	}

	MovieSpeedCase& lastSpeed = m_vSpeedVec.back();
	if (lastSpeed.m_nStartTime > speed.m_nStartTime)
		return;

	if (lastSpeed == speed)
	{
		lastSpeed.m_nEndTime = speed.m_nEndTime;
	}
	else
	{
		m_vSpeedVec.push_back(speed);
	}
}

MovieCameraCase CRecordedCameras::GateCamera(long nTime)const
{
	int nCount = (int)m_vCamVec.size();
	for (int i = 0; i < nCount; i++)
	{
		const MovieCameraCase& CameraCase = m_vCamVec[i];
		if (CameraCase.m_nStartTime <= nTime && CameraCase.m_nEndTime >= nTime)
		{
			return CameraCase;
		}
	}

	return m_vCamVec.back();
}

MovieSpeedCase CRecordedCameras::GetSpeed(long nTime)const
{
	int nCount = (int)m_vSpeedVec.size();
	for (int i = 0; i < nCount; i++)
	{
		const MovieSpeedCase& speedCase = m_vSpeedVec[i];
		if (speedCase.m_nStartTime <= nTime && speedCase.m_nEndTime >= nTime)
		{
			return speedCase;
		}
	}

	return m_vSpeedVec.back();
}