#include "StdAfx.h"
#include "RenderWallShape3D.h"
#include "CommonData/WallShapeBase.h"
#include "Common/ARCPath.h"
#include "ShapeBuilder.h"
#include "Common/ARCPipe.h"
using namespace Ogre;

void CRenderWallShape3D::Setup3D( WallShapeBase* pWallShape, int idx )
{
	ASSERT(pWallShape);

	if(!pWallShape->m_dispProperties.bDisplay[WSDP_DISP_SHAPE])
	{
		RemoveFromParent();
		return;
	}

	ManualObject* pObj = _GetOrCreateManualObject(GetName()); 
	if(pObj){
		ARCPath3 path;
		path.reserve(pWallShape->GetPointCount());
		for(int i=0;i<(int)pWallShape->GetPointCount();i++)
		{
			Point pt= pWallShape->GetPointAt(i);
			path.push_back( ARCVector3(pt.getX(),pt.getY(),0) );
		}
		ARCColor3& color = pWallShape->m_dispProperties.color[WSDP_DISP_SHAPE];
		MaterialPtr pmat = OgreUtil::createOrRetrieveColorMaterial(color);
		CShapeBuilder::DrawWallShape(pObj,pmat->getName().c_str(),ARCPipe(path,pWallShape->GetWidth()),pWallShape->GetHeight());
		AddObject(pObj);
	}
	///UpdateSelected(GetSelected());
}
