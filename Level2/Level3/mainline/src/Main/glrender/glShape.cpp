#include "stdafx.h"
#include <cmath>
#include "glshape.h"

//const static char szFlightClass[] = "boli";
//const static char szFlightTire[] = "tire";
const static char materialcolor[] =  "default2";

CglShape::CglShape(){
	m_p3dsFile = NULL;
	m_ptextures = new CTextureResource();
}

CglShape::~CglShape(){	
	freefile(m_p3dsFile);
	if(m_p3dsFile)lib3ds_file_free(m_p3dsFile);
}
void CglShape::freefile(Lib3dsFile* file){
	if(!file)return;
	for(Lib3dsNode * node = file->nodes;node!=NULL;node=node->next)
		freeNode(node,file);
}
void CglShape::freeNode(Lib3dsNode * node,Lib3dsFile *file){
	for(Lib3dsNode * childnode = node->childs;childnode!=NULL;childnode=childnode->next){
		freeNode(childnode,file);
	}
	Lib3dsMesh *mesh = lib3ds_file_mesh_by_name(file, node->data.object.morph);
	if( mesh == NULL )
		mesh = lib3ds_file_mesh_by_name(file, node->name);
	freeMesh(mesh);
}
void CglShape::freeMesh(Lib3dsMesh * mesh){
	if(!mesh)return;
	if(mesh->user.d)glDeleteLists(mesh->user.d,1);
}


bool CglShape::LoadModel(){
	if(m_p3dsFile)lib3ds_file_free(m_p3dsFile);
	m_p3dsFile = lib3ds_file_load( _T(m_strFileName) );	

	if(!m_p3dsFile){
		return false;
	}
	//Lib3dsVector bmin,bmax;
	lib3ds_file_bounding_box_of_objects(m_p3dsFile,TRUE, FALSE, FALSE, m_min, m_max);

	m_scale[0] = m_max[0] - m_min[0];
	m_scale[1] = m_max[1] - m_min[1];
	m_scale[2] = m_max[2] - m_min[2];
	m_center[0] = (m_min[0] + m_max[0])/2;
	m_center[1] = (m_min[1] + m_max[1])/2;
	m_center[2] = (m_min[2] + m_max[2])/2; 
	m_size = max(m_scale[0], m_scale[1]); 
	m_size = max(m_size, m_scale[2]);

	if( !m_p3dsFile->nodes )
	{
		Lib3dsMesh *mesh;
		Lib3dsNode *node;

		for(mesh = m_p3dsFile->meshes; mesh != NULL; mesh = mesh->next)
		{
			node = lib3ds_node_new_object();
			strcpy(node->name, mesh->name);
			node->parent_id = LIB3DS_NO_PARENT;
			node->data.object.scl_track.keyL = lib3ds_lin3_key_new();
			node->data.object.scl_track.keyL->value[0] = 1.;
			node->data.object.scl_track.keyL->value[1] = 1.;
			node->data.object.scl_track.keyL->value[2] = 1.;
			lib3ds_file_insert_node(m_p3dsFile, node);
		}
	}
	//load material texture
	char strpath[512];
	strcpy(strpath,_T(m_strFileName));
	for(int i=(int) strlen(strpath)-1;i>=0;--i){
		if(strpath[i] =='\\'|| strpath[i] == '//'){
			strpath[i+1]=0;break;
		}
	}
	for(Lib3dsMaterial * pMat = m_p3dsFile->materials;pMat!=NULL;pMat = pMat->next){
		if(!pMat->texture1_map.name[0])continue;
		CString texturefile;
		pMat->user.p = (void *)m_ptextures;
		texturefile.Format("%s%s",strpath,_T(pMat->texture1_map.name));
		m_ptextures->NewTextureAndReside(texturefile,_T(pMat->texture1_map.name));
	}



	return true;
	//if(m_pFileLoader)
	//	m_pFileLoader = new C3DFileLoader(); 
	//  t3dModel = new C3dModel();
	//m_pFileLoader->LoadModel(m_strFileName,t3dModel);

}

