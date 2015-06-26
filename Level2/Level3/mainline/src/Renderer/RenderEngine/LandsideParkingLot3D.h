#pragma once
#include "RenderScene3DNode.h"
#include "CommonData/3DViewCommon.h"
#include "Common\Point2008.h"

#include "landside/LandsideParkingLot.h"

class LandsideParkingLot;
class LandsideParkingLotLevel;

//////////////////////////////////////////////////////////////////////////
class LandsideParkingLot3D : public CLayoutObject3DNode
{
public:
	SCENENODE_CONVERSION_DEF(LandsideParkingLot3D, CLayoutObject3DNode)
	PROTOTYPE_DECLARE(LandsideParkingLot3D)

	virtual void OnUpdate3D();
	
	LandsideParkingLot* getLot();

protected:
	void SetQueryData(LandsideParkingLot* pLot);
};

enum emLandsideItemType;
class LandsideParkingLot3DItem : public CRenderScene3DNode
{
public:
	SCENENODE_CONVERSION_DEF(LandsideParkingLot3DItem, CRenderScene3DNode)
	LandsideParkingLot* getLot()const;
	int getLevelIdx()const;
	virtual emLandsideItemType getType()const=0;
	
};

//////////////////////////////////////////////////////////////////////////
class ParkingLotLevel3DNode : public LandsideParkingLot3DItem
{
public:
	SCENENODE_CONVERSION_DEF(ParkingLotLevel3DNode, LandsideParkingLot3DItem)
	PROTOTYPE_DECLARE(ParkingLotLevel3DNode)
	emLandsideItemType getType()const;

	void SetQueryData(LandsideParkingLot* pLot, int nLevel);
	LandsideParkingLotLevel* getLevel();

	//dynamic drag
	virtual void StartDrag(const MouseSceneInfo& lastMouseInfo, const MouseSceneInfo& curMouseInfo);
	virtual void OnDrag(const MouseSceneInfo& lastMouseInfo, const MouseSceneInfo& curMouseInfo );
	virtual void EndDrag();
	virtual bool IsDraggable()const;
	//bool UpdateBaseLevel(LandsideParkingLot* pLot, CPoint2008& ptCenter,BOOL InEdit );
	void UpdateLevel(LandsideParkingLot* pLot,int nLevelIndex, const CPoint2008& ptCenter);	
protected:	
	CRenderScene3DNode GetAreaNode();
};
//////////////////////////////////////////////////////////////////////////

class ParkingLotParkingSpace3DNode : public LandsideParkingLot3DItem
{
public:
	SCENENODE_CONVERSION_DEF(ParkingLotParkingSpace3DNode, LandsideParkingLot3DItem)
		PROTOTYPE_DECLARE(ParkingLotParkingSpace3DNode)
	emLandsideItemType getType()const;

	virtual void Build3D(LandsideParkingLot* pLot, int nLevelIndex,int iAreaIndex,const CPoint2008& ptCenter);
	
	void OnLButtonDblClk( const MouseSceneInfo& mouseinfo );
	void SetQueryData(LandsideParkingLot* pLot, int nLevel,int nIndex);
	//dynamic drag
	virtual void StartDrag(const MouseSceneInfo& lastMouseInfo, const MouseSceneInfo& curMouseInfo);
	virtual void OnDrag(const MouseSceneInfo& lastMouseInfo, const MouseSceneInfo& curMouseInfo );
	virtual void EndDrag();
	virtual bool IsDraggable()const;

	virtual void OnUpdate3D();


	static void RenderSpace(ParkingSpace& space,int iIndex, Ogre::ManualObject* pObj, Ogre::ManualObject* pselObj, const CPoint2008& ptCenter );
};
//////////////////////////////////////////////////////////////////////////
class ParkLotEntryExit;
class ParkingLotEntry3DNode : public LandsideParkingLot3DItem
{
public:
	SCENENODE_CONVERSION_DEF(ParkingLotEntry3DNode, LandsideParkingLot3DItem)
		PROTOTYPE_DECLARE(ParkingLotEntry3DNode)
	emLandsideItemType getType()const;

	virtual void Buil3D(LandsideParkingLot* pLot,int nLevelIndex,int nEntryIndex, const CPoint2008& ptCenter);
	virtual void OnUpdateEditMode(BOOL b);
	virtual void OnUpdate3D();
	void OnLButtonDblClk( const MouseSceneInfo& mouseinfo );
	void SetQueryData(LandsideParkingLot* pLot, int nLevel,int nIndex);
	//void SetQueryData(ParkLotEntryExit *pEntry);

