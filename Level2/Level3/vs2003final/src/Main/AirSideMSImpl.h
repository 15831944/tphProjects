#pragma once

//class CModelingSequenceImpl;
#include "ModelingSequenceImpl.h"
#include "TVNode.h"
#include "Common\template.h"
#include <map>
#include <utility>
#include "../Engine/ContourTree.h"

namespace AirsideGUI
{
	class NodeViewDbClickHandler;
}
typedef void (AirsideGUI::NodeViewDbClickHandler::*PMSVDbClickFunction)();


class CAirSideMSImpl :public CModelingSequenceImpl
{
public:
	CAirSideMSImpl();
	~CAirSideMSImpl();
	virtual void BuildTVNodeTree();
	/************************************************************************
	FUNCTION: add structure(surface) to node view
	IN		: pGroupNode,the parent node of the node added
	OUT		:                                                                
	/************************************************************************/
	void AddStructureGroup(CTVNode* pGroupNode = NULL);
	/************************************************************************
	FUNCTION: add WallShape to node view
	IN		: pGroupNode,the parent node of the node added(NULL DEFAULT)
	OUT		:                                                                
	/************************************************************************/
	void AddWallShapeGroup(CTVNode* pGroupNode = NULL);
	void AddProcessorGroup(int iType, CTVNode* pGroupNode = NULL);
	//add deice processor group
	void AddProcessorDeiceGroup(CTVNode* pGroupNode = NULL);
	//add ARP Processor Group
	void AddProcessorARPGroup(CTVNode* pGroupNode = NULL);

	CTVNode* GetProcNode(ProcessorClassList ProcType);
	CTVNode* GetProcNode(CString& str);
	CTVNode* FindProcTypeNode(int iType);

	BOOL ChangeNodeName(const CString& strSrc, const CString& strDest);
	void AddNode( CContourNode *pNode,CTVNode *pTVNode);

	CTVNode *RebuildProcessorTree(ProcessorClassList ProcType);

	PMSVDbClickFunction GetItemDbClickFunction(CTVNode* pNode);

protected:
	//process node
	TVNODELISTPCL m_tvNodeListPcl;
	TVNODELISTSTR m_tvNodeListStr;
	//other nodep

	// 
	std::map<CTVNode*, PMSVDbClickFunction> m_mapItemDbClick;
};
