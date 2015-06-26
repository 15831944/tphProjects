#pragma once
#include "3DScene.h"
#include "AnimaPax3D.h"
#include "grid3d.h"
#include <common/Camera.h>
enum emCameraType{
	camTop,camLeft,camFront,camPerspect,
};

enum SelectionFlag
{
	UnSelectable = 0,
	ModelEntity = 1,
};

class CNonPaxRelativePos;
class RENDERENGINE_API CNonPaxShape3D : public CAnimaPax3D
{
public:
	CNonPaxShape3D(){}
	CNonPaxShape3D(CNonPaxRelativePos* pPosData, Ogre::SceneNode* pNode);

	bool operator==(const CNonPaxShape3D& pax)const{ return  mpData == pax.mpData; }
	void FlushChangeToData();
	void UpdateDisplay(); //update display from data

	void DoRotate(double dAngle,const ARCVector3& axis);
	void Move(const ARCVector3& offset);

	CNonPaxRelativePos* mpData;

};

class RENDERENGINE_API CNonPaxRelativePosDefScene : public C3DSceneBase
{
public:

	CNonPaxRelativePosDefScene();

	std::map<int,CString> m_shapeNameMap;

	CString GetShapeName(int nTypeIdx)const;
	void SetupCamera();
	void SetupScene();

	//void GetMousePouse
	virtual Ogre::Plane getFocusPlan()const;
	//Ogre::Plane GetFocusPlan(const Ogre::Vector3& pt)const;
	void AddUpdate(CNonPaxRelativePos* pPosData);

	void SetCamera(emCameraType camType);
	void RestoreCameraToDefault(emCameraType camType);

	//void ZoomCam(double percent);
	//void RotateCam(double dAnlge,double yawAngle); //rotate camera with scene center
	
	

	CGrid3D m_sceneGrid;
	CAnimaPax3D m_pAnimPax;

	std::vector<CNonPaxShape3D> mvNonPaxlist;

	CNonPaxShape3D Get3DShape(CNonPaxRelativePos* pData)const;
	CNonPaxShape3D GetSelectNode(Ogre::MovableObject* pObj)const;

	void RemoveShape3D(CNonPaxShape3D p3d);


	Ogre::Camera*CreateCamera();
	//Ogre::Vector3 mCenter;
	Ogre::SceneNode* mpPerpCamNode;

	CCameraData mpTopCamera;
	CCameraData mpLeftCamera;
	CCameraData mpFrontCamera;
	CCameraData mPerspectCamera;

	CCameraNode mCamera3D;	
	emCameraType m_curCamType;



};