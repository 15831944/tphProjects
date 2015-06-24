// ProbDistDlg.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "ProbDistDlg.h"
#include "inputs\Flt_DB.h"
#include "inputs\Pax_DB.h"
#include "inputs\Probab.h"
#include "FlightDialog.h"
#include "PassengerTypeDialog.h"
#include "inputs\in_term.h"
#include "inputs\fltdata.h"
#include "TermPlanDoc.h"
#include "..\common\probdistmanager.h"
#include "common\WinMsg.h"
#include <Common/ProbabilityDistribution.h>
#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define CONSTRAINT_LEN 50

/////////////////////////////////////////////////////////////////////////////
// CProbDistDlg dialog


CProbDistDlg::CProbDistDlg( bool _bNeedReturn, CWnd* pParent /*= NULL*/ )
	: CDialog(CProbDistDlg::IDD, pParent)
{

	m_bNeedReturn = _bNeedReturn;	
	
	//{{AFX_DATA_INIT(CProbDistDlg)
	m_dFirstVal = 0.0;
	m_dForthVal = 0.0;
	m_dSecondVal = 0.0;
	m_dThirdVal = 0.0;
	//}}AFX_DATA_INIT
	m_bCodeSelTree = false;
	m_nIdxChanged = -1;
	m_nLastSelection = -1;
}


CProbDistDlg::~CProbDistDlg()
{
}

void CProbDistDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CProbDistDlg)
	DDX_Control(pDX, IDC_STATIC_GROUP, m_staticGroup);
	DDX_Control(pDX, IDC_TOOLBARCONTENTER1, m_toolbarcontenter1);
	DDX_Control(pDX, IDC_PIC, m_staPic);
	DDX_Control(pDX, IDC_STATIC_NOTE, m_staticNote);
	DDX_Control(pDX, IDCANCEL, m_btnCancel);
	DDX_Control(pDX, IDC_STATIC_TOTALVAL, m_staticTotalVal);
	DDX_Control(pDX, IDC_EDIT_NAME, m_editName);
	DDX_Control(pDX, IDC_BUTTON_SAVE, m_btnSave);
	DDX_Control(pDX, IDC_BUTTON_DELETE, m_btnDel);
	DDX_Control(pDX, IDC_STATIC_TOTAL, m_staticTotal);
	DDX_Control(pDX, IDC_STATIC_THIRDVAL, m_staticThirdVal);
	DDX_Control(pDX, IDC_STATIC_SECONDVAL, m_staticSecondVal);
	DDX_Control(pDX, IDC_STATIC_FORTHVAL, m_staticForthVal);
	DDX_Control(pDX, IDC_STATIC_FIRSTVAL, m_staticFirstVal);
	DDX_Control(pDX, IDC_LIST_VALUE, m_listCtrlValue);
	DDX_Control(pDX, IDC_EDIT_THIRDVAL, m_editThirdVal);
	DDX_Control(pDX, IDC_EDIT_SECONDVAL, m_editSecondVal);
	DDX_Control(pDX, IDC_EDIT_FORTHVAL, m_editForthVal);
	DDX_Control(pDX, IDC_EDIT_FIRSTVAL, m_editFirstVal);
	DDX_Control(pDX, IDC_LIST_NAMELIST, m_listBoxName);
	DDX_Control(pDX, IDC_TREE_DISTRIBUTION, m_treeDist);
	DDX_Text(pDX, IDC_EDIT_FIRSTVAL, m_dFirstVal);
	DDX_Text(pDX, IDC_EDIT_FORTHVAL, m_dForthVal);
	DDX_Text(pDX, IDC_EDIT_SECONDVAL, m_dSecondVal);
	DDX_Text(pDX, IDC_EDIT_THIRDVAL, m_dThirdVal);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CProbDistDlg, CDialog)
	//{{AFX_MSG_MAP(CProbDistDlg)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_DISTRIBUTION, OnSelchangedTreeDistribution)
	ON_LBN_SELCHANGE(IDC_LIST_NAMELIST, OnSelchangeListNamelist)
	ON_BN_CLICKED(IDC_BUTTON_ADD, OnButtonAdd)
	ON_EN_CHANGE(IDC_EDIT_FIRSTVAL, OnChangeEdit)
	ON_BN_CLICKED(IDC_BUTTON_DELETE, OnButtonDelete)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnButtonSave)
	ON_COMMAND(ID_PEOPLEMOVER_DELETE, OnPeoplemoverDelete)
	ON_COMMAND(ID_PEOPLEMOVER_NEW, OnPeoplemoverNew)
	ON_EN_KILLFOCUS(IDC_EDIT_NAME, OnKillfocusEditName)
	ON_EN_KILLFOCUS(IDC_EDIT_FIRSTVAL, OnKillfocusEditVal)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_LIST_VALUE, OnEndlabeleditListValue)
	ON_WM_CREATE()
	ON_EN_CHANGE(IDC_EDIT_FORTHVAL, OnChangeEdit)
	ON_EN_CHANGE(IDC_EDIT_SECONDVAL, OnChangeEdit)
	ON_EN_CHANGE(IDC_EDIT_THIRDVAL, OnChangeEdit)
	ON_EN_CHANGE(IDC_EDIT_TOTAL, OnChangeEdit)
	ON_EN_CHANGE(IDC_EDIT_UNITS, OnChangeEdit)
	ON_EN_CHANGE(IDC_EDIT_NAME, OnChangeEdit)
	ON_EN_KILLFOCUS(IDC_EDIT_SECONDVAL, OnKillfocusEditVal)
	ON_EN_KILLFOCUS(IDC_EDIT_FORTHVAL, OnKillfocusEditVal)
	ON_EN_KILLFOCUS(IDC_EDIT_THIRDVAL, OnKillfocusEditVal)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_VALUE, OnItemchangedListValue)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProbDistDlg message handlers

