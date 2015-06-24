#pragma once


#include <map>
#include <Common/Referenced.h>
#include <Common/ref_ptr.h>
#include "glTextureResource.h"
//#include "./3DSFileLoader.h"
#include <vector>
#include <lib3ds/file.h>
#include <lib3ds/camera.h>
#include <lib3ds/mesh.h>
#include <lib3ds/node.h>

#include <lib3ds/material.h>
#include <lib3ds/matrix.h>
#include <lib3ds/vector.h>


class CglShape  : public  Referenced
{
	friend class CglShapeResource;
	friend class CglRenderer;
public:
	Lib3dsFile * m_p3dsFile; 
	//bounding box
	bool m_bloadmodel;
	//
	bool LoadModel();
	void freeNode(Lib3dsNode * node,Lib3dsFile *file);
	void freeMesh(Lib3dsMesh * mesh);
	void freefile(Lib3dsFile * file);

	bool applyMaterial(Lib3dsMaterial * pMat);
	virtual void displayMesh(Lib3dsMesh *mesh);
	virtual void displayNode(Lib3dsNode *node);
	CString m_strFileName;      // file name of the 3ds
	
public:
	
	CglShape();
	~CglShape();

	Lib3dsVector m_scale, m_center;
	float m_size;
	Lib3dsVector m_min, m_max;
//    C3DSFileLoader *m_pFileLoader;
//	C3dModel t3dModel;
	CTextureResource* m_ptextures;
	
	virtual void display();

};
extern "C"{
	void lib3ds_caculate_normal(const Lib3dsVector vertexs[3], Lib3dsVector normal);
};