//return true : have texture  return false: no texture
bool  CglShape::applyMaterial(Lib3dsMaterial * pMat){
	glDisable(GL_TEXTURE_2D);

	if(!pMat) return false;
	float s;
	/*if( pMat->two_sided )
	glDisable(GL_CULL_FACE);
	else
	glEnable(GL_CULL_FACE);*/

	glMaterialfv(GL_FRONT, GL_AMBIENT, pMat->ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, pMat->diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, pMat->specular);
	s =std::pow(2.0f, 10.0f*pMat->shininess);
	if (s>128.0) s=128.0;

	glMaterialf(GL_FRONT, GL_SHININESS, s);

	if(pMat->texture1_map.name[0]){
		CTextureResource * pTextureres = (CTextureResource*)pMat->user.p;
		CTexture * pTexture = pTextureres->getTexture( _T(pMat->texture1_map.name) );

		if(pTexture){
			pTexture->Apply();
			return true;
		}
	}
	return false;
}
void  CglShape::displayMesh(Lib3dsMesh *mesh)
{
	if(!mesh) return;
	ASSERT(m_p3dsFile);	
	bool tex_mode = false;
	if (glIsEnabled(GL_COLOR_MATERIAL))
	{
		glDisable(GL_COLOR_MATERIAL);
	}

	Lib3dsFace * pFace = NULL;
	pFace = &mesh->faceL[0];
	Lib3dsMaterial * mat_test = lib3ds_file_material_by_name(m_p3dsFile,pFace->material);
	if (strcmp(materialcolor,mat_test->name)==0)
	{
		glEnable(GL_COLOR_MATERIAL);
		glColorMaterial(GL_FRONT,GL_AMBIENT_AND_DIFFUSE);
	}
		
	if (!mesh->user.d) 
	{
		mesh->user.d=glGenLists(1);
		glNewList(mesh->user.d, GL_COMPILE);

		{
			unsigned p;
			Lib3dsVector *normalL=(Lib3dsVector *)malloc(3*sizeof(Lib3dsVector)*mesh->faces);
			Lib3dsMaterial *oldmat = (Lib3dsMaterial *)-1;				
			lib3ds_mesh_calculate_normals(mesh, normalL);

			bool isdefaultMat = false;
			for (p=0; p<mesh->faces; ++p) {
				Lib3dsFace *f=&mesh->faceL[p];
				Lib3dsMaterial *mat=0;
				if (f->material[0]) {
					mat=lib3ds_file_material_by_name(m_p3dsFile, f->material);
					isdefaultMat = (strcmp(mat->name,"default")==0);
				}	

				if(mat != oldmat){					
					tex_mode = applyMaterial(mat);
					oldmat = mat;
				}
				glBegin(GL_TRIANGLES);
				glNormal3fv(f->normal);
				for (int i=0; i<3; ++i) {
					glNormal3fv(normalL[3*p+i]);

					if (tex_mode) {
						glTexCoord2fv( mesh->texelL[f->points[i]] );
					}

					glVertex3fv(mesh->pointL[f->points[i]].pos);
				}
				glEnd();


			}

			free(normalL);
		}

		glEndList();
	}

	if (mesh->user.d)
	{	
		glPushMatrix();
		glCallList(mesh->user.d);
		glPopMatrix();
	}

	//if(glIsEnabled(GL_COLOR_MATERIAL))
	//{
		//glDisable(GL_COLOR_MATERIAL);
	//}
    glEnable(GL_COLOR_MATERIAL);

}
void  CglShape::displayNode(Lib3dsNode *node){
	ASSERT(m_p3dsFile);

	{
		Lib3dsNode *p;
		for (p=node->childs; p!=0; p=p->next) {
			displayNode(p);
		}
	}
	if (node->type==LIB3DS_OBJECT_NODE) {
		Lib3dsMesh *mesh;

		if (strcmp(node->name,"$$$DUMMY")==0) {
			return;
		}

		mesh = lib3ds_file_mesh_by_name(m_p3dsFile, node->data.object.morph);
		if( mesh == NULL )
			mesh = lib3ds_file_mesh_by_name(m_p3dsFile, node->name);
		displayMesh(mesh);
	}
}

void CglShape::display(){	
	Lib3dsFile * pfile = m_p3dsFile;
	if(!pfile) return;
	if(pfile){
		for(Lib3dsNode* node = pfile->nodes;node!=NULL;node= node->next){
			displayNode(node);
		}
	}
	glDisable(GL_TEXTURE_2D);

	// t3dModel.Sync();
}