// DlgProbDist.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "..\MFCExControl\InPlaceEdit.h"
#include "..\MFCExControl\CoolTreeEditSpin.h"

#include "chart\vcplot.h"
#include "chart\vcaxis.h"
#include "chart\vcvaluescale.h"
#include "chart\vccategoryscale.h"
#include "chart\vclabels.h"
#include "chart\vclabel.h"
#include "chart\vcfont.h"
#include "chart\vcrect.h"
#include "chart\vccoor.h"
#include "chart\vcintersection.h"

#include "common\WinMsg.h"
#include "common\floatutils.h"
#include "common\exeption.h"
#include "common\probdistmanager.h"
#include "inputs\probdisthelper.h"
#include "DlgProbDist.h"
#include "common\airportdatabase.h"
#include "engine\terminal.h"
#include "assert.h"
#include "DlgSaveWarningWithCheckBox.h"
#include "DlgProbDistHistData.h"
#include ".\dlgprobdist.h"
#include "../Common/AirportDatabaseList.h"
#include "../Database/ARCportADODatabase.h"
#include <Common/ProbabilityDistribution.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static char* szPDTypeLabel[] = {
	"UNSPECIFIED",
	"BERNOULLI",
	"BETA",
	"CONSTANT",
	"EMPIRICAL",
	"ERLANG",
	"EXPONENTIAL",
	"HISTOGRAM",
	"GAMMA",
	"NORMAL",
	"TRIANGLE",
	"UNIFORM",
	"WEIBULL",
};

static const int nPDTypeLParam[] = {
	-1,
	BERNOULLI,
	BETA,
	CONSTANT,
	EMPIRICAL,
	ERLANG,
	EXPONENTIAL,
	HISTOGRAM,
	GAMMA,
	NORMAL,
	TRIANGLE,
	UNIFORM,
	WEIBULL,
};

static const int nPDTypeTable[] = {
	1, //BERNOULLI
	2, //BETA
	3, //CONSTANT
	4, //EMPIRICAL
	6, //EXPONENTIAL
	7, //HISTOGRAM
	9, //NORMAL
	11, //UNIFORM
	0, //PASSENGER_TYPE
    0, //PROCESSOR
    0, //FLIGHT_TYPE
	0, //PROCESSORWITHPIPE
	12, //WEIBULL
	8,//GAMMA
	5,//ERLANG
	10//TRIANGLE
};

typedef std::pair< int, CProbDistManager* >  SORT_PARAM_PAIR;

static void SaveHistogramData(const CListCtrl& _lstParams, HistogramDistribution* _pd)
{
	//store values from list to pd
	int nCount = _lstParams.GetItemCount();
	double* dVal = new double[nCount];
	double* nProb = new double[nCount];
	int counter = 0;
	double total = 0;
	for( int i=0; i<nCount; i++ )
	{
		CString csVal = _lstParams.GetItemText( i, 0 );
		CString csPercent = _lstParams.GetItemText( i, 1 );
		if( csVal.IsEmpty() || csPercent.IsEmpty() )
			continue;

		dVal[counter] = atof( csVal );
		nProb[counter] = atof( csPercent );
		total += nProb[counter];
		counter++;
	}
	
	try
	{
		_pd->init( counter, dVal, nProb );
	}
	catch( TwoStringError* _pError )
	{
		delete _pError;
	}


	delete [] dVal;
	delete [] nProb;
}

static void InitListColumns(CListCtrl& lstDist, CListCtrl& lstParams)
{
	CRect r;
	lstDist.GetWindowRect(&r);
	DWORD dwStyle = lstDist.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES;
	lstDist.SetExtendedStyle( dwStyle );
	lstDist.InsertColumn(0, _T("Name"), LVCFMT_LEFT, (r.Width()/3)-2);
	lstDist.InsertColumn(1, _T("Type"), LVCFMT_LEFT, (r.Width()/3)-2);
	lstDist.InsertColumn(2, _T("Variable"), LVCFMT_LEFT, (r.Width()/3)-2);

	lstParams.GetWindowRect(&r);
	dwStyle = lstParams.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES;
	lstParams.SetExtendedStyle( dwStyle );
	lstParams.InsertColumn(0, _T("Parameter"), LVCFMT_LEFT, (r.Width()/2)-2,0);
	lstParams.InsertColumn(1, _T("Value"), LVCFMT_LEFT, (r.Width()/2)-2,1);
}
/////////////////////////////////////////////////////////////////////////////
// CDlgProbDist dialog


CDlgProbDist::CDlgProbDist(CAirportDatabase* _pAirportDB, BOOL bNeedRet, CWnd* pParent /*=NULL*/) :
	CDialog(CDlgProbDist::IDD, pParent),m_pProbDistMan( NULL ),
	m_bNeedReturn(bNeedRet),
	m_sizeLast(0,0),
	m_commitDefault(FALSE),
	m_distTypeInPlaceComboBox(1),
	m_pAirportDB(NULL),m_IsEdit(FALSE)
{
	//{{AFX_DATA_INIT(CDlgProbDist)
	//}}AFX_DATA_INIT
	if( _pAirportDB )
	{
		m_pProbDistMan = _pAirportDB->getProbDistMan();
		m_pAirportDB = _pAirportDB;
	}
}
CDlgProbDist::~CDlgProbDist()
{

}

void CDlgProbDist::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgProbDist)
	DDX_Control(pDX, IDC_COMBO_GRAPHTYPE, m_cmbGraphType);
	DDX_Control(pDX, IDC_STATIC_PROBDENSITYFUNC, m_staticProbDensityFunc);
	DDX_Control(pDX, IDC_STATIC_PARAMGROUP2, m_staticParamGroup2);
	DDX_Control(pDX, IDC_STATIC_PARAMGROUP, m_staticParamGroup);
	DDX_Control(pDX, IDC_STATIC_DISTGROUP2, m_staticDistGroup2);
	DDX_Control(pDX, IDC_STATIC_DISTGROUP, m_staticDistGroup);
	DDX_Control(pDX, IDC_TOOLBARCONTAINER, m_staticToolbarParams);
	DDX_Control(pDX, IDC_TOOLBARCONTAINER2, m_staticToolbarDist);
	DDX_Control(pDX, IDC_LIST_DISTRIBUTIONS, m_lstDistributions);
	DDX_Control(pDX, IDC_LIST_PARAMETERS, m_lstParameters);
	DDX_Control(pDX, IDC_STATIC_EXTRAPARAM, m_staticExtraParamName);
	DDX_Control(pDX, IDC_EDIT_EXTRAPARAM, m_editExtraParam);
	DDX_Control(pDX, IDC_MSCHART2, m_chart);
	//}}AFX_DATA_MAP

	DDX_Control(pDX, IDC_BUTTON_LOADDEFAULT__PRO, m_button_loadDefault);
//	DDX_Control(pDX, IDC_STATIC_LOAD, m_StaticLoad);
}


BEGIN_MESSAGE_MAP(CDlgProbDist, CDialog)
	//{{AFX_MSG_MAP(CDlgProbDist)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_NOTIFY(LVN_GETDISPINFO, IDC_LIST_DISTRIBUTIONS, OnGetDispInfoListDistributions)
	ON_NOTIFY(LVN_GETDISPINFO, IDC_LIST_PARAMETERS, OnGetDispInfoListParameters)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_LIST_DISTRIBUTIONS, OnColumnclickListDistributions)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_LIST_DISTRIBUTIONS, OnEndLabelEditListDistributions)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_DISTRIBUTIONS, OnDblClickListDistributions)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_DISTRIBUTIONS, OnItemChangedListDistributions)
	ON_NOTIFY(LVN_ITEMCHANGING, IDC_LIST_DISTRIBUTIONS, OnItemChangingListDistributions)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_PARAMETERS, OnItemChangedListParameters)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_PARAMETERS, OnDblClickListParameters)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_LIST_PARAMETERS, OnEndLabelEditListParameters)
	ON_CBN_SELCHANGE(IDC_COMBO_GRAPHTYPE, OnChangeGraphType)
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_TOOLBARBUTTONADD, OnDistToolbarbuttonADD)
	ON_COMMAND(ID_PDTOOLBARBTNADD, OnParamToolbarbuttonADD)
	ON_COMMAND(ID_TOOLBARBUTTONDEL, OnDistToolbarbuttonDEL)
	ON_COMMAND(ID_PDTOOLBARBTNDEL, OnParamToolbarbuttonDEL)
	ON_COMMAND(ID_TOOLBARBUTTONCOPY, OnDistToolbarbuttonCopy)
	ON_COMMAND(ID_TOOLBARBUTTONPASTE, OnDistToolbarbuttonPaste)
	ON_COMMAND(ID_PDTOOLBARBTNFROMFILE, OnParamToolbarbuttonFF)
	ON_COMMAND(ID_PDTOOLBARBTNRAWFROMFILE, OnParamToolbarbuttonRAWFF)
	ON_COMMAND(ID_AIRPORT_SAVEAS, OnSaveAsTemplate)
	ON_COMMAND(ID_AIRPORT_LOAD, OnLoadFromTemplate)

	ON_NOTIFY_EX(TTN_NEEDTEXTA, 0, OnToolTipText)

	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_COMMAND(IDCANCEL, OnCancel)
	ON_BN_CLICKED(IDC_BUTTON_LOADDEFAULT__PRO, OnBnClickedButtonLoaddefault)
