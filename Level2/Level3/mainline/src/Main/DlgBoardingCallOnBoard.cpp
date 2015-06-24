// DlgBoardingCallOnBoard.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "TermPlanDoc.h"
#include "DlgBoardingCallOnBoard.h"
#include "DlgSeatBlock.h"
#include "../InputOnboard/OnBoardAnalysisCondidates.h"
#include "../Inputs/flight.h"
#include "../Inputs/IN_TERM.H"
#include "FlightDialog.h"
#include "SelectALTObjectDialog.h"
#include "DlgNewPassengerType.h"
#include "../Common/ProbDistEntry.h"
#include "../Common/ProbabilityDistribution.h"
#include "../Common/ProbDistManager.h"
#include "DlgProbDist.h"
#include "..\inputs\probab.h"

// CDlgBoardingCallOnBoard dialog

IMPLEMENT_DYNAMIC(CDlgBoardingCallOnBoard, CXTResizeDialog)
CDlgBoardingCallOnBoard::CDlgBoardingCallOnBoard(int nProjID,InputTerminal* pInterm,CAirportDatabase* pAirportDB,COnBoardAnalysisCandidates* _pOnBoardList,CWnd* pParent /*=NULL*/)
	: CXTResizeDialog(CDlgBoardingCallOnBoard::IDD, pParent)
	,m_pOnBoardList(_pOnBoardList)
	,m_boardingCall(pAirportDB,pInterm->inStrDict)
	,m_nProjID(nProjID)
	,m_pInterm(pInterm)
{

}

CDlgBoardingCallOnBoard::~CDlgBoardingCallOnBoard()
{
}

void CDlgBoardingCallOnBoard::DoDataExchange(CDataExchange* pDX)
{
	CXTResizeDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_DATA, m_wndTreeCtrl);
}


BEGIN_MESSAGE_MAP(CDlgBoardingCallOnBoard, CXTResizeDialog)
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_BUTTON_BLOCK,OnBtnBlock)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_SAVE,OnBtnSave)
	ON_COMMAND(ID_MODEL_NEW,OnAddStage)
	ON_COMMAND(ID_MODEL_DELETE,OnRemoveStage)
	ON_COMMAND(ID_MODEL_EDIT,OnEditStage)
	ON_COMMAND(ID_ADD_FLIGHTTYPE,OnAddFlightType)
	ON_COMMAND(ID_DELETE_FLIGHTTYPE,OnDeleteFlightType)
	ON_COMMAND(ID_ADD_STAND,OnAddStand)
	ON_COMMAND(ID_EDIT_STAND,OnEditStand)
	ON_COMMAND(ID_DELETE_STAND,OnDeleteStand)
	ON_COMMAND(ID_ADD_PASSENGERTYPE,OnAddPaxType)
	ON_COMMAND(ID_ADD_SEATBLOCK,OnAddSeatBlock)
	ON_COMMAND(ID_DELETE_SEATBLOCK,OnDeleteSeatBlock)
END_MESSAGE_MAP()


// CDlgBoardingCallOnBoard message handlers

BOOL CDlgBoardingCallOnBoard::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog();

	SetResize(IDC_STATIC_GROUP,SZ_TOP_LEFT,SZ_BOTTOM_RIGHT);
	SetResize(m_wndToolBar.GetDlgCtrlID(),SZ_TOP_LEFT,SZ_TOP_LEFT);
	SetResize(IDC_TREE_DATA,SZ_TOP_LEFT,SZ_BOTTOM_RIGHT);
	SetResize(IDC_BUTTON_BLOCK,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);
	SetResize(IDC_BUTTON_SAVE,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);
	SetResize(IDOK,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);
	SetResize(IDCANCEL,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);
	OnInitToolbar();
	m_boardingCall.ReadData(m_nProjID);
	OnInitTreeCtrl();
	UpdateToolbarStatus();
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_BLOCK)->EnableWindow(FALSE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),TRUE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),FALSE);
	return TRUE;
}


int CDlgBoardingCallOnBoard::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CXTResizeDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_ALIGN_TOP
		| CBRS_TOOLTIPS ) ||
		!m_wndToolBar.LoadToolBar(IDR_DATABASE_MODEL))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	return 0;
}

void CDlgBoardingCallOnBoard::OnInitToolbar()
{
	CRect rect;
	m_wndTreeCtrl.GetWindowRect(&rect);
	ScreenToClient(&rect);
	rect.bottom = rect.top - 6;
	rect.top = rect.bottom - 22;
	rect.left = rect.left + 2;
	m_wndToolBar.MoveWindow(&rect,true);
	m_wndToolBar.ShowWindow(SW_SHOW);

	m_wndToolBar.GetToolBarCtrl().EnableButton(ID_MODEL_NEW,TRUE);
	m_wndToolBar.GetToolBarCtrl().EnableButton(ID_MODEL_DELETE,TRUE);
	m_wndToolBar.GetToolBarCtrl().EnableButton(ID_MODEL_EDIT,TRUE);
}

void CDlgBoardingCallOnBoard::OnBtnBlock()
{
	HTREEITEM hItem = m_wndTreeCtrl.GetSelectedItem();
	COnBoardingCallFlight* pFlight = (COnBoardingCallFlight*)m_wndTreeCtrl.GetItemData(hItem);

	CDlgSeatBlock dlg(&pFlight->getSeatBlockList(),pFlight->GetFlightAnalysis().getID());
	dlg.DoModal();
	GetDlgItem(IDC_BUTTON_BLOCK)->EnableWindow(FALSE);
}

