// ViewBar.cpp: implementation of the CViewBar class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "termplan.h"
#include "ViewBar.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CViewBar::CViewBar()
{

}

CViewBar::~CViewBar()
{

}

BEGIN_MESSAGE_MAP(CViewBar, CToolBar24X24)
	//{{AFX_MSG_MAP(CViewBar)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

int CViewBar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CToolBar24X24::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	InitImageList(IDB_LAYOUTTOOLBARGREY,IDB_LAYOUTTOOLBAR,IDB_LAYOUTTOOLBARWHITE,RGB(255,0,255));
	SetImageList();
	
	TBBUTTON tbb;
	CToolBar24X24::InitTBBUTTON(tbb) ;
	
	tbb.iBitmap = 0;
	tbb.idCommand = ID_LAYOUTLOCK;
	AddButton(&tbb);
	
	
	tbb.iBitmap = 1;
	tbb.idCommand = ID_DISTANCEMEASURE;
	AddButton(&tbb);
	
	tbb.iBitmap = 0;
	tbb.idCommand = ID_LAYOUTLOCK;
	AddButton(&tbb);
	
	
	tbb.iBitmap = 1;
	tbb.idCommand = ID_DISTANCEMEASURE;
	AddButton(&tbb);

	tbb.iBitmap = 0;
	tbb.idCommand = ID_LAYOUTLOCK;
	AddButton(&tbb);
	
	
	tbb.iBitmap = 1;
	tbb.idCommand = ID_DISTANCEMEASURE;
	AddButton(&tbb);
	
	tbb.iBitmap = 0;
	tbb.idCommand = ID_LAYOUTLOCK;
	AddButton(&tbb);
	
	
	tbb.iBitmap = 1;
	tbb.idCommand = ID_DISTANCEMEASURE;
	AddButton(&tbb);
	
	SetWindowText (_T ("View"));

	EnableDocking(CBRS_ALIGN_ANY);
	
	return 0;
}

