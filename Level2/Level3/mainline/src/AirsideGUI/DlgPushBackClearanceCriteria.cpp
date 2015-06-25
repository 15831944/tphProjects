// DlgPushBackClearanceCriteria.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"
#include "DlgPushBackClearanceCriteria.h"
#include ".\dlgpushbackclearancecriteria.h"
#include "DlgSelectALTObject.h"
#include "DlgSelectRunway.h"
#include "DlgStandFamily.h"

// CDlgPushBackClearanceCriteria dialog
static const UINT ID_NEW_ITEM = 10;
static const UINT ID_DEL_ITEM = 11;
static const UINT ID_EDIT_ITEM = 12;

IMPLEMENT_DYNAMIC(CDlgPushBackClearanceCriteria, CXTResizeDialog)
CDlgPushBackClearanceCriteria::CDlgPushBackClearanceCriteria(int nProjID,CUnitsManager* pGlobalUnits,PSelectFlightType	pSelectFlightType,CAirportDatabase *pAirportDatabase , CWnd* pParent /*=NULL*/)
	: CXTResizeDialog(CDlgPushBackClearanceCriteria::IDD, pParent)
	, m_nProjID(nProjID)
	,m_pGlobalUnits(pGlobalUnits)
    ,m_criteriaProcessor(nProjID,pAirportDatabase)
	,m_pSelectFlightType(pSelectFlightType)
	,m_pAirportDatabase(pAirportDatabase) 
{
	m_hTreeSelItem = 0;
	m_dDistancNoPushBackWithin = 100;
	m_lTimeNoLeadWithin = 5;
	m_lMaxOutboundFltToAssignedRunway = 100;
	m_lMaxOutboundFltToAllRunway = 1000;
	m_bChangeItself = true;
}

CDlgPushBackClearanceCriteria::~CDlgPushBackClearanceCriteria()
{
}

void CDlgPushBackClearanceCriteria::DoDataExchange(CDataExchange* pDX)
{
	CXTResizeDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_CONDITION, m_wndConditionTree);
	DDX_Control(pDX, IDC_LIST_CRITERIA_VALUE, m_wndCriteriaList);
}


BEGIN_MESSAGE_MAP(CDlgPushBackClearanceCriteria, CXTResizeDialog)
	ON_WM_CREATE()
	ON_COMMAND(ID_NEW_ITEM, OnNewItem)
	ON_COMMAND(ID_DEL_ITEM, OnDeleteItem)
	//ON_COMMAND(ID_EDIT_ITEM, OnEditItem)
	ON_WM_SIZE()
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_CONDITION, OnTvnSelchangedTreeCondition)
	ON_BN_CLICKED(IDC_SAVE, OnBnClickedSave)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_LIST_CRITERIA_VALUE, OnLvnEndlabeleditListCriteriaValue)
	//ON_EN_CHANGE(IDC_LIST_CRITERIA_VALUE, OnLvnEndlabeleditListCriteriaValue)
	ON_EN_CHANGE(IDC_EDIT1, OnEnChangeEdit1)
	ON_EN_CHANGE(IDC_EDIT2, OnEnChangeEdit2)
END_MESSAGE_MAP()

// CDlgPushBackClearanceCriteria message handlers

void CDlgPushBackClearanceCriteria::InitializeTreeCtrl(void)
{
	m_wndConditionTree.SetRedraw(FALSE);

	m_hTreeSelItem = m_wndConditionTree.InsertItem(_T("Other Aircraft PushBack"));	
	m_wndConditionTree.InsertItem(_T("Takeoff queue length"));
	m_wndConditionTree.InsertItem(_T("Deicing queue length"));
	m_wndConditionTree.InsertItem(_T("Slot time"));
	m_wndConditionTree.InsertItem(_T("Number of aircraft inbound"));
	m_wndConditionTree.InsertItem(_T("Number of aircraft outbound"));
	m_wndConditionTree.SelectItem(m_hTreeSelItem);

	m_wndConditionTree.SetRedraw(TRUE);
	return;
}

void CDlgPushBackClearanceCriteria::InitToolBar(void)
{
	ASSERT(::IsWindow(m_wndToolBar.m_hWnd));
	CRect rectToolBar;
	GetDlgItem(IDC_STATIC_TOOLBAR_RECT)->GetWindowRect(&rectToolBar);
	ScreenToClient(rectToolBar);
	rectToolBar.left += 2;
	m_wndToolBar.MoveWindow(rectToolBar);	
	GetDlgItem(IDC_STATIC_TOOLBAR_RECT)->ShowWindow(SW_HIDE);
	m_wndToolBar.ShowWindow(SW_SHOW);

	CToolBarCtrl& toolbar = m_wndToolBar.GetToolBarCtrl();
	toolbar.SetCmdID(0, ID_NEW_ITEM);
	toolbar.SetCmdID(1, ID_DEL_ITEM);
	toolbar.SetCmdID(2, ID_EDIT_ITEM);

	toolbar.EnableButton(ID_NEW_ITEM, TRUE);
	toolbar.EnableButton(ID_DEL_ITEM, TRUE);
	toolbar.HideButton(ID_EDIT_ITEM, TRUE);
}

