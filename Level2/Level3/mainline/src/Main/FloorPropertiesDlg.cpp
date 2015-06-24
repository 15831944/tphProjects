// FloorPropertiesDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
//#include "TermPlanDoc.h"

#include "Floor2.h"
#include "3DGridOptionsDlg.h"
#include "DlgLayerOptions.h"

#include  <math.h>
#include "../common/UnitsManager.h"
#include "common\FloatUtils.h"
//#include <inputairside/ARCUnitManager.h>

#include "FloorPropertiesDlg.h"
#include "TermPlanDoc.h"
#include "ViewMsg.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CProperiesDlg::CProperiesDlg(CBaseFloor* pFloor,CTermPlanDoc *pDoc, CWnd* pParent )
:m_pFloor(pFloor),CDialog(CProperiesDlg::IDD, pParent)
{
	m_sFloorName = _T("");
	m_dRotation = 0.0;
	m_dXOffset = 0.0;
	m_dYOffset = 0.0;
	m_dAlt = 0.0;
	m_nAltRdo = -1;
	m_bFloorVisible = FALSE;
	m_sUnits4 = _T("");
	m_dRealAlt = 0.0;
	m_bIsCADLoaded = FALSE ;
	//}}AFX_DATA_INI

	m_pDoc = pDoc;
}
void CProperiesDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFloorPropertiesDlg)
	DDX_Control(pDX, IDC_COMBO_SOURCEUNITS, m_cmbSourceUnits);
	DDX_Control(pDX, IDC_MAPFILENAME, m_cMapFileName);
	DDX_Text(pDX, IDC_FLOORNAME, m_sFloorName);
	DDX_Text(pDX, IDC_MAPROT, m_dRotation);
	DDX_Text(pDX, IDC_MAPXOFFSET, m_dXOffset);
	DDX_Text(pDX, IDC_MAPYOFFSET, m_dYOffset);
	DDX_Text(pDX, IDC_ALT, m_dAlt);
	DDX_Radio(pDX, IDC_ALTRDO, m_nAltRdo);
	DDX_Text(pDX, IDC_DUNITS, m_sUnits1);
	DDX_Text(pDX, IDC_DUNITS2, m_sUnits2);
	DDX_Text(pDX, IDC_DUNITS3, m_sUnits3);
	DDX_Check(pDX, IDC_CHK_FLOORVISIBLE, m_bFloorVisible);
	DDX_Text(pDX, IDC_DUNITS4, m_sUnits4);
	DDX_Text(pDX, IDC_REALALT, m_dRealAlt);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CProperiesDlg, CDialog)
	//{{AFX_MSG_MAP(CFloorPropertiesDlg)
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_MAPFILENAME, OnMapFileName)
	ON_BN_CLICKED(IDC_GRIDOPTIONS, OnGridOptions)
	ON_BN_CLICKED(IDC_LAYEROPTIONS, OnLayerOptions)
	ON_BN_CLICKED(IDC_BUTTON_REMOVEMAP, OnButtonRemovemap)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFloorPropertiesDlg message handlers

int CProperiesDlg::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	/*
	m_cLevelSpin.SetRange(-10, 20);
	m_cLevelSpin.SetPos(1);
	m_cLevelSpin.SetBuddy(&m_cLevelEdit);
	*/

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// CFloorPropertiesDlg dialog

int CProperiesDlg::GetScaleIdx(double scale) 
{
	for(int i=0; i<SOURCEUNITS_COUNT; i++) {
		if(SOURCEUNITS_EPSILON > fabs(scale - SOURCEUNITS_TOCENTIMETERS[i]))
			return i;
	}
	return SOURCEUNITS_DEFAULT;
}

