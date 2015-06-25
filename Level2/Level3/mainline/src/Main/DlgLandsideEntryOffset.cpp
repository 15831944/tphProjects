// DlgLandsideEntryOffset.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "DlgLandsideEntryOffset.h"
#include ".\dlglandsideentryoffset.h"
#include "../Landside/LandsideEntryOffsetData.h"
#include "TreeCtrlItemInDlg.h"
#include "DlgSelectLandsideObject.h"
#include "landside/InputLandside.h"
#include "PassengerTypeDialog.h"
#include "DlgTimeRange.h"
#include "../common/WinMsg.h"
#include "../common/ProbDistEntry.h"
#include "../Inputs/PROBAB.H"
#include "../common/ProbDistManager.h"

using namespace LandsideEntryOffset;

#define ID_LANDENTRYOFFSET_ADDENTRY ID_ACTYPE_ALIAS_ADD
#define ID_LANDENTRYOFFSET_DELETE ID_ACTYPE_ALIAS_DEL
#define ID_LANDENTRYOFFSET_EDIT ID_ACTYPE_ALIAS_EDIT

#define ID_LANDENTRYOFFSET_ADDTIME ID_LANDENTRYOFFSET_EDIT+2
#define ID_LANDENTRYOFFSET_ADDPAX ID_LANDENTRYOFFSET_EDIT+3
// CDlgLandsideEntryOffset dialog

IMPLEMENT_DYNAMIC(CDlgLandsideEntryOffset, CDialog)
CDlgLandsideEntryOffset::CDlgLandsideEntryOffset(InputLandside* pInput, CWnd* pParent /*=NULL*/)
	: CXTResizeDialog(CDlgLandsideEntryOffset::IDD, pParent)
{
	m_pInputLandside = pInput;
	m_pOffsetEntries = new LandsideEntryOffset::OffsetEntries();
	m_pOffsetEntries->ReadData(pInput->getInputTerminal());
}

CDlgLandsideEntryOffset::~CDlgLandsideEntryOffset()
{
	delete m_pOffsetEntries;
}

void CDlgLandsideEntryOffset::DoDataExchange(CDataExchange* pDX)
{
	CXTResizeDialog::DoDataExchange(pDX);

	DDX_Control(pDX,IDC_TREE_ENTRYOFFSET,m_ctrlTree);
}


BEGIN_MESSAGE_MAP(CDlgLandsideEntryOffset, CXTResizeDialog)
	ON_COMMAND(ID_ACTYPE_ALIAS_ADD, OnNewItem)
	ON_COMMAND(ID_ACTYPE_ALIAS_DEL, OnDelItem)
	ON_COMMAND(ID_ACTYPE_ALIAS_EDIT, OnEditItem)


	ON_COMMAND(ID_LANDENTRYOFFSET_ADDPAX, OnNewPaxType)
	ON_COMMAND(ID_LANDENTRYOFFSET_ADDTIME, OnNewTimeWindow)


	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_ENTRYOFFSET, OnTvnSelchangedTree)
	ON_WM_CONTEXTMENU()
	ON_MESSAGE(WM_OBJECTTREE_DOUBLECLICK, OnPropTreeDoubleClick)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnBnClickedButtonSave)
END_MESSAGE_MAP()


// CDlgLandsideEntryOffset message handlers