END_MESSAGE_MAP()

void CDlgProbDist::InitToolBars()
{
	CRect rc;
	m_staticToolbarParams.GetWindowRect( &rc );
	ScreenToClient(&rc);
	m_ToolBarParams.MoveWindow(&rc);
	m_ToolBarParams.ShowWindow(SW_SHOW);
	m_ToolBarParams.GetToolBarCtrl().EnableButton( ID_PDTOOLBARBTNADD,FALSE );
	m_ToolBarParams.GetToolBarCtrl().EnableButton( ID_PDTOOLBARBTNDEL,FALSE );
	m_ToolBarParams.GetToolBarCtrl().EnableButton( ID_PDTOOLBARBTNFROMFILE,FALSE );
	m_ToolBarParams.GetToolBarCtrl().EnableButton( ID_PDTOOLBARBTNRAWFROMFILE,FALSE );

	//m_ToolBarParams.GetToolBarCtrl().HideButton( ID_PEOPLEMOVER_CHANGE );
	m_staticToolbarParams.ShowWindow(SW_HIDE);

	m_staticToolbarDist.GetWindowRect( &rc );
	ScreenToClient(&rc);
	m_ToolBarDist.MoveWindow(&rc);
	m_ToolBarDist.ShowWindow(SW_SHOW);
	m_ToolBarDist.GetToolBarCtrl().EnableButton( ID_TOOLBARBUTTONADD,TRUE );
	m_ToolBarDist.GetToolBarCtrl().EnableButton( ID_TOOLBARBUTTONDEL,FALSE );
	m_ToolBarDist.GetToolBarCtrl().EnableButton( ID_TOOLBARBUTTONCOPY,FALSE );
	m_ToolBarDist.GetToolBarCtrl().EnableButton( ID_TOOLBARBUTTONPASTE,FALSE );
	m_staticToolbarDist.ShowWindow(SW_HIDE);
}

void CDlgProbDist::LoadDistributionList()
{	
	m_lstDistributions.DeleteAllItems();
	int nCount = m_pProbDistMan->getCount();
	for( int i=0; i<nCount; i++ )
	{
		//CProbDistEntry* pProbDistItem = PROBDISTMANAGER->getItem( i );
		m_lstDistributions.InsertItem(LVIF_TEXT | LVIF_PARAM, i, LPSTR_TEXTCALLBACK, 0, 0, 0, (LPARAM) i);
	}
}

int CALLBACK CDlgProbDist::CompareFuncAscending(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	int idx1 = (int) lParam1;
	int idx2 = (int) lParam2;
	
	SORT_PARAM_PAIR* _pParamPair = ( SORT_PARAM_PAIR*)lParamSort;

	CProbDistEntry* pItem1 = _pParamPair->second->getItem(idx1);
	CProbDistEntry* pItem2 = _pParamPair->second->getItem(idx2);
	int nResult;
	switch(_pParamPair->first ) 
	{
		case 0://pd name
			nResult = pItem1->m_csName.CompareNoCase( pItem2->m_csName );
			break;
		case 1: // pd type
			nResult = _stricmp(pItem1->m_pProbDist->getProbabilityName(), pItem2->m_pProbDist->getProbabilityName());
			break;
		case 2: // rv type
			nResult = _stricmp(pItem1->m_pProbDist->getRandomVariableTypeName(), pItem2->m_pProbDist->getRandomVariableTypeName());
			break;
		default:
			ASSERT(0);
			break;
		
	}

	return nResult;
}

int CALLBACK CDlgProbDist::CompareFuncDescending(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	int idx1 = (int) lParam2;
	int idx2 = (int) lParam1;

	SORT_PARAM_PAIR* _pParamPair = ( SORT_PARAM_PAIR*)lParamSort;

	CProbDistEntry* pItem1 = _pParamPair->second ->getItem(idx1);
	CProbDistEntry* pItem2 = _pParamPair->second ->getItem(idx2);
	int nResult;
	switch(_pParamPair->first) 
	{
		case 0://pd name
			nResult = pItem1->m_csName.CompareNoCase( pItem2->m_csName );
			break;
		case 1: // pd type
			nResult = _stricmp(pItem1->m_pProbDist->getProbabilityName(), pItem2->m_pProbDist->getProbabilityName());
			break;
		case 2: // rv type
			nResult = _stricmp(pItem1->m_pProbDist->getRandomVariableTypeName(), pItem2->m_pProbDist->getRandomVariableTypeName());
			break;
		default:
			ASSERT(0);
			break;
		
	}

	return nResult;
}


/////////////////////////////////////////////////////////////////////////////
// CDlgProbDist message handlers

int CDlgProbDist::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_ToolBarParams.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_ToolBarParams.LoadToolBar(IDR_PDPARAMTOOLBAR))
	{
		TRACE0("Failed to create params toolbar\n");
		return -1;      // fail to create
	}

	if (!m_ToolBarDist.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_ToolBarDist.LoadToolBar(IDR_DISTRIBUTIONTOOLBAR))
	{
		TRACE0("Failed to create dist toolbar\n");
		return -1;      // fail to create
	}

	CRect rectClient;
	GetClientRect( rectClient );
	m_toolTipCtrl.Create( this, TTS_ALWAYSTIP );
	m_toolTipCtrl.AddTool( this, LPSTR_TEXTCALLBACK, rectClient, IDR_PDPARAMTOOLBAR );	
	m_toolTipCtrl.SetMaxTipWidth(SHRT_MAX);
	m_toolTipCtrl.SetDelayTime(TTDT_AUTOPOP, 5000);
	m_toolTipCtrl.Activate(true);	

	return 0;
}

BOOL CDlgProbDist::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_nSelectedPD = -1;
	m_nSortColumn = -1;
	
	m_Commit_Oper = FALSE ;
	m_Load_Oper = FALSE ;

	if(m_bNeedReturn)
		GetDlgItem(IDOK)->EnableWindow(FALSE);

	InitToolBars();
	InitializeChart();
	InitListColumns(m_lstDistributions, m_lstParameters);
	LoadDistributionList();
	UpdateDistributionData();

	if(m_sizeLast.cx!=0 && m_sizeLast.cy!=0) {
		DoResize(m_sizeLast.cx, m_sizeLast.cy);
	}
	
	//	Init Cool button
	m_button_loadDefault.SetOperation(0);
	m_button_loadDefault.SetWindowText(_T("Database Template"));
	m_button_loadDefault.SetOpenButton(FALSE);
	m_button_loadDefault.SetType(CCoolBtn::TY_DISTRIBUTION);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgProbDist::OnGetDispInfoListDistributions(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;

	if(pDispInfo->item.mask & LVIF_TEXT) {
		int idxPD = (int)pDispInfo->item.lParam;
		CProbDistEntry* pProbDistItem = m_pProbDistMan->getItem(idxPD);
		switch(pDispInfo->item.iSubItem) {
		case 0:
			lstrcpy(pDispInfo->item.pszText, pProbDistItem->m_csName);
			break;
		case 1:
			if(pProbDistItem->m_pProbDist)
				lstrcpy(pDispInfo->item.pszText, pProbDistItem->m_pProbDist->getProbabilityName());
			else
				lstrcpy(pDispInfo->item.pszText, "Unspecified");
			break;
		case 2:
			if(pProbDistItem->m_pProbDist)
				lstrcpy(pDispInfo->item.pszText, pProbDistItem->m_pProbDist->getRandomVariableTypeName());
			break;
		default:
			ASSERT(0);
			break;
		}
	}	
	
	*pResult = 0;
}

void CDlgProbDist::SortByColumn(int nSortCol)
{
	static bool bSortAscending[3] = { true, true, true };
	if(nSortCol == m_nSortColumn) 
	{
		//toggle ascending/descending
		bSortAscending[nSortCol] = !(bSortAscending[nSortCol]);
	}
	
	static SORT_PARAM_PAIR _sortParameterPair;
	_sortParameterPair = std::make_pair( nSortCol, m_pProbDistMan );

	if(bSortAscending[nSortCol])
		m_lstDistributions.SortItems(CompareFuncAscending,  (long)&_sortParameterPair);
	else
		m_lstDistributions.SortItems(CompareFuncDescending, (long)&_sortParameterPair);
	
	m_nSortColumn = nSortCol;
}

