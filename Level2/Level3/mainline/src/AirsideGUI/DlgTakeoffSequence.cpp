// DlgTakeoffSequence.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "DlgTakeoffSequence.h"
#include ".\dlgtakeoffsequence.h"
#include "DlgTimeRange.h"
#include "common\TimeRange.h"

#include "DlgTakeOffPositionSelection.h"
#include "DlgChooseTakeoffPosition.h"
#include "..\Common\AirportDatabase.h"

static const UINT ID_NEW_SEQUENCE = 10;
static const UINT ID_DEL_SEQUENCE = 11;
static const UINT ID_EDIT_SEQUENCE = 12;
// CDlgTakeoffSequence dialog

IMPLEMENT_DYNAMIC(CDlgTakeoffSequence, CXTResizeDialog)
CDlgTakeoffSequence::CDlgTakeoffSequence(int nProjID, PSelectFlightType pSelectFlightType,CAirportDatabase *pAirPortdb, CWnd* pParent /*=NULL*/)
	: CXTResizeDialog(CDlgTakeoffSequence::IDD, pParent)
 ,m_nProjID(nProjID),m_SelFlightType(pSelectFlightType),m_AirportDB(pAirPortdb)
{
	m_pTakeoffSequencingList = 0;
}

CDlgTakeoffSequence::~CDlgTakeoffSequence()
{
}

void CDlgTakeoffSequence::DoDataExchange(CDataExchange* pDX)
{
	CXTResizeDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_TAKEOFFSEQUENCE, m_wndTreeSequence);
}


BEGIN_MESSAGE_MAP(CDlgTakeoffSequence, CXTResizeDialog)
	ON_WM_CREATE()
	ON_COMMAND(ID_NEW_SEQUENCE, OnMsgNewSequence)
	ON_COMMAND(ID_DEL_SEQUENCE, OnMsgDelSequence)
	ON_COMMAND(ID_EDIT_SEQUENCE, OnMsgEditSequence)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnBnClickedButtonSave)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_TAKEOFFSEQUENCE, OnTvnSelchangedTreeTakeoffsequence)
	ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()


// CDlgTakeoffSequence message handlers

void CDlgTakeoffSequence::OnBnClickedButtonSave()
{
	// TODO: Add your control notification handler code here
	try
	{
		CADODatabase::BeginTransaction() ;
		m_pTakeoffSequencingList->SaveData();
		CADODatabase::CommitTransaction() ;

	}
	catch (CADOException e)
	{
		CADODatabase::RollBackTransation() ;
	}
	
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(FALSE);
}

void CDlgTakeoffSequence::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	OnBnClickedButtonSave();
	OnOK();
}

void CDlgTakeoffSequence::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	OnCancel();
}

void CDlgTakeoffSequence::SetAirportDB(CAirportDatabase* pDB)
{
	m_AirportDB = pDB;
}

int CDlgTakeoffSequence::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CXTResizeDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndSequenceToolbar.CreateEx(this, TBSTYLE_FLAT,WS_CHILD|WS_VISIBLE|CBRS_ALIGN_TOP)
		|| !m_wndSequenceToolbar.LoadToolBar(IDR_TOOLBAR_ADD_DEL_EDIT))
	{
		return -1;
	}

	CToolBarCtrl& sequenceToolbar = m_wndSequenceToolbar.GetToolBarCtrl();
	sequenceToolbar.SetCmdID(0, ID_NEW_SEQUENCE);
	sequenceToolbar.SetCmdID(1, ID_DEL_SEQUENCE);
	sequenceToolbar.SetCmdID(2, ID_EDIT_SEQUENCE);

	return 0;
}


BOOL CDlgTakeoffSequence::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog();


	// set the position of the sequence toolbar
	CRect rectSequenceToolbar;
	m_wndTreeSequence.GetWindowRect(&rectSequenceToolbar);
	ScreenToClient(&rectSequenceToolbar);
	rectSequenceToolbar.top -= 26;
	rectSequenceToolbar.bottom = rectSequenceToolbar.top + 22;
	rectSequenceToolbar.left += 4;
	m_wndSequenceToolbar.MoveWindow(&rectSequenceToolbar);

	m_wndSequenceToolbar.GetToolBarCtrl().EnableButton(ID_NEW_SEQUENCE);
	m_wndSequenceToolbar.GetToolBarCtrl().EnableButton(ID_DEL_SEQUENCE);
	m_wndSequenceToolbar.GetToolBarCtrl().EnableButton(ID_EDIT_SEQUENCE);
	ReadData();
	InitSequenceTreeCtrl();
	UpdateState();

	SetResize(m_wndSequenceToolbar.GetDlgCtrlID(), SZ_TOP_LEFT, SZ_TOP_LEFT);
	SetResize(IDC_TREE_TAKEOFFSEQUENCE, SZ_TOP_LEFT, SZ_BOTTOM_RIGHT);
	SetResize(IDC_BUTTON_SAVE, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDOK, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDCANCEL, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);

	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),TRUE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),FALSE);

	return TRUE;
}

