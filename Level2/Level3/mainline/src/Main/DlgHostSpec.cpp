// DlgHostSpec.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "TermPlan.h"
#include "DlgHostSpec.h"
#include "termplandoc.h"
#include ".\Inputs\TrayHost.h"


// CDlgHostSpec dialog

const UINT ID_NEW     = 10;
const UINT ID_MODIFY    = 11;
const UINT ID_DEL     = 12;

IMPLEMENT_DYNAMIC(CDlgHostSpec, CXTResizeDialog)
CDlgHostSpec::CDlgHostSpec(int nProjID,InputTerminal* pInterm,CWnd* pParent /*=NULL*/)
	: CXTResizeDialog(CDlgHostSpec::IDD, pParent)
	,m_nProjID(nProjID)
	,m_pInterm(pInterm)
{
	m_pTrayHostList = pInterm->GetTrayHostList();
}

CDlgHostSpec::~CDlgHostSpec()
{

}

void CDlgHostSpec::DoDataExchange(CDataExchange* pDX)
{
	CXTResizeDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_HOST, m_wndTreeCtrl);
}


BEGIN_MESSAGE_MAP(CDlgHostSpec, CXTResizeDialog)
	ON_WM_CREATE()
	ON_COMMAND(ID_NEW, OnCmdNew)
	ON_COMMAND(ID_DEL, OnCmdDelete)
	ON_COMMAND(ID_MODIFY, OnCmdEdit)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnBnClickedSave)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_HOST, OnTvnSelchangedTreeItem)
END_MESSAGE_MAP()


// CDlgHostSpec message handlers
int CDlgHostSpec::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CXTResizeDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_ToolBar.CreateEx(this, TBSTYLE_FLAT,WS_CHILD|WS_VISIBLE|CBRS_ALIGN_TOP)
		|| !m_ToolBar.LoadToolBar(IDR_BAGDEV_GATE))
	{
		return -1;
	}

	CToolBarCtrl& toolbar = m_ToolBar.GetToolBarCtrl();
	toolbar.SetCmdID(0, ID_NEW);
	toolbar.SetCmdID(1, ID_DEL);
	toolbar.SetCmdID(2, ID_MODIFY);

	return 0;
}

BOOL CDlgHostSpec::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog();

	InitToolBar();

	SetTreeContent();

	UpdateToolBar();

	SetResize(IDC_BUTTON_SAVE, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDOK, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDCANCEL, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDC_STATIC, SZ_TOP_LEFT, SZ_BOTTOM_RIGHT);
	SetResize(m_ToolBar.GetDlgCtrlID(), SZ_TOP_LEFT, SZ_TOP_LEFT);
	SetResize(IDC_TREE_HOST, SZ_TOP_LEFT, SZ_BOTTOM_RIGHT);

	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(FALSE);

	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),TRUE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),FALSE);

	return TRUE;
}

void CDlgHostSpec::InitToolBar(void)
{
	CRect rect;
	m_wndTreeCtrl.GetWindowRect( &rect );
	ScreenToClient( &rect );
	rect.top -= 26;
	rect.bottom = rect.top + 22;
	rect.left += 4;
	m_ToolBar.MoveWindow(&rect);

	m_ToolBar.GetToolBarCtrl().EnableButton( ID_NEW, FALSE);
	m_ToolBar.GetToolBarCtrl().EnableButton( ID_DEL, FALSE);
	m_ToolBar.GetToolBarCtrl().EnableButton( ID_MODIFY, FALSE);
}

