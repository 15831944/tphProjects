#include "StdAfx.h"
#include ".\renderer.h"
#include <common\ProjectManager.h>
#include <common\AircraftAliasManager.h>


void CRenderer::SetSingletonPtr( CProjectManager* pProjMan )
{
	CProjectManager::SetInstance(pProjMan);		
}

void CRenderer::SetSingletonPtr( AircraftAliasManager* pAiasManager )
{
	AircraftAliasManager::SetInstance(pAiasManager);
}

void CRenderer::GetRotate( const ARCVector4& quat,double& degree,ARCVector3& axis )
{
	Ogre::Quaternion q(quat.w,quat.x,quat.y,quat.z);
	Ogre::Degree deg;
	Ogre::Vector3 oaxis;
	q.ToAngleAxis(deg,oaxis);
	degree = deg.valueDegrees();
	axis = ARCVECTOR(oaxis);
}