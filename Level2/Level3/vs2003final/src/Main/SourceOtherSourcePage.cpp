#include "StdAfx.h"
#include ".\SourceOtherSourcePage.h"

IMPLEMENT_DYNCREATE(CSourceOtherSourcePage, CPollutantSourcePage)

CSourceOtherSourcePage::CSourceOtherSourcePage(CSourceEmissionManager *pSourceEmissionManager,FuelProperties *pFuelProperties,CAirPollutantList *pAirPollutantList)
:CPollutantSourcePage(IDS_POLLUTANTSOURCE_OTHER,pFuelProperties,pAirPollutantList)
,m_pOtherSourceComposite(pSourceEmissionManager->GetOtherSourceComposite())
{
	m_pOtherSourceComposite->ReadData();
}
CSourceOtherSourcePage::CSourceOtherSourcePage()
{

}

CSourceOtherSourcePage::~CSourceOtherSourcePage(void)
{
}
BEGIN_MESSAGE_MAP(CSourceOtherSourcePage, CPollutantSourcePage)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_MENU_ADD_POLLUTANT,OnNewPollutant)
END_MESSAGE_MAP()
void CSourceOtherSourcePage::ReloadTree()
{
	for (int i=0;i<m_pOtherSourceComposite->GetCount();i++)
	{
		COtherSourceProperty *pOtherSourceProperty=m_pOtherSourceComposite->GetItem(i);
		AddOtherSourceItem(pOtherSourceProperty);
	}
}
BOOL CSourceOtherSourcePage::OnApply()
{
	m_pOtherSourceComposite->SaveData();
	return true;
}
void CSourceOtherSourcePage::OnButtonAdd()
{
	HTREEITEM hItem=m_tree.GetSelectedItem();
	if (hItem==NULL)
	{
		OnNewOtherSource();
	}
	if ((ItemType)GetItemType(hItem)==Type_SourceName)
	{
		OnNewPollutant();
	}	
}
void CSourceOtherSourcePage::OnButtonDel()
{
	HTREEITEM hItem=m_tree.GetSelectedItem();
	if (hItem==NULL)
	{
		return;
	}
	ItemType itemType=(ItemType)GetItemType(hItem);
	if (itemType==Type_SourceName)
	{
		OnDelOtherSource();
	}else if(itemType==Type_Pollutant)
	{
		OnDelPollutant();
	}
	SetModified();
}
void CSourceOtherSourcePage::OnNewOtherSource()
{
	COtherSourceProperty* pOtherSourceProperty=new COtherSourceProperty();
	m_pOtherSourceComposite->AddItem(pOtherSourceProperty);
	AddOtherSourceItem(pOtherSourceProperty);
	SetModified();
}
void CSourceOtherSourcePage::OnNewPollutant()
{
	HTREEITEM hItem=m_tree.GetSelectedItem();
	if (hItem==NULL)
	{
		return;
	}
	COtherSourceProperty* pOtherSourceProperty=(COtherSourceProperty *)m_tree.GetItemData(hItem);
	if (pOtherSourceProperty==NULL)
	{
		return;
	}
	COtherSourcePollutantData *pSetting=new COtherSourcePollutantData();
	pOtherSourceProperty->AddItem(pSetting);
	AddPollutantItem(pSetting,hItem);
	SetModified();
}

