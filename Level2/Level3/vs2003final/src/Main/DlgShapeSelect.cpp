// DlgShapeSelect.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "ShapesManager.h"
#include "DlgShapeSelect.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgShapeSelect dialog


CDlgShapeSelect::CDlgShapeSelect(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgShapeSelect::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgShapeSelect)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pSelectedShape = NULL;
}


void CDlgShapeSelect::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgShapeSelect)
	DDX_Control(pDX, IDC_SHAPESELECT_LIST, m_lstShapes);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgShapeSelect, CDialog)
	//{{AFX_MSG_MAP(CDlgShapeSelect)
	ON_WM_CREATE()
	ON_LBN_SELCHANGE(IDC_SHAPESELECT_LIST, OnSelChangeShapeList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgShapeSelect message handlers

BOOL CDlgShapeSelect::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CShape::CShapeList* pSL = SHAPESMANAGER->GetShapeList();
	ASSERT(pSL != NULL);
	m_lstShapes.m_vImgSizes.reserve(pSL->size());
	for(int i=0; i<static_cast<int>(pSL->size()); i++) {
		ASSERT(pSL->at(i) != NULL);
		CBitmap* pBM = pSL->at(i)->GetShapeImageBitmap();
		ASSERT(pBM != NULL);
		BITMAP bm;
		VERIFY(pBM->GetBitmap(&bm));
		m_lstShapes.m_vImgSizes.push_back(CSize(bm.bmWidth,bm.bmHeight));
	}
	for(i=0; i<static_cast<int>(pSL->size()); i++) {
		m_lstShapes.AddItem(pSL->at(i));
	}

	CWnd* pOK = GetDlgItem(IDOK);
		pOK->EnableWindow(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

int CDlgShapeSelect::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	//m_lstShapes.Create(WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_BORDER | LBS_NOTIFY |
	
	return 0;
}

void CDlgShapeSelect::OnSelChangeShapeList() 
{
	int nSelIdx = m_lstShapes.GetCurSel();
	if(nSelIdx != LB_ERR) {
		CShape* pSelShape = (CShape*) m_lstShapes.GetItemData(nSelIdx);
		ASSERT(pSelShape != NULL);
		m_pSelectedShape = pSelShape;
		CWnd* pOK = GetDlgItem(IDOK);
		pOK->EnableWindow(TRUE);
	}
	else {
		CWnd* pOK = GetDlgItem(IDOK);
		pOK->EnableWindow(FALSE);
		m_pSelectedShape = NULL;
	}
}