BOOL CProbDistDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	InitToolBar();
	SetTree();
	SetListCtrl();


	if( m_bNeedReturn )
		m_btnCancel.EnableWindow( FALSE );
	ReloadDatabase( -1 );
	m_staticNote.ShowWindow( SW_HIDE );

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}





// Set the probdist tree label
void CProbDistDlg::SetTree()
{
	CBitmap bmL;
	m_ilNodes.Create(16,16,ILC_COLOR16 | ILC_MASK,0,1);
	bmL.LoadBitmap(IDB_SMALLNODEIMAGES);
	m_ilNodes.Add(&bmL, RGB(255,0,255));
	bmL.Detach();
	bmL.LoadBitmap(IDB_NODESMALLIMG);
	m_ilNodes.Add(&bmL, RGB(255,0,255));
	bmL.Detach();
	bmL.LoadBitmap(IDB_FLOORSMALLIMG);
	m_ilNodes.Add(&bmL, RGB(255,0,255));
	m_treeDist.SetImageList(&m_ilNodes, TVSIL_NORMAL);


	//populate the tree view with the pd types
	CString s;
	s.LoadString(IDS_TVNN_CONTINUOUSDISTS);
	HTREEITEM hItem = m_treeDist.InsertItem(s, TVI_ROOT, TVI_LAST);
	
	s.LoadString(IDS_TVNN_BERNOULLIDIST);
	HTREEITEM hThisItem = m_treeDist.InsertItem( s, hItem, TVI_LAST);
	m_treeDist.SetItemData( hThisItem, BERNOULLI );

	s.LoadString(IDS_TVNN_BETADIST);
	hThisItem = m_treeDist.InsertItem( s, hItem, TVI_LAST);
	m_treeDist.SetItemData( hThisItem, BETA );

	s.LoadString(IDS_TVNN_CONSTANTDIST);
	hThisItem = m_treeDist.InsertItem( s, hItem, TVI_LAST);
	m_treeDist.SetItemData( hThisItem, CONSTANT );

	s.LoadString(IDS_TVNN_EXPONDIST);
	hThisItem = m_treeDist.InsertItem( s, hItem, TVI_LAST);
	m_treeDist.SetItemData( hThisItem, EXPONENTIAL );

	s.LoadString(IDS_TVNN_NORMALDIST);
	hThisItem = m_treeDist.InsertItem( s, hItem, TVI_LAST);
	m_treeDist.SetItemData( hThisItem, NORMAL );

	s.LoadString(IDS_TVNN_UNIFORMDIST);
	hThisItem = m_treeDist.InsertItem( s, hItem, TVI_LAST);
	m_treeDist.SetItemData( hThisItem, UNIFORM );

	m_treeDist.Expand(hItem, TVE_EXPAND);

	s.LoadString(IDS_TVNN_DISCRETEDISTS);
	
	hItem = m_treeDist.InsertItem(s, TVI_ROOT, TVI_LAST);

	s.LoadString(IDS_TVNN_EMPDIST);
	hThisItem = m_treeDist.InsertItem( s, hItem, TVI_LAST);
	m_treeDist.SetItemData( hThisItem, EMPIRICAL );

	s.LoadString(IDS_TVNN_HISTDIST);
	hThisItem = m_treeDist.InsertItem( s, hItem, TVI_LAST);
	m_treeDist.SetItemData( hThisItem, HISTOGRAM );
	
	m_treeDist.Expand(hItem, TVE_EXPAND);
}


