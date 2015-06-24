// StationLinkerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "StationLinkerDlg.h"
#include "TermPlanDoc.h"
#include "common\WinMsg.h"
#include "3DView.h"
#include "MainFrm.h"
#include <CommonData/Fallback.h>
#include "StationLinkerPopDlg.h"
#include "./MFCExControl/ARCBaseTree.h"
#include "NodeView.h"
#include "..\Inputs\RailWayData.h"
#include "..\Inputs\AllCarSchedule.h"
#include "../common/UnitsManager.h"
#include <algorithm>
#include "ChangeVerticalProfileDlg.h"
#include "Floor2.h"
#include "../Common/EchoSystem.h"


using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CStationLinkerDlg dialog


CStationLinkerDlg::CStationLinkerDlg(TrainTrafficGraph* _pDocTrafficGraph,CTermPlanDoc* _pDoc,CWnd* pParent /*=NULL*/)
	: CDialog(CStationLinkerDlg::IDD, pParent)
{
	m_pDoc = _pDoc;
	m_pDocTrafficGraph=_pDocTrafficGraph;
	m_tempTrafficeGraph=*_pDocTrafficGraph;
	m_pSelectedRailWay = NULL;
	m_hSelectedItem =NULL;
	m_iSeletedType = -1;

	//if (ECHOLOG->IsEnable() && ECHOLOG->IsLogEnable(RailWayLog))
	{
		CString strLog = _T("");
		strLog = _T("Copy Railway Data to Define Dialog's temporary variable;");
		ECHOLOG->Log(RailWayLog,strLog);
	}
}


void CStationLinkerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CStationLinkerDlg)
	DDX_Control(pDX, IDC_STATION_LINKER_TOOLBAR_POSITION, m_toolbarPosition);
	DDX_Control(pDX, IDC_TREE_RELATION, m_tvStation);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CStationLinkerDlg, CDialog)
	//{{AFX_MSG_MAP(CStationLinkerDlg)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_STATIONLINKER_ADDLINKER, OnStationlinkerAddlinker)
	ON_COMMAND(ID_STATIONLINKER_PICK, OnStationlinkerPick)
	ON_COMMAND(ID_STATIONLINKER_Z_POS, OnStationLinkerZPos)
	ON_WM_CREATE()
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_RELATION, OnSelchangedTreeRelation)
	ON_COMMAND(ID_STATION_LINKER_NEW_RAILWAY, OnStationLinkerNewRailway)
	ON_COMMAND(ID_STATION_LINKER_DELETE_RAILWAY, OnStationLinkerDeleteRailway)
	//}}AFX_MSG_MAP
	ON_MESSAGE(TP_DATA_BACK, OnTempFallbackFinished)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStationLinkerDlg message handlers

