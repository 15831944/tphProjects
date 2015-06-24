// PaxProbDistDlg.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "PaxProbDistDlg.h"
#include "inputs\Probab.h"
#include "inputs\constrnt.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPaxProbDistDlg dialog


CPaxProbDistDlg::CPaxProbDistDlg(ConstraintEntry* _pEntry, CWnd* pParent /*=NULL*/)
	: CDialog(CPaxProbDistDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPaxProbDistDlg)
	m_dFirstVal = 0.0;
	m_dSecondVal = 0.0;
	m_dThirdVal = 0.0;
	m_dForthVal = 0.0;
	m_nTotal = 0;
	m_csPaxType = _T("");
	m_csDistName = _T("");
	m_csUnits = _T("");
	//}}AFX_DATA_INIT
	m_pEntry = _pEntry;
}


void CPaxProbDistDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPaxProbDistDlg)
	DDX_Control(pDX, IDC_EDIT_TOTAL, m_editTotal);
	DDX_Control(pDX, IDC_STATIC_TOTAL, m_staticTotal);
	DDX_Control(pDX, IDC_LIST_VALUE, m_listCtrlValue);
	DDX_Control(pDX, IDC_EDIT_FORTHVAL, m_editForthVal);
	DDX_Control(pDX, IDC_STATIC_FORTHVAL, m_staticForthVal);
	DDX_Control(pDX, IDC_EDIT_THIRDVAL, m_editThirdVal);
	DDX_Control(pDX, IDC_STATIC_THIRDVAL, m_staticThirdVal);
	DDX_Control(pDX, IDC_EDIT_SECONDVAL, m_editSecondVal);
	DDX_Control(pDX, IDC_STATIC_SECONDVAL, m_staticSecondVal);
	DDX_Control(pDX, IDC_EDIT_FIRSTVAL, m_editFirstVal);
	DDX_Control(pDX, IDC_STATIC_FIRSTVAL, m_staticFirstVal);
	DDX_Control(pDX, IDC_EDIT_UNITS, m_editUnts);
	DDX_Control(pDX, IDC_STATIC_UNITS, m_staticUnits);
	DDX_Control(pDX, IDC_TREE_DISTRIBUTION, m_treeDist);
	DDX_Text(pDX, IDC_EDIT_FIRSTVAL, m_dFirstVal);
	DDX_Text(pDX, IDC_EDIT_SECONDVAL, m_dSecondVal);
	DDX_Text(pDX, IDC_EDIT_THIRDVAL, m_dThirdVal);
	DDX_Text(pDX, IDC_EDIT_FORTHVAL, m_dForthVal);
	DDX_Text(pDX, IDC_EDIT_TOTAL, m_nTotal);
	DDX_Text(pDX, IDC_EDIT_PAXTYPE, m_csPaxType);
	DDX_Text(pDX, IDC_EDIT_DISTRIBUTION, m_csDistName);
	DDX_Text(pDX, IDC_EDIT_UNITS, m_csUnits);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPaxProbDistDlg, CDialog)
	//{{AFX_MSG_MAP(CPaxProbDistDlg)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_DISTRIBUTION, OnSelchangedTreeDistribution)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPaxProbDistDlg message handlers

