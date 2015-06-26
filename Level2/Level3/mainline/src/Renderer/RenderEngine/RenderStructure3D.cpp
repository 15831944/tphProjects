#include "StdAfx.h"
#include "RenderStructure3D.h"
//////////////////////////////////////////////////////////////////////////
#include "CommonData/StructureBase.h"
#include <CommonData/Textures.h>
#include "Common/ARCPath.h"
#include "Common/ARCPipe.h"
#include "ShapeBuilder.h"

using namespace Ogre;

void CRenderStructure3D::Setup3D( CStructureBase* pStructure,int idx,CString texturefilename)
{

	if(false)
	{
		RemoveFromParent();
		return;
	}

	ManualObject* pObj = _GetOrCreateManualObject(GetName()); 
	if(pObj){

		CString strMaterial = OgreUtil::createOrRetrieveTextureMaterial(texturefilename).c_str();
		ARCPath3 path;
		path.reserve(pStructure->getPointNum());
		for(int i=0;i<(int)pStructure->getPointNum();i++)
		{
			Point pt= pStructure->getPointAt(i);
			path.push_back( ARCVector3(pt.getX(),pt.getY(),0) );
		}
		CShapeBuilder::DrawArea(pObj,strMaterial,path.getPath2008(),ARCColor3::White);
		AddObject(pObj);
	}
}
