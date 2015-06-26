// DlgTakeoffClearanceItems.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"
#include "DlgTakeoffClearanceItems.h"
#include "../InputAirside/runway.h"
#include "DlgApproachRunwaySelection.h"
#include ".\dlgtakeoffclearanceitems.h"

// CDlgTakeoffClearanceItems dialog
static UINT ID_NEW_ITEM = 12;
static UINT ID_DEL_ITEM = 13;
static UINT ID_EDIT_ITEM = 14;

IMPLEMENT_DYNAMIC(CDlgTakeoffClearanceItems, CTabPageSSL)
CDlgTakeoffClearanceItems::CDlgTakeoffClearanceItems(int nClearanceTypeListIdx,InputAirside* pInputAirside, PSelectProbDistEntry pSelectProbDistEntry,CWnd* pParent /*=NULL*/)
: CTabPageSSL(CDlgTakeoffClearanceItems::IDD, pParent),
m_nClearanceTypeListIdx(nClearanceTypeListIdx),
m_pInputAirside(pInputAirside),
m_pSelectProbDistEntry(pSelectProbDistEntry)
, m_strLeadDistance(_T(""))
, m_strAfterTime(_T(""))
, m_strAppDistance(_T(""))
, m_strAppTime(_T(""))
, m_strDepartureTime(_T(""))
,m_unitManager(CARCUnitManager::GetInstance())
{
	m_pCurNode = 0;
	m_pCurItem = 0;
}

CDlgTakeoffClearanceItems::~CDlgTakeoffClearanceItems()
{
}

void CDlgTakeoffClearanceItems::DoDataExchange(CDataExchange* pDX)
{
	CTabPageSSL::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_BASIS, m_wndListCtrlBasis);
	DDX_Control(pDX, IDC_LIST_APPROACH, m_wndListCtrlApproach);
	DDX_Text(pDX, IDC_EDIT2, m_strLeadDistance);
	DDX_Text(pDX, IDC_EDIT3, m_strAfterTime);
	DDX_Text(pDX, IDC_EDIT4, m_strAppDistance);
	DDX_Text(pDX, IDC_EDIT5, m_strAppTime);
	DDX_Text(pDX, IDC_EDIT6, m_strDepartureTime);
}


BEGIN_MESSAGE_MAP(CDlgTakeoffClearanceItems, CTabPageSSL)
	ON_WM_CREATE()
	ON_CBN_SELCHANGE(IDC_COMBO1, OnCbnSelchangeCombo1)
	ON_COMMAND(ID_NEW_ITEM,OnNewItem)
	ON_COMMAND(ID_DEL_ITEM,OnDelItem)
	ON_COMMAND(ID_EDIT_ITEM,OnEditItem)
	ON_NOTIFY(LVN_ENDLABELEDIT,IDC_LIST_BASIS,OnSelComboBox)
	ON_NOTIFY(NM_CLICK, IDC_LIST_APPROACH, OnNMClickListApproach)
	ON_NOTIFY(NM_CLICK, IDC_LIST_BASIS, OnNMClickListBasis)
	ON_BN_CLICKED(IDC_RADIO1, OnBnClickedRadio1)
	ON_BN_CLICKED(IDC_RADIO2, OnBnClickedRadio2)
	ON_BN_CLICKED(IDC_RADIO4, OnBnClickedRadio4)
	ON_BN_CLICKED(IDC_RADIO5, OnBnClickedRadio5)
	ON_EN_CHANGE(IDC_EDIT2, OnEnChangeEdit2)
	ON_EN_CHANGE(IDC_EDIT3, OnEnChangeEdit3)
	ON_EN_CHANGE(IDC_EDIT4, OnEnChangeEdit4)
	ON_EN_CHANGE(IDC_EDIT5, OnEnChangeEdit5)
	ON_EN_CHANGE(IDC_EDIT6, OnEnChangeEdit6)
END_MESSAGE_MAP()


// CDlgTakeoffClearanceItems message handlers


// CDlgApproachSeparationItems message handlers
int CDlgTakeoffClearanceItems::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CTabPageSSL::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndToolbar.CreateEx(this, TBSTYLE_FLAT,WS_CHILD|WS_VISIBLE|CBRS_ALIGN_TOP)
		|| !m_wndToolbar.LoadToolBar(IDR_TOOLBAR_APPROACH))
	{
		return -1;
	}


	return 0;
}