void CDlgBoardingCallOnBoard::OnAddStage()
{
	HTREEITEM hItem = m_wndTreeCtrl.GetRootItem();
	COnBoardingCallStage* pItem = new COnBoardingCallStage();
	int nStage = m_boardingCall.getInvalidStageIndex();
	pItem->setStage(nStage);
	m_boardingCall.addItem(pItem);

	CString strStage = _T("");
	COOLTREE_NODE_INFO cni;
	CARCBaseTree::InitNodeInfo(this,cni);
	cni.unMenuID = IDR_MENU_ONBOARD_STAGE;
	strStage.Format(_T("Stage %d"),nStage);
	HTREEITEM hStageItem = m_wndTreeCtrl.InsertItem(strStage,cni,FALSE,FALSE,hItem);
	m_wndTreeCtrl.SetItemData(hStageItem,(DWORD_PTR)pItem);
	m_wndTreeCtrl.Expand(hItem,TVE_EXPAND);
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
}

void CDlgBoardingCallOnBoard::OnRemoveStage()
{
	HTREEITEM hItem = m_wndTreeCtrl.GetSelectedItem();
	COnBoardingCallStage* pItem = (COnBoardingCallStage*)m_wndTreeCtrl.GetItemData(hItem);
	m_boardingCall.removeItem(pItem);
	m_wndTreeCtrl.DeleteItem(hItem);
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
}

void CDlgBoardingCallOnBoard::OnEditStage()
{
	HTREEITEM hItem = m_wndTreeCtrl.GetSelectedItem();
	m_wndTreeCtrl.DoEdit(hItem);
}

void CDlgBoardingCallOnBoard::OnAddFlightType()
{
	HTREEITEM hItem = m_wndTreeCtrl.GetSelectedItem();
	COnBoardingCallStage* pStage = (COnBoardingCallStage*)m_wndTreeCtrl.GetItemData(hItem);
	FlightConstraint fltType;
	CFlightDialog flightDlg(NULL);
	if(IDOK == flightDlg.DoModal())
	{
		fltType = flightDlg.GetFlightSelection();
	}
	else
	{
		return;
	}
	for (int i = 0; i < pStage->getCount(); i++)
	{
		COnBoardingCallFltType* pItem = pStage->getItem(i);
		if (pItem->GetFltType() == fltType)
		{
			::AfxMessageBox(_T("The Flight Type is exist..!"));
			return;
		}
	}

	COnBoardingCallFltType* pItem = new COnBoardingCallFltType();
	pStage->addItem(pItem);
	COOLTREE_NODE_INFO cni;
	CARCBaseTree::InitNodeInfo(this,cni);
	CString strData = _T("");
	char szBuffer[1024] = {0};
	fltType.screenPrint(szBuffer);
	strData.Format(_T("Flight Type: %s"),szBuffer);
	cni.unMenuID = IDR_MENU_DELETE_FLIGHTTYPE;
	HTREEITEM hFltTypeItem = m_wndTreeCtrl.InsertItem(strData,cni,FALSE,FALSE,hItem);
	m_wndTreeCtrl.SetItemData(hFltTypeItem,(DWORD_PTR)pItem);
	int nFlightCount = m_pInterm->flightSchedule->getCount();
	for (int i = 0; i < nFlightCount; i++)
	{
		Flight* pFlight = m_pInterm->flightSchedule->getItem(i);
		if (fltType.fits(pFlight->getLogEntry()))
		{
			cni.unMenuID = IDR_MENU_FLIGHT_TYPE;
			cni.nt = NT_NORMAL;
			cni.net = NET_NORMAL;
			char szID[1024] = {0};
			int nOnBoardCount = m_pOnBoardList->getCount();
			CString strValue = _T("");
			HTREEITEM hFlightItem = NULL;
			if(pFlight->isArriving())
			{
				pFlight->getFullID(szID,'A');
				for (int j = 0; j < nOnBoardCount; j++)
				{
					COnboardFlight* pFlightAnalysis = NULL;
					CFlightTypeCandidate* pFlt = m_pOnBoardList->getItem(j);
					pFlightAnalysis = pFlt->findItem(szID);
					if (pFlightAnalysis)
					{
						COnBoardingCallFlight* pCallFlight = new COnBoardingCallFlight(m_pInterm->inStrDict, m_pInterm->m_pAirportDB);
						pCallFlight->SetFlightAnalysis(*pFlightAnalysis);
						pItem->addItem(pCallFlight);
						strValue.Format(_T("Flight %s - ACTYPE %s Arr"),pFlightAnalysis->getFlightID(),pFlightAnalysis->getACType());
						HTREEITEM hFlightItem = m_wndTreeCtrl.InsertItem(strValue,cni,FALSE,FALSE,hFltTypeItem);
						m_wndTreeCtrl.SetItemData(hFlightItem,(DWORD_PTR)pCallFlight);
					}
				}
			}
			if (pFlight->isDeparting())
			{
				pFlight->getFullID(szID,'D');
				for (int j = 0; j < nOnBoardCount; j++)
				{
					COnboardFlight* pFlightAnalysis = NULL;
					CFlightTypeCandidate* pFlt = m_pOnBoardList->getItem(j);
					pFlightAnalysis = pFlt->findItem(szID);
					if (pFlightAnalysis)
					{
						COnBoardingCallFlight* pCallFlight = new COnBoardingCallFlight(m_pInterm->inStrDict, m_pInterm->m_pAirportDB);
						pCallFlight->SetFlightAnalysis(*pFlightAnalysis);
						pItem->addItem(pCallFlight);
						strValue.Format(_T("Flight %s - ACTYPE %s Dep"),pFlightAnalysis->getFlightID(),pFlightAnalysis->getACType());
						hFlightItem = m_wndTreeCtrl.InsertItem(strValue,cni,FALSE,FALSE,hFltTypeItem);
						m_wndTreeCtrl.SetItemData(hFlightItem,(DWORD_PTR)pCallFlight);
					}
				}
			}
		}
		m_wndTreeCtrl.Expand(hFltTypeItem,TVE_EXPAND);
	}
	m_wndTreeCtrl.Expand(hItem,TVE_EXPAND);
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
}

