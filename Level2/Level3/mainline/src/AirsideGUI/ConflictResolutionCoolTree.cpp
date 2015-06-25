// CoolTree.cpp : implementation file
//

#include "stdafx.h"
#include "ConflictResolutionCoolTree.h"
#include "..\MFCExControl\CoolTreeEditSpin.h"
#include "Resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#define IDD_COOLTREE_EDITSPIN           6005
/////////////////////////////////////////////////////////////////////////////
// CConflictResolutionCoolTree
CConflictResolutionCoolTree::CConflictResolutionCoolTree()
{
	m_pWndEdit=NULL;
	m_pWndEditSpin=NULL;
	m_pWndComboBox=NULL;
	m_pWndComboBoxWithEdit=NULL;
	m_hEditedItem=NULL;

	m_EnableMutiTree = FALSE;
	m_nWidth = 100;
}

CConflictResolutionCoolTree::~CConflictResolutionCoolTree()
{
	size_t nSize=m_vNOdeInfo.size();
	for (size_t i = 0; i < nSize; i++)
	{
		COOLTREE_NODE_INFO* pCNI=m_vNOdeInfo[i];

		if(pCNI->pEditWnd&&::IsWindow(pCNI->pEditWnd->m_hWnd))
			delete pCNI->pEditWnd;

		delete pCNI;
	}

	m_vNOdeInfo.clear();

	delete m_pWndEdit;
	delete m_pWndEditSpin;
	delete m_pWndComboBox;
}


BEGIN_MESSAGE_MAP(CConflictResolutionCoolTree, CTreeCtrl)
	//{{AFX_MSG_MAP(CConflictResolutionCoolTree)
	ON_NOTIFY_REFLECT(NM_CLICK, OnClick)
	ON_NOTIFY_REFLECT(NM_DBLCLK, OnDblclk)
	ON_WM_LBUTTONDBLCLK()
	ON_EN_KILLFOCUS(ID_COOLTREE_EDIT, OnKillfocusEdit1)
	ON_WM_PAINT()
	ON_NOTIFY_REFLECT(NM_RCLICK, OnRclick)
	ON_CBN_SELCHANGE(ID_COOLTREE_COMBO, OnSelchangeCombo)
	
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CConflictResolutionCoolTree message handlers

HTREEITEM CConflictResolutionCoolTree::InsertItem(LPCTSTR lpszItem, COOLTREE_NODE_INFO cni,BOOL bCheckOnOrRadioOn, 
	BOOL bHoldEditWnd,HTREEITEM hParent, HTREEITEM hInsertAfter)
{
	COOLTREE_NODE_INFO* pNewCNI=new COOLTREE_NODE_INFO;
	*pNewCNI=cni;
	m_vNOdeInfo.push_back(pNewCNI);
	HTREEITEM hItem=NULL;
	switch(pNewCNI->nt)
	{
	case NT_NULL:
		hItem=CTreeCtrl::InsertItem(lpszItem,TIIT_NULL,TIIT_NULL,hParent,hInsertAfter);
		break;
	case NT_NORMAL:
	case NT_FREE:
		hItem=CTreeCtrl::InsertItem(lpszItem,cni.nImage,cni.nImageSeled,hParent,hInsertAfter);
		break;
	case NT_CHECKBOX:
		{
			if(bCheckOnOrRadioOn)
				hItem=CTreeCtrl::InsertItem(lpszItem,TIIT_CHECKBOX_TRUE,TIIT_CHECKBOX_TRUE,hParent,hInsertAfter);
			else
				hItem=CTreeCtrl::InsertItem(lpszItem,TIIT_CHECKBOX_FALSE,TIIT_CHECKBOX_FALSE,hParent,hInsertAfter);
		}
		break;
	case NT_RADIOBTN:
		{
			if(bCheckOnOrRadioOn)
				hItem=CTreeCtrl::InsertItem(lpszItem,TIIT_RADIOBTN_TRUE,TIIT_RADIOBTN_TRUE,hParent,hInsertAfter);
			else
				hItem=CTreeCtrl::InsertItem(lpszItem,TIIT_RADIOBTN_FALSE,TIIT_RADIOBTN_FALSE,hParent,hInsertAfter);
			AutoAdjustRadioStatus(hItem);
		}
		break;
	}
	//Create a new Wnd for edit
	if(bHoldEditWnd)
	{
		pNewCNI->pEditWnd=CreateEditWnd(pNewCNI->net);
	}
	else
	{
		pNewCNI->pEditWnd=NULL;
	}
	CTreeCtrl::SetItemData(hItem,(DWORD_PTR)pNewCNI);
	AutoAdjustItemEnableStatus(hItem);
	return hItem;
}

