#pragma once

#include "ModelingSequenceImpl.h"
#include "Processor2.h"
class CTerminalMSImpl :public CModelingSequenceImpl
{
public:
	CTerminalMSImpl();
	~CTerminalMSImpl();
	virtual void BuildTVNodeTree();
	void RebuildProcessorTree(const ProcessorID* _pIdToSelect);
	void RebuildProcessor2Tree();
	/************************************************************************
	FUNCTION: add WallShape to node view
	IN		: pGroupNode,the parent node of the node added(NULL DEFAULT)
	OUT		:                                                                
	/************************************************************************/
	void AddWallShapeGroup(CTVNode* pGroupNode  = NULL );
	/************************************************************************
	FUNCTION: add WallShape to node view
	IN		: pGroupNode,the parent node of the node added(NULL DEFAULT)
	OUT		:                                                                
	/************************************************************************/
	void AddStructureGroup(CTVNode* pGroupNode  = NULL );
};
