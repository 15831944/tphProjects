#include "StdAfx.h"
#include ".\3dmodel.h"
#include "./3DView.h"
#include "./OglTextureResource.h"
#include "./TermPlan.h"

C3dModel::C3dModel(void)
{
	Init();
}

C3dModel::~C3dModel(void)
{
	glDeleteLists(m_gDisplist,1);
	for(size_t i=0;i<g_Texture.size();i++)
		g_Texture[i]->unRef();

	g_Texture.clear();

}

void C3dModel::Render(C3DView * pView)
{
	if(NeedSync())Sync(pView);
	glEnable(GL_LIGHTING);
	glCallList(m_gDisplist);
	glDisable(GL_LIGHTING);
}
void C3dModel::Sync(C3DView * pView)
{	
	//unref the g_texture;
	for(size_t i=0;i<g_Texture.size();i++)
		g_Texture[i]->unRef();

	g_Texture.clear();
	for(int i = 0; i < g_3DModel.numOfMaterials; i++)
	{
		if(strlen(g_3DModel.pMaterials[i].strFile) > 0)
		{			
			//CreateTexture(g_Texture, g_3DModel.pMaterials[i].strFile, i);	
			CString fullpath ;
			fullpath.Format("%s\\%s",((CTermPlanApp*)AfxGetApp())->GetTexturePath(),g_3DModel.pMaterials[i].strFile);
			OglTexture * pTex = & pView->getTextureRes().RefTextureByName(fullpath);
			g_Texture.push_back( pTex );
			g_3DModel.pMaterials[i].texureId = g_Texture.size()-1;
		}		
		else g_3DModel.pMaterials[i].texureId = -1;
	}
	glDeleteLists(m_gDisplist,1);
	m_gDisplist = glGenLists(1);
	glNewList(m_gDisplist,GL_COMPILE);
	for(int i = 0; i < g_3DModel.numOfObjects; i++)
	{

		if(g_3DModel.pObject.size() <= 0) break;

		t3DObject *pObject = &g_3DModel.pObject[i];

		if(pObject->bHasTexture) {		
			
			int texid = g_3DModel.pMaterials[pObject->materialID].texureId;
			if(texid <(int) g_Texture.size() && texid >=0 )
			{
				glColor3ub(255,255,255);
				g_Texture[ texid ]->Activate();
				glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);	
			}
		} else {			
			glDisable(GL_TEXTURE_2D);//g_Texture[pObject->materialID]->DeActivate();
			glColor3ub(255,255,255);
		}

		glBegin(GL_TRIANGLES);					

		for(int j = 0; j < pObject->numOfFaces; j++)
		{

			for(int whichVertex = 0; whichVertex < 3; whichVertex++)
			{

				int index = pObject->pFaces[j].vertIndex[whichVertex];


				glNormal3f(pObject->pNormals[ index ].x, pObject->pNormals[ index ].y, pObject->pNormals[ index ].z);


				if(pObject->bHasTexture) {


					if(pObject->pTexVerts) {
						glTexCoord2f(pObject->pTexVerts[ index ].x, pObject->pTexVerts[ index ].y);
					}
				} else {

					if(g_3DModel.pMaterials.size() && pObject->materialID >= 0) 
					{
						BYTE *pColor = g_3DModel.pMaterials[pObject->materialID].color;
						glColor3ub(pColor[0], pColor[1], pColor[2]);
					}
				}
				glVertex3f(pObject->pVerts[ index ].x, pObject->pVerts[ index ].y, pObject->pVerts[ index ].z);
			}
		}

		glEnd();				

		if(pObject->bHasTexture) {			
			glDisable(GL_TEXTURE_2D);//g_Texture[pObject->materialID]->DeActivate();
			glColor3ub(255, 255, 255);
		}

	}
	glEndList();
	m_bneedSync = false;
}

void C3dModel::Init()
{	
	g_3DModel.numOfMaterials = 0;
	g_3DModel.numOfObjects =0;
	m_bneedSync = true;	
	
}
void C3dModel::Uninit()
{
	for(size_t i=0;i<g_Texture.size();i++)
	{
		if(g_Texture[i])
			g_Texture[i]->unRef();
	}
}
void C3dModel::Release()
{	
	for(int i = 0; i < g_3DModel.numOfObjects; i++)
	{
		delete [] g_3DModel.pObject[i].pFaces;
		delete [] g_3DModel.pObject[i].pNormals;
		delete [] g_3DModel.pObject[i].pVerts;
		delete [] g_3DModel.pObject[i].pTexVerts;
	}

}