#pragma once
#include "3DScene.h"
#include "AnimaPax3D.h"
#include "AnimaFlight3D.h"
#include "AnimaVehicle3D.h"
#include "LandsideVehicle3D.h"
#include "SceneNodeType.h"



class Point;
class IEnvModeEditContext;
class IARCportLayoutEditContext;
class CGuid;
class CBaseObject;
class CPipeBase;
struct tgaInfo;
class InputAirside;
class SceneNodeQueryData;
class ILandsideEditCommand;
class FallbackData;
class QueryData;
class IRenderSceneMouseState;
class ARCPath3;
class RailwayInfo;
class CLandsidePortals;
class CSharpModeBackGroundScene;
//replacement for 3dview
//
class CRender3DSceneTags;
class ILayoutObject;
class CRenderFilletTaxiway3DInAirport;
class FloorSurfaceRender;
enum LinkGroupState;
enum Cross_LightState;
class CTempTracerData;

class RENDERENGINE_API  CRender3DScene :
	public C3DSceneBase 
{
	friend class CRenderLandside3D;
public:
	//
	CRender3DScene();
	~CRender3DScene();		
	void Setup(IARCportLayoutEditContext* pEdCtx); //setup scene camera
	

	IARCportLayoutEditContext* mpEditor;//@remove
	IEnvModeEditContext* GetEditContext()const; //@remove

	//create or retrieve Layout Object
	C3DNodeObject GetLayoutObjectNode(const CGuid& guid);
	

	CString GetMouseHintText()const;
	BOOL isInDistanceMeasure()const;
	
public:
	//
	void StartAnim( long t );
	void UpdateAnim( long t );
	void StopAnim();
	long m_lastAnimaTime;
	
	//
	Ogre::TextureUnitState* m_pConveyTexture; //temp convey Texture unit pointer for animation

	//mouse functions on the scene, inherit from scene base
	virtual BOOL OnLButtonDown(const MouseSceneInfo& mouseInfo);
	virtual BOOL OnMouseMove(const MouseSceneInfo& mouseInfo);
	virtual BOOL OnLButtonUp(const MouseSceneInfo& mouseInfo);
	virtual BOOL OnRButtonDown(const MouseSceneInfo& mouseInfo);
	virtual BOOL OnLButtonDblClk(const MouseSceneInfo& mouseinfo);
	virtual Ogre::Plane getFocusPlan()const;

	void UpdateDrawing();

	void BeginMouseOperation(HWND fallbackWnd, LPARAM lParam);	
	void EndMouseOperation();
	

	//NS3DViewCommon::EMouseState GetMouseState() const { return m_eMouseState; }
	void FlipLayoutLocked();


	//C3DNodeObject* GetSel3DNodeRealObj( const MouseSceneInfo &mouseInfo, SceneNodeQueryData& nodeQueryData );
	tgaInfo* GetTextureInfo(unsigned int nID) const;
	void ReloadTextures();

	enum PathType
	{
		TexturePath,
	};
	CString GetPath(PathType type) const;

	CAnimaPax3DList& GetPax3DList() { return m_animPaxList; }
	CAnimaFlight3DList& GetFlight3DList() { return m_flightList; }
	CAnimaVehicle3DList& GetVehicle3DList() { return m_vehicleList; }
	//CRenderProcessor3DList& GetProcessor3DList(){ return m_processors3D; }
	CAnimaElevator3DList& GetElvator3DList(){ return m_animElevatorList; }
	CAnimaTrain3DList& GetTrain3DList(){ return m_animTransList; }
	CAnimaResourceElm3DList& GetResource3DList(){ return m_animResourceList; };
	CAnimaLandsideVehicle3DList& GetLandsideVehicle3DList(){ return m_animlsVehcile3DList; }
    
	CRender3DSceneTags& Get3DTags();

	static void SetSceneNodeQueryData(C3DNodeObject& node, const SceneNodeQueryData& data);
	static void SetSceneNodeQueryDataInt(C3DNodeObject& node, SceneNodeType eNodeType, int nData);


	bool UpdateSelectRedSquares();
	//void UpdateProcessorTagShow(BOOL b);
	//void OnUpdateLayoutObject(ILayoutObject* pObj);
	//void RenderVehicleTraces(std::vector<CTrackerVector3>& track,CString str,bool mShow);
	void RenderVehicleTraces(CTempTracerData* tempVehicleTracerData,bool mShow);

	bool OnUpdate( LPARAM lHint, CObject* pHint );	

	IRenderSceneMouseState* getMouseOperation()const;
protected:
	void ReleaseTextures();
	//CRender3DSceneTags* m_renderTags;
//	CSharpModeBackGroundScene* m_sharpScene;

	void SetupSharpRect();

	//

protected:
	//mobile elements
	CAnimaPax3DList m_animPaxList;
	CAnimaElevator3DList m_animElevatorList;
	CAnimaTrain3DList m_animTransList;
	CAnimaResourceElm3DList m_animResourceList;

	CAnimaFlight3DList m_flightList;
	CAnimaVehicle3DList m_vehicleList;
	CAnimaLandsideVehicle3DList m_animlsVehcile3DList;

	CRenderFilletTaxiway3DInAirport* m_pRenderFilletTaxiway3DInAirport;
	FloorSurfaceRender *m_pFloorSurfaceRender;
	IRenderSceneMouseState* m_pMouse3DOperation;	

	std::vector<tgaInfo*> m_vecTextures;	

public:
	//lister the scene's changes
	BEGINE_LISTENER_DECLARE()
		LISTEN_METHOD_DECLARE(void OnUpdateDrawing())
		LISTEN_METHOD_DECLARE(void OnFinishMeasure(double dDistance))
		LISTEN_METHOD_DECLARE(void UpdateAlignMarker(ARCVector3 ptMousePos))
		//LISTEN_METHOD_DECLARE(void* OnQueryDataObject(const CGuid& guid, const GUID& cls_guid))
		//LISTEN_METHOD_DECLARE(bool OnQueryDataObjectList(std::vector<void*>& vData, const GUID& cls_guid))
		//LISTEN_METHOD_DECLARE(bool OnSaveDataObject(const CGuid& guid, const GUID& cls_guid))
		//LISTEN_METHOD_DECLARE(CString OnQueryPath(CRender3DScene::PathType type))		
		//LISTEN_METHOD_DECLARE(InputAirside* OnQueryInputAirside())
		//LISTEN_METHOD_DECLARE(bool GetAllRailWayInfo(std::vector<RailwayInfo*>& railWayVect))
		//LISTEN_METHOD_DECLARE(int OnQueryFloorCount())
		//LISTEN_METHOD_DECLARE(bool OnQueryFloorVisible(int floor))
		//LISTEN_METHOD_DECLARE(double OnQueryFloorHeight(int floor))
		LISTEN_METHOD_DECLARE(void OnClick3DNodeObject(C3DNodeObject* p3DObj,SceneNodeType eNodeType, int id))
	END_LISTENER_DECLARE()

public:

	//------------------------------------------------------------
	// Data Objects Query
	// the specified class DataType must has a static member of unique GUID value with name class_guid
	//template <class DataType>
	//	DataType* QueryDataObject(const CGuid& guid)
	//{
	//	return 0;//(DataType*)DEAL_FIRST_LISTENER(OnQueryDataObject(guid, DataType::getTypeGUID()));
	//}
	//template <class DataType>
	//	bool QueryDataObjectList(std::vector<DataType*>& vData)
	//{
	//	/*std::vector<void*> vPtrs;
	//	if (DEAL_FIRST_LISTENER(OnQueryDataObjectList(vPtrs, DataType::getTypeGUID())))
	//	{
	//		vData.reserve(vData.size() + vPtrs.size());
	//		for (std::vector<void*>::iterator ite = vPtrs.begin();ite!=vPtrs.end();ite++)
	//		{
	//			vData.push_back((DataType*)*ite);
	//		}
	//		return true;
	//	}*/
	//	return false;
	//}
	// 3D Object Query
	/*Ogre::SceneNode* AddGUIDQueryableSceneNode(const CGuid& guid, const GUID& cls_guid);
	template <class DataType>
		Ogre::SceneNode* AddGUIDQueryableSceneNode(const DataType* pData)
	{
		return AddGUIDQueryableSceneNode(pData->getGuid(), DataType::getTypeGUID());
	}
	static void ReferToParentWhenQuery(C3DNodeObject nodeObj);
	C3DNodeObject* Query3DObject( C3DNodeObject org3DObj, SceneNodeQueryData& nodeQueryData );*/
	//CModeBase3DObject::SharedPtr QueryGUID3DObject( const CGuid& guid, const GUID& cls_guid );
	// Update Data Object 3D
	//void Update3DObject(const CGuid& guid, const GUID& cls_guid);
	//void Update3DObjectList(const GUID& cls_guid);
	// Save Data Object
	//void SaveDataObject(const CGuid& guid, const GUID& cls_guid);

	//InputAirside* OnQueryInputAirside();
	//bool GetAllRailWayInfo(std::vector<RailwayInfo*>& railWayVect);
	//int OnQueryFloorCount();
	//bool OnQueryFloorVisible(int floor);
	//double OnQueryFloorHeight(int floor);

	//------------------------------------------------------------
	
protected:
	//void OnPickAScenePoint(const ARCVector3& pt);
	//void OnDoneScenePointPick(const ARCVector3& lastpt);
	//void OnCancleScenePick();
	
public:
	void SetIntersectionTrafficLight(int nIntersectionID,int nGroupID,LinkGroupState eState);
	void SetCrossWalkTrafficLight(int nCrossWalkID,Cross_LightState eState);
	void UpdataTerminalProcessors(IARCportLayoutEditContext* pEdCtx);
};