// init list control
void CProbDistDlg::SetListCtrl()
{
	// set list ctrl	
	DWORD dwStyle = m_listCtrlValue.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES;
	m_listCtrlValue.SetExtendedStyle( dwStyle );

	LV_COLUMNEX	lvc;
	lvc.mask = LVCF_WIDTH | LVCF_TEXT ;
	char* columnLabel[] = {	"Value", "Percent" };
	int DefaultColumnWidth[] = { 140, 140 };
	int nFormat[] = {	LVCFMT_CENTER, LVCFMT_CENTER };
	for( int i=0; i<2; i++ )
	{
		CStringList strList;
		lvc.csList = &strList;
		lvc.pszText = columnLabel[i];
		lvc.cx = DefaultColumnWidth[i];
		if( i == 1 )
			lvc.fmt = LVCFMT_SPINEDIT;
		else
			lvc.fmt = 0;
		m_listCtrlValue.InsertColumn( i, &lvc );
	}
}


// load glabal probdist db entry name list. use index as item data.
void CProbDistDlg::ReloadDatabase( int _nSelDBIdx )
{
	m_listBoxName.ResetContent();
	int nCount = _g_GetActiveProbMan( GetInputTerminal() )->getCount();
	for( int i=0; i<nCount; i++ )
	{
		CProbDistEntry* pProbDistItem = _g_GetActiveProbMan( GetInputTerminal() )->getItem( i );
		int nIdx = m_listBoxName.AddString( pProbDistItem->m_csName );
		m_listBoxName.SetItemData( nIdx, i );
		if( i == _nSelDBIdx )
			m_listBoxName.SetCurSel( i );
	}
	if( _nSelDBIdx == -1 )
		m_listBoxName.SetCurSel( -1 );
	ReloadContents();
}


void CProbDistDlg::ReloadContents()
{
	int nCurSel = m_listBoxName.GetCurSel();
	if( nCurSel == LB_ERR )
	{
		m_nLastSelection = -1;
		m_editName.SetWindowText( "" );
		m_treeDist.SelectItem( NULL );
		SetDistValue( NULL );
		return;
	}
	

	m_nLastSelection = m_listBoxName.GetItemData(nCurSel);
	CProbDistEntry* pPDEntry = _g_GetActiveProbMan( GetInputTerminal() )->getItem( m_nLastSelection );
	// set name
	m_editName.SetWindowText( pPDEntry->m_csName );
	m_bEditChanged = false; 

	// select tree item
	ProbabilityDistribution* pProbDist = pPDEntry->m_pProbDist;
	int nDistType = pProbDist->getProbabilityType();
	HTREEITEM hItemRoot = m_treeDist.GetRootItem();
	HTREEITEM hItem = m_treeDist.GetChildItem( hItemRoot );
	while( hItem )
	{
		if( m_treeDist.GetItemData( hItem ) == nDistType )
		{
			m_bCodeSelTree = true;
			m_treeDist.SelectItem( hItem );
			break;
		}
		hItem = m_treeDist.GetNextItem( hItem, TVGN_NEXT );
	}

	hItem = m_treeDist.GetNextItem( hItemRoot, TVGN_NEXT );
	hItem = m_treeDist.GetChildItem( hItem );
	while( hItem )
	{
		if( m_treeDist.GetItemData( hItem ) == nDistType )
		{
			m_bCodeSelTree = true;
			m_treeDist.SelectItem( hItem );
			break;
		}
		hItem = m_treeDist.GetNextItem( hItem, TVGN_NEXT );
	}


	// set value
	SetDistValue( pPDEntry->m_pProbDist );
}



void CProbDistDlg::OnButtonAdd() 
{
	// add new item into database
	CString csNew = _g_GetActiveProbMan( GetInputTerminal() )->GetNewName();
	ConstantDistribution* pProbDist = new ConstantDistribution();
	pProbDist->init( 0.0 );

	CProbDistEntry* pPDEntry = new CProbDistEntry( csNew, pProbDist );
	_g_GetActiveProbMan( GetInputTerminal() )->AddItem( pPDEntry );

	// add new item into name list box
	int nCount = _g_GetActiveProbMan( GetInputTerminal() )->getCount();
	ReloadDatabase( nCount-1 );
	m_btnSave.EnableWindow();
}


void CProbDistDlg::OnButtonDelete() 
{
	// TODO: Add your control notification handler code here
	int nItem = m_listBoxName.GetCurSel();
	if( nItem >= 0 )
	{
		int nIdx = m_listBoxName.GetItemData( nItem );
		_g_GetActiveProbMan( GetInputTerminal() )->DeleteItem( nIdx );
		ReloadDatabase( -1 );
	}	
}

