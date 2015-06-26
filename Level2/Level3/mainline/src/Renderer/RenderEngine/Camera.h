#pragma once


#include "3DSceneNode.h"
#include "RenderEngineAPI.h"
class ARCVector3;
class CCameraData;
class C3DSceneBase;

class RENDERENGINE_API CCameraNode : public C3DSceneNode
{
public:
	CCameraNode(){ mpCamera = NULL; }
	CCameraNode(Ogre::Camera* pcam);

	void LookAt(const ARCVector3& vcamPos, const ARCVector3& vlookPos, const ARCVector3& vUpdir);
	void Set3D(double fovy  = 30);
	void Set2D(double worldWidth);
	void Set2D(double worldWidth,double worldHeight);
	
	void UpdateNearFar(); //auto update near far
	void SetNearFar(double dNear, double dFar);
	double GetFocusDist( const Ogre::Plane& plan);

	void Load(const CCameraData& camera); //load from input
	void Save(CCameraData& camera);//save to input

	void Pan(const CPoint& mouseDiff,int vpHeight, const Ogre::Plane& plan ) ;
	void Tumble( const CPoint& mouseDiff,  const Ogre::Plane& plan);
	void ZoomInOut( const CPoint& mouseDiff,  const Ogre::Plane& plan);


	bool MoveCamera(UINT nFlags, const CPoint& ptDiff, C3DSceneBase* pScene );

	bool IsValid()const{ return mpCamera!=NULL; }
public:
	Ogre::Camera* mpCamera;
};
