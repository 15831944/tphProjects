// ProcGroupIdxTree.cpp: implementation of the CProcGroupIdxTree class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "ProcGroupIdxTree.h"
#include "ProcGroupIdxTreeNode.h"
#include <CommonData/procid.h>
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CProcGroupIdxTree::CProcGroupIdxTree()
{

}

CProcGroupIdxTree::~CProcGroupIdxTree()
{
	ClearTree();
}
void CProcGroupIdxTree::BuildTree( const CStructureID* _pProcID , const GroupIndex& _groupIdx )
{
	ASSERT( _pProcID );
	std::list<CProcGroupIdxTreeNode*>::iterator iter = m_listTreeRoot.begin();
	std::list<CProcGroupIdxTreeNode*>::iterator iterEnd = m_listTreeRoot.end();
	for( ; iter!= iterEnd; ++iter )
	{
		if( (*iter)->GetIdxInStringDic() == _pProcID->getIdIdxAtLevel( 0 ) )
		{		
			if( _pProcID->idLength() == 1 )
			{
				(*iter)->SetGroupIdx( _groupIdx );
			}
			else
			{
				(*iter)->BuildTree( _pProcID, 1, _groupIdx );
			}			
			return; 
		}
	}

	//not find
	CProcGroupIdxTreeNode* pNode = new CProcGroupIdxTreeNode;
	if( _pProcID->idLength() == 1 )
	{
		pNode->SetGroupIdx( _groupIdx );
	}
	pNode->SetIdxInStringDic( _pProcID->getIdIdxAtLevel( 0 ) );
	pNode->BuildTree( _pProcID, 1, _groupIdx );
	m_listTreeRoot.push_back( pNode );	
}
GroupIndex CProcGroupIdxTree::FindGroup( const CStructureID& _pID )const
{
	std::list<CProcGroupIdxTreeNode*>::const_iterator iter = m_listTreeRoot.begin();
	std::list<CProcGroupIdxTreeNode*>::const_iterator iterEnd = m_listTreeRoot.end();
	for( ; iter!= iterEnd; ++iter )
	{
		if( (*iter)->GetIdxInStringDic() == _pID.getIdIdxAtLevel( 0 ) )
		{	
			if( _pID.idLength() == 1 )
			{
				return (*iter)->GetGroupIdx(); 
			}
			else
			{
				return (*iter)->FindGroup( _pID, 1 ) ;
			}
		}
	}
	
	GroupIndex errorGroup;
	errorGroup.start = errorGroup.end = -1;
	return errorGroup;
}
void CProcGroupIdxTree::ClearTree()
{
	std::list<CProcGroupIdxTreeNode*>::iterator iter = m_listTreeRoot.begin();
	std::list<CProcGroupIdxTreeNode*>::iterator iterEnd = m_listTreeRoot.end();
	for( ; iter!= iterEnd; ++iter )
	{
		delete *iter;
	}

	m_listTreeRoot.clear();
}
