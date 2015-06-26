// ReportParamDlg.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "ReportParamDlg.h"
#include "inputs\in_term.h"
#include "reports\ReportType.h"
#include "../common/UnitsManager.h"
#include "TermPlanDoc.h"
#include "ProcesserDialog.h"
#include "FlightDialog.h"
#include "PassengerTypeDialog.h"
#include "reports\ReportParaDB.h"
#include "reports\ReportParameter.h"
#include "reports\reportParaOfTime.h"
#include "reports\reportParaOfProcs.h"
#include "reports\reportParaOfReportType.h"
#include "reports\reportParaOfThreshold.h"
#include "reports\reportParaOfPaxtype.h"
#include "reports\reportParaWithArea.h"
#include "reports\reportParaWithPortal.h"
//#include "common\ARCCriticalException.h"
#include "reports\ReportParaOfTwoGroupProcs.h"
#include "./MFCExControl/ARCBaseTree.h"
#include "RepControlView.h"
#include "NodeView.h"
#include <Inputs/PROCIDList.h>
#include "Common/SimAndReportManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CReportParamDlg dialog
CReportParamDlg::CReportParamDlg( int _type,InputTerminal* _pTerm, CWnd* pParent )
	: CDialog(CReportParamDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CReportParamDlg)
	m_oleDTEndTime		= COleDateTime::GetCurrentTime();
	m_oleDTStartTime	= COleDateTime::GetCurrentTime();
	m_nRepType			= 0;
	m_nInterval			= 1;
	m_oleDTInterval		= COleDateTime::GetCurrentTime();
	m_fThreshold		= 0.0f;
	m_oleThreshold		= COleDateTime::GetCurrentTime();

	m_oleDTStartDate = COleDateTime::GetCurrentTime();
	m_oleDTEndDate = COleDateTime::GetCurrentTime();
	//}}AFX_DATA_INIT
	m_iReportType = _type;
	m_pTerm = _pTerm;

	m_pReportParam = NULL;	
	m_vProcList.clear();
	m_bNewData = false;
}
CReportParamDlg::~CReportParamDlg()
{
	for (int i = 0 ; i <(int) m_TreeDataProID.size() ; i ++)
	{
		if(m_TreeDataProID[i] != NULL)
			delete m_TreeDataProID[i] ;
	}
}

void CReportParamDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CReportParamDlg)
	DDX_Control(pDX, IDC_BUTTON_LOADFILE, m_btnLoad);
	DDX_Control(pDX, IDC_BUTTON_SAVEFILE, m_btnSave);
	DDX_Control(pDX, IDC_BUTTON_PASTE, m_btnPaste);
	DDX_Control(pDX, IDC_BUTTON_COPY, m_btnCopy);
	DDX_Control(pDX,IDC_TREE_PAXTYPE,m_treePaxType);
	DDX_Control(pDX, IDC_DATETIMEPICKER_THRESHOLD, m_dtctrlThreshold);
	DDX_Control(pDX, IDC_DATETIMEPICKER_INTERVAL, m_dtctrlInterval);
	DDX_Control(pDX, IDC_SPIN_THRESHOLD, m_spinThreshold);
	DDX_Control(pDX, IDC_STATIC_AREA, m_staticArea);
	DDX_Control(pDX, IDC_COMBO_AREA, m_comboAreaPortal);
	DDX_Control(pDX, IDC_STATIC_THRESHOLD, m_staticThreshold);
	DDX_Control(pDX, IDC_STATIC_TYPE, m_staticType);
	DDX_Control(pDX, IDC_STATIC_INTERVAL, m_staticInterval);
	DDX_Control(pDX, IDC_STATICPROTYPE, m_staProc);
	DDX_Control(pDX, IDC_STATICPASTYPE, m_staPax);
	DDX_Control(pDX, IDC_TOOLBARCONTENTER2, m_toolbarcontenter2);
	DDX_Control(pDX, IDC_TOOLBARCONTENTER1, m_toolbarcontenter1);
	DDX_Control(pDX, IDC_LIST_PROTYPE, m_lstboProType);
	DDX_Control(pDX, ID_BUTTON_CANCEL, m_btnCancel);
	DDX_Control(pDX, IDC_DATETIMEPICKER_START, m_dtctrlStartTime);
	DDX_Control(pDX, IDC_DATETIMEPICKER_END, m_dtctrlEndTime);
	DDX_Control(pDX, IDC_RADIO_DETAILED, m_radioDetailed);
	DDX_Control(pDX, IDC_RADIO_SUMMARY, m_radioSummary);
	DDX_Control(pDX, IDC_LIST_PAXTYPE, m_lstboPaxType);
	DDX_Control(pDX, IDC_EDIT_THRESHOLD, m_edtThreshold);
	DDX_Control(pDX, IDC_CHECK_THRESHOLD, m_chkThreshold);
	DDX_Control(pDX, IDC_BUTTON_APPLY, m_btnApply);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER_END, m_oleDTEndTime);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER_START, m_oleDTStartTime);
	DDX_Radio(pDX, IDC_RADIO_DETAILED, m_nRepType);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER_INTERVAL, m_oleDTInterval);
	DDX_Text(pDX, IDC_EDIT_THRESHOLD, m_fThreshold);
	DDV_MinMaxFloat(pDX, m_fThreshold, 0.f, 1000.f);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER_THRESHOLD, m_oleThreshold);
	
	DDX_Control(pDX, IDC_DLGDATETIMEPICKER_START2, m_dtctrlStartDate);
	DDX_DateTimeCtrl(pDX, IDC_DLGDATETIMEPICKER_START2, m_oleDTStartDate);
	DDX_Control(pDX, IDC_DLGDATETIMEPICKER_END2, m_dtctrlEndDate);
	DDX_DateTimeCtrl(pDX, IDC_DLGDATETIMEPICKER_END2, m_oleDTEndDate);
	DDX_Control(pDX, IDC_DLGCOMBO_STARTDAY, m_comboStartDay);
	DDX_Control(pDX, IDC_DLGCOMBO_ENDDAY, m_comboEndDay);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CReportParamDlg, CDialog)
	//{{AFX_MSG_MAP(CReportParamDlg)
	ON_LBN_SELCHANGE(IDC_LIST_PAXTYPE, OnSelchangeListPaxtype)
	ON_WM_CREATE()
	ON_COMMAND(ID_PEOPLEMOVER_NEW, OnPeoplemoverNew)
	ON_COMMAND(ID_PEOPLEMOVER_DELETE, OnPeoplemoverDelete)
	ON_COMMAND(ID_TOOLBARBUTTONADD, OnToolbarbuttonadd)
	ON_COMMAND(ID_TOOLBARBUTTONDEL, OnToolbarbuttondel)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, OnButtonApply)
	ON_BN_CLICKED(ID_BUTTON_CANCEL, OnButtonCancel)
	ON_LBN_SELCHANGE(IDC_LIST_PROTYPE, OnSelchangeListProtype)
	ON_BN_CLICKED(IDC_BUTTON_PASTE, OnButtonPaste)
	ON_BN_CLICKED(IDC_BUTTON_COPY, OnButtonCopy)
	ON_BN_CLICKED(IDC_BUTTON_LOADFILE, OnButtonLoadfile)
	ON_BN_CLICKED(IDC_BUTTON_SAVEFILE, OnButtonSavefile)
	ON_BN_CLICKED(IDC_RADIO_DETAILED, OnRadioDetailed)
	ON_BN_CLICKED(IDC_RADIO_SUMMARY, OnRadioSummary)
	//}}AFX_MSG_MAP
	ON_UPDATE_COMMAND_UI(ID_TOOLBARBUTTONADD,OnUpdateBtnAdd)
	ON_UPDATE_COMMAND_UI(ID_TOOLBARBUTTONDEL, OnUpdateBtnDel)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CReportParamDlg message handlers

BOOL CReportParamDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	InitToolbar();
	//////////////////////////////////////////////////////////////////////////
	COleDateTime oleStartTime,oleEndTime,oleInterval,oleThreshold;
	oleStartTime.SetTime( 0, 0, 0 );
	m_dtctrlStartTime.SetTime( oleStartTime);
	oleEndTime = oleStartTime + COleDateTimeSpan( 1, 0, 0, 0 );
	m_dtctrlEndTime.SetTime(oleEndTime);
	
	m_dtctrlInterval.SetFormat( "HH:mm" );
	oleInterval.SetTime( 1, 0, 0 );
	m_dtctrlInterval.SetTime( oleInterval );
	
	m_dtctrlThreshold.SetFormat( "HH:mm" );
	oleThreshold.SetTime( 1, 0, 0 );
	m_dtctrlInterval.SetTime( oleThreshold );
	
	m_spinThreshold.SetRange( 0, 1000 );
	
	m_paxConst.initDefault( m_pTerm );
	m_startDate = m_pDoc->GetTerminal().GetSimReportManager()->GetStartDate();

	CString strReportType;
	getReportString(strReportType);
	m_pReportParam = getReportParam( strReportType );
	assert( m_pReportParam );

	InitAllContent();
	Clear();

	ReadDataFromLib();
	m_btnApply.SetWindowText("OK");
	// disable/enable paste button 
	assert( m_pParentWnd );
	m_btnPaste.EnableWindow( ((CNodeView*)m_pParentWnd)->getCopyReportPara() != NULL ? TRUE : FALSE );

//by aivin
	InitComboDay(NULL);
	InitControl();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CReportParamDlg::InitComboDay(std::vector<std::string>* pvString)
{
	std::vector<std::string>* pVect = NULL;
	std::vector<std::string> vString;
	int i = 0;
	if(pvString != NULL)
		pVect = pvString;
	else
	{
		for(i = 0; i < 3; i++)
		{
			char strDay[64];
			sprintf(strDay, "Day %d", i + 1);
			vString.push_back(strDay);
		}
		pVect = &vString;
	}
	

	m_comboStartDay.ResetContent();
	m_comboEndDay.ResetContent();
	
	for(i = 0; i < static_cast<int>(pVect->size()); i++)
	{
	    m_comboStartDay.InsertString(i, pVect->at(i).c_str());
		m_comboEndDay.InsertString(i, pVect->at(i).c_str());
	}
	
	if(m_comboStartDay.GetCount() > 0)
		m_comboStartDay.SetCurSel(0);
	if(m_comboEndDay.GetCount() > 0)
		m_comboEndDay.SetCurSel(0);
}

