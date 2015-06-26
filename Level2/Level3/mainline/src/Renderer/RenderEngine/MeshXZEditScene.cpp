#include "StdAfx.h"
#include ".\meshxzeditscene.h"

#include "../../InputOnBoard/ComponentModel.h"

using namespace Ogre;

CMeshXZEditScene::CMeshXZEditScene(void)
{
}

CMeshXZEditScene::~CMeshXZEditScene(void)
{
}

#define GridName _T("Grid")
#define CenterLineName _T("CenterLine")
#define EditModelName _T("EditModel")


void CMeshXZEditScene::Setup( CComponentMeshModel* pModel )
{
	if(!MakeSureInited())
	{
		ASSERT(FALSE);
		return;
	}	
	//set up light
	mpScene->setAmbientLight(ColourValue(0.2,0.2,0.2));


	//set up camera	
	{		
		CModelGrid& grid = pModel->mGrid;
		double dHeight = pModel->getHeight();
		mGrid = AddSceneNode(GridName);
		mCenerLine = AddSceneNode(CenterLineName);		
		//setup up center line
		UpdateGridCenterLine(pModel);
	}


	//add model to scene
	AddSceneNodeTo(EditModelName,CenterLineName);
	//UpdateModel(pModel);

	//add model sections to scene	
}

void CMeshXZEditScene::UpdateGridCenterLine( CComponentMeshModel* pModel )
{
	double dHeight = pModel->getHeight();
	double dFrom = -0.5*pModel->mGrid.dSizeX;
	double dTo = -dFrom;
	mGrid.Update(pModel->mGrid,CGrid3D::XZExtent,true);
	mCenerLine.Update(*this, dFrom,dTo);
	mCenerLine.SetHeight(dHeight);
}