void CDlgPushBackClearanceCriteria::RebuildList(HTREEITEM hSelItem)
{
	if(!hSelItem)
		return;
	CString strNodeTitle = _T("");
	strNodeTitle = m_wndConditionTree.GetItemText(hSelItem);
	CToolBarCtrl& toolbar = m_wndToolBar.GetToolBarCtrl();
	toolbar.EnableButton(ID_NEW_ITEM, TRUE);
	toolbar.EnableButton(ID_DEL_ITEM, TRUE);
	toolbar.EnableButton(ID_EDIT_ITEM, FALSE);
	if(_T("Other Aircraft PushBack") == strNodeTitle)
	{
		toolbar.EnableButton(ID_NEW_ITEM, FALSE);
		toolbar.EnableButton(ID_DEL_ITEM, FALSE);
		toolbar.EnableButton(ID_EDIT_ITEM, FALSE);
		m_wndCriteriaList.DeleteAllItems();
		int nColumnCount = m_wndCriteriaList.GetHeaderCtrl()->GetItemCount();
		for (int i=0;i < nColumnCount;i++)
			m_wndCriteriaList.DeleteColumn(0);

		DWORD dwStyle = m_wndCriteriaList.GetExtendedStyle();
		dwStyle |= LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES;
		m_wndCriteriaList.SetExtendedStyle(dwStyle);

		CStringList strList;
		LV_COLUMNEX lvc;
		memset(&lvc,0,sizeof(lvc));
		lvc.mask = LVCF_WIDTH|LVCF_TEXT|LVCF_FMT;
		CString strHeadTitle = _T("");
		strHeadTitle.Format(_T("%s (%s)"),_T("No other push back towards \n same taxiway within distance"),m_pGlobalUnits->GetLengthUnitLongString(m_pGlobalUnits->GetLengthUnits()));
		lvc.pszText = strHeadTitle.GetBuffer(strHeadTitle.GetLength());
		lvc.cx = 400;
		lvc.csList = &strList;
		lvc.fmt = LVCFMT_LEFT/*|LVCFMT_NOEDIT*/;
		m_wndCriteriaList.InsertColumn(0, &lvc);

		lvc.pszText = _T("No taxing aircraft expected to be \n at lead in line intersection within time");
		m_wndCriteriaList.InsertColumn(1, &lvc);

		CString strValue = _T("");
		strValue.Format(_T("%.2f"),m_criteriaProcessor.GetDistancNoPushBackWithin());
		m_wndCriteriaList.InsertItem(0,strValue);
		strValue.Format(_T("%d"),m_criteriaProcessor.GetTimeNoLeadWithin());
		m_wndCriteriaList.SetItemText(0,1,strValue);
	}
	else if(_T("Takeoff queue length") == strNodeTitle)
	{
		m_wndCriteriaList.DeleteAllItems();
		int nColumnCount = m_wndCriteriaList.GetHeaderCtrl()->GetItemCount();
		for (int i=0;i < nColumnCount;i++)
			m_wndCriteriaList.DeleteColumn(0);

		DWORD dwStyle = m_wndCriteriaList.GetExtendedStyle();
		dwStyle |= LVS_EX_FULLROWSELECT| LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES;
		m_wndCriteriaList.SetExtendedStyle(dwStyle);

		CStringList strList;
		LV_COLUMNEX lvc;
		memset(&lvc,0,sizeof(lvc));
		lvc.mask = LVCF_WIDTH|LVCF_TEXT|LVCF_FMT;
		CString strHeadTitle = _T("");
		lvc.pszText = _T("Runway Marking");
		lvc.cx = 200;
		lvc.csList = &strList;
		lvc.fmt = LVCFMT_LEFT|LVCFMT_NOEDIT;
		m_wndCriteriaList.InsertColumn(0, &lvc);

		lvc.fmt = LVCFMT_LEFT;
		lvc.pszText = _T("Number of aircraft less than");
		m_wndCriteriaList.InsertColumn(1, &lvc);

		CString strValue = _T("");
		for(int iIndex = 0; iIndex < m_criteriaProcessor.GetTakeoffQueueCount();iIndex ++)
		{
			Runway runway;
			runway.ReadObject(m_criteriaProcessor.GetTakeoffQueueRunwayID(iIndex));
			if(RUNWAYMARK_FIRST == (RUNWAY_MARK)m_criteriaProcessor.GetTakeoffQueueRunwayMarkingIndex(iIndex))
				strValue = runway.GetMarking1().c_str();
			else
				strValue = runway.GetMarking2().c_str();
			m_wndCriteriaList.InsertItem(iIndex,strValue);
			strValue.Format(_T("%d"),m_criteriaProcessor.GetTakeoffQueueLengthAtRunway(iIndex));
			m_wndCriteriaList.SetItemText(iIndex,1,strValue);
		}
	}
	else if(_T("Deicing queue length") == strNodeTitle)
	{
		m_wndCriteriaList.DeleteAllItems();
		int nColumnCount = m_wndCriteriaList.GetHeaderCtrl()->GetItemCount();
		for (int i=0;i < nColumnCount;i++)
			m_wndCriteriaList.DeleteColumn(0);

		DWORD dwStyle = m_wndCriteriaList.GetExtendedStyle();
		dwStyle |= LVS_EX_FULLROWSELECT| LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES;
		m_wndCriteriaList.SetExtendedStyle(dwStyle);

		CStringList strList;
		LV_COLUMNEX lvc;
		memset(&lvc,0,sizeof(lvc));
		lvc.mask = LVCF_WIDTH|LVCF_TEXT|LVCF_FMT;
		CString strHeadTitle = _T("");
		lvc.pszText = _T("Pad name");
		lvc.cx = 200;
		lvc.csList = &strList;
		lvc.fmt = LVCFMT_LEFT|LVCFMT_NOEDIT;
		m_wndCriteriaList.InsertColumn(0, &lvc);

		lvc.fmt = LVCFMT_LEFT;
		lvc.pszText = _T("Number of aircraft less than");
		m_wndCriteriaList.InsertColumn(1, &lvc);

		CString strValue = _T("");
		for(int iIndex = 0; iIndex < m_criteriaProcessor.GetDeicingQueueCount();iIndex ++)
		{
			strValue.Format(_T("%s"),m_criteriaProcessor.GetDeicingQueueDeicePadName(iIndex));
			m_wndCriteriaList.InsertItem(iIndex,strValue);
			strValue.Format(_T("%d"),m_criteriaProcessor.GetDeicingQueueLengthAtDeicePad(iIndex));
			m_wndCriteriaList.SetItemText(iIndex,1,strValue);
		}
	}
	else if(_T("Slot time") == strNodeTitle)
	{
		m_wndCriteriaList.DeleteAllItems();
		int nColumnCount = m_wndCriteriaList.GetHeaderCtrl()->GetItemCount();
		for (int i=0;i < nColumnCount;i++)
			m_wndCriteriaList.DeleteColumn(0);

		DWORD dwStyle = m_wndCriteriaList.GetExtendedStyle();
		dwStyle |= LVS_EX_FULLROWSELECT| LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES;
		m_wndCriteriaList.SetExtendedStyle(dwStyle);

		CStringList strList;
		LV_COLUMNEX lvc;
		memset(&lvc,0,sizeof(lvc));
		lvc.mask = LVCF_WIDTH|LVCF_TEXT|LVCF_FMT;
		CString strHeadTitle = _T("");
		lvc.pszText = _T("Flight Type");
		lvc.cx = 200;
		lvc.csList = &strList;
		lvc.fmt = LVCFMT_LEFT|LVCFMT_NOEDIT;
		m_wndCriteriaList.InsertColumn(0, &lvc);

		lvc.fmt = LVCFMT_LEFT;
		lvc.pszText = _T("Slot time less than");
		m_wndCriteriaList.InsertColumn(1, &lvc);

		CString strValue = _T("");
		for(int iIndex = 0; iIndex < m_criteriaProcessor.GetSlotTimeCount();iIndex ++)
		{
			strValue.Format(_T("%s"),m_criteriaProcessor.GetSlotTimeFltTypeName(iIndex));
			//strValue = strValue.Mid(strValue.Find('\\')+1);//show only screenprint flight constraint.
			FlightConstraint flightType;
			if(!m_pAirportDatabase)return;
			flightType.SetAirportDB(m_pAirportDatabase);
			flightType.setConstraintWithVersion(strValue);
			strValue.Empty(); // must empty strValue before screenPrint, see FlightConstraint::screenPrint for more details
			flightType.screenPrint(strValue);

			m_wndCriteriaList.InsertItem(iIndex,strValue);
			strValue.Format(_T("%d"),m_criteriaProcessor.GetFltTypeMaxSlotTime(iIndex));
			m_wndCriteriaList.SetItemText(iIndex,1,strValue);
		}
	}
	else if(_T("Number of aircraft inbound") == strNodeTitle)
	{
		m_wndCriteriaList.DeleteAllItems();
		int nColumnCount = m_wndCriteriaList.GetHeaderCtrl()->GetItemCount();
		for (int i=0;i < nColumnCount;i++)
			m_wndCriteriaList.DeleteColumn(0);

		DWORD dwStyle = m_wndCriteriaList.GetExtendedStyle();
		dwStyle |= LVS_EX_FULLROWSELECT| LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES;
		m_wndCriteriaList.SetExtendedStyle(dwStyle);

		CStringList strList;
		LV_COLUMNEX lvc;
		memset(&lvc,0,sizeof(lvc));
		lvc.mask = LVCF_WIDTH|LVCF_TEXT|LVCF_FMT;
		CString strHeadTitle = _T("");
		lvc.pszText = _T("Stand family");
		lvc.cx = 200;
		lvc.csList = &strList;
		lvc.fmt = LVCFMT_LEFT|LVCFMT_NOEDIT;
		m_wndCriteriaList.InsertColumn(0, &lvc);

		lvc.fmt = LVCFMT_LEFT;
		lvc.pszText = _T("Number of aircraft inbound less than");
		m_wndCriteriaList.InsertColumn(1, &lvc);

		CString strValue = _T("");
		for(int iIndex = 0; iIndex < m_criteriaProcessor.GetCountOfStandForInboundFlt();iIndex ++)
		{
			strValue = m_criteriaProcessor.GetNameOfStandForFltInbound(iIndex);
			if(strValue.IsEmpty())
				strValue.Format(_T("%s"),_T("All Stands"));
			m_wndCriteriaList.InsertItem(iIndex,strValue);
			strValue.Format(_T("%d"),m_criteriaProcessor.GetInboundFltNumberAtStand(iIndex));
			m_wndCriteriaList.SetItemText(iIndex,1,strValue);
		}
	}
	else if(_T("Number of aircraft outbound") == strNodeTitle)
	{
		toolbar.EnableButton(ID_NEW_ITEM, FALSE);
		toolbar.EnableButton(ID_DEL_ITEM, FALSE);
		toolbar.EnableButton(ID_EDIT_ITEM, FALSE);
		m_wndCriteriaList.DeleteAllItems();
		int nColumnCount = m_wndCriteriaList.GetHeaderCtrl()->GetItemCount();
		for (int i=0;i < nColumnCount;i++)
			m_wndCriteriaList.DeleteColumn(0);

		DWORD dwStyle = m_wndCriteriaList.GetExtendedStyle();
		dwStyle |= LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES;
		m_wndCriteriaList.SetExtendedStyle(dwStyle);

		CStringList strList;
		LV_COLUMNEX lvc;
		memset(&lvc,0,sizeof(lvc));
		lvc.mask = LVCF_WIDTH|LVCF_TEXT|LVCF_FMT;
		CString strHeadTitle = _T("");
		lvc.pszText = _T("Number of aircraft outbound \n to assigned runway less than");
		lvc.cx = 400;
		lvc.csList = &strList;
		lvc.fmt = LVCFMT_LEFT;
		m_wndCriteriaList.InsertColumn(0, &lvc);

		lvc.pszText = _T("Number of aircraft outbound \n to all runway less than");
		m_wndCriteriaList.InsertColumn(1, &lvc);

		CString strValue = _T("");
		strValue.Format(_T("%d"),m_criteriaProcessor.GetMaxOutboundFltToAssignedRunway());
		m_wndCriteriaList.InsertItem(0,strValue);
		strValue.Format(_T("%d"),m_criteriaProcessor.GetMaxOutboundFltToAllRunway());
		m_wndCriteriaList.SetItemText(0,1,strValue);
	}
    
	return;
}