BOOL CStationLinkerDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
//	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CNodeView*)m_pParentWnd)->GetDocument();
	m_pDoc->m_tempProcInfo.SetTrafficGraph(&m_tempTrafficeGraph);
	m_pDoc->m_tempProcInfo.SetProcessorGroup( false );

	InitToolbar();
	LoadTree();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CStationLinkerDlg::InitToolbar()
{
	CRect rc;
	m_toolbarPosition.GetWindowRect( &rc );
	ScreenToClient(&rc);
	m_ToolBar.MoveWindow(&rc);
	m_ToolBar.ShowWindow(SW_SHOW);
	m_toolbarPosition.ShowWindow(SW_HIDE);
	m_ToolBar.GetToolBarCtrl().EnableButton( ID_STATION_LINKER_NEW_RAILWAY,FALSE );
	m_ToolBar.GetToolBarCtrl().EnableButton( ID_STATION_LINKER_DELETE_RAILWAY,FALSE );
	
}
void CStationLinkerDlg::LoadTree()
{
	m_tvStation.DeleteAllItems();
	m_port1Vector.clear();
	m_port2Vector.clear();
	m_railWayVector.clear();

	m_treeRoot=m_tvStation.InsertItem(" Station relation ");

	STATIONANDRALLWAY::const_iterator iter=m_tempTrafficeGraph.GetTrafficGraph().begin();
	STATIONANDRALLWAY::const_iterator iterEnd=m_tempTrafficeGraph.GetTrafficGraph().end();
	//CString sStationName;
	    char stationName[256];
	for(int i=0; iter!=iterEnd; ++iter,++i )
	{
		(*iter)->GetStation()->getID()->printID(stationName);
		HTREEITEM stationItem=m_tvStation.InsertItem( stationName, m_treeRoot);		
		m_tvStation.SetItemData(stationItem,i);
		
		HTREEITEM port1Item=m_tvStation.InsertItem( " Port 1" ,stationItem);		
		m_tvStation.SetItemData(port1Item,1);
		m_port1Vector.push_back(port1Item);
		
		RAILWAY railVector;
		(*iter)->GetLinkedRailWay(1,railVector);//get all railway which linked with station port1
		BuildRailWay(port1Item,railVector,i);
		m_tvStation.Expand(port1Item,TVE_EXPAND);


		HTREEITEM port2Item=m_tvStation.InsertItem( " Port 2" ,stationItem);		
		m_tvStation.SetItemData(port2Item,2);
		m_port2Vector.push_back(port2Item);
		
		(*iter)->GetLinkedRailWay(2,railVector);//get all railway which linked with station port2
		BuildRailWay(port2Item,railVector,i);
		m_tvStation.Expand(port2Item,TVE_EXPAND);


		m_tvStation.Expand(stationItem,TVE_EXPAND);

	}
	
   if( m_hSelectedItem )
   {
		m_tvStation.SelectItem( m_hSelectedItem );
		m_tvStation.EnsureVisible( m_hSelectedItem );
   }
	
	
	m_tvStation.Expand(m_treeRoot,TVE_EXPAND);
}



void CStationLinkerDlg::BuildRailWay(HTREEITEM _parentItem,RAILWAY& railVector,int _stationIndex)
{

	RAILWAY::const_iterator iterRail=railVector.begin();
	RAILWAY::const_iterator iterRailEnd=railVector.end();
	char stationName[256];
	for(;iterRail!=iterRailEnd;++iterRail)
	{
		int iFirstStationIndex=(*iterRail)->GetFirstStationIndex();
		int iSecondStationIndex=(*iterRail)->GetSecondStationIndex();
		const IntegratedStation* pFirstStation= m_tempTrafficeGraph.GetTrafficGraph().at(iFirstStationIndex)->GetStation();
		const IntegratedStation* pSecondStation= m_tempTrafficeGraph.GetTrafficGraph().at(iSecondStationIndex)->GetStation();

		CString firstStationName;
		pFirstStation->getID()->printID(stationName);
		firstStationName=stationName;

		CString secondStationName;
		pSecondStation->getID()->printID(stationName);
		secondStationName=stationName;

		CString railWayName;
		int iLinkedType=(*iterRail)->GetTypeFromFirstViewPoint();

		if(_stationIndex != (*iterRail)->GetOwnerIndex())
		{
			iLinkedType=(*iterRail)->GetTypeFromSecondViewPoint();
			CString temp=firstStationName;
			firstStationName=secondStationName;
			secondStationName=temp;
		}
		switch (iLinkedType)
		{
		case PORT1_PORT1:
			{
				railWayName=firstStationName + " Port1---" + secondStationName + " Port1";
				break;
			}
		case PORT1_PORT2 :
			{
				railWayName=firstStationName + " Port1---" + secondStationName + " Port2";
				break;
			}
		case PORT2_PORT1:
			{
				//if(_stationIndex==(*iterRail) )
				railWayName=firstStationName + " Port2---" + secondStationName + " Port1";
				break;
			}
		case PORT2_PORT2:
			{
				railWayName=firstStationName + " Port2---" + secondStationName + " Port2";
				break;
			}

		}

		int iStationIndex = m_tvStation.GetItemData( m_tvStation.GetParentItem( _parentItem ));	
		CString sUnit = UNITSMANAGER->GetLengthUnitString(UNITSMANAGER->GetLengthUnits());
		railWayName += " (" + sUnit + ")";
		HTREEITEM railWayItem =m_tvStation.InsertItem(railWayName,_parentItem);
		m_tvStation.SetItemData(railWayItem,(DWORD)(*iterRail));
		m_railWayVector.push_back(railWayItem);

		if( m_pSelectedRailWay && m_pSelectedRailWay == *iterRail && iStationIndex == m_iCurrentStationIndex )
		{
			m_hSelectedItem = railWayItem;
			m_pSelectedRailWay = NULL;
			m_iSeletedType =-1;
		}
		else if ( m_iSeletedType == iLinkedType && iStationIndex == m_iCurrentStationIndex && m_iSecondStationIndex == iSecondStationIndex )
		{
			m_hSelectedItem = railWayItem;
			m_iSeletedType =-1;
			m_pSelectedRailWay = NULL;
		}

		int pointNum=(*iterRail)->GetRailWayPath().getCount();
		CString csPoint;
		for( int i=0; i<pointNum; i++ )
		{							
			csPoint.Format( "x = %.2f; y = %.2f; z = %.2f", 
				UNITSMANAGER->ConvertLength( (*iterRail)->GetRailWayPath().getPoint(i).getX() ),
				UNITSMANAGER->ConvertLength( (*iterRail)->GetRailWayPath().getPoint(i).getY() ),
				UNITSMANAGER->ConvertLength( (*iterRail)->GetRailWayPath().getPoint(i).getZ() ) );

			m_tvStation.Expand(m_tvStation.InsertItem( csPoint, railWayItem),TVE_EXPAND);
			m_tvStation.Expand(railWayItem,TVE_EXPAND);
		}			
	}
}

