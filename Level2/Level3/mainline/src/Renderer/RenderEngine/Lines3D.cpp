#include "StdAfx.h"
#include ".\lines3d.h"
#include "MaterialDef.h"
#include "OgreConvert.h"


using namespace Ogre;

#define STR_LINE_MAT _T("GridLine")

static void Build( ManualObject* pLinesObj, const CLines3D::VertexList& vertexList, ARCColor3 lineColor, float fAlpha )
{
	pLinesObj->clear();

	pLinesObj->begin( STR_LINE_MAT ,RenderOperation::OT_LINE_STRIP);
	pLinesObj->colour(OgreConvert::GetColor(lineColor, fAlpha));

	for (CLines3D::VertexList::const_iterator ite = vertexList.begin();ite != vertexList.end();ite++)
	{
		pLinesObj->position(ite->x, ite->y, ite->z);
	}
	pLinesObj->end();
	pLinesObj->setRenderQueueGroup(RENDER_QUEUE_6);
}

void CLines3D::Update( const VertexList& vertexList, ARCColor3 lineColor /*= ARCColor3::BLACK*/, float fAlpha /*= 1.0f*/ )
{
	SceneManager* pScene = GetScene();
	ManualObject* pObj = OgreUtil::createOrRetrieveManualObject( GetIDName(),pScene);
	Build(pObj, vertexList, lineColor, 1.0f);	
	pObj->setVisibilityFlags(NormalVisibleFlag);
	OgreUtil::AttachMovableObject(pObj,mpNode);

}
