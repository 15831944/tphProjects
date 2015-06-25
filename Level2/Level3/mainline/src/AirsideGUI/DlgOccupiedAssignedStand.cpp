// DlgOccupiedAssignedStand.cpp : implementation file
//

#include "stdafx.h"
//#include "AirsideGUI.h"
#include "Resource.h"
// #include "DlgOccupiedAssignedStand.h"
#include "..\InputAirside\InputAirside.h"
#include "..\Common\AirportDatabase.h"
#include ".\dlgoccupiedassignedstand.h"
#include "..\InputAirside\Taxiway.h"
#include "..\InputAirside\stand.h"
#include "..\InputAirside\ALTAirport.h"
#include "..\InputAirside\ALTObjectGroup.h"

#include "DlgOccupiedAssignedStandSelectIntersection.h"

#include "DlgRunwayExitSelect.h"
#include "DlgParkingStandSelect.h"
#include "InputAirside/AirportGroundNetwork.h"
#include "DlgTimeRange.h"
#include "DlgParkingStandList.h"
#include "DlgTemporaryParkingList.h"


static const UINT ID_NEW_FLTTYPE = 10;
static const UINT ID_DEL_FLTTYPE = 11;
static const UINT ID_EDIT_FLTTYPE = 12;

static const UINT ID_EDIT_PRIORITY = 20;
static const UINT ID_UP_PRIORITY = 21;
static const UINT ID_DOWN_PRIORITY = 22;

IMPLEMENT_DYNAMIC(CDlgOccupiedAssignedStand, CXTResizeDialog)
CDlgOccupiedAssignedStand::CDlgOccupiedAssignedStand(int nProjID, PFuncSelectFlightType pSelectFlightType, InputAirside* pInputAirside, CWnd* pParent /*=NULL*/)
	: CXTResizeDialog(CDlgOccupiedAssignedStand::IDD, pParent)
	, m_nProjID(nProjID)
	, m_pSelectFlightType(pSelectFlightType)
	,m_bWindowClose(false)
{
	
	m_pAirportDB = pInputAirside->m_pAirportDB;
	m_OccupiedAssignedStandCriteria.SetAirportDB(m_pAirportDB);
	m_OccupiedAssignedStandCriteria.SetPrjID(nProjID);
	m_pAltNetwork = new CAirportGroundNetwork(nProjID);

	std::vector<int> vAirportIds;
	InputAirside::GetAirportList(m_nProjID, vAirportIds);
	for (std::vector<int>::iterator iterAirportID = vAirportIds.begin(); iterAirportID != vAirportIds.end(); ++iterAirportID)
	{		
		std::vector<int> vTaxiwaysIDs;
		ALTAirport::GetTaxiwaysIDs(*iterAirportID, vTaxiwaysIDs);
		std::vector<int>::iterator iterTaxiwayID = vTaxiwaysIDs.begin();
		for (; iterTaxiwayID != vTaxiwaysIDs.end(); ++iterTaxiwayID)
		{
			Taxiway taxiway;
			taxiway.ReadObject(*iterTaxiwayID);
			m_vectTaxiway.push_back(taxiway);
		}
	}


}

CDlgOccupiedAssignedStand::~CDlgOccupiedAssignedStand()
{
 	delete m_pAltNetwork;
}
BEGIN_MESSAGE_MAP(CDlgOccupiedAssignedStand, CXTResizeDialog)
	ON_WM_CREATE()
	ON_WM_CONTEXTMENU()
	ON_WM_SIZE()	
	ON_COMMAND(ID_TOOLBARBUTTONADD,OnNewExits)
	ON_COMMAND(ID_ADD_PARKINGSTANDS,OnNewParkingStands)
	ON_COMMAND(ID_ADD_FLIGHTTYPE,OnNewFlightType)
	ON_COMMAND(ID_ADD_TIMEWINDOW,OnNewTimeWindow)
	ON_COMMAND(ID_TOOLBARBUTTONUP,OnStrategyUp)
	ON_COMMAND(ID_TOOLBARBUTTONDOWN,OnStrategyDown)
	ON_COMMAND(ID_TOOLBARBUTTONDEL,OnDelButton)
	ON_NOTIFY(TVN_SELCHANGED,IDC_TREE_CRITERIA,OnTvnSelchangedTree)
	ON_BN_CLICKED(IDSAVE, OnBnClickedSave)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
END_MESSAGE_MAP()
void CDlgOccupiedAssignedStand::DoDataExchange(CDataExchange* pDX)
{
	CXTResizeDialog::DoDataExchange(pDX);
	DDX_Control(pDX,IDC_TREE_CRITERIA,m_TreeCriteria);
	DDX_Control(pDX,IDSAVE,btSave);
	DDX_Control(pDX,IDOK,btOk);
	DDX_Control(pDX,IDCANCEL,btCancel);
}

