#include "stdafx.h"
#include "scenestate.h"
using namespace Ogre;

MouseSceneInfo::MouseSceneInfo()
{	
	mOnObject=NULL;
	mpFromWnd = NULL;	
	bInPlane = false;	
}

void MouseSceneInfo::SetSceneInfo( Ogre::Ray& ray, const Ogre::Plane& activePlane,Ogre::MovableObject* pObj/*=NULL*/ )
{
	std::pair<bool,Real> ret = 	ray.intersects(activePlane);
	mSceneRay = ray;
	mOnObject = pObj;
	bInPlane = ret.first;
	mWorldPos = ray.getPoint(ret.second);	
}

void MouseSceneInfo::SetWndInfo( UINT nFlag, const CPoint& pt, CWnd* pWnd )
{
	mnFlags = nFlag; //mouse flag
	mpt = pt;  //point on window
	mpFromWnd = pWnd;
}