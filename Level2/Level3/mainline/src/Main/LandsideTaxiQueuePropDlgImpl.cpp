#include "StdAfx.h"
#include "Resource.h"
#include ".\landsidetaxiqueuepropdlgimpl.h"
#include "Landside/InputLandside.h"
#include "common/WinMsg.h"
#include "landside\LandsideLayoutObject.h"
#include "ARCLayoutObjectDlg.h"
#include <common/Path2008.h>
#include "TermplanDoc.h"
#include <commondata/QueryData.h>
#include "landside/LandsideQueryData.h"
#include "Landside/LandsideTaxiQueue.h"

static const char* strParkingAngle[] = {"Parallel","Angled"};
LandsideTaxiQueuePropDlgImpl::LandsideTaxiQueuePropDlgImpl(LandsideFacilityLayoutObject* pObj,CARCLayoutObjectDlg* pDlg)
:ILayoutObjectPropDlgImpl(pObj,pDlg)
{
}

LandsideTaxiQueuePropDlgImpl::~LandsideTaxiQueuePropDlgImpl(void)
{
}

CString LandsideTaxiQueuePropDlgImpl::getTitle()
{
	return _T("Landside Taxi Queue Specification");
}

void LandsideTaxiQueuePropDlgImpl::LoadTree()
{
	LandsideTaxiQueue* pTaxiQueue = getTaxiQueue();
	if(!pTaxiQueue) return;

	CAirsideObjectTreeCtrl& m_treeProp = GetTreeCtrl();
	ARCUnit_Length  curLengthUnit = GetCurLengthUnit();
	CString curLengthUnitString = CARCLengthUnit::GetLengthUnitString(curLengthUnit);


	m_treeProp.DeleteAllItems();
	m_hRClickItem = NULL;
	m_treeProp.SetRedraw(0);	

	//linked stretch
	{
		CString sStretch;
		LandsideStretch* pStretch = pTaxiQueue->getStrech();
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
			fromItem.SetIntValueText(sLaneFrom, pTaxiQueue->m_nLaneFrom);
		}
		//laneto
		{
			CString sLaneTo = "Lane To";
			TreeCtrlItemInDlg toItem = stretchItem.AddChild(sLaneTo);
			m_hLaneTo = toItem.m_hItem;
			toItem.SetIntValueText(sLaneTo, pTaxiQueue->m_nLaneTo);
		}
		stretchItem.Expand();

	}

	//taxi Parking
	{
		int nIdx = pTaxiQueue->IsAngleParking() ? 1 : 0;
		CString strAngle = "Taxi parking";
		m_hAngle = m_treeProp.InsertItem(strAngle);
		TreeCtrlItemInDlg taxiParkingItem(m_treeProp,m_hAngle);
		taxiParkingItem.SetStringText("Taxi parking", strParkingAngle[nIdx]);

		if (pTaxiQueue->IsAngleParking())
		{
			CString strDegree = "at degrees";
			TreeCtrlItemInDlg angleItem = taxiParkingItem.AddChild(strDegree);
			m_hDegree = angleItem.m_hItem;
			angleItem.SetDegreeValueText(strDegree,pTaxiQueue->GetDegree());
		}
		taxiParkingItem.Expand();
		
	}
	//location
	{
		CString sPath = "Location of sign(draw line for orientation)";
		m_hPath = m_treeProp.InsertItem(sPath);
		TreeCtrlItemInDlg pathItem(m_treeProp, m_hPath);
		pathItem.AddChildPath(getTaxiQueue()->getPath(),GetCurLengthUnit());
		pathItem.Expand();
	}



	//waiting path
	{
		CString sPath = "waiting path";
		m_hWaitingPath = m_treeProp.InsertItem(sPath);
		TreeCtrlItemInDlg waitItem(m_treeProp, m_hWaitingPath);
		m_treeProp.SetItemText(m_hWaitingPath, sPath);

		CPath2008& path = pTaxiQueue->getWaittingPath();
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

			csPoint.Format("point %d: x=%.2f; y =%.2f",i, xUnit, yUnit);

			HTREEITEM item = m_treeProp.InsertItem( csPoint, m_hWaitingPath);
			m_treeProp.SetItemDataEx(item,aoidDataEx);
		}
		waitItem.Expand();
	}

	m_treeProp.SetRedraw(1);
}

