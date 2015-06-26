#pragma once

#include <vector>
//#include "Terminal3DList.h"
#include <CommonData/Processor2Base.h>
#include "3DSceneNode.h"
//#include "Terminal3DObject.h"
//
//class Path;
//class CProcessor2Base;
//
//class RENDERENGINE_API CRenderProcessor3D : public CTerminal3DObject
//{
//public:
//	CRenderProcessor3D(Ogre::SceneNode* pNode, CRender3DScene* p3DScene, const CGuid& guid)
//		: CTerminal3DObject(pNode, p3DScene, guid)
//	{}
//
//	typedef boost::shared_ptr<CRenderProcessor3D> SharedPtr;
//	typedef std::vector<SharedPtr> List;
//
//	virtual void UpdateSelected(bool b); // show bounding box by default, or overrided
//	virtual void Update3D(CObjectDisplay* pDispObj, int nUpdateCode = Terminal3DUpdateAll);
//	virtual void Update3D(CProcessor2Base* pProc, int nUpdateCode = Terminal3DUpdateAll);
//
//
//	virtual void UpdateNoShape(CProcessor2Base* pProc, UINT nDrawMask=DRAWALL, int nUpdateCode = Terminal3DUpdateAll);
//
//	CProcessor2Base* GetProcessor() const;
//	virtual CObjectDisplay* GetDisplayObject() const;
//private:
//	C3DNodeObject m_shape;
//};
//
//
//
//class RENDERENGINE_API CRenderProcessor3DList : public CTerminal3DList<CRenderProcessor3DList, CProcessor2Base, CRenderProcessor3D>
//{
//public:
//	CRenderProcessor3D* CreateNew3D(CProcessor2Base* pData, size_t /*nIndex*/) const;
//
//	int getCount()const;
//	Shared3DPtr& getShared3D(int idx){ return m_3DObjects[idx]; }
//	const Shared3DPtr& getShared3D(int idx)const{ return m_3DObjects[idx];}
//};


class RENDERENGINE_API CRenderProcessor3D : public C3DSceneNode
{
public:
	SCENE_NODE(CRenderProcessor3D,C3DSceneNode)
	virtual void Setup3D(CProcessor2Base* pProc, int idx);
	virtual void UpdateNoShape(CProcessor2Base* pProc, UINT nDrawMask=DRAWALL);
	virtual void Update3D(CObjectDisplay* pDispObj);
protected:
	static void _updateNoShape(CProcessor2Base* pProc, C3DSceneNode& node, UINT nDrawMask=DRAWALL);

};
