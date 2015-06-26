// ServiceRequirementDlg.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "ServiceRequirementDlg.h"
#include "DlgFlightRelativeServiceLocations.h"
#include "../InputAirside/FlightServicingRequirementList.h"
#include "../InputAirside/VehicleSpecifications.h"

#include "../Common/ProbDistManager.h"
#include "../Common/WinMsg.h"
#include "..\common\ProbabilityDistribution.h"
#include "../Database/DBElementCollection_Impl.h"

using namespace std;

//char* conditionCategory[] = {
//	                         "/100 liters",
//							 "/10 pax",
//							 "duration of carts",
//							 "/10 bags",
//							 "per vehicle",
//							 "prior push to push"
//                             };
// CServiceRequirementDlg dialog
namespace
{
	const UINT ID_NEW_FLTTYPE  = 20;
	const UINT ID_DEL_FLTTYPE  = 21;
	const UINT ID_EDIT_FLTTYPE  = 22;
}

IMPLEMENT_DYNAMIC(CServiceRequirementDlg, CXTResizeDialog)
CServiceRequirementDlg::CServiceRequirementDlg(int nProjID, PSelectFlightType pSelectFlightType,
											   InputAirside* pInputAirside,
											   PSelectProbDistEntry pSelectProbDistEntry,
											   CAirportDatabase* pAirportDB,
											   CWnd* pParent /* = NULL */)
	: CXTResizeDialog(CServiceRequirementDlg::IDD, pParent)
    , m_pServicingRequirement(NULL)
	, m_pFlightServicingRequirement(NULL)
	, m_pAirportDB(pAirportDB)
	, m_pSelectFlightType(pSelectFlightType)
	, m_pInputAirside(pInputAirside)
	, m_pSelectProbDistEntry(pSelectProbDistEntry)
	, m_nProj(nProjID)
{
	m_pFltServicingRequirementList = new CFlightServicingRequirementList();
	m_pFltServicingRequirementList->SetAirportDatabase(pInputAirside->m_pAirportDB);
	
	m_pVehicleSpecifications = new CVehicleSpecifications();
	m_pVehicleSpecifications->ReadData(nProjID);

	m_pFltServicingRequirementList->ReadData(nProjID, m_pVehicleSpecifications);

	InitServiceTimeList();

	//init condition List
	for (int i=enumVehicleTypeCondition_Per100Liters; i<enumVehicleTypeCondition_Count; i++)
	{
		m_ConditionNameList.AddTail(VehicleConditionName[i]);
	}
}

CServiceRequirementDlg::~CServiceRequirementDlg()
{
}

void CServiceRequirementDlg::DoDataExchange(CDataExchange* pDX)
{
	CXTResizeDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_FLIGHTTYPE, m_ListBoxFltType);
	DDX_Control(pDX, IDC_LIST_SERVICEREQUIREMENT, m_ListFltTypeServiceRequirement);
}


BEGIN_MESSAGE_MAP(CServiceRequirementDlg, CXTResizeDialog)
	ON_WM_CREATE()
	ON_COMMAND(ID_NEW_FLTTYPE, OnCmdNewFltType)
	ON_COMMAND(ID_DEL_FLTTYPE, OnCmdDeleteFltType)
	ON_COMMAND(ID_EDIT_FLTTYPE, OnCmdEditFltType)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_LIST_SERVICEREQUIREMENT, OnLvnEndlabeleditListVehicleNum)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_SERVICEREQUIREMENT, OnLvnItemchangedListVehicleNum)
	ON_LBN_SELCHANGE(IDC_LIST_FLIGHTTYPE, OnLbnSelchangeListFlightType)
	ON_MESSAGE(WM_INPLACE_COMBO_KILLFOUCUS, OnMsgComboChange)
	ON_BN_CLICKED(IDC_BUTTON_SLS, OnBtnServiceLocationSpecification)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnBnClickedButtonSave)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CServiceRequirementDlg message handlers
int CServiceRequirementDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CXTResizeDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_ToolBarFltType.CreateEx(this, TBSTYLE_FLAT,WS_CHILD|WS_VISIBLE|CBRS_ALIGN_TOP)
		|| !m_ToolBarFltType.LoadToolBar(IDR_TOOLBAR_ADD_DEL_EDIT))
	{
		return -1;
	}

	CToolBarCtrl& toolbarFltType = m_ToolBarFltType.GetToolBarCtrl();
	toolbarFltType.SetCmdID(0, ID_NEW_FLTTYPE);
	toolbarFltType.SetCmdID(1, ID_DEL_FLTTYPE);
	toolbarFltType.SetCmdID(2, ID_EDIT_FLTTYPE);

	return 0;
}

BOOL CServiceRequirementDlg::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog();

	InitToolBar();
	InitListCtrl();
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(FALSE);

	//SetResize(m_ToolBarFltType.GetDlgCtrlID(), SZ_TOP_CENTER, SZ_TOP_CENTER);

	SetResize(IDC_BUTTON_SAVE, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDOK, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDCANCEL, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDC_BUTTON_SLS, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);

	SetResize(IDC_STATIC_FLTTYPE, SZ_TOP_LEFT, SZ_BOTTOM_CENTER);
	SetResize(IDC_LIST_FLIGHTTYPE, SZ_TOP_LEFT, SZ_BOTTOM_CENTER);

	SetResize(IDC_LIST_SERVICEREQUIREMENT, SZ_TOP_CENTER, SZ_BOTTOM_RIGHT);

	//init flt type
	SetFlightType();

	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),TRUE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),FALSE);

	return TRUE;
}

void CServiceRequirementDlg::InitToolBar(void)
{
	//Runway exit and stand family
	CRect rect;
	m_ListBoxFltType.GetWindowRect( &rect );
	ScreenToClient( &rect );
	rect.top -= 26;
	rect.bottom = rect.top + 22;
	rect.left += 4;
	m_ToolBarFltType.MoveWindow(&rect);

	m_ToolBarFltType.GetToolBarCtrl().EnableButton( ID_NEW_FLTTYPE, TRUE);
	m_ToolBarFltType.GetToolBarCtrl().EnableButton( ID_DEL_FLTTYPE, FALSE );
	m_ToolBarFltType.GetToolBarCtrl().EnableButton( ID_EDIT_FLTTYPE, FALSE );
}

void CServiceRequirementDlg::InitListCtrl(void)
{
	// set list control window style
	DWORD dwStyle = m_ListFltTypeServiceRequirement.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES;
	m_ListFltTypeServiceRequirement.SetExtendedStyle(dwStyle);

	CStringList strList;
	strList.RemoveAll();
	CString strCaption;

	LV_COLUMNEX lvc;
	lvc.mask = LVCF_WIDTH|LVCF_TEXT;

	strCaption.LoadString(IDS_NAME);
	lvc.pszText = (LPSTR)(LPCTSTR)strCaption;
	lvc.cx = 100;
	lvc.fmt = LVCFMT_NOEDIT;
	lvc.csList = &strList;
	m_ListFltTypeServiceRequirement.InsertColumn(0, &lvc);

	strCaption.LoadString(IDS_NUMBER);
	lvc.pszText = (LPSTR)(LPCTSTR)strCaption;
	lvc.cx = 100;
	lvc.fmt = LVCFMT_EDIT;
	m_ListFltTypeServiceRequirement.InsertColumn(1, &lvc);

	strCaption.LoadString(IDS_AIRSERVICETIME);
	lvc.pszText = (LPSTR)(LPCTSTR)strCaption;
	lvc.cx = 150;
	lvc.fmt = LVCFMT_DROPDOWN;
	lvc.csList = &m_ServiceTimeList;
	m_ListFltTypeServiceRequirement.InsertColumn(2, &lvc);

	strCaption.LoadString(IDS_SERVICETIME);
	lvc.pszText = (LPSTR)(LPCTSTR)strCaption;
	lvc.cx = 190;
	lvc.fmt = LVCFMT_NOEDIT;
	lvc.csList = &m_ServiceTimeList;
	m_ListFltTypeServiceRequirement.InsertColumn(3, &lvc);

	//condition
	strCaption.LoadString(IDS_CONDITION);
	lvc.pszText = (LPSTR)(LPCTSTR)strCaption;
	lvc.cx = 130;
	lvc.fmt = LVCFMT_DROPDOWN;
	lvc.csList = &m_ConditionNameList;
	m_ListFltTypeServiceRequirement.InsertColumn(4, &lvc);
}

