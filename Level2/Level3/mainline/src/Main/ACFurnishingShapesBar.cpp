// ACFurnishingShapesBar.cpp: implementation of the CACFurnishingShapesBar class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ACFurnishingShapesBar.h"
#include "common\WinMsg.h"

#include "InputOnboard/AircraftFurnishingSection.h"
#include "ACFurnishingShapesManager.h"

#include <Common/OleDragDropManager.h>
#include <Common/BitmapSaver.h>
#include <vector>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CACFurnishingShapesBar::CACFurnishingShapesBar()
{
	ACFURNISHINGSHAPESMANAGER.SetShapesBar(this);
}
CACFurnishingShapesBar::~CACFurnishingShapesBar()
{
	ACFURNISHINGSHAPESMANAGER.SetShapesBar(NULL);
}
BEGIN_MESSAGE_MAP(CACFurnishingShapesBar, CShapesBarBase)
	//{{AFX_MSG_MAP(CACFurnishingShapesBar)
	ON_MESSAGE(WM_SLB_SELCHANGED, OnSLBSelChanged)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CACFurnishingShapesBar message handlers

LRESULT CACFurnishingShapesBar::OnSLBSelChanged(WPARAM wParam/* CComponentMeshModel* */, LPARAM lParam)
{
	if (wParam)
	{
		CAircraftFurnishingModel* pSelModel = (CAircraftFurnishingModel*)(wParam);
		ASSERT(pSelModel != NULL);

		DragDropDataItem dataItem(DragDropDataItem::Type_AircraftFurnishing);
		CString strPtr; strPtr.Format("%d",pSelModel);
		dataItem.AddDragDropString(strPtr);
		//dataItem.AddDragDropString(pSelModel->msMeshFileName);
		//dataItem.AddDragDropString(pSelModel->msTmpWorkDir);
		OleDragDropManager::GetInstance().SetDragDropData(dataItem);
	}
	return 0;
}


void CACFurnishingShapesBar::ReloadContent(CAircraftFurnishingSectionManager* pManager)
{
	ClearAllContent();

// 	m_wndOutBarCtrl.AddFolder(_T("Aircraft Furnishing"), 0);
	std::vector<CString> vecFolders;

	if (pManager)
	{
		for(int i=0;i<pManager->GetModelCount();i++)
		{
			CAircraftFurnishingModel* pModel = pManager->GetModel(i);
			std::vector<CString>::iterator ite = std::find(vecFolders.begin(), vecFolders.end(), pModel->m_Type);
			int nFolder = ite - vecFolders.begin();
			if (vecFolders.end() == ite)
			{
				m_wndOutBarCtrl.AddFolder(pModel->m_Type, 0);
				vecFolders.push_back(pModel->m_Type);
			}

			HBITMAP hBitmap = BitmapSaver::FromFile(pModel->msTmpWorkDir + _T("ThumbP.bmp"));
			if (hBitmap) // if bitmap not found, do not load the furnishing
			{
				AddLargeImage(hBitmap, RGB(0,0,0));
				AddSmallImage(hBitmap, RGB(0,0,0));
				::DeleteObject(hBitmap);
				m_wndOutBarCtrl.InsertItem(nFolder, -1, pModel->m_sModelName,i, (DWORD)pModel);
			}
		}
		if (vecFolders.size())
			m_wndOutBarCtrl.SetSelFolder(0);
	}
	else
	{
		ShowWindow(SW_HIDE);
	}
}