CWnd* CConflictResolutionCoolTree::CreateEditWnd(NODE_EDIT_TYPE net)
{
	CWnd* pNewWnd=NULL;
	switch(net)
	{
	case NET_NORMAL:
		break;
	case NET_EDIT:
	case NET_EDIT_FLOAT:
	case NET_EDIT_INT:
	case NET_EDIT_WITH_VALUE:
		{
/*
			CEdit *pCB = new CEdit;
			DWORD dwComboBoxStyle = WS_CHILD|WS_TABSTOP|ES_AUTOHSCROLL ;
			if(net==NET_EDIT_INT )
				dwComboBoxStyle |= ES_NUMBER;
			if( pCB->CreateEx(WS_EX_CLIENTEDGE,"Edit",NULL, dwComboBoxStyle, CRect(), this, ID_COOLTREE_EDIT ) )
				pNewWnd=pCB;		*/
			CCoolTreeEditSpin* pCTES=new CCoolTreeEditSpin(this);
			pCTES->Create(IDD_COOLTREE_EDITSPIN,this);
			pCTES->m_editValue.ModifyStyle(ES_NUMBER,0);
			//pCTES->m_spinValue.ShowWindow(SW_HIDE);
			pCTES->SetParent(this);
			pNewWnd=pCTES;
		}
		break;
	case NET_EDITSPIN_WITH_VALUE:
	case NET_EDITSPIN:
		{
			CCoolTreeEditSpin* pCTES=new CCoolTreeEditSpin(this);
			pCTES->Create(IDD_COOLTREE_EDITSPIN,this);
			pCTES->SetParent(this);
			pNewWnd=pCTES;
		}
		break;
	case NET_COMBOBOX:
	case NET_COMBOBOX_WITH_EDIT:
		{
			CTreeComboBox *pCB = new CTreeComboBox;
			DWORD dwComboBoxStyle = WS_CHILD | WS_VSCROLL;
			if(net==NET_COMBOBOX_WITH_EDIT)
				dwComboBoxStyle |= CBS_DROPDOWN;
			else
				dwComboBoxStyle |= CBS_DROPDOWNLIST;
			if( pCB->Create( dwComboBoxStyle, CRect(), this, ID_COOLTREE_COMBO ) )
				pNewWnd=pCB;
				
		}
		break;
	case NET_SHOW_DIALOGBOX:
		break;
	}
	pNewWnd->SetFont(GetFont());
	return pNewWnd;
}

COOLTREE_NODE_INFO* CConflictResolutionCoolTree::GetItemNodeInfo(HTREEITEM hItem)
{
	COOLTREE_NODE_INFO* pCNI=NULL;
	pCNI=(COOLTREE_NODE_INFO*)CTreeCtrl::GetItemData(hItem);
	return pCNI;
}

int CConflictResolutionCoolTree::IsCheckItem(HTREEITEM hItem)
{
	int nImage,nReturn;
	if(nImage=GetItemImage(hItem))
	{
		switch(nImage)
		{
		case TIIT_CHECKBOX_TRUE:
		case TIIT_CHECKBOX_TRUE_DISABLE:
		case TIIT_CHECKBOX_2_TRUE:
		case TIIT_CHECKBOX_2_TRUE_DISABLE:
		case TIIT_RADIOBTN_TRUE:
		case TIIT_RADIOBTN_TRUE_DISABLE:
			nReturn=1;
			break;
		case TIIT_CHECKBOX_FALSE:
		case TIIT_CHECKBOX_FALSE_DISABLE:
		case TIIT_RADIOBTN_FALSE:
		case TIIT_RADIOBTN_FALSE_DISABLE:
			nReturn=0;
			break;
		default:
			nReturn=-1;
		}

	}
	return nReturn;
}

BOOL CConflictResolutionCoolTree::IsEnableItem(HTREEITEM hItem)
{
	int nImage;
	if(nImage=GetItemImage(hItem))
	{
		if(nImage%2==0)
			return TRUE;

	}
	return FALSE;
}

void CConflictResolutionCoolTree::PreSubclassWindow() 
{
	if (m_imageList.m_hImageList == NULL)
	{
		m_imageList.Create(16,16,ILC_COLOR8|ILC_MASK,0,1);
		CBitmap bmp;
		bmp.LoadBitmap(IDB_COOLTREE);
		m_imageList.Add(&bmp,RGB(255,0,255));
	}
	SetImageList(&m_imageList,TVSIL_NORMAL);
	
	CTreeCtrl::PreSubclassWindow();
}

void CConflictResolutionCoolTree::SetCheckStatus(HTREEITEM hItem, BOOL bCheck,BOOL bRecursive)
{
	if(IsCheckItem(hItem)==bCheck?1:0) return;
	if(bCheck)
	{

		if(IsEnableItem(hItem))
			SetItemImage(hItem,TIIT_CHECKBOX_TRUE,TIIT_CHECKBOX_TRUE);
		else
			SetItemImage(hItem,TIIT_CHECKBOX_TRUE_DISABLE,TIIT_CHECKBOX_TRUE_DISABLE);
		HTREEITEM hItemChild=GetChildItem(hItem);
		while(hItemChild)
		{
			if (m_EnableMutiTree == FALSE)
				SetEnableStatus(hItemChild,bCheck);
			else
			{
				if(IsCheckBox(hItemChild))
				SetCheckStatus(hItemChild,bCheck,TRUE);
			}
			hItemChild=GetNextSiblingItem(hItemChild);
		}
		//Set Parent Check staus
		if (m_EnableMutiTree == TRUE)
		{
			HTREEITEM hParent=GetParentItem(hItem);
			while (hParent)   
			{
				SetItemImage(hParent,TIIT_CHECKBOX_TRUE,TIIT_CHECKBOX_TRUE);
				hParent = GetParentItem(hParent);
			}
		}

		AutoAdjustCheck2Status(hItem);
	}
	else
	{
		if(IsEnableItem(hItem))
			SetItemImage(hItem,TIIT_CHECKBOX_FALSE,TIIT_CHECKBOX_FALSE);
		else
			SetItemImage(hItem,TIIT_CHECKBOX_FALSE_DISABLE,TIIT_CHECKBOX_FALSE_DISABLE);
		HTREEITEM hItemChild=GetChildItem(hItem);
		while(hItemChild)
		{
			if (m_EnableMutiTree == FALSE)
				SetEnableStatus(hItemChild,bCheck);
			else
			{
				if(IsCheckBox(hItemChild))
				SetCheckStatus(hItemChild,bCheck,TRUE);
			}
			hItemChild=GetNextSiblingItem(hItemChild);
		}
	}
	// if(bRecursive) return;
	HTREEITEM hParent=GetParentItem(hItem);
	if(hParent)
	{
		if(IsCheckBox(hParent)&&IsCheckItem(hParent)==1)
		{
			HTREEITEM hChild=GetChildItem(hParent);
			int nCheckStatue=-2;
			while(hChild)
			{
				if(IsCheckBox(hChild)&&nCheckStatue!=-2&&
					IsCheckItem(hChild)!=nCheckStatue)
				{
					
					SetChect2Status(hParent);
					return;
				}
				nCheckStatue=IsCheckItem(hChild);
				hChild=GetNextSiblingItem(hChild);
			}
			SetItemImage(hParent,TIIT_CHECKBOX_TRUE,TIIT_CHECKBOX_TRUE);
//			else if(nCheckStatue==0)
//				SetItemImage(hParent,TIIT_CHECKBOX_FALSE,TIIT_CHECKBOX_FALSE);
			
		}
	}
}