LRESULT CDlgHostSpec::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	case UM_CEW_LABLE_END:
		{
			HTREEITEM hItem = (HTREEITEM)wParam;
			CString strValue = *((CString*)lParam);
			TowNodeData* pNodeData = (TowNodeData*)m_wndTreeCtrl.GetItemData(hItem);
			TrayHost* pData = (TrayHost*)pNodeData->dwData;
			if (strValue.IsEmpty())
			{
				::AfxMessageBox(_T("Host name can not be empty"));
				m_wndTreeCtrl.DoEdit(hItem);
			}
			else if (m_pTrayHostList->FindItem(strValue) != -1)
			{
				::AfxMessageBox(_T("Host name is exist !."));
				m_wndTreeCtrl.DoEdit(hItem);
			}
			else
			{
				pData->SetHostName(strValue);
				m_wndTreeCtrl.SetItemText(hItem,strValue);
			}
		}
		break;
	case UM_CEW_EDITSPIN_END:
		{
			HTREEITEM hItem = (HTREEITEM)wParam;
			CString strValue = *((CString*)lParam);
			TowNodeData* pNodeData = (TowNodeData*)m_wndTreeCtrl.GetItemData(hItem);
			COOLTREE_NODE_INFO* cni = m_wndTreeCtrl.GetItemNodeInfo(hItem);
			TrayHost* pData = (TrayHost*)pNodeData->dwData;
			CString strData = _T("");
		
			pData->SetRadius(atoi(strValue));
			strData.Format("Within(cm): %d",pData->GetRadius());
				
			cni->fMinValue = static_cast<float>(atoi(strValue));
			m_wndTreeCtrl.SetItemText(hItem,strData);
			GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
		}
		break;
	case UM_CEW_COMBOBOX_BEGIN:
		{
			CRect rectWnd;
			HTREEITEM hItem = (HTREEITEM)wParam;
			CWnd* pWnd=m_wndTreeCtrl.GetEditWnd(hItem);
			m_wndTreeCtrl.GetItemRect(hItem,rectWnd,TRUE);
			pWnd->SetWindowPos(NULL,rectWnd.right,rectWnd.top,0,0,SWP_NOSIZE);
			CComboBox* pCB=(CComboBox*)pWnd;
			pCB->ResetContent();
			TowNodeData* pNodeData = (TowNodeData*)m_wndTreeCtrl.GetItemData(hItem);
			TrayHost* pData = (TrayHost*)pNodeData->dwData;

			if (pCB->GetCount() == 0)
			{
				int iNopaxCount = m_pInterm->m_pMobElemTypeStrDB->GetCount();
				for( int i=1; i<iNopaxCount; i++ )
				{	
					CString strLabel = m_pInterm->m_pMobElemTypeStrDB->GetString( i );
					int iIndex = pCB->AddString( strLabel );
					assert( iIndex!=LB_ERR );
					pCB->SetItemData( iIndex, i ); 
				}
			}
			if (pCB->GetCount() > 0 && pNodeData->nNonPaxIndex == -1)
			{
				pCB->SetCurSel(0);
			}
		}
		break;
	case UM_CEW_COMBOBOX_SELCHANGE:
		{
			HTREEITEM hItem = (HTREEITEM)wParam;
			int nSel = m_wndTreeCtrl.GetCmbBoxCurSel(hItem);
			TowNodeData* pNodeData = (TowNodeData*)m_wndTreeCtrl.GetItemData(hItem);
			CString strValue = _T("");
			TrayHost* pData = (TrayHost*)pNodeData->dwData;

			switch(pNodeData->emNodeType)
			{
			case HOST_MOB:
				{
					if (pData->FindItem(nSel+1) == -1)
					{
						pData->SetHostIndex(nSel + 1);
					}
					else
					{
						CString strNonPax(_T(""));
						strNonPax.Format(_T("%s is exist !"),m_pInterm->m_pMobElemTypeStrDB->GetString(nSel + 1));
						::AfxMessageBox(strNonPax);
					}
					strValue.Format("Non-passenger mobile element: %s",m_pInterm->m_pMobElemTypeStrDB->GetString(pData->GetHostIndex()));
					m_wndTreeCtrl.SetItemText(hItem,strValue);
				}
				break;
			case HOST_ITEM:
				{
					if (pNodeData->nNonPaxIndex == -1 && pData->FindItem(nSel + 1) == -1 && nSel + 1 != pData->GetHostIndex())
					{
						pData->AddItem(nSel + 1);
						m_wndTreeCtrl.SetItemText(hItem,m_pInterm->m_pMobElemTypeStrDB->GetString(nSel + 1));
					}
					else if(pNodeData->nNonPaxIndex != nSel + 1 && pData->FindItem(nSel + 1) == -1 && nSel + 1 != pData->GetHostIndex())
					{
						pData->SetItem(pNodeData->nNonPaxIndex,nSel + 1);
						m_wndTreeCtrl.SetItemText(hItem,m_pInterm->m_pMobElemTypeStrDB->GetString(nSel + 1));
					}
					else if (pNodeData->nNonPaxIndex == nSel + 1)
					{
						m_wndTreeCtrl.SetItemText(hItem,m_pInterm->m_pMobElemTypeStrDB->GetString(nSel + 1));
					}
					else
					{
						CString strNonPax(_T(""));
						strNonPax.Format(_T("%s is exist !"),m_pInterm->m_pMobElemTypeStrDB->GetString(nSel + 1));
						::AfxMessageBox(strNonPax);
						m_wndTreeCtrl.DeleteItem(hItem);
					}
				}
				break;
			default:
				ASSERT(FALSE);
				break;
			}
		
		//	m_wndTreeCtrl.Expand(hItem,TVE_EXPAND);
			GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
		}
		break;
	default:
		break;

	}
	return CXTResizeDialog::DefWindowProc(message, wParam, lParam);
}