void CDlgTakeoffSequence::ReadData()
{
	if(m_pTakeoffSequencingList){
		delete m_pTakeoffSequencingList;
		m_pTakeoffSequencingList = 0;
	}
	m_pTakeoffSequencingList = new TakeoffSequencingList(m_nProjID,m_AirportDB);
	if(!m_pTakeoffSequencingList)return;
	m_pTakeoffSequencingList->ReadData();
}

void CDlgTakeoffSequence::InitSequenceTreeCtrl()
{
	m_wndTreeSequence.DeleteAllItems();
	m_wndTreeSequence.SetRedraw(FALSE);
	int nTakeOffSeqCount = -1;
	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);
	nTakeOffSeqCount = m_pTakeoffSequencingList->GetTakeoffSequencingCount();
	m_hTakeOffPositions = m_wndTreeSequence.InsertItem(_T("Take off positions"),cni,FALSE);
	for (int i = 0; i< nTakeOffSeqCount; i++)
	{	
		CCoolTree::InitNodeInfo(this,cni);
		TakeoffSequencing* pTakeoffSequencing = m_pTakeoffSequencingList->GetItem(i);
		if(!pTakeoffSequencing)continue;

		CString strTakeOffPosition = _T("Take off position:");
		int nTakeOffPosCount = pTakeoffSequencing->GetTakeOffPositionCount();
		for (int nTakeOffPosIndex = 0;nTakeOffPosIndex < nTakeOffPosCount;++nTakeOffPosIndex) 
		{
			TakeOffPositionInfo * pTakeOffPositionInfo = pTakeoffSequencing->GetTakeOffPositionInfo(nTakeOffPosIndex);
			if(pTakeOffPositionInfo){
				if(nTakeOffPosIndex != nTakeOffPosCount - 1)
					strTakeOffPosition += pTakeOffPositionInfo->m_strTakeOffPositionName + _T(" , "); 
				else
					strTakeOffPosition += pTakeOffPositionInfo->m_strTakeOffPositionName;
			}
		}

		HTREEITEM hTakeOffPositionTreeItem = m_wndTreeSequence.InsertItem(strTakeOffPosition,cni,FALSE,FALSE,m_hTakeOffPositions);
		m_wndTreeSequence.SetItemData(hTakeOffPositionTreeItem,(DWORD_PTR)pTakeoffSequencing);

		int nTakeOffSeqItemCount = -1;
		CString strTimeRange = _T("");
		nTakeOffSeqItemCount = pTakeoffSequencing->GetTakeOffSequenceInfoCount();
		for (int nTakeOffSeqItemIndex = 0;nTakeOffSeqItemIndex < nTakeOffSeqItemCount;++nTakeOffSeqItemIndex) 
		{
			TakeOffSequenceInfo* pTakeOffSequenceInfoItem = pTakeoffSequencing->GetTakeOffSequenceInfo(nTakeOffSeqItemIndex);
			if(!pTakeOffSequenceInfoItem)continue;
			strTimeRange.Format(_T("TimeRange:Day%d %02d:%02d:%02d - Day%d %02d:%02d:%02d"),\
				pTakeOffSequenceInfoItem->m_startTime.GetDay(),\
				pTakeOffSequenceInfoItem->m_startTime.GetHour(),\
				pTakeOffSequenceInfoItem->m_startTime.GetMinute(),\
				pTakeOffSequenceInfoItem->m_startTime.GetSecond(),\
				pTakeOffSequenceInfoItem->m_endTime.GetDay(),\
				pTakeOffSequenceInfoItem->m_endTime.GetHour(),\
				pTakeOffSequenceInfoItem->m_endTime.GetMinute(),\
				pTakeOffSequenceInfoItem->m_endTime.GetSecond());    
			cni.net = NET_SHOW_DIALOGBOX;
			HTREEITEM hTimeRangeTreeItem = m_wndTreeSequence.InsertItem(strTimeRange,cni,FALSE,FALSE,hTakeOffPositionTreeItem);
			m_wndTreeSequence.SetItemData(hTimeRangeTreeItem,(DWORD_PTR)pTakeOffSequenceInfoItem);
			for (int j = 0; j < PriorityCount; j++)	{	
				CString strPriorityName = TakeoffSequencingList::GetPriorityName(pTakeOffSequenceInfoItem->m_priority[j].m_Type);
				CString strpriority = _T("");
				strpriority.Format(_T("Priority %d: %s"), j+1, strPriorityName);

				cni.nt = NT_NORMAL;
				cni.net = NET_COMBOBOX;
				cni.bAutoSetItemText = false;
				cni.fMinValue =(float)j ;
				HTREEITEM hPriorityItem = m_wndTreeSequence.InsertItem(strpriority,cni,FALSE,FALSE, hTimeRangeTreeItem);
				m_wndTreeSequence.SetItemData(hPriorityItem,j) ;

				if(pTakeOffSequenceInfoItem->m_priority[j].m_Type == PriorityFlightType)
					InitFlightTyItem(hPriorityItem,pTakeOffSequenceInfoItem->m_priority[j]) ;
				else
					InitChangeConditionItem(hPriorityItem,pTakeOffSequenceInfoItem->m_priority[j]) ;
			}
			m_wndTreeSequence.Expand(hTimeRangeTreeItem, TVE_EXPAND);
		}	
		m_wndTreeSequence.Expand(hTakeOffPositionTreeItem, TVE_EXPAND);
	}	
	m_wndTreeSequence.Expand(m_hTakeOffPositions,TVE_EXPAND);
	m_wndTreeSequence.SetRedraw(TRUE);
}
void CDlgTakeoffSequence::InitChangeConditionItem(HTREEITEM _PriorityItem,const CPriorityData& _priorityData)
{
	CString strVal ;
	strVal.Format(_T("Change condition:")) ;
	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);
	HTREEITEM ItemCondition = m_wndTreeSequence.InsertItem(strVal,cni,FALSE,FALSE,_PriorityItem) ;
	m_wndTreeSequence.SetItemData(ItemCondition,TREENODE_CHANGE_CONDITION) ;

	cni.net = NET_COMBOBOX ;
	strVal = FormatCrossNodeCstring(_priorityData) ;
	HTREEITEM PItem = m_wndTreeSequence.InsertItem(strVal,cni,FALSE,FALSE,ItemCondition) ;
	m_wndTreeSequence.SetItemData(PItem,TREENODE_CROSS) ;

	strVal = FormatEnCroachNodeCSring(_priorityData) ;
	PItem = m_wndTreeSequence.InsertItem(strVal,cni,FALSE,FALSE,ItemCondition) ;
	m_wndTreeSequence.SetItemData(PItem,TREENODE_ENCROACH) ;

	strVal = FormatTakeoffNodeCSring(_priorityData) ;
	PItem = m_wndTreeSequence.InsertItem(strVal,cni,FALSE,FALSE,ItemCondition) ;
	m_wndTreeSequence.SetItemData(PItem,TREENODE_TAKEOFF) ;
}
void CDlgTakeoffSequence::InitFlightTyItem(HTREEITEM _PriorityItem,const CPriorityData& _priorityData)
{
	CString strVal ;
	_priorityData.m_FltTy.screenPrint(strVal) ;
	CString ItemText ;
	ItemText.Format(_T("Priority Flight Type: %s"),strVal) ;
	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);
	cni.net = NET_SHOW_DIALOGBOX ;
	HTREEITEM ItemCondition = m_wndTreeSequence.InsertItem(ItemText,cni,FALSE,FALSE,_PriorityItem) ;
	m_wndTreeSequence.SetItemData(ItemCondition,TREENODE_FLIGHTTYPE) ;
}
CString CDlgTakeoffSequence::FormatCrossNodeCstring(const CPriorityData& _priorityData)
{
	CString strVal ;
	if(_priorityData.m_CrossPoint)
		strVal.Format(_T("A/C would be delayed at crossing waypoints (%s)"),"YES") ;
	else
		strVal.Format(_T("A/C would be delayed at crossing waypoints (%s)"),"NO") ;
	return strVal ;
}
CString CDlgTakeoffSequence::FormatEnCroachNodeCSring(const CPriorityData& _priorityData)
{
	CString strVal ;
	if(_priorityData.m_CanEncroach)
		strVal.Format(_T("A/C would encroach intrail seperation on same SID (%s)"),"YES") ;
	else
		strVal.Format(_T("A/C would encroach intrail seperation on same SID (%s)"),"NO") ;
	return strVal ;
}
CString CDlgTakeoffSequence::FormatTakeoffNodeCSring(const CPriorityData& _priorityData)
{
	CString strVal ;
	if(_priorityData.m_CanTakeOff)
		strVal.Format(_T("Priority flight type able take off in another Q (%s)"),"YES") ;
	else
		strVal.Format(_T("Priority flight type able take off in another Q (%s)"),"NO") ;
	return strVal ;
}
void CDlgTakeoffSequence::OnMsgNewSequence()
{
	HTREEITEM hSel = m_wndTreeSequence.GetSelectedItem();
	if(!hSel)return;

	HTREEITEM hFirstGrade = hSel;
	HTREEITEM hSecondGrade = m_wndTreeSequence.GetParentItem(hSel);
	TakeoffSequencing* pItemData = 0;
	TakeOffPositionInfo * pTakeOffPosInfo = 0;
	TakeOffSequenceInfo * pTakeOffSeqInfo = 0;
	if(hFirstGrade == m_hTakeOffPositions)
	{		
			CDlgSelectTakeoffPosition dlgSelTakeOffPos(m_nProjID, -1, -1, this);
			dlgSelTakeOffPos.SetAirportDB(m_AirportDB);
			if(dlgSelTakeOffPos.DoModal() == IDOK)
			{
				pItemData = new TakeoffSequencing(m_nProjID,m_AirportDB);
				if(pItemData)
				{
					m_pTakeoffSequencingList->AddItem(pItemData);
					if(dlgSelTakeOffPos.m_bSelAll)
					{
						pTakeOffPosInfo = new TakeOffPositionInfo(-1);
						if(pTakeOffPosInfo)
						{
							pTakeOffPosInfo->m_nTakeOffPositionID = -1;
							pTakeOffPosInfo->m_nRwyID = -1;
							pTakeOffPosInfo->m_nRwyMark = -1;
							pTakeOffPosInfo->m_strTakeOffPositionName = _T("All");
							pItemData->AddTakeOffPositionInfo(pTakeOffPosInfo);
						}
					}
					else
					{
						int nIndex = 0;
						for (std::vector<int>::iterator itrTakeOffPos = dlgSelTakeOffPos.m_vrSelID.begin();\
							itrTakeOffPos != dlgSelTakeOffPos.m_vrSelID.end();++itrTakeOffPos,++nIndex) 
						{
								pTakeOffPosInfo = new TakeOffPositionInfo(-1);
								if(pTakeOffPosInfo)
								{
									pTakeOffPosInfo->m_nTakeOffPositionID = *itrTakeOffPos;
									pTakeOffPosInfo->m_strTakeOffPositionName = dlgSelTakeOffPos.m_arSelName[nIndex];
									pTakeOffPosInfo->m_nRwyID = dlgSelTakeOffPos.m_nRwyID;
									pTakeOffPosInfo->m_nRwyMark = dlgSelTakeOffPos.m_nRwyMark;
									pItemData->AddTakeOffPositionInfo(pTakeOffPosInfo);
								}
						}
					}
					pTakeOffSeqInfo = new TakeOffSequenceInfo(-1,m_AirportDB);
					if(pTakeOffSeqInfo)
					{
						pTakeOffSeqInfo->m_startTime.set(0L);
						pTakeOffSeqInfo->m_endTime.set(86399L);
						pItemData->AddTakeOffSequenceInfo(pTakeOffSeqInfo);
					}
					InitSequenceTreeCtrl();
				}
			}
	}
	else if(hSecondGrade == m_hTakeOffPositions)
	{	
		TakeoffSequencing* pSelItemData = (TakeoffSequencing* )m_wndTreeSequence.GetItemData(hSel);
		if(!pSelItemData)return;
		if(pSelItemData->GetTakeOffPositionCount() < 2)	
		{
			ElapsedTime etStart, etEnd;
			CDlgTimeRange dlgTimeRange(etStart, etEnd,this);
			if(IDOK == dlgTimeRange.DoModal()){
				pTakeOffSeqInfo = new TakeOffSequenceInfo(-1,m_AirportDB);
				if(pTakeOffSeqInfo)
				{
					pTakeOffSeqInfo->m_startTime = dlgTimeRange.GetStartTime();
					pTakeOffSeqInfo->m_endTime   = dlgTimeRange.GetEndTime();
					if(!pSelItemData->AddTakeOffSequenceInfo(pTakeOffSeqInfo))
					{
						MessageBox(_T("Time range overlap!"));
					}
					InitSequenceTreeCtrl();
				}
			}
			return;
		}

		int iResult = MessageBox(_T("Do you want to split these TakeOffPosition(Yes),or add TimeRange(No)?"),_T(""),MB_YESNOCANCEL);
		
		if(iResult == IDYES){			
				CDlgChooseTakeOffPosition dlgSelSplit(pSelItemData,this);
				if(dlgSelSplit.DoModal() == IDOK){
					pItemData = new TakeoffSequencing(m_nProjID,m_AirportDB);
					if(pItemData){
						m_pTakeoffSequencingList->AddItem(pItemData);
						for (std::vector<TakeOffPositionInfo*>::iterator itrSel = dlgSelSplit.m_vrSelItemInfo.begin();itrSel != dlgSelSplit.m_vrSelItemInfo.end();++itrSel) {
								if(*itrSel){
									pSelItemData->DeleteTakeOffPositionItem(*itrSel);
									pTakeOffPosInfo = new TakeOffPositionInfo(-1);
									if (pTakeOffPosInfo) {
										*pTakeOffPosInfo = **itrSel;
										pItemData->AddTakeOffPositionInfo(pTakeOffPosInfo);
									}
								}
							}
						pTakeOffSeqInfo = new TakeOffSequenceInfo(-1,m_AirportDB);
						if(pTakeOffSeqInfo){
							pTakeOffSeqInfo->m_startTime.set(0L);
							pTakeOffSeqInfo->m_endTime.set(86399L);
							pItemData->AddTakeOffSequenceInfo(pTakeOffSeqInfo);
						}
						InitSequenceTreeCtrl();
					}
				}
		}else if(iResult == IDNO){
			ElapsedTime etStart, etEnd;
			CDlgTimeRange dlgTimeRange(etStart, etEnd,this);
			if(IDOK == dlgTimeRange.DoModal()){
				pTakeOffSeqInfo = new TakeOffSequenceInfo(-1,m_AirportDB);
				if(pTakeOffSeqInfo){
					pTakeOffSeqInfo->m_startTime = dlgTimeRange.GetStartTime();
					pTakeOffSeqInfo->m_endTime   = dlgTimeRange.GetEndTime();
					pSelItemData->AddTakeOffSequenceInfo(pTakeOffSeqInfo);
					InitSequenceTreeCtrl();
				}
			}
		}else
			return;
	}
}