BOOL CDlgTakeoffClearanceItems::OnInitDialog()
{
	CTabPageSSL::OnInitDialog();

	CRect rectToolbar;
	GetDlgItem(IDC_STATIC_TOOLBAR)->GetWindowRect(&rectToolbar);
	ScreenToClient(&rectToolbar);
	m_wndToolbar.MoveWindow(&rectToolbar, true);
	m_wndToolbar.GetToolBarCtrl().SetCmdID(0,ID_NEW_ITEM);
	m_wndToolbar.GetToolBarCtrl().SetCmdID(1,ID_DEL_ITEM);
	m_wndToolbar.GetToolBarCtrl().SetCmdID(2,ID_EDIT_ITEM);
	m_wndToolbar.GetToolBarCtrl().EnableButton(ID_NEW_ITEM, TRUE);
	m_wndToolbar.GetToolBarCtrl().EnableButton(ID_DEL_ITEM, TRUE);
	m_wndToolbar.GetToolBarCtrl().EnableButton(ID_EDIT_ITEM, FALSE);

	((CComboBox*)(GetDlgItem(IDC_COMBO1)))->AddString(_T("WingSpan"));//	0
	((CComboBox*)(GetDlgItem(IDC_COMBO1)))->AddString(_T("Surface bearing weight"));	//1
	((CComboBox*)(GetDlgItem(IDC_COMBO1)))->AddString(_T("Wake Vortex"));	//2	
	((CComboBox*)(GetDlgItem(IDC_COMBO1)))->AddString(_T("Approach speed"));//3
	((CComboBox*)(GetDlgItem(IDC_COMBO1)))->AddString(_T("None"));//4

	InitApproachListCtrl();
	InitBasisListCtrl();
	SetApproachListCtrlContent();

	SetResize(IDC_STATIC_TOOLBAR, SZ_TOP_LEFT, SZ_TOP_LEFT);
	SetResize(m_wndToolbar.GetDlgCtrlID(), SZ_TOP_LEFT, SZ_TOP_LEFT);
	SetResize(IDC_LIST_APPROACH, SZ_TOP_LEFT, SZ_MIDDLE_RIGHT);
	SetResize(IDC_STATIC_CLEAR_TO_POS, SZ_MIDDLE_LEFT, SZ_MIDDLE_RIGHT);
	SetResize(IDC_STATIC_FT1, SZ_MIDDLE_LEFT, SZ_MIDDLE_LEFT);
	SetResize(IDC_RADIO1, SZ_MIDDLE_LEFT, SZ_MIDDLE_LEFT);
	SetResize(IDC_RADIO2, SZ_MIDDLE_LEFT, SZ_MIDDLE_LEFT);
	SetResize(IDC_RADIO4, SZ_MIDDLE_LEFT, SZ_MIDDLE_LEFT);
	SetResize(IDC_RADIO5, SZ_MIDDLE_LEFT, SZ_MIDDLE_LEFT);	
	SetResize(IDC_STATIC_SEC1, SZ_MIDDLE_LEFT, SZ_MIDDLE_LEFT);
	SetResize(IDC_STATIC_ATR1, SZ_MIDDLE_LEFT, SZ_MIDDLE_LEFT);
	SetResize(IDC_STATIC_NM2, SZ_MIDDLE_LEFT, SZ_MIDDLE_LEFT);
	SetResize(IDC_EDIT2, SZ_MIDDLE_LEFT, SZ_MIDDLE_LEFT);
	SetResize(IDC_EDIT3, SZ_MIDDLE_LEFT, SZ_MIDDLE_LEFT);
	SetResize(IDC_EDIT4, SZ_MIDDLE_LEFT, SZ_MIDDLE_LEFT);
	SetResize(IDC_EDIT5, SZ_MIDDLE_LEFT, SZ_MIDDLE_LEFT);	
	SetResize(IDC_EDIT6, SZ_MIDDLE_LEFT, SZ_MIDDLE_LEFT);
	SetResize(IDC_SPIN1, SZ_MIDDLE_LEFT, SZ_MIDDLE_LEFT);
	SetResize(IDC_SPIN2, SZ_MIDDLE_LEFT, SZ_MIDDLE_LEFT);
	SetResize(IDC_SPIN3, SZ_MIDDLE_LEFT, SZ_MIDDLE_LEFT);
	SetResize(IDC_SPIN4, SZ_MIDDLE_LEFT, SZ_MIDDLE_LEFT);	
	SetResize(IDC_SPIN5, SZ_MIDDLE_LEFT, SZ_MIDDLE_LEFT);
	SetResize(IDC_STATIC_DTS1, SZ_MIDDLE_LEFT, SZ_MIDDLE_LEFT);
	SetResize(IDC_STATIC_SEC3, SZ_MIDDLE_LEFT, SZ_MIDDLE_LEFT);
	SetResize(IDC_STATIC_SEC2, SZ_MIDDLE_LEFT, SZ_MIDDLE_LEFT);

	SetResize(IDC_STATIC_CLEAR_TO_IF_EX,SZ_MIDDLE_LEFT ,SZ_BOTTOM_RIGHT );	
	SetResize(IDC_STATIC_CBASIS, SZ_MIDDLE_LEFT, SZ_MIDDLE_LEFT);
	SetResize(IDC_COMBO1, SZ_MIDDLE_LEFT, SZ_MIDDLE_LEFT);
	SetResize(IDC_LIST_BASIS, SZ_MIDDLE_LEFT, SZ_BOTTOM_RIGHT);

	return TRUE; 
}