BOOL CDlgPushBackClearanceCriteria::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog(); 
	InitToolBar();

	SetResize(m_wndToolBar.GetDlgCtrlID(), SZ_TOP_LEFT, SZ_TOP_LEFT);
	//SetResize(IDC_STATIC_ALLINCLUDE, SZ_TOP_LEFT, SZ_BOTTOM_RIGHT);
	SetResize(IDC_SAVE, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDOK, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDCANCEL, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDC_TREE_CONDITION, SZ_TOP_LEFT, SZ_BOTTOM_LEFT);
	//SetResize(IDC_STATIC_VALUEINCLUDE, SZ_TOP_LEFT, SZ_BOTTOM_RIGHT);
	//SetResize(IDC_STATIC_FORANY, SZ_TOP_LEFT, SZ_TOP_LEFT);
	SetResize(IDC_STATIC_TOOLBAR_RECT, SZ_TOP_LEFT, SZ_TOP_LEFT);
	SetResize(IDC_LIST_CRITERIA_VALUE, SZ_TOP_LEFT, SZ_BOTTOM_RIGHT);
	SetResize(IDC_STATIC_RIGHTINCLUDE, SZ_TOP_LEFT, SZ_BOTTOM_RIGHT);
	SetResize(IDC_STATIC_FIRST, SZ_MIDDLE_CENTER, SZ_MIDDLE_CENTER);
	SetResize(IDC_EDIT1, SZ_MIDDLE_CENTER, SZ_MIDDLE_CENTER);
	SetResize(IDC_STATIC_SECOND, SZ_MIDDLE_CENTER, SZ_MIDDLE_CENTER);
	SetResize(IDC_EDIT2, SZ_MIDDLE_CENTER, SZ_MIDDLE_CENTER);
	SetResize(IDC_STATIC_SECOND_HEAD, SZ_MIDDLE_CENTER, SZ_MIDDLE_CENTER);
	SetResize(IDC_STATIC_FIRST_HEAD, SZ_MIDDLE_CENTER, SZ_MIDDLE_CENTER);

	m_bChangeItself = true;
	ReadData();
	InitializeTreeCtrl();	
	UpdateSelItem(m_hTreeSelItem);
	RebuildList(m_hTreeSelItem);

	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),TRUE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

