#include "StdAfx.h"
#include "modelingsequencemanager.h"
#include "TermPlanDoc.h"
#include "resource.h"

CModelingSequenceManager::CModelingSequenceManager()
{
	m_pTermPlanDoc = NULL;
	m_pLandSideSurfaceAreasNode = NULL;
	m_pTerminalSideSurfaceAreasNode = NULL;
	m_pAirsideSurfaceAreasNode = NULL;
}

CModelingSequenceManager::~CModelingSequenceManager()
{
}

void CModelingSequenceManager::SetTermPlanDoc(CTermPlanDoc *pDoc)
{
	ASSERT(pDoc != NULL);
	m_pTermPlanDoc = pDoc;
}

CTVNode* CModelingSequenceManager::GetTVRootNode()
{
	if(m_pTermPlanDoc->m_systemMode == EnvMode_Terminal)
	{
		return m_msImplTerminal.GetTVRootNode();
	}
	else if (m_pTermPlanDoc->m_systemMode==EnvMode_AirSide)
	{
		return m_msImplAirSide.GetTVRootNode();
	}
	else if (m_pTermPlanDoc->m_systemMode==EnvMode_LandSide)
	{
		return m_msImplLandSide.GetTVRootNode();
	}
	else
		ASSERT(0);

	return NULL;
}
CTVNode* CModelingSequenceManager::FindNodeByData(DWORD dwData)
{
	std::deque<CTVNode*> vList;
	//put top level nodes into list..
	CTVNode* pRootNode = NULL;
	if (m_pTermPlanDoc->m_systemMode == EnvMode_Terminal)
		pRootNode = m_msImplTerminal.GetTVRootNode();
	else if (m_pTermPlanDoc->m_systemMode == EnvMode_AirSide)
		pRootNode = m_msImplAirSide.GetTVRootNode();
	else if (m_pTermPlanDoc->m_systemMode==EnvMode_LandSide)
	{
		pRootNode=m_msImplLandSide.GetTVRootNode();
	}
	else
	{
		ASSERT(0);
	}

	ASSERT(pRootNode != NULL);
	vList.push_front(pRootNode);

	while(vList.size() != 0) {
		CTVNode* pNode = vList[0];
		vList.pop_front();
		if(pNode->m_dwData == dwData)
			return pNode;
		for(int i=0; i<pNode->GetChildCount(); i++) {
			vList.push_front((CTVNode*) pNode->GetChildByIdx(i));
		}
	}
	return NULL;

}

CTVNode *CModelingSequenceManager::FindNodeByName(LPCTSTR sName)
{
	std::deque<CTVNode*> vList;
	//put top level nodes into list..
	
	CTVNode* pRootNode = NULL;
	if (m_pTermPlanDoc->m_systemMode == EnvMode_Terminal)
		pRootNode = m_msImplTerminal.GetTVRootNode();
	else if (m_pTermPlanDoc->m_systemMode == EnvMode_AirSide)
	{
		//pRootNode = m_msImplAirSide.GetTVRootNode();
		return m_msImplAirSide.GetProcNode((CString)sName);
	}
	else if (m_pTermPlanDoc->m_systemMode==EnvMode_LandSide)
	{
		pRootNode=m_msImplLandSide.GetTVRootNode();
	}
	else
		ASSERT(0);

	ASSERT(pRootNode != NULL);
	vList.push_front(pRootNode);

	while(vList.size() != 0) {
		CTVNode* pNode = vList[0];
		vList.pop_front();
		if(pNode->Name().CompareNoCase(sName) == 0)
			return pNode;
		for(int i=0; i<pNode->GetChildCount(); i++) {
			vList.push_front((CTVNode*) pNode->GetChildByIdx(i));
		}
	}
	return NULL;
}

CTVNode *CModelingSequenceManager::FindNodeByName(UINT idName)
{
	CString s;
	if(!s.LoadString(idName))
		return NULL;
	return FindNodeByName(s);
}

void CModelingSequenceManager::InitMSManager()
{
	m_msImplAirSide.SetTermPlanDoc(m_pTermPlanDoc);
	m_msImplAirSide.BuildTVNodeTree();

	m_msImplTerminal.SetTermPlanDoc(m_pTermPlanDoc);
	m_msImplTerminal.BuildTVNodeTree();

	m_msImplLandSide.SetTermPlanDoc(m_pTermPlanDoc);
	m_msImplLandSide.BuildTVNodeTree();
}

CTVNode* CModelingSequenceManager::GetAirSideRootTVNode()
{
	return m_msImplAirSide.GetTVRootNode();
}

CTVNode* CModelingSequenceManager::GetTerminalRootTVNode()
{
	return m_msImplTerminal.GetTVRootNode();
}
CTVNode* CModelingSequenceManager::GetLandSideRootTVNode()
{
	return m_msImplLandSide.GetTVRootNode();
}

