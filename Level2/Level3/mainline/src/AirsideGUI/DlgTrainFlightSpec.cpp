// DlgTrainFlightSpec.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"
#include "DlgTrainFlightSpec.h"
#include "DlgSelectAirrouteObject.h"
#include "DlgSelectALTObject.h"
#include "../Common/AirportDatabase.h"
#include "../MFCExControl/CoolTreeEdit.h"
#include "DlgTimeRange.h"
#include "../Common/WinMsg.h"
#include "../Common/ProbDistManager.h"
#include "../Inputs/IN_TERM.H"
#include "../Common/ACTypesManager.h"
#include "../MFCExControl/SmartEdit.h"
#include "../Common/UnitPiece.h"
#include "../common/AirlineManager.h"

#define  IDC_NAME_EDIT 1001
// CDlgTrainFlightSpec dialog

IMPLEMENT_DYNAMIC(CDlgTrainFlightSpec, CXTResizeDialog)
CDlgTrainFlightSpec::CDlgTrainFlightSpec(int nProjID,InputTerminal* pInTerm,InputAirside* pInputAirside,PSelectProbDistEntry pSelectProbDistEntry,CWnd* pParent /*=NULL*/)
	: CXTResizeDialog(CDlgTrainFlightSpec::IDD, pParent)
	,m_nProjID(nProjID)
	,m_pInTerm(pInTerm)
	,m_pAirsideInput(pInputAirside)
	,m_pSelectProbDistEntry(pSelectProbDistEntry)
{
}

CDlgTrainFlightSpec::~CDlgTrainFlightSpec()
{
}

void CDlgTrainFlightSpec::DoDataExchange(CDataExchange* pDX)
{
	CXTResizeDialog::DoDataExchange(pDX);
	DDX_Control(pDX,IDC_LIST_DATA,m_wndListBox);
	DDX_Control(pDX,IDC_TREE_DATA,m_wndTreeCtrl);
}


BEGIN_MESSAGE_MAP(CDlgTrainFlightSpec, CXTResizeDialog)
	ON_WM_CREATE()
	ON_MESSAGE(WM_END_EDIT,OnEndEdit)
	ON_LBN_SELCHANGE(IDC_LIST_DATA, OnSelchangeListBox)
	ON_COMMAND(ID_TOOLBAR_FLIGHT_TYPE_ADD,AddTrainFlight)
	ON_COMMAND(ID_TOOLBAR_FLIGHT_TYPE_DEL,DeleteTrainFlight)
	ON_COMMAND(ID_TOOLBAR_FLIGHT_TYPE_EDIT,EditTrainFlight)
	ON_BN_CLICKED(IDC_BUTTON_SAVE,OnSave)
END_MESSAGE_MAP()

// CDlgTrainFlightSpec message handlers


void CDlgTrainFlightSpec::OnInitToolbar()
{
	CRect rect;
	m_wndListBox.GetWindowRect(&rect);
	ScreenToClient(&rect);
	rect.bottom = rect.top - 6;
	rect.top = rect.bottom - 22;
	rect.left = rect.left + 2;
	m_wndToolBar.MoveWindow(&rect,true);
	m_wndToolBar.ShowWindow(SW_SHOW);;
	m_wndToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_FLIGHT_TYPE_ADD,TRUE);
	m_wndToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_FLIGHT_TYPE_DEL,FALSE);
	m_wndToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_FLIGHT_TYPE_EDIT,FALSE);
}

int CDlgTrainFlightSpec::OnCreate( LPCREATESTRUCT lpCreateStruct )
{
	if (CXTResizeDialog::OnCreate(lpCreateStruct) == -1)
	{
		return -1;
	}

	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT,WS_CHILD|WS_VISIBLE|CBRS_ALIGN_TOP|CBRS_TOOLTIPS,CRect(0,0,0,0),IDR_TOOLBAR_FLIGHT_TYPE)||
		!m_wndToolBar.LoadToolBar(IDR_TOOLBAR_FLIGHT_TYPE))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}
	return 0;
}

BOOL CDlgTrainFlightSpec::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog();

	OnInitToolbar();

	SetResize(m_wndToolBar.GetDlgCtrlID(), SZ_TOP_LEFT, SZ_TOP_LEFT);
	SetResize(IDC_STATIC_FLIGHT, SZ_TOP_LEFT, SZ_BOTTOM_RIGHT);
	SetResize(IDC_LIST_DATA, SZ_TOP_LEFT, SZ_BOTTOM_CENTER);
	SetResize(IDC_TREE_DATA, SZ_TOP_CENTER , SZ_BOTTOM_RIGHT);
	SetResize(IDC_BUTTON_SAVE, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDOK, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDCANCEL, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);

	m_trainFlightContainer.ReadData();
	CUnitPiece::InitializeUnitPiece(m_nProjID,UP_ID_1001,this);

	CRect rectItem;
	GetDlgItem(IDOK)->GetWindowRect(rectItem);
	ScreenToClient(rectItem);
	int yPos = rectItem.top;
	m_wndListBox.GetWindowRect(rectItem);
	ScreenToClient(rectItem);
	int xPos = rectItem.left;
	CUnitPiece::MoveUnitButton(xPos,yPos);
	SetResize(XIAOHUABUTTON_ID, SZ_BOTTOM_LEFT, SZ_BOTTOM_LEFT);
	CUnitPiece::SetUnitInUse(ARCUnit_ALL_InUse);

	DisplayListBoxContent();
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(FALSE);

	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),TRUE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),FALSE);

	_g_GetActiveACMan( m_pInTerm->m_pAirportDB )->GetAllIATACode(sortAcTypeList);
	std::sort(sortAcTypeList.begin(),sortAcTypeList.end());

	CAIRLINELIST* pAirlineList = m_pInTerm->m_pAirportDB->getAirlineMan()->GetAirlineList();
	if(pAirlineList){
		for(size_t i=0;i<pAirlineList->size();i++){
			sortAirlineList.push_back(pAirlineList->at(i)->m_sAirlineIATACode);
		}
		std::sort(sortAirlineList.begin(),sortAirlineList.end());
	}


	return TRUE;
}