void CDlgProbDist::OnColumnclickListDistributions(NMHDR* pNMHDR, LRESULT* pResult) 
{
	
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	SortByColumn(pNMListView->iSubItem);
		
	*pResult = 0;
}



void CDlgProbDist::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);

	if((cx==0 && cy==0) || (m_sizeLast.cx==0 && m_sizeLast.cy==0)) {
		m_sizeLast.cx=cx; m_sizeLast.cy=cy;
		return;
	}
	
	DoResize(cx, cy);
}

LRESULT CDlgProbDist::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	if (message == WM_NOTIFY)
	{
		switch(wParam)
		{
		case ID_WND_SPLITTER1:
		case ID_WND_SPLITTER2:
		{
			SPC_NMHDR* pHdr = (SPC_NMHDR*) lParam;
			DoSplitterResize(pHdr->delta,wParam);
		}
		break;
		}
	}
	else if (message == WM_INPLACE_COMBO)
	{
		if(m_lstDistributions.GetItemCount()!=0) {

			int nIdx = (int) lParam;
			UINT nID = (UINT) wParam;

			if(nID == 1) { //pdtype
				int idxPD = (int) m_lstDistributions.GetItemData(m_nPDTypeSelItem);
				CProbDistEntry* pItem = m_pProbDistMan->getItem(idxPD);
				ASSERT(m_nSelectedPD == idxPD);
				// TRACE("selected idx %d for pd type\n", nIdx);
				if(pItem->m_pProbDist) { //pd already created
					if(nPDTypeLParam[nIdx] == -1) {
						//user tried to set pd type to unspecified
						AfxMessageBox("You have selected \"unspecified\" for the probability type.\nThe probability type will remain unchanged.");
					}
					else if(pItem->m_pProbDist->getProbabilityType() != nPDTypeLParam[nIdx]) { //user has changed pd type
						delete pItem->m_pProbDist;
						pItem->m_pProbDist = ProbDistHelper::CreateProbabilityDistribution(nPDTypeLParam[nIdx]);
						UpdateDistributionData();
						//LoadParamList(pItem);
					}
				}
				else { //create new pd
					if(nIdx != 0) { //if user chose a pd type other than "unspecified"
						pItem->m_pProbDist = ProbDistHelper::CreateProbabilityDistribution(nPDTypeLParam[nIdx]);
						//LoadParamList(pItem);
						UpdateDistributionData();
					}
				}
			}
			else {
				ASSERT(0);
			}
			SetFocus();
			return TRUE;
		}
	}

	return CDialog::DefWindowProc(message, wParam, lParam);
}

void CDlgProbDist::DoSplitterResize(int delta, UINT nIDSplitter)
{
	CRect rectWnd;
	switch(nIDSplitter)
	{
	case ID_WND_SPLITTER1: //vertical
		{
			CSplitterControl::ChangeWidth(&m_staticDistGroup, delta, CW_LEFTALIGN);
			CSplitterControl::ChangeWidth(&m_staticDistGroup2, delta, CW_LEFTALIGN);
			CSplitterControl::ChangeWidth(&m_ToolBarDist, delta, CW_LEFTALIGN);
			CSplitterControl::ChangeWidth(&m_lstDistributions, delta, CW_LEFTALIGN);
			
			CSplitterControl::ChangeWidth(&m_staticParamGroup, -delta, CW_RIGHTALIGN);
			CSplitterControl::ChangeWidth(&m_staticParamGroup2, -delta, CW_RIGHTALIGN);
			CSplitterControl::ChangeWidth(&m_ToolBarParams, -delta, CW_RIGHTALIGN);
			CSplitterControl::ChangeWidth(&m_lstParameters, -delta, CW_RIGHTALIGN);
			CSplitterControl::ChangePos(&m_staticExtraParamName, -delta, 0);
			CSplitterControl::ChangePos(&m_editExtraParam, -delta, 0);
		}
		break;
	case ID_WND_SPLITTER2: //horizontal
		{
			CSplitterControl::ChangeHeight(&m_staticDistGroup, delta, CW_TOPALIGN);
			CSplitterControl::ChangeHeight(&m_staticDistGroup2, delta, CW_TOPALIGN);
			CSplitterControl::ChangeHeight(&m_lstDistributions, delta, CW_TOPALIGN);
			CSplitterControl::ChangeHeight(&m_staticParamGroup, delta, CW_TOPALIGN);
			CSplitterControl::ChangeHeight(&m_staticParamGroup2, delta, CW_TOPALIGN);
			CSplitterControl::ChangeHeight(&m_lstParameters, delta, CW_TOPALIGN);
			CSplitterControl::ChangePos(&m_staticExtraParamName, 0, delta);
			CSplitterControl::ChangePos(&m_editExtraParam, 0, delta);
			CSplitterControl::ChangeHeight(&m_wndSplitter1, delta, CW_TOPALIGN);
			
			CSplitterControl::ChangeHeight(&m_staticProbDensityFunc, -delta, CW_BOTTOMALIGN);
			CSplitterControl::ChangeHeight(&m_chart, -delta, CW_BOTTOMALIGN);
		}
		break;
	}
	Invalidate();
}

void CDlgProbDist::OnEndLabelEditListParameters(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	*pResult = 0;

	if(!pDispInfo->item.pszText)
		return;

	int nItem = pDispInfo->item.iItem;
	int nSubItem = pDispInfo->item.iSubItem;

	// TRACE("End Label Edit: %s, item %d, subitem %d\n", pDispInfo->item.pszText, nItem, nSubItem);

	CProbDistEntry* pde = m_pProbDistMan->getItem(m_nSelectedPD);

	if(ProbDistHelper::HasParams(pde->m_pProbDist)) {
		ASSERT(nSubItem == 1); // the param value
		ASSERT(nItem < ProbDistHelper::GetParamCount(pde->m_pProbDist));
		int nError;
		if(!ProbDistHelper::SetParamValue(pde->m_pProbDist, nItem, atof(pDispInfo->item.pszText), &nError)) {
			AfxMessageBox(ProbDistHelper::GetErrorMsg(nError));
			return;
		}
	}
	else { //histogram or emprical
		double val = atof(pDispInfo->item.pszText);
		CString s;
		if(nSubItem==0)
			s.Format("%.2f", val);
		else
			s.Format("%.2f", RoundDouble(val,2));
		m_lstParameters.SetItemText(nItem, nSubItem, s);
		
		SaveHistogramData(m_lstParameters, (HistogramDistribution*) GetSelectedPD()->m_pProbDist);
	}

	UpdateDistributionData();

	*pResult=1;
}

void CDlgProbDist::OnEndLabelEditListDistributions(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	*pResult = 0;

	if(!pDispInfo->item.pszText)
		return;

	int nIdx = pDispInfo->item.iItem;
	
	int idxPD = (int)m_lstDistributions.GetItemData(nIdx);
	ASSERT(m_nSelectedPD == idxPD);
	CProbDistEntry* pde = m_pProbDistMan->getItem(idxPD);
	pde->m_csName = pDispInfo->item.pszText;

	UpdateDistributionData();

	*pResult =1;
}

void CDlgProbDist::InitializeChart()
{
	//m_chart.SetAllowSelections(FALSE);
	m_chart.SetChartType(16l);
	CVcPlot plot = m_chart.GetPlot();
	plot.SetUniformAxis(FALSE);
	for(int nAxis=0; nAxis<2; nAxis++) 
	{
		VARIANT Index;
		CVcAxis axis = plot.GetAxis(nAxis,Index); 
		for(int i=1; i<=axis.GetLabels().GetCount(); i++)
		{
			CVcFont font = axis.GetLabels().GetItem(i).GetVtFont();
			font.SetSize(10.0f);
		}
		
		if(nAxis == 1) { //y axis always 0 to 1
			CVcValueScale scale = axis.GetValueScale();
			//scale.SetAuto(TRUE);
			//scale.SetMaximum(1); 
			//scale.SetMinimum(0);
			//scale.SetMajorDivision(2);
			//axis.GetCategoryScale().SetAuto(FALSE);
		}
	}
	m_chart.Refresh();
	//m_chart.SetEnabled(FALSE);

	m_cmbGraphType.AddString("Probability Density Function");
	m_cmbGraphType.AddString("Cumulative Density Function");
	m_cmbGraphType.SetCurSel(0);
}

