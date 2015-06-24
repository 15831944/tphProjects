#include "StdAfx.h"
#include "TermPlan.h"
#include ".\dlglayoutobjectdisplayproperties.h"
#include "common\ILayoutObject.h"
#include "Landside/LandsideLayoutObject.h"
//#include "ARCConsole.h"


// CDlgLayoutObjectDisplayProperties dialog

IMPLEMENT_DYNAMIC(CDlgLayoutObjectDisplayProperties, CDialog)
CDlgLayoutObjectDisplayProperties::CDlgLayoutObjectDisplayProperties(LandsideFacilityLayoutObjectList* pLayoutObjectList,CWnd* pParent /*=NULL*/)
: CDialog(CDlgLayoutObjectDisplayProperties::IDD, pParent)
{
	//p3DProp= pDisplayProp;
	m_pLayoutObjectList = pLayoutObjectList;
}

CDlgLayoutObjectDisplayProperties::~CDlgLayoutObjectDisplayProperties()
{
}

void CDlgLayoutObjectDisplayProperties::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Check(pDX, IDC_CHK_DP1, m_nDisplay[0]);
	DDX_Check(pDX, IDC_CHK_DP2, m_nDisplay[1]);
	DDX_Check(pDX, IDC_CHK_DP3, m_nDisplay[2]);
	DDX_Check(pDX, IDC_CHK_DP4, m_nDisplay[3]);
	DDX_Check(pDX, IDC_CHK_DP5, m_nDisplay[4]);
	DDX_Check(pDX, IDC_CHK_DP6, m_nDisplay[5]);

	//DDX_Check(pDX, IDC_CHK_DP1, m_bApplyDisplay[0]);
	//DDX_Check(pDX, IDC_CHK_DP2, m_bApplyDisplay[1]);
	//DDX_Check(pDX, IDC_CHK_DP3, m_bApplyDisplay[2]);
	//DDX_Check(pDX, IDC_CHK_DP4, m_bApplyDisplay[3]);
	//DDX_Check(pDX, IDC_CHK_DP5, m_bApplyDisplay[4]);
	//DDX_Check(pDX, IDC_CHK_DP6, m_bApplyDisplay[5]);

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


BEGIN_MESSAGE_MAP(CDlgLayoutObjectDisplayProperties, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_CHANGECOLOR_DP1, IDC_CHANGECOLOR_DP6, OnChangeColor)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_CHK_DP1, IDC_CHK_DP6, OnChkDisplay)

	ON_MESSAGE(CPN_SELENDOK,     OnSelEndOK)
	ON_MESSAGE(CPN_SELENDCANCEL, OnSelEndCancel)
	ON_MESSAGE(CPN_SELCHANGE,    OnSelChange)
	ON_MESSAGE(CPN_CLOSEUP,         OnCloseUp)
	ON_MESSAGE(CPN_DROPDOWN,     OnDropDown)
END_MESSAGE_MAP()


// CDlgLayoutObjectDisplayProperties message handlers
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

void CDlgLayoutObjectDisplayProperties::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	OnOK();
}

