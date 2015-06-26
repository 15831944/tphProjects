#pragma once
#include "3dscene.h"
#include "Grid3D.h"
#include "SimpleMesh3D.h"

class CComponentMeshModel;
class RENDERENGINE_API CMeshXZEditScene :
	public C3DSceneBase
{
public:
	CMeshXZEditScene(void);
	~CMeshXZEditScene(void);

	void Setup(CComponentMeshModel* pModel); //setup scene camera
	void OnModelChange( CComponentMeshModel* pModel );

	void UpdateGridCenterLine(CComponentMeshModel* pModel);	
	CCenterLine3D mCenerLine;
	CGrid3D mGrid;
};
