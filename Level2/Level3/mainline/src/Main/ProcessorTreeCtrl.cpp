// ProcessorTreeCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "ProcessorTreeCtrl.h"
#include "..\engine\proclist.h"
#include "engine\gate.h"
#include <Inputs/IN_TERM.H>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define SELECTED_COLOR RGB(53,151,53)
/////////////////////////////////////////////////////////////////////////////
// CProcessorTreeCtrl

CProcessorTreeCtrl::CProcessorTreeCtrl()
{
	m_strSelectedID="";
	m_bDisplayAll=FALSE;

	hSelItem=NULL;
	m_bOnlyShowACStandGate = false;
	m_bAirsideMode = false;
}

CProcessorTreeCtrl::~CProcessorTreeCtrl()
{
}

 
BEGIN_MESSAGE_MAP(CProcessorTreeCtrl, CColorTreeCtrl)
//{{AFX_MSG_MAP(CProcessorTreeCtrl)
ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnCustomDraw)
ON_NOTIFY_REFLECT(TVN_ITEMEXPANDED, OnItemexpanded)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProcessorTreeCtrl message handlers

// Load all processor in the _pProcList
// in:
// _pProcList,  the processor list where the processor be loaded.
void CProcessorTreeCtrl::LoadData( ProcessorList* _pProcList )
{	
	DeleteAllItems();
	m_vectID.clear();	
	m_pProcList = _pProcList;
	m_nProcType = -1;
	m_pProcDB = NULL;
	if( m_pProcList == NULL )
		return;	
	// Set data to processor tree
	int includeBarriers = 0;	
	StringList strList;
	m_pProcList->getAllGroupNames (strList, m_nProcType );
	if (includeBarriers && !strList.getCount())
		if (includeBarriers)
			m_pProcList->getAllGroupNames( strList, BarrierProc);
		
		for( int i = 0; i < strList.getCount(); i++ )
		{
			CString csLabel = strList.getString(i);
			
			if( !checkACStandFlag(csLabel) )
				continue;
			
			HTREEITEM hItem = InsertItem( csLabel );
			LoadChild( hItem, csLabel );
		}
}
void CProcessorTreeCtrl::LoadData( ProcessorList* _pProcList , ProcessorDatabase* _pProcDB)
{
	DeleteAllItems();
	m_vectID.clear();	
	m_pProcList = _pProcList;
	m_nProcType = -1;
	m_pProcDB = _pProcDB;
	if( m_pProcList == NULL )
		return;	
	// Set data to processor tree
	int includeBarriers = 0;	
	StringList strList;
	m_pProcList->getAllGroupNames (strList, m_nProcType );
	if (includeBarriers && !strList.getCount())
		if (includeBarriers)
			m_pProcList->getAllGroupNames( strList, BarrierProc);

	for( int i = 0; i < strList.getCount(); i++ )
	{
		CString csLabel = strList.getString(i);

		if( !checkACStandFlag(csLabel) )
			continue;

		HTREEITEM hItem = InsertItem( csLabel );

		if( m_pProcDB )
		{
			SetItemData( hItem, GetDBIndex( csLabel, m_pProcDB ) );


			if( IsSelected( csLabel, m_pProcDB ) )
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

		LoadChild( hItem, csLabel );

	}
}

// Load Processor of _pProcType ( or stand ), set the data if processor is in the _pProcDB
// in:
//   _bAirsideMode, if is airside mode
//   _pInTerm,  inputTerminal where the proclist stay
//   _pProcDB,  the related processor related data.
//   _nProctype,  processor type be loaded
//   bGateStandOnly,  if stand
void CProcessorTreeCtrl::LoadData( InputTerminal* _pInTerm, ProcessorDatabase* _pProcDB, int _nProcType /*=-1*/ ,BOOL bGateStandOnly )
{
	if( _nProcType == StandProcessor )
		m_bAirsideMode = true;
	m_nProcType = _nProcType;

	m_pInTerm = _pInTerm;
	m_pProcDB = _pProcDB;
	
	DeleteAllItems();
	m_vectID.clear();
	
	if (m_bAirsideMode)
	{
		m_pProcList = _pInTerm->GetAirsideProcessorList();
	}
	else
	{
		m_pProcList = _pInTerm->GetTerminalProcessorList();
	}
	
	if( m_pProcList == NULL )
		return;
	
	// Set data to processor tree
	int includeBarriers = 0;
	
	StringList strList;
	m_pProcList->getAllGroupNames (strList, _nProcType ,bGateStandOnly);
	if (includeBarriers && !strList.getCount())
		if (includeBarriers)
			m_pProcList->getAllGroupNames( strList, BarrierProc);
		
		for( int i = 0; i < strList.getCount(); i++ )
		{
			CString csLabel = strList.getString(i);
			
			if( !m_bAirsideMode && !checkACStandFlag(csLabel) )
				continue;

			HTREEITEM hItem = InsertItem( csLabel );
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
			
			LoadChild( hItem, csLabel ,bGateStandOnly);
		}

	if( hSelItem )
		SelectItem( hSelItem );
}



void CProcessorTreeCtrl::LoadGateData( InputTerminal* _pInTerm, ProcessorDatabase* _pProcDB,int _gateType )
{


	m_pInTerm = _pInTerm;
	m_pProcDB = _pProcDB;
	m_nProcType = GateProc ;
	DeleteAllItems();
	m_vectID.clear();

	if (m_bAirsideMode)
	{
		m_pProcList = _pInTerm->GetAirsideProcessorList();
	}
	else
	{
		m_pProcList = _pInTerm->GetTerminalProcessorList();
	}

	if( m_pProcList == NULL )
		return;

	// Set data to processor tree
	int includeBarriers = 0;

	StringList strList;
	m_pProcList->getAllGroupNames (strList, GateProc );

	for( int i = 0; i < strList.getCount(); i++ )
	{
		CString csLabel = strList.getString(i);

		if(!CheckTheGateType(csLabel,_gateType))
			continue;

		HTREEITEM hItem = InsertItem( csLabel );
		if( _pProcDB )
		{
			SetItemData( hItem, GetDBIndex( csLabel, _pProcDB ) );
				SetItemColor( hItem, RGB(0,0,0));
				SetTextColor( -1);
		}

		// Added by Xie Bo, 2002.5.9
		// Ensure Visible
		if(csLabel==m_strSelectedID)
		{
			hSelItem=hItem;
		}

		LoadGateChild( hItem, csLabel ,_gateType);
	}

	if( hSelItem )
		SelectItem( hSelItem );
}
void CProcessorTreeCtrl::LoadGateChild( HTREEITEM _hItem, CString _csStr,int  _type )
{
	// Added by Xie Bo 2002.5.12
	DeleteAllChild(_hItem);

	ProcessorID id;
	id.SetStrDict( m_pInTerm->inStrDict );
	id.setID( (LPCSTR)_csStr );
	StringList strList;
	m_pProcList->getMemberNames( id, strList, m_nProcType,FALSE );
	bool bNeedExpand = FALSE ;
	for( int i = 0; i < strList.getCount(); i++ ) 
	{
		CString csStr = strList.getString( i );
		CString csLabel = _csStr + "-" + csStr;


		if(! CheckTheGateType(csLabel,_type))
			continue;

		HTREEITEM hItem = InsertItem( csLabel, _hItem );
		if( m_pProcDB )
		{
			SetItemData( hItem, GetDBIndex( csLabel, m_pProcDB ) );
			bNeedExpand = TRUE;
			SetItemColor( hItem, RGB(0,0,0));
			SetTextColor( -1);
		}
		if(csLabel==m_strSelectedID)
		{
			hSelItem=hItem;
		}
	}
	if ( bNeedExpand )
	{
		Expand( _hItem, TVE_EXPAND );
	}
	if( hSelItem )
		SelectItem( hSelItem );
}
BOOL CProcessorTreeCtrl::CheckTheGateType(CString _proName , int _type)
{
	ProcessorID procID;
	procID.SetStrDict( m_pInTerm->inStrDict );
	procID.setID( _proName );
	GroupIndex group = m_pInTerm->GetProcessorList()->getGroupIndex( procID );
	if(group.start == -1)
		return FALSE ;
	for( int i= group.start; i<= group.end; i++ )
	{
		Processor* proc = m_pInTerm->GetProcessorList()->getProcessor( i );
		if( proc->getProcessorType() != GateProc )
			continue ;
		// if the AcStandGateFlag of one processor is true.
		// return true
		GateProcessor* pGate = (GateProcessor*)proc;
		if( pGate->getGateType() == (GateType)_type)
			return TRUE ;
	}
	return FALSE ;
}

// load child of _csStr after _hItem.
// in:  
//   _hItem, where to add child after
//   _csStr, processor group id
//   bGateStandOnly,  if stand.
void CProcessorTreeCtrl::LoadChild( HTREEITEM _hItem, CString _csStr,BOOL bGateStandOnly )
{
	// Added by Xie Bo 2002.5.12
	DeleteAllChild(_hItem);
	
	ProcessorID id;
	id.SetStrDict( m_pInTerm->inStrDict );
	id.setID( (LPCSTR)_csStr );
	StringList strList;
	m_pProcList->getMemberNames( id, strList, m_nProcType,bGateStandOnly );
	bool bNeedExpand = FALSE ;
	for( int i = 0; i < strList.getCount(); i++ ) 
	{
		CString csStr = strList.getString( i );
		CString csLabel = _csStr + "-" + csStr;


		if( !m_bAirsideMode && !checkACStandFlag( csLabel) )
			continue;
		
		HTREEITEM hItem = InsertItem( csLabel, _hItem );
		if( m_pProcDB )
		{
			SetItemData( hItem, GetDBIndex( csLabel, m_pProcDB ) );

			
			if( IsSelected( csLabel, m_pProcDB ) )
			{
				SetItemColor( hItem, SELECTED_COLOR );
				SetItemBold( hItem, true );
				bNeedExpand = TRUE;
			}
			else
			{
				SetItemColor( hItem, RGB(0,0,0));
				SetTextColor( -1);
			}
		}
		if(csLabel==m_strSelectedID)
		{
			hSelItem=hItem;
		}
	}
	if ( bNeedExpand )
	{
		Expand( _hItem, TVE_EXPAND );
	}
	if( hSelItem )
		SelectItem( hSelItem );
}

bool CProcessorTreeCtrl::IsSelected( CString strProcess, ProcessorDatabase* _pProcDB )
{
	//// TRACE("The string will be compared is %s\n",strProcess);
	//	if(m_bDisplayAll==TRUE)
	//		return TRUE;
	int nCount = _pProcDB->getCount();
	for( int i=0; i<nCount; i++ )
	{
		const ProcessorID* pProcID = _pProcDB->getProcName( i );
		char buf[128];
		pProcID->printID( buf );
		CString str(buf);
		if( strProcess == str )
			return true;
	}
	return false;
}

int CProcessorTreeCtrl::GetDBIndex(CString _str, ProcessorDatabase * _pProcDB)
{
	int nCount = _pProcDB->getCount();
	for( int i=0; i<nCount; i++ )
	{
		const ProcessorID* pProcID = _pProcDB->getProcName( i );
		char buf[128];
		pProcID->printID( buf );
		CString str(buf);
		if( _str == str )
			return i;
	}
	return -1;
}

void CProcessorTreeCtrl::OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult)
{
/*
switch( pNMHDR->code)
{
case NM_CUSTOMDRAW:
{
LPNMTVCUSTOMDRAW pCustomDraw = (LPNMTVCUSTOMDRAW)lParam;
switch (pCustomDraw->nmcd.dwDrawStage)
{
case CDDS_PREPAINT:
// Need to process this case and set pResult to CDRF_NOTIFYITEMDRAW, 
// otherwise parent will never receive CDDS_ITEMPREPAINT notification. (GGH) 
*pResult = CDRF_NOTIFYITEMDRAW;
return true;

  case CDDS_ITEMPREPAINT:
  switch (pCustomDraw->iLevel)
  {
  // painting all 0-level items blue,
  // and all 1-level items red (GGH)
  case 0:
  if (pCustomDraw->nmcd.uItemState == (CDIS_FOCUS | CDIS_SELECTED)) // selected
  pCustomDraw->clrText = RGB(255, 255, 255);
  else	
  pCustomDraw->clrText = RGB(0, 0, 255);
  break;
  case 1:
  if (pCustomDraw->nmcd.uItemState == (CDIS_FOCUS | CDIS_SELECTED)) // selected
  pCustomDraw->clrText = RGB(255, 255, 255);
  else	
  pCustomDraw->clrText = RGB(255, 0, 0);
  break;
  }
  
	*pResult = CDRF_SKIPDEFAULT;
	return false;
	
	  }
	  }
	  break;
	  }
	  
	return CFrameWnd::OnNotify(wParam, lParam, pResult);*/
}
void CProcessorTreeCtrl::OnItemexpanded(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	HTREEITEM hItem = pNMTreeView->itemNew.hItem;
	HTREEITEM hChildItem = GetChildItem( hItem );
	while( hChildItem )
	{
		LoadChild( hChildItem, GetItemText( hChildItem ) );
		hChildItem = GetNextItem( hChildItem, TVGN_NEXT );
	}	
	*pResult = 0;
}

