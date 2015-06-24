// CarScheduleSheet.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "termplan.h"
#include "CarScheduleSheet.h"
#include "StationSheet.h"
#include "CarScheduleDlg.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCarScheduleSheet

IMPLEMENT_DYNAMIC(CCarScheduleSheet, CPropertySheet)

CCarScheduleSheet::CCarScheduleSheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{

}

CCarScheduleSheet::CCarScheduleSheet(LPCTSTR pszCaption, TrainTrafficGraph* _pTrafficGraph,CAllCarSchedule* _pDocCarSchedule,CWnd* pParentWnd , UINT iSelectPage )
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{

		m_pProcDataPage[0] = new CCarScheduleDlg( _pTrafficGraph, _pDocCarSchedule, pParentWnd );
		m_pProcDataPage[1] = new CStationSheet( _pTrafficGraph,((CCarScheduleDlg*)m_pProcDataPage[0])->GetAllCarSchedule() );
		
		AddPage( m_pProcDataPage[0] );
		AddPage( m_pProcDataPage[1] );

}

CCarScheduleSheet::~CCarScheduleSheet()
{
	delete m_pProcDataPage[0];
	delete m_pProcDataPage[1];
}


BEGIN_MESSAGE_MAP(CCarScheduleSheet, CPropertySheet)
	//{{AFX_MSG_MAP(CCarScheduleSheet)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCarScheduleSheet message handlers
