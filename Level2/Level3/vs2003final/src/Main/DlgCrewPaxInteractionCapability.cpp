// DlgCrewPaxInteractionCapability.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "DlgCrewPaxInteractionCapability.h"
#include ".\dlgcrewpaxinteractioncapability.h"
#include "../InputOnBoard/CrewPaxInteractionCapability.h"
#include "../InputOnBoard/CabinCrewGeneration.h"
#include "../Inputs/IN_TERM.H"
#include "../Database/ADODatabase.h"
#include "DlgNewPassengerType.h"

// CDlgCrewPaxInteractionCapability dialog

IMPLEMENT_DYNAMIC(CDlgCrewPaxInteractionCapability, CXTResizeDialog)
CDlgCrewPaxInteractionCapability::CDlgCrewPaxInteractionCapability(InputTerminal* pTerminal, CWnd* pParent /*=NULL*/)
	: CXTResizeDialog(CDlgCrewPaxInteractionCapability::IDD, pParent)
	,m_pInputTerminal(pTerminal)
{
	try
	{
		m_pCrewPaxInteractionCapabilitys = new CCrewPaxInteractionCapabilityList;
		m_pCrewPaxInteractionCapabilitys->ReadData(m_pInputTerminal->inStrDict);

		m_pCabinCrewGeneration = new CabinCrewGeneration;
		m_pCabinCrewGeneration->ReadData();
	}
	catch (CADOException* error)
	{
		CString strError = _T("");
		strError.Format("Database operation error: %s",error->ErrorMessage());
		MessageBox(strError);

		delete error;
		error = NULL;
		if (m_pCrewPaxInteractionCapabilitys)
		{
			delete m_pCrewPaxInteractionCapabilitys;
			m_pCrewPaxInteractionCapabilitys = NULL;
		}

		if (m_pCabinCrewGeneration)
		{
			delete m_pCabinCrewGeneration;
			m_pCabinCrewGeneration = NULL;
		}
		return;
	}
}

CDlgCrewPaxInteractionCapability::~CDlgCrewPaxInteractionCapability()
{
	delete m_pCrewPaxInteractionCapabilitys;
	m_pCrewPaxInteractionCapabilitys = NULL;

	delete m_pCabinCrewGeneration;
	m_pCabinCrewGeneration = NULL;
}

void CDlgCrewPaxInteractionCapability::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX,IDC_TREE_PAXCREWINTERACTION,m_wndTreeCtrl);
}


BEGIN_MESSAGE_MAP(CDlgCrewPaxInteractionCapability, CXTResizeDialog)
	ON_WM_CREATE()
	ON_COMMAND(ID_TOOLBARBUTTONADD,AddNewItem)
	ON_COMMAND(ID_TOOLBARBUTTONDEL,RemoveItem)
	ON_COMMAND(ID_TOOLBARBUTTONEDIT,EditItem)
	ON_NOTIFY(TVN_SELCHANGED,IDC_TREE_PAXCREWINTERACTION, OnSelchangedTree)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnBnClickedButtonSave)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgCrewPaxInteractionCapability message handlers

void CDlgCrewPaxInteractionCapability::OnBnClickedButtonSave()
{
	// TODO: Add your control notification handler code here
	try
	{
		CADODatabase::BeginTransaction() ;
		m_pCrewPaxInteractionCapabilitys->SaveData();
		CADODatabase::CommitTransaction() ;
		GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(FALSE);
	}
	catch (CADOException* error)
	{
		CADODatabase::RollBackTransation() ;
		CString strError = _T("");
		strError.Format("Database operation error: %s",error->ErrorMessage());
		MessageBox(strError);

		delete error;
		error = NULL;
		return;
	}
}

void CDlgCrewPaxInteractionCapability::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	OnBnClickedButtonSave();
	OnOK();
}

