// RecordedCameras.h: interface for the CRecordedCameras class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RECORDEDCAMERAS_H__EE3395C7_5AAF_4CA7_8843_F58C91D05D23__INCLUDED_)
#define AFX_RECORDEDCAMERAS_H__EE3395C7_5AAF_4CA7_8843_F58C91D05D23__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "3DCamera.h"
#include <vector>

struct MovieCameraCase
{
	bool operator == (const MovieCameraCase& CameraCase)const
	{
		if (CameraCase.m_cam == m_cam)
		{
			return true;
		}
		return false;
	}

	C3DCamera m_cam;
	long m_nStartTime;
	long m_nEndTime;
};

struct MovieSpeedCase
{
	bool operator == (const MovieSpeedCase& speedCase)const
	{
		if (speedCase.m_speed == m_speed)
		{
			return true;
		}
		return false;
	}

	int m_speed;
	long m_nStartTime;
	long m_nEndTime;
};
 
class CRecordedCameras  
{
public:
	CRecordedCameras();
	virtual ~CRecordedCameras();

	void AddCamera(const MovieCameraCase& cam);
	int GetCameraCount() { return m_vCamVec.size(); }
	const MovieCameraCase& GetCameraAt(int idx) { return m_vCamVec[idx]; }
	MovieCameraCase GateCamera(long nTime)const;
	void Start(long nStartTime, UINT nFPS) { m_nStartTime = nStartTime; m_nRecordFps = nFPS; Clear(); }
	void End(long nEndTime) { m_nEndTime = nEndTime; }
	void Clear();
	UINT GetFPS() { return m_nRecordFps; }
	long GetStartTime() { return m_nStartTime; }
	long GetEndTime() { return m_nEndTime; }

	//speed case
	void AddSpeedCase(const MovieSpeedCase& speed);
	MovieSpeedCase GetSpeed(long nTime)const;

protected:
	std::vector<MovieCameraCase> m_vCamVec;
	std::vector<MovieSpeedCase> m_vSpeedVec;
	long m_nStartTime;
	long m_nEndTime;
	UINT m_nRecordFps;

};

#endif // !defined(AFX_RECORDEDCAMERAS_H__EE3395C7_5AAF_4CA7_8843_F58C91D05D23__INCLUDED_)