//by aivin add fouc
void CReportParamDlg::InitControl()
{
	if(m_startDate.IsAbsoluteDate())
	{
		m_dtctrlStartDate.ShowWindow(SW_SHOW);
		m_dtctrlEndDate.ShowWindow(SW_SHOW);
	
		m_comboStartDay.ShowWindow(SW_HIDE);
		m_comboEndDay.ShowWindow(SW_HIDE);
	}
	else								
	{	
		m_comboStartDay.ShowWindow(SW_SHOW);
		m_comboEndDay.ShowWindow(SW_SHOW);

		m_dtctrlStartDate.ShowWindow(SW_HIDE);
		m_dtctrlEndDate.ShowWindow(SW_HIDE);
	}

}

void CReportParamDlg::InitToolbar()
{
	CRect rc;
	m_toolbarcontenter1.GetWindowRect( &rc );
	ScreenToClient(&rc);
	m_PasToolBar.MoveWindow(&rc);
	m_PasToolBar.ShowWindow(SW_SHOW);
	m_toolbarcontenter1.ShowWindow(SW_HIDE);
	m_PasToolBar.GetToolBarCtrl().EnableButton( ID_TOOLBARBUTTONDEL,FALSE );
	m_PasToolBar.GetToolBarCtrl().EnableButton( ID_TOOLBARBUTTONADD );
	m_PasToolBar.GetToolBarCtrl().HideButton( ID_TOOLBARBUTTONEDIT);
	
	m_toolbarcontenter2.GetWindowRect( &rc );
	ScreenToClient(&rc);
	m_ProToolBar.MoveWindow(&rc);
	m_ProToolBar.ShowWindow(SW_SHOW);
	m_toolbarcontenter2.ShowWindow(SW_HIDE);
	m_ProToolBar.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_NEW );
	m_ProToolBar.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_DELETE,FALSE );
	m_ProToolBar.GetToolBarCtrl().HideButton( ID_PEOPLEMOVER_CHANGE);
}


int CReportParamDlg::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	if (!m_PasToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_PasToolBar.LoadToolBar(IDR_ADDDELEDITTOOLBAR))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}
	if (!m_ProToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_ProToolBar.LoadToolBar(IDR_PEOPLEMOVEBAR))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}
	
	return 0;
}

void CReportParamDlg::InitAllContent()
{
	UpdateData();
	m_nInterval		= 1;
	m_oleDTInterval.SetTime( 0,0,0 );
	m_fThreshold	= 0.0f;
	m_oleThreshold.SetTime( 0,0,0 );
	UpdateData( FALSE );
		
	switch( m_iReportType )
	{
	case ENUM_DISTANCE_REP:	
		{
			CUnitsManager* pUM  = CUnitsManager::GetInstance();
			int nUnit			= pUM->GetLengthUnits();				
			CString str			= "Threshold (" + pUM->GetLengthUnitString( nUnit ) + ")";
			m_staticThreshold.SetWindowText( str );
			break;
		}
	case ENUM_AVGQUEUELENGTH_REP:
	case ENUM_QUEUELENGTH_REP:
	case ENUM_BAGCOUNT_REP:
	case ENUM_THROUGHPUT_REP:
	case ENUM_PAXCOUNT_REP:
	case ENUM_CONVEYOR_WAIT_LENGTH_REP:
		m_staticThreshold.SetWindowText( "Threshold (count)" );
		break;
	case ENUM_BAGWAITTIME_REP:
	case ENUM_DURATION_REP:
	case ENUM_QUEUETIME_REP:
	case ENUM_SERVICETIME_REP:
	case ENUM_FIREEVACUTION_REP:
	case ENUM_CONVEYOR_WAIT_TIME_REP:
		m_staticThreshold.SetWindowText( "Threshold (min)" );
		break;
	default:
		m_staticThreshold.SetWindowText( "Threshold" );
	}
	
	//Set Pax Type to Default
	//and Clear Processor
	ReloadSpecs();
	m_paxConst.initDefault( m_pTerm );
	LoadPaxList();
	m_strProcessor="";
}

void CReportParamDlg::LoadPaxList()
{
	switch(m_iReportType)
	{
	case ENUM_THROUGHPUT_REP:
		{
			m_treePaxType.DeleteAllItems() ;
			m_treePaxType.SetRedraw(FALSE);
			m_throughput_paxroot = m_treePaxType.InsertItem(_T("Pax Type")) ;
			m_throughput_proroot = m_treePaxType.InsertItem(_T("Processor")) ;
			m_treePaxType.SetItemData(m_throughput_paxroot,(DWORD)PAX_TYPE) ;
			m_treePaxType.SetItemData(m_throughput_proroot,(DWORD)THROUGH_PUT_PROCESSOR);
			int count=m_paxConst.getCount();
			CString str;
			HTREEITEM hItem = NULL ;
			for(int i=0;i<count;i++)
			{
				const CMobileElemConstraint* paxConstr = m_paxConst.getConstraint( i );
				paxConstr->screenPrint( str, 0, 128 );
				hItem=m_treePaxType.InsertItem(str,m_throughput_paxroot);
				m_treePaxType.SetItemData(hItem,i);
			}
			m_treePaxType.SetRedraw(TRUE);
			m_treePaxType.RedrawWindow();
		}
		break;
	case ENUM_DISTANCE_REP:
	case ENUM_DURATION_REP:
		{
			Terminal* pTmnl=(Terminal*)m_pTerm;
			InputTerminal* pInTmnl=(InputTerminal*)pTmnl;

			m_treePaxType.DeleteAllItems();
			m_treePaxType.SetRedraw(FALSE);
			m_hItemPaxType=m_treePaxType.InsertItem("Pax Type");
			int count=m_paxConst.getCount();
			if(count==0)
			{
				CMobileElemConstraint def(pTmnl);
				m_paxConst.addConstraint(def);
			}

			for(int i=0;i<count;i++)
			{
				const CMobileElemConstraint* paxConstr = m_paxConst.getConstraint( i );
				//char str[128];
				CString str;
				//paxConstr->screenPrint( str, 128 );
				paxConstr->screenPrint( str, 0, 128 );
				HTREEITEM hItem=m_treePaxType.InsertItem(str.GetBuffer(0),m_hItemPaxType);
				m_treePaxType.SetItemData(hItem,i);
			}

			HTREEITEM hItemFromTo=m_treePaxType.InsertItem("Processor To Processor");

			CReportParameter::FROM_TO_PROCS _fromToProcs;
			m_pReportParam->GetFromToProcs(_fromToProcs);


			if(_fromToProcs.m_vFromProcs.size()==0)
			{
				const ProcessorID* pID=pInTmnl->procList->getProcessor(START_PROCESSOR_INDEX)->getID();
				_fromToProcs.m_vFromProcs.push_back(*pID);
				m_pReportParam->SetFromToProcs(_fromToProcs);
			}

			if(_fromToProcs.m_vToProcs.size()==0)
			{
				const ProcessorID* pID=pInTmnl->procList->getProcessor(END_PROCESSOR_INDEX)->getID();
				_fromToProcs.m_vToProcs.push_back(*pID);
				m_pReportParam->SetFromToProcs(_fromToProcs);

			}



			m_hItemFrom=m_treePaxType.InsertItem("From",hItemFromTo);
			for(int k=0;k<static_cast<int>(_fromToProcs.m_vFromProcs.size());k++)
			{
				HTREEITEM hItem=m_treePaxType.InsertItem(_fromToProcs.m_vFromProcs[k].GetIDString(),m_hItemFrom);
				m_treePaxType.SetItemData(hItem,k);
			}

			m_hItemTo=m_treePaxType.InsertItem("To",hItemFromTo);
			for(int j=0;j<static_cast<int>(_fromToProcs.m_vToProcs.size());j++)
			{
				HTREEITEM hItem=m_treePaxType.InsertItem(_fromToProcs.m_vToProcs[j].GetIDString(),m_hItemTo);
				m_treePaxType.SetItemData(hItem,j);
			}

			m_treePaxType.SetRedraw(TRUE);
			m_treePaxType.RedrawWindow();
		}
		break;
	default:
		{
			m_lstboPaxType.ResetContent();
			int count = m_paxConst.getCount();
			for( int i=0; i<count; i++ )
			{
				const CMobileElemConstraint* paxConstr = m_paxConst.getConstraint( i );
				//char str[128];
				CString str;
				paxConstr->screenPrint( str, 0, 128 );
				m_lstboPaxType.AddString( str.GetBuffer(0) );
			}
			m_lstboPaxType.SetCurSel( -1 );
		}
		break;
	}
	OnSelchangeListPaxtype();
}

void CReportParamDlg::OnSelchangeListPaxtype() 
{
	// TODO: Add your control notification handler code here
	int nCurSel = m_lstboPaxType.GetCurSel();
	if( nCurSel == LB_ERR )
		m_PasToolBar.GetToolBarCtrl().EnableButton( ID_TOOLBARBUTTONDEL,FALSE );
	else
		m_PasToolBar.GetToolBarCtrl().EnableButton( ID_TOOLBARBUTTONDEL );	
}

void CReportParamDlg::ReloadSpecs()
{
	CRect rect;
	CRect rc;
	switch( m_iReportType )
	{
	case ENUM_PAXLOG_REP:
	case ENUM_DISTANCE_REP:
	case ENUM_DURATION_REP:
	case ENUM_ACTIVEBKDOWN_REP:
	case ENUM_PAXCOUNT_REP:
	case ENUM_PAXDENS_REP:
	case ENUM_SPACETHROUGHPUT_REP:
	case ENUM_ACOPERATION_REP:
	case ENUM_BAGDISTRIBUTION_REP:
	case ENUM_BAGDELIVTIME_REP:
	case ENUM_COLLISIONS_REP:
	case ENUM_FIREEVACUTION_REP:
		break;
		
	case ENUM_UTILIZATION_REP:
	case ENUM_THROUGHPUT_REP:
	case ENUM_QUEUELENGTH_REP:
	case ENUM_AVGQUEUELENGTH_REP:
	case ENUM_BAGCOUNT_REP:
	case ENUM_CONVEYOR_WAIT_LENGTH_REP:
		m_staPax.GetWindowRect(&rc);
		ScreenToClient(&rc);
		m_PasToolBar.GetWindowRect(&rc);
		ScreenToClient(&rc);
		break;
	default:
		
		
		m_staPax.GetWindowRect(&rc);
		rc.left+=160;
		rc.right+=160;
		ScreenToClient(&rc);
		
		m_PasToolBar.GetWindowRect(&rc);
		rc.left+=160;
		rc.right+=160;
		ScreenToClient(&rc);
		break;
	}

	//OnBtnModify();
	ModifyControl();
}

