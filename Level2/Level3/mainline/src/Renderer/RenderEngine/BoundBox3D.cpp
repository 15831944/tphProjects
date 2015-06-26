#include "StdAfx.h"
#include "BoundBox3D.h"
#include "OgreConvert.h"

using namespace Ogre;
#define BOUNDBOX_LINE_MAT _T("BoundBoxLine")

static void Build(const CBoundBox3D::Column& col, ManualObject* pObj)
{
	ColourValue clr(0.0f, 0.0f, 0.0f, 1.0f);
	pObj->clear();
	for(size_t i=0;i<col.size();i++)
	{
		const CBoundBox3D::CrossSection& sec = col[i];
		pObj->begin(BOUNDBOX_LINE_MAT, RenderOperation::OT_LINE_STRIP);
		for(size_t j=0;j<sec.size();j++)
		{
			const ARCVector3& pos = sec[j];
			pObj->position(pos.x, pos.y, pos.z);
			pObj->colour(clr);
		}
		pObj->end();
	}

	size_t nSections = col.size();
	if (0 == nSections)
		return;

	size_t nPointsPerSection = col[0].size();
	for(size_t j=0;j<nPointsPerSection;j++)
	{
		pObj->begin(BOUNDBOX_LINE_MAT, RenderOperation::OT_POINT_LIST);
		for(size_t i=0;i<nSections;i++)
		{
			const ARCVector3& pos = col[i][j];
			pObj->position(pos.x, pos.y, pos.z);
			pObj->colour(clr);
		}
		pObj->end();
	}
}

void CBoundBox3D::Update() const
{
	SceneManager* pScene = GetScene();
	ManualObject* pObj = OgreUtil::createOrRetrieveManualObject(mpNode->getName().c_str(),pScene);
	Build(m_column, pObj);
	pObj->setVisibilityFlags(NormalVisibleFlag);
	OgreUtil::AttachMovableObject(pObj,mpNode);
}