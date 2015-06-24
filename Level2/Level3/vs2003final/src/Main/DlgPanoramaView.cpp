#include "StdAfx.h"
#include "resource.h"
#include ".\dlgpanoramaview.h"


IMPLEMENT_DYNAMIC(CDlgPanoramaView, CXTResizeDialog)


CDlgPanoramaView::CDlgPanoramaView( PanoramaViewDataList& viewData, CWnd* pParent /*= NULL*/ )
: CXTResizeDialog(CDlgPanoramaView::IDD, pParent ), m_theDataList(viewData)
{
	m_nSelectItem = -1;
}
CDlgPanoramaView::~CDlgPanoramaView(void)
{
}

void CDlgPanoramaView::DoDataExchange( CDataExchange* pDx )
{
	CXTResizeDialog::DoDataExchange(pDx);
}

BEGIN_MESSAGE_MAP(CDlgPanoramaView, CXTResizeDialog)
	ON_WM_CREATE()
END_MESSAGE_MAP()

BOOL CDlgPanoramaView::InitToolBar()
{
	if (!m_ToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_ToolBar.LoadToolBar(IDR_WALKTHROUGHTOOLBAR))
	{
		TRACE0("Failed to create toolbar\n");
		return FALSE;      // fail to create
	}	
	return TRUE;
}

int CDlgPanoramaView::OnCreate( LPCREATESTRUCT lpCreateStruct )
{
	if(CXTResizeDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if(!InitToolBar())
		return -1;
	return 0;
}

BOOL CDlgPanoramaView::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog();
	
	return TRUE;
}