void CDlgHostSpec::UpdateToolBar(void)
{
 	HTREEITEM hItem = m_wndTreeCtrl.GetSelectedItem();
 	if (hItem)
 	{
 		TowNodeData* pNodeData = (TowNodeData*)m_wndTreeCtrl.GetItemData(hItem);
 		TrayHost* pData = (TrayHost*)pNodeData->dwData;
 		if (hItem == m_wndTreeCtrl.GetRootItem())
 		{
 			m_ToolBar.GetToolBarCtrl().EnableButton( ID_NEW, TRUE);
 			m_ToolBar.GetToolBarCtrl().EnableButton( ID_DEL, FALSE );
 			m_ToolBar.GetToolBarCtrl().EnableButton( ID_MODIFY, FALSE );
 		}
		else if (pNodeData->emNodeType == HOST_MOB)
		{
			m_ToolBar.GetToolBarCtrl().EnableButton( ID_NEW, FALSE);
			m_ToolBar.GetToolBarCtrl().EnableButton( ID_DEL, FALSE );
			m_ToolBar.GetToolBarCtrl().EnableButton( ID_MODIFY, TRUE );
		}
 		else if (pNodeData->emNodeType == HOST_CON)
 		{
			m_ToolBar.GetToolBarCtrl().EnableButton( ID_NEW, TRUE);
			m_ToolBar.GetToolBarCtrl().EnableButton( ID_DEL, FALSE );
			m_ToolBar.GetToolBarCtrl().EnableButton( ID_MODIFY, FALSE );
 		}
		else if (pNodeData->emNodeType == HOST_ITEM)
 		{
 			m_ToolBar.GetToolBarCtrl().EnableButton( ID_NEW, FALSE);
 			m_ToolBar.GetToolBarCtrl().EnableButton( ID_DEL, TRUE );
 			m_ToolBar.GetToolBarCtrl().EnableButton( ID_MODIFY, TRUE );
 		}
		else if (pNodeData->emNodeType == HOST_NAME)
		{
			m_ToolBar.GetToolBarCtrl().EnableButton( ID_NEW, FALSE);
			m_ToolBar.GetToolBarCtrl().EnableButton( ID_DEL, TRUE );
			m_ToolBar.GetToolBarCtrl().EnableButton( ID_MODIFY, TRUE );
		}
		else if (pNodeData->emNodeType == HOST_RADIUS)
		{
			m_ToolBar.GetToolBarCtrl().EnableButton( ID_NEW, FALSE);
			m_ToolBar.GetToolBarCtrl().EnableButton( ID_DEL, FALSE );
			m_ToolBar.GetToolBarCtrl().EnableButton( ID_MODIFY, TRUE );
		}
 	}
 	else
 	{
 		m_ToolBar.GetToolBarCtrl().EnableButton( ID_NEW, FALSE);
 		m_ToolBar.GetToolBarCtrl().EnableButton( ID_DEL, FALSE );
 		m_ToolBar.GetToolBarCtrl().EnableButton( ID_MODIFY, FALSE );
 	}
}
void CDlgHostSpec::AddHostNode()
{
	TrayHost *pHost = new TrayHost;

	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);
	cni.nt = NT_NORMAL;
	cni.net = NET_LABLE;
	CString strCaption(_T(""));

	TowNodeData* pNodeData = new TowNodeData();
	strCaption.Format(_T(" %s"),pHost->GetHostName());
	HTREEITEM hHostItem = m_wndTreeCtrl.InsertItem(strCaption,cni,FALSE,FALSE,m_wndTreeCtrl.GetRootItem(),TVI_LAST);
	pNodeData->dwData = (DWORD)pHost;
	pNodeData->emNodeType = HOST_NAME;
	m_wndTreeCtrl.SetItemData(hHostItem, (DWORD_PTR)pNodeData);

	cni.nt = NT_NORMAL;
	cni.net = NET_COMBOBOX;
	strCaption.Format("Non-passenger mobile element: %s",m_pInterm->m_pMobElemTypeStrDB->GetString(pHost->GetHostIndex()));
	pNodeData = new TowNodeData();
	pNodeData->dwData = (DWORD)pHost;
	pNodeData->emNodeType = HOST_MOB;
	HTREEITEM hMobItem = m_wndTreeCtrl.InsertItem(strCaption,cni,FALSE,FALSE,hHostItem,TVI_LAST);
	m_wndTreeCtrl.SetItemData(hMobItem,(DWORD_PTR)pNodeData);

	cni.nt = NT_NORMAL;
	cni.net = NET_NORMAL;
	CString strEnplanementTime(_T(""));
	strCaption.Format("Constrains");
	pNodeData = new TowNodeData();
	pNodeData->dwData = (DWORD)pHost;
	pNodeData->emNodeType = HOST_CON;
	HTREEITEM hConItem = m_wndTreeCtrl.InsertItem(strCaption,cni,FALSE,FALSE,hMobItem,TVI_LAST);
	m_wndTreeCtrl.SetItemData(hConItem,(DWORD_PTR)pNodeData);

	cni.nt = NT_NORMAL;
	cni.net = NET_EDIT_INT;
	cni.fMinValue = static_cast<float>(pHost->GetRadius());
	strCaption.Format(_T("Within(cm): %d"),pHost->GetRadius());
	pNodeData = new TowNodeData();
	pNodeData->dwData = (DWORD)pHost;
	pNodeData->emNodeType = HOST_RADIUS;
	HTREEITEM hRadiusItem = m_wndTreeCtrl.InsertItem(strCaption,cni,FALSE,FALSE,hMobItem,TVI_LAST);
	m_wndTreeCtrl.SetItemData(hRadiusItem,(DWORD_PTR)pNodeData);

	for(int j = 0; j < pHost->GetCount(); j++)
	{
		cni.nt = NT_NORMAL;
		cni.net = NET_COMBOBOX;
		int nNonPaxIndex = pHost->GetItem(j);
		pNodeData = new TowNodeData();
		pNodeData->dwData = (DWORD)pHost;
		pNodeData->emNodeType = HOST_ITEM;
		pNodeData->nNonPaxIndex = nNonPaxIndex;
		HTREEITEM hNonItem = m_wndTreeCtrl.InsertItem(m_pInterm->m_pMobElemTypeStrDB->GetString(nNonPaxIndex),cni,FALSE,FALSE,hMobItem,TVI_LAST);
		m_wndTreeCtrl.SetItemData(hNonItem,(DWORD_PTR)pNodeData);
	}

	m_pTrayHostList->AddItem(pHost);
	m_wndTreeCtrl.Expand(m_wndTreeCtrl.GetRootItem(),TVE_EXPAND);
	m_wndTreeCtrl.Expand(hHostItem,TVE_EXPAND);
	m_wndTreeCtrl.Expand(hMobItem,TVE_EXPAND);
	m_wndTreeCtrl.DoEdit(hHostItem);
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
}