int CDlgPushBackClearanceCriteria::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CXTResizeDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_wndToolBar.CreateEx(this);
	m_wndToolBar.LoadToolBar(IDR_TOOLBAR_ADD_DEL_EDIT);

	return 0;
}

void CDlgPushBackClearanceCriteria::OnDeleteItem(void)
{
	POSITION pos = m_wndCriteriaList.GetFirstSelectedItemPosition();
	int nItem = m_wndCriteriaList.GetNextSelectedItem(pos);
	CString strName = _T("");
	strName = m_wndCriteriaList.GetItemText(nItem,0);

	if(!strName.IsEmpty())	
	{		
		CString strNodeTitle = _T("");
		strNodeTitle = m_wndConditionTree.GetItemText(m_hTreeSelItem);
		if(_T("No content within") == strNodeTitle)
		{	
		}
		else if(_T("Takeoff queue length") == strNodeTitle)
		{
			m_criteriaProcessor.DeleteTakeoffQueueRunway(nItem);	
		}
		else if(_T("Deicing queue length") == strNodeTitle)
		{
			m_criteriaProcessor.DeleteDeicingQueueDeicePad(nItem);
		}
		else if(_T("Slot time") == strNodeTitle)
		{
			m_criteriaProcessor.DeleteSlotTimeFltType(nItem);	
		}
		else if(_T("Number of aircraft inbound") == strNodeTitle)
		{
			m_criteriaProcessor.DeleteStandForFltInbound(nItem);
		}
		else if(_T("Number of aircraft outbound") == strNodeTitle)
		{			
		}

		RebuildList(m_hTreeSelItem);
	}//end if
	else
	{
		CString strError = _T("No item selected in the list.");
		MessageBox(strError);
	}
}