void CProbDistDlg::ReCreateCurDistItem(int _nType, 	ProbabilityDistribution* _pCurDist )
{
	ProbabilityDistribution* pDist;

	switch( _nType )
	{
	case BERNOULLI:
		pDist = new BernoulliDistribution();
		if( _pCurDist )
			*(BernoulliDistribution*)pDist = *(BernoulliDistribution*)_pCurDist;
		break;
	case BETA:
		pDist = new BetaDistribution( 0, 0 );
		if( _pCurDist )
			*(BetaDistribution*)pDist = *(BetaDistribution*)_pCurDist;
		break;
	case CONSTANT:
		pDist = new ConstantDistribution( 0 );
		if( _pCurDist )
			*(ConstantDistribution*)pDist = *(ConstantDistribution*)_pCurDist;
		break;
	case EXPONENTIAL:
		pDist = new ExponentialDistribution( 0 );
		if( _pCurDist )
			*(ExponentialDistribution*)pDist = *(ExponentialDistribution*)_pCurDist;
		break;
	case NORMAL:
		pDist = new NormalDistribution( 0, 0 );
		if( _pCurDist )
		{
			((NormalDistribution*)pDist)->init( ((NormalDistribution*)_pCurDist)->getMean(),
				((NormalDistribution*)_pCurDist)->getStdDev(), ((NormalDistribution*)_pCurDist)->getTruncation() );
		}
		break;
	case UNIFORM:
		pDist = new UniformDistribution( 0, 0 );
		if( _pCurDist )
			*(UniformDistribution*)pDist = *(UniformDistribution*)_pCurDist;
		break;
	case EMPIRICAL:
		pDist = new EmpiricalDistribution();
		if( _pCurDist )
			*(EmpiricalDistribution*)pDist = *(EmpiricalDistribution*)_pCurDist;
		break;
	case HISTOGRAM:
		pDist = new HistogramDistribution();
		if( _pCurDist )
			*(HistogramDistribution*)pDist = *(HistogramDistribution*)_pCurDist;
		break;
	}

	int nIdx = GetSelDBIndex();
	ConstraintEntry *pEntry = m_pConDB->getItem( nIdx );
	pEntry->setValue( pDist );
}


ProbabilityDistribution* CProbDistDlg::GetDist( int _nIdx )
{
	if( _nIdx < 0 )
		return NULL;

	ConstraintEntry *pEntry = m_pConDB->getItem( _nIdx );
	return (ProbabilityDistribution *)pEntry->getValue();
}

InputTerminal* CProbDistDlg::GetInputTerminal()
{
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();	
    return (InputTerminal*)&pDoc->GetTerminal();
}


CString CProbDistDlg::GetProjPath()
{
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();	
	return pDoc->m_ProjInfo.path;
}


