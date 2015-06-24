// ProcGroupIdxTreeNode.h: interface for the CProcGroupIdxTreeNode class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PROCGROUPIDXTREENODE_H__F0B61100_7EBD_4719_9CC2_286F2F400D5F__INCLUDED_)
#define AFX_PROCGROUPIDXTREENODE_H__F0B61100_7EBD_4719_9CC2_286F2F400D5F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <list>
#include "common\template.h"
class CStructureID;
class CProcGroupIdxTreeNode  
{
	/****** key field ******/
	int m_iIdxInStringDic;

	/***** value field *****/
	GroupIndex m_groupIdx;
	std::list<CProcGroupIdxTreeNode*>m_listChild;
public:
	CProcGroupIdxTreeNode();
	virtual ~CProcGroupIdxTreeNode();
public:
	void SetIdxInStringDic( int _iIdx ) { m_iIdxInStringDic = _iIdx;}
	int GetIdxInStringDic()const  { return m_iIdxInStringDic ;}
	const GroupIndex& GetGroupIdx()const { return m_groupIdx;	}
	void SetGroupIdx( const GroupIndex& _groupIdx ) { m_groupIdx = _groupIdx;	}
	std::list<CProcGroupIdxTreeNode*>& GetChildList() { return m_listChild;};
	bool operator == ( const CProcGroupIdxTreeNode& _anotherNode )const { return m_iIdxInStringDic == _anotherNode.m_iIdxInStringDic ;};

	void BuildTree( const CStructureID* _pProcID , int _iLevel, const GroupIndex& _groupIdx);
	GroupIndex FindGroup( const CStructureID& _pID , int _iLevel )const;
	void ClearTree();
};

#endif // !defined(AFX_PROCGROUPIDXTREENODE_H__F0B61100_7EBD_4719_9CC2_286F2F400D5F__INCLUDED_)
