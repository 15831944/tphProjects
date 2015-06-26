#pragma once
#include "AutoScaleNodeUpdater.h"


class MouseSceneInfo
{
public:
	MouseSceneInfo();
	void SetWndInfo(UINT nFlag, const CPoint& pt, CWnd* pWnd);
	void SetSceneInfo(Ogre::Ray& ray, const Ogre::Plane& activePlane,Ogre::MovableObject* pObj=NULL);


	//windows flag
	UINT mnFlags; //mouse flag
	CPoint mpt;  //point on window
	CWnd* mpFromWnd;


	//scene flag
	Ogre::Ray mSceneRay; //mouse ray
	bool bInPlane;
	Ogre::Vector3 mWorldPos; //intersect result on focus plan of the view
	Ogre::MovableObject* mOnObject; //intersect object
};


class SceneState
{
public:
	AutoScaleNodeUpdater mNodeUpdater;
	MouseSceneInfo m_LastMoseMoveInfo;
	MouseSceneInfo m_lastLBtnDownInfo;
	MouseSceneInfo m_lastRBtnDownInfo;
	MouseSceneInfo m_lastMbtnDownInfo;
};

