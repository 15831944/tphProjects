#include "StdAfx.h"
#include "resource.h"

#include "Landside/InputLandside.h"
#include "common/WinMsg.h"
#include ".\landsideparkinglotpropdlgimpl.h"
#include "landside\LandsideLayoutObject.h"
#include "ARCLayoutObjectDlg.h"
#include <common/Path2008.h>
#include "landside/LandsideParkingLot.h"
#include "CommonData/QueryData.h"
#include "Floor2.h"
#include "TermPlanDoc.h"
#include "../common/UnitsManager.h"
#include "ChangeVerticalProfileDlg.h"
	

#define PARK_LEVEL_HEIGHT "Level Height"
#define PARK_LEVEL_THICKNESS "Level Thickness"

#define PARK_AREA_VISIBLE "visible"
#define PARK_AREA_INVISIBLE "invisible"

#define PARK_ENTRYEXIT_PORT "Port"
#define PARK_DRIVEPIPE_WIDTH "Pipe Width"
#define PARK_DRIVEPIPE_LANNUM "Lane Num"
#define PARK_DRIVEPIPE_DIRECTION "Direction"

class ParkLotTreeItemData : public IPropDlgTreeItemData
{
public:
	ParkLotTreeItemData(PLItemType t)
	{
		mType = t; 
		nLevelIndex = 0;
		nOtherIndex = 0;
	} 
	PLItemType mType;
	int nLevelIndex;
	int nOtherIndex;
};


class ParkLotTreeItemDataInst : public PropDlgTreeItemDataInstance
{
public:
	ParkLotTreeItemData* create(PLItemType pt,int nLevelIndex=-2)
	{
		//nLevelIndex: -2:none;-1:base level;>=0:other level index
		ParkLotTreeItemData* pData = new ParkLotTreeItemData(pt);
		pData->nLevelIndex=nLevelIndex;
		Add(pData);
		return pData;
	};
};
//////////////////////////////////////////////////////////////////////////
class ParkingLotTreeItem : public TreeCtrlItemInDlg
{
public:
	DLG_TREEIREM_CONVERSION_DEF(ParkingLotTreeItem, TreeCtrlItemInDlg)	
	
	void SetLotItemData(ParkLotTreeItemData* pdata)
	{
		AirsideObjectTreeCtrlItemDataEx* pParentData= GetData();
		if(!pParentData)
			pParentData = InitData();
		pParentData->dwptrItemData = (DWORD)pdata;
	}
	ParkLotTreeItemData* GetLotItemData()
	{
		if( AirsideObjectTreeCtrlItemDataEx* pParentData= GetData() )
		{
			return (ParkLotTreeItemData*)pParentData->dwptrItemData;
		}
		return NULL;
	}

	ParkingLotTreeItem AddChildItem(CString s,ParkLotTreeItemData*pData )
	{
		ParkingLotTreeItem item = AddChild(s);
		item.SetLotItemData(pData);
		return item;
	}

	PLItemType GetItemType()
	{
		if(ParkLotTreeItemData* pData = GetLotItemData())
		{
			return pData->mType;
		}
		return _NonType;
	}

	int GetOtherData()
	{
		if(ParkLotTreeItemData* pData = GetLotItemData())
		{
			return pData->nOtherIndex;
		}
		return 0;
	}
	void SetOtherData(int index)
	{
		if(ParkLotTreeItemData* pData = GetLotItemData())
		{
			pData->nOtherIndex = index;
		}	
	}	
};
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
CString LandParkingLotPropDlgImpl::getTitle()
{
	/*if(m_pObject->getID()<0)
		return "New Landside ParkingLot";
	else
		return "Edit Landside ParkingLot";*/
	return "Landside ParkingLot Specification";
}

void LandParkingLotPropDlgImpl::LoadTree()
{
	getCheckBoxCellPhone()->ShowWindow(SW_SHOW);



	LandsideParkingLot* pparkLot = (LandsideParkingLot*)m_pObject;
	if(!pparkLot)
		return;

	if(pparkLot->IsCellPhone())
		getCheckBoxCellPhone()->SetCheck(TRUE);
	else
		getCheckBoxCellPhone()->SetCheck(FALSE);



	m_vSelectedItem.clear();
	CAirsideObjectTreeCtrl& m_treeProp = GetTreeCtrl();
	m_treeProp.SetRedraw(0);	

	m_bReloadTree=true;
	m_treeProp.DeleteAllItems();
	m_bReloadTree=false;
	mDataInst->Clear();
	m_hRClickItem = NULL;


	//load base level	
	/*if (pparkLot->m_baselevel.m_name==_T(""))
	{
		pparkLot->m_baselevel.m_name=_T("Base Level");
	}*/
	//HTREEITEM hBaseLevel = m_treeProp.InsertItem(pparkLot->m_baselevel.m_name+_T("(0)"));
	//ParkingLotTreeItem baseLevelItem = ParkingLotTreeItem(GetTreeCtrl(),hBaseLevel);
	//baseLevelItem.SetLotItemData(mDataInst->create(_BaseLevel,-1));
	//
	//AddLevelChildItems(baseLevelItem,pparkLot->m_baselevel,-1);

	//baseLevelItem.Expand();
	CString strLevelNum;
	for (int i=0;i<pparkLot->GetLevelCount();i++)
	{
		strLevelNum.Format(_T("(%d)"),i+1);
		LandsideParkingLotLevel *curParkingLevel= pparkLot->GetLevel(i);
		if(curParkingLevel->m_name==_T(""))
		{
			curParkingLevel->m_name=_T("New Level");
		}
		HTREEITEM hOtherLevel = m_treeProp.InsertItem(curParkingLevel->m_name+strLevelNum);
		ParkingLotTreeItem otherLevelItem = ParkingLotTreeItem(GetTreeCtrl(),hOtherLevel);
		otherLevelItem.SetLotItemData(mDataInst->create(_OtherLevel,i));
		AddLevelChildItems(otherLevelItem,*curParkingLevel,i);
		otherLevelItem.Expand();
	}

	m_treeProp.SetRedraw(1);

	if (!m_vSelectedItem.empty())
	{
		GetTreeCtrl().SetFocus();
		HTREEITEM hItem = m_vSelectedItem.front();
		GetTreeCtrl().Expand(hItem,TVE_EXPAND);
		GetTreeCtrl().SelectItem(hItem);
		GetTreeCtrl().SetItemState(hItem,TVIS_SELECTED|TVIS_FOCUSED,TVIS_SELECTED|TVIS_FOCUSED);
	}
}


