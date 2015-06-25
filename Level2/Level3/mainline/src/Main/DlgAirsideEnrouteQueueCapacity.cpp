// AirsideEnrouteQueueCapacity.cpp : implementation file
//

#include "stdafx.h"
#include "DlgAirsideEnrouteQueueCapacity.h"
#include "..\InputAirside\InputAirside.h"
#include "..\InputAirside\ALTAirport.h"
#include "DlgTimeRange.h"
#include "DlgRunwayExitSelect.h"
#include "Common\WinMsg.h"

// CAirsideEnrouteQueueCapacity dialog

IMPLEMENT_DYNAMIC(CDlgAirsideEnrouteQueueCapacity, CXTResizeDialog)
CDlgAirsideEnrouteQueueCapacity::CDlgAirsideEnrouteQueueCapacity(int nProjID, CWnd* pParent /*=NULL*/)
	: CXTResizeDialog(CDlgAirsideEnrouteQueueCapacity::IDD, pParent)
	,m_nProjectID(nProjID)
{
}

CDlgAirsideEnrouteQueueCapacity::~CDlgAirsideEnrouteQueueCapacity()
{
}

void CDlgAirsideEnrouteQueueCapacity::DoDataExchange(CDataExchange* pDX)
{
	CXTResizeDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_CONTENT, m_treeCtrl);
}


BEGIN_MESSAGE_MAP(CDlgAirsideEnrouteQueueCapacity, CXTResizeDialog)
	ON_WM_CREATE()
	ON_COMMAND(ID_TOOLBARBUTTON_ADD, OnNewOperation)
	ON_COMMAND(ID_TOOLBARBUTTON_DEL, OnDelOperation)
	ON_COMMAND(ID_TOOLBARBUTTON_EDIT, OnEditOperation)

	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnBnClickedButtonSave)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_CONTENT, OnTvnSelchangedTreeData)
	ON_MESSAGE(WM_OBJECTTREE_DOUBLECLICK, OnTreeDoubleClick)


END_MESSAGE_MAP()


// CAirsideEnrouteQueueCapacity message handlers
int CDlgAirsideEnrouteQueueCapacity::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CXTResizeDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here	
	if(m_wndToolBar.CreateEx(this,TBSTYLE_FLAT,WS_CHILD|WS_VISIBLE|CBRS_ALIGN_TOP,CRect(0,0,0,0))&&
		!m_wndToolBar.LoadToolBar(IDR_ADDDELEDITTOOLBAR_ENROUTEQCAPACITY))
	{
		AfxMessageBox("Can't create toolbar!");
		return FALSE;
	}
	return 0;
}

BOOL CDlgAirsideEnrouteQueueCapacity::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	InitToolbar();

	SetResize(m_wndToolBar.GetDlgCtrlID(),SZ_TOP_LEFT,SZ_TOP_LEFT);
	SetResize(IDC_STATIC_FRAME,SZ_TOP_LEFT,SZ_BOTTOM_RIGHT);
	SetResize(IDC_TREE_CONTENT,SZ_TOP_LEFT,SZ_BOTTOM_RIGHT);
	SetResize(IDC_BUTTON_SAVE,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);
	SetResize(IDOK,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);
	SetResize(IDCANCEL,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);

	ReadRunwayList();
	m_enrouteQCapacity.ReadData(0);
	LoadTree();
	UpdateToolbarState();
	SetModified(FALSE);


	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgAirsideEnrouteQueueCapacity::InitToolbar()
{
	CRect rcToolbar;
	// set the position of the arrival delay trigger list toolbar	
	m_treeCtrl.GetWindowRect(&rcToolbar);
	ScreenToClient(&rcToolbar);
	rcToolbar.top -= 23;
	rcToolbar.bottom = rcToolbar.top + 22;
	rcToolbar.left += 2;
	m_wndToolBar.MoveWindow(rcToolbar);

	m_wndToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTON_ADD);
	m_wndToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTON_DEL);
	m_wndToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTON_EDIT);
}

