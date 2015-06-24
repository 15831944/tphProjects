#include "StdAfx.h"
#include ".\otherplacement.h"
#include "../Inputs/AreasPortals.h"
#include "../Inputs/Structure.h"
#include "../Inputs/Structurelist.h"
#include "../Inputs/WallShape.h"
#include "../Inputs/WallShapeList.h"
#include "../Inputs/Pipe.h"
#include "../Inputs/PipeDataSet.h"

COtherPlacement::COtherPlacement(void)
{
}

COtherPlacement::~COtherPlacement(void)
{
}

//void COtherPlacement::InitPlacement()
//{
//
//}

void COtherPlacement::InitPlacement( CPortals *pPortals )
{
	CPortalList& lstPortals  = pPortals->m_vPortals;
	
	int nCount = static_cast<int>(lstPortals.size());

	for (int i =0; i < nCount; ++i)
	{
		CObjectDisplay *pObjectDisplay = new CObjectDisplay();
		CPortal *pPortal = lstPortals.at(i);
		pObjectDisplay->setBaseObject(pPortal);
		

		m_vPortalDsp.AddItem(pObjectDisplay);
	}

}

void COtherPlacement::InitPlacement( CAreas *pAreas )
{
	CAreaList & lstAreas = pAreas->m_vAreas;

	int nCount = static_cast<int>(lstAreas.size());

	for (int i =0; i < nCount; ++i)
	{
		CObjectDisplay *pObjectDisplay = new CObjectDisplay();
		CArea *pPortal = lstAreas.at(i);
		pObjectDisplay->setBaseObject(pPortal);


		m_vAreasDsp.AddItem(pObjectDisplay);
	}

}

void COtherPlacement::InitPlacement( WallShapeList *pWallShapes )
{
	int nCount = static_cast<int>(pWallShapes->getShapeNum());
	
	for (int i = 0; i < nCount; ++i)
	{
		WallShape *pWallShape = pWallShapes->getShapeAt(i);

		CObjectDisplay *pObjectDisplay = new CObjectDisplay();

		pObjectDisplay->setBaseObject(pWallShape);

		m_vWallshapeDsp.AddItem(pObjectDisplay);
	}

}

void COtherPlacement::InitPlacement( CStructureList* pStructureList )
{
	int nCount = static_cast<int>(pStructureList->getStructureNum());

	for (int i =0; i < nCount; ++i)
	{
		CStructure *pStructure = pStructureList->getStructureAt(i);

		CObjectDisplay *pObjectDisplay = new CObjectDisplay();

		pObjectDisplay->setBaseObject(pStructure);

		m_vStructureDsp.AddItem(pObjectDisplay);
	}

}

void COtherPlacement::InitPlacement( CPipeDataSet* pPipes )
{
	int nCount = pPipes->GetPipeCount();
	for (int i =0; i < nCount; ++i)
	{
	
		CPipeDisplay *pObjectDisplay = new CPipeDisplay();

		pObjectDisplay->setBaseObject(pPipes,i);

		m_vPipeObjectDsp.AddItem(pObjectDisplay);

	}

}





























