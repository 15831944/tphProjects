#pragma once
#include "3DNodeObject.h"

#include <Common/RenderFloor.h>
#include "BaseFloor3D.h"

class RENDERENGINE_API CRenderFloor3D  : public CBaseFloor3D
{
public:
	typedef boost::shared_ptr<CRenderFloor3D> SharedPtr;
	CRenderFloor3D(Ogre::SceneNode* pNode):CBaseFloor3D(pNode){ }
	~CRenderFloor3D();

	void Update(CRenderFloor* floorData, C3DNodeObject& parent);
	void UpdateAltitude(double dAltitude);
	ARCVector2 m_picSize;

	CString createTextureMaterialOnce(const CString& tex);
	void UpdatePicture( CRenderFloor* floorData,C3DNodeObject& parentnode );
};

class RENDERENGINE_API CRenderFloor3DList : public C3DNodeObject
{
public:
	CRenderFloor3DList(){ }
	CRenderFloor3DList(Ogre::SceneNode* pNode):C3DNodeObject(pNode){ }

	void Update(CRenderFloorList& floorData, BOOL inEditMode);
	void UpdateFloor(CRenderFloor* pFloorData, BOOL inEditMode); // find and update one floor, if not found, return
	void UpdateFloorAltitude( CRenderFloor* pFloorData );

	CRenderFloor3D::SharedPtr GetFloorByData( CRenderFloor* pFloorData );
	CRenderFloor3D::SharedPtr GetFloorByLevel( int level );
	
private:
	typedef std::vector<CRenderFloor3D::SharedPtr> RenderFloor3DVector;
	RenderFloor3DVector m_vecFloors;
};
