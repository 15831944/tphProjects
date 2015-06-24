#include "StdAfx.h"
#include "resource.h"
#include ".\landsidestretchsegmentpropdlgimpl.h"
#include "Landside/InputLandside.h"
#include "common/WinMsg.h"
#include "landside\LandsideLayoutObject.h"
#include "ARCLayoutObjectDlg.h"
#include <common/Path2008.h>
#include "Landside/LandsideStretchSegment.h"
#include <commondata/QueryData.h>
#include "landside/LandsideQueryData.h"
#include "Landside/LandsideStretch.h"

LandsideStretchSegmentPropDlgImpl::LandsideStretchSegmentPropDlgImpl(LandsideFacilityLayoutObject* pObj,CARCLayoutObjectDlg* pDlg)
:ILayoutObjectPropDlgImpl(pObj,pDlg)
{
}

CString LandsideStretchSegmentPropDlgImpl::getTitle()
{
	/*if(m_pObject->getID()<0)
		return "New Landside Stretch Segment";
	else
		return "Edit Landside Stretch Segment";*/
	return _T("Landside Stretch Segment Specification");
}

void LandsideStretchSegmentPropDlgImpl::LoadTree()
{
	const LandsideStretchSegment* pStretchSeg = (LandsideStretchSegment*)m_pObject;
	if(!pStretchSeg)
		return;

	CAirsideObjectTreeCtrl& m_treeProp = GetTreeCtrl();
	ARCUnit_Length  curLengthUnit = GetCurLengthUnit();
	m_treeProp.DeleteAllItems();
	m_hRClickItem = NULL;
	m_treeProp.SetRedraw(0);	

	//link stretch
	{
		CString sStretch;
		if(pStretchSeg->getStrech())
			sStretch = pStretchSeg->getStrech()->getName().GetIDString();
		m_hLinkStretch = m_treeProp.InsertItem("Link Stretch");
		TreeCtrlItemInDlg stretchItem(m_treeProp,m_hLinkStretch);
		stretchItem.SetStringText("Link Stretch",sStretch);
	}
	//link stretch length
	{
		CString sLength = "Link Stretch Length";
		m_hLength = m_treeProp.InsertItem(sLength);
		TreeCtrlItemInDlg lengthItem(m_treeProp,m_hLength);
		lengthItem.SetLengthValueText(sLength,pStretchSeg->GetLength(),curLengthUnit);
	}
	//land form
	{
		CString sLaneFrom = "Lane From";
		m_hLaneFrom = m_treeProp.InsertItem(sLaneFrom);
		TreeCtrlItemInDlg laneFromItem(m_treeProp,m_hLaneFrom);
		laneFromItem.SetIntValueText(sLaneFrom,pStretchSeg->GetLaneFrom());
	}
	//lane to
	{
		CString sLaneTo = "Lane To";
		m_hLaneTo = m_treeProp.InsertItem(sLaneTo);
		TreeCtrlItemInDlg laneToItem(m_treeProp,m_hLaneTo);
		laneToItem.SetIntValueText(sLaneTo,pStretchSeg->GetLaneTo());
	}


	m_treeProp.SetRedraw(1);
}

FallbackReason LandsideStretchSegmentPropDlgImpl::GetFallBackReason()
{
	if(m_hRClickItem == m_hLinkStretch)
		return PICK_STRETCH_POS;

	return PICK_MANYPOINTS;
}

void LandsideStretchSegmentPropDlgImpl::OnContextMenu( CMenu& menuPopup, CMenu *&pMenu )
{
	if(m_hRClickItem == m_hLinkStretch)
	{		
		pMenu = menuPopup.GetSubMenu(73);	
	}
}

void LandsideStretchSegmentPropDlgImpl::OnDoubleClickPropTree( HTREEITEM hTreeItem )
{
	m_hRClickItem = hTreeItem;	
	if(hTreeItem == m_hLength)
	{
		OnEditLength(hTreeItem);
	}
	else if(hTreeItem == m_hLaneFrom)
	{
		OnEditLaneFrom(hTreeItem);
	}
	else if(hTreeItem == m_hLaneTo)
	{
		OnEditLaneTo(hTreeItem);
	}
}

void LandsideStretchSegmentPropDlgImpl::DoFallBackFinished( WPARAM wParam, LPARAM lPara )
{
	LandsideStretchSegment* pStretchSeg = (LandsideStretchSegment*)m_pObject;
	if (pStretchSeg == NULL)
		return;
	

	if(m_hRClickItem == m_hLinkStretch)
	{
		QueryData& qdata = *((QueryData*)wParam);
		LandsideStretch*pStretch = NULL;
		double dPos = 0;
		if( qdata.GetData(KeyAltObjectPointer,(int&)pStretch) 
			&&	qdata.GetData(KeyPosInStretch,dPos)  
			&& pStretch )
		{
			pStretchSeg->SetStretch(pStretch);
			pStretchSeg->SetPosInStretch(dPos);
			LoadTree();
			Update3D();
		}

	}
}

