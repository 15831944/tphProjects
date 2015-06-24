// ModifyLatLong.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "ModifyLatLong.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CModifyLatLong dialog


CModifyLatLong::CModifyLatLong(CWnd* pParent /*=NULL*/)
	: CDialog(CModifyLatLong::IDD, pParent)
{
	//{{AFX_DATA_INIT(CModifyLatLong)
	m_strLat = _T("");
	m_strLong = _T("");
	//}}AFX_DATA_INIT
}


void CModifyLatLong::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CModifyLatLong)
	DDX_Control(pDX, IDC_EDIT_LONG, m_edtLong);
	DDX_Control(pDX, IDC_EDIT_LAT, m_edtLat);
	DDX_Text(pDX, IDC_EDIT_LAT, m_strLat);
	DDX_Text(pDX, IDC_EDIT_LONG, m_strLong);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CModifyLatLong, CDialog)
	//{{AFX_MSG_MAP(CModifyLatLong)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CModifyLatLong message handlers

int CModifyLatLong::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here

	
	return 0;
}
