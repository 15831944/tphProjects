#include "stdafx.h"
#include "TermPlan.h"
#include "DlgEntryFlightTime.h"
#include "afxdialogex.h"

IMPLEMENT_DYNAMIC(DlgEntryFlightTime, CDialog)

DlgEntryFlightTime::DlgEntryFlightTime(InputTerminal* pTerminal, CWnd* pParent /*=NULL*/)
	: CDialog(DlgEntryFlightTime::IDD, pParent),
	m_pTerminal(pTerminal)
{ 
}

DlgEntryFlightTime::~DlgEntryFlightTime()
{
}

void DlgEntryFlightTime::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDOK, m_btnOk);
	DDX_Control(pDX, IDC_BUTTON_SAVE, m_btnSave);
	DDX_Control(pDX, IDC_BUTTON_IMPORT, m_btnImport);
	DDX_Control(pDX, IDCANCEL, m_btnCancel);
}


BEGIN_MESSAGE_MAP(DlgEntryFlightTime, CDialog)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_BN_CLICKED(IDOK, &DlgEntryFlightTime::OnBnClickedOk)
END_MESSAGE_MAP()


int DlgEntryFlightTime::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rect;
	rect.SetRectEmpty();
	if(!m_listCtrl.CreateEx(NULL, WS_VISIBLE | WS_CHILD | WS_BORDER | LVS_ICON| LVS_ALIGNTOP |
		LVS_AUTOARRANGE | LVS_SINGLESEL | LVS_SHOWSELALWAYS,
		rect, this, 100000))
	{
		TRACE0("Failed to create list control\n");
		return -1;
	}

	if(!m_toolbar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_toolbar.LoadToolBar(IDR_TOOLBAR_ENTRYFLIGHTTIME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;
	}

	return 0;
}

BOOL DlgEntryFlightTime::OnInitDialog()
{
 	CDialog::OnInitDialog();
	CRect rect;
	GetClientRect(rect);
	LayoutAllControls(rect.Height(), rect.Width());

	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),TRUE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),FALSE);
	ReloadData();
	return 0;
}

void DlgEntryFlightTime::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	if(m_btnOk.m_hWnd == NULL)
		return;

	LayoutAllControls(cy, cx);

	InvalidateRect(NULL);
}

void DlgEntryFlightTime::ReloadData()
{

}

void DlgEntryFlightTime::OnBnClickedOk()
{
	CDialog::OnOK();
}

void DlgEntryFlightTime::LayoutAllControls(int cy, int cx)
{
	int btnHeight = 22;
	int toolbarHeight = 30;

	int btnWidth = 80;
	if(cx < (btnWidth*4+35))
	{
		btnWidth = (cx -35)/4;
	}
	m_btnImport.MoveWindow(10, cy-15-btnHeight, btnWidth, btnHeight);
	m_btnSave.MoveWindow(cx-3*btnWidth-20, cy-15-btnHeight, btnWidth, btnHeight);
	m_btnOk.MoveWindow(cx-2*btnWidth-15, cy-15-btnHeight, btnWidth, btnHeight);
	m_btnCancel.MoveWindow(cx-btnWidth-10, cy-15-btnHeight, btnWidth, btnHeight);

	m_toolbar.MoveWindow(12, 10, cx-22, toolbarHeight);
	m_listCtrl.MoveWindow(10, 10+toolbarHeight, cx-20, cy-toolbarHeight-btnHeight-40);
}