void CReportParamDlg::ModifyControl()
{
	// SET UP PAXLIST AND PROC LIST
	switch( m_iReportType )
	{
	case ENUM_PAXLOG_REP:
	case ENUM_DISTANCE_REP:
	case ENUM_DURATION_REP:
	case ENUM_ACTIVEBKDOWN_REP:
	case ENUM_PAXCOUNT_REP:
	case ENUM_PAXDENS_REP:
	case ENUM_SPACETHROUGHPUT_REP:
	case ENUM_ACOPERATION_REP:
//	case ENUM_BAGDISTRIBUTION_REP:
//	case ENUM_BAGDELIVTIME_REP:
	case ENUM_COLLISIONS_REP:
	case ENUM_FIREEVACUTION_REP:
		{
			// show pax list only
			m_lstboProType.ShowWindow( SW_HIDE );
			m_lstboProType.EnableWindow( FALSE);

			m_staProc.ShowWindow( SW_HIDE );
			if (m_iReportType == ENUM_DISTANCE_REP || m_iReportType == ENUM_DURATION_REP)
			{
				m_PasToolBar.GetToolBarCtrl().ShowWindow(SW_HIDE);
			}
			else
			{
				m_ProToolBar.GetToolBarCtrl().ShowWindow( SW_HIDE );
			}

			if( m_iReportType == ENUM_ACOPERATION_REP || m_iReportType == ENUM_BAGDISTRIBUTION_REP )
				m_staPax.SetWindowText( "Flight Type" );
			else 
				m_staPax.SetWindowText( "Passenger Type" );

			m_lstboPaxType.ShowWindow( SW_SHOW );

			m_lstboPaxType.EnableWindow( TRUE);
			m_PasToolBar.GetToolBarCtrl().ShowWindow( SW_SHOW );
			break;
		}
	case ENUM_UTILIZATION_REP:
	case ENUM_THROUGHPUT_REP:
	case ENUM_QUEUELENGTH_REP:
	case ENUM_AVGQUEUELENGTH_REP:
	case ENUM_BAGCOUNT_REP:
	case ENUM_CONVEYOR_WAIT_LENGTH_REP:
		// show proc list only
		m_lstboProType.EnableWindow( FALSE);
		m_lstboProType.ShowWindow( SW_SHOW );
		if (m_iReportType != ENUM_THROUGHPUT_REP)
		{
			m_ProToolBar.GetToolBarCtrl().ShowWindow( SW_SHOW );
			m_PasToolBar.GetToolBarCtrl().ShowWindow( SW_HIDE );
			m_lstboProType.EnableWindow( TRUE);
		}
		else
		{
			m_PasToolBar.GetToolBarCtrl().ShowWindow( SW_SHOW );
			m_ProToolBar.GetToolBarCtrl().ShowWindow( SW_HIDE );
		}
		m_staProc.ShowWindow( SW_SHOW );
		
		m_lstboPaxType.ShowWindow( SW_HIDE );
		m_lstboPaxType.EnableWindow( FALSE);
		break;

	//case ENUM_ACOPERATION_REP:
	case ENUM_BAGDISTRIBUTION_REP:
	case ENUM_BAGDELIVTIME_REP:	
		m_lstboPaxType.EnableWindow( FALSE );
		DisableTBAllBtn(&m_PasToolBar);
		m_ProToolBar.EnableWindow(FALSE);
		m_staPax.EnableWindow( FALSE );

		m_lstboProType.ShowWindow( SW_HIDE );
		m_lstboProType.EnableWindow( FALSE);
		m_staProc.ShowWindow( SW_HIDE );
		m_ProToolBar.GetToolBarCtrl().ShowWindow( SW_HIDE );
		break;

	default:
		// show both
		m_lstboProType.ShowWindow( SW_SHOW );
		m_lstboProType.EnableWindow( TRUE);
		m_staProc.ShowWindow( SW_SHOW );
		m_ProToolBar.GetToolBarCtrl().ShowWindow( SW_SHOW );

		m_lstboPaxType.ShowWindow( SW_SHOW );
		m_lstboPaxType.EnableWindow( TRUE);
		m_PasToolBar.GetToolBarCtrl().ShowWindow( SW_SHOW );
		break;
	}


	//detail / summary
	BOOL bEnable = TRUE;
	if( m_iReportType == ENUM_PAXLOG_REP || 
		m_iReportType == ENUM_PAXDENS_REP ||
		m_iReportType == ENUM_SPACETHROUGHPUT_REP ||
		m_iReportType == ENUM_ACOPERATION_REP ||
		m_iReportType == ENUM_BAGDISTRIBUTION_REP ||
		m_iReportType == ENUM_COLLISIONS_REP )
	{
		bEnable = FALSE;
	}
	m_staticType.EnableWindow( bEnable );
	m_radioDetailed.EnableWindow( bEnable );
	m_radioSummary.EnableWindow( bEnable );

	//For Threshold int/float/time type
	switch( m_iReportType )
	{
	case ENUM_DISTANCE_REP:
	case ENUM_AVGQUEUELENGTH_REP:
	case ENUM_QUEUELENGTH_REP:
	case ENUM_BAGCOUNT_REP:
	case ENUM_THROUGHPUT_REP:
	case ENUM_PAXCOUNT_REP:
	case ENUM_FIREEVACUTION_REP:
	case ENUM_CONVEYOR_WAIT_LENGTH_REP:
		m_staticThreshold.EnableWindow( TRUE );
		m_edtThreshold.EnableWindow( TRUE );
		m_spinThreshold.EnableWindow( TRUE );
		m_chkThreshold.EnableWindow( TRUE );
		m_dtctrlThreshold.EnableWindow( FALSE );
		m_dtctrlThreshold.ShowWindow( SW_HIDE );
		m_edtThreshold.ShowWindow(SW_SHOW);
		m_spinThreshold.ShowWindow( SW_SHOW );
		break;
	case ENUM_BAGWAITTIME_REP:
	case ENUM_DURATION_REP:
	case ENUM_QUEUETIME_REP:
	case ENUM_SERVICETIME_REP:
	case ENUM_CONVEYOR_WAIT_TIME_REP:
		m_staticThreshold.EnableWindow( TRUE );
		m_edtThreshold.EnableWindow( FALSE );
		m_spinThreshold.EnableWindow( FALSE );
		m_chkThreshold.EnableWindow( TRUE );
		m_dtctrlThreshold.EnableWindow( TRUE );
		m_dtctrlThreshold.ShowWindow( SW_SHOW );
		m_edtThreshold.ShowWindow(SW_HIDE);
		m_spinThreshold.ShowWindow( SW_HIDE );
		break;
	case ENUM_COLLISIONS_REP:
		m_edtThreshold.EnableWindow(TRUE);
		m_edtThreshold.SetWindowText("1");
		m_staticThreshold.EnableWindow(TRUE);
		m_spinThreshold.EnableWindow(TRUE);
		m_dtctrlThreshold.EnableWindow( FALSE );
		m_dtctrlThreshold.ShowWindow( SW_HIDE );
		m_spinThreshold.ShowWindow(SW_SHOW);
		m_chkThreshold.SetCheck(TRUE);
		m_chkThreshold.EnableWindow(FALSE);
		break;
	default:
		m_staticThreshold.EnableWindow( FALSE );
		m_edtThreshold.EnableWindow( FALSE );
		m_spinThreshold.EnableWindow( FALSE );
		m_chkThreshold.EnableWindow( FALSE );
		m_dtctrlThreshold.EnableWindow( FALSE );
		m_dtctrlThreshold.ShowWindow( SW_HIDE );
		m_edtThreshold.ShowWindow(SW_SHOW);
		m_spinThreshold.ShowWindow( SW_SHOW );
	}

	// interval 
	bEnable = FALSE;
	if( m_iReportType == ENUM_QUEUELENGTH_REP ||
		m_iReportType == ENUM_PAXDENS_REP ||
		m_iReportType == ENUM_BAGDISTRIBUTION_REP||
		m_iReportType == ENUM_BAGCOUNT_REP ||
		m_iReportType == ENUM_PAXCOUNT_REP||
		m_iReportType == ENUM_SPACETHROUGHPUT_REP ||
		m_iReportType == ENUM_COLLISIONS_REP ||
		m_iReportType == ENUM_ACOPERATION_REP ||
		m_iReportType == ENUM_CONVEYOR_WAIT_LENGTH_REP)
		bEnable = TRUE;
	m_staticInterval.EnableWindow( bEnable );
	m_dtctrlInterval.EnableWindow(bEnable);
//	m_editInterval.EnableWindow( bEnable );
//	m_spinInterval.EnableWindow( bEnable );

	// area

	if(m_iReportType == ENUM_COLLISIONS_REP)
	{
		m_staticThreshold.SetWindowText("Threashold(meter)");

		m_staticArea.ShowWindow( SW_SHOW );
		m_comboAreaPortal.ShowWindow( SW_SHOW );
		m_comboAreaPortal.EnableWindow(TRUE);
		m_staticArea.SetWindowText( "Areas:" );

		CAreas* pAreas = m_pDoc->GetTerminal().m_pAreas;
		int nCount = pAreas->m_vAreas.size();
		m_comboAreaPortal.ResetContent();
		int nIdx = m_comboAreaPortal.AddString("Without Area");
		m_comboAreaPortal.SetItemData(nIdx ,-2);
		m_comboAreaPortal.SetCurSel(nIdx);
		for( int i=0; i<nCount; i++ )
		{		
			CString csStr = pAreas->m_vAreas[i]->name;
			
			 nIdx = m_comboAreaPortal.AddString( csStr );
			m_comboAreaPortal.SetItemData( nIdx, i );
			
		}
	} else 	if( m_iReportType == ENUM_PAXDENS_REP )
	{
		m_staticArea.ShowWindow( SW_SHOW );
		m_comboAreaPortal.ShowWindow( SW_SHOW );
		m_comboAreaPortal.EnableWindow(TRUE);
		m_staticArea.SetWindowText( "Areas:" );

		CAreas* pAreas = m_pDoc->GetTerminal().m_pAreas;
		int nCount = pAreas->m_vAreas.size();
		m_comboAreaPortal.ResetContent();
		for( int i=0; i<nCount; i++ )
		{		
			CString csStr = pAreas->m_vAreas[i]->name;
			int nIdx = m_comboAreaPortal.AddString( csStr );
			m_comboAreaPortal.SetItemData( nIdx, i );
			
		}

	}
	else if( m_iReportType == ENUM_SPACETHROUGHPUT_REP )
	{
		m_staticArea.SetWindowText( "Portals:" );
		m_staticArea.ShowWindow( SW_SHOW );
		m_comboAreaPortal.ShowWindow( SW_SHOW );
		m_comboAreaPortal.EnableWindow(TRUE);
		CPortals* pPortals = m_pDoc->GetTerminal().m_pPortals;
		int nCount = pPortals->m_vPortals.size();
		m_comboAreaPortal.ResetContent();
		for( int i=0; i<nCount; i++ )
		{		
			CString csStr = pPortals->m_vPortals[i]->name;
			
			int nIdx = m_comboAreaPortal.AddString( csStr );
			m_comboAreaPortal.SetItemData( nIdx, i );
		}
	}
	else
	{
		m_staticArea.ShowWindow( SW_HIDE );
		m_comboAreaPortal.ShowWindow( SW_HIDE );
		m_comboAreaPortal.EnableWindow(FALSE);
	}

	
	UpdateData(TRUE);
	if( m_iReportType == ENUM_BAGDELIVTIME_REP ||
		m_iReportType == ENUM_BAGDISTRIBUTION_REP ||
		m_iReportType == ENUM_PAXDENS_REP ||
		m_iReportType == ENUM_SPACETHROUGHPUT_REP ||
		m_iReportType == ENUM_COLLISIONS_REP)
			m_nRepType = 0;
	else 
			m_nRepType = 1;
	UpdateData( false );
	//Move Proc type list
	BOOL bVisible=m_lstboPaxType.IsWindowEnabled();
	if(bVisible)
	{
		CRect rect;
		m_lstboPaxType.GetWindowRect(rect);
		ScreenToClient(rect);
		m_staProc.SetWindowPos(NULL,rect.left,rect.bottom+6,0,0,SWP_NOSIZE);
		m_staProc.GetWindowRect(rect);
		ScreenToClient(rect);
		m_ProToolBar.SetWindowPos(NULL,rect.left,rect.bottom+6,0,0,SWP_NOSIZE);
		m_ProToolBar.GetWindowRect(rect);
		ScreenToClient(rect);
		m_lstboProType.SetWindowPos(NULL,rect.left,rect.bottom,0,0,SWP_NOSIZE);
		
	}
	else
	{
		CRect rect;
		m_staPax.GetWindowRect(rect);
		ScreenToClient(rect);
		m_staProc.MoveWindow(rect);

		m_PasToolBar.GetWindowRect(rect);
		ScreenToClient(rect);
		m_ProToolBar.MoveWindow(rect);

		m_lstboPaxType.GetWindowRect(rect);
		ScreenToClient(rect);
		m_lstboProType.MoveWindow(rect);
		
	}

	//Move ComboBox
	if(bVisible=m_lstboProType.IsWindowEnabled())
	{
		CRect rect;
		m_lstboProType.GetWindowRect(rect);
		ScreenToClient(rect);
		m_staticArea.SetWindowPos(NULL,rect.left,rect.bottom+6,0,0,SWP_NOSIZE);
		m_staticArea.GetWindowRect(rect);
		ScreenToClient(rect);
		m_comboAreaPortal.SetWindowPos(NULL,rect.left,rect.bottom+6,0,0,SWP_NOSIZE);
	}	
	else
	{
		CRect rect;
		
		m_lstboPaxType.GetWindowRect(rect);
		ScreenToClient(rect);
		m_staticArea.SetWindowPos(NULL,rect.left,rect.bottom+6,0,0,SWP_NOSIZE);
		m_staticArea.GetWindowRect(rect);
		ScreenToClient(rect);
		m_comboAreaPortal.SetWindowPos(NULL,rect.left,rect.bottom+6,0,0,SWP_NOSIZE);
	}
	//Move Main Wnd
	bVisible=m_comboAreaPortal.IsWindowEnabled();
	CRect rectWnd;
	GetWindowRect(rectWnd);
	if(bVisible)
	{
		CRect rect;
		m_comboAreaPortal.GetWindowRect(rect);
		ScreenToClient(rect);
		SetWindowPos(NULL,0,0,rectWnd.Width(),rect.bottom+46,SWP_NOMOVE);
	}
	else if(bVisible=m_lstboProType.IsWindowEnabled())
	{
		CRect rect;
		m_lstboProType.GetWindowRect(rect);
		ScreenToClient(rect);
		SetWindowPos(NULL,0,0,rectWnd.Width(),rect.bottom+46,SWP_NOMOVE);
	}
	else //if(bVisible=m_lstboPaxType.IsWindowEnabled())
	{
		CRect rect;
		m_lstboPaxType.GetWindowRect(rect);
		ScreenToClient(rect);
		SetWindowPos(NULL,0,0,rectWnd.Width(),rect.bottom+46,SWP_NOMOVE);
	}

	if (m_iReportType == ENUM_THROUGHPUT_REP||
		m_iReportType == ENUM_DISTANCE_REP ||
		m_iReportType == ENUM_DURATION_REP)
	{
		CRect rect;
		m_lstboPaxType.GetWindowRect(&rect);
		ScreenToClient(&rect);
		m_lstboPaxType.ShowWindow(SW_HIDE);
			
		m_treePaxType.MoveWindow(rect, true);
		m_treePaxType.ShowWindow(SW_SHOW);
		UpdateWindow();
	}
}