//if is new item set bIsNewItem true,else set false
void CServiceRequirementDlg::SetListContent(bool bIsNewItem)
{
	ASSERT(m_pFlightServicingRequirement);

	m_ListFltTypeServiceRequirement.DeleteAllItems();

	for (size_t i=0; i<m_pFlightServicingRequirement->GetElementCount(); i++)
	{
		CServicingRequirement *pServicingRequirement = m_pFlightServicingRequirement->GetItem(i);

		//Get VehicleName
		CString strVehiclename;	
		for (size_t j=0; j<m_pVehicleSpecifications->GetElementCount(); j++)
		{
			CVehicleSpecificationItem *pVehiSepcItem = m_pVehicleSpecifications->GetItem(j);

			if (pServicingRequirement->GetServicingRequirementNameID() == pVehiSepcItem->GetID())
			{
				strVehiclename = pVehiSepcItem->getName();

				//Insert one row
				m_ListFltTypeServiceRequirement.InsertItem(i, strVehiclename);
				m_ListFltTypeServiceRequirement.SetItemData(i, (DWORD_PTR)pServicingRequirement);
				//enumVehicleBaseType enumBaseType = pVehiSepcItem->getBaseType();
				CString strNum;
			
				if (-1 == pServicingRequirement->GetNum())
				{
					strNum.Format("variable");
				}
				else
				{
					strNum.Format("%d", pServicingRequirement->GetNum());
				}
				
				CString strCondition;
				//new item
				if (bIsNewItem)
				{
					//get condition
					/*InitConditionCol(enumBaseType, pServicingRequirement);*/
					m_pFlightServicingRequirement->SetDefaultValue(pServicingRequirement, pVehiSepcItem);
					strCondition = VehicleConditionName[pServicingRequirement->GetConditionType()];

				}
				//old item
				else
				{
					//compatible with old version
					if (pServicingRequirement->GetConditionType() >= enumVehicleTypeCondition_Count)
					{
						strCondition = VehicleConditionName[0];
					}
					else
					{
						//m_pFlightServicingRequirement->SetDefaultValue(pServicingRequirement, pVehiSepcItem);
						if(pServicingRequirement->GetConditionType() == enumVehicleTypeCondition_DurationOfCarts)
							pServicingRequirement->SetConditionType(enumVehicleTypeCondition_Per10Bags);
						strCondition = VehicleConditionName[pServicingRequirement->GetConditionType()];
					}
				}
				
				m_ListFltTypeServiceRequirement.SetItemText(i, 1, strNum);
				m_ListFltTypeServiceRequirement.SetItemText(i, 2, pServicingRequirement->GetDistScreenPrint());
				if(strVehiclename == "BAGGAGE TUG"|| strVehiclename == "PAX BUS A"||strVehiclename == "PAX BUS B"||strVehiclename == "PAX BUS C")
				{
					m_ListFltTypeServiceRequirement.SetItemText(i, 3, pServicingRequirement->GetSubDistScreenPrint());
				}
// 				else
// 					m_ListFltTypeServiceRequirement.SetItemState(i,LVCFMT_NOEDIT,LVCFMT_NOEDIT);
				m_ListFltTypeServiceRequirement.SetItemText(i, 4, strCondition);
				

				break;
			}			
		}
		
	}
}