BOOL CDlgLandsideEntryOffset::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	if(!m_wndToolBar.CreateEx(this,TBSTYLE_FLAT,WS_CHILD|WS_VISIBLE|CBRS_ALIGN_TOP)||
		!m_wndToolBar.LoadToolBar(IDR_ACTYPE_ALIASNAME))
	{
		TRACE(_T("Create tool bar error "));
		return FALSE;
	}

	//InitToolBar();
	CRect rcDlg;
	GetWindowRect(&rcDlg);
	ScreenToClient(&rcDlg);

	CRect rcToolbar(14, 22,rcDlg.right-10,45);
	m_wndToolBar.MoveWindow(rcToolbar);
	m_wndToolBar.GetToolBarCtrl().EnableButton(ID_ACTYPE_ALIAS_ADD,TRUE);
	m_wndToolBar.GetToolBarCtrl().EnableButton(ID_ACTYPE_ALIAS_DEL,FALSE);
	m_wndToolBar.GetToolBarCtrl().EnableButton(ID_ACTYPE_ALIAS_EDIT,FALSE);
	m_wndToolBar.ShowWindow(SW_SHOW);
	//InitListCtrl();
	LoadTree();

	//resize
	SetResize(m_wndToolBar.GetDlgCtrlID(), SZ_TOP_LEFT, SZ_TOP_LEFT);
	SetResize(IDC_TREE_ENTRYOFFSET, SZ_TOP_LEFT , SZ_BOTTOM_RIGHT);
	SetResize(IDC_STATIC, SZ_TOP_LEFT , SZ_BOTTOM_RIGHT);	
	SetResize(IDC_BUTTON_SAVE, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDOK, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDCANCEL, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);

	OnDataChange(FALSE);

	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),TRUE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}




;

class EntryOffsetTreeItem : public TreeCtrlItemInDlg
{
public:
	DLG_TREEIREM_CONVERSION_DEF(EntryOffsetTreeItem,TreeCtrlItemInDlg)

	enum DataType{
		_Distribution,
		_TimeWindow,
		_PaxType,
		_Entry,
	};

	void LoadEntry(OffsetEntry* ptr){
		SetItem(ptr);
		//load child 
		for(int i=0;i<ptr->m_paxTypelist.GetItemCount();i++){
			EntryOffsetTreeItem paxItem = AddChild();
			paxItem.LoadPaxType(ptr->m_paxTypelist.GetItem(i));
		}
		Expand();
	}
	void SetItem(OffsetEntry* ptr)
	{		
		AirsideObjectTreeCtrlItemDataEx* data = GetInitData();
		data->nSubType = _Entry;
		data->dwptrItemData= (DWORD_PTR)ptr;		
		SetStringText("Entry",ptr->GetEntryName());
	}
	


	void LoadTimeWindow(OffsetTimeWindow* ptr){
		SetItem(ptr);
		//
		EntryOffsetTreeItem distItem = AddChild();
		distItem.SetDistItem(ptr);		
		Expand();
	}
	void SetItem(OffsetTimeWindow* ptr){
		AirsideObjectTreeCtrlItemDataEx* data = GetInitData();
		data->nSubType = _TimeWindow;
		data->dwptrItemData= (DWORD_PTR)ptr;
		SetStringText("Time Range",ptr->m_timeRange.ScreenPrint());
	}
	void SetDistItem(OffsetTimeWindow* ptr){
		SetStringText("Distribution(min)", ptr->m_distribute.GetProbDistribution()->screenPrint());
		AirsideObjectTreeCtrlItemDataEx* data = GetInitData();
		data->nSubType = _Distribution;
		data->dwptrItemData = (DWORD_PTR)ptr;
	}

	//

	void LoadPaxType(OffsetPaxType* ptr){
		SetItem(ptr);
		for(int i=0;i<ptr->m_timeList.GetItemCount();i++){
			EntryOffsetTreeItem timeItem  = AddChild();
			timeItem.LoadTimeWindow(ptr->m_timeList.GetItem(i));
		}
		Expand();
	}
	void SetItem(OffsetPaxType* ptr){
		AirsideObjectTreeCtrlItemDataEx* data = GetInitData();
		data->nSubType = _PaxType;
		data->dwptrItemData= (DWORD_PTR)ptr;
		if(CMobileElemConstraint* pPaxType = ptr->getPaxType()){
			CString sT;
			pPaxType->screenPrint(sT);
			SetStringText("Pax Type",sT);
		}
		else 
			SetStringText("Pax Type", "Undefined");
	}

	OffsetEntry* isEntryItem(){
		if(AirsideObjectTreeCtrlItemDataEx* data = GetData()){
			if(data->nSubType == _Entry){
				return  (OffsetEntry*)data->dwptrItemData;
			}
		}
		return NULL;
	}

