// DlgAreaPortal.cpp : implementation file
//

#include "stdafx.h" 
#include "termplan.h"
#include "DlgAreaPortal.h"
#include "TermPlanDoc.h"
#include "../common/UnitsManager.h"
#include "Landside\InputLandside.h"
#include <CommonData/Fallback.h>
#include "common\WinMsg.h"
#include ".\dlgareaportal.h"
#include "MainFrm.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static CString GetCoordString(const std::vector<ARCPoint2>& _vList)
{
	CString s;
	CString sTmp;
	for(int i=0; i<static_cast<int>(_vList.size()); i++) {
		sTmp.Format("(%.2f,%.2f) ", UNITSMANAGER->ConvertLength(_vList[i][VX]), UNITSMANAGER->ConvertLength(_vList[i][VX]));
		s += sTmp;
	}
	return s;
}
/////////////////////////////////////////////////////////////////////////////
// CDlgAreaPortal dialog


CDlgAreaPortal::CDlgAreaPortal(CTermPlanDoc* pDoc,_areaportal type, CNamedPointList* pNamedList, CWnd* pParent /*=NULL*/,CWnd* pOwner)
	: CDialog(CDlgAreaPortal::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgAreaPortal)
	m_sCoords = _T("");
	m_sName = _T("");
	m_sNameTag = _T("");
	//}}AFX_DATA_INIT
	m_pDoc=pDoc;
	m_pOwner=pOwner;
	m_pNamedList = pNamedList;
	m_pParent = pParent;
	m_eType = type;
	if(m_pNamedList != NULL) {
		m_sName	= m_pNamedList->name;
		m_sCoords = GetCoordString(m_pNamedList->points);
	}
	m_Points.reserve(m_pNamedList->points.size());
	m_Points = m_pNamedList->points;
}


void CDlgAreaPortal::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgAreaPortal)
	DDX_Text(pDX, IDC_EDIT_COORDS, m_sCoords);
	DDX_Text(pDX, IDC_EDIT_NAME, m_sName);
	DDX_Text(pDX, IDC_STATIC_NAMETAG, m_sNameTag);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgAreaPortal, CDialog)
	//{{AFX_MSG_MAP(CDlgAreaPortal)
	ON_BN_CLICKED(IDC_PFM_COORDS, OnPfmCoords)
	//}}AFX_MSG_MAP
	ON_MESSAGE(TP_DATA_BACK, OnTempFallbackFinished)
	ON_EN_CHANGE(IDC_EDIT_NAME, OnEnChangeEditName)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgAreaPortal message handlers

void CDlgAreaPortal::OnPfmCoords() 
{
	if (m_pDoc->GetCurrentMode() == EnvMode_LandSide)
	{
		CMDIChildWnd* pWnd = m_pDoc->GetMainFrame()->CreateOrActiveRender3DView();
		if(!pWnd)
			return;
	}
	CWnd* wnd = GetParent();
	HideDialog(wnd);
	FallbackData data;
	data.hSourceWnd = GetSafeHwnd();

	//m_pListToSet = m_pNamedList->points;
	//m_pStrToSet = &m_sCoords;

	UpdateData(TRUE);
	LPARAM lParam = (LPARAM) ((m_eType==area)?PICK_MANYPOINTS:PICK_TWOPOINTS);
	
	if(!wnd->SendMessage(TP_TEMP_FALLBACK, (WPARAM) &data, lParam )) {
		AfxMessageBox("error");
		wnd = GetParent();
		ShowDialog(wnd);
	}
	if(m_pOwner)
		m_pOwner->SendMessage(UM_DLGAREAPORTAL_SHOW_OR_HIDE,0);
}


int CDlgAreaPortal::DoFakeModal()
{
	if(CDialog::Create(CDlgAreaPortal::IDD, m_pParent)) {
		CenterWindow();
		ShowWindow(SW_SHOW);
		GetParent()->EnableWindow(FALSE);
		EnableWindow();
		int nReturn = RunModalLoop();
		DestroyWindow();
		return nReturn;
	}
	else
		return IDCANCEL;
}

void CDlgAreaPortal::HideDialog(CWnd* parentWnd)
{
	ShowWindow(SW_HIDE);
	while(!parentWnd->IsKindOf(RUNTIME_CLASS(CMDIFrameWnd))) {
		parentWnd->ShowWindow(SW_HIDE);
		parentWnd = parentWnd->GetParent();
	}
	parentWnd->EnableWindow(TRUE);
	parentWnd->SetActiveWindow();
}

void CDlgAreaPortal::ShowDialog(CWnd* parentWnd)
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

int CDlgAreaPortal::DoModal()
{
	return CDialog::DoModal();
}

BOOL CDlgAreaPortal::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	if(m_eType == area) {
		m_sNameTag = _T("Area Name");
		this->SetWindowText(_T("Area Definition Dialog"));
	}
	else if(m_eType == portal) {
		m_sNameTag = _T("Portal Name");
		this->SetWindowText(_T("Portal Definition Dialog"));
	}

	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


LRESULT CDlgAreaPortal::OnTempFallbackFinished(WPARAM wParam, LPARAM lParam)
{
	if(m_pOwner)
		m_pOwner->SendMessage(UM_DLGAREAPORTAL_SHOW_OR_HIDE,1);
	CWnd* wnd = GetParent();
	while (!wnd->IsKindOf(RUNTIME_CLASS(CMDIFrameWnd)) )
	{
		wnd->ShowWindow(SW_SHOW);
		wnd->EnableWindow(false);
		wnd = wnd->GetParent();
	}
	wnd->EnableWindow(false);
	ShowWindow(SW_SHOW);	
	EnableWindow();
	
	std::vector<ARCVector3>* pData = reinterpret_cast< std::vector<ARCVector3>* >(wParam);

	m_Points.clear();
	for(int i=0; i<static_cast<int>(pData->size()); i++) {
		m_Points.push_back(ARCPoint2(pData->at(i)[VX], pData->at(i)[VY]));
	}

	m_sCoords = GetCoordString(m_Points);

	UpdateData(FALSE);

#ifdef _DEBUG
	// TRACE("Points Selected: ");
	for(int i=0; i<static_cast<int>(m_Points.size()); i++) {
		// TRACE("(%.2f, %.2f), ", m_Points[i][VX], m_Points[i][VY]);
	}
	// TRACE("\n");
#endif
	
	return TRUE;
}