//void CServiceRequirementDlg::InitConditionCol(enumVehicleBaseType enumBaseType,enumVehicleUnit enumUnit, CServicingRequirement* pServicingRequirement)
//{
//	ASSERT(pServicingRequirement);
//
//	if (enumBaseType == VehicleType_FuelTruck)
//	{
//		pServicingRequirement->SetConditionType(enumVehicleTypeCondition_Per100Liters);
//	}
//	else if (enumBaseType == VehicleType_CateringTruck
//		|| enumBaseType == VehicleType_Stairs
//		|| enumBaseType == VehicleType_PaxTruck)
//	{
//		pServicingRequirement->SetConditionType(enumVehicleTypeCondition_Per10Pax);
//
//	}
//	else if (enumBaseType == VehicleType_BaggageTug)
//	{
//		pServicingRequirement->SetConditionType(enumVehicleTypeCondition_DurationOfCarts);
//
//	}
//	else if (enumBaseType == VehicleType_BaggageCart
//		|| enumBaseType == VehicleType_Conveyor)
//	{
//		pServicingRequirement->SetConditionType(enumVehicleTypeCondition_Per10Bags);
//
//	}
//	else if (enumBaseType == VehicleType_MaintenanceTruck
//		|| enumBaseType == VehicleType_Lift
//		|| enumBaseType == VehicleType_CargoTug
//		|| enumBaseType == VehicleType_CargoULDCart
//		|| enumBaseType == VehicleType_StaffCar
//		|| enumBaseType == VehicleType_CrewBus
//		|| enumBaseType == VehicleType_DeicingTruck)
//	{
//		pServicingRequirement->SetConditionType(enumVehicleTypeCondition_PerVehicle);
//
//	}
//	else if (enumBaseType == VehicleType_TowTruck)
//	{
//		pServicingRequirement->SetConditionType(enumVehicleTypeCondition_PriorPushToPush);
//
//	}
//	else
//	{
//		pServicingRequirement->SetConditionType(enumVehicleTypeCondition_Per100Liters);
//
//	}
//	
//
//}

void CServiceRequirementDlg::OnCmdNewFltType()
{
	if (m_pSelectFlightType == NULL)
		return;

	FlightConstraint fltType = (*m_pSelectFlightType)(NULL);

	char szBuffer[1024] = {0};
	fltType.screenPrint(szBuffer);


	//find the flttype
	if (LB_ERR != m_ListBoxFltType.FindString(-1, szBuffer))
	{
		MessageBox(_T("The Flight Type already exists!"));

		return;
	}

	int nAddStringIndex = m_ListBoxFltType.AddString(szBuffer);
	m_ListBoxFltType.SetCurSel(nAddStringIndex);

	m_ToolBarFltType.GetToolBarCtrl().EnableButton( ID_DEL_FLTTYPE, TRUE );
	m_ToolBarFltType.GetToolBarCtrl().EnableButton( ID_EDIT_FLTTYPE, TRUE );

	m_pFlightServicingRequirement = new CFlightServicingRequirement(fltType, m_pVehicleSpecifications);
	//m_pFlightServicingRequirement->SetFltType(fltType);

	////Add vehicle specifications
	//int nCount = m_pVehicleSpecifications->GetElementCount();
	//for (int i=0; i<nCount; i++)
	//{
	//	CVehicleSpecificationItem *pVehicleSpecItem = m_pVehicleSpecifications->GetItem(i);

	//	CServicingRequirement *pServicingRequirement = new CServicingRequirement;

	//	pServicingRequirement->SetServicingRequirementNameID(pVehicleSpecItem->GetID());

	//	m_pFlightServicingRequirement->AddNewItem(pServicingRequirement);
	//}

	m_pFltServicingRequirementList->AddNewItem(m_pFlightServicingRequirement);

	m_ListBoxFltType.SetItemData(nAddStringIndex, (DWORD_PTR)m_pFlightServicingRequirement);

	SetListContent(true);

	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
}