void CConflictResolutionCoolTree::SetEnableStatus(HTREEITEM hItem, BOOL bEnable)
{
	if (m_EnableMutiTree == TRUE)
		return;

	if(IsEnableItem(hItem)==bEnable) return;
	TREENODE_IMAGE_INDEX_TABLE tiit=GetItemImage(hItem);
	if(bEnable)//Enable
	{
		SetItemImage(hItem,tiit-1,tiit-1);
		COOLTREE_NODE_INFO* pCNI=GetItemNodeInfo(hItem);
		if(pCNI)
			pCNI->clrItem=RGB(0,0,0);
		if(IsCheckItem(hItem)==1||IsCheckItem(hItem)==-1)
		{
			//Make Child Item Disable
			HTREEITEM hItemChild=GetChildItem(hItem);
			while(hItemChild)
			{
				SetEnableStatus(hItemChild,bEnable);
				hItemChild=GetNextSiblingItem(hItemChild);
			}
		}
	}
	else//Disable
	{
		SetItemImage(hItem,tiit+1,tiit+1);
		COOLTREE_NODE_INFO* pCNI=GetItemNodeInfo(hItem);
		if(pCNI)
			pCNI->clrItem=RGB(192,192,192);
		//Make Child Item Disable
		HTREEITEM hItemChild=GetChildItem(hItem);
		while(hItemChild)
		{
			SetEnableStatus(hItemChild,bEnable);
			hItemChild=GetNextSiblingItem(hItemChild);
		}
		
	}
	
}

TREENODE_IMAGE_INDEX_TABLE CConflictResolutionCoolTree::GetItemImage(HTREEITEM hItem)
{
	int nImage,nSelectedImage;
	CTreeCtrl::GetItemImage(hItem,nImage,nSelectedImage);
	return TREENODE_IMAGE_INDEX_TABLE(nImage);
}

void CConflictResolutionCoolTree::OnClick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	if(pNMHDR)
		NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	CPoint pt;
	CRect rt;
	GetCursorPos(&pt);
	ScreenToClient(&pt);
	UINT flag;
	HTREEITEM hItem = HitTest(pt, &flag);
	m_hEditedItem=hItem;
	CTreeCtrl::SelectItem(m_hEditedItem); // add by adam 2007-05-28
	if ((pNMHDR==NULL&&pResult==NULL)||(flag & TVHT_ONITEMICON)== TVHT_ONITEMICON)
	{
		if(!IsEnableItem(hItem))
		{
			if(pResult)
				*pResult=0;
			return;
		}
		COOLTREE_NODE_INFO* pCNI=GetItemNodeInfo(hItem);
		if(pCNI)
		{
			switch(pCNI->nt)
			{
			case NT_NORMAL:
				break;
			case NT_RADIOBTN:
				{
					DoRadio(hItem);
					GetParent()->SendMessage(UM_CEW_STATUS_CHANGE,(WPARAM)hItem);
				}
				break;
			case NT_CHECKBOX:
				{
					DoCheck(hItem);
					GetParent()->SendMessage(UM_CEW_STATUS_CHANGE,(WPARAM)hItem);
				}
				break;
			}
		}
		if(pResult)
			*pResult=0;
		return;
	}
	if(pResult)
		*pResult = 0;
}

void CConflictResolutionCoolTree::DoCheck(HTREEITEM hItem)
{
	SetCheckStatus(hItem,!IsCheckItem(hItem));
}

void CConflictResolutionCoolTree::DoRadio(HTREEITEM hItem)
{
	SetRadioStatus(hItem,TRUE);
}

void CConflictResolutionCoolTree::SetRadioStatus(HTREEITEM hItem,BOOL bRadio)
{
	BOOL bEnable=IsEnableItem(hItem);
	if(bRadio)
	{
		if(bEnable)
			SetItemImage(hItem,TIIT_RADIOBTN_TRUE,TIIT_RADIOBTN_TRUE);
		else
			SetItemImage(hItem,TIIT_RADIOBTN_TRUE_DISABLE,TIIT_RADIOBTN_TRUE_DISABLE);
		HTREEITEM hParentItem=GetParentItem(hItem);
		
		HTREEITEM hChildItem=GetChildItem(hParentItem);
		while(hChildItem)
		{
			if(hChildItem==hItem)
			{
				hChildItem=GetNextSiblingItem(hChildItem);
				continue;
			}
			COOLTREE_NODE_INFO* pCNI=GetItemNodeInfo(hChildItem);
			if(pCNI&&pCNI->nt==NT_RADIOBTN)
			{
				SetRadioStatus(hChildItem,FALSE);
			}
			hChildItem=GetNextSiblingItem(hChildItem);
		}
	}
	else
	{
		if(bEnable)
			SetItemImage(hItem,TIIT_RADIOBTN_FALSE,TIIT_RADIOBTN_FALSE);
		else
			SetItemImage(hItem,TIIT_RADIOBTN_FALSE_DISABLE,TIIT_RADIOBTN_FALSE_DISABLE);
	}
	//Make Child Item Enable
	HTREEITEM hItemChild=GetChildItem(hItem);
	while(hItemChild)
	{
		SetEnableStatus(hItemChild,bRadio);
		hItemChild=GetNextSiblingItem(hItemChild);
	}		
				
}