void CDlgTrainFlightSpec::AddTrainFlight()
{
	int nSel = m_wndListBox.AddString( "" );
	m_wndListBox.SetCurSel( nSel );

	CTrainingFlightData* pNewTrainFlight = new CTrainingFlightData;
	m_trainFlightContainer.AddNewItem(pNewTrainFlight);
	m_wndListBox.SetItemData(nSel,(DWORD)pNewTrainFlight);

	CRect rectItem;
	CRect rectLB;
	m_wndListBox.GetWindowRect( &rectLB );
	ScreenToClient( &rectLB );
	m_wndListBox.GetItemRect( nSel, &rectItem );

	rectItem.OffsetRect( rectLB.left+2, rectLB.top+1 );

	rectItem.right += 1;
	rectItem.bottom += 4;

	CString csItemText;
	m_wndListBox.GetText( nSel, csItemText );
	CSmartEdit* pEdit = new CSmartEdit(csItemText);
	DWORD dwStyle = ES_LEFT | WS_BORDER|WS_CHILD|WS_VISIBLE|ES_AUTOHSCROLL;
	pEdit->Create( dwStyle, rectItem, this, IDC_NAME_EDIT );	

	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
}

void CDlgTrainFlightSpec::DeleteTrainFlight()
{
	int nSel = m_wndListBox.GetCurSel();
	if (nSel == LB_ERR)
		return;

	CTrainingFlightData* pTrainFligh = (CTrainingFlightData*)m_wndListBox.GetItemData(nSel);
	m_trainFlightContainer.DeleteItem(pTrainFligh);
	m_wndListBox.DeleteString(nSel);
	if (m_wndListBox.GetCount())
	{
		int nSelectItem = nSel ? nSel - 1: 0;
		m_wndListBox.SetCurSel(nSelectItem);
		OnSelchangeListBox();
	}
	else
	{
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_FLIGHT_TYPE_ADD,TRUE);
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_FLIGHT_TYPE_DEL,FALSE);
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_FLIGHT_TYPE_EDIT,FALSE);
		DisplayTrainFlight(NULL);
	}
	
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
}

void CDlgTrainFlightSpec::EditTrainFlight()
{

}

void CDlgTrainFlightSpec::DisplayListBoxContent()
{
 	for (unsigned i = 0; i < m_trainFlightContainer.GetElementCount(); i++)
 	{
 		CTrainingFlightData* pTrainFlight = m_trainFlightContainer.GetItem(i);
 		int nItem = m_wndListBox.AddString(pTrainFlight->GetName());
 		m_wndListBox.SetItemData(nItem,(DWORD)pTrainFlight);
 	}

	if (m_wndListBox.GetCount() >= 0)
	{
		m_wndListBox.SetCurSel(0);
		OnSelchangeListBox();
	}
}

LONG CDlgTrainFlightSpec::OnEndEdit( WPARAM p_wParam, LPARAM p_lParam )
{
	if( p_lParam != IDC_NAME_EDIT )
		return 0;

	int nSel = m_wndListBox.GetCurSel();
	if( nSel == LB_ERR )
		return 0;

	CTrainingFlightData* pTrainFlgiht = (CTrainingFlightData*)m_wndListBox.GetItemData(nSel);
	CString csStr( (LPTSTR)p_wParam );
	if( csStr.IsEmpty() )
	{
		m_wndListBox.DeleteString( nSel );
		m_wndListBox.SetCurSel( -1 );
		m_trainFlightContainer.DeleteItem(pTrainFlgiht);
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_FLIGHT_TYPE_ADD,TRUE);
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_FLIGHT_TYPE_DEL,FALSE);
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_FLIGHT_TYPE_EDIT,FALSE);
		DisplayTrainFlight(NULL);
	}
	else
	{
		// check if dup
		int i=0;
		for(;  i<nSel; i++ )
		{
			CString csTemp;
			m_wndListBox.GetText( i, csTemp );
			if( csTemp.CompareNoCase( csStr ) == 0 )
			{
				MessageBox( "Duplicated name", "Error", MB_OK|MB_ICONWARNING );
				m_wndListBox.DeleteString( nSel );
				m_wndListBox.SetCurSel( -1 );
				m_trainFlightContainer.DeleteItem(pTrainFlgiht);
				m_wndToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_FLIGHT_TYPE_ADD,TRUE);
				m_wndToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_FLIGHT_TYPE_DEL,FALSE);
				m_wndToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_FLIGHT_TYPE_EDIT,FALSE);
				DisplayTrainFlight(NULL);
				break;
			}
		}

		if( i == nSel )
		{

			pTrainFlgiht->SetName(csStr);
			m_wndListBox.InsertString( nSel, (LPTSTR)p_wParam );
			m_wndListBox.DeleteString( nSel + 1 );

			m_wndListBox.SetCurSel( nSel );
			m_wndListBox.SetItemData(nSel,(DWORD)pTrainFlgiht);
			OnSelchangeListBox();
		}
	}
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
	return 0;
}