void CReportParamDlg::Clear()
{
	switch( m_iReportType )
	{
	case ENUM_COLLISIONS_REP:
		m_lstboPaxType.ResetContent();
		break;
	case ENUM_PAXLOG_REP:
	case ENUM_DISTANCE_REP:
	case ENUM_DURATION_REP:
	case ENUM_ACTIVEBKDOWN_REP:
	case ENUM_PAXCOUNT_REP:
	case ENUM_PAXDENS_REP:
	case ENUM_SPACETHROUGHPUT_REP:
	case ENUM_BAGDELIVTIME_REP:
	case ENUM_FIREEVACUTION_REP:
		m_paxConst.initDefault( m_pTerm );
		LoadPaxList();
		break;
	case ENUM_ACOPERATION_REP:
	case ENUM_BAGDISTRIBUTION_REP:
		m_vFltList.clear();
		LoadFltList();
		break;
		
	case ENUM_UTILIZATION_REP:
	case ENUM_THROUGHPUT_REP:
	case ENUM_QUEUELENGTH_REP:
	case ENUM_AVGQUEUELENGTH_REP:
	case ENUM_BAGCOUNT_REP:
	case ENUM_CONVEYOR_WAIT_LENGTH_REP:
		m_lstboProType.ResetContent();
		break;
		
	default:
		m_paxConst.initDefault( m_pTerm );
		LoadPaxList();
		m_lstboProType.ResetContent();
		break;
    }
}

void CReportParamDlg::LoadFltList()
{
	m_lstboPaxType.ResetContent();
	int nCount = m_vFltList.size();
	for( int i=0; i<nCount; i++ )
	{
		CString str;
		//char str[128];
		m_vFltList[i].screenPrint( str,0, 128 );
		m_lstboPaxType.AddString( str.GetBuffer(0) );
	}
	m_lstboPaxType.SetCurSel( -1 );
	
	OnSelchangeListPaxtype();
}

//////////////////////////////////////////////////////////////////////////
BOOL CReportParamDlg::PreCreateWindow(CREATESTRUCT& cs) 
{
	// TODO: Add your specialized code here and/or call the base class
	//cs.style &= ~WS_CHILD;
	//cs.style &= ~WS_DLGFRAME;
	//cs.style |= WS_POPUP|WS_SIZEBOX;
	
	return CDialog::PreCreateWindow(cs);
}

void CReportParamDlg::OnPeoplemoverNew() 
{
	// TODO: Add your command handler code here
	switch( m_iReportType)
	{
	case ENUM_PAXLOG_REP:
	case ENUM_DISTANCE_REP:
	case ENUM_DURATION_REP:
	case ENUM_ACTIVEBKDOWN_REP:
	case ENUM_PAXCOUNT_REP:
	case ENUM_PAXDENS_REP:
	case ENUM_SPACETHROUGHPUT_REP:
	case ENUM_ACOPERATION_REP:
	case ENUM_BAGDISTRIBUTION_REP:
	case ENUM_BAGDELIVTIME_REP:
	case ENUM_COLLISIONS_REP:
	case ENUM_FIREEVACUTION_REP:
		return;
	default:
		{
			CProcesserDialog dlg( m_pTerm );
			if( dlg.DoModal() == IDOK )
			{
				ProcessorID id;
				ProcessorIDList idList;
				if( dlg.GetProcessorIDList(idList) )
				{
					int nIDcount = idList.getCount();
					for(int i = 0; i < nIDcount; i++)
					{
						id = *idList[i];

						// check if exist in the current list.
						int nProcCount = m_vProcList.size();
						bool bFind = false;
						for( int j=0; j<nProcCount; j++ )
						{
							if( m_vProcList[j] == id )
							{
								bFind = true;
								break;
							}
						}
						if( bFind )
							continue;
						
						
						if( nProcCount == 0 )
							m_lstboProType.ResetContent();
						
						// add into the data list
						m_vProcList.push_back( id );
						
						
						// add into gui list
						char szBuf[256];
						id.printID(szBuf);						
						int nIdx = m_lstboProType.AddString( szBuf );
						m_lstboProType.SetItemData( nIdx, m_vProcList.size()-1 );
						m_lstboProType.SetCurSel( nIdx );
					}
				}
				
				int nCount = m_lstboProType.GetCount();
				if(LB_ERR ==nCount)
				{
					return;
				}
				CString strSelect;
				if( nCount <= 0 );
					//m_ReportSpecs.m_strProcessor="ALL PROCESSORS";
				else
				{
					m_lstboProType.GetText(0,strSelect);
					//m_ReportSpecs.m_strProcessor=strSelect;
					for (int i = 1; i < nCount; i++)
					{
						m_lstboProType.GetText(i,strSelect);
						//m_ReportSpecs.m_strProcessor=m_ReportSpecs.m_strProcessor+";"+strSelect;
					}
				}
			}	
			break;
		}
    }
}