void CDlgBoardingCallOnBoard::OnDeleteFlightType()
{
	HTREEITEM hItem = m_wndTreeCtrl.GetSelectedItem();
	HTREEITEM hParentItem = m_wndTreeCtrl.GetParentItem(hItem);
	COnBoardingCallStage* pStage = (COnBoardingCallStage*)m_wndTreeCtrl.GetItemData(hParentItem);
	COnBoardingCallFltType* pFlightType = (COnBoardingCallFltType*)m_wndTreeCtrl.GetItemData(hItem);
	pStage->removeItem(pFlightType);
	m_wndTreeCtrl.DeleteItem(hItem);
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
}

void CDlgBoardingCallOnBoard::OnAddStand()
{
	HTREEITEM hItem = m_wndTreeCtrl.GetSelectedItem();
	COnBoardingCallFlight* pFlight = (COnBoardingCallFlight*)m_wndTreeCtrl.GetItemData(hItem);
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();
	CSelectALTObjectDialog objDlg(0,pDoc->GetCurrentAirport());
	objDlg.SetType(ALT_STAND);
	CString strList = _T("");
	CString strTreeItem = _T("");
	if(objDlg.DoModal()==IDOK)
	{
		objDlg.GetObjectIDString(strList);
		if(strList.IsEmpty())
		{
			strTreeItem  = "Stand: All";
		}
		else 
		{
			strTreeItem.Format("Stand: %s", strList);
		}
		ProcessorID procID;
		procID.SetStrDict( m_pInterm->inStrDict );
		procID.setID(strList);

		int nCount = pFlight->getCount();
		for (int i = 0; i < nCount; i++)
		{
			COnBoardingCallStand* pItem = pFlight->getItem(i);
			if (procID == pItem->getStand())
			{
				::AfxMessageBox(_T("The stand is exist"));
				return;
			}
		}
		COnBoardingCallStand* pStand = new COnBoardingCallStand();
		pStand->setStand( procID );
	
		COOLTREE_NODE_INFO cni;
		CARCBaseTree::InitNodeInfo(this,cni);
		cni.unMenuID = IDR_MENU_STAND_PROCESSOR;
		HTREEITEM hStandItem = m_wndTreeCtrl.InsertItem(strTreeItem,cni,FALSE,FALSE,hItem);
		m_wndTreeCtrl.SetItemData(hStandItem,(DWORD_PTR)pStand);
		m_vStandItem.push_back(hStandItem);
		pFlight->addItem(pStand);
	}
	m_wndTreeCtrl.Expand(hItem,TVE_EXPAND);
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
}

void CDlgBoardingCallOnBoard::OnEditStand()
{
	HTREEITEM hItem = m_wndTreeCtrl.GetSelectedItem();
	COnBoardingCallStand* pStand = (COnBoardingCallStand*)m_wndTreeCtrl.GetItemData(hItem);
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();
	CSelectALTObjectDialog objDlg(0,pDoc->GetCurrentAirport());
	objDlg.SetType(ALT_STAND);
	CString strList = _T("");
	CString strTreeItem = _T("");
	if(objDlg.DoModal()==IDOK)
	{
		objDlg.GetObjectIDString(strList);
		if(strList.IsEmpty())
		{
			strTreeItem  = "Stand: All";
		}
		else 
		{
			strTreeItem.Format("Stand: %s", strList);
		}
		ProcessorID procID;
		procID.SetStrDict( m_pInterm->inStrDict );
		procID.setID(strList);

		COnBoardingCallStand* pStand = new COnBoardingCallStand();
		pStand->setStand( procID );
		m_wndTreeCtrl.SetItemText(hItem,strTreeItem);
		m_wndTreeCtrl.SetItemData(hItem,(DWORD_PTR)pStand);
	}
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
}

void CDlgBoardingCallOnBoard::OnDeleteStand()
{
	HTREEITEM hItem = m_wndTreeCtrl.GetSelectedItem();
	HTREEITEM hParentItem = m_wndTreeCtrl.GetParentItem(hItem);
	COnBoardingCallFlight* pFlight = (COnBoardingCallFlight*)m_wndTreeCtrl.GetItemData(hParentItem);
	COnBoardingCallStand* pStand = (COnBoardingCallStand*)m_wndTreeCtrl.GetItemData(hItem);
	pFlight->removeItem(pStand);
	m_wndTreeCtrl.DeleteItem(hItem);
	m_vStandItem.erase(std::find(m_vStandItem.begin(),m_vStandItem.end(),hItem));
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
}