	//dynamic drag
	virtual void StartDrag(const MouseSceneInfo& lastMouseInfo, const MouseSceneInfo& curMouseInfo);
	virtual void OnDrag(const MouseSceneInfo& lastMouseInfo, const MouseSceneInfo& curMouseInfo );
	virtual void EndDrag();
	virtual bool IsDraggable()const;
protected:
	void RenderEntryExit(ParkLotEntryExit& entexit, const CPoint2008& ptCenter,double _height );
};
//////////////////////////////////////////////////////////////////////////
class ParkingLotAreas;
class Pollygon2008;
class ParkingLotArea3DNode : public LandsideParkingLot3DItem
{
public:
	SCENENODE_CONVERSION_DEF(ParkingLotArea3DNode, LandsideParkingLot3DItem)
	PROTOTYPE_DECLARE(ParkingLotArea3DNode)
	emLandsideItemType getType()const;

	virtual void OnUpdate3D();
	virtual void OnUpdateEditMode(BOOL b);
	void OnLButtonDblClk( const MouseSceneInfo& mouseinfo );

	virtual void StartDrag(const MouseSceneInfo& lastMouseInfo, const MouseSceneInfo& curMouseInfo);
	virtual void OnDrag(const MouseSceneInfo& lastMouseInfo, const MouseSceneInfo& curMouseInfo );
	virtual void EndDrag();
	virtual bool IsDraggable()const;

	void Build3D(LandsideParkingLot* pLot, int nLevel,const CPoint2008& ptCenter);

protected:
	void BuildArea(LandsideParkingLotLevel* plevel,const CPoint2008& ptCenter);
	void SetQueryData(LandsideParkingLot* pLot, int nLevel);

};
//////////////////////////////////////////////////////////////////////////
class ParkingDrivePipe;
class ParkingLotDrivePipe3DNode : public LandsideParkingLot3DItem
{
public:
	SCENENODE_CONVERSION_DEF(ParkingLotDrivePipe3DNode, LandsideParkingLot3DItem)
	PROTOTYPE_DECLARE(ParkingLotDrivePipe3DNode)

	emLandsideItemType getType()const;

	virtual void OnUpdate3D();
	//virtual void OnUpdate3D(LandsideParkingLot* pLot,int nLevelIndex,ParkingDrivePipe& pipe,const CPoint2008& ptCenter);
	void OnLButtonDblClk( const MouseSceneInfo& mouseinfo );

	//dynamic drag
	virtual void StartDrag(const MouseSceneInfo& lastMouseInfo, const MouseSceneInfo& curMouseInfo);
	virtual void OnDrag(const MouseSceneInfo& lastMouseInfo, const MouseSceneInfo& curMouseInfo );
	virtual void EndDrag();
	virtual bool IsDraggable()const;



	void SetQueryData(LandsideParkingLot* pLot, int nLevel,int nIndex);
	void Build3D(LandsideParkingLot* pLot,int nLevelIndex,int nPipeIndex, const CPoint2008& ptCenter,double dLevelHeight);
protected:
	ParkingDrivePipe* getPipe();
};
//////////////////////////////////////////////////////////////////////////
class CPath2008;
class ParkingLotControlPoint3D :  public LandsideParkingLot3DItem 
{
public:
	SCENENODE_CONVERSION_DEF(ParkingLotControlPoint3D, LandsideParkingLot3DItem)
	virtual void StartDrag(const MouseSceneInfo& lastMouseInfo, const MouseSceneInfo& curMouseInfo);
	virtual void EndDrag();
	virtual void OnDrag( const MouseSceneInfo& lastMouseInfo, const MouseSceneInfo& curMouseInfo );
	
	virtual bool IsDraggable()const;
	int getPointIndex()const;
	int getPathIndex()const;

	virtual CPath2008& getPath(LandsideParkingLot* pLot,int nPathIndex);	
	void Update3D(LandsideParkingLot* pLot,int nLevelIndex, int nPathIndex, int iPtIdx,CPath2008& path);
};

class ParkingLotControlPath3D : public LandsideParkingLot3DItem
{
public:
	SCENENODE_CONVERSION_DEF(ParkingLotControlPath3D, LandsideParkingLot3DItem)
	virtual CRenderScene3DNode GetControlPoint3D(int idx);

	virtual CPath2008& getPath(LandsideParkingLot* pLot,int nLevelIndex,int nPathIndex)=0;
	virtual void Update3D(LandsideParkingLot* pLot, int nLevelIndex,int iAreaIndex, const CPoint2008& ptCenter);

