#include "StdAfx.h"

#include <Common/replace.h>

#include "AnimaFlight3D.h"
#include "OgreUtil.h"
#include "OgreConvert.h"

#include <boost/bind.hpp>
#include <InputOnBoard/CInputOnboard.h>
#include "Shape3DMeshManager.h"
#include <Plugins/ConvertToOgreMesh/ConvertToOgreMeshCommon.h>
#include <InputOnBoard/AircaftLayOut.h>

#include "Render3DScene.h"
#include "CustomizedRenderQueue.h"

using namespace Ogre;

CAnimaFlight3D::CAnimaFlight3D( int nFlightID, Ogre::SceneNode* pNode )
	: C3DNodeObject(pNode)
	, m_nFlightID(nFlightID)
{
	m_acModel = CreateNewChild().GetSceneNode();
	//CRender3DScene::ReferToParentWhenQuery(m_acModel);
	m_layoutNode = CreateNewChild().GetSceneNode();
	m_nOnboardFlightID = -1;
}

CAnimaFlight3D::~CAnimaFlight3D( void )
{

}



void CAnimaFlight3D::SetShape( const CString& strAcType, double dlen, double dwingspan, InputOnboard* pOnboard /*= NULL */ )
{
	if(m_strShape!=strAcType)
	{
		if(pOnboard)
			m_acModel.Load(strAcType,pOnboard);
		else
			m_acModel.Load(strAcType);

		Entity* pFlightEntity = m_acModel.GetFlightEnt();
		if (pFlightEntity)
		{
			pFlightEntity->setRenderQueueGroup(RenderObject_AnimationObject);
		}
		m_acModel.SetFlightSize(dwingspan,dlen);
	}
}


void CAnimaFlight3D::SetMatByAirlineColor( CString strAirline, COLORREF color ,bool bOnBoard)
{
	Ogre::Entity* pFlightEnt = m_acModel.GetFlightEnt();
	if(!pFlightEnt)
		return;

	if(bOnBoard)
	{
		m_acModel.SetFadeMaterial(strAirline,color);
	}
	else
	{
		if (strAirline.IsEmpty()) // logo is empty
		{
			MaterialPtr matPtr = OgreUtil::createOrRetrieveColorMaterial(color);
			if (!matPtr.isNull())
			{
				pFlightEnt->setMaterial(matPtr);
			}
		}
		else // show logo
		{
			CString strImageFileName;
			VERIFY(CShape3DMeshManager::GetInstance().GetAirlineImageFileName(strImageFileName, strAirline));
			CString strMatName;
			strMatName.Format(_T("FltMat%08x%s"), color, strImageFileName);
	
			bool bCreated = false;
			MaterialPtr matPtr = OgreUtil::createOrRetrieveMaterial2(strMatName, bCreated);			
			if(!matPtr.isNull())
			{
				if (bCreated)
				{
					if(!matPtr->getNumTechniques())
						matPtr->createTechnique();
					Technique* pTech = matPtr->getTechnique(0);
					if(!pTech->getNumPasses()){
						pTech->createPass();
					}
					Pass * pPass = pTech->getPass(0);
					Ogre::ColourValue ogreColor = OgreConvert::GetColor(color);
					pPass->setAmbient( ogreColor );
					pPass->setDiffuse( ogreColor );
					//pPass->setCullingMode(CULL_NONE);

					if (!pPass->getNumTextureUnitStates())
						pPass->createTextureUnitState();
					pPass->getTextureUnitState(0)->setTextureName((LPCTSTR)strImageFileName);
					
				}
				pFlightEnt->setMaterial(matPtr);
			}
		}
	}
}


void CAnimaFlight3D::LoadLayout( InputOnboard* pOnboard, int nFlightId )
{
	if(m_nOnboardFlightID != nFlightId)
	{
		m_nOnboardFlightID = nFlightId;
		CAircaftLayOut* playout = pOnboard->GetAircaftLayoutManager()->GetAircraftLayOutByFlightID(nFlightId);
		if(!playout)
		{
			return;
		}
		m_layoutNode.Detach();
		m_layoutNode.LoadLayout(pOnboard,playout,*this);
	}
}

void CAnimaFlight3D::DrawDeckCells( int nDeckIndex, const CString& strDeckName, std::vector<std::pair<ARCColor3, Path > >& vCells )
{
	m_layoutNode.DrawDeckCells(nDeckIndex,strDeckName,vCells);
}
//void CAnimaFlight3DList::Clear()
//{
////	for(size_t i=0;i<m_vFlightList.size();i++)
////#ifdef ALLOC_FLIGHT3D_USE_POOL
////		m_flightPool.destroy(m_vFlightList[i]);
////#else
////		delete m_vFlightList[i];
////#endif // ALLOC_FLIGHT3D_USE_POOL
//	m_vFlightList.clear();
//}
//
//
//CAnimaFlight3DList::CAnimaFlight3DList()
//{
//
//}
//
//CAnimaFlight3DList::~CAnimaFlight3DList()
//{
//	Clear();
//}
//
//void CAnimaFlight3DList::ShowFlight3D( int nFlightID, bool bShow )
//{
//	//FlightList::iterator ite = std::lower_bound(m_vFlightList.begin(), m_vFlightList.end(), nFlightID, Flight3DIDCmp());
//	//if (ite != m_vFlightList.end() && (*ite)->GetFlightID() == nFlightID)
//	//{
//	//	ShowFlight3D(*ite, bShow);
//	//}
//}
//
//void CAnimaFlight3DList::ShowFlight3D( CAnimaFlight3D* pFlight3D, bool bShow )
//{
//	bShow ? pFlight3D->AttachTo(m_rootNode) : pFlight3D->Detach();
//}
//
//bool CAnimaFlight3DList::IsFlight3DShow( CAnimaFlight3D* pFlight3D )
//{
//	return pFlight3D->GetParent()/* == m_rootNode*/;
//}
//
//CAnimaFlight3D* CAnimaFlight3DList::CreateOrRetrieveFlight3D( int nFlightID, bool& bCreated )
//{
//
//	
//
//	bCreated = false;
//	
//	
//	FlightList::iterator ite = std::lower_bound(m_vFlightList.begin(), m_vFlightList.end(), nFlightID, Flight3DIDCmp());
//	if (ite != m_vFlightList.end() && (*ite)->GetFlightID() == nFlightID)
//		return *ite;
//
//#ifdef ALLOC_FLIGHT3D_USE_POOL
//	CAnimaFlight3D* pFlight3D = m_flightPool.malloc();
//	new (pFlight3D) CAnimaFlight3D(nFlightID, m_rootNode.CreateNewChild().GetSceneNode());
//#else
//	CAnimaFlight3D* pFlight3D = new CAnimaFlight3D(nFlightID, m_rootNode.CreateNewChild().GetSceneNode());
//#endif // ALLOC_FLIGHT3D_USE_POOL
//
//	m_vFlightList.push_back(pFlight3D);
//	std::sort(m_vFlightList.begin(), m_vFlightList.end(), Flight3DCmp());
//	bCreated = true;
//	return pFlight3D;
//}
//
//void CAnimaFlight3DList::UpdateAllFlight( bool bShow )
//{
//	if(bShow)
//	{
//// 		std::for_each(m_vFlightList.begin(), m_vFlightList.end(), boost::BOOST_BIND(&CAnimaFlight3D::AttachTo, _1, m_rootNode));
//	}
//	else
//	{
//		Clear();
//	}
//}