void CDlgTakeoffSequence::OnMsgDelSequence()
{
	HTREEITEM hSel = m_wndTreeSequence.GetSelectedItem();
	if(!hSel)return;

	HTREEITEM hSecondGrade = m_wndTreeSequence.GetParentItem(hSel);
	HTREEITEM hThirdGrade = m_wndTreeSequence.GetParentItem(m_wndTreeSequence.GetParentItem(hSel)); 
	if(hSecondGrade == m_hTakeOffPositions){
		TakeoffSequencing* pSelItemData = (TakeoffSequencing* )m_wndTreeSequence.GetItemData(hSel);
		m_pTakeoffSequencingList->DeleteItem(pSelItemData);
		InitSequenceTreeCtrl();
	}else if(hThirdGrade == m_hTakeOffPositions){
		TakeOffSequenceInfo* pSelItemData = (TakeOffSequenceInfo*)m_wndTreeSequence.GetItemData(hSel);
		TakeoffSequencing* pSelItemParentData = (TakeoffSequencing* )m_wndTreeSequence.GetItemData(m_wndTreeSequence.GetParentItem(hSel));
		if(pSelItemParentData){
			pSelItemParentData->DeleteTakeOffSequenceInfoItem(pSelItemData);
			InitSequenceTreeCtrl();
		}
	}
}