void CDlgBoardingCallOnBoard::OnAddPaxType()
{
	HTREEITEM hItem = m_wndTreeCtrl.GetSelectedItem();
	COnBoardingCallSeatBlock* pSeatBlock = (COnBoardingCallSeatBlock*)m_wndTreeCtrl.GetItemData(hItem);

	CPassengerType paxType(m_pInterm->inStrDict);
	CDlgNewPassengerType dlg(&paxType,m_pInterm);
	if (dlg.DoModal() == IDOK)
	{
		CString strPaxType = _T("");
		strPaxType = paxType.PrintStringForShow();
		int nCount = pSeatBlock->getCount();
		for (int i=0; i<nCount; i++)
		{
			COnBoardingCallPaxType* pItem = pSeatBlock->getItem(i);
			CString strType = _T("");
			CPassengerType onBoardPaxType(pItem->getMobileElemCnt());
			strType = onBoardPaxType.PrintStringForShow();
			if (!strType.Compare(strPaxType))
			{
				MessageBox(_T("The Passenger Type already exists!"));

				return;
			}
		}
		COnBoardingCallPaxType* pPaxType = new COnBoardingCallPaxType(m_pInterm->inStrDict);
		pPaxType->setMobileElemCnt(paxType);
		pSeatBlock->addItem(pPaxType);
		COOLTREE_NODE_INFO cni;
		CARCBaseTree::InitNodeInfo(this,cni);
		cni.nt = NT_NORMAL;
		cni.net = NET_NORMAL;
		cni.unMenuID = IDR_MENU_PASSENGER_TYPE;
		CString strValue = _T("");
		strValue.Format(_T("Passenger Type: %s"),strPaxType);
		HTREEITEM hPaxTypeItem = m_wndTreeCtrl.InsertItem(strValue,cni,FALSE,FALSE,hItem);
		m_wndTreeCtrl.SetItemData(hPaxTypeItem,(DWORD_PTR)pPaxType);
		m_vPaxTypeItem.push_back(hPaxTypeItem);
		
		cni.nt = NT_NORMAL;
		cni.net = NET_COMBOBOX;
		cni.unMenuID = 0;
		HTREEITEM hDistItem = m_wndTreeCtrl.InsertItem(pPaxType->getPaxDistribution().getPrintDist(),cni,FALSE,FALSE,hPaxTypeItem);
		m_wndTreeCtrl.SetItemData(hDistItem,(DWORD_PTR)pPaxType);
		m_wndTreeCtrl.Expand(hPaxTypeItem,TVE_EXPAND);
		GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
	}
	m_wndTreeCtrl.Expand(hItem,TVE_EXPAND);
}

void CDlgBoardingCallOnBoard::OnEditPaxType()
{
	HTREEITEM hItem = m_wndTreeCtrl.GetSelectedItem();
	HTREEITEM hParentItem = m_wndTreeCtrl.GetParentItem(hItem);
	CPassengerType paxType(m_pInterm->inStrDict);
	CDlgNewPassengerType dlg(&paxType,m_pInterm);
	if (dlg.DoModal() == IDOK)
	{
		CString strPaxType = _T("");
		strPaxType = paxType.PrintStringForShow();
		COnBoardingCallPaxType*	pPaxType = (COnBoardingCallPaxType*)m_wndTreeCtrl.GetItemData(hItem);
		COnBoardingCallSeatBlock* pSeatBlock = (COnBoardingCallSeatBlock*)m_wndTreeCtrl.GetItemData(hParentItem);
		int nCount = pSeatBlock->getCount();
		for (int i=0; i<nCount; i++)
		{
			COnBoardingCallPaxType* pItem = pSeatBlock->getItem(i);
			CString strType = _T("");
			CPassengerType onBoardPaxType(pItem->getMobileElemCnt());
			strType = onBoardPaxType.GetStringForDatabase();
			if (!strType.Compare(strPaxType) && pItem != pPaxType)
			{
				MessageBox(_T("The Passenger Type already exists!"));

				return;
			}
		}
		pPaxType->setMobileElemCnt(paxType);
		CString strValue = _T("");
		strValue.Format(_T("Passenger Type: %s"),strPaxType);
		m_wndTreeCtrl.SetItemText(hItem,strValue);
		m_wndTreeCtrl.SetItemData(hItem,(DWORD_PTR)pPaxType);
		GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
	}
}

void CDlgBoardingCallOnBoard::OnDeletePaxType()
{
	HTREEITEM hItem = m_wndTreeCtrl.GetSelectedItem();
	HTREEITEM hParentItem = m_wndTreeCtrl.GetParentItem(hItem);
	COnBoardingCallSeatBlock* pSeatBlock = (COnBoardingCallSeatBlock*)m_wndTreeCtrl.GetItemData(hParentItem);
	COnBoardingCallPaxType* pPaxType = (COnBoardingCallPaxType*)m_wndTreeCtrl.GetItemData(hItem);
	pSeatBlock->removeItem(pPaxType);
	m_wndTreeCtrl.DeleteItem(hItem);
	m_vPaxTypeItem.erase(std::find(m_vPaxTypeItem.begin(),m_vPaxTypeItem.end(),hItem));
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
}

