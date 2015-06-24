// ProcGroupIdxTree.h: interface for the CProcGroupIdxTree class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PROCGROUPIDXTREE_H__36864919_C7CD_4B44_B105_8EB4814DD972__INCLUDED_)
#define AFX_PROCGROUPIDXTREE_H__36864919_C7CD_4B44_B105_8EB4814DD972__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
class CProcGroupIdxTreeNode;
class CStructureID;
#include "../common/template.h"
#include <list>
class CProcGroupIdxTree  
{
	std::list<CProcGroupIdxTreeNode*>m_listTreeRoot;
public:
	CProcGroupIdxTree();
	virtual ~CProcGroupIdxTree();
public:
	void BuildTree( const CStructureID* _pProcID , const GroupIndex& _groupIdx );
	GroupIndex FindGroup( const CStructureID& _pID )const;
	void ClearTree();
};

#endif // !defined(AFX_PROCGROUPIDXTREE_H__36864919_C7CD_4B44_B105_8EB4814DD972__INCLUDED_)
