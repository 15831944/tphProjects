#pragma once


class C3DCADMapLayer;
class CCADFileDisplayInfo;
#include <common/CADInfo.h>
#include "3DNodeObject.h"

class CCADLayer;
class C3DCADMapLayer : public AutoDeleteNodeObject
{
public:
	typedef boost::shared_ptr<C3DCADMapLayer> SharedPtr;
	C3DCADMapLayer(CCADLayer* player,Ogre::SceneNode* pnode):AutoDeleteNodeObject(pnode) { m_player = player; }
	CCADLayer* getLayer()const{ return m_player ;}

	//void UpdateMaterial(const CCADFileDisplayInfo& displyinfo );
	void UpdateLayer(const ARCColor3& color);

	//if blinelist= true, draw line list to one manal object section else draw to other manual object section
	static void UpdateLayerToStaticGeo(CCADLayer* m_player,const ARCColor3& color,Ogre::ManualObject* staticGeo, bool bLineList);
	//void UpdateMaterial();
protected:
	CCADLayer* m_player;
};

//
class C3DCADMapLayerList : public C3DNodeObject
{
public:
	C3DCADMapLayerList(){}
	C3DCADMapLayerList(Ogre::SceneNode* pNode):C3DNodeObject(pNode){}

	void Update(const CCADFileDisplayInfo& displyinfo );
	void Update(CCADFileContent* pFlieLayers );
	// get texture map from

	static bool UpdateToGeometry(CCADFileContent* pFlieLayers ,Ogre::ManualObject* geo );
public:
	//void Build(const CCADFileDisplayInfo& displyinfo, bool bSharp);	
	void UpdateTransform(const CCADInfo& mCADInfo);

	typedef std::vector<C3DCADMapLayer::SharedPtr> LayerList;
	LayerList mLayerlist;

public:
	static CString GenMapTexture(const CCADFileDisplayInfo& displyinfo,CString matName );
	static CString GenMapTexture( CCADFileContent* pFlieLayers,CString textTurename );
	//static bool GenStaticGeometry(const CCADFileDisplayInfo& displyinfo,Ogre::StaticGeometry* geo);
};

class C3DCADMapNonSharp : public C3DNodeObject
{
public:
	C3DCADMapNonSharp(){}
	C3DCADMapNonSharp(Ogre::SceneNode* pNode):C3DNodeObject(pNode){}
	void Update(const CCADFileDisplayInfo& displyinfo);
	void UpdateTransform(const CCADInfo& mCADInfo);

	static CString updateRTTMaterial(const CCADFileDisplayInfo& displyinfo, CString matName);
	static CString updateRTTMaterial(CCADFileContent* pFlieLayers, CString matName);
};
