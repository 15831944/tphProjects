// E:\WORK\Codes\branch\src\Main\Onboard\MapOptionsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "../Resource.h"
#include "MapOptionsDlg.h"
#include "../LayerListCtrl.h"
#include "./DlgLayerOptions2.h"
#include ".\mapoptionsdlg.h"
#include <common\FloatUtils.h>
#include <common/UnitsManager.h>


#define SOURCEUNITS_COUNT 9
#define SOURCEUNITS_DEFAULT 4
#define SOURCEUNITS_EPSILON	0.0001

static const CString SOURCEUNITS_NAMES[] = {
	"inches", "feet", "millimeters", "centimeters", "meters", "kilometers", "nautical miles", "stat. miles", "yards"
};

static const double SOURCEUNITS_TOCENTIMETERS[] = {
	2.54, 30.48, 0.10, 1.0, 100.0, 100000.0, 185200.0, 160934.4, 91.44
};

static int GetScaleIdx(double scale) 
{
	for(int i=0; i<SOURCEUNITS_COUNT; i++) {
		if(SOURCEUNITS_EPSILON > fabs(scale - SOURCEUNITS_TOCENTIMETERS[i]))
			return i;
	}
	return SOURCEUNITS_DEFAULT;
}




// CMapOptionsDlg dialog

IMPLEMENT_DYNAMIC(CMapOptionsDlg, CDialog)
CMapOptionsDlg::CMapOptionsDlg(const CCADFileDisplayInfo& displayInfo,CWnd* pParent /*=NULL*/)
	: CDialog(CMapOptionsDlg::IDD, pParent)
{
	m_cadInfos = displayInfo;
	
}

CMapOptionsDlg::~CMapOptionsDlg()
{
}

void CMapOptionsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_SOURCEUNITS, m_cmbSourceUnits);
	DDX_Control(pDX, IDC_MAPFILENAME, m_cBtnMapFileName);	
	DDX_Check(pDX, IDC_CHK_MAPVISIBLE, m_cadInfos.bVisible);
	DDX_Text(pDX,IDC_DUNITSMX,m_sUnits);
	DDX_Text(pDX,IDC_DUNITSMY,m_sUnits);
	DDX_Text(pDX, IDC_MAPROT, m_dRotation);
	DDX_Text(pDX, IDC_MAPXOFFSET, m_dXOffset);
	DDX_Text(pDX, IDC_MAPYOFFSET, m_dYOffset);

}


BEGIN_MESSAGE_MAP(CMapOptionsDlg, CDialog)
	ON_BN_CLICKED(IDC_LAYEROPTIONS, OnBnClickedLayeroptions)
	ON_BN_CLICKED(IDC_MAPFILENAME, OnBnClickedMapfilename)
	ON_BN_CLICKED(IDC_BUTTON_REMOVEMAP, OnBnClickedButtonRemovemap)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()

void CMapOptionsDlg::OnBnClickedLayeroptions()
{
	// TODO: Add your control notification handler code here
	CDlgLayerOptions2 dlg(m_cadInfos.mLayersInfo);
	if(dlg.DoModal() == IDOK)
	{
		if(dlg.IsLayerDirty()) //if at least one layer option was changed
		{//need to notify update cad map
			m_cadInfos.mLayersInfo = dlg.m_layerInfos;
			m_bMapChanged = TRUE;
		}		
	}
}


void CMapOptionsDlg::OnBnClickedMapfilename()
{
	// TODO: Add your control notification handler code here
	TCHAR* p = NULL;
	CString sMapFileName = m_cadInfos.mCADInfo.sFileName;
	CString sMapPathName = m_cadInfos.mCADInfo.sPathName;


	if(sMapFileName.CompareNoCase(NO_MAP_STR) != 0)
	{
		p = (TCHAR*) sMapPathName.GetBuffer(255);
	}

	//CFileDialog dlg(TRUE, "dxf", p, 0, "AutoCad DXF (*.dxf)|*.dxf|AutoCad DWG files (*.dwg)|*.dwg|All Files (*.*)|*.*||");
	CFileDialog dlg(TRUE, "dxf", p, 0, "AutoCad DXF/DWG files (*.dxf, *.dwg)|*.dxf;*.dwg|ARCTERMv1 CAD files (*.cad)|*.cad|All Files (*.*)|*.*||");
	if(dlg.DoModal() == IDOK)
	{
		m_cBtnMapFileName.SetWindowText(dlg.GetFileName());
		sMapFileName = dlg.GetFileName();
		sMapPathName = dlg.GetPathName();	

		CWnd* pLO = GetDlgItem(IDC_LAYEROPTIONS);
		pLO->EnableWindow(TRUE);

		CCADInfo& cadInfo = m_cadInfos.mCADInfo;
		//load cad file and get layers... do not process cad yet
		BOOL bMapChanged = (sMapPathName.Compare( cadInfo.sPathName ) != 0);
		if(bMapChanged) 
		{		
			m_cadInfos.SetMap(sMapFileName,sMapPathName);		
		}
	}
}


void CMapOptionsDlg::OnBnClickedButtonRemovemap()
{
	// TODO: Add your control notification handler code here
	//
	CString sMapFileName = NO_MAP_STR;
	CString sMapPathName = _T("");
	//
	m_cBtnMapFileName.SetWindowText(NO_MAP_STR);
	m_cadInfos = CCADFileDisplayInfo();
	m_bMapChanged = TRUE;

	CWnd* pLO = GetDlgItem(IDC_LAYEROPTIONS);
	pLO->EnableWindow(FALSE);
}

BOOL CMapOptionsDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	//add units
	for(int i=0; i<SOURCEUNITS_COUNT; i++) {
		m_cmbSourceUnits.AddString(SOURCEUNITS_NAMES[i]);
	}

	m_sUnits = UNITSMANAGER->GetLengthUnitString(UNITSMANAGER->GetLengthUnits());	
	m_dXOffset = RoundDouble(UNITSMANAGER->ConvertLength(m_cadInfos.mCADInfo.vOffset[0]),4);
	m_dYOffset = RoundDouble(UNITSMANAGER->ConvertLength(m_cadInfos.mCADInfo.vOffset[1]),4);
	m_dRotation = RoundDouble(m_cadInfos.mCADInfo.dRotation,4);

	UpdateData(FALSE);

	//INIT DIALOG
	double dScale = m_cadInfos.mCADInfo.dScale;
	m_cmbSourceUnits.SetCurSel( GetScaleIdx(dScale));
	m_cBtnMapFileName.SetWindowText(m_cadInfos.mCADInfo.sFileName);
	if(m_cadInfos.mCADInfo.sFileName.CompareNoCase(NO_MAP_STR) == 0) {
		CWnd* pLO = GetDlgItem(IDC_LAYEROPTIONS);
		pLO->EnableWindow(FALSE);
	}

	return TRUE;
}


// CMapOptionsDlg message handlers

void CMapOptionsDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	//get data form dialog
	UpdateData(TRUE);

	m_cadInfos.mCADInfo.dRotation = m_dRotation;
	m_cadInfos.mCADInfo.vOffset[VX] = UNITSMANAGER->UnConvertLength(m_dXOffset);
	m_cadInfos.mCADInfo.vOffset[VY] = UNITSMANAGER->UnConvertLength(m_dYOffset);

	int nSel = -1;
	if(-1 != (nSel = m_cmbSourceUnits.GetCurSel()))
		m_cadInfos.mCADInfo.dScale = (SOURCEUNITS_TOCENTIMETERS[nSel]);
	else
		m_cadInfos.mCADInfo.dScale = (1.0);

	OnOK();
}
