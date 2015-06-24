// DlgBookmarkedCameras.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "DlgBookmarkedCamerasRender.h"

#include <algorithm>
#include ".\dlgbookmarkedcamerasrender.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#pragma warning (disable:4018)
/////////////////////////////////////////////////////////////////////////////
// CDlgBookmarkedCamerasRender dialog

CDlgBookmarkedCamerasRender::CDlgBookmarkedCamerasRender( const C3DViewCameras& cams, bool bAllowAdd, CWnd* pParent /*= NULL*/ )
	: CDialog(CDlgBookmarkedCamerasRender::IDD, pParent)
	, m_cams(cams)
	, m_bAllowAdd(bAllowAdd)
	, m_bLabelEditing(false)
{

}

void CDlgBookmarkedCamerasRender::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgBookmarkedCamerasRender)
	DDX_Control(pDX, IDC_STATIC_TOOLBAR, m_staticToolbar);
	DDX_Control(pDX, IDC_LIST_BMCAMERAS, m_lstBMCameras);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgBookmarkedCamerasRender, CDialog)
	//{{AFX_MSG_MAP(CDlgBookmarkedCamerasRender)
	ON_WM_CREATE()
	ON_COMMAND(ID_TOOLBARBUTTONDEL, OnToolbarbuttonDEL)
	ON_COMMAND(ID_TOOLBARBUTTONEDIT, OnToolbarbuttonEDIT)
	ON_COMMAND(ID_TOOLBARBUTTONADD, OnToolbarbuttonADD)
	ON_NOTIFY(LVN_GETDISPINFO, IDC_LIST_BMCAMERAS, OnGetDispInfoListBMCameras)
	ON_NOTIFY(LVN_BEGINLABELEDIT, IDC_LIST_BMCAMERAS, OnBeginLabelEditListBMCameras)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_LIST_BMCAMERAS, OnEndLabelEditListBMCameras)
	//}}AFX_MSG_MAP
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_BMCAMERAS, OnLvnItemchangedListBmcameras)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgBookmarkedCamerasRender message handlers

BOOL CDlgBookmarkedCamerasRender::OnInitDialog() 
{
	CDialog::OnInitDialog();
	InitToolBar();
	UpdateToolbar();

	m_lstBMCameras.InsertColumn(0,_T("Name"),LVCFMT_LEFT,150);
	m_lstBMCameras.InsertColumn(1,_T("Type"),LVCFMT_LEFT,50);
	m_lstBMCameras.InsertColumn(2,_T("Split?"),LVCFMT_LEFT,50);
	LoadBMCameraData();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgBookmarkedCamerasRender::LoadBMCameraData()
{
	const C3DViewCameraDesc::List savedCams = m_cams.m_savedCameras;
	size_t nCount = savedCams.size();
	for(size_t i=0;i<nCount;i++)
	{
		m_lstBMCameras.InsertItem(LVIF_TEXT, i, LPSTR_TEXTCALLBACK, 0, 0, 0, NULL);
	}
}

int CDlgBookmarkedCamerasRender::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_toolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_toolBar.LoadToolBar(IDR_ADDDELEDITTOOLBAR))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}
	
	return 0;
}

void CDlgBookmarkedCamerasRender::InitToolBar()
{
	CRect rc;
	m_staticToolbar.GetWindowRect( &rc );
	ScreenToClient(&rc);
	m_toolBar.MoveWindow(&rc,true);
	m_toolBar.ShowWindow( SW_SHOW );
}

void CDlgBookmarkedCamerasRender::OnEndLabelEditListBMCameras(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	*pResult = 0;

	if(pDispInfo->item.pszText)
	{
		int nIdx = pDispInfo->item.iItem;

		C3DViewCameraDesc& camDesc = m_cams.m_savedCameras[nIdx];
		camDesc.SetName(pDispInfo->item.pszText);

		*pResult =1;
	}

	m_bLabelEditing = false;
	UpdateToolbar();
}