void CDlgTakeoffSequence::OnMsgEditSequence()
{
	HTREEITEM hSel = m_wndTreeSequence.GetSelectedItem();
	if(!hSel)return;

	HTREEITEM hSecondGrade = m_wndTreeSequence.GetParentItem(hSel);
	HTREEITEM hThirdGrade = m_wndTreeSequence.GetParentItem(m_wndTreeSequence.GetParentItem(hSel)); 
	HTREEITEM hFourthGrade =  m_wndTreeSequence.GetParentItem(m_wndTreeSequence.GetParentItem(m_wndTreeSequence.GetParentItem(hSel))); 

	if(hSecondGrade == m_hTakeOffPositions){
		CDlgSelectTakeoffPosition dlgSelTakeOffPos(m_nProjID,-1,-1,this);
		dlgSelTakeOffPos.SetAirportDB(m_AirportDB) ;
		if(dlgSelTakeOffPos.DoModal() == IDOK){
			TakeoffSequencing* pSelItemData = (TakeoffSequencing*)m_wndTreeSequence.GetItemData(hSel);
			TakeOffPositionInfo * pTakeOffPosInfo = 0;
			if (pSelItemData) {
				pSelItemData->DeleteAllTakeOffPositionItem();
				if(dlgSelTakeOffPos.m_bSelAll){
					pTakeOffPosInfo = new TakeOffPositionInfo(-1);
					if(pTakeOffPosInfo){
						pTakeOffPosInfo->m_nTakeOffPositionID = -1;
						pTakeOffPosInfo->m_nRwyID = -1;
						pTakeOffPosInfo->m_nRwyMark = -1;
						pTakeOffPosInfo->m_strTakeOffPositionName = _T("All");
						pSelItemData->AddTakeOffPositionInfo(pTakeOffPosInfo);
					}
				}else{
					int nIndex = 0;
					for (std::vector<int>::iterator itrTakeOffPos = dlgSelTakeOffPos.m_vrSelID.begin();\
						itrTakeOffPos != dlgSelTakeOffPos.m_vrSelID.end();++itrTakeOffPos,++nIndex) {
							pTakeOffPosInfo = new TakeOffPositionInfo(-1);
							if(pTakeOffPosInfo){
								pTakeOffPosInfo->m_nTakeOffPositionID = *itrTakeOffPos;
								pTakeOffPosInfo->m_strTakeOffPositionName = dlgSelTakeOffPos.m_arSelName[nIndex];
								pTakeOffPosInfo->m_nRwyID = dlgSelTakeOffPos.m_nRwyID;
								pTakeOffPosInfo->m_nRwyMark = dlgSelTakeOffPos.m_nRwyMark;
								pSelItemData->AddTakeOffPositionInfo(pTakeOffPosInfo);
							}
						}
				}
				InitSequenceTreeCtrl();
			}
		}
	}else if(hThirdGrade == m_hTakeOffPositions){
		TakeOffSequenceInfo * pSelTakeOffSeqInfo = (TakeOffSequenceInfo * )  m_wndTreeSequence.GetItemData(hSel);
		if(pSelTakeOffSeqInfo){
			CDlgTimeRange dlgTimeRange(pSelTakeOffSeqInfo->m_startTime,pSelTakeOffSeqInfo->m_endTime,this);
			if(IDOK == dlgTimeRange.DoModal()){
					pSelTakeOffSeqInfo->m_startTime = dlgTimeRange.GetStartTime();
					pSelTakeOffSeqInfo->m_endTime   = dlgTimeRange.GetEndTime();
					InitSequenceTreeCtrl();
			}
		}
	}else if(hFourthGrade == m_hTakeOffPositions){
		m_wndTreeSequence.ShowComboBoxWnd(hSel,m_wndTreeSequence.GetItemNodeInfo(hSel));
	}
}