LRESULT CDlgOccupiedAssignedStand::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	if (m_bWindowClose)
	{
		return CXTResizeDialog::DefWindowProc(message, wParam, lParam);
	}
	HTREEITEM selctItem=NULL;
	selctItem=m_TreeCriteria.GetSelectedItem();
	if (selctItem==NULL)
	{
		return CXTResizeDialog::DefWindowProc(message, wParam, lParam);
	}
	int nLevelNum=getSelItemLevel();
	if (nLevelNum<LEVEL_EXIT||nLevelNum>LEVEL_STRATEGYDETAIL)
	{
		return CXTResizeDialog::DefWindowProc(message, wParam, lParam);
	}
 	CriteriaLevel nLevel=(CriteriaLevel)nLevelNum;
	switch(message)
	{
	case UM_CEW_SHOW_DIALOGBOX_BEGIN:
		{				
			switch(nLevel)
			{
			case LEVEL_EXIT:
				{
					CDlgRunwayExitSelect dlg(CDlgRunwayExitSelect::IDD,m_nProjID,  _T(""));
					if(dlg.DoModal()!=IDOK)
					{
						return CXTResizeDialog::DefWindowProc(message, wParam, lParam);
					}
					CString strRunwayExitName = dlg.GetRunwayExitName();
					std::vector<int> vExits = dlg.GetRunwayExitList();					
					COOLTREE_NODE_INFO *cni=m_TreeCriteria.GetItemNodeInfo(selctItem);
 					m_TreeCriteria.SetItemText(selctItem,"EXITS: "+strRunwayExitName);
					m_TreeCriteria.SetItemNodeInfo(selctItem,*cni);
					CAirSideCriteriaExits *selExits=(CAirSideCriteriaExits *)m_TreeCriteria.GetItemData(selctItem);
					selExits->setExitList(vExits);	
					break;				
				}
			case LEVEL_PARKINGSTAND:
				{
					CDlgParkingStandSelect dlg(CDlgParkingStandSelect::IDD,m_nProjID,_T(""));
					if(IDOK != dlg.DoModal())
					{
						return CXTResizeDialog::DefWindowProc(message, wParam, lParam);
					}
					CString strParkingStandName=dlg.GetStandFamilyName();
					std::vector<int> vStands=dlg.GetStandGroupList();

					COOLTREE_NODE_INFO *cni=m_TreeCriteria.GetItemNodeInfo(selctItem);	
					m_TreeCriteria.SetItemText(selctItem,"PARKING STANDS: "+strParkingStandName);
					m_TreeCriteria.SetItemNodeInfo(selctItem,*cni);

					CAirSideCriteriaParkingStands *selParkingStand=(CAirSideCriteriaParkingStands *)m_TreeCriteria.GetItemData(selctItem);
					selParkingStand->setParkingStandList(vStands);
					break;
				}
			case LEVEL_FLIGHTTYPE:
				{
					FlightConstraint fltType;
					if( !(*m_pSelectFlightType)(NULL,fltType) )
						return CXTResizeDialog::DefWindowProc(message, wParam, lParam);
					CString flightName;
					fltType.screenPrint(flightName);
					COOLTREE_NODE_INFO *cni=m_TreeCriteria.GetItemNodeInfo(selctItem);	
					m_TreeCriteria.SetItemText(selctItem,"FLIGHT TYPE: "+flightName);
					m_TreeCriteria.SetItemNodeInfo(selctItem,*cni);
					CAirSideCriteriaFlightType *flightTypeItem=(CAirSideCriteriaFlightType *)m_TreeCriteria.GetItemData(selctItem);
					flightTypeItem->setFlightType(fltType);					
					break;
				}
			case LEVEL_TIMEWINDOW:
				{
					ElapsedTime estFromTime;
					ElapsedTime estToTime;
					CAirSideCreteriaTimeWin *curTimeWindow=(CAirSideCreteriaTimeWin *)m_TreeCriteria.GetItemData(selctItem);
					estFromTime=curTimeWindow->GetStartTime();
					estToTime=curTimeWindow->GetEndTime();

					CDlgTimeRange dlg(estFromTime, estToTime);

					if(IDOK!= dlg.DoModal())
						return CXTResizeDialog::DefWindowProc(message, wParam, lParam);

					CString strTimeRange;
					strTimeRange.Format("TIME WINDOW: %s - %s", dlg.GetStartTimeString(), dlg.GetEndTimeString());

					estFromTime = dlg.GetStartTime();
					estToTime = dlg.GetEndTime();

					COOLTREE_NODE_INFO *cni=m_TreeCriteria.GetItemNodeInfo(selctItem);	
					m_TreeCriteria.SetItemText(selctItem,strTimeRange);
					m_TreeCriteria.SetItemNodeInfo(selctItem,*cni);
					
					curTimeWindow->SetEndTime(estToTime);
					curTimeWindow->SetStartTime(estFromTime);
					break;
				}
			case LEVEL_STRATEGYDETAIL:
				{
					HTREEITEM strategyItem=m_TreeCriteria.GetParentItem(selctItem);
					COccupiedAssignedStandStrategy *curStrategy=(COccupiedAssignedStandStrategy *)m_TreeCriteria.GetItemData(strategyItem);


					StrategyType strategyType=(StrategyType)m_TreeCriteria.GetItemData(selctItem);
					switch(strategyType)
					{					

					case ToIntersection:
						{
							CDlgOccupiedSelectIntersection dlgIntersection(m_pAltNetwork, m_vectTaxiway, m_pTaxiwayVectorMap,curStrategy->GetIntersectionID());
							if(IDOK != dlgIntersection.DoModal())
								return CXTResizeDialog::DefWindowProc(message, wParam, lParam);;

							int nIntersectionID = dlgIntersection.GetIntersectionID();
							CString strIntersection = dlgIntersection.GetIntersectionName();	

							strIntersection.Format(_T("%d: Stop short of intersection [") + strIntersection + _T("] till assigned stand free"),getItemIndex(selctItem)+1) ;
							m_TreeCriteria.SetItemText(selctItem, strIntersection);							
							curStrategy->SetIntersectionID(nIntersectionID);							
							break;
						}	
					case ToTemporaryParking:
						{
							CDlgTemporaryParkingList dlg(CDlgParkingStandList::IDD,m_nProjID,curStrategy->GetTmpParkingList(),&m_TemporaryParkingVectorMap);
							if(IDOK != dlg.DoModal())
								return CXTResizeDialog::DefWindowProc(message, wParam, lParam);;
							ObjIDList &parkingList=dlg.GetParkingList();
							curStrategy->SetTmpParkingList(parkingList);						

							CString strDetail,strStrategy;
							if ((int)parkingList.size()==0)
							{								
								strDetail = _T("Please Edit");
								strDetail = _T("Go to temporary parking in order [") + strDetail + _T("]");												

							}else
							{
								strDetail=_T("");
								ObjIDList::iterator tmpParkingIter=parkingList.begin();

								AltObjectVectorMapIter iter;
								for(;tmpParkingIter!=parkingList.end();tmpParkingIter++)
								{
									int nFlag = 0;
									iter = m_TemporaryParkingVectorMap.begin();
									for(; iter != m_TemporaryParkingVectorMap.end(); iter++)
									{
										AltObjectVector& vec = iter->second;
										for(AltObjectVectorIter it = vec.begin();it != vec.end(); it++)
										{
											if(it->second == (int)(*tmpParkingIter))
											{
												strDetail =strDetail+_T("[")+ it->first+_T("]");
												nFlag = 1;
												break;
											}
										}
										if(nFlag == 1)
											break;
									}
								}
								strDetail = _T("Go to temporary parking in order ") + strDetail ;
							}
							strStrategy.Format(_T("%d: %s"),getItemIndex(selctItem)+1,strDetail);
							COOLTREE_NODE_INFO *cni=m_TreeCriteria.GetItemNodeInfo(selctItem);
							m_TreeCriteria.SetItemText(selctItem,strStrategy);
							m_TreeCriteria.SetItemNodeInfo(selctItem,*cni);			
							break;
						}
					case ToStand:
						{
							CDlgParkingStandList dlg(CDlgParkingStandList::IDD,m_nProjID,curStrategy->GetTmpStandList());
							if(IDOK != dlg.DoModal())
								return CXTResizeDialog::DefWindowProc(message, wParam, lParam);;
							ObjIDList &standList=dlg.GetStandList();
							curStrategy->SetTmpStandList(standList);							


							CString strDetail,strStrategy;
							if ((int)standList.size()==0)
							{								
								strDetail = _T("Please Edit");
								
							}else
							{
								strDetail=_T("");
								ObjIDList::iterator standIter=standList.begin();
								for(;standIter!=standList.end();standIter++)
								{
									int nStandID=(int)(*standIter);

									ALTObjectGroup altObjGroup;	
									altObjGroup.ReadData(nStandID);
									strDetail= strDetail+_T("[")+altObjGroup.getName().GetIDString()+_T("]");	
								}
															
							}
							strDetail = _T("Go to Stand in priority till assigned stand free.[") + strDetail + _T("]");
							strStrategy.Format(_T("%d: %s"),getItemIndex(selctItem)+1,strDetail);

							COOLTREE_NODE_INFO *cni=m_TreeCriteria.GetItemNodeInfo(selctItem);
							m_TreeCriteria.SetItemText(selctItem,strStrategy);
							m_TreeCriteria.SetItemNodeInfo(selctItem,*cni);

							break;
						}
					default:
						{
							break;
						}

					}

					break;
				}

			default:
				{
					break;
				}
			}
			break;
			
		}
	case UM_CEW_EDITSPIN_END:
	case UM_CEW_EDIT_END:
		{	

			StrategyType strategyType=(StrategyType)m_TreeCriteria.GetItemData(selctItem);
			if (strategyType!=Delaytime||nLevel!=LEVEL_STRATEGYDETAIL)
			{
				return CXTResizeDialog::DefWindowProc(message, wParam, lParam);
			}
			HTREEITEM strategyItem=m_TreeCriteria.GetParentItem(selctItem);
			COccupiedAssignedStandStrategy *curStrategy=(COccupiedAssignedStandStrategy *)m_TreeCriteria.GetItemData(strategyItem);

			CString strValue = *((CString*)lParam);
			CString strStrategy,strDetail;
			int delayTime;
			if( strValue == "" )
			{
				delayTime=10;
			}else
			{
				delayTime= atoi( strValue );
			}			
			curStrategy->SetDelaytime(delayTime);
			strDetail.Format(_T("Slow down to arrive [%d] mins later"),delayTime);
			strStrategy.Format(_T("%d: %s"),getItemIndex(selctItem)+1,strDetail);
			COOLTREE_NODE_INFO *cni=m_TreeCriteria.GetItemNodeInfo(selctItem);
			m_TreeCriteria.SetItemText(selctItem,strStrategy);
			cni->fMinValue=(float)delayTime;
			break;		
			
		}
	}
	return CXTResizeDialog::DefWindowProc(message, wParam, lParam);

}