void CDlgTrainFlightSpec::DisplayTrainFlight( CTrainingFlightData* pTrainFlight )
{
	m_wndTreeCtrl.DeleteAllItems();
	m_wndTreeCtrl.SetImageList(m_wndTreeCtrl.GetImageList(TVSIL_NORMAL),TVSIL_NORMAL);

 	if (pTrainFlight == NULL)
 		return;
 	
 	COOLTREE_NODE_INFO cni;
 	m_wndTreeCtrl.InitNodeInfo(this,cni);
 
 	//carrier code
 	cni.nt = NT_NORMAL;
 	cni.net = NET_COMBOBOX;
 	CString strCarrierCode;
 	strCarrierCode.Format(_T("Carrier Code(2 or 3 characters): %s"),pTrainFlight->GetCarrierCode());
 	m_hCarrierItem = m_wndTreeCtrl.InsertItem(strCarrierCode,cni,FALSE,FALSE);
 
 	//aircraft type
 	cni.nt = NT_NORMAL;
 	cni.net = NET_COMBOBOX;
	if (pTrainFlight->GetAcType().IsEmpty())
	{
		CACTYPELIST* pvACTList;
		pvACTList = _g_GetActiveACMan( m_pInTerm->m_pAirportDB )->GetACTypeList();
		if(pvACTList && !pvACTList->empty())
		{
			CACType* pAcType = (*pvACTList).front();
			pTrainFlight->SetAcType(pAcType->GetIATACode());
		}
	}
 	CString strActype;
 	strActype.Format(_T("AC Type: %s"),pTrainFlight->GetAcType());
 	m_hActypItem = m_wndTreeCtrl.InsertItem(strActype,cni,FALSE,FALSE);
 
 	//time range
 	cni.nt = NT_NORMAL;
 	cni.net = NET_SHOW_DIALOGBOX;
 	CString strTimeRange;
 	strTimeRange.Format(_T("Time Range: %s"),pTrainFlight->GetTimeRaneString());
 	m_hTimeRange = m_wndTreeCtrl.InsertItem(strTimeRange,cni,FALSE,FALSE);
 
 	//number of flight
 	cni.nt = NT_NORMAL;
 	cni.net = NET_EDIT_INT;
	cni.fMinValue = static_cast<float>(pTrainFlight->GetNumFlight());
 	CString strNumFlight;
 	strNumFlight.Format(_T("Number of aircraft: %d"),pTrainFlight->GetNumFlight());
 	m_hNumFlightItem = m_wndTreeCtrl.InsertItem(strNumFlight,cni,FALSE,FALSE);
 	//start point
 	cni.nt = NT_NORMAL;
 	cni.net = NET_COMBOBOX;
 	CString strStartPoint;
	if (pTrainFlight->GetStartPoint().IsBlank())
	{
		strStartPoint.Format(_T("Start Point(Stands or STARS): All"));
	}
	else
	{
		strStartPoint.Format(_T("Start Point(Stands or STARS): %s"),pTrainFlight->GetStartPoint().GetIDString());
	}
 	m_hStartPointItem = m_wndTreeCtrl.InsertItem(strStartPoint,cni,FALSE,FALSE);
 
 	//interarrival
 	cni.nt = NT_NORMAL;
 	cni.net = NET_COMBOBOX;
 	CString strDist;
 	strDist.Format(_T("Interarrival distribution(mins): %s"),pTrainFlight->GetDistribution().getPrintDist());
 	m_hDistItem = m_wndTreeCtrl.InsertItem(strDist,cni,FALSE,FALSE);
 
 	//interntion
 	cni.nt = NT_NORMAL;
 	cni.net = NET_NORMAL;
 	HTREEITEM hInterntionsItem = m_wndTreeCtrl.InsertItem(_T("Intentions"),cni,FALSE,FALSE);
 	
 	HTREEITEM hLowItem = m_wndTreeCtrl.InsertItem(_T("Low and Over"),cni,FALSE,FALSE,hInterntionsItem);
 	
 	//low num
 	cni.nt = NT_NORMAL;
 	cni.net = NET_EDIT_INT;
	cni.fMinValue = static_cast<float>(pTrainFlight->GetLowFlight());
 	CString strLowNum;
 	strLowNum.Format(_T("Number: %d"),pTrainFlight->GetLowFlight());
 	m_hLowNumItem = m_wndTreeCtrl.InsertItem(strLowNum,cni,FALSE,FALSE,hLowItem);
 
 	//low value
 	cni.nt = NT_NORMAL;
 	cni.net = NET_EDIT_FLOAT;
	double dLowValue = ConvertValueToCurrentUnit(Unit_Length,pTrainFlight->GetLowValue());
	cni.fMinValue = static_cast<float>(dLowValue);
 	CString strLowValue;
 	strLowValue.Format(_T("feet above runway threshold(%s): %.2f"),GetUintString(Unit_Length),dLowValue);
 	m_hLowValueItem = m_wndTreeCtrl.InsertItem(strLowValue,cni,FALSE,FALSE,hLowItem);
 
 	cni.nt = NT_NORMAL;
 	cni.net = NET_NORMAL;
 	HTREEITEM hTouchItem = m_wndTreeCtrl.InsertItem(_T("Touch and go"),cni,FALSE,FALSE,hInterntionsItem);
 
 	//touch num
 	cni.nt = NT_NORMAL;
 	cni.net = NET_EDIT_INT;
	cni.fMinValue = static_cast<float>(pTrainFlight->GetTouchFlight());
 	CString strTouchNum;
 	strTouchNum.Format(_T("Number: %d"),pTrainFlight->GetTouchFlight());
 	m_hTouchNumItem = m_wndTreeCtrl.InsertItem(strTouchNum,cni,FALSE,FALSE,hTouchItem);
 
 	//touch value
 	cni.nt = NT_NORMAL;
 	cni.net = NET_EDIT_FLOAT;
	double dTouchValue = ConvertValueToCurrentUnit(Unit_Speed,pTrainFlight->GetTouchValue());
	cni.fMinValue = static_cast<float>(dTouchValue);
 	CString strTouchValue;
 	strTouchValue.Format(_T("deceleratingn to speed(%s): %.2f"),GetUintString(Unit_Speed),dTouchValue);
 	m_hTouchValueItem = m_wndTreeCtrl.InsertItem(strTouchValue,cni,FALSE,FALSE,hTouchItem);
 
 	cni.nt = NT_NORMAL;
 	cni.net = NET_NORMAL;
 	HTREEITEM hStopItem = m_wndTreeCtrl.InsertItem(_T("Stop and go"),cni,FALSE,FALSE,hInterntionsItem);
 
 	//Stop num
 	cni.nt = NT_NORMAL;
 	cni.net = NET_EDIT_INT;
	cni.fMinValue = static_cast<float>(pTrainFlight->GetStopFlight());
 	CString strStopNum;
 	strStopNum.Format(_T("Number: %d"),pTrainFlight->GetStopFlight());
 	m_hStopNumItem = m_wndTreeCtrl.InsertItem(strStopNum,cni,FALSE,FALSE,hStopItem);
 
 	//stop value
 	cni.nt = NT_NORMAL;
 	cni.net = NET_COMBOBOX;
 	CString strStopValue;
 	strStopValue.Format(_T("stop time(mins): %s"),pTrainFlight->GetStopValue().getPrintDist());
 	m_hStopValueItem = m_wndTreeCtrl.InsertItem(strStopValue,cni,FALSE,FALSE,hStopItem);

	//Force and go
	cni.nt = NT_NORMAL;
	cni.net = NET_NORMAL;
	HTREEITEM hForceItem = m_wndTreeCtrl.InsertItem(_T("Full Stop and Takeoff"),cni,FALSE,FALSE,hInterntionsItem);

	//force num
	cni.nt = NT_NORMAL;
	cni.net = NET_EDIT_INT;
	cni.fMinValue = static_cast<float>(pTrainFlight->GetForceGo());
	CString strForceNum;
	strForceNum.Format(_T("Number: %d"),pTrainFlight->GetForceGo());
	m_hForceNumItem = m_wndTreeCtrl.InsertItem(strForceNum,cni,FALSE,FALSE,hForceItem);
 
 	//end point
 	cni.nt = NT_NORMAL;
 	cni.net = NET_COMBOBOX;
 	CString strEndPoint;
	if (pTrainFlight->GetEndPoint().IsBlank())
	{
		strEndPoint.Format(_T("Terminate point(Stands/SID): All"));
	}
	else
	{
		strEndPoint.Format(_T("Terminate point(Stands/SID): %s"),pTrainFlight->GetEndPoint().GetIDString());
	}
 	m_hEndPointItem = m_wndTreeCtrl.InsertItem(strEndPoint,cni,FALSE,FALSE);

	m_wndTreeCtrl.Expand(hInterntionsItem,TVE_EXPAND);
	m_wndTreeCtrl.Expand(hLowItem,TVE_EXPAND);
	m_wndTreeCtrl.Expand(hTouchItem,TVE_EXPAND);
	m_wndTreeCtrl.Expand(hStopItem,TVE_EXPAND);
	m_wndTreeCtrl.Expand(hForceItem,TVE_EXPAND);

}

