#pragma once
#include <InputAirside/Structure.h>

class CRenderAirsideStructure3D : 
	public C3DSceneNode
{
public:
	SCENE_NODE(CRenderAirsideStructure3D,C3DSceneNode)
	void Setup3D(ALTObject*);
protected:
	virtual void UpdateSelected(bool b);
	virtual void Update3D(ALTObject* pBaseObj);

protected:
	void Build(ALTObject* pBaseObject, Ogre::ManualObject* pObj);
	const Ogre::String& UpdateTexture(const CString& matName, Structure* pStructure,StructFace* pFace);//update texture and return mat name;
};