BOOL CPaxProbDistDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	const Constraint* pConstr = m_pEntry->getConstraint();
	//char buf[128];
	CString buf;
	pConstr->screenPrint( buf, 0, 128 );
	m_csPaxType.Format( "%s", buf.GetBuffer(0) );
	m_csUnits = CString( "Seconds" );
	UpdateData( false );
	LoadTree();
	RefreshDistControl();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPaxProbDistDlg::LoadTree()
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


	const ProbabilityDistribution* pDist = m_pEntry->getValue();
	int nDBDistType = pDist->getProbabilityType();

	//populate the tree view with the pd types
	CString s;
	s.LoadString(IDS_TVNN_CONTINUOUSDISTS);
	HTREEITEM hItem = m_treeDist.InsertItem(s, TVI_ROOT, TVI_LAST);
	
	s.LoadString(IDS_TVNN_BERNOULLIDIST);
	HTREEITEM hThisItem = m_treeDist.InsertItem( s, hItem, TVI_LAST);
	m_treeDist.SetItemData( hThisItem, BERNOULLI );
	if( nDBDistType == BERNOULLI )
	{
		m_treeDist.SelectItem( hThisItem );
	}

	s.LoadString(IDS_TVNN_BETADIST);
	hThisItem = m_treeDist.InsertItem( s, hItem, TVI_LAST);
	m_treeDist.SetItemData( hThisItem, BETA );
	if( nDBDistType == BETA )
	{
		m_treeDist.SelectItem( hThisItem );
	}

	s.LoadString(IDS_TVNN_EXPONDIST);
	hThisItem = m_treeDist.InsertItem( s, hItem, TVI_LAST);
	m_treeDist.SetItemData( hThisItem, EXPONENTIAL );
	if( nDBDistType == EXPONENTIAL )
	{
		m_treeDist.SelectItem( hThisItem );
	}

	s.LoadString(IDS_TVNN_HISTDIST);
	hThisItem = m_treeDist.InsertItem( s, hItem, TVI_LAST);
	m_treeDist.SetItemData( hThisItem, HISTOGRAM );
	if( nDBDistType == HISTOGRAM )
	{
		m_treeDist.SelectItem( hThisItem );
	}
	
	s.LoadString(IDS_TVNN_NORMALDIST);
	hThisItem = m_treeDist.InsertItem( s, hItem, TVI_LAST);
	m_treeDist.SetItemData( hThisItem, NORMAL );
	if( nDBDistType == NORMAL )
	{
		m_treeDist.SelectItem( hThisItem );
	}

	s.LoadString(IDS_TVNN_UNIFORMDIST);
	hThisItem = m_treeDist.InsertItem( s, hItem, TVI_LAST);
	m_treeDist.SetItemData( hThisItem, UNIFORM );
	if( nDBDistType == UNIFORM )
	{
		m_treeDist.SelectItem( hThisItem );
	}

	m_treeDist.Expand(hItem, TVE_EXPAND);

	s.LoadString(IDS_TVNN_DISCRETEDISTS);
	
	hItem = m_treeDist.InsertItem(s, TVI_ROOT, TVI_LAST);

	s.LoadString(IDS_TVNN_CONSTANTDIST);
	hThisItem = m_treeDist.InsertItem( s, hItem, TVI_LAST);
	m_treeDist.SetItemData( hThisItem, CONSTANT );
	if( nDBDistType == CONSTANT )
	{
		m_treeDist.SelectItem( hThisItem );
	}

	s.LoadString(IDS_TVNN_EMPDIST);
	hThisItem = m_treeDist.InsertItem( s, hItem, TVI_LAST);
	m_treeDist.SetItemData( hThisItem, EMPIRICAL );
	if( nDBDistType == EMPIRICAL )
	{
		m_treeDist.SelectItem( hThisItem );
	}

	m_treeDist.Expand(hItem, TVE_EXPAND);

}