BOOL CDlgCrewPaxInteractionCapability::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog();

	CRect rect;
	GetDlgItem(IDC_STATIC_CREWPAXINTERACTION)->GetWindowRect(&rect);
	GetDlgItem(IDC_STATIC_CREWPAXINTERACTION)->ShowWindow(FALSE);
	ScreenToClient(&rect);
	m_wndToolbar.MoveWindow(&rect,true);
	m_wndToolbar.ShowWindow(SW_SHOW);

	m_wndToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONADD,TRUE);
	m_wndToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONDEL,FALSE);
	m_wndToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONEDIT,FALSE);
	OnInitTreeCtrl();

	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(FALSE);
	SetResize(m_wndToolbar.GetDlgCtrlID(),SZ_TOP_LEFT,SZ_TOP_LEFT);
	SetResize(IDC_TREE_PAXCREWINTERACTION,SZ_TOP_LEFT,SZ_BOTTOM_RIGHT);
	SetResize(IDC_BUTTON_SAVE,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);
	SetResize(IDOK,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);
	SetResize(IDCANCEL,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);

	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),TRUE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),FALSE);

	return TRUE;
}

int CDlgCrewPaxInteractionCapability::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CXTResizeDialog::OnCreate(lpCreateStruct) == -1)
	{
		return -1;
	}
	if (!m_wndToolbar.CreateEx(this, TBSTYLE_FLAT,WS_CHILD|WS_VISIBLE|CBRS_ALIGN_TOP|CBRS_TOOLTIPS,CRect(0,0,0,0),IDR_ADDDELEDITTOOLBAR)||
		!m_wndToolbar.LoadToolBar(IDR_ADDDELEDITTOOLBAR))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	return 0;
}

LRESULT CDlgCrewPaxInteractionCapability::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if(message == UM_CEW_COMBOBOX_BEGIN)
	{
		CWnd* pWnd = m_wndTreeCtrl.GetEditWnd((HTREEITEM)wParam);
		CRect rectWnd;
		HTREEITEM hItem = (HTREEITEM)wParam;
		m_wndTreeCtrl.GetItemRect((HTREEITEM)wParam,rectWnd,TRUE);
		pWnd->SetWindowPos(NULL,rectWnd.right,rectWnd.top,0,0,SWP_NOSIZE);
		CComboBox* pCB=(CComboBox*)pWnd;
		if (pCB->GetCount() != 0)
			pCB->ResetContent();

		if (pCB->GetCount() == 0)
		{
			HTREEITEM hParentItem = m_wndTreeCtrl.GetParentItem(hItem);
			if (hParentItem)
			{
				HTREEITEM hParParentItem = m_wndTreeCtrl.GetParentItem(hParentItem);
				if (hParParentItem != NULL)		//pax type set	
					return 0;
				//crew set
				CString strRank = m_wndTreeCtrl.GetItemText(hParentItem);
				std::vector<CabinCrewPhysicalCharacteristics*> vCrewList;
				m_pCabinCrewGeneration->GetCrewListByRank(strRank, vCrewList);

				ASSERT(!vCrewList.empty());

				int nCount = vCrewList.size();
				for (int i =0; i < nCount; i++)
				{
					CabinCrewPhysicalCharacteristics* pData = vCrewList.at(i);
					ASSERT(pData != NULL);

					CString strCrew= "";
					CString strSex = "Female";
					if (pData->getSex() == Male)
						strSex = "Male";
					strCrew.Format("ID %d (%s)",pData->getID(),strSex);
					int nIdx = pCB->AddString(strCrew);
					pCB->SetItemData(nIdx, pData->getIdx());
				}
				
			}
			else		//rank set
			{
				CStringList strRankList;
				m_pCabinCrewGeneration->GetRankList(strRankList);
				if (strRankList.IsEmpty())
				{
					MessageBox("There is no crew defined in Cabin Crew Generation!");
					return 0;
				}
				
				 int nCount = strRankList.GetCount();
				 POSITION pos;
				 for (int i =0; i < nCount; i++)
				 {
					 if ((pos = strRankList.FindIndex(i)) != NULL)
					 {
						 pCB->AddString(strRankList.GetAt(pos));
					 }
				 }


			}
			pCB->SetCurSel(0);
		}
	}
	if (message == UM_CEW_COMBOBOX_SELCHANGE)
	{
		CWnd* pWnd = m_wndTreeCtrl.GetEditWnd((HTREEITEM)wParam);
		CComboBox* pCB = (CComboBox*)pWnd;
		HTREEITEM hItem = (HTREEITEM)wParam;
		HTREEITEM hParentItem = m_wndTreeCtrl.GetParentItem(hItem);
		if (hParentItem)		//Crew setting
		{
			CString strText =  *(CString*)lParam;
			CString strTextOld = m_wndTreeCtrl.GetItemText(hItem);
			if(strText == strTextOld)
				return 0;
			m_wndTreeCtrl.SetItemText(hItem,strText);

			CCrewPaxInteractions* pData = (CCrewPaxInteractions*)m_wndTreeCtrl.GetItemData(hItem);
			int nSel = pCB->GetCurSel();
			int nCrewIdx = (int)pCB->GetItemData(nSel);
			pData->setCrewIdx(nCrewIdx);
		}
		else			//rank setting
		{
			CString strText =  *(CString*)lParam;
			CString strTextOld = m_wndTreeCtrl.GetItemText(hItem);
			if(strText == strTextOld)
				return 0;
			int nItemCount = m_pCrewPaxInteractionCapabilitys->getDataCount();
			for (int i =0; i < nItemCount; i++)
			{
				if (strText == m_pCrewPaxInteractionCapabilitys->getData(i)->getRank())
				{
					MessageBox("The Rank has been defined!");
					return 0;
				}
			}
			m_wndTreeCtrl.SetItemText(hItem,strText);

			CCrewPaxInteractionCapability* pData = (CCrewPaxInteractionCapability*)m_wndTreeCtrl.GetItemData(hItem);
			pData->setRank(strText);	
		}
		GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
	}

	return CXTResizeDialog::DefWindowProc(message, wParam, lParam);
}

