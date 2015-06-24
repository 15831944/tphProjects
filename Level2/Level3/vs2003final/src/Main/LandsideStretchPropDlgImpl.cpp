#include "stdafx.h"
#include "resource.h"
#include "LandsideStretchPropDlgImpl.h"

#include "Landside/InputLandside.h"
#include "common/WinMsg.h"
#include ".\landsideparkinglotpropdlgimpl.h"
#include "landside\LandsideLayoutObject.h"
#include "ARCLayoutObjectDlg.h"
#include <common/Path2008.h>
#include "landside/LandsideParkingLot.h"
#include "Landside/ILandsideEditContext.h"


//////////////////////////////////////////////////////////////////////////
LandsideStretchPropDlgImpl::LandsideStretchPropDlgImpl( LandsideFacilityLayoutObject* pObj,CARCLayoutObjectDlg* pDlg )
:ILayoutObjectPropDlgImpl(pObj,pDlg)
{
	
}

CString LandsideStretchPropDlgImpl::getTitle()
{
	//if(m_pObject->getID()<0)
	//	return "New Landside Stretch";
	//else
	//	return "Edit Landside Stretch";
	return _T("Landside Stretch Specification");
}

const static CString sSCType = "Stretch Type";

void LandsideStretchPropDlgImpl::LoadTree()
{
	//load stretch
	CAirsideObjectTreeCtrl& m_treeProp = GetTreeCtrl();
	ARCUnit_Length  curLengthUnit = GetCurLengthUnit();
	m_treeProp.DeleteAllItems();
	m_treeProp.SetRedraw(0);
	LandsideStretch* pstretch = getStretch();


	CString strLabel = _T("");

	AirsideObjectTreeCtrlItemDataEx aoidDataEx;
	ItemStringSectionColor isscStringColor;
	CString strTemp = _T("");
	HTREEITEM hItemTemp = 0;

	

	// control points.
	strLabel = CString( "Control Point(") + CARCLengthUnit::GetLengthUnitString(curLengthUnit) + CString( ")" );

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
	
	m_hType = m_treeProp.InsertItem(sSCType);
	TreeCtrlItemInDlg typeItem(m_treeProp, m_hType);
	typeItem.SetStringText(sSCType, pstretch->getStringStretchType());

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
	strLabel.Format(_T("Lane Width: %.2f (%s) "),CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,curLengthUnit,pstretch->getLaneWidth()) , CARCLengthUnit::GetLengthUnitString(curLengthUnit)  );
	aoidDataEx.lSize = sizeof(aoidDataEx);
	aoidDataEx.dwptrItemData = 0;
	aoidDataEx.vrItemStringSectionColorShow.clear();
	isscStringColor.colorSection = RGB(0,0,255); 
	strTemp.Format(_T("%.2f"),CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,curLengthUnit,pstretch->getLaneWidth()));
	isscStringColor.strSection = strTemp;
	aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);
	m_hLaneWidth = m_treeProp.InsertItem(strLabel);
	m_treeProp.SetItemDataEx(m_hLaneWidth,aoidDataEx);

	m_hSideWalkWidth = NULL;
	if(getStretch()->getStretchType()==LandsideStretch::_street)
	{
		CString sSCSidewalkWid = "Side Walk Width";
		m_hSideWalkWidth = m_treeProp.InsertItem(sSCSidewalkWid);
		TreeCtrlItemInDlg walkitem(m_treeProp, m_hSideWalkWidth);
		walkitem.SetLengthValueText(sSCSidewalkWid, pstretch->getSideWalkWidth(),curLengthUnit);
	}

	m_treeProp.SetRedraw(1);
}

FallbackReason LandsideStretchPropDlgImpl::GetFallBackReason()
{
	if(m_hRClickItem == m_hCtrlPoints)
		return PICK_MANYPOINTS;
	return PICK_START_INDEX;
}

void LandsideStretchPropDlgImpl::DonePickPoints( const CPath2008& pickpoints )
{
	if(m_hCtrlPoints == m_hRClickItem)
	{
		ILandsideEditContext* plandsideCtx = GetLandsideEditContext();
		ASSERT(plandsideCtx);
		if(!plandsideCtx)return;
	
		int nLevel = plandsideCtx->GetActiveLevel();
		LandsideStretch* pStretch = (LandsideStretch*)m_pObject;
		//set the level
		CPath2008 pathret= pickpoints;
		for(int i=0;i<pickpoints.getCount();i++){
			pathret[i].z = nLevel;
		}
		//
		pStretch->getControlPath().setPath(pathret);
		LoadTree();
		Update3D();	
	}
}

