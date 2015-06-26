#pragma once

#include "3DNodeObject.h"

enum emCoordElm
{
	CE_XAxis,
	CE_YAxis,
	CE_ZAxis,
	CE_XYPlan,
	CE_YZPlan,
	CE_XZPlan,
	CE_XRotate,
	CE_YRotate,
	CE_ZRotate,
};

//can manipulate the scene node the coordinate attached to 
class CSceneNodeCoordinate : public C3DNodeObject
{
public:
	class EventListener
	{
	public:
		virtual void BeginTransform(Ogre::SceneNode* pParent)=0;
		virtual void OnTransform(Ogre::SceneNode* pParent)=0;
		virtual void EndTransform(Ogre::SceneNode* pParent)=0;
	};
	typedef EventListener* ListenerPtr;

	CSceneNodeCoordinate(Ogre::SceneNode* pNode);
	Ogre::ManualObject* GetCoordinateElm(emCoordElm elm);

	//
	void StartDrag(const Ogre::Ray& startRay, Ogre::MovableObject* pObj);
	void OnDrag(const Ogre::Ray& mouseRayFrom, const Ogre::Ray& mouseRayTo);
	void EndDrag(const Ogre::Ray& mouseRay);
	bool IsStartDrag()const{ return mpDragObj!=NULL; }

	void OnMousemove(const Ogre::Ray& mouseRay,Ogre::MovableObject* pObj); //when mouse move on the obj


	void SetManipulateNode(Ogre::SceneNode* pNode);
	//
	void AddListener(EventListener* pListen);

protected:
	static void BuildXCoord(Ogre::ManualObject* pObj);
	static void BuildYCoord(Ogre::ManualObject* pObj);
	static void BuildZCoord(Ogre::ManualObject* pObj);

	static void BuildXYPlan(Ogre::ManualObject* pObj);
	static void BuildYZPlan(Ogre::ManualObject* pObj);
	static void BuildXZPlan(Ogre::ManualObject* pObj);

	static void BuildXRotate(Ogre::ManualObject* pObj);
	static void BuildYRotate(Ogre::ManualObject* pObj);
	static void BuildZRotate(Ogre::ManualObject* pObj);


	bool getRayIntersectPos( const Ogre::Ray& ray, emCoordElm em,Ogre::Vector3& outPos );
	void UpdateHintText();

	void InitOnce();
	std::vector<ListenerPtr> mvListers;

	Ogre::Ray mStartRay;
	Ogre::Vector3 mStartDragPos;
	Ogre::Vector3 mLastDragPos;
	
	Ogre::MovableObject* mpDragObj;
	Ogre::MovableObject* mpLastMouseOnObj;

	Ogre::String mHintTextName;

	Ogre::SceneNode* mpManipulateNode;
	Ogre::SceneNode* GetManipulateNode(){ return mpManipulateNode; } 
};