void CDlgProbDist::UpdateChart(CProbDistEntry* _pd)
{
	if(!_pd || !_pd->m_pProbDist) {
		//disable chart
		//m_chart.EnableWindow(FALSE);
		m_chart.ShowWindow(SW_HIDE);
	}	
	else {
		m_chart.EnableWindow(TRUE);
		m_chart.ShowWindow(SW_SHOW);
		
		int nPtCount;
		if(m_cmbGraphType.GetCurSel() == 0)
			nPtCount = ProbDistHelper::GetPDFGraphPointCount(_pd->m_pProbDist);
		else
			nPtCount = ProbDistHelper::GetCDFGraphPointCount(_pd->m_pProbDist);
		if(nPtCount == 0)
			return;
		
		double* x = new double[nPtCount];
		double* y = new double[nPtCount];

		if(m_cmbGraphType.GetCurSel() == 0)
			ProbDistHelper::GetPDFGraphPoints(_pd->m_pProbDist, x, y);
		else
			ProbDistHelper::GetCDFGraphPoints(_pd->m_pProbDist, x, y);


		/*
		// TRACE("points: \n   ");
		for(int i=0; i<nPtCount; i++) {
			// TRACE("(%.1f, %.1f) ", x[i],y[i]);
		}
		// TRACE("\n");
*/
			
		COleSafeArray saData;
		DWORD numElements[] = {nPtCount, 2};
		saData.Create(VT_R8, 2, numElements);
		long index[2];
		for(index[0]=0; index[0]<static_cast<long>(numElements[0]); index[0]++) {
			for(index[1]=0; index[1]<static_cast<long>(numElements[1]); index[1]++) {
				if(!index[1]) {
					saData.PutElement(index, &(x[index[0]]));
				}
				else {
					saData.PutElement(index, &(y[index[0]]));
					double temp;
					saData.GetElement(index, &temp);
					// TRACE("y=%f\n", temp);
				}
			}
		}
		m_chart.SetChartData(saData.Detach());

		CVcPlot plot = m_chart.GetPlot();
		VARIANT Index;
		CVcAxis axisX = plot.GetAxis(0,Index); 
		axisX.GetValueScale().SetAuto(FALSE);
		axisX.GetValueScale().SetMaximum(x[nPtCount-1]); 
		axisX.GetValueScale().SetMinimum(x[0]);
		axisX.GetValueScale().SetMajorDivision(4);
		axisX.GetCategoryScale().SetAuto(FALSE);

		CVcAxis axisY = plot.GetAxis(1,Index);
		axisY.GetIntersection().SetAuto(FALSE);
		axisY.GetIntersection().SetPoint(x[0]);
		

		m_chart.Refresh();

		delete [] x;
		delete [] y;
	}	
}

void CDlgProbDist::UpdateExtraParam(CProbDistEntry* _pd)
{
	static char buffer[33];
	if (_pd &&
		_pd->m_pProbDist &&
	    (_pd->m_pProbDist->getProbabilityType() == HISTOGRAM ||
		 _pd->m_pProbDist->getProbabilityType() == EMPIRICAL))
	{
		m_staticExtraParamName.ShowWindow(SW_SHOW);
		m_editExtraParam.ShowWindow(SW_SHOW);
		m_editExtraParam.SetReadOnly(TRUE);

		m_staticExtraParamName.SetWindowText("Total Percentage");
		//get total from params list ctrl
		int nCount = m_lstParameters.GetItemCount();
		double total=0;
		for(int i=0;i<nCount; i++)
		{
			m_lstParameters.GetItemText(i,1,buffer,32);
			double val = atof(buffer);
			total+=val;
		}
		CString s;
		s.Format("%.2f", total);
		m_editExtraParam.SetWindowText(s);
	}
	else
	{
		m_staticExtraParamName.ShowWindow(SW_HIDE);
		m_editExtraParam.ShowWindow(SW_HIDE);
	}
}

void CDlgProbDist::UpdateDistributionData()
{
	CProbDistEntry* pde = NULL;
	if(m_nSelectedPD != -1)
		pde = GetSelectedPD();

	LoadParamList(pde);
	UpdateExtraParam(pde);
	UpdateChart(pde);

	CRect r;
	this->GetClientRect(&r);
	DoResize(r.Width(), r.Height());
}

void CDlgProbDist::LoadParamList(CProbDistEntry* _pd)
{
	static TCHAR buffer[256];
	if(!_pd || !_pd->m_pProbDist) {
		m_lstParameters.DeleteAllItems();
		m_lstParameters.EnableWindow(FALSE);
		m_staticParamGroup.SetWindowText("");
		m_ToolBarParams.GetToolBarCtrl().EnableButton( ID_PDTOOLBARBTNADD,FALSE );
		m_ToolBarParams.GetToolBarCtrl().EnableButton( ID_PDTOOLBARBTNFROMFILE,FALSE );

		UpdateExtraParam(_pd);
		return;
	}

	LV_COLUMN lvc;
	lvc.pszText = buffer;
	lvc.cchTextMax = 255;
	lvc.mask = LVCF_TEXT;
	CRect r;
	m_lstParameters.EnableWindow(TRUE);
	m_lstParameters.GetWindowRect(&r);
	//insert columns
	if (_pd->m_pProbDist->getProbabilityType() == HISTOGRAM ||
		_pd->m_pProbDist->getProbabilityType() == EMPIRICAL)
	{	
		m_lstParameters.GetColumn(0, &lvc);
		lstrcpy(lvc.pszText, "Sample");
		m_lstParameters.SetColumn(0, &lvc);
		m_lstParameters.GetColumn(1, &lvc);
		lstrcpy(lvc.pszText, "Pct");
		m_lstParameters.SetColumn(1, &lvc);
		//enable add and delete buttons
		m_ToolBarParams.GetToolBarCtrl().EnableButton( ID_PDTOOLBARBTNADD,TRUE );
		m_ToolBarParams.GetToolBarCtrl().EnableButton( ID_PDTOOLBARBTNFROMFILE,TRUE );
	}
	else {
		m_lstParameters.GetColumn(0, &lvc);
		lstrcpy(lvc.pszText, "Parameter");
		m_lstParameters.SetColumn(0, &lvc);
		m_lstParameters.GetColumn(1, &lvc);
		lstrcpy(lvc.pszText, "Value");
		m_lstParameters.SetColumn(1, &lvc);
		//disable add and delete buttons
		m_ToolBarParams.GetToolBarCtrl().EnableButton( ID_PDTOOLBARBTNADD,FALSE );
		m_ToolBarParams.GetToolBarCtrl().EnableButton( ID_PDTOOLBARBTNFROMFILE,FALSE );		
	}
	m_ToolBarParams.GetToolBarCtrl().EnableButton( ID_PDTOOLBARBTNDEL, FALSE );
		
	//delete all entries
	m_lstParameters.DeleteAllItems();
	int nCount = ProbDistHelper::GetParamCount(_pd->m_pProbDist);
	if(ProbDistHelper::HasParams(_pd->m_pProbDist)) {
		for( int i=0; i<nCount; i++ ) {
			m_lstParameters.InsertItem(LVIF_TEXT | LVIF_PARAM, i, LPSTR_TEXTCALLBACK, 0, 0, 0, (LPARAM) i);
		}
	}
	else {
		for( int i=0; i<nCount; i++ ) {
			CString s;
			s.Format("%.2f", ProbDistHelper::GetSampleValue(_pd->m_pProbDist, i));
			m_lstParameters.InsertItem(LVIF_TEXT, i, s, LVIS_SELECTED|LVIS_FOCUSED, 0, 0, NULL);
			s.Format("%.2f", ProbDistHelper::GetSamplePct(_pd->m_pProbDist, i));
			m_lstParameters.SetItemText(i,1,s);
		}
		m_lstParameters.SetCallbackMask(0);
	}

	//set name of gropup box to pd's name and type
	CString s;
	s.Format("%s (%s) Parameters", _pd->m_csName, _pd->m_pProbDist->getProbabilityName());
	m_staticParamGroup.SetWindowText(s);

	
}

void CDlgProbDist::OnGetDispInfoListParameters(NMHDR* pNMHDR, LRESULT* pResult) 
{
	static const int numdigits = 4;
	static char buffer[numdigits+5];
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;

	POSITION pos = m_lstDistributions.GetFirstSelectedItemPosition();
	int idx = m_lstDistributions.GetNextSelectedItem(pos);
	if(idx == -1)
		return;

	int idxPD = m_lstDistributions.GetItemData(idx);
	CProbDistEntry* pde = m_pProbDistMan->getItem(idxPD);
	ASSERT(m_nSelectedPD == idxPD);

	if(pDispInfo->item.mask & LVIF_TEXT) {
		int paramidx = (int) pDispInfo->item.lParam;
		switch(pDispInfo->item.iSubItem) {
		case 0:
			if(ProbDistHelper::HasParams(pde->m_pProbDist))
				lstrcpy(pDispInfo->item.pszText, ProbDistHelper::GetParamName(pde->m_pProbDist, paramidx));
			else {
				ASSERT(0);
			}
			break;
		case 1:
			if(ProbDistHelper::HasParams(pde->m_pProbDist))
				sprintf(pDispInfo->item.pszText, "%.2f", ProbDistHelper::GetParamValue(pde->m_pProbDist, paramidx));
			else {
				ASSERT(0);
			}
			break;
		default:
			ASSERT(0);
			break;
		}
	}	
	
	*pResult = 0;
}