BOOL CProperiesDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	//add units
	for(int i=0; i<SOURCEUNITS_COUNT; i++) {
		m_cmbSourceUnits.AddString(SOURCEUNITS_NAMES[i]);
	}

	m_bFloorVisible = m_pFloor->IsVisible();
	m_nAltRdo = 0;
	ARCVector2 vOffset(m_pFloor->GetFloorOffset());
	m_dRotation = m_pFloor->MapRotation();

	/*if(m_pDoc && m_pDoc->m_systemMode == EnvMode_AirSide){
		m_sUnits1 = m_sUnits2 = m_sUnits3 = m_sUnits4 = CARCLengthUnit::GetLengthUnitString(m_pDoc->m_umGlobalUnitManager.GetCurSelLengthUnit());
		m_dAlt = RoundDouble(CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,m_pDoc->m_umGlobalUnitManager.GetCurSelLengthUnit(),m_pFloor->Altitude()),2);
		m_dRealAlt = RoundDouble(CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,m_pDoc->m_umGlobalUnitManager.GetCurSelLengthUnit(),m_pFloor->RealAltitude()),2);
		m_dXOffset = RoundDouble(CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,m_pDoc->m_umGlobalUnitManager.GetCurSelLengthUnit(),vOffset[0]),4);
		m_dYOffset = RoundDouble(CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,m_pDoc->m_umGlobalUnitManager.GetCurSelLengthUnit(),vOffset[1]),4);
	}else*/
	{
		m_sUnits1 = m_sUnits2 = m_sUnits3 = m_sUnits4 = UNITSMANAGER->GetLengthUnitString(UNITSMANAGER->GetLengthUnits());
		m_dAlt = RoundDouble(CUnitsManager::GetInstance()->ConvertLength(m_pFloor->Altitude()),2);
		m_dRealAlt = RoundDouble(CUnitsManager::GetInstance()->ConvertLength(m_pFloor->RealAltitude()),2);
		m_dXOffset = RoundDouble(CUnitsManager::GetInstance()->ConvertLength(vOffset[0]),4);
		m_dYOffset = RoundDouble(CUnitsManager::GetInstance()->ConvertLength(vOffset[1]),4);
	}
	
	//////m_dScale = m_pFloor->MapScale();
	m_cmbSourceUnits.SetCurSel(GetScaleIdx(m_pFloor->MapScale()));
	m_sMapFileName = m_pFloor->MapFileName();
	m_sMapPathName = m_pFloor->MapPathName();
	m_cMapFileName.SetWindowText(m_sMapFileName);

	UpdateData(FALSE);

	if(m_sMapFileName.CompareNoCase(_T("No map selected")) == 0) {
		CWnd* pLO = GetDlgItem(IDC_LAYEROPTIONS);
		pLO->EnableWindow(FALSE);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CProperiesDlg::OnMapFileName() 
{
	CFileDialog dlg(TRUE, "dxf", m_sMapFileName.CompareNoCase(_T("No map selected")) ? (LPCTSTR)m_sMapFileName : NULL, 0,
		"AutoCad DXF/DWG files (*.dxf, *.dwg)|*.dxf;*.dwg|ARCTERMv1 CAD files (*.cad)|*.cad|All Files (*.*)|*.*||",NULL, 0, FALSE);
	if(dlg.DoModal() == IDOK)
	{
		m_cMapFileName.SetWindowText(dlg.GetFileName());
		m_sMapFileName = dlg.GetFileName();
		m_sMapPathName = dlg.GetPathName();	

		CWnd* pLO = GetDlgItem(IDC_LAYEROPTIONS);
		pLO->EnableWindow(TRUE);

		//load cad file and get layers... do not process cad yet
		BOOL bMapChanged = (m_sMapPathName.Compare(m_pFloor->MapPathName()) != 0);
		if(bMapChanged) {
			m_pFloor->MapFileName(m_sMapFileName);
			m_pFloor->MapPathName(m_sMapPathName);
			m_pFloor->InvalidateTexture();
			ASSERT(m_pFloor != NULL);
			if( FALSE == m_pFloor->LoadCAD())
			{
				m_bIsCADLoaded = FALSE; 
				CString strMessage;
				strMessage.Format(_T("invalid map file: %s"), m_sMapFileName);
				MessageBox(LPCTSTR(strMessage), "map file error", MB_ICONERROR);
				OnButtonRemovemap();
				return;
			}
			VERIFY(m_pFloor->LoadCADLayers());
			m_bIsCADLoaded = TRUE;
		}
	}
}

void CProperiesDlg::UpdateFloorData()
{
	m_pFloor->IsVisible(m_bFloorVisible);

	double xOffset = -1.0,yOffset = -1.0;
	/*if(m_pDoc && m_pDoc->m_systemMode == EnvMode_AirSide){		
		m_pFloor->Altitude(CARCLengthUnit::ConvertLength(m_pDoc->m_umGlobalUnitManager.GetCurSelLengthUnit(),DEFAULT_DATABASE_LENGTH_UNIT,m_dAlt));
		m_pFloor->RealAltitude(CARCLengthUnit::ConvertLength(m_pDoc->m_umGlobalUnitManager.GetCurSelLengthUnit(),DEFAULT_DATABASE_LENGTH_UNIT,m_dRealAlt));
		xOffset = CARCLengthUnit::ConvertLength(m_pDoc->m_umGlobalUnitManager.GetCurSelLengthUnit(),DEFAULT_DATABASE_LENGTH_UNIT,m_dXOffset);
		yOffset = CARCLengthUnit::ConvertLength(m_pDoc->m_umGlobalUnitManager.GetCurSelLengthUnit(),DEFAULT_DATABASE_LENGTH_UNIT,m_dYOffset);
	}else*/{
		m_pFloor->Altitude(CUnitsManager::GetInstance()->UnConvertLength(m_dAlt));
		m_pFloor->RealAltitude(CUnitsManager::GetInstance()->UnConvertLength(m_dRealAlt));
		xOffset = CUnitsManager::GetInstance()->UnConvertLength(m_dXOffset);
		yOffset = CUnitsManager::GetInstance()->UnConvertLength(m_dYOffset);
	}

	ARCVector2 vOffset(xOffset,yOffset);

	/*
	BOOL bMapChanged = (m_sMapPathName.Compare(m_pFloor->MapPathName()) != 0) ||
		(m_dScale != m_pFloor->MapScale()) ||
		(vOffset != m_pFloor->MapOffset()) ||
		(m_dRotation != m_pFloor->MapRotation());*/

	if(m_sMapFileName.CompareNoCase(_T("No map selected")) != 0) {
		CString sOldMapFile = m_pFloor->MapFileName();
		if(sOldMapFile.CompareNoCase(m_sMapFileName) != 0) {
			m_pFloor->MapFileName(m_sMapFileName);
			m_pFloor->InvalidateTexture();
		}
		CString sOldMapPath = m_pFloor->MapPathName();
		if(sOldMapPath.CompareNoCase(m_sMapPathName) != 0) {
			m_pFloor->MapPathName(m_sMapPathName);
			m_pFloor->InvalidateTexture();
		}
			
	}
	int nSel = -1;
	if(-1 != (nSel = m_cmbSourceUnits.GetCurSel()))
		m_pFloor->MapScale(SOURCEUNITS_TOCENTIMETERS[nSel]);
	else
		m_pFloor->MapScale(1.0);
	m_pFloor->SetFloorOffset(vOffset);
	m_pFloor->MapRotation(m_dRotation);
}

void CProperiesDlg::OnGridOptions()
{
	C3DGridOptionsDlg dlg(*(m_pFloor->GetGrid()),m_pDoc);
	if(dlg.DoModal() == IDOK) {
		dlg.UpdateGridData(m_pFloor->GetGrid());
	}
}

void CProperiesDlg::OnLayerOptions()
{

	CDlgLayerOptions dlg(m_pFloor->GetFloorLayers());
	if(dlg.DoModal() == IDOK) {
		if(dlg.IsLayerDirty()) //if at least one layer option was changed
			m_pFloor->InvalidateTexture();
	}
}

void CProperiesDlg::OnOK() 
{

	
	//UpdateData(TRUE);
	//UpdateFloorData();
	//if( m_bIsCADLoaded || (m_pFloor->IsCADLoaded() && m_pFloor->IsCADLayersLoaded() && !m_pFloor->IsCADProcessed() /*if no new map, but still need to process cad. */) )
	//{
	//	VERIFY(m_pFloor->ProcessCAD());
	//	m_bIsCADLoaded = FALSE;
	//}
	//if(m_pFloor != NULL && m_pFloor->IsMapValid()) {
	//	//test extents.. are they reasonable?
	//	if(!m_pFloor->IsExtentsSane()) {
	//		CUnitsManager* pUM = CUnitsManager::GetInstance();
	//		ARCVector2 vMin, vMax;
	//		m_pFloor->GetExtents(&vMin, &vMax);
	//		ARCVector2 vExt(vMax-vMin);
	//		vExt[VX] = pUM->ConvertLength(vExt[VX]);
	//		vExt[VY] = pUM->ConvertLength(vExt[VY]);
	//		CString sUnits = pUM->GetLengthUnitLongString(pUM->GetLengthUnits());
	//		CString sMsg;
	//		sMsg.Format("The selected map spans %.2f %s in the X direction and %.2f %s in the Y direction.\nAre you sure you want to continue with these settings?",
	//			vExt[VX], sUnits, vExt[VY], sUnits);
	//		if(AfxMessageBox(sMsg, MB_YESNO) == IDNO)
	//			return;
	//	}
	//}

	if (SaveData())
	{
		CDialog::OnOK();
	}
}
BOOL CProperiesDlg::SaveData()
{
	UpdateData(TRUE);
	UpdateFloorData();
	if (m_pDoc->m_systemMode == EnvMode_LandSide)
	{
		CRenderFloor* pfloor = (CRenderFloor*)m_pFloor;
		int nTermanalFloor = pfloor->linkToTerminalFloor();
		if (nTermanalFloor >= 0)
		{
			const CFloors& pTerminalFloors = m_pDoc->GetFloorByMode(EnvMode_Terminal);
			CRenderFloor* terminalfloor = (CRenderFloor*)pTerminalFloors.GetFloor2(nTermanalFloor);
			terminalfloor->Altitude(pfloor->Altitude());
			terminalfloor->RealAltitude(pfloor->RealAltitude());
			m_pDoc->GetFloorByMode(EnvMode_Terminal).SaveData( m_pDoc->m_ProjInfo.path, true );
			if(!m_pDoc->IsKindOf(RUNTIME_CLASS(CTermPlanDoc)))
				return false;
			m_pDoc->UpdateAllViews(NULL,VM_UPDATE_ONE_FLOOR,(CObject*)terminalfloor);
		}
	}
	if (m_pDoc->m_systemMode == EnvMode_Terminal)
	{
		const CFloors& pLandsideFloors = m_pDoc->GetFloorByMode(EnvMode_LandSide);
		int nLandsidefloorCount = pLandsideFloors.GetCount();
		for (int i = 0; i < nLandsidefloorCount; i++)
		{
			CRenderFloor* Landsidefloor = (CRenderFloor*)pLandsideFloors.GetFloor2(i);
			int nTermanalFloor = Landsidefloor->linkToTerminalFloor();
			CRenderFloor* pfloor = (CRenderFloor*)m_pFloor;
			if (nTermanalFloor == pfloor->Level())
			{
				Landsidefloor->Altitude(pfloor->Altitude());
				Landsidefloor->RealAltitude(pfloor->RealAltitude());
				m_pDoc->GetFloorByMode(EnvMode_LandSide).SaveData( m_pDoc->m_ProjInfo.path, true );
// 				if(!m_pDoc->IsKindOf(RUNTIME_CLASS(CTermPlanDoc)))
// 					return ;
// 				m_pDoc->UpdateAllViews(NULL,VM_UPDATE_ONE_FLOOR,(CObject*)Landsidefloor);
			}
		}

	}
	if( m_bIsCADLoaded || (m_pFloor->IsCADLoaded() && m_pFloor->IsCADLayersLoaded() && !m_pFloor->IsCADProcessed() /*if no new map, but still need to process cad. */) )
	{
		VERIFY(m_pFloor->ProcessCAD());
		m_bIsCADLoaded = FALSE;
	}
	if(m_pFloor != NULL && m_pFloor->IsMapValid()) {
		//test extents.. are they reasonable?
		if(!m_pFloor->IsExtentsSane()) {
			CUnitsManager* pUM = CUnitsManager::GetInstance();
			ARCVector2 vMin, vMax;
			m_pFloor->GetExtents(&vMin, &vMax);
			ARCVector2 vExt(vMax-vMin);
			vExt = vExt*m_pFloor->MapScale();
			CString sUnits =_T("");
			/*if(m_pDoc && m_pDoc->m_systemMode == EnvMode_AirSide){		
				vExt[VX] = CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,m_pDoc->m_umGlobalUnitManager.GetCurSelLengthUnit(),vExt[VX]);
				vExt[VY] = CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,m_pDoc->m_umGlobalUnitManager.GetCurSelLengthUnit(),vExt[VY]);
				sUnits = CARCLengthUnit::GetLengthUnitString(m_pDoc->m_umGlobalUnitManager.GetCurSelLengthUnit());
			}else */
			{
				vExt[VX] = pUM->ConvertLength(vExt[VX]);
				vExt[VY] = pUM->ConvertLength(vExt[VY]);
				//int nSel = m_cmbSourceUnits.GetCurSel();
				sUnits = pUM->GetLengthUnitLongString(pUM->GetLengthUnits());
				//sUnits=SOURCEUNITS_NAMES[nSel];
			}
			
			CString sMsg;
			sMsg.Format("The selected map spans %.2f %s in the X direction and %.2f %s in the Y direction.\nAre you sure you want to continue with these settings?",
				vExt[VX], sUnits, vExt[VY], sUnits);
			if(AfxMessageBox(sMsg, MB_YESNO) == IDNO)
				return FALSE;
		}
	}

	return TRUE;

}
void CProperiesDlg::OnButtonRemovemap() 
{
	m_sMapFileName = _T("No map selected");
	m_sMapPathName = _T("");
	m_pFloor->MapFileName(m_sMapFileName);
	m_pFloor->MapPathName(m_sMapPathName);
	m_pFloor->MapValid(FALSE);
	m_cMapFileName.SetWindowText(m_sMapFileName);
	m_pFloor->DeleteAllLayers();
   // m_pFloor->GetFloorLayers()->clear() ;
	CWnd* pLO = GetDlgItem(IDC_LAYEROPTIONS);
	pLO->EnableWindow(FALSE);
}
CFloorPropertiesDlg::CFloorPropertiesDlg(CFloor2* pFloor,CTermPlanDoc *pDoc, CWnd* pParent /*=NULL*/)
: CProperiesDlg(pFloor,pDoc, pParent)
{
	//{{AFX_DATA_INIT(CFloorPropertiesDlg)
	m_sFloorName = pFloor->FloorName();
	m_bIsCADLoaded = FALSE;
}