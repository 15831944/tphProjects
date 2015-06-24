// AirsideIntersectionsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "AirsideLaneAdapterDlg.h"
#include "../InputAirside/Stretch.h"
#include "../common\WinMsg.h"

// CAirsideIntersectionsDlg dialog

IMPLEMENT_DYNAMIC(CAirsideLaneAdapterDlg, CAirsideObjectBaseDlg)
BEGIN_MESSAGE_MAP(CAirsideLaneAdapterDlg, CAirsideObjectBaseDlg)
END_MESSAGE_MAP()

CAirsideLaneAdapterDlg::CAirsideLaneAdapterDlg(int nLaneAdapterID ,int nAirportID,int nProjID,CWnd *pParent)
:CAirsideObjectBaseDlg(nLaneAdapterID,nAirportID, nProjID,pParent)
{
	m_pObject = new LaneAdapter();
	m_pObject->setID(nLaneAdapterID);
	m_pObject->setAptID(nAirportID);
	m_bPathModified = false;
}

// CAirsideLaneAdapterDlg::CAirsideLaneAdapterDlg(LaneAdapter * pLaneAdapter,int nProjID,CWnd * pParent /*= NULL*/ )
// : CAirsideObjectBaseDlg(pLaneAdapter, nProjID,pParent)
// {
// 	if(pLaneAdapter)
// 	{
// 		for(InsecObjectPartVector::const_iterator ctitrOBJPart = pLaneAdapter->GetIntersectionsPart().begin();\
// 	CAirsideLaneAdapterDlg(LaneAdapter * pLaneAdapter,int nProjID,CWnd * pParent = NULL);
CAirsideLaneAdapterDlg::~CAirsideLaneAdapterDlg(void)
{
	m_vrLinkStretches.clear();
}

void CAirsideLaneAdapterDlg::DoDataExchange(CDataExchange* pDX)
{
	CAirsideObjectBaseDlg::DoDataExchange(pDX);
}

BOOL CAirsideLaneAdapterDlg::OnInitDialog(void)
{
	CAirsideObjectBaseDlg::OnInitDialog();
	// If no bitmap is defined for the static control, define the bitmap 
	// to the system close bitmap.
	HBITMAP bitmap = ::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BITMAP_RUNWAY));

	ASSERT(bitmap);
	m_ImageStatic.SetBitmap( /*bitmap*/0 );

	if (m_nObjID != -1)
	{
		SetWindowText(_T("Modify LaneAdapter"));
	}
	else
	{
		SetWindowText(_T("Define LaneAdapter"));
	}
	
	if(GetObject()->getID() != -1)
	{
		((LaneAdapter *)GetObject())->ReadObject(GetObject()->getID());
		for(InsecObjectPartVector::const_iterator ctitrOBJPart = ((LaneAdapter *)GetObject())->GetIntersectionsPart().begin();\
			ctitrOBJPart != ((LaneAdapter *)GetObject())->GetIntersectionsPart().end();++ctitrOBJPart)
		{
			m_vrLinkStretches.push_back(*ctitrOBJPart);
		}
	}

	LoadTree();
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
void CAirsideLaneAdapterDlg::OnContextMenu(CMenu& menuPopup, CMenu *& pMenu)
{
	if(m_hRClickItem == m_hLinkStretches)
	{
		pMenu=menuPopup.GetSubMenu(73);
	}
}


LRESULT CAirsideLaneAdapterDlg::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{		
	return CAirsideObjectBaseDlg::DefWindowProc(message, wParam, lParam);
}

FallbackReason CAirsideLaneAdapterDlg::GetFallBackReason(void)
{	
	return PICK_MANYPOINTS;
}
void CAirsideLaneAdapterDlg::SetObjectPath(CPath2008& path)
{
	return;
}

void CAirsideLaneAdapterDlg::LoadTree(void)
{
	m_treeProp.DeleteAllItems();
	m_treeProp.SetRedraw(0);

	CString strLabel = _T("");	
	// control points.
	strLabel = _T( "Linking Stretches ");
	m_hLinkStretches  = m_treeProp.InsertItem(strLabel);

	Stretch stretch;
	for(InsecObjectPartVector::const_iterator ctitrOBJPart = m_vrLinkStretches.begin();ctitrOBJPart != m_vrLinkStretches.end();++ctitrOBJPart)
	{
		stretch.setID((*ctitrOBJPart).nALTObjectID);
		strLabel = stretch.ReadObjectByID((*ctitrOBJPart).nALTObjectID)->GetObjectName().GetIDString();
		strLabel.Format("%s:%d",strLabel,(*ctitrOBJPart).part);
		m_treeProp.InsertItem(strLabel , m_hLinkStretches);
	}

    m_treeProp.SetRedraw(1);
}


void CAirsideLaneAdapterDlg::OnOK(void)
{
	if(GetObject())
	{
		try
		{
			CADODatabase::BeginTransaction() ;
			((LaneAdapter *)GetObject())->SetIntersectionsPart(m_vrLinkStretches);
			if(-1 != GetObject()->getID())
			{
				((LaneAdapter *)GetObject())->UpdateObject(GetObject()->getID());
			}
			else
			{
				((LaneAdapter *)GetObject())->SaveObject(m_nAirportID);
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

bool CAirsideLaneAdapterDlg::UpdateOtherData(void)
{
	((LaneAdapter *)GetObject())->SetIntersectionsPart(m_vrLinkStretches);

	return (true);
}