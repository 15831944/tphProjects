// PaxFlowTreeNode.cpp: implementation of the CPaxFlowTreeNode class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PaxFlowTreeNode.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPaxFlowTreeNode::CPaxFlowTreeNode():m_iDistPercent(0),m_pParentNode( NULL ),m_bReadOnly( true )
{
	
}

CPaxFlowTreeNode::~CPaxFlowTreeNode()
{
	CHILDNODE::iterator iter = m_vChildNodes.begin();
	CHILDNODE::iterator iterEnd = m_vChildNodes.end();
	for( ; iter != iterEnd; ++iter )
		delete *iter;
	m_vChildNodes.clear();

}

CPaxFlowTreeNode* CPaxFlowTreeNode::GetChild( int _iIdx )
{
	ASSERT( _iIdx >=0 && _iIdx < static_cast<int>(m_vChildNodes.size()) );
	return m_vChildNodes[ _iIdx ];
}

void CPaxFlowTreeNode::AddChild( CPaxFlowTreeNode* _pChild )
{
	ASSERT( _pChild );
	_pChild->SetParentNode( this );
	m_vChildNodes.push_back( _pChild );
}

void CPaxFlowTreeNode::DeleteChild( int _iIdx , bool _bDeleteSubtree  )
{
	ASSERT( _iIdx >=0 && _iIdx < static_cast<int>(m_vChildNodes.size()) );
	if( _bDeleteSubtree )
	{
		delete m_vChildNodes[ _iIdx ];
		m_vChildNodes.erase( m_vChildNodes.begin() + _iIdx );
	}
	else
	{
		CPaxFlowTreeNode * pChild = m_vChildNodes [ _iIdx ];
		int iCount = pChild->GetChildCount();
		
		for ( int i=0 ; i< iCount ; ++i )
		{
			pChild->GetChild( i )->SetParentNode( this );
			m_vChildNodes.push_back( pChild->GetChild( i ) );
		}
		pChild->EraseAllChild();
		delete pChild;
		m_vChildNodes.erase( m_vChildNodes.begin() + _iIdx );
	}
}

void CPaxFlowTreeNode::EraseChild( int _iIdx )
{
	ASSERT( _iIdx >=0 && _iIdx < static_cast<int>(m_vChildNodes.size()) );
	m_vChildNodes.erase( m_vChildNodes.begin() + _iIdx );
}
void CPaxFlowTreeNode::EraseAllChild()
{
	m_vChildNodes.clear();
}

int CPaxFlowTreeNode::GetChildIndex( CPaxFlowTreeNode* _pChildNode ) const
{
	CHILDNODE::const_iterator iter = m_vChildNodes.begin();
	CHILDNODE::const_iterator iterEnd = m_vChildNodes.end();
	for(int i=0 ; iter != iterEnd; ++iter ,++i )
	{
		if( *iter == _pChildNode )
		{
			return i;
		}
	}

	return -1;
}
void CPaxFlowTreeNode::SetChild (int _iIdx ,CPaxFlowTreeNode* _pChildNode )
{
	ASSERT( _iIdx >=0 && _iIdx < static_cast<int>(m_vChildNodes.size()) );
	m_vChildNodes.at( _iIdx ) = _pChildNode;
}

void CPaxFlowTreeNode::EvenPercent()
{
	
}

CString CPaxFlowTreeNode::GetProcessorIDStr() const 
{
	char sId [ 128 ];
	m_procId.printID( sId );
	return CString(sId);
}