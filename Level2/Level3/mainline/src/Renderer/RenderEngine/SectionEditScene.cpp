#include "StdAfx.h"
#include ".\sectioneditscene.h"
#include <common\ModelGrid.h>
#include <inputonboard\ComponentModel.h>
#include "Grid3D.h"


using namespace Ogre;
CSectionEditScene::CSectionEditScene(void)
{
}


void CSectionEditScene::Setup( CComponentMeshModel* pModel,CComponentMeshSection* pSection )
{
	if(!MakeSureInited())
		return;

	//set up light
	mpScene->setAmbientLight(ColourValue::Black);	


	mGrid = AddSceneNode(_T("Grid"));
	UpdateGrid(pModel);
}

void CSectionEditScene::UpdateGrid( CComponentMeshModel* pModel )
{
	CModelGrid& grid = pModel->mGrid;
	mGrid.Update(grid,CGrid3D::YZExtent,true);
}