BOOL CConflictResolutionCoolTree::IsRadioItem(HTREEITEM hItem)
{
	return IsCheckItem(hItem)==1;
}

void CConflictResolutionCoolTree::AutoAdjustItemEnableStatus(HTREEITEM hItem)
{
	if (m_EnableMutiTree == TRUE)
		return;

	HTREEITEM hParentItem=GetParentItem(hItem);
	if(!hParentItem) return;
	if(!IsEnableItem(hParentItem))
	{
		SetEnableStatus(hItem,FALSE);
	}
	else
	{
		COOLTREE_NODE_INFO* pCNI=GetItemNodeInfo(hParentItem);
		if(IsCheckItem(hParentItem)==0&&pCNI&&(pCNI->nt==NT_CHECKBOX||
			pCNI->nt==NT_RADIOBTN))
			SetEnableStatus(hItem,FALSE);
	}
}

void CConflictResolutionCoolTree::AutoAdjustRadioStatus(HTREEITEM hItem)
{
	if(IsCheckItem(hItem)==1)
	{
		SetRadioStatus(hItem);
	}
}

void CConflictResolutionCoolTree::OnDblclk(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	
	*pResult = 0;
}

void CConflictResolutionCoolTree::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	if( GetKeyState(VK_CONTROL) & 0x8000 )
	{
		CTreeCtrl::OnLButtonDblClk(nFlags, point);
		return;
	}
	
	// Determine where DblClick appears
	UINT flags;
	HTREEITEM hItem=HitTest(point, &flags);
	m_hEditedItem=hItem;
	CTreeCtrl::SelectItem(m_hEditedItem); // add by adam 2007-05-28
	if( hItem &&
		(flags & TVHT_ONITEMLABEL)&&IsEnableItem(hItem))
	{
		DoEdit(hItem);
		return;
	}
	CTreeCtrl::OnLButtonDblClk(nFlags, point);	
}

void CConflictResolutionCoolTree::ShowEditSpinWnd(HTREEITEM hItem,COOLTREE_NODE_INFO* pCNI)
{
	CWnd* pWnd=NULL;
	if(pCNI->pEditWnd)
	{
		pWnd=pCNI->pEditWnd;
	}
	else
	{
 		if(m_pWndEditSpin==NULL||m_pWndEditSpin->m_hWnd==NULL)
		{
			m_pWndEditSpin=CreateEditWnd(pCNI->net);
			if(m_pWndEditSpin==NULL)
				return;
		}
		pWnd=m_pWndEditSpin;
	}
	if(pCNI->net==NET_EDITSPIN_WITH_VALUE||pCNI->net==NET_EDIT_WITH_VALUE)
	{
		CRect rect;
		if(GetItemRect(hItem,rect,TRUE))
		{
			rect.left = rect.right;
			pWnd->SetWindowPos(NULL,rect.left,rect.top-1,0,0,SWP_NOSIZE);
			
			CString strItemData;
			if(pCNI->net==NET_EDIT_WITH_VALUE)
			{
				float fItemData=static_cast<float>(GetItemData(hItem)/100.0);
				strItemData.Format("%.2f",fItemData);	
			}
			else
			{
				long lItemData;
				lItemData= GetItemData( hItem );
				strItemData.Format("%d",lItemData);
			}
			pWnd->SetWindowPos(NULL,rect.left,rect.top,0,0,SWP_NOSIZE);
			((CCoolTreeEditSpin*)pWnd)->m_editValue.SetWindowText( strItemData );
			((CCoolTreeEditSpin*)pWnd)->m_editValue.SetSel(0,-1);
			GetParent()->SendMessage(pCNI->net==NET_EDIT_WITH_VALUE?UM_CEW_EDIT_BEGIN:UM_CEW_EDITSPIN_BEGIN,(WPARAM)hItem,(LPARAM)pCNI);
			((CCoolTreeEditSpin*)pWnd)->m_editValue.SetLimitText(pCNI->nMaxCharNum);
			((CCoolTreeEditSpin*)pWnd)->m_spinValue.SetRange32(int(pCNI->fMinValue),int(pCNI->fMaxValue));
			pWnd->ShowWindow(SW_SHOWNORMAL);
		}
		return;
	}

	CRect rect;
	if(GetItemRect(hItem,rect,TRUE))
	{
		pWnd->SetWindowPos(NULL,rect.left,rect.top,0,0,SWP_NOSIZE);
		((CCoolTreeEditSpin*)pWnd)->m_editValue.SetWindowText(GetItemText(hItem));
		((CCoolTreeEditSpin*)pWnd)->m_editValue.SetSel(0,-1);
		GetParent()->SendMessage(UM_CEW_EDIT_BEGIN,(WPARAM)hItem,(LPARAM)pCNI);
		((CCoolTreeEditSpin*)pWnd)->m_editValue.SetLimitText(pCNI->nMaxCharNum);
		((CCoolTreeEditSpin*)pWnd)->m_spinValue.SetRange32(int(pCNI->fMinValue),int(pCNI->fMaxValue));
		pWnd->ShowWindow(SW_SHOWNORMAL);
	}
}