// set value and show / hide the control base on the dist type
void CProbDistDlg::SetDistValue(ProbabilityDistribution *_pProbDist)
{
	m_staticFirstVal.ShowWindow( false );
	m_editFirstVal.ShowWindow( false );
	m_staticSecondVal.ShowWindow( false );
	m_editSecondVal.ShowWindow( false );
	m_staticThirdVal.ShowWindow( false );
	m_editThirdVal.ShowWindow( false );
	m_staticForthVal.ShowWindow( false );
	m_editForthVal.ShowWindow( false );
	m_toolbarcontenter1.ShowWindow( false );
	m_ToolBar.ShowWindow(FALSE);
	m_listCtrlValue.ShowWindow( false );
	m_staticTotal.ShowWindow( false );
	m_staticTotalVal.ShowWindow( false );
	m_staticGroup.ShowWindow(false);

	if( _pProbDist == NULL )
		return;

	int nDistType = _pProbDist->getProbabilityType();

	switch( nDistType )
	{
	case BERNOULLI:
		{
			HBITMAP hBitmap = (HBITMAP) ::LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_NORCDF), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);
			if(hBitmap == NULL) 
			{
				CString s;
				s.Format(_T("Can not open Resource File"));
				AfxMessageBox(s);
				return ;
			}
			m_staPic.SetBitmap(hBitmap);
			double dVal1 = ((BernoulliDistribution*)_pProbDist)->getValue1();
			double dVal2 = ((BernoulliDistribution*)_pProbDist)->getValue2();
			double dProb = ((BernoulliDistribution*)_pProbDist)->getProb1();
			m_dFirstVal = dVal1;
			m_dSecondVal = dVal2;
			m_dThirdVal = dProb * 100.0;
			UpdateData( false );
			m_staticFirstVal.ShowWindow( true );
			m_staticFirstVal.SetWindowText("First Value :");
			m_editFirstVal.ShowWindow( true );
			m_staticSecondVal.ShowWindow( true );
			m_staticSecondVal.SetWindowText("Second Value :");
			m_editSecondVal.ShowWindow( true );
			m_staticThirdVal.ShowWindow( true );
			m_staticThirdVal.SetWindowText("First Prob(%) :");
			m_editThirdVal.ShowWindow( true );
			return;
			
		}
	case BETA:
		{
			HBITMAP hBitmap = (HBITMAP) ::LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_NORCHA), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);
			if(hBitmap == NULL) 
			{
				CString s;
				s.Format(_T("Can not open Resource File"));
				AfxMessageBox(s);
				return ;
			}
			m_staPic.SetBitmap(hBitmap);
			double dMin = ((BetaDistribution*)_pProbDist)->getMin();
			double dMax = ((BetaDistribution*)_pProbDist)->getMax();
			double dAlpha = ((BetaDistribution*)_pProbDist)->getAlpha();
			double dBeta = ((BetaDistribution*)_pProbDist)->getBeta();
			CString csVal;
			m_dFirstVal = dMin;
			m_dSecondVal = dMax;
			m_dThirdVal = dAlpha;
			m_dForthVal = dBeta;
			UpdateData( false );
			m_staticFirstVal.ShowWindow( true );
			m_staticFirstVal.SetWindowText("Minimum :");
			m_editFirstVal.ShowWindow( true );
			m_staticSecondVal.ShowWindow( true );
			m_staticSecondVal.SetWindowText("Maximum :");
			m_editSecondVal.ShowWindow( true );
			m_staticThirdVal.ShowWindow( true );
			m_staticThirdVal.SetWindowText("Alpha :");
			m_editThirdVal.ShowWindow( true );
			m_staticForthVal.ShowWindow( true );
			m_staticForthVal.SetWindowText("Beta :");
			m_editForthVal.ShowWindow( true );
			return;
		}
	case CONSTANT:
		{
			HBITMAP hBitmap = (HBITMAP) ::LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_NORCDF), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);
			if(hBitmap == NULL) 
			{
				CString s;
				s.Format(_T("Can not open Resource File"));
				AfxMessageBox(s);
				return ;
			}
			m_staPic.SetBitmap(hBitmap);
			double dMean = ((ConstantDistribution*)_pProbDist)->getMean();
			m_dFirstVal = dMean;
			UpdateData( false );
			m_staticFirstVal.ShowWindow( true );
			m_staticFirstVal.SetWindowText("Value :");
			m_editFirstVal.ShowWindow( true );
			return;
		}
	case EXPONENTIAL:
		{
			HBITMAP hBitmap = (HBITMAP) ::LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_EXPONENTIAL), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);
			if(hBitmap == NULL) 
			{
				CString s;
				s.Format(_T("Can not open Resource File"));
				AfxMessageBox(s);
				return ;
			}
			m_staPic.SetBitmap(hBitmap);
			double dMean = ((ExponentialDistribution*)_pProbDist)->getMean();
			m_dFirstVal = dMean;
			UpdateData( false );
			m_staticFirstVal.ShowWindow( true );
			m_staticFirstVal.SetWindowText("Lambda :");
			m_editFirstVal.ShowWindow( true );
			return;
		}
	case NORMAL:
		{
			HBITMAP hBitmap = (HBITMAP) ::LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_NORCDF), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);
			if(hBitmap == NULL) 
			{
				CString s;
				s.Format(_T("Can not open Resource File"));
				AfxMessageBox(s);
				return ;
			}
			m_staPic.SetBitmap(hBitmap);
			double dMean = ((NormalDistribution*)_pProbDist)->getMean();
			double dStdDev = ((NormalDistribution*)_pProbDist)->getStdDev();
			m_dFirstVal = dMean;
			m_dSecondVal = dStdDev;
			UpdateData( false );
			m_staticFirstVal.ShowWindow( true );
			m_staticFirstVal.SetWindowText("Mean :");
			m_editFirstVal.ShowWindow( true );
			m_staticSecondVal.ShowWindow( true );
			m_staticSecondVal.SetWindowText("StdDev :");
			m_editSecondVal.ShowWindow( true );
			return;
		}
	case UNIFORM:
		{
			HBITMAP hBitmap = (HBITMAP) ::LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_UNIFORM), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);
			if(hBitmap == NULL) 
			{
				CString s;
				s.Format(_T("Can not open Resource File"));
				AfxMessageBox(s);
				return ;
			}
			m_staPic.SetBitmap(hBitmap);
			double dMin = ((UniformDistribution*)_pProbDist)->getMin();
			double dMax = ((UniformDistribution*)_pProbDist)->getMax();
			m_dFirstVal = dMin;
			m_dSecondVal = dMax;
			UpdateData( false );
			m_staticFirstVal.ShowWindow( true );
			m_staticFirstVal.SetWindowText("Minimum :");
			m_editFirstVal.ShowWindow( true );
			m_staticSecondVal.ShowWindow( true );
			m_staticSecondVal.SetWindowText("Maximum :");
			m_editSecondVal.ShowWindow( true );
			return;
		}
	case EMPIRICAL:
		{
			m_ToolBar.ShowWindow(TRUE);
			m_ToolBar.GetToolBarCtrl().ShowWindow( TRUE );
			m_ToolBar.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_NEW );
			m_staticGroup.ShowWindow(true);
			HBITMAP hBitmap = (HBITMAP) ::LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_EXPIRICAL), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);
			if(hBitmap == NULL) 
			{
				CString s;
				s.Format(_T("Can not open Resource File"));
				AfxMessageBox(s);
				return ;
			}
			m_staPic.SetBitmap(hBitmap);
			m_listCtrlValue.DeleteAllItems();
			int nCount = ((HistogramDistribution*)_pProbDist)->getCount();
			int prevprob = 0;
			unsigned char prob = 0;
			for( int i=0; i<nCount; i++ )
			{
				CString csStr;
				double dValue = ((HistogramDistribution*)_pProbDist)->getValue( i );
				csStr.Format( "%.2f", dValue);
				m_listCtrlValue.InsertItem( i, csStr );
				prob = ((HistogramDistribution*)_pProbDist)->getProb( i );
				csStr.Format( "%d%%", prob - prevprob );
				prevprob = prob;
				m_listCtrlValue.SetItemText( i, 1, csStr );
			}
			CString csTotal;
			csTotal.Format( "%d%%", prob);
			m_staticTotalVal.SetWindowText( csTotal );
			UpdateData( false );
			m_listCtrlValue.ShowWindow( true );
			m_staticTotal.ShowWindow( true );
			m_staticTotalVal.ShowWindow( true );
			return;
		}
	case HISTOGRAM:
		{
			m_ToolBar.ShowWindow(TRUE);
			m_ToolBar.GetToolBarCtrl().ShowWindow( TRUE );
			m_ToolBar.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_NEW );
			m_staticGroup.ShowWindow(TRUE);
			HBITMAP hBitmap = (HBITMAP) ::LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_HISTOGRAM), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);
			if(hBitmap == NULL) 
			{
				CString s;
				s.Format(_T("Can not open Resource File"));
				AfxMessageBox(s);
				return ;
			}
			m_staPic.SetBitmap(hBitmap);
			m_listCtrlValue.DeleteAllItems();
			int nCount = ((HistogramDistribution*)_pProbDist)->getCount();
			int prevprob = 0;
			unsigned char prob = 0;
			for( int i=0; i<nCount; i++ )
			{
				CString csStr;
				double dValue = ((HistogramDistribution*)_pProbDist)->getValue( i );
				csStr.Format( "%.2f", dValue);
				m_listCtrlValue.InsertItem( i, csStr );
				prob = ((HistogramDistribution*)_pProbDist)->getProb( i );
				csStr.Format( "%d%%", prob - prevprob );
				prevprob = prob;
				m_listCtrlValue.SetItemText( i, 1, csStr );
			}
			CString csTotal;
			csTotal.Format( "%d%%", prob);
			m_staticTotalVal.SetWindowText( csTotal );
			UpdateData( false );
			m_listCtrlValue.ShowWindow( true );
			m_staticTotal.ShowWindow( true );
			m_staticTotalVal.ShowWindow( true );
			return;
		}
	}

}