void LandParkingLotPropDlgImpl::AddLevelChildItems(ParkingLotTreeItem levelItem,const LandsideParkingLotLevel &parkingLevel,int nLevelIndex)
{
	//nLevelIndex: -1:base level; >0:the other level index in otherlevel list 
	CAirsideObjectTreeCtrl& m_treeProp = GetTreeCtrl();
	ARCUnit_Length  curLengthUnit = GetCurLengthUnit();
	CString curLengthUnitString = CARCLengthUnit::GetLengthUnitString(curLengthUnit);

	//if(nLevelIndex!=-1)
	{	
		CString sfloorName = GetFloorName((int)parkingLevel.m_dHeight);
		ParkingLotTreeItem hHeightItem=levelItem.AddChildItem(PARK_LEVEL_HEIGHT,mDataInst->create(_Height,nLevelIndex));
		hHeightItem.SetStringText(PARK_LEVEL_HEIGHT,sfloorName);
	}
	ParkingLotTreeItem hThicknessItem=levelItem.AddChildItem(PARK_LEVEL_THICKNESS,mDataInst->create(_Thickness,nLevelIndex));
	hThicknessItem.SetLengthValueText(PARK_LEVEL_THICKNESS,parkingLevel.m_dThickness,curLengthUnit);

	{//area
		ParkingLotTreeItem areaRootItem = levelItem.AddChildItem("Region Areas:",mDataInst->create(_AreaRoot,nLevelIndex));
		areaRootItem.GetLotItemData()->nLevelIndex=nLevelIndex;
		const ParkingLotAreas& areas = parkingLevel.m_areas;
		for(size_t i=0;i<areas.m_areas.size();i++)
		{
			const ParkingLotAreas::Area& area = areas.m_areas[i];
			CString sArea;
			sArea.Format("Region %d",i+1);					
			ParkingLotTreeItem areaItem = areaRootItem.AddChildItem(sArea,mDataInst->create(_Area,nLevelIndex));			
			areaItem.GetLotItemData()->nLevelIndex=nLevelIndex;
			areaItem.SetOtherData(i);

			ParkingLotTreeItem visbItem = areaRootItem.AddChildItem("", mDataInst->create(_AreaVisible,nLevelIndex));
			visbItem.SetString(area.bVisble?PARK_AREA_VISIBLE:PARK_AREA_INVISIBLE,true);
			visbItem.SetOtherData(i);

			CString sAreaPath;
			sAreaPath.Format("Area Path(%s)",curLengthUnitString);
			ParkingLotTreeItem areaPath = areaItem.AddChildItem(sAreaPath,mDataInst->create(_AreaPath,nLevelIndex));
			areaPath.AddChildPath(area.mpoly,GetCurLengthUnit(),0);
			areaPath.SetOtherData(i);		

	
			if (m_newExpandItem.type == _AreaRoot) {
				if (i == areas.m_areas.size() - 1) {
					m_treeProp.Expand(areaItem.m_hItem, TVE_EXPAND);
					m_treeProp.Expand(areaPath.m_hItem, TVE_EXPAND);
				}
			}
			if (m_newExpandItem.type == _AreaPath) {
				if (i == m_newExpandItem.itemIndex) {
					m_treeProp.Expand(areaItem.m_hItem, TVE_EXPAND);
					m_treeProp.Expand(areaPath.m_hItem, TVE_EXPAND);
				}				
			}

			SelectParkingLotItem(areaItem.m_hItem,nLevelIndex,_ParkingLotArea,i);
		}
		areaRootItem.Expand();

	}
	{//parkspace
		ParkingLotTreeItem parkSpaceRootItem = levelItem.AddChildItem("Parking Spaces:",mDataInst->create(_ParkSpaceRoot,nLevelIndex));
		const ParkingLotParkingSpaceList& spacelist = parkingLevel.m_parkspaces;
		for(size_t i=0;i<spacelist.m_spaces.size();i++)
		{
			const ParkingSpace& parkspace = spacelist.m_spaces[i];
			CString sPark;
			sPark.Format("Parking Space(%d):",i+1);
			ParkingLotTreeItem spaceItem = parkSpaceRootItem.AddChildItem(sPark,mDataInst->create(_ParkSpace,nLevelIndex));		
			spaceItem.SetOtherData(i);

			CString sPath = "Path (" + curLengthUnitString + ")";
			ParkingLotTreeItem pathItem = spaceItem.AddChildItem(sPath,mDataInst->create(_PSPath,nLevelIndex));
			pathItem.AddChildPath(parkspace.m_ctrlPath,curLengthUnit,0);
			pathItem.SetOtherData(i);
			pathItem.Expand();

			{			
				ParkingLotTreeItem typeItem = spaceItem.AddChildItem(PARK_SPOT_TYPE,mDataInst->create(_PSType,nLevelIndex));
				typeItem.SetStringText(PARK_SPOT_TYPE,  parkspace.GetTypeString());		
				typeItem.SetOtherData(i);
			}
			{			
				ParkingLotTreeItem withItem = spaceItem.AddChildItem(PARK_SPOT_WIDTH,mDataInst->create(_PSWidth,nLevelIndex));
				withItem.SetLengthValueText(PARK_SPOT_WIDTH,parkspace.m_dWidth,curLengthUnit);	
				withItem.SetOtherData(i);
			}
			{				
				ParkingLotTreeItem withItem = spaceItem.AddChildItem(PARK_SPOT_LENGTH,mDataInst->create(_PSLength,nLevelIndex));
				withItem.SetLengthValueText(PARK_SPOT_LENGTH,parkspace.m_dLength,curLengthUnit);	
				withItem.SetOtherData(i);
			}
			{
				ParkingLotTreeItem angleItem = spaceItem.AddChildItem(PARK_SPOT_ANGLE,mDataInst->create(_PSAngle,nLevelIndex));
				angleItem.SetDegreeValueText(PARK_SPOT_ANGLE,parkspace.m_dAngle);	
				angleItem.SetOtherData(i);
			}
			{
				ParkingLotTreeItem opereationItem =  spaceItem.AddChildItem(PARK_SPOT_OPERRATION,mDataInst->create(_PSOperation,nLevelIndex));
				opereationItem.SetStringText(PARK_SPOT_OPERRATION,parkspace.GetOpTypeString());
				opereationItem.SetOtherData(i);
			}

			if (m_newExpandItem.type == _ParkSpaceRoot) {
				if (i == spacelist.m_spaces.size() - 1) {
					m_treeProp.Expand(spaceItem.m_hItem, TVE_EXPAND);
					m_treeProp.Expand(pathItem.m_hItem, TVE_EXPAND);
// 					m_treeProp.SelectItem(spaceItem.m_hItem);
				}
			}
			if (m_newExpandItem.type == _PSPath) {
				if (i == m_newExpandItem.itemIndex) {
					m_treeProp.Expand(spaceItem.m_hItem, TVE_EXPAND);
					m_treeProp.Expand(pathItem.m_hItem, TVE_EXPAND);
// 					m_treeProp.SelectItem(spaceItem.m_hItem);
				}
			}

		SelectParkingLotItem(spaceItem.m_hItem,nLevelIndex,_ParkingLotSpace,i);
		}
		parkSpaceRootItem.Expand();
	}
	{//drive pipe
		ParkingLotTreeItem parkDrivePipeRootItem = levelItem.AddChildItem("Parking DriveLanes:",mDataInst->create(_ParkDrivePipeRoot,nLevelIndex));
		const ParkingDrivePipeList& pipelist = parkingLevel.m_dirvepipes;

		for(size_t i=0;i<pipelist.m_pipes.size();i++)
		{
			const ParkingDrivePipe& parkpipe = pipelist.m_pipes[i];
			CString sPipe;
			sPipe.Format("Parking DrivePipe(%d):",i+1);
			ParkingLotTreeItem pipeItem = parkDrivePipeRootItem.AddChildItem(sPipe,mDataInst->create(_ParkDrivePipe,nLevelIndex));			
			pipeItem.SetOtherData(i);
			pipeItem.Expand();

			CString sPath = "Control Point(" + curLengthUnitString + ")";
			ParkingLotTreeItem pathItem = pipeItem.AddChildItem(sPath,mDataInst->create(_PPPath,nLevelIndex));
			double dLevel = static_cast<double>(nLevelIndex);
			pathItem.AddDrivePipePath(parkpipe.m_ctrlPath,curLengthUnit,dLevel);
			pathItem.SetOtherData(i);

			{			
				ParkingLotTreeItem widthItem = pipeItem.AddChildItem(PARK_DRIVEPIPE_WIDTH,mDataInst->create(_PPWidth,nLevelIndex));
				widthItem.SetLengthValueText(PARK_DRIVEPIPE_WIDTH,parkpipe.m_width,curLengthUnit);	
				widthItem.SetOtherData(i);

				ParkingLotTreeItem lannumItem = pipeItem.AddChildItem(PARK_DRIVEPIPE_LANNUM, mDataInst->create(_PPLaneNum,nLevelIndex));
				lannumItem.SetIntValueText(PARK_DRIVEPIPE_LANNUM,parkpipe.m_nLaneNum);
				lannumItem.SetOtherData(i);

				ParkingLotTreeItem directionItem = pipeItem.AddChildItem(PARK_DRIVEPIPE_DIRECTION,mDataInst->create(_PPDirection,nLevelIndex));
				directionItem.SetStringText(PARK_DRIVEPIPE_DIRECTION,parkpipe.getDirectionTypeStr());
				directionItem.SetOtherData(i);
			}

			if (m_newExpandItem.type == _ParkDrivePipeRoot) {
				if (i == pipelist.m_pipes.size() - 1) {
					m_treeProp.Expand(pipeItem.m_hItem, TVE_EXPAND);
					m_treeProp.Expand(pathItem.m_hItem, TVE_EXPAND);
// 					m_treeProp.SelectItem(pipeItem.m_hItem);
				}
			}
			if (m_newExpandItem.type == _PPPath) {
				if (i == m_newExpandItem.itemIndex) {
					m_treeProp.Expand(pipeItem.m_hItem, TVE_EXPAND);
					m_treeProp.Expand(pathItem.m_hItem, TVE_EXPAND);
// 					m_treeProp.SelectItem(pipeItem.m_hItem);
				}
			}

			SelectParkingLotItem(pipeItem.m_hItem,nLevelIndex,_ParkingLotDrivePipe,i);
		}
		parkDrivePipeRootItem.Expand();
	}
	{//entry exit
		ParkingLotTreeItem entryExitRootItem = levelItem.AddChildItem("Entry && Exit:", mDataInst->create(_EntryExitRoot,nLevelIndex));
		const ParkingLotEntryExitList& entExits = parkingLevel.m_entryexits;

		for(size_t i=0;i<entExits.m_entexits.size();i++)
		{
			const ParkLotEntryExit& entexit = entExits.m_entexits[i];
			CString sEntExit;
			sEntExit.Format("Entry/Exit %d:",i+1);
			ParkingLotTreeItem entExitItem = entryExitRootItem.AddChildItem(sEntExit,mDataInst->create(_EntryExit,nLevelIndex));
			entExitItem.SetOtherData(i);

			CString sLinePath;
			sLinePath.Format("Line Path(%s):", curLengthUnitString.GetString());
			ParkingLotTreeItem lineItem = entExitItem.AddChildItem(sLinePath,mDataInst->create(_LinePath,nLevelIndex));
			lineItem.AddChildPath(entexit.m_line,curLengthUnit,0);
			lineItem.SetOtherData(i);


			if (m_newExpandItem.type == _EntryExitRoot) {
				if (i == entExits.m_entexits.size() - 1) {
					m_treeProp.Expand(entExitItem.m_hItem, TVE_EXPAND);
					m_treeProp.Expand(lineItem.m_hItem, TVE_EXPAND);
// 					m_treeProp.SelectItem(entExitItem.m_hItem);
				}
			}
			if (m_newExpandItem.type == _LinePath) {
				if (i == m_newExpandItem.itemIndex) {
					m_treeProp.Expand(entExitItem.m_hItem, TVE_EXPAND);
					m_treeProp.Expand(lineItem.m_hItem, TVE_EXPAND);
// 					m_treeProp.SelectItem(entExitItem.m_hItem);
				}
			}

			//
			CString sLinkNode = "Link Lanes:";
			ParkingLotTreeItem laneNodeRootItem = entExitItem.AddChildItem(sLinkNode,mDataInst->create(_LaneNodeRoot,nLevelIndex));
			laneNodeRootItem.SetOtherData(i);

			for(size_t k=0;k<entexit.m_vLinkNodes.size();k++)
			{
				const LaneNode& node = entexit.m_vLinkNodes[k];
				CString sNode;
				sNode.Format("%s:%s Index:%d", node.IsLaneEntry()?"Lane Entry":"Lane Exit",node.m_pStretch->getName().GetIDString(), node.nLaneIdx);
				ParkingLotTreeItem nodeItem =  laneNodeRootItem.AddChildItem(sNode,mDataInst->create(_LaneNode,nLevelIndex));
				nodeItem.AddTokenItem(node.m_pStretch->getName().GetIDString());
				nodeItem.SetOtherData(k);

				if (m_newExpandItem.type == _LaneNodeRoot) {
					if (i == m_newExpandItem.itemIndex) {
						m_treeProp.Expand(entExitItem.m_hItem, TVE_EXPAND);
						m_treeProp.Expand(lineItem.m_hItem, TVE_EXPAND);
						m_treeProp.Expand(laneNodeRootItem.m_hItem, TVE_EXPAND);
						m_treeProp.Expand(nodeItem.m_hItem, TVE_EXPAND);
// 						m_treeProp.SelectItem(entExitItem.m_hItem);
					}
				}
			}
			{			
				ParkingLotTreeItem entPortItem = entExitItem.AddChildItem(PARK_ENTRYEXIT_PORT,mDataInst->create(_EntExitPort,nLevelIndex));
				CString strport;
				strport.Format(" %d",entexit.m_nInOutPort);

				entPortItem.SetStringText(PARK_ENTRYEXIT_PORT,strport);
				entPortItem.SetOtherData(i);
			}

			SelectParkingLotItem(entExitItem.m_hItem,nLevelIndex,_ParkingLotEntry,i);
		}
		entryExitRootItem.Expand();
	}	
}
LandParkingLotPropDlgImpl::LandParkingLotPropDlgImpl( LandsideFacilityLayoutObject* pObj,CARCLayoutObjectDlg* pDlg,QueryData* pData )
:ILayoutObjectPropDlgImpl(pObj,pDlg)
{
	if(pData)
		m_pData = *pData;

	mDataInst = new ParkLotTreeItemDataInst();
	m_bReloadTree = false;
}