// CDlgOccupiedAssignedStand message handlers

int CDlgOccupiedAssignedStand::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CXTResizeDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here

	if(!m_toolbarCriteria.CreateEx(this, TBSTYLE_FLAT,WS_CHILD|WS_VISIBLE|CBRS_ALIGN_TOP)
		||!m_toolbarCriteria.LoadToolBar(IDR_TOOLBAR_ASSIGNEDSTAND_CRITERIA))
	{
		return -1;
	}



	return 0;
}

void CDlgOccupiedAssignedStand::InitToolBar()
{
	CRect rcToolbar;

	// set the position of the fltType toolbar	
	m_TreeCriteria.GetWindowRect(&rcToolbar);
	ScreenToClient(&rcToolbar);
	rcToolbar.top -= 26;
	rcToolbar.bottom = rcToolbar.top + 22;
	m_toolbarCriteria.MoveWindow(rcToolbar);
	m_toolbarCriteria.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONADD);	
	m_toolbarCriteria.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONDEL,false);			
	m_toolbarCriteria.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONEDIT,false);
	m_toolbarCriteria.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONUP,false);
	m_toolbarCriteria.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONDOWN,false);

}
void CDlgOccupiedAssignedStand::ReloadTree()
{

	m_TreeCriteria.DeleteAllItems();
	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);
	cni.nt=NT_NORMAL; 
	ExitsList &exitsList=m_OccupiedAssignedStandCriteria.getExitsList();
	ExitsList::iterator exitsIter;
	CString strItem;
	for (exitsIter=exitsList.begin();exitsIter!=exitsList.end();exitsIter++)
	{
		CAirSideCriteriaExits *curExits=(CAirSideCriteriaExits *)(*exitsIter);
		addExitsItem(curExits);
	}
}
HTREEITEM CDlgOccupiedAssignedStand::addExitsItem(CAirSideCriteriaExits *exits)
{

	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);
	cni.net=NET_SHOW_DIALOGBOX;

	CString strItem="";

	if (exits->IsAllRunwayExit())
	{
		strItem="ALL";

	}else
	{				
		ObjIDList exitList=exits->getExitList();
		for (int j =0; j < (int)exitList.size(); j++)
		{
			int nExitID = exitList.at(j);
			RunwayExit rwyExit;
			rwyExit.ReadData(nExitID);
			if (strItem == "")
			{
				strItem = rwyExit.GetName();
			}
			else
			{
				strItem +=", " + rwyExit.GetName();
			}
		}			
	}

	strItem="EXITS: "+strItem;
	HTREEITEM hCriteria=m_TreeCriteria.InsertItem(strItem,cni,FALSE);
	m_TreeCriteria.SetItemData(hCriteria,(DWORD)exits);		

	ParkingStandsList &parkingStandsList=exits->getParkingStandsList();			

	ParkingStandsList::iterator parkingStandsIter;

	for (parkingStandsIter=parkingStandsList.begin();parkingStandsIter!=parkingStandsList.end();parkingStandsIter++)
	{
		CAirSideCriteriaParkingStands *curParkingStands=(CAirSideCriteriaParkingStands *)(*parkingStandsIter);
		addParkingStandsItem(hCriteria,curParkingStands);
	}
	m_TreeCriteria.Expand(hCriteria,TVE_EXPAND);
	m_TreeCriteria.SelectItem(hCriteria);
	return hCriteria;
}
HTREEITEM CDlgOccupiedAssignedStand::addParkingStandsItem(HTREEITEM parentItem,CAirSideCriteriaParkingStands *parkingStands)
{

	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);
	cni.net = NET_SHOW_DIALOGBOX;

	CString strItem="";

	FlightTypeList &flightTypeList=parkingStands->getFlightTypeList();

	if (parkingStands->isAllParkingStand())
	{				
		strItem="ALL";

	}else
	{				
		ObjIDList vStandFmailys=parkingStands->getParkingStandList();
		int nSize = vStandFmailys.size();
		for (int j =0; j < nSize; j++)
		{
			int nStandFamilyID = vStandFmailys.at(j);
			ALTObjectGroup altObjGroup;
			altObjGroup.ReadData(nStandFamilyID);
			ALTObjectID altObjID = altObjGroup.getName();
			if (strItem == "")
			{
				strItem = altObjID.GetIDString();
			}
			else
			{
				strItem += ", "+altObjID.GetIDString();
			}
		}								
	}

	strItem="PARKING STANDS: "+strItem;
	HTREEITEM hParkingStands=m_TreeCriteria.InsertItem(strItem,cni,FALSE,FALSE,parentItem);
	m_TreeCriteria.SetItemData(hParkingStands,(DWORD)parkingStands);

	FlightTypeList::iterator flightTypeIter;
	for (flightTypeIter=flightTypeList.begin();flightTypeIter!=flightTypeList.end();flightTypeIter++)
	{								
		CAirSideCriteriaFlightType *curFlightType=(CAirSideCriteriaFlightType *)(*flightTypeIter);
		addFlightTypeItem(hParkingStands,curFlightType);
	}
	m_TreeCriteria.Expand(hParkingStands,TVE_EXPAND);
	m_TreeCriteria.SelectItem(hParkingStands);
	return hParkingStands;
}
HTREEITEM CDlgOccupiedAssignedStand::addFlightTypeItem(HTREEITEM parentItem,CAirSideCriteriaFlightType *flightType)
{
	CString strItem="";

	FlightConstraint &flightConstraint=flightType->getFlightType();

	TimeWindowList &timeWinList=flightType->getTimeWindowList();

	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);
	cni.net = NET_SHOW_DIALOGBOX;

	if (flightConstraint.isDefault())
	{					
		strItem="DEFAULT";
	}else
	{
		flightConstraint.screenPrint(strItem);					
	}
	strItem="FLIGHT TYPE: "+strItem;
	HTREEITEM hFlightType=m_TreeCriteria.InsertItem(strItem,cni,FALSE,FALSE,parentItem);
	m_TreeCriteria.SetItemData(hFlightType,(DWORD)flightType);

	TimeWindowList::iterator timeWindowIter;
	for (timeWindowIter=timeWinList.begin();timeWindowIter!=timeWinList.end();timeWindowIter++)
	{	
		strItem="";
		CAirSideCreteriaTimeWin *curTimeWin=(CAirSideCreteriaTimeWin *)(*timeWindowIter);
		addTimeWinItem(hFlightType,curTimeWin);

	}
	m_TreeCriteria.Expand(hFlightType,TVE_EXPAND);
	m_TreeCriteria.SelectItem(hFlightType);
	return hFlightType;
}
HTREEITEM CDlgOccupiedAssignedStand::addTimeWinItem(HTREEITEM parentItem,CAirSideCreteriaTimeWin *timeWin)
{
	//CriteriaLevel cLevle=(CriteriaLevel)getSelItemLevel();
	CString strItem;
	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);
	ElapsedTime startTime=timeWin->GetStartTime();
	ElapsedTime endTime=timeWin->GetEndTime();
	strItem.Format(_T("Day%d %02d:%02d:%02d - Day%d %02d:%02d:%02d"),\
		startTime.GetDay(),startTime.GetHour(),startTime.GetMinute(),startTime.GetSecond(),\
		endTime.GetDay(),endTime.GetHour(),endTime.GetMinute(),endTime.GetSecond());

	strItem="TIME WINDOW: "+strItem;

	cni.net=NET_SHOW_DIALOGBOX;

	HTREEITEM hTimeWindow=m_TreeCriteria.InsertItem(strItem,cni,FALSE,FALSE,parentItem);
	m_TreeCriteria.SetItemData(hTimeWindow,(DWORD)timeWin);

	COccupiedAssignedStandStrategy &strategy=timeWin->GetStrategy();
	cni.net=NET_NORMAL;
	HTREEITEM hStrategy=m_TreeCriteria.InsertItem("STRATEGY",cni,FALSE,FALSE,hTimeWindow);
	m_TreeCriteria.SetItemData(hStrategy,(DWORD)&strategy);					

	const StrategyTypeList &order=strategy.GetOrder();
	if (StrategyNum!=(int)order.size())
	{	
		return NULL;
	}
	CString strStrategy,strDetail;
	for (int i=0;i<StrategyNum;i++)
	{
		StrategyType strategyType=(StrategyType)order.at(i);
		switch(strategyType)
		{
		case Delaytime:
			{								
				cni.net = NET_EDIT_INT;
				strDetail.Format(_T("Slow down to arrive [%d] mins later"),strategy.GetDelaytime());
				strStrategy.Format(_T("%d: %s"),i+1,strDetail);
				cni.fMinValue=(float)strategy.GetDelaytime();
				HTREEITEM hDetail=m_TreeCriteria.InsertItem(strStrategy,cni,FALSE,FALSE,hStrategy);								
				m_TreeCriteria.SetItemData(hDetail,strategyType);

				break;
			}
		case ToDynamicalStand:
			{
				cni.net = NET_SHOW_DIALOGBOX;
				strDetail.Format(_T("Go dynamically reassigned gate"));
				strStrategy.Format(_T("%d: %s"),i+1,strDetail);
				HTREEITEM hDetail=m_TreeCriteria.InsertItem(strStrategy,cni,FALSE,FALSE,hStrategy);
				m_TreeCriteria.SetItemData(hDetail,(DWORD)strategyType);
				break;
			}
		case ToIntersection:
			{
				cni.net = NET_SHOW_DIALOGBOX;

				int nFlag;
				AltObjectVectorMapIter iter;

				int nIntersectionID = strategy.GetIntersectionID();
				if(nIntersectionID != -1)
				{
					IntersectionNode intersectNode;
					intersectNode.ReadData(nIntersectionID);

					CString strIntersectionTaxiway1Name,strIntersectionTaxiway2Name;
					nFlag = 0;
					iter = m_TaxiwayVectorMap.begin();
					for(; iter != m_TaxiwayVectorMap.end(); iter++)
					{
						AltObjectVector& vec = iter->second;
						for(AltObjectVectorIter it = vec.begin();it != vec.end(); it++)
						{
							if( intersectNode.HasObject(it->second) )
							{
								nFlag++;
								if (nFlag==1)
								{
									strIntersectionTaxiway1Name = it->first;
								}else
								{
									strIntersectionTaxiway2Name = it->first;
								}


								if(nFlag == 2)
									break;
							}				
						}
						if(nFlag == 2)
							break;
					}
					strDetail.Format("%s(%s&%d)",strIntersectionTaxiway1Name,strIntersectionTaxiway2Name,intersectNode.GetIndex()/*+1*/);
				}
				else
					strDetail = _T("Please Edit");

				strDetail = _T("Stop short of intersection [") + strDetail + _T("] till assigned stand free");

				strStrategy.Format(_T("%d: %s"),i+1,strDetail);
				HTREEITEM hDetail=m_TreeCriteria.InsertItem(strStrategy,cni,FALSE,FALSE,hStrategy);
				m_TreeCriteria.SetItemData(hDetail,(DWORD)strategyType);
				break;
			}
		case ToStand:
			{

				cni.net = NET_SHOW_DIALOGBOX;								
				ObjIDList &standList= strategy.GetTmpStandList();
				if ((int)standList.size()==0)
				{								
					strDetail = _T("Please Edit");					
				}else
				{
					strDetail=_T("");
					ObjIDList::iterator standIter=standList.begin();
					for(;standIter!=standList.end();standIter++)
					{
						int nStandID=(int)(*standIter);

						ALTObjectGroup altObjGroup;	
						altObjGroup.ReadData(nStandID);
						strDetail= strDetail+_T("[")+altObjGroup.getName().GetIDString()+_T("]");	
					}
					
				}
				strDetail = _T("Go to Stand in priority till assigned stand free.[") + strDetail+_T("]") ;
				strStrategy.Format(_T("%d: %s"),i+1,strDetail);
				HTREEITEM hDetail=m_TreeCriteria.InsertItem(strStrategy,cni,FALSE,FALSE,hStrategy);
				m_TreeCriteria.SetItemData(hDetail,(DWORD)strategyType);

				break;
			}
		case ToTemporaryParking:
			{
				cni.net = NET_SHOW_DIALOGBOX;	
				ObjIDList &tmpParkingList=strategy.GetTmpParkingList();

				if (tmpParkingList.size()==0)
				{
					strDetail = _T("Please Edit");
					strDetail = _T("Go to temporary parking in order[") + strDetail + _T("] ");
				}else
				{
					strDetail=_T("");
					ObjIDList::iterator tmpParkingIter=tmpParkingList.begin();

					AltObjectVectorMapIter iter;
					for(;tmpParkingIter!=tmpParkingList.end();tmpParkingIter++)
					{
						int nFlag = 0;
						iter = m_TemporaryParkingVectorMap.begin();
						for(; iter != m_TemporaryParkingVectorMap.end(); iter++)
						{
							AltObjectVector& vec = iter->second;
							for(AltObjectVectorIter it = vec.begin();it != vec.end(); it++)
							{
								if(it->second == (int)(*tmpParkingIter))
								{
									strDetail =strDetail+_T("[")+ it->first+_T("]");
									nFlag = 1;
									break;
								}
							}
							if(nFlag == 1)
								break;
						}
					}

					strDetail = _T("Go to temporary parking in order") + strDetail ;

				}

				strStrategy.Format(_T("%d: %s"),i+1,strDetail);
				HTREEITEM hDetail=m_TreeCriteria.InsertItem(strStrategy,cni,FALSE,FALSE,hStrategy);
				m_TreeCriteria.SetItemData(hDetail,(DWORD)strategyType);
				break;
			}

		}


	}
	m_TreeCriteria.Expand(hTimeWindow,TVE_EXPAND);
	m_TreeCriteria.Expand(hStrategy,TVE_EXPAND);
	m_TreeCriteria.SelectItem(hTimeWindow);
	return hTimeWindow;
}
void CDlgOccupiedAssignedStand::GetTaxiwayMap()
{
	std::vector<int> vAirportIds;
	InputAirside::GetAirportList(m_nProjID, vAirportIds);
	for (std::vector<int>::iterator iterAirportID = vAirportIds.begin(); iterAirportID != vAirportIds.end(); ++iterAirportID)
	{
		ALTAirport airport;
		airport.ReadAirport(*iterAirportID);

		std::vector<int> vTaxiwaysIDs;
		ALTAirport::GetTaxiwaysIDs(*iterAirportID, vTaxiwaysIDs);

		AltObjectVector vTaxiwayNameID; 

		std::vector<int>::iterator iterTaxiwayID = vTaxiwaysIDs.begin();
		for (; iterTaxiwayID != vTaxiwaysIDs.end(); ++iterTaxiwayID)
		{
			Taxiway taxiway;
			taxiway.ReadObject(*iterTaxiwayID);
			vTaxiwayNameID.push_back(std::make_pair(taxiway.GetMarking().c_str(),taxiway.getID()));
		}
		m_TaxiwayVectorMap.insert(std::make_pair(airport.getName(),vTaxiwayNameID));
	}
	m_pTaxiwayVectorMap = &m_TaxiwayVectorMap;
}
void CDlgOccupiedAssignedStand::OnBnClickedSave()
{
	// TODO: Add your control notification handler code here
	try
	{
		CADODatabase::BeginTransaction() ;
		m_OccupiedAssignedStandCriteria.SaveData();
		CADODatabase::CommitTransaction() ;

	}
	catch (CADOException e)
	{
		CADODatabase::RollBackTransation() ;
	}
// 	CXTResizeDialog::OnOK();
}
void CDlgOccupiedAssignedStand::OnTvnSelchangedTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	HTREEITEM hsel=m_TreeCriteria.GetSelectedItem();
	int nLevelNum=getSelItemLevel();
	if (nLevelNum<LEVEL_EXIT||nLevelNum>LEVEL_STRATEGYDETAIL)
	{
		return;
	}
	CriteriaLevel cLevel=(CriteriaLevel)nLevelNum;
	switch(cLevel)
	{
	case LEVEL_STRATEGYDETAIL:
		{
			m_toolbarCriteria.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONADD);	
			m_toolbarCriteria.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONDEL,false);			
			m_toolbarCriteria.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONEDIT,false);
			m_toolbarCriteria.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONUP);
			m_toolbarCriteria.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONDOWN);
			break;
		}
	case LEVEL_EXIT:		
	case LEVEL_PARKINGSTAND:
	case LEVEL_FLIGHTTYPE:
	case LEVEL_TIMEWINDOW:
		{			
			m_toolbarCriteria.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONADD);	
			m_toolbarCriteria.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONDEL);			
			m_toolbarCriteria.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONEDIT,false);
			m_toolbarCriteria.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONUP,false);
			m_toolbarCriteria.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONDOWN,false);
			break;
		}
	default:
		{
			m_toolbarCriteria.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONADD);	
			m_toolbarCriteria.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONDEL,false);			
			m_toolbarCriteria.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONEDIT,false);
			m_toolbarCriteria.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONUP,false);
			m_toolbarCriteria.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONDOWN,false);
		}
	}
	
}


