// DlgAircraftModelEdit.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "DlgAircraftModelEdit.h"
#include <Common/ALTObjectID.h>
#include <inputOnboard/AircraftComponentModelDatabase.h>
#include "../Common/AirportDatabase.h"
#include <InputOnBoard/ComponentModel.h>
#include "../InputOnBoard/CInputOnboard.h"
#include "TermPlan.h"
#include "TermPlanDoc.h"
#include "MainFrm.h"
#include "../Common/WinMsg.h"


// CDlgAircraftModelEdit dialog
IMPLEMENT_DYNAMIC(CDlgAircraftModelEdit, CDialog)
CDlgAircraftModelEdit::CDlgAircraftModelEdit(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgAircraftModelEdit::IDD, pParent)
{
	m_pParent = pParent;
	m_bDragging = false;
	m_pDragImage = NULL;
}

CDlgAircraftModelEdit::~CDlgAircraftModelEdit()
{
}

void CDlgAircraftModelEdit::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_MODLE, m_wndTreeCtrl);
}


BEGIN_MESSAGE_MAP(CDlgAircraftModelEdit, CDialog)
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_MESSAGE(WM_SLB_SELCHANGED, OnSLBSelChanged)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_MODLE, OnSelchangedTree)
	ON_NOTIFY(TVN_BEGINDRAG, IDC_TREE_MODLE, OnRClick)
	ON_NOTIFY(NM_SETFOCUS, IDC_TREE_MODLE, OnSetfocusTree)
END_MESSAGE_MAP()


// CDlgAircraftModelEdit message handlers
BOOL CDlgAircraftModelEdit::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	LoadAircraftData();
	m_image.Create ( IDB_PROCESS,16,1,RGB(255,0,255) );
	CBitmap bm;
	bm.LoadBitmap(IDB_PROCESSES);
	m_image.Add(&bm,RGB(255,0,255));
	m_wndTreeCtrl.SetImageList ( &m_image,TVSIL_NORMAL );

	return TRUE;
}

void CDlgAircraftModelEdit::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	if (::IsWindow(m_wndTreeCtrl.m_hWnd))
		m_wndTreeCtrl.MoveWindow(0, 0, cx, cy, TRUE);
}

int CDlgAircraftModelEdit::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	//DWORD dwStyle = WS_CHILD | WS_VISIBLE |TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS | TVS_SHOWSELALWAYS | TVS_EDITLABELS;
	//BOOL bResult = m_wndTreeCtrl.Create(dwStyle,CRect(),this,IDC_MODLE_TREE);

	return  0;
}

void CDlgAircraftModelEdit::LoadAircraftData()
{
	m_wndTreeCtrl.DeleteAllItems();
	if (m_pInputOnboard == NULL)
	{
		return;
	}
	CAircraftComponentModelDatabase* pModelDB = m_pInputOnboard->GetAirportDB()->getACCompModelDB();

	ALTObjectIDList lstExistObjectName;
	for (int i = 0; i < pModelDB->GetModelCount(); i++)
	{
		ALTObjectID altObjID;
		CComponentMeshModel* pModel = pModelDB->GetModel(i);
		altObjID.FromString(pModel->m_sModelName);
		lstExistObjectName.push_back(altObjID);
	}
	SortedStringList levelName;
	lstExistObjectName.GetLevel1StringList(levelName);
	SortedStringList::iterator iter = levelName.begin();
	for (; iter != levelName.end(); ++iter)
	{
		/*COOLTREE_NODE_INFO cni;
		CARCBaseTree::InitNodeInfo(this,cni);*/
		HTREEITEM hItem = m_wndTreeCtrl.InsertItem(*iter,TVI_ROOT);

		SortedStringList level1Name;
		CString strLeve1Name = *iter;
		lstExistObjectName.GetLevel2StringList(*iter,level1Name);
		if ((int)level1Name.size() == 0)
		{
			m_wndTreeCtrl.SetItemData(hItem,(DWORD_PTR)pModelDB->getModelByName(ALTObjectID(strLeve1Name)));
		}
		SortedStringList::iterator iter1 = level1Name.begin();
		for (; iter1 != level1Name.end(); ++iter1)
		{
			HTREEITEM hlevel1Item = m_wndTreeCtrl.InsertItem(*iter1,hItem,TVI_LAST);

			SortedStringList level2Name;
			CString strLevel2Name = strLeve1Name + CString("-") + CString(*iter1);
			lstExistObjectName.GetLevel3StringList(*iter,*iter1,level2Name);
			if ((int)level2Name.size() == 0)
			{
				m_wndTreeCtrl.SetItemData(hlevel1Item,(DWORD_PTR)pModelDB->getModelByName(ALTObjectID(strLevel2Name)));
			}
			SortedStringList::iterator iter2 = level2Name.begin();
			for (; iter2 != level2Name.end(); ++iter2)
			{
				HTREEITEM hlevel2Item = m_wndTreeCtrl.InsertItem(*iter2,hlevel1Item,TVI_LAST);

				SortedStringList level3Name;
				CString strLevel3Name = strLevel2Name + CString("-") + CString(*iter2);
				lstExistObjectName.GetLevel4StringList(*iter,*iter1,*iter2,level3Name);
				if ((int)level3Name.size() == 0)
				{
					m_wndTreeCtrl.SetItemData(hlevel2Item,(DWORD_PTR)pModelDB->getModelByName(ALTObjectID(strLevel3Name)));
				}
				SortedStringList::iterator iter3 = level3Name.begin();
				for (; iter3 != level3Name.end(); ++iter3)
				{
					CString strLevel4Name = strLevel3Name + CString("-") + CString(*iter3);
					HTREEITEM hlevel3Item = m_wndTreeCtrl.InsertItem(*iter3,hlevel2Item,TVI_LAST);
					m_wndTreeCtrl.SetItemData(hlevel3Item,(DWORD_PTR)pModelDB->getModelByName(ALTObjectID(strLevel4Name)));
				}
			}
		}
	}

}