void CDlgTrainFlightSpec::OnSelchangeListBox()
{
	int nCurSel = m_wndListBox.GetCurSel();
	if (nCurSel == LB_ERR)
	{
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_FLIGHT_TYPE_ADD,TRUE);
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_FLIGHT_TYPE_DEL,FALSE);
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_FLIGHT_TYPE_EDIT,FALSE);
		return;
	}
	
	m_wndToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_FLIGHT_TYPE_ADD,TRUE);
	m_wndToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_FLIGHT_TYPE_DEL,TRUE);
	m_wndToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_FLIGHT_TYPE_EDIT,FALSE);

	CTrainingFlightData* pTrainFlight = (CTrainingFlightData*)m_wndListBox.GetItemData(nCurSel);
	DisplayTrainFlight(pTrainFlight);
}

LRESULT CDlgTrainFlightSpec::DefWindowProc( UINT message, WPARAM wParam, LPARAM lParam )
{
 	switch(message)
 	{
 		case UM_CEW_LABLE_END:
 			{
				HTREEITEM hItem = (HTREEITEM)wParam;
				CString strValue = *((CString*)lParam);
				CTrainingFlightData* pTrainFlight = GetTrainFlightData();
				if (pTrainFlight)
				{
					int nStringLength = strValue.GetLength();
					if (nStringLength <2  || nStringLength > 3)
					{
						MessageBox(_T("Carrier code should have two or three characters!!"),"Warning",MB_OK);
						m_wndTreeCtrl.DoEdit(hItem);
					}
					else
					{
						pTrainFlight->SetCarrierCode(strValue);
						CString strText;
						strText.Format(_T("Carrier Code(2 or 3 characters): %s"),pTrainFlight->GetCarrierCode());
						m_wndTreeCtrl.SetItemText(hItem,strText);
					}
				}
 				GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
 			}
 			break;
 		case UM_CEW_EDITSPIN_END:
 			{
				HTREEITEM hItem = (HTREEITEM)wParam;
				CString strValue = *((CString*)lParam);
				CTrainingFlightData* pTrainFlight = GetTrainFlightData();
				if (pTrainFlight)
				{
					CString strFlight;
					COOLTREE_NODE_INFO* cni = m_wndTreeCtrl.GetItemNodeInfo(hItem);
					if (hItem == m_hNumFlightItem)
					{
						int nValue = atoi(strValue);
						pTrainFlight->SetNumFlight(nValue);
						cni->fMinValue = static_cast<float>(nValue);
						strFlight.Format(_T("Number of aircraft: %d"),pTrainFlight->GetNumFlight());
					}
					else if (hItem == m_hLowNumItem)
					{
						int nValue = atoi(strValue);
						pTrainFlight->SetLowFlight(nValue);
						cni->fMinValue = static_cast<float>(nValue);
						strFlight.Format(_T("Number: %d"),pTrainFlight->GetLowFlight());
					}
					else if (hItem == m_hTouchNumItem)
					{
						int nValue = atoi(strValue);
						pTrainFlight->SetTouchFlight(nValue);
						cni->fMinValue = static_cast<float>(nValue);
						strFlight.Format(_T("Number: %d"),pTrainFlight->GetTouchFlight());
					}
					else if (hItem == m_hStopNumItem)
					{
						int nValue = atoi(strValue);
						pTrainFlight->SetStopFlight(nValue);
						cni->fMinValue = static_cast<float>(nValue);
						strFlight.Format(_T("Number: %d"),pTrainFlight->GetStopFlight());
					}
					else if (hItem == m_hLowValueItem)
					{
						double dValue = atof(strValue);
						double dLowValue = ConvertValueToDefaultUnit(Unit_Length,dValue);
						pTrainFlight->SetLowValue(dLowValue);
						cni->fMinValue = static_cast<float>(dLowValue);
						strFlight.Format(_T("feet above runway threshold(%s): %.2f"),GetUintString(Unit_Length),dValue);
					}
					else if (hItem == m_hTouchValueItem)
					{
						double dValue = atof(strValue);
						double dTouchValue = ConvertValueToDefaultUnit(Unit_Speed,dValue);
						pTrainFlight->SetTouchValue(dValue);
						cni->fMinValue = static_cast<float>(dTouchValue);
						strFlight.Format(_T("deceleratingn to speed(%s): %.2f"),GetUintString(Unit_Speed),dValue);
					}
					else if (hItem == m_hForceNumItem)
					{
						int nValue = atoi(strValue);
						pTrainFlight->SetForceGo(nValue);
						cni->fMinValue = static_cast<float>(nValue);
						strFlight.Format(_T("Number: %d"),pTrainFlight->GetForceGo());
					}
					
					m_wndTreeCtrl.SetItemText(hItem,strFlight);
					GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
				}
 				
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
				
 				if (hItem == m_hActypItem)
 				{									
 					for(size_t i=0; i<sortAcTypeList.size(); i++) 
 					{ 						
 						int nIdx = pCB->AddString( sortAcTypeList[i] );
 						pCB->SetItemData( nIdx, i );
 					}					
 				}
				else if(hItem == m_hCarrierItem)
				{					
					for(size_t i=0; i<sortAirlineList.size(); i++) 
					{ 						
						int nIdx = pCB->AddString( sortAirlineList[i] );
						pCB->SetItemData( nIdx, i );
					}	
				}
 				else if(hItem == m_hDistItem || hItem == m_hStopValueItem)
 				{
 					CStringList strList;
 					CString s(_T("New Probability Distribution..."));
 					pCB->AddString(s);
 					int nCount = m_pAirsideInput->m_pAirportDB->getProbDistMan()->getCount();
 					for( int m=0; m<nCount; m++ )
 					{
 						CProbDistEntry* pPBEntry =m_pAirsideInput->m_pAirportDB->getProbDistMan()->getItem( m );			
 						pCB->AddString(pPBEntry->m_csName);
 					}
 				}
 				else if (hItem == m_hStartPointItem)
 				{
 					pCB->AddString(_T("Select Stand"));
 					pCB->AddString(_T("Select STARs"));
 				}
 				else if (hItem == m_hEndPointItem)
 				{
 					pCB->AddString(_T("Select Stand"));
 					pCB->AddString(_T("SID"));
 				}
 			}
 			break;
 		case UM_CEW_COMBOBOX_SELCHANGE:
 			{
				HTREEITEM hItem = (HTREEITEM)wParam;
				CString strValue = *((CString*)lParam);
				CTrainingFlightData* pTrainFlight = GetTrainFlightData();
				if (pTrainFlight)
				{
					int nIndexSeled = m_wndTreeCtrl.GetCmbBoxCurSel(hItem);
					if (hItem == m_hActypItem)
					{
						CString strActype = sortAcTypeList.at(nIndexSeled);
						pTrainFlight->SetAcType(strActype);
						
						strActype.Format(_T("AC Type: %s"),pTrainFlight->GetAcType());
						m_wndTreeCtrl.SetItemText(hItem,strActype);

					}
					else if(hItem == m_hCarrierItem){
						CString strCarrer = sortAirlineList.at(nIndexSeled);
						pTrainFlight->SetCarrierCode(strCarrer);
						strCarrer.Format(_T("Carrier Code(2 or 3 characters): %s"),pTrainFlight->GetCarrierCode());
						m_wndTreeCtrl.SetItemText(hItem,strCarrer);
					}
					else if (hItem == m_hDistItem)
					{
						CProbDistEntry* pEntry = ChangeProbDist(hItem,strValue);
						if (pEntry)
						{
							pTrainFlight->SetDistribution(pEntry);
							CString strDist;
							strDist.Format(_T("Interarrival distribution(mins): %s"),pTrainFlight->GetDistribution().getPrintDist());
							m_wndTreeCtrl.SetItemText(hItem,strDist);
						}

					}
					else if (hItem == m_hStopValueItem)
					{
						CProbDistEntry* pEntry = ChangeProbDist(hItem,strValue);
						if (pEntry)
						{
							pTrainFlight->SetStopValue(pEntry);
							CString strStopValue;
							strStopValue.Format(_T("stop time(mins): %s"),pTrainFlight->GetStopValue().getPrintDist());
							m_wndTreeCtrl.SetItemText(hItem,strStopValue);
						}
					}
					else if (hItem == m_hStartPointItem)
					{
						int nIndexSeled=m_wndTreeCtrl.GetCmbBoxCurSel(hItem);
						if(nIndexSeled == 0)
						{
							CDlgSelectALTObject dlgSelect(m_nProjID,ALT_STAND);
							if(dlgSelect.DoModal()==IDOK )
							{
								CString strStartPoint;
								if(dlgSelect.GetALTObject().IsBlank())
									strStartPoint.Format(_T("Start Point(Stands or STARS): All"));
								else
									strStartPoint.Format(_T("Start Point(Stands or STARS): %s"),dlgSelect.GetALTObject().GetIDString());
								m_wndTreeCtrl.SetItemText(hItem,strStartPoint);
								pTrainFlight->SetStartStand(true);
								pTrainFlight->SetStartPoint(dlgSelect.GetALTObject());
								m_wndTreeCtrl.SetItemText(hItem,strStartPoint);
							}
							else
							{
								CString strStartPoint;
								if(pTrainFlight->GetStartPoint().IsBlank())
									strStartPoint.Format(_T("Start Point(Stands/SID): All)"));
								else
									strStartPoint.Format(_T("Start Point(Stands/SID): %s"),pTrainFlight->GetStartPoint().GetIDString());
								m_wndTreeCtrl.SetItemText(hItem,strStartPoint);
							}
						}
						else
						{
							CDlgSelectAirrouteObject dlgSelect(m_nProjID,CAirRoute::STAR);
							if(dlgSelect.DoModal()==IDOK )
							{
								CString strStartPoint;
								if(dlgSelect.GetALTObject().IsBlank())
									strStartPoint.Format(_T("Start Point(Stands or STARS): All"));
								else
									strStartPoint.Format(_T("Start Point(Stands or STARS): %s"),dlgSelect.GetALTObject().GetIDString());
								m_wndTreeCtrl.SetItemText(hItem,strStartPoint);
								pTrainFlight->SetStartStand(false);
								pTrainFlight->SetStartPoint(dlgSelect.GetALTObject());
								m_wndTreeCtrl.SetItemText(hItem,strStartPoint);
							}
							else
							{
								CString strStartPoint;
								if(pTrainFlight->GetStartPoint().IsBlank())
									strStartPoint.Format(_T("Start Point(Stands/SID): All)"));
								else
									strStartPoint.Format(_T("Start Point(Stands/SID): %s"),pTrainFlight->GetStartPoint().GetIDString());
								m_wndTreeCtrl.SetItemText(hItem,strStartPoint);
							}
						}
					}
					else if (hItem == m_hEndPointItem)
					{
						int nIndexSeled=m_wndTreeCtrl.GetCmbBoxCurSel(hItem);
						if(nIndexSeled == 0)
						{
							CDlgSelectALTObject dlgSelect(m_nProjID,ALT_STAND);
							if(dlgSelect.DoModal()==IDOK )
							{
								CString strEndPoint;
								if(dlgSelect.GetALTObject().IsBlank())
									strEndPoint.Format(_T("Terminate point(Stands/SID): All)"));
								else
									strEndPoint.Format(_T("Terminate point(Stands/SID): %s"),dlgSelect.GetALTObject().GetIDString());
								m_wndTreeCtrl.SetItemText(hItem,strEndPoint);
								pTrainFlight->SetEndStand(true);
								pTrainFlight->SetEndPoint(dlgSelect.GetALTObject());
								m_wndTreeCtrl.SetItemText(hItem,strEndPoint);
							}
							else
							{
								CString strEndPoint;
								if(pTrainFlight->GetEndPoint().IsBlank())
									strEndPoint.Format(_T("Terminate point(Stands/SID): All)"));
								else
									strEndPoint.Format(_T("Terminate point(Stands/SID): %s"),pTrainFlight->GetEndPoint().GetIDString());
								m_wndTreeCtrl.SetItemText(hItem,strEndPoint);
							}
						}
						else
						{
							CDlgSelectAirrouteObject dlgSelect(m_nProjID,CAirRoute::SID);
							if(dlgSelect.DoModal()==IDOK )
							{
								CString strEndPoint;
								if(dlgSelect.GetALTObject().IsBlank())
									strEndPoint.Format(_T("Terminate point(Stands/SID): All)"));
								else
									strEndPoint.Format(_T("Terminate point(Stands/SID): %s"),dlgSelect.GetALTObject().GetIDString());
								m_wndTreeCtrl.SetItemText(hItem,strEndPoint);
								pTrainFlight->SetEndStand(false);
								pTrainFlight->SetEndPoint(dlgSelect.GetALTObject());
								m_wndTreeCtrl.SetItemText(hItem,strEndPoint);
							}
							else
							{
								CString strEndPoint;
								if(pTrainFlight->GetEndPoint().IsBlank())
									strEndPoint.Format(_T("Terminate point(Stands/SID): All)"));
								else
									strEndPoint.Format(_T("Terminate point(Stands/SID): %s"),pTrainFlight->GetEndPoint().GetIDString());
								m_wndTreeCtrl.SetItemText(hItem,strEndPoint);
							}
						}
					}
					GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
				}
 			}
 			break;
 		case WM_LBUTTONDBLCLK:
 			{
 				OnLButtonDblClk(wParam,lParam);
 			}
 			break;
 		default:
 			break;
 	}
	return CXTResizeDialog::DefWindowProc(message,wParam,lParam);
}