	OffsetTimeWindow* isTimeItem(){
		if(AirsideObjectTreeCtrlItemDataEx* data = GetData()){
			if(data->nSubType == _TimeWindow){
				return  (OffsetTimeWindow*)data->dwptrItemData;
			}
		}
		return NULL;
	}
	OffsetTimeWindow* isDistItem(){
		if(AirsideObjectTreeCtrlItemDataEx* data = GetData()){
			if(data->nSubType == _Distribution){
				return  (OffsetTimeWindow*)data->dwptrItemData;
			}
		}
		return NULL;
	}
	
	OffsetPaxType* isPaxItem(){
		if(AirsideObjectTreeCtrlItemDataEx* data = GetData()){
			if(data->nSubType == _PaxType){
				return  (OffsetPaxType*)data->dwptrItemData;
			}
		}
		return NULL;
	}

};



void CDlgLandsideEntryOffset::LoadTree()
{
	m_ctrlTree.DeleteAllItems();
	
	
	TreeCtrlItemInDlg rootItem = TreeCtrlItemInDlg::GetRootItem(m_ctrlTree);
	OffsetEntries* pEntrylist= getEntryList();
	for(int i=0;i<pEntrylist->GetItemCount();i++)
	{
		OffsetEntry* pEntry = pEntrylist->GetItem(i);
		EntryOffsetTreeItem dataItem = rootItem.AddChild();
		dataItem.LoadEntry(pEntry);	
	}
}


void CDlgLandsideEntryOffset::OnDelItem()
{
	HTREEITEM hItem = m_ctrlTree.GetSelectedItem();
	EntryOffsetTreeItem item(m_ctrlTree,hItem);

	bool bDelItem = false;
	OffsetEntries* pEntrylist= getEntryList();

	if(OffsetEntry* pentry = item.isEntryItem()){
		bDelItem = pEntrylist->DeleteItem(pentry);
	}
	if(OffsetPaxType* pPax = item.isPaxItem()){
		EntryOffsetTreeItem pitem  = item.GetParent();
		if(OffsetEntry* pentry = pitem.isEntryItem()){
			bDelItem = pentry->m_paxTypelist.DeleteItem(pPax);			
		}
	}
	if(OffsetTimeWindow* ptime = item.isTimeItem())
	{
		EntryOffsetTreeItem pitem = item.GetParent();
		if(OffsetPaxType* pPax = pitem.isPaxItem()){
			bDelItem = pPax->m_timeList.DeleteItem(ptime);
		}
	}
	if(bDelItem){
		OnDataChange(TRUE);
		item.Destroy();
	}
	
}


void CDlgLandsideEntryOffset::OnNewItem()
{

	//pop up the select object dlg
	CDlgSelectLandsideObject dlg(&m_pInputLandside->getObjectList(),this);
	dlg.AddObjType(ALT_LEXT_NODE);
	if(dlg.DoModal()!=IDOK)
		return;
	//

	ALTObjectID entryId= dlg.getSelectObject();
	
	OffsetEntry* pEntry = new OffsetEntry();
	pEntry->m_entryid = entryId;
	
	OffsetEntries* pEntrylist= getEntryList();
	pEntrylist->AddItem(pEntry);

	OnDataChange(TRUE);

	//TreeCtrlItemInDlg rootItem = TreeCtrlItemInDlg::GetRootItem(m_ctrlTree);
	HTREEITEM hItem = m_ctrlTree.InsertItem("");
	EntryOffsetTreeItem entryItem(m_ctrlTree,hItem);
	entryItem.LoadEntry(pEntry);
	entryItem.SetSelect();
}