void CDlgProbDist::OnItemChangedListParameters(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_LISTVIEW* pnmv = (NM_LISTVIEW*)pNMHDR;
	m_ToolBarParams.GetToolBarCtrl().EnableButton(ID_PDTOOLBARBTNDEL, FALSE);
	CProbDistEntry* pde = GetSelectedPD();
	if((pnmv->uNewState & LVIS_SELECTED) && !(pnmv->uOldState & LVIS_SELECTED)) {
		//item selected
		if(pde->m_pProbDist && (pde->m_pProbDist->getProbabilityType()==HISTOGRAM || pde->m_pProbDist->getProbabilityType()==EMPIRICAL)) {\
			m_ToolBarParams.GetToolBarCtrl().EnableButton(ID_PDTOOLBARBTNDEL, TRUE);
		}
	}
	else if(!(pnmv->uNewState & LVIS_SELECTED) && (pnmv->uOldState & LVIS_SELECTED)) {
		//item unselected
		// TRACE("unselected param\n");
	}

	*pResult = 0;	
}

void CDlgProbDist::OnItemChangedListDistributions(NMHDR* pNMHDR, LRESULT* pResult)
{
	
	NM_LISTVIEW* pnmv = (NM_LISTVIEW*)pNMHDR;

	*pResult = 0;
	
	if ((pnmv->uChanged & LVIF_STATE) != LVIF_STATE)
		return;

	//// TRACE("LVN_ITEMCHANGING message handled - iItem: %d, iSubItem: %d\n", pnmv->iItem, pnmv->iSubItem);
	if(!(pnmv->uOldState & LVIS_SELECTED) && !(pnmv->uNewState & LVIS_SELECTED))
		return;

	

	if(pnmv->uOldState & LVIS_SELECTED) {
		if(!(pnmv->uNewState & LVIS_SELECTED)) {
			// TRACE("Unselected Item %d...\n", pnmv->iItem);

			CProbDistEntry* pde = GetSelectedPD();

			if(!pde->m_pProbDist) {
				//undefined pd
				
			}
			else if(!ProbDistHelper::HasParams(pde->m_pProbDist)) {
				SaveHistogramData(m_lstParameters, (HistogramDistribution*) pde->m_pProbDist);
				
				if(!ProbDistHelper::Is100Pct(pde->m_pProbDist) ) {
					// TRACE("!!!!!!!!!WARNING!!!!!!!!!!!\n");
				}
			}
		}
	}
	else if(pnmv->uNewState & LVIS_SELECTED) {
		//a new item has been selected
		// TRACE("Selected Item %d...\n", pnmv->iItem);
		m_nSelectedPD = (int) m_lstDistributions.GetItemData(pnmv->iItem);
	}
	else
		ASSERT(0);


	GetDlgItem(IDOK)->EnableWindow(TRUE);
	if(m_lstDistributions.GetSelectedCount() > 0) {
		m_ToolBarDist.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONDEL,TRUE);
		m_ToolBarDist.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONCOPY,TRUE);
	} 
	else {
		m_ToolBarDist.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONDEL,FALSE);
		m_ToolBarDist.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONCOPY,FALSE);
		if(m_bNeedReturn)
			GetDlgItem(IDOK)->EnableWindow(FALSE);
	}

	UpdateDistributionData();

}

void CDlgProbDist::OnItemChangingListDistributions(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_LISTVIEW* pnmv = (NM_LISTVIEW*)pNMHDR;

	*pResult = 0;
	
	if ((pnmv->uChanged & LVIF_STATE) != LVIF_STATE)
		return; 

	if((pnmv->uOldState & LVIS_SELECTED) && !(pnmv->uNewState & LVIS_SELECTED)) {
		// TRACE("Unselecting Item %d...\n", pnmv->iItem);
	}
	else if((pnmv->uNewState & LVIS_SELECTED) && !(pnmv->uOldState & LVIS_SELECTED)) {
		// TRACE("Selecting Item %d...\n", pnmv->iItem);
	}
}

void CDlgProbDist::OnDistToolbarbuttonDEL()
{
	ASSERT(m_nSelectedPD!=-1);

	POSITION pos = m_lstDistributions.GetFirstSelectedItemPosition();
	int nItem = m_lstDistributions.GetNextSelectedItem(pos);

	int idxPD = (int) m_lstDistributions.GetItemData(nItem);
	CProbDistEntry* pde = m_pProbDistMan->getItem(idxPD);

	m_lstDistributions.DeleteItem(nItem);

	m_pProbDistMan->DeleteItem(idxPD);

	delete pde;

	m_nSelectedPD = -1;

	int sbpos = m_lstDistributions.GetScrollPos(SB_VERT);
	LoadDistributionList();
	if(m_nSortColumn!=-1)
		SortByColumn(m_nSortColumn);
	
	if(m_lstDistributions.GetItemCount()>0) {
		CRect rc;
		m_lstDistributions.GetItemRect(0,&rc,LVIR_LABEL);
		m_lstDistributions.Scroll(CSize(0,sbpos*rc.Height()));
	}
     m_IsEdit = TRUE ;
	UpdateDistributionData();
}

void CDlgProbDist::OnParamToolbarbuttonDEL()
{
	ASSERT(m_nSelectedPD!=-1);
	CProbDistEntry* pde = GetSelectedPD();
	ASSERT(pde);
	POSITION pos = m_lstParameters.GetFirstSelectedItemPosition();
	int paramidx = m_lstParameters.GetNextSelectedItem(pos);
	ASSERT(paramidx != -1);
	ASSERT(!ProbDistHelper::HasParams(pde->m_pProbDist)); //must be HIST or EMP dist
	m_lstParameters.DeleteItem(paramidx);
	UpdateExtraParam(pde);
	m_IsEdit = TRUE ;
}

void CDlgProbDist::OnDistToolbarbuttonADD()
{
	// add new item into database
	CString csNew = m_pProbDistMan->GetNewName();

	CProbDistEntry* pPDEntry = new CProbDistEntry( csNew, NULL );
	m_pProbDistMan->AddItem( pPDEntry );

	// add new item into name list box
	int idx = m_pProbDistMan->getCount()-1;
	m_lstDistributions.InsertItem(LVIF_TEXT | LVIF_PARAM, idx, LPSTR_TEXTCALLBACK, 0, 0, 0, (LPARAM) idx);
	m_lstDistributions.SetItemState(idx, LVIS_SELECTED, LVIS_SELECTED);
	m_lstDistributions.SetFocus();
	m_lstDistributions.EditLabel(idx);
	m_IsEdit = TRUE ;
}

void CDlgProbDist::OnParamToolbarbuttonADD()
{
	
	ASSERT(m_nSelectedPD != - 1);
	CProbDistEntry* pde = GetSelectedPD();
	ASSERT(pde->m_pProbDist && 
		((pde->m_pProbDist->getProbabilityType()==EMPIRICAL) ||
	 	 (pde->m_pProbDist->getProbabilityType()==HISTOGRAM)));

	int idx = m_lstParameters.GetItemCount();
	m_lstParameters.InsertItem(LVIF_TEXT, idx, "      ", 0, 0, 0, NULL);
	m_lstParameters.SetItemText(idx,1,"0.0");
	m_lstParameters.SetFocus();
	EditSubItem(idx,0);
	m_IsEdit = TRUE ;
}

void CDlgProbDist::OnDblClickListParameters(NMHDR* pNMHDR, LRESULT* pResult)
{
	CPoint ptMsg = GetMessagePos();
	this->ScreenToClient( &ptMsg );
	NMLISTVIEW* pNMLW = (NMLISTVIEW*) pNMHDR;
	int nItem = pNMLW->iItem;
	CProbDistEntry* pde = GetSelectedPD();
	if(nItem != -1 && pde) 
	{
		int nSubItem = pNMLW->iSubItem;
		if(ProbDistHelper::HasParams(pde->m_pProbDist)) { //not histogram or empirical
			if(nSubItem == 1)  { //param value
				//in-place edit
				EditSubItem(nItem, nSubItem);
			}
		}
		else {
			EditSubItem(nItem, nSubItem);
			switch(nSubItem) {
			case 0:
				break;
			case 1:
				break;
			}
		}
	}
}

