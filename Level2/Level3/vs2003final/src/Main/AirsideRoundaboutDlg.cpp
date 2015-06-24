// AirsideRoundaboutDlg.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"

#include "../AirsideGUI/UnitPiece.h"
#include "../InputAirside/ARCUnitManagerOwnerEnum.h"
#include "../InputAirside/Stretch.h"
#include "AirsideRoundaboutDlg.h"
#include "../common\WinMsg.h"

// CAirsideRoundaboutDlg

IMPLEMENT_DYNAMIC(CAirsideRoundaboutDlg, CAirsideObjectBaseDlg)
BEGIN_MESSAGE_MAP(CAirsideRoundaboutDlg, CAirsideObjectBaseDlg)
	ON_COMMAND(ID_ALTOBJECTNUMBER_DEFINENUMBER,OnDefineNumber)
	ON_COMMAND(ID_ALTOBJECTWIDTH_DEFINEWIDTH,OnDefineWidth)
END_MESSAGE_MAP()

CAirsideRoundaboutDlg::CAirsideRoundaboutDlg(int nRoundAboutID ,int nAirportID,int nProjID,CWnd *pParent)
:CAirsideObjectBaseDlg(nRoundAboutID,nAirportID,nProjID,pParent)
{
	m_pObject = new Roundabout();
	m_pObject->setID(nRoundAboutID);
	m_pObject->setAptID(nAirportID);
	m_bPathModified = false;
}

// CAirsideRoundaboutDlg::CAirsideRoundaboutDlg(Roundabout * pRoundAbout,int nProjID,CWnd * pParent /*= NULL*/ )
// : CAirsideObjectBaseDlg(pRoundAbout,nProjID,pParent)
// {
// 	if(pRoundAbout)
// 	{
// 		for(InsecObjectPartVector::const_iterator ctitrOBJPart = pRoundAbout->GetIntersectionsPart().begin();\
// 			ctitrOBJPart != pRoundAbout->GetIntersectionsPart().end();++ctitrOBJPart)
// 		{
// 			m_vrLinkStretches.push_back(*ctitrOBJPart);
// 		}
// 	}	
// 
// 	m_bPathModified = false;
// }
CAirsideRoundaboutDlg::~CAirsideRoundaboutDlg(void)
{
	m_vrLinkStretches.clear();
}

void CAirsideRoundaboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CAirsideObjectBaseDlg::DoDataExchange(pDX);
}

BOOL CAirsideRoundaboutDlg::OnInitDialog(void)
{
	CAirsideObjectBaseDlg::OnInitDialog();
	// If no bitmap is defined for the static control, define the bitmap 
	// to the system close bitmap.
	HBITMAP bitmap = ::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BITMAP_RUNWAY));

	ASSERT(bitmap);
	m_ImageStatic.SetBitmap(/* bitmap */0);

	if (m_nObjID != -1)
	{
		SetWindowText(_T("Modify Roundabout"));
	}
	else
	{
		SetWindowText(_T("Define Roundabout"));
	}
	
	m_dLaneWidth = 8;
	m_iLaneNum   = 1;
	if(GetObject()->getID() != -1)
	{
		((Roundabout *)GetObject())->ReadObject(GetObject()->getID());
		m_dLaneWidth = ((Roundabout *)GetObject())->GetLaneWidth();
		m_iLaneNum   = ((Roundabout *)GetObject())->GetLaneNumber();
		for(InsecObjectPartVector::const_iterator ctitrOBJPart = ((Roundabout *)GetObject())->GetIntersectionsPart().begin();\
			ctitrOBJPart != ((Roundabout *)GetObject())->GetIntersectionsPart().end();++ctitrOBJPart)
		{
			m_vrLinkStretches.push_back(*ctitrOBJPart);
		}
	}

	/////
	//initialize unit manager
	CUnitPiece::InitializeUnitPiece(m_nProjID,UM_ID_6,this);
	CRect rectItem;
	GetDlgItem(IDOK)->GetWindowRect(rectItem);
	ScreenToClient(rectItem);
	int yPos = rectItem.top;
	m_treeProp.GetWindowRect(rectItem);
	ScreenToClient(rectItem);
	int xPos = rectItem.left;
	CUnitPiece::MoveUnitButton(xPos,yPos);
	SetResize(XIAOHUABUTTON_ID, SZ_BOTTOM_LEFT, SZ_BOTTOM_LEFT);
	CUnitPiece::SetUnitInUse(ARCUnit_Length_InUse);
	/////

	LoadTree();
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CAirsideRoundaboutDlg::OnDoubleClickPropTree(HTREEITEM hTreeItem)
{
	if(hTreeItem == m_hLaneNumber)
	{
		m_hRClickItem = hTreeItem;
		OnDefineNumber();
	}

	if(hTreeItem == m_hLaneWidth)
	{
		m_hRClickItem = hTreeItem;
		OnDefineWidth();
	}
}