void CDlgAirsideEnrouteQueueCapacity::LoadTree()
{
	int nCount = m_enrouteQCapacity.GetElementCount();
	for (int nItem = 0; nItem < nCount; ++nItem)
	{
		AirsideEnrouteQCapacityTimeRange *pItem = m_enrouteQCapacity.GetItem(nItem);
		if(pItem == NULL)
			continue;

		AddTimeRange(pItem);
	}

}

void CDlgAirsideEnrouteQueueCapacity::OnNewOperation()
{
	HTREEITEM hItemSelected = m_treeCtrl.GetSelectedItem();
	if(hItemSelected == NULL)
	{
		ElapsedTime etStart, etEnd;
		CDlgTimeRange dlg(etStart, etEnd);
		if(dlg.DoModal() == IDOK)
		{
			AirsideEnrouteQCapacityTimeRange* pTimeRangeData = new AirsideEnrouteQCapacityTimeRange;
			pTimeRangeData->SetTimeRange(dlg.GetStartTime(),dlg.GetEndTime());
			m_enrouteQCapacity.AddNewItem(pTimeRangeData);
		
			AddTimeRange(pTimeRangeData);
			SetModified();
		}
	}
	else
	{
		CARCTreeCtrlExWithColor::CItemData *pItemDataEx = m_treeCtrl.GetItemDataEx(hItemSelected);
		if(pItemDataEx == NULL)
			return;

		CTreeItemData *pItemData  = (CTreeItemData *)pItemDataEx->dwptrItemData;
		if(pItemData == NULL)
			return;
		if(pItemData->enumType == ItemType_TimeRange)
		{
			//new take off position
			CDlgRunwayExitSelect dlg(&m_vRunways,true,this);
			if(dlg.DoModal() == IDOK)
			{
				std::vector<CSelectData>* vSelectData = dlg.GetSelectExitName();
				if(vSelectData->size() > 0)
				{
					CSelectData runwayExit = vSelectData->at(0);
					AirsideEnrouteQCapacityItem *pItem = new AirsideEnrouteQCapacityItem;
					pItem->setTakeOffPosition(runwayExit.m_ExitID);
					pItemData->pTimeRangeData->getEnrouteQCapacity()->AddNewItem(pItem);
					AddEnrouteQueueCapacityItem(pItemData->pTimeRangeData, pItem, hItemSelected);
					SetModified();
				}
			}
		}
	}

}
void CDlgAirsideEnrouteQueueCapacity::ReadRunwayList()
{
	int firstIndex = 0;
	std::vector<int> vAirportIds;
	InputAirside::GetAirportList(m_nProjectID, vAirportIds);
	for (std::vector<int>::iterator iterAirportID = vAirportIds.begin(); iterAirportID != vAirportIds.end(); ++iterAirportID)
	{
		ALTAirport airport;
		airport.ReadAirport(*iterAirportID);

		ALTObject::GetObjectList(ALT_RUNWAY,*iterAirportID,m_vRunways);
	}

}
void CDlgAirsideEnrouteQueueCapacity::OnEditOperation()
{
	HTREEITEM hItemSelected = m_treeCtrl.GetSelectedItem();
	if(hItemSelected == NULL)
		return;

	CARCTreeCtrlExWithColor::CItemData *pItemDataEx = m_treeCtrl.GetItemDataEx(hItemSelected);
	if(pItemDataEx == NULL)
		return;

	CTreeItemData *pItemData  = (CTreeItemData *)pItemDataEx->dwptrItemData;
	if(pItemData == NULL)
		return;
	if(pItemData->enumType == ItemType_TimeRange)
	{
		AirsideEnrouteQCapacityTimeRange* pTimeRangeData = pItemData->pTimeRangeData ;
		if(pTimeRangeData == NULL)
			return;

		ElapsedTime etStart = pTimeRangeData->getTimeRange()->GetStartTime();
		ElapsedTime etEnd =  pTimeRangeData->getTimeRange()->GetEndTime();

		CDlgTimeRange dlg(etStart, etEnd);
		if(dlg.DoModal() == IDOK)
		{
		
			pTimeRangeData->SetTimeRange(dlg.GetStartTime(),dlg.GetEndTime());
			
			UpdateItemText(hItemSelected);
		}
	}
	else if(pItemData->enumType == ItemType_TakeOffPos)
	{
		//new take off position
		CDlgRunwayExitSelect dlg(&m_vRunways,true,this);
		if(dlg.DoModal() == IDOK)
		{
			std::vector<CSelectData>* vSelectData = dlg.GetSelectExitName();
			if(vSelectData->size() > 0)
			{
				CSelectData runwayExit = vSelectData->at(0);
				AirsideEnrouteQCapacityItem *pItem = pItemData->pEnrouteQItem;
				pItem->setTakeOffPosition(runwayExit.m_ExitID);
				
				UpdateItemText(hItemSelected);
			}
		}
	}
	else if(pItemData->enumType == ItemType_MaxLen)
	{
		
		double dMaxQueueLength =pItemData->pEnrouteQItem->getMaxLength();
		CString strQueueLength ;
		strQueueLength.Format(_T("%0f"),dMaxQueueLength);
		m_treeCtrl.SetDisplayNum(static_cast<int>(dMaxQueueLength));
		m_treeCtrl.SetDisplayType(2);
		m_treeCtrl.SetSpinRange(0,1000000);
		m_treeCtrl.SpinEditLabel(hItemSelected,strQueueLength);
	}

}
void CDlgAirsideEnrouteQueueCapacity::UpdateItemText( HTREEITEM hItemUpdate )
{
	if(hItemUpdate == NULL)
		return;
	CARCTreeCtrlExWithColor::CItemData *pItemDataEx = m_treeCtrl.GetItemDataEx(hItemUpdate);
	if(pItemDataEx == NULL)
		return;

	CTreeItemData *pItemData  = (CTreeItemData *)pItemDataEx->dwptrItemData;
	if(pItemData == NULL)
		return;


	CARCTreeCtrlExWithColor::CItemData colorItemData;
	CARCTreeCtrlExWithColor::CColorStringSection colorStringSection;
	colorItemData.lSize = sizeof(colorItemData);
	colorStringSection.colorSection = RGB(0,0,255);


	if(pItemData->enumType == ItemType_TimeRange)
	{
		if(pItemData->pTimeRangeData == NULL)
			return;

		CString strStartTime = pItemData->pTimeRangeData->getTimeRange()->GetStartTime().PrintDateTime();
		CString strEndTime = pItemData->pTimeRangeData->getTimeRange()->GetEndTime().PrintDateTime();
		colorStringSection.strSection = strStartTime;
		colorItemData.vrItemStringSectionColorShow.push_back(colorStringSection);

		colorStringSection.strSection = strEndTime;
		colorItemData.vrItemStringSectionColorShow.push_back(colorStringSection);

		CString strNodeText;
		strNodeText.Format(_T(" %s - %s"),strStartTime ,strEndTime);
		m_treeCtrl.SetItemText(hItemUpdate,strNodeText);

		SetModified();
	}
	else if (pItemData->enumType == ItemType_TakeOffPos)
	{

		if(pItemData->pEnrouteQItem == NULL)
			return;

		colorItemData.vrItemStringSectionColorShow.clear();

		CString strTakeOffPosName = pItemData->pEnrouteQItem->getTakeOffPosition()->GetName();
		colorStringSection.strSection = strTakeOffPosName;
		colorItemData.vrItemStringSectionColorShow.push_back(colorStringSection);

		CString strNodeText;
		strNodeText.Format(_T("Enroute Queue at: %s"),strTakeOffPosName);
		m_treeCtrl.SetItemText(hItemUpdate,strNodeText);

		SetModified();

	}
	else if (pItemData->enumType == ItemType_MaxLen)
	{	
		if(pItemData->pEnrouteQItem == NULL)
			return;
		
		colorItemData.vrItemStringSectionColorShow.clear();
		CString strMaxLength;
		strMaxLength.Format(_T("%.2f"), pItemData->pEnrouteQItem->getMaxLength());
		colorStringSection.strSection = strMaxLength;
		colorItemData.vrItemStringSectionColorShow.push_back(colorStringSection);

		CString strNodeText;
		strNodeText.Format(_T("Max Length(meters): %s"),strMaxLength);
		m_treeCtrl.SetItemText(hItemUpdate,strNodeText);
		
		SetModified();
	}
}

