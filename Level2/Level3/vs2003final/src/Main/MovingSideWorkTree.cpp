// MovingSideWorkTree.cpp: implementation of the CMovingSideWorkTree class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MovingSideWorkTree.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMovingSideWorkTree::CMovingSideWorkTree()
{

}

CMovingSideWorkTree::~CMovingSideWorkTree()
{

}
void CMovingSideWorkTree::LoadData( InputTerminal* _pInTerm, ProcessorDatabase* _pProcDB, int _nProcType )
{
	/*
	m_nProcType = _nProcType;
	m_pInTerm = _pInTerm;
	m_pProcDB = _pProcDB;
	
	DeleteAllItems();
	m_vectID.clear();
	
	m_pProcList = _pInTerm->procList;
	if( m_pProcList == NULL )
		return;
	
	// Set data to processor tree
	int includeBarriers = 0;
	
	StringList strList;
	m_pProcList->getAllGroupNames (strList, _nProcType );
	if (includeBarriers && !strList.getCount())
		if (includeBarriers)
			m_pProcList->getAllGroupNames( strList, BarrierProc);
		
		for( int i = 0; i < strList.getCount(); i++ )
		{
			CString csLabel = strList.getString(i);
			HTREEITEM hItem = InsertItem( csLabel );
			if( _pProcDB )
			{
				SetItemData( hItem, GetDBIndex( csLabel, _pProcDB ) );
				if( IsSelected( csLabel, _pProcDB ) )
				{
					SetItemColor( hItem, SELECTED_COLOR );
				}
				else
				{
					SetItemColor( hItem, RGB(0,0,0));
					SetTextColor( -1);
				}
			}

			// Added by Xie Bo, 2002.5.9
		    // Ensure Visible
			if(csLabel==m_strSelectedID)
			{
				hSelItem=hItem;
			}
			
			LoadChild( hItem, csLabel );
		}

	if( hSelItem )
		SelectItem( hSelItem );	
*/
}