void CDlgPushBackClearanceCriteria::OnNewItem(void)
{
	CString strNodeTitle = _T("");
	strNodeTitle = m_wndConditionTree.GetItemText(m_hTreeSelItem);
	if(_T("No content within") == strNodeTitle)
	{	
	}
	else if(_T("Takeoff queue length") == strNodeTitle)
	{
		CDlgSelectRunway dlg(m_nProjID);
		if(IDOK == dlg.DoModal())
		{
			m_criteriaProcessor.AddTakeoffQueueRunway(dlg.GetSelectedRunway(),dlg.GetSelectedRunwayMarkingIndex());
		}		
	}
	else if(_T("Deicing queue length") == strNodeTitle)
	{
		CDlgSelectALTObject dlg(m_nProjID, ALT_DEICEBAY,this);
		if (IDOK == dlg.DoModal())
		{
			m_criteriaProcessor.AddDeicingQueueDeicePadName(dlg.GetALTObject().GetIDString());
		}
	}
	else if(_T("Slot time") == strNodeTitle)
	{
		if(m_pSelectFlightType)
		{
			FlightConstraint fltType = (*m_pSelectFlightType)(NULL);
			char szBuffer[1024] = {0};
			fltType.WriteSyntaxStringWithVersion(szBuffer);
			CString strBuffer = szBuffer;
			//fltType.screenPrint(szBuffer);

			//strBuffer += _T("\\") ;//flight constraint ,save data and print data separate by '\\'
			//strBuffer += szBuffer;

			m_criteriaProcessor.AddSlotTimeFltTypeName(strBuffer);
		}		
	}
	else if(_T("Number of aircraft inbound") == strNodeTitle)
	{
		CDlgStandFamily dlg(m_nProjID);
		if(IDOK == dlg.DoModal())
		{
			if(dlg.GetSelStandFamilyID() == -1)
				m_criteriaProcessor.AddStandForFltInbound(_T(""));
			else
				m_criteriaProcessor.AddStandForFltInbound(dlg.GetSelStandFamilyName());
		}
		/*CDlgSelectALTObject dlg(m_nProjID, ALT_STAND,this);
		if (IDOK == dlg.DoModal())
		{
			m_criteriaProcessor.AddStandForFltInbound(dlg.GetALTObject().GetIDString());
		}*/
	}
	else if(_T("Number of aircraft outbound") == strNodeTitle)
	{			
	}

	RebuildList(m_hTreeSelItem);
}
void CDlgPushBackClearanceCriteria::OnSize(UINT nType, int cx, int cy)
{
	CXTResizeDialog::OnSize(nType, cx, cy);
	//InitToolBar();
}

