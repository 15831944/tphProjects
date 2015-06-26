#include "stdafx.h"
#include "termplan.h"
#include "OpenComparativeReportsGroup.h"
#include "../compare/ComparativeReportManager.h"
#include "../compare/ModelsManager.h"
#include "../common/ProjectManager.h"
#include "DlgAddNewCmpReport.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CDlgOpenComparativeReport::CDlgOpenComparativeReport(CWnd* pParent /*=NULL*/)
    : CDialog(CDlgOpenComparativeReport::IDD, pParent),
    m_oldCx(0),
    m_oldCy(0)
{
    //{{AFX_DATA_INIT(COpenComparativeReportsGroup)
    // NOTE: the ClassWizard will add member initialization here
    //}}AFX_DATA_INIT
}


void CDlgOpenComparativeReport::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(COpenComparativeReportsGroup)
    DDX_Control(pDX, IDC_LISTREPORTGROUPS, m_ListReportsGroup);
    //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgOpenComparativeReport, CDialog)
    //{{AFX_MSG_MAP(COpenComparativeReportsGroup)
    ON_WM_SIZE()
    ON_NOTIFY(NM_DBLCLK, IDC_LISTREPORTGROUPS, OnDblclkProjlist)
    ON_NOTIFY(LVN_COLUMNCLICK, IDC_LISTREPORTGROUPS, OnColumnclickListreportgroups)
    //}}AFX_MSG_MAP
    ON_COMMAND(ID_TOOLBARBUTTONADD, OnAddProject)
    ON_COMMAND(ID_TOOLBARBUTTONEDIT, OnEditProject)
    ON_COMMAND(ID_TOOLBARBUTTONDEL, OnDelProject)
    ON_WM_CREATE()
    ON_WM_GETMINMAXINFO()
    ON_NOTIFY(LVN_ITEMCHANGED, IDC_LISTREPORTGROUPS, &CDlgOpenComparativeReport::OnLvnItemchangedListreportgroups)
END_MESSAGE_MAP()


int CDlgOpenComparativeReport::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CDialog::OnCreate(lpCreateStruct) == -1)
        return -1;

    if (!m_toolbar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
        | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
        !m_toolbar.LoadToolBar(IDR_ADDDELEDITTOOLBAR))
    {
        TRACE0("Failed to create toolbar\n");
        return -1;
    }
    return 0;
}

void CDlgOpenComparativeReport::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
    lpMMI->ptMinTrackSize.x = 300;
    lpMMI->ptMinTrackSize.y = 200;
    CDialog::OnGetMinMaxInfo(lpMMI);
}

BOOL CDlgOpenComparativeReport::OnInitDialog() 
{
    CDialog::OnInitDialog();

    InitListCtrl();
    FillProjectList();

    CRect rectClient;
    GetClientRect(&rectClient);
    m_oldCx = rectClient.Width();
    m_oldCy = rectClient.Height();

    CRect rect;
    m_ListReportsGroup.GetWindowRect(rect);
    ScreenToClient(rect);
    m_toolbar.SetWindowPos(NULL,rect.left,rect.top-26,rect.Width(),26,NULL);

    m_toolbar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONEDIT, FALSE);
    m_toolbar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONDEL, FALSE);

    return TRUE;
}

void CDlgOpenComparativeReport::OnSize(UINT nType, int cx, int cy)
{
    CDialog::OnSize(nType, cx, cy);
    CRect rectTemp;
    GetWindowRect(rectTemp);
    ScreenToClient(rectTemp);
    if(m_toolbar.m_hWnd != NULL)
    {
        LayoutControl(&m_ListReportsGroup, TopLeft, BottomRight, cx, cy);
        LayoutControl(GetDlgItem(IDCANCEL), BottomRight, BottomRight, cx, cy);
        LayoutControl(GetDlgItem(IDOK), BottomRight, BottomRight, cx, cy);
        LayoutControl(&m_toolbar, TopLeft, TopRight, cx, cy);
    }

    if (nType != SIZE_MINIMIZED)
    {
        m_oldCx = cx;
        m_oldCy = cy;
    }
    InvalidateRect(NULL);
}

