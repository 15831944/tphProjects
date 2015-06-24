#pragma once


#include "tvnode.h"

#include <map>
class CTermPlanDoc;
typedef std::map<int, CTVNode*> TVNODELISTPCL;
typedef std::map<std::string, CTVNode*> TVNODELISTSTR;

class CModelingSequenceImpl
{
public:
	CModelingSequenceImpl();
	~CModelingSequenceImpl();
	CTVNode m_tvRootNode;
	virtual void BuildTVNodeTree();

	CTermPlanDoc *m_pTermPlanDoc;

	void SetTermPlanDoc(CTermPlanDoc *pDoc);
	CTVNode *GetTVRootNode();

//	void AddProcessorGroup(int iType, CTVNode* pGroupNode);
//	CTVNode* FindProcTypeNode(int iType);
};
