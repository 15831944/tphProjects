// Movie.cpp: implementation of the CMovie class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "termplan.h"
#include "KeyFrame.h"
#include "common\TERMFILE.h"
#include <algorithm>
#include "Movie.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMovie::CMovie() :
	m_bNeedsRecalc(FALSE),
	m_sName(_T("UNTITLED")),
	m_nAnimSpeed(100) //10x speed
{
}

CMovie::CMovie(const CString& _sName) :
	m_bNeedsRecalc(FALSE),
	m_sName(_sName),
	m_nAnimSpeed(100) //10x speed
{
}

CMovie::~CMovie()
{
	int nCount = m_vKFVec.size();
	for(int i=0; i<nCount; i++) {
		delete m_vKFVec[i];
	}
	m_vKFVec.clear();
}

int CMovie::GetKFCount() const
{
	return m_vKFVec.size();
}

const C3DCamera& CMovie::GetKFCamera(int _idx) const
{
	return m_vKFVec[_idx]->GetCamera();
}

long CMovie::GetKFTime(int _idx) const
{
	return m_vKFVec[_idx]->GetTime();
}

long CMovie::GetKFRunTime(int _idxEnd,int _idxBegin) const
{
	ASSERT(_idxBegin<=_idxEnd);
	return 10*(m_vKFVec[_idxEnd]->GetTime() - m_vKFVec[_idxBegin]->GetTime()) / m_nAnimSpeed;
}

void CMovie::Recalc()
{
}

void CMovie::Interpolate(long _time, C3DCamera& _cam) const
{
	int nCount = m_vKFVec.size();
	int i=0;
	for(; i<nCount; i++) 
	{
		if(m_vKFVec[i]->GetTime() >= _time) 
		{
			break;
		}
	}
	//m_vKFVec[i]->GetTime() >= _time OR i=m_vKFVec.size()
	if(i==0)
	{
		//_time before 1st keyframe
		_cam = GetKFCamera(0);
		return;
	}
	else if(i==m_vKFVec.size()) 
	{
		//_time after last KF
		_cam = GetKFCamera(m_vKFVec.size()-1);
		return;
	}
	//const C3DCamera& next = GetKFCamera(i);
	//const C3DCamera& prev = GetKFCamera(i-1);;
	CMovieKeyFrame* next = m_vKFVec[i];
	CMovieKeyFrame* prev = m_vKFVec[i-1];
	int w = next->GetTime() - prev->GetTime();
	int d = next->GetTime() - _time;
	double r = ((double) d) / w;

	C3DCamera::Interpolate(prev->GetCamera(), next->GetCamera(), r, _cam);
	_cam.Update();
}

int CMovie::MoveKF(long _fromtime, long _totime)
{
	//returns the index of the KF moved, or -1 if no KF at _fromtime
	int nCount = m_vKFVec.size();
	int i=0; 
	for(; i<nCount; i++) 
	{
		if(m_vKFVec[i]->GetTime() >= _fromtime) 
		{
			break;
		}
	}
	if(i<nCount) {
		m_vKFVec[i]->SetTime(_totime);
		return i;
	}
	else {
		return -1;
	}
}

int CMovie::DeleteKF(long _time)
{
	int nCount = m_vKFVec.size();
	int i=0;
	for(; i<nCount; i++)
	{
		if(m_vKFVec[i]->GetTime() >= _time) 
		{
			break;
		}
	}
	if(i<nCount) 
	{
		m_vKFVec.erase( m_vKFVec.begin()+i);
		return i;
	}
	else 
	{
		return -1;
	}
}

int CMovie::InsertKF(const CCameraData& _cam, long _time)
{
	int nCount = m_vKFVec.size();
	int i=0;
	for( ; i<nCount; i++) 
	{
		if(m_vKFVec[i]->GetTime() >= _time)
		{
			break;
		}
	}
	m_vKFVec.insert( m_vKFVec.begin()+i, new CMovieKeyFrame(_cam, _time) );
	return i;
}

int CMovie::InsertKF(const C3DCamera& _cam, long _time)
{
	int nCount = m_vKFVec.size();
	for(int i=0; i<nCount; i++) {
		if(m_vKFVec[i]->GetTime() >= _time) {
			break;
		}
	}
	m_vKFVec.insert( m_vKFVec.begin()+i, new CMovieKeyFrame(_cam, _time) );
	return i;
}

void CMovie::SetName(const CString& _sName)
{
	m_sName = _sName;
}

void CMovie::ReadFromFile(ArctermFile& _file)
{
	char s[256];
	_file.getField(s, 255);
	m_sName = s;
	_file.getInteger(m_nAnimSpeed);
	int nKFCout;
	_file.getInteger(nKFCout);
	_file.getLine();
	for(int i=0; i<nKFCout; i++) {
		long l;
		_file.getInteger(l);
		C3DCamera cam;
		cam.ReadCamera(&_file);
		m_vKFVec.push_back(new CMovieKeyFrame(cam, l));
		_file.getLine();
	}
}

void CMovie::ReadObsoleteFromFile(ArctermFile& _file)
{
	int nKFCout;
	_file.getInteger(nKFCout);
	_file.getLine();
	for(int i=0; i<nKFCout; i++) {
		long l;
		_file.getInteger(l);
		C3DCamera cam;
		cam.ReadCamera(&_file);
		m_vKFVec.push_back(new CMovieKeyFrame(cam, l));
		_file.getLine();
	}
	m_nAnimSpeed = 100;
}

void CMovie::WriteToFile(ArctermFile& _file) const
{
	_file.writeField(m_sName);
	_file.writeInt(m_nAnimSpeed);
	int nKFCount = GetKFCount();
	_file.writeInt(nKFCount);
	_file.writeLine();
	for(int i=0; i<nKFCount; i++) {
		_file.writeInt(GetKFTime(i));
		GetKFCamera(i).WriteCamera(&_file);
		_file.writeLine();
	}
}

CString CMovie::GetKFRunTimeFormated(int _idxEnd, int _idxBegin)
{
	long nRunTime=GetKFRunTime(_idxEnd,_idxBegin);

	int nHours = (nRunTime/360000);
	int nMins = (nRunTime%360000)/6000;
	int nSecs = (nRunTime%6000)/100;
	CString strTime;
	strTime.Format("%.2d:%.2d:%.2d", nHours,nMins, nSecs);
	return strTime;
}

void CMovie::SetRelationKeyFrame(int i, void *pVoid)
{
	m_vKFVec[i]->SetRelationKeyFrame(pVoid);
}

int CMovie::FindRelationKeyFrame(void *pVoid)
{
	for(int i=0;i<static_cast<int>(m_vKFVec.size());i++)
	{
		if(m_vKFVec[i]->GetRelationKeyFrame()==pVoid)
			return i;
	}
	return -1;
}
