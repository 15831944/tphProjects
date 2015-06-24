#ifndef __MODLEFILE_LOADER
#define __MODLEFILE_LOADER
#include "./3dModel.h"
//  (Primary Chunk)
#define PRIMARY       0x4D4D

//  (Main Chunks)
#define OBJECTINFO    0x3D3D				// 
#define C3DSVERSION       0x0002				// .3ds
#define EDITKEYFRAME  0xB000				// 

// 
#define MATERIAL	  0xAFFF				// 
#define OBJECT		  0x4000				// ¡¢

//  
#define MATNAME       0xA000				// 
#define MATDIFFUSE    0xA020				// /
#define MATMAP        0xA200				// 
#define MATMAPFILE    0xA300				// 

#define OBJECT_MESH   0x4100				// 

//  OBJECT_MESH
#define OBJECT_VERTICES     0x4110			// 
#define OBJECT_FACES		0x4120			// 
#define OBJECT_MATERIAL		0x4130			// 
#define OBJECT_UV			0x4140			// 


struct tIndices 
{							
	unsigned short a, b, c, bVisible;	
};

// 
struct tChunk
{
	unsigned short int ID;					// ID		
	unsigned int length;					// 
	unsigned int bytesRead;					// 
};

class ModelFileLoader
{
public:
	virtual bool LoadModel(CString filename,C3dModel & tmodel) = 0;
};
#endif


#ifndef __3DSFILELOADER_H
#define __3DSFILELOADER_H

class C3DSFileLoader : public ModelFileLoader
{

public:
	C3DSFileLoader();

	virtual bool LoadModel(CString filename,C3dModel & tmodel);
	//bool Import3DS(t3DModel *pModel, char *strFileName);

private:
	// 
	int GetString(char *);
	// 
	void ReadChunk(tChunk *);
	// 
	void ProcessNextChunk(t3DModel *pModel, tChunk *);
	// 
	void ProcessNextObjectChunk(t3DModel *pModel, t3DObject *pObject, tChunk *);
	// 
	void ProcessNextMaterialChunk(t3DModel *pModel, tChunk *);
	// RGB
	void ReadColorChunk(tMaterialInfo *pMaterial, tChunk *pChunk);
	// 
	void ReadVertices(t3DObject *pObject, tChunk *);
	// 
	void ReadVertexIndices(t3DObject *pObject, tChunk *);
	// 
	void ReadUVCoordinates(t3DObject *pObject, tChunk *);
	// 
	void ReadObjectMaterial(t3DModel *pModel, t3DObject *pObject, tChunk *pPreviousChunk);
	// 
	void ComputeNormals(t3DModel *pModel);
	// £¬
	void CleanUp();
	// 
	FILE *m_FilePointer;

	tChunk *m_CurrentChunk;
	tChunk *m_TempChunk;
};

#endif