#pragma once

class CProjectManager;
class AircraftAliasManager;
#include <common\ARCVector4.h>
#include ".\AfxHeader.h"
#include "RenderEngineAPI.h"

 class  RENDERENGINE_API CRenderer
{
public:
	static void SetSingletonPtr(CProjectManager* pProjMan);
	static void SetSingletonPtr(AircraftAliasManager* pAiasManager);
	static void GetRotate(const ARCVector4& quat,double& degree,ARCVector3& axis);
};
