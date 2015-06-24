#include "StdAfx.h"
#include ".\selectionmap.h"
#include "./ALTObject3D.h"
#include <algorithm>

CSelectionMap::CSelectionMap(void)
{
}

CSelectionMap::~CSelectionMap(void)
{
}

GLuint CSelectionMap::NewSelectable( Selectable * psel )
{
	if(!psel) return 0; 
	if(m_map.find(psel) != m_map.end())
		return m_map[psel];

	GLuint id =  m_map.size() + 0x0FFF0000;
	return m_map[psel] = id;
}

void CSelectionMap::Clear()
{
	m_map.clear();
}

GLint CSelectionMap::GetSelectID( Selectable * psel ) 
{
	return m_map[psel];
}

Selectable * CSelectionMap::GetSelectable(GLint selID)const{
	
	for(SelectionMapType::const_iterator itr = m_map.begin(); itr != m_map.end(); itr++){
		if(selID == (*itr).second ) return (*itr).first;
	}
	return NULL;
}

Selectable::Selectable()
{
	m_bEnableSelect  = true;
	m_bSelected = false;
	m_bOnMove = false;
}

void Selectable::OnMove( DistanceUnit dx, DistanceUnit dy,DistanceUnit dz /*= 0*/ )
{
	m_bOnMove = true;
}

void Selectable::AfterMove()
{
	m_bOnMove = false;
}

void SelectableList::insert( Selectable * pSel )
{
	if(!pSel) return ;

	if( end() != std::find(begin(),end(),pSel) ) return;

	if(pSel->GetSelectType() == Selectable::ALT_OBJECT ){
		push_back(pSel);
	}else if(pSel->GetSelectType() == Selectable::ALT_OBJECT_PATH_CONTROLPOINT ) {
		std::vector< ref_ptr<Selectable> >::insert(begin(),pSel);
	}else
	{
		push_back(pSel);
	}

}

void SelectableList::GetSelectObjects( ALTObject3DList& list )
{
	for(SelectableList::iterator itr = begin(); itr!=end();itr++){
		if( (*itr)->GetSelectType() == Selectable::ALT_OBJECT ){
			list.push_back( (ALTObject3D*)(*itr).get() );
		}
	}
}