CWnd* CDlgProbDist::EditSubItem(int nItem, int nSubItem)
{
	CRect rcSubItem;
	m_lstParameters.GetSubItemRect( nItem, nSubItem, LVIR_LABEL, rcSubItem );

	CEdit *pEdit = new CInPlaceEdit(nItem, nSubItem, m_lstParameters.GetItemText(nItem, nSubItem), IPEDIT_TYPE_NUMBER2);
	pEdit->Create( WS_BORDER|WS_CHILD|WS_VISIBLE|ES_AUTOHSCROLL, rcSubItem, &m_lstParameters, IDC_IPEDIT );
	pEdit->BringWindowToTop();
	pEdit->SetFocus();
	m_IsEdit = TRUE ;
	return pEdit;

	/*
	static char buf[32];

	CRect rcSubItem, rcEdit;
	m_lstParameters.GetSubItemRect( nItem, nSubItem, LVIR_LABEL, rcSubItem );
	m_lstParameters.GetItemText(nItem, nSubItem, buf,32);

	CCoolTreeEditSpin* pCTES=new CCoolTreeEditSpin(this);
	pCTES->Create(IDD_COOLTREE_EDITSPIN,this);
	pCTES->SetParent(this);
	pCTES->GetWindowRect(rcEdit);
	pCTES->MoveWindow(rcSubItem.left, rcSubItem.top, rcEdit.Width(), rcEdit.Height());
	pCTES->m_editValue.SetWindowText( buf );
	pCTES->m_editValue.SetSel(0,-1);
	pCTES->m_editValue.SetLimitText(10);
	pCTES->m_spinValue.SetRange32(0,200);
	pCTES->BringWindowToTop();
	pCTES->SetFocus();
	return pCTES;
*/
}

void CDlgProbDist::OnDblClickListDistributions(NMHDR* pNMHDR, LRESULT* pResult)
{
	CPoint ptMsg = GetMessagePos();
	this->ScreenToClient( &ptMsg );
	NMLISTVIEW* pNMLW = (NMLISTVIEW*) pNMHDR;
	int nItem = pNMLW->iItem;
	CProbDistEntry* pItem = GetSelectedPD();
	if(nItem != -1) 
	{
		ASSERT(pItem);
		if(pNMLW->iSubItem == 0) {
			m_lstDistributions.EditLabel(nItem);
		}
		else if(pNMLW->iSubItem == 1) {
			// pd type
			CRect rcPDType;
			//CRect rcList;
			m_lstDistributions.GetSubItemRect( pNMLW->iItem, pNMLW->iSubItem, LVIR_LABEL, rcPDType);

			int nHeight = rcPDType.Height();
			rcPDType.bottom += nHeight * 10;
		
			m_nPDTypeSelItem = nItem;
			if (!m_distTypeInPlaceComboBox.GetSafeHwnd())
			{
				DWORD dwStyle = WS_BORDER|WS_CHILD|WS_VISIBLE|WS_VSCROLL|CBS_DROPDOWNLIST|CBS_DISABLENOSCROLL;
				
				m_distTypeInPlaceComboBox.Create( dwStyle, rcPDType, /*this*/&m_lstDistributions, IDC_COMBO_LEVEL );
				m_distTypeInPlaceComboBox.SetItemHeight( -1, nHeight );
				m_distTypeInPlaceComboBox.SetHorizontalExtent( 300 );
				COMBOBOXEXITEM cbItem;
				cbItem.mask = CBEIF_TEXT | CBEIF_LPARAM;
				for(int i=0; i<13; i++) {
					cbItem.iItem = i;
					cbItem.iImage = i;
					cbItem.iSelectedImage = i;
					//lstrcpy(cbItem.pszText, szPDTypeLabel[i]);
					cbItem.pszText = szPDTypeLabel[i];
					cbItem.lParam = (LPARAM) nPDTypeLParam[i];
					m_distTypeInPlaceComboBox.InsertItem( &cbItem );
				}
			}
			else
			{
				m_distTypeInPlaceComboBox.MoveWindow(&rcPDType);
			}

			if(pItem->m_pProbDist)
				m_distTypeInPlaceComboBox.SetCurSel( nPDTypeTable[(int)pItem->m_pProbDist->getProbabilityType()] );
			else
				m_distTypeInPlaceComboBox.SetCurSel( 0 );
			m_distTypeInPlaceComboBox.ShowWindow(SW_SHOW);
			m_distTypeInPlaceComboBox.BringWindowToTop();
			m_distTypeInPlaceComboBox.SetFocus();
			m_IsEdit = TRUE ;
		}
		Invalidate();
	}
	*pResult = 0;

}

void CDlgProbDist::DoResize(int cx, int cy)
{
	CRect r, r2, r3, r4;
	static const int nSplitterWidth = 9;
	static const int nSmallClearance = 4;
	static const int nLargeClearance = 10;
	static const int nGroupToToolbarHeight = 6;
	int nWidth = (cx - (3*nSplitterWidth)) / 2;
	int nHeight = (cy - (3*nSplitterWidth)) / 2;

	//m_staticDistGroup
	m_staticDistGroup.MoveWindow( nSplitterWidth, nSplitterWidth, nWidth, nHeight, TRUE);
	m_staticDistGroup.GetWindowRect(&r);
	ScreenToClient(&r);

		//m_staticDistGroup2
		m_staticDistGroup2.MoveWindow(r.left+nSmallClearance, r.top+nLargeClearance, r.Width()-2*nSmallClearance, r.Height()-(nSmallClearance+nLargeClearance), TRUE);
		m_staticDistGroup2.GetWindowRect(&r2);
		ScreenToClient(&r2);

		//m_ToolBarDist
		m_ToolBarDist.GetWindowRect(&r3);
		ScreenToClient(&r3);
		m_ToolBarDist.MoveWindow(r2.left+nSmallClearance, r2.top+nGroupToToolbarHeight, r3.Width(), r3.Height(), TRUE);

		//m_lstDistributions
		m_lstDistributions.MoveWindow(r2.left, r2.top+nGroupToToolbarHeight+r3.Height()+nSmallClearance, r2.Width(), r2.Height()-(nGroupToToolbarHeight+r3.Height()+nSmallClearance), TRUE);

	//m_wndSplitter1
	if(!m_wndSplitter1.m_hWnd) {
		m_wndSplitter1.Create(WS_CHILD | WS_VISIBLE, CRect(0,0,0,0), this, ID_WND_SPLITTER1);
		m_wndSplitter1.SetStyle(SPS_VERTICAL);
	}
	m_wndSplitter1.MoveWindow( r.right,r.top,nSplitterWidth,nHeight,TRUE);
	m_wndSplitter1.SetRange(100,cx-100);
	m_wndSplitter1.GetWindowRect(&r);
	ScreenToClient(&r);

	//m_staticParamGroup
	m_staticParamGroup.MoveWindow(r.right,r.top,nWidth,nHeight,TRUE);
	m_staticParamGroup.GetWindowRect(&r);
	ScreenToClient(&r);

		int nGrpHeight = r.Height()-(nSmallClearance+nLargeClearance);
		if(m_editExtraParam.IsWindowVisible()) {
			m_editExtraParam.GetWindowRect(&r4);
			ScreenToClient(&r4);			
			nGrpHeight -= (r4.Height()+2*nSmallClearance);
		}

		//m_staticParamGroup2
		m_staticParamGroup2.MoveWindow( r.left+nSmallClearance, r.top+nLargeClearance, r.Width()-2*nSmallClearance, nGrpHeight, TRUE);
		m_staticParamGroup2.GetWindowRect(&r2);
		ScreenToClient(&r2);

		//m_ToolBarParams
		m_ToolBarParams.GetWindowRect(&r3);
		ScreenToClient(&r3);
		m_ToolBarParams.MoveWindow( r2.left+nSmallClearance, r2.top+nGroupToToolbarHeight, r3.Width(), r3.Height(), TRUE);


		int nLstHeight = r2.Height()-(nGroupToToolbarHeight+r3.Height()+nSmallClearance);
		//m_lstParameters
		m_lstParameters.MoveWindow( r2.left, r2.top+nGroupToToolbarHeight+r3.Height()+nSmallClearance, r2.Width(), nLstHeight, TRUE);

		//m_staticExtraParamName
		m_lstParameters.GetWindowRect(&r2);
		ScreenToClient(&r2);
		m_staticExtraParamName.MoveWindow( r2.left, r2.bottom+nLargeClearance, r2.Width()/2, 13, TRUE);
		
		//m_editExtraParam
		m_editExtraParam.MoveWindow( r2.left + (r2.Width()/2), r2.bottom+nSmallClearance, r2.Width()/2, 23, TRUE);

	//m_wndSplitter2
	m_staticDistGroup.GetWindowRect(&r);
	ScreenToClient(&r);
	if(!m_wndSplitter2.m_hWnd) {
		m_wndSplitter2.Create(WS_CHILD | WS_VISIBLE, CRect(0,0,0,0), this, ID_WND_SPLITTER2);
		m_wndSplitter2.SetStyle(SPS_HORIZONTAL);
	}
	m_wndSplitter2.MoveWindow( r.left,r.bottom,nWidth*2 + nSplitterWidth,nSplitterWidth,TRUE);
	m_wndSplitter2.SetRange(100,cy-100);
	m_wndSplitter2.GetWindowRect(&r);
	ScreenToClient(&r);

	//
	CWnd* cancelbtn = GetDlgItem(IDCANCEL);
	cancelbtn->GetWindowRect(&r2);
	ScreenToClient(&r2);
	cancelbtn->MoveWindow( cx-10-r2.Width(), cy-10-r2.Height(), r2.Width(), r2.Height(), TRUE);
	int width = r2.Width() ;
	CWnd* okbtn = GetDlgItem(IDOK);
	okbtn->GetWindowRect(&r2);
	ScreenToClient(&r2);
	okbtn->MoveWindow( cx - 35 -  r2.Width() - width,cy-10-r2.Height(), r2.Width(), r2.Height(), TRUE);

	m_staticProbDensityFunc.MoveWindow(r.left,r.bottom,nWidth*2 + nSplitterWidth, nHeight-(r2.Height()+2*nSmallClearance) ,TRUE);
	m_staticProbDensityFunc.GetWindowRect(&r);
	ScreenToClient(&r);


	r.DeflateRect(nLargeClearance, nLargeClearance+nSmallClearance, nLargeClearance, nLargeClearance);

	m_cmbGraphType.GetWindowRect(&r3);
	ScreenToClient(&r3);
	m_cmbGraphType.MoveWindow(r.left, r.top, r.Width(), r3.Height(), TRUE);

	r.top += (r3.Height()+nSmallClearance);

	m_chart.MoveWindow(r.left,r.top,r.Width(),r.Height(),TRUE);
	m_chart.BringWindowToTop();




	m_button_loadDefault.GetWindowRect(&r2) ;
	ScreenToClient(&r2);
	m_button_loadDefault.MoveWindow(cx - 50 -  r2.Width() - 2*width,cy-10-r2.Height(), r2.Width(), r2.Height(), TRUE) ;
	width = r2.Width() ;

//	m_StaticLoad.GetWindowRect(&r2) ;
//	m_StaticLoad.MoveWindow(r.left + width + 5,cy-10-r2.Height(), r2.Width(), r2.Height(), TRUE) ;
//	width = width + r2.Width();

	/*
	CVcPlot plot = m_chart.GetPlot();
	CVcRect vcr =  plot.GetLocationRect();
	CVcCoor maxCoord = vcr.GetMax();
	float x = maxCoord.GetX();
	float y = maxCoord.GetY();
	//maxCoord.SetX(r.Width());
	//maxCoord.SetY(r.Height());
	CVcCoor minCoord = vcr.GetMin();
	x = minCoord.GetX();
	y = minCoord.GetY();
	//minCoord.SetX(0.0);
	//minCoord.SetY(0.0);
*/



	Invalidate(FALSE);
}

