#include "StdAfx.h"
#include ".\arcportlayouteditor.h"
#include "TerminalEditContext.h"
#include "LandsideEditContext.h"
#include "AirsideEditContext.h"
#include "onboard\AircraftLayoutEditor.h"
#include "TermPlanDoc.h"
#include "Resource.h"
#include "ARCLayoutObjectDlg.h"
#include "landside/LandsideEditCommand.h"
#include "Landside/LandsideLayoutObject.h"
#include <common/ViewMsg.h>
#include <common/ARCStringConvert.h>
#include "landside/LandsideQueryData.h"
#include "Landside/LandsideCurbSide.h"
#include "Landside/LandsideIntersectionNode.h"
#include "Landside/landsideparkinglot.h"
#include "Landside/InputLandside.h"
#include "Landside/LandsideWalkway.h"
#include "Landside/LandsideBusStation.h"
#include "landside/LandsideTaxiPool.h"
#include "Landside/LandsideDeleteOperationCmd.h"

#include <Inputs/PipeDataSet.h>
#include <Inputs/WallShapeList.h>
#include <Inputs/Structurelist.h>

#include "InputAirside/AirWayPoint.h"
#include "InputAirside/AirHold.h"
#include "InputAirside/AirRoute.h"
#include "InputAirside/AirSector.h"
#include "Placement.h"
#include "inputs/Structure.h"
#include "InputAirside/Structure.h"
#include "InputAirside/Surface.h"
#include "InputAirside/contour.h"
#include "AirsideLevelList.h"
#include "InputAirside/ALTAirportLayout.h"
#include "TermPlan.h"
#include "Inputs/TerminalProcessorTypeSet.h"

CARCportLayoutEditor::CARCportLayoutEditor( CTermPlanDoc* pDoc )
{
	m_pDoc = pDoc;

	m_terminalEdit = new CTerminalEditContext();
	m_terminalEdit->SetDocument(pDoc);

	m_landsideEdit = new CLandsideEditContext();
	m_landsideEdit->SetDocument(pDoc);

	m_airsideEdit = new CAirsideEditContext();
	m_airsideEdit->SetDocument(pDoc);

	mpAircraftLayoutEditor = new CAircraftLayoutEditor(pDoc);
	m_LayoutObjectDlg = NULL;
}

class EditorLayoutObjectCallBack : public CDlgCallBack
{
public:
	EditorLayoutObjectCallBack(ILandsideEditObjectCmd* pCmd, CARCportLayoutEditor* pEditor)
	:m_pCmd(pCmd),m_pEditor(pEditor){}

	virtual void OnOk()
	{	
		m_pCmd->EndOp();
		m_pEditor->GetLandsideContext()->AddCommand(m_pCmd);
		m_pEditor->GetDocument()->UpdateAllViews(NULL,VM_UPDATE_COMMAND,(CObject*)m_pCmd);
	}
	virtual void OnCancel()
	{
		m_pCmd->CancelOp();
		m_pEditor->GetDocument()->UpdateAllViews(NULL,VM_UPDATE_COMMAND,(CObject*)m_pCmd);		
		delete m_pCmd;
		m_pCmd = NULL;
	}
protected:
	ILandsideEditObjectCmd* m_pCmd;
	CARCportLayoutEditor* m_pEditor;
};



CARCportLayoutEditor::~CARCportLayoutEditor( void )
{

	delete m_landsideEdit;
	delete m_terminalEdit;
	delete m_airsideEdit;
	delete mpAircraftLayoutEditor;
	if (m_LayoutObjectDlg!= NULL)
	{
		m_LayoutObjectDlg->DestroyWindow();
		delete m_LayoutObjectDlg;
		m_LayoutObjectDlg = NULL;
	}
	
	cpputil::deletePtrMap(m_AirsideLevelsMap);
}

IEnvModeEditContext* CARCportLayoutEditor::GetContext( EnvironmentMode mode )const
{
	switch(mode)
	{
	case EnvMode_AirSide:
		return m_airsideEdit;
	case EnvMode_Terminal:
		return m_terminalEdit;
	case EnvMode_OnBoard:
		return mpAircraftLayoutEditor;
	case EnvMode_LandSide:
		return m_landsideEdit;
	default:;
		
	};
	return m_terminalEdit;
}



IEnvModeEditContext* CARCportLayoutEditor::GetCurContext()const
{
	return GetContext( m_pDoc->m_systemMode);
}



CNewMenu* CARCportLayoutEditor::GetContextMenu(CNewMenu& menu)
{
	return GetCurContext()->GetContexMenu(menu);
}



void CARCportLayoutEditor::OnCtxProcproperties(LandsideFacilityLayoutObject* pObj,CWnd* pParent,QueryData* pData)
{
	InputLandside* pInput = m_landsideEdit->getInput();
	LandsideModifyLayoutObjectCmd* pModCmd = new LandsideModifyLayoutObjectCmd(pObj,pInput);
	pModCmd->BeginOp();
	if (m_LayoutObjectDlg)
	{
		m_LayoutObjectDlg->DestroyWindow();
		delete m_LayoutObjectDlg;
		m_LayoutObjectDlg = 0;
	}
	m_LayoutObjectDlg = new CARCLayoutObjectDlg( pObj,new EditorLayoutObjectCallBack(pModCmd,this),pParent,pData );
	m_LayoutObjectDlg->Create(CARCLayoutObjectDlg::IDD);
	m_LayoutObjectDlg->CenterWindow();
	m_LayoutObjectDlg->ShowWindow(SW_SHOWNORMAL);
}

void CARCportLayoutEditor::OnCtxProcproperties( CWnd* pParent )
{
	if(LandsideFacilityLayoutObject* pObj = GetRClickObject())
	{
		OnCtxProcproperties(pObj,pParent);
	}
}