void CServiceRequirementDlg::OnCmdDeleteFltType()
{
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);

	int nCurSel = m_ListBoxFltType.GetCurSel();

	if (LB_ERR == nCurSel)
	{
		return;
	}

	m_pFlightServicingRequirement = (CFlightServicingRequirement*)m_ListBoxFltType.GetItemData(nCurSel);
	ASSERT(m_pFlightServicingRequirement);

	m_pFltServicingRequirementList->DeleteItem(m_pFlightServicingRequirement);

	m_ListBoxFltType.DeleteString(nCurSel);

	if (0 == m_ListBoxFltType.GetCount())
	{
		m_ToolBarFltType.GetToolBarCtrl().EnableButton( ID_DEL_FLTTYPE, FALSE );
		m_ToolBarFltType.GetToolBarCtrl().EnableButton( ID_EDIT_FLTTYPE, FALSE );
	}

	//
	m_ListFltTypeServiceRequirement.DeleteAllItems();
	//m_ServiceTimeList.RemoveAll();
	//m_ServiceTimeMap.clear();
	m_pFlightServicingRequirement = NULL;
	m_pServicingRequirement = NULL;
}

void CServiceRequirementDlg::OnCmdEditFltType()
{
	if (m_pSelectFlightType == NULL)
		return;

	FlightConstraint fltType = (*m_pSelectFlightType)(NULL);

	char szBuffer[1024] = {0};
	fltType.screenPrint(szBuffer);

	CString strDefault;
	strDefault.Format("%s", szBuffer);
	////haven't modify
	//if (!strDefault.CompareNoCase(_T("DEFAULT")))
	//{
	//	return;
	//}


	//the flight type is exist
	if (LB_ERR != m_ListBoxFltType.FindString(-1, szBuffer))
		//&& m_ListBoxFltType.GetCurSel()!=m_ListBoxFltType.FindString(-1, szBuffer))
	{
		MessageBox(_T("The Flight Type already exists!"));

		return;
	}

	int nCurSel = m_ListBoxFltType.GetCurSel();

	m_pFlightServicingRequirement = (CFlightServicingRequirement*)m_ListBoxFltType.GetItemData(nCurSel);
	ASSERT(m_pFlightServicingRequirement);

	m_pFltServicingRequirementList->DeleteItem(m_pFlightServicingRequirement);

	m_ListBoxFltType.DeleteString(nCurSel);
	int nModifyCurSel = m_ListBoxFltType.InsertString(nCurSel, szBuffer);

	m_ListBoxFltType.SetCurSel(nModifyCurSel);

	m_pFlightServicingRequirement = new CFlightServicingRequirement;
	m_pFlightServicingRequirement->SetFltType(fltType);

	m_ListBoxFltType.SetItemData(nModifyCurSel, (DWORD_PTR)m_pFlightServicingRequirement);

	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
}

void CServiceRequirementDlg::OnLvnEndlabeleditListVehicleNum(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;

	if (NULL == m_pServicingRequirement)
	{
		return;
	}


	LV_ITEM* plvItem = &pDispInfo->item;
	int nItem = plvItem->iItem;
	int nSubItem = plvItem->iSubItem;
	if (nItem < 0)
		return;

	CString strItemData;
	int nItemData = 1;
	//variable
	if (!m_ListFltTypeServiceRequirement.GetItemText(nItem, nSubItem).CompareNoCase("variable"))
	{
		strItemData.Format("variable");
		nItemData = -1;
	}
	else
	{
		try
		{
			nItemData = atoi(m_ListFltTypeServiceRequirement.GetItemText(nItem, nSubItem));
		}
		catch (CException* e)
		{
			CString strErrorMsg;
			e->GetErrorMessage((LPTSTR)(LPCTSTR)strErrorMsg, 255);
			nItemData = 1;
		}
		strItemData.Format("%d", nItemData);
	}	
	m_ListFltTypeServiceRequirement.SetItemText(nItem, nSubItem, strItemData);

	switch(nSubItem) 
	{
	case 1:
		m_pServicingRequirement->SetNum(nItemData);
		break;

	default:
		break;
	}

	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
}