void CDlgCrewPaxInteractionCapability::OnSelchangedTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	m_hRclickItem = m_wndTreeCtrl.GetSelectedItem();

	if (m_hRclickItem == NULL)
	{
		m_wndToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONADD,TRUE);
		m_wndToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONDEL,FALSE);
	}
	else 
	{
		HTREEITEM hParItem = m_wndTreeCtrl.GetParentItem(m_hRclickItem);

		HTREEITEM hGrandItem= NULL;
		if (hParItem)
			hGrandItem = m_wndTreeCtrl.GetParentItem(hParItem);

		if (hGrandItem == NULL || hParItem == NULL)
		{
			m_wndToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONADD,TRUE);
			m_wndToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONDEL,TRUE);
		}
		else
		{
			m_wndToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONADD,FALSE);
			m_wndToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONDEL,TRUE);
		}

	}
}

void CDlgCrewPaxInteractionCapability::OnInitTreeCtrl()
{
	m_wndTreeCtrl.DeleteAllItems();
	m_wndTreeCtrl.SetImageList(m_wndTreeCtrl.GetImageList(TVSIL_NORMAL),TVSIL_NORMAL);
	HTREEITEM hItem;
	HTREEITEM hChildItem;
	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);
	int nCount =  m_pCrewPaxInteractionCapabilitys->getDataCount();
	for(int i = 0; i < nCount; i++)
	{
		CCrewPaxInteractionCapability* pData = m_pCrewPaxInteractionCapabilitys->getData(i);
		std::vector<CabinCrewPhysicalCharacteristics*> vCrewList;
		m_pCabinCrewGeneration->GetCrewListByRank(pData->getRank(), vCrewList);
		if (vCrewList.empty())
		{
			m_pCrewPaxInteractionCapabilitys->DelData(pData);
			nCount--;
			i--;
			continue;
		}
		cni.nt = NT_NORMAL;
		cni.net = NET_COMBOBOX;
		hItem = m_wndTreeCtrl.InsertItem(pData->getRank(),cni,FALSE,FALSE,TVI_ROOT,TVI_LAST);
		m_wndTreeCtrl.SetItemData(hItem,(DWORD_PTR)pData);

		int nCrewCount = pData->getDataCount();
		for (int j = 0; j < nCrewCount; j++)
		{
			CCrewPaxInteractions* pCrewData = pData->getData(j);
			cni.nt = NT_NORMAL;
			cni.net = NET_COMBOBOX;

			CabinCrewPhysicalCharacteristics* pCabinCrew = m_pCabinCrewGeneration->getDataByCrewIdx(pCrewData->getCrewIdx());
			ASSERT(pCabinCrew);		//the data cannot be null
			CString strSex = "Female";
			if (pCabinCrew->getSex() == Male)
				strSex = "Male";

			CString strCrew;
			strCrew.Format("ID %d (%s)",pCabinCrew->getID(),strSex);
			hChildItem = m_wndTreeCtrl.InsertItem(strCrew,cni,FALSE,FALSE,hItem,TVI_LAST);
			m_wndTreeCtrl.SetItemData(hChildItem,(DWORD_PTR)pCrewData);

			int nPaxCount = pCrewData->getPaxTypeCount();
			for (int k =0; k < nPaxCount; k ++)
			{
				CPassengerType* pPaxType = pCrewData->getPaxType(k);
				cni.nt = NT_NORMAL;
				cni.net = NET_NORMAL;
				HTREEITEM hPaxType = m_wndTreeCtrl.InsertItem(pPaxType->PrintStringForShow(),cni, FALSE,FALSE,hChildItem,TVI_LAST);
				m_wndTreeCtrl.SetItemData(hPaxType,(DWORD_PTR)pPaxType);
			}

			m_wndTreeCtrl.Expand(hChildItem,TVE_EXPAND);
			
		}
		m_wndTreeCtrl.Expand(hItem,TVE_EXPAND);
	}
}