void CARCportLayoutEditor::OnCtxDelLandsideProc(LandsideFacilityLayoutObject* pObj)
{
	CTermPlanDoc* pDoc = GetDocument();	
	InputLandside* pInput = pDoc->getARCport()->m_pInputLandside;
	LandsideDelLayoutObjectCmd *pDelCmd = new LandsideDelLayoutObjectCmd(pObj,pInput);
	LandsideDeleteOperationCmd deleteOperationCmd(pObj,pDoc->GetInputLandside());
	
	CString strObject;
	if(pObj)
	{
		strObject.Format(_T("Are you sure to delete landside layout object: %s ?"),pObj->getName().GetIDString());
	}

	if (AfxMessageBox(strObject, MB_YESNO|MB_ICONQUESTION) == IDYES)
	{
		if (deleteOperationCmd.DeleteOperationValid())
		{
			MessageBox(NULL,deleteOperationCmd.GetErrorString(),_T("Error"),MB_OK);
			return;
		}

		pDelCmd->Do();
		GetCurContext()->AddCommand(pDelCmd);
		pDoc->UpdateAllViews(NULL,VM_UPDATE_COMMAND,(CObject*)pDelCmd);
	}
	
	
}

void CARCportLayoutEditor::OnCtxDelLandsideProc()
{
	if(LandsideFacilityLayoutObject* pObj = GetRClickObject())
	{
		typedef std::vector<LandsideFacilityLayoutObject*>	RelatedObjectList;
		typedef RelatedObjectList::iterator RelatedObject_Iter;

		RelatedObjectList relatedObjects;

		CTermPlanDoc* pDoc = GetDocument();	
		InputLandside* pInput = pDoc->getARCport()->m_pInputLandside;
		if (pObj->GetType() == ALT_LSTRETCH)
		{
			relatedObjects.clear();
			CTermPlanDoc* pDoc = GetDocument();	
			InputLandside* pInput = pDoc->getARCport()->m_pInputLandside;
			LandsideFacilityLayoutObjectList& list = pInput->getObjectList();
			list.GetObjectList(relatedObjects, ALT_LCROSSWALK);
			for (RelatedObject_Iter it = relatedObjects.begin(); it != relatedObjects.end(); ++it)
			{
				if((*it)->getRelatedObject() == pObj)
					OnCtxDelLandsideProc(*it);
			}

			relatedObjects.clear();
			pInput->getObjectList().GetObjectList(relatedObjects, ALT_LCURBSIDE);
			for (RelatedObject_Iter it = relatedObjects.begin(); it != relatedObjects.end(); ++it)
			{
				if((*it)->getRelatedObject() == pObj)
					OnCtxDelLandsideProc(*it);
			}

			relatedObjects.clear();
			pInput->getObjectList().GetObjectList(relatedObjects, ALT_LSTRETCHSEGMENT);
			for (RelatedObject_Iter it = relatedObjects.begin(); it != relatedObjects.end(); ++it)
			{
				if((*it)->getRelatedObject() == pObj)
					OnCtxDelLandsideProc(*it);
			}

			relatedObjects.clear();
			pInput->getObjectList().GetObjectList(relatedObjects, ALT_LBUSSTATION);
			for (RelatedObject_Iter it = relatedObjects.begin(); it != relatedObjects.end(); ++it)
			{
				if((*it)->getRelatedObject() == pObj)
					OnCtxDelLandsideProc(*it);
			}
		}
		
		OnCtxDelLandsideProc(pObj);
	}
}



void CARCportLayoutEditor::OnNewLandsideProc( ALTOBJECT_TYPE type ,CWnd* pParent)
{
	LandsideLayoutObject::RefPtr pObj = LandsideLayoutObject::NewObject(type);
	if(!pObj)return;

	LandsideFacilityLayoutObject* pfcObject = pObj->toFacilityObject();
	if(!pfcObject)return;

	
	LandsideAddLayoutObjectCmd* pAddCmd = new LandsideAddLayoutObjectCmd(pObj.get(),m_landsideEdit->getInput());
	pAddCmd->BeginOp();

	if (m_LayoutObjectDlg)
	{
		m_LayoutObjectDlg->DestroyWindow();
		delete m_LayoutObjectDlg;
		m_LayoutObjectDlg = NULL;
	}
	m_LayoutObjectDlg = new CARCLayoutObjectDlg( pfcObject,new EditorLayoutObjectCallBack(pAddCmd,this),pParent );
	m_LayoutObjectDlg->Create(CARCLayoutObjectDlg::IDD);
	m_LayoutObjectDlg->CenterWindow();
	m_LayoutObjectDlg->ShowWindow(SW_SHOWNORMAL);
}

LandsideFacilityLayoutObject* CARCportLayoutEditor::GetRClickObject()
{
	QueryData& itemdata = GetCurContext()->getRClickItemData();
	emLandsideItemType itemType;
	if(itemdata.GetData(KeyLandsideItemType,(int&)itemType))
	{
		switch(itemType)
		{
		case _LayoutObject:
			{
				LandsideFacilityLayoutObject* pObj = NULL;
				if(itemdata.GetData(KeyAltObjectPointer, (int&)pObj) )
				{
					return pObj;
				}
			}
			break;
		}
	}
	return NULL;
}