void CDlgBoardingCallOnBoard::OnInitTreeCtrl()
{
	COOLTREE_NODE_INFO cni;
	CARCBaseTree::InitNodeInfo(this,cni);
	HTREEITEM hItem = m_wndTreeCtrl.InsertItem(_T("Boarding Calls"),cni,FALSE);
	for (int i = 0; i < m_boardingCall.getCount(); i++)
	{
		COnBoardingCallStage* pStage = m_boardingCall.getItem(i);
		CString strValue = _T("");
		strValue.Format(_T("Stage %d"),pStage->getStage());
		cni.nt = NT_NORMAL;
		cni.net = NET_NORMAL;
		cni.unMenuID = IDR_MENU_ONBOARD_STAGE;
		HTREEITEM hStageItem = m_wndTreeCtrl.InsertItem(strValue,cni,FALSE,FALSE,hItem);
		m_wndTreeCtrl.SetItemData(hStageItem,(DWORD_PTR)pStage);
		
		for (int j = 0; j < pStage->getCount(); j++)
		{
			COnBoardingCallFltType* pFltType = pStage->getItem(j);
			FlightConstraint fltType = pFltType->GetFltType();
			char szBuffer[1024] = {0};
			fltType.screenPrint(szBuffer);
			strValue.Format(_T("Flight Type: %s"),szBuffer);
			cni.nt = NT_NORMAL;
			cni.net = NET_NORMAL;
			cni.unMenuID = IDR_MENU_DELETE_FLIGHTTYPE;
			HTREEITEM hFltTypeItem = m_wndTreeCtrl.InsertItem(strValue,cni,FALSE,FALSE,hStageItem);
			m_wndTreeCtrl.SetItemData(hFltTypeItem,(DWORD_PTR)pFltType);

			for (int n = 0; n < pFltType->getCount(); n++)
			{
				cni.unMenuID = IDR_MENU_FLIGHT_TYPE;
				cni.nt = NT_NORMAL;
				cni.net = NET_NORMAL;
				COnBoardingCallFlight* pFlight = pFltType->getItem(n);
				COnboardFlight& pFlightAnalysis = pFlight->GetFlightAnalysis();
				if (pFlightAnalysis.getFlightType() == ArrFlight)
				{
					strValue.Format(_T("Flight %s - ACTYPE %s Arr"),pFlightAnalysis.getFlightID(),pFlightAnalysis.getACType());
				}
				else
				{
					strValue.Format(_T("Flight %s - ACTYPE %s Dep"),pFlightAnalysis.getFlightID(),pFlightAnalysis.getACType());
				}
				HTREEITEM hFlightItem = m_wndTreeCtrl.InsertItem(strValue,cni,FALSE,FALSE,hFltTypeItem);
				m_wndTreeCtrl.SetItemData(hFlightItem,(DWORD_PTR)pFlight);
				for (int m = 0; m < pFlight->getCount(); m++)
				{
					COnBoardingCallStand* pStand = pFlight->getItem(m);
					const ProcessorID& standID = pStand->getStand();
					HTREEITEM hStandItem = NULL;
					cni.nt = NT_NORMAL;
					cni.net = NET_NORMAL;
					cni.unMenuID = IDR_MENU_STAND_PROCESSOR;
					strValue.Format(_T("Stand: %s"),standID.GetIDString());
					if (standID.GetIDString().IsEmpty())
					{
						hStandItem = m_wndTreeCtrl.InsertItem(_T("Stand All"),cni,FALSE,FALSE,hFlightItem);
					}
					else
					{
						hStandItem = m_wndTreeCtrl.InsertItem(strValue,cni,FALSE,FALSE,hFlightItem);
					}
					m_wndTreeCtrl.SetItemData(hStandItem,(DWORD_PTR)pStand);
					m_vStandItem.push_back(hStandItem);
					for (int k = 0; k < pStand->getCount(); k++)
					{
						COnBoardingCallSeatBlock* pCallSeat = pStand->getItem(k);
						COnBoardSeatBlockList& seatBlockList = pFlight->getSeatBlockList();
						COnBoardSeatBlockItem* pBlockItem = seatBlockList.findItem(pCallSeat->getSeatBlock());
						strValue.Format(_T("Seat Block: %s"),pBlockItem->getBlockName());
						cni.nt = NT_NORMAL;
						cni.net = NET_COMBOBOX;
						cni.unMenuID = IDR_MENU_SEAT_BLOCK;
						HTREEITEM hBlockItem = m_wndTreeCtrl.InsertItem(strValue,cni,FALSE,FALSE,hStandItem);
						m_wndTreeCtrl.SetItemData(hBlockItem,(DWORD_PTR)pCallSeat);

						for (int r = 0; r < pCallSeat->getCount(); r++)
						{
							COnBoardingCallPaxType* pPaxType = pCallSeat->getItem(r);
							CPassengerType paxType = pPaxType->getMobileElemCnt();
							strValue.Format(_T("Passenger Type: %s"),paxType.PrintStringForShow());
							cni.nt = NT_NORMAL;
							cni.net = NET_NORMAL;
							cni.unMenuID = IDR_MENU_PASSENGER_TYPE;
							HTREEITEM hPaxItem = m_wndTreeCtrl.InsertItem(strValue,cni,FALSE,FALSE,hBlockItem);
							m_wndTreeCtrl.SetItemData(hPaxItem,(DWORD_PTR)pPaxType);
							m_vPaxTypeItem.push_back(hPaxItem);
							cni.nt = NT_NORMAL;
							cni.net = NET_COMBOBOX;
							cni.unMenuID = 0;
							HTREEITEM hDistItem = m_wndTreeCtrl.InsertItem(pPaxType->getPaxDistribution().getPrintDist(),cni,FALSE,FALSE,hPaxItem);
							m_wndTreeCtrl.SetItemData(hDistItem,(DWORD_PTR)pPaxType);
							m_wndTreeCtrl.Expand(hPaxItem,TVE_EXPAND);
						}
						m_wndTreeCtrl.Expand(hBlockItem,TVE_EXPAND);
					}
					m_wndTreeCtrl.Expand(hStandItem,TVE_EXPAND);
				}
				m_wndTreeCtrl.Expand(hFlightItem,TVE_EXPAND);
			}
			m_wndTreeCtrl.Expand(hFltTypeItem,TVE_EXPAND);
		}
		m_wndTreeCtrl.Expand(hStageItem,TVE_EXPAND);
	}
	m_wndTreeCtrl.Expand(hItem,TVE_EXPAND);
}

