#include "StdAfx.h"

#include <Common/replace.h>

#include "AnimaPax3D.h"
#include "OgreUtil.h"
#include "OgreConvert.h"
#include "CustomizedRenderQueue.h"

#include <boost/bind.hpp>

using namespace Ogre;

#include "Shape3DMeshManager.h"

#include <Plugins/ConvertToOgreMesh/ConvertToOgreMeshCommon.h>
#include "ManualObjectUtil.h"
#include "MaterialDef.h"


const TCHAR* const PaxAnimationData::m_sAnimationStateStrings[PaxStateCount] =
{
	_T("listen"),//PaxStanding
	_T("walk"),//PaxWalking
	_T("cheer"),//PaxPuttingLuggage
	_T("sitidle"),//seat on seat
	_T("strafeleft"),//sidle left
	_T("straferight"),//sidle right
	_T("backaway"),
	_T("pickup"),
	_T("pushdown"),
	_T("pushup")
};

void PaxAnimationData::StartOrContinueAnimation( EAnimaPaxState _eAnimaState, long _nTimeStart, long _nCurTime, long _nTimeLength )
{
	if (eAnimaState == _eAnimaState)
	{
		nTimePosition = _nCurTime - nTimeStart;
		return;
	}
	eAnimaState = _eAnimaState;
	nTimeStart = _nTimeStart;
	nTimeLength = _nTimeLength;
	nTimePosition = _nCurTime - _nTimeStart;
	bLooped = (PaxPushDown != eAnimaState&&PaxPushOn!=eAnimaState&&PaxPickup!=eAnimaState);
}

LPCTSTR PaxAnimationData::GetAnimationStateString() const
{
	return GetAnimationStateString(eAnimaState/*, nSubAnimaState*/);
}

LPCTSTR PaxAnimationData::GetAnimationStateString( EAnimaPaxState _eAnimaState/*, int _nSubAnimaState*/ )
{
	if (_eAnimaState>=PaxStateStart && _eAnimaState<PaxStateCount)
	{
		return m_sAnimationStateStrings[_eAnimaState];
	}
	return NULL;
}

CAnimaPax3D::CAnimaPax3D( int nPaxid, SceneNode* pNode )
	: C3DNodeObject(pNode)
	, m_nPaxID(nPaxid)
	, m_pPaxEnt(NULL)
 	, m_pAnimationState(NULL)
{

}

CAnimaPax3D::CAnimaPax3D()
: m_pPaxEnt(NULL)
{

}

static CString GetPaxShapeName(int nID,const std::pair<int, int>& _shape)
{
	CString strShapeName;
	VERIFY(CShape3DMeshManager::GetInstance().GetPluginShapeName(strShapeName, _shape.first, _shape.second));
	return strShapeName;
}


void CAnimaPax3D::SetShape( const std::pair<int, int>& shapePair )
{
	SetShape( GetPaxShapeName(m_nPaxID,shapePair) );
}

static CString GetPaxAnimationShapeName(int nID,const std::pair<int, int>& _shape)
{
	CString strShapeName;
	VERIFY(CShape3DMeshManager::GetInstance().GetAnimationShapeName(strShapeName, _shape.first, _shape.second));
	return strShapeName;
}


void CAnimaPax3D::SetAnimationShape( const std::pair<int, int>& shapePair )
{
	SetShape( GetPaxAnimationShapeName(m_nPaxID,shapePair) );
}

void CAnimaPax3D::SetShape( const CString& strShape )
{

	if(m_strShape!=strShape)
	{
		m_strShape = strShape;

		DestroyEntity();
		CString strEntName = GetIDName();
		//strEntName.Format("PaxEnt%x",(int)this);
		CString strMeshName= strShape;
		//strMeshName.Replace(' ','_');
		strMeshName+= _T(".mesh");
		//strMeshName.Format("%s.mesh", strShape.GetString());	

		m_pPaxEnt = OgreUtil::createOrRetrieveEntity(strEntName.GetString(), strMeshName.GetString(), GetScene() );
		if (m_pPaxEnt)
		{
			m_pPaxEnt->setRenderQueueGroup(RenderObject_AnimationObject);
			AddObject(m_pPaxEnt, true);

//			//set dim
//#ifdef DEBUG
//			Vector3 vSize  = m_pPaxEnt->getBoundingBox().getSize();
//			float dscalex = 40/vSize.x;
//			float dscaley = 20/vSize.y;
//			float dscalez = (dscalex+dscaley)*0.5;
//			SetScale(ARCVector3(dscalex,dscaley,dscalez));
//#endif
		}

		m_pAnimationState = NULL; // after shape modified, should retrieve animation state using it next time


	}
}


