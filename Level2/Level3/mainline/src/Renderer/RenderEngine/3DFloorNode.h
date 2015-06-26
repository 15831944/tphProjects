#pragma once
#include "3DSceneNode.h"

class CGrid;
class CBaseFloor;
class CRenderFloor;
//floor
class C3DFloorNode : public C3DSceneNode
{
	SCENE_NODE(C3DFloorNode,C3DSceneNode)
public:
	void UpdateAll(CBaseFloor* pFloor ,bool bInMode);
	//altitude
	void UpdateAltitude(double dAlt);
	//grid
	void UpdateGrid(CBaseFloor* pFloor,bool bInMode);

	//map
	void UpdateFloorSolid(CBaseFloor* pFloor);

	void UpdateShowSharp(bool b);
private:
	Ogre::ManualObject* _getGridObject();
	Ogre::ManualObject* _getFloorObject();

	void _UpdateGridRegion(CBaseFloor* pFloor);
	void _UpdateVisibleRegion( CBaseFloor* pFloor, bool bShowMap,const CString& matname);
	void _UpdateMapRegion(CBaseFloor* pFloor,const CString& matname);

	bool UpdatePicture( CRenderFloor* floorData );


	//
	//create picture texture
	bool _createPicuterTextureMat(const CString& picfile, CString& matName);
	bool _updateCadMapMaterial(CBaseFloor* pFloor,CString& matName);

	ARCVector2 m_picSize;

	//the node display the sharp cad file
	C3DSceneNode _getSharpModeNode();
	bool _UpdateSharpModeNode(CBaseFloor* pFloor,CString MatName,int StencilValue);


};