	virtual void UpdatePoints(LandsideParkingLot* pLot,int nLevelIndex, int iAreaIndex,CPath2008& path)=0;

};
//////////////////////////////////////////////////////////////////////////
//control path of the parking lot area
class ParkingLotAreaControlPath3D  :  public ParkingLotControlPath3D 
{
public:
	SCENENODE_CONVERSION_DEF(ParkingLotAreaControlPath3D, ParkingLotControlPath3D)
	PROTOTYPE_DECLARE(ParkingLotAreaControlPath3D)	
	virtual CPath2008& getPath(LandsideParkingLot* pLot,int nLevelIndex,int nPathIndex);	
	emLandsideItemType getType()const;	
	virtual void UpdatePoints(LandsideParkingLot* pLot, int nLevelIndex,int iAreaIndex,CPath2008& path);
	void OnRButtonDown( const MouseSceneInfo& mouseInfo );
	void Update3D(LandsideParkingLot* pLot,int nLevelIndex, int iAreaIndex, const CPoint2008& ptCenter);
};

class ParkinglotAreaControlPoint3D : public ParkingLotControlPoint3D
{
public:
	SCENENODE_CONVERSION_DEF(ParkinglotAreaControlPoint3D, ParkingLotControlPoint3D)
	PROTOTYPE_DECLARE(ParkinglotAreaControlPoint3D)

	emLandsideItemType getType()const;
// 	virtual CPath2008& getPath(LandsideParkingLot* pLot,int nPathIndex);		
};

//////////////////////////////////////////////////////////////////////////
class ParkingSpaceControlPath3D : public ParkingLotControlPath3D
{
public:
	SCENENODE_CONVERSION_DEF(ParkingSpaceControlPath3D, ParkingLotControlPath3D)
	PROTOTYPE_DECLARE(ParkingSpaceControlPath3D)

	emLandsideItemType getType()const;
	virtual CPath2008& getPath(LandsideParkingLot* pLot,int nLevelIndex,int nPathIndex);
	void UpdatePoints(LandsideParkingLot* pLot, int nLevelIndex,int iAreaIndex,CPath2008& path );
	void OnRButtonDown(const MouseSceneInfo& mouseInfo);
};

class ParkingSpaceControlPoint3D : public ParkingLotControlPoint3D
{
public:
	SCENENODE_CONVERSION_DEF(ParkingSpaceControlPoint3D, ParkingLotControlPoint3D)
	PROTOTYPE_DECLARE(ParkingSpaceControlPoint3D)

	emLandsideItemType getType()const;
// 	virtual CPath2008& getPath(LandsideParkingLot* pLot,int nPathIndex);
};
//////////////////////////////////////////////////////////////////////////
class ParkingLotDrivePipeControlPath3D : public ParkingLotControlPath3D
{
public:
	SCENENODE_CONVERSION_DEF(ParkingLotDrivePipeControlPath3D,ParkingLotControlPath3D)
	PROTOTYPE_DECLARE(ParkingLotDrivePipeControlPath3D)

public:
	emLandsideItemType getType() const;
	virtual CPath2008& getPath(LandsideParkingLot* pLot,int nLevelIndex,int nPipeIndex);
	void UpdatePoints(LandsideParkingLot* pLot, int nLevelIndex,int nPipeIndex,const CPath2008& path );
	void UpdatePoints(LandsideParkingLot* pLot, int nLevelIndex,int nPipeIndex,CPath2008& path );
	virtual void Update3D(LandsideParkingLot* pLot, int nLevelIndex,int iAreaIndex, const CPath2008& drawPath);
	void OnRButtonDown(const MouseSceneInfo& mouseInfo);
};

class ParkingLotDrivePipeControlPoint3D : public ParkingLotControlPoint3D
{
public:
	SCENENODE_CONVERSION_DEF(ParkingLotDrivePipeControlPoint3D, ParkingLotControlPoint3D)
	PROTOTYPE_DECLARE(ParkingLotDrivePipeControlPoint3D)
public:
	emLandsideItemType getType()const;
// 	virtual CPath2008& getPath(LandsideParkingLot* pLot,int nPipeIndex);
};

//////////////////////////////////////////////////////////////////////////

class ParkingLotEntryControlPath3D : public ParkingLotControlPath3D
{
public:
	SCENENODE_CONVERSION_DEF(ParkingLotEntryControlPath3D, ParkingLotControlPath3D)
		PROTOTYPE_DECLARE(ParkingLotEntryControlPath3D)
	emLandsideItemType getType()const;	
	virtual CPath2008& getPath(LandsideParkingLot* pLot,int nLevelIndex,int nPathIndex);	
	void UpdatePoints(LandsideParkingLot* pLot,int nLevelIndex, int iAreaIndex,CPath2008& path );

};

class ParkingLotEntryControlPoint3D : public ParkingLotControlPoint3D
{
public:
	SCENENODE_CONVERSION_DEF(ParkingLotEntryControlPoint3D, ParkingLotControlPoint3D)
	PROTOTYPE_DECLARE(ParkingLotEntryControlPoint3D)
	emLandsideItemType getType()const;

};