#pragma once
#include "AfxHeader.h"

#include "3DUIOperationTarget.h"

//do the update the object, how to operate with the use operations 
class IMovableObjectImpl
{
public:
	//user operations left button double click
	virtual BOOL OnLBDblClk(Ogre::MovableObject* pthisObj, const MouseSceneInfo& mouseinfo){  return TRUE; }

};

//the representative of the ogre movable object, with some function in arcport
class C3DMovableObject 
{
public:
	void OnLBDblClk(const MouseSceneInfo& mouseinfo);

	IMovableObjectImpl* getImpl();
	void setImpl(IMovableObjectImpl* pImpl);
protected:
	Ogre::MovableObject* m_pObject;
};