void CProbDistDlg::OnSelchangeListNamelist() 
{
	ReloadContents();
	m_btnDel.EnableWindow();
}


void CProbDistDlg::OnCancel() 
{
	// TODO: Add extra cleanup here
	if( !m_btnSave.IsWindowEnabled() )  // nonzero if the window was previously disabled
	{
		CDialog::OnCancel();
		return;
	}
	
	AfxGetApp()->BeginWaitCursor();
	try
	{
		_g_GetActiveProbMan( GetInputTerminal() )->loadDataSet("");
	}
	catch( FileVersionTooNewError* _pError )
	{
		char szBuf[128];
		_pError->getMessage( szBuf );
		MessageBox( szBuf, "Error", MB_OK|MB_ICONWARNING );
		delete _pError;			
	}
	AfxGetApp()->EndWaitCursor();
	CDialog::OnCancel();
}

void CProbDistDlg::OnOK() 
{
	// TODO: Add extra validation here
	if( m_bNeedReturn && m_nLastSelection == -1 )
	{
		MessageBox( "No entry has been selected.", "Error", MB_OK|MB_ICONWARNING );
		return;
	}
	if( !CheckFullPercent() )
	{
		MessageBox( "Some Histogram Distribution's total is not 100%", "Error", MB_OK|MB_ICONWARNING );
		return;
	}

	if( !m_btnSave.IsWindowEnabled() )  // nonzero if the window was previously disabled
	{
		CDialog::OnOK();
		return;
	}
	
	AfxGetApp()->BeginWaitCursor();
	_g_GetActiveProbMan( GetInputTerminal() )->saveDataSet("", true);
	AfxGetApp()->EndWaitCursor();

	CDialog::OnOK();
}