FallbackReason LandsideTaxiQueuePropDlgImpl::GetFallBackReason()
{
	if(m_hRClickItem == m_hStretch)
		return PICK_STRETCH_POS;

	if(m_hRClickItem == m_hWaitingPath)
		return PICK_MANYPOINTS;

	return PICK_TWOPOINTS;
}

void LandsideTaxiQueuePropDlgImpl::OnContextMenu( CMenu& menuPopup, CMenu *&pMenu )
{
	if(m_hRClickItem == m_hStretch || m_hRClickItem == m_hPath || m_hRClickItem == m_hWaitingPath)
	{		
		pMenu = menuPopup.GetSubMenu(73);	
	}
}

void LandsideTaxiQueuePropDlgImpl::OnNodeTypeCombo()
{
	std::vector<CString> vString;
	vString.push_back(strParkingAngle[0]);
	vString.push_back(strParkingAngle[1]);
	GetTreeCtrl().SetComboWidth(300);
	GetTreeCtrl().SetComboString(m_hAngle,vString);	
	LandsideTaxiQueue* pTaxiQueue = getTaxiQueue();
	int nCurSel = pTaxiQueue->IsAngleParking();
	GetTreeCtrl().m_comboBox.SetCurSel(nCurSel);
}

void LandsideTaxiQueuePropDlgImpl::OnDoubleClickPropTree( HTREEITEM hTreeItem )
{
	m_hRClickItem = hTreeItem;
	if (hTreeItem == m_hLaneFrom)
		OnEditLaneFrom(hTreeItem);
	else if (hTreeItem == m_hLaneTo)
		OnEditLaneTo(hTreeItem);
	else if (hTreeItem == m_hDegree)
		OnEditDegree(hTreeItem);
	else if (hTreeItem == m_hAngle)
		OnNodeTypeCombo();
}

void LandsideTaxiQueuePropDlgImpl::DoFallBackFinished( WPARAM wParam, LPARAM lPara )
{
	LandsideTaxiQueue* pTaxiQueue = getTaxiQueue();

	if(!pTaxiQueue) return;

	if(m_hRClickItem == m_hStretch)
	{
		QueryData& qdata = *((QueryData*)wParam);
		LandsideStretch*pStretch = NULL;
		double dPos = 0;
		if( qdata.GetData(KeyAltObjectPointer,(int&)pStretch) 
			&&	qdata.GetData(KeyPosInStretch,dPos)  
			&& pStretch )
		{
			pTaxiQueue->SetStretch(pStretch);
			pTaxiQueue->SetPosInStretch(dPos);
			LoadTree();
			Update3D();
		}
	}
	if (m_hRClickItem == m_hPath)
	{
		CPath2008& path = pTaxiQueue->getPath();
		GetFallBackAsPath(wParam, lPara, path);
		LoadTree();
		Update3D();
	}
	if (m_hRClickItem == m_hWaitingPath)
	{
		CPath2008& path = pTaxiQueue->getWaittingPath();
		GetFallBackAsPath(wParam, lPara, path);
		LoadTree();
		Update3D();
	}
}