void CDlgLandsideEntryOffset::EditItem(HTREEITEM hItem)
{
	//HTREEITEM hItem = m_ctrlTree.GetSelectedItem();
	EntryOffsetTreeItem item(m_ctrlTree,hItem);

	//Edit entry 
	if(OffsetEntry* pEntry = item.isEntryItem()){
		//edit entry 
		CDlgSelectLandsideObject dlg(&m_pInputLandside->getObjectList(),this);
		dlg.AddObjType(ALT_LEXT_NODE);
		if(dlg.DoModal()!=IDOK)
			return;
		//

		ALTObjectID entryId= dlg.getSelectObject();
		pEntry->m_entryid = entryId;
		//update item
		item.SetItem(pEntry);	
		OnDataChange(TRUE);

	}
	//edit pax type
	if(OffsetPaxType* pPax = item.isPaxItem()){
		//edit pax type
		CPassengerTypeDialog dlg( this ,FALSE, TRUE );

		if( dlg.DoModal() == IDOK ){
			pPax->setPaxType(dlg.GetMobileSelection());			
			item.SetItem(pPax);
			OnDataChange(TRUE);

		}
	}
	//edit time window
	if(OffsetTimeWindow *pTime = item.isTimeItem()){
		//edit time
		CDlgTimeRange dlg(pTime->m_timeRange.GetStartTime(),pTime->m_timeRange.GetEndTime(),FALSE, this);
		if(dlg.DoModal() == IDOK){
			pTime->m_timeRange.SetStartTime(dlg.GetStartTime());
			pTime->m_timeRange.SetEndTime(dlg.GetEndTime());

			item.SetItem(pTime);
			OnDataChange(TRUE);

		}
	}
	//edit distribution
	if(OffsetTimeWindow* pTime = item.isDistItem()){
		InputTerminal* pInterm = m_pInputLandside->getInputTerminal();
		m_ctrlTree.EditProbabilityDistribution(pInterm,item.m_hItem,false);
		OnDataChange(TRUE);

	}
}

void CDlgLandsideEntryOffset::OnEditItem()
{
	EditItem(m_ctrlTree.GetSelectedItem());
}

void CDlgLandsideEntryOffset::OnNewPaxType()
{
	HTREEITEM hItem = m_ctrlTree.GetSelectedItem();
	EntryOffsetTreeItem item(m_ctrlTree,hItem);

	if(OffsetEntry* pEntry = item.isEntryItem()){
		//
		CPassengerTypeDialog dlg( this ,FALSE, TRUE );

		if( dlg.DoModal() == IDOK ){
			OffsetPaxType * pPax = new OffsetPaxType();
			pPax->setPaxType(dlg.GetMobileSelection());
			pEntry->m_paxTypelist.AddItem(pPax);
			EntryOffsetTreeItem paxItem = item.AddChild();
			paxItem.LoadPaxType(pPax);			
			paxItem.SetSelect();
			OnDataChange(TRUE);

		}
	}

}

void CDlgLandsideEntryOffset::OnNewTimeWindow()
{
	HTREEITEM hItem = m_ctrlTree.GetSelectedItem();
	EntryOffsetTreeItem item(m_ctrlTree,hItem);
	if(OffsetPaxType* pPax = item.isPaxItem()){
		CDlgTimeRange dlg(ElapsedTime(),ElapsedTime(), FALSE, this);
		if(dlg.DoModal() == IDOK){
			OffsetTimeWindow* pTime = new OffsetTimeWindow();
			pTime->m_timeRange.SetStartTime(dlg.GetStartTime());
			pTime->m_timeRange.SetEndTime(dlg.GetEndTime());

			pPax->m_timeList.AddItem(pTime);
			EntryOffsetTreeItem timeItem = item.AddChild();
			timeItem.LoadTimeWindow(pTime);
			timeItem.SetSelect();
			
		}
	}


}

LandsideEntryOffset::OffsetEntries* CDlgLandsideEntryOffset::getEntryList()
{
	return m_pOffsetEntries;
}


void CDlgLandsideEntryOffset::OnTvnSelchangedTree( NMHDR *pNMHDR, LRESULT *pResult )
{
	UpdateToolBar();
}

