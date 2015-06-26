#pragma once
class CModel;
#include "./AfxHeader.h"
#include "RenderEngineAPI.h"
class RENDERENGINE_API C3DS2Mesh
{
public:
	static bool ImportModel( CString s3dsfile, CModel& model , CString dir, float scale);

};