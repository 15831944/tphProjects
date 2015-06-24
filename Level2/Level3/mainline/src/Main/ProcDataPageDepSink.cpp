// ProcDataPageDepSink.cpp: implementation of the CProcDataPageDepSink class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "termplan.h"
#include "ProcDataPageDepSink.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
#include "ProcesserDialog.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CProcDataPageDepSink::CProcDataPageDepSink()
{
	
}

CProcDataPageDepSink::~CProcDataPageDepSink()
{

}

void CProcDataPageDepSink::AddPro1()
{
	MiscData* pMiscData = GetCurMiscData();
	if( pMiscData == NULL )
		return;

	HTREEITEM SelectedItem=m_hCurrentSelItem;
	CString strItemName=m_TreeData.GetItemText(SelectedItem);
	
	if (strcmp(strItemName,"Dependents")==0)
	{
 		m_nLastLevelNo=0;		
		HTREEITEM  ItemDependentChild=m_TreeData.GetChildItem(SelectedItem);
		HTREEITEM  ItemLastLevel = NULL;
		while(ItemDependentChild != NULL)
		{
			m_nLastLevelNo++;
			ItemLastLevel=ItemDependentChild;
			ItemDependentChild = m_TreeData.GetNextItem(ItemDependentChild,TVGN_NEXT);
		}

		if (m_TreeData.GetChildItem(ItemLastLevel)==NULL&&m_nLastLevelNo != 0)
		{
			MessageBox("Can't add new blank level!");
			return;
		}
		
		COOLTREE_NODE_INFO cni;
		CCoolTree::InitNodeInfo(this,cni);
		CString strLeveltemp;
		
		strLeveltemp.Format("Priority%d",++m_nLastLevelNo);	 //add new level	
		m_TreeData.InsertItem( strLeveltemp,cni,FALSE,FALSE, SelectedItem );
		m_TreeData.Expand(SelectedItem,TVE_EXPAND);
		return;
	}
	
	ProcessorID id;
	ProcessorIDList idList;
	ProcessorIDList idListShow;
	int nIDcount;
	if (strcmp(strItemName.Left(8),"Priority")==0)
	{
		CProcesserDialog dlg( m_pInTerm );
		int nProcType = -1;
		switch( m_nProcDataType )
		{
		case DepSourceProc:
			nProcType = DepSinkProc;
			break;
		case DepSinkProc:
			nProcType = DepSourceProc;
			break;
		case FloorChangeProc:
			nProcType = FloorChangeProc;
			break;
		}
		dlg.SetType( nProcType );
		if( dlg.DoModal() == IDCANCEL )
			return;

		MiscData* pMiscData = GetCurMiscData();
		if( pMiscData == NULL )
			return;
		
		
//		if( !dlg.GetProcessorID(id) )
//			return;
		
//		if( SelectLinkProc1(id) )	// check if exist.
//			return;

		if( !dlg.GetProcessorIDList(idList) )
		   return;		
		
		nIDcount = idList.getCount();
		for(int i = 0; i < nIDcount; i++)
		{
			if (!SelectLinkProc1(*(idList.getID(i))))  // check if exist
			{
				idListShow.Add(idList[i]);
			}
		}
		
		nIDcount = idListShow.getCount();
		for (int i = 0; i < nIDcount; i++ )
		{
			id = *(idListShow.getID(i));		
			
			
			MiscProcessorIDListWithLevel* pProcIDList = (MiscProcessorIDListWithLevel*)GetLinkProc1List( pMiscData );
			MiscProcessorIDWithLevel* pID = new MiscProcessorIDWithLevel( id );
			
			CString strSubItemNo=strItemName.Right(1);
			int nlevel=0;
			sscanf(strSubItemNo,"%d",&nlevel);		// CString to int
			pID->SetLevel(nlevel);
			
			pProcIDList->addItem( pID );
		}
//		SelectLinkProc1( id );	// select new item.		
	}
	if ( nIDcount > 0)
	{
		ReloadLinkProc1List( pMiscData );
		SelectLinkProc1( id );
		SetModified();		
	}
}

BOOL CProcDataPageDepSink::OnInitDialog()
{
	CProcDataPage::OnInitDialog();	
	return 1;
}

