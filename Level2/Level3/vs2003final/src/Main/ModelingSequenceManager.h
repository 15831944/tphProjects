#pragma once

#include "AirSideMSImpl.h"
#include "TerminalMSImpl.h"
#include "LandSideMSImpl.h"
#include <Inputs/IN_TERM.H>

class CTermPlanDoc;
class CModelingSequenceManager
{
public:
	CModelingSequenceManager();
	~CModelingSequenceManager();

public:

	CLandSideMSImpl m_msImplLandSide;	
	CAirSideMSImpl m_msImplAirSide;
	CTerminalMSImpl m_msImplTerminal;
	
	CTermPlanDoc *m_pTermPlanDoc;

	void SetTermPlanDoc(CTermPlanDoc* pDoc);

	CTVNode* GetTVRootNode();
	CTVNode* FindNodeByData(DWORD dwData);
	CTVNode* FindNodeByName(LPCTSTR sName);

	CTVNode* FindNodeByName(UINT idName);
	void InitMSManager();


	CTVNode* GetAirSideRootTVNode();
	CTVNode* GetTerminalRootTVNode();
	CTVNode* GetLandSideRootTVNode();

	CTVNode* GetSurfaceAreasNode(EnvironmentMode mode = EnvMode_Unknow);
	void SetSurfaceAreasNode(CTVNode*, EnvironmentMode mode = EnvMode_Unknow);
	void AddSurfaceAreaNode(LPCTSTR lpszName, int SurfaceID ,EnvironmentMode mode = EnvMode_Unknow);
	void AddWallShapeNode(LPCTSTR lpszName, int WallShapeID ,EnvironmentMode mode = EnvMode_Unknow);

	/************************************************************************
	FUNCTION: node view reload the wall shape list,refresh the node view
	IN		: mode,the current environment(terminal,airside)
	OUT		:                                                                      
	/************************************************************************/
	void RefreshWallShape(EnvironmentMode mode = EnvMode_Unknow);
	/************************************************************************
	FUNCTION: node view reload the surface(structure) list,refresh the node view
	IN		: mode,the current environment(terminal,airside)
	OUT		:                                                                      
	/************************************************************************/
	void RefreshSurface(EnvironmentMode mode = EnvMode_Unknow);
private:
	CTVNode* m_pLandSideSurfaceAreasNode;
	CTVNode* m_pTerminalSideSurfaceAreasNode;
	CTVNode* m_pAirsideSurfaceAreasNode;
};
