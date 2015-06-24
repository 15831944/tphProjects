#include "StdAfx.h"
#include ".\glrenderer.h"
#include <cmath>
#include "glShapeResource.h"

CglRenderer::CglRenderer(){
	m_pShapes = new CglShapeResource;
	m_pTextures = new CTextureResource;
}


CglRenderer::~CglRenderer(){
	delete m_pShapes;
	delete m_pTextures;
}


bool CglRenderer::apply_material(Lib3dsMaterial * pMat){
	
	
	glDisable(GL_TEXTURE_2D);

	if(!pMat){
		static const Lib3dsRgba a={0.2f, 0.2f, 0.2f, 1.0f};
		static const Lib3dsRgba d={0.8f, 0.8f, 0.8f, 1.0f};
		static const Lib3dsRgba s={0.0f, 0.0f, 0.0f, 1.0f};
		glMaterialfv(GL_FRONT, GL_AMBIENT, a);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, d);
		glMaterialfv(GL_FRONT, GL_SPECULAR, s);
		return false;
	}

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


void CglRenderer::render_node(Lib3dsNode *node, Lib3dsFile * file){
	ASSERT(file);

	{
		Lib3dsNode *p;
		for (p=node->childs; p!=0; p=p->next) {
			render_node(p,file);
		}
	}
	if (node->type==LIB3DS_OBJECT_NODE) {
		Lib3dsMesh *mesh;

		if (strcmp(node->name,"$$$DUMMY")==0) {
			return;
		}

		mesh = lib3ds_file_mesh_by_name(file, node->data.object.morph);
		if( mesh == NULL )
			mesh = lib3ds_file_mesh_by_name(file, node->name);
		render_mesh(mesh,file);
	}

}


void CglRenderer::render_mesh(Lib3dsMesh *mesh, Lib3dsFile * file){
	if(!mesh) return;
	ASSERT(file);	
	bool tex_mode = false;

	if (!mesh->user.d) {
		ASSERT(mesh);
		if (!mesh) {
			return;
		}

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
					mat=lib3ds_file_material_by_name(file, f->material);
					isdefaultMat = (strcmp(mat->name,"default")==0);
				}	
				
				/*glDisable(GL_COLOR_MATERIAL);		
				if(isdefaultMat) glEnable(GL_COLOR_MATERIAL);*/
				
				if(mat != oldmat){					
					tex_mode = apply_material(mat);
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

	if (mesh->user.d) {	

		glPushMatrix();


		glCallList(mesh->user.d);

		glPopMatrix();

	}
	glEnable(GL_COLOR_MATERIAL);
}


void CglRenderer::render_shape(const CString& shapeIDstr){
	CglShape * pShape = m_pShapes->GetShape(shapeIDstr);

	if(!pShape)return;

	Lib3dsFile * pfile = pShape->m_p3dsFile;
	if(!pfile) pShape->LoadModel();
	if(pfile){
		pShape->display();
	}
}