void CARCportLayoutEditor::OnCtxDeleteControlPoint()
{	
	QueryData& itemdata = GetCurContext()->getRClickItemData();
	emLandsideItemType itemType;
	LandsideFacilityLayoutObject* pObj = NULL;
	int PtIndex = -1;
	if(itemdata.GetData(KeyAltObjectPointer, (int&)pObj) 
		&& itemdata.GetData(KeyControlPointIndex,PtIndex))
	{
		if(pObj->GetType() == ALT_LSTRETCH)
		{
			InputLandside* pInput = m_landsideEdit->getInput();;
			LandsideModifyLayoutObjectCmd* pModCmd = new LandsideModifyLayoutObjectCmd(pObj,pInput);
			GetCurContext()->StartEditOp(pModCmd); 
			LandsideStretch* pStreth = (LandsideStretch*)pObj;
			if( pStreth->getControlPath().removePoint(PtIndex) )
			{
				GetCurContext()->DoneCurEditOp(); 
				GetDocument()->UpdateAllViews(NULL,VM_UPDATE_COMMAND,(CObject*)pModCmd);
			}
			else
			{
				GetDocument()->UpdateAllViews(NULL,VM_UPDATE_COMMAND,(CObject*)pModCmd);
				//delete pModCmd;

			}
		}
		if (pObj->GetType() == ALT_LTAXIPOOL)
		{
			InputLandside* pInput = m_landsideEdit->getInput();
			LandsideModifyLayoutObjectCmd* pModCmd = new LandsideModifyLayoutObjectCmd(pObj,pInput);
			GetCurContext()->StartEditOp(pModCmd); 
			LandsideTaxiPool* pTaxiPool = (LandsideTaxiPool*)pObj;
			if(pTaxiPool->getControlPath().removePoint(PtIndex))
			{
				GetCurContext()->DoneCurEditOp(); 
				GetDocument()->UpdateAllViews(NULL,VM_UPDATE_COMMAND,(CObject*)pModCmd);
			}
			else
			{
				GetDocument()->UpdateAllViews(NULL,VM_UPDATE_COMMAND,(CObject*)pModCmd);
				//delete pModCmd;
			}
		}
		if (pObj->GetType() == ALT_LCURBSIDE)
		{
			InputLandside* pInput = m_landsideEdit->getInput();;
			LandsideModifyLayoutObjectCmd* pModCmd = new LandsideModifyLayoutObjectCmd(pObj,pInput);
			GetCurContext()->StartEditOp(pModCmd); 
			LandsideCurbSide* pCurb = (LandsideCurbSide*)pObj;
			if( pCurb->RemovePickAreaVertex(PtIndex))
			{
				GetCurContext()->DoneCurEditOp(); 
				GetDocument()->UpdateAllViews(NULL,VM_UPDATE_COMMAND,(CObject*)pModCmd);
			}
			else
			{
				GetDocument()->UpdateAllViews(NULL,VM_UPDATE_COMMAND,(CObject*)pModCmd);
				//delete pModCmd;
			}
		}
		if (pObj->GetType() == ALT_LPARKINGLOT)
		{
			if (itemdata.GetData(KeyLandsideItemType,(int&)itemType))
			{
				if (itemType == _ParkingLotAreaPoint)
				{
					int areaIdx = -1;
					int levelidx = -1;
					if (itemdata.GetData(KeyControlPathIndex, areaIdx) && itemdata.GetData(KeyLevelIndex,levelidx) )
					{
						LandsideParkingLot* pLot = (LandsideParkingLot*)pObj;
						if(LandsideParkingLotLevel* pLevel = pLot->GetLevel(levelidx))
						{
							InputLandside* pInput = m_landsideEdit->getInput();
							LandsideModifyLayoutObjectCmd* pModCmd = new LandsideModifyLayoutObjectCmd(pObj,pInput);
							GetCurContext()->StartEditOp(pModCmd);

							if(pLevel->m_areas.RemovePoint(areaIdx, PtIndex))
							{
								GetCurContext()->DoneCurEditOp(); 
								GetDocument()->UpdateAllViews(NULL,VM_UPDATE_COMMAND,(CObject*)pModCmd);
							}
							else
							{
								GetDocument()->UpdateAllViews(NULL,VM_UPDATE_COMMAND,(CObject*)pModCmd);
								//delete pModCmd;
							}
						}				
					
					}
				}
				if (itemType == _ParkingLotSpacePoint)
				{
					int pathIdx = -1;int levelidx = -1;
					if (itemdata.GetData(KeyControlPathIndex, pathIdx) && itemdata.GetData(KeyLevelIndex,levelidx) )
					{
						InputLandside* pInput = m_landsideEdit->getInput();
						LandsideParkingLot* pLot = (LandsideParkingLot*)pObj;
						if(LandsideParkingLotLevel* pLevel = pLot->GetLevel(levelidx))
						{
							LandsideModifyLayoutObjectCmd* pModCmd = new LandsideModifyLayoutObjectCmd(pObj,pInput);
							GetCurContext()->StartEditOp(pModCmd); 

							if(pLevel->m_parkspaces.m_spaces[pathIdx].DeleteControlPoint(PtIndex))
							{
								GetCurContext()->DoneCurEditOp(); 
								GetDocument()->UpdateAllViews(NULL,VM_UPDATE_COMMAND,(CObject*)pModCmd);
							}
							else
							{
								GetDocument()->UpdateAllViews(NULL,VM_UPDATE_COMMAND,(CObject*)pModCmd);
								//delete pModCmd;
							}
						}
						
					}
				}
				if (itemType == _ParkingLotDrivePipePoint)
				{
					int areaIdx = -1;int levelidx = -1;
					if (itemdata.GetData(KeyControlPathIndex, areaIdx)&& itemdata.GetData(KeyLevelIndex,levelidx))
					{
						InputLandside* pInput = m_landsideEdit->getInput();
						LandsideParkingLot* pLot = (LandsideParkingLot*)pObj;
						if(LandsideParkingLotLevel* pLevel = pLot->GetLevel(levelidx))
						{
							LandsideModifyLayoutObjectCmd* pModCmd = new LandsideModifyLayoutObjectCmd(pObj,pInput);
							GetCurContext()->StartEditOp(pModCmd); 

							if(pLevel->m_dirvepipes.RemovePoint(areaIdx, PtIndex))
							{
								GetCurContext()->DoneCurEditOp(); 
								GetDocument()->UpdateAllViews(NULL,VM_UPDATE_COMMAND,(CObject*)pModCmd);
							}
							else
							{
								GetDocument()->UpdateAllViews(NULL,VM_UPDATE_COMMAND,(CObject*)pModCmd);
								//delete pModCmd;
							}
						}						
					}
				}
			}

		}
		if (pObj->GetType() == ALT_LWALKWAY)
		{
			if(itemdata.GetData(KeyLandsideItemType,(int&)itemType))
			{
				if (itemType == _WalkwayControlPoint)
				{
					InputLandside* pInput = m_landsideEdit->getInput();
					LandsideModifyLayoutObjectCmd* pModCmd = new LandsideModifyLayoutObjectCmd(pObj,pInput);
					GetCurContext()->StartEditOp(pModCmd); 

					LandsideWalkway* pWalkway = (LandsideWalkway*)pObj;
					if(pWalkway->DeleteControlPoint(PtIndex))
					{
						GetCurContext()->DoneCurEditOp(); 
						GetDocument()->UpdateAllViews(NULL,VM_UPDATE_COMMAND,(CObject*)pModCmd);
					}
					else
					{
						GetDocument()->UpdateAllViews(NULL,VM_UPDATE_COMMAND,(CObject*)pModCmd);
						//delete pModCmd;
					}
				}
			}
		}
		if (pObj->GetType() == ALT_LBUSSTATION)
		{
			if(itemdata.GetData(KeyLandsideItemType,(int&)itemType))
			{
				if (itemType == _BusStationWaitingPoint)
				{
					InputLandside* pInput = m_landsideEdit->getInput();
					LandsideModifyLayoutObjectCmd* pModCmd = new LandsideModifyLayoutObjectCmd(pObj,pInput);
					GetCurContext()->StartEditOp(pModCmd); 

					LandsideBusStation* pBusStation = (LandsideBusStation*)pObj;
					if(pBusStation->RemoveWaitingControlPoint(PtIndex))
					{
						GetCurContext()->DoneCurEditOp(); 
						GetDocument()->UpdateAllViews(NULL,VM_UPDATE_COMMAND,(CObject*)pModCmd);
					}
					else
					{
						GetDocument()->UpdateAllViews(NULL,VM_UPDATE_COMMAND,(CObject*)pModCmd);
						//delete pModCmd;
					}
				}
			}
		}
	}
}

