// Converts a 3D Studio file into an Ogre3D mesh and material
#include "StdAfx.h"
#include <memory>
#include <set>
#include <string>
#include <vector>
#include "RenderEngine.h"
#include "3ds2mesh.h"
#include <common/Guid.h>
#include <common/FileInDB.h>
#include "../3DTools/3DTools.h"
#include <Shlwapi.h>
#include ".\modelsnapshot.h"
bool C3DS2Mesh::ImportModel( CString s3dsfile, CModel& model, CString dir, float scale )
{	
	
	bool bFileValid = ( !s3dsfile.IsEmpty() && PathFileExists(s3dsfile) );
	if(!bFileValid)
		return false;

	CString gid = (CString)model.mguid;

	C3DTools::C3DS2Mesh toMesh(gid.GetString(),model.m_sModelName.GetString());
	Ogre::Vector3 dim;
	bool b =  toMesh.Convert(s3dsfile.GetString(),dir.GetString(),dim);	
	model.mDimension = ARCVECTOR(dim);
	
	CModelSnapShot snapshot;
	snapshot.SetSize(512,512);
	snapshot.DoSnapeShot(model);

	return b;
}