void CStationLinkerDlg::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	HTREEITEM _hRClickItem = NULL;

	CRect rectTree;
	m_tvStation.GetWindowRect( &rectTree );
	if( !rectTree.PtInRect(point) ) 
		return;

	m_tvStation.SetFocus();
	CPoint pt = point;
	m_tvStation.ScreenToClient( &pt );
	UINT iRet;
	_hRClickItem = m_tvStation.HitTest( pt, &iRet );
	if (iRet != TVHT_ONITEMLABEL)   // Must click on the label
	{
		_hRClickItem = NULL;
	}


	if(_hRClickItem)
	{
		m_tvStation.SelectItem( _hRClickItem );
		CMenu menuPopup;
		menuPopup.LoadMenu( IDR_MENU_POPUP );
		
		m_currentItem=_hRClickItem;
		CMenu* pMenu = menuPopup.GetSubMenu( 11 );
		
		TREEITEM::iterator iter=find(m_port1Vector.begin(),m_port1Vector.end(),_hRClickItem);
		if(iter!=m_port1Vector.end())//click on port1 item
		{
			pMenu->EnableMenuItem( ID_STATIONLINKER_PICK, MF_GRAYED );
			pMenu->EnableMenuItem( ID_STATIONLINKER_Z_POS, MF_GRAYED );
			m_iCurrentStationIndex=m_tvStation.GetItemData(m_tvStation.GetParentItem(_hRClickItem));
			m_iCurrentPortIndex=1;
			m_pSelectedRailWay = NULL;
			m_hSelectedItem = NULL;
		}
		else
		{
			iter=find(m_port2Vector.begin(),m_port2Vector.end(),_hRClickItem);
			if(iter!=m_port2Vector.end())//click on port2 item
			{
				m_iCurrentStationIndex=m_tvStation.GetItemData(m_tvStation.GetParentItem(_hRClickItem));
				m_iCurrentPortIndex=2;
				m_pSelectedRailWay = NULL;
				m_hSelectedItem = NULL;
				pMenu->EnableMenuItem( ID_STATIONLINKER_PICK, MF_GRAYED );
			}
			else
			{
				iter=find(m_railWayVector.begin(),m_railWayVector.end(),_hRClickItem);
				if(iter!=m_railWayVector.end())//click on railway item
				{
					pMenu->EnableMenuItem( ID_STATIONLINKER_ADDLINKER, MF_GRAYED );
					m_iCurrentStationIndex = m_tvStation.GetItemData(m_tvStation.GetParentItem(m_tvStation.GetParentItem(_hRClickItem)));
					m_pSelectedRailWay = (RailwayInfo*)m_tvStation.GetItemData( _hRClickItem );
				}
				else
					return;
			}


		}

		pMenu->TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);	

	}
}