void CDlgAirsideEnrouteQueueCapacity::OnDelOperation()
{
	HTREEITEM hItemSelected = m_treeCtrl.GetSelectedItem();
	if(hItemSelected == NULL)
		return;

	CARCTreeCtrlExWithColor::CItemData *pItemDataEx = m_treeCtrl.GetItemDataEx(hItemSelected);
	if(pItemDataEx == NULL)
		return;

	CTreeItemData *pItemData  = (CTreeItemData *)pItemDataEx->dwptrItemData;
	if(pItemData == NULL)
		return;
	if(pItemData->enumType == ItemType_TimeRange)
	{
		if(pItemData->pTimeRangeData == NULL)
			return;

		m_enrouteQCapacity.DeleteItem(pItemData->pTimeRangeData);
		m_treeCtrl.DeleteItem(hItemSelected);
		
		SetModified();
	}
	else if(pItemData->enumType == ItemType_TakeOffPos)
	{
		if(pItemData->pTimeRangeData == NULL || pItemData->pEnrouteQItem == NULL)
			return;

		pItemData->pTimeRangeData->getEnrouteQCapacity()->DeleteItem(pItemData->pEnrouteQItem);
		m_treeCtrl.DeleteItem(hItemSelected);

		SetModified();
	}


}

void CDlgAirsideEnrouteQueueCapacity::AddTimeRange( AirsideEnrouteQCapacityTimeRange *pTimeRangeData )
{

	CARCTreeCtrlExWithColor::CItemData colorItemData;
	CARCTreeCtrlExWithColor::CColorStringSection colorStringSection;
	colorItemData.lSize = sizeof(colorItemData);
	colorStringSection.colorSection = RGB(0,0,255);


	CTreeItemData *pTreeItemData = new CTreeItemData(ItemType_TimeRange);
	pTreeItemData->pTimeRangeData = pTimeRangeData;

	colorItemData.dwptrItemData = (DWORD_PTR)pTreeItemData;
	colorItemData.vrItemStringSectionColorShow.clear();

	CString strStartTime = pTimeRangeData->getTimeRange()->GetStartTime().PrintDateTime();
	CString strEndTime = pTimeRangeData->getTimeRange()->GetEndTime().PrintDateTime();
	colorStringSection.strSection = strStartTime;
	colorItemData.vrItemStringSectionColorShow.push_back(colorStringSection);

	colorStringSection.strSection = strEndTime;
	colorItemData.vrItemStringSectionColorShow.push_back(colorStringSection);

	CString strNodeText;
	strNodeText.Format(_T(" %s - %s"),strStartTime ,strEndTime);

	HTREEITEM hItemTimeRange = m_treeCtrl.InsertItem(strNodeText,TVI_ROOT);
	m_treeCtrl.SetItemDataEx(hItemTimeRange,colorItemData);

	AddEnrouteQueueCapacity(pTimeRangeData, pTimeRangeData->getEnrouteQCapacity(), hItemTimeRange);

	m_treeCtrl.Expand(hItemTimeRange,TVE_EXPAND);

}



