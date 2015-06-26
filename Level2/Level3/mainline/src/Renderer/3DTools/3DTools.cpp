// 3DTools.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "3DTools.h"
#include <Ogre.h>
#include <OgreStringConverter.h>
#include <OgreDefaultHardwareBufferManager.h>
#include <OgreHardwareVertexBuffer.h>
#include <OgreVertexIndexData.h>
#include <OgreResourceGroupManager.h>

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
    return TRUE;
}


//////////////////////////////////////////////////////////////////////////



#define VERSION "1.1"

#ifdef WIN32
#include "getopt.h"
#else
#include <getopt.h>
#endif
extern "C" {
#include "3dsftk3/inc/3dsftk.h"
}


using namespace std;



class Face {
public:
	Face() {}
	Face(ushort3ds i1, ushort3ds i2, ushort3ds i3) : v1(i1), v2(i2), v3(i3) {}
	ushort3ds v1, v2, v3;
};

Ogre::String ReplaceSpaces(const Ogre::String& s)
{
	Ogre::String res(s);
	replace(res.begin(), res.end(), ' ', '_');

	return res;
}
#define PRINT_ERRORS_RETURN(file) {if(ftkerr3ds){DumpErrList3ds(file); if (!ignoreftkerr3ds) return (false);}}

class File3DS
{
public:
	File3DS(file3ds* pfile){ mpfile=pfile;}
	~File3DS(){ if(mpfile)CloseFile3ds(mpfile); mpfile=NULL;}
	file3ds* mpfile;
};