void CProbDistDlg::OnButtonSave() 
{
	// TODO: Add your control notification handler code here
	if( !CheckFullPercent() )
	{
		MessageBox( "Some Histogram Distribution's total is not 100%", "Error", MB_OK|MB_ICONWARNING );
		return;
	}
	_g_GetActiveProbMan( GetInputTerminal() )->saveDataSet("", true);
	m_btnSave.EnableWindow( FALSE );
}


void CProbDistDlg::OnKillfocusEditName() 
{
	// TODO: Add your control notification handler code here

	if( !m_bEditChanged )
		return;


	int nSelIdx = m_listBoxName.GetCurSel();
	if( nSelIdx == LB_ERR )
		return;

	int nSelDBIdx = m_listBoxName.GetItemData( nSelIdx );
	CProbDistEntry* pPDEntry = _g_GetActiveProbMan( GetInputTerminal() )->getItem( nSelDBIdx );
	m_editName.GetWindowText( pPDEntry->m_csName );
	ReloadDatabase( nSelDBIdx );
	m_btnSave.EnableWindow();
}

void CProbDistDlg::OnChangeEdit() 
{
	m_bEditChanged = true;

}


// new item create new item with default value.
// modify the entry in the db 
// load it.
void CProbDistDlg::OnSelchangedTreeDistribution(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here	
	*pResult = 0;
	
	int nSelIdx = m_listBoxName.GetCurSel();
	if( nSelIdx == LB_ERR )
		return;

	int nSelDBIdx = m_listBoxName.GetItemData( nSelIdx );
	CProbDistEntry* pPDEntry = _g_GetActiveProbMan( GetInputTerminal() )->getItem( nSelDBIdx );

	// get tree item
	HTREEITEM hItem = m_treeDist.GetSelectedItem();
	int nDistType = m_treeDist.GetItemData( hItem );
	if( nDistType == pPDEntry->m_pProbDist->getProbabilityType() )
		return;


	ProbabilityDistribution* pDist;

	switch( nDistType )
	{
	case BERNOULLI:
		pDist = new BernoulliDistribution();
		break;
	case BETA:
		pDist = new BetaDistribution( 0, 0 );
		break;
	case CONSTANT:
		pDist = new ConstantDistribution( 0 );
		break;
	case EXPONENTIAL:
		pDist = new ExponentialDistribution( 0 );
		break;
	case NORMAL:
		pDist = new NormalDistribution( 0, 0 );
		break;
	case UNIFORM:
		pDist = new UniformDistribution( 0, 0 );
		break;
	case EMPIRICAL:
		pDist = new EmpiricalDistribution();
		break;
	case HISTOGRAM:
		pDist = new HistogramDistribution();
		break;
	}


	// modify db
	delete pPDEntry->m_pProbDist;
	pPDEntry->m_pProbDist = pDist;
	
	// reload.
	ReloadDatabase( nSelDBIdx );
	m_btnSave.EnableWindow();

}


// -1 for no selection.
int CProbDistDlg::GetSelDBIndex()
{
	int nItem = m_listBoxName.GetCurSel();
	if( nItem >= 0 )
	{
		return m_listBoxName.GetItemData( nItem );
	}
	return -1;
}


// 
void CProbDistDlg::OnKillfocusEditVal() 
{
	if( !m_bEditChanged )
		return;

	UpdateData();

	int nCurSel = m_listBoxName.GetCurSel();
	if( nCurSel == LB_ERR )
		return;	

	CProbDistEntry* pPDEntry = _g_GetActiveProbMan( GetInputTerminal() )->getItem( m_listBoxName.GetItemData(nCurSel) );

	ProbabilityDistribution* pProbDist = pPDEntry->m_pProbDist;
	int nDistType = pProbDist->getProbabilityType();

	switch( nDistType )
	{
	case BERNOULLI:
		((BernoulliDistribution*)pProbDist)->init( m_dFirstVal, m_dSecondVal, m_dThirdVal/100.0 );
		break;
	case BETA:
		((BetaDistribution*)pProbDist)->init( m_dFirstVal, m_dSecondVal, m_dThirdVal, m_dForthVal );
		break;
	case CONSTANT:
		((ConstantDistribution*)pProbDist)->init( m_dFirstVal );
		break;
	case EXPONENTIAL:
		((ExponentialDistribution*)pProbDist)->init( m_dFirstVal );
		break;
	case NORMAL:
		((NormalDistribution*)pProbDist)->init( m_dFirstVal, m_dSecondVal, 6);
		break;
	case UNIFORM:
		((UniformDistribution*)pProbDist)->init( m_dFirstVal, m_dSecondVal );
		break;
	}
	m_bEditChanged = false; 
	m_btnSave.EnableWindow();	
}



