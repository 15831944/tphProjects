// DlgSeatRowProp.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "DlgSeatRowProp.h"
#include ".\dlgseatrowprop.h"
#include "./onboard/OnboardViewMsg.h"
#include "./TermPlanDoc.h"
#include "./MainFrm.h"
#include "onboard/AircraftLayOutFrame.h"
#include "onboard/AircraftLayout3DView.h"
#include <CommonData/Fallback.h>

// CDlgSeatRowProp dialog

IMPLEMENT_DYNAMIC(CDlgSeatRowProp, CDialog)
CDlgSeatRowProp::CDlgSeatRowProp(const CString& rowName,const ARCPath3& rowPath  ,CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSeatRowProp::IDD, pParent)
	, m_nRowCount(0)	
{	
	m_nRowCount  = rowPath.size();
	m_sRowName = rowName;
	mDestPath = rowPath;
}

CDlgSeatRowProp::~CDlgSeatRowProp()
{
}

void CDlgSeatRowProp::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_nRowCount);
}


BEGIN_MESSAGE_MAP(CDlgSeatRowProp, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_MESSAGE(TP_ONBOARD_DATABACK, OnTempFallbackFinished)
	ON_BN_CLICKED(IDC_BUTTON_PICKLINE, OnBnClickedButtonPickline)
END_MESSAGE_MAP()


// CDlgSeatRowProp message handlers

void CDlgSeatRowProp::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	if(m_nRowCount <1)
	{
		AfxMessageBox("At Least define 1 seat");
		return ;
	}
	if(mDestPath.size()<2)
	{
		AfxMessageBox("At Least Pick 2 Points");
		return;
	}


	
	double dLen = mDestPath.GetLength();
	ASSERT(dLen>0);
	//sub divide the pick lines

	ARCPath3 path;
	path.resize(m_nRowCount);
	if(m_nRowCount==1)
	{
		path[0] = mDestPath[0];
	}
	else
	{
		for(int i=0;i<m_nRowCount;i++)
		{
			double dOffset = dLen/(m_nRowCount-1);
			ARCVector3 vPt = mDestPath.getDistancePoint(dOffset*i);
			vPt[VZ]= 0;		
			path[i] = vPt; 
		}
	}

	mDestPath = path;
	OnOK();
}

BOOL CDlgSeatRowProp::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	//CString strTmp;
	//strTmp.Format("%d", m_rowCount);
	CString sTitile;
	sTitile.Format(_T("Edit Seat Row Count : %s"),m_sRowName.GetString());
	SetWindowText(sTitile);
	UpdateData(FALSE);//m_editRowNumber.SetWindowText(m_sRowName);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgSeatRowProp::OnBnClickedButtonPickline()
{
	// TODO: Add your control notification handler code here
	ASSERT(m_pParentWnd);

	CWaitCursor wc;
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();
	CAircraftLayOutFrame* layoutFrame = NULL ;
	layoutFrame = (CAircraftLayOutFrame*)pDoc->GetMainFrame()->CreateOrActivateFrame(theApp.m_pAircraftLayoutTermplate, RUNTIME_CLASS(CAircraftLayout3DView));
	wc.Restore();

	HideDialog( pDoc->GetMainFrame() );
	FallbackData data;
	data.hSourceWnd = GetSafeHwnd();

	enum FallbackReason enumReason = PICK_MANYPOINTS;	

	if( !layoutFrame->m_LayoutView->SendMessage( TP_ONBOARDLAYOUT_FALLBACK, (WPARAM)&data, (LPARAM)enumReason ) )
	{
		MessageBox( "Error" );
		ShowDialog( pDoc->GetMainFrame() );
		return;
	}
}

LRESULT CDlgSeatRowProp::OnTempFallbackFinished( WPARAM wParam, LPARAM lParam )
{
	ShowWindow(SW_SHOW);	
	EnableWindow();

	std::vector<ARCVector3>* pData = reinterpret_cast< std::vector<ARCVector3>* >(wParam);
	size_t nSize = pData->size();

	if(nSize <2)
	{
		AfxMessageBox("At least Pick 2 Points ");
		return false;
	}

	mDestPath = *pData;
	return true;
}

void CDlgSeatRowProp::HideDialog( CWnd* parentWnd )
{
	ShowWindow(SW_HIDE);
	while(!parentWnd->IsKindOf(RUNTIME_CLASS(CMDIFrameWnd))) {
		parentWnd->ShowWindow(SW_HIDE);
		parentWnd = parentWnd->GetParent();
	}
	parentWnd->EnableWindow(TRUE);
	parentWnd->SetActiveWindow();
}

void CDlgSeatRowProp::ShowDialog( CWnd* parentWnd )
{
	while (!parentWnd->IsKindOf(RUNTIME_CLASS(CMDIFrameWnd)) )
	{
		parentWnd->ShowWindow(SW_SHOW);
		parentWnd->EnableWindow(FALSE);
		parentWnd = parentWnd->GetParent();
	}
	parentWnd->EnableWindow(FALSE);
	ShowWindow(SW_SHOW);	
	EnableWindow();
}