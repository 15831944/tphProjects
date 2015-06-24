// DlgSeatArraySpec.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "DlgSeatArraySpec.h"
#include ".\dlgseatarrayspec.h"
#include "../InputOnBoard/Seat.h"
#include "./TermPlanDoc.h"
#include "./MainFrm.h"
#include <CommonData/Fallback.h>
#include "./ViewMsg.h"
#include "./onboard/OnboardViewMsg.h"
#include "InputOnBoard/Seat.h"
#include "onboard/AircraftLayOutFrame.h"
#include "onboard/AircraftLayout3DView.h"


// CDlgSeatArraySpec dialog

IMPLEMENT_DYNAMIC(CDlgSeatArraySpec, CDialog)
CDlgSeatArraySpec::CDlgSeatArraySpec(CSeatGroup* pseatGroup ,CAircraftPlacements* placements, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSeatArraySpec::IDD, pParent)	
{
	m_pSeatGroup = pseatGroup;
	m_pPlacements = placements;
	if(pseatGroup)
		m_points = pseatGroup->m_path;
}


CDlgSeatArraySpec::~CDlgSeatArraySpec()
{
}

void CDlgSeatArraySpec::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_NAME, m_editName);
	DDX_Control(pDX, IDC_EDIT_PITCH, m_editSeatPitch);
	DDX_Control(pDX, IDC_EDIT_ROWNUM, m_editrowNum);
	DDX_Control(pDX, IDC_RADIO_NUMROW, m_radioNumRow);
	DDX_Control(pDX, IDC_RADIO_LONGAXIS, m_radioPerToLongAxis);
}


BEGIN_MESSAGE_MAP(CDlgSeatArraySpec, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_DRAWLINE, OnBnClickedButtonDrawline)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)		
	ON_MESSAGE(TP_ONBOARD_DATABACK, OnTempFallbackFinished)
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_RADIO_NUMROW, OnBnClickedRadioNumrow)
	ON_BN_CLICKED(IDC_RADIO_FitRow, OnBnClickedRadioFitrow)
END_MESSAGE_MAP()


// CDlgSeatArraySpec message handlers
#include <common/ARCStringConvert.h>
void CDlgSeatArraySpec::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	if(UpdateDataModify())
	{
		OnOK();
	}
}

void CDlgSeatArraySpec::HideDialog(CWnd* parentWnd)
{
	ShowWindow(SW_HIDE);
	while(!parentWnd->IsKindOf(RUNTIME_CLASS(CMDIFrameWnd))) {
		parentWnd->ShowWindow(SW_HIDE);
		parentWnd = parentWnd->GetParent();
	}
	parentWnd->EnableWindow(TRUE);
	parentWnd->SetActiveWindow();
}

void CDlgSeatArraySpec::ShowDialog(CWnd* parentWnd)
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

void CDlgSeatArraySpec::OnBnClickedButtonDrawline()
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

void CDlgSeatArraySpec::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	//restore old data

	OnCancel();
}
#include <InputOnboard/AircraftPlacements.h>
bool CDlgSeatArraySpec::UpdateDataModify()
{
	CString strArrayName;
	m_editName.GetWindowText(strArrayName);
	if(strArrayName.IsEmpty())
	{
		AfxMessageBox("Please Input A Name of The Seat Array");
		return false;
	}
	//else //check the name in the database
	//{
	//	//CSeatGroupDataSet* pSeatGroups = m_pPlacements->GetSeatGroupData();
	//	//for(int i=0;i<pSeatGroups->GetItemCount(); i++)
	//	//{
	//	//	CSeatGroup *pSeatGrp = pSeatGroups->GetItem(i);
	//	//	if(pSeatGrp == m_pSeatGroup)
	//	//		continue;
	//	//	/*if(pSeatGrp->m_strname == strArrayName )
	//	//	{
	//	//		AfxMessageBox("The Group Name is Already Exist!");
	//	//		return false;
	//	//	}*/
	//	//}

	//	
	//}
	
	CString strPitch;
	m_editSeatPitch.GetWindowText(strPitch);
	double dPitch;
	if(!ARCStringConvert::isNumber(strPitch, dPitch))
	{
		AfxMessageBox("The Seat Pitch Should be a Number");
		return false;
	}

	bool bUserowCount;
	int nRowCount;
	if(m_radioNumRow.GetCheck())
	{
		bUserowCount = true;
		CString strNum;
		m_editrowNum.GetWindowText(strNum);
		nRowCount = atoi(strNum.GetString());
	}
	else
		bUserowCount = false;
	PerpendicularType type;
	if(m_radioPerToLongAxis.GetCheck())
		type = LongitudalAxis;
	else
		type = ArrayLine;

	if(m_points.getCount() <2)
	{
		AfxMessageBox("At least Pick 2 Points ");
		return false;
	}
	
	//m_pSeatGroup->m_strname = strArrayName;
	m_pSeatGroup->m_strname =strArrayName;

	m_pSeatGroup->m_dPitch = dPitch * SCALE_FACTOR;
	m_pSeatGroup->m_bUserRowCount = bUserowCount;
	if(bUserowCount)
		m_pSeatGroup->m_nRowCount = nRowCount;
	m_pSeatGroup->m_PerpType = type;
	m_pSeatGroup->m_path = m_points;		
	return true;
	
}

LRESULT CDlgSeatArraySpec::OnTempFallbackFinished(WPARAM wParam, LPARAM lParam)
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

	CPoint2008 *pointList = new CPoint2008[nSize];
	for (size_t i =0; i < nSize; ++i)
	{
		ARCVector3 v3 = pData->at(i);
		pointList[i].setPoint(v3[VX] ,v3[VY],0.0);
	}
	m_points.init(nSize,pointList);	
	delete []pointList;	
	return true;
}

int CDlgSeatArraySpec::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	return 0;
}

BOOL CDlgSeatArraySpec::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	if(m_pSeatGroup && m_editName.GetSafeHwnd())
	{
		m_editName.SetWindowText(m_pSeatGroup->m_strname);


		m_radioNumRow.SetCheck(m_pSeatGroup->m_bUserRowCount);

		int IDCheck = m_pSeatGroup->m_bUserRowCount?IDC_RADIO_NUMROW : IDC_RADIO_FitRow;
		CheckRadioButton(IDC_RADIO_NUMROW, IDC_RADIO_FitRow, IDCheck);

		m_editrowNum.EnableWindow( m_pSeatGroup->m_bUserRowCount );

		CString strTmp;
		strTmp.Format("%.2f",m_pSeatGroup->m_dPitch/SCALE_FACTOR );
		m_editSeatPitch.SetWindowText(strTmp);

		strTmp.Format("%d", m_pSeatGroup->m_nRowCount);
		m_editrowNum.SetWindowText( strTmp );

		if( m_pSeatGroup->m_PerpType == LongitudalAxis )
			IDCheck = IDC_RADIO_LONGAXIS;
		else 
			IDCheck = IDC_RADIO_ARRAYLINE;
		CheckRadioButton(IDC_RADIO_LONGAXIS,IDC_RADIO_ARRAYLINE, IDCheck);		
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgSeatArraySpec::OnBnClickedRadioNumrow()
{
	// TODO: Add your control notification handler code here
	//UpdateData(TRUE);		
	m_editrowNum.EnableWindow( TRUE );	
	//UpdateData(FALSE);
}

void CDlgSeatArraySpec::OnBnClickedRadioFitrow()
{
	// TODO: Add your control notification handler code here
	m_editrowNum.EnableWindow(FALSE);
}