void CReportParamDlg::OnPeoplemoverDelete() 
{
	int nCurSel = m_lstboProType.GetCurSel();
	if( nCurSel == LB_ERR )
	{
		return;
	}
	
	CString strSelect;
	int nCount=0;
	switch( m_iReportType )
	{
	case ENUM_PAXLOG_REP:
	case ENUM_DISTANCE_REP:
	case ENUM_DURATION_REP:
	case ENUM_ACTIVEBKDOWN_REP:
	case ENUM_PAXCOUNT_REP:
	case ENUM_PAXDENS_REP:
	case ENUM_SPACETHROUGHPUT_REP:
	case ENUM_ACOPERATION_REP:
	case ENUM_BAGDISTRIBUTION_REP:
	case ENUM_BAGDELIVTIME_REP:
	case ENUM_COLLISIONS_REP:
	case ENUM_FIREEVACUTION_REP:
		return;
	default:

		// Handle deleting all processors
		int nIdx = m_lstboProType.GetItemData( nCurSel );
		if( nIdx == -1 )
		{
			return;
		}
		
		// Delete from data
		m_vProcList.erase( m_vProcList.begin() + nIdx );
		
		
		// delete from gui
		
		
		m_lstboProType.DeleteString( nCurSel );
		m_lstboProType.SetCurSel( -1 );
		
		if( m_vProcList.size() == 0 )
		{
			int nIdx = m_lstboProType.AddString( "All Processors" );
			m_lstboProType.SetItemData( nIdx, -1 );
		}
		break;		
    }
}

void CReportParamDlg::OnToolbarbuttonadd() 
{
	// TODO: Add your command handler code here
	switch( m_iReportType)
	{
	case ENUM_DISTANCE_REP:
	case ENUM_DURATION_REP:
		{
			HTREEITEM hItem=m_treePaxType.GetSelectedItem();
			if(hItem==m_hItemFrom||hItem==m_hItemTo)	// Level 2
			{
				CProcesserDialog dlg( &m_pDoc->GetTerminal() );
				if( dlg.DoModal() == IDOK )
				{
					ProcessorIDList idList;
					ProcessorID id;
					if( dlg.GetProcessorIDList(idList) )
					{
						int nIDcount = idList.getCount();
						for(int i = 0; i < nIDcount; i++)
						{							
							id = *idList[i];
							char szBuf[256];
							id.printID(szBuf);

							CReportParameter::FROM_TO_PROCS _fromToProcs;
							m_pReportParam->GetFromToProcs(_fromToProcs);
							if(hItem==m_hItemFrom)
							{
								_fromToProcs.m_vFromProcs.push_back(id);
							}
							else if(hItem==m_hItemTo)
							{
								_fromToProcs.m_vToProcs.push_back(id);
							}
							m_pReportParam->SetFromToProcs(_fromToProcs);
							m_treePaxType.InsertItem(szBuf,hItem);
							m_treePaxType.Expand(hItem,TVE_EXPAND);
						}
					}

				}	
			}
			else if(hItem==m_hItemPaxType)	// Level 1
			{
				CPassengerTypeDialog dlg( this );
				if( dlg.DoModal() == IDOK )
				{
					CMobileElemConstraint mobileConstr = dlg.GetMobileSelection();
					int nCount = m_paxConst.getCount();
					for( int i=0; i<nCount; i++ )
					{
						const CMobileElemConstraint * pPaxConstr = m_paxConst.getConstraint( i );
						if( mobileConstr.isEqual(pPaxConstr) )
						{
							CString szPaxType;
							mobileConstr.screenPrint( szPaxType );
							CString csMsg;
							csMsg.Format( "Passenger Type : %s exists in the list", szPaxType.GetBuffer(0) );
							MessageBox( csMsg, "Error", MB_OK|MB_ICONWARNING );
							return;
						}
					}
					m_paxConst.addConstraint( mobileConstr );

					CString szPaxType;
					mobileConstr.screenPrint( szPaxType );
					m_treePaxType.InsertItem(szPaxType.GetBuffer(0),m_hItemPaxType);
				}
			}
		}
		break;
	case ENUM_THROUGHPUT_REP:
		{
			HTREEITEM item =m_treePaxType.GetSelectedItem() ;
			//char szPaxType[128];
			CString szPaxType;
			if(item == NULL)
				return ;
			int item_ty = m_treePaxType.GetItemData(item) ;
			if(item_ty == PAX_TYPE)
			{

				CPassengerTypeDialog dlg( this );
				if( dlg.DoModal() == IDOK )
				{
					CMobileElemConstraint mobileConstr = dlg.GetMobileSelection();
					int nCount = m_paxConst.getCount();
					for( int i=0; i<nCount; i++ )
					{
						const CMobileElemConstraint * pPaxConstr = m_paxConst.getConstraint( i );
						if( mobileConstr.isEqual(pPaxConstr) )
						{
							mobileConstr.screenPrint( szPaxType );
							CString csMsg;
							csMsg.Format( "Pax Type : %s exists in the list", szPaxType.GetBuffer(0) );
							MessageBox( csMsg, "Error", MB_OK|MB_ICONWARNING );
							return;
						}
					}
					m_paxConst.addConstraint( mobileConstr );
					CString szPaxType;
					mobileConstr.screenPrint( szPaxType );
					item = m_treePaxType.InsertItem(szPaxType,m_throughput_paxroot);
					m_treePaxType.SetItemData(item , m_paxConst.getCount() -1) ;
				}

			}
			m_treePaxType.Expand(m_throughput_paxroot,TVE_EXPAND) ;
			if(item_ty == THROUGH_PUT_PROCESSOR)
			{
				CProcesserDialog dlg(m_pTerm);
				if( dlg.DoModal() == IDOK )
				{
					ProcessorID id;
					ProcessorIDList idList;
					if( dlg.GetProcessorIDList(idList) )
					{
						int nIDcount = idList.getCount();
						for(int i = 0; i < nIDcount; i++)
						{
							id = *idList[i];
							// check if exist in the current list.
							int nProcCount = m_vProcList.size();
							bool bFind = false;
							for( int j=0; j<nProcCount; j++ )
							{
								if( m_vProcList[j] == id )
								{
									bFind = true;
									break;
								}
							}
							if( bFind )
								continue;
							if(m_vProcList.size() == 0 )
							{
								HTREEITEM item = m_treePaxType.GetChildItem(m_throughput_proroot) ;
								if(item != NULL)
									m_treePaxType.DeleteItem(item) ;
							}
							m_vProcList.push_back( id );
							id.SetStrDict(m_pTerm->inStrDict );
							TCHAR szBuf[256]= {0};
							id.printID(szBuf);
							ProcessorID* proid = new ProcessorID(id) ;
							m_TreeDataProID.push_back(proid) ;
							item = m_treePaxType.InsertItem(szBuf,m_throughput_proroot) ;
							m_treePaxType.SetItemData(item ,(DWORD_PTR)(proid) );

						}
					}
				 }
					m_treePaxType.Expand(m_throughput_proroot,TVE_EXPAND) ;
			}
		}
		break;
	case ENUM_UTILIZATION_REP:
	case ENUM_QUEUELENGTH_REP:
	case ENUM_AVGQUEUELENGTH_REP:
	case ENUM_BAGCOUNT_REP:
	case ENUM_CONVEYOR_WAIT_LENGTH_REP:
		return;
		
	case ENUM_ACOPERATION_REP:
	case ENUM_BAGDISTRIBUTION_REP:
		//	case ENUM_COLLISIONS_REP:
		{
			CFlightDialog dlg( m_pParentWnd );
			if( dlg.DoModal() == IDOK )
			{
				FlightConstraint newFltConstr = dlg.GetFlightSelection();
				int nCount = m_vFltList.size();
				for( int i=0; i<nCount; i++ )
				{
					if( m_vFltList[i].isEqual( &newFltConstr ) )
					{
						//char szPaxType[128];
						CString szPaxType;
						//newFltConstr.screenPrint( szPaxType, 128 );
						newFltConstr.screenPrint( szPaxType, 0, 128 );
						CString csMsg;
						csMsg.Format( "Flight Type : %s exists in the list", szPaxType.GetBuffer(0) );
						MessageBox( csMsg, "Error", MB_OK|MB_ICONWARNING );
						return;
					}
				}
				
				m_vFltList.push_back( newFltConstr );
				LoadFltList();
			}
			break;
		}
	default:
		{
			CPassengerTypeDialog dlg( m_pParentWnd );
			if( dlg.DoModal() == IDOK )
			{
				//PassengerConstraint paxSelection;
				//paxSelection = dlg.GetPaxSelection();
				CMobileElemConstraint mobileConstr = dlg.GetMobileSelection();
				int nCount = m_paxConst.getCount();
				for( int i=0; i<nCount; i++ )
				{
					const CMobileElemConstraint * pPaxConstr = m_paxConst.getConstraint( i );
					if( mobileConstr.isEqual(pPaxConstr) )
					{
						//char szPaxType[128];
						CString szPaxType;
						mobileConstr.screenPrint( szPaxType );
						CString csMsg;
						csMsg.Format( "Passenger Type : %s exists in the list", szPaxType.GetBuffer(0) );
						MessageBox( csMsg, "Error", MB_OK|MB_ICONWARNING );
						return;
					}
				}
				
				m_paxConst.addConstraint( mobileConstr );
				LoadPaxList();
			}
			break;
		}
    }
}