LRESULT CDlgTakeoffSequence::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message){	
		case UM_CEW_SHOW_DIALOGBOX_BEGIN:
			{
				HTREEITEM hSel = (HTREEITEM)wParam;
				COOLTREE_NODE_INFO* pCNI = (COOLTREE_NODE_INFO*)lParam;
				if(m_wndTreeSequence.GetItemData(hSel) == TREENODE_FLIGHTTYPE)
				{
					FlightConstraint fltType = (*m_SelFlightType)(NULL) ;
					HTREEITEM pParItem = m_wndTreeSequence.GetParentItem(hSel) ;
					pParItem = m_wndTreeSequence.GetParentItem(pParItem) ;

					TakeOffSequenceInfo * pSelTakeOffSeqInfo = (TakeOffSequenceInfo * )  m_wndTreeSequence.GetItemData(pParItem);

					pSelTakeOffSeqInfo->m_priority[PriorityCount-1].m_FltTy = fltType ;

					CString strVal ;
					CString ItemText ;
					fltType.screenPrint(strVal);
					ItemText.Format(_T("Priority Flight Type: %s"),strVal) ;
					m_wndTreeSequence.SetItemText(hSel,ItemText) ;
					break ;
				}
				if (pCNI) {
				TakeOffSequenceInfo * pSelTakeOffSeqInfo = (TakeOffSequenceInfo * )  m_wndTreeSequence.GetItemData(hSel);
				if(pSelTakeOffSeqInfo)
					pCNI->pEditWnd = new CDlgTimeRange(pSelTakeOffSeqInfo->m_startTime,pSelTakeOffSeqInfo->m_endTime,this);
				}
			}
			break;
		case UM_CEW_SHOW_DIALOGBOX_END:
			{
				HTREEITEM hSel = (HTREEITEM)wParam;
				COOLTREE_NODE_INFO* pCNI = (COOLTREE_NODE_INFO*)lParam;
				if (pCNI) {
					TakeOffSequenceInfo * pSelTakeOffSeqInfo = (TakeOffSequenceInfo * )  m_wndTreeSequence.GetItemData(hSel);
					if(pSelTakeOffSeqInfo){
						pSelTakeOffSeqInfo->m_startTime = ((CDlgTimeRange*)pCNI->pEditWnd)->GetStartTime();
						pSelTakeOffSeqInfo->m_endTime   = ((CDlgTimeRange*)pCNI->pEditWnd)->GetEndTime();
						InitSequenceTreeCtrl();
					}
				}
			}
			break;
		case UM_CEW_COMBOBOX_BEGIN:
			{
				CWnd* pWnd=m_wndTreeSequence.GetEditWnd((HTREEITEM)wParam);
				CRect rectWnd;
				m_wndTreeSequence.GetItemRect((HTREEITEM)wParam,rectWnd,TRUE);
				pWnd->SetWindowPos(NULL,rectWnd.right,rectWnd.top,0,0,SWP_NOSIZE);
				CComboBox* pCB=(CComboBox*)pWnd;
				pCB->ResetContent() ;
				int type = m_wndTreeSequence.GetItemData((HTREEITEM)wParam) ;
				if(type == TREENODE_CROSS || type == TREENODE_ENCROACH || type == TREENODE_TAKEOFF)
				{
						pCB->AddString(_T("YES")) ;
						pCB->AddString(_T("NO")) ;
				}else
				{
						pCB->AddString(_T("FIFO"));
						pCB->AddString(_T("LongestQFirst"));
						pCB->AddString(_T("RoundRobin"));
						pCB->AddString(_T("DepartureSlot"));
						pCB->AddString(_T("PriorityFlightType"));
				}
			}
			break;
		case UM_CEW_COMBOBOX_SELCHANGE:
			{
				HTREEITEM hItem=(HTREEITEM)wParam;
				CString strText=*(CString*)lParam;
				
				int type = m_wndTreeSequence.GetItemData((HTREEITEM)wParam) ;
				if(type == TREENODE_CROSS || type == TREENODE_ENCROACH || type == TREENODE_TAKEOFF)
				{
					HTREEITEM ParentItem = m_wndTreeSequence.GetParentItem(hItem) ;
					ParentItem = m_wndTreeSequence.GetParentItem(ParentItem) ;
					int priorityNdx = (int)m_wndTreeSequence.GetItemNodeInfo(ParentItem)->fMinValue ;
					if(priorityNdx < 0 || priorityNdx >= PriorityCount)
						break ;
					ParentItem = m_wndTreeSequence.GetParentItem(ParentItem) ;
					TakeOffSequenceInfo * pSelTakeOffSeqInfo = (TakeOffSequenceInfo *) m_wndTreeSequence.GetItemData(ParentItem) ;
					BOOL SelType = FALSE ;
					CString NewItemText ;
					if(strText.CompareNoCase("YES") == 0)
						SelType = TRUE ;
					else
						SelType = FALSE ;
					if(type == TREENODE_CROSS)
					{
						pSelTakeOffSeqInfo->m_priority[priorityNdx].m_CrossPoint = SelType ;
						NewItemText = FormatCrossNodeCstring(pSelTakeOffSeqInfo->m_priority[priorityNdx]) ;
					}
					if(type == TREENODE_ENCROACH)
					{
						pSelTakeOffSeqInfo->m_priority[priorityNdx].m_CanEncroach =SelType ;
						NewItemText = FormatEnCroachNodeCSring(pSelTakeOffSeqInfo->m_priority[priorityNdx]) ;
					}
					if(type == TREENODE_TAKEOFF)
					{
						pSelTakeOffSeqInfo->m_priority[priorityNdx].m_CanTakeOff = SelType ;
						NewItemText = FormatTakeoffNodeCSring(pSelTakeOffSeqInfo->m_priority[priorityNdx]) ;
					}

					m_wndTreeSequence.SetItemText(hItem,NewItemText) ;
					break ;

				}
				HTREEITEM hTempItem = hItem;
				int nPos = 1;
				int nPos2 = 1;
				while (hTempItem = m_wndTreeSequence.GetPrevSiblingItem(hTempItem))
					++nPos;
				
				CString strText2 = m_wndTreeSequence.GetItemText(hItem);
				strText2 = strText2.Right(strText2.GetLength()-12);
				int nFlag = 0;
				if(strcmp(strText,strText2)!=0)
				{
					CString strNewText;
					strNewText.Format(_T("Priority %d: %s"), nPos, strText);
					m_wndTreeSequence.SetItemText(hItem,strNewText);

					int itemp,itemp2;
					HTREEITEM hParentItem = m_wndTreeSequence.GetParentItem(hItem);
					if (m_wndTreeSequence.ItemHasChildren(hParentItem))
					{
						HTREEITEM hNextItem;
						HTREEITEM hChildItem = m_wndTreeSequence.GetChildItem(hParentItem);

						while (hChildItem != NULL)
						{
							hNextItem = m_wndTreeSequence.GetNextItem(hChildItem, TVGN_NEXT);
							if(hChildItem!=hItem)
							{	
								CString strTemp = m_wndTreeSequence.GetItemText(hChildItem);
								strTemp = strTemp.Right(strTemp.GetLength()-12);
								if(strcmp(strTemp,strText) == 0)
								{
									strNewText.Format(_T("Priority %d: %s"), nPos2, strText2);
									m_wndTreeSequence.SetItemText(hChildItem,strNewText);
									break;
								}	
							}					
							++nPos2;
							hChildItem = hNextItem;
						}
					}
					if(strcmp(strText,"FIFO") == 0)
						itemp = 0;
					else if(strcmp(strText,"LongestQFirst") == 0)
						itemp = 1;
					else if(strcmp(strText,"RoundRobin") == 0)
						itemp = 2;
					else if(strcmp(strText,"DepartureSlot") == 0)
						itemp = 3;
					else if(strcmp(strText,"PriorityFlightType") == 0)
						itemp = 4;

					if(strcmp(strText2,"FIFO") == 0)
						itemp2 = 0;
					else if(strcmp(strText2,"LongestQFirst") == 0)
						itemp2 = 1;
					else if(strcmp(strText2,"RoundRobin") == 0)
						itemp2 = 2;
					else if(strcmp(strText2,"DepartureSlot") == 0)
						itemp2 = 3;
					else if(strcmp(strText2,"PriorityFlightType") == 0)
						itemp2 = 4;

					TakeOffSequenceInfo * pSelTakeOffSeqInfo = (TakeOffSequenceInfo *) m_wndTreeSequence.GetItemData(m_wndTreeSequence.GetParentItem(hItem));
					if(pSelTakeOffSeqInfo){
						int priorityNdx = (int)m_wndTreeSequence.GetItemNodeInfo(hItem)->fMinValue ;
						if(priorityNdx < 0 || priorityNdx >= PriorityCount)
							break ;
						Priority emTemp = pSelTakeOffSeqInfo->m_priority[priorityNdx].m_Type;
						pSelTakeOffSeqInfo->m_priority[priorityNdx].m_Type = (Priority)itemp;

						if(emTemp != (Priority)itemp )
						{
							HTREEITEM hChildItem = m_wndTreeSequence.GetChildItem(hItem) ;
							m_wndTreeSequence.DeleteItem(hChildItem) ;
							if((Priority)itemp == PriorityFlightType)
								InitFlightTyItem(hItem,pSelTakeOffSeqInfo->m_priority[priorityNdx]) ;
							else
								InitChangeConditionItem(hItem,pSelTakeOffSeqInfo->m_priority[priorityNdx]) ;
						}
					}
				}
			}
			break;

		default:
			break;
	}
	return CXTResizeDialog::DefWindowProc(message, wParam, lParam);
}