BOOL LandsideTaxiQueuePropDlgImpl::OnDefWindowProc( UINT message, WPARAM wParam, LPARAM lParam )
{
	if (!m_hRClickItem)
		return TRUE;

	LandsideTaxiQueue* pTaxiQueue = getTaxiQueue();

	if (message == WM_INPLACE_COMBO2)
	{	
		CString strText;
		CAirsideObjectTreeCtrl& m_treeProp = GetTreeCtrl();
		m_treeProp.m_comboBox.GetWindowText(strText);
		int selitem = GetTreeCtrl().m_comboBox.GetCurSel();	
		if (m_hRClickItem == m_hAngle)
		{
			if(selitem >=0)
			{
				BOOL bAngle = selitem ? TRUE : FALSE;
				pTaxiQueue->SetAngleParking(bAngle);
				LoadTree();
				Update3D();
			}
		}
	}
	else if( message == WM_INPLACE_SPIN )
	{
		if(m_hLaneFrom == m_hRClickItem)
		{
			LPSPINTEXT pst = (LPSPINTEXT) lParam;
			pTaxiQueue->SetLaneFrom(pst->iPercent);
		}
		if (m_hLaneTo == m_hRClickItem)
		{
			LPSPINTEXT pst = (LPSPINTEXT) lParam;
			pTaxiQueue->SetLaneTo(pst->iPercent);
		}
		if (m_hDegree == m_hRClickItem)
		{
			LPSPINTEXT pst = (LPSPINTEXT) lParam;
			pTaxiQueue->SetDegree(static_cast<double>(pst->iPercent));
		}

		LoadTree();	
		Update3D();

	}
	return TRUE;
}

bool LandsideTaxiQueuePropDlgImpl::OnOK( CString& errorMsg )
{
	if(LandsideTaxiQueue* pTaxiQueue = getTaxiQueue())
	{
		if( !pTaxiQueue->getStrech() )
		{
			errorMsg = _T("define Taxi Queue on a stretch");
			return false;
		}
		return __super::OnOK(errorMsg);
	}	
	return false;
}

LandsideTaxiQueue* LandsideTaxiQueuePropDlgImpl::getTaxiQueue()
{
	if (m_pObject->GetType() == ALT_LTAXIQUEUE)
	{
		return (LandsideTaxiQueue*)m_pObject;
	}
	return NULL;
}

void LandsideTaxiQueuePropDlgImpl::OnEditLaneFrom( HTREEITEM hItem )
{
	LandsideTaxiQueue* pTaxiQueue = (LandsideTaxiQueue*)m_pObject;
	LandsideStretch* pStretch = pTaxiQueue->getStrech();
	int maxNum = pStretch->getLaneNum();

	GetTreeCtrl().SetDisplayType( 2 );
	GetTreeCtrl().SetSpinRange(1,maxNum);
	GetTreeCtrl().SetDisplayNum(pTaxiQueue->GetLaneFrom());	
	GetTreeCtrl().SpinEditLabel( hItem );
}

void LandsideTaxiQueuePropDlgImpl::OnEditLaneTo( HTREEITEM hItem )
{
	LandsideTaxiQueue* pTaxiQueue = (LandsideTaxiQueue*)m_pObject;
	LandsideStretch* pStretch = pTaxiQueue->getStrech();
	int maxNum = pStretch->getLaneNum();

	GetTreeCtrl().SetDisplayType( 2 );
	GetTreeCtrl().SetSpinRange(1,maxNum);
	GetTreeCtrl().SetDisplayNum(pTaxiQueue->GetLaneTo());	
	GetTreeCtrl().SpinEditLabel( hItem );
}

void LandsideTaxiQueuePropDlgImpl::OnEditDegree( HTREEITEM hItem )
{
	LandsideTaxiQueue* pTaxiQueue = (LandsideTaxiQueue*)m_pObject;
	LandsideStretch* pStretch = pTaxiQueue->getStrech();


	GetTreeCtrl().SetDisplayType( 2 );
	GetTreeCtrl().SetSpinRange(1,360);
	GetTreeCtrl().SetDisplayNum(static_cast<int>(pTaxiQueue->GetDegree()));	
	GetTreeCtrl().SpinEditLabel( hItem );
}