void CDlgHostSpec::AddNonPaxNode(HTREEITEM hItem)
{
	ASSERT(hItem);
	CString strData(_T(""));
	TowNodeData* pNodeData = (TowNodeData*)m_wndTreeCtrl.GetItemData(hItem);
	TrayHost* pData = (TrayHost*)pNodeData->dwData;

	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);
	cni.nt = NT_NORMAL;
	cni.net = NET_COMBOBOX;

	pNodeData = new TowNodeData();
	pNodeData->dwData = (DWORD)pData;
	pNodeData->emNodeType = HOST_ITEM;
	
	HTREEITEM hNonItem = m_wndTreeCtrl.InsertItem(_T(""),cni,FALSE,FALSE,hItem,TVI_LAST);
	m_wndTreeCtrl.Expand(hItem,TVE_EXPAND);
	m_wndTreeCtrl.SetItemData(hNonItem,(DWORD_PTR)pNodeData);
	m_wndTreeCtrl.DoEdit(hNonItem);
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);	
}

void CDlgHostSpec::OnCmdNew()
{
	HTREEITEM hItem = m_wndTreeCtrl.GetSelectedItem();
	if (hItem == m_wndTreeCtrl.GetRootItem())
	{
		AddHostNode();
	}
	else if(hItem)
	{
		TowNodeData* pNodeData = (TowNodeData*)m_wndTreeCtrl.GetItemData(hItem);
		TrayHost* pData = (TrayHost*)pNodeData->dwData;
		if (pNodeData->emNodeType == HOST_NAME)
		{
			AddHostNode();
		}
		else if (pNodeData->emNodeType == HOST_CON)
		{
			AddNonPaxNode(hItem);
		}
	}
}