void CServiceRequirementDlg::OnLvnItemchangedListVehicleNum(NMHDR *pNMHDR, LRESULT *pResult)
{
	int nCurSel = m_ListFltTypeServiceRequirement.GetCurSel();

	if (LB_ERR == nCurSel
		|| m_ListFltTypeServiceRequirement.GetItemCount()-1 < nCurSel)
	{
		m_pServicingRequirement = NULL;
		return;
	}
	
	m_pServicingRequirement = (CServicingRequirement*)m_ListFltTypeServiceRequirement.GetItemData(nCurSel);

	LVCOLDROPLIST *pColumnStyle = m_ListFltTypeServiceRequirement.GetColumnStyle(3);
	if(pColumnStyle)
	{
		CVehicleSpecificationItem *pVehiSepcItem = m_pVehicleSpecifications->GetItem(m_pServicingRequirement->GetServicingRequirementNameID()-1);
		if (pVehiSepcItem)
		{
			CString strVehiclename = pVehiSepcItem->getName();
			if(strVehiclename == "BAGGAGE TUG"|| strVehiclename == "PAX BUS A"||strVehiclename == "PAX BUS B"||strVehiclename == "PAX BUS C")
				pColumnStyle->Style = DROP_DOWN;
			else
				pColumnStyle->Style = NO_EDIT;
		}
	}
}

void CServiceRequirementDlg::OnLbnSelchangeListFlightType()
{
	int nCurSel = m_ListBoxFltType.GetCurSel();

	if (LB_ERR == nCurSel
		|| m_ListBoxFltType.GetCount()-1 < nCurSel)
	{
		m_pFlightServicingRequirement = NULL;
		return ;
	}

	m_pFlightServicingRequirement = (CFlightServicingRequirement*)m_ListBoxFltType.GetItemData(nCurSel);
	SetListContent(false);
}

void CServiceRequirementDlg::InitServiceTimeList(void)
{
	m_ServiceTimeList.RemoveAll();
	m_ServiceTimeList.AddTail("NEW PROBABILITY DISTRIBUTION");

	m_ServiceTimeMap.clear();

	m_ServiceTimeMap.insert(make_pair(0, m_ServiceTimeList.GetTail()));

	CProbDistManager* pProbDistMan = m_pInputAirside->m_pAirportDB->getProbDistMan();
	int nCount = static_cast<int>(pProbDistMan ->getCount());

	for( int m=0; m<nCount; m++ )
	{
		CProbDistEntry* pPBEntry = pProbDistMan->getItem( m );
		m_ServiceTimeList.AddTail(pPBEntry->m_csName);
		m_ServiceTimeMap[m+1]= m_ServiceTimeList.GetTail();
	}
}

