// KeyFrame.cpp: implementation of the CKeyFrame class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "termplan.h"
#include "KeyFrame.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMovieKeyFrame::CMovieKeyFrame(const C3DCamera& _cam, long _time, void* pKeyFrame) : 
	m_camera(_cam),
	m_nTime(_time),
	m_pRelationKeyFrameInFrameCtrl(pKeyFrame)
{
}

CMovieKeyFrame::~CMovieKeyFrame()
{
}


void CMovieKeyFrame::SetCamera(const C3DCamera& _cam)
{
	m_camera = _cam;
}

void CMovieKeyFrame::SetTime(long _time)
{
	m_nTime = _time;
}

const C3DCamera& CMovieKeyFrame::GetCamera() const
{
	return m_camera;
}

long CMovieKeyFrame::GetTime() const
{
	return m_nTime;
}

void CMovieKeyFrame::SetRelationKeyFrame(void *pVoid)
{
	m_pRelationKeyFrameInFrameCtrl=pVoid;
}

void* CMovieKeyFrame::GetRelationKeyFrame()
{
	return m_pRelationKeyFrameInFrameCtrl;
}