LandParkingLotPropDlgImpl::~LandParkingLotPropDlgImpl()
{
	delete mDataInst;
}


void LandParkingLotPropDlgImpl::OnContextMenu( CMenu& menuPopup, CMenu *&pMenu )
{
	ParkingLotTreeItem clickItem(GetTreeCtrl(),m_hRClickItem);
	LandsideParkingLot* pparkLot = (LandsideParkingLot*)m_pObject;
	if (m_hRClickItem==NULL)
	{
		return;
	}
	PLItemType itemType = clickItem.GetItemType();
	if (itemType == _BaseLevel||itemType ==_OtherLevel)
	{
		pMenu = menuPopup.GetSubMenu(102);
		ParkLotTreeItemData* pData = clickItem.GetLotItemData();
		LandsideParkingLotLevel* plevel  = pparkLot->GetLevel(pData->nLevelIndex);
		pMenu->ModifyMenu(0,MF_BYPOSITION,ID_PARKINGLOTLEVEL_SHOW,plevel->m_bVisible?"Hide Level":"Show Level");
		
	}
	if(itemType == _AreaPath  || itemType == _AreaRoot 
		|| itemType == _ParkSpaceRoot || itemType == _PSPath
		|| itemType == _EntryExitRoot || itemType == _LinePath 
		|| itemType == _LaneNodeRoot
		|| itemType == _ParkDrivePipeRoot || itemType ==  _PPPath)
	{
		pMenu = menuPopup.GetSubMenu(73);			
	}



	if(itemType == _Area || itemType == _ParkSpace || itemType == _LaneNode || itemType == _ParkDrivePipe || itemType == _EntryExit)
	{
		pMenu = menuPopup.GetSubMenu(100);
	}
	GetTreeCtrl().SelectItem(m_hRClickItem);
}
FallbackReason LandParkingLotPropDlgImpl::GetFallBackReason()
{
	ParkingLotTreeItem clickItem(GetTreeCtrl(),m_hRClickItem);
	PLItemType itemType = clickItem.GetItemType();

	if(itemType == _AreaPath  || itemType == _AreaRoot 
		|| itemType == _ParkSpaceRoot || itemType == _PSPath || itemType == _ParkDrivePipeRoot || itemType == _PPPath)
	{
		return	PICK_MANYPOINTS;
	}
	if(itemType == _EntryExitRoot || itemType == _LinePath)
	{
		return PICK_TWOPOINTS;
	}
	if(itemType == _LaneNodeRoot)
	{
		return PICK_PARKLOT_ENTTRY_LANENODE;
	}
	return PICK_START_INDEX;
}


void LandParkingLotPropDlgImpl::DoFallBackFinished( WPARAM wParam, LPARAM lParam )
{
	ParkingLotTreeItem clickItem(GetTreeCtrl(),m_hRClickItem);
	ParkLotTreeItemData* pData = clickItem.GetLotItemData();

	LandsideParkingLot* pparkLot = (LandsideParkingLot*)m_pObject;
	PLItemType itemType = clickItem.GetItemType();

	LandsideParkingLotLevel *pLevel = pparkLot->GetLevel(pData->nLevelIndex);
	if(!pLevel)
		return;
	
	ParkingLotAreas& areas = pLevel->m_areas;
	if(!pData)
		return;
	
		
	m_newExpandItem.SetNewItemInfo(itemType, pData->nOtherIndex);
	if(itemType == _AreaPath )
	{		
		ParkingLotAreas::Area& area = areas.getArea(pData->nOtherIndex);		
		GetFallBackAsPath(wParam,lParam,area.mpoly);		
		
	}
	if(itemType == _AreaRoot)
	{	
		ParkingLotAreas::Area area;		
		GetFallBackAsPath(wParam,lParam,area.mpoly);
		areas.AddArea(area);
	}

	if(itemType == _PSPath)
	{	
		ParkingLotParkingSpaceList& parkspaces = pLevel->m_parkspaces;		
		if( ParkingSpace* space = parkspaces.getSpacePtr(pData->nOtherIndex))
		{
			GetFallBackAsPath(wParam,lParam,space->m_ctrlPath);
		}
				
	}
	if(itemType == _ParkSpaceRoot)
	{
		ParkingSpace space;
		GetFallBackAsPath(wParam,lParam,space.m_ctrlPath);
		ParkingLotParkingSpaceList& parkspaces = pLevel->m_parkspaces;		
		parkspaces.AddSpace(space);
	}
	if (itemType == _PPPath)
	{
		ParkingDrivePipeList& drivePipes = pLevel->m_dirvepipes;
		ParkingDrivePipe& pipe = drivePipes.getPipe(pData->nOtherIndex);
		GetFallBackAsPath(wParam,lParam, pipe.m_ctrlPath);
	}
	if (itemType == _ParkDrivePipeRoot)
	{
		ParkingDrivePipe pipe;
		GetFallBackAsPath(wParam,lParam, pipe.m_ctrlPath);
		ParkingDrivePipeList& drivePipes = pLevel->m_dirvepipes;
		drivePipes.AddPipe(pipe);
	}
	if(itemType == _EntryExitRoot)
	{
		ParkLotEntryExit ent;
		GetFallBackAsPath(wParam,lParam,ent.m_line);
		ParkingLotEntryExitList& entlist = pLevel->m_entryexits;
		entlist.AddEnt(ent);		
	}
	if(itemType == _LinePath)
	{
		ParkingLotEntryExitList& entlist = pLevel->m_entryexits;
		ParkLotEntryExit* ent = entlist.getEntryPtr(pData->nOtherIndex);
		if(ent)
		{
			GetFallBackAsPath(wParam,lParam,ent->m_line);
		}
	}
	if(itemType == _LaneNodeRoot)
	{		
		int nEntExitIdx = pData->nOtherIndex;
		ParkingLotEntryExitList& entlist = pLevel->m_entryexits;
		ParkLotEntryExit& entexit = entlist.getEnt(nEntExitIdx);
		std::vector<LaneNode>* nodeList = (std::vector<LaneNode>*)wParam;
		for(size_t i=0;i<nodeList->size();i++)
		{
			entexit.addNode(nodeList->at(i));
		}			
	}
	Update3D();
	LoadTree();

}