void CDlgOccupiedAssignedStand::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class
	m_bWindowClose=true;
	OnBnClickedSave();
	CXTResizeDialog::OnOK();
}

void CDlgOccupiedAssignedStand::OnContextMenu(CWnd* pWnd, CPoint point)
{
	CRect rcCriteriaTree;
	m_TreeCriteria.GetWindowRect(rcCriteriaTree);
	if (!rcCriteriaTree.PtInRect(point))
	{
		return;
	}
	m_TreeCriteria.SetFocus();
	CPoint pt=point;
	m_TreeCriteria.ScreenToClient(&pt);
	UINT iRet;
	HTREEITEM hRClickItem=m_TreeCriteria.HitTest(pt,&iRet);
	if (iRet!=TVHT_ONITEMLABEL)// Must click on the label
	{
		hRClickItem=NULL;
	}
	if (hRClickItem==NULL)
	{
		return;
	}
	m_TreeCriteria.SelectItem(hRClickItem);
	CMenu menuPopup;

	int nLevelNum=getSelItemLevel();

	if (nLevelNum<LEVEL_EXIT||nLevelNum>LEVEL_STRATEGYDETAIL)
	{
		return;
	}
	CriteriaLevel selLevel=(CriteriaLevel)nLevelNum;
	switch (selLevel)
	{
	case LEVEL_EXIT:
		{
			menuPopup.LoadMenu(IDR_MENU_ADD_PARKINGSTANDS);
			CMenu *pMenu=NULL;
			pMenu=menuPopup.GetSubMenu(0);
			if (pMenu)
			{
				pMenu->TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);	
			}
			break;
		}
	case LEVEL_PARKINGSTAND:
		{
			menuPopup.LoadMenu(IDR_MENU_ADD_FLIGHTTYPE);
			CMenu *pMenu=NULL;
			pMenu=menuPopup.GetSubMenu(0);
			if (pMenu)
			{
				pMenu->TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);	
			}
			break;
		}
	case LEVEL_FLIGHTTYPE:
		{
			menuPopup.LoadMenu(IDR_MENU_ADD_TIMEWINDOW);
			CMenu *pMenu=NULL;
			pMenu=menuPopup.GetSubMenu(0);
			if (pMenu)
			{
				pMenu->TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);	
			}
			break;
		}
	default:
		{
			break;
		}
	}
	

}
void CDlgOccupiedAssignedStand::OnNewExits()
{
	CAirSideCriteriaExits *exits=new CAirSideCriteriaExits();
	m_OccupiedAssignedStandCriteria.addExits(exits);
	addExitsItem(exits);
}
void CDlgOccupiedAssignedStand::OnNewParkingStands()
{
	HTREEITEM hSelItem = m_TreeCriteria.GetSelectedItem();
	if (hSelItem == NULL)
		return;	

	CAirSideCriteriaExits *curExits=(CAirSideCriteriaExits *)m_TreeCriteria.GetItemData(hSelItem);
	CAirSideCriteriaParkingStands *parkingStand=curExits->addDefaultData();	
	addParkingStandsItem(hSelItem,parkingStand);
}
void CDlgOccupiedAssignedStand::OnNewFlightType()
{
	HTREEITEM hSelItem = m_TreeCriteria.GetSelectedItem();
	if (hSelItem == NULL)
		return;

	CAirSideCriteriaParkingStands *curParkingStands=(CAirSideCriteriaParkingStands *)m_TreeCriteria.GetItemData(hSelItem);
	CAirSideCriteriaFlightType *flightType=curParkingStands->addDefaultData();
    addFlightTypeItem(hSelItem,flightType);
}
void CDlgOccupiedAssignedStand::OnNewTimeWindow()
{
	HTREEITEM hSelItem = m_TreeCriteria.GetSelectedItem();
	if (hSelItem == NULL)
		return;

	CAirSideCriteriaFlightType *flightTypeItem=(CAirSideCriteriaFlightType *)m_TreeCriteria.GetItemData(hSelItem);
	CAirSideCreteriaTimeWin *timeWindow=flightTypeItem->addDefaultData();
	addTimeWinItem(hSelItem,timeWindow);
}
void CDlgOccupiedAssignedStand::OnStrategyUp()
{
	HTREEITEM hSelItem=m_TreeCriteria.GetSelectedItem();
	HTREEITEM hParentItem=m_TreeCriteria.GetParentItem(hSelItem);
	if (hSelItem==NULL||hParentItem==NULL)
	{
		return;
	}
	int selIndex=getItemIndex(hSelItem);
	if (selIndex<1)
	{
		return;
	}
	COccupiedAssignedStandStrategy *curStrategy=(COccupiedAssignedStandStrategy *)m_TreeCriteria.GetItemData(hParentItem);
	curStrategy->StrategyUp(selIndex);

	HTREEITEM hPreItem=m_TreeCriteria.GetPrevSiblingItem(hSelItem);
	exchangeStrategyItem(hSelItem,hPreItem);
	m_TreeCriteria.SelectItem(hPreItem);	

}
void CDlgOccupiedAssignedStand::OnStrategyDown()
{
	HTREEITEM hSelItem=m_TreeCriteria.GetSelectedItem();
	HTREEITEM hParentItem=m_TreeCriteria.GetParentItem(hSelItem);
	if (hSelItem==NULL||hParentItem==NULL)
	{
		return;
	}
	int selIndex=getItemIndex(hSelItem);
	if (selIndex>StrategyNum-2)
	{
		return;
	}
	COccupiedAssignedStandStrategy *curStrategy=(COccupiedAssignedStandStrategy *)m_TreeCriteria.GetItemData(hParentItem);
	curStrategy->StrategyDown(selIndex);
	HTREEITEM hNextItem=m_TreeCriteria.GetNextItem(hSelItem,TVGN_NEXT);
	exchangeStrategyItem(hSelItem,hNextItem);
	m_TreeCriteria.SelectItem(hNextItem);
	
	
}
void CDlgOccupiedAssignedStand::OnDelButton()
{
	HTREEITEM hSelItem=m_TreeCriteria.GetSelectedItem();
	if (hSelItem==NULL)
	{
		return;
	}
	HTREEITEM hParentItem=m_TreeCriteria.GetParentItem(hSelItem);
	HTREEITEM nextItem=m_TreeCriteria.GetNextItem(hSelItem,TVGN_NEXT);
	HTREEITEM preItem=m_TreeCriteria.GetPrevSiblingItem(hSelItem);
	int nLevelNum=getSelItemLevel();
	if (nLevelNum<LEVEL_EXIT||nLevelNum>LEVEL_STRATEGYDETAIL)
	{
		return;
	}
	CriteriaLevel cLevle=(CriteriaLevel)nLevelNum;
	switch(cLevle)
	{
	case LEVEL_EXIT:
		{
			int curIndex=0;
			HTREEITEM tmpExitsItem=m_TreeCriteria.GetNextItem(NULL,TVGN_CHILD);
			
			while (tmpExitsItem!=hSelItem)
			{
				tmpExitsItem=m_TreeCriteria.GetNextItem(tmpExitsItem,TVGN_NEXT);
				curIndex++;
			}
			m_OccupiedAssignedStandCriteria.delExits(curIndex);

			m_TreeCriteria.DeleteItem(hSelItem);
			if (nextItem!=NULL)
			{
				m_TreeCriteria.SelectItem(nextItem);				
			}else if(preItem!=NULL)
			{
				m_TreeCriteria.SelectItem(preItem);	
			}else
			{				
				HTREEITEM newItem=addExitsItem(m_OccupiedAssignedStandCriteria.getExits(0));
				if (newItem!=NULL)
				{
					m_TreeCriteria.SelectItem(newItem);
				}				
			}

			break;
		}
	case LEVEL_PARKINGSTAND:
		{			
			CAirSideCriteriaExits *curExits=(CAirSideCriteriaExits *)m_TreeCriteria.GetItemData(hParentItem);
            curExits->delParkingStands(getItemIndex(hSelItem));

			m_TreeCriteria.DeleteItem(hSelItem);
			if (nextItem!=NULL)
			{
				m_TreeCriteria.SelectItem(nextItem);				
			}else if(preItem!=NULL)
			{
				m_TreeCriteria.SelectItem(preItem);	
			}else
			{				
				HTREEITEM newItem=addParkingStandsItem(hParentItem,curExits->getParkingStands(0));
				if (newItem!=NULL)
				{
					m_TreeCriteria.SelectItem(newItem);
				}	
			}
			break;
		}
	case LEVEL_FLIGHTTYPE:
		{
			CAirSideCriteriaParkingStands *curParkingStands=(CAirSideCriteriaParkingStands *)m_TreeCriteria.GetItemData(hParentItem);
			curParkingStands->delFlightType(getItemIndex(hSelItem));

			m_TreeCriteria.DeleteItem(hSelItem);
			if (nextItem!=NULL)
			{
				m_TreeCriteria.SelectItem(nextItem);				
			}else if(preItem!=NULL)
			{
				m_TreeCriteria.SelectItem(preItem);	
			}else
			{				
				HTREEITEM newItem=addFlightTypeItem(hParentItem,curParkingStands->getFlightType(0));
				if (newItem!=NULL)
				{
					m_TreeCriteria.SelectItem(newItem);
				}	
			}
			break;
		}
	case LEVEL_TIMEWINDOW:
		{
			CAirSideCriteriaFlightType *curFlightType=(CAirSideCriteriaFlightType *)m_TreeCriteria.GetItemData(hParentItem);
			curFlightType->delTimeWindow(getItemIndex(hSelItem));

			m_TreeCriteria.DeleteItem(hSelItem);
			if (nextItem!=NULL)
			{
				m_TreeCriteria.SelectItem(nextItem);				
			}else if(preItem!=NULL)
			{
				m_TreeCriteria.SelectItem(preItem);	
			}else
			{				
				HTREEITEM newItem=addTimeWinItem(hParentItem,curFlightType->getTimeWindow(0));
				if (newItem!=NULL)
				{
					m_TreeCriteria.SelectItem(newItem);
				}	
			}
			break;
		}
	}


	
}
BOOL CDlgOccupiedAssignedStand::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog();


	// TODO:  Add extra initialization here
	SetResize(IDC_TREE_CRITERIA,SZ_TOP_LEFT, SZ_BOTTOM_RIGHT);
	SetResize(IDSAVE,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);
	SetResize(IDOK,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);
	SetResize(IDCANCEL,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);

	
	m_OccupiedAssignedStandCriteria.ReadData();
	InitToolBar();
	GetTaxiwayMap();	
	GetTemporaryParkingMap();
	ReloadTree();

	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),TRUE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgOccupiedAssignedStand::GetTemporaryParkingMap()
{
	CString strSQL;
	strSQL.Format(_T("SELECT TAXIWAYID\
					 FROM IN_TEMPPARKINGCRITERIA_TAXIWAY\
					 WHERE (PROJID = %d)"),m_nProjID);

	long lRecordCount = 0;
	CADORecordset adoRecordset;
	CADODatabase::ExecuteSQLStatement(strSQL,lRecordCount,adoRecordset);
	std::vector<int> vTemporaryParkingIds;
	int nTemporaryParkingID;
	while(!adoRecordset.IsEOF())
	{
		adoRecordset.GetFieldValue(_T("TAXIWAYID"),nTemporaryParkingID);
		vTemporaryParkingIds.push_back(nTemporaryParkingID);
		adoRecordset.MoveNextData();
	}

	int nFlag;
	std::map<int, AltObjectVector> TemporaryParkingVectorMap;
	for (std::vector<int>::iterator iterTemporaryParkingID = vTemporaryParkingIds.begin(); iterTemporaryParkingID != vTemporaryParkingIds.end(); ++iterTemporaryParkingID)
	{
		nFlag = 0;
		AltObjectVectorMapIter iter = m_TaxiwayVectorMap.begin();
		for(; iter != m_TaxiwayVectorMap.end(); iter++)
		{
			AltObjectVector vTemporaryMarkID; 
			AltObjectVector& vec = iter->second;
			for(AltObjectVectorIter it = vec.begin();it != vec.end(); it++)
			{
				if(it->second == *iterTemporaryParkingID)
				{
					CString strTemporaryParkingMark = it->first;
					Taxiway taxiway;
					taxiway.ReadObject(*iterTemporaryParkingID);
					int nAptID = taxiway.getAptID();
					TemporaryParkingVectorMap[nAptID].push_back(std::make_pair(strTemporaryParkingMark,*iterTemporaryParkingID));
					nFlag = 1;
					break;
				}
			}
			if(nFlag == 1)
				break;
		}
	}

	std::map<int, AltObjectVector>::iterator iter = TemporaryParkingVectorMap.begin();
	for(; iter != TemporaryParkingVectorMap.end(); iter++)
	{
		ALTAirport airport;
		airport.ReadAirport(iter->first);
		m_TemporaryParkingVectorMap.insert(std::make_pair(airport.getName(),iter->second));
	}
	AltObjectVectorMapIter mapiter = m_TemporaryParkingVectorMap.begin();
	if(mapiter != m_TemporaryParkingVectorMap.end())
		m_pTemporaryParkingVectorMap = &m_TemporaryParkingVectorMap;

}
int CDlgOccupiedAssignedStand::getSelItemLevel()
{
	HTREEITEM selItem=m_TreeCriteria.GetSelectedItem();
	int nLevel=-1;
	if (selItem!=NULL)
	{
		HTREEITEM tmpItem=selItem;
		while(tmpItem!=NULL)
		{
			tmpItem=m_TreeCriteria.GetParentItem(tmpItem);
			nLevel++;
		}
	}
	if (nLevel<0||nLevel>5)
	{
		int iii=0;
	}
	return nLevel;	
}
int CDlgOccupiedAssignedStand::getItemIndex(HTREEITEM selItem)
{
	if (selItem==NULL)
	{
		return -1;
	}
	
	HTREEITEM parentItem,tmpItem;
	parentItem=m_TreeCriteria.GetParentItem(selItem);
	if (parentItem==NULL)
	{
		return -1;
	}
	tmpItem=m_TreeCriteria.GetChildItem(parentItem);
	int itemIndex=0;
	while(tmpItem!=NULL&&tmpItem!=selItem)
	{
		tmpItem=m_TreeCriteria.GetNextItem(tmpItem,TVGN_NEXT);
		itemIndex++;
	}
	return itemIndex;
}
void CDlgOccupiedAssignedStand::exchangeStrategyItem(HTREEITEM &item1,HTREEITEM &item2)
{
	CString strItem1,strItem2;
	strItem1=m_TreeCriteria.GetItemText(item1);
	strItem2=m_TreeCriteria.GetItemText(item2);
	char tmpChar;
	tmpChar=strItem1.GetAt(0);
	strItem1.SetAt(0,strItem2.GetAt(0));
	strItem2.SetAt(0,tmpChar);

	m_TreeCriteria.SetItemText(item1,strItem2);
	m_TreeCriteria.SetItemText(item2,strItem1);	

	COOLTREE_NODE_INFO cni1=*m_TreeCriteria.GetItemNodeInfo(item1);
	COOLTREE_NODE_INFO cni2=*m_TreeCriteria.GetItemNodeInfo(item2);
	m_TreeCriteria.SetItemNodeInfo(item1,cni2);
	m_TreeCriteria.SetItemNodeInfo(item2,cni1);
}
void CDlgOccupiedAssignedStand::OnLvnItemchangedListOccupiedPriority(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}

void CDlgOccupiedAssignedStand::OnNMDblclkListOccupiedPriority(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	*pResult = 0;
}

void CDlgOccupiedAssignedStand::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	m_bWindowClose=true;
	CXTResizeDialog::OnCancel();
}
