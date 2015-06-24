// DlgAirsideObjectDisplayProperties.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "DlgAirsideObjectDisplayProperties.h"
#include ".\dlgairsideobjectdisplayproperties.h"
#include "ALTObject3D.h"
#include "../InputAirside/ALTObject.h"
#include "../InputAirside/ALTObjectDisplayProp.h"


const CString CDlgAirsideObjectDisplayProperties::DSPPROPNAME[] ={
	_T("Display Object Name"),
		_T("Display Object Shape"),
		_T("Display In Constraint"),
		_T("Display Out Constraint"),
		_T("Display Taxiway Direction"),
		_T("Display Taxiway Center Line"),
		_T("Display Taxiway Marking"),	
		_T("Display Deice Pad"),
		_T("Display Deice Truck")
};

// CDlgAirsideObjectDisplayProperties dialog

IMPLEMENT_DYNAMIC(CDlgAirsideObjectDisplayProperties, CDialog)
CDlgAirsideObjectDisplayProperties::CDlgAirsideObjectDisplayProperties(ALTObject3D *p3DObject,CWnd* pParent /*=NULL*/)
	: CDialog(CDlgAirsideObjectDisplayProperties::IDD, pParent)
{
	ASSERT(p3DObject != NULL);
	m_p3DObject = p3DObject;
}

CDlgAirsideObjectDisplayProperties::~CDlgAirsideObjectDisplayProperties()
{
}

void CDlgAirsideObjectDisplayProperties::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Check(pDX, IDC_CHK_DP1, m_nDisplay[0]);
	DDX_Check(pDX, IDC_CHK_DP2, m_nDisplay[1]);
	DDX_Check(pDX, IDC_CHK_DP3, m_nDisplay[2]);
	DDX_Check(pDX, IDC_CHK_DP4, m_nDisplay[3]);
	DDX_Check(pDX, IDC_CHK_DP5, m_nDisplay[4]);
	DDX_Check(pDX, IDC_CHK_DP6, m_nDisplay[5]);
	DDX_Control(pDX, IDC_CHK_DP1, m_chkDisplay[0]);
	DDX_Control(pDX, IDC_CHK_DP2, m_chkDisplay[1]);
	DDX_Control(pDX, IDC_CHK_DP3, m_chkDisplay[2]);
	DDX_Control(pDX, IDC_CHK_DP4, m_chkDisplay[3]);
	DDX_Control(pDX, IDC_CHK_DP5, m_chkDisplay[4]);
	DDX_Control(pDX, IDC_CHK_DP6, m_chkDisplay[5]);

	DDX_Control(pDX, IDC_CHANGECOLOR_DP1, m_btnColor[0]);
	DDX_Control(pDX, IDC_CHANGECOLOR_DP2, m_btnColor[1]);
	DDX_Control(pDX, IDC_CHANGECOLOR_DP3, m_btnColor[2]);
	DDX_Control(pDX, IDC_CHANGECOLOR_DP4, m_btnColor[3]);
	DDX_Control(pDX, IDC_CHANGECOLOR_DP5, m_btnColor[4]);
	DDX_Control(pDX, IDC_CHANGECOLOR_DP6, m_btnColor[5]);
}


BEGIN_MESSAGE_MAP(CDlgAirsideObjectDisplayProperties, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_CHANGECOLOR_DP1, IDC_CHANGECOLOR_DP6, OnChangeColor)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_CHK_DP1, IDC_CHK_DP6, OnChkDisplay)

	ON_MESSAGE(CPN_SELENDOK,     OnSelEndOK)
	ON_MESSAGE(CPN_SELENDCANCEL, OnSelEndCancel)
	ON_MESSAGE(CPN_SELCHANGE,    OnSelChange)
	ON_MESSAGE(CPN_CLOSEUP,         OnCloseUp)
	ON_MESSAGE(CPN_DROPDOWN,     OnDropDown)
END_MESSAGE_MAP()


// CDlgAirsideObjectDisplayProperties message handlers
//IDC_CHK_DP1
//IDC_CHK_DP2
//IDC_CHK_DP3
//IDC_CHK_DP4
//IDC_CHK_DP5
//IDC_CHK_DP6
//
//IDC_CHANGECOLOR_DP1
//IDC_CHANGECOLOR_DP2
//IDC_CHANGECOLOR_DP3
//IDC_CHANGECOLOR_DP4
//IDC_CHANGECOLOR_DP5
//IDC_CHANGECOLOR_DP6

void CDlgAirsideObjectDisplayProperties::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	OnOK();
}

