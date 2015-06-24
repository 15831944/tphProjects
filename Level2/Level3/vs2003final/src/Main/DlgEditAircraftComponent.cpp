// DlgEditAircraftComponent.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "DlgEditAircraftComponent.h"
#include ".\dlgeditaircraftcomponent.h"


// CDlgEditAircraftComponent dialog

IMPLEMENT_DYNAMIC(CDlgEditAircraftComponent, CDialog)
CDlgEditAircraftComponent::CDlgEditAircraftComponent(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgEditAircraftComponent::IDD, pParent)
{
}

CDlgEditAircraftComponent::~CDlgEditAircraftComponent()
{
}

void CDlgEditAircraftComponent::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgEditAircraftComponent, CDialog)
END_MESSAGE_MAP()

void CDlgEditAircraftComponent::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		//dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}



// CDlgEditAircraftComponent message handlers

BOOL CDlgEditAircraftComponent::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here

	/***************/
	CRect rect;

	//// Get size and position of the template textfield we created before in the dialog editor
	//GetDlgItem(IDC_OPENGL)->GetWindowRect(rect);

	// Convert screen coordinates to client coordinates
	ScreenToClient(rect);

	// Create OpenGL Control window
	//m_ctrl1..oglCreate(rect, this);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
