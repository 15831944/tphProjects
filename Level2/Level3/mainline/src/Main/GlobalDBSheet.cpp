// GlobalDBSheet.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "GlobalDBSheet.h"
#include "GlobalDbmanDlg.h"
#include "SetProjectDBDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGlobalDBSheet

IMPLEMENT_DYNAMIC(CGlobalDBSheet, CPropertySheet)

CGlobalDBSheet::CGlobalDBSheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
}

CGlobalDBSheet::CGlobalDBSheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
	CGlobalDBManDlg* _pDBManDlg = new CGlobalDBManDlg( pParentWnd );
	_pDBManDlg->setAllProjectList( &m_allProjInfo );
	CSetProjectDBDlg* _pSetProjectDBDlg = new CSetProjectDBDlg(  pParentWnd );
	_pSetProjectDBDlg->setAllProjectList( &m_allProjInfo );

	AddPage( _pDBManDlg );
	AddPage( _pSetProjectDBDlg );

	m_PropertyPageArr[0]= _pDBManDlg;
	m_PropertyPageArr[1]= _pSetProjectDBDlg;
}

CGlobalDBSheet::~CGlobalDBSheet()
{
	for( int i=0; i< 2; i++ )
	{
		if( m_PropertyPageArr[i] )
			delete m_PropertyPageArr[i];
	}
	
	clearAllProjectInfo();
}


BEGIN_MESSAGE_MAP(CGlobalDBSheet, CPropertySheet)
	//{{AFX_MSG_MAP(CGlobalDBSheet)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGlobalDBSheet message handlers

BOOL CGlobalDBSheet::OnInitDialog() 
{
	BOOL bResult = CPropertySheet::OnInitDialog();
	
	loadAllProjectInfo();
	return bResult;
}



// load all project info
void CGlobalDBSheet::loadAllProjectInfo( void )
{
	// first delete all old data
	clearAllProjectInfo();
	
	// get all project info
	std::vector<CString> vList;
	PROJMANAGER->GetProjectList(vList);
	for(int i=0; i<static_cast<int>(vList.size()); i++) 
	{
		PROJECTINFO* pi = new PROJECTINFO();
		PROJMANAGER->GetProjectInfo(vList[i], pi);
		
		m_allProjInfo.push_back( pi );
	}	
}

// clear all project info
void CGlobalDBSheet::clearAllProjectInfo( void )
{
	for( int i =0; i< static_cast<int>(m_allProjInfo.size()); i++ )
		delete m_allProjInfo[i];
	
	m_allProjInfo.clear();
}