void CDlgLandsideEntryOffset::OnContextMenu( CWnd* pWnd, CPoint point )
{
	CRect rectTree;
	m_ctrlTree.GetWindowRect(&rectTree);
	if (!rectTree.PtInRect(point)) 
		return;

	m_ctrlTree.SetFocus();
	CPoint pt = point;
	m_ctrlTree.ScreenToClient(&pt);
	UINT iRet;
	HTREEITEM hRClickItem = m_ctrlTree.HitTest(pt, &iRet);
	if (iRet != TVHT_ONITEMLABEL)
	{
		hRClickItem = NULL;
		return;
	}

	m_ctrlTree.SelectItem(hRClickItem);


	EntryOffsetTreeItem item(m_ctrlTree,hRClickItem);

	CMenu menuPopup; 
	menuPopup.CreatePopupMenu();
	if(item.isPaxItem()){
		menuPopup.AppendMenu(MF_POPUP,ID_LANDENTRYOFFSET_ADDTIME,_T("Select Time Window"));
		menuPopup.AppendMenu(MF_POPUP,ID_LANDENTRYOFFSET_EDIT,_T("Edit Pax"));
		menuPopup.AppendMenu(MF_POPUP,ID_LANDENTRYOFFSET_DELETE, _T("Delete Pax"));
		menuPopup.TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);

	}
	if(item.isTimeItem()){
		menuPopup.AppendMenu(MF_POPUP,ID_LANDENTRYOFFSET_EDIT,_T("Edit Time Window"));
		menuPopup.AppendMenu(MF_POPUP,ID_LANDENTRYOFFSET_DELETE, _T("Delete Time Window"));
		menuPopup.TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);

	}
	if(item.isEntryItem()){
		menuPopup.AppendMenu(MF_POPUP,ID_LANDENTRYOFFSET_ADDPAX,_T("Add Pax Type"));
		menuPopup.AppendMenu(MF_POPUP,ID_LANDENTRYOFFSET_EDIT,_T("Edit Entry"));
		menuPopup.AppendMenu(MF_POPUP,ID_LANDENTRYOFFSET_DELETE, _T("Delete Entry"));
		menuPopup.TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);

	}
	menuPopup.DestroyMenu();

}

void CDlgLandsideEntryOffset::UpdateToolBar()
{
	EntryOffsetTreeItem item(m_ctrlTree, m_ctrlTree.GetSelectedItem());

	m_wndToolBar.GetToolBarCtrl().EnableButton(ID_ACTYPE_ALIAS_ADD,TRUE);
	m_wndToolBar.GetToolBarCtrl().EnableButton(ID_ACTYPE_ALIAS_DEL,FALSE);
	m_wndToolBar.GetToolBarCtrl().EnableButton(ID_ACTYPE_ALIAS_EDIT,FALSE);

	if(item.isPaxItem() || item.isEntryItem() || item.isTimeItem() ){
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_ACTYPE_ALIAS_DEL,TRUE);
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_ACTYPE_ALIAS_EDIT,TRUE);
	}
	if(item.isDistItem()){
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_ACTYPE_ALIAS_EDIT,TRUE);
	}
}


LRESULT CDlgLandsideEntryOffset::OnPropTreeDoubleClick( WPARAM, LPARAM lparam)
{
	OnEditItem();
	return TRUE;
}

LRESULT CDlgLandsideEntryOffset::DefWindowProc( UINT message, WPARAM wParam, LPARAM lParam )
{
	// TODO: Add your specialized code here and/or call the base class
	if( message == WM_INPLACE_COMBO )
	{
		EntryOffsetTreeItem item(m_ctrlTree, m_ctrlTree.GetSelectedItem());
		if(OffsetTimeWindow* pTime = item.isDistItem()){
			int nSelIdx = (int) lParam;
			CProbDistEntry* pPDEntry = _g_GetActiveProbMan( m_pInputLandside->getInputTerminal() )->getItem( nSelIdx );
			if(pPDEntry)
			{				
				pTime->setDistribution( pPDEntry );	
				item.SetDistItem(pTime);
				OnDataChange(TRUE);

			}
		}

		
		
		return TRUE;
	}
	return CDialog::DefWindowProc(message, wParam, lParam);
}



void CDlgLandsideEntryOffset::SaveData()
{
	if(m_bDataChanged){
		getEntryList()->SaveData(-1);
		OnDataChange(FALSE);

	}	
}

void CDlgLandsideEntryOffset::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class
	SaveData();
		
	CXTResizeDialog::OnOK();
}

void CDlgLandsideEntryOffset::OnBnClickedButtonSave()
{
	SaveData();
	// TODO: Add your control notification handler code here
}

void CDlgLandsideEntryOffset::OnDataChange(BOOL b)
{
	m_bDataChanged = b;
	//update gui
	if(CWnd* pwnd = GetDlgItem(IDC_BUTTON_SAVE)){
		pwnd->EnableWindow(b);	
	}
}