void LandsideStretchPropDlgImpl::OnContextMenu( CMenu& menuPopup, CMenu *&pMenu )
{
	if(m_hRClickItem == m_hCtrlPoints)
	{
		pMenu=menuPopup.GetSubMenu(103);
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

void LandsideStretchPropDlgImpl::OnDoubleClickPropTree( HTREEITEM hTreeItem )
{
	m_hRClickItem = hTreeItem;
	if(hTreeItem == m_hLaneNumber)
	{
		m_hRClickItem = hTreeItem;
		OnDefineNumber(hTreeItem);
	}

	if(hTreeItem == m_hLaneWidth)
	{
		m_hRClickItem = hTreeItem;
		OnDefineWidth(hTreeItem);
	}
	if(hTreeItem == m_hType)
	{
		m_hRClickItem = hTreeItem;
		OnDefineType(hTreeItem);
	}
	if(hTreeItem == m_hSideWalkWidth)
	{
		m_hRClickItem = hTreeItem;
		OnDefineSideWalk(hTreeItem);
	}
}

void LandsideStretchPropDlgImpl::OnDefineNumber( HTREEITEM hTreeItem )
{
	CAirsideObjectTreeCtrl& m_treeProp = m_pParentDlg->GetTreeCtrl();
	LandsideStretch* pStretch = (LandsideStretch*)m_pObject;
	int m_iLaneNum = pStretch->getLaneNum();
	m_treeProp.SetDisplayType( 2 );
	m_treeProp.SetDisplayNum(m_iLaneNum);
	m_treeProp.SetSpinRange( 1,40);
	m_treeProp.SpinEditLabel( hTreeItem );
}

void LandsideStretchPropDlgImpl::OnDefineWidth( HTREEITEM hTreeItem )
{
	CAirsideObjectTreeCtrl& m_treeProp = m_pParentDlg->GetTreeCtrl();
	LandsideStretch* pStretch = (LandsideStretch*)m_pObject;
	double m_dLaneWidth = pStretch->getLaneWidth();
	m_treeProp.SetDisplayType( 2 );
	m_treeProp.SetDisplayNum( static_cast<int>(ConvertLength(m_dLaneWidth)+0.5) );		
	m_treeProp.SpinEditLabel( hTreeItem );
}

BOOL LandsideStretchPropDlgImpl::OnDefWindowProc( UINT message, WPARAM wParam, LPARAM lParam )
{
	LandsideStretch* pStretch = (LandsideStretch*)m_pObject;
	if( message == WM_INPLACE_SPIN )
	{
		LPSPINTEXT pst = (LPSPINTEXT) lParam;
		if (m_hRClickItem == m_hLaneWidth)
		{			
			double m_dLaneWidth = ( CARCLengthUnit::ConvertLength(GetCurLengthUnit(),DEFAULT_DATABASE_LENGTH_UNIT,pst->iPercent) );
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
		else if(m_hRClickItem == m_hSideWalkWidth)
		{
			double m_dLaneWidth = UnConvertLength(pst->iPercent);
			if(m_dLaneWidth <= 0)
				m_dLaneWidth = 1.0;
			m_bPropModified = true;
			pStretch->setSideWalkWidth(m_dLaneWidth);	
		}

		LoadTree();
		Update3D();
		return TRUE;
	}
	if(message == WM_INPLACE_COMBO2)
	{
		if(m_hRClickItem == m_hType)
		{
			int iSel = GetTreeCtrl().m_comboBox.GetCurSel();
			iSel = MIN(LandsideStretch::getStretchTypeCount(),iSel);
			iSel = MAX(0,iSel);
			if(getStretch()->getStretchType()!=iSel)
			{
				getStretch()->setStretchType((LandsideStretch::TYPE)iSel);
				LoadTree();
				Update3D();
			}		

		}
	}
	return FALSE;
}

void LandsideStretchPropDlgImpl::DoFallBackFinished( WPARAM wParam, LPARAM lParam )
{
	CPath2008 path;
	GetFallBackAsPath(wParam,lParam,path);
	DonePickPoints(path);
}

void LandsideStretchPropDlgImpl::OnDefineType(HTREEITEM hTreeItem)
{
	std::vector<CString> vString;
	for(int i=0;i<LandsideStretch::getStretchTypeCount();i++)
		vString.push_back(LandsideStretch::getStretchTypeString((LandsideStretch::TYPE)i));


	GetTreeCtrl().SetComboWidth(300);
	GetTreeCtrl().SetComboString(hTreeItem,vString);
	GetTreeCtrl().m_comboBox.SetCurSel(getStretch()->getStretchType());
	

}

LandsideStretch* LandsideStretchPropDlgImpl::getStretch()
{
	return (LandsideStretch*) m_pObject;
}

void LandsideStretchPropDlgImpl::OnDefineSideWalk(HTREEITEM hTreeItem)
{
	CAirsideObjectTreeCtrl& m_treeProp = m_pParentDlg->GetTreeCtrl();
	LandsideStretch* pStretch = (LandsideStretch*)m_pObject;
	double m_dLaneWidth = pStretch->getSideWalkWidth();
	m_treeProp.SetDisplayType( 2 );	
	m_treeProp.SetDisplayNum( static_cast<int>(ConvertLength(m_dLaneWidth)+0.5) );	
	m_treeProp.SpinEditLabel( hTreeItem );
}

bool LandsideStretchPropDlgImpl::OnOK( CString& errorMsg )
{
	if(LandsideStretch* pStretch = getStretch())
	{
		if( pStretch->getControlPath().getCount()<2 )
		{
			errorMsg = _T("Pick more than 2 points");
			return false;
		}
		return __super::OnOK(errorMsg);
	}	
	return false;
}

void LandsideStretchPropDlgImpl::OnControlPathReverse()
{
	if(LandsideStretch* pStretch = getStretch())
	{
		
		{
			pStretch->getControlPath().Reverse();
			LoadTree();
			Update3D();	
		}		
	}	
}
