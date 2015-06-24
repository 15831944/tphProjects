// LandsideProcDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "TermPlanDoc.h"
#include "LandsideProcDlg.h"
#include ".\landsideprocdlg.h"
//#include "./landsideStretchProc.h"
#include "../Engine/StretchProc.h"
#include "../Engine/IntersectionProc.h"
#include "../common/UnitsManager.h"
#include "./LandSideDocument.h"
#include "./MainFrm.h"
#include "./3dView.h"
#include <CommonData/Fallback.h>
#include "../common\WinMsg.h"
#include "./ChangeVerticalProfileDlg.h"
#include "./Floors.h"
#include "./Floor2.h"
#include "./LandSideDocument.h"
#include "Landside/LandsideLayoutObject.h"
#include "landside/LandsideStretch.h"
// CLandsideProcDlg dialog

IMPLEMENT_DYNAMIC(CLandsideProcDlg, CARCLayoutObjectDlg)
CLandsideProcDlg::CLandsideProcDlg(LandsideLayoutObject* pProc, CWnd* pParent /*=NULL*/)
	: CARCLayoutObjectDlg(pParent)
{
	m_pProc  = pProc; 	

}
//set values
CLandsideProcDlg::~CLandsideProcDlg()
{
}

void CLandsideProcDlg::DoDataExchange(CDataExchange* pDX)
{
	CARCLayoutObjectDlg::DoDataExchange(pDX);		
}


BEGIN_MESSAGE_MAP(CLandsideProcDlg, CARCLayoutObjectDlg)
	//ON_NOTIFY(NM_RCLICK, IDC_TREE_PROPERTIES, OnNMRclickTreeProperties)
//	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	//ON_WM_CONTEXTMENU()
	//ON_WM_SIZE()
	//ON_MESSAGE(TP_DATA_BACK, OnTempFallbackFinished)
	//ON_COMMAND(ID_LANDSIDE_CTRLPOINTS, OnProcpropPickfrommap)
	
//	ON_CBN_DROPDOWN(IDC_COMBO_LEVEL2, OnCbnDropdownComboLevel2)
//	ON_CBN_DROPDOWN(IDC_COMBO_LEVEL3, OnCbnDropdownComboLevel3)
//	ON_CBN_DROPDOWN(IDC_COMBO_LEVEL4, OnCbnDropdownComboLevel4)
//	ON_NOTIFY(NM_DBLCLK, IDC_TREE_PROPERTY, OnNMDblclkTreeProperty)
//	ON_COMMAND(ID_STRETCHWIDTH_DEFINEWIDTH, OnStretchwidthDefinewidth)
//	ON_COMMAND(ID_PROCESSORLANNUM_DEFINELANENUMBER, OnProcessorlannumDefinelanenumber)
//	ON_COMMAND(ID_PROCESSORVERTICAL_EDITVERTICALPROFILE, OnProcessorverticalEditverticalprofile)
END_MESSAGE_MAP()

void CLandsideProcDlg::SetObjectLock(bool b)
{
	if(m_pProc)
		m_pProc->SetLocked(b);
}
bool CLandsideProcDlg::IsObjectLocked()const
{
	if(m_pProc)
		return m_pProc->GetLocked();
	return false;
}

void CLandsideProcDlg::InitData()
{

}

FallbackReason CLandsideProcDlg::GetFallBackReason()
{
	if(m_hRClickItem == m_hCtrlPoints)
		return PICK_MANYPOINTS;
	return PICK_START_INDEX;
}

int CLandsideProcDlg::GetProjectID()
{
	return GetDocument()->GetProjectID();
}


void CLandsideProcDlg::LoadTree()
{
	//load stretch
	m_treeProp.DeleteAllItems();
	m_treeProp.SetRedraw(0);

	CString strLabel = _T("");

	AirsideObjectTreeCtrlItemDataEx aoidDataEx;
	ItemStringSectionColor isscStringColor;
	CString strTemp = _T("");
	HTREEITEM hItemTemp = 0;

	// control points.
	strLabel = CString( "Control Point(") + CARCLengthUnit::GetLengthUnitString(CUnitPiece::GetCurSelLengthUnit()) + CString( ")" );

	LandsideStretch* pstretch = (LandsideStretch*)m_pProc;
	m_hCtrlPoints = m_treeProp.InsertItem(strLabel);
	LoadTreeSubItemCtrlPoints(m_hCtrlPoints,pstretch->getControlPath());	
	m_treeProp.Expand( m_hCtrlPoints, TVE_EXPAND );

	//vertical profile.
	//strLabel = _T("Vertical Profile");
	//m_hVerticalProfile = m_treeProp.InsertItem(strLabel);

	/*CString strSelect = _T("");	
	strLabel.Format(_T("Stretch Sorts : %s"),strSelect);
	aoidDataEx.lSize = sizeof(aoidDataEx);
	aoidDataEx.dwptrItemData = 0;
	aoidDataEx.vrItemStringSectionColorShow.clear();
	isscStringColor.colorSection = RGB(0,0,255); 
	strTemp.Format(_T("%s"),strSelect);
	isscStringColor.strSection = strTemp;
	aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);
	m_hStretchSort = m_treeProp.InsertItem(strLabel);
	m_treeProp.SetItemDataEx(m_hStretchSort,aoidDataEx);*/

	//number of lanes.
	int m_iLaneNum = pstretch->getLaneNum();
	strLabel.Format(_T("Number of Lanes : %d"), m_iLaneNum );
	aoidDataEx.lSize = sizeof(aoidDataEx);
	aoidDataEx.dwptrItemData = 0;
	aoidDataEx.vrItemStringSectionColorShow.clear();
	isscStringColor.colorSection = RGB(0,0,255); 
	strTemp.Format(_T("%d"),m_iLaneNum);
	isscStringColor.strSection = strTemp;
	aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);
	m_hLaneNumber = m_treeProp.InsertItem(strLabel);
	m_treeProp.SetItemDataEx(m_hLaneNumber,aoidDataEx);

	//lane width.
	strLabel.Format(_T("Lane Width: %.2f (%s) "),CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),pstretch->getLaneWidth()) , CARCLengthUnit::GetLengthUnitString(CUnitPiece::GetCurSelLengthUnit())  );
	aoidDataEx.lSize = sizeof(aoidDataEx);
	aoidDataEx.dwptrItemData = 0;
	aoidDataEx.vrItemStringSectionColorShow.clear();
	isscStringColor.colorSection = RGB(0,0,255); 
	strTemp.Format(_T("%.2f"),CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),pstretch->getLaneWidth()));
	isscStringColor.strSection = strTemp;
	aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);
	m_hLaneWidth = m_treeProp.InsertItem(strLabel);
	m_treeProp.SetItemDataEx(m_hLaneWidth,aoidDataEx);

	m_treeProp.SetRedraw(1);
}