void CDlgCrewPaxInteractionCapability::AddNewItem()
{
	if (m_pCabinCrewGeneration->getDataCount() ==0)
	{
		MessageBox("There is no crew defined in Cabin Crew Generation!");
		return;
	}
	HTREEITEM hSel = m_wndTreeCtrl.GetSelectedItem();
	if (hSel == NULL)
	{
		return AddRankItem();
	}
	HTREEITEM hParentItem = m_wndTreeCtrl.GetParentItem(hSel);
	if (hParentItem == NULL)
	{
		AddCrewItem();
	}
	else
	{
		AddPaxType();
	}

}

void CDlgCrewPaxInteractionCapability::RemoveItem()
{
	HTREEITEM hSel = m_wndTreeCtrl.GetSelectedItem();
	if (hSel == NULL)
		return;

	HTREEITEM hParentItem = m_wndTreeCtrl.GetParentItem(hSel);
	if (hParentItem == NULL)
	{
		return RemoveRankItem();
	}
	else
	{
		HTREEITEM hGrandItem = m_wndTreeCtrl.GetParentItem(hParentItem);
		if (hGrandItem == NULL)
		{
			RemoveCrewItem();
		}
		else
		{
			RemovePaxType();
		}
	}
}

void CDlgCrewPaxInteractionCapability::EditItem()
{
	HTREEITEM hSel = m_wndTreeCtrl.GetSelectedItem();
	if (hSel == NULL)
		return;

}

void CDlgCrewPaxInteractionCapability::AddRankItem()
{
	CCrewPaxInteractionCapability* pRankItem = new CCrewPaxInteractionCapability;

	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);
	cni.nt=NT_NORMAL;
	cni.net=NET_COMBOBOX;
	cni.bAutoSetItemText= false;
	CString strObjName = "Please Select";

	m_wndTreeCtrl.SetRedraw(FALSE);
	HTREEITEM hRankItem = m_wndTreeCtrl.InsertItem(strObjName,cni,FALSE);
	m_wndTreeCtrl.SetRedraw(TRUE);

	m_wndTreeCtrl.SetItemData(hRankItem,(DWORD_PTR)pRankItem);
	m_pCrewPaxInteractionCapabilitys->AddData(pRankItem);
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
	m_wndTreeCtrl.DoEdit(hRankItem);
}

