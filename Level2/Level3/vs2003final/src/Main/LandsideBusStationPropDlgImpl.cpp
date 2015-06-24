#include "stdafx.h"
#include "resource.h"
#include "LandsideBusStationPropDlgImpl.h"
#include "Landside/InputLandside.h"
#include "common/WinMsg.h"
#include "landside\LandsideLayoutObject.h"
#include "ARCLayoutObjectDlg.h"
#include <common/Path2008.h>
#include "TermplanDoc.h"
#include <commondata/QueryData.h>
#include "landside/LandsideQueryData.h"
#include "landside/LandsideBusStation.h"

//////////////////////////////////////////////////////////////////////////
LandsideBusStationPropDlgImpl::LandsideBusStationPropDlgImpl( LandsideFacilityLayoutObject* pObj,CARCLayoutObjectDlg* pDlg )
:ILayoutObjectPropDlgImpl(pObj,pDlg)
{
	m_hEmbedParkinglot = NULL;
}

CString LandsideBusStationPropDlgImpl::getTitle()
{
	/*if (m_pObject->getID()<0)
		return "New Landside BusStation";
	else
		return "Edit Landside BusStation";*/
	return _T("Landside Busstation Specification");
}

void LandsideBusStationPropDlgImpl::LoadTree()
{
	LandsideBusStation* pBusStation = getBusStation();
	if(!pBusStation) return;

	CAirsideObjectTreeCtrl& m_treeProp = GetTreeCtrl();
	ARCUnit_Length  curLengthUnit = GetCurLengthUnit();
	CString curLengthUnitString = CARCLengthUnit::GetLengthUnitString(curLengthUnit);


	m_treeProp.DeleteAllItems();
	m_hRClickItem = NULL;
	m_treeProp.SetRedraw(0);	

	//linked stretch
	{
		CString sStretch;
		LandsideStretch* pStretch = pBusStation->getStrech();
		if (pStretch)
			sStretch = pStretch->getName().GetIDString();
		m_hStretch = m_treeProp.InsertItem("Link Stretch");
		TreeCtrlItemInDlg stretchItem(m_treeProp,m_hStretch);
		stretchItem.SetStringText("Link Stretch", sStretch);

		//lanefrom
		{
			CString sLaneFrom = "Lane From";			
			TreeCtrlItemInDlg fromItem  = stretchItem.AddChild(sLaneFrom);
			m_hLaneFrom = fromItem.m_hItem;
			fromItem.SetIntValueText(sLaneFrom, pBusStation->m_nLaneFrom);
		}
		//laneto
		{
			CString sLaneTo = "Lane To";
			TreeCtrlItemInDlg toItem = stretchItem.AddChild(sLaneTo);
			m_hLaneTo = toItem.m_hItem;
			toItem.SetIntValueText(sLaneTo, pBusStation->m_nLaneTo);
		}
		stretchItem.Expand();
		
	}

	//bus station
	{
		CString sPath = "Location";
		m_hPath = m_treeProp.InsertItem(sPath);
		TreeCtrlItemInDlg pathItem(m_treeProp, m_hPath);
		pathItem.AddChildPath(getBusStation()->getPath(),GetCurLengthUnit());
		pathItem.Expand();
	}
	
	

	//waiting path
	{
		CString sPath = "waiting path";
		m_hWaitingPath = m_treeProp.InsertItem(sPath);
		TreeCtrlItemInDlg waitItem(m_treeProp, m_hWaitingPath);
		m_treeProp.SetItemText(m_hWaitingPath, sPath);

		CPath2008& path = pBusStation->getWaittingPath();
		for (int i = 0; i < path.getCount(); i++)
		{
			AirsideObjectTreeCtrlItemDataEx aoidDataEx;
			ItemStringSectionColor isscStringColor;

			//coordinate
			CString str, csPoint;
			aoidDataEx.lSize = sizeof(aoidDataEx);
			aoidDataEx.dwptrItemData = 0;
			aoidDataEx.vrItemStringSectionColorShow.clear();
			isscStringColor.colorSection = RGB(0,0,255);

			double xUnit = CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,curLengthUnit,path[i].x);
			double yUnit = CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,curLengthUnit,path[i].y);

			//set x, y, z text value and text colour
			str.Format(_T("%.2f"),xUnit);		
			isscStringColor.strSection = str;
			aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);

			str.Format(_T("%.2f"),yUnit);		
			isscStringColor.strSection = str;
			aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);

			csPoint.Format("point %d: x=%.2f; y =%.2f; z =%.2f ",i, xUnit, yUnit, curLengthUnit,path[i].z +1  );

			HTREEITEM item = m_treeProp.InsertItem( csPoint, m_hWaitingPath);
			m_treeProp.SetItemDataEx(item,aoidDataEx);
		}
		waitItem.Expand();
	}

	{
		int nParkingLotID = pBusStation->getEmbedParkingLotID();
		
		LandsideFacilityLayoutObject* pParkingLot = NULL;
		if(m_pParentDlg)
			pParkingLot = m_pParentDlg->getInputLandside()->getObjectList().getObjectByID(nParkingLotID);

		CString strParkingLotName;
		if(pParkingLot)
		{
			strParkingLotName = pParkingLot->getName().GetIDString();
		}
		else
			strParkingLotName = _T("NOT EMBEDDED");

		CString sItemTextParkingLot = _T("Embedded in Parking Lot");
		m_hEmbedParkinglot = m_treeProp.InsertItem(sItemTextParkingLot);
		TreeCtrlItemInDlg parkinglotItem(m_treeProp, m_hEmbedParkinglot);
		parkinglotItem.SetStringText(sItemTextParkingLot, strParkingLotName);
		

	}

	m_treeProp.SetRedraw(1);
}