void CDlgAirsideEnrouteQueueCapacity::AddEnrouteQueueCapacity(AirsideEnrouteQCapacityTimeRange *pTimeRangeData,  AirsideEnrouteQCapacityItemList *pEnrouteQ, HTREEITEM hTimeRangeItem )
{
	int nCount = pEnrouteQ->GetElementCount();
	for (int nItem = 0; nItem < nCount; ++ nItem)
	{
		AirsideEnrouteQCapacityItem* pItem = pEnrouteQ->GetItem(nItem);
		if(pItem == NULL)
			continue;

		AddEnrouteQueueCapacityItem(pTimeRangeData, pItem, hTimeRangeItem);
	}
}

void CDlgAirsideEnrouteQueueCapacity::AddEnrouteQueueCapacityItem(AirsideEnrouteQCapacityTimeRange *pTimeRangeData,  AirsideEnrouteQCapacityItem *pEnrouteQItem, HTREEITEM hTimeRangeItem )
{

	AddEnrouteQueueTakeOffPositionItem(pTimeRangeData, pEnrouteQItem, hTimeRangeItem);

}

void CDlgAirsideEnrouteQueueCapacity::AddEnrouteQueueTakeOffPositionItem(AirsideEnrouteQCapacityTimeRange *pTimeRangeData,  AirsideEnrouteQCapacityItem *pEnrouteQItem, HTREEITEM hTimeRangeItem )
{
	CARCTreeCtrlExWithColor::CItemData colorItemData;
	CARCTreeCtrlExWithColor::CColorStringSection colorStringSection;
	colorItemData.lSize = sizeof(colorItemData);
	colorStringSection.colorSection = RGB(0,0,255);


	CTreeItemData *pTreeItemData = new CTreeItemData(ItemType_TakeOffPos);
	pTreeItemData->pTimeRangeData = pTimeRangeData;
	pTreeItemData->pEnrouteQItem = pEnrouteQItem;

	colorItemData.dwptrItemData = (DWORD_PTR)pTreeItemData;
	colorItemData.vrItemStringSectionColorShow.clear();

	CString strTakeOffPosName = pEnrouteQItem->getTakeOffPosition()->GetName();
	colorStringSection.strSection = strTakeOffPosName;
	colorItemData.vrItemStringSectionColorShow.push_back(colorStringSection);



	CString strNodeText;
	strNodeText.Format(_T("Enroute Queue at: %s"),strTakeOffPosName);

	HTREEITEM hTakeOffPos = m_treeCtrl.InsertItem(strNodeText,hTimeRangeItem);
	m_treeCtrl.SetItemDataEx(hTakeOffPos,colorItemData);

	AddEnrouteQueueMaxLengthItem(pTimeRangeData, pEnrouteQItem, hTakeOffPos);

	m_treeCtrl.Expand(hTakeOffPos,TVE_EXPAND);
	m_treeCtrl.Expand(hTimeRangeItem, TVE_EXPAND);
}