void CStationLinkerDlg::OnStationlinkerAddlinker() 
{
	
	
	CStationLinkerPopDlg dlg(m_iCurrentStationIndex,m_iCurrentPortIndex,&m_tempTrafficeGraph);
	if(dlg.DoModal() == IDOK)
	{
		int iLinkedType=-1;
		 m_iSecondStationIndex=dlg.m_iSelectedStationIndex;
		 m_iSecondStationPort=dlg.m_iSelectedStationPort;

		if(m_iCurrentPortIndex==1 &&m_iSecondStationPort==1 )
			iLinkedType=PORT1_PORT1;
		else if(m_iCurrentPortIndex==1 &&m_iSecondStationPort==2 )
			iLinkedType=PORT1_PORT2;
		else if(m_iCurrentPortIndex==2 &&m_iSecondStationPort==1 )
			iLinkedType=PORT2_PORT1;
		else 
			iLinkedType=PORT2_PORT2;

		m_iSeletedType = iLinkedType;
		m_pSelectedRailWay = NULL;
		if(m_tempTrafficeGraph.IsTwoStationDirectLinked(m_iCurrentStationIndex,m_iSecondStationIndex,iLinkedType))
		{
			AfxMessageBox("You have added this railway path!");
			return ;
		}
		else
		{
			Point defaultRailWayPath[2];
			
			if( m_iCurrentPortIndex == 1 )
			{
				defaultRailWayPath[0] = m_tempTrafficeGraph.GetTrafficGraph().at( m_iCurrentStationIndex )->GetStation()->GetPort1Path()->getPoint(0);
			}
			else
			{
				defaultRailWayPath[0] = m_tempTrafficeGraph.GetTrafficGraph().at( m_iCurrentStationIndex )->GetStation()->GetPort2Path()->getPoint(0);
			}

			Point SecondPoint;
			
			if( m_iSecondStationPort == 1 )
			{
				defaultRailWayPath[1] = m_tempTrafficeGraph.GetTrafficGraph().at( m_iSecondStationIndex )->GetStation()->GetPort1Path()->getPoint(0);
			}
			else
			{
				defaultRailWayPath[1] = m_tempTrafficeGraph.GetTrafficGraph().at( m_iSecondStationIndex )->GetStation()->GetPort2Path()->getPoint(0);
			}

			
			m_tempTrafficeGraph.InsertRailWay(m_iCurrentStationIndex,m_iSecondStationIndex,iLinkedType,0,NULL);
			m_tempTrafficeGraph.UpdateRailWayInfo( m_iCurrentStationIndex, m_iSecondStationIndex, iLinkedType , 2, defaultRailWayPath );

			LoadTree();

		}
	}

}

void CStationLinkerDlg::OnStationlinkerPick() 
{

	//	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CNodeView*)m_pParentWnd)->GetDocument();
		C3DView* p3DView = m_pDoc->Get3DView();
		if( p3DView == NULL )
		{
			m_pDoc->GetMainFrame()->CreateOrActive3DView();
			p3DView = m_pDoc->Get3DView();
		}
		if (!p3DView)
		{
			ASSERT(FALSE);
			return;
		}

		HideDialog( m_pDoc->GetMainFrame() );

		
		FallbackData data;
		data.hSourceWnd = GetSafeHwnd();
		enum FallbackReason enumReason;
		enumReason = PICK_MANYPOINTS;

		
		if( !m_pDoc->GetMainFrame()->SendMessage( TP_TEMP_FALLBACK, (WPARAM)&data, (LPARAM)enumReason ) )
		{
			MessageBox( "Error" );
			ShowDialog( m_pDoc->GetMainFrame() );
			return;
		}

	
}

