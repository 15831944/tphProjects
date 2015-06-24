#include "stdafx.h"
#include "resource.h"
#include "LandsideControlDevicePropDlgImpl.h"
#include "Landside/InputLandside.h"
#include "common/WinMsg.h"
#include "landside\LandsideLayoutObject.h"
#include "ARCLayoutObjectDlg.h"
#include <common/Path2008.h>
#include "TermplanDoc.h"
#include <commondata/QueryData.h>
#include "landside/LandsideQueryData.h"
#include "landside/LandsideControlDevice.h"


//////////////////////////////////////////////////////////////////////////
LandsideControlDevicePropDlgImpl::LandsideControlDevicePropDlgImpl( LandsideFacilityLayoutObject* pObj,CARCLayoutObjectDlg* pDlg )
:ILayoutObjectPropDlgImpl(pObj,pDlg)
{
}

CString LandsideControlDevicePropDlgImpl::getTitle()
{	
	if (m_pObject->GetType() == ALT_LTRAFFICLIGHT)
	{
		return _T("Landside TrafficLight Specification");
	}
	if (m_pObject->GetType() == ALT_LTOLLGATE)
	{
		return _T("Landside TollGate Specification");
	}
	if (m_pObject->GetType() == ALT_LSTOPSIGN)
	{
		return _T("Landside StopSign Specification");
	}
	if (m_pObject->GetType() == ALT_LYIELDSIGN)
	{
		return _T("Landside YieldSign Specification");
	}
	if(m_pObject->GetType() == ALT_LSPEEDSIGN)
	{
		return _T("Landside SpeedSign Specification");
	}
	return _T("Landside ControlDevice Specification");
}
#define  sSpeedLimit "Speed Limit"

void LandsideControlDevicePropDlgImpl::LoadTree()
{
	LandsideControlDevice* pDevie = getDevice();
	
	CAirsideObjectTreeCtrl& m_treeProp = GetTreeCtrl();
	ARCUnit_Length  curLengthUnit = GetCurLengthUnit();
	CString curLengthUnitString = CARCLengthUnit::GetLengthUnitString(curLengthUnit);


	m_treeProp.DeleteAllItems();
	m_hRClickItem = NULL;
	m_treeProp.SetRedraw(0);	

	
	//linked stretch
	{
		CString sStretch;
		LandsideStretch* pStretch = pDevie->getStretch();
		if (pStretch)
			sStretch = pStretch->getName().GetIDString();
		m_hStretch = m_treeProp.InsertItem("Link Stretch");
		TreeCtrlItemInDlg stretchItem(m_treeProp,m_hStretch);
		stretchItem.SetStringText("Link Stretch", sStretch);
	}
	//speed limit
	if(LandsideSpeedSign* pSpeedSing =  pDevie->toSpeedSign() )
	{
		m_hSpdLimit = m_treeProp.InsertItem(sSpeedLimit);
		TreeCtrlItemInDlg spdItem(m_treeProp,m_hSpdLimit);
		spdItem.SetSpeedValueText(sSpeedLimit,pSpeedSing->m_dSpeedLimit, GetCurSpeedUnit() );
	}
	m_treeProp.SetRedraw(1);
}

FallbackReason LandsideControlDevicePropDlgImpl::GetFallBackReason()
{
	if(m_hRClickItem == m_hStretch)
		return PICK_STRETCH_POS;

	return PICK_MANYPOINTS;
}

void LandsideControlDevicePropDlgImpl::OnContextMenu(CMenu& menuPopup, CMenu *&pMenu)
{
	if(m_hRClickItem == m_hStretch)
	{		
		pMenu = menuPopup.GetSubMenu(73);	
	}

}

