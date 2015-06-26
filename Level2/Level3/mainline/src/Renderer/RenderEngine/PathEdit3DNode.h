#pragma once
#include <common/ARCPath.h>
#include <Renderer/RenderEngine/3DNodeObject.h>
#include "KeyNodeList.h"

struct EditPointKey
{
	EditPointKey()
		: nPathIndex(-1)
		, nNodeIndex(-1)
	{

	}
	EditPointKey(int _nPathIndex, int _nNodeIndex)
		: nPathIndex(_nPathIndex)
		, nNodeIndex(_nNodeIndex)
	{

	}

	bool operator==(const EditPointKey& rhs) const
	{
		return nPathIndex == rhs.nPathIndex && nNodeIndex == rhs.nNodeIndex;
	}

	int nPathIndex; // in-path's index
	int nNodeIndex; // index
};

class RENDERENGINE_API CEditPoint3DNode : public AutoDetachNodeObject
{
public:
	CEditPoint3DNode(){}
	CEditPoint3DNode(const C3DNodeObject& otherNode):AutoDetachNodeObject(otherNode.GetSceneNode()){}
	void Update(EditPointKey ptKey, double dSize,const Ogre::ColourValue& c);
	Ogre::ManualObject* GetPointObject();

	EditPointKey getKey()const{ return m_nidx; }
protected:
	EditPointKey m_nidx;
};

struct EditPathConfig
{
	EditPathConfig()
		: clr(ARCColor3::Red)
		, ptSize(5)
		, bLoop(false)
		, bShowNode(true)
	{

	}

	void Init()
	{
		clr = ARCColor4(255, 0, 0, 255);
		ptSize = 5;
		bLoop = false;
		bShowNode = true;
	}

	CPath2008 path;
	ARCColor4 clr;
	double    ptSize;
	bool      bLoop;
	bool      bShowNode;
};

typedef std::vector<EditPathConfig> EditPathConfigList;

class RENDERENGINE_API CEditMultiPath3DNode : public C3DNodeObject, public Key3DNodeList<EditPointKey, CEditPoint3DNode>
{
public:
	enum
	{
		Default_Path = 0,

		Default_PathCount,
	};
	CEditMultiPath3DNode(int nPathCount = Default_PathCount){ Init(nPathCount); }
	CEditMultiPath3DNode(Ogre::SceneNode* pNode, int nPathCount = Default_PathCount):C3DNodeObject(pNode){ Init(nPathCount); }


	int GetPathCount() const;
	void InitPathConfig(int nPathCount);
	EditPathConfig& GetPathConfig(int nPathIndex = Default_Path);
	const EditPathConfig& GetPathConfig(int nPathIndex = Default_Path) const;

	void setOnlyPath(const CPath2008& pathIn);
	void setPath(const CPath2008& pathIn, int nPathIndex = Default_Path){ GetPathConfig(nPathIndex).path = pathIn; }
	CPath2008& getPath(int nPathIndex = Default_Path){ return GetPathConfig(nPathIndex).path; }
	const CPath2008& getPath(int nPathIndex = Default_Path) const { return GetPathConfig(nPathIndex).path; }

	void setColor(ARCColor4 clr, int nPathIndex = Default_Path){ GetPathConfig(nPathIndex).clr = clr; }
	void setPointSize(double ptSize, int nPathIndex = Default_Path){ GetPathConfig(nPathIndex).ptSize = ptSize; }
	void setLoop(bool b, int nPathIndex = Default_Path){ GetPathConfig(nPathIndex).bLoop = b; }
	void setShowNode(bool b, int nPathIndex = Default_Path){ GetPathConfig(nPathIndex).bShowNode = b; }

	void Update();

	void UpdateLines();
	void UpdateLine(int nPathIndex = Default_Path);

	void Clear();

	Ogre::ManualObject* GetPathLine(int nPathIndex = Default_Path);	

	bool OnDrag(Ogre::MovableObject* pobj, const ARCVector3& offset, EditPointKey& ptKey);

	// Test what the movable object belongs to
	// if line, nPathIndex is the path index and nNodeIndex = -1
	// if edit point, nPathIndex is the path index and nNodeIndex is the point index
	bool HitTest(Ogre::MovableObject* pobj, int& nPathIndex, int& nNodeIndex);

	void DoOffset(const ARCVector3& ptOffset);

protected:

	void Init(int nPathCount = Default_PathCount);
	void UpdateEditPoints();
	Ogre::SceneNode* GetPtNode(int nNodeIndex = Default_Path);

	EditPathConfigList m_pathConfigs;
};

//class CSeatGroup;
//class CAircraftLayoutScene;
//class RENDERENGINE_API CEditSeatGroupNode : public C3DNodeObject
//{
//public:
//	CEditSeatGroupNode(){}
//	CEditSeatGroupNode(Ogre::SceneNode* pNode):C3DNodeObject(pNode){}
//
//	void Update(CSeatGroup* pSeatGrp, CAircraftLayoutScene& scene);
//	void ShowEditPath(BOOL bshow);
//protected:
//	CEditMultiPath3DNode m_pathNode;
//};
