#include "StdAfx.h"
#include "resource.h"
#include ".\layoutobjectpropdlgimpl.h"
#include "common/WinMsg.h"
#include "CommonData/QueryData.h"

#include "landside\LandsideLayoutObject.h"
#include "Landside/InputLandside.h"
#include "LandsideParkingLotPropDlgImpl.h"
#include "landside/LandsideStretch.h"
#include "ARCLayoutObjectDlg.h"
#include "LandsideCurbSidePropDlgImpl.h"
#include "common/ViewMsg.h"
#include "./termplandoc.h"
#include "LandsideStretchPropDlgImpl.h"
#include "LandsideCrosswalkPropDlgImpl.h"
#include "LandsideWalkwayPropDlgImpl.h"
#include "LandsideBusStationPropDlgImpl.h"
#include "LandsideIntersectionPropDlgImpl.h"
#include "LandsideExternalNodePropDlgImpl.h"
#include "LandsideRoundaboutPropDlgImpl.h"
#include "LandsideStretchSegmentPropDlgImpl.h"
#include "LandsideControlDevicePropDlgImpl.h"
#include "Common/IEnvModeEditContext.h"
#include "ARCportLayoutEditor.h"
#include "LandsideTaxiQueuePropDlgImpl.h"
#include "LandsideTaxiPoolProDlgImpl.h"
#include "LandsideDecisionPointPropDlgImpl.h"

ILayoutObjectPropDlgImpl* ILayoutObjectPropDlgImpl::Create( LandsideFacilityLayoutObject* pObj, CARCLayoutObjectDlg* pDlg,QueryData* pData )
{
	switch(pObj->GetType())
	{
	case ALT_LSTRETCH:
		return new LandsideStretchPropDlgImpl(pObj,pDlg);
	case ALT_LINTERSECTION:
		return new LandsideIntersectionPropDlgImpl(pObj,pDlg);
	case ALT_LPARKINGLOT:
		return new LandParkingLotPropDlgImpl(pObj,pDlg,pData);
	case ALT_LCURBSIDE:
		return new LandsideCurbSidePropDlgImpl(pObj,pDlg);
	case ALT_LEXT_NODE:
		return new LandsideExternalNodePropDlgImpl(pObj,pDlg);
	case ALT_LCROSSWALK:
		return new LandsideCrosswalkPropDlgImpl(pObj, pDlg);
	case ALT_LWALKWAY:
		return new LandsideWalkwayPropDlgImpl(pObj, pDlg);
	case ALT_LBUSSTATION:
		return new LandsideBusStationPropDlgImpl(pObj, pDlg);
	case ALT_LROUNDABOUT:
		return new LandsideRoundaboutPropDlgImpl(pObj,pDlg);
	case ALT_LSTRETCHSEGMENT:
		return new LandsideStretchSegmentPropDlgImpl(pObj,pDlg);
	case ALT_LTRAFFICLIGHT:
	case ALT_LYIELDSIGN:
	case ALT_LSTOPSIGN:
	case ALT_LTOLLGATE:
	case ALT_LSPEEDSIGN:
		return new LandsideControlDevicePropDlgImpl(pObj,pDlg);
	case ALT_LTAXIQUEUE:
		return new LandsideTaxiQueuePropDlgImpl(pObj,pDlg);
	case ALT_LTAXIPOOL:
		return new LandsideTaxiPoolProDlgImpl(pObj,pDlg);
	case ALT_LDECISIONPOINT:
		return new LandsideDecisionPointPropDlgImpl(pObj,pDlg);
	}

	return NULL;
}

bool ILayoutObjectPropDlgImpl::IsObjectLocked()
{
	return m_pObject->GetLocked();
}

void ILayoutObjectPropDlgImpl::SetObjectLock( bool b )
{
	m_pObject->SetLocked(b);
}



