#pragma once
#include "3dscene.h"
#include "Grid3D.h"
#include <Common/Grid.h>
#include <Common/Camera.h>
#include <Common/ListenerCommon.h>

#include "Lines3D.h"

namespace Ogre
{
	class Camera;
}
class C3DObjModelEditContext;
class Point;

class RENDERENGINE_API  C3DObjModelEditScene :
	public C3DSceneBase
{
public:
	//lister the scene's changes
	BEGINE_LISTENER_DECLARE()
		LISTEN_METHOD_DECLARE(void OnUpdateDrawing())
		LISTEN_METHOD_DECLARE(void OnFinishMeasure(double dDistance))
		LISTEN_METHOD_DECLARE(void OnSelect3DObj(C3DNodeObject nodeObj))
	END_LISTENER_DECLARE()

public:
	void UpdateDrawing();

	//
	C3DObjModelEditScene();
	void Setup(C3DObjModelEditContext* pEdCtx); //setup scene camera

	//save model snap bmp and mesh
	void SaveModel();
	// load data from working directory
	bool LoadModel();

	void StartDistanceMeasure();
	void UpdatePickLine();

public:
	//mouse functions on the scene, inherit from scene base
	virtual BOOL OnLButtonDown(const MouseSceneInfo& mouseInfo);
	virtual BOOL OnMouseMove(const MouseSceneInfo& mouseInfo);
	virtual BOOL OnLButtonUp(const MouseSceneInfo& mouseInfo);

	virtual BOOL OnRButtonDown(const MouseSceneInfo& mouseInfo);

	void UpdateGrid();

	C3DObjModelEditContext* GetEditContext() const { return mpEditContext; }
	void SetEditContext(C3DObjModelEditContext* pEditContext){ mpEditContext = pEditContext; }

	CGrid& GetModelGrid();
	//CCameraData& GetActiveCam();

	C3DNodeObject AddNewComponent(CString strMeshFileName, const ARCVector3& pos);


	enum MouseState
	{
		Default_state = 0,

		Measure_state,
	};
	MouseState GetMouseState() const { return m_eMouseState; }

	enum ViewType
	{
		VT_TypeStart = 0,

		VT_Solid = VT_TypeStart,
		VT_Transparent,
		VT_Wireframe,
		VT_Solid_Wireframe,
		VT_Transparent_Wireframe,
		VT_Hidden_Wire,

		VT_TypeEnd,
	};
	static LPCTSTR const m_lpszViewType[];
	static ViewType GetViewTypeFromString(LPCTSTR lpszViewType);

	ViewType GetModelViewType() const { return mModelViewType; }
	void SetModelViewType(ViewType modelViewType);

protected:
	MouseState m_eMouseState;
	CLines3D m_picLine3D;
	std::vector<ARCVector3> m_vMousePosList;
	ARCVector3 m_vecMoveOfXY;

protected://data members
	C3DObjModelEditContext* mpEditContext;	
	//------------------------------------------------------------
	// for data process and saving
	//CGrid mGrid;
	//CSceneCamera mActiveCam;
	//------------------------------------------------------------

	CGrid3D mGridXY; // for ogre
// 	CGrid3D mGridYZ;
// 	CGrid3D mGridXZ;

	ViewType mModelViewType;
};