void CConflictResolutionCoolTree::ShowEditWnd(HTREEITEM hItem,COOLTREE_NODE_INFO* pCNI,BOOL bCreateOnly)
{
	if(bCreateOnly)
	{
		if(m_pWndEdit==NULL||m_pWndEdit->m_hWnd==NULL)
		{
			m_pWndEdit=CreateEditWnd(pCNI->net);
		}
		return;
	}
	CWnd* pWnd=NULL;
	if(pCNI->pEditWnd)
	{
		pWnd=pCNI->pEditWnd;
	}
	else
	{
		if(m_pWndEdit==NULL||m_pWndEdit->m_hWnd==NULL)
		{
			m_pWndEdit=CreateEditWnd(pCNI->net);
			if(m_pWndEdit==NULL)
				return;
		}
		pWnd=m_pWndEdit;
	}
/*
	if(pCNI->net==NET_EDIT_WITH_VALUE)
	{
		CRect rect;
		if(GetItemRect(hItem,rect,TRUE))
		{
			rect.left = rect.right;
			
			long lItemData= GetItemData( hItem );
			CString strItemData;
			strItemData.Format("%d",lItemData);
			
			pWnd->SetWindowPos(NULL,rect.left,rect.top,0,0,SWP_NOSIZE);
			pWnd->SetWindowText( strItemData );
			((CEdit*)pWnd)->SetSel(0,-1);
			//pWnd->SetFocus();
			GetParent()->SendMessage(UM_CEW_EDIT_BEGIN,(WPARAM)hItem,(LPARAM)pCNI);
			((CEdit*)pWnd)->SetLimitText(pCNI->nMaxCharNum);
			pWnd->ShowWindow(SW_SHOWNORMAL);
		}
		return;
	}*/

	CRect rect;
	if(GetItemRect(hItem,rect,TRUE))
	{
		if(pCNI->net==NET_EDIT_WITH_VALUE)
		{
			rect.left=rect.right;
			long lItemData= GetItemData( hItem );
			CString strItemData;
			strItemData.Format("%d",lItemData);
			pWnd->SetWindowText( strItemData );
		
		}
		else
		{
			pWnd->SetWindowText(GetItemText(hItem));
		}
		if(rect.Width()<100)
			rect.right=rect.left+100;
		pWnd->MoveWindow(rect);
		((CEdit*)pWnd)->SetSel(0,-1);
		pWnd->SetFocus();
		GetParent()->SendMessage(UM_CEW_EDIT_BEGIN,(WPARAM)hItem,(LPARAM)pCNI);
		((CEdit*)pWnd)->SetLimitText(pCNI->nMaxCharNum);
		pWnd->ShowWindow(SW_SHOW);
	}
}

LRESULT CConflictResolutionCoolTree::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	switch(message)
	{
	case UM_CEW_EDIT_END:
	case UM_CEW_EDITSPIN_END:
	case UM_CEW_COMBOBOX_END:
	case UM_CEW_COMBOBOX_SELCHANGE:
		{
			CString strValue=*((CString*)lParam);
			COOLTREE_NODE_INFO* pCNI=(COOLTREE_NODE_INFO*)GetItemNodeInfo(m_hEditedItem);
			if(!pCNI) return TRUE;
			switch(pCNI->net)
			{
			case NET_EDIT_FLOAT:
			case NET_EDIT_INT:
			case NET_EDITSPIN:
			case NET_EDITSPIN_WITH_VALUE:
				{
					int nVal=atoi(strValue);
					if(pCNI->bVerify&&(nVal<pCNI->fMinValue||nVal>pCNI->fMaxValue))
					{
						CString strMsg;
						strMsg.Format("Please enter a number between %d and %d.",int(pCNI->fMinValue),int(pCNI->fMaxValue));
						AfxMessageBox(strMsg);
						return TRUE;
					}
				}
				break;
			}
			if(pCNI->bAutoSetItemText)
				SetItemText(m_hEditedItem,strValue);
			GetParent()->SendMessage(message,(WPARAM)m_hEditedItem,lParam);
			return TRUE;
		}
		break;
	}
	return CTreeCtrl::DefWindowProc(message, wParam, lParam);
}

void CConflictResolutionCoolTree::ShowComboBoxWnd(HTREEITEM hItem, COOLTREE_NODE_INFO *pCNI,BOOL bEditable)
{
	CWnd* pWnd=NULL;
	if(!bEditable)
	{
		if(pCNI->pEditWnd)
		{
			pWnd=pCNI->pEditWnd;
		}
		else
		{
			if(m_pWndComboBox==NULL||m_pWndComboBox->m_hWnd==NULL)
			{
				m_pWndComboBox=CreateEditWnd(pCNI->net);
				if(m_pWndComboBox==NULL)
					return;
			}
			pWnd=m_pWndComboBox;	
		}

		
	}
	else
	{
		if(pCNI->pEditWnd)
		{
			pWnd=pCNI->pEditWnd;
		}
		else
		{
			if(m_pWndComboBoxWithEdit==NULL||m_pWndComboBoxWithEdit->m_hWnd==NULL)
			{
				m_pWndComboBoxWithEdit=CreateEditWnd(pCNI->net);
				if(m_pWndComboBoxWithEdit==NULL)
					return;
			}
			pWnd=m_pWndComboBoxWithEdit;
		}

	}

	CRect rect;
	if(GetItemRect(hItem,rect,TRUE))
	{
	//	if(rect.Width()<100)
		GetParent()->SendMessage(UM_CEW_COMBOBOX_SETWIDTH,(WPARAM)hItem,(LPARAM)pCNI);
			rect.right=rect.left+m_nWidth;
		rect.top-=1;
		rect.bottom+=240;
		pWnd->MoveWindow(rect);
		pWnd->SetFocus();
		GetParent()->SendMessage(UM_CEW_COMBOBOX_BEGIN,(WPARAM)hItem,(LPARAM)pCNI);
		if(((CComboBox*)pWnd)->GetCount()==0)
			return;
		((CTreeComboBox*)pWnd)->LimitText(pCNI->nMaxCharNum);
		pWnd->ShowWindow(SW_SHOW);
		((CTreeComboBox*)pWnd)->ShowDropDown( TRUE );
		
	}		

}