void CLandsideProcDlg::LoadTreeSubItemCtrlPoints( HTREEITEM preItem,const ControlPath& path )
{
	//m_vLevelList.ReadFullLevelList(); 
//
	AirsideObjectTreeCtrlItemDataEx aoidDataEx;
	ItemStringSectionColor isscStringColor;
	CString strTemp = _T("");
	HTREEITEM hItemTemp = 0;

	int iPathCount = path.getCount();
	for(int i = 0;i < iPathCount; i++)
	{
		CString csPoint;
		double FloorAlt = path.getPoint(i).m_pt.getZ()/SCALE_FACTOR;//floors.getVisibleFloorAltitude() ;

		aoidDataEx.lSize = sizeof(aoidDataEx);
		aoidDataEx.dwptrItemData = 0;
		aoidDataEx.vrItemStringSectionColorShow.clear();
		isscStringColor.colorSection = RGB(0,0,255);
		strTemp.Format(_T("%.2f"),CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),path.getPoint(i).m_pt.getX()));		
		isscStringColor.strSection = strTemp;
		aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);

		strTemp.Format(_T("%.2f"),CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),path.getPoint(i).m_pt.getY()));		
		isscStringColor.strSection = strTemp;
		aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);

		strTemp.Format(_T("%.2f"),FloorAlt);		
		isscStringColor.strSection = strTemp;
		aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);

		csPoint.Format("x=%.2f; y =%.2f ; floor z = %.2f",CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(), path.getPoint(i).m_pt.getX() ), \
			CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(), path.getPoint(i).m_pt.getY() ), FloorAlt);
		hItemTemp = m_treeProp.InsertItem( csPoint, preItem  );
		m_treeProp.SetItemDataEx(hItemTemp,aoidDataEx);
	}
}

void CLandsideProcDlg::OnContextMenu( CMenu& menuPopup, CMenu *& pMenu )
{
	if(m_hRClickItem == m_hCtrlPoints)
	{
		pMenu=menuPopup.GetSubMenu(73);
	}
	else if (m_hRClickItem == m_hVerticalProfile)
	{
		pMenu = menuPopup.GetSubMenu( 74 );
	}
	else if (m_hRClickItem == m_hLaneNumber)
	{
		pMenu = menuPopup.GetSubMenu(75);
	}
	else if (m_hRClickItem == m_hLaneWidth)
	{
		pMenu = menuPopup.GetSubMenu(76);
	}
	else if (m_hRClickItem == m_hStretchSort)
	{
		pMenu = menuPopup.GetSubMenu(87);
	}
}

void CLandsideProcDlg::DonePickPoints( const CPath2008& pickpoints )
{
	if(m_hCtrlPoints == m_hRClickItem)
	{
		LandsideStretch* pStretch = (LandsideStretch*)m_pProc;
		pStretch->getControlPath().setPath(pickpoints);
		LoadTree();
	}
}

LRESULT CLandsideProcDlg::DefWindowProc( UINT message, WPARAM wParam, LPARAM lParam )
{
	LandsideStretch* pStretch = (LandsideStretch*)m_pProc;
	if( message == WM_INPLACE_SPIN && pStretch)
	{
		LPSPINTEXT pst = (LPSPINTEXT) lParam;
		if (m_hRClickItem == m_hLaneWidth)
		{			
			double m_dLaneWidth = ( CARCLengthUnit::ConvertLength(CUnitPiece::GetCurSelLengthUnit(),DEFAULT_DATABASE_LENGTH_UNIT,pst->iPercent) );
			if(m_dLaneWidth <= 0)
				m_dLaneWidth = 1.0;
			m_bPropModified = true;
			
			pStretch->setLaneWidth(m_dLaneWidth);
			

		}
		else if (m_hRClickItem == m_hLaneNumber)
		{
			int m_iLaneNum = (pst->iPercent);		
			if(m_iLaneNum <= 0)
				m_iLaneNum = 1;
			m_bPropModified = true;
			
			pStretch->setLaneNumber(m_iLaneNum);		
			

		}
//		UpdateObjectViews();
		LoadTree();		
	}
	return CARCLayoutObjectDlg::DefWindowProc(message, wParam, lParam);
}