LRESULT CStationLinkerDlg::OnTempFallbackFinished(WPARAM wParam, LPARAM lParam)
{
	CWnd* wnd = GetParent();
	while (!wnd->IsKindOf(RUNTIME_CLASS(CMDIFrameWnd)) )
	{
		wnd->ShowWindow(SW_SHOW);
		wnd->EnableWindow(false);
		wnd = wnd->GetParent();
	}
	wnd->EnableWindow(false);
	ShowWindow(SW_SHOW);	
	EnableWindow();
   // CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CNodeView*)m_pParentWnd)->GetDocument();
	std::vector<ARCVector3>* pData = reinterpret_cast< std::vector<ARCVector3>* >(wParam);

	if(pData->size()>0)
		{
			RailwayInfo* pRailWay=(RailwayInfo*)m_tvStation.GetItemData(m_currentItem);
			ASSERT(pRailWay);

			Point ptList[MAX_POINTS];
			ptList[0]=pRailWay->GetRailWayPath().getPoint(0);

			int nCount = pData->size();
			for( int i=1; i<=nCount; i++ )
			{
				ARCVector3 v3D = pData->at(i-1);
				ptList[i].setX( v3D[VX] );
				ptList[i].setY( v3D[VY] );
				ptList[i].setZ( m_pDoc->m_nActiveFloor * SCALE_FACTOR );
			}

			int iRailwayPointCount = pRailWay->GetRailWayPath().getCount();
			ptList[nCount+1]=pRailWay->GetRailWayPath().getPoint(iRailwayPointCount-1);

			
			pRailWay->SetRailWay(nCount+2,ptList);
			//m_tempTrafficeGraph.UpdateRailWayInfo(pRailWay->GetFirstStationIndex(),pRailWay->GetSecondStationIndex(),
												//	pRailWay->GetTypeFromFirstViewPoint(),nCount,ptList);

			LoadTree();
			m_pDoc->UpdateAllViews(NULL);
	
		}

	return TRUE;
}

void CStationLinkerDlg::HideDialog(CWnd* parentWnd)
{
	ShowWindow(SW_HIDE);
	while(!parentWnd->IsKindOf(RUNTIME_CLASS(CMDIFrameWnd))) {
		parentWnd->ShowWindow(SW_HIDE);
		parentWnd = parentWnd->GetParent();
	}
	parentWnd->EnableWindow(TRUE);
	parentWnd->SetActiveWindow();
}

void CStationLinkerDlg::ShowDialog(CWnd* parentWnd)
{
	while (!parentWnd->IsKindOf(RUNTIME_CLASS(CMDIFrameWnd)) )
	{
		parentWnd->ShowWindow(SW_SHOW);
		parentWnd->EnableWindow(FALSE);
		parentWnd = parentWnd->GetParent();
	}
	parentWnd->EnableWindow(FALSE);
	ShowWindow(SW_SHOW);	
	EnableWindow();
}