// Implementation of CConflictResolutionCoolTree::CTreeComboBox class
BEGIN_MESSAGE_MAP(CConflictResolutionCoolTree::CTreeComboBox, CComboBox)
//{{AFX_MSG_MAP(CTreeComboBox)
ON_CONTROL_REFLECT(CBN_KILLFOCUS, CTreeComboBox::OnKillFocus)
//ON_CONTROL_REFLECT(CBN_SELCHANGE, CTreeComboBox::OnSelChange)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()
void CConflictResolutionCoolTree::CTreeComboBox::OnKillFocus() 
{
	ShowWindow( SW_HIDE );
	m_nIndexSeled=GetCurSel();
	if(m_nIndexSeled == -1)
		return;
	CString strValue;
	GetWindowText(strValue);
	GetParent()->SendMessage(UM_CEW_COMBOBOX_END,(WPARAM)0,(LPARAM)&strValue);
	
}

void CConflictResolutionCoolTree::CTreeComboBox::OnSelChange(NMHDR* pNMHDR, LRESULT* pResult)
{
	ShowWindow( SW_HIDE );
	m_nIndexSeled=GetCurSel();
	CString strValue;
	GetWindowText(strValue);
	GetParent()->SendMessage(UM_CEW_COMBOBOX_SELCHANGE,(WPARAM)0,(LPARAM)&strValue);
	
}

DWORD_PTR CConflictResolutionCoolTree::CTreeComboBox::GetItemData(int nIndex )
{
	return CComboBox::GetItemData(nIndex);
}

void CConflictResolutionCoolTree::OnKillfocusEdit1() 
{
	// TODO: Add your control notification handler code here
	m_pWndEdit->ShowWindow(SW_HIDE);
	CString strValue;
	m_pWndEdit->GetWindowText(strValue);
	SendMessage(UM_CEW_EDIT_END,0,(LPARAM)&strValue);
}

CWnd* CConflictResolutionCoolTree::GetEditWnd(HTREEITEM hItem)
{
	CWnd* pWnd=NULL;
	COOLTREE_NODE_INFO* pCNI=GetItemNodeInfo(hItem);
	if(!pCNI)return NULL;
	if(pCNI->pEditWnd==NULL)
		pWnd=GetEditWnd(pCNI->net);
	else
		pWnd=pCNI->pEditWnd;
	return pWnd;
}

CWnd* CConflictResolutionCoolTree::GetEditWnd(NODE_EDIT_TYPE net)
{
	CWnd* pWnd=NULL;
	switch(net)
	{
	case NET_EDIT:
	case NET_EDIT_INT:
	case NET_EDIT_FLOAT:
	case NET_EDIT_WITH_VALUE:
/*
		ShowEditWnd(NULL,NULL,TRUE);
		pWnd=m_pWndEdit;
*/
		pWnd=&(((CCoolTreeEditSpin*)m_pWndEditSpin)->m_editValue);
		
		break;
	case NET_EDITSPIN:
	case NET_EDITSPIN_WITH_VALUE:
		pWnd=&(((CCoolTreeEditSpin*)m_pWndEditSpin)->m_editValue);
		break;
	case NET_COMBOBOX:
		pWnd=m_pWndComboBox;
		break;
	case NET_COMBOBOX_WITH_EDIT:
		pWnd=m_pWndComboBoxWithEdit;
		break;
	}
	return pWnd;
}

void CConflictResolutionCoolTree::SetItemNodeInfo(HTREEITEM hItem, COOLTREE_NODE_INFO cni)
{
	COOLTREE_NODE_INFO* pCNI=GetItemNodeInfo(hItem);
	if(pCNI)
		*pCNI=cni;
}

DWORD CConflictResolutionCoolTree::GetItemData(HTREEITEM hItem)
{
	COOLTREE_NODE_INFO* pCNI=GetItemNodeInfo(hItem);
	if(pCNI)
		return pCNI->dwItemData;
	return NULL;
}

BOOL CConflictResolutionCoolTree::SetItemData(HTREEITEM hItem, DWORD dwData)
{
	COOLTREE_NODE_INFO* pCNI=GetItemNodeInfo(hItem);
	if(pCNI)
	{
		pCNI->dwItemData=dwData;
		return TRUE;
	}
	return FALSE;
}

void CConflictResolutionCoolTree::SelectFromText(CComboBox *pCB, CString strValue)
{
	int nCount=pCB->GetCount();
	CString str;
	for(int i=0;i<nCount;i++)
	{
		pCB->GetLBText(i,str);
		if(str==strValue)
		{
			pCB->SetCurSel(i);
			return;
		}
	}
}

