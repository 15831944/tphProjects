// StationLinkerPopDlg.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "StationLinkerPopDlg.h"
#include <algorithm>
using namespace std;
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CStationLinkerPopDlg dialog


CStationLinkerPopDlg::CStationLinkerPopDlg(int _firstStationIndex, int _firstStationPort, TrainTrafficGraph* _pGraph,CWnd* pParent /*=NULL*/)
	: CDialog(CStationLinkerPopDlg::IDD, pParent)
{
m_iFirstStationIndex=_firstStationIndex;
m_iFirstStationPort= _firstStationPort;
m_pTempTrafficeGraph=_pGraph;
m_bHaveSelect=false;
m_currentItem=NULL;
}


void CStationLinkerPopDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CStationLinkerPopDlg)
	DDX_Control(pDX, IDC_TREE1, m_tree);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CStationLinkerPopDlg, CDialog)
	//{{AFX_MSG_MAP(CStationLinkerPopDlg)
	//ON_WM_CONTEXTMENU()
	//ON_COMMAND(ID_STATIONLINKER_POPDLG_SELECT, OnStationlinkerPopdlgSelect)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStationLinkerPopDlg message handlers

BOOL CStationLinkerPopDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	LoadTree();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CStationLinkerPopDlg::LoadTree()
{
	
	//HTREEITEM m_treeRoot=m_tree.InsertItem(" Select Station Port ");

	STATIONANDRALLWAY::const_iterator iter=m_pTempTrafficeGraph->GetTrafficGraph().begin();
	STATIONANDRALLWAY::const_iterator iterEnd=m_pTempTrafficeGraph->GetTrafficGraph().end();
	//CString sStationName;
	char stationName[256];
	int iLinkFromFirstType1;
	int iLinkFromFirstType2;
	int iLinkFromSecondType1;
	int iLinkFromSecondType2;
	if( m_iFirstStationPort == 1 )
	{
		iLinkFromFirstType1 = PORT1_PORT1;
		iLinkFromSecondType1 = PORT1_PORT2;

		iLinkFromFirstType2 = PORT1_PORT2;	
		iLinkFromSecondType2 = PORT2_PORT2;
		
		
	}
	else
	{
		iLinkFromFirstType1 = PORT2_PORT1;
		iLinkFromSecondType1 = PORT1_PORT1;

		iLinkFromFirstType2 = PORT2_PORT2;
		iLinkFromSecondType2 = PORT2_PORT1;
	}
	for(int i=0; iter!=iterEnd; ++iter,++i )
	{
		if(i!=m_iFirstStationIndex)
		{
			// we have a asumption that one station port can only link with another staiton once
			if( m_pTempTrafficeGraph->IsTwoStationDirectLinked( m_iFirstStationIndex, i, iLinkFromFirstType1 ) )
				continue;
			if( m_pTempTrafficeGraph->IsTwoStationDirectLinked( m_iFirstStationIndex, i, iLinkFromFirstType2 ) )
				continue;

			(*iter)->GetStation()->getID()->printID(stationName);
			HTREEITEM stationItem=m_tree.InsertItem( stationName);
			m_tree.SetItemData(stationItem,i);

			if( !m_pTempTrafficeGraph->IsTwoStationDirectLinked( i, m_iFirstStationIndex , iLinkFromSecondType1 ))
			{

				HTREEITEM port1Item=m_tree.InsertItem( " Port 1" ,stationItem);
				m_tree.SetItemData(port1Item,1);
				m_portVector.push_back(port1Item);
			}
			

			if( ! m_pTempTrafficeGraph->IsTwoStationDirectLinked( i, m_iFirstStationIndex, iLinkFromSecondType2 ) )
			{
				HTREEITEM port2Item=m_tree.InsertItem( " Port 2" ,stationItem);
				m_tree.SetItemData(port2Item,2);
				m_portVector.push_back(port2Item);

			}
			m_tree.Expand(stationItem,TVE_EXPAND);

		}
		
	}

	
}

/*
void CStationLinkerPopDlg::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	HTREEITEM _hRClickItem = NULL;

	CRect rectTree;
	m_tree.GetWindowRect( &rectTree );
	if( !rectTree.PtInRect(point) ) 
		return;

	m_tree.SetFocus();
	CPoint pt = point;
	m_tree.ScreenToClient( &pt );
	UINT iRet;
	_hRClickItem = m_tree.HitTest( pt, &iRet );
	if (iRet != TVHT_ONITEMLABEL)   // Must click on the label
	{
		_hRClickItem = NULL;
	}


	if(_hRClickItem)
	{

		TREEITEM::iterator iter=find(m_portVector.begin(),m_portVector.end(),_hRClickItem);
		if(iter!=m_portVector.end())//click on port item
		{
			CMenu menuPopup;
			menuPopup.LoadMenu( IDR_MENU_POPUP );
			
			m_currentItem=_hRClickItem;
			CMenu* pMenu = menuPopup.GetSubMenu( 12 );			
			m_bHaveSelect=true;
			pMenu->TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);	
			
		}
		else
		{
			m_bHaveSelect=false;
			return;
		}
			
	}

}

void CStationLinkerPopDlg::OnStationlinkerPopdlgSelect() 
{
	m_iSelectedStationPort=m_tree.GetItemData(m_currentItem);
	m_iSelectedStationIndex=m_tree.GetItemData(m_tree.GetParentItem(m_currentItem));
}

*/
void CStationLinkerPopDlg::OnOK() 
{

	m_currentItem=m_tree.GetSelectedItem();
	if(!m_currentItem)
	{
		AfxMessageBox("You must select a correct station port!");
		return;
	}

	TREEITEM::iterator iter=find(m_portVector.begin(),m_portVector.end(),m_currentItem);
	if(iter==m_portVector.end())//not click on port item
	{
		AfxMessageBox("You must select a correct station port!");
		return;
	}



	m_iSelectedStationPort=m_tree.GetItemData(m_currentItem);
	m_iSelectedStationIndex=m_tree.GetItemData(m_tree.GetParentItem(m_currentItem));

	/*
	if(!m_bHaveSelect)
	{
		AfxMessageBox("You must select a correct station port!");
		return;
	}
	*/
	CDialog::OnOK();
}
