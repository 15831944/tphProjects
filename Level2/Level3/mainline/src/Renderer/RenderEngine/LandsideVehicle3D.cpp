#include "StdAfx.h"
#include ".\landsidevehicle3d.h"
#include "CustomizedRenderQueue.h"
#include "..\..\Plugins\ConvertToOgreMesh\ConvertToOgreMeshCommon.h"
//PROTYPE_INSTANCE(CLandsideVehicle3D)
#pragma warning (disable:4101)
using namespace Ogre;
CString CLandsideVehicle3D::GetNodeName( int nID )
{
	CString str;
	str.Format("LandVehicle(%d)",nID);
	return str;
}


void CLandsideVehicle3D::SetShape( const CString& strAcType, double dlen, double dwidth, double dheight)
{
	RemoveAllObject();		

	Entity* pEnt = OgreUtil::createOrRetrieveEntity(GetIDName(), strAcType, GetScene() );
	if(pEnt)
	{
		AddObject(pEnt,2);
		pEnt->setRenderQueueGroup(RenderObject_AnimationObject);

		Vector3 vSize  = pEnt->getBoundingBox().getSize();
		float dscalex = dlen/vSize.x;
		float dscaley = dwidth/vSize.y;
		float dscalez = dheight/vSize.z;
		SetScale(ARCVector3(dscalex,dscaley,dscalez));
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
			if ( matEnt.substr(0, ARC_LANDSIDE_SHAPE_COLOR_MATERIAL_PREFIX_LEN) == ARC_LANDSIDE_SHAPE_COLOR_MATERIAL_PREFIX ||\
				matEnt.substr(0,ARC_LANDSIDE_SHAPE_COLOR_MATERIAL_DEFAULT_LEN) == ARC_LANDSIDE_SHAPE_COLOR_MATERIAL_DEFAULT_NAME)
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

void CLandsideVehicle3D::SetColor( COLORREF color )
{
 	if(!mpNode)
 		return;
	try
	{
		if(MovableObject* pObj = mpNode->getAttachedObject(mpNode->getName()))
		{
			if(pObj->getMovableType() == EntityFactory::FACTORY_TYPE_NAME )
			{
				Entity* pEnt = (Entity*)pObj;
				MaterialPtr material = OgreUtil::createOrRetrieveColorMaterial(color);
				if(!material.isNull())
				{
					SetEntColor(pEnt,material);
					for(size_t i=0;i<pEnt->getNumManualLodLevels();i++)
					{
						SetEntColor(pEnt->getManualLodLevel(i),material);
					}
				}
			}
		}
	}	
	catch (Ogre::Exception* e)
	{
	}
 	
}

CLandsideVehicle3D::CLandsideVehicle3D( int nPaxid, Ogre::SceneNode* pNode )
:C3DNodeObject(pNode)
{
	//InitQueryData();
}

void CLandsideVehicle3D::SetQueryData()
{

}

void CLandsideVehicle3D::UpdateDebugShape( double dlen, double dwidth, double dSpeed )
{
	
}