void LandParkingLotPropDlgImpl::OnPropDelete()
{
	ParkingLotTreeItem clickItem(GetTreeCtrl(),m_hRClickItem);
	if(!m_hRClickItem)
	{
		return;
	}
	HTREEITEM selItem=GetTreeCtrl().GetParentItem(m_hRClickItem);

	PLItemType itemType = clickItem.GetItemType();
	LandsideParkingLot* pparkLot = (LandsideParkingLot*)m_pObject;
	ParkLotTreeItemData* pData = clickItem.GetLotItemData();
	if(!pData)
		return;
	LandsideParkingLotLevel *curLevel= pparkLot->GetLevel(pData->nLevelIndex);
   /* if (pData->nLevelIndex==-1)
    {
		curLevel=&pparkLot->m_baselevel;
    }else if (pData->nLevelIndex>-1&&pData->nLevelIndex<(int)pparkLot->m_otherLevels.size())
    {
		curLevel=&(pparkLot->m_otherLevels.at());
    }else*/
	if(!curLevel){
		return;
	}

	//delete area
	if(itemType == _Area )
	{		
		ParkingLotAreas& areas = curLevel->m_areas;
		areas.RemoveArea(pData->nOtherIndex);

		HTREEITEM tmpItem=GetTreeCtrl().GetNextItem(m_hRClickItem,TVGN_NEXT);
		int index=clickItem.GetOtherData();
		CString sName;

		while(tmpItem!=NULL)
		{
			sName.Format("Region %d",index+1);
			GetTreeCtrl().SetItemText(tmpItem,sName);
			ParkingLotTreeItem tmpParkingLotItem(GetTreeCtrl(),tmpItem);
			tmpParkingLotItem.SetOtherData(index);
			tmpItem=GetTreeCtrl().GetNextItem(tmpItem,TVGN_NEXT);
			index++;
		}
	}
	//delete parkspot
	if(itemType == _ParkSpace)
	{
		ParkingLotParkingSpaceList& parkspaces = curLevel->m_parkspaces;
		parkspaces.RemoveSpace(pData->nOtherIndex);	

		HTREEITEM tmpItem=GetTreeCtrl().GetNextItem(m_hRClickItem,TVGN_NEXT);
		int index=clickItem.GetOtherData();
		CString sName;

		while(tmpItem!=NULL)
		{
			sName.Format("Parking Space(%d):",index+1);
			GetTreeCtrl().SetItemText(tmpItem,sName);
			ParkingLotTreeItem tmpParkingLotItem(GetTreeCtrl(),tmpItem);
			tmpParkingLotItem.SetOtherData(index);
			tmpItem=GetTreeCtrl().GetNextItem(tmpItem,TVGN_NEXT);
			index++;
		}
	}
	//delete drive pipe
	if (itemType == _ParkDrivePipe)
	{
		ParkingDrivePipeList& drivePipes = curLevel->m_dirvepipes;
		drivePipes.RemovePipe(pData->nOtherIndex);

		HTREEITEM tmpItem=GetTreeCtrl().GetNextItem(m_hRClickItem,TVGN_NEXT);
		int index=clickItem.GetOtherData();
		CString sName;

		while(tmpItem!=NULL)
		{
			sName.Format("Parking DrivePipe(%d):",index+1);
			GetTreeCtrl().SetItemText(tmpItem,sName);
			ParkingLotTreeItem tmpParkingLotItem(GetTreeCtrl(),tmpItem);
			tmpParkingLotItem.SetOtherData(index);
			tmpItem=GetTreeCtrl().GetNextItem(tmpItem,TVGN_NEXT);
			index++;
		}
	}
	//delete ent exit
	if(itemType == _EntryExit)
	{
		ParkingLotEntryExitList& entlist = curLevel->m_entryexits;
		entlist.RemoveEnt(pData->nOtherIndex);	

		HTREEITEM tmpItem=GetTreeCtrl().GetNextItem(m_hRClickItem,TVGN_NEXT);
		int index=clickItem.GetOtherData();
		CString sName;

		while(tmpItem!=NULL)
		{
			sName.Format("Entry/Exit %d:",index+1);
			GetTreeCtrl().SetItemText(tmpItem,sName);
			ParkingLotTreeItem tmpParkingLotItem(GetTreeCtrl(),tmpItem);
			tmpParkingLotItem.SetOtherData(index);
			tmpItem=GetTreeCtrl().GetNextItem(tmpItem,TVGN_NEXT);
			index++;
		}
	}
	//delete link node
	if(itemType==_LaneNode)
	{
		ParkingLotTreeItem parentItem = clickItem.GetParent();
		ParkLotTreeItemData* pParentData = parentItem.GetLotItemData();
		if(pParentData)
		{
			int nEntExitIdx = pParentData->nOtherIndex;
			ParkingLotEntryExitList& entlist = curLevel->m_entryexits;
			ParkLotEntryExit& entexit = entlist.getEnt(nEntExitIdx);
			entexit.removeNode(pData->nOtherIndex);			
		}
	}

	//LoadTree();
	GetTreeCtrl().DeleteItem(m_hRClickItem);
	if (selItem!=NULL)
	{
		GetTreeCtrl().SelectItem(selItem);
	}
	Update3D();
}
void LandParkingLotPropDlgImpl::OnLevelNameEdit(HTREEITEM hTreeItem)
{
	ParkingLotTreeItem clickItem(GetTreeCtrl(),hTreeItem);
	ParkLotTreeItemData* pItemData = clickItem.GetLotItemData();
	if(!pItemData)
		return;

	CAirsideObjectTreeCtrl& m_treeProp = m_pParentDlg->GetTreeCtrl();
	LandsideParkingLot* pLot = (LandsideParkingLot*)m_pObject;
	LandsideParkingLotLevel *pLevel= pLot->GetLevel(pItemData->nLevelIndex);
	if(pLevel)
        m_treeProp.StringEditLabel(hTreeItem,pLevel->m_name);
}

void LandParkingLotPropDlgImpl::OnLevelHeightEdit(HTREEITEM hTreeItem)
{
	ParkingLotTreeItem clickItem(GetTreeCtrl(),hTreeItem);
	ParkLotTreeItemData* pItemData = ParkingLotTreeItem(clickItem.GetParent()).GetLotItemData();
	if(!pItemData)
		return;

	CAirsideObjectTreeCtrl& m_treeProp = m_pParentDlg->GetTreeCtrl();
	LandsideParkingLot* pLot = (LandsideParkingLot*)m_pObject;
	LandsideParkingLotLevel *pLevel=pLot->GetLevel(pItemData->nLevelIndex);;
	if(!pLevel)
		return;


	std::vector<CString> vString;
	CFloors& termFloors  = m_pParentDlg->GetDocument()->GetFloorByMode(EnvMode_LandSide);
	for(int i=0;i<termFloors.GetCount();i++)
	{
		CRenderFloor* pFloor = termFloors.GetFloor( i );
		vString.push_back(pFloor->FloorName());
	}
	GetTreeCtrl().SetComboWidth(300);
	GetTreeCtrl().SetComboString(hTreeItem,vString);
	GetTreeCtrl().m_comboBox.SetCurSel(0);
	
//// 	ParkingSpaceList& spaceList = pLevel->m_parkspaces;
////	ParkingSpace& space = spaceList.getSpace(pItemData->nOtherIndex);
//	double dUnitNum = ConvertLength(pLevel->m_dHeight);	
//
//	m_treeProp.SetDisplayType( 2 );
//	m_treeProp.SetSpinRange(0,10000);
//	m_treeProp.SetDisplayNum(static_cast<int>(dUnitNum+0.5));	
//	m_treeProp.SpinEditLabel( hTreeItem );
}
void LandParkingLotPropDlgImpl::OnLevelThicknessEdit(HTREEITEM hTreeItem)
{
	ParkingLotTreeItem clickItem(GetTreeCtrl(),hTreeItem);
	ParkLotTreeItemData* pItemData = ParkingLotTreeItem(clickItem.GetParent()).GetLotItemData();
	if(!pItemData)
		return;
	CAirsideObjectTreeCtrl& m_treeProp = m_pParentDlg->GetTreeCtrl();
	LandsideParkingLot* pLot = (LandsideParkingLot*)m_pObject;
	LandsideParkingLotLevel *pLevel=pLot->GetLevel(pItemData->nLevelIndex);;
	if(!pLevel)
		return;


	double dUnitNum = ConvertLength(pLevel->m_dThickness);	

	m_treeProp.SetDisplayType( 2 );
	m_treeProp.SetSpinRange(0,10000);
	m_treeProp.SetDisplayNum(static_cast<int>(dUnitNum+0.5));	
	m_treeProp.SpinEditLabel( hTreeItem );
}
void LandParkingLotPropDlgImpl::OnSpotWidthEdit(HTREEITEM hTreeItem)
{
	ParkingLotTreeItem clickItem(GetTreeCtrl(),hTreeItem);
	ParkLotTreeItemData* pItemData = ParkingLotTreeItem(clickItem.GetParent()).GetLotItemData();
	if(!pItemData)
		return;
	

	CAirsideObjectTreeCtrl& m_treeProp = m_pParentDlg->GetTreeCtrl();
	LandsideParkingLot* pLot = (LandsideParkingLot*)m_pObject;
	LandsideParkingLotLevel *pLevel=pLot->GetLevel(pItemData->nLevelIndex);;
	if(!pLevel)
		return;


	//double dUnitNum = ConvertLength(pLevel->m_dThickness);	

	ParkingLotParkingSpaceList& spaceList = pLevel->m_parkspaces;
	ParkingSpace& space = spaceList.getSpace(pItemData->nOtherIndex);
	double dUnitNum = ConvertLength(space.m_dWidth);	

	m_treeProp.SetDisplayType( 2 );
	m_treeProp.SetSpinRange(0,10000);
	m_treeProp.SetDisplayNum(static_cast<int>(dUnitNum+0.5));	
	m_treeProp.SpinEditLabel( hTreeItem );
}