void LandsideControlDevicePropDlgImpl::OnDoubleClickPropTree(HTREEITEM hTreeItem)
{
	m_hRClickItem = hTreeItem;	
	if(m_hRClickItem == m_hSpdLimit)
	{
		if( LandsideSpeedSign* spdSign = getDevice()->toSpeedSign() )
		{
			double dUnitNum = ConvertSpeed(spdSign->m_dSpeedLimit);	
			double dMaxLen = 10000;		

			GetTreeCtrl().SetDisplayType( 2 );
			GetTreeCtrl().SetSpinRange(0,static_cast<int>(dMaxLen) );
			GetTreeCtrl().SetDisplayNum(static_cast<int>(dUnitNum+0.5));	
			GetTreeCtrl().SpinEditLabel( hTreeItem );
		}
	}

}

void LandsideControlDevicePropDlgImpl::DoFallBackFinished(WPARAM wParam, LPARAM lPara)
{
	LandsideControlDevice* pdevice = getDevice();

	if(m_hRClickItem == m_hStretch && pdevice)
	{
		QueryData& qdata = *((QueryData*)wParam);
		LandsideStretch*pStretch = NULL;
		double dPos = 0;
		if( qdata.GetData(KeyAltObjectPointer,(int&)pStretch) 
			&&	qdata.GetData(KeyPosInStretch,dPos)  
			&& pStretch )
		{
			pdevice->setStretch(pStretch);
			pdevice->setPosOnStretch(dPos);
			LoadTree();
			Update3D();
		}

	}
}

BOOL LandsideControlDevicePropDlgImpl::OnDefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (!m_hRClickItem)
		return TRUE;
	if( message == WM_INPLACE_SPIN )
	{
		if(m_hSpdLimit == m_hRClickItem)
		{
			if(LandsideSpeedSign* spdSign = getDevice()->toSpeedSign())
			{
				LPSPINTEXT pst = (LPSPINTEXT) lParam;
				double dValue = UnConvertSpeed(pst->iPercent);
				spdSign->m_dSpeedLimit = (dValue);

				LoadTree();	
				Update3D();
			}
		}
	}	
	return TRUE;
}

bool LandsideControlDevicePropDlgImpl::OnOK( CString& errorMsg )
{
	if(LandsideControlDevice* pdevice = getDevice())
	{
		if( !pdevice->getStretch() )
		{
			errorMsg = _T("Must define the controldevice on a stretch");
			return false;
		}	
		return __super::OnOK(errorMsg);
	}	
	return false;
}

LandsideControlDevice* LandsideControlDevicePropDlgImpl::getDevice()
{
	if (m_pObject->GetType() == ALT_LYIELDSIGN
		|| m_pObject->GetType() == ALT_LSTOPSIGN
		|| m_pObject->GetType() ==  ALT_LTOLLGATE
		|| m_pObject->GetType() == ALT_LTRAFFICLIGHT 
		|| m_pObject->GetType() == ALT_LSPEEDSIGN )
	{
		return (LandsideControlDevice*)m_pObject;
	}
	return NULL;
}

int LandsideControlDevicePropDlgImpl::getBitmapResourceID()
{
	if (m_pObject->GetType() == ALT_LTRAFFICLIGHT)
	{
		return IDB_BITMAP_LANDSIDE_TRAFFICLIGHT;
	}
	if (m_pObject->GetType() == ALT_LTOLLGATE)
	{
		return IDB_BITMAP_LANDSIDE_TOLLGATE;
	}
	if (m_pObject->GetType() == ALT_LSTOPSIGN)
	{
		return IDB_BITMAP_LANDSIDE_STOPSIGN;
	}
	if (m_pObject->GetType() == ALT_LYIELDSIGN)
	{
		return IDB_BITMAP_LANDSIDE_YIELDSIGN;
	}
	return IDB_BITMAP_LANDSIDE_CROSSWALK;
}

void LandsideControlDevicePropDlgImpl::InitUnitPiece( CUnitPiece* unit )
{
	unit->SetUnitInUse(	ARCUnit_Length_InUse | ARCUnit_Velocity_InUse); 
}