bool C3DTools::C3DS2Mesh::Convert( const char* sz3dsFileName, const char* _outfolder ,Ogre::Vector3& dim)
{
	Ogre::LogManager logMgr;
	Ogre::LogManager::getSingleton().createLog("3DSOgreExporter.log", true);
	Ogre::ResourceGroupManager resGrpMgr;
	Ogre::LodStrategyManager lodMgr;
	Ogre::MaterialManager matMgr;
	matMgr.initialise();
	Ogre::DefaultHardwareBufferManager defHWBufMgr;
	Ogre::MeshManager meshMgr;
	Ogre::SkeletonManager skelMgr;
	bool bTextureVerticalFlip = false;


	const char* szMeshPrefix = mMeshPrefix;
	file3ds *ifile = NULL;
	database3ds *db = NULL;

	CString dsfolder = sz3dsFileName;
	int nIndx = dsfolder.ReverseFind('\\');
	dsfolder = dsfolder.Left(nIndx+1);
	std::set<CString> vTextnames;

	FILE* file = stderr;
	ClearErrList3ds(); 

	Ogre::String sMatPrefix = mMaterailPrefix;
	Ogre::String outfolder = _outfolder;

	ifile = OpenFile3ds(sz3dsFileName, "r");
	PRINT_ERRORS_RETURN(file);
	File3DS thefile(ifile);

	InitDatabase3ds(&db);
	PRINT_ERRORS_RETURN(file);
	CreateDatabase3ds(ifile, db);
	PRINT_ERRORS_RETURN(file);

	meshset3ds* mset = NULL;
	GetMeshSet3ds(db, &mset);
	ReleaseMeshSet3ds(&mset);

	assert(GetDatabaseType3ds(db) == MeshFile);

	// If we are called in an Ogre application, Manager are already there
	Ogre::LogManager *pLogMgr = Ogre::LogManager::getSingletonPtr();
	assert(pLogMgr != NULL);
	pLogMgr->logMessage("OGRE 3DS Exporter Log");
	pLogMgr->logMessage("---------------------------");

	ulong3ds i;

	Ogre::MeshManager* pMeshMgr = Ogre::MeshManager::getSingletonPtr();
	assert(pMeshMgr != NULL);

	pMeshMgr->unload(szMeshPrefix);
	pMeshMgr->remove(szMeshPrefix);
	Ogre::MeshPtr ogreMesh = pMeshMgr->createManual(Ogre::String(szMeshPrefix),
		Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

	Ogre::SkeletonPtr ogreSkeleton;

	bool bCreateSkeleton = false;
	if (bCreateSkeleton)
	{
		// create Skeleton
		Ogre::String skeletonName = Ogre::String(szMeshPrefix) + ".skeleton";
		Ogre::SkeletonManager::getSingleton().unload(skeletonName);
		Ogre::SkeletonManager::getSingleton().remove(skeletonName);
		ogreSkeleton = Ogre::SkeletonManager::getSingleton().create(skeletonName, 
			Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

		ogreMesh->_notifySkeleton(ogreSkeleton);
	}

	bool bBonesExist = false;


	namelist3ds *objectlist = NULL;
	namelist3ds *meshlist = NULL;

	mesh3ds *mesh = NULL;
	kfmesh3ds *kfmesh = NULL;

	size_t j;
	Ogre::Vector3 min, max, currpos;
	Ogre::Real maxSquaredRadius = 0;
	bool bFirst = true;

	/* Get the list of meshes */
	GetMeshNameList3ds(db, &meshlist);
	PRINT_ERRORS_RETURN(file);
	GetObjectNodeNameList3ds(db, &objectlist);
	PRINT_ERRORS_RETURN(file);

	/* Store name of found material */
	set<string> foundMaterial;

	for (i = 0; i < meshlist->count; i++)
	{
		GetMeshByName3ds(db, meshlist->list[i].name, &mesh);
		PRINT_ERRORS_RETURN(file);

		// loop materials in sub mesh and create a sub mesh for each material 
		for (int k = 0; k < mesh->nmats; k++)
		{
			// get current material
			objmat3ds * mat = mesh->matarray+k;

			// if the material doen't have faces - continue
			if (mat->nfaces == 0)
				continue;

			// not all the vertexes in the mesh are using this material
			// we want to find the vertexes of this material from the
			// material faces.

			vector<bool> isVertexUsedInMaterial(mesh->nvertices, false);
			vector<ushort3ds> origVertexToMaterialVertex(mesh->nvertices);

			//  find the material vertexes from the faces of the material
			for (j = 0; j < mat->nfaces; ++j)
			{
				ushort3ds faceindex = mat->faceindex[j];

				isVertexUsedInMaterial[mesh->facearray[faceindex].v1] = true;
				isVertexUsedInMaterial[mesh->facearray[faceindex].v2] = true;
				isVertexUsedInMaterial[mesh->facearray[faceindex].v3] = true;
			}

			ushort3ds materialVertexCount = 0;
			// build a map that will be our index guide from the 3ds vertexes indexes to
			// the material vertexes indexes
			for (j = 0; j < mesh->nvertices; ++j)
			{
				if (isVertexUsedInMaterial[j])
				{
					origVertexToMaterialVertex[j] = materialVertexCount;
					materialVertexCount++;
				}
			}

			Ogre::String subMeshName = Ogre::String(meshlist->list[i].name) + "__" + Ogre::String(mat->name);
			pLogMgr->logMessage("Creating SubMesh object..." + subMeshName);
			Ogre::SubMesh* ogreSubMesh = ogreMesh->createSubMesh(subMeshName);
			pLogMgr->logMessage("SubMesh object created.");

			ogreSubMesh->vertexData = new Ogre::VertexData();
			ogreSubMesh->vertexData->vertexCount = materialVertexCount;
			ogreSubMesh->vertexData->vertexStart = 0;
			Ogre::VertexBufferBinding* bind = ogreSubMesh->vertexData->vertexBufferBinding;
			Ogre::VertexDeclaration* decl = ogreSubMesh->vertexData->vertexDeclaration;

			// Always 2D coords and normals
			// Texture coords only if there is texture coordinates in the 3ds mesh
#define POSITION_BINDING 0
#define NORMAL_BINDING 1
#define TEXCOORD_BINDING 2
			decl->addElement(POSITION_BINDING, 0, Ogre::VET_FLOAT3, Ogre::VES_POSITION);
			decl->addElement(NORMAL_BINDING, 0, Ogre::VET_FLOAT3, Ogre::VES_NORMAL);
			if (mesh->ntextverts != 0) 
				decl->addElement(TEXCOORD_BINDING, 0, Ogre::VET_FLOAT2, Ogre::VES_TEXTURE_COORDINATES);
			// Create buffers
			Ogre::HardwareVertexBufferSharedPtr pbuf = Ogre::HardwareBufferManager::getSingleton().
				createVertexBuffer(decl->getVertexSize(POSITION_BINDING), ogreSubMesh->vertexData->vertexCount,
				Ogre::HardwareBuffer::HBU_DYNAMIC, false);
			Ogre::HardwareVertexBufferSharedPtr nbuf = Ogre::HardwareBufferManager::getSingleton().
				createVertexBuffer(decl->getVertexSize(NORMAL_BINDING), ogreSubMesh->vertexData->vertexCount,
				Ogre::HardwareBuffer::HBU_DYNAMIC, false);
			Ogre::HardwareVertexBufferSharedPtr tbuf = Ogre::HardwareBufferManager::getSingleton().
				createVertexBuffer(decl->getVertexSize(TEXCOORD_BINDING), ogreSubMesh->vertexData->vertexCount,
				Ogre::HardwareBuffer::HBU_DYNAMIC, false);
			bind->setBinding(POSITION_BINDING, pbuf);
			bind->setBinding(NORMAL_BINDING, nbuf);
			if (mesh->ntextverts != 0) 
				bind->setBinding(TEXCOORD_BINDING, tbuf);

			ogreSubMesh->useSharedVertices = false;

			float* pPos = static_cast<float*>(
				pbuf->lock(Ogre::HardwareBuffer::HBL_DISCARD));
			float* pTex;

			pLogMgr->logMessage("Doing positions ...");
			assert(mesh->ntextverts == 0 || mesh->ntextverts == mesh->nvertices);
			if (mesh->ntextverts != 0)
			{
				pLogMgr->logMessage("and texture coordinates ...");
				pTex = static_cast<float*>(
					tbuf->lock(Ogre::HardwareBuffer::HBL_DISCARD));
			}

			std::vector<Ogre::Vector3> vertices;
			std::vector<Face> faces;
			// For each vertex a list of faces containing it
			std::vector<std::vector<ushort3ds> > vertexFaces; 
			vertexFaces.resize(ogreSubMesh->vertexData->vertexCount);

			for (j = 0; j < mesh->nvertices; ++j)
			{
				if (isVertexUsedInMaterial[j] == false)
					continue;

				// Do some coordsys change
				currpos = Ogre::Vector3(mesh->vertexarray[j].x,
					mesh->vertexarray[j].y,
					mesh->vertexarray[j].z)*mScale;
				*pPos = currpos.x; pPos++;
				*pPos = currpos.y; pPos++;
				*pPos = currpos.z; pPos++;

				vertices.push_back(currpos);

				if (mesh->ntextverts != 0) 
				{
					*pTex = mesh->textarray[j].u; ++pTex;
					if (bTextureVerticalFlip)
						*pTex = mesh->textarray[j].v;
					else
						*pTex = 1.0f - mesh->textarray[j].v;
					++pTex;
				}
				// Deal with bounds
				if (bFirst)
				{
					min = max = currpos;
					maxSquaredRadius = currpos.squaredLength();
					bFirst = false;
				}
				else
				{
					min.makeFloor(currpos);
					max.makeCeil(currpos);
					maxSquaredRadius = std::max(maxSquaredRadius, currpos.squaredLength());
				}
			}
			if (mesh->ntextverts != 0)
				tbuf->unlock();
			pbuf->unlock();

			ogreSubMesh->indexData->indexStart = 0;
			ogreSubMesh->indexData->indexCount = mat->nfaces*3;

			Ogre::HardwareIndexBufferSharedPtr ibuf;
			ibuf = Ogre::HardwareBufferManager::getSingleton().createIndexBuffer(
				Ogre::HardwareIndexBuffer::IT_16BIT,
				ogreSubMesh->indexData->indexCount,
				Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY);
			ogreSubMesh->indexData->indexBuffer = ibuf;

			pLogMgr->logMessage("Doing indices ...");
			unsigned short *pShort = static_cast<unsigned short*>(
				ibuf->lock(Ogre::HardwareBuffer::HBL_DISCARD));
			assert(mat->nfaces < 65536);
			for (j = 0; j < mat->nfaces; ++j)
			{
				ushort3ds faceindex = mat->faceindex[j];

				Face f(origVertexToMaterialVertex[mesh->facearray[faceindex].v1],
					origVertexToMaterialVertex[mesh->facearray[faceindex].v2],
					origVertexToMaterialVertex[mesh->facearray[faceindex].v3]);
				faces.push_back(f);

				*pShort = f.v1; ++pShort;
				*pShort = f.v2; ++pShort;
				*pShort = f.v3; ++pShort;

				vertexFaces[f.v1].push_back((ushort3ds)j);
				vertexFaces[f.v2].push_back((ushort3ds)j);
				vertexFaces[f.v3].push_back((ushort3ds)j);
			}
			ibuf->unlock();

			// Compute the normal of each face
			// TODO: take into account smoothing group
			pLogMgr->logMessage("Computing face normals ...");
			std::vector<Ogre::Vector3> faceNormals;
			std::vector<Face>::const_iterator itF;
			for (itF = faces.begin(); itF != faces.end(); ++itF)
			{
				Ogre::Vector3 normal;
				Ogre::Vector3 ab = vertices[(*itF).v2] - vertices[(*itF).v1];
				Ogre::Vector3 ac = vertices[(*itF).v3] - vertices[(*itF).v1];

				normal = ab.crossProduct(ac);
				faceNormals.push_back(normal);
			}
			assert(faceNormals.size() == mat->nfaces);

			// Compute the normal for each vertices
			assert(vertices.size() < 65536);
			ushort3ds iv;
			float* pNorm = static_cast<float*>(nbuf->lock(Ogre::HardwareBuffer::HBL_DISCARD));
			for (iv = 0; iv < vertices.size(); ++iv)
			{
				// Sum up the normals
				Ogre::Vector3 normal(0,0,0);
				std::vector<ushort3ds>::const_iterator itF;
				//assert(!vertexFaces[iv].empty());
				for (itF = vertexFaces[iv].begin(); itF != vertexFaces[iv].end(); ++itF)
					normal += faceNormals[*itF];

				normal.normalise();
				//assert(Ogre::Math::Abs(normal.length() - 1.0) < 0.1);

				*pNorm = normal.x; ++pNorm; 
				*pNorm = normal.y; ++pNorm; 
				*pNorm = normal.z; ++pNorm; 
			}
			nbuf->unlock();

			// Now use Ogre's ability to reorganize the vertex buffers the best way
			Ogre::VertexDeclaration* newDecl = 
				ogreSubMesh->vertexData->vertexDeclaration->getAutoOrganisedDeclaration(false,false,false);
			Ogre::BufferUsageList bufferUsages;
			for (size_t u = 0; u <= newDecl->getMaxSource(); ++u)
				bufferUsages.push_back(Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY);
			ogreSubMesh->vertexData->reorganiseBuffers(newDecl, bufferUsages);

			if (mesh->matarray != NULL)
			{
				ogreSubMesh->setMaterialName(ReplaceSpaces(sMatPrefix+mat->name));
				foundMaterial.insert(mat->name);
			}
		}
		RelMeshObj3ds (&mesh);
	}

	// Handle only the materials found on the object
	namelist3ds *matlist = NULL;
	material3ds *mat = NULL;

	Ogre::MaterialManager* pMatMgr = Ogre::MaterialManager::getSingletonPtr();
	assert(pMatMgr != NULL);

	Ogre::MaterialSerializer matSer;
	bool bExportMat = false;

	GetMaterialNameList3ds(db, &matlist);
	PRINT_ERRORS_RETURN(file);
	for (i = 0; i < matlist->count; i++)
	{
		if (foundMaterial.find(matlist->list[i].name) == foundMaterial.end())
			continue;

		pLogMgr->logMessage("Creating material " + ReplaceSpaces(matlist->list[i].name));
		GetMaterialByName3ds(db, matlist->list[i].name, &mat);
		PRINT_ERRORS_RETURN(file);

		// When used in Ogre application, the material could already exist
		pMatMgr->unload(ReplaceSpaces(sMatPrefix+matlist->list[i].name));
		pMatMgr->remove(ReplaceSpaces(sMatPrefix+matlist->list[i].name));

		Ogre::MaterialPtr ogremat = pMatMgr->create(ReplaceSpaces(sMatPrefix+matlist->list[i].name), 
			Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
		pLogMgr->logMessage("Created.");

		ogremat->setAmbient(mat->ambient.r, mat->ambient.g, mat->ambient.b);
		ogremat->setDiffuse(mat->diffuse.r, mat->diffuse.g, mat->diffuse.b, 1 + mat->transparency);
		// TODO : check why we should ignore these
		// ogremat->setSpecular(mat->specular.r, mat->specular.g, mat->specular.b, 1);
		//ogremat->setShininess(mat->shininess);

		// Handle transparency
		if (1.0f + mat->transparency < 1.0f)
		{
			ogremat->setSceneBlending(Ogre::SBT_TRANSPARENT_ALPHA); 
			ogremat->setDepthWriteEnabled(false); 
		}

		if (mat->twosided)
			ogremat->setCullingMode(Ogre::CULL_NONE);

		bitmap3ds& mapset = mat->texture.map;
		if (strlen(mapset.name) > 0)
		{
			Ogre::TextureUnitState *tu;		
			tu = ogremat->getTechnique(0)->getPass(0)->createTextureUnitState(mapset.name);
			tu->setTextureScale(mapset.uscale,mapset.vscale);
			tu->setTextureScroll(mapset.uoffset,mapset.voffset);
			tu->setTextureRotate( Ogre::Degree(mapset.rotation) );	
			vTextnames.insert(mapset.name);
		}

		matSer.queueForExport(ogremat);
		bExportMat = true;
	}
	if (bExportMat)
	{
		matSer.exportQueued(outfolder + Ogre::String(szMeshPrefix)+".material");
		for(std::set<CString>::const_iterator itr=vTextnames.begin();itr!=vTextnames.end();++itr)
		{
			CString texPath = dsfolder + *itr;
			CString destPath = _outfolder+*itr;
			::CopyFile(texPath,destPath,false);
		}
	}
	ReleaseNameList3ds(&matlist);

	// Set bounds
	ogreMesh->_setBoundingSphereRadius(Ogre::Math::Sqrt(maxSquaredRadius));
	ogreMesh->_setBounds(Ogre::AxisAlignedBox(min, max), false);
	dim = max - min;

	ReleaseNameList3ds(&meshlist);
	ReleaseNameList3ds(&objectlist);

	// Write the mesh file
	Ogre::MeshSerializer meshSer;
	meshSer.exportMesh(ogreMesh.getPointer(), outfolder+Ogre::String(szMeshPrefix)+".mesh");
	Ogre::MeshManager::getSingleton().remove(ogreMesh->getHandle());

	ReleaseDatabase3ds(&db);
	//CloseFile3ds(ifile);

	return true;
}

C3DTools::C3DS2Mesh::C3DS2Mesh( const char* meshPrefix, const char* matPrefix, float scale /*=1.0f*/ )
{
	mMeshPrefix = meshPrefix;
	mMaterailPrefix = matPrefix;
	mScale = scale;
}