ILayoutObjectPropDlgImpl::ILayoutObjectPropDlgImpl( LandsideFacilityLayoutObject* pObj,CARCLayoutObjectDlg* pDlg )
{
	m_pObject = pObj;
	m_pParentDlg = pDlg;
	m_hRClickItem = NULL;	
	m_bPropModified = false;
	m_bNameModified = false;	
	
	//get object list Name
	std::vector<LandsideFacilityLayoutObject*> m_list;
	m_pParentDlg->getInputLandside()->getObjectList().GetObjectList(m_list,m_pObject->GetType());
	for ( int i = 0; i<(int)m_list.size(); i++)
	{
		LandsideFacilityLayoutObject* pObj = m_list[i];
		m_lstExistObjectName.Add(pObj->getName());
	}
}

void ILayoutObjectPropDlgImpl::OnCancel()
{
	if(m_pObject->getID()<0)
	{
		return;
	}
	else
	{
		InputLandside* pInput  = m_pParentDlg->getInputLandside();
		m_pObject->ReadObject(m_pObject->getID(), pInput);
	}
}



bool ILayoutObjectPropDlgImpl::OnOK( CString& errorMsg )
{
	m_pObject->SaveObject(m_pParentDlg->GetProjectID());
	return true;
}



ARCUnit_Length ILayoutObjectPropDlgImpl::GetCurLengthUnit()
{
	return m_pParentDlg->GetCurSelLengthUnit();
}

CAirsideObjectTreeCtrl& ILayoutObjectPropDlgImpl::GetTreeCtrl()
{
	return m_pParentDlg->GetTreeCtrl();
}

bool ILayoutObjectPropDlgImpl::SetObjectName( const ALTObjectID& id )
{
	
	if( !m_pParentDlg->getInputLandside()->getObjectList().IsNameValid(id,m_pObject) )
		return false;

	if(m_pObject->getName()==id)
		return true;
	
	m_pObject->getName() = id;
	m_bNameModified = true;
	return true;
}

ALTObjectID& ILayoutObjectPropDlgImpl::GetObjectName()
{
	return m_pObject->getName();
}

double ILayoutObjectPropDlgImpl::ConvertLength( double cm )
{
	ARCUnit_Length  curLengthUnit = GetCurLengthUnit();
	return CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,curLengthUnit,cm);
}

double ILayoutObjectPropDlgImpl::UnConvertLength( double dlgunit )
{
	ARCUnit_Length  curLengthUnit = GetCurLengthUnit();
	return CARCLengthUnit::ConvertLength(curLengthUnit,DEFAULT_DATABASE_LENGTH_UNIT,dlgunit);
}

void ILayoutObjectPropDlgImpl::GetFallBackAsPath( WPARAM wParam, LPARAM lParam , CPath2008& path )
{
	std::vector<ARCVector3>* pData = reinterpret_cast< std::vector<ARCVector3>* >(wParam);
	size_t nSize = pData->size();

	path.init(nSize);	
	for (size_t i =0; i < nSize; ++i)
	{
		ARCVector3 v3 = pData->at(i);
		path[i].setPoint(v3[VX] ,v3[VY],0.0);
	}	
}

void ILayoutObjectPropDlgImpl::Update3D()
{
	if(!m_pParentDlg)
		return;

	if(CTermPlanDoc* pDoc =  m_pParentDlg->GetDocument())
	{
		pDoc->UpdateAllViews(NULL,VM_UPDATE_ALTOBJECT3D,(CObject*)m_pObject);
	}
}

FallbackReason ILayoutObjectPropDlgImpl::GetFallBackReason()
{
	return PICK_END_INDEX;
}

void ILayoutObjectPropDlgImpl::SetRClickItem( HTREEITEM hItem )
{
	m_hRClickItem = hItem;
}

void ILayoutObjectPropDlgImpl::CreateChangeVerticalProfileMenu(CMenu *pMenu)
{
	if (NeedChangeVerticalProfile())
	{
		CString strMenu;
		strMenu.LoadString(ID_MENU_CHPROFILE);
		pMenu->AppendMenu(MF_POPUP,ID_MENU_CHPROFILE,strMenu);
	}
}