void CDlgBookmarkedCamerasRender::DeleteListItem(int _idx)
{
	if (_idx < m_lstBMCameras.GetItemCount() && _idx < m_cams.m_savedCameras.size())
	{
		m_lstBMCameras.DeleteItem(_idx);
		m_cams.m_savedCameras.erase(m_cams.m_savedCameras.begin() + _idx);
	} 
	else
		ASSERT(FALSE);
}

void CDlgBookmarkedCamerasRender::OnToolbarbuttonDEL()
{
	ASSERT(m_lstBMCameras.GetSelectedCount() > 0);
	POSITION pos = m_lstBMCameras.GetFirstSelectedItemPosition();
	std::vector<int> vSelIdx;
	while(pos != NULL) {
		vSelIdx.push_back(m_lstBMCameras.GetNextSelectedItem(pos));
	}
	std::sort(vSelIdx.begin(), vSelIdx.end());
	for(std::vector<int>::reverse_iterator rit=vSelIdx.rbegin();
		rit!=vSelIdx.rend();
		rit++) {
		DeleteListItem(*rit);
	}

	UpdateToolbar();
}

void CDlgBookmarkedCamerasRender::OnToolbarbuttonEDIT()
{
	ASSERT(m_lstBMCameras.GetSelectedCount() == 1);
	POSITION pos = m_lstBMCameras.GetFirstSelectedItemPosition();

	m_lstBMCameras.EditLabel(m_lstBMCameras.GetNextSelectedItem(pos));

	UpdateToolbar();
}

void CDlgBookmarkedCamerasRender::OnToolbarbuttonADD()
{
	m_cams.AddSavedCameraDesc(_T("       "));
	int idx = m_cams.m_savedCameras.size() - 1;
	ASSERT(idx == m_lstBMCameras.GetItemCount());
	m_lstBMCameras.InsertItem(LVIF_TEXT, idx, LPSTR_TEXTCALLBACK, 0, 0, 0, NULL);
	m_lstBMCameras.SetItemState(idx, LVIS_SELECTED, LVIS_SELECTED);
	m_lstBMCameras.SetFocus();
	m_lstBMCameras.EditLabel(idx);

	UpdateToolbar();
}

void CDlgBookmarkedCamerasRender::OnBeginLabelEditListBMCameras(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;

	m_bLabelEditing = true;
	UpdateToolbar();
	
	*pResult = 0;
}

void CDlgBookmarkedCamerasRender::OnGetDispInfoListBMCameras(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;

	if (pDispInfo->item.mask & LVIF_TEXT)
	{
		int nIndex = pDispInfo->item.iItem;
		if (nIndex <= m_cams.m_savedCameras.size())
		{
			const C3DViewCameraDesc& camDesc = m_cams.m_savedCameras[nIndex];
			switch(pDispInfo->item.iSubItem)
			{
			case 0:
				lstrcpy(pDispInfo->item.pszText, camDesc.GetName());
				break;
			case 1:
				lstrcpy( pDispInfo->item.pszText,
					camDesc.GetDefaultCameraData().GetProjectionType()==CCameraData::perspective ? _T("3D") : _T("2D")
					);
				break;
			case 2:
				lstrcpy(pDispInfo->item.pszText,
					(camDesc.GetPaneInfo().GetColCount()>C3DViewPaneInfo::MinColNum || camDesc.GetPaneInfo().GetRowCount()>C3DViewPaneInfo::MinRowNum) ? _T("Yes") : _T("No")
					);
				break;
			default:
				ASSERT(FALSE);
				break;
			}
		}
		else
			ASSERT(FALSE);
	}

	*pResult = 0;
}

void CDlgBookmarkedCamerasRender::UpdateToolbar()
{
	m_toolBar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONADD,!m_bLabelEditing && m_bAllowAdd);
	m_toolBar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONEDIT,!m_bLabelEditing && m_lstBMCameras.GetSelectedCount() == 1);
	m_toolBar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONDEL,!m_bLabelEditing && m_lstBMCameras.GetSelectedCount());
}
void CDlgBookmarkedCamerasRender::OnLvnItemchangedListBmcameras(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMListView = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here

	if (pNMListView->uChanged & LVIF_STATE) 
	{ 
		UpdateToolbar();
	} 

	*pResult = 0;
}