void LandParkingLotPropDlgImpl::OnSpotTypeCombo(HTREEITEM hItem)
{
	ParkingLotTreeItem clickItem(GetTreeCtrl(),hItem);
	ParkLotTreeItemData* pItemData = ParkingLotTreeItem(clickItem.GetParent()).GetLotItemData();
	if(!pItemData)
		return;

	CAirsideObjectTreeCtrl& m_treeProp = m_pParentDlg->GetTreeCtrl();
	LandsideParkingLot* pLot = (LandsideParkingLot*)m_pObject;
	LandsideParkingLotLevel *pLevel=pLot->GetLevel(pItemData->nLevelIndex);;
	if(!pLevel)
		return;


	double dUnitNum = ConvertLength(pLevel->m_dThickness);	

	ParkingLotParkingSpaceList& spaceList = pLevel->m_parkspaces;
	ParkingSpace& space = spaceList.getSpace(pItemData->nOtherIndex);

	std::vector<CString> vString;
	vString.push_back(ParkingSpace::TypeString(ParkingSpace::_perpendicular));
	vString.push_back(ParkingSpace::TypeString(ParkingSpace::_parallel));
	vString.push_back(ParkingSpace::TypeString(ParkingSpace::_angle));
	
	GetTreeCtrl().SetComboWidth(300);
	GetTreeCtrl().SetComboString(hItem,vString);
	GetTreeCtrl().m_comboBox.SetCurSel(space.m_type);
}

void LandParkingLotPropDlgImpl::OnSpotLengthEdit(HTREEITEM hItem)
{
	ParkingLotTreeItem clickItem(GetTreeCtrl(),hItem);
	ParkLotTreeItemData* pItemData = ParkingLotTreeItem(clickItem.GetParent()).GetLotItemData();
	if(!pItemData)
		return;
	CAirsideObjectTreeCtrl& m_treeProp = m_pParentDlg->GetTreeCtrl();
	LandsideParkingLot* pLot = (LandsideParkingLot*)m_pObject;
	LandsideParkingLotLevel *pLevel=pLot->GetLevel(pItemData->nLevelIndex);;
	if(!pLevel)
		return;


	//double dUnitNum = ConvertLength(pLevel->m_dThickness);	

	ParkingLotParkingSpaceList& spaceList = pLevel->m_parkspaces;
	ParkingSpace& space = spaceList.getSpace(pItemData->nOtherIndex);

	double dUnitNum = ConvertLength(space.m_dLength);	

	m_treeProp.SetDisplayType( 2 );
	m_treeProp.SetSpinRange(0,10000);
	m_treeProp.SetDisplayNum(static_cast<int>(dUnitNum+0.5));	
	m_treeProp.SpinEditLabel( hItem );

}

void LandParkingLotPropDlgImpl::OnSpotAngleEdit(HTREEITEM hItem)
{
	ParkingLotTreeItem clickItem(GetTreeCtrl(),hItem);
	ParkLotTreeItemData* pItemData = ParkingLotTreeItem(clickItem.GetParent()).GetLotItemData();
	if(!pItemData)
		return;
	CAirsideObjectTreeCtrl& m_treeProp = m_pParentDlg->GetTreeCtrl();
	LandsideParkingLot* pLot = (LandsideParkingLot*)m_pObject;
	LandsideParkingLotLevel *pLevel=pLot->GetLevel(pItemData->nLevelIndex);;
	if(!pLevel)
		return;


	//double dUnitNum = ConvertLength(pLevel->m_dThickness);	

	ParkingLotParkingSpaceList& spaceList = pLevel->m_parkspaces;
	ParkingSpace& space = spaceList.getSpace(pItemData->nOtherIndex);

	double dUnitNum = space.m_dAngle;
	m_treeProp.SetDisplayType( 2 );
	m_treeProp.SetSpinRange(-90,90);
	m_treeProp.SetDisplayNum(static_cast<int>(dUnitNum+0.5));	
	m_treeProp.SpinEditLabel( m_hRClickItem );

}

void LandParkingLotPropDlgImpl::OnSpotExitEntryPortEdit(HTREEITEM hItem)
{
	ParkingLotTreeItem clickItem(GetTreeCtrl(),hItem);
	ParkLotTreeItemData* pItemData = ParkingLotTreeItem(clickItem.GetParent()).GetLotItemData();
	if(!pItemData)
		return;
	CAirsideObjectTreeCtrl& m_treeProp = m_pParentDlg->GetTreeCtrl();
	LandsideParkingLot* pLot = (LandsideParkingLot*)m_pObject;
	LandsideParkingLotLevel *pLevel=pLot->GetLevel(pItemData->nLevelIndex);;
	if(!pLevel)
		return;

	ParkingLotEntryExitList& eeList = pLevel->m_entryexits;
	ParkLotEntryExit& exit_entry = eeList.getEnt(pItemData->nOtherIndex);

	if(exit_entry.m_nInOutPort < 1) exit_entry.m_nInOutPort = 1;
	m_treeProp.SetDisplayType( 2 );
	m_treeProp.SetSpinRange(1,10000);
	m_treeProp.SetDisplayNum(exit_entry.m_nInOutPort);	
	m_treeProp.SpinEditLabel( m_hRClickItem );

}

void LandParkingLotPropDlgImpl::OnDoubleClickPropTree( HTREEITEM hTreeItem )
{
	ParkingLotTreeItem clickItem(GetTreeCtrl(),hTreeItem);
	PLItemType itemType = clickItem.GetItemType();
	m_hRClickItem = hTreeItem;
	if (itemType==_BaseLevel || itemType==_OtherLevel)
	{	
		OnLevelNameEdit(hTreeItem);
	}
	if(itemType == _Height)
	{
		
		OnLevelHeightEdit(hTreeItem);
	}	
	if(itemType == _Area){

	}
	if(itemType == _Thickness)
	{		
		OnLevelThicknessEdit(hTreeItem);
	}
	
	if(itemType == _PPWidth)
	{
		
		OnDrivePathWidth(hTreeItem);
	}
	if (itemType == _EntExitPort)
	{
		
		OnSpotExitEntryPortEdit(hTreeItem);
	}
	if(itemType == _PSWidth)
	{

		OnSpotWidthEdit(hTreeItem);
	}
	if(itemType == _PSAngle)
	{
		
		OnSpotAngleEdit(hTreeItem);
	}
	if(itemType == _PSLength)
	{
	
		OnSpotLengthEdit(hTreeItem);
	}
	if(itemType == _PSType)
	{
		
		OnSpotTypeCombo(hTreeItem);
	}
	if(itemType == _PSOperation){
		
		OnSpotOpTypeCombo(hTreeItem);
	}
	if(itemType == _AreaVisible){
		OnLevelAreaVisible(hTreeItem);
	}
	if(itemType ==  _PPDirection){
		OnDrivePathDir(hTreeItem);
	}
	if(itemType == _PPLaneNum){
		OnDrivePathLaneNum(hTreeItem);
	}
}

void LandParkingLotPropDlgImpl::OnSelChangedPropTree(HTREEITEM hTreeItem)
{
	if (hTreeItem== NULL||m_bReloadTree)
	{
		return;
	}
	int nItemCount=GetTreeCtrl().GetCount();
	if (nItemCount<=0)
	{
		return;
	}
	ParkingLotTreeItem clickItem(GetTreeCtrl(),hTreeItem);

	ParkLotTreeItemData* pData = clickItem.GetLotItemData();
	if(!pData)
		return;

	PLItemType itemType = clickItem.GetItemType();
	LandsideParkingLot* pparkLot = (LandsideParkingLot*)m_pObject;
	LandsideParkingLotLevel *level= pparkLot->GetLevel(pData->nLevelIndex);
	if(!level)
		return;

	if(itemType == _Area )
	{		
		ParkingLotAreas& areas = level->m_areas;
		areas.getArea(pData->nOtherIndex).bSelected=true;
		//areas.RemoveArea(pData->nOtherIndex);			
	}
	if(itemType == _ParkSpace)
	{
		ParkingLotParkingSpaceList& parkspaces = level->m_parkspaces;
		parkspaces.getSpace(pData->nOtherIndex).m_bSelected=true;		
	}
	if (itemType == _ParkDrivePipe)
	{
		ParkingDrivePipeList& drivePipes = level->m_dirvepipes;
		drivePipes.getPipe(pData->nOtherIndex).m_bSelected=true;
	}
	if(itemType == _EntryExit)
	{
		ParkingLotEntryExitList& entlist = level->m_entryexits;
		entlist.getEnt(pData->nOtherIndex).m_bSelected=true;		
	}
	if(itemType==_LaneNode)
	{
		ParkingLotTreeItem parentItem = clickItem.GetParent();
		ParkLotTreeItemData* pParentData = parentItem.GetLotItemData();
		if(pParentData)
		{
			int nEntExitIdx = pParentData->nOtherIndex;
			ParkingLotEntryExitList& entlist = level->m_entryexits;
			ParkLotEntryExit& entexit = entlist.getEnt(nEntExitIdx);
			//entexit.removeNode(pData->nOtherIndex);			
		}
	}
	Update3D();
}

