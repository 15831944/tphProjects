// CameraBar.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "CameraBar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCameraBar

CCameraBar::CCameraBar()
{
}

CCameraBar::~CCameraBar()
{
}


BEGIN_MESSAGE_MAP(CCameraBar, CToolBar24X24)
	//{{AFX_MSG_MAP(CCameraBar)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCameraBar message handlers

int CCameraBar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CToolBar24X24::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	InitImageList(IDB_TOOLBAR_CAMERA_NORMAL,IDB_TOOLBAR_CAMERA_HOT,IDB_TOOLBAR_CAMERA_DISABLE,RGB(255,0,255));
	SetImageList();
	
	TBBUTTON tbb;
	CToolBar24X24::InitTBBUTTON(tbb) ;
	
	tbb.iBitmap = 0;
	tbb.idCommand = ID_CAMTUMBLE;
	AddButton(&tbb);
	
	
	tbb.iBitmap = 1;
	tbb.idCommand = ID_CAMDOLLY;
	AddButton(&tbb);
	
	tbb.iBitmap = 2;
	tbb.idCommand = ID_CAMPAN;
	AddButton(&tbb);
	
	
	SetWindowText (_T ("Camera"));

	EnableDocking(CBRS_ALIGN_ANY);
	
	return 0;
}