// base on the tree selection 
// compare with the m_pEntry 
// if the same show the data in m_pEntry
// else show the default
// if no selection show nothing.
void CPaxProbDistDlg::RefreshDistControl()
{
	m_staticUnits.ShowWindow( false );
	m_editUnts.ShowWindow( false );
	m_staticFirstVal.ShowWindow( false );
	m_editFirstVal.ShowWindow( false );
	m_staticSecondVal.ShowWindow( false );
	m_editSecondVal.ShowWindow( false );
	m_staticThirdVal.ShowWindow( false );
	m_editThirdVal.ShowWindow( false );
	m_staticForthVal.ShowWindow( false );
	m_editForthVal.ShowWindow( false );
	m_listCtrlValue.ShowWindow( false );
	m_staticTotal.ShowWindow( false );
	m_editTotal.ShowWindow( false );

	m_csDistName = GetTreeDistName();

	const ProbabilityDistribution* pDist = m_pEntry->getValue();
	int nDBDistType = pDist->getProbabilityType();
	int nDistType = GetTreeDistType();
	switch( nDistType )
	{

		case BERNOULLI:
		{
			double dVal1 = 0.0;
			double dVal2 = 0.0;
			double dProb = 0.0;

			if( nDistType == nDBDistType )
			{
				dVal1 = ((BernoulliDistribution*)pDist)->getValue1();
				dVal2 = ((BernoulliDistribution*)pDist)->getValue2();
				dProb = ((BernoulliDistribution*)pDist)->getProb1();
			}

			m_dFirstVal = dVal1;
			m_dSecondVal = dVal2;
			m_dThirdVal = dProb * 100.0;
			m_staticUnits.ShowWindow( true );
			m_editUnts.ShowWindow( true );
			m_staticFirstVal.ShowWindow( true );
			m_staticFirstVal.SetWindowText("First Value :");
			m_editFirstVal.ShowWindow( true );
			m_staticSecondVal.ShowWindow( true );
			m_staticSecondVal.SetWindowText("Second Value :");
			m_editSecondVal.ShowWindow( true );
			m_staticThirdVal.ShowWindow( true );
			m_staticThirdVal.SetWindowText("First Prob(%) :");
			m_editThirdVal.ShowWindow( true );
			break;
		}
		case BETA:
		{
			double dMin = 0.0;
			double dMax = 0.0;
			double dAlpha = DEFAULT_ALPHA;
			double dBeta = DEFAULT_BETA;
			
			if( nDistType == nDBDistType )
			{
				dMin = ((BetaDistribution*)pDist)->getMin();
				dMax = ((BetaDistribution*)pDist)->getMax();
				dAlpha = ((BetaDistribution*)pDist)->getAlpha();
				dBeta = ((BetaDistribution*)pDist)->getBeta();
			}
			CString csVal;
			m_dFirstVal = dMin;
			m_dSecondVal = dMax;
			m_dThirdVal = dAlpha;
			m_dForthVal = dBeta;
			m_staticUnits.ShowWindow( true );
			m_editUnts.ShowWindow( true );
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
			break;
		}
		case CONSTANT:
		{
			double dMean = 0.0;
			if( nDistType == nDBDistType )
			{
				dMean = ((ConstantDistribution*)pDist)->getMean();
			}
			m_dFirstVal = dMean;
			m_staticUnits.ShowWindow( true );
			m_editUnts.ShowWindow( true );
			m_staticFirstVal.ShowWindow( true );
			m_staticFirstVal.SetWindowText("Value :");
			m_editFirstVal.ShowWindow( true );
			break;
		}
		case EXPONENTIAL:
		{
			double dMean = 0.0;
			if( nDistType == nDBDistType )
			{
				dMean = ((ExponentialDistribution*)pDist)->getMean();
			}
			m_dFirstVal = dMean;
			m_staticUnits.ShowWindow( true );
			m_editUnts.ShowWindow( true );
			m_staticFirstVal.ShowWindow( true );
			m_staticFirstVal.SetWindowText("Lambda :");
			m_editFirstVal.ShowWindow( true );
			break;
		}
		case NORMAL:
		{
			double dMean = 0.0;
			double dStdDev = 0.0;
			if( nDistType == nDBDistType )
			{
				dMean = ((NormalDistribution*)pDist)->getMean();
				dStdDev = ((NormalDistribution*)pDist)->getStdDev();
			}
			m_dFirstVal = dMean;
			m_dSecondVal = dStdDev;
			m_staticUnits.ShowWindow( true );
			m_editUnts.ShowWindow( true );
			m_staticFirstVal.ShowWindow( true );
			m_staticFirstVal.SetWindowText("Mean :");
			m_editFirstVal.ShowWindow( true );
			m_staticSecondVal.ShowWindow( true );
			m_staticSecondVal.SetWindowText("StdDev :");
			m_editSecondVal.ShowWindow( true );
			break;
		}
		case UNIFORM:
		{
			double dMin = 0.0;
			double dMax = 0.0;
			if( nDistType == nDBDistType )
			{
				dMin = ((UniformDistribution*)pDist)->getMin();
				dMax = ((UniformDistribution*)pDist)->getMax();
			}
			m_dFirstVal = dMin;
			m_dSecondVal = dMax;
			m_staticUnits.ShowWindow( true );
			m_editUnts.ShowWindow( true );
			m_staticFirstVal.ShowWindow( true );
			m_staticFirstVal.SetWindowText("Minimum :");
			m_editFirstVal.ShowWindow( true );
			m_staticSecondVal.ShowWindow( true );
			m_staticSecondVal.SetWindowText("Maximum :");
			m_editSecondVal.ShowWindow( true );
			break;
		}
		case EMPIRICAL:
		case HISTOGRAM:
		{
			unsigned char prob = 0;
			m_listCtrlValue.DeleteAllItems();
			if( nDistType == nDBDistType )
			{
				int nCount = ((EmpiricalDistribution*)pDist)->getCount();
				int prevprob = 0;
				for( int i=0; i<nCount; i++ )
				{
					CString csStr;
					double dValue = ((EmpiricalDistribution*)pDist)->getValue( i );
					csStr.Format( "%.2f", dValue);
					m_listCtrlValue.InsertItem( i, csStr );
					prob = ((EmpiricalDistribution*)pDist)->getProb( i );
					csStr.Format( "%d", prob - prevprob );
					prevprob = prob;
					m_listCtrlValue.SetItemText( i, 1, csStr );
				}
			}
			m_nTotal = prob;
			m_staticUnits.ShowWindow( true );
			m_editUnts.ShowWindow( true );
			m_listCtrlValue.ShowWindow( true );
			m_staticTotal.ShowWindow( true );
			m_editTotal.ShowWindow( true );
			break;
		}
	}
	UpdateData( false );
}