void CDlgPushBackClearanceCriteria::ReadData(void)
{//unit convert
	if (m_nProjID == -1)
		return;
	m_criteriaProcessor.ReadData(m_nProjID);
	double dValue = 0.0;
	if(m_pGlobalUnits)
	{
		dValue = m_pGlobalUnits->ConvertLength(UM_LEN_CENTIMETERS,/*m_pGlobalUnits->GetLengthUnits()*/UM_LEN_FEET,m_criteriaProcessor.GetDistancNoPushBackWithin());
		m_criteriaProcessor.SetDistancNoPushBackWithin(dValue);
		m_criteriaProcessor.SetTimeNoLeadWithin(m_criteriaProcessor.GetTimeNoLeadWithin()/60);

		m_dDistancNoPushBackWithin = m_criteriaProcessor.GetDistancNoPushBackWithin();
		m_lTimeNoLeadWithin = m_criteriaProcessor.GetTimeNoLeadWithin();
		m_lMaxOutboundFltToAssignedRunway = m_criteriaProcessor.GetMaxOutboundFltToAssignedRunway();
		m_lMaxOutboundFltToAllRunway = m_criteriaProcessor.GetMaxOutboundFltToAllRunway();
		for(int iIndex = 0; iIndex < m_criteriaProcessor.GetSlotTimeCount();iIndex ++)
		{
			m_criteriaProcessor.SetMaxSlotTimeOfFltType(iIndex,m_criteriaProcessor.GetFltTypeMaxSlotTime(iIndex)/60);
		}
	}	
}

void CDlgPushBackClearanceCriteria::SaveData(void)
{//unit convert
	CString strNodeTitle = _T("");
	strNodeTitle = m_wndConditionTree.GetItemText(m_hTreeSelItem);
	CString strValue = _T("");
	if(_T("Other Aircraft PushBack") == strNodeTitle)
	{ 
		GetDlgItem(IDC_EDIT1)->GetWindowText(strValue);
		m_dDistancNoPushBackWithin = atof(strValue); 
		GetDlgItem(IDC_EDIT2)->GetWindowText(strValue);
		m_lTimeNoLeadWithin = atol(strValue); 
	}
	else if(_T("Number of aircraft outbound") == strNodeTitle)
	{			
		GetDlgItem(IDC_EDIT1)->GetWindowText(strValue);
		m_lMaxOutboundFltToAssignedRunway = atol(strValue);  
		GetDlgItem(IDC_EDIT2)->GetWindowText(strValue);
		m_lMaxOutboundFltToAllRunway = atol(strValue); 
	}
	m_criteriaProcessor.SetDistancNoPushBackWithin(m_dDistancNoPushBackWithin);
	m_criteriaProcessor.SetTimeNoLeadWithin(m_lTimeNoLeadWithin);
	m_criteriaProcessor.SetMaxOutboundFltToAssignedRunway(m_lMaxOutboundFltToAssignedRunway);
	m_criteriaProcessor.SetMaxOutboundFltToAllRunway(m_lMaxOutboundFltToAllRunway);

	if (m_nProjID == -1)
		return;
	double dValue = 0.0;
	if(m_pGlobalUnits)
	{
		dValue = m_pGlobalUnits->ConvertLength(/*m_pGlobalUnits->GetLengthUnits()*/UM_LEN_FEET,UM_LEN_CENTIMETERS,m_criteriaProcessor.GetDistancNoPushBackWithin());
		m_criteriaProcessor.SetDistancNoPushBackWithin(dValue);
		m_criteriaProcessor.SetTimeNoLeadWithin(m_criteriaProcessor.GetTimeNoLeadWithin()*60);

		for(int iIndex = 0; iIndex < m_criteriaProcessor.GetSlotTimeCount();iIndex ++)
		{
			m_criteriaProcessor.SetMaxSlotTimeOfFltType(iIndex,m_criteriaProcessor.GetFltTypeMaxSlotTime(iIndex)*60);
		}
	}

	try
	{
		CADODatabase::BeginTransaction() ;
		m_criteriaProcessor.SaveData();
		CADODatabase::CommitTransaction() ;
	}
	catch (CADOException e)
	{
		CADODatabase::RollBackTransation() ;
	}
	

	if(m_pGlobalUnits)
	{
		dValue = m_pGlobalUnits->ConvertLength(UM_LEN_CENTIMETERS,/*m_pGlobalUnits->GetLengthUnits()*/UM_LEN_FEET,m_criteriaProcessor.GetDistancNoPushBackWithin());
		m_criteriaProcessor.SetDistancNoPushBackWithin(dValue);
		m_criteriaProcessor.SetTimeNoLeadWithin(m_criteriaProcessor.GetTimeNoLeadWithin()/60);

		for(int iIndex = 0; iIndex < m_criteriaProcessor.GetSlotTimeCount();iIndex ++)
		{
			m_criteriaProcessor.SetMaxSlotTimeOfFltType(iIndex,m_criteriaProcessor.GetFltTypeMaxSlotTime(iIndex)/60);
		}
	}	
}