void CDlgProbDist::OnOK() 
{
	
	//if selected pd is HIST or EMP, save it
	if(m_nSelectedPD!=-1 && GetSelectedPD()->m_pProbDist && !ProbDistHelper::HasParams(GetSelectedPD()->m_pProbDist))
	{
		SaveHistogramData(m_lstParameters, (HistogramDistribution*) GetSelectedPD()->m_pProbDist);
	}

	//scan the pd list
	//if any undefined pds are found, warn user and give option to cancel or continue and delete all undefined pds
	int nCount = m_pProbDistMan->getCount();
	std::vector<int> badIdxVector;
	std::vector<int> not100Vector;
	std::vector<int> notValidHistVector;
	for(int i=0; i<nCount; i++)
	{
		CProbDistEntry* pde = m_pProbDistMan->getItem(i);
		if(!pde->m_pProbDist)
		{
			badIdxVector.push_back(i);
		}
		else if(!ProbDistHelper::HasParams( pde->m_pProbDist ) && !ProbDistHelper::Is100Pct( pde->m_pProbDist ))
		{
			not100Vector.push_back(i);
		}
		else if(!ProbDistHelper::HasParams( pde->m_pProbDist ) && !ProbDistHelper::IsValidHistogram( pde->m_pProbDist ))
		{
			notValidHistVector.push_back(i);
		}	
	}
	int nNot100 = not100Vector.size();
	if(nNot100 > 0)
	{
		CString sMsg = "Warning! The following histogram or empirical distributions have errors:\n";
		for(i=0; i<nNot100; i++)
		{
			sMsg += m_pProbDistMan->getItem(not100Vector[i])->m_csName + "\n";
		}
		sMsg += "The percentages do not total 100%.\n"
			"Click OK to return to the probability distribution editor.\n";
		AfxMessageBox(sMsg);
		return;
	}

	int nNotValidHist = notValidHistVector.size();
	if(nNotValidHist > 0) 
	{
		CString sMsg = "Warning! The following histogram distributions have errors:\n";
		for(i=0; i<nNotValidHist; i++)
		{
			sMsg += m_pProbDistMan->getItem(notValidHistVector[i])->m_csName + "\n";
		}
		sMsg += "The last percentage should be 0.\n"
			"Click OK to continue with these errors. You will likely get undesired results.\n"
			"Click CANCEL to return to the probability distribution editor and make changes.\n";

		if(AfxMessageBox(sMsg, MB_OKCANCEL)==IDCANCEL)
			return;
	}

	int nBadSize = badIdxVector.size();
	if(nBadSize > 0) 
	{
		CString sMsg = "Warning! The following probability distributions are undefined:\n";
		for(i=0; i<nBadSize; i++) 
		{
			sMsg += m_pProbDistMan->getItem(badIdxVector[i])->m_csName + "\n";
		}
		sMsg += "Click OK to continue.  All undefined probability distributions will be lost.\n"
				"Click CANCEL to return to the probability distribution editor.";
		if(AfxMessageBox(sMsg, MB_OKCANCEL)==IDOK)
		{
			for(i=nBadSize-1; i>=0; i--)
			{
				m_pProbDistMan->DeleteItem(badIdxVector[i]);
			}
		}
		else 
		{
			return;
		}
	}
	if(m_IsEdit)
	{
           //save local first 
		try
		{
			CDatabaseADOConnetion::BeginTransaction(m_pAirportDB->GetAirportConnection()) ;
			if(m_Load_Oper)
			{
				m_pProbDistMan->deleteDatabase(m_pAirportDB) ;
				m_pProbDistMan->ResetAllID() ;
				m_pProbDistMan->saveDatabase(m_pAirportDB) ;
			}else
				m_pProbDistMan->saveDatabase( m_pAirportDB);
			CDatabaseADOConnetion::CommitTransaction(m_pAirportDB->GetAirportConnection()) ;
		}
		catch (CADOException& e)
		{
			e.ErrorMessage() ;
			CDatabaseADOConnetion::RollBackTransation(m_pAirportDB->GetAirportConnection()) ;
		}
	}
    if(m_IsEdit == FALSE && m_Load_Oper)
	{
		try
		{
			CDatabaseADOConnetion::BeginTransaction(m_pAirportDB->GetAirportConnection()) ;
			m_pProbDistMan->deleteDatabase(m_pAirportDB) ;
			m_pProbDistMan->ResetAllID() ;
			m_pProbDistMan->saveDatabase(m_pAirportDB) ;
			CDatabaseADOConnetion::CommitTransaction(m_pAirportDB->GetAirportConnection()) ;
		}
		catch (CADOException& e)
		{
			e.ErrorMessage() ;
			CDatabaseADOConnetion::RollBackTransation(m_pAirportDB->GetAirportConnection()) ;
		}
	}
	CDialog::OnOK();
}

CProbDistEntry* CDlgProbDist::GetSelectedPD()
{
	ASSERT(m_nSelectedPD>=0 && m_nSelectedPD<static_cast<int>(m_pProbDistMan->getCount()));
	if (m_nSelectedPD<0 || m_nSelectedPD > static_cast<int>(m_pProbDistMan->getCount()-1))
		return NULL;

	return m_pProbDistMan->getItem(m_nSelectedPD);
}

void CDlgProbDist::OnChangeGraphType()
{
	if(m_nSelectedPD != -1)
		UpdateChart(GetSelectedPD());
}

void CDlgProbDist::OnParamToolbarbuttonFF()
{
	OpenAndReadFile();
	m_IsEdit = TRUE ;
}

void CDlgProbDist::OnParamToolbarbuttonRAWFF()
{
}

/*
void CDlgProbDist::setProbDistMan( CProbDistManager* _pProbDistMan )
{
	assert( _pProbDistMan );
	m_pProbDistMan = _pProbDistMan;
}
void CDlgProbDist::setAirportDatabase(CAirportDatabase* _pAirportDB )
{
	assert(_pAirportDB);
	m_pAirportDB = _pAirportDB;
}*/