void CProbDistDlg::OnEndlabeleditListValue(NMHDR* pNMHDR, LRESULT* pResult) 
{
	SetHistDistDB();

	*pResult = 0;
}


// from the listctrol set the database.
void CProbDistDlg::SetHistDistDB()
{
	int nCurSel = m_listBoxName.GetCurSel();
	if( nCurSel == LB_ERR )
		return;


	CProbDistEntry* pPDEntry = _g_GetActiveProbMan( GetInputTerminal() )->getItem( m_listBoxName.GetItemData(nCurSel) );

	// select tree item
	ProbabilityDistribution* pProbDist = pPDEntry->m_pProbDist;
	int nDistType = pProbDist->getProbabilityType();
	assert( nDistType == EMPIRICAL || nDistType == HISTOGRAM );


	// create data list from the list ctrol

	int nCount = m_listCtrlValue.GetItemCount();
	double* dVal = new double[nCount];
	double* nProb = new double[nCount];
	int nDBCount = 0;
	int nTotal = 0;
	for( int i=0; i<nCount; i++ )
	{
		CString csVal = m_listCtrlValue.GetItemText( i, 0 );
		CString csPercent = m_listCtrlValue.GetItemText( i, 1 );
		if( csVal.IsEmpty() || csPercent.IsEmpty() )
			continue;

		dVal[nDBCount] = atof( csVal );
		csPercent.Remove( '%' );
		int nPercent = atoi( csPercent );
		nProb[nDBCount] = nPercent;
		nTotal += nPercent;
		nDBCount++;
	}
	try
	{
		((HistogramDistribution*)pProbDist)->init( nDBCount, dVal, nProb );
	}
	catch( TwoStringError* _pError )
	{
		delete _pError;
	}

	delete [] dVal;
	delete [] nProb;
	CString csTotal;
	csTotal.Format( "%d%%", nTotal );
	m_staticTotalVal.SetWindowText( csTotal );
	m_btnSave.EnableWindow();
}

LRESULT CProbDistDlg::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	if (message == WM_INPLACE_SPIN)
	{
		SetHistDistDB();
		return TRUE;
	}
	
	return CDialog::DefWindowProc(message, wParam, lParam);
}


// call by onbtnsave to see if there are still some place is not 100% full.
bool CProbDistDlg::CheckFullPercent()
{
	int nCountEntry = _g_GetActiveProbMan( GetInputTerminal() )->getCount();
	for( int i=0; i<nCountEntry; i++ )
	{
		CProbDistEntry* pPDEntry = _g_GetActiveProbMan( GetInputTerminal() )->getItem( i );
		ProbabilityDistribution* pProbDist = pPDEntry->m_pProbDist;
		int nDistType = pProbDist->getProbabilityType();
		if( nDistType == HISTOGRAM || nDistType == EMPIRICAL )
		{
			int n = ((HistogramDistribution*)pProbDist)->getCount();
			if( n < 1 || ((HistogramDistribution*)pProbDist)->getProb( n - 1 ) != 100 )
				return false;
		}
	}
	return true;
}

int CProbDistDlg::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	if (!m_ToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_ToolBar.LoadToolBar(IDR_PEOPLEMOVEBAR))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}
	return 0;
}

void CProbDistDlg::InitToolBar()
{
	CRect rc;
	m_toolbarcontenter1.GetWindowRect( &rc );
	ScreenToClient(&rc);
	m_ToolBar.MoveWindow(&rc);
	m_ToolBar.ShowWindow(SW_SHOW);
	m_toolbarcontenter1.ShowWindow(SW_HIDE);
	m_ToolBar.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_NEW,FALSE );
	m_ToolBar.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_DELETE,FALSE );
	m_ToolBar.GetToolBarCtrl().HideButton( ID_PEOPLEMOVER_CHANGE);
}
void CProbDistDlg::OnPeoplemoverDelete() 
{
	int nSelIdx = GetSelectedItem();
	if( nSelIdx == -1 )
		return;

	m_btnSave.EnableWindow();
	m_listCtrlValue.DeleteItem( nSelIdx );
	m_ToolBar.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_DELETE ,FALSE);
}
void CProbDistDlg::OnPeoplemoverNew() 
{
	m_listCtrlValue.EditNew();
}

void CProbDistDlg::OnItemchangedListValue(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	if (GetSelectedItem() >= 0)
		m_ToolBar.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_DELETE ,TRUE);
	
	*pResult = 0;
}

int CProbDistDlg::GetSelectedItem()
{
	int nCount = m_listCtrlValue.GetItemCount();
	for( int i=0; i<nCount; i++ )
	{
		if( m_listCtrlValue.GetItemState( i, LVIS_SELECTED ) == LVIS_SELECTED )
		{
			return i;
		}
	}
	return -1;
}
