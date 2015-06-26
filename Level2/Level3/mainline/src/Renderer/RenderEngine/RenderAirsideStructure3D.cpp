#include "StdAfx.h"
#include "SelectionManager.h"
#include "Render3DScene.h"
#include "OgreConvert.h"
#include "InputAirside/Surface.h"
#include "Boundbox3D.h"
#include "InputAirside/ALTObjectDisplayProp.h"
#include "InputAirside/Structure.h"
#include "InputAirside/InputAirside.h"
#include "InputAirside/ALTAirport.h"
#include ".\MaterialDef.h"
#include "ShapeBuilder.h"
#include "minmax.h"
#include "Common/IARCportLayoutEditContext.h"
#include "RenderAirsideStructure3D.h"
#include "InputAirside/ALTAirportLayout.h"
using namespace Ogre;

//CRenderAirsideStructure3D::CRenderAirsideStructure3D( Ogre::SceneNode* pNode, CRender3DScene* p3DScene, const CGuid& guid )
//:CAirside3DObject(pNode,p3DScene,guid )
//{
//
//}
//
//CRenderAirsideStructure3D::~CRenderAirsideStructure3D(void)
//{
//}

void CRenderAirsideStructure3D::UpdateSelected( bool b )
{

}
void CRenderAirsideStructure3D::Update3D( ALTObject* pBaseObj )
{
	if(pBaseObj == NULL || pBaseObj->GetType()  != ALT_STRUCTURE)
		return;

	//SceneManager* pScene = GetScene();
	Ogre::ManualObject* pObj =_GetOrCreateManualObject(GetName());// OgreUtil::createOrRetrieveManualObject(GetIDName(),pScene);

	Build(pBaseObj,pObj);

	AddObject(pObj,true);
	//UpdateSelected(GetSelected());
}

void CRenderAirsideStructure3D::Build( ALTObject* pBaseObject, Ogre::ManualObject* pObj )
{
	if(pBaseObject == NULL || pBaseObject->GetType() != ALT_STRUCTURE)
		return;

	Structure *pStructure = (Structure *)pBaseObject;
	if(pStructure == NULL)
		return;

	const CPath2008& structPath = pStructure->GetPath();
	int nPtCount = structPath.getCount();
	ASSERT(nPtCount == pStructure->getUserDataCount());
	ASSERT(nPtCount+1 == pStructure->getStructFaceCount());

	InputAirside *pAirside =  GetRoot().GetModel()->OnQueryInputAirside();
	ALTAirportLayout *pAirport = pAirside->GetActiveAirport();
	DistanceUnit dALt = pAirport->getElevation();
	int nVertexAdd =0;
	for(int i =0 ;i< structPath.getCount(); i++)
	{
		CPoint2008 pointI = structPath.getPoint(i);
		CPoint2008 pointNext = structPath.getPoint( (i+1)%structPath.getCount() );

		UserInputData * pUseInput = pStructure->getUserData(i);
		UserInputData * pUseInputNext = pStructure->getUserData( (i+1)%structPath.getCount() );
		StructFace* pFace = pStructure->getStructFace(i);

		CString matName(_T(""));
		matName.Format(_T("%s%dFace"),pStructure->GetObjNameString(),i);
		const Ogre::String& strMat = UpdateTexture(matName,pStructure,pFace);

		pObj->begin(strMat,RenderOperation::OT_TRIANGLE_LIST);
		
	
		
		ARCVector3 vDir = pointNext - pointI;
		vDir.Normalize();

		pObj->position(pointI.getX(), pointI.getY(), pUseInput->dElevation - dALt);
		pObj->normal(vDir[VY], -vDir[VX],0);
		pObj->textureCoord(0,1);

		pObj->position(pointI.getX(), pointI.getY(), pUseInput->dElevation + pUseInput->dStructHeight - dALt);
		pObj->textureCoord(0,0);

		pObj->position(pointNext.getX(), pointNext.getY(),  pUseInputNext->dElevation + pUseInputNext->dStructHeight - dALt);
		pObj->textureCoord(1,0);

		pObj->position(pointNext.getX(), pointNext.getY(),  pUseInputNext->dElevation - dALt);
		pObj->textureCoord(1,1);

		pObj->quad(3,2,1,0);
		pObj->end();
	}

	ARCVector3 minPt, maxPt;
	minPt = structPath.getPoint(0);
	maxPt = structPath.getPoint(0);
	for(int i =1;i< structPath.getCount();i++)
	{
		ARCVector3 pt = structPath.getPoint(i);
		minPt[VX] = min( minPt[VX],pt[VX] );
		minPt[VY] = min( minPt[VY],pt[VY] );
		minPt[VZ] = min( minPt[VZ],pt[VZ] );

		maxPt[VX] = max(maxPt[VX], pt[VX] );
		maxPt[VY] = max(maxPt[VY], pt[VY] );
		maxPt[VZ] = max(maxPt[VZ], pt[VZ] );
	}

	//render top

	StructFace* pFace = pStructure->getStructFace(nPtCount);

	CString matName(_T(""));
	matName.Format(_T("%sTopFace"),pStructure->GetObjNameString());
	const Ogre::String& strMat = UpdateTexture(matName,pStructure,pFace);
	pObj->begin(strMat,RenderOperation::OT_TRIANGLE_FAN);
	for(int i =0;i< structPath.getCount();i++)
	{
		CPoint2008 pointI = structPath.getPoint(i);
		UserInputData * pUseInput = pStructure->getUserData(i);
		double dTexU = (pointI.getX() - minPt[VX])/(maxPt[VX]-minPt[VX]);
		double dTexV = (pointI.getY() - minPt[VY])/(maxPt[VY]-minPt[VY]);
		pObj->position(pointI.getX(), pointI.getY(), pUseInput->dElevation + pUseInput->dStructHeight - dALt);
		pObj->normal(0,0,1);
		pObj->textureCoord(dTexU,-dTexV);
	}
	pObj->end();


}