FallbackReason LandsideBusStationPropDlgImpl::GetFallBackReason()
{
	if(m_hRClickItem == m_hStretch)
		return PICK_STRETCH_POS;

	if(m_hRClickItem == m_hWaitingPath)
		return PICK_MANYPOINTS;

	return PICK_TWOPOINTS;
}

void LandsideBusStationPropDlgImpl::OnContextMenu(CMenu& menuPopup, CMenu *&pMenu)
{
	if(m_hRClickItem == m_hStretch || m_hRClickItem == m_hPath || m_hRClickItem == m_hWaitingPath)
	{		
		pMenu = menuPopup.GetSubMenu(73);	
	}

}

void LandsideBusStationPropDlgImpl::OnDoubleClickPropTree(HTREEITEM hTreeItem)
{
	m_hRClickItem = hTreeItem;
	if (hTreeItem == m_hLaneFrom)
		OnEditLaneFrom(hTreeItem);
	else if (hTreeItem == m_hLaneTo)
		OnEditLaneTo(hTreeItem);
	else if (hTreeItem == m_hEmbedParkinglot)
	{
		//get all parking lots

		m_lstParkinglot.clear();

		if(m_pParentDlg)
			m_pParentDlg->getInputLandside()->getObjectList().GetObjectList(m_lstParkinglot, ALT_LPARKINGLOT);
		

		std::vector<CString> vParkingLotName;
		vParkingLotName.push_back(_T("NOT EMBEDDED"));

		int nParkinglotCount = static_cast<int>(m_lstParkinglot.size());
		for (int nParkingLot = 0; nParkingLot < nParkinglotCount; ++ nParkingLot)
		{
			LandsideFacilityLayoutObject* pParkingLot = m_lstParkinglot.at(nParkingLot);
			if(pParkingLot)
			{
				vParkingLotName.push_back(pParkingLot->getName().GetIDString());
			}
		}

		GetTreeCtrl().SetComboWidth(120);
		GetTreeCtrl().SetComboString(hTreeItem, vParkingLotName);

	}
}