BOOL CDlgAirsideObjectDisplayProperties::OnInitDialog()
{
	CDialog::OnInitDialog();

	ALTOBJECT_TYPE objType = m_p3DObject->GetObject()->GetType();
	ALTObjectDisplayProp* p3DProp = m_p3DObject->GetDisplayProp();
	//try
	//{
	//	p3DProp->ReadData(m_p3DObject->GetObject()->getID());
	//}
	//catch (CADOException &e)
	//{
	//	e.ErrorMessage();
	//}
	ASSERT(p3DProp != NULL);

	//set text to the button
	//name
	m_chkDisplay[0].SetWindowText(DSPPROPNAME[DSPTYPE_NAME]);
	m_nDisplay[0] = p3DProp->m_dpName.bOn?1:0;
	m_cColor[0] = (COLORREF)p3DProp->m_dpName.cColor;
	m_btnColor[0].SetColor(m_cColor[0]);

	//shape	
	m_chkDisplay[1].SetWindowText(DSPPROPNAME[DSPTYPE_SHAPE]);
	m_nDisplay[1] = p3DProp->m_dpShape.bOn?1:0;
	m_cColor[1] = (COLORREF)p3DProp->m_dpShape.cColor;
	m_btnColor[1].SetColor(m_cColor[1]);
	
	
	//in/out constraint
	if (objType == ALT_STAND )
	{
		m_chkDisplay[2].SetWindowText(DSPPROPNAME[DSPTYPE_INCONS]);

		m_nDisplay[2] = ((StandDisplayProp *)p3DProp)->m_dpInConstr.bOn?1:0;
		m_cColor[2] = (COLORREF)((StandDisplayProp *)p3DProp)->m_dpInConstr.cColor;
		m_btnColor[2].SetColor(m_cColor[2]);

		m_chkDisplay[3].SetWindowText(DSPPROPNAME[DSPTYPE_OUTCONS]);

		m_nDisplay[3] = ((StandDisplayProp *)p3DProp)->m_dpOutConstr.bOn?1:0;
		m_cColor[3] = (COLORREF)((StandDisplayProp *)p3DProp)->m_dpOutConstr.cColor;
		m_btnColor[3].SetColor(m_cColor[3]);


		m_chkDisplay[4].ShowWindow(FALSE);
		m_btnColor[4].ShowWindow(FALSE);
		m_chkDisplay[5].ShowWindow(FALSE);
		m_btnColor[5].ShowWindow(FALSE);

	}
	else if( objType == ALT_DEICEBAY)
	{
		m_chkDisplay[2].SetWindowText(DSPPROPNAME[DSPTYPE_INCONS]);

		m_nDisplay[2] = ((DeicePadDisplayProp *)p3DProp)->m_dpInConstr.bOn?1:0;
		m_cColor[2] = (COLORREF)((DeicePadDisplayProp *)p3DProp)->m_dpInConstr.cColor;
		m_btnColor[2].SetColor(m_cColor[2]);


		m_chkDisplay[3].SetWindowText(DSPPROPNAME[DSPTYPE_OUTCONS]);
		m_nDisplay[3] = ((DeicePadDisplayProp *)p3DProp)->m_dpOutConstr.bOn?1:0;
		m_cColor[3] = (COLORREF)((DeicePadDisplayProp *)p3DProp)->m_dpOutConstr.cColor;
		m_btnColor[3].SetColor(m_cColor[3]);


		m_chkDisplay[4].SetWindowText(DSPPROPNAME[DSPTYPE_PAD]);
		m_nDisplay[4] = ((DeicePadDisplayProp *)p3DProp)->m_dpPad.bOn?1:0;
		m_cColor[4] = (COLORREF)((DeicePadDisplayProp *)p3DProp)->m_dpPad.cColor;
		m_btnColor[4].SetColor(m_cColor[4]);

		m_chkDisplay[5].SetWindowText(DSPPROPNAME[DSPTYPE_TRUCK]);
		m_nDisplay[5] = ((DeicePadDisplayProp *)p3DProp)->m_dpTruck.bOn?1:0;
		m_cColor[5] = (COLORREF)((DeicePadDisplayProp *)p3DProp)->m_dpTruck.cColor;
		m_btnColor[5].SetColor(m_cColor[5]);

	}
	else if (objType == ALT_TAXIWAY)
	{
		m_chkDisplay[2].SetWindowText(DSPPROPNAME[DSPTYPE_DIRECTION]);
		
		m_nDisplay[2] = ((TaxiwayDisplayProp *)p3DProp)->m_dpDir.bOn?1:0;
		m_cColor[2] = (COLORREF)((TaxiwayDisplayProp *)p3DProp)->m_dpDir.cColor;
		m_btnColor[2].SetColor(m_cColor[2]);


		m_chkDisplay[3].SetWindowText(DSPPROPNAME[DSPTYPE_CENTERLINE]);
		m_nDisplay[3] = ((TaxiwayDisplayProp *)p3DProp)->m_dpCenterLine.bOn?1:0;
		m_cColor[3] = (COLORREF)((TaxiwayDisplayProp *)p3DProp)->m_dpCenterLine.cColor;
		m_btnColor[3].SetColor(m_cColor[3]);

		m_chkDisplay[4].SetWindowText(DSPPROPNAME[DSPTYPE_MARKING]);
		m_nDisplay[4] = ((TaxiwayDisplayProp *)p3DProp)->m_dpMarking.bOn?1:0;
		m_cColor[4] = (COLORREF)((TaxiwayDisplayProp *)p3DProp)->m_dpMarking.cColor;
		m_btnColor[4].SetColor(m_cColor[4]);

		m_chkDisplay[5].ShowWindow(FALSE);
		m_btnColor[5].ShowWindow(FALSE);
	}
	else if (objType == ALT_RUNWAY)
	{
		m_chkDisplay[1].ShowWindow(FALSE);
		m_btnColor[1].ShowWindow(FALSE);
		m_chkDisplay[2].ShowWindow(FALSE);
		m_btnColor[2].ShowWindow(FALSE);
		m_chkDisplay[3].ShowWindow(FALSE);
		m_btnColor[3].ShowWindow(FALSE);
		m_chkDisplay[4].ShowWindow(FALSE);
		m_btnColor[4].ShowWindow(FALSE);
		m_chkDisplay[5].ShowWindow(FALSE);
		m_btnColor[5].ShowWindow(FALSE);
	}
	else
	{
		m_chkDisplay[2].ShowWindow(FALSE);
		m_btnColor[2].ShowWindow(FALSE);
		m_chkDisplay[3].ShowWindow(FALSE);
		m_btnColor[3].ShowWindow(FALSE);
		m_chkDisplay[4].ShowWindow(FALSE);
		m_btnColor[4].ShowWindow(FALSE);
		m_chkDisplay[5].ShowWindow(FALSE);
		m_btnColor[5].ShowWindow(FALSE);
	}
	
	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgAirsideObjectDisplayProperties::OnOK()
{
	UpdateData(TRUE);

	ALTOBJECT_TYPE objType = m_p3DObject->GetObject()->GetType();
	ALTObjectDisplayProp* p3DProp = m_p3DObject->GetDisplayProp();
	ASSERT(p3DProp != NULL);
	
	//name
	if (m_nDisplay[0] > 0)
		p3DProp->m_dpName.bOn = true;
	else
		p3DProp->m_dpName.bOn = false;
	p3DProp->m_dpName.cColor = ARCColor4(m_cColor[0]);

	//shape
	if (m_nDisplay[1] > 0)
		p3DProp->m_dpShape.bOn = true;
	else
		p3DProp->m_dpShape.bOn  = false;
	p3DProp->m_dpShape.cColor = ARCColor4(m_cColor[1]);

	if (objType == ALT_STAND )
	{
		//in constraint
		if(m_nDisplay[2] >0)
			((StandDisplayProp *)p3DProp)->m_dpInConstr.bOn = true;
		else
			((StandDisplayProp *)p3DProp)->m_dpInConstr.bOn = false;
		((StandDisplayProp *)p3DProp)->m_dpInConstr.cColor = ARCColor4(m_cColor[2]) ;

		//out constraint
		if(m_nDisplay[3] >0)
			((StandDisplayProp *)p3DProp)->m_dpOutConstr.bOn = true;
		else
			((StandDisplayProp *)p3DProp)->m_dpOutConstr.bOn = false;
		((StandDisplayProp *)p3DProp)->m_dpOutConstr.cColor = ARCColor4(m_cColor[3]) ;

	}
	else if( objType == ALT_DEICEBAY)
	{
		//in constraint
		if(m_nDisplay[2] >0)
			((DeicePadDisplayProp *)p3DProp)->m_dpInConstr.bOn = true;
		else
			((DeicePadDisplayProp *)p3DProp)->m_dpInConstr.bOn = false;
		((DeicePadDisplayProp *)p3DProp)->m_dpInConstr.cColor = ARCColor4(m_cColor[2]) ;

		//out constraint
		if(m_nDisplay[3] >0)
			((DeicePadDisplayProp *)p3DProp)->m_dpOutConstr.bOn = true;
		else
			((DeicePadDisplayProp *)p3DProp)->m_dpOutConstr.bOn = false;
		((DeicePadDisplayProp *)p3DProp)->m_dpOutConstr.cColor = ARCColor4(m_cColor[3]) ;
	
		if(m_nDisplay[4] >0)
			((DeicePadDisplayProp *)p3DProp)->m_dpPad.bOn = true;
		else
			((DeicePadDisplayProp *)p3DProp)->m_dpPad.bOn = false;
		((DeicePadDisplayProp *)p3DProp)->m_dpPad.cColor = ARCColor4(m_cColor[4]) ;
	
		if(m_nDisplay[5] >0)
			((DeicePadDisplayProp *)p3DProp)->m_dpTruck.bOn = true;
		else
			((DeicePadDisplayProp *)p3DProp)->m_dpTruck.bOn = false;
		((DeicePadDisplayProp *)p3DProp)->m_dpTruck.cColor = ARCColor4(m_cColor[5]) ;	
	
	}
	else if (objType == ALT_TAXIWAY)
	{
		//direction
		if(m_nDisplay[2] >0)
			((TaxiwayDisplayProp *)p3DProp)->m_dpDir.bOn = true;
		else
			((TaxiwayDisplayProp *)p3DProp)->m_dpDir.bOn = false;
		((TaxiwayDisplayProp *)p3DProp)->m_dpDir.cColor = ARCColor4(m_cColor[2]) ;


		if(m_nDisplay[3] >0)
			((TaxiwayDisplayProp *)p3DProp)->m_dpCenterLine.bOn = true;
		else
			((TaxiwayDisplayProp *)p3DProp)->m_dpCenterLine.bOn = false;
		((TaxiwayDisplayProp *)p3DProp)->m_dpCenterLine.cColor = ARCColor4(m_cColor[3]) ;

		if(m_nDisplay[4] >0)
			((TaxiwayDisplayProp *)p3DProp)->m_dpMarking.bOn = true;
		else
			((TaxiwayDisplayProp *)p3DProp)->m_dpMarking.bOn = false;
		((TaxiwayDisplayProp *)p3DProp)->m_dpMarking.cColor = ARCColor4(m_cColor[4]) ;
	}


	try
	{
		CADODatabase::BeginTransaction() ;
		p3DProp->SaveData(m_p3DObject->GetObject()->getID());
		CADODatabase::CommitTransaction() ;
	}
	catch (CADOException& e)
	{
		e.ErrorMessage();
		CADODatabase::RollBackTransation() ;
	}

	CDialog::OnOK();
}

void CDlgAirsideObjectDisplayProperties::OnCancel()
{
	// TODO: Add your specialized code here and/or call the base class

	CDialog::OnCancel();
}
void CDlgAirsideObjectDisplayProperties::OnChangeColor(UINT nID) 
{

}
void CDlgAirsideObjectDisplayProperties::OnChkDisplay(UINT nID)
{
	//int idx = nID - IDC_CHK_DP1;
	//UpdateData(TRUE);
	//if(m_nDisplay[idx] != 2) 
	//{
	//	m_bApplyDisplay[idx] = TRUE;
	//}
	//else
	//	m_bApplyDisplay[idx] = FALSE;
}
LONG CDlgAirsideObjectDisplayProperties::OnSelEndOK(UINT lParam, LONG wParam)
{
   m_cColor[wParam-IDC_CHANGECOLOR_DP1] = lParam;
   m_bApplyColor[wParam-IDC_CHANGECOLOR_DP1] = TRUE;
    return TRUE;
}

LONG CDlgAirsideObjectDisplayProperties::OnSelEndCancel(UINT /*lParam*/, LONG /*wParam*/)
{
    TRACE0("Selection cancelled\n");
    return TRUE;
}

LONG CDlgAirsideObjectDisplayProperties::OnSelChange(UINT /*lParam*/, LONG /*wParam*/)
{
    TRACE0("Selection changed\n");
    return TRUE;
}

LONG CDlgAirsideObjectDisplayProperties::OnCloseUp(UINT /*lParam*/, LONG /*wParam*/)
{
    TRACE0("Colour picker close up\n");
    return TRUE;
}

LONG CDlgAirsideObjectDisplayProperties::OnDropDown(UINT /*lParam*/, LONG /*wParam*/)
{
    TRACE0("Colour picker drop down\n");
    return TRUE;
}