BOOL LandParkingLotPropDlgImpl::OnDefWindowProc( UINT message, WPARAM wParam, LPARAM lParam )
{

	if(!m_hRClickItem)
		return TRUE;

	LandsideParkingLot* pLot = (LandsideParkingLot*)m_pObject;



	if (message == WM_INPLACE_COMBO2)
	{	

		ParkingLotTreeItem clickItem(GetTreeCtrl(),m_hRClickItem);
		PLItemType itemType = clickItem.GetItemType();
		ParkLotTreeItemData *itemData=clickItem.GetLotItemData();

		LandsideParkingLotLevel *pLevel= pLot->GetLevel(itemData->nLevelIndex);
		if(!pLevel)
			return false;

		int selitem = GetTreeCtrl().m_comboBox.GetCurSel();	
		if (itemType == _Height)
		{
			pLevel->m_dHeight = selitem;
			clickItem.SetStringText(PARK_LEVEL_HEIGHT, GetFloorName(selitem));
		}
		if(itemType == _PSType)
		{
			ParkingLotParkingSpaceList& spaceList = pLevel->m_parkspaces;
			selitem = MAX(ParkingSpace::_perpendicular,selitem);
			selitem = MIN(ParkingSpace::_angle,selitem);
			int nSpotIndex = clickItem.GetOtherData();			
			ParkingSpace& space = spaceList.getSpace(nSpotIndex);
			space.m_type = (ParkingSpace::ParkType)selitem;
			clickItem.SetStringText(PARK_SPOT_TYPE,space.GetTypeString());				
		}
		if(itemType == _PSOperation){
			ParkingLotParkingSpaceList& spaceList = pLevel->m_parkspaces;
			selitem = MAX(ParkingSpace::_backup,selitem);
			selitem = MIN(ParkingSpace::_through,selitem);
			int nSpotIndex = clickItem.GetOtherData();			
			ParkingSpace& space = spaceList.getSpace(nSpotIndex);
			space.m_opType = (ParkingSpace::OperationType)selitem;
			clickItem.SetStringText(PARK_SPOT_OPERRATION,space.GetOpTypeString());
		}
		if(itemType == _AreaVisible){
			ParkingLotAreas& arealist= pLevel->m_areas;
			ParkingLotAreas::Area* pArea = arealist.getAreaPtr(clickItem.GetOtherData());
			if(pArea){
				pArea->bVisble = selitem>0?false:true;
				clickItem.SetString(pArea->bVisble?PARK_AREA_VISIBLE:PARK_AREA_INVISIBLE,true);
			}			
		}
		if(itemType == _PPDirection){
			ParkingDrivePipe* pdp = pLevel->m_dirvepipes.getPipePtr(clickItem.GetOtherData());
			if(pdp){
				pdp->m_nType = selitem>0?ParkingDrivePipe::_bidirectional:ParkingDrivePipe::_directional;
				clickItem.SetStringText(PARK_DRIVEPIPE_DIRECTION, pdp->getDirectionTypeStr());
			}
		}
		Update3D();

	}
	if( message == WM_INPLACE_SPIN )
	{
		ParkingLotTreeItem clickItem(GetTreeCtrl(),m_hRClickItem);
		PLItemType itemType = clickItem.GetItemType();
		ParkLotTreeItemData *itemData=clickItem.GetLotItemData();

		LandsideParkingLotLevel *pLevel= pLot->GetLevel(itemData->nLevelIndex);
		if(!pLevel)
			return false;

		ParkingLotParkingSpaceList& spaceList = pLevel->m_parkspaces;

		LPSPINTEXT pst = (LPSPINTEXT) lParam;
		int nOtherIndex = clickItem.GetOtherData();
		/*if (itemType == _Height)
		{
			double dValue = UnConvertLength(pst->iPercent);
            pLevel->m_dHeight=dValue;	
			clickItem.SetLengthValueText(PARK_LEVEL_HEIGHT,dValue,GetCurLengthUnit());
		}*/
		if (itemType == _Thickness)
		{
			double dValue = UnConvertLength(pst->iPercent);
			pLevel->m_dThickness=dValue;	
			clickItem.SetLengthValueText(PARK_LEVEL_THICKNESS,dValue,GetCurLengthUnit());
		}
		if(itemType == _PSWidth)
		{
			double dValue = UnConvertLength(pst->iPercent);
			ParkingSpace& space = spaceList.getSpace(nOtherIndex);
			space.m_dWidth = dValue;
			clickItem.SetLengthValueText(PARK_SPOT_WIDTH,dValue,GetCurLengthUnit());
			
		}
		if(itemType == _PSLength)
		{
			double dValue = UnConvertLength(pst->iPercent);
			ParkingSpace& space = spaceList.getSpace(nOtherIndex);
			space.m_dLength = dValue;
			clickItem.SetLengthValueText(PARK_SPOT_LENGTH,dValue,GetCurLengthUnit());
			
		}
		if(itemType == _PPPath)
		{
			ParkingDrivePipeList& pipeList = pLevel->m_dirvepipes;
			double dValue = UnConvertLength(pst->iPercent);
			ParkingDrivePipe& pipe = pipeList.getPipe(nOtherIndex);
			pipe.m_width = dValue;
			clickItem.SetLengthValueText(PARK_DRIVEPIPE_WIDTH,dValue,GetCurLengthUnit());

		}
		if (itemType == _EntExitPort)
		{
			ParkingLotEntryExitList& entryexitlist = pLevel->m_entryexits;
			ParkLotEntryExit& entryexit = entryexitlist.getEnt(nOtherIndex);
			if(pst->iPercent < 1)
				entryexit.m_nInOutPort = 1;
			entryexit.m_nInOutPort = pst->iPercent;
			clickItem.SetIntValueText(PARK_ENTRYEXIT_PORT,entryexit.m_nInOutPort);
		}
		if(itemType==_PSAngle)
		{
			double dValue = pst->iPercent;
			ParkingSpace& space = spaceList.getSpace(nOtherIndex);
			space.m_dAngle = dValue;
			clickItem.SetDegreeValueText(PARK_SPOT_ANGLE,dValue);
			
		}
		if(itemType ==_PPWidth){
			double dValue = UnConvertLength(pst->iPercent);
			ParkingDrivePipe* pdp = pLevel->m_dirvepipes.getPipePtr(nOtherIndex);
			if(pdp){
				pdp->m_width = dValue;
				clickItem.SetLengthValueText(PARK_DRIVEPIPE_WIDTH,dValue,GetCurLengthUnit());
			}
		}
		if(itemType == _PPLaneNum){
			double dValue = pst->iPercent;
			ParkingDrivePipe* pdp = pLevel->m_dirvepipes.getPipePtr(nOtherIndex);
			if(pdp){
				pdp->m_nLaneNum = (int)dValue;
				clickItem.SetIntValueText(PARK_DRIVEPIPE_LANNUM,pdp->m_nLaneNum);
			}
		}
		Update3D();

	}
	if (message == WM_COMMAND)
	{
		if(wParam == ID_PARKINGLOTLEVEL_SHOW){
			ParkingLotTreeItem clickItem(GetTreeCtrl(),m_hRClickItem);
			PLItemType itemType = clickItem.GetItemType();
			ParkLotTreeItemData *itemData=clickItem.GetLotItemData();

			LandsideParkingLotLevel *pLevel= pLot->GetLevel(itemData->nLevelIndex);
			if(!pLevel){
				return FALSE;
			}
			(pLevel->m_bVisible= !pLevel->m_bVisible);
			//Update3D();
		}
		if (wParam == ID_PARKINGLOTLEVEL_ADDNEWLEVEL)
		{
			LandsideParkingLot* pparkLot= getParkingLot();
			if(pparkLot)
			{
				LandsideParkingLotLevel* pLevel = pparkLot->AddNewLevel();
				//LoadTree();
				//LandsideParkingLotLevel &curParkingLevel=(LandsideParkingLotLevel)pparkLot->m_otherLevels.back();
				if(pLevel->m_name==_T(""))
				{
					pLevel->m_name=_T("New Level");
				}
				CString strLevelNum;
				strLevelNum.Format(_T("(%d)"),(int)pparkLot->GetLevelCount());
				HTREEITEM hOtherLevel = GetTreeCtrl().InsertItem(pLevel->m_name+strLevelNum);
				ParkingLotTreeItem otherLevelItem = ParkingLotTreeItem(GetTreeCtrl(),hOtherLevel);
				otherLevelItem.SetLotItemData(mDataInst->create(_OtherLevel,pparkLot->GetLevelCount()));
				AddLevelChildItems(otherLevelItem,*pLevel,(int)pparkLot->GetLevelCount()-1);
				otherLevelItem.Expand();
				GetTreeCtrl().SelectItem(hOtherLevel);
				Update3D();
			}
		}
		if (wParam == ID_PARKINGLOTLEVEL_COPYLEVEL)
		{
			ParkingLotTreeItem clickItem(GetTreeCtrl(),m_hRClickItem);
			PLItemType itemType = clickItem.GetItemType();
			ParkLotTreeItemData *itemData=clickItem.GetLotItemData();

			LandsideParkingLotLevel *pLevel= pLot->GetLevel(itemData->nLevelIndex);
			if(!pLevel){
				return FALSE;
			}
			LandsideParkingLot* pparkLot = getParkingLot();
			if (itemData && pparkLot)
			{
				LandsideParkingLotLevel* pNewLevel  = pparkLot->CopyLevel(*pLevel);
				LandsideParkingLotLevel &curParkingLevel= *pNewLevel;//(LandsideParkingLotLevel)pparkLot->m_otherLevels.back();
				if(curParkingLevel.m_name==_T(""))
				{
					curParkingLevel.m_name=_T("New Level");
				}
				CString strLevelNum;
				strLevelNum.Format(_T("(%d)"),(int)pparkLot->GetLevelCount());
				HTREEITEM hOtherLevel = GetTreeCtrl().InsertItem(curParkingLevel.m_name+strLevelNum);
				ParkingLotTreeItem otherLevelItem = ParkingLotTreeItem(GetTreeCtrl(),hOtherLevel);
				otherLevelItem.SetLotItemData(mDataInst->create(_OtherLevel,(int)pparkLot->GetLevelCount()-1));
				AddLevelChildItems(otherLevelItem,curParkingLevel,(int)pparkLot->GetLevelCount()-1);
				otherLevelItem.Expand();
				GetTreeCtrl().SelectItem(hOtherLevel);
				Update3D();
			}
		}
		if (wParam == ID_PARKINGLOTLEVEL_DELETELEVEL)
		{
			ParkingLotTreeItem clickItem(GetTreeCtrl(),m_hRClickItem);
			CAirsideObjectTreeCtrl& treeCtrl=GetTreeCtrl();
			PLItemType itemType = clickItem.GetItemType();
			ParkLotTreeItemData *itemData=clickItem.GetLotItemData();

			LandsideParkingLot* pNode = getParkingLot();
			if (itemData && pNode)
			{
				int nDelLevelIndex=itemData->nLevelIndex;
				if (nDelLevelIndex==-1)
				{
					MessageBox(NULL,_T("Can't delete base level."),_T("Error"),MB_OK);
					return false;
				}
				pNode->DeleteLevel(nDelLevelIndex);
				Update3D();
				HTREEITEM selItem=treeCtrl.GetPrevSiblingItem(clickItem.m_hItem);
				HTREEITEM tmpItem1,tmpItem2;
				tmpItem1=clickItem.m_hItem;
				while(tmpItem1)
				{
					tmpItem2=treeCtrl.GetNextItem(tmpItem1,TVGN_NEXT);
					treeCtrl.DeleteItem(tmpItem1);
					tmpItem1=tmpItem2;
				}
				for (int i=nDelLevelIndex;i<(int)pLot->GetLevelCount();i++)
				{
					LandsideParkingLotLevel &curParkingLevel= *pLot->GetLevel(i);
					if(curParkingLevel.m_name==_T(""))
					{
						curParkingLevel.m_name=_T("New Level");
					}
					CString strLevelNum;
					strLevelNum.Format(_T("(%d)"),i+1);
					HTREEITEM hOtherLevel = GetTreeCtrl().InsertItem(curParkingLevel.m_name+strLevelNum);
					ParkingLotTreeItem otherLevelItem = ParkingLotTreeItem(GetTreeCtrl(),hOtherLevel);
					otherLevelItem.SetLotItemData(mDataInst->create(_OtherLevel,i));
					AddLevelChildItems(otherLevelItem,curParkingLevel,i);
					otherLevelItem.Expand();			
				}
				if (treeCtrl.GetNextItem(selItem,TVGN_NEXT))
				{
					treeCtrl.SelectItem(treeCtrl.GetNextItem(selItem,TVGN_NEXT));
				}else
				{
					treeCtrl.SelectItem(selItem);
				}
				Update3D();
			}
		}
		if(wParam == ID_ALTOBJECTVERTICAL_EDITVERTICALPROFILE)
		{
			
		}
	}
	if (message == WM_INPLACE_EDIT)
	{
		CString strValue=*(CString *)lParam;

		ParkingLotTreeItem clickItem(GetTreeCtrl(),m_hRClickItem);
		PLItemType itemType = clickItem.GetItemType();
		ParkLotTreeItemData *itemData=clickItem.GetLotItemData();
		LandsideParkingLotLevel *pLevel= pLot->GetLevel(itemData->nLevelIndex);
		
		if(strValue!=_T("") && pLevel )
		{
			pLevel->m_name=strValue;
		}

		LoadTree();
		
	}
	return TRUE;
}