void LandsideBusStationPropDlgImpl::DoFallBackFinished(WPARAM wParam, LPARAM lParam)
{
	LandsideBusStation* pBusStation = getBusStation();

	if(!pBusStation) return;

	if(m_hRClickItem == m_hStretch)
	{
		QueryData& qdata = *((QueryData*)wParam);
		LandsideStretch*pStretch = NULL;
		double dPos = 0;
		if( qdata.GetData(KeyAltObjectPointer,(int&)pStretch) 
			&&	qdata.GetData(KeyPosInStretch,dPos)  
			&& pStretch )
		{
			pBusStation->SetStretch(pStretch);
			pBusStation->SetPosInStretch(dPos);
			LoadTree();
			Update3D();
		}
	}
	if (m_hRClickItem == m_hPath)
	{
		CPath2008& path = pBusStation->getPath();
		GetFallBackAsPath(wParam, lParam, path);
		LoadTree();
		Update3D();
	}
	if (m_hRClickItem == m_hWaitingPath)
	{
		CPath2008& path = pBusStation->getWaittingPath();
		GetFallBackAsPath(wParam, lParam, path);
		LoadTree();
		Update3D();
	}
}

LandsideBusStation* LandsideBusStationPropDlgImpl::getBusStation()
{
	if (m_pObject->GetType() == ALT_LBUSSTATION)
	{
		return (LandsideBusStation*)m_pObject;
	}
	return NULL;
}

void LandsideBusStationPropDlgImpl::OnEditLaneFrom(HTREEITEM hItem)
{
	LandsideBusStation* pBusStation = (LandsideBusStation*)m_pObject;
	LandsideStretch* pStretch = pBusStation->getStrech();
	int maxNum = pStretch->getLaneNum();

	GetTreeCtrl().SetDisplayType( 2 );
	GetTreeCtrl().SetSpinRange(1,maxNum);
	GetTreeCtrl().SetDisplayNum(pBusStation->GetLaneFrom());	
	GetTreeCtrl().SpinEditLabel( hItem );
}

void LandsideBusStationPropDlgImpl::OnEditLaneTo(HTREEITEM hItem)
{
	LandsideBusStation* pBusStation = (LandsideBusStation*)m_pObject;
	LandsideStretch* pStretch = pBusStation->getStrech();
	int maxNum = pStretch->getLaneNum();

	GetTreeCtrl().SetDisplayType( 2 );
	GetTreeCtrl().SetSpinRange(1,maxNum);
	GetTreeCtrl().SetDisplayNum(pBusStation->GetLaneTo());	
	GetTreeCtrl().SpinEditLabel( hItem );
}

BOOL LandsideBusStationPropDlgImpl::OnDefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (!m_hRClickItem)
		return TRUE;

	LandsideBusStation* pBusStation = getBusStation();

	if( message == WM_INPLACE_SPIN )
	{
		if(m_hLaneFrom == m_hRClickItem)
		{
			LPSPINTEXT pst = (LPSPINTEXT) lParam;
			int nLaneIdx = pst->iPercent;
			pBusStation->m_nLaneFrom = nLaneIdx;			
		}
		if (m_hLaneTo == m_hRClickItem)
		{
			LPSPINTEXT pst = (LPSPINTEXT) lParam;
			int nLaneIdx = pst->iPercent;
			pBusStation->SetLaneTo(nLaneIdx);
		}

		LoadTree();	
		Update3D();

	}
	if (message == WM_INPLACE_COMBO2)
	{
		if (m_hRClickItem == m_hEmbedParkinglot)
		{
			int selitem = GetTreeCtrl().m_comboBox.GetCurSel();	
			if(selitem >=0)
			{
				if(selitem == 0)//not embedded
					pBusStation->setEmbedParkingLotID(-1);
				else
				{
					int nCount = static_cast<int>(m_lstParkinglot.size());
					if( selitem - 1 < nCount)//ignore the NOT EMBEDED item
					{
						LandsideFacilityLayoutObject* pParkingLot = m_lstParkinglot.at( selitem - 1);
						if(pParkingLot)
							pBusStation->setEmbedParkingLotID(pParkingLot->getID());
					}
				}

				LoadTree();	
				Update3D();
			}
		}
	}

	return TRUE;
}

bool LandsideBusStationPropDlgImpl::OnOK( CString& errorMsg )
{
	if(LandsideBusStation* pBusStation = getBusStation())
	{
		if( !pBusStation->getStrech() )
		{
			errorMsg = _T("define BusStation on a stretch");
			return false;
		}
		return __super::OnOK(errorMsg);
	}	
	return false;
}