void CARCportLayoutEditor::OnCtxBeginMoveControlPointVertical()
{
	QueryData& itemdata = GetCurContext()->getRClickItemData();
	LandsideFacilityLayoutObject* pObj = NULL;
	int PtIndex = -1;
	if(itemdata.GetData(KeyAltObjectPointer, (int&)pObj) 
		&& itemdata.GetData(KeyControlPointIndex,PtIndex))
	{
		if(pObj->GetType() == ALT_LSTRETCH)
		{
			InputLandside* pInput = m_landsideEdit->getInput();;
			LandsideModifyLayoutObjectCmd* pModCmd = new LandsideModifyLayoutObjectCmd(pObj,pInput);			
			GetCurContext()->StartEditOp(pModCmd); 
		}		

	}
}

void CARCportLayoutEditor::OnCtxAddControlPoint()
{
	QueryData& itemdata = GetCurContext()->getRClickItemData();
	LandsideFacilityLayoutObject* pObj = NULL;
	emLandsideItemType itemType;
	CString posStr;
	CString rayOrigin;
	CString rayDirection;
	double dPosIndex = 0;

	if( itemdata.GetData(KeyLandsideItemType,(int&)itemType) )
	{
		if(itemType == _ObjectControlPath )
		{
			if(itemdata.GetData(KeyAltObjectPointer, (int&)pObj) && itemdata.GetData(KeyRClickWorldPos,posStr) 
				&& itemdata.GetData(KeyPosIndexInStretchBuildPath,dPosIndex) )
			{
				if(pObj->GetType() == ALT_LSTRETCH)
				{
					LandsideStretch* pStretch = (LandsideStretch*)pObj;
					ARCVector3 worldpos= ARCStringConvert::parseVector3(posStr);
				
                    InputLandside* pInput = m_landsideEdit->getInput();;
					LandsideModifyLayoutObjectCmd* pModCmd = new LandsideModifyLayoutObjectCmd(pObj,pInput);	
					GetCurContext()->StartEditOp(pModCmd); 
					int ctrPos = pStretch->getControlPath().getCtrlPathSegIndex((int)dPosIndex);
					pStretch->getControlPath().addPoint( CPoint2008(worldpos.x,worldpos.y,worldpos.z),ctrPos+1);			
					GetCurContext()->DoneCurEditOp(); 
					GetDocument()->UpdateAllViews(NULL,VM_UPDATE_COMMAND,(CObject*)pModCmd);
				}
				if(pObj->GetType() == ALT_LTAXIPOOL)
				{
					LandsideTaxiPool* pTaxiPool = (LandsideTaxiPool*)pObj;
					ARCVector3 worldpos= ARCStringConvert::parseVector3(posStr);

					InputLandside* pInput = m_landsideEdit->getInput();;
					LandsideModifyLayoutObjectCmd* pModCmd = new LandsideModifyLayoutObjectCmd(pObj,pInput);	
					GetCurContext()->StartEditOp(pModCmd); 
					int ctrPos = pTaxiPool->getControlPath().getCtrlPathSegIndex((int)dPosIndex);
					pTaxiPool->getControlPath().addPoint( CPoint2008(worldpos.x,worldpos.y,worldpos.z),ctrPos+1);			
					GetCurContext()->DoneCurEditOp(); 
					GetDocument()->UpdateAllViews(NULL,VM_UPDATE_COMMAND,(CObject*)pModCmd);
				}
			}
		}
		if (itemType == _CurbSideControlPath)
		{
			if (itemdata.GetData(KeyAltObjectPointer, (int&)pObj) && itemdata.GetData(KeyRClickWorldPos,posStr) )
			{
				if (pObj->GetType() == ALT_LCURBSIDE)
				{
					LandsideCurbSide* pCurb = (LandsideCurbSide*)pObj;
					ARCVector3 worldpos = ARCStringConvert::parseVector3(posStr);

					InputLandside* pInput = m_landsideEdit->getInput();;
					LandsideModifyLayoutObjectCmd* pModCmd = new LandsideModifyLayoutObjectCmd(pObj,pInput);	
					GetCurContext()->StartEditOp(pModCmd); 
					
					//add the new point to border of pick area(LandsideCurbside::m_PickArea)
					CPoint2008 ptworld = CPoint2008(worldpos.x,worldpos.y,worldpos.z);
					int idx = pCurb->GetTheNewPointIndex(ptworld);
					if (idx != -1)
						pCurb->AddPickAreaVertex(ptworld, idx);

					GetCurContext()->DoneCurEditOp(); 
					GetDocument()->UpdateAllViews(NULL,VM_UPDATE_COMMAND,(CObject*)pModCmd);
				}
			}
		}
		if (itemType == _ParkingLotAreaPath)
		{
			int areaIdx = -1;
			int levelidx = -1;
			if (itemdata.GetData(KeyAltObjectPointer, (int&)pObj)  
				&& itemdata.GetData(KeyRClickWorldPos, posStr)
				&& itemdata.GetData(KeyControlPathIndex, areaIdx)
				&& itemdata.GetData(KeyLevelIndex,levelidx))
			{
				LandsideParkingLot* pLot = (LandsideParkingLot*)pObj;
				if(LandsideParkingLotLevel*plevel =  pLot->GetLevel(levelidx))
				{
					
					ARCVector3 worldpos = ARCStringConvert::parseVector3(posStr);
					InputLandside* pInput = m_landsideEdit->getInput();;
					LandsideModifyLayoutObjectCmd* pModCmd = new LandsideModifyLayoutObjectCmd(pObj,pInput);	
					GetCurContext()->StartEditOp(pModCmd);
					
					CPoint2008 newPoint(worldpos.x,worldpos.y,worldpos.z);
					plevel->m_areas.AddPoint(areaIdx, newPoint);	

					GetCurContext()->DoneCurEditOp(); 
					GetDocument()->UpdateAllViews(NULL,VM_UPDATE_COMMAND,(CObject*)pModCmd);
				}
			}
		}
		if (itemType == _ParkingLotDrivePipePath)
		{
			int areaIdx = -1;
			int levelidx = -1;
			if (itemdata.GetData(KeyAltObjectPointer, (int&)pObj) 
				&& itemdata.GetData(KeyRClickWorldPos, posStr)
				&& itemdata.GetData(KeyControlPathIndex,areaIdx)
				&& itemdata.GetData(KeyLevelIndex,levelidx))
			{
				LandsideParkingLot* pLot = (LandsideParkingLot*)pObj;
				if(LandsideParkingLotLevel*plevel =  pLot->GetLevel(levelidx))
				{				
					
					ARCVector3 worldpos = ARCStringConvert::parseVector3(posStr);
					InputLandside* pInput = m_landsideEdit->getInput();;
					LandsideModifyLayoutObjectCmd* pModCmd = new LandsideModifyLayoutObjectCmd(pObj,pInput);	
					GetCurContext()->StartEditOp(pModCmd); 
					
					CPoint2008 newPoint(worldpos.x,worldpos.y,worldpos.z);
					plevel->m_dirvepipes.AddPoint(areaIdx, newPoint);	

					GetCurContext()->DoneCurEditOp(); 
					GetDocument()->UpdateAllViews(NULL,VM_UPDATE_COMMAND,(CObject*)pModCmd);
				}
			}
		}
		if (itemType == _ParkingLotSpacePath)
		{
			int pathIndex = -1;
			int levelidx  =-1;
			if (itemdata.GetData(KeyAltObjectPointer, (int&)pObj) 
				&& itemdata.GetData(KeyRClickWorldPos, posStr)
				&& itemdata.GetData(KeyControlPathIndex, pathIndex)
				&& itemdata.GetData(KeyLevelIndex,levelidx))
			{
				LandsideParkingLot* pLot = (LandsideParkingLot*)pObj;
				if(LandsideParkingLotLevel* plevel = pLot->GetLevel(levelidx))
				{
					ARCVector3 worldpos = ARCStringConvert::parseVector3(posStr);
					InputLandside* pInput = m_landsideEdit->getInput();;
					LandsideModifyLayoutObjectCmd* pModCmd = new LandsideModifyLayoutObjectCmd(pObj,pInput);	
					GetCurContext()->StartEditOp(pModCmd); 
					
					CPoint2008 newPoint(worldpos.x,worldpos.y,worldpos.z);
					plevel->m_parkspaces.m_spaces[pathIndex].AddControlPoint(newPoint);

					GetCurContext()->DoneCurEditOp(); 
					GetDocument()->UpdateAllViews(NULL,VM_UPDATE_COMMAND,(CObject*)pModCmd);
				}				
			}
		}
		if (itemType == _WalkwayControlPath)
		{
			if (itemdata.GetData(KeyAltObjectPointer, (int&)pObj) && itemdata.GetData(KeyRClickWorldPos, posStr))
			{
				ARCVector3 worldpos = ARCStringConvert::parseVector3(posStr);
				InputLandside* pInput = m_landsideEdit->getInput();;
				LandsideModifyLayoutObjectCmd* pModCmd = new LandsideModifyLayoutObjectCmd(pObj,pInput);	
				GetCurContext()->StartEditOp(pModCmd); 

				LandsideWalkway* pWalkway = (LandsideWalkway*)pObj;
				CPoint2008 newPoint(worldpos.x, worldpos.y, worldpos.z);
				pWalkway->AddControlPoint(newPoint);

				GetCurContext()->DoneCurEditOp(); 
				GetDocument()->UpdateAllViews(NULL,VM_UPDATE_COMMAND,(CObject*)pModCmd);
			}
		}
		if (itemType == _BusStationWaitingPath)
		{
			if (itemdata.GetData(KeyAltObjectPointer, (int&)pObj) && itemdata.GetData(KeyRClickWorldPos, posStr))
			{
				ARCVector3 worldpos = ARCStringConvert::parseVector3(posStr);
				InputLandside* pInput = m_landsideEdit->getInput();;
				LandsideModifyLayoutObjectCmd* pModCmd = new LandsideModifyLayoutObjectCmd(pObj,pInput);	
				GetCurContext()->StartEditOp(pModCmd); 

				LandsideBusStation* pBusStation = (LandsideBusStation*)pObj;
				CPoint2008 newPoint(worldpos.x, worldpos.y, worldpos.z);
				pBusStation->AddWaitingControlPoint(newPoint);

				GetCurContext()->DoneCurEditOp(); 
				GetDocument()->UpdateAllViews(NULL,VM_UPDATE_COMMAND,(CObject*)pModCmd);
			}
		}
	}
}
#include "Landside/InputLandside.h"
void CARCportLayoutEditor::OnCtxCopyLayoutObject()
{
	QueryData& itemdata = GetCurContext()->getRClickItemData();
	LandsideFacilityLayoutObject* pObj = NULL;	
	emLandsideItemType itemType;

	if( itemdata.GetData(KeyLandsideItemType,(int&)itemType) )
	{
		if(itemType == _LayoutObject )
		{
			if(itemdata.GetData(KeyAltObjectPointer, (int&)pObj) )
			{
				InputLandside* pInput = m_landsideEdit->getInput();
				LandsideLayoutObject* pNewObj = LandsideLayoutObject::NewObject(pObj->GetType());
				if(!pNewObj)return;
				LandsideFacilityLayoutObject* newObj = pNewObj->toFacilityObject();			
				if(!newObj)return;

				newObj->setDisplayProp(pObj->getDisplayProp());
				newObj->SetNewObjectFlag(true);
				pObj->SetNewObjectFlag(false);
				if(newObj)
				{
					LandsideCopyLayoutObjectCmd* pCopyCmd = new LandsideCopyLayoutObjectCmd(pObj,newObj,pInput);
					GetCurContext()->StartEditOp(pCopyCmd);
					newObj->CopyPropFrom(pObj);
					newObj->getName() = pInput->getObjectList().GetNextValidID(pObj->getName());
					GetCurContext()->DoneCurEditOp();
					GetDocument()->UpdateAllViews(NULL,VM_UPDATE_COMMAND,(CObject*)pCopyCmd);			
				}			
			}
		}
	}
}