bool CDlgHostSpec::CheckDeleteOperationValid(TrayHost* pHost)
{
	ASSERT(pHost);
	CPaxFlowConvertor m_paxFlowConvertor;
	m_paxFlowConvertor.SetInputTerm(m_pInterm);
	CAllPaxTypeFlow m_allPaxFlow;
	m_allPaxFlow.ClearAll();
	m_allPaxFlow.FromOldToDigraphStructure( m_paxFlowConvertor );
	for (int i = 0; i < m_allPaxFlow.GetSinglePaxTypeFlowCount(); i++)
	{
		CSinglePaxTypeFlow* pSingleFlow = m_allPaxFlow.GetSinglePaxTypeFlowAt(i);
		int nIndex = pSingleFlow->GetPassengerConstrain()->GetTypeIndex();
		if(nIndex == m_pTrayHostList->FindItem(pHost->GetHostName()) + ALLPAX_COUNT)
		{
			return true;
		}
	}
	return false;
}

void CDlgHostSpec::OnCmdDelete()
{
	HTREEITEM hItem = m_wndTreeCtrl.GetSelectedItem();
	ASSERT(hItem);
	TowNodeData* pNodeData = NULL;
	pNodeData = (TowNodeData*)m_wndTreeCtrl.GetItemData(hItem);
	if (pNodeData)
	{
		TrayHost* pHost = (TrayHost*)pNodeData->dwData;
		switch(pNodeData->emNodeType)
		{
		case HOST_NAME:
			{
				if (CheckDeleteOperationValid(pHost))
				{
					CString strError(_T(""));
					strError.Format(_T("Can not delete %s, %s is used in passenger flow"),pHost->GetHostName(),pHost->GetHostName());
					::AfxMessageBox(strError);
				}
				else
				{
					m_pTrayHostList->DeleteItem(pHost);
					m_wndTreeCtrl.DeleteItem(hItem);
					GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
				}
			}
			break;
		case HOST_ITEM:
			{
				pHost->RemoveItem(pNodeData->nNonPaxIndex);
				m_wndTreeCtrl.DeleteItem(hItem);
				GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
			}
			break;
		default:
			ASSERT(FALSE);
			break;
		}
	}
}

void CDlgHostSpec::OnCmdEdit()
{
	HTREEITEM hItem = m_wndTreeCtrl.GetSelectedItem();
	if (hItem)
	{
		m_wndTreeCtrl.DoEdit(hItem);
	}
}

