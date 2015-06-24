#pragma once

#include "ModelingSequenceImpl.h"
#include "Common\template.h"
#include "../Engine/LandfieldProcessor.h"
#include <map>
#include <utility>


enum ALTOBJECT_TYPE;
class CLandSideMSImpl :public CModelingSequenceImpl
{
public:
	CLandSideMSImpl();
	~CLandSideMSImpl();
	
	virtual void BuildTVNodeTree();
	void RebuildLayoutObjectTree(ALTOBJECT_TYPE ProcType);
	CTVNode* FindObjectTypeNode(ALTOBJECT_TYPE ProcType);
protected:
	//process node
	TVNODELISTPCL m_tvNodeListPcl;
	TVNODELISTSTR m_tvNodeListStr;
	//void AddProcessorGroup(ProcessorClassList iType, CTVNode* pGroupNode);

	void AddLayoutObjectGroup(ALTOBJECT_TYPE iType,CTVNode* pGroupNode);

	//ProcessorList & GetProcessorList();
};