void CARCportLayoutEditor::OnCtxChangeControlPointType()
{
	QueryData& itemdata = GetCurContext()->getRClickItemData();
	LandsideFacilityLayoutObject* pObj = NULL;	
	emLandsideItemType itemType;

	if( itemdata.GetData(KeyLandsideItemType,(int&)itemType) )
	{
		if(itemType == _ObjectControlPoint )
		{
			if(itemdata.GetData(KeyAltObjectPointer, (int&)pObj) )
			{
				if(pObj->GetType() == ALT_LSTRETCH)
				{
					int nIdx = -1;
					itemdata.GetData(KeyControlPointIndex,nIdx);
					LandsideStretch* pStretch = (LandsideStretch*)pObj;

					InputLandside* pInput = m_landsideEdit->getInput();;
					LandsideModifyLayoutObjectCmd* pModCmd = new LandsideModifyLayoutObjectCmd(pObj,pInput);	
					GetCurContext()->StartEditOp(pModCmd); 

					pStretch->getControlPath().flipPointType(nIdx);
					GetCurContext()->DoneCurEditOp(); 
					GetDocument()->UpdateAllViews(NULL,VM_UPDATE_COMMAND,(CObject*)pModCmd);
				}
			}
		}
	}


}

void CARCportLayoutEditor::OnCtxMoveCtrlPointToLevel( int iFloor )
{	
	m_landsideEdit->OnCtxSnapPointToLevel(iFloor);
}
void CARCportLayoutEditor::OnSnapPointToParkLotLevel( int nLevel )
{	
	m_landsideEdit->OnSnapPointToParkLotLevel(nLevel);
}
void CARCportLayoutEditor::OnCopyParkLotItem()
{
	m_landsideEdit->OnCopyParkLotItem();
}
void CARCportLayoutEditor::OnDelParkLotItem()
{
	m_landsideEdit->OnDelParkLotItem();
}

