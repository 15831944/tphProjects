#pragma once
#include "3DNodeObject.h"

class CDeck;
class CDeckManager;

class CDeck3DNode : public AutoDetachNodeObject
{
public:
	typedef boost::shared_ptr<CDeck3DNode> SharedPtr;	
	CDeck3DNode(Ogre::SceneNode* pNode,CDeck* pDeck):AutoDetachNodeObject(pNode){ mpDeck = pDeck; }

protected:
	CDeck* mpDeck;
};

template <class T, class T3D> 
class C3DNodeObjectList
{
public:
	C3DNodeObjectList();
	//typedef CSectionPos3DController::SharedPtr DataItem;
	//typedef std::vector<DataItem> DataList;
	//typedef DataList::iterator DataIterator;
	//typedef DataList::const_iterator DataConstIterator;

	//DataItem FindItem(CComponentMeshSection* pSection);
	//void RemoveItem(CComponentMeshSection* pSection);
	//void Clear(){ mvSectionlist.clear(); }
	//void Update(CComponentMeshModel* pModel,C3DSceneBase& scene);

	//void UpdateAllShape();
public:
};

class CDeck3DNodeList : public C3DNodeObject
{
public:
	CDeck3DNodeList(){ }
	CDeck3DNodeList(Ogre::SceneNode* pNode):C3DNodeObject(pNode){}




	void Update(CDeckManager* vDecks);
};