void CDlgTakeoffSequence::UpdateState()
{
	HTREEITEM hSel = m_wndTreeSequence.GetSelectedItem();
	if(!hSel)return;

	HTREEITEM hFirstGrade = hSel;
	HTREEITEM hSecondGrade = m_wndTreeSequence.GetParentItem(hSel);
	HTREEITEM hThirdGrade = m_wndTreeSequence.GetParentItem(m_wndTreeSequence.GetParentItem(hSel)); 
	HTREEITEM hFourthGrade =  m_wndTreeSequence.GetParentItem(m_wndTreeSequence.GetParentItem(m_wndTreeSequence.GetParentItem(hSel))); 
	int type =  m_wndTreeSequence.GetItemData(hSel) ;
	if(type == TREENODE_TAKEOFF || type == TREENODE_ENCROACH || type == TREENODE_CROSS || type == TREENODE_CHANGE_CONDITION)
	{
		m_wndSequenceToolbar.GetToolBarCtrl().EnableButton(ID_NEW_SEQUENCE,FALSE);
		m_wndSequenceToolbar.GetToolBarCtrl().EnableButton(ID_DEL_SEQUENCE,FALSE);
		m_wndSequenceToolbar.GetToolBarCtrl().EnableButton(ID_EDIT_SEQUENCE,FALSE);
		return ;
	}
	if (hFirstGrade == m_hTakeOffPositions) {
		m_wndSequenceToolbar.GetToolBarCtrl().EnableButton(ID_NEW_SEQUENCE,TRUE);
		m_wndSequenceToolbar.GetToolBarCtrl().EnableButton(ID_DEL_SEQUENCE,FALSE);
		m_wndSequenceToolbar.GetToolBarCtrl().EnableButton(ID_EDIT_SEQUENCE,FALSE);
	}else if (hSecondGrade == m_hTakeOffPositions) {
		m_wndSequenceToolbar.GetToolBarCtrl().EnableButton(ID_NEW_SEQUENCE,TRUE);
		m_wndSequenceToolbar.GetToolBarCtrl().EnableButton(ID_DEL_SEQUENCE,TRUE);
		m_wndSequenceToolbar.GetToolBarCtrl().EnableButton(ID_EDIT_SEQUENCE,TRUE);
	}else if (hThirdGrade == m_hTakeOffPositions) {
		m_wndSequenceToolbar.GetToolBarCtrl().EnableButton(ID_NEW_SEQUENCE,FALSE);
		m_wndSequenceToolbar.GetToolBarCtrl().EnableButton(ID_DEL_SEQUENCE,TRUE);
		m_wndSequenceToolbar.GetToolBarCtrl().EnableButton(ID_EDIT_SEQUENCE,TRUE);
	}else if (hFourthGrade == m_hTakeOffPositions) {
		m_wndSequenceToolbar.GetToolBarCtrl().EnableButton(ID_NEW_SEQUENCE,FALSE);
		m_wndSequenceToolbar.GetToolBarCtrl().EnableButton(ID_DEL_SEQUENCE,FALSE);
		m_wndSequenceToolbar.GetToolBarCtrl().EnableButton(ID_EDIT_SEQUENCE,TRUE);
	}
}

