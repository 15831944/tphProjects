#include "StdAfx.h"
#include ".\RenderSurface3D.h"
#include "SelectionManager.h"
#include "Render3DScene.h"
#include "OgreConvert.h"
#include "InputAirside/Surface.h"
#include "Boundbox3D.h"
#include "InputAirside/ALTObjectDisplayProp.h"
#include "InputAirside/InputAirside.h"
#include "InputAirside/ALTAirport.h"
#include ".\MaterialDef.h"
#include "ShapeBuilder.h"
using namespace Ogre;


//CRenderSurface3D::CRenderSurface3D( Ogre::SceneNode* pNode, CRender3DScene* p3DScene, const CGuid& guid )
//:CAirside3DObject(pNode,p3DScene,guid )
//{
//
//}
//
//CRenderSurface3D::~CRenderSurface3D(void)
//{
//}

void CRenderSurface3D::Update3D( ALTObject* pBaseObj )
{
	if(pBaseObj == NULL || pBaseObj->GetType()  != ALT_SURFACE)
		return;

	//SceneManager* pScene = GetScene();
	Ogre::ManualObject* pObj = _GetOrCreateManualObject(GetName());//OgreUtil::createOrRetrieveManualObject(GetIDName(),pScene);

	Build(pBaseObj,pObj);

	AddObject(pObj,true);
	//UpdateSelected(GetSelected());
}

void CRenderSurface3D::UpdateSelected( bool b )
{

}

 void CRenderSurface3D::Build( ALTObject* pBaseObject, Ogre::ManualObject* pObj )
 {
 	if(pBaseObject == NULL || pBaseObject->GetType() != ALT_SURFACE)
 		return;
 
	Surface *pSurface = (Surface *)pBaseObject;
	if(pSurface == NULL)
		return;
 
	ALTObjectDisplayProp *pDisplay = pSurface->getDisplayProp();
	ASSERT(pDisplay != NULL);
	ARCColor3 color = (COLORREF)(pDisplay->m_dpShape.cColor);
	CString strText = _T("SurfaceArea/")+pSurface->GetTexture();
	strText.MakeLower();
	CShapeBuilder::DrawArea(pObj,strText,pSurface->GetPath(),color);
 
 }

 void CRenderSurface3D::Setup3D( ALTObject* pBaseObj)
 {
	Update3D(pBaseObj);
 }

 