void CProcDataPageDepSink::ReloadLinkProc1List(MiscData *_pMiscData)
{
	if( !m_hProc1Item )
		return;
	
	DeleteAllChildItems( m_hProc1Item );
	
	// inster item
	MiscProcessorIDListWithLevel* pProcIDList = (MiscProcessorIDListWithLevel*)GetLinkProc1List( _pMiscData );
	if( !pProcIDList )
		return;
	
	char str[80];
	const MiscProcessorIDWithLevel *procID = NULL;
	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);	

	int nCurLevel=0;
	m_nLastLevelNo=0;
	int nListCount=pProcIDList->getCount();

	for (int i = 0; i < nListCount; i++)//get the m_nLastlevelNo
	{
		procID = (MiscProcessorIDWithLevel*)pProcIDList->getID (i);
		procID->printID (str);
		nCurLevel=procID->GetLevel();
 		if (strcmp(str,"All Processors")==0&&nCurLevel==1) 
		{
//			pProcIDList->deleteItem(0);
			nListCount=0;
			m_nLastLevelNo=0;
		}		
		if (nCurLevel > m_nLastLevelNo)
		{
			m_nLastLevelNo=nCurLevel;  
		}	
	}

	HTREEITEM* hlevelItem = new HTREEITEM[m_nLastLevelNo];

	for(int i = 0;i<m_nLastLevelNo;i++)
	{
		CString strLeveltemp;
		strLeveltemp.Format("Priority%d",i+1);  //Create level
		hlevelItem[i]= m_TreeData.InsertItem(strLeveltemp,cni,FALSE,FALSE, m_hProc1Item);								
	}
	
	int nIndex=0;
	for (int i = 0; i < nListCount; i++)
	{
		procID = (MiscProcessorIDWithLevel*)pProcIDList->getID (i);
		nCurLevel=procID->GetLevel();
		if (nCurLevel>=1)
		{
			procID->printID (str);
			//add item to level
			HTREEITEM hItem = m_TreeData.InsertItem( str,cni,FALSE,FALSE, hlevelItem[nCurLevel-1] );		
			m_TreeData.SetItemData( hItem, nIndex++);
		}
	}	
	
    for(int i=0;i<m_nLastLevelNo;i++)
	{
		m_TreeData.Expand(hlevelItem[i],TVE_EXPAND);
	}

	delete [] hlevelItem;

	m_TreeData.Expand( m_hProc1Item, TVE_EXPAND );	

}

void CProcDataPageDepSink::OnSelchangedTreeData(NMHDR *pNMHDR, LRESULT *pResult)
{
	CProcDataPage::OnSelchangedTreeData(pNMHDR,pResult);
	
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;	
	HTREEITEM  hItem=pNMTreeView->itemNew.hItem;
	m_hCurrentSelItem = hItem;
	
	CString strItemName=m_TreeData.GetItemText(hItem);
	strItemName=strItemName.Left(8);
    if (strcmp(strItemName,"Priority")==0) 
	{
		m_ToolBar.GetToolBarCtrl().EnableButton( ID_TOOLBARBUTTONADD, TRUE);
		m_ToolBar.GetToolBarCtrl().EnableButton( ID_TOOLBARBUTTONDEL, FALSE);		
    }
	if (strcmp(strItemName,"Denpendents")==NULL) 
	{
		m_ToolBar.GetToolBarCtrl().EnableButton( ID_TOOLBARBUTTONDEL, FALSE);
	}
	
}

void CProcDataPageDepSink::OnToolbarbuttonadd()
{
	HTREEITEM SelectedItem = m_hCurrentSelItem;
	if( SelectedItem == NULL )
		return;
    
	CString strItemName=m_TreeData.GetItemText(SelectedItem);	
	if (strcmp(strItemName.Left(8),"Priority")==0||strcmp(strItemName,"Dependents")==0) 
	{
		AddPro1();
		return;
	}
	CProcDataPage::OnToolbarbuttonadd();
}

bool CProcDataPageDepSink::SelectLinkProc1(ProcessorID _id)
{
	MiscData* pMiscData = GetCurMiscData();
	if( pMiscData == NULL )
		return false;
	
	ProcessorIDList* pProcIDList = GetLinkProc1List( pMiscData );
	if( pProcIDList == NULL )
		return false;
	
	if( !m_hProc1Item )
		return false;

	HTREEITEM hItem = m_TreeData.GetChildItem( m_hProc1Item );	
	if (hItem==NULL) 
	{
		return false;
	}
	for(int n=0;n<m_nLastLevelNo;n++)
	{
			HTREEITEM hItemChild = m_TreeData.GetChildItem( hItem );
			while (hItemChild) 
			{
				int nDBIdx = m_TreeData.GetItemData( hItemChild);
				
				const ProcessorID* pProcID = pProcIDList->getID( nDBIdx );
				if( *pProcID == _id )
				{
					m_TreeData.SelectItem( hItemChild );
					return true;
				}
				hItemChild=m_TreeData.GetNextItem(hItemChild,TVGN_NEXT);
			}			
			hItem = m_TreeData.GetNextItem( hItem, TVGN_NEXT );
			if (hItem==NULL) 
			{
				return false;
			}			
	}
	return false;
}

