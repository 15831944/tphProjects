// DlgSectionProp.cpp : implementation file
//

#include "stdafx.h"
#include "../Resource.h"
#include "DlgSectionProp.h"
#include ".\dlgsectionprop.h"
#include "InputOnboard\AircraftComponentModelDatabase.h"
#include <inputonboard\ComponentModel.h>
#include <common/UnitsManager.h>
#include <OpenDWGToolKit/CADFileLayer.h>
//#include "../DlgLayerOptions.h"
#include <common\FloatUtils.h>
#include <inputonboard\ComponentSection.h>
#include "MapOptionsDlg.h"


// CDlgSectionProp dialog

IMPLEMENT_DYNAMIC(CDlgSectionProp, CDialog)
CDlgSectionProp::CDlgSectionProp(CComponentMeshSection* pSection, const CString& strName, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSectionProp::IDD, pParent)
	,m_sName(strName)
{
	mpSection = pSection;
	mbSectionChange =false;
	mbMapChange = false;

	m_sName.MakeUpper();
	m_dPos = 0;
	m_dDefaultThick = 0;
	mCenter = ARCVector2(0,0);
	m_dRad = UNITSMANAGER->ConvertLength(100);

	if(mpSection)
	{
		m_sName = mpSection->getName();		
		m_dPos = RoundDouble(UNITSMANAGER->ConvertLength(mpSection->getPos()),4);
		m_dDefaultThick = RoundDouble(UNITSMANAGER->ConvertLength(mpSection->getDefaultThick()),4);
		mCenter[VX] = RoundDouble(UNITSMANAGER->ConvertLength(mpSection->mCenter[VX]),4);
		mCenter[VY] = RoundDouble(UNITSMANAGER->ConvertLength(mpSection->mCenter[VY]),4);	
		m_cadFileInfo = mpSection->m_cadFileInfo;
	}	
	
}

CDlgSectionProp::~CDlgSectionProp()
{
}

//DDX_Control(pDX,IDC_EDIT_POSITION,mEditCtrlPos);
//DDX_Control(pDX,IDC_EDIT_RAD,mEditCtrlRad);
//DDX_Control(pDX,IDC_XOFFSET,mEditOffsetX);
//DDX_Control(pDX,IDC_YOFFSET,mEditOffsetY);
//DDX_Control(pDX,IDC_EDIT_THICK,mEditCtrlThick);
void CDlgSectionProp::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgSectionProp)

	DDX_Control(pDX,IDC_COMBO_PTCOUNT,m_cmbPointCount);
	DDX_Text(pDX, IDC_EDIT_NAME, m_sName);
	DDX_Text(pDX,IDC_DUNITSP,m_sUnits);
	DDX_Text(pDX,IDC_DUNITST,m_sUnits);
	DDX_Text(pDX,IDC_DUNITSR,m_sUnits);
	DDX_Text(pDX,IDC_DUNITSS1,m_sUnits);
	DDX_Text(pDX,IDC_DUNITSS2,m_sUnits);

	DDX_Text(pDX,IDC_EDIT_POSITION,m_dPos);
	DDX_Text(pDX,IDC_EDIT_RAD,m_dRad);
	DDX_Text(pDX,IDC_XOFFSET,mCenter[VX]);
	DDX_Text(pDX,IDC_YOFFSET,mCenter[VY]);
	DDX_Text(pDX,IDC_EDIT_THICK,m_dDefaultThick);
	
	DDX_Control(pDX, IDC_COMBO_SECTIONTYPE, m_combsectionType);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgSectionProp, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_CBN_SELCHANGE(IDC_COMBO_SECTIONTYPE, OnCbnSelchangeComboSectiontype)
	ON_BN_CLICKED(IDC_BUTTON_MAPOPTIONS, OnBnClickedButtonMapoptions)
END_MESSAGE_MAP()