void CDlgCrewPaxInteractionCapability::RemoveRankItem()
{
	HTREEITEM hSel = m_wndTreeCtrl.GetSelectedItem();
	if(hSel != NULL)
	{
		CCrewPaxInteractionCapability* pRankItem = (CCrewPaxInteractionCapability*)m_wndTreeCtrl.GetItemData(hSel);
		m_pCrewPaxInteractionCapabilitys->DelData(pRankItem);
		m_wndTreeCtrl.DeleteItem(hSel);
		GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
	}	
}

void CDlgCrewPaxInteractionCapability::AddCrewItem()
{
	HTREEITEM hSel = m_wndTreeCtrl.GetSelectedItem();

	CCrewPaxInteractionCapability* pRankItem = (CCrewPaxInteractionCapability*)m_wndTreeCtrl.GetItemData(hSel);

	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);
	cni.nt=NT_NORMAL;
	cni.net=NET_COMBOBOX;
	cni.bAutoSetItemText= false;
	CString strObjName = "Please Select";

	m_wndTreeCtrl.SetRedraw(FALSE);
	HTREEITEM hCrewItem = m_wndTreeCtrl.InsertItem(strObjName,cni,FALSE,FALSE,hSel);
	m_wndTreeCtrl.SetRedraw(TRUE);
	CCrewPaxInteractions* pData = new CCrewPaxInteractions;
	m_wndTreeCtrl.SetItemData(hCrewItem,(DWORD_PTR)pData);
	pRankItem->AddData(pData);
	m_wndTreeCtrl.Expand(hSel,TVE_EXPAND);
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
	m_wndTreeCtrl.DoEdit(hCrewItem);
}

void CDlgCrewPaxInteractionCapability::RemoveCrewItem()
{
	HTREEITEM hSel = m_wndTreeCtrl.GetSelectedItem();
	if(hSel != NULL)
	{
		CCrewPaxInteractions* pCrewItem = (CCrewPaxInteractions*)m_wndTreeCtrl.GetItemData(hSel);
		HTREEITEM hParSel = m_wndTreeCtrl.GetParentItem(hSel);
		CCrewPaxInteractionCapability* pRankItem = (CCrewPaxInteractionCapability*)m_wndTreeCtrl.GetItemData(hParSel);
		pRankItem->DelData(pCrewItem);
		m_wndTreeCtrl.DeleteItem(hSel);
		GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
	}	
}

void CDlgCrewPaxInteractionCapability::AddPaxType()
{
	HTREEITEM hSel = m_wndTreeCtrl.GetSelectedItem();
	CCrewPaxInteractions* pCrewItem = (CCrewPaxInteractions*)m_wndTreeCtrl.GetItemData(hSel);

	CPassengerType* pPaxType = new CPassengerType(m_pInputTerminal->inStrDict);
	CDlgNewPassengerType dlg(pPaxType,m_pInputTerminal);
	dlg.DoModal();

	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);
	cni.nt=NT_NORMAL;
	cni.net=NET_NORMAL;
	cni.bAutoSetItemText= false;
	CString strObjName = pPaxType->PrintStringForShow();

	pCrewItem->AddData(pPaxType);
	m_wndTreeCtrl.SetRedraw(FALSE);
	HTREEITEM hPaxItem = m_wndTreeCtrl.InsertItem(strObjName,cni,FALSE,FALSE,hSel);
	m_wndTreeCtrl.SetRedraw(TRUE);
	m_wndTreeCtrl.SetItemData(hPaxItem,(DWORD_PTR)pPaxType);
	m_wndTreeCtrl.Expand(hSel,TVE_EXPAND);
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
}
void CDlgCrewPaxInteractionCapability::RemovePaxType()
{
	HTREEITEM hSel = m_wndTreeCtrl.GetSelectedItem();
	if(hSel != NULL)
	{
		CPassengerType* pPaxItem = (CPassengerType*)m_wndTreeCtrl.GetItemData(hSel);
		HTREEITEM hParSel = m_wndTreeCtrl.GetParentItem(hSel);
		CCrewPaxInteractions* pCrewItem = (CCrewPaxInteractions*)m_wndTreeCtrl.GetItemData(hParSel);
		pCrewItem->DelData(pPaxItem);
		m_wndTreeCtrl.DeleteItem(hSel);
		GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
	}	
}