void CDlgTakeoffSequence::OnTvnSelchangedTreeTakeoffsequence(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	UpdateState();
	*pResult = 0;
}


void CDlgTakeoffSequence::OnContextMenu(CWnd* pWnd, CPoint point)
{
	// TODO: Add your message handler code here
	CRect rectTree;
	m_wndTreeSequence.GetWindowRect(&rectTree);
	if (!rectTree.PtInRect(point)) 
		return;

	m_wndTreeSequence.SetFocus();
	CPoint pt = point;
	m_wndTreeSequence.ScreenToClient(&pt);
	UINT iRet;
	HTREEITEM hRClickItem = m_wndTreeSequence.HitTest(pt, &iRet);
	if (iRet != TVHT_ONITEMLABEL)
	{
		hRClickItem = NULL;
		return;
	}

	m_wndTreeSequence.SelectItem(hRClickItem);	
	HTREEITEM hFirstGrade = hRClickItem;
	HTREEITEM hSecondGrade = m_wndTreeSequence.GetParentItem(hRClickItem);
	HTREEITEM hThirdGrade = m_wndTreeSequence.GetParentItem(m_wndTreeSequence.GetParentItem(hRClickItem)); 
	HTREEITEM hFourthGrade =  m_wndTreeSequence.GetParentItem(m_wndTreeSequence.GetParentItem(m_wndTreeSequence.GetParentItem(hRClickItem))); 


	CMenu menuPopup; 
	menuPopup.CreatePopupMenu(); 

	if (hFirstGrade == m_hTakeOffPositions) {
		menuPopup.AppendMenu(MF_POPUP, (UINT) ID_NEW_SEQUENCE, _T("Add take off position...") );
	}else if (hSecondGrade == m_hTakeOffPositions) {
		menuPopup.AppendMenu(MF_POPUP, (UINT) ID_NEW_SEQUENCE, _T("Add time range...") ); 
		menuPopup.AppendMenu(MF_POPUP, (UINT) ID_DEL_SEQUENCE, _T("Delete take off position") ); 
		menuPopup.AppendMenu(MF_POPUP, (UINT) ID_EDIT_SEQUENCE, _T("Edit take off position...") );
	}else if (hThirdGrade == m_hTakeOffPositions) {
		menuPopup.AppendMenu(MF_POPUP, (UINT) ID_DEL_SEQUENCE, _T("Delete time range...") ); 
		menuPopup.AppendMenu(MF_POPUP, (UINT) ID_EDIT_SEQUENCE, _T("Edit time range...") );
	}else if (hFourthGrade == m_hTakeOffPositions) {
		menuPopup.AppendMenu(MF_POPUP, (UINT) ID_EDIT_SEQUENCE, _T("Edit priority...") );
	}
	menuPopup.TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);
	menuPopup.DestroyMenu();
}