BOOL LandsideStretchSegmentPropDlgImpl::OnDefWindowProc( UINT message, WPARAM wParam, LPARAM lParam )
{
	if(!m_hRClickItem)
		return TRUE;

	LandsideStretchSegment* pStretchSeg = (LandsideStretchSegment*)m_pObject;
	if (pStretchSeg == NULL)
		return TRUE;

	if( message == WM_INPLACE_SPIN )
	{
		if(m_hLength == m_hRClickItem)
		{
			LPSPINTEXT pst = (LPSPINTEXT) lParam;
			double dValue = UnConvertLength(pst->iPercent);
			pStretchSeg->SetLength(dValue);
		}
		if(m_hLaneFrom == m_hRClickItem)
		{
			LPSPINTEXT pst = (LPSPINTEXT) lParam;
			int nLaneIdx = pst->iPercent;
			pStretchSeg->SetLaneFrom(nLaneIdx);
		}
		if(m_hLaneTo == m_hRClickItem)
		{
			LPSPINTEXT pst = (LPSPINTEXT) lParam;
			int nLaneIdx = pst->iPercent;
			pStretchSeg->SetLaneTo(nLaneIdx);
		}

		Update3D();
		LoadTree();	
	}
	return TRUE;
}


bool LandsideStretchSegmentPropDlgImpl::OnOK( CString& errorMsg )
{
	if(LandsideStretchSegment* pStretchSeg = getStretchSegment())
	{
		if( !pStretchSeg->getStrech() )
		{
			errorMsg = _T("define curbside on a stretch");
			return false;
		}
		return __super::OnOK(errorMsg);
	}	
	return false;
}

LandsideStretchSegment* LandsideStretchSegmentPropDlgImpl::getStretchSegment()
{
	if(m_pObject->GetType() == ALT_LSTRETCHSEGMENT)
	{
		return (LandsideStretchSegment*)m_pObject;
	}
	return NULL;
}

void LandsideStretchSegmentPropDlgImpl::OnEditLength( HTREEITEM hItem )
{
	LandsideStretchSegment* pStretchSeg = (LandsideStretchSegment*)m_pObject;

	double dUnitNum = ConvertLength(pStretchSeg->GetLength());	
	double dMaxLen = 10000;
	if(pStretchSeg->getStrech())
	{
		LandsideStretch* pStretch = (LandsideStretch*)pStretchSeg->getStrech();
		dMaxLen = ConvertLength(pStretch->getControlPath().getBuildPath().GetTotalLength());
	}

	GetTreeCtrl().SetDisplayType( 2 );
	GetTreeCtrl().SetSpinRange(0,10000);
	GetTreeCtrl().SetDisplayNum(static_cast<int>(dUnitNum+0.5));	
	GetTreeCtrl().SpinEditLabel( hItem );
}

void LandsideStretchSegmentPropDlgImpl::OnEditLaneFrom( HTREEITEM hItem )
{
	LandsideStretchSegment* pStretchSeg = (LandsideStretchSegment*)m_pObject;
	int nLaneNum = 0;
	if(LandsideStretch* pStretch = pStretchSeg->getStrech())
		nLaneNum = pStretch->getLaneNum();	
	else 
		return;

	GetTreeCtrl().SetDisplayType( 2 );
	GetTreeCtrl().SetSpinRange(1,nLaneNum);
	GetTreeCtrl().SetDisplayNum(pStretchSeg->GetLaneFrom());	
	GetTreeCtrl().SpinEditLabel( hItem );
}

void LandsideStretchSegmentPropDlgImpl::OnEditLaneTo( HTREEITEM hItem )
{
	LandsideStretchSegment* pStretchSeg = (LandsideStretchSegment*)m_pObject;
	int nLaneNum = 0;
	if(LandsideStretch* pStretch = pStretchSeg->getStrech())
		nLaneNum = pStretch->getLaneNum();	
	else 
		return;


	GetTreeCtrl().SetDisplayType( 2 );
	GetTreeCtrl().SetSpinRange(1,nLaneNum);
	GetTreeCtrl().SetDisplayNum(pStretchSeg->GetLaneTo());	
	GetTreeCtrl().SpinEditLabel( hItem );
}