// PaxFlowTree.h: interface for the CPaxFlowTree class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PAXFLOWTREE_H__BF11174D_BCFE_4064_8712_DF583C4A07B8__INCLUDED_)
#define AFX_PAXFLOWTREE_H__BF11174D_BCFE_4064_8712_DF583C4A07B8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "PaxFlowTreeNode.h"
#include "inputs\MobileElemConstraint.h"
class CPaxFlowTree;
typedef std::vector<CPaxFlowTree* > PAXFLOW ;
class CPaxFlowTree  
{

public:
	CPaxFlowTree();
	virtual ~CPaxFlowTree();
private:
	CPaxFlowTreeNode* m_treeHead;
	CMobileElemConstraint* m_pPaxConstrain;
private:
	void DisplaySubTree( CTreeCtrl* _pTreeCtrl,HTREEITEM _hParentItem ,CPaxFlowTreeNode* _pCurrentNode );
public:
	CPaxFlowTreeNode * GetTreeHead() { return m_treeHead ;};
	void InsertBeforeNode( CPaxFlowTreeNode * _pBeforeNode , CPaxFlowTreeNode* _pNewNode );
//	void AddNewPaxConstrain( InputTerminal* _pInput,CString _csRawPaxString );
	CString GetPaxConstrainScreenStr() const;
	void DisplayTree( CTreeCtrl* _pTreeCtrl,HTREEITEM _hParentItem );

};

#endif // !defined(AFX_PAXFLOWTREE_H__BF11174D_BCFE_4064_8712_DF583C4A07B8__INCLUDED_)