// CDlgSectionProp message handlers
void CDlgSectionProp::OnBnClickedOk()
{
	UpdateData(TRUE);
	int nCurSel  = m_combsectionType.GetCurSel();
	m_iSectType = (int)m_combsectionType.GetItemData(nCurSel);
	nCurSel = m_cmbPointCount.GetCurSel();
	m_iPtCount = (int)m_cmbPointCount.GetItemData(nCurSel);	


	if(mpSection)
	{
		double dDefaultThick = UNITSMANAGER->UnConvertLength(m_dDefaultThick);
		double dPos = UNITSMANAGER->UnConvertLength(m_dPos);
		double dRad = UNITSMANAGER->UnConvertLength(m_dRad);
		ARCVector2 vCenter;
		vCenter[VX] = UNITSMANAGER->UnConvertLength(mCenter[VX]);
		vCenter[VY] = UNITSMANAGER->UnConvertLength(mCenter[VY]);
		double dThick = UNITSMANAGER->UnConvertLength(m_dDefaultThick);	
		mpSection->setPos(dPos);
		mpSection->setOffset(vCenter);
		mpSection->setName(m_sName);
		mpSection->setDefaultThick(dThick);

		if(mpSection->ToCircleSection())
		{
			mpSection->ToCircleSection()->setRadius(dRad);
		}

		mpSection->m_cadFileInfo = m_cadFileInfo;
		
	}
	OnOK();
}


#include <common/ARCStringConvert.h>

BOOL CDlgSectionProp::OnInitDialog()
{
	CDialog::OnInitDialog();

	//add point count
	//if(!mpSection) //
	{
		for(int i=1;i<=20;i++)
		{
			int idx = m_cmbPointCount.AddString(ARCStringConvert::toString(i));
			m_cmbPointCount.SetItemData(idx,i);
		}
		m_cmbPointCount.SetCurSel(3);
	}

	//add section type	
	{
		int itemIdx =  m_combsectionType.AddString("SplineLine");
		m_combsectionType.SetItemData(itemIdx,ST_CSpline);
		itemIdx = m_combsectionType.AddString("Polygon");
		m_combsectionType.SetItemData(itemIdx,ST_Polygon);
		itemIdx = m_combsectionType.AddString("Circle");
		m_combsectionType.SetItemData(itemIdx,ST_Circle);
		m_combsectionType.SetCurSel(0);

		
	}

	if(mpSection)
	{
		m_combsectionType.SetCurSel(mpSection->getType());
		m_combsectionType.EnableWindow(FALSE);
		m_cmbPointCount.SetCurSel(mpSection->GetCtrlPtCount()-1);
		m_cmbPointCount.EnableWindow(FALSE);
	}
	//set map button
	m_sUnits  = UNITSMANAGER->GetLengthUnitString(UNITSMANAGER->GetLengthUnits());	

	UpdateData(FALSE);

	return TRUE;  // return T
}
void CDlgSectionProp::OnCbnSelchangeComboSectiontype()
{
	// TODO: Add your control notification handler code here
	int nCurSel  = m_combsectionType.GetCurSel();
	ComponetSectionType ItemData = (ComponetSectionType)m_combsectionType.GetItemData(nCurSel);
	if(ItemData == ST_Circle)
	{
		m_cmbPointCount.EnableWindow(FALSE);		
	}
	else
	{
		m_cmbPointCount.EnableWindow(TRUE);		
	}
}


void CDlgSectionProp::OnBnClickedButtonMapoptions()
{
	// TODO: Add your control notification handler code here
	CMapOptionsDlg dlg(m_cadFileInfo,this);
	if( dlg.DoModal() == IDOK)
	{
		//if(dlg.m_bMapChanged)
		{
			m_cadFileInfo = dlg.m_cadInfos;
			mbMapChange = true;
		}
	}
}

CComponentMeshSection* CDlgSectionProp::createSection( CComponentMeshModel* pModel ) const
{
	double dDefaultThick = UNITSMANAGER->UnConvertLength(m_dDefaultThick);
	double dPos = UNITSMANAGER->UnConvertLength(m_dPos);
	double dRad = UNITSMANAGER->UnConvertLength(m_dRad);
	ARCVector2 vCenter;
	vCenter[VX] = UNITSMANAGER->UnConvertLength(mCenter[VX]);
	vCenter[VY] = UNITSMANAGER->UnConvertLength(mCenter[VY]);



	CComponentMeshSection* pSection = pModel->createSection((ComponetSectionType)m_iSectType,m_iPtCount,dRad,dDefaultThick);
	pSection->setPos(dPos);
	pSection->setOffset(vCenter);
	pSection->setName(m_sName);
	pSection->m_cadFileInfo = m_cadFileInfo;
	return pSection;
}

void CDlgSectionProp::SetPos( double d )
{
	m_dPos = RoundDouble(UNITSMANAGER->ConvertLength(d),4);
}