LRESULT CServiceRequirementDlg::OnMsgComboChange(WPARAM wParam, LPARAM lParam)
{
	int nComboxSel = (int)wParam;
	if (nComboxSel == LB_ERR)
		return 0;

	if (NULL == m_pServicingRequirement)
	{
		return 0;
	}


	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)lParam;
	LV_ITEM* plvItem = &pDispInfo->item;

	int nItem = plvItem->iItem;
	int nSubItem = plvItem->iSubItem;
	if (nItem < 0)
		return 0;

	switch(nSubItem)
	{
	case 2:
		{
			ProbabilityDistribution* pProbDist = NULL;
			CProbDistManager* pProbDistMan = m_pInputAirside->m_pAirportDB->getProbDistMan();

			//if select the first item
			if( nComboxSel == 0 )
			{
				CProbDistEntry* pPDEntry = NULL;
				pPDEntry = (*m_pSelectProbDistEntry)(NULL, m_pInputAirside);
				if(pPDEntry == NULL)
					return 0;
				pProbDist = pPDEntry->m_pProbDist;
				assert( pProbDist );

				CString strDistName = pPDEntry->m_csName;
				//if(strDistName == m_strDistName)
				//	return;

				//m_strDistName = strDistName;
				char szBuffer[1024] = {0};
				pProbDist->screenPrint(szBuffer);
				m_pServicingRequirement->SetDistScreenPrint(szBuffer);
				m_pServicingRequirement->SetProbTypes((ProbTypes)pProbDist->getProbabilityType());
				pProbDist->printDistribution(szBuffer);
				m_pServicingRequirement->SetPrintDist(szBuffer);

				//m_strDistScreenPrint = szBuffer;
				//----------------
				//int nIndex = 0;
				//if ((nIndex=m_ComBoDistribution.FindString(nIndex, m_strDistName)) != CB_ERR)
				//	m_ComBoDistribution.SetCurSel( nIndex );	
				//else
				//{
				//	nIndex = m_ComBoDistribution.AddString(m_strDistName);
				//	m_ComBoDistribution.SetCurSel(nIndex);
				//}
				InitServiceTimeList();

				m_ListFltTypeServiceRequirement.SetItemText(plvItem->iItem, plvItem->iSubItem, strDistName);
				m_pServicingRequirement->SetServiceTime(strDistName);

			}
			else
			{
				map<int, CString>::const_iterator iter = m_ServiceTimeMap.find(nComboxSel);
				CString strServiceTime = iter->second;
				m_pServicingRequirement->SetServiceTime(strServiceTime);

				CProbDistEntry* pPDEntry = NULL;
				int nCount = pProbDistMan->getCount();
				for( int i=0; i<nCount; i++ )
				{
					pPDEntry = pProbDistMan->getItem( i );
					if( strcmp( pPDEntry->m_csName, strServiceTime ) == 0 )
						break;
				}
				//assert( i < nCount );
				pProbDist = pPDEntry->m_pProbDist;
				assert( pProbDist );
				char szBuffer[1024] = {0};
				pProbDist->screenPrint(szBuffer);
				m_pServicingRequirement->SetDistScreenPrint(szBuffer);
				m_pServicingRequirement->SetProbTypes((ProbTypes)pProbDist->getProbabilityType());
				pProbDist->printDistribution(szBuffer);
				m_pServicingRequirement->SetPrintDist(szBuffer);
			}
		}
		break;

	case 3:
		{
			ProbabilityDistribution* pProbDist = NULL;
			CProbDistManager* pProbDistMan = m_pInputAirside->m_pAirportDB->getProbDistMan();

			//if select the first item
			if( nComboxSel == 0 )
			{
				CProbDistEntry* pPDEntry = NULL;
				pPDEntry = (*m_pSelectProbDistEntry)(NULL, m_pInputAirside);
				if(pPDEntry == NULL)
					return 0;
				pProbDist = pPDEntry->m_pProbDist;
				assert( pProbDist );

				CString strDistName = pPDEntry->m_csName;
				//if(strDistName == m_strDistName)
				//	return;

				//m_strDistName = strDistName;
				char szBuffer[1024] = {0};
				pProbDist->screenPrint(szBuffer);
				m_pServicingRequirement->SetSubDistScreenPrint(szBuffer);
				m_pServicingRequirement->SetSubProbTypes((ProbTypes)pProbDist->getProbabilityType());
				pProbDist->printDistribution(szBuffer);
				m_pServicingRequirement->SetSubPrintDist(szBuffer);

				//m_strDistScreenPrint = szBuffer;
				//----------------
				//int nIndex = 0;
				//if ((nIndex=m_ComBoDistribution.FindString(nIndex, m_strDistName)) != CB_ERR)
				//	m_ComBoDistribution.SetCurSel( nIndex );	
				//else
				//{
				//	nIndex = m_ComBoDistribution.AddString(m_strDistName);
				//	m_ComBoDistribution.SetCurSel(nIndex);
				//}
				InitServiceTimeList();

				m_ListFltTypeServiceRequirement.SetItemText(plvItem->iItem, plvItem->iSubItem, strDistName);
				m_pServicingRequirement->SetSubServiceTime(strDistName);

			}
			else
			{
				map<int, CString>::const_iterator iter = m_ServiceTimeMap.find(nComboxSel);
				CString strServiceTime = iter->second;
				m_pServicingRequirement->SetSubServiceTime(strServiceTime);

				CProbDistEntry* pPDEntry = NULL;
				int nCount = pProbDistMan->getCount();
				for( int i=0; i<nCount; i++ )
				{
					pPDEntry = pProbDistMan->getItem( i );
					if( strcmp( pPDEntry->m_csName, strServiceTime ) == 0 )
						break;
				}
				//assert( i < nCount );
				pProbDist = pPDEntry->m_pProbDist;
				assert( pProbDist );
				char szBuffer[1024] = {0};
				pProbDist->screenPrint(szBuffer);
				m_pServicingRequirement->SetSubDistScreenPrint(szBuffer);
				m_pServicingRequirement->SetSubProbTypes((ProbTypes)pProbDist->getProbabilityType());
				pProbDist->printDistribution(szBuffer);
				m_pServicingRequirement->SetSubPrintDist(szBuffer);
			}
		}
		break;
	//condition
	case 4:
		{
			enumVehicleTypeCondition nConditionType = enumVehicleTypeCondition_Per100Liters;
			if (nComboxSel >= enumVehicleTypeCondition_Per100Liters && nComboxSel < enumVehicleTypeCondition_Count)
			{
				nConditionType = (enumVehicleTypeCondition)nComboxSel;
			}

			m_pServicingRequirement->SetConditionType(nConditionType);
		}
	    break;

	default:
	    break;
	}

	//InitListCtrl();
	SetListContent(false);

	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);

	return 0;
}


