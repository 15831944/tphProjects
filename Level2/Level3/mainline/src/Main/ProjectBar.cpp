// ProjectBar.cpp: implementation of the CProjectBar class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "termplan.h"
#include "ProjectBar.h"
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CProjectBar::CProjectBar()
{

}

CProjectBar::~CProjectBar()
{

}

BEGIN_MESSAGE_MAP(CProjectBar, CToolBar24X24)
	//{{AFX_MSG_MAP(CMainBar)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP

END_MESSAGE_MAP()

int CProjectBar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CToolBar24X24::OnCreate(lpCreateStruct) == -1)
		return -1;

	SetSizes(CSize(129,32), CSize(24,24));

	InitImageList(IDB_TOOLBAR_MAIN_NORMAL,IDB_TOOLBAR_MAIN_HOT,IDB_TOOLBAR_MAIN_DISABLE,RGB(255,0,255));
	SetImageList();
	//Insert a temp button(size 2)
	TBBUTTON tbb;
	CToolBar24X24::InitTBBUTTON(tbb) ;
	tbb.iBitmap = -1;
	tbb.idCommand =TEMP_ID_FOR_NOT_BUTTON+m_nCountNotButton;
	tbb.fsState &=~ TBSTATE_ENABLED;
	AddButton(&tbb,2,TRUE);
	
	AddTextComboBox(" Project:  ",&m_cbUint3,CSize(250,160),IDC_COMBO_PROJSELECT,CBS_DROPDOWNLIST);

	SetWindowText (_T ("Project"));

	EnableDocking(CBRS_ALIGN_ANY);
	
	
	return 0;
}


CComboBox* CProjectBar::GetComboBoxProj()
{
	return &m_cbUint3;
}