void CStationLinkerDlg::OnOK() 
{
	if(!m_tempTrafficeGraph.IsAllRailWayHasDefinedPath())
	{
		AfxMessageBox("Every railway must pick up a path!");
		return;
	}

	if ( AfxMessageBox( " Since you have changed the railway system definiton, then all  train schedule which use this system will be update consistently!" ,MB_OKCANCEL) == IDOK )
	{
		*m_pDocTrafficGraph	= m_tempTrafficeGraph;
	//	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CNodeView*)m_pParentWnd)->GetDocument();
		m_pDoc->m_tempProcInfo.SetTrafficGraph(NULL);	

		vector<RailwayInfo*> allRailWay;
		m_pDocTrafficGraph->GetAllRailWayInfo(allRailWay);
		m_pDoc->GetTerminal().pRailWay->SetAllRailWayInfoVector(allRailWay);
		m_pDoc->GetTerminal().pRailWay->saveDataSet(m_pDoc->m_ProjInfo.path,true);

		m_pDoc->GetTerminal().m_pAllCarSchedule->SetSystemRailWay( m_pDoc->GetTerminal().pRailWay );
		m_pDoc->GetTerminal().m_pAllCarSchedule->AdjustRailWay( m_pDocTrafficGraph );
		m_pDoc->GetTerminal().m_pAllCarSchedule->saveDataSet( m_pDoc->m_ProjInfo.path,false );

		//if (ECHOLOG->IsEnable() && ECHOLOG->IsLogEnable(RailWayLog))
		{
			CString strLog = _T("");
			strLog = _T("Railway Data change by Define Dialog;");
			ECHOLOG->Log(RailWayLog,strLog);
		}

		m_pDoc->UpdateAllViews(NULL);
		CDialog::OnOK();
	}
	else
		return;
}

void CStationLinkerDlg::OnCancel() 
{
	// TODO: Add extra cleanup here
	//CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CNodeView*)m_pParentWnd)->GetDocument();
	m_pDoc->m_tempProcInfo.SetTrafficGraph(NULL);
	m_pDoc->UpdateAllViews(NULL);
	CDialog::OnCancel();
}

int CStationLinkerDlg::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_ToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_ToolBar.LoadToolBar(IDR_TOOLBAR_STATION_LINKER))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}	
	return 0;
}



void CStationLinkerDlg::OnSelchangedTreeRelation(NMHDR* pNMHDR, LRESULT* pResult) 
{
	HTREEITEM _hClickItem = m_tvStation.GetSelectedItem();
	if(_hClickItem )
	{
		TREEITEM::const_iterator iter=find(m_port1Vector.begin(),m_port1Vector.end(),_hClickItem);
		if(iter!=m_port1Vector.end())
		{
			m_ToolBar.GetToolBarCtrl().EnableButton( ID_STATION_LINKER_NEW_RAILWAY,TRUE );
			m_ToolBar.GetToolBarCtrl().EnableButton( ID_STATION_LINKER_DELETE_RAILWAY,FALSE );
			
			m_iCurrentStationIndex=m_tvStation.GetItemData(m_tvStation.GetParentItem(_hClickItem));
			m_iCurrentPortIndex=1;

		}
		else
		{
			iter=find(m_port2Vector.begin(),m_port2Vector.end(),_hClickItem);
			if(iter!=m_port2Vector.end())
			{
				m_ToolBar.GetToolBarCtrl().EnableButton( ID_STATION_LINKER_NEW_RAILWAY,TRUE );
				m_ToolBar.GetToolBarCtrl().EnableButton( ID_STATION_LINKER_DELETE_RAILWAY,FALSE );
				m_iCurrentStationIndex=m_tvStation.GetItemData(m_tvStation.GetParentItem(_hClickItem));
				m_iCurrentPortIndex=2;
			}
			else
			{
				iter=find(m_railWayVector.begin(),m_railWayVector.end(),_hClickItem);
				if(iter!=m_railWayVector.end())//click on railway item
				{
					m_ToolBar.GetToolBarCtrl().EnableButton( ID_STATION_LINKER_NEW_RAILWAY,FALSE );
					m_ToolBar.GetToolBarCtrl().EnableButton( ID_STATION_LINKER_DELETE_RAILWAY,TRUE );
				}
				else
				{
					m_ToolBar.GetToolBarCtrl().EnableButton( ID_STATION_LINKER_NEW_RAILWAY,FALSE );
					m_ToolBar.GetToolBarCtrl().EnableButton( ID_STATION_LINKER_DELETE_RAILWAY,FALSE );

				}
			}
		}
	}
	else
	{
		m_ToolBar.GetToolBarCtrl().EnableButton( ID_STATION_LINKER_NEW_RAILWAY,FALSE );
		m_ToolBar.GetToolBarCtrl().EnableButton( ID_STATION_LINKER_DELETE_RAILWAY,FALSE );
	}
	
	*pResult = 0;
}