void CSourceOtherSourcePage::OnDelOtherSource()
{
	HTREEITEM hItem=m_tree.GetSelectedItem();
	if (hItem==NULL)
	{
		return;
	}
	COtherSourceProperty* pOtherSourceProperty=(COtherSourceProperty *)m_tree.GetItemData(hItem);
	m_pOtherSourceComposite->DeleteItem(pOtherSourceProperty);
	HTREEITEM hSelItem=m_tree.GetNextItem(hItem,TVGN_NEXT);
	if (hSelItem==NULL)
	{
		hSelItem=m_tree.GetPrevSiblingItem(hItem);
	}	
	m_tree.DeleteItem(hItem);
	if (hSelItem!=NULL)
	{
		m_tree.SelectItem(hSelItem);
	}else
	{
		m_toolBar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONDEL,FALSE);
	}
}
void CSourceOtherSourcePage::OnDelPollutant()
{
	HTREEITEM hItem=m_tree.GetSelectedItem();
	if (hItem==NULL)
	{
		return;
	}
	COtherSourcePollutantData *pSetting=(COtherSourcePollutantData *)m_tree.GetItemData(hItem);
	HTREEITEM hParentItem=m_tree.GetParentItem(hItem);
	COtherSourceProperty* pOtherSourceProperty=(COtherSourceProperty *)m_tree.GetItemData(hParentItem);
	pOtherSourceProperty->DeleteItem(pSetting);

	HTREEITEM hSelItem=m_tree.GetNextItem(hItem,TVGN_NEXT);
	if (hSelItem==NULL)
	{
		hSelItem=m_tree.GetPrevSiblingItem(hItem);
		if (hSelItem==NULL)
		{
			hSelItem=m_tree.GetParentItem(hItem);
		}
	}	
	m_tree.DeleteItem(hItem);
	if (hSelItem!=NULL)
	{
		m_tree.SelectItem(hSelItem);
	}
}

