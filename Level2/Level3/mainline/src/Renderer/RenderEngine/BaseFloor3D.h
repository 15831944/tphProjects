#pragma once
#include "3DNodeObject.h"
#include "Grid3D.h"
#include "FloorAlignMarker.h"
#include "KeyNodeList.h"



class CBaseFloor;
class CBaseFloor3D;
class CBaseFloor3D : public AutoDetachNodeObject{
public:
	//typedef boost::shared_ptr<CBaseFloor3D> SharedPtr;	
	CBaseFloor3D(Ogre::SceneNode* pNode):AutoDetachNodeObject(pNode){ mpDeck = NULL; }
	CBaseFloor3D(Ogre::SceneNode* pNode,CBaseFloor* pDeck):AutoDetachNodeObject(pNode){ mpDeck = pDeck; }

	CBaseFloor* getBaseFloor()const{ return mpDeck; }

	CGrid3D mGrid;
	void ShowGrid(BOOL b);

	void UpdateDeckRegion(bool bShowMap = true);
	void UpdateMaterial();
	void UpdateGrid();
	void UpdatePos();
	void UpdateMarkers();

	//update all 
	void Update(C3DNodeObject& parentNode);
	//void Update(CBaseFloor* pDeck);

	CBaseFloor* getKey()const{ return mpDeck; }


protected:
	CBaseFloor* mpDeck;
	CString mMapMat;

	CFloorAlignMarker m_alignMarker1;
	CFloorAlignMarker m_alignMarker2;
};



class CBaseFloor3DList : public Key3DNodeList<CBaseFloor*,CBaseFloor3D>
{
public:
	void Update( const std::vector<CBaseFloor*>& vFlrList , C3DNodeObject& parentNode);
};