CTVNode* CModelingSequenceManager::GetSurfaceAreasNode(EnvironmentMode mode)
{
	ASSERT(m_pTermPlanDoc != NULL);

	CTVNode* pRetNode = NULL;

	EnvironmentMode curMode;
	if (mode == EnvMode_Unknow)
		curMode = m_pTermPlanDoc->m_systemMode;
	else
		curMode = mode;

	switch(curMode)
	{
	case EnvMode_LandSide:
		pRetNode = m_pLandSideSurfaceAreasNode;
		break;

	case EnvMode_Terminal:
		pRetNode = m_pTerminalSideSurfaceAreasNode;
		break;

	case EnvMode_AirSide:
		pRetNode = m_pAirsideSurfaceAreasNode;
		break;
	default:
		{
			ASSERT(0);
			break;
		}
	}

	return pRetNode;
}

void CModelingSequenceManager::SetSurfaceAreasNode(CTVNode* pNode, EnvironmentMode mode)
{
	ASSERT(m_pTermPlanDoc != NULL);

	EnvironmentMode curMode;
	if (mode == EnvMode_Unknow)
		curMode = m_pTermPlanDoc->m_systemMode;
	else
		curMode = mode;
	
	switch(curMode)
	{
	case EnvMode_LandSide:
		m_pLandSideSurfaceAreasNode = pNode;
		break;

	case EnvMode_Terminal:
		m_pTerminalSideSurfaceAreasNode = pNode;
		break;

	case EnvMode_AirSide:
		m_pAirsideSurfaceAreasNode = pNode;
		break;

	default:
		{
			ASSERT(0);
			break;
		}
	}
}

void CModelingSequenceManager::AddSurfaceAreaNode(LPCTSTR lpszName,int SurfaceID, EnvironmentMode mode)
{
	CTVNode* pParentNode = GetSurfaceAreasNode(mode);
	if (pParentNode == NULL)
		return;
	CTVNode* pNodeLevel=NULL;
	//parse the string ben 
	CString strName(lpszName);
	CString strToken;
	CString prestrToken="";
	int curpos=0;
	do{		
		strToken=strName.Tokenize("-",curpos);
		if(prestrToken!=""){
			pNodeLevel = (CTVNode*)pParentNode->GetChildByName(prestrToken);
			if(strToken==""){
				pNodeLevel = new CTVNode(prestrToken,IDR_CTX_STRUCTURE);
				pParentNode->AddChild(pNodeLevel);
				pNodeLevel->m_dwData=SurfaceID;
			}else
			{
				pNodeLevel = new CTVNode(prestrToken);
				pParentNode->AddChild(pNodeLevel);
			}
			pParentNode = pNodeLevel;
		}
		
		
		prestrToken=strToken;
	}while(strToken!="");
	
}



void CModelingSequenceManager::AddWallShapeNode(LPCTSTR lpszName, int WallShapeID ,EnvironmentMode mode )
{
	CTVNode* pParentNode = FindNodeByName(IDS_TVNN_WALLSHAPE);
	if (pParentNode == NULL)
		return;
	CTVNode* pNodeLevel=NULL;
	//parse the string ben 
	CString strName(lpszName);
	CString strToken;
	CString prestrToken="";
	int curpos=0;
	do{		
		strToken=strName.Tokenize("-",curpos);
		if(prestrToken!=""){
			pNodeLevel = (CTVNode*)pParentNode->GetChildByName(prestrToken);
			if(strToken==""){
				pNodeLevel = new CTVNode(prestrToken,IDR_CTX_WALLSHAPE);
				pParentNode->AddChild(pNodeLevel);
				pNodeLevel->m_dwData=WallShapeID;
			}else
			{
				pNodeLevel = new CTVNode(prestrToken);
				pParentNode->AddChild(pNodeLevel);
			}
			pParentNode = pNodeLevel;
		}


		prestrToken=strToken;
	}while(strToken!="");
}

/************************************************************************
FUNCTION: node view reload the wall shape list,refresh the node view
IN		: mode,the current environment(terminal,airside)
OUT		:                                                                      
/************************************************************************/
void CModelingSequenceManager::RefreshWallShape(EnvironmentMode mode /* = EnvMode_Unknow */)
{
	switch(mode)
	{
	case EnvMode_AirSide:
		m_msImplAirSide.AddWallShapeGroup();
		break;
	case EnvMode_Terminal:
		m_msImplTerminal.AddWallShapeGroup();
		break;
	default:
		{
			ASSERT(0);
			break;
		}
	}
}
/************************************************************************
FUNCTION: node view reload the surface(structure) list,refresh the node view
IN		: mode,the current environment(terminal,airside)
OUT		:                                                                      
/************************************************************************/
void CModelingSequenceManager::RefreshSurface(EnvironmentMode mode /* = EnvMode_Unknow */)
{
	switch(mode)
	{
	case EnvMode_AirSide:
		m_msImplAirSide.AddStructureGroup();
		break;
	case EnvMode_Terminal:
		m_msImplTerminal.AddStructureGroup();
		break;
	default:
		{
			ASSERT(0);
			break;
		}
	}	
}