void CAirsideRoundaboutDlg::OnContextMenu(CMenu& menuPopup, CMenu *& pMenu)
{
	if(m_hRClickItem == m_hLinkStretches)
	{
		pMenu=menuPopup.GetSubMenu(73);
	}
	else if (m_hRClickItem == m_hLaneNumber)
	{
		pMenu = menuPopup.GetSubMenu(75);
	}
	else if (m_hRClickItem == m_hLaneWidth)
	{
		pMenu = menuPopup.GetSubMenu(76);
	}
}


LRESULT CAirsideRoundaboutDlg::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{		
	if( message == WM_INPLACE_SPIN && GetObject())
	{
		LPSPINTEXT pst = (LPSPINTEXT) lParam;
		if (m_hRClickItem == m_hLaneWidth)
		{			
			m_dLaneWidth = ( CARCLengthUnit::ConvertLength(CUnitPiece::GetCurSelLengthUnit(),DEFAULT_DATABASE_LENGTH_UNIT,pst->iPercent) );
		}
		else if (m_hRClickItem == m_hLaneNumber)
		{
			m_iLaneNum = (pst->iPercent);			
		}
		LoadTree();
		return TRUE;
	}
	return CAirsideObjectBaseDlg::DefWindowProc(message, wParam, lParam);
}

FallbackReason CAirsideRoundaboutDlg::GetFallBackReason(void)
{	
	return PICK_MANYPOINTS;
}
void CAirsideRoundaboutDlg::SetObjectPath(CPath2008& path)
{
	return;
}

void CAirsideRoundaboutDlg::LoadTree(void)
{
	m_treeProp.DeleteAllItems();
	m_treeProp.SetRedraw(0);

	CString strLabel = _T("");	
	AirsideObjectTreeCtrlItemDataEx aoidDataEx;
	ItemStringSectionColor isscStringColor;
	CString strTemp = _T("");
	HTREEITEM hItemTemp = 0;

	//number of lanes.
	strLabel.Format(_T("Number of Lanes : %d"), m_iLaneNum );
	aoidDataEx.lSize = sizeof(aoidDataEx);
	aoidDataEx.dwptrItemData = 0;
	aoidDataEx.vrItemStringSectionColorShow.clear();
	isscStringColor.colorSection = RGB(0,0,255); 
	strTemp.Format(_T("%d"),m_iLaneNum);
	isscStringColor.strSection = strTemp;
	aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);
	m_hLaneNumber = m_treeProp.InsertItem(strLabel);
	m_treeProp.SetItemDataEx(m_hLaneNumber,aoidDataEx);

	//lane width.
	strLabel.Format(_T("Lane Width: %.2f (%s) "),CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),m_dLaneWidth) ,\
		 CARCLengthUnit::GetLengthUnitString(CUnitPiece::GetCurSelLengthUnit()) );
	aoidDataEx.lSize = sizeof(aoidDataEx);
	aoidDataEx.dwptrItemData = 0;
	aoidDataEx.vrItemStringSectionColorShow.clear();
	isscStringColor.colorSection = RGB(0,0,255); 
	strTemp.Format(_T("%.2f"),CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),m_dLaneWidth));
	isscStringColor.strSection = strTemp;
	aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);
	m_hLaneWidth = m_treeProp.InsertItem(strLabel);
	m_treeProp.SetItemDataEx(m_hLaneWidth,aoidDataEx);

	// control points.
	strLabel = _T( "Linking Stretches ");
	m_hLinkStretches  = m_treeProp.InsertItem(strLabel);

	Stretch stretch;
	for(InsecObjectPartVector::const_iterator ctitrOBJPart = m_vrLinkStretches.begin();ctitrOBJPart != m_vrLinkStretches.end();++ctitrOBJPart)
	{
		stretch.setID((*ctitrOBJPart).nALTObjectID);
		strLabel = stretch.ReadObjectByID((*ctitrOBJPart).nALTObjectID)->GetObjectName().GetIDString();
		strLabel.Format("%s:%d",strLabel,(*ctitrOBJPart).part);

		aoidDataEx.lSize = sizeof(aoidDataEx);
		aoidDataEx.dwptrItemData = 0;
		aoidDataEx.vrItemStringSectionColorShow.clear();
		isscStringColor.colorSection = RGB(0,0,255); 
		isscStringColor.strSection = strLabel;
		aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor);

		strTemp.Format(_T("%d"),(*ctitrOBJPart).part);		
		isscStringColor.strSection = strTemp;
		aoidDataEx.vrItemStringSectionColorShow.push_back(isscStringColor); 

		hItemTemp = m_treeProp.InsertItem(strLabel , m_hLinkStretches);
		m_treeProp.SetItemDataEx(hItemTemp,aoidDataEx);
	}

    m_treeProp.SetRedraw(1);
}


