#pragma once
#include "3dscene.h"
#include "Grid3D.h"

class CComponentMeshSection;
class CComponentMeshModel;
class RENDERENGINE_API CSectionEditScene : public C3DSceneBase
{
public:
	CSectionEditScene(void);

	//setup scene
	void Setup(CComponentMeshModel* pModel,CComponentMeshSection* pSection);	
	//
	void UpdateGrid(CComponentMeshModel* pModel);
	//

	CGrid3D mGrid;
};