void CDlgTakeoffClearanceItems::SetApproachListCtrlContent(void)
{
	m_wndListCtrlApproach.DeleteAllItems();
	if (m_pTakeoffClearanceData) {
		CTakeoffClaaranceRunwayNode* pNode = 0;
		int nCount = m_pTakeoffClearanceData->GetNodeCount();
		for (int i = 0;i < nCount;++i) {
			pNode = m_pTakeoffClearanceData->GetNode(i);
			if (pNode) {
				Runway runway;
				runway.ReadObject(pNode->m_nFirstRunwayID);

				CString strTitle = runway.GetObjNameString() + _T(" with mark:");
				if (pNode->m_emFirstRunwayMark == RUNWAYMARK_FIRST) 
					strTitle = runway.GetMarking1().c_str();
				else
					strTitle = runway.GetMarking2().c_str();
				m_wndListCtrlApproach.InsertItem(i,strTitle);

				runway.ReadObject(pNode->m_nSecondRunwayID);
				strTitle = runway.GetObjNameString() + _T(" with mark:");
				if (pNode->m_emSecondRunwayMark == RUNWAYMARK_FIRST) 
					strTitle = runway.GetMarking1().c_str();
				else
					strTitle = runway.GetMarking2().c_str();
				m_wndListCtrlApproach.SetItemText(i,1,strTitle);

				if(m_pTakeoffClearanceCriteria)
				{
					RunwayRelationType emRRT;
					RunwayIntersectionType emRIT;
					double dRw1Threshold = -1.0,dRw2Threshold = -1.0;
					CPoint2008 InterP;
					emRRT = m_pTakeoffClearanceCriteria->m_runwaySeparationRelation.GetLogicRunwayRelationship(pNode->m_emTakeoffClearanceType,pNode->m_nFirstRunwayID,pNode->m_emFirstRunwayMark,pNode->m_nSecondRunwayID,pNode->m_emSecondRunwayMark,emRIT,InterP, dRw1Threshold,dRw2Threshold);
					{

						m_wndListCtrlApproach.SetItemText(i,2,m_pTakeoffClearanceCriteria->m_runwaySeparationRelation.GetApproachRelationTypeString(emRRT));
						m_wndListCtrlApproach.SetItemText(i,3,m_pTakeoffClearanceCriteria->m_runwaySeparationRelation.GetApproachIntersectionTypeString(emRIT));

						pNode->m_emRRT = emRRT;
						pNode->m_emRIT = emRIT;
						pNode->m_dRw1ThresholdFromInterNode = dRw1Threshold;
						pNode->m_dRw2ThresholdFromInterNode = dRw2Threshold;

						CString strTemp =_T("");
						if (dRw1Threshold >= 0) 
						{
							double dbTmp = CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT, m_unitManager.GetCurSelLengthUnit(),dRw1Threshold);
							strTemp.Format(_T("%.2f"),dbTmp);

							//strTemp.Format(_T("%.2f"),dRw1Threshold);
						}
						else
							strTemp = _T("N/A");
						m_wndListCtrlApproach.SetItemText(i,4,strTemp);
						if (dRw2Threshold >= 0) 
						{
							double dbTmp = CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT, m_unitManager.GetCurSelLengthUnit(),dRw2Threshold);
							strTemp.Format(_T("%.2f"),dbTmp);						
							//strTemp.Format(_T("%.2f"),dRw2Threshold);
						}
						else
							strTemp = _T("N/A");
						m_wndListCtrlApproach.SetItemText(i,5,strTemp);
					}
					//else
					//{
					//	m_wndListCtrlApproach.SetItemText(i,2,_T(" "));
					//	m_wndListCtrlApproach.SetItemText(i,3,_T(" "));
					//	m_wndListCtrlApproach.SetItemText(i,4,_T(" "));
					//	m_wndListCtrlApproach.SetItemText(i,5,_T(" "));
					//}
				}
				else
				{
					m_wndListCtrlApproach.SetItemText(i,2,_T(" "));
					m_wndListCtrlApproach.SetItemText(i,3,_T(" "));
					m_wndListCtrlApproach.SetItemText(i,4,_T(" "));
					m_wndListCtrlApproach.SetItemText(i,5,_T(" "));
				}

				m_wndListCtrlApproach.SetItemData(i,(DWORD_PTR)pNode);
			}
		}

		m_wndListCtrlApproach.SetItemState(nCount - 1, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
		m_pCurNode = m_pTakeoffClearanceData->GetNode(nCount-1);
		if (m_pCurNode) {
			if (m_pCurNode->m_bClearByTimeOrDistance) {
				((CButton*)(GetDlgItem(IDC_RADIO1)))->SetCheck(BST_CHECKED);
				((CButton*)(GetDlgItem(IDC_RADIO2)))->SetCheck(BST_UNCHECKED);
				m_strLeadDistance.Format(_T("%.2f"),m_pCurNode->m_dLeadDistance);
			}else{
				((CButton*)(GetDlgItem(IDC_RADIO1)))->SetCheck(BST_UNCHECKED);
				((CButton*)(GetDlgItem(IDC_RADIO2)))->SetCheck(BST_CHECKED);
				m_strAfterTime.Format(_T("%.2f"),m_pCurNode->m_dTimeAfter);
			}


			if (m_pCurNode->m_bApproachByTimeOrDistance) {
				((CButton*)(GetDlgItem(IDC_RADIO4)))->SetCheck(BST_CHECKED);
				((CButton*)(GetDlgItem(IDC_RADIO5)))->SetCheck(BST_UNCHECKED);
				m_strAppDistance.Format(_T("%.2f"),m_pCurNode->m_dApproachDistance);
			}else{
				((CButton*)(GetDlgItem(IDC_RADIO4)))->SetCheck(BST_UNCHECKED);
				((CButton*)(GetDlgItem(IDC_RADIO5)))->SetCheck(BST_CHECKED);
				m_strAppTime.Format(_T("%.2f"),m_pCurNode->m_dApproachTime);
			}
			m_strDepartureTime.Format(_T("%.2f"),m_pCurNode->m_dDepartureTime);
			UpdateData(FALSE);

			((CComboBox*)(GetDlgItem(IDC_COMBO1)))->SetCurSel(m_pCurNode->m_emClassType);
		}
		if (m_pCurNode)
		{
			SetBasisListCtrlContent(m_pCurNode->m_emClassType);
		}
	}
}