void CReportParamDlg::OnToolbarbuttondel() 
{
	// TODO: Add your command handler code here
	switch( m_iReportType )
	{
	case ENUM_THROUGHPUT_REP:
		{
			HTREEITEM item = m_treePaxType.GetSelectedItem();
			if(item == NULL)
				break ;
			HTREEITEM paresent = m_treePaxType.GetParentItem(item);	
			if(paresent != NULL)
			{
				int item_TY = 0 ;
				int proID = 0 ;
				int index = 0 ;
				item_TY = m_treePaxType.GetItemData(paresent) ;		
				if(item_TY == PAX_TYPE)
				{
					index = m_treePaxType.GetItemData(item) ;
					m_paxConst.deleteConst(index) ;
					m_treePaxType.DeleteItem(item) ;
					m_treePaxType.Expand(m_throughput_paxroot,TVE_EXPAND) ;
				}
				if(item_TY == THROUGH_PUT_PROCESSOR)
				{
					ProcessorID* proID = (ProcessorID*)m_treePaxType.GetItemData(item) ;
					std::vector<ProcessorID>::iterator iter =  std::find(m_vProcList.begin(),m_vProcList.end(),*proID) ;
					if(iter != m_vProcList.end())
					{
						m_vProcList.erase( iter);
						m_treePaxType.DeleteItem(item) ;
						std::vector<ProcessorID*>::iterator p_iter = std::find(m_TreeDataProID.begin(),m_TreeDataProID.end(),proID) ;
						if(p_iter != m_TreeDataProID.end())
						{
							m_TreeDataProID.erase(p_iter) ;
							delete proID ;
						}
					}
					m_treePaxType.Expand(m_throughput_paxroot,TVE_EXPAND) ;
				}
			}
			break ;
		}
	case ENUM_DISTANCE_REP:
	case ENUM_DURATION_REP:
		{
			HTREEITEM hItem=m_treePaxType.GetSelectedItem();
			HTREEITEM hItemParent=m_treePaxType.GetParentItem(hItem);
			if(hItemParent==m_hItemPaxType)
			{
				m_paxConst.deleteConst( m_treePaxType.GetItemData(hItem) );
				m_treePaxType.DeleteItem(hItem);
			}
			else if(hItemParent==m_hItemFrom)
			{
				CReportParameter::FROM_TO_PROCS _fromToProcs;
				m_pReportParam->GetFromToProcs(_fromToProcs);

				int nIndex=m_treePaxType.GetItemData(hItem);
				_fromToProcs.m_vFromProcs.erase(_fromToProcs.m_vFromProcs.begin()+nIndex);
				m_treePaxType.DeleteItem(hItem);

				m_pReportParam->SetFromToProcs(_fromToProcs);

			}
			else if(hItemParent==m_hItemTo)
			{
				CReportParameter::FROM_TO_PROCS _fromToProcs;
				m_pReportParam->GetFromToProcs(_fromToProcs);

				int nIndex=m_treePaxType.GetItemData(hItem);
				_fromToProcs.m_vToProcs.erase(_fromToProcs.m_vToProcs.begin()+nIndex);
				m_treePaxType.DeleteItem(hItem);

				m_pReportParam->SetFromToProcs(_fromToProcs);
			}
			return;
		}
		break;
	}
	int nCurSel = m_lstboPaxType.GetCurSel();
	if( nCurSel == LB_ERR )
	{
		return;
	}
	
	switch( m_iReportType )
	{
	case ENUM_UTILIZATION_REP:
	case ENUM_THROUGHPUT_REP:
	case ENUM_QUEUELENGTH_REP:
	case ENUM_AVGQUEUELENGTH_REP:
	case ENUM_BAGCOUNT_REP:
	case ENUM_CONVEYOR_WAIT_LENGTH_REP:
		break;
		
	case ENUM_ACOPERATION_REP:
	case ENUM_BAGDISTRIBUTION_REP:
		m_vFltList.erase( m_vFltList.begin() + nCurSel );
		LoadFltList();
		break;
		
	default:
		m_paxConst.deleteConst( nCurSel );
		LoadPaxList();		
    }
	
}

void CReportParamDlg::getReportString(CString &strReportType)
{
	switch( m_iReportType ) 
	{
	case ENUM_PAXLOG_REP:
		strReportType.LoadString( IDS_TVNN_REP_PAX_LOG );
		break;
		
	case ENUM_DISTANCE_REP:
		strReportType.LoadString( IDS_TVNN_REP_PAX_DISTANCE );
		break;
		
	case ENUM_QUEUETIME_REP:
		strReportType.LoadString( IDS_TVNN_REP_PAX_TIMEINQ );
		break;
		
	case ENUM_DURATION_REP:
		strReportType.LoadString( IDS_TVNN_REP_PAX_TIMEINT );
		break;
		
	case ENUM_SERVICETIME_REP:
		strReportType.LoadString( IDS_TVNN_REP_PAX_TIMEINS );
		break;
		
	case ENUM_ACTIVEBKDOWN_REP:
		strReportType.LoadString( IDS_TVNN_REP_PAX_BREAKDOWN );
		break;
		
	case ENUM_UTILIZATION_REP:
		strReportType.LoadString( IDS_TVNN_REP_PROC_UTILISATION );
		break;
		
	case ENUM_THROUGHPUT_REP:
		strReportType.LoadString( IDS_TVNN_REP_PROC_THROUGHPUT );
		break;
		
	case ENUM_QUEUELENGTH_REP:
		strReportType.LoadString( IDS_TVNN_REP_PROC_QLENGTH );
		break;
		
	case ENUM_AVGQUEUELENGTH_REP:
		strReportType.LoadString( IDS_TVNN_REP_PROC_QUEUEPARAM );
		break;
		
	case ENUM_PAXCOUNT_REP:
		strReportType.LoadString( IDS_TVNN_REP_SPACE_OCCUPANCY );
		break;
		
	case ENUM_PAXDENS_REP:
		strReportType.LoadString( IDS_TVNN_REP_SPACE_DENSITY );
		break;
		
	case ENUM_SPACETHROUGHPUT_REP:
		strReportType.LoadString( IDS_TVNN_REP_SPACE_THROUGHPUT );
		break;
		
	case ENUM_COLLISIONS_REP:
		strReportType.LoadString( IDS_TVNN_REP_SPACE_COLLISIONS );
		break;
		
	case ENUM_ACOPERATION_REP:
		strReportType.LoadString( IDS_TVNN_REP_AIRCRAFT );
		break;
		
	case ENUM_BAGCOUNT_REP:
		strReportType.LoadString( IDS_TVNN_REP_BAG_COUNT );
		break;
		
	case ENUM_BAGWAITTIME_REP:
		strReportType.LoadString( IDS_TVNN_REP_BAG_WAITTIME );
		break;
		
	case ENUM_BAGDISTRIBUTION_REP:
		strReportType.LoadString( IDS_TVNN_REP_BAG_DISTRIBUTION );
		break;
		
	case ENUM_BAGDELIVTIME_REP:
		strReportType.LoadString( IDS_TVNN_REP_BAG_DELIVERYTIME );
		break;
		
	case ENUM_ECONOMIC_REP:
		strReportType.LoadString( IDS_TVNN_REP_ECONOMIC );
		break;
	
	case ENUM_FIREEVACUTION_REP:
		strReportType.LoadString( IDS_TVNN_REP_FIREEVACUTION );
		break;
	case ENUM_CONVEYOR_WAIT_LENGTH_REP:
		strReportType.LoadString( IDS_TVNN_REP_CONVEYOR_WAIT_LENGTH );
		break;
	case ENUM_CONVEYOR_WAIT_TIME_REP:
		strReportType.LoadString( IDS_TVNN_REP_CONVEYOR_WAIT_TIME );
		break;

	default:
		strReportType = "ERROR!";
		break;
	}	
	return;
}


