// ShapesBar.cpp: implementation of the CACComponentShapesBar class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ACComponentShapesBar.h"
#include "common\WinMsg.h"

#include <InputOnBoard/ComponentModel.h>
#include <InputOnBoard/AircraftComponentModelDatabase.h>

#include "ACComponentShapesManager.h"

#include <Common/OleDragDropManager.h>
#include <Common/BitmapSaver.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CACComponentShapesBar::CACComponentShapesBar()
{
	ACCOMPONENTSHAPESMANAGER.SetShapesBar(this);
}
CACComponentShapesBar::~CACComponentShapesBar()
{
	ACCOMPONENTSHAPESMANAGER.SetShapesBar(NULL);
}
BEGIN_MESSAGE_MAP(CACComponentShapesBar, CShapesBarBase)
	//{{AFX_MSG_MAP(CACComponentShapesBar)
	ON_MESSAGE(WM_SLB_SELCHANGED, OnSLBSelChanged)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CACComponentShapesBar message handlers

LRESULT CACComponentShapesBar::OnSLBSelChanged(WPARAM wParam/* CComponentMeshModel* */, LPARAM lParam)
{
	if (wParam)
	{
		CComponentMeshModel* pSelModel = (CComponentMeshModel*)(wParam);
		ASSERT(pSelModel != NULL);

		DragDropDataItem dataItem(DragDropDataItem::Type_AircraftComponentModel);
		dataItem.AddDragDropString(pSelModel->m_sModelName);
		dataItem.AddDragDropString(pSelModel->GetMeshFileName());
		dataItem.AddDragDropString(pSelModel->msTmpWorkDir);
		DistanceUnit dHeight = pSelModel->getHeight();
		dataItem.AddDragDropData(&dHeight, sizeof(DistanceUnit));
		OleDragDropManager::GetInstance().SetDragDropData(dataItem);
	}
	return 0;
}


void CACComponentShapesBar::ReloadContent(CAircraftComponentModelDatabase* pDB)
{
	ClearAllContent();

	m_wndOutBarCtrl.AddFolder(_T("Components"), 0);

	if (pDB)
	{
		for(int i=0;i<pDB->GetModelCount();i++)
		{
			CComponentMeshModel* pModel = pDB->GetModel(i);

			HBITMAP hBitmap = BitmapSaver::FromFile(pModel->msTmpWorkDir + _T("ThumbP.bmp"));
			if (hBitmap) // if bitmap not found, do not load the component
			{
				AddLargeImage(hBitmap, RGB(0,0,0));
				AddSmallImage(hBitmap, RGB(0,0,0));
				::DeleteObject(hBitmap);
				m_wndOutBarCtrl.InsertItem(0, -1, pModel->m_sModelName,i, (DWORD)pModel);
			}
		}
		m_wndOutBarCtrl.SetSelFolder(0);
	}
	else
	{
		ShowWindow(SW_HIDE);
	}
}