void CDlgTrainFlightSpec::OnLButtonDblClk( WPARAM wParam, LPARAM lParam )
{
 	UINT uFlags;
 	CPoint point = *(CPoint*)lParam;
 	HTREEITEM hItem = m_wndTreeCtrl.HitTest(point, &uFlags);
 
 	CTrainingFlightData* pTrainFlight = NULL;
 	int nSel = m_wndListBox.GetCurSel();
 	if (nSel != LB_ERR)
 		pTrainFlight = (CTrainingFlightData*)m_wndListBox.GetItemData(nSel);
 	if (pTrainFlight == NULL)
 		return;
 	
 	if (hItem == m_hTimeRange)
 	{
 		TimeRange timeRange = pTrainFlight->GetTimeRange();
 		CDlgTimeRange dlg(timeRange.GetStartTime(), timeRange.GetEndTime());
 		if(IDOK!= dlg.DoModal())
 			return;
 
 		ElapsedTime tStartTime = dlg.GetStartTime();
 		ElapsedTime tEndTime = dlg.GetEndTime();
 
 		pTrainFlight->SetTimeRange(tStartTime,tEndTime);
 		//timeRange = pTrainFlight->GetTimeRange();
 		CString strTimeRange;
 		strTimeRange.Format(_T("Time Range: %s"),pTrainFlight->GetTimeRaneString());
 		m_wndTreeCtrl.SetItemText(hItem,strTimeRange);
		GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
 	}
}