void CARCportLayoutEditor::OnCtxDeleteLayoutItem()
{
	QueryData& itemdata = GetCurContext()->getRClickItemData();
	LandsideFacilityLayoutObject* pObj = NULL;
	int PtIndex = -1;
	emLandsideItemType itemType;
	if(itemdata.GetData(KeyAltObjectPointer, (int&)pObj) 
		&& itemdata.GetData(KeyControlPointIndex,PtIndex))
	{
		if(pObj->GetType() == ALT_LSTRETCH)
		{
			InputLandside* pInput = m_landsideEdit->getInput();;
			LandsideModifyLayoutObjectCmd* pModCmd = new LandsideModifyLayoutObjectCmd(pObj,pInput);
			GetCurContext()->StartEditOp(pModCmd); 
			LandsideStretch* pStreth = (LandsideStretch*)pObj;
			if( pStreth->getControlPath().removePoint(PtIndex) )
			{
				GetCurContext()->DoneCurEditOp(); 
				GetDocument()->UpdateAllViews(NULL,VM_UPDATE_COMMAND,(CObject*)pModCmd);
			}
			else
			{
				GetDocument()->UpdateAllViews(NULL,VM_UPDATE_COMMAND,(CObject*)pModCmd);
				//delete pModCmd;
			}
		}	

	}
	if (itemdata.GetData(KeyAltObjectPointer, (int&)pObj)
		&& itemdata.GetData(KeyLandsideItemType, (int&)itemType))
	{
		if (itemType == _LaneLinkage)
		{
			InputLandside* pInput = m_landsideEdit->getInput();;
			LandsideModifyLayoutObjectCmd* pModCmd = new LandsideModifyLayoutObjectCmd(pObj,pInput);	
			GetCurContext()->StartEditOp(pModCmd); 

			//remove arrow link line connected stretches. 
			LandsideIntersectionNode* pIntersection3DNode = (LandsideIntersectionNode*)pObj;
			int linkLineIdx = -1;
			if( pObj->GetType() == ALT_LINTERSECTION && itemdata.GetData(KeyItemIndex, linkLineIdx) )
			{
				if (linkLineIdx != -1)
				{
					pIntersection3DNode->RemoveLinkage(linkLineIdx);
				}
			}

			GetCurContext()->DoneCurEditOp(); 
			GetDocument()->UpdateAllViews(NULL,VM_UPDATE_COMMAND,(CObject*)pModCmd);
		}
	}
}

#include "DlgLayoutObjectDisplayProperties.h"
void CARCportLayoutEditor::OnCtxLayoutObjectDisplayProp(CWnd* pParent)
{
	QueryData& itemdata = GetCurContext()->getRClickItemData();
	LandsideFacilityLayoutObject* pObj = NULL;
	int PtIndex = -1;
	if(itemdata.GetData(KeyAltObjectPointer, (int&)pObj) )
	{
		LandsideFacilityLayoutObjectList layoutList;
		layoutList.AddObject(pObj);
		CDlgLayoutObjectDisplayProperties dlgProp(&layoutList,pParent);
		if( dlgProp.DoModal() ==IDOK)
		{
			pObj->SaveDisplayProp();
			//update all views
			GetDocument()->UpdateAllViews(NULL, VM_UPDATE_ALTOBJECT3D,(CObject*)pObj);
		}
	}
}

CTermPlanDoc* CARCportLayoutEditor::GetDocument() const
{
	return m_pDoc;
}