void CReportParamDlg::SetGUIFromPara()
{
	ElapsedTime time;
	// time
	//
	bool bAbsDate;
	COleDateTime ddate;
	int nIdx;
	COleDateTime tptime;
	if( m_pReportParam->GetStartTime( time ) )
	{
		m_startDate.GetDateTime(time,bAbsDate,ddate,nIdx,tptime);
		if(bAbsDate)
		{
			m_oleDTStartDate = ddate;
		}
		else
		{
			m_comboStartDay.SetCurSel(nIdx);
		}
		m_oleDTStartTime = tptime;
	}	
	
	if( m_pReportParam->GetEndTime( time ) )
	{
		m_startDate.GetDateTime(time,bAbsDate,ddate,nIdx,tptime);
		if(bAbsDate)
		{
			m_oleDTEndDate = ddate;
		}
		else
		{
			m_comboEndDay.SetCurSel(nIdx);
		}
		m_oleDTEndTime = tptime;
	}
//	if( m_pReportParam->GetStartTime( time ) )
//		m_oleDTStartTime.SetTime( time.asHours(), ((int)time.asMinutes()) % 60, ((int)time.asSeconds()) % 60);
//	if( m_pReportParam->GetEndTime( time ) )
//		m_oleDTEndTime.SetTime( time.asHours(), ((int)time.asMinutes()) % 60, ((int)time.asSeconds()) % 60 );

	long second;
	if( m_pReportParam->GetInterval( second ))
	{
		time = second;
		m_oleDTInterval.SetTime( time.asHours(), ((int)time.asMinutes()) % 60, ((int)time.asSeconds()) % 60 );
	}

	//report type
	int iReportType;
	if( m_pReportParam->GetReportType( iReportType ) )
		m_nRepType = iReportType;
	//throdhold
	long lThreshold;
	if( m_pReportParam->GetThreshold( lThreshold) )
	{
		if( lThreshold == LONG_MAX )
		{
			m_chkThreshold.SetCheck( 0 );
			time = 0l;
		}
		else
		{
			if( lThreshold == 0 )
			{
				m_chkThreshold.SetCheck( 0 );
			}
			else
			{
				m_chkThreshold.SetCheck( 1 );
			}			
			time = lThreshold;

			if( lThreshold >= 1000 )
				m_fThreshold = 0 ;
			else
				m_fThreshold = static_cast<float>(lThreshold);
		}
	
		m_oleThreshold.SetTime( time.asHours(), ((int)time.asMinutes()) % 60, ((int)time.asSeconds()) % 60 );	
	}

	// pax type
	std::vector<CMobileElemConstraint> mobList;
	if( m_pReportParam->GetPaxType( mobList ) )
	{
		switch( m_iReportType)
		{
			case ENUM_UTILIZATION_REP:
	//		case ENUM_THROUGHPUT_REP:
			case ENUM_QUEUELENGTH_REP:
			case ENUM_AVGQUEUELENGTH_REP:
			case ENUM_BAGCOUNT_REP:
			case ENUM_CONVEYOR_WAIT_LENGTH_REP:
				break;;
				
			case ENUM_ACOPERATION_REP:
			case ENUM_BAGDISTRIBUTION_REP:
			// flight constraint
			{
				m_vFltList.clear();
				FlightConstraint flight;
				for( int i= 0; i<static_cast<int>(mobList.size()); i++ )
				{
					flight = (FlightConstraint) mobList[i];
					m_vFltList.push_back( flight );
				}
				LoadFltList();
				break;
			}
			default:
			{
				m_paxConst.initDefault( m_pTerm );				
				m_paxConst.deleteConst(0);
				for( int i=0; i<static_cast<int>(mobList.size()); i++ )
				{
					m_paxConst.addConstraint( mobList[i] );
				}
				
				LoadPaxList();
				break;
			}
		}	
	}

	// proce type
	if( m_pReportParam->GetProcessorGroup( m_vProcList ) )
	{
		LoadProTree();
		LoadProcList();
	}

	// area
	std::vector<CString> m_vArea;
	if( m_pReportParam->GetAreas( m_vArea ) )
	{
		if( m_vArea.size() >= 1)
		{
			CString strArea = m_vArea[0];
			m_comboAreaPortal.SetCurSel( -1 );
			for( int i=0; i<m_comboAreaPortal.GetCount(); i++ )
			{
				CString str;
				m_comboAreaPortal.GetLBText(i,str );
				if( strArea ==  str)
				{
					m_comboAreaPortal.SetCurSel( i );
					break;
				}
			}
		}
	}

	//portal
	std::vector<CString> m_vPortals;
	if( m_pReportParam->GetPortals( m_vPortals ) )
	{
		if( m_vArea.size() >= 1)
		{
			CString stPortal = m_vArea[0];
			m_comboAreaPortal.SetCurSel( -1 );
			for( int i=0; i<m_comboAreaPortal.GetCount(); i++ )
			{
				CString str;
				m_comboAreaPortal.GetLBText(i,str);
				if( stPortal == str )
				{
					m_comboAreaPortal.SetCurSel( i );
					break;
				}
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////
	if( m_pReportParam->GetReportCategory() == ENUM_BAGDELIVTIME_REP )
	{
		m_PasToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONADD);
		int iReportType;
		if( m_pReportParam->GetReportType( iReportType ) && iReportType == 1 )
		{
			m_lstboPaxType.EnableWindow( TRUE );
			m_PasToolBar.ShowWindow( SW_SHOW );
			m_PasToolBar.EnableWindow( TRUE );
		}
		else
		{
			m_lstboPaxType.EnableWindow( FALSE );
			m_PasToolBar.ShowWindow( SW_HIDE );
			m_PasToolBar.EnableWindow( FALSE );		
		}
	}
	//////////////////////////////////////////////////////////////////////////
	
	UpdateData( FALSE );
}


void CReportParamDlg::ReadDataFromLib()
{
	CString strReportType;
	getReportString(strReportType);
	
	m_pReportParam = getReportParam( strReportType );
	
	assert( m_pReportParam );
	
	if( !m_bNewData )
	{
		SetGUIFromPara();
	}
}


CReportParameter* CReportParamDlg::getReportParam(CString strName)
{
	m_bNewData = false;
	strName.MakeUpper();

	CReportParameter* pReportParam = m_pTerm->m_pReportParaDB->GetParameterbyName(strName);
	if (pReportParam != NULL)
		return pReportParam;

	// not exists, new a ReportParameter
	m_bNewData = true;
	return m_pTerm->m_pReportParaDB->BuildReportParam(ENUM_REPORT_TYPE(m_iReportType));
}

void CReportParamDlg::LoadProcList()
{
	m_lstboProType.ResetContent();
	
	int nProcTypeCount = m_vProcList.size();
	
	if( nProcTypeCount == 0 )
	{
		int nIdx = m_lstboProType.AddString( "All Processors" );
		m_lstboProType.SetItemData( nIdx, -1 );
		return;
	}
	
	for( int i=0; i<nProcTypeCount; i++ )
	{
		ProcessorID id = m_vProcList[i];
		id.SetStrDict( m_pTerm->inStrDict );
		char szBuf[256];
		id.printID(szBuf);
		int nIdx = m_lstboProType.AddString( szBuf );
		m_lstboProType.SetItemData( nIdx, i );
	}
}

BOOL CReportParamDlg::GetParaFromGUI()
{
	assert( m_pReportParam );
	UpdateData( TRUE );

	// check value
	CString str;
	m_comboAreaPortal.GetWindowText(str);
	str.TrimLeft();
	str.TrimRight();
	
	if( (m_iReportType == ENUM_PAXDENS_REP || m_iReportType == ENUM_COLLISIONS_REP ) && str.GetLength() == 0 )
	{
		MessageBox( "No Area has been Selected.", "Error", MB_OK|MB_ICONWARNING );
		return FALSE;
	}
	
	if( m_iReportType == ENUM_SPACETHROUGHPUT_REP && str.GetLength() == 0 )
	{
		MessageBox( "No Portal has been Selected.", "Error", MB_OK|MB_ICONWARNING );
		return FALSE;
	}
	
	if(m_iReportType == ENUM_QUEUELENGTH_REP || m_iReportType == ENUM_BAGCOUNT_REP || m_iReportType == ENUM_PAXDENS_REP 
		|| m_iReportType == ENUM_PAXCOUNT_REP || m_iReportType == ENUM_SPACETHROUGHPUT_REP || m_iReportType == ENUM_BAGCOUNT_REP 
		|| m_iReportType == ENUM_COLLISIONS_REP || m_iReportType == ENUM_ACOPERATION_REP || m_iReportType == ENUM_BAGDISTRIBUTION_REP
		|| m_iReportType == ENUM_CONVEYOR_WAIT_LENGTH_REP)
	{
		if(GetTotalMinutes(m_oleDTInterval)>120 || GetTotalMinutes(m_oleDTInterval)<1)
		{
			MessageBox( "You should set interval between 00:01 and 02:00", "Error", MB_OK|MB_ICONWARNING );
			return FALSE;
		}
	}
	//////////////////////////////////////////////////////////////////////////


	ElapsedTime time;

	bool bAbsDate = false;
	int nDtIdx=0;
	if (m_startDate.IsAbsoluteDate())
	{
		bAbsDate = true;
		m_startDate.GetElapsedTime(bAbsDate,m_oleDTStartDate,nDtIdx,m_oleDTStartTime,time);	
		m_pReportParam->SetStartTime( time );	
		m_startDate.GetElapsedTime(bAbsDate,m_oleDTEndDate,nDtIdx,m_oleDTEndTime,time);	
		m_pReportParam->SetEndTime( time );	
	}
	else
	{
		bAbsDate = false;
		nDtIdx = m_comboStartDay.GetCurSel();
		m_startDate.GetElapsedTime(bAbsDate,m_oleDTStartDate,nDtIdx,m_oleDTStartTime,time);	
		m_pReportParam->SetStartTime( time );
		
		nDtIdx = m_comboEndDay.GetCurSel();
		m_startDate.GetElapsedTime(bAbsDate,m_oleDTEndDate,nDtIdx,m_oleDTEndTime,time);	
		m_pReportParam->SetEndTime( time );	
	}
//	time.set( m_oleDTStartTime.GetHour(), m_oleDTStartTime.GetMinute(), m_oleDTStartTime.GetSecond()  );
//	m_pReportParam->SetStartTime( time );	
//	time.set( m_oleDTEndTime.GetHour(), m_oleDTEndTime.GetMinute(), m_oleDTEndTime.GetSecond() );
//	m_pReportParam->SetEndTime( time );
	
	time.set( m_oleDTInterval.GetHour(), m_oleDTInterval.GetMinute(), m_oleDTInterval.GetSecond() );
	m_pReportParam->SetInterval( time.asSeconds() );
	
	m_pReportParam->SetReportType( m_nRepType );
	
	
	long lThreshold;
	if( m_chkThreshold.GetCheck() == 0)	// uncheck
	{
		lThreshold = LONG_MAX;
	}
	else								// check
	{
		if( m_dtctrlThreshold.IsWindowEnabled() )
		{
			time.set( m_oleThreshold.GetHour(), m_oleThreshold.GetMinute(), m_oleThreshold.GetSecond() );
			lThreshold = time.asSeconds();
		}
		else
		{
			lThreshold = (long) m_fThreshold;	
		}
	}
	m_pReportParam->SetThreshold( lThreshold );
	
	
	std::vector<CMobileElemConstraint> mobList;
	switch( m_iReportType)
	{
	case ENUM_UTILIZATION_REP:
	case ENUM_THROUGHPUT_REP:
	case ENUM_QUEUELENGTH_REP:
	case ENUM_AVGQUEUELENGTH_REP:
	case ENUM_BAGCOUNT_REP:
	case ENUM_CONVEYOR_WAIT_LENGTH_REP:
		break;
		
	case ENUM_ACOPERATION_REP:
	case ENUM_BAGDISTRIBUTION_REP:
		// flight constraint
		{
			for( int i=0; i<static_cast<int>(m_vFltList.size()); i++ )
			{
				CMobileElemConstraint mob(m_pTerm);
				(FlightConstraint&)mob= m_vFltList[i];
				//mob.SetInputTerminal(&GetDocument)
				mobList.push_back( mob );
			}
			m_pReportParam->SetPaxType( mobList );
			break;
		}
	default:
		{	
			for( int i=0; i<m_paxConst.getCount(); i++)
			{
				mobList.push_back( *(m_paxConst.getConstraint(i)));
			}
			m_pReportParam->SetPaxType( mobList );
			break;
		}
	}
	
	m_pReportParam->SetProcessorGroup( m_vProcList );
	
	std::vector<CString> strAreaPotal;
	if( m_comboAreaPortal.IsWindowEnabled() )
	{
		CString str;
		m_comboAreaPortal.GetWindowText(str);
		str.TrimLeft();
		str.TrimRight();
		
		if(! str.IsEmpty() )
		{
			strAreaPotal.push_back( str );
			m_pReportParam->SetAreas( strAreaPotal );
			m_pReportParam->SetPortals( strAreaPotal );
		}
	}
	
	return TRUE;
}
void CReportParamDlg::OnButtonApply() 
{
	// TODO: Add your control notification handler code here
	if( !GetParaFromGUI() )
		return;

	if( m_bNewData )
	{
		m_pTerm->m_pReportParaDB->Add( m_pReportParam );
		m_bNewData = false;
	}

	m_pTerm->m_pReportParaDB->saveDataSet( m_pDoc->m_ProjInfo.path, false );

	CDialog::OnOK();
}

void CReportParamDlg::OnButtonCancel() 
{
	// TODO: Add your control notification handler code here
	if( m_bNewData )
	{
		delete m_pReportParam;
	}
	m_pTerm->m_pReportParaDB->loadDataSet( m_pDoc->m_ProjInfo.path );
	CDialog::OnCancel();
}

void CReportParamDlg::DisableTBAllBtn(CToolBar* pToolBar)
{
	CToolBarCtrl* pTBC=&(pToolBar->GetToolBarCtrl());
	pTBC->EnableButton(ID_TOOLBARBUTTONADD,FALSE);
	pTBC->EnableButton(ID_TOOLBARBUTTONDEL,FALSE);

}

void CReportParamDlg::OnSelchangeListProtype() 
{
	int nCurSel = m_lstboProType.GetCurSel();
	if( nCurSel == LB_ERR )
		m_ProToolBar.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_DELETE,FALSE );
	else
		m_ProToolBar.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_DELETE, TRUE );		
}