CProbDistEntry* CDlgTrainFlightSpec::ChangeProbDist( HTREEITEM hItem,const CString& strDist)
{
 	int nIndexSeled=m_wndTreeCtrl.GetCmbBoxCurSel(hItem);
 	CString strValue = _T("New Probability Distribution...");
 
 	CProbDistEntry* pPDEntry = NULL;
 	if( strcmp( strValue, strDist ) == 0 )
 	{
 		pPDEntry = (*m_pSelectProbDistEntry)(NULL, m_pAirsideInput);
 	}
 	else
 	{
 		int nCount = m_pAirsideInput->m_pAirportDB->getProbDistMan()->getCount();
 		for( int i=0; i<nCount; i++ )
 		{
 			pPDEntry = m_pAirsideInput->m_pAirportDB->getProbDistMan()->getItem( i );
 			if( strcmp( pPDEntry->m_csName, strDist ) == 0 )
 				break;
 		}
 	}
 
 	return pPDEntry;
}

CTrainingFlightData* CDlgTrainFlightSpec::GetTrainFlightData()
{
	int nSel = m_wndListBox.GetCurSel();
	if (nSel == LB_ERR)
		return NULL;
	
	return (CTrainingFlightData*)m_wndListBox.GetItemData(nSel);
}

void CDlgTrainFlightSpec::OnOK()
{
	for (unsigned i = 0; i < m_trainFlightContainer.GetElementCount();i++)
	{
		CTrainingFlightData* pTrainFlight = m_trainFlightContainer.GetItem(i);
		if (pTrainFlight->GetCarrierCode().IsEmpty())
		{
			CString strWarning;
			strWarning.Format(_T("Carrier code of %s can't be empty"),pTrainFlight->GetName());
			MessageBox(strWarning,"Warning",MB_OK);
			return;
		}
	}
	try
	{
		CADODatabase::BeginTransaction();
		m_trainFlightContainer.SaveData(-1);
		CADODatabase::CommitTransaction();
	}
	catch (CADOException& e)
	{
		e.ErrorMessage();
		CADODatabase::RollBackTransation();
		MessageBox(_T("Save database error"),_T("ERROR"),MB_OK);
		return;
	}

	CXTResizeDialog::OnOK();
}