bool LandParkingLotPropDlgImpl::OnOK( CString& errorMsg )
{
	if(getCheckBoxCellPhone()->GetCheck() == BST_CHECKED)
	{
		if(getParkingLot())
			getParkingLot()->SetCellPhone(TRUE);
	}
	else
	{
		if(getParkingLot())
			getParkingLot()->SetCellPhone(FALSE);

	}

	return true;
}

LandsideParkingLot* LandParkingLotPropDlgImpl::getParkingLot()
{
	if(m_pObject->GetType()==ALT_LPARKINGLOT)
		return (LandsideParkingLot*)m_pObject;
	return NULL;
}

void LandParkingLotPropDlgImpl::GetFallBackReasonData( FallbackData& data )
{
	ParkingLotTreeItem clickItem(GetTreeCtrl(),m_hRClickItem);
	ParkLotTreeItemData* pData = clickItem.GetLotItemData();
	LandsideParkingLot* pparkLot = (LandsideParkingLot*)m_pObject;
	LandsideParkingLotLevel *pLevel = pparkLot->GetLevel(pData->nLevelIndex);
	if(!pLevel){
		return ;
	}

	if(pLevel)
	{
		data.dWorkHeight = pLevel->m_dHeight;
		data.bCustomHeight = true;
	}

}

void LandParkingLotPropDlgImpl::OnSpotOpTypeCombo( HTREEITEM hItem )
{
	ParkingLotTreeItem clickItem(GetTreeCtrl(),hItem);
	CAirsideObjectTreeCtrl& m_treeProp = m_pParentDlg->GetTreeCtrl();
	ParkLotTreeItemData* pItemData = ParkingLotTreeItem(clickItem.GetParent()).GetLotItemData();
	if(!pItemData)
		return;
	LandsideParkingLot* pLot = (LandsideParkingLot*)m_pObject;
	LandsideParkingLotLevel *pLevel = pLot->GetLevel(pItemData->nLevelIndex);
	if(!pLevel){
		return ;
	}

	double dUnitNum = ConvertLength(pLevel->m_dThickness);	

	ParkingLotParkingSpaceList& spaceList = pLevel->m_parkspaces;
	ParkingSpace& space = spaceList.getSpace(pItemData->nOtherIndex);

	std::vector<CString> vString;
	vString.push_back(ParkingSpace::OpTypeString(ParkingSpace::_backup));
	vString.push_back(ParkingSpace::OpTypeString(ParkingSpace::_through));
	

	GetTreeCtrl().SetComboWidth(300);
	GetTreeCtrl().SetComboString(hItem,vString);
	GetTreeCtrl().m_comboBox.SetCurSel(space.m_type);
}

void LandParkingLotPropDlgImpl::SelectParkingLotItem( HTREEITEM hItem,int nLevel,emLandsideItemType parkLotItemType,int nIndex )
{
	//if (m_pData == NULL)
	//	return;
	
	int nParkingLotLevel = 0;
	if(!m_pData.GetData(KeyLevelIndex,(int&)nParkingLotLevel) )
		return;

	if (nLevel != nParkingLotLevel)
		return;
	
	//check whether belong to parking lot
	emLandsideItemType curItemType;
	if(!m_pData.GetData(KeyParkLotItemType,(int&)curItemType))
		return;

	if (parkLotItemType != curItemType)
		return;

	int nParkingLotItemIdx = 0;
	if(!m_pData.GetData(KeyControlPathIndex,(int&)nParkingLotItemIdx))
		return;

	if (nParkingLotItemIdx != nIndex)
		return;
	
	m_vSelectedItem.push_back(hItem);
}

bool LandParkingLotPropDlgImpl::ChangeFocus()const
{
	//if (m_pData)
	//	return true;
	
	return false;
}