void CDlgTakeoffClearanceItems::SetBasisListCtrlContent(FlightClassificationBasisType emClassType)
{
	m_wndListCtrlBasis.DeleteAllItems();
	if(m_pCurNode) {
		m_pCurNode->MakeDataValid(emClassType);
		std::vector <CTakeoffClaaranceSeparationItem *> vTakeOffItem = m_pCurNode->GetItemCompounding(emClassType);
		CTakeoffClaaranceSeparationItem * pItem = 0;
		int nCount = (int)vTakeOffItem.size();
		for (int i = 0;i < nCount;++i) {
			pItem = vTakeOffItem.at(i);
			if (pItem) {
				m_wndListCtrlBasis.InsertItem(i,pItem->m_aciLead.getName());
				m_wndListCtrlBasis.SetItemText(i,1,pItem->m_aciTrail.getName());
				CString strTemp = _T("");
				strTemp.Format(_T("%.2f"),pItem->m_dMinDistance);
				m_wndListCtrlBasis.SetItemText(i,2,strTemp);
				m_wndListCtrlBasis.SetItemText(i,3,pItem->m_strDisDistName);
				strTemp.Format(_T("%.2f"),pItem->m_dMinTime);
				m_wndListCtrlBasis.SetItemText(i,4,strTemp);
				m_wndListCtrlBasis.SetItemText(i,5,pItem->m_strTimeDistName);

				m_wndListCtrlBasis.SetItemData(i,(DWORD_PTR)pItem);
			}
		}

		if (nCount > 0)
		{
			m_wndListCtrlBasis.SetItemState(nCount - 1, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
			m_pCurItem = vTakeOffItem.at(nCount-1);
		}
	}
}

void CDlgTakeoffClearanceItems::InitApproachListCtrl()
{
	DWORD dwStyle = m_wndListCtrlApproach.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES;
	m_wndListCtrlApproach.SetExtendedStyle(dwStyle);
	CStringList strlist;
	LV_COLUMNEX lvc;
	lvc.mask = LVCF_WIDTH|LVCF_TEXT|LVCF_FMT;
	lvc.cx = 120;
	lvc.fmt = LVCFMT_NOEDIT;
	lvc.csList = &strlist;

	if (m_pTakeoffClearanceData) {
		lvc.pszText = "Taking off AC runway";
		m_wndListCtrlApproach.InsertColumn(0, &lvc);
		if (m_pTakeoffClearanceData->m_emTakeoffClearanceType == AIRCRAFTOPERATION_TYPE_TAKEOFF_BEHIND_LANDED) 
			lvc.pszText = "Landed AC runway";
		else if (m_pTakeoffClearanceData->m_emTakeoffClearanceType == AIRCRAFTOPERATION_TYPE_TAKEOFF_BEHIND_TAKEOFF) 
			lvc.pszText = "Taken off AC runway";
		else
			lvc.pszText = "AC runway";
		m_wndListCtrlApproach.InsertColumn(1, &lvc);

		lvc.pszText = "Relation";
		m_wndListCtrlApproach.InsertColumn(2, &lvc);
		lvc.pszText = "Intersection";
		m_wndListCtrlApproach.InsertColumn(3, &lvc);

		lvc.cx = 200;
		lvc.pszText = "Distance from Landing threshold";
		m_wndListCtrlApproach.InsertColumn(4, &lvc);

		if (m_pTakeoffClearanceData->m_emTakeoffClearanceType == AIRCRAFTOPERATION_TYPE_TAKEOFF_BEHIND_LANDED) 
			lvc.pszText = "Distance from Landed threshold";
		else if (m_pTakeoffClearanceData->m_emTakeoffClearanceType == AIRCRAFTOPERATION_TYPE_TAKEOFF_BEHIND_TAKEOFF) 
			lvc.pszText = "Distance from Take off threshold";
		else
			lvc.pszText = "Distance from threshold";
		m_wndListCtrlApproach.InsertColumn(5, &lvc);
	}

}


void CDlgTakeoffClearanceItems::InitBasisListCtrl()
{
	DWORD dwStyle = m_wndListCtrlBasis.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES;
	m_wndListCtrlBasis.SetExtendedStyle(dwStyle);
	CStringList strlist;
	CProbDistManager* pProbDistMan = 0;	
	CProbDistEntry* pPDEntry = 0;
	int nCount = -1;
	if(m_pInputAirside && m_pInputAirside->m_pAirportDB)
		pProbDistMan = m_pInputAirside->m_pAirportDB->getProbDistMan();
	strlist.AddTail(_T("New Probability Distribution..."));
	if(pProbDistMan)
		nCount = pProbDistMan->getCount();
	for( int i=0; i< nCount; i++ )
	{
		pPDEntry = pProbDistMan->getItem( i );
		if(pPDEntry)
			strlist.AddTail(pPDEntry->m_csName);
	}

	for (int i = m_wndListCtrlBasis.GetHeaderCtrl()->GetItemCount();i > 0;i--)
		m_wndListCtrlBasis.DeleteColumn(0);
	LV_COLUMNEX lvc;
	lvc.mask = LVCF_WIDTH|LVCF_TEXT|LVCF_FMT;
	lvc.cx = 120;
	lvc.fmt = LVCFMT_NOEDIT;
	lvc.csList = &strlist;
	
	lvc.pszText = "Take off AC";
	m_wndListCtrlBasis.InsertColumn(0, &lvc);

	lvc.pszText = "Other AC";
	m_wndListCtrlBasis.InsertColumn(1, &lvc);

	lvc.fmt = LVCFMT_NUMBER;
	lvc.pszText = "Min Distance(nm)";
	m_wndListCtrlBasis.InsertColumn(2, &lvc);

	lvc.fmt = LVCFMT_DROPDOWN;
	lvc.pszText = "Dis Distribution(nm)";
	m_wndListCtrlBasis.InsertColumn(3, &lvc);

	lvc.fmt = LVCFMT_NUMBER;
	lvc.pszText = "Min Time(sec)";
	m_wndListCtrlBasis.InsertColumn(4, &lvc);

	lvc.fmt = LVCFMT_DROPDOWN;
	lvc.pszText = "Time Distribution";
	m_wndListCtrlBasis.InsertColumn(5, &lvc);
}

void CDlgTakeoffClearanceItems::OnNMClickListApproach(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	int nRow = -1;
	nRow = m_wndListCtrlApproach.GetCurSel();

	m_wndListCtrlApproach.SetItemState(nRow, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);	

	if (m_pTakeoffClearanceData){
		m_pCurNode = m_pTakeoffClearanceData->GetNode(nRow);
		if (m_pCurNode) {
			if (m_pCurNode->m_bClearByTimeOrDistance) {
				((CButton*)(GetDlgItem(IDC_RADIO1)))->SetCheck(BST_CHECKED);
				((CButton*)(GetDlgItem(IDC_RADIO2)))->SetCheck(BST_UNCHECKED);
				m_strLeadDistance.Format(_T("%.2f"),m_pCurNode->m_dLeadDistance);
			}else{
				((CButton*)(GetDlgItem(IDC_RADIO1)))->SetCheck(BST_UNCHECKED);
				((CButton*)(GetDlgItem(IDC_RADIO2)))->SetCheck(BST_CHECKED);
				m_strAfterTime.Format(_T("%.2f"),m_pCurNode->m_dTimeAfter);
			}


			if (m_pCurNode->m_bApproachByTimeOrDistance) {
				((CButton*)(GetDlgItem(IDC_RADIO4)))->SetCheck(BST_CHECKED);
				((CButton*)(GetDlgItem(IDC_RADIO5)))->SetCheck(BST_UNCHECKED);
				m_strAppDistance.Format(_T("%.2f"),m_pCurNode->m_dApproachDistance);
			}else{
				((CButton*)(GetDlgItem(IDC_RADIO4)))->SetCheck(BST_UNCHECKED);
				((CButton*)(GetDlgItem(IDC_RADIO5)))->SetCheck(BST_CHECKED);
				m_strAppTime.Format(_T("%.2f"),m_pCurNode->m_dApproachTime);
			}
			m_strDepartureTime.Format(_T("%.2f"),m_pCurNode->m_dDepartureTime);
			UpdateData(FALSE);

			((CComboBox*)(GetDlgItem(IDC_COMBO1)))->SetCurSel(m_pCurNode->m_emClassType);
			SetBasisListCtrlContent(m_pCurNode->m_emClassType);
		}
	}
	*pResult = 0;
}

void CDlgTakeoffClearanceItems::OnNMClickListBasis(NMHDR *pNMHDR, LRESULT *pResult)
{	
	int nRow = -1;
	nRow = m_wndListCtrlBasis.GetCurSel();

	m_wndListCtrlBasis.SetItemState(nRow, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);	
	if (nRow > -1) 
		m_pCurItem = (CTakeoffClaaranceSeparationItem*)m_wndListCtrlBasis.GetItemData(nRow);

	*pResult = 0;
}

void CDlgTakeoffClearanceItems::OnCbnSelchangeCombo1()
{
	if (m_pCurNode) {
		m_pCurNode->m_emClassType = (FlightClassificationBasisType) (((CComboBox*)(GetDlgItem(IDC_COMBO1)))->GetCurSel() );
		SetBasisListCtrlContent(m_pCurNode->m_emClassType);
	}
}

void CDlgTakeoffClearanceItems::OnNewItem(void)
{
	if (m_pTakeoffClearanceData) {
		CDlgApproachRunwaySelection dlgSelRunway(m_pTakeoffClearanceData->GetProjectID(),m_pTakeoffClearanceData->m_emTakeoffClearanceType,this);
		dlgSelRunway.SetFlag(0x1);
		if (IDOK == dlgSelRunway.DoModal()) {
			CTakeoffClaaranceRunwayNode * pNewNode = new CTakeoffClaaranceRunwayNode(m_pTakeoffClearanceData->GetProjectID(),m_pTakeoffClearanceData->m_emTakeoffClearanceType);
			if (pNewNode) {
				pNewNode->m_nFirstRunwayID = dlgSelRunway.m_nFirstRunwayID;
				pNewNode->m_emFirstRunwayMark = dlgSelRunway.m_emFirstRunwayMark;
				pNewNode->m_nSecondRunwayID = dlgSelRunway.m_nSecondRunwayID;
				pNewNode->m_emSecondRunwayMark = dlgSelRunway.m_emSecondRunwayMark;
				
				/*Runway runway;
				runway.ReadObject(pNewNode->m_nFirstRunwayID);
				std::vector<Runway::RefPtr> interRunways = runway.GetIntersectRunways();
				for (std::vector<Runway::RefPtr>::iterator itrInterRunway = interRunways.begin();itrInterRunway != interRunways.end();++itrInterRunway) {
				if ((*itrInterRunway)->getID() == pNewNode->m_nSecondRunwayID ){
				pNewNode->m_bLAHSO = true;
				break;
				}
				}*/
				pNewNode->GetItemCompounding(pNewNode->m_emClassType);
				m_pTakeoffClearanceData->AddNode(pNewNode);
				SetApproachListCtrlContent();
			}
		}
	}
}

void CDlgTakeoffClearanceItems::OnDelItem(void)
{
	if(m_pTakeoffClearanceData)
		m_pTakeoffClearanceData->DeleteNode(m_pCurNode);
	SetApproachListCtrlContent();
}

void CDlgTakeoffClearanceItems::OnEditItem(void)
{
}

void CDlgTakeoffClearanceItems::OnSelComboBox( NMHDR * pNotifyStruct, LRESULT * result )
{
	LV_DISPINFO* dispinfo = (LV_DISPINFO * )pNotifyStruct;
	*result = 0;
	if(!dispinfo)
		return;

    //number edit
    if (dispinfo->item.iSubItem == 2)
    {
        if(m_pCurItem)
        {
            if(dispinfo->item.pszText != NULL)
            {
                m_pCurItem->m_dMinDistance = atof(dispinfo->item.pszText);
                CString strTmp;
                strTmp.Format(_T("%.02f"), m_pCurItem->m_dMinDistance);
                m_wndListCtrlBasis.SetItemText(dispinfo->item.iItem, dispinfo->item.iSubItem, strTmp);
                return;
            }
            else
            {
                return;
            }
        }
    }
    else if (dispinfo->item.iSubItem == 4)
    {
        if(m_pCurItem)
        {
            if(dispinfo->item.pszText != NULL)
            {
                m_pCurItem->m_dMinTime = atof(dispinfo->item.pszText);
                CString strTmp;
                strTmp.Format(_T("%.02f"), m_pCurItem->m_dMinTime);
                m_wndListCtrlBasis.SetItemText(dispinfo->item.iItem, dispinfo->item.iSubItem, strTmp);
                return;
            }
            else
            {
                return;
            }
        }
    }


	//distribution edit
	CString strSel;
	strSel = dispinfo->item.pszText;
	CString strDistName = _T("");

	char szBuffer[1024] = {0};
	ProbabilityDistribution* pProbDist = NULL;
	CProbDistManager* pProbDistMan = m_pInputAirside->m_pAirportDB->getProbDistMan();
	if(strSel == _T("New Probability Distribution..." ) )
	{
		CProbDistEntry* pPDEntry = NULL;
		pPDEntry = (*m_pSelectProbDistEntry)(NULL, m_pInputAirside);
		if(pPDEntry == NULL)
		{
			SetBasisListCtrlContent(m_pCurNode->m_emClassType);
			return;
		}
		pProbDist = pPDEntry->m_pProbDist;
		assert( pProbDist );
		strDistName = pPDEntry->m_csName;

		pProbDist->printDistribution(szBuffer);
		if(dispinfo->item.iSubItem == 3)
		{
			if(m_pCurItem){
				m_pCurItem->m_strDisDistName = strDistName;
				m_pCurItem->m_enumDisProbType = (ProbTypes)pProbDist->getProbabilityType();
				m_pCurItem->m_strDisPrintDist = szBuffer;
			}
		}
		else if(dispinfo->item.iSubItem == 5)
		{
			if(m_pCurItem){
				m_pCurItem->m_strTimeDistName = strDistName;
				m_pCurItem->m_enumTimeProbType = (ProbTypes)pProbDist->getProbabilityType();
				m_pCurItem->m_strTimePrintDist = szBuffer;
			}
		}  		
	}
	else
	{
		//if(m_strDistName == strSel)
		//	return;
		CProbDistEntry* pPDEntry = NULL;

		int nCount = pProbDistMan->getCount();
		for( int i=0; i<nCount; i++ )
		{
			pPDEntry = pProbDistMan->getItem( i );
			if(pPDEntry->m_csName == strSel)
				break;
		}
		//assert( i < nCount );
		pProbDist = pPDEntry->m_pProbDist;
		assert( pProbDist );

		strDistName = pPDEntry->m_csName;	
		pProbDist->printDistribution(szBuffer);
		if(dispinfo->item.iSubItem == 3)
		{
			if(m_pCurItem){
				m_pCurItem->m_strDisDistName = strDistName;
				m_pCurItem->m_enumDisProbType = (ProbTypes)pProbDist->getProbabilityType();
				m_pCurItem->m_strDisPrintDist = szBuffer;
			}
		}
		else if(dispinfo->item.iSubItem == 5)
		{
			if(m_pCurItem){
				m_pCurItem->m_strTimeDistName = strDistName;
				m_pCurItem->m_enumTimeProbType = (ProbTypes)pProbDist->getProbabilityType();
				m_pCurItem->m_strTimePrintDist = szBuffer;
			}
		}  		 
	}

	InitBasisListCtrl();
	SetBasisListCtrlContent(m_pCurNode->m_emClassType);
	/*if(m_pCurItem)
		m_wndListCtrlBasis.SetItemText(dispinfo->item.iItem,dispinfo->item.iSubItem,strDistName);*/
}

void CDlgTakeoffClearanceItems::OnBnClickedRadio1()
{
	if (m_pCurNode) {
		m_pCurNode->m_bClearByTimeOrDistance = true;
		m_strLeadDistance.Format(_T("%.2f"),m_pCurNode->m_dLeadDistance);
		m_strAfterTime = _T("");
		UpdateData(FALSE);
	}
}

void CDlgTakeoffClearanceItems::OnBnClickedRadio2()
{
	if (m_pCurNode) {
		m_pCurNode->m_bClearByTimeOrDistance = false;
		m_strLeadDistance = _T("");
		m_strAfterTime.Format(_T("%.2f"),m_pCurNode->m_dTimeAfter);
		UpdateData(FALSE);
	}
}

void CDlgTakeoffClearanceItems::OnBnClickedRadio4()
{
	if (m_pCurNode) {
		m_pCurNode->m_bApproachByTimeOrDistance = true;
		m_strAppDistance.Format(_T("%.2f"),m_pCurNode->m_dApproachDistance);
		m_strAppTime = _T("");
		UpdateData(FALSE);
	}
}

void CDlgTakeoffClearanceItems::OnBnClickedRadio5()
{
	if (m_pCurNode) {
		m_pCurNode->m_bApproachByTimeOrDistance = false;
		m_strAppDistance = _T("");
		m_strAppTime.Format(_T("%.2f"),m_pCurNode->m_dApproachTime);
		UpdateData(FALSE);
	}
}

void CDlgTakeoffClearanceItems::OnEnChangeEdit2()
{
	double dTemp = -1.0;

	CString strLeadDistance = _T("");
	GetDlgItem(IDC_EDIT2)->GetWindowText(strLeadDistance);
	dTemp = _tstof(strLeadDistance);
	if (dTemp < 0.0)
		dTemp = 0.0;
	if (m_pCurNode) 
		m_pCurNode->m_dLeadDistance = dTemp;
}

void CDlgTakeoffClearanceItems::OnEnChangeEdit3()
{
	double dTemp = -1.0;

	CString strAfterTime = _T("");
	GetDlgItem(IDC_EDIT3)->GetWindowText(strAfterTime);
	dTemp = _tstof(strAfterTime);
	if (dTemp < 0.0)
		dTemp = 0.0;
	if (m_pCurNode) 
		m_pCurNode->m_dTimeAfter = dTemp;
}

void CDlgTakeoffClearanceItems::OnEnChangeEdit4()
{
	double dTemp = -1.0;

	CString strAppDistance = _T("");
	GetDlgItem(IDC_EDIT4)->GetWindowText(strAppDistance);
	dTemp = _tstof(strAppDistance);
	if (dTemp < 0.0)
		dTemp = 0.0;
	if (m_pCurNode) 
		m_pCurNode->m_dApproachDistance = dTemp;
}

void CDlgTakeoffClearanceItems::OnEnChangeEdit5()
{
	double dTemp = -1.0;

	CString strAppTime = _T("");
	GetDlgItem(IDC_EDIT5)->GetWindowText(strAppTime);
	dTemp = _tstof(strAppTime);
	if (dTemp < 0.0)
		dTemp = 0.0;
	if (m_pCurNode) 
		m_pCurNode->m_dApproachTime = dTemp;
}

void CDlgTakeoffClearanceItems::OnEnChangeEdit6()
{
	double dTemp = -1.0;

	CString strDepartureTime = _T("");
	GetDlgItem(IDC_EDIT6)->GetWindowText(strDepartureTime);
	dTemp = _tstof(strDepartureTime);
	if (dTemp < 0.0)
		dTemp = 0.0;
	if (m_pCurNode) 
		m_pCurNode->m_dDepartureTime = dTemp;
}

void CDlgTakeoffClearanceItems::SetTakeoffClearanceCriteria( CTakeoffClearanceCriteria* pData )
{
	m_pTakeoffClearanceCriteria = pData;
	m_pTakeoffClearanceData = pData->m_vrTakeoffClearanceTypeList.at(m_nClearanceTypeListIdx);
}