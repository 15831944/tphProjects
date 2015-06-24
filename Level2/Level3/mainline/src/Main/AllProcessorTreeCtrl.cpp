// AllProcessorTreeCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "AllProcessorTreeCtrl.h"
#include "..\engine\proclist.h"
#include "inputs\in_term.h"
#include "inputs\assigndb.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define SELECTED_COLOR RGB(53,151,53)
/////////////////////////////////////////////////////////////////////////////
// CAllProcessorTreeCtrl

CAllProcessorTreeCtrl::CAllProcessorTreeCtrl()
{
}

CAllProcessorTreeCtrl::~CAllProcessorTreeCtrl()
{
}


BEGIN_MESSAGE_MAP(CAllProcessorTreeCtrl, CProcessorTreeCtrl)
	//{{AFX_MSG_MAP(CAllProcessorTreeCtrl)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	ON_NOTIFY_REFLECT(TVN_ITEMEXPANDED, OnItemexpanded)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAllProcessorTreeCtrl message handlers

void CAllProcessorTreeCtrl::OnItemexpanded(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	HTREEITEM hItem = pNMTreeView->itemNew.hItem;
	if( GetItemText(hItem)=="ALL PROCESSOR")
		return;
 
	CProcessorTreeCtrl::OnItemexpanded( pNMHDR,pResult);
}

void CAllProcessorTreeCtrl::LoadData(InputTerminal *_pInTerm, ProcessorDatabase *_pProcDB, int _nProcType)
{
	m_nProcType = _nProcType;

	m_pInTerm = _pInTerm;
	m_pProcDB = _pProcDB;
	
	DeleteAllItems();
	m_vectID.clear();
	
	m_pProcList = _pInTerm->GetProcessorList();
	if( m_pProcList == NULL )
		return;

	//first insert "ALL PROCESSOR"
	HTREEITEM hRootItem = InsertItem("ALL PROCESSOR");
	SetItemData( hRootItem,-1);
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
			HTREEITEM hItem = InsertItem( csLabel,hRootItem );
			if( _pProcDB )
			{
				SetItemData( hItem, GetDBIndex( csLabel, _pProcDB ) );
				if( IsSelected( csLabel, _pProcDB ) )
				{
					SetItemColor( hItem, SELECTED_COLOR );
					SetItemBold( hItem, true );
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

	Expand(hRootItem,TVE_EXPAND);
}


bool CAllProcessorTreeCtrl::IsSelected( CString strProcess, ProcessorDatabase* _pProcDB )
{
	assert( m_pInTerm );

	ProcAssignDatabase* procAssDB = (ProcAssignDatabase*)_pProcDB;
	CString strTitle = procAssDB->getTitle();
	if( strTitle == CString("Processor Assignment file") )		// is ProcAssignDatabase
	{
		UnmergedAssignments _unMergedAssgn;
		_unMergedAssgn.SetInputTerminal( m_pInTerm );
		int nCount = procAssDB->getCount();
		for( int i=0; i<nCount; i++ )
		{
			const ProcessorID* pProcID = procAssDB->getProcName(i);
			CString str = pProcID->GetIDString();
			if( str == strProcess )
			{
				ProcessorRosterSchedule* pSchd = procAssDB->getDatabase(i);

				if(  pSchd->getCount() == 0)
					return false;
				else
					return true;
			}	
		}
		return false;
	}

	return CProcessorTreeCtrl::IsSelected( strProcess, _pProcDB );
}