void CDlgAirsideEnrouteQueueCapacity::AddEnrouteQueueMaxLengthItem(AirsideEnrouteQCapacityTimeRange *pTimeRangeData,  AirsideEnrouteQCapacityItem *pEnrouteQItem, HTREEITEM hMaxLengthItem )
{
	CARCTreeCtrlExWithColor::CItemData colorItemData;
	CARCTreeCtrlExWithColor::CColorStringSection colorStringSection;
	colorItemData.lSize = sizeof(colorItemData);
	colorStringSection.colorSection = RGB(0,0,255);


	CTreeItemData *pTreeItemData = new CTreeItemData(ItemType_MaxLen);
	pTreeItemData->pTimeRangeData = pTimeRangeData;
	pTreeItemData->pEnrouteQItem = pEnrouteQItem;

	colorItemData.dwptrItemData = (DWORD_PTR)pTreeItemData;
	colorItemData.vrItemStringSectionColorShow.clear();


	CString strMaxLength;
	strMaxLength.Format(_T("%.2f"), pEnrouteQItem->getMaxLength());
	colorStringSection.strSection = strMaxLength;
	colorItemData.vrItemStringSectionColorShow.push_back(colorStringSection);



	CString strNodeText;
	strNodeText.Format(_T("Max Length(meters): %s"),strMaxLength);

	HTREEITEM hMaxLen = m_treeCtrl.InsertItem(strNodeText,hMaxLengthItem);
	m_treeCtrl.SetItemDataEx(hMaxLen,colorItemData);


}