void CConflictResolutionCoolTree::InitNodeInfo(CWnd* pParent,COOLTREE_NODE_INFO& CNI,BOOL bVerify)
{
	CNI.bEnable=TRUE;
	CNI.dwItemData=NULL;
	CNI.fMaxValue=100;
	CNI.fMinValue=0;
	CNI.net=NET_NORMAL;
	CNI.nImage=TIIT_NORMAL;
	CNI.nImageDisable=TIIT_NORMAL_DISABLE;
	CNI.nImageSeled=CNI.nImage;
	CNI.nMaxCharNum=256;
	CNI.nt=NT_NORMAL;
	CNI.pEditWnd=NULL;
	CNI.bVerify=FALSE;
	CNI.clrItem=RGB(0,0,0);
	CNI.bAutoSetItemText=TRUE;
	pParent->GetFont()->GetLogFont(&(CNI.lfontItem));
	CNI.unMenuID=0;
}

void CConflictResolutionCoolTree::DoEdit(HTREEITEM hItem)
{
	COOLTREE_NODE_INFO* pCNI=GetItemNodeInfo(hItem);
	if(!pCNI) return;
	switch(pCNI->net)
	{
	case NET_NORMAL:
		OnClick(NULL,NULL);
		break;
	case NET_EDIT:
	case NET_EDIT_WITH_VALUE:
	case NET_EDIT_FLOAT:
	case NET_EDIT_INT:
		//ShowEditWnd(hItem,pCNI);
		ShowEditSpinWnd(hItem,pCNI);
		break;
	case NET_EDITSPIN_WITH_VALUE:
	case NET_EDITSPIN:
		ShowEditSpinWnd(hItem,pCNI);
		break;
	case NET_COMBOBOX:
		ShowComboBoxWnd(hItem,pCNI);
		break;
	case NET_COMBOBOX_WITH_EDIT:
		ShowComboBoxWnd(hItem,pCNI,TRUE);
		break;
	case NET_SHOW_DIALOGBOX:
		GetParent()->SendMessage(UM_CEW_SHOW_DIALOGBOX_BEGIN,(WPARAM)hItem,(LPARAM)pCNI);
		CDialog* pDlg=((CDialog*)pCNI->pEditWnd);
		if(pDlg&&pDlg->DoModal()==IDOK)
			GetParent()->SendMessage(UM_CEW_SHOW_DIALOGBOX_END,(WPARAM)hItem,(LPARAM)pCNI);
		break;
	}
	m_hEditedItem=hItem;
	CTreeCtrl::SelectItem(m_hEditedItem); // add by adam 2007-05-28
}

void CConflictResolutionCoolTree::SetItemValueRange(HTREEITEM hItem,int nMin, int nMax, BOOL bVerify)
{
	COOLTREE_NODE_INFO* pCNI=GetItemNodeInfo(hItem);
	if(pCNI)
	{
		pCNI->fMinValue=static_cast<float>(nMin);
		pCNI->fMaxValue=static_cast<float>(nMax);
		pCNI->bVerify=bVerify;
	}
}

void CConflictResolutionCoolTree::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	// Create a memory DC compatible with the paint DC
	CDC memDC;
	memDC.CreateCompatibleDC( &dc );
	
	CRect rcClip, rcClient;
	dc.GetClipBox( &rcClip );
	GetClientRect(&rcClient);
	
	// Select a compatible bitmap into the memory DC
	CBitmap bitmap;
	bitmap.CreateCompatibleBitmap( &dc, rcClient.Width(), rcClient.Height() );
	memDC.SelectObject( &bitmap );
	
	// Set clip region to be same as that in paint DC
	CRgn rgn;
	rgn.CreateRectRgnIndirect( &rcClip );
	memDC.SelectClipRgn(&rgn);
	rgn.DeleteObject();
	
	
	
	// First let the control do its default drawing.
	CWnd::DefWindowProc( WM_PAINT, (WPARAM)memDC.m_hDC, 0 );
	
	HTREEITEM hItem = GetFirstVisibleItem();
	int n = GetVisibleCount()+1;
	while( hItem && n--)
	{
		CRect rect;
		
		// Do not meddle with selected items or drop highlighted items
		UINT selflag = TVIS_DROPHILITED | TVIS_SELECTED;
		COOLTREE_NODE_INFO* pCNI=GetItemNodeInfo(hItem)	;

		if ( !(GetItemState( hItem, selflag ) & selflag )&&pCNI)
		{
							
			CFont *pFontDC;
			CFont fontDC;//,fontBoldDC;
			LOGFONT logfont;
			
			if( pCNI->lfontItem.lfFaceName[0] != '\0' ) 
			{
				logfont = pCNI->lfontItem;
			}
			else
			{
				// No font specified, so use window font
				CFont *pFont = GetFont();
				pFont->GetLogFont( &logfont );
			}
			
			fontDC.CreateFontIndirect( &logfont );
	//		logfont.lfWeight=FW_BOLD;
	//		fontBoldDC.CreateFontIndirect( &logfont );
			pFontDC = memDC.SelectObject( &fontDC);

			
			memDC.SetTextColor( pCNI->clrItem);
			
			CString sItem = GetItemText( hItem );
			
			GetItemRect( hItem, &rect, TRUE );
			memDC.SetBkColor( GetSysColor( COLOR_WINDOW ) );
	//		if(pCNI->net==	NET_EDITSPIN_WITH_VALUE||pCNI->net==NET_EDIT_WITH_VALUE)
	//		{
				int nPosFind=sItem.Find(":",0);
				if(nPosFind != -1)
				{
					CString strLeft,strRight;
					strLeft=sItem.Left(nPosFind+1);
					strRight=sItem.Right(sItem.GetLength()-nPosFind-1);
					memDC.TextOut(rect.left+2,rect.top+1,strLeft);
					int nWidth=memDC.GetTextExtent(strLeft).cx;
				//	memDC.SelectObject(&fontBoldDC);
					memDC.SetTextColor( RGB(0,0,255));
					memDC.TextOut(rect.left+2+nWidth,rect.top+1,strRight);
				}
				else
				{
					memDC.TextOut(rect.left+2,rect.top+1,sItem);
				}

	//		}
	//		else
	//			memDC.TextOut(rect.left+2,rect.top+1,sItem);

			

				
			memDC.SelectObject( pFontDC );
		}
		hItem = GetNextVisibleItem( hItem );
	}
	
	
	dc.BitBlt( rcClip.left, rcClip.top, rcClip.Width(), rcClip.Height(), &memDC, 
		rcClip.left, rcClip.top, SRCCOPY );
	
}

