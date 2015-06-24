// AircraftFurningCommentView.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "AircraftFurningCommentView.h"


// CAircraftFurningCommentView

IMPLEMENT_DYNCREATE(CAircraftFurningCommentView, CFormView)

CAircraftFurningCommentView::CAircraftFurningCommentView()
	: CFormView(CAircraftFurningCommentView::IDD)
{
}

CAircraftFurningCommentView::~CAircraftFurningCommentView()
{
}

void CAircraftFurningCommentView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_COMMENT, m_editComment);
	DDX_Control(pDX, IDC_STATIC_TITLE, m_staticTitile);
	DDX_Control(pDX, IDOK, m_btnOK);
	DDX_Control(pDX, IDC_BUTTONSAVE, m_btnSave);
	DDX_Control(pDX, IDC_BUTTON_SAVEAS, m_btnSaveAs);
	DDX_Control(pDX, IDCANCEL, m_btnCancel);
}

BEGIN_MESSAGE_MAP(CAircraftFurningCommentView, CFormView)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON_SAVEAS, OnBnClickedButtonSaveas)
	ON_BN_CLICKED(IDC_BUTTONSAVE, OnBnClickedButtonsave)
//	ON_EN_CHANGE(IDC_EDIT_COMMENT, OnEnChangeEditComment)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CAircraftFurningCommentView diagnostics
void CAircraftFurningCommentView::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here		 
	if(m_btnOK.GetSafeHwnd())
	{
		SetRedraw(FALSE);
		CRect btnrc;
		m_btnSave.GetWindowRect( &btnrc );

		CRect editRc;
		m_editComment.GetWindowRect(&editRc);

		m_editComment.MoveWindow(7,32, cx-14, cy - 72);

		m_btnSaveAs.MoveWindow(cx-25-4*btnrc.Width(), cy-8-btnrc.Height(), btnrc.Width(),btnrc.Height());
		m_btnSave.MoveWindow( cx-20-3*btnrc.Width(),cy-8-btnrc.Height(),btnrc.Width(),btnrc.Height() );
		m_btnOK.MoveWindow( cx-15-2*btnrc.Width(),cy-8-btnrc.Height(),btnrc.Width(),btnrc.Height() );
		m_btnCancel.MoveWindow( cx-10-btnrc.Width(),cy-8-btnrc.Height(),btnrc.Width(),btnrc.Height() );
		SetRedraw(TRUE);	
		InvalidateRect(NULL);

	}
}
void CAircraftFurningCommentView::OnBnClickedOk()
{

}

void CAircraftFurningCommentView::OnBnClickedCancel()
{

}
void CAircraftFurningCommentView::OnBnClickedButtonSaveas()
{

}
void CAircraftFurningCommentView::OnBnClickedButtonsave()
{

}
//void CAircraftFurningCommentView::OnEnChangeEditComment()
//{
//
//}