void CDlgAircraftModelEdit::SetInputOnboard(InputOnboard* pInputOnboard)
{
	ASSERT(pInputOnboard);
	m_pInputOnboard = pInputOnboard;
}

LRESULT CDlgAircraftModelEdit::OnSLBSelChanged(WPARAM wParam, LPARAM lParam)
{
	// TRACE("CShapesBar::OnSLBSelChanged()\n");
	//ASSERT(GetParent()->IsKindOf(RUNTIME_CLASS(CMDIFrameWnd)));
	CWnd* pWnd = GetParent();
	while(pWnd != NULL && !pWnd->IsKindOf(RUNTIME_CLASS(CMainFrame))) {
		pWnd = pWnd->GetParent();
	}
	if(pWnd == NULL) {
		TRACE0("Error: Could not get MainFrame to send message!\n");
		return 0;
	}
	pWnd->SendMessage(WM_SLB_SELCHANGED, wParam, lParam);
	return 0;
}

void CDlgAircraftModelEdit::OnRClick(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	*pResult = 0;

	m_hItemDragSrc = pNMTreeView->itemNew.hItem;
	if (m_wndTreeCtrl.GetChildItem(m_hItemDragSrc))
		return;

	if(m_pDragImage)
		delete m_pDragImage;

	CPoint ptTemp(0,0);
	m_pDragImage = m_wndTreeCtrl.CreateDragImage(m_hItemDragSrc);
	if( !m_pDragImage )
		return;

	m_bDragging = true;
	m_pDragImage->BeginDrag ( 0, CPoint(8,8) );
	CPoint  pt = pNMTreeView->ptDrag;
	ClientToScreen( &pt );
	m_pDragImage->DragEnter( GetDesktopWindow(), pt );  

	SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
	SetCapture();
}

void CDlgAircraftModelEdit::OnMouseMove(UINT nFlags, CPoint point)
{
	if( m_bDragging)
	{
		CPoint pt (point);
		ClientToScreen (&pt);

		// move the drag image
		VERIFY(m_pDragImage->DragMove(pt));
		// unlock window updates
		m_pDragImage->DragShowNolock(FALSE);
		//////////////////////////////////////////////////////
		CRect rcSM;
		GetWindowRect( &rcSM );
		if( rcSM.PtInRect( pt ) )	
		{
			SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
		}
		else
		{
			SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ICON));
		}
		m_pDragImage->DragShowNolock (TRUE);
		m_wndTreeCtrl.Invalidate(FALSE);
	}

	CDialog::OnMouseMove(nFlags, point);
}

void CDlgAircraftModelEdit::OnLButtonUp(UINT nFlags, CPoint point)
{
	if( m_bDragging )
	{
		CPoint pt (point);
		ClientToScreen (&pt);

		m_bDragging = false;
		ReleaseCapture();
		SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
		// end dragging
		m_pDragImage->DragLeave( GetDesktopWindow() );
		m_pDragImage->EndDrag();
		delete m_pDragImage;
		m_pDragImage = NULL;
		CWnd* pWnd = m_pParent;

		while(pWnd != NULL && !pWnd->IsKindOf(RUNTIME_CLASS(CMainFrame))) {
			pWnd = pWnd->GetParent();
		}
		if(pWnd == NULL) {
			TRACE0("Error: Could not get MainFrame to send message!\n");
			return ;
		}
		pWnd->SendMessage( WM_AIR_SELCHANGED, (WPARAM) m_wndTreeCtrl.GetItemData(m_hItemDragSrc), (LPARAM) NULL);
	}	
	CDialog::OnLButtonUp(nFlags, point);
}

void CDlgAircraftModelEdit::OnSelchangedTree(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	HTREEITEM hItem = m_wndTreeCtrl.GetSelectedItem();
	m_wndTreeCtrl.SetItemState(hItem,0,TVIS_SELECTED);

 	*pResult = 0;
}

void CDlgAircraftModelEdit::OnSetfocusTree(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	HTREEITEM hItem = m_wndTreeCtrl.GetSelectedItem();
	if(hItem)
	{
		//UINT nState=m_ProcTree.GetItemState(hTI,TVIS_SELECTED);
		m_wndTreeCtrl.SetItemState(hItem,0,TVIS_SELECTED);
	}
	*pResult = 0;
}