void CStationLinkerDlg::OnStationLinkerNewRailway() 
{
	OnStationlinkerAddlinker();
}

void CStationLinkerDlg::OnStationLinkerDeleteRailway() 
{
	HTREEITEM _hClickItem = m_tvStation.GetSelectedItem();

	if(_hClickItem)
	{
		RailwayInfo* pRailWay=(RailwayInfo*)m_tvStation.GetItemData(_hClickItem);
		m_tempTrafficeGraph.DeleteRailWay(pRailWay->GetFirstStationIndex(),pRailWay->GetSecondStationIndex(),pRailWay->GetTypeFromFirstViewPoint());
		delete pRailWay;
		LoadTree();
		m_pDoc->UpdateAllViews(NULL);
	}
}

void CStationLinkerDlg::OnStationLinkerZPos() 
{
	// get every floor's real height
	int nFloorCount = m_pDoc->GetCurModeFloor().m_vFloors.size();
	std::vector<double>vFloorAltitudes;
	for(int i=0; i<nFloorCount; i++) 
	{
		vFloorAltitudes.push_back( i );				
	}

	//get x (distance from point 0) and z
	std::vector<double>vXPos;
	std::vector<double>vZPos;
	Path _tmpPath;

	RailwayInfo* pRailWay=(RailwayInfo*)m_tvStation.GetItemData(m_currentItem);
	ASSERT(pRailWay);

	const Path& path = pRailWay->GetRailWayPath();
	Point* ptList = path.getPointList();
	int ptCount = path.getCount();
	if(ptCount>0) 
	{
		vXPos.reserve(ptCount);
		vZPos.reserve(ptCount);
		vXPos.push_back(0.0);
		vZPos.push_back(ptList[0].getZ() /SCALE_FACTOR );
		double dLenSoFar = 0.0;
		for( int i=1; i<ptCount; i++ )
		{			
			ARCVector3 v3D(ptList[i].getX()-ptList[i-1].getX(), ptList[i].getY()-ptList[i-1].getY(), 0.0);
			dLenSoFar += UNITSMANAGER->ConvertLength(v3D.Magnitude());
			vXPos.push_back(dLenSoFar);				
			vZPos.push_back( ptList[i].getZ() /SCALE_FACTOR );				
		}

		// init tem path
		_tmpPath = path;
	}

	CStringArray strArrFloorName;
	strArrFloorName.SetSize(nFloorCount);
	for(int k=0;k<nFloorCount;k++)
	{
		strArrFloorName[k]=m_pDoc->GetCurModeFloor().m_vFloors[k]->FloorName();
	}
	// call dialog to define z pos

	CChangeVerticalProfileDlg dlg(vXPos, vZPos, vFloorAltitudes, _tmpPath ,&strArrFloorName);

	if( dlg.DoModal() == IDOK )
	{
		ASSERT( vXPos.size() == vZPos.size() );
		vZPos = dlg.m_wndPtLineChart.m_vPointYPos;
		// convert z value from real height to the format which is same to the log's z format
		for(int i=0; i<static_cast<int>(vZPos.size()); ++i )
		{
			vZPos[ i ] = vZPos[ i ] * SCALE_FACTOR;	

			// TRACE( "%f\n", vZPos[i] );
		}


		// update z value
		for( int i=0; i<static_cast<int>(vZPos.size()); i++ )
		{
			ptList[i].setZ( vZPos[i] );	
		}

		LoadTree();
		m_pDoc->UpdateAllViews(NULL);
	}
}