void CDlgAreaPortal::OnOK() 
{
	UpdateData(TRUE);

	CString strBlank = "";

    //when add new Area or Portal,check the name exist or not
	//when modify the name of the Area or Portal,check the name exist or not
	if (m_pNamedList->name == strBlank || m_pNamedList->name != m_sName)
	{	
		if(m_sName.IsEmpty()) {
			CString s;
			if(m_eType == area)
				AfxMessageBox(_T("Please enter a name for the area."), MB_ICONEXCLAMATION | MB_OK);
			else
				AfxMessageBox(_T("Please enter a name for the portal."), MB_ICONEXCLAMATION | MB_OK);
			return;
		}

		m_sName.TrimLeft();
		m_sName.TrimRight();

		if(m_eType==portal)
		{
			CString strTemp1;
			CString strTemp2;
			if (m_pDoc->m_systemMode == EnvMode_Terminal)
			{
				CPortalList::iterator it;
				for(it=m_pDoc->m_portals.m_vPortals.begin();
					it!=m_pDoc->m_portals.m_vPortals.end();it++)
				{
					strTemp1=(*it)->name;
					strTemp2=m_sName;
					strTemp1.MakeLower();
					strTemp2.MakeLower();
					if(strTemp1==strTemp2)
					{
						AfxMessageBox(_T("This name has existed,Please enter another different name than this name."));
						return;
					}
				}
			}
			if (m_pDoc->m_systemMode == EnvMode_LandSide)
			{
				CLandsidePortals* pPortals = &(m_pDoc->GetInputLandside()->GetPortals());
				for (int i = 0; i < pPortals->getCount(); i++)
				{
					CLandsidePortal* pPortal = (CLandsidePortal*)pPortals->getLayoutObject(i);
					strTemp1 = pPortal->name;
					strTemp2 = m_sName;
					strTemp1.MakeLower();
					strTemp2.MakeLower();
					if(strTemp1==strTemp2)
					{
						AfxMessageBox(_T("This name has existed,Please enter another different name than this name."));
						return;
					}
				}
			}
			
		}
		if(m_eType==area)
		{
			CString strTemp1;
			CString strTemp2;
			if (m_pDoc->m_systemMode == EnvMode_Terminal)
			{
				CAreaList::iterator it;
				for(it=m_pDoc->GetTerminal().m_pAreas->m_vAreas.begin();
				it!=m_pDoc->GetTerminal().m_pAreas->m_vAreas.end();it++)
				{
					strTemp1=(*it)->name;
					strTemp2=m_sName;
					strTemp1.MakeLower();
					strTemp2.MakeLower();
					if(strTemp1==strTemp2)
					{
						AfxMessageBox(_T("This name has existed,Please enter another different name than this name."));
						return;
					 }
				 }
			 }
			if (m_pDoc->m_systemMode == EnvMode_LandSide)
			{
				CLandsideAreas* pAreas = &(m_pDoc->GetInputLandside()->GetLandsideAreas());
				for (int i = 0; i < pAreas->getCount(); i++)
				{
					CLandsideArea* pArea = (CLandsideArea*)pAreas->getLayoutObject(i);
					strTemp1 = pArea->name;
					strTemp2 = m_sName;
					strTemp1.MakeLower();
					strTemp2.MakeLower();
					if(strTemp1==strTemp2)
					{
						AfxMessageBox(_T("This name has existed,Please enter another different name than this name."));
						return;
					}
				}
			}
		}

		if(m_eType == portal && m_Points.size() != 2) {
			AfxMessageBox(_T("Please choose 2 points to define the portal."), MB_ICONEXCLAMATION | MB_OK);
			return;
		}
		if(m_eType == area && m_Points.size() < 3) {
			AfxMessageBox(_T("Please choose at least 3 points to define the area."), MB_ICONEXCLAMATION | MB_OK);
			return;
		}

		m_pNamedList->name = m_sName;
		m_pNamedList->points = m_Points;
		m_pNamedList->floor = m_pDoc->m_nActiveFloor;

		CDialog::OnOK();
	}
	else//modify the Area or Portal,when the Area's or Portal's name is not change
	{
		if(m_sName.IsEmpty()) {
			CString s;
			if(m_eType == area)
				AfxMessageBox(_T("Please enter a name for the area."), MB_ICONEXCLAMATION | MB_OK);
			else
				AfxMessageBox(_T("Please enter a name for the portal."), MB_ICONEXCLAMATION | MB_OK);
			return;
		}

		if(m_eType == portal && m_Points.size() != 2) {
			AfxMessageBox(_T("Please choose 2 points to define the portal."), MB_ICONEXCLAMATION | MB_OK);
			return;
		}
		if(m_eType == area && m_Points.size() < 3) {
			AfxMessageBox(_T("Please choose at least 3 points to define the area."), MB_ICONEXCLAMATION | MB_OK);
			return;
		}

		m_pNamedList->name = m_sName;
		m_pNamedList->points = m_Points;
		m_pNamedList->floor = m_pDoc->m_nActiveFloor;

		CDialog::OnOK();
	}
}

void CDlgAreaPortal::OnEnChangeEditName()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
}