void* CARCportLayoutEditor::OnQueryDataObject( const CGuid& guid, const GUID& cls_guid )
{
	CTermPlanDoc* pDoc = GetDocument();
	Terminal& term = pDoc->GetTerminal();
	if (cls_guid == CPipeBase::getTypeGUID())
	{
		return term.m_pPipeDataSet->FindPipeByGuid(guid, cls_guid);
	}
	else if (cls_guid == CAreaBase::getTypeGUID())
	{
		return term.m_pAreas->FindAreaByGuid(guid);
	}
	else if (cls_guid == CPortalBase::getTypeGUID())
	{
		return pDoc->m_portals.FindPortalByGuid(guid);
	}
	else if (cls_guid == CProcessor2Base::getTypeGUID())
	{
		CPlacement* pCurPlace = pDoc->GetCurrentPlacement(EnvMode_Terminal);
		CProcessor2* pProc = pCurPlace->m_vDefined.GetProc2ByGuid(guid);
		if (NULL == pProc)
			pProc = pCurPlace->m_vUndefined.GetProc2ByGuid(guid);
		return pProc;
	}
	else if (cls_guid == WallShapeBase::getTypeGUID())
	{
		return term.m_pWallShapeList->GetShapelist().FindWallShapeByGuid(guid);
	}
	else if (cls_guid == CStructureBase::getTypeGUID())
	{
		return term.m_pStructureList->GetStructureList().FindStructureByGuid(guid);
	}	
	else if( cls_guid == AirWayPoint::getTypeGUID())
	{
		if(pDoc)
		{
			ALTAirspace *pAirspace = pDoc->GetInputAirside().GetAirspace();
			if(pAirspace)
			{
				return (void *)pAirspace->GetWaypoint(guid);
			}
		}
	}
	else if (cls_guid == AirHold::getTypeGUID())
	{
		if(pDoc)
		{
			ALTAirspace *pAirspace = pDoc->GetInputAirside().GetAirspace();
			if(pAirspace)
			{
				return (void *)pAirspace->GetAirHold(guid);
			}
		}
	}
	else if (cls_guid == AirSector::getTypeGUID())
	{
		if(pDoc)
		{
			ALTAirspace *pAirspace = pDoc->GetInputAirside().GetAirspace();
			if(pAirspace)
			{
				return (void *)pAirspace->GetAirSector(guid);
			}
		}
	}
	else if (cls_guid == CAirRoute::getTypeGUID())
	{
		if(pDoc)
		{
			ALTAirspace *pAirspace = pDoc->GetInputAirside().GetAirspace();
			if(pAirspace)
			{
				return (void *)pAirspace->GetAirRoute(guid);
			}
		}

	}
	else if(cls_guid == ALTAirport::getTypeGUID())
	{
		if(pDoc)
		{
			return (void*) pDoc->GetInputAirside().GetAirport(guid);
		}
	}
	else if (cls_guid == CTaxiInterruptLine::getTypeGUID())
	{
		if (pDoc)
		{
			const TaxiInterruptLineList& lines = pDoc->GetInputAirside().GetTaxiInterruptLines();
			return lines.GetTaxiInterruptLine(guid);
		}
	}


	return NULL;
}

