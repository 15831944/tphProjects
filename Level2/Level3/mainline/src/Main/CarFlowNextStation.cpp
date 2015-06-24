// CarFlowNextStation.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "CarFlowNextStation.h"
//#include ""
#include<algorithm>
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCarFlowNextStation dialog


CCarFlowNextStation::CCarFlowNextStation(TrainTrafficGraph* _pRailWayTraffic,IntegratedStation* _pStartStation,CWnd* pParent /*=NULL*/)
	: CDialog(CCarFlowNextStation::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCarFlowNextStation)
	
	//}}AFX_DATA_INIT
	m_pRailWayTraffic = _pRailWayTraffic;
	m_pStartStaion = _pStartStation;
	m_bSelect = false;
}


void CCarFlowNextStation::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCarFlowNextStation)
	DDX_Control(pDX, IDC_TREE_NEXTSTATION_TREE, m_treeNextStation);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCarFlowNextStation, CDialog)
	//{{AFX_MSG_MAP(CCarFlowNextStation)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_NEXTSTATION_TREE, OnSelchangedTreeNextstationTree)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCarFlowNextStation message handlers

BOOL CCarFlowNextStation::OnInitDialog() 
{
	CDialog::OnInitDialog();
//	m_spinStayTime.SetRange(60,10000);
//	m_spinStayTime.SetBuddy( GetDlgItem(IDC_EDIT_STAY_MINITURE));
	
	LoadTree();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CCarFlowNextStation::LoadTree()
{
	m_vStationItem.clear();

	if( m_pStartStaion == NULL ) 
	{
		STATIONANDRALLWAY::const_iterator iter = m_pRailWayTraffic->GetTrafficGraph().begin();
		STATIONANDRALLWAY::const_iterator iterEnd = m_pRailWayTraffic->GetTrafficGraph().end();
		for ( ; iter!=iterEnd; ++iter )
		{
			char procName[256];
			(*iter)->GetStation()->getID()->printID( procName );
			m_treeNextStation.SetItemData( m_treeNextStation.InsertItem( procName ), (long)((*iter)->GetStation()) );			
		}
	}
	else
	{
		int iStationIndex = m_pRailWayTraffic->GetStationIndexInTraffic( m_pStartStaion );
		if( iStationIndex == -1 )//not find
			return ;
		int iSize = m_pRailWayTraffic->GetTrafficGraph().size();
		RAILWAY _railWayVector;
		for( int i=0; i<iSize; ++i )
		{
			if( iStationIndex != i )
			{
				if( !m_pRailWayTraffic->IsTwoStationDirectLinked( iStationIndex, i ) )
					continue;

				m_pRailWayTraffic->GetStationLinkedRailWay(iStationIndex , i , 0,  _railWayVector );
				if( _railWayVector.size() == 1 )
				{
					char sSecondStationName [256];
					m_pRailWayTraffic->GetTrafficGraph().at( i )->GetStation()->getID()->printID( sSecondStationName );
					HTREEITEM hItem = m_treeNextStation.InsertItem( sSecondStationName );
					m_treeNextStation.SetItemData( hItem , (long)( _railWayVector.at( 0 ) ) );
				}
				else if ( _railWayVector.size() ==2 )
				{
					char sSecondStationName [256];
					m_pRailWayTraffic->GetTrafficGraph().at( i )->GetStation()->getID()->printID( sSecondStationName );
					HTREEITEM hItem = m_treeNextStation.InsertItem( sSecondStationName );
					m_vStationItem.push_back( hItem );
					
					HTREEITEM hItemPort1 = m_treeNextStation.InsertItem( " Port1 " , hItem );
					HTREEITEM hItemPort2 = m_treeNextStation.InsertItem( " Port2 " , hItem );
					m_treeNextStation.Expand( hItem ,TVE_EXPAND );
					
					RailwayInfo* pRailWay0 = _railWayVector.at( 0 );
					RailwayInfo* pRailWay1 = _railWayVector.at( 1 );
					//ASSERT( pRailWay->GetFirstStationIndex() == iStationIndex );
					if( pRailWay0->GetFirstStationIndex() == iStationIndex )
					{
						if( pRailWay0->GetTypeFromFirstViewPoint() == PORT1_PORT1
							|| pRailWay0->GetTypeFromFirstViewPoint() == PORT2_PORT1 )
						{
							m_treeNextStation.SetItemData( hItemPort1 ,(long)pRailWay0 );
						}
						else
						{
							m_treeNextStation.SetItemData( hItemPort2, (long)pRailWay0 );
						}
					}
					else
					{
						if( pRailWay0->GetTypeFromSecondViewPoint() == PORT1_PORT1
							|| pRailWay0->GetTypeFromSecondViewPoint() == PORT2_PORT1 )
						{
							m_treeNextStation.SetItemData( hItemPort1 ,(long)pRailWay0 );
						}
						else
						{
							m_treeNextStation.SetItemData( hItemPort2 ,(long)pRailWay0 );
						}
					}

					if( pRailWay1->GetFirstStationIndex() == iStationIndex )
					{
						if( pRailWay1->GetTypeFromFirstViewPoint() == PORT1_PORT1
							|| pRailWay1->GetTypeFromFirstViewPoint() == PORT2_PORT1 )
						{
							m_treeNextStation.SetItemData( hItemPort1 ,(long)pRailWay1 );
						}
						else
						{
							m_treeNextStation.SetItemData( hItemPort2, (long)pRailWay1 );
						}
					}
					else
					{
						if( pRailWay1->GetTypeFromSecondViewPoint() == PORT1_PORT1
							|| pRailWay1->GetTypeFromSecondViewPoint() == PORT2_PORT1 )
						{
							m_treeNextStation.SetItemData( hItemPort1 ,(long)pRailWay1 );
						}
						else
						{
							m_treeNextStation.SetItemData( hItemPort2 ,(long)pRailWay1 );
						}
					}


				
				}
			}
		}

		//RAILWAY allLinkedRailWay;
		//m_pRailWayTraffic->GetStationLinkedRailWay( iStationIndex, 0, allLinkedRailWay );
		
		/*
		RAILWAY::const_iterator iter = allLinkedRailWay.begin();
		RAILWAY::const_iterator iterEnd = allLinkedRailWay.end();

		for(; iter!=iterEnd; ++iter )
		{
				CString sRailWayName = GetRailWayName( (*iter), iStationIndex );				
				m_treeNextStation.SetItemData( m_treeNextStation.InsertItem( sRailWayName ) , (long)(*iter) );
		}
		*/

	}
}



void CCarFlowNextStation::OnSelchangedTreeNextstationTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	
	HTREEITEM hSelectItem = m_treeNextStation.GetSelectedItem();

	if( hSelectItem )
	{
		std::vector<HTREEITEM>::const_iterator iterFind = std::find( m_vStationItem.begin(), m_vStationItem.end() , hSelectItem );
		if( iterFind != m_vStationItem.end() )
		{
			m_bSelect = false;
			return;
		}
		else
		{
			m_bSelect = true;
		}

		/*
		if( m_pStartStaion == NULL )
		{
			m_bSelect = true;
			m_pSelectedStation = (IntegratedStation*)m_treeNextStation.GetItemData( hSelectItem );
			char procName[256];
			m_pSelectedStation->getID()->printID( procName );
			CString sDisplayStr =	" Dwell Time at station   ";
			sDisplayStr += procName;
			sDisplayStr += " (S) " ;
				
			this->SetDlgItemText( IDC_STATIC_STAYTIME ,sDisplayStr );
		}
		else
		{
			m_bSelect = true;
			int iStationIndex = m_pRailWayTraffic->GetStationIndexInTraffic( m_pStartStaion );
			m_pSelectedRailWay = (RailwayInfo*)m_treeNextStation.GetItemData( hSelectItem );
		
			int iNextStationIndex = -1;
			iStationIndex == m_pSelectedRailWay->GetFirstStationIndex() ? iNextStationIndex = m_pSelectedRailWay->GetSecondStationIndex() :
																		  iNextStationIndex = m_pSelectedRailWay->GetFirstStationIndex();
			char procName[256];
			m_pRailWayTraffic->GetTrafficGraph().at( iNextStationIndex )->GetStation()->getID()->printID( procName );
			CString sDisplayStr =	" Dwell Time at station   ";
			sDisplayStr += procName;
			sDisplayStr += " (S) " ;
				
			this->SetDlgItemText( IDC_STATIC_STAYTIME ,sDisplayStr );
		
		}

		*/

	}
	
	*pResult = 0;
}

