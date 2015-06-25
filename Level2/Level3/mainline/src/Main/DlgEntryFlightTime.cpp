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
	ResizeAllControls(rect.Height(), rect.Width());

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

	ResizeAllControls(cy, cx);

	InvalidateRect(NULL);
}

void DlgEntryFlightTime::ReloadData()
{

}

void DlgEntryFlightTime::OnBnClickedOk()
{
	CDialog::OnOK();
}

void DlgEntryFlightTime::ResizeAllControls(int cy, int cx)
{
	const int BUTTON_HEIGHT = 22;
	const int BUTTON_WIDTH = 80;
	const long TOOLBAR_HEIGHT = 30;

	m_btnImport.MoveWindow(10, cy-15-BUTTON_HEIGHT, BUTTON_WIDTH, BUTTON_HEIGHT);
	m_btnSave.MoveWindow(cx-3*BUTTON_WIDTH-20, cy-15-BUTTON_HEIGHT, BUTTON_WIDTH, BUTTON_HEIGHT);
	m_btnOk.MoveWindow(cx-2*BUTTON_WIDTH-15, cy-15-BUTTON_HEIGHT, BUTTON_WIDTH, BUTTON_HEIGHT);
	m_btnCancel.MoveWindow(cx-BUTTON_WIDTH-10, cy-15-BUTTON_HEIGHT, BUTTON_WIDTH, BUTTON_HEIGHT);

	m_toolbar.MoveWindow(12, 10, cx-22, TOOLBAR_HEIGHT);
	m_listCtrl.MoveWindow(10, 10+TOOLBAR_HEIGHT, cx-20, cy-TOOLBAR_HEIGHT-BUTTON_HEIGHT-40);
}