static void SetEntColor(Entity* pEnt , const MaterialPtr& matPtr)
{
	unsigned int nEntCount = pEnt->getNumSubEntities();
	if (nEntCount)
	{
		for (unsigned int i=0;i<nEntCount;i++)
		{					
			SubEntity* pSubEnt = pEnt->getSubEntity(i);					
			String matEnt = pSubEnt->getMaterialName();
			if ( pSubEnt->getMaterialName().substr(0, ARC_SHAPE_PLUGIN_MATERIAL_NAME_PREFIX_LEN) != ARC_SHAPE_PLUGIN_MATERIAL_NAME_PREFIX )
			{
				pSubEnt->setMaterial(matPtr);
			}
		}
	}
	else
	{
		pEnt->setMaterial(matPtr);
	}
}

void CAnimaPax3D::SetColor( COLORREF color )
{
	if(m_pPaxEnt)
	{
		MaterialPtr matPtr = OgreUtil::createOrRetrieveColorMaterial(color);
		if(!matPtr.isNull())
		{			
			SetEntColor(m_pPaxEnt,matPtr);
			for(size_t i=0;i<m_pPaxEnt->getNumManualLodLevels();i++)
			{
				SetEntColor(m_pPaxEnt->getManualLodLevel(i),matPtr);
			}
		}
	}
}

void CAnimaPax3D::DestroyEntity()
{
	if(GetScene() && m_pPaxEnt)
	{
		GetSceneNode()->detachAllObjects();
		OgreUtil::DetachMovableObject(m_pPaxEnt);
		m_pPaxEnt = NULL;
	}

}

void CAnimaPax3D::OnTimeUpdate( double dTime )
{
	if(m_pAnimationState)
	{
		m_pAnimationState->setTimePosition(dTime - m_animaData.nTimeStart/100.0);
		m_animaData.nTimePosition = dTime;
	}
}

void CAnimaPax3D::SetAnimationData( const PaxAnimationData& animaData )
{
	if (m_animaData.eAnimaState == animaData.eAnimaState)
	{
		//ASSERT(m_animaData.nTimeStart == animaData.nTimeStart
		//	&& m_animaData.nTimeLength == animaData.nTimeLength
		//	&& m_animaData.bLooped == animaData.bLooped);
		m_animaData.nTimePosition = animaData.nTimePosition; // only update animation time position

		if(m_pAnimationState)
		{
			m_pAnimationState->setTimePosition(m_animaData.nTimePosition/100.0);
			return;
		}
	}
	else
	{
		m_animaData = animaData;
	}

	if (m_pPaxEnt)
	{
		AnimationStateSet* pStateSet = m_pPaxEnt->getAllAnimationStates();
		if (pStateSet)
		{
			LPCTSTR statestr = m_animaData.GetAnimationStateString();
			if (statestr)
			{
				// disable all first
				AnimationStateIterator itr = pStateSet->getAnimationStateIterator();
				while(itr.hasMoreElements())
				{
					AnimationState* pState = itr.getNext();
					pState->setEnabled(false);
				}
	
				// set state data
				if (pStateSet->hasAnimationState(statestr))
				{
					m_pAnimationState = pStateSet->getAnimationState(statestr);
					if (m_pAnimationState)
					{
						m_pAnimationState->setEnabled(true);
						m_pAnimationState->setLoop(m_animaData.bLooped);
						if (m_animaData.nTimeLength>=0)
							m_pAnimationState->setLength(m_animaData.nTimeLength/100.0);
						m_pAnimationState->setTimePosition(m_animaData.nTimePosition/100.0);
					}
				}
			}
		}
	}	

}

CString CAnimaPax3D::GetShapeName() const
{
	return m_strShape;
}