LRESULT CDlgAirsideEnrouteQueueCapacity::DefWindowProc( UINT message, WPARAM wParam, LPARAM lParam )
{
	if( message == WM_INPLACE_SPIN )
	{

		HTREEITEM hItemSelected = m_treeCtrl.GetSelectedItem();
		if(hItemSelected == NULL)
			return 1;

		CARCTreeCtrlExWithColor::CItemData *pItemDataEx = m_treeCtrl.GetItemDataEx(hItemSelected);
		if(pItemDataEx == NULL)
			return 1;

		CTreeItemData *pItemData  = (CTreeItemData *)pItemDataEx->dwptrItemData;
		if(pItemData == NULL)
			return 1;

		LPSPINTEXT pst = (LPSPINTEXT) lParam;
		int nValue = pst->iPercent;

		HTREEITEM hSelItem = m_treeCtrl.GetSelectedItem();

		if(pItemData->enumType == ItemType_MaxLen)
		{
			if(pItemData->pEnrouteQItem)
			{
				pItemData->pEnrouteQItem->setMaxLength(nValue) ;
				UpdateItemText(hItemSelected);
				SetModified();
			}
		}
	}

	return CXTResizeDialog::DefWindowProc(message,wParam,lParam);
}

void CDlgAirsideEnrouteQueueCapacity::OnBnClickedButtonSave()
{
	try
	{
		CADODatabase::BeginTransaction();
		m_enrouteQCapacity.SaveData( m_nProjectID );
		CADODatabase::CommitTransaction() ;
		SetModified(FALSE);
	}
	catch (CADOException e)
	{
		CADODatabase::RollBackTransation() ;
	}

}

void CDlgAirsideEnrouteQueueCapacity::OnBnClickedOk()
{
	try
	{
		CADODatabase::BeginTransaction();
		m_enrouteQCapacity.SaveData( m_nProjectID );
		CADODatabase::CommitTransaction() ;
	}
	catch (CADOException e)
	{
		CADODatabase::RollBackTransation() ;
	}
	OnOK();
}

void CDlgAirsideEnrouteQueueCapacity::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	OnCancel();
}

void CDlgAirsideEnrouteQueueCapacity::OnTvnSelchangedTreeData( NMHDR *pNMHDR, LRESULT *pResult )
{

	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);

	UpdateToolbarState();
	*pResult = 0;

}

LRESULT CDlgAirsideEnrouteQueueCapacity::OnTreeDoubleClick( WPARAM, LPARAM )
{
	OnEditOperation();


	return 1;
}

void CDlgAirsideEnrouteQueueCapacity::UpdateToolbarState()
{
	HTREEITEM hItemSelected = m_treeCtrl.GetSelectedItem();
	if(hItemSelected == NULL)
	{
		//only "New" available
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTON_ADD,TRUE);
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTON_DEL,FALSE);
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTON_EDIT,FALSE);
		return;
	}

	CARCTreeCtrlExWithColor::CItemData *pItemDataEx = m_treeCtrl.GetItemDataEx(hItemSelected);
	if(pItemDataEx == NULL)
	{
		//only "New" available
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTON_ADD,TRUE);
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTON_DEL,FALSE);
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTON_EDIT,FALSE);
		return;
	}
	CTreeItemData *pItemData  = (CTreeItemData *)pItemDataEx->dwptrItemData;
	if(pItemData == NULL)
	{
		//only "New" available
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTON_ADD,TRUE);
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTON_DEL,FALSE);
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTON_EDIT,FALSE);
		return;
	}
	if(pItemData->enumType == ItemType_MaxLen)
	{
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTON_ADD,FALSE);
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTON_DEL,FALSE);
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTON_EDIT,TRUE);
	}
	else if(pItemData->enumType == ItemType_TakeOffPos)
	{
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTON_ADD,FALSE);
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTON_DEL,TRUE);
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTON_EDIT,TRUE);
	}
	else
	{
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTON_ADD,TRUE);
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTON_DEL,TRUE);
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTON_EDIT,TRUE);
	}

}

void CDlgAirsideEnrouteQueueCapacity::SetModified( BOOL bModified /*= TRUE*/ )
{
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(bModified);
}










