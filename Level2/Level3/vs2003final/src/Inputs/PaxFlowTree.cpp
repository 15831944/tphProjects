// PaxFlowTree.cpp: implementation of the CPaxFlowTree class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PaxFlowTree.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPaxFlowTree::CPaxFlowTree()
{
	m_treeHead = new CPaxFlowTreeNode();
	m_pPaxConstrain = NULL;
}

CPaxFlowTree::~CPaxFlowTree()
{
	delete m_treeHead;
}
void CPaxFlowTree::InsertBeforeNode( CPaxFlowTreeNode * _pBeforeNode , CPaxFlowTreeNode* _pNewNode )
{
	ASSERT( _pNewNode && _pBeforeNode && _pBeforeNode->GetParentNode() );

	int iIndex = _pBeforeNode->GetParentNode()->GetChildIndex( _pBeforeNode );
	ASSERT( iIndex >= 0 );
	
	_pNewNode->SetParentNode( _pBeforeNode->GetParentNode() );
	_pNewNode->GetParentNode()->SetChild( iIndex , _pNewNode );
	_pNewNode->AddChild( _pBeforeNode );
	_pBeforeNode->SetParentNode( _pNewNode );	
}

/*
void CPaxFlowTree::AddNewPaxConstrain( InputTerminal* _pInput,CString _csRawPaxString )
{
	if( m_pPaxConstrain )
	{
		m_pPaxConstrain->SetInputTerminal( _pInput );
		m_pPaxConstrain->setConstraint( _csRawPaxString );	
	}
	else
	{
		m_pPaxConstrain = new CMobileElemConstraint();
		m_pPaxConstrain->SetInputTerminal( _pInput );
		m_pPaxConstrain->setConstraint( _csRawPaxString );	
	}
}
*/

CString CPaxFlowTree::GetPaxConstrainScreenStr() const
{
	ASSERT( m_pPaxConstrain );
	//char szScreenStr[128];
	CString szScreenStr;
	m_pPaxConstrain->screenPrint(szScreenStr,0,128); 
	return  szScreenStr ;
}


void CPaxFlowTree::DisplayTree( CTreeCtrl* _pTreeCtrl,HTREEITEM _hParentItem )
{
	if( m_treeHead->GetChildCount() > 0 )
	{
		DisplaySubTree( _pTreeCtrl, _hParentItem, m_treeHead );
	}
	else
	{
		_pTreeCtrl->InsertItem( " END (100%) " , _hParentItem );
		_pTreeCtrl->Expand( _hParentItem ,TVE_EXPAND );
		return ;
	}	
	
}
void CPaxFlowTree::DisplaySubTree( CTreeCtrl* _pTreeCtrl,HTREEITEM _hParentItem ,CPaxFlowTreeNode* _pCurrentNode )
{
	int iChildCount = -1;
	
	if( _pCurrentNode &&   (iChildCount = _pCurrentNode->GetChildCount() ) > 0  )
	{
		for( int i = 0; i < iChildCount ; ++i )
		{
			CPaxFlowTreeNode* pChild = _pCurrentNode->GetChild( i );
			ASSERT( pChild );
			CString sProcIDStr = pChild->GetProcessorIDStr();
			CString sPercent;
			sPercent.Format(" (%d%%) ", pChild->GetDistributionPercent() );
			sProcIDStr += sPercent;
			HTREEITEM hSubTree = _pTreeCtrl->InsertItem( sProcIDStr ,_hParentItem );
			if( !pChild->IsReadOnly() )
			{
				//_pTreeCtrl->SetTextColor( RGB(0,0,255 ) );
			}
			else
			{
				//_pTreeCtrl->SetTextColor( RGB( 0,0,0) );
			}
			
			_pTreeCtrl->SetItemData( hSubTree , (DWORD)pChild );
			DisplaySubTree( _pTreeCtrl, hSubTree, pChild );
		}

		_pTreeCtrl->Expand( _hParentItem ,TVE_EXPAND );
	}
	else
	{
		_pTreeCtrl->InsertItem( " END (100%) " , _hParentItem );
		_pTreeCtrl->Expand( _hParentItem ,TVE_EXPAND );
		return;
	}
		
}