void CProcDataPageDepSink::OnToolbarbuttondel()
{
	//////////////////////////////////////////////////////////////////////////
	HTREEITEM SelectedItem = m_hCurrentSelItem;
	if( SelectedItem == NULL )
		return;
    HTREEITEM ParentSelectedItem=m_TreeData.GetParentItem(SelectedItem);

	CString strItemName	= m_TreeData.GetItemText(ParentSelectedItem);
	if (strcmp(strItemName.Left(8),"Priority")!=0) 
	{
		CProcDataPage::OnToolbarbuttondel();	
		return;
	}
	//////////////////////////////////////////////////////////////////////////
	
	
	HTREEITEM hItem = m_hCurrentSelItem;
	if( hItem == NULL )
		return;

	MiscData* pMiscData = GetCurMiscData();
	if( pMiscData == NULL )
		return;
	
	int nDBIdx = m_TreeData.GetItemData( hItem );
	
	MiscProcessorIDListWithLevel* pProcIDList = (MiscProcessorIDListWithLevel*)GetLinkProc1List( pMiscData );
	if( pProcIDList == NULL )
		return;	

	int nDelelevel,nCompareLevel;
	 MiscProcessorIDWithLevel *GetprocID = NULL;	
	 GetprocID = (MiscProcessorIDWithLevel*)pProcIDList->getItem(nDBIdx);    

	 pProcIDList->removeItem( nDBIdx );
	 
//////////////////////////////////////////////////////////////////////////
	nDelelevel=GetprocID->GetLevel();
	bool bNoOthers=TRUE;
	for (int i = 0; i < pProcIDList->getCount(); i++)
	{
		GetprocID = (MiscProcessorIDWithLevel*)pProcIDList->getItem(i);
		nCompareLevel=GetprocID->GetLevel();
		if (nCompareLevel<0)nCompareLevel=0;		
		if (nCompareLevel==nDelelevel)
		{
			bNoOthers=FALSE;
			break;
		}
	}
//////////////////////////////////////////////////////////////////////////
	if (bNoOthers) 
	{
		for (int i = 0; i < pProcIDList->getCount(); i++)
		{
			GetprocID = (MiscProcessorIDWithLevel*)pProcIDList->getItem(i);
			nCompareLevel=GetprocID->GetLevel();
			if (nCompareLevel<0)nCompareLevel=1;			
			if (nCompareLevel>nDelelevel)
			{
				GetprocID->SetLevel(nCompareLevel-1);     
				pProcIDList->setID(i,*(ProcessorID*)GetprocID);
			}
		}		
	}	
	ReloadLinkProc1List( pMiscData );
	m_TreeData.SelectItem( ParentSelectedItem ) ;
	SetModified();		
	m_ToolBar.GetToolBarCtrl().EnableButton( ID_TOOLBARBUTTONDEL, FALSE);	
}



BOOL CProcDataPageDepSink::OnToolTipText(UINT, NMHDR *pNMHDR, LRESULT *pResult)
{
	ASSERT(pNMHDR->code == TTN_NEEDTEXTA || pNMHDR->code == TTN_NEEDTEXTW);
	
	// UNICODE
	TOOLTIPTEXTA* pTTTA = (TOOLTIPTEXTA*)pNMHDR;
	TOOLTIPTEXTW* pTTTW = (TOOLTIPTEXTW*)pNMHDR;
	//TCHAR szFullText[512];
	CString strTipText;
	UINT nID = pNMHDR->idFrom;
	
	if (pNMHDR->code == TTN_NEEDTEXTA && (pTTTA->uFlags & TTF_IDISHWND) ||
		pNMHDR->code == TTN_NEEDTEXTW && (pTTTW->uFlags & TTF_IDISHWND))
	{
		// idFromHWND 
		nID = ::GetDlgCtrlID((HWND)nID);
	}
	
	if (nID != 0)
	{
		HTREEITEM hItem = m_hCurrentSelItem;
		CString strItemName=m_TreeData.GetItemText(hItem);		
		
		strTipText.LoadString(nID);
		strTipText = strTipText.Mid(strTipText.Find('\n',0)+1);

		if (nID==ID_TOOLBARBUTTONADD) 
		{
			if (strcmp(strItemName,"Dependents")==0)
			{
				strTipText="Add level on dependents";
			}
			else
			{
				strItemName=strItemName.Left(8);
				if (strcmp(strItemName,"Priority")==0)
				{
					strTipText="Add item on Level";
				}				
			}
		}
		
#ifndef _UNICODE
		if (pNMHDR->code == TTN_NEEDTEXTA)
		{
			lstrcpyn(pTTTA->szText, strTipText, sizeof(pTTTA->szText));
		}
		else
		{
			_mbstowcsz(pTTTW->szText, strTipText, sizeof(pTTTW->szText));
		}
#else
		if (pNMHDR->code == TTN_NEEDTEXTA)
		{
			_wcstombsz(pTTTA->szText, strTipText,sizeof(pTTTA->szText));
		}
		else
		{
			lstrcpyn(pTTTW->szText, strTipText, sizeof(pTTTW->szText));
		}
#endif
		
		*pResult = 0;
		
		::SetWindowPos(pNMHDR->hwndFrom, HWND_TOP, 0, 0, 0, 0,SWP_NOACTIVATE|
			SWP_NOSIZE|SWP_NOMOVE|SWP_NOOWNERZORDER); 
		return TRUE;
	}
	return TRUE;
}