void CDlgTrainFlightSpec::OnSave()
{
	for (unsigned i = 0; i < m_trainFlightContainer.GetElementCount();i++)
	{
		CTrainingFlightData* pTrainFlight = m_trainFlightContainer.GetItem(i);
		if (pTrainFlight->GetCarrierCode().IsEmpty())
		{
			CString strWarning;
			strWarning.Format(_T("Carrier code of %s can't be empty"),pTrainFlight->GetName());
			MessageBox(strWarning,"Warning",MB_OK);
			return;
		}
	}

	try
	{
		CADODatabase::BeginTransaction();
		m_trainFlightContainer.SaveData(-1);
		CADODatabase::CommitTransaction();
	}
	catch (CADOException& e)
	{
		e.ErrorMessage();
		CADODatabase::RollBackTransation();
		MessageBox(_T("Save database error"),_T("ERROR"),MB_OK);
		return;
	}
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(FALSE);
}

bool CDlgTrainFlightSpec::ConvertUnitFromDBtoGUI( void )
{
	return (false);
}

bool CDlgTrainFlightSpec::RefreshGUI( void )
{
	int nSel = m_wndListBox.GetCurSel();
	if (nSel == LB_ERR)
		return false;

	CTrainingFlightData* pTrainFlight = (CTrainingFlightData*)m_wndListBox.GetItemData(nSel);
	DisplayTrainFlight(pTrainFlight);
	return true;
}

