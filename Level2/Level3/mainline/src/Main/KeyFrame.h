// KeyFrame.h: interface for the CKeyFrame class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_KEYFRAME_H__828023B6_B8BC_40F3_9FD4_130A00ACA81F__INCLUDED_)
#define AFX_KEYFRAME_H__828023B6_B8BC_40F3_9FD4_130A00ACA81F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "3DCamera.h"


class CMovieKeyFrame  
{
public:
	CMovieKeyFrame(const C3DCamera& _cam, long _time, void* pKeyFrame=NULL);
	virtual ~CMovieKeyFrame();

	void SetCamera(const C3DCamera& _cam);
	void SetTime(long _time);

	const C3DCamera& GetCamera() const;
	long GetTime() const;

	void* GetRelationKeyFrame();
	void SetRelationKeyFrame(void* pVoid);
protected:
	C3DCamera m_camera;
	long m_nTime;
	void* m_pRelationKeyFrameInFrameCtrl;
};

#endif // !defined(AFX_KEYFRAME_H__828023B6_B8BC_40F3_9FD4_130A00ACA81F__INCLUDED_)