//void CAnimaPax3DList::Clear()
//{
////	for(size_t i=0;i<m_vPaxList.size();i++)
////#ifdef ALLOC_PAX3D_USE_POOL
////		m_paxPool.destroy(m_vPaxList[i]);
////#else
////		delete m_vPaxList[i];
////#endif // ALLOC_PAX3D_USE_POOL
//	m_vPaxList.clear();
//}
//
//
//CAnimaPax3DList::CAnimaPax3DList()
//{
//
//}
//
//CAnimaPax3DList::~CAnimaPax3DList()
//{
//	Clear();
//}
//
//void CAnimaPax3DList::ShowPax3D( int nPaxID, bool bShow )
//{
//	if((int)m_vPaxList.size()<=nPaxID)
//		return;
//	
//	ShowPax3D( &m_vPaxList[nPaxID], bShow);
//	/*PaxList::iterator ite = std::lower_bound(m_vPaxList.begin(), m_vPaxList.end(), nPaxID, Pax3DIDCmp());
//	if (ite != m_vPaxList.end() && (*ite)->GetPaxID() == nPaxID)
//	{
//		ShowPax3D(*ite, bShow);
//	}*/
//}
//
//void CAnimaPax3DList::ShowPax3D( CAnimaPax3D* pPax3D, bool bShow )
//{
//	bShow ? pPax3D->AttachTo(m_rootNode) : pPax3D->Detach();
//}
//
//bool CAnimaPax3DList::IsPax3DShow( CAnimaPax3D* pPax3D )
//{
//	return pPax3D->GetParent()/* == m_rootNode*/;
//}
//
//CAnimaPax3D* CAnimaPax3DList::CreateOrRetrievePax3D( int nPaxID, bool& bCreated )
//{
//	if((int)m_vPaxList.size()<=nPaxID)
//		m_vPaxList.resize(nPaxID+1);
//
//	bCreated = false;
//	CAnimaPax3D& pax = m_vPaxList[nPaxID];
//	if(!pax)
//	{
//		bCreated = true;
//		pax = CAnimaPax3D(nPaxID,m_rootNode.CreateNewChild().GetSceneNode());		
//	}
//	return &pax;
//	//PaxList::iterator ite = std::lower_bound(m_vPaxList.begin(), m_vPaxList.end(), nPaxID, Pax3DIDCmp());
//	//if (ite != m_vPaxList.end() && (*ite)->GetPaxID() == nPaxID)
//		//return *ite;
//
////#ifdef ALLOC_PAX3D_USE_POOL
////	CAnimaPax3D* pPax3D = m_paxPool.malloc();
////	new (pPax3D) CAnimaPax3D(nPaxID, m_rootNode.CreateNewChild().GetSceneNode());
////#else
////	CAnimaPax3D* pPax3D = new CAnimaPax3D(nPaxID, m_rootNode.CreateNewChild().GetSceneNode());
////#endif // ALLOC_PAX3D_USE_POOL
//
//	//m_vPaxList.push_back(pPax3D);
//	//std::sort(m_vPaxList.begin(), m_vPaxList.end(), Pax3DCmp());
//	
//}
//
//void CAnimaPax3DList::UpdateAllPax( bool bShow )
//{
//	if(bShow)
//	{
//// 		std::for_each(m_vPaxList.begin(), m_vPaxList.end(), boost::BOOST_BIND(&CAnimaPax3D::AttachTo, _1, m_rootNode));
//	}
//	else
//	{
//		Clear();
//	}
//}






void CAnimaElevator3D::BuildShape( double dlen, double dwidth, double doritation,const ARCColor3& color )
{
	Entity* pEnt = OgreUtil::createOrRetrieveEntity(GetIDName(),"ElevatorTray.mesh",GetScene());
	if(pEnt)
	{
		AddObject(pEnt);
		const AxisAlignedBox& bbox = pEnt->getBoundingBox();
		Vector3 vSize = bbox.getSize();
		double scalx = dlen/vSize.x;
		double scaley = dwidth/vSize.y;
		double scalez = (scalx+scaley)*0.5;		
		SetScale(ARCVector3(scalx,scaley,scalez));
		SetRotation(ARCVector3(0,0,doritation));
	}

}

void CAnimaTrain3D::BuildShape( double dlen, double dwidth, int carNum,const ARCColor3& color )
{

	//Entity* pEnt = OgreUtil::createOrRetrieveEntity(GetIDName(),"ElevatorTray.mesh",GetScene());
	//if(pEnt)
	//{
	//	AddObject(pEnt);
	//}

	//return;
	//
	ManualObject* pObj = OgreUtil::createOrRetrieveManualObject(GetIDName(),GetScene());
	if(!pObj)
	{
		return; 
	}
	Painter pen;
	pen.UseVertexColor(ARCColor4(color));
	pen.msMat= VERTEXCOLOR_LIGHTON;
	pen.mbAutoNorm = true;
	pen.mbBackFace = true;


	ManualObjectUtil shape(pObj);
	shape.translate(ARCVector3::UNIT_Z);
	shape.beginTriangleList(pen);
	shape.drawOpenBox(dlen*carNum,dwidth,100);		
	for(int i=1; i<=carNum-1; i++)
	{
		double xpos = dlen*carNum*(-0.5+i*(1.0/carNum));		
		shape.drawQuad(
			ARCVector3(xpos,-dwidth*0.5, 0),
			ARCVector3(xpos,dwidth*0.5,0),
			ARCVector3(xpos,dwidth*0.5,100),
			ARCVector3(xpos,-dwidth*0.5,100)
			);
	}
	shape.end();

	AddObject(pObj);
}