//////////////////////////////////////////////////////////////////////////
// copy & paste & save & load
//////////////////////////////////////////////////////////////////////////
void CReportParamDlg::OnButtonPaste() 
{
	assert( m_pParentWnd );
	assert( m_pParentWnd->IsKindOf( RUNTIME_CLASS( CNodeView) ) );
	
	CReportParameter* pCopyPara = ((CNodeView*)m_pParentWnd)->getCopyReportPara();
	assert( pCopyPara );
	
	// copy data from m_pCopyReportPara
	CopyParaData( pCopyPara );
	//////////////////////////////////////////////////////////////////////////
	SetGUIFromPara();
}

void CReportParamDlg::CopyParaData( CReportParameter* _pCopyPara )
{
	ElapsedTime time;
	if( _pCopyPara->GetStartTime( time ) )
		m_pReportParam->SetStartTime( time );
	if( _pCopyPara->GetEndTime( time ) )
		m_pReportParam->SetEndTime( time );
	long lTemp;
	if( _pCopyPara->GetInterval(lTemp) )
		m_pReportParam->SetInterval( lTemp );
	
	int iTemp;
	if( _pCopyPara->GetReportType( iTemp ))
		m_pReportParam->SetReportType( iTemp );
	if( _pCopyPara->GetThreshold( lTemp) )
		m_pReportParam->SetThreshold( lTemp );
	
	// pax type
	std::vector<CMobileElemConstraint> mobList;
	if( _pCopyPara->GetPaxType( mobList ) )
		m_pReportParam->SetPaxType( mobList );
	
	// processor type
	std::vector<ProcessorID> vProcessorList;
	if( _pCopyPara->GetProcessorGroup( vProcessorList ) )
		m_pReportParam->SetProcessorGroup( vProcessorList );
	
	// area
	std::vector<CString> vArea;
	if( _pCopyPara->GetAreas( vArea ) )
		m_pReportParam->SetAreas( vArea );
	
	//portal
	std::vector<CString> vPortals;
	if( _pCopyPara->GetPortals( vPortals ) )
		m_pReportParam->SetPortals( vPortals );	
	
}

void CReportParamDlg::OnButtonCopy() 
{
	assert( m_pParentWnd );
	assert( m_pParentWnd->IsKindOf( RUNTIME_CLASS( CNodeView) ) );
	
	if( !GetParaFromGUI() )
		return;

	((CNodeView*)m_pParentWnd )->setCopyReortPara( m_pReportParam );
}

// save report parameter to a file
void CReportParamDlg::OnButtonSavefile() 
{
	if( !GetParaFromGUI() )
		return;
	
	CFileDialog savedlg(FALSE,".par",NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
						"Report Parameter(*.par)|*.par||",this, 0, FALSE  );
	if( savedlg.DoModal() == IDOK )
	{
		CString strFileName = savedlg.GetPathName();
		m_pReportParam->SaveReportParaToFile( strFileName );
	}
}

// load report parameter from a file
void CReportParamDlg::OnButtonLoadfile() 
{
	CFileDialog loaddlg(TRUE,NULL,NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"Report Parameter(*.par)|*.par||",this, 0, FALSE  );
	if( loaddlg.DoModal() == IDOK )
	{
		CString strFileName = loaddlg.GetPathName();
		CReportParameter* pPara = CReportParameter::LoadReoprtParaFromFile( strFileName, m_pTerm );
		if( pPara )
		{
			CopyParaData( pPara );
			SetGUIFromPara();
			delete pPara;
		}
	}
}


int CReportParamDlg::GetTotalMinutes(COleDateTime _oleDT)
{
	return _oleDT.GetHour()*60+_oleDT.GetMinute();
}

void CReportParamDlg::OnRadioDetailed() 
{
	m_lstboPaxType.EnableWindow( FALSE );
	m_PasToolBar.ShowWindow( SW_HIDE );
	m_PasToolBar.EnableWindow( FALSE );		
}
	

void CReportParamDlg::OnRadioSummary() 
{
	m_lstboPaxType.EnableWindow( TRUE );
	m_PasToolBar.ShowWindow( SW_SHOW );
	m_PasToolBar.EnableWindow( TRUE );	
}

void CReportParamDlg::LoadProTree()
{
	int nProcTypeCount = m_vProcList.size();
	if( nProcTypeCount == 0 )
	{
		HTREEITEM hItem = m_treePaxType.InsertItem( "All Processors", m_throughput_proroot);
		m_treePaxType.SetItemData( hItem, -1);
	}else
	{
		for( int i=0; i<nProcTypeCount; i++ )
		{
			ProcessorID id = m_vProcList[i];
			id.SetStrDict(m_pTerm->inStrDict );
			char szBuf[256] = {0};
			id.printID(szBuf);
			HTREEITEM hItem = m_treePaxType.InsertItem( szBuf ,m_throughput_proroot);
			ProcessorID* proid = new ProcessorID(id) ;
			m_treePaxType.SetItemData( hItem, (DWORD_PTR)(proid) );
		}
	}
}

void CReportParamDlg::OnUpdateBtnAdd(CCmdUI* pCmdUI)
{
	if(m_iReportType==ENUM_DISTANCE_REP||m_iReportType==ENUM_DURATION_REP)
	{
		HTREEITEM hItem=m_treePaxType.GetSelectedItem();
		HTREEITEM hItemParent=m_treePaxType.GetParentItem(hItem);
		if(hItem==m_hItemPaxType||hItem==m_hItemFrom||hItem==m_hItemTo)	// Level 3
			pCmdUI->Enable(TRUE);
		else
			pCmdUI->Enable(FALSE);
		return ;
	}
	if(m_iReportType == ENUM_THROUGHPUT_REP)
	{
		HTREEITEM item = m_treePaxType.GetSelectedItem() ;
		if(item == NULL)
		{
			pCmdUI->Enable(FALSE) ;
			return ;
		}
		int type  = m_treePaxType.GetItemData(item);
		if(type == PAX_TYPE || type == THROUGH_PUT_PROCESSOR)
			pCmdUI->Enable(TRUE) ;
		else
			pCmdUI->Enable(FALSE) ;
	}
	else
		pCmdUI->Enable(TRUE);
}

void CReportParamDlg::OnUpdateBtnDel(CCmdUI* pCmdUI)
{
	if(m_iReportType==ENUM_DISTANCE_REP||m_iReportType==ENUM_DURATION_REP)
	{
		HTREEITEM hItem=m_treePaxType.GetSelectedItem();
		HTREEITEM hItemParent=m_treePaxType.GetParentItem(hItem);
		if(hItemParent==m_hItemPaxType||hItemParent==m_hItemFrom||hItemParent==m_hItemTo)	// Level 3
			pCmdUI->Enable(TRUE);
		else
			pCmdUI->Enable(FALSE);
		return ;
	}
	if(m_iReportType == ENUM_THROUGHPUT_REP)
	{
		HTREEITEM item = m_treePaxType.GetSelectedItem() ;
		if(item == NULL)
		{
			pCmdUI->Enable(FALSE) ;
			return ;
		}
		int type  = m_treePaxType.GetItemData(item);
		if(type == PAX_TYPE || type == THROUGH_PUT_PROCESSOR)
			pCmdUI->Enable(FALSE) ;
		else
			pCmdUI->Enable(TRUE) ;
	}
	else
	{
		int nCurSel = m_lstboPaxType.GetCurSel();
		pCmdUI->Enable( nCurSel != LB_ERR );
	}
}

BOOL CReportParamDlg::ContinueModal()
{
	if( m_PasToolBar.IsWindowVisible() )
	{
		m_PasToolBar.OnUpdateCmdUI(( CFrameWnd*)(m_PasToolBar.GetParent()), ( WPARAM ) TRUE );
	}
	return CDialog::ContinueModal();
}