void CDlgOpenComparativeReport::InitListCtrl()
{
    DWORD dwStyle = m_ListReportsGroup.GetExtendedStyle();
    dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES;
    m_ListReportsGroup.SetExtendedStyle(dwStyle);

    char columnLable[][128]={"Name", "Description", "User", "Machine", "Date Created", "Last Modified"};

    CRect rclist;
    CStringList strList;
    m_ListReportsGroup.GetWindowRect(rclist);
    int defaultColumnwidth[]={rclist.Width()*2/9, rclist.Width()/9, 
        rclist.Width()/9, rclist.Width()/9, rclist.Width()*2/9-1, rclist.Width()*2/9-1};
    int _iFormat[]={LVCFMT_LEFT|LVCFMT_NOEDIT,LVCFMT_LEFT|LVCFMT_NOEDIT, LVCFMT_LEFT|LVCFMT_NOEDIT,
        LVCFMT_LEFT|LVCFMT_NOEDIT, LVCFMT_LEFT|LVCFMT_NOEDIT, LVCFMT_LEFT|LVCFMT_NOEDIT};
    EDataType dt[] = {dtSTRING, dtSTRING, dtSTRING, dtSTRING, dtDATETIME, dtDATETIME};

    m_headerCtl.SubclassWindow(m_ListReportsGroup.GetHeaderCtrl()->m_hWnd);
    for(int i=0; i<6; i++)
    {
        m_ListReportsGroup.InsertColumn(i, columnLable[i], _iFormat[i], defaultColumnwidth[i]);
        m_headerCtl.SetDataType(i, dt[i]);
    }
}

void CDlgOpenComparativeReport::FillProjectList()
{
    for (int nCmp = 0; nCmp < CMPPROJECTMANAGER->getProjectCount(); nCmp++)
    {
        CComparativeProject *pProject = CMPPROJECTMANAGER->getCmpProject(nCmp);
        ASSERT(pProject != NULL);
        AddProjInfoToListEnd(pProject);
    }

}

void CDlgOpenComparativeReport::OnColumnclickListreportgroups(NMHDR* pNMHDR, LRESULT* pResult) 
{
    NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
    int nTestIndex = pNMListView->iSubItem;
    if( nTestIndex >= 0 )
    {
        CWaitCursor wCursor;
        if(::GetKeyState( VK_CONTROL ) < 0 ) // Is <CTRL> Key Down
            m_headerCtl.SortColumn( nTestIndex, MULTI_COLUMN_SORT );
        else
            m_headerCtl.SortColumn( nTestIndex, SINGLE_COLUMN_SORT );
        m_headerCtl.SaveSortList();
    }
    *pResult = 0;
}

int CDlgOpenComparativeReport::GetSelIndex()
{
    int i= m_ListReportsGroup.GetItemCount()-1;
    for( ; i>-1; i-- )
    {
        if( ( m_ListReportsGroup.GetItemState(i,LVIS_SELECTED) & LVIS_SELECTED ) == LVIS_SELECTED )
        {
            break;
        }
    }
    if( i == -1 )
        return -1;

    return i;
}

void CDlgOpenComparativeReport::LayoutControl(CWnd* pCtrl, LayoutRef refTopLeft, LayoutRef refBottomRight, int cx, int cy)
{
    CRect rcS;
    pCtrl->GetWindowRect(&rcS);
    ScreenToClient(&rcS);
    int deltaX = cx - m_oldCx;
    int deltaY = cy - m_oldCy;
    if(refTopLeft == TopLeft && refBottomRight == TopLeft)
    {
        pCtrl->MoveWindow(&rcS);
    }
    else if(refTopLeft == TopLeft && refBottomRight == TopRight)
    {
        pCtrl->MoveWindow(rcS.left, rcS.top, rcS.Width()+deltaX, rcS.Height());
    }
    else if(refTopLeft == TopLeft && refBottomRight == BottomLeft)
    {
        pCtrl->MoveWindow(rcS.left, rcS.top, rcS.Width(), rcS.Height()+deltaY);
    }
    else if(refTopLeft == TopLeft && refBottomRight == BottomRight)
    {
        pCtrl->MoveWindow(rcS.left, rcS.top, rcS.Width()+deltaX, rcS.Height()+deltaY);
    }
    else if(refTopLeft == TopRight && refBottomRight == TopRight)
    {
        pCtrl->MoveWindow(rcS.left+deltaX, rcS.top, rcS.Width(), rcS.Height());
    }
    else if(refTopLeft == TopRight && refBottomRight == BottomRight)
    {
        pCtrl->MoveWindow(rcS.left+deltaX, rcS.top, rcS.Width(), cy+deltaY);
    }
    else if(refTopLeft == BottomLeft && refBottomRight == BottomLeft)
    {
        pCtrl->MoveWindow(rcS.left, rcS.top+deltaY, rcS.Width(), rcS.Height());
    }
    else if(refTopLeft == BottomLeft && refBottomRight == BottomRight)
    {
        pCtrl->MoveWindow(rcS.left, rcS.top+deltaY, cx+deltaX, rcS.Height());
    }
    else if(refTopLeft == BottomRight && refBottomRight == BottomRight)
    {
        pCtrl->MoveWindow(rcS.left+deltaX, rcS.top+deltaY, rcS.Width(), rcS.Height());
    }
}

