#pragma once
#include "ObjectDisplay.h"

class CPortals;
class CAreas;
class WallShapeList;
class CStructureList;
class CPipeDataSet;




class COtherPlacement
{
	
public:
	COtherPlacement(void);
	~COtherPlacement(void);


public:
	void InitPlacement(CPortals *pPortals);
	void InitPlacement(CAreas *pPortals);
	void InitPlacement(WallShapeList *pWallShapes);
	void InitPlacement(CStructureList* pStructureList);
	void InitPlacement(CPipeDataSet* pPipes);

protected:
	CObjectDisplayList m_vPipeObjectDsp;
	CObjectDisplayList m_vStructureDsp;
	CObjectDisplayList m_vAreasDsp;
	CObjectDisplayList m_vPortalDsp;
	CObjectDisplayList m_vWallshapeDsp;


};