bool CDlgTrainFlightSpec::ConvertUnitFromGUItoDB( void )
{
	return (false);
}

CString CDlgTrainFlightSpec::GetUintString( UnitType emType )
{
	if (emType == Unit_Length)
	{
		return CARCLengthUnit::GetLengthUnitString(CUnitPiece::GetCurSelLengthUnit());
	}
	else if (emType == Unit_Speed)
	{
		return CARCVelocityUnit::GetVelocityUnitString(CUnitPiece::GetCurSelVelocityUnit());
	}
	else if (emType == Unit_Time)
	{
	
		return CARCTimeUnit::GetTimeUnitString(CUnitPiece::GetCurSelTimeUnit());
	}
	return CString(_T(""));
}

double CDlgTrainFlightSpec::ConvertValueToDefaultUnit( UnitType emType,double dValue )
{
	if (emType == Unit_Length)
	{
		return CARCLengthUnit::ConvertLength(CUnitPiece::GetCurSelLengthUnit(),DEFAULT_DATABASE_LENGTH_UNIT,dValue);
	}
	else if (emType == Unit_Speed)
	{
		return CARCVelocityUnit::ConvertVelocity(CUnitPiece::GetCurSelVelocityUnit(),DEFAULT_DATABASE_VELOCITY_UNIT,dValue);
	}
	else if (emType == Unit_Time)
	{
		return CARCTimeUnit::ConvertTime(CUnitPiece::GetCurSelTimeUnit(),DEFAULT_DATABASE_TIME_UNIT,dValue);
	}
	return 0.0;
}

double CDlgTrainFlightSpec::ConvertValueToCurrentUnit( UnitType emType,double dValue )
{
	if (emType == Unit_Length)
	{
		return CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),dValue);
	}
	else if (emType == Unit_Speed)
	{
		return CARCVelocityUnit::ConvertVelocity(DEFAULT_DATABASE_VELOCITY_UNIT,CUnitPiece::GetCurSelVelocityUnit(),dValue);
	}
	else if (emType == Unit_Time)
	{
		return CARCTimeUnit::ConvertTime(DEFAULT_DATABASE_TIME_UNIT,CUnitPiece::GetCurSelTimeUnit(),dValue);
	}
	return 0.0;
}