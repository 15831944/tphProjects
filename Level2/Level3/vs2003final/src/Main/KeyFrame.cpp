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

CKeyFrame::CKeyFrame(const C3DCamera& _cam, long _time, void* pKeyFrame) : 
	m_camera(_cam),
	m_nTime(_time),
	m_pRelationKeyFrameInFrameCtrl(pKeyFrame)
{
}

CKeyFrame::~CKeyFrame()
{
}


void CKeyFrame::SetCamera(const C3DCamera& _cam)
{
	m_camera = _cam;
}

void CKeyFrame::SetTime(long _time)
{
	m_nTime = _time;
}

const C3DCamera& CKeyFrame::GetCamera() const
{
	return m_camera;
}

long CKeyFrame::GetTime() const
{
	return m_nTime;
}

void CKeyFrame::SetRelationKeyFrame(void *pVoid)
{
	m_pRelationKeyFrameInFrameCtrl=pVoid;
}

void* CKeyFrame::GetRelationKeyFrame()
{
	return m_pRelationKeyFrameInFrameCtrl;
}