void CServiceRequirementDlg::OnBtnServiceLocationSpecification()
{
	// display the CDlgFlightRelativeServiceLocations
	CDlgFlightRelativeServiceLocations dlg(m_nProj, m_pSelectFlightType, m_pAirportDB);
	dlg.DoModal();
}


void CServiceRequirementDlg::OnBnClickedButtonSave()
{
	// TODO: Add your control notification handler code here
	try
	{
		CADODatabase::BeginTransaction();
		m_pFltServicingRequirementList->SaveData(m_nProj);
		CADODatabase::CommitTransaction();

	}
	catch (CADOException &e)
	{
		CADODatabase::RollBackTransation();
		e.ErrorMessage();
	}

	GetDlgItem(IDC_SAVE)->EnableWindow(FALSE);
}

void CServiceRequirementDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	OnBnClickedButtonSave();

	OnOK();
}

void CServiceRequirementDlg::SetFlightType(void)
{
	ASSERT(m_pFltServicingRequirementList);

	if (0 < m_pFltServicingRequirementList->GetElementCount())
	{
		m_ToolBarFltType.GetToolBarCtrl().EnableButton( ID_DEL_FLTTYPE, TRUE );
		m_ToolBarFltType.GetToolBarCtrl().EnableButton( ID_EDIT_FLTTYPE, TRUE );
	}

	for (size_t i=0; i<m_pFltServicingRequirementList->GetElementCount(); i++)
	{
		CFlightServicingRequirement *pFltSR = m_pFltServicingRequirementList->GetItem(i);
		FlightConstraint fltType = pFltSR->GetFltType();

		char szBuffer[1024] = {0};
		fltType.screenPrint(szBuffer);

		int nAddStringIndex = m_ListBoxFltType.AddString(szBuffer);
		m_ListBoxFltType.SetItemData(nAddStringIndex, (DWORD_PTR)pFltSR);
	}
}