bool CARCportLayoutEditor::OnQueryDataObjectList( std::vector<void*>& vData, const GUID& cls_guid )
{
	CTermPlanDoc* pDoc = GetDocument();
	Terminal& term = pDoc->GetTerminal();
	size_t nFirst = vData.size();
	if (cls_guid == CPipeBase::getTypeGUID())
	{
		CPipeDataSet* pPipeDataSet = term.m_pPipeDataSet;
		int nIncrease = pPipeDataSet->GetPipeCount();
		vData.reserve(nFirst + nIncrease);
		for(int i=0;i<nIncrease;i++)
			vData.push_back(pPipeDataSet->GetPipeAt(i));
		return true;
	}
	else if (cls_guid == CAreaBase::getTypeGUID())
	{
		CAreaList& areas = term.m_pAreas->m_vAreas;
		vData.resize(nFirst + areas.size());
		std::copy(areas.begin(), areas.end(), vData.begin() + nFirst);
		return true;
	}
	else if (cls_guid == CPortalBase::getTypeGUID())
	{
		CPortalList& portals = pDoc->m_portals.m_vPortals;
		vData.resize(nFirst + portals.size());
		std::copy(portals.begin(), portals.end(), vData.begin() + nFirst);
		return true;
	}
	else if (cls_guid == CProcessor2Base::getTypeGUID())
	{
		CPlacement* pCurPlace = pDoc->GetCurrentPlacement(EnvMode_Terminal);
		vData.resize(nFirst + pCurPlace->m_vDefined.size() + pCurPlace->m_vUndefined.size());
		std::copy(pCurPlace->m_vDefined.begin(), pCurPlace->m_vDefined.end(), vData.begin() + nFirst);
		std::copy(pCurPlace->m_vUndefined.begin(), pCurPlace->m_vUndefined.end(), vData.begin() + nFirst + pCurPlace->m_vDefined.size());
		return true;
	}
	else if (cls_guid == WallShapeBase::getTypeGUID())
	{
		const WallShapArray& wallShapeArray = term.m_pWallShapeList->GetShapelist();
		int nIncrease = wallShapeArray.getCount();
		vData.reserve(nFirst + nIncrease);
		for(int i=0;i<nIncrease;i++)
			vData.push_back(wallShapeArray.getItem(i));
		return true;
	}
	else if (cls_guid == CStructureBase::getTypeGUID())
	{
		const StructureArray& structureArray = term.m_pStructureList->GetStructureList();
		int nIncrease = structureArray.getCount();
		vData.reserve(nFirst + nIncrease);
		for(int i=0;i<nIncrease;i++)
			vData.push_back(structureArray.getItem(i));
		return true;
	}
	else if( cls_guid == AirWayPoint::getTypeGUID())
	{
		if(pDoc)
		{
			ALTAirspace *pAirspace = pDoc->GetInputAirside().GetAirspace();
			if(pAirspace)
			{
				ALTObjectList* pObjList = pAirspace->getWaypointList();
				vData.reserve(pObjList->size());

				for (ALTObjectList::iterator iter = pObjList->begin();
					iter != pObjList->end(); ++ iter)
				{
					vData.push_back((*iter).get());
				}
			}
		}
		return true;
	}
	else if (cls_guid == AirHold::getTypeGUID())
	{
		if(pDoc)
		{
			ALTAirspace *pAirspace = pDoc->GetInputAirside().GetAirspace();
			if(pAirspace)
			{
				ALTObjectList* pObjList = pAirspace->getAirHoldList();
				vData.reserve(pObjList->size());

				for (ALTObjectList::iterator iter = pObjList->begin();
					iter != pObjList->end(); ++ iter)
				{
					vData.push_back((*iter).get());
				}
			}
		}
		return true;
	}
	else if (cls_guid == AirSector::getTypeGUID())
	{
		if(pDoc)
		{
			ALTAirspace *pAirspace = pDoc->GetInputAirside().GetAirspace();
			if(pAirspace)
			{
				ALTObjectList* pObjList = pAirspace->getAirSectorList();
				vData.reserve(pObjList->size());

				for (ALTObjectList::iterator iter = pObjList->begin();
					iter != pObjList->end(); ++ iter)
				{
					vData.push_back((*iter).get());
				}
			}
		}
		return true;
	}
	else if (cls_guid == CAirRoute::getTypeGUID())
	{
		if(pDoc)
		{
			ALTAirspace *pAirspace = pDoc->GetInputAirside().GetAirspace();
			if(pAirspace)
			{
				ALTObjectList* pObjList = pAirspace->getAirRouteList();
				vData.reserve(pObjList->size());

				for (ALTObjectList::iterator iter = pObjList->begin();
					iter != pObjList->end(); ++ iter)
				{
					vData.push_back((*iter).get());
				}
			}
		}
		return true;
	}
	else if (cls_guid == ALTAirport::getTypeGUID())
	{
		if(pDoc)
		{
			InputAirside& inputAirside = pDoc->GetInputAirside();
			int nAirportCount = inputAirside.GetAirportCount();
			vData.reserve(nAirportCount);
			for (int nAirport = 0; nAirport <nAirportCount; ++nAirport)
			{
				ALTAirport *pAirport = inputAirside.GetAirportbyIndex(nAirport);

				vData.push_back(pAirport);
			}
		}
		return true;
	}
	else if (cls_guid == CTaxiInterruptLine::getTypeGUID())
	{
		if (pDoc)
		{
			TaxiInterruptLineList& lines = pDoc->GetInputAirside().GetTaxiInterruptLines();
			size_t nCount = lines.GetElementCount();
			for(size_t i=0;i<nCount;i++)
			{
				CTaxiInterruptLine* pLine = lines.GetItem(i);
				vData.push_back(pLine);
			}
		}
		return true;
	}
	else if (cls_guid == Structure::getTypeGUID())
	{
		if (pDoc)
		{
			Topography* inputTopography = pDoc->GetInputAirside().GetTopography();
			if (inputTopography)
			{
				ALTObjectList* pObjList = inputTopography->getAirStructureList();
				vData.reserve(pObjList->size());
				for (ALTObjectList::iterator iter = pObjList->begin();
					iter != pObjList->end(); ++ iter)
				{
					vData.push_back((*iter).get());
				}
			}
		}
		return true;
	}
	else if (cls_guid == Surface::getTypeGUID())
	{
		if (pDoc)
		{
			Topography* inputTopography = pDoc->GetInputAirside().GetTopography();
			if (inputTopography)
			{
				ALTObjectList* pObjList = inputTopography->getSurfaceList();
				vData.reserve(pObjList->size());
				for (ALTObjectList::iterator iter = pObjList->begin();
					iter != pObjList->end(); ++ iter)
				{
					vData.push_back((*iter).get());
				}
			}
		}
		return true;
	}
	else if(cls_guid == Contour::getTypeGUID())
	{
		if (pDoc)
		{
			Topography* inputTopography = pDoc->GetInputAirside().GetTopography();
			if (inputTopography)
			{
				ALTObjectList* pObjList = inputTopography->getContourList();
				vData.reserve(pObjList->size());
				for (ALTObjectList::iterator iter = pObjList->begin();
					iter != pObjList->end(); ++ iter)
				{
					vData.push_back((*iter).get());
				}
			}
		}
		return true;
	}
	else
	{
		ASSERT(0);
	}


	return false;
}

//bool CARCportLayoutEditor::OnSaveDataObject( const CGuid& guid, const GUID& cls_guid )
//{
//	if (cls_guid == CPipeBase::getTypeGUID())
//	{
//		GetDocument()->GetTerminal().m_pPipeDataSet->saveDataSet(GetDocument()->m_ProjInfo.path, false);
//		return true;
//	}
//	return NULL;
//}

InputAirside* CARCportLayoutEditor::OnQueryInputAirside()
{
	return	&(m_pDoc->GetInputAirside());	
}

CRenderFloorList CARCportLayoutEditor::OnQueryAirsideFloor( int nAirportID )
{
	std::map<int ,CAirsideLevelList*>::iterator itr= m_AirsideLevelsMap.find(nAirportID);
	CAirsideLevelList* list = NULL;
	if(m_AirsideLevelsMap.end()!=itr){
		list = itr->second;
	}
	list = new CAirsideLevelList(nAirportID);
	m_AirsideLevelsMap[nAirportID] = list;

	list->ReadFullLevelList();

	CRenderFloorList ret;
	for(int i=0;i<list->GetCount();i++){
		ret.push_back(&list->GetLevel(i));
	}
	return ret;
}

bool CARCportLayoutEditor::OnQueryAirportDataObjectList( int airportID,std::vector<void*>& vData, const GUID& cls_guid )
{
	InputAirside* airside = OnQueryInputAirside();
	if(!airside)return false;
	ALTAirportLayout *pAirport = airside->GetAirportLayoutByID(airportID);

	if(pAirport == NULL)
		return false;
	if(pAirport->GetObjectList(cls_guid,vData))
		return true;

	return false;
}

CString CARCportLayoutEditor::GetTexturePath() const
{
	return ((CTermPlanApp*)AfxGetApp())->GetTexturePath();
}



bool CARCportLayoutEditor::IsShowText() const
{
	return !m_pDoc->m_displayoverrides.m_pbProcDisplayOverrides[PDP_DISP_PROCNAME];
}

bool CARCportLayoutEditor::IsEditable() const
{
	if(m_pDoc->m_bLayoutLocked)
	{
		return false;
	}
	if(m_pDoc->m_eAnimState != CTermPlanDoc::anim_none)
	{
		return false;
	}
	return true;
}


