#pragma once
#include "3DSceneNode.h"
#include "Common\Guid.h"

class CRenderPipe3D;

//class RenderPipeControlPoint : RenderControlPoint
//{
//public:
//	enum PipePointType
//	{
//		left_type,
//		center_type,
//		right_type,
//	};
//	typedef boost::shared_ptr<RenderPipeControlPoint> SharedPtr;
//	typedef std::vector<SharedPtr> List;
//	RenderPipeControlPoint(Ogre::SceneNode* pNode, CRenderPipe3D* pPipe3D, int nIndex, PipePointType pttype);
//
//	virtual void OnMove(const ARCVector3& newPos);
//	void Update( ARCVector3 pos, double size, const Ogre::ColourValue& clr );
//public:
//	CRenderPipe3D* m_pPipe3D;
//	int m_nIndex;
//	PipePointType m_type;
//};

class CPipeBase;
class CRenderPipe3D : public C3DSceneNode
{
	friend class C3DPipeNodeImpl;
public:
	SCENE_NODE(CRenderPipe3D,C3DSceneNode	)
	void Setup3D(CPipeBase* pPipe, int idx);
protected:
	void Build(CPipeBase* pPipe,int index, Ogre::ManualObject* pObj);
	void Update3D(CPipeBase* pPipe, int idx);
};