void CConflictResolutionCoolTree::SetItemBold(HTREEITEM hItem,BOOL bBold,BOOL bRedraw)
{
	COOLTREE_NODE_INFO* pCNI=GetItemNodeInfo(hItem);
	if(pCNI)
	{
		pCNI->lfontItem.lfWeight==bBold?FW_BOLD:FW_NORMAL;
		if(bRedraw)
			Invalidate();
	}
}

void CConflictResolutionCoolTree::OnRclick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	CPoint point;
	GetCursorPos(&point);
	ScreenToClient(&point);
	UINT iRet;
	HTREEITEM hItem=HitTest(point,&iRet);
	m_hEditedItem=hItem;
	CTreeCtrl::SelectItem(m_hEditedItem); // add by adam 2007-05-28
	if(hItem)
	{
		SelectItem(hItem);
//		CRect rectItem;
//		GetItemRect(hItem,rectItem,TRUE);
//		CPoint ptCenter=rectItem.CenterPoint();
//		SendMessage(WM_LBUTTONDOWN,MK_LBUTTON,MAKELPARAM(ptCenter.x,ptCenter.y));
		COOLTREE_NODE_INFO* pCNI=GetItemNodeInfo(hItem);
		GetParent()->SendMessage(UM_CEW_SHOWMENU_BEGIN,(WPARAM)hItem,(LPARAM)pCNI);
		if(pCNI->unMenuID)
		{
			CMenu menu,*pMenu;
			menu.LoadMenu(pCNI->unMenuID);
			pMenu=menu.GetSubMenu(0);
			CPoint pt;
			GetCursorPos(&pt);
			GetParent()->SendMessage(UM_CEW_SHOWMENU_READY,(WPARAM)hItem,(LPARAM)pMenu);
			pMenu->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON, pt.x, pt.y, GetParent());
		}
		GetParent()->SendMessage(UM_CEW_SHOWMENU_END,(WPARAM)hItem,(LPARAM)pCNI);
	}
	*pResult = 0;
}

BOOL CConflictResolutionCoolTree::IsCheckBox(HTREEITEM hItem)
{
	COOLTREE_NODE_INFO* pCNI=GetItemNodeInfo(hItem);
	if(pCNI&&pCNI->nt==NT_CHECKBOX)
		return TRUE;
	return FALSE;
}

void CConflictResolutionCoolTree::SetChect2Status(HTREEITEM hItem)
{
	if(IsEnableItem(hItem))
		SetItemImage(hItem,TIIT_CHECKBOX_2_TRUE,TIIT_CHECKBOX_2_TRUE);
	else
		SetItemImage(hItem,TIIT_CHECKBOX_2_TRUE_DISABLE,TIIT_CHECKBOX_2_TRUE_DISABLE);
}

void CConflictResolutionCoolTree::AutoAdjustCheck2Status(HTREEITEM hItem)
{
	HTREEITEM hChild=GetChildItem(hItem);
	int nCheckStatue=-2;
	while(hChild)
	{
		if(IsCheckBox(hChild)&&nCheckStatue!=-2&&
			IsCheckItem(hChild)!=nCheckStatue)
		{
			
			SetChect2Status(hItem);
			return;
		}
		nCheckStatue=IsCheckItem(hChild);
		hChild=GetNextSiblingItem(hChild);
	}
	
}


int CConflictResolutionCoolTree::GetCmbBoxCurSel(HTREEITEM hItem)
{
	CWnd* pWnd=GetEditWnd(hItem);
	CTreeComboBox* pTCB=(CTreeComboBox*)pWnd;
	return pTCB->m_nIndexSeled;
}

DWORD_PTR CConflictResolutionCoolTree::GetCurSelCmbBoxItemData(HTREEITEM hItem)
{
	CWnd* pWnd=GetEditWnd(hItem);
	CTreeComboBox* pTCB=(CTreeComboBox*)pWnd;

	return pTCB->GetItemData(pTCB->m_nIndexSeled);
}

void CConflictResolutionCoolTree::OnSelchangeCombo()
{
	if(m_pWndComboBox==NULL) return;
	CTreeComboBox* pTCB=(CTreeComboBox*)m_pWndComboBox;
	pTCB->ShowWindow( SW_HIDE );
	pTCB->m_nIndexSeled=pTCB->GetCurSel();
	CString strValue;
	pTCB->GetWindowText(strValue);
	SendMessage(UM_CEW_COMBOBOX_SELCHANGE,(WPARAM)0,(LPARAM)&strValue);
}

HTREEITEM CConflictResolutionCoolTree::GetSelectedItem()
{
	return m_hEditedItem;
}

BOOL CConflictResolutionCoolTree::SelectItem(HTREEITEM hItem)
{
	if (NULL == hItem)
	{
		return FALSE;
	}

	m_hEditedItem = hItem;

	return CTreeCtrl::SelectItem(m_hEditedItem);
}

BOOL CConflictResolutionCoolTree::DeleteItem( HTREEITEM hItem )
{
	if( hItem == m_hEditedItem )
		m_hEditedItem = NULL;

	return CTreeCtrl::DeleteItem( hItem );
}