void CCarFlowNextStation::OnOK() 
{

	HTREEITEM hSelectItem = m_treeNextStation.GetSelectedItem();
	if( hSelectItem )
	{
		if( m_bSelect )
		{
			if( m_pStartStaion == NULL )
			{
				m_pSelectedStation = (IntegratedStation*)m_treeNextStation.GetItemData( hSelectItem );
			}
			else
			{
				m_pSelectedRailWay = (RailwayInfo*)m_treeNextStation.GetItemData( hSelectItem );			
			}
		}
	}
	else
	{
		return ;
	}
	CDialog::OnOK();
}

/*
CString CCarFlowNextStation::GetRailWayName( RailwayInfo* _pRailWay ,int _iFirstStationIndex )

{
	char sFirstStationName [256 ];
	char sSecondStationName [256];
	CString sReturnName;
	int iLinkedType;
	m_pRailWayTraffic->GetTrafficGraph().at( _iFirstStationIndex )->GetStation()->getID()->printID( sFirstStationName );
	if( _iFirstStationIndex == _pRailWay->GetFirstStationIndex() )
	{
		int iSecondStationIndex = _pRailWay->GetSecondStationIndex();
		m_pRailWayTraffic->GetTrafficGraph().at( iSecondStationIndex )->GetStation()->getID()->printID( sSecondStationName );
		 iLinkedType = _pRailWay->GetTypeFromFirstViewPoint();
		
	}
	else
	{
		int iSecondStationIndex = _pRailWay->GetFirstStationIndex();
		m_pRailWayTraffic->GetTrafficGraph().at( iSecondStationIndex )->GetStation()->getID()->printID( sSecondStationName );
		iLinkedType = _pRailWay->GetTypeFromSecondViewPoint();
		
	}

	switch ( iLinkedType )
		{
			case Port1_Port1:
				{
					sReturnName = sFirstStationName;
					sReturnName += " Port1 --- ";
					sReturnName += sSecondStationName;
					sReturnName += " Port1 ";
					break;
				}
			case Port1_Port2 :
				{
					sReturnName = sFirstStationName;
					sReturnName += " Port1 --- ";
					sReturnName += sSecondStationName;
					sReturnName += " Port2 ";
					break;
				}
			case Port2_Port1:
				{
					sReturnName = sFirstStationName;
					sReturnName += " Port2 --- ";
					sReturnName += sSecondStationName;
					sReturnName += " Port1 ";
					break;
				}
			default:
				{
					sReturnName = sFirstStationName;
					sReturnName += " Port2 --- ";
					sReturnName += sSecondStationName;
					sReturnName += " Port2 ";
				}
		}

	return sReturnName;
}
*/