void CDlgOpenComparativeReport::OnOK() 
{
    int nCurSel = GetSelIndex();

    if (nCurSel == -1)
    {
        AfxMessageBox(_T("Please select an item."));
        return;
    }

    m_strName = m_ListReportsGroup.GetItemText(nCurSel, 0);
    m_strDesc = m_ListReportsGroup.GetItemText(nCurSel, 1);

    CDialog::OnOK();
}

void CDlgOpenComparativeReport::OnCancel() 
{
    CDialog::OnCancel();
}

void CDlgOpenComparativeReport::OnDblclkProjlist(NMHDR* pNMHDR, LRESULT* pResult)
{
    OnOK();
}

void CDlgOpenComparativeReport::OnAddProject()
{
    DlgAddNewCmpReport dlg;
    if(dlg.DoModal() == IDOK)
    {
        CString strName = dlg.m_strName;
        CString strDesc = dlg.m_strDesc;
        CComparativeProject* pNewProj = CMPPROJECTMANAGER->AddNewProject(strName, strDesc);
        CMPPROJECTMANAGER->saveDataSet(PROJMANAGER->GetAppPath(), false);
        AddProjInfoToListEnd(pNewProj);
    }
}

void CDlgOpenComparativeReport::OnEditProject()
{
    int nCurSel = GetSelIndex();
    ASSERT(nCurSel != -1);

    CComparativeProject* pCmpProj = (CComparativeProject*)m_ListReportsGroup.GetItemData(nCurSel);
    ASSERT(pCmpProj != NULL);

    CString strCmpProj = pCmpProj->GetName();
    CString strDesc = pCmpProj->GetDescription();

    DlgEditCmpReport dlg;
    dlg.SetCmpProj(pCmpProj);
    if(dlg.DoModal() == IDOK)
    {
        pCmpProj->SetName(dlg.m_strName);
        pCmpProj->SetDescription(dlg.m_strDesc);
        CMPPROJECTMANAGER->saveDataSet(PROJMANAGER->GetAppPath(), false);
        m_ListReportsGroup.SetItemText(nCurSel, 0, pCmpProj->GetName());
        m_ListReportsGroup.SetItemText(nCurSel, 1, pCmpProj->GetDescription());
    }
}

void CDlgOpenComparativeReport::OnDelProject()
{
    int nCurSel = GetSelIndex();
    ASSERT(nCurSel != -1);

    CComparativeProject* pCmpProj = (CComparativeProject*)m_ListReportsGroup.GetItemData(nCurSel);
    ASSERT(pCmpProj != NULL);

    CString strCmpProj = pCmpProj->GetName();
    CString strMsg;
    strMsg.Format(_T("Delete Comparative Report Project %s?"), strCmpProj);
    if(MessageBox(strMsg, NULL, MB_YESNO) == IDYES)
    {
        CMPPROJECTMANAGER->DeleteProjectByName(strCmpProj);
        CMPPROJECTMANAGER->saveDataSet(PROJMANAGER->GetAppPath(), false);
        m_ListReportsGroup.DeleteItem(nCurSel);
    }
}

void CDlgOpenComparativeReport::AddProjInfoToListEnd( CComparativeProject * pProject )
{
    int nIndex = m_ListReportsGroup.GetItemCount();
    m_ListReportsGroup.InsertItem(nIndex, pProject->GetName());
    m_ListReportsGroup.SetItemText(nIndex, 1, pProject->GetDescription());
    m_ListReportsGroup.SetItemText(nIndex, 2, pProject->GetUser());
    m_ListReportsGroup.SetItemText(nIndex, 3, pProject->GetServerName());
    CTime t = pProject->GetCreatedTime();
    m_ListReportsGroup.SetItemText(nIndex, 4, t.Format("%m/%d/%y %H:%M:%S"));
    t = pProject->GetLastModifiedTime();
    m_ListReportsGroup.SetItemText(nIndex, 5, t.Format("%m/%d/%y %H:%M:%S"));
    m_ListReportsGroup.SetItemData(nIndex, (DWORD)pProject);
}



void CDlgOpenComparativeReport::OnLvnItemchangedListreportgroups(NMHDR *pNMHDR, LRESULT *pResult)
{
    m_toolbar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONEDIT, FALSE);
    m_toolbar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONDEL, FALSE);

    LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
    int nSel = m_ListReportsGroup.GetSelectedCount();
    if(nSel != 0)
    {
        m_toolbar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONEDIT, TRUE);
        m_toolbar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONDEL, TRUE);
    }

    *pResult = 0;
}