LRESULT CDlgBoardingCallOnBoard::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	case WM_LBUTTONDBLCLK:
		{
			OnLButtonDblClk(wParam,lParam);
			GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
			return 0;
		}
		break;
	case UM_CEW_COMBOBOX_BEGIN:
		{
			COOLTREE_NODE_INFO* cni = m_wndTreeCtrl.GetItemNodeInfo((HTREEITEM)wParam);	
			if(cni->unMenuID == IDR_MENU_SEAT_BLOCK)
			{
				CreateSeatBlock((HTREEITEM)wParam);
			}
			else
			{
				CreateProdistribution((HTREEITEM)wParam);
			}
			GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
		}
		break;
	case UM_CEW_COMBOBOX_SELCHANGE:
		{
			HTREEITEM hItem=(HTREEITEM)wParam;
			COOLTREE_NODE_INFO* cni = m_wndTreeCtrl.GetItemNodeInfo(hItem);	
			if(cni->unMenuID == IDR_MENU_SEAT_BLOCK)
			{
				HandleSeatBlock(hItem);
			}
			else
			{
				HandleProdistribution(hItem);
			}
			GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
		}
	    break;
	case SELCHANGED:
		{
			OnSelChanged(wParam,lParam);
		}
	    break;
	default:
	    break;
	}
	return CXTResizeDialog::WindowProc(message,wParam,lParam);
}

void CDlgBoardingCallOnBoard::CreateSeatBlock(HTREEITEM hItem)
{
	HTREEITEM hParentItem = m_wndTreeCtrl.GetParentItem(m_wndTreeCtrl.GetParentItem(hItem));
	COnBoardingCallFlight* pFlight = (COnBoardingCallFlight*)m_wndTreeCtrl.GetItemData(hParentItem);
	CRect rectWnd;
	CWnd* pWnd=m_wndTreeCtrl.GetEditWnd(hItem);
	m_wndTreeCtrl.GetItemRect(hItem,rectWnd,TRUE);
	pWnd->SetWindowPos(NULL,rectWnd.right,rectWnd.top,0,0,SWP_NOSIZE);
	CComboBox* pCB=(CComboBox*)pWnd;
	pCB->ResetContent();
	if (pCB->GetCount() == 0)
	{
		COnBoardSeatBlockList& setBlockList = pFlight->getSeatBlockList();
		for (int i = 0; i < setBlockList.getCount(); i++)
		{
			COnBoardSeatBlockItem* pItem = setBlockList.getItem(i);
			pCB->AddString(pItem->getBlockName());
		}
	}
}

void CDlgBoardingCallOnBoard::HandleSeatBlock(HTREEITEM hItem)
{
	int nSel = m_wndTreeCtrl.GetCmbBoxCurSel(hItem);
	HTREEITEM hParentItem = m_wndTreeCtrl.GetParentItem(m_wndTreeCtrl.GetParentItem(hItem));
	COnBoardingCallFlight* pFlight = (COnBoardingCallFlight*)m_wndTreeCtrl.GetItemData(hParentItem);
	COnBoardingCallSeatBlock* pSeatBlock = (COnBoardingCallSeatBlock*)m_wndTreeCtrl.GetItemData(hItem);
	COnBoardingCallStand* pStand = (COnBoardingCallStand*)m_wndTreeCtrl.GetItemData(m_wndTreeCtrl.GetParentItem(hItem));
	COnBoardSeatBlockList& setBlockList = pFlight->getSeatBlockList();
	int nCount = pStand->getCount();
	for (int i=0; i<nCount; i++)
	{
		COnBoardingCallSeatBlock* pItem = pStand->getItem(i);
		if (pItem->getSeatBlock() == setBlockList.getItem(nSel)->getID()\
			&& pItem != pSeatBlock)
		{
			MessageBox(_T("The seat block already exists!"));

			return;
		}
	}
	bool bExit = false;
	for (int j = 0 ; j < nCount; j++)
	{
		if(pSeatBlock == pStand->getItem(j))
		{
			bExit = true;
			break;
		}
	}
	if (!bExit)
	{
		pStand->addItem(pSeatBlock);
	}
	pSeatBlock->setSeatBlockItemID(setBlockList.getItem(nSel)->getID());
	CString strValue = _T("");
	strValue.Format(_T("Seat Block: %s"),setBlockList.getItem(nSel)->getBlockName());
	m_wndTreeCtrl.SetItemText(hItem,strValue);
}

