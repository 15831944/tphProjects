// AirsideIntersectionsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "AirsideIntersectionsDlg.h"
#include "../InputAirside/Stretch.h"
#include "../common\WinMsg.h"

// CAirsideIntersectionsDlg dialog

IMPLEMENT_DYNAMIC(CAirsideIntersectionsDlg, CAirsideObjectBaseDlg)
BEGIN_MESSAGE_MAP(CAirsideIntersectionsDlg, CAirsideObjectBaseDlg)
END_MESSAGE_MAP()

CAirsideIntersectionsDlg::CAirsideIntersectionsDlg(int nIntersectionsID ,int nAirportID,int nProjID,CWnd *pParent)
:CAirsideObjectBaseDlg(nIntersectionsID,nAirportID, nProjID,pParent)
{
	m_pObject = new Intersections();
	m_pObject->setID(nIntersectionsID);
	m_pObject->setAptID(nAirportID);
	m_bPathModified = false;
}

//CAirsideIntersectionsDlg::CAirsideIntersectionsDlg(Intersections * pIntersections,int nProjID,CWnd * pParent /*= NULL*/ )
//: CAirsideObjectBaseDlg(pIntersections, nProjID,pParent)
//{
//	if(pIntersections)
//	{
//		m_vrLinkStretches.clear();
//		for(InsecObjectPartVector::const_iterator ctitrOBJPart = pIntersections->GetIntersectionsPart().begin();\
//			ctitrOBJPart != pIntersections->GetIntersectionsPart().end();++ctitrOBJPart)
//		{
//			m_vrLinkStretches.push_back(*ctitrOBJPart);
//		}
//	}	
//
//	m_bPathModified = false;
//}

CAirsideIntersectionsDlg::~CAirsideIntersectionsDlg(void)
{
	m_vrLinkStretches.clear();
}

void CAirsideIntersectionsDlg::DoDataExchange(CDataExchange* pDX)
{
	CAirsideObjectBaseDlg::DoDataExchange(pDX);
}

BOOL CAirsideIntersectionsDlg::OnInitDialog(void)
{
	CAirsideObjectBaseDlg::OnInitDialog();
	// If no bitmap is defined for the static control, define the bitmap 
	// to the system close bitmap.
	HBITMAP bitmap = ::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BITMAP_RUNWAY));

	ASSERT(bitmap);
	m_ImageStatic.SetBitmap(/* bitmap*/0 );

	if (m_nObjID != -1)
	{
		SetWindowText(_T("Modify Intersections"));
	}
	else
	{
		SetWindowText(_T("Define Intersections"));
	}
	
	if(GetObject()->getID() != -1)
	{
		((Intersections *)GetObject())->ReadObject(GetObject()->getID());
		m_vrLinkStretches.clear();
		for(InsecObjectPartVector::const_iterator ctitrOBJPart = ((Intersections *)GetObject())->GetIntersectionsPart().begin();\
			ctitrOBJPart != ((Intersections *)GetObject())->GetIntersectionsPart().end();++ctitrOBJPart)
		{
			m_vrLinkStretches.push_back(*ctitrOBJPart);
		}
	}

	LoadTree();
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
void CAirsideIntersectionsDlg::OnContextMenu(CMenu& menuPopup, CMenu *& pMenu)
{
	if(m_hRClickItem == m_hLinkStretches)
	{
		pMenu=menuPopup.GetSubMenu(73);
	}
}


LRESULT CAirsideIntersectionsDlg::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{		
	return CAirsideObjectBaseDlg::DefWindowProc(message, wParam, lParam);
}

FallbackReason CAirsideIntersectionsDlg::GetFallBackReason(void)
{	
	return PICK_STRETCH_POS;
}
void CAirsideIntersectionsDlg::SetObjectPath(CPath2008& path)
{
	return;
}

void CAirsideIntersectionsDlg::LoadTree(void)
{
	m_treeProp.DeleteAllItems();
	m_treeProp.SetRedraw(0);

	CString strLabel = _T("");	
	// control points.
	strLabel = _T( "Linking Stretches ");
	m_hLinkStretches  = m_treeProp.InsertItem(strLabel);

	
	for(InsecObjectPartVector::iterator ctitrOBJPart = m_vrLinkStretches.begin();ctitrOBJPart != m_vrLinkStretches.end();++ctitrOBJPart)
	{		
		strLabel.Format("%s",(*ctitrOBJPart).GetObject()->GetObjNameString());
		m_treeProp.InsertItem(strLabel , m_hLinkStretches);
	}
	m_treeProp.Expand(m_hLinkStretches,TVE_EXPAND);

    m_treeProp.SetRedraw(1);
}


void CAirsideIntersectionsDlg::OnOK(void)
{
	if(GetObject())
	{
		((Intersections *)GetObject())->SetIntersectionsPart(m_vrLinkStretches);
		/*if(-1 != GetObject()->getID())
		{
			((Intersections *)GetObject())->UpdateObject(GetObject()->getID());
		}
		else
		{
			((Intersections *)GetObject())->SaveObject(m_nAirportID);
		}*/
	}

	CAirsideObjectBaseDlg::OnOK();
}

bool CAirsideIntersectionsDlg::UpdateOtherData(void)
{
	return (true);
}

bool CAirsideIntersectionsDlg::DoTempFallBackFinished( WPARAM wParam, LPARAM lParam )
{
	if(GetFallBackReason() == PICK_STRETCH_POS)
	{		
		InsecObjectPartVector* pData = reinterpret_cast< InsecObjectPartVector* >(wParam);
		//remove duplicate selections
		///*InsecObjectPartVector cleanData;
		//for(int i=0;i<(int)pData->size();i++)
		//{	
		//	InsecObjectPart& newItem = (*pData)[i];
		//	for(int j =0;j<(int)cleanData.size();j++)
		//	{
		//		InsecObjectPart& cleanItem = cleanData[j];
		//		if(newItem.GetObjectID() == cleanItem.GetObjectID())
		//		{
		//			break;
		//		}
		//	}

		//}*/

		if(GetObject())
		{
			//((Intersections *)GetObject())->SetIntersectionsPart(*pData);
			m_vrLinkStretches = *pData ;
			LoadTree();
			m_bPropModified = TRUE;
		}
	}
	return true;
}