#include "StdAfx.h"
#include "resource.h"
#include ".\landsidedecisionpointpropdlgimpl.h"
#include "Landside/InputLandside.h"
#include "common/WinMsg.h"
#include "landside\LandsideLayoutObject.h"
#include "ARCLayoutObjectDlg.h"
#include <common/Path2008.h>
#include "TermplanDoc.h"
#include <commondata/QueryData.h>
#include "landside/LandsideQueryData.h"
#include "landside/LandsideControlDevice.h"

LandsideDecisionPointPropDlgImpl::LandsideDecisionPointPropDlgImpl(LandsideFacilityLayoutObject* pObj,CARCLayoutObjectDlg* pDlg)
:ILayoutObjectPropDlgImpl(pObj,pDlg)
{
}

LandsideDecisionPointPropDlgImpl::~LandsideDecisionPointPropDlgImpl(void)
{
}

CString LandsideDecisionPointPropDlgImpl::getTitle()
{
	return _T("Landside Decision Point Specification");
}

void LandsideDecisionPointPropDlgImpl::LoadTree()
{
	LandsideDecisionPoint* pPoint = getDecisionPoint();

	CAirsideObjectTreeCtrl& m_treeProp = GetTreeCtrl();
	ARCUnit_Length  curLengthUnit = GetCurLengthUnit();
	CString curLengthUnitString = CARCLengthUnit::GetLengthUnitString(curLengthUnit);

	m_treeProp.DeleteAllItems();
	m_hRClickItem = NULL;
	m_treeProp.SetRedraw(0);	

	TreeCtrlItemInDlg rootItem = TreeCtrlItemInDlg::GetRootItem(m_treeProp);

	//position
	CString strLabel;
	strLabel  = _T("Position:");
	TreeCtrlItemInDlg posItem = rootItem.AddChild(strLabel);
	m_hPos = posItem.m_hItem;

	//link stretch
	{
		CString sStretch;
		LandsideStretch* pStretch = pPoint->getStretch();
		if (pStretch)
			sStretch = pStretch->getName().GetIDString();
		m_hStretch = m_treeProp.InsertItem("Link Stretch",m_hPos);
		TreeCtrlItemInDlg stretchItem(m_treeProp,m_hStretch);
		stretchItem.SetStringText("Link Stretch", sStretch);
	}

	//distance in stretch
	{
		CString sDistance = "Distance in Stretch";
		m_hDist = m_treeProp.InsertItem(sDistance,m_hPos);
		TreeCtrlItemInDlg spdItem(m_treeProp,m_hDist);
		spdItem.SetLengthValueText(sDistance,pPoint->GetPosOnStretch(), GetCurLengthUnit() );
	}
	m_treeProp.Expand(m_hPos,TVE_EXPAND);
	m_treeProp.SetRedraw(1);
}

int LandsideDecisionPointPropDlgImpl::getBitmapResourceID()
{
	return IDB_BITMAP_LANDSIDE_STRETCH;
}

FallbackReason LandsideDecisionPointPropDlgImpl::GetFallBackReason()
{
	if(m_hRClickItem == m_hStretch)
		return PICK_STRETCH_POS;

	return PICK_MANYPOINTS;
}

void LandsideDecisionPointPropDlgImpl::OnContextMenu( CMenu& menuPopup, CMenu *&pMenu )
{
	if(m_hRClickItem == m_hStretch)
	{		
		pMenu = menuPopup.GetSubMenu(73);	
	}

}

void LandsideDecisionPointPropDlgImpl::OnDoubleClickPropTree( HTREEITEM hTreeItem )
{
	m_hRClickItem = hTreeItem;	
	if(m_hRClickItem == m_hDist)
	{
		if( LandsideDecisionPoint* pPoint = getDecisionPoint())
		{
			double dUnitNum = ConvertLength(pPoint->GetPosOnStretch());	
			double dMaxLen = 10000;		

			GetTreeCtrl().SetDisplayType( 2 );
			GetTreeCtrl().SetSpinRange(0,static_cast<int>(dMaxLen) );
			GetTreeCtrl().SetDisplayNum(static_cast<int>(dUnitNum+0.5));	
			GetTreeCtrl().SpinEditLabel( hTreeItem );
		}
	}
}

void LandsideDecisionPointPropDlgImpl::DoFallBackFinished( WPARAM wParam, LPARAM lPara )
{
	LandsideControlDevice* pPoint = getDecisionPoint();

	if(m_hRClickItem == m_hStretch && pPoint)
	{
		QueryData& qdata = *((QueryData*)wParam);
		LandsideStretch*pStretch = NULL;
		double dPos = 0;
		if( qdata.GetData(KeyAltObjectPointer,(int&)pStretch) 
			&&	qdata.GetData(KeyPosInStretch,dPos)  
			&& pStretch )
		{
			pPoint->setStretch(pStretch);
			pPoint->setPosOnStretch(dPos);
			LoadTree();
			Update3D();
		}

	}
}

BOOL LandsideDecisionPointPropDlgImpl::OnDefWindowProc( UINT message, WPARAM wParam, LPARAM lParam )
{
	if (!m_hRClickItem)
		return TRUE;
	if( message == WM_INPLACE_SPIN )
	{
		if(m_hDist == m_hRClickItem)
		{
			if(LandsideDecisionPoint* pPoint = getDecisionPoint())
			{
				LPSPINTEXT pst = (LPSPINTEXT) lParam;
				double dValue = UnConvertLength(pst->iPercent);
				pPoint->setPosOnStretch(dValue);

				LoadTree();	
				Update3D();
			}
		}
	}	
	return TRUE;
}

bool LandsideDecisionPointPropDlgImpl::OnOK( CString& errorMsg )
{
	if(LandsideControlDevice* pdevice = getDecisionPoint())
	{
		if( !pdevice->getStretch() )
		{
			errorMsg = _T("Must define the decision point on a stretch");
			return false;
		}	
		return __super::OnOK(errorMsg);
	}	
	return false;
}

LandsideDecisionPoint* LandsideDecisionPointPropDlgImpl::getDecisionPoint()
{
	if (m_pObject->GetType() == ALT_LDECISIONPOINT)
	{
		return (LandsideDecisionPoint*)m_pObject;
	}
	return NULL;
}

void LandsideDecisionPointPropDlgImpl::InitUnitPiece( CUnitPiece* unit )
{
	unit->SetUnitInUse(	ARCUnit_Length_InUse); 
}