void CDlgBoardingCallOnBoard::CreateProdistribution(HTREEITEM hItem)
{
	CWnd* pWnd=m_wndTreeCtrl.GetEditWnd(hItem);
	CComboBox* pCB=(CComboBox*)pWnd;
	pCB->ResetContent();
	if(pCB->GetCount()==0)
	{
		CStringList strList;
		CString s;
		s.LoadString( IDS_STRING_NEWDIST );
		pCB->AddString(s);
		int nCount = _g_GetActiveProbMan( m_pInterm)->getCount();
		for( int m=0; m<nCount; m++ )
		{
			CProbDistEntry* pPBEntry = _g_GetActiveProbMan( m_pInterm)->getItem( m );			
			pCB->AddString(pPBEntry->m_csName);
		}
	}		
}

void CDlgBoardingCallOnBoard::HandleProdistribution(HTREEITEM hItem)
{
	int nIndexSeled=m_wndTreeCtrl.GetCmbBoxCurSel(hItem);
	CString strValue = _T("");
	strValue.LoadString( IDS_STRING_NEWDIST );

	COnBoardingCallPaxType* pPaxType = (COnBoardingCallPaxType*)m_wndTreeCtrl.GetItemData(hItem);
	CString strItem=m_wndTreeCtrl.GetItemText(hItem);
	if( strcmp( strItem, strValue ) == 0 )
	{
		CDlgProbDist dlg(m_pInterm->m_pAirportDB, true );
		if(dlg.DoModal()==IDOK) {
			int idxPD = dlg.GetSelectedPDIdx();
			ASSERT(idxPD!=-1);
			CProbDistEntry* pPDEntry = _g_GetActiveProbMan( m_pInterm )->getItem(idxPD);
			pPaxType->setPaxDistribution(pPDEntry);
			m_wndTreeCtrl.SetItemText( hItem, pPDEntry->m_csName );
		}
		else {
			//cancel
			m_wndTreeCtrl.SetItemText(hItem, pPaxType->getPaxDistribution().getPrintDist());
		}
	}
	else
	{
		CProbDistEntry* pPDEntry = NULL;
		int nCount = _g_GetActiveProbMan( m_pInterm )->getCount();
		for( int i=0; i<nCount; i++ )
		{
			pPDEntry = _g_GetActiveProbMan( m_pInterm )->getItem( i );
			if( strcmp( pPDEntry->m_csName, strItem ) == 0 )
				break;
		}
		pPaxType->setPaxDistribution(pPDEntry);
	}
}
void CDlgBoardingCallOnBoard::OnLButtonDblClk(WPARAM wParam, LPARAM lParam)
{
	UINT uFlags;
	CPoint point = *(CPoint*)lParam;
	HTREEITEM hItem = m_wndTreeCtrl.HitTest(point, &uFlags);
	HTREEITEM hParentItem = m_wndTreeCtrl.GetParentItem(hItem);

	if (std::find(m_vStandItem.begin(),m_vStandItem.end(),hItem) != m_vStandItem.end())
	{
		COnBoardingCallStand* pStand = (COnBoardingCallStand*)m_wndTreeCtrl.GetItemData(hItem);
		COnBoardingCallFlight* pFlight = (COnBoardingCallFlight*)m_wndTreeCtrl.GetItemData(hParentItem);
		CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();
		CSelectALTObjectDialog objDlg(0,pDoc->GetCurrentAirport());
		objDlg.SetType(ALT_STAND);
		CString strList = _T("");
		CString strTreeItem = _T("");
		if(objDlg.DoModal()==IDOK)
		{
			objDlg.GetObjectIDString(strList);
			if(strList.IsEmpty())
			{
				strTreeItem  = "Stand: All";
			}
			else 
			{
				strTreeItem.Format("Stand: %s", strList);
			}
			ProcessorID procID;
			procID.SetStrDict( m_pInterm->inStrDict );
			procID.setID(strList);
			
			int nCount = pFlight->getCount();
			for (int i=0; i<nCount; i++)
			{
				COnBoardingCallStand* pItem = pFlight->getItem(i);
				if (procID == pItem->getStand()&& pItem != pStand)
				{
					MessageBox(_T("The stand already exists!"));

					return;
				}
			}
			pStand->setStand( procID );
			m_wndTreeCtrl.SetItemText(hItem,strTreeItem);
			m_wndTreeCtrl.SetItemData(hItem,(DWORD_PTR)pStand);
		}
		GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
	}

	if (std::find(m_vPaxTypeItem.begin(),m_vPaxTypeItem.end(),hItem) != m_vPaxTypeItem.end())
	{
		CPassengerType paxType(m_pInterm->inStrDict);
		CDlgNewPassengerType dlg(&paxType,m_pInterm);
		if (dlg.DoModal() == IDOK)
		{
			CString strPaxType = _T("");
			strPaxType = paxType.PrintStringForShow();
			COnBoardingCallPaxType*	pPaxType = (COnBoardingCallPaxType*)m_wndTreeCtrl.GetItemData(hItem);
			COnBoardingCallSeatBlock* pSeatBlock = (COnBoardingCallSeatBlock*)m_wndTreeCtrl.GetItemData(hParentItem);
			int nCount = pSeatBlock->getCount();
			for (int i=0; i<nCount; i++)
			{
				COnBoardingCallPaxType* pItem = pSeatBlock->getItem(i);
				CString strType = _T("");
				CPassengerType onBoardPaxType(pItem->getMobileElemCnt());
				strType = onBoardPaxType.GetStringForDatabase();
				if (!strType.Compare(strPaxType) && pItem != pPaxType)
				{
					MessageBox(_T("The Passenger Type already exists!"));

					return;
				}
			}
			pPaxType->setMobileElemCnt(paxType);
			CString strValue = _T("");
			strValue.Format(_T("Passenger Type: %s"),strPaxType);
			m_wndTreeCtrl.SetItemText(hItem,strValue);
			m_wndTreeCtrl.SetItemData(hItem,(DWORD_PTR)pPaxType);
			GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
		}
	}
}