// not reload. just reset the item data and color.
void CProcessorTreeCtrl::ResetTreeData( HTREEITEM _hItem )
{
	HTREEITEM hItem;
	if( _hItem == NULL )
		hItem = GetRootItem();
	else
		hItem = GetChildItem( _hItem );
	while( hItem )
	{
		CString csLabel = GetItemText( hItem );

		if( m_pProcDB )
		{
			int iDBIdx = GetDBIndex( csLabel, m_pProcDB );
			SetItemData( hItem, GetDBIndex( csLabel, m_pProcDB ) );
			if( IsSelected( csLabel, m_pProcDB) )
			{
				SetItemColor( hItem, SELECTED_COLOR );
				SetItemBold( hItem, true );
			}
			else
			{
				SetItemColor( hItem, RGB(0,0,0) );
				SetItemBold( hItem, false );	
			}
		}
		
		ResetTreeData( hItem );
		hItem = GetNextItem( hItem, TVGN_NEXT );
	}
}

void CProcessorTreeCtrl::DeleteAllChild(HTREEITEM _hItem)
{
	if (ItemHasChildren(_hItem))
	{
		HTREEITEM hNextItem;
		HTREEITEM hChildItem = GetChildItem(_hItem);
		while (hChildItem != NULL)
		{
			hNextItem = GetNextItem(hChildItem, TVGN_NEXT);
			DeleteItem(hChildItem);
			hChildItem = hNextItem;
		}
	}

}


void CProcessorTreeCtrl::SelectProcessor( ProcessorID _procID )
{
	
}


bool CProcessorTreeCtrl::checkACStandFlag( const CString& procIDString )
{
	// if need to check ac stand flag
	if( m_bOnlyShowACStandGate )
	{
		// get all processor index
		ProcessorID procID;
		procID.SetStrDict( m_pInTerm->inStrDict );
		procID.setID( procIDString );
		GroupIndex group = m_pInTerm->GetProcessorList()->getGroupIndex( procID );
		for( int i= group.start; i<= group.end; i++ )
		{
			// for each processor
			// if is not gate_proc, return true( need to add the string to tree
			Processor* proc = m_pInTerm->GetProcessorList()->getProcessor( i );
			assert( proc );
			if( proc->getProcessorType() != GateProc )
				return true;
			// if the AcStandGateFlag of one processor is true.
			// return true
			GateProcessor* pGate = (GateProcessor*)proc;
			if( pGate->getACStandGateFlag() )
				return true;
		}
		// if the AcStandGateFlag of all processor all is false.
		// return false
		return false;
	}
	
	return true;
}