void CAirsideRoundaboutDlg::OnOK(void)
{
	if(GetObject())
	{
		m_iLaneNum   = _tstoi(m_treeProp.GetItemText(m_hLaneNumber));
		m_dLaneWidth = _tstof(m_treeProp.GetItemText(m_hLaneWidth));
		((Roundabout *)GetObject())->SetLaneWidth(m_dLaneWidth);
		((Roundabout *)GetObject())->SetLaneNumber(m_iLaneNum);
		((Roundabout *)GetObject())->SetIntersectionsPart(m_vrLinkStretches);

		try
		{
			CADODatabase::BeginTransaction() ;
			if(-1 != GetObject()->getID())
			{
				((Roundabout *)GetObject())->UpdateObject(GetObject()->getID());
			}
			else
			{
				((Roundabout *)GetObject())->SaveObject(m_nAirportID);
			}
			CADODatabase::CommitTransaction() ;
		}
		catch (CADOException e)
		{
			CADODatabase::RollBackTransation() ;
		}

	}

	CAirsideObjectBaseDlg::OnOK();
}

bool CAirsideRoundaboutDlg::UpdateOtherData(void)
{
	((Roundabout *)GetObject())->SetIntersectionsPart(m_vrLinkStretches);
	((Roundabout *)GetObject())->SetLaneWidth(m_dLaneWidth);
	((Roundabout *)GetObject())->SetLaneNumber(m_iLaneNum);

	return (true);
}
void CAirsideRoundaboutDlg::OnDefineNumber(void)
{
	m_treeProp.SetDisplayType( 2 ); 
	m_treeProp.SetDisplayNum(m_iLaneNum);
	m_treeProp.SetSpinRange( 0,40);
	m_treeProp.SpinEditLabel( m_hRClickItem );
}
void CAirsideRoundaboutDlg::OnDefineWidth(void)
{
	m_treeProp.SetDisplayType( 2 );
	m_treeProp.SetDisplayNum( static_cast<int>(CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),m_dLaneWidth)) );
	m_treeProp.SetSpinRange( 0,360);
	m_treeProp.SpinEditLabel( m_hRClickItem );
} 

bool CAirsideRoundaboutDlg::ConvertUnitFromDBtoGUI(void)
{
	//it do nothing , so return (false);
	return (false);
}

bool CAirsideRoundaboutDlg::RefreshGUI(void)
{
	LoadTree();
	return (true);
}

bool CAirsideRoundaboutDlg::ConvertUnitFromGUItoDB(void)
{
	//it do nothing , so return (false);
	return (false);
}