ILandsideEditContext* ILayoutObjectPropDlgImpl::GetLandsideEditContext() const
{
	if(m_pParentDlg){
		if( CTermPlanDoc* pDoc = m_pParentDlg->GetDocument() ){
			IEnvModeEditContext* pCtx  = pDoc->GetLayoutEditor()->GetContext(EnvMode_LandSide);
			if(pCtx){
				return pCtx->toLandside();
			}
		}
	}
	return NULL;
}

ITerminalEditContext* ILayoutObjectPropDlgImpl::GetTerminalEditContext() const
{
	if(m_pParentDlg){
		if( CTermPlanDoc* pDoc = m_pParentDlg->GetDocument() ){
			IEnvModeEditContext* pCtx  = pDoc->GetLayoutEditor()->GetContext(EnvMode_Terminal);
			if(pCtx){
				return pCtx->toTerminal();
			}
		}
	}
	return NULL;
}

CButton* ILayoutObjectPropDlgImpl::getCheckBoxCellPhone()
{
	return m_pParentDlg->GetCheckBoxCellPhone();
}

ARCUnit_Velocity ILayoutObjectPropDlgImpl::GetCurSpeedUnit()
{
	return m_pParentDlg->GetCurSelVelocityUnit();
}

double ILayoutObjectPropDlgImpl::ConvertSpeed( double dbunit )
{
	ARCUnit_Velocity  curLengthUnit = GetCurSpeedUnit();
	return CARCVelocityUnit::ConvertVelocity(DEFAULT_DATABASE_VELOCITY_UNIT,curLengthUnit,dbunit);
}

double ILayoutObjectPropDlgImpl::UnConvertSpeed( double dspunit )
{
	ARCUnit_Velocity  curLengthUnit = GetCurSpeedUnit();
	return CARCVelocityUnit::ConvertVelocity(curLengthUnit, DEFAULT_DATABASE_VELOCITY_UNIT,dspunit);
}

void ILayoutObjectPropDlgImpl::InitUnitPiece( CUnitPiece* unit )
{
	unit->SetUnitInUse(ARCUnit_Length_InUse);
}

void ILayoutObjectPropDlgImpl::LoadTreeSubItemCtrlPoints( HTREEITEM preItem,const ControlPath& path )
{
	CAirsideObjectTreeCtrl& m_treeProp = m_pParentDlg->GetTreeCtrl();
	ARCUnit_Length  curLengthUnit = m_pParentDlg->GetCurSelLengthUnit();

	//
	AirsideObjectTreeCtrlItemDataEx aoidDataEx;
	ItemStringSectionColor isscStringColor;
	CString strTemp = _T("");
	HTREEITEM hItemTemp = 0;

	int iPathCount = path.getCount();
	for(int i = 0;i < iPathCount; i++)
	{
		CString csPoint;
		double FloorAlt = path.getPoint(i).m_pt.getZ();//floors.getVisibleFloorAltitude() ;

		aoidDataEx.lSize = sizeof(aoidDataEx);
		aoidDataEx.dwptrItemData = 0;
		aoidDataEx.vrItemStringSectionColorShow.clear();
		isscStringColor.colorSection = RGB(0,0,255);
		strTemp.Format(_T("%.2f"),CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,curLengthUnit,path.getPoint(i).m_pt.getX()));		
		isscStringColor.strSection = strTemp;
		aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);

		strTemp.Format(_T("%.2f"),CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,curLengthUnit,path.getPoint(i).m_pt.getY()));		
		isscStringColor.strSection = strTemp;
		aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);

		strTemp.Format(_T("%.2f"),FloorAlt);		
		isscStringColor.strSection = strTemp;
		aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);

		csPoint.Format("x=%.2f; y =%.2f ; floor z = %.2f",CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,curLengthUnit, path.getPoint(i).m_pt.getX() ), \
			CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,curLengthUnit, path.getPoint(i).m_pt.getY() ), FloorAlt);
		hItemTemp = m_treeProp.InsertItem( csPoint, preItem  );
		m_treeProp.SetItemDataEx(hItemTemp,aoidDataEx);
	}
}