const Ogre::String& CRenderAirsideStructure3D::UpdateTexture(const CString& matName,Structure* pStructure,StructFace* pFace)
{
	ALTObjectDisplayProp *pDisplay = pStructure->getDisplayProp();
	ASSERT(pDisplay != NULL);
	Ogre::ColourValue color;
	color = OgreConvert::GetColor(pDisplay->m_dpShape.cColor);

	MaterialPtr matPtr;

	if (pFace->bShow)//use user define texture
	{
		matPtr = OgreUtil::createOrRetrieveMaterial(matName.GetString());

	}
	else//user default texture
	{
		matPtr = OgreUtil::createOrRetrieveColorMaterial(pDisplay->m_dpShape.cColor);
	}

	if( !matPtr.isNull() &&  matPtr->getNumTechniques() )
	{
		Technique* pTech = matPtr->getTechnique(0);
		if(pTech && pTech->getNumPasses())
		{

			Pass* pPas = pTech->getPass(0);
			pPas->setDiffuse(color);
			pPas->setAmbient(color);
			pPas->setCullingMode(CULL_NONE);

			if (pFace->bShow)//use user define texture
			{
				ASSERT(pPas);
				TextureUnitState* pTextureUnit = NULL;
				if(pPas->getNumTextureUnitStates())
				{
					pTextureUnit = pPas->getTextureUnitState(0);
				}
				else
				{
					pTextureUnit = pPas->createTextureUnitState();
				}
				int nPos = pFace->strPicPath.ReverseFind('\\');
				int nLength = pFace->strPicPath.GetLength();
				CString configfilePath = pFace->strPicPath.Left(pFace->strPicPath.ReverseFind('\\')+1);
				CString strFileName = pFace->strPicPath.Right(nLength - nPos - 1);
				Ogre::ResourceGroupManager::getSingleton().addResourceLocation(configfilePath.GetString(), "FileSystem");
				Ogre::ResourceGroupManager::getSingleton().initialiseResourceGroup(ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
				pTextureUnit->setTextureName(strFileName.GetString());
			}
		}
	}
	

	return matPtr->getName();
}

void CRenderAirsideStructure3D::Setup3D( ALTObject* pBaseObj )
{
	Update3D(pBaseObj);
}

//ALTObject* CRenderAirsideStructure3D::GetBaseObject() const
//{
//	return NULL;
//}