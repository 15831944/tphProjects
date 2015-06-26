#pragma once
#include <boost\shared_ptr.hpp>
#include <Renderer\RenderEngine\3DNodeObject.h>

class CComponentMeshSection;
class CComponentMeshModel;
class C3DSceneBase;
//display section lines
class RENDERENGINE_API CMeshSection3D : public C3DNodeObject
{
public:	
	CMeshSection3D(){}
	CMeshSection3D(Ogre::SceneNode* pNode):C3DNodeObject(pNode){}
	void Update(CComponentMeshSection* pSection);
	void UpdatePos(CComponentMeshSection* pSection);
	static CString GetNodeName(CComponentMeshSection* pSection);
protected:	
};

class CSimpleMesh3D : public C3DNodeObject
{
public:
	CSimpleMesh3D(){};
	CSimpleMesh3D(CComponentMeshModel* pModeData,Ogre::SceneNode* pNode);
	void Update(const CString& strMat);
	void SetMaterial(const CString& str);
	void OnMouseMove(const Ogre::Ray& mouseRay,Ogre::MovableObject* pObj); //
	
	void SaveThumbnail();
	void SaveMesh();
protected:
	CComponentMeshModel* m_pModeData;
	
};


class CComponentMeshSection;
//control the section position at the x axis
class RENDERENGINE_API CSectionPos3DController
{
public:
	typedef boost::shared_ptr<CSectionPos3DController> SharedPtr;

	CSectionPos3DController(CComponentMeshSection* pSection){ mpSection = pSection; }
	~CSectionPos3DController();

	void Update(C3DSceneBase& scene);

	void UpdateShape();
	void UpdatePos();
	CComponentMeshSection* mpSection;

	void OnMouseMove(const Ogre::Ray& rayFrom, const Ogre::Ray& rayTo);

public:
	CMeshSection3D mSection3D;
	C3DNodeObject mSectionPos; //just a node of the section pos;
};


//list of the section position 
class RENDERENGINE_API CSection3DControllerList
{
public:
	typedef CSectionPos3DController::SharedPtr DataItem;
	typedef std::vector<DataItem> DataList;
	typedef DataList::iterator DataIterator;
	typedef DataList::const_iterator DataConstIterator;

	DataItem FindItem(CComponentMeshSection* pSection);
	void RemoveItem(CComponentMeshSection* pSection);
	int getCount()const{ return (int)mvSectionlist.size(); }
	DataItem& ItemAt(int i){ return mvSectionlist.at(i); } 
	const DataItem& ItemAt(int i)const{ return mvSectionlist.at(i); } 
	void Clear(){ mvSectionlist.clear(); }
	void Update(CComponentMeshModel* pModel,C3DSceneBase& scene);

	void UpdateAllShape();

	void ActiveItem(CComponentMeshSection* pSection);

	CSectionPos3DController* IsMouseOn(Ogre::MovableObject* pObj);
protected:
	DataList mvSectionlist;

};

//section edit control
class RENDERENGINE_API CSectionDetailController : public C3DNodeObject
{
public:
	CSectionDetailController(){ mpSection = NULL; }
	CSectionDetailController(CComponentMeshSection* pSection, Ogre::SceneNode* pNode);
	void Update(C3DSceneBase& scene);

	void Update();

	bool IsCenterCtrl(Ogre::MovableObject* pObj);
	bool IsSectionCtrl(Ogre::MovableObject* pObj, int& idx);
	bool IsThickNessCtrl(Ogre::MovableObject* pObj,int& idx);

	void OnMouseMoveCenter(const Ogre::Ray& rayFrom, const Ogre::Ray& rayTo);
	void OnMouseMoveCtrlPt(int idx,const Ogre::Ray& rayFrom, const Ogre::Ray& rayTo );
	void OnMouseMoveThickness(int idx, const Ogre::Ray& rayFrom, const Ogre::Ray& rayTo);

	C3DNodeObject mSectionCenterCtrl; //control the section center
	typedef std::vector<AutoDetachNodeObject::SharedPtr> SectionCtrlPtsList;
	SectionCtrlPtsList mctrlPtsList; //control the section edit points
	//SectionCtrlPtsList mThicknessCtrlList; //control the section thickness	
	CComponentMeshSection* mpSection;
protected:
};


class CComponentMeshModel;
class RENDERENGINE_API CCenterLine3D : public C3DNodeObject
{
public:
	CCenterLine3D(){}
	CCenterLine3D(Ogre::SceneNode* pNode);
	
	void Update(CComponentMeshModel* pMeshModel, C3DSceneBase& scene);
	void UpdatePos(CComponentMeshModel* pMeshModel);
	void OnMove(const Ogre::Ray& rayFrom, const Ogre::Ray& rayTo);

	C3DNodeObject mCtrlPt;
protected:

};