void CDlgHostSpec::SetTreeContent(void)
{
	m_wndTreeCtrl.DeleteAllItems();
	m_wndTreeCtrl.SetImageList(m_wndTreeCtrl.GetImageList(TVSIL_NORMAL),TVSIL_NORMAL);

	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);
	cni.nt = NT_NORMAL;
	cni.net = NET_SHOW_DIALOGBOX;

	TowNodeData* pNodeData = new TowNodeData();
	pNodeData->dwData = (DWORD)m_pTrayHostList;
	pNodeData->emNodeType = HOST_NON;
	HTREEITEM hItem = m_wndTreeCtrl.InsertItem(_T("Hosts"),cni,FALSE);
	m_wndTreeCtrl.SetItemData(hItem,(DWORD_PTR)pNodeData);

	for (int i=0; i<(int)m_pTrayHostList->GetItemCount(); i++)
	{
		TrayHost *pHost = m_pTrayHostList->GetItem(i);

		cni.nt = NT_NORMAL;
		cni.net = NET_LABLE;
		CString strCaption(_T(""));

		TowNodeData* pNodeData = new TowNodeData();
		strCaption.Format(_T(" %s"),pHost->GetHostName());
		HTREEITEM hHostItem = m_wndTreeCtrl.InsertItem(strCaption,cni,FALSE,FALSE,hItem,TVI_LAST);
		pNodeData->dwData = (DWORD)pHost;
		pNodeData->emNodeType = HOST_NAME;
		m_wndTreeCtrl.SetItemData(hHostItem, (DWORD_PTR)pNodeData);

		cni.nt = NT_NORMAL;
		cni.net = NET_COMBOBOX;
		strCaption.Format("Non-passenger mobile element: %s",m_pInterm->m_pMobElemTypeStrDB->GetString(pHost->GetHostIndex()));
		pNodeData = new TowNodeData();
		pNodeData->dwData = (DWORD)pHost;
		pNodeData->emNodeType = HOST_MOB;
		HTREEITEM hMobItem = m_wndTreeCtrl.InsertItem(strCaption,cni,FALSE,FALSE,hHostItem,TVI_LAST);
		m_wndTreeCtrl.SetItemData(hMobItem,(DWORD_PTR)pNodeData);

		cni.nt = NT_NORMAL;
		cni.net = NET_NORMAL;
		CString strEnplanementTime(_T(""));
		strCaption.Format("Constrains");
		pNodeData = new TowNodeData();
		pNodeData->dwData = (DWORD)pHost;
		pNodeData->emNodeType = HOST_CON;
		HTREEITEM hConItem = m_wndTreeCtrl.InsertItem(strCaption,cni,FALSE,FALSE,hMobItem,TVI_LAST);
		m_wndTreeCtrl.SetItemData(hConItem,(DWORD_PTR)pNodeData);

		cni.nt = NT_NORMAL;
		cni.net = NET_EDIT_INT;
		cni.fMinValue = static_cast<float>(pHost->GetRadius());
		strCaption.Format(_T("Within(cm): %d"),pHost->GetRadius());
		pNodeData = new TowNodeData();
		pNodeData->dwData = (DWORD)pHost;
		pNodeData->emNodeType = HOST_RADIUS;
		HTREEITEM hRadiusItem = m_wndTreeCtrl.InsertItem(strCaption,cni,FALSE,FALSE,hMobItem,TVI_LAST);
		m_wndTreeCtrl.SetItemData(hRadiusItem,(DWORD_PTR)pNodeData);

		for(int j = 0; j < pHost->GetCount(); j++)
		{
			cni.nt = NT_NORMAL;
			cni.net = NET_COMBOBOX;
			int nNonPaxIndex = pHost->GetItem(j);
			pNodeData = new TowNodeData();
			pNodeData->dwData = (DWORD)pHost;
			pNodeData->emNodeType = HOST_ITEM;
			pNodeData->nNonPaxIndex = nNonPaxIndex;
			HTREEITEM hNonItem = m_wndTreeCtrl.InsertItem(m_pInterm->m_pMobElemTypeStrDB->GetString(nNonPaxIndex),cni,FALSE,FALSE,hConItem,TVI_LAST);
			m_wndTreeCtrl.SetItemData(hNonItem,(DWORD_PTR)pNodeData);
		}

		m_wndTreeCtrl.Expand(hItem,TVE_EXPAND);
		m_wndTreeCtrl.Expand(hHostItem,TVE_EXPAND);
		m_wndTreeCtrl.Expand(hMobItem,TVE_EXPAND);
		m_wndTreeCtrl.Expand(hConItem,TVE_EXPAND);
	}
}


void CDlgHostSpec::OnBnClickedOk()
{
	OnBnClickedSave();
	OnOK();
}

void CDlgHostSpec::OnCancel()
{
	if(m_pTrayHostList)
	{
		//m_pTrayHostList->CleanData();
		m_pTrayHostList->ReadData(m_nProjID);
	}

	CXTResizeDialog::OnCancel();
}

void CDlgHostSpec::OnBnClickedSave()
{
	try
	{
		CADODatabase::BeginTransaction();
		m_pTrayHostList->SaveData(m_nProjID);
		CADODatabase::CommitTransaction();

	}
	catch (CADOException &e)
	{
		CADODatabase::RollBackTransation();
		e.ErrorMessage();
	}

	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(FALSE);
}

void CDlgHostSpec::OnTvnSelchangedTreeItem(NMHDR *pNMHDR, LRESULT *pResult)
{
	UpdateToolBar();
}