BOOL CDlgLayoutObjectDisplayProperties::OnInitDialog()
{
	CDialog::OnInitDialog();
	
//	ASSERT(p3DProp != NULL);
	ASSERT(m_pLayoutObjectList);
	ASSERT(m_pLayoutObjectList->getCount());

	LandsideFacilityLayoutObject* pFrontObject = m_pLayoutObjectList->getObject(0);
	ASSERT(pFrontObject);

	
	ILayoutObjectDisplay* p3DProp = &pFrontObject->getDisplayProp();

	//set text to the button
	////name
	{
		int nPropType = ILayoutObjectDisplay::_Name;
		m_chkDisplay[0].SetWindowText(p3DProp->GetPropTypeString(nPropType));
		ILayoutObjectDisplay::DspProp& prop = p3DProp->GetProp(nPropType);

		CButton* checkbox = (CButton*)GetDlgItem(IDC_CHK_DP1);
		checkbox->SetCheck(prop.bOn);//default Layout Object name not be showed

		m_nDisplay[0] = prop.bOn?1:0;

		m_cColor[0] = (COLORREF)prop.cColor;
		m_btnColor[0].SetColor(m_cColor[0]);
	}
	//shape	
	{
		int nPropType = ILayoutObjectDisplay::_Shape;
		m_chkDisplay[1].SetWindowText(p3DProp->GetPropTypeString(nPropType));
		ILayoutObjectDisplay::DspProp& prop = p3DProp->GetProp(nPropType);
			m_nDisplay[1] = prop.bOn?1:0;
		m_cColor[1] = (COLORREF)prop.cColor;
		m_btnColor[1].SetColor(m_cColor[1]);
	}

	for(int i=2;i<6;i++)
	{
		m_chkDisplay[i].ShowWindow(FALSE);
		m_btnColor[i].ShowWindow(FALSE);
	}

	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgLayoutObjectDisplayProperties::OnOK()
{
	UpdateData(TRUE);
	
	ASSERT(m_pLayoutObjectList);
	for (int i = 0; i < m_pLayoutObjectList->getCount(); i++)
	{
		LandsideFacilityLayoutObject* pLayoutObject = m_pLayoutObjectList->getObject(i);
		if (pLayoutObject == NULL)
			return;

		ILayoutObjectDisplay* p3DProp = &pLayoutObject->getDisplayProp();
		ASSERT(p3DProp != NULL);

		//name
		{
			int nPropType = ILayoutObjectDisplay::_Name;		
			ILayoutObjectDisplay::DspProp& prop = p3DProp->GetProp(nPropType);
			
			prop.bOn = m_nDisplay[0]>0;
			prop.cColor = ARCColor4(m_cColor[0]);


		}
		//shape
		{

			int nPropType = ILayoutObjectDisplay::_Shape;		
			ILayoutObjectDisplay::DspProp& prop = p3DProp->GetProp(nPropType);

			prop.bOn = m_nDisplay[1]>0;		
			prop.cColor = ARCColor4(m_cColor[1]);
		}
	}
	

	
	
	CDialog::OnOK();
}

void CDlgLayoutObjectDisplayProperties::OnCancel()
{
	// TODO: Add your specialized code here and/or call the base class

	CDialog::OnCancel();
}
void CDlgLayoutObjectDisplayProperties::OnChangeColor(UINT nID) 
{

}
void CDlgLayoutObjectDisplayProperties::OnChkDisplay(UINT nID)
{
	int idx = nID - IDC_CHK_DP1;
	UpdateData(TRUE);
}
LONG CDlgLayoutObjectDisplayProperties::OnSelEndOK(UINT lParam, LONG wParam)
{
	m_cColor[wParam-IDC_CHANGECOLOR_DP1] = lParam;
	m_bApplyColor[wParam-IDC_CHANGECOLOR_DP1] = TRUE;
	return TRUE;
}

LONG CDlgLayoutObjectDisplayProperties::OnSelEndCancel(UINT /*lParam*/, LONG /*wParam*/)
{
	TRACE0("Selection cancelled\n");
	return TRUE;
}

LONG CDlgLayoutObjectDisplayProperties::OnSelChange(UINT /*lParam*/, LONG /*wParam*/)
{
	TRACE0("Selection changed\n");
	return TRUE;
}

LONG CDlgLayoutObjectDisplayProperties::OnCloseUp(UINT /*lParam*/, LONG /*wParam*/)
{
	TRACE0("Colour picker close up\n");
	return TRUE;
}

LONG CDlgLayoutObjectDisplayProperties::OnDropDown(UINT /*lParam*/, LONG /*wParam*/)
{
	TRACE0("Colour picker drop down\n");
	return TRUE;
}