// base on the selection of the tree return type
int CPaxProbDistDlg::GetTreeDistType()
{
	HTREEITEM hItem = m_treeDist.GetSelectedItem();
	if( hItem == NULL )
		return -1;

	return m_treeDist.GetItemData( hItem );
}


void CPaxProbDistDlg::OnSelchangedTreeDistribution(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	RefreshDistControl();
	
	*pResult = 0;
}


// set the m_pEntry
void CPaxProbDistDlg::OnOK() 
{
	// TODO: Add extra validation here
	const ProbabilityDistribution* pDist = m_pEntry->getValue();
	ProbabilityDistribution* pNewDist = NULL;

	int nDBDistType = pDist->getProbabilityType();
	int nDistType = GetTreeDistType();

	UpdateData();
	switch( nDistType )
	{
	case BERNOULLI:
		if( nDistType != nDBDistType )
		{
			pNewDist = new BernoulliDistribution();
			((BernoulliDistribution*)pNewDist)->init( m_dFirstVal, m_dSecondVal, m_dThirdVal / 100.0 );
		}
		else
			((BernoulliDistribution*)pDist)->init( m_dFirstVal, m_dSecondVal, m_dThirdVal / 100.0 );
		break;
	case BETA:
		if( nDistType != nDBDistType )
		{
			pNewDist = new BetaDistribution( 0, 0 );
			((BetaDistribution*)pNewDist)->init( m_dFirstVal, m_dSecondVal, m_dThirdVal, m_dForthVal );
		}
		else
			((BetaDistribution*)pDist)->init( m_dFirstVal, m_dSecondVal, m_dThirdVal, m_dForthVal );
		break;
	case CONSTANT:
		if( nDistType != nDBDistType )
		{
			pNewDist = new ConstantDistribution( 0 );
			((ConstantDistribution*)pNewDist)->init( m_dFirstVal );
		}
		else
			((ConstantDistribution*)pDist)->init( m_dFirstVal );
		break;
	case EXPONENTIAL:
		if( nDistType != nDBDistType )
		{
			pNewDist = new ExponentialDistribution( 0 );
			((ExponentialDistribution*)pNewDist)->init( m_dFirstVal );
		}
		else
			((ExponentialDistribution*)pDist)->init( m_dFirstVal );
		break;
	case NORMAL:
		if( nDistType != nDBDistType )
		{
			pNewDist = new NormalDistribution( 0, 0 );
			((NormalDistribution*)pNewDist)->init( m_dFirstVal, m_dSecondVal );
		}
		else
			((NormalDistribution*)pDist)->init( m_dFirstVal, m_dSecondVal );
		break;
	case UNIFORM:
		if( nDistType != nDBDistType )
		{
			pNewDist = new UniformDistribution( 0, 0 );
			((UniformDistribution*)pNewDist)->init( m_dFirstVal, m_dSecondVal );
		}
		else
			((UniformDistribution*)pDist)->init( m_dFirstVal, m_dSecondVal );
		break;
	case EMPIRICAL:
		if( nDistType != nDBDistType )
		{
			pNewDist = new EmpiricalDistribution();
		}
		break;
	case HISTOGRAM:
		if( nDistType != nDBDistType )
		{
			pNewDist = new HistogramDistribution();
		}
		break;

	}
	if( nDistType != nDBDistType )
	{
		m_pEntry->setValue( pNewDist );
	}
	CDialog::OnOK();
}

CString CPaxProbDistDlg::GetTreeDistName()
{
	HTREEITEM hItem = m_treeDist.GetSelectedItem();
	if( hItem == NULL )
		return "";

	return m_treeDist.GetItemText( hItem );
}
