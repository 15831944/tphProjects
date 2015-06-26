#include "StdAfx.h"
#include ".\aircraftmodel3dnode.h"
#include <common\FileInDB.h>
#include <InputOnBoard\CInputOnboard.h>
#include "RenderEngine.h"
#include ".\shape3dmeshManager.h"
#include <common/AircraftAliasManager.h>
#include <common/ACTypesManager.h>
#include <common/AirportDatabase.h>
#include <common\util.h>
using namespace Ogre;
CAircraftModel3DNode::CAircraftModel3DNode(void)
{
	m_pFlightEnt = NULL;
}

CAircraftModel3DNode::CAircraftModel3DNode( Ogre::SceneNode* pNode ):C3DNodeObject(pNode)
{
	m_pFlightEnt = NULL;
}

static MaterialPtr GetAirlineLodTransparentMat(CString strAirline, COLORREF color)
{
	const static int nLod = 5;
	Real lodlist[nLod-1] = { 1000,2000,5000,10000 };
	Real alph[nLod] = { 0.05f, 0.2f , 0.5f, 0.8f,1.0f };

	const static CString prefix = _T("FltLod");
	CString strMatName;
	strMatName.Format(_T("FltLod%08x%s"), color, strAirline.GetString() );
	bool bCreated = false;
	MaterialPtr matPtr = OgreUtil::createOrRetrieveMaterial2(strMatName, bCreated);
	if(!matPtr.isNull() && bCreated)
	{
		Ogre::Material::LodValueList lodlist(lodlist,lodlist+nLod);		
		matPtr->setLodLevels(lodlist);
		int nCurTechCnt = matPtr->getNumTechniques();
		for(int i=nCurTechCnt;i<nLod;i++)
		{
			matPtr->createTechnique();
		}


		for(int i=0;i<nLod;i++)
		{
			Technique *pTech = matPtr->getTechnique(i);
			pTech->setLodIndex(i);
			int nCurPassCnt = pTech->getNumPasses();
			for(int j=nCurPassCnt;j<2;++j)
			{
				pTech->createPass();
			}
			Pass* pPass = pTech->getPass(0);
			pPass->setSceneBlending(SBT_TRANSPARENT_ALPHA);
			pPass->setColourWriteEnabled(false);
			pPass->setDepthWriteEnabled(false);

			pPass = pTech->getPass(1);		
			ARCColor3 arccol(color);
			pPass->setAmbient( OGRECOLOR(arccol,alph[i]) );
			pPass->setDiffuse( OGRECOLOR(arccol,alph[i]) );
			pPass->setSceneBlending(SBT_TRANSPARENT_ALPHA);			
			if(!strAirline.IsEmpty())
			{
				CString strImageFileName;
				VERIFY(CShape3DMeshManager::GetInstance().GetAirlineImageFileName(strImageFileName, strAirline));
				TextureUnitState* pTex = pPass->createTextureUnitState();
				pTex->setTextureName((LPCTSTR)strImageFileName);
			}			
		}

	}
	return matPtr;
}


void CAircraftModel3DNode::Load( CString acType,InputOnboard* pOnboard )
{
	CModel* pModel = pOnboard->GetAircraftModel(acType);	
	if(pModel)
	{		
		if(!pModel->MakeSureFileOpen()){ ASSERT(FALSE); }
		pModel->ReadContent();
		ResourceFileArchive::AddDirectory(pModel->msTmpWorkDir,(CString)pModel->mguid); //make sure resource added
		CString strMeshName = pModel->GetMeshFileName();
		m_pFlightEnt =  OgreUtil::createOrRetrieveEntity(GetIDName(),strMeshName,GetScene());
		if(m_pFlightEnt)
		{
			//set dimension
			AddObject(m_pFlightEnt,false);			
		}
// 		AddObject(m_pFlightEnt,true);
	}	
	if(!m_pFlightEnt)
	{
		Load(acType);
	}

	CACTypesManager *pACMan = pOnboard->GetAirportDB()->getAcMan();
	CACType* pAcType = pACMan->getACTypeItem(acType);
	if(pAcType)
	{
		SetFlightSize(pAcType->m_fSpan*SCALE_FACTOR,pAcType->m_fLen*SCALE_FACTOR);
	}
}



void CAircraftModel3DNode::Load( CString acType)
{
	CString aliashape = ACALIASMANAGER->getOglShapeName(acType);
	CString strShape;
	VERIFY(CShape3DMeshManager::GetInstance().GetFlightShapeName(strShape, aliashape));	
	CString strMeshName;
	strMeshName.Format("%s.mesh", strShape.GetString());	

	m_pFlightEnt =  OgreUtil::createOrRetrieveEntity(GetIDName(),strMeshName,GetScene());
	if(m_pFlightEnt)
	{
		//set dimension		
		AddObject(m_pFlightEnt,true);			
	}
	else
		ASSERT(FALSE);
}

void CAircraftModel3DNode::SetFlightSize( double dwingspan, double dlength )
{
	if(m_pFlightEnt)
	{
		//set dimension
		const AxisAlignedBox& bbox = m_pFlightEnt->getBoundingBox();
		Vector3 vSize = bbox.getSize();
		double scalx = dlength/vSize.x;
		double scaley = dwingspan/vSize.y;
		double scalez = (scalx+scaley)*0.5;		
		SetScale(ARCVector3(scalx,scaley,scalez));		
	}
}

void CAircraftModel3DNode::SetFadeMaterial( CString strAirline, COLORREF color )
{
	if(m_pFlightEnt)
	{
		MaterialPtr matptr = GetAirlineLodTransparentMat(strAirline,color);
		if(!matptr.isNull())
		{
			m_pFlightEnt->setMaterial(matptr);
			m_pFlightEnt->setRenderQueueGroup(RENDER_QUEUE_7);
		}
	}
}