void LandParkingLotPropDlgImpl::HandleChangeVerticalProfile()
{
	//Find current change vertical profile related to drive pipe
	int nCurrentLevel = -1;
	if (!GetCurrentLevel(nCurrentLevel))
		return;
	
	ParkingDrivePipe* pParkingDrivePipe = GetParkingDrivePipe();
	if (pParkingDrivePipe == NULL)
		return;

	//nCurrentLevel++;
	LandsideParkingLot* pParkingLot = getParkingLot();
	if (pParkingLot == NULL)
		return;
	
	// get every level's real height
	int nLevelCount = (int)(pParkingLot->GetLevelCount());
	if (nLevelCount==0)
	{
		::MessageBox(NULL,_T("You should define more one parking lot level to use this function"),_T("Warning"),MB_OK|MB_ICONINFORMATION);
		return;
	}
	std::vector<double>vFloorAltitudes;
	for(int i=0; i<nLevelCount; i++) 
	{			
		vFloorAltitudes.push_back( i );				
	}

	

	//get x (distance from point 0) and z
	std::vector<double>vXPos;
	std::vector<double>vZPos;
	Path _tmpPath;

	int nCount = pParkingDrivePipe->m_ctrlPath.getCount();
	if( pParkingDrivePipe->m_ctrlPath.getCount() > 0 )
	{
		vXPos.reserve(nCount);
		vZPos.reserve(nCount);
		vXPos.push_back(0.0);

		double dLevel = nCurrentLevel + pParkingDrivePipe->m_ctrlPath.getPoint(0).getZ();
		vZPos.push_back( dLevel );
		
		double dLenSoFar = 0.0;
		for( int i=1; i<nCount; i++ )
		{
			ARCVector3 v3D = pParkingDrivePipe->m_ctrlPath.getPoint(i)- pParkingDrivePipe->m_ctrlPath.getPoint(i-1);	
			v3D.z = (0.0);
			dLenSoFar += UNITSMANAGER->ConvertLength(v3D.Length());
			vXPos.push_back(dLenSoFar);
			double dLevel = nCurrentLevel + pParkingDrivePipe->m_ctrlPath.getPoint(i).getZ();
			vZPos.push_back( dLevel);
		}			

		// init temp path
		_tmpPath.init( nCount );
		for( i=0; i< nCount; i++ )
		{
			CPoint2008 v3D =  pParkingDrivePipe->m_ctrlPath.getPoint(i);
			_tmpPath[i] = Point( v3D.getX(), v3D.getY(), v3D.getZ() );
		}
	}


	CStringArray strArrFloorName;
	strArrFloorName.SetSize(nLevelCount);
	//strArrFloorName[0] = pParkingLot->m_baselevel.m_name;
	for(int k=0;k<nLevelCount;k++)
	{
		strArrFloorName[k]= pParkingLot->GetLevel(k)->m_name;
	}
	// call dialog to define z pos
	CChangeVerticalProfileDlg dlg(vXPos, vZPos, vFloorAltitudes, _tmpPath ,&strArrFloorName);

	if( dlg.DoModal() == IDOK )
	{
		ASSERT( vXPos.size() == vZPos.size() );
		vZPos = dlg.m_wndPtLineChart.m_vPointYPos;
		// convert z value from real height to the format which is same to the log's z format
		for( i=0; i<static_cast<int>(vZPos.size()); ++i )
		{
			vZPos[ i ] = vZPos[ i ] - nCurrentLevel;	
		}

		// update z value
		if( nCount > 0 )
		{
			for( int i=0; i<static_cast<int>(vZPos.size()); i++ )
			{
				CPoint2008& v3D = pParkingDrivePipe->m_ctrlPath[ i ];
				v3D.setZ(vZPos[ i ]);
			}			
		}
	
		/////////////////////end set z value

		LoadTree();
	}
}


bool LandParkingLotPropDlgImpl::NeedChangeVerticalProfile() 
{
	ParkingLotTreeItem clickItem(GetTreeCtrl(),m_hRClickItem);
	if (m_hRClickItem==NULL)
	{
		return false;
	}
	PLItemType itemType = clickItem.GetItemType();

	if(  itemType != _ParkDrivePipe)
	{
		return false;
	}
	
	return true;
}

bool LandParkingLotPropDlgImpl::GetCurrentLevel(int& nCurrentLevel)
{
	LandsideParkingLot* pparkLot = (LandsideParkingLot*)m_pObject;
	if(!m_hRClickItem)
		return false;
	ParkingLotTreeItem parkingLotItem(GetTreeCtrl(),m_hRClickItem);
	ParkLotTreeItemData* pData = parkingLotItem.GetLotItemData();
	if(!pData)
		return false;

	nCurrentLevel = pData->nLevelIndex;
	return true;
}

ParkingDrivePipe* LandParkingLotPropDlgImpl::GetParkingDrivePipe()
{
	if(!m_hRClickItem)
		return NULL;
 	ParkingLotTreeItem clickItem(GetTreeCtrl(),m_hRClickItem);
 
	int nCurrentLevel = -1;
	if (!GetCurrentLevel(nCurrentLevel))
		return NULL;

 	PLItemType itemType = clickItem.GetItemType();
 	ParkLotTreeItemData* pData = clickItem.GetLotItemData();
 	if(!pData)
 		return NULL;
 
 	if (itemType != _ParkDrivePipe)
 		return NULL;
	
	LandsideParkingLot* pParkingLot = (LandsideParkingLot*)m_pObject;
	LandsideParkingLotLevel* pCurLevel = pParkingLot->GetLevel(nCurrentLevel);
	if (pCurLevel == NULL)
		return NULL;
	
	ParkingDrivePipeList& drivePipes = pCurLevel->m_dirvepipes;
	
	HTREEITEM tmpItem=GetTreeCtrl().GetNextItem(m_hRClickItem,TVGN_NEXT);
	int index = clickItem.GetOtherData();
	
	return &drivePipes.getPipe(index);
}	



void LandParkingLotPropDlgImpl::OnLevelAreaVisible( HTREEITEM hItem )
{
	ParkingLotTreeItem clickItem(GetTreeCtrl(),hItem);
	ParkLotTreeItemData* pItemData = ParkingLotTreeItem(clickItem.GetParent()).GetLotItemData();
	if(!pItemData)
		return;
	LandsideParkingLot* pLot = (LandsideParkingLot*)m_pObject;
	LandsideParkingLotLevel *pLevel= pLot->GetLevel(pItemData->nLevelIndex);
	if(!pLevel)
		return;

	ParkingLotAreas::Area* area =  pLevel->m_areas.getAreaPtr(pItemData->nOtherIndex);
	if(area){
		std::vector<CString> vString;
		vString.push_back(PARK_AREA_VISIBLE);
		vString.push_back(PARK_AREA_INVISIBLE);
		GetTreeCtrl().SetComboWidth(300);
		GetTreeCtrl().SetComboString(hItem,vString);
		GetTreeCtrl().m_comboBox.SetCurSel(area->bVisble?0:1);
	}

}

void LandParkingLotPropDlgImpl::OnDrivePathLaneNum( HTREEITEM hItem )
{
	ParkingLotTreeItem clickItem(GetTreeCtrl(),hItem);
	ParkLotTreeItemData* pItemData = ParkingLotTreeItem(clickItem.GetParent()).GetLotItemData();
	if(!pItemData)
		return;
	LandsideParkingLot* pLot = (LandsideParkingLot*)m_pObject;
	LandsideParkingLotLevel *pLevel= pLot->GetLevel(pItemData->nLevelIndex);
	if(!pLevel)
		return;

	ParkingDrivePipe* pdp = pLevel->m_dirvepipes.getPipePtr(pItemData->nOtherIndex);
	if(!pdp)
		return;

	GetTreeCtrl().SetDisplayType( 2 );
	GetTreeCtrl().SetSpinRange(1,10000);
	GetTreeCtrl().SetDisplayNum(pdp->m_nLaneNum);	
	GetTreeCtrl().SpinEditLabel( hItem );

}

void LandParkingLotPropDlgImpl::OnDrivePathDir( HTREEITEM hItem )
{

	ParkingLotTreeItem clickItem(GetTreeCtrl(),hItem);
	ParkLotTreeItemData* pItemData = ParkingLotTreeItem(clickItem.GetParent()).GetLotItemData();
	if(!pItemData)
		return;
	LandsideParkingLot* pLot = (LandsideParkingLot*)m_pObject;
	LandsideParkingLotLevel *pLevel= pLot->GetLevel(pItemData->nLevelIndex);
	if(!pLevel)
		return;

	ParkingDrivePipe* pdp = pLevel->m_dirvepipes.getPipePtr(pItemData->nOtherIndex);
	if(!pdp)
		return;

	std::vector<CString> vString;
	vString.push_back(ParkingDrivePipe::getDirectionTypeStr(ParkingDrivePipe::_directional));
	vString.push_back(ParkingDrivePipe::getDirectionTypeStr(ParkingDrivePipe::_bidirectional));
	GetTreeCtrl().SetComboWidth(300);
	GetTreeCtrl().SetComboString(hItem,vString);
	GetTreeCtrl().m_comboBox.SetCurSel(pdp->m_nType);
}

void LandParkingLotPropDlgImpl::OnDrivePathWidth( HTREEITEM hItem )
{
	ParkingLotTreeItem clickItem(GetTreeCtrl(),hItem);
	ParkLotTreeItemData* pItemData = ParkingLotTreeItem(clickItem.GetParent()).GetLotItemData();
	if(!pItemData)
		return;
	LandsideParkingLot* pLot = (LandsideParkingLot*)m_pObject;
	LandsideParkingLotLevel *pLevel= pLot->GetLevel(pItemData->nLevelIndex);
	if(!pLevel)
		return;

	ParkingDrivePipe* pdp = pLevel->m_dirvepipes.getPipePtr(pItemData->nOtherIndex);
	if(!pdp)
		return;

	double dUnitNum = ConvertLength(pdp->m_width);	

	GetTreeCtrl().SetDisplayType( 2 );
	GetTreeCtrl().SetSpinRange(1,10000);
	GetTreeCtrl().SetDisplayNum(int(dUnitNum+.5));	
	GetTreeCtrl().SpinEditLabel( hItem );
}

CString LandParkingLotPropDlgImpl::GetFloorName( int idx ) const
{
	CFloors& termFloors  = m_pParentDlg->GetDocument()->GetFloorByMode(EnvMode_LandSide);
	CRenderFloor* pFloor = termFloors.GetFloor( idx );
	CString sfloorName = "Undefined Floor";
	if(pFloor){ sfloorName = pFloor->FloorName();  }
	return sfloorName;
}