int CSourceOtherSourcePage::GetItemType(HTREEITEM hItem)
{
	int nLevel=-1;
	if (hItem!=NULL)
	{
		HTREEITEM tmpItem=hItem;
		while(tmpItem!=NULL)
		{
			tmpItem=m_tree.GetParentItem(tmpItem);
			nLevel++;
		}
	}
	return nLevel;	
}
HTREEITEM CSourceOtherSourcePage::AddOtherSourceItem(COtherSourceProperty* pOtherSourceProperty)
{
	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);
	cni.net=NET_LABLE;
    CString strSourceName;
	strSourceName=_T("Source Name:")+pOtherSourceProperty->GetName();
	HTREEITEM hOtherSourceItem=m_tree.InsertItem(strSourceName,cni,FALSE);
	m_tree.SetItemData(hOtherSourceItem,(DWORD)pOtherSourceProperty);

	for (int i=0;i<pOtherSourceProperty->GetCount();i++)
	{
		COtherSourcePollutantData *pSetting=pOtherSourceProperty->GetItem(i);
		AddPollutantItem(pSetting,hOtherSourceItem);
	}

	m_tree.Expand(hOtherSourceItem,TVE_EXPAND);
	m_tree.SelectItem(hOtherSourceItem);
	return hOtherSourceItem;
}
HTREEITEM CSourceOtherSourcePage::AddPollutantItem(COtherSourcePollutantData *pSetting,HTREEITEM hParentItem)
{
	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);
	cni.net=NET_COMBOBOX;
	
	CString strPollutant;
	strPollutant=m_pAirPollutantList->getSymbolByID(pSetting->m_dPollutantID);
	if (strPollutant==_T(""))
	{
		strPollutant=_T("Pollutant: Please Choose");
	}else
	{
		strPollutant=_T("Pollutant: ")+strPollutant;
	}
	
	HTREEITEM hPollutantItem=m_tree.InsertItem(strPollutant,cni,FALSE,FALSE,hParentItem);
	m_tree.SetItemData(hPollutantItem,(DWORD)pSetting);
	
	cni.net=NET_EDIT_FLOAT;
	CString strEmission;
	strEmission.Format(_T("Emission(gm/day):%.2f"),pSetting->m_dEmission);
	cni.fMinValue=(float)pSetting->m_dEmission;
	HTREEITEM hEmissionItem=m_tree.InsertItem(strEmission,cni,FALSE,FALSE,hPollutantItem);

	m_tree.Expand(hPollutantItem,TVE_EXPAND);
	m_tree.SelectItem(hPollutantItem);
	return hPollutantItem;	
	
}
void CSourceOtherSourcePage::OnContextMenu(CWnd* pWnd, CPoint point)
{
	CRect rcTree;
	m_tree.GetWindowRect(rcTree);
	if (!rcTree.PtInRect(point))
	{
		return;
	}
	m_tree.SetFocus();
	CPoint pt=point;
	m_tree.ScreenToClient(&pt);
	UINT iRet;
	HTREEITEM hRClickItem=m_tree.HitTest(pt,&iRet);
	if (iRet!=TVHT_ONITEMLABEL)
	{
		hRClickItem=NULL;
	}
	if (hRClickItem==NULL)
	{
		return;
	}
	m_tree.SelectItem(hRClickItem);
	int itemType=GetItemType(hRClickItem);

	CMenu menuPopup;
	
	switch ((ItemType)itemType)
	{
	case Type_SourceName:
		{
			menuPopup.LoadMenu(IDR_MENU_ADD_POLLUTANT);
			CMenu *pMenu=NULL;
			pMenu=menuPopup.GetSubMenu(0);
			if (pMenu)
			{
				pMenu->TrackPopupMenu(TPM_LEFTALIGN,point.x,point.y,this);
			}
			break;
		}
	}
}
void CSourceOtherSourcePage::OnTvnSelchangedTreePollutantsource(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	HTREEITEM hItem=pNMTreeView->itemNew.hItem;
	if (hItem==NULL)
	{
		m_toolBar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONADD,TRUE);
		return;
	}
	ItemType itemType=(ItemType)GetItemType(hItem);
	if (itemType==Type_SourceName)
	{
		m_toolBar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONDEL,TRUE);
		m_toolBar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONADD,TRUE);
	}
	if (itemType==Type_Pollutant)
	{
		m_toolBar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONDEL,TRUE);
		m_toolBar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONADD,FALSE);
	}
	if(itemType==Type_Emission)
	{
		m_toolBar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONDEL,FALSE);
		m_toolBar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONADD,FALSE);
	}

	*pResult = 0;
}
LRESULT CSourceOtherSourcePage::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{	
	case UM_CEW_LABLE_END:
		{
			HTREEITEM hItem=(HTREEITEM)wParam;
			CString strValue = *((CString*)lParam);
			COtherSourceProperty *pOtherSourceProperty=(COtherSourceProperty *)m_tree.GetItemData(hItem);
			if (pOtherSourceProperty==NULL)
			{
				break;
			}
			CString strSourceName;

			if( strValue == "" )
			{
				strSourceName=_T("Source Name:")+pOtherSourceProperty->GetName();
				m_tree.SetItemText(hItem,strSourceName);
				break;
			}

			if (pOtherSourceProperty->GetName()==strValue)
			{
				strSourceName=_T("Source Name:")+pOtherSourceProperty->GetName();
				m_tree.SetItemText(hItem,strSourceName);
				break;
			}
			if (m_pOtherSourceComposite->NameExist(strValue))
			{
				strSourceName=_T("Source Name:")+pOtherSourceProperty->GetName();
				m_tree.SetItemText(hItem,strSourceName);
				MessageBox("Name already exists.");
				break;
			}
			strSourceName=_T("Source Name:")+strValue;
			m_tree.SetItemText(hItem,strSourceName);
			pOtherSourceProperty->SetName(strValue);
			SetModified();
			break;
		}
	case UM_CEW_EDIT_END:
	case UM_CEW_EDITSPIN_END:
		{
			HTREEITEM hItem=(HTREEITEM)wParam;
			CString strValue = *((CString*)lParam);
				
			HTREEITEM hParentItem=m_tree.GetParentItem(hItem);
			if (hParentItem==NULL)
			{
				break;
			}
			COtherSourcePollutantData *pSetting=(COtherSourcePollutantData *)m_tree.GetItemData(hParentItem);
			if (pSetting==NULL)
			{
				break;
			}

			double dValue;
			if( strValue == "" )
			{
				dValue=0;
			}else
			{
				dValue= atof( strValue );
			}			

			CString strEmission;
			if (dValue<0 )
			{						
				dValue=0;
				break;
			}
			pSetting->m_dEmission=dValue;
			strEmission.Format(_T("Emission(gm/day):%.2f"),dValue);
			m_tree.SetItemText(hItem,strEmission);

			COOLTREE_NODE_INFO *cni=m_tree.GetItemNodeInfo(hItem);
			cni->fMinValue=(float)dValue;

			SetModified();

			break;
		}
	case UM_CEW_COMBOBOX_BEGIN:
		{
			HTREEITEM hItem=(HTREEITEM)wParam;
			CWnd* pWnd=m_tree.GetEditWnd(hItem);
			CRect rectWnd;
			m_tree.GetItemRect(hItem,rectWnd,TRUE);
			pWnd->SetWindowPos(NULL,rectWnd.right,rectWnd.top,0,0,SWP_NOSIZE);
			CComboBox* pCB=(CComboBox*)pWnd;
			pCB->ResetContent();
			pCB->SetDroppedWidth(100);
			CAirpollutant *curPollutant;
			for (int i=0;i<m_pAirPollutantList->GetItemCount();i++)
			{
				curPollutant=(CAirpollutant *)m_pAirPollutantList->GetItem(i);
				int nIndex = pCB->AddString(curPollutant->getSymbol());
				pCB->SetItemData(nIndex,curPollutant->GetID());
			}
			break;

			break;
		}
	case UM_CEW_COMBOBOX_SELCHANGE:
		{
			HTREEITEM hFirstItem=m_tree.GetFirstVisibleItem();

			CWnd* pWnd = m_tree.GetEditWnd((HTREEITEM)wParam);
			CComboBox* pCB = (CComboBox*)pWnd;
 			HTREEITEM hPollutantItem=(HTREEITEM)wParam;
			HTREEITEM hParentItem=m_tree.GetParentItem(hPollutantItem);
			if (hPollutantItem==NULL || hParentItem==NULL)
			{
				break;
			}
			COtherSourcePollutantData *pSetting=(COtherSourcePollutantData *)m_tree.GetItemData(hPollutantItem);
			if (pSetting==NULL)
			{
				break;
			}

			CString strText = *(CString*)lParam;
			CString strPollutant;

			int nSel = pCB->GetCurSel();
			int nPollutantID = pCB->GetItemData(nSel);
			

			if (pSetting->m_dPollutantID==nPollutantID)
			{
				strPollutant=_T("Pollutant: ")+m_pAirPollutantList->getSymbolByID(nPollutantID);
				m_tree.SetItemText(hPollutantItem,strPollutant);
				break;
			}

			COtherSourceProperty *pOtherSourceProperty=(COtherSourceProperty *)m_tree.GetItemData(hParentItem);
			if (pOtherSourceProperty==NULL)
			{
				break;
			}


			if (pOtherSourceProperty->PollutantExist(nPollutantID))
			{
				if (pSetting->m_dPollutantID==-1)
				{
                    strPollutant=_T("Pollutant: Please choose");
				}else
				{
					strPollutant=_T("Pollutant: ")+m_pAirPollutantList->getSymbolByID(pSetting->m_dPollutantID);
				}
				
				m_tree.SetItemText(hPollutantItem,strPollutant);
				MessageBox("Pollutant already exist");
				break;
			}
			pSetting->m_dPollutantID=nPollutantID;

			strPollutant=m_pAirPollutantList->getSymbolByID(nPollutantID);
			if (strPollutant==_T(""))
			{
				strPollutant=_T("Pollutant: Please Choose");
			}else
			{
				strPollutant=_T("Pollutant: ")+strPollutant;
			}

			m_tree.SetItemText(hPollutantItem,strPollutant);
			m_tree.EnsureVisible(hFirstItem);

			SetModified();

			break;
		}
	}
	return CPollutantSourcePage::DefWindowProc(message, wParam, lParam);
}