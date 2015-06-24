// ProcGroupIdxTreeNode.cpp: implementation of the CProcGroupIdxTreeNode class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "ProcGroupIdxTreeNode.h"
#include <CommonData/procid.h>
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CProcGroupIdxTreeNode::CProcGroupIdxTreeNode():m_iIdxInStringDic( -1 )
{
	m_groupIdx.start = -1;
	m_groupIdx.end = -1;
}

CProcGroupIdxTreeNode::~CProcGroupIdxTreeNode()
{
	ClearTree();
}
void CProcGroupIdxTreeNode::BuildTree( const CStructureID* _pProcID , int _iLevel, const GroupIndex& _groupIdx)
{
	if( _iLevel >= _pProcID->idLength() )
	{
		return;
	}
	
	ASSERT( _pProcID );
	std::list<CProcGroupIdxTreeNode*>::iterator iter = m_listChild.begin();
	std::list<CProcGroupIdxTreeNode*>::iterator iterEnd = m_listChild.end();
	for( ; iter!= iterEnd; ++iter )
	{
		if( (*iter)->GetIdxInStringDic() == _pProcID->getIdIdxAtLevel( _iLevel ) )
		{
			if( _pProcID->idLength() == _iLevel +1 )
			{
				(*iter)->SetGroupIdx( _groupIdx );
			}
			else
			{
				(*iter)->BuildTree( _pProcID, _iLevel +1 , _groupIdx );
			}			
			return; 
		}
	}
	
	//not find
	CProcGroupIdxTreeNode* pNode = new CProcGroupIdxTreeNode;
	if( _pProcID->idLength() == _iLevel +1 )
	{
		pNode->SetGroupIdx( _groupIdx );
	}
	pNode->SetIdxInStringDic( _pProcID->getIdIdxAtLevel( _iLevel ) );
	pNode->BuildTree( _pProcID, _iLevel +1 , _groupIdx );
	m_listChild.push_back( pNode );	
}
GroupIndex CProcGroupIdxTreeNode::FindGroup( const CStructureID& _pID , int _iLevel )const
{
	if( _iLevel >= _pID.idLength() )
	{
		GroupIndex errorGroup;
		errorGroup.start = errorGroup.end = -1;
		return errorGroup;
	}
	
	std::list<CProcGroupIdxTreeNode*>::const_iterator iter = m_listChild.begin();
	std::list<CProcGroupIdxTreeNode*>::const_iterator iterEnd = m_listChild.end();
	for( ; iter!= iterEnd; ++iter )
	{
		if( (*iter)->GetIdxInStringDic() == _pID.getIdIdxAtLevel( _iLevel ) )
		{
			if( _pID.idLength() == _iLevel +1 )
			{
				return (*iter)->GetGroupIdx();
			}
			else
			{
				return (*iter)->FindGroup( _pID, _iLevel + 1 );
			}				
		}
	}	

	GroupIndex errorGroup;
	errorGroup.start = errorGroup.end = -1;
	return errorGroup;
}
void CProcGroupIdxTreeNode::ClearTree()
{
	std::list<CProcGroupIdxTreeNode*>::iterator iter = m_listChild.begin();
	std::list<CProcGroupIdxTreeNode*>::iterator iterEnd = m_listChild.end();
	for( ; iter!= iterEnd; ++iter )
	{
		delete *iter;
	}
	
	m_listChild.clear();
}