void CDlgProbDist::OpenAndReadFile()
{

	CProbDistEntry* pde = GetSelectedPD();

	UINT nType = 0;
	if (pde->m_pProbDist->getProbabilityType() == HISTOGRAM)
		nType = 0;
	else if(pde->m_pProbDist->getProbabilityType() == EMPIRICAL)
		nType = 1;

	if(nType == 0 && m_lstParameters.GetItemCount() < 2)
	{
		AfxMessageBox("Please add two Samples at least!");
		return ;
	}
	
	m_vdbValue.clear();
	m_vPairRange.clear();

	if(nType == 0)
	{
		std::vector<double> vect;
		int i = 0;
		for(i = 0; i < m_lstParameters.GetItemCount(); i++ )
		{
			CString str = m_lstParameters.GetItemText(i, 0);
			vect.push_back( ::atof( str.GetBuffer(0) ) );
		}
		std::sort( vect.begin(), vect.end() );
		for(i = 0; i < static_cast<int>(vect.size()); i++)
		{
			//TRACE("%f-", vect[i]);
		}
		for(i = 0; i < static_cast<int>(vect.size())-1; i++)
		{
			pair_range range;
			range.first = vect[i];
			range.second = vect[i+1];
			m_vPairRange.push_back(range);
		}
	}
	
	
	CDlgProbDistHistData dlg;
	dlg.SetType(nType);
	dlg.SetRangeVectorPtr(&m_vPairRange);
	dlg.SetValueVectorPtr(&m_vdbValue);


	if(dlg.DoModal() != IDOK)
		return ;

	if(m_vdbValue.size() <= 0)
	{
		AfxMessageBox("No data in the file!");
		return ;
	}
	
	if(nType == 0)
	{
		HistogramDistribution* pHist = (HistogramDistribution*) GetSelectedPD()->m_pProbDist;
		if(!pHist->InitFromVector(&m_vdbValue, &m_vPairRange))
		{
			AfxMessageBox("Invalid data in the file!");
			return ;
		}
	}
	else
	{
		EmpiricalDistribution* pHist = (EmpiricalDistribution*) GetSelectedPD()->m_pProbDist;
		if(!pHist->InitFromVector(&m_vdbValue, &m_vPairRange))
		{
			AfxMessageBox("Invalid data in the file!");
			return ;
		}
	}

	UpdateDistributionData();
}

BOOL CDlgProbDist::OnToolTipText(UINT id, NMHDR *pNMHDR, LRESULT *pResult)
{
	TOOLTIPTEXTA* pTTTA = (TOOLTIPTEXTA*)pNMHDR;
	CString strTipText;
	CString strStatusText;
	UINT nID = pNMHDR->idFrom;
	if (pNMHDR->code == TTN_NEEDTEXTA && (pTTTA->uFlags & TTF_IDISHWND))
	{
		nID = ::GetDlgCtrlID((HWND)nID);
	}
	
	if (nID != 0)
	{
		strTipText.LoadString(nID);
		int len =strTipText.Find('\n',0);
		strStatusText = strTipText.Left(len);
		strTipText = strTipText.Mid(len+1);

		if (pNMHDR->code == TTN_NEEDTEXTA)
		{
			lstrcpyn(pTTTA->szText, strTipText, sizeof(pTTTA->szText));
		}
		*pResult = 0; 
		::SetWindowPos(pNMHDR->hwndFrom, HWND_TOP, 0, 0, 0, 0,SWP_NOACTIVATE|
			SWP_NOSIZE|SWP_NOMOVE|SWP_NOOWNERZORDER); 
		return TRUE;
	}
	return TRUE;
}

void CDlgProbDist::OnClickMultiBtn()
{
	int type = m_button_loadDefault.GetOperation() ;
	if (type == 0)
		OnLoadFromTemplate();
	if(type == 1)
		OnSaveAsTemplate();
}

void CDlgProbDist::OnLoadFromTemplate()
{
	m_button_loadDefault.SetOperation(0);
	m_button_loadDefault.SetWindowText(_T("Load from Template"));
	OnBnClickedButtonLoaddefault();
}

void CDlgProbDist::OnSaveAsTemplate()
{
	m_button_loadDefault.SetOperation(1);
	m_button_loadDefault.SetWindowText(_T("Save as Template"));

	CAirportDatabase* DefaultDB = AIRPORTDBLIST->getAirportDBByName(m_pAirportDB->getName());
	if(DefaultDB == NULL)
	{
		CString strError(_T(""));
		strError.Format(_T("%s AirportDB is not Exist in AirportDB template."),m_pAirportDB->getName());
		MessageBox(strError,"Warning",MB_OK);
		return ;
	}
	//read from local 
	m_pAirportDB->saveAsTemplateDatabase(DefaultDB);
}

BOOL CDlgProbDist::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	if( ::IsWindow(m_toolTipCtrl.m_hWnd) && pMsg->hwnd == m_hWnd)
	{
		switch(pMsg->message)
		{
		case WM_LBUTTONDOWN:
		case WM_MOUSEMOVE:
		case WM_LBUTTONUP:
		case WM_RBUTTONDOWN:
		case WM_MBUTTONDOWN:
		case WM_RBUTTONUP:
		case WM_MBUTTONUP:
			m_toolTipCtrl.RelayEvent(pMsg);
			break;
		default: 
			break;
		}
	}	
	
	return CDialog::PreTranslateMessage(pMsg);
}

void CDlgProbDist::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	if( ::IsWindow(m_toolTipCtrl.m_hWnd) )
	{
//		int nIndex = GetRectIndex( point );	
//		if( nIndex != m_nCurToolTipsIndex )
//			m_toolTipCtrl.Pop();
//		m_nCurToolTipsIndex = nIndex;
	}

	
	CDialog::OnMouseMove(nFlags, point);
}

void CDlgProbDist::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	OnOK();
}

void CDlgProbDist::OnCancel()
{

	if(m_IsEdit || m_Load_Oper)
       m_pProbDistMan->loadDatabase(m_pAirportDB) ;
	CDialog::OnCancel();
}

void CDlgProbDist::OnBnClickedButtonLoaddefault()
{
	// TODO: Add your control notification handler code here
	if(m_pAirportDB == NULL || m_pProbDistMan == NULL)
		return ;
	CAirportDatabase* DefaultDB = AIRPORTDBLIST->getAirportDBByName(m_pAirportDB->getName());
	if(DefaultDB == NULL)
	{
		CString msg ;
		msg.Format("%s AirportDB is not exist in AirportDB template.",m_pAirportDB->getName()) ;
		MessageBox(msg,"Warning",MB_OK) ;
		return ;
	}
	m_pProbDistMan->loadDatabase(DefaultDB) ;
	CString msg ;
    msg.Format("Load %s from %s AirportDB template successfully.",DefaultDB->getName(),DefaultDB->getName()) ;
	 MessageBox(msg,NULL,MB_OK) ;
    m_Load_Oper = TRUE ;
	m_IsEdit = FALSE ;
	LoadDistributionList();
	UpdateDistributionData();
}

void CDlgProbDist::OnDistToolbarbuttonCopy()
{
	m_ToolBarDist.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONPASTE,TRUE);
}

void CDlgProbDist::OnDistToolbarbuttonPaste()
{
	int i=0;
	i=0;
	return;
}

CDlgProbDistForDefaultDB::CDlgProbDistForDefaultDB(CAirportDatabase* _pAirportDB, BOOL bNeedRet, CAirportDatabase* _globalDB , CWnd* pParent /* = NULL */):CDlgProbDist(_pAirportDB,bNeedRet,pParent),m_GlobalDB(_globalDB
																																																				 )
{

}
CDlgProbDistForDefaultDB::~CDlgProbDistForDefaultDB()
{

}
void CDlgProbDistForDefaultDB::OnBnClickedOk()
{
	try
	{
		CDatabaseADOConnetion::BeginTransaction(m_GlobalDB->GetAirportConnection()) ;
		m_pProbDistMan->saveDatabase(m_GlobalDB) ;
		CDatabaseADOConnetion::CommitTransaction(m_GlobalDB->GetAirportConnection()) ;
	}
	catch (CADOException& e)
	{
		e.ErrorMessage() ;
		CDatabaseADOConnetion::RollBackTransation(m_GlobalDB->GetAirportConnection()) ;
	}
	
	CDialog::OnOK();
}
void CDlgProbDistForDefaultDB::OnCancel()
{
	m_pProbDistMan->loadDatabase(m_GlobalDB) ;
	CDialog::OnCancel() ;
}
BOOL CDlgProbDistForDefaultDB::OnInitDialog()
{
	CDlgProbDist::OnInitDialog() ;
	//m_ToolBarParams.ShowWindow(SW_HIDE);
 //   m_ToolBarDist.ShowWindow(SW_HIDE) ;
	m_button_loadDefault.ShowWindow(SW_HIDE);
//	m_StaticLoad.ShowWindow(SW_HIDE);
	SetWindowText("Global ProbDist");
	return TRUE;
}