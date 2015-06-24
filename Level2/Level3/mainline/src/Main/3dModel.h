#ifndef __C3DMODEL_H
#define __C3DMODEL_H
#include <vector>
#include "../Common/Referenced.h"
#include "../Common/ref_ptr.h"
#pragma once

//vector for store vertex
class CVector3 
{
public:
	float x, y, z;
};

//vector for store texture coordinate
class CVector2 
{
public:
	float x, y;
};

//face info
struct tFace
{
	int vertIndex[3];			// vertex index 
	int coordIndex[3];			// texture coordinate index
};

//material info
struct tMaterialInfo
{
	char  strName[255];			// texture name
	char  strFile[255];			// filename of texture
	BYTE  color[3];				// color of object
	int   texureId;				// texture id
	float uTile;				// u 
	float vTile;				// v 
	float uOffset;			    // u offset
	float vOffset;				// v offset
} ;

struct t3DObject 
{
	int  numOfVerts;			// 
	int  numOfFaces;			// 
	int  numTexVertex;			// 
	int  materialID;			// 
	bool bHasTexture;			// 
	char strName[255];			// 
	CVector3  *pVerts;			// 
	CVector3  *pNormals;		// 
	CVector2  *pTexVerts;		// 
	tFace *pFaces;				// 
};

struct t3DModel 
{
	int numOfObjects;					// 
	int numOfMaterials;					// 
	std::vector<tMaterialInfo> pMaterials;	// 
	std::vector<t3DObject> pObject;			// 
};

class C3DView;
class OglTexture;

class C3dModel :public Referenced
{
friend class C3DSFileLoader;

const static GLuint viewMode = GL_TRIANGLES;
public:
	C3dModel(void);
	void Init();
	void Uninit();
	void Release();
	virtual ~C3dModel(void);
		
	void Render(C3DView * pView);
	bool NeedSync(){ return m_bneedSync; }
	void Update() { m_bneedSync = true; } 
	void Sync(C3DView * pView);
protected:
	t3DModel g_3DModel;
	std::vector<OglTexture*> g_Texture;

	GLuint m_gDisplist;
	bool m_bneedSync;
};






#endif