void CDlgPushBackClearanceCriteria::OnTvnSelchangedTreeCondition(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);

	HTREEITEM hSelItem = 0;
	hSelItem = m_wndConditionTree.GetSelectedItem();
	if(hSelItem != m_hTreeSelItem)
	{
		UpdateSelItem(hSelItem);
		RebuildList(hSelItem);
		m_hTreeSelItem = hSelItem;
	}

	*pResult = 0;
}

void CDlgPushBackClearanceCriteria::UpdateSelItem(HTREEITEM hSelItem)
{
	CString strNodeTitle = _T("");
	strNodeTitle = m_wndConditionTree.GetItemText(hSelItem);
	bool bTemp = m_bChangeItself;
	m_bChangeItself = false;
	if(_T("Other Aircraft PushBack") == strNodeTitle)
	{
		//GetDlgItem(IDC_STATIC_FORANY)->ShowWindow(SW_HIDE);
		/*GetDlgItem(IDC_STATIC_VALUEINCLUDE)->ShowWindow(SW_HIDE);*/
		GetDlgItem(IDC_LIST_CRITERIA_VALUE)->ShowWindow(SW_HIDE);
		m_wndToolBar.ShowWindow(SW_HIDE);

		
		CString strTitle = _T("");
		GetDlgItem(IDC_STATIC_FIRST_HEAD)->SetWindowText(_T("No other push back towards same taxiway ,"));
		strTitle.Format(_T("%s (ft) :"),_T("within distance")/*,m_pGlobalUnits->GetLengthUnitLongString(m_pGlobalUnits->GetLengthUnits()*/);
		GetDlgItem(IDC_STATIC_FIRST)->SetWindowText(strTitle);

		GetDlgItem(IDC_STATIC_SECOND_HEAD)->SetWindowText(_T("No taxing aircraft expected to be lead in line intersection ,")); 
		GetDlgItem(IDC_STATIC_SECOND)->SetWindowText(_T("within time (mins) :"));
		CString strValue = _T(""); 
		strValue.Format(_T("%.2f"),m_dDistancNoPushBackWithin);
		GetDlgItem(IDC_EDIT1)->SetWindowText(strValue); 
		strValue.Format(_T("%d"),m_lTimeNoLeadWithin);
		GetDlgItem(IDC_EDIT2)->SetWindowText(strValue);

		GetDlgItem(IDC_STATIC_FIRST_HEAD)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STATIC_FIRST)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_EDIT1)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STATIC_SECOND_HEAD)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STATIC_SECOND)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_EDIT2)->ShowWindow(SW_SHOW);	
	}
	else if(_T("Number of aircraft outbound") == strNodeTitle)
	{
		//GetDlgItem(IDC_STATIC_FORANY)->ShowWindow(SW_HIDE);
		//GetDlgItem(IDC_STATIC_VALUEINCLUDE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LIST_CRITERIA_VALUE)->ShowWindow(SW_HIDE);
		m_wndToolBar.ShowWindow(SW_HIDE);
		 
		
		GetDlgItem(IDC_STATIC_FIRST_HEAD)->SetWindowText(_T("Number of aircraft outbound to assigned runway ,"));
		GetDlgItem(IDC_STATIC_FIRST)->SetWindowText(_T("less than :"));
		GetDlgItem(IDC_STATIC_SECOND_HEAD)->SetWindowText(_T("Number of aircraft outbound to all runway ,"));
		GetDlgItem(IDC_STATIC_SECOND)->SetWindowText(_T("less than :"));
		CString strValue = _T(""); 
		strValue.Format(_T("%d"),m_lMaxOutboundFltToAssignedRunway);
		GetDlgItem(IDC_EDIT1)->SetWindowText(strValue); 
		strValue.Format(_T("%d"),m_lMaxOutboundFltToAllRunway);
		GetDlgItem(IDC_EDIT2)->SetWindowText(strValue);

		GetDlgItem(IDC_STATIC_FIRST_HEAD)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STATIC_FIRST)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_EDIT1)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STATIC_SECOND_HEAD)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STATIC_SECOND)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_EDIT2)->ShowWindow(SW_SHOW);	
	}
	else
	{
		//GetDlgItem(IDC_STATIC_FORANY)->ShowWindow(SW_SHOW);
		//GetDlgItem(IDC_STATIC_VALUEINCLUDE)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_LIST_CRITERIA_VALUE)->ShowWindow(SW_SHOW);
		m_wndToolBar.ShowWindow(SW_SHOW);

		GetDlgItem(IDC_STATIC_FIRST_HEAD)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_FIRST)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_EDIT1)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_SECOND_HEAD)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_SECOND)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_EDIT2)->ShowWindow(SW_HIDE); 
	}
	m_bChangeItself = bTemp;
}

void CDlgPushBackClearanceCriteria::OnBnClickedSave()
{	
	SaveData();
}

void CDlgPushBackClearanceCriteria::OnBnClickedOk()
{
	SaveData();
	CXTResizeDialog::OnOK();
}

void CDlgPushBackClearanceCriteria::OnLvnEndlabeleditListCriteriaValue(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	LV_ITEM* plvItem = &pDispInfo->item;
	CString strNodeTitle = _T("");
	strNodeTitle = m_wndConditionTree.GetItemText(m_hTreeSelItem);
	if(_T("Other Aircraft PushBack") == strNodeTitle)
	{	
		if(plvItem->iSubItem == 0)
			m_criteriaProcessor.SetDistancNoPushBackWithin(atof(plvItem->pszText));
		else
			m_criteriaProcessor.SetTimeNoLeadWithin(atol(plvItem->pszText));
	}
	else if(_T("Takeoff queue length") == strNodeTitle)
	{
		m_criteriaProcessor.SetTakeoffQueueRunwayOfQueueLength(plvItem->iItem,atoi(plvItem->pszText));
	}
	else if(_T("Deicing queue length") == strNodeTitle)
	{
		m_criteriaProcessor.SetDeicingQueueDeicePadOfQueueLength(plvItem->iItem,atoi(plvItem->pszText));
	}
	else if(_T("Slot time") == strNodeTitle)
	{
		m_criteriaProcessor.SetMaxSlotTimeOfFltType(plvItem->iItem,atoi(plvItem->pszText));
	}
	else if(_T("Number of aircraft inbound") == strNodeTitle)
	{
		m_criteriaProcessor.SetFltInboundNumberAtStand(plvItem->iItem,atoi(plvItem->pszText));
	}
	else if(_T("Number of aircraft outbound") == strNodeTitle)
	{
		if(plvItem->iSubItem == 0)
			m_criteriaProcessor.SetMaxOutboundFltToAssignedRunway(atol(plvItem->pszText));
		else
			m_criteriaProcessor.SetMaxOutboundFltToAllRunway(atol(plvItem->pszText));		
	}

	*pResult = 0;
}

void CDlgPushBackClearanceCriteria::OnEnChangeEdit1()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogResize::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	CString strNodeTitle = _T("");
	strNodeTitle = m_wndConditionTree.GetItemText(m_hTreeSelItem);
	CString strValue = _T("");
	if(!m_bChangeItself)return;
	if(_T("Other Aircraft PushBack") == strNodeTitle)
	{ 
		GetDlgItem(IDC_EDIT1)->GetWindowText(strValue);
		m_dDistancNoPushBackWithin = atof(strValue); 
	}
	else if(_T("Number of aircraft outbound") == strNodeTitle)
	{			
		GetDlgItem(IDC_EDIT1)->GetWindowText(strValue);
		m_lMaxOutboundFltToAssignedRunway = atol(strValue); 
	}
}

void CDlgPushBackClearanceCriteria::OnEnChangeEdit2()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogResize::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	CString strNodeTitle = _T("");
	strNodeTitle = m_wndConditionTree.GetItemText(m_hTreeSelItem);
	CString strValue = _T("");
	if(!m_bChangeItself)return;
	if(_T("Other Aircraft PushBack") == strNodeTitle)
	{  
		GetDlgItem(IDC_EDIT2)->GetWindowText(strValue);
		m_lTimeNoLeadWithin = atol(strValue); 
	}
	else if(_T("Number of aircraft outbound") == strNodeTitle)
	{ 
		GetDlgItem(IDC_EDIT2)->GetWindowText(strValue);
		m_lMaxOutboundFltToAllRunway = atol(strValue); 
	}
}