LRESULT CDlgBoardingCallOnBoard::OnSelChanged(WPARAM wParam, LPARAM lParam)
{
	NM_TREEVIEW* pnmv = (NM_TREEVIEW*)wParam;

	UpdateToolbarStatus();
	UpdateBlockBtnStatus();

	return 0;
}

void CDlgBoardingCallOnBoard::UpdateToolbarStatus()
{
	HTREEITEM hItem = m_wndTreeCtrl.GetSelectedItem();
	if (hItem == NULL)
	{
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_MODEL_NEW,TRUE);
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_MODEL_EDIT,FALSE);
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_MODEL_DELETE,FALSE);
		return;
	}
	if (hItem == m_wndTreeCtrl.GetRootItem())
	{
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_MODEL_NEW,TRUE);
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_MODEL_EDIT,FALSE);
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_MODEL_DELETE,FALSE);
	}
	else if(m_wndTreeCtrl.GetParentItem(hItem) == m_wndTreeCtrl.GetRootItem())
	{
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_MODEL_NEW,FALSE);
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_MODEL_EDIT,TRUE);
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_MODEL_DELETE,TRUE);
	}
	else
	{
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_MODEL_NEW,FALSE);
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_MODEL_EDIT,FALSE);
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_MODEL_DELETE,FALSE);
	}
}

void CDlgBoardingCallOnBoard::UpdateBlockBtnStatus()
{
	HTREEITEM hItem = m_wndTreeCtrl.GetSelectedItem();
	if (hItem == NULL)
	{
		return;
	}
	COOLTREE_NODE_INFO* cni = m_wndTreeCtrl.GetItemNodeInfo(hItem);
	if (cni->unMenuID == IDR_MENU_FLIGHT_TYPE)
	{
		GetDlgItem(IDC_BUTTON_BLOCK)->EnableWindow(TRUE);
	}
	else
	{
		GetDlgItem(IDC_BUTTON_BLOCK)->EnableWindow(FALSE);
	}
}

void CDlgBoardingCallOnBoard::OnBnClickedOk()
{
	OnBtnSave();
	CXTResizeDialog::OnOK();
}

void CDlgBoardingCallOnBoard::OnBtnSave()
{
	try
	{
		CADODatabase::BeginTransaction() ;
        m_boardingCall.SaveData(m_nProjID);
		CADODatabase::CommitTransaction() ;
	}
	catch (CADOException e)
	{
		CADODatabase::RollBackTransation() ;
	}
	
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(FALSE);
}

void CDlgBoardingCallOnBoard::OnAddSeatBlock()
{
	HTREEITEM hItem = m_wndTreeCtrl.GetSelectedItem();
	HTREEITEM hParenItem = m_wndTreeCtrl.GetParentItem(hItem);
	COnBoardingCallStand* pStand = (COnBoardingCallStand*)m_wndTreeCtrl.GetItemData(hItem);
	COnBoardingCallFlight* pFlight = (COnBoardingCallFlight*)m_wndTreeCtrl.GetItemData(hParenItem);
	if (pFlight->getSeatBlockList().getCount() == 0)
	{
		MessageBox(_T("Please define seat block "));
		return;
	}
	COnBoardingCallSeatBlock* pSeatBlock = new COnBoardingCallSeatBlock();
	COOLTREE_NODE_INFO cni;
	CARCBaseTree::InitNodeInfo(this,cni);
	cni.unMenuID = IDR_MENU_SEAT_BLOCK;
	cni.nt = NT_NORMAL;
	cni.net = NET_COMBOBOX;
	HTREEITEM hSeatBlockItem = m_wndTreeCtrl.InsertItem(_T("Seat Block:"),cni,FALSE,FALSE,hItem);
	m_wndTreeCtrl.SetItemData(hSeatBlockItem,(DWORD_PTR)pSeatBlock);
	m_wndTreeCtrl.Expand(hItem,TVE_EXPAND);
	m_wndTreeCtrl.DoEdit(hSeatBlockItem);
}

void CDlgBoardingCallOnBoard::OnDeleteSeatBlock()
{
	HTREEITEM hItem = m_wndTreeCtrl.GetSelectedItem();
	HTREEITEM hParentItem = m_wndTreeCtrl.GetParentItem(hItem);
	COnBoardingCallStand* pStand = (COnBoardingCallStand*)m_wndTreeCtrl.GetItemData(hParentItem);
	COnBoardingCallSeatBlock* pSeatBlock = (COnBoardingCallSeatBlock*)m_wndTreeCtrl.GetItemData(hItem);
	pStand->removeItem(pSeatBlock);
	m_wndTreeCtrl.DeleteItem(hItem);
}
