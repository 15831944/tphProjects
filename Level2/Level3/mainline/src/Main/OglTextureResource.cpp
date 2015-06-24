#include "StdAfx.h"
#include ".\ogltextureresource.h"
#include "../common/ARCMathCommon.h"
#include "./TermPlan.h"

void OglTexture::Activate()
{
	glEnable(m_texType);
	glBindTexture(m_texType,m_nTexId);		
}
void OglTexture::DeActivate()
{
	glDisable(m_texType);
}
void OglTexture::SetImageData(int width,int height,GLenum format /* = GL_RGBA */,GLenum dataType /* = GL_UNSIGNED_BYTE */,GLvoid* data /* = 0 */)
{
	Activate();
	m_height = height;m_width = width;
	
	gluBuild2DMipmaps(m_texType,3,width,height,format,dataType,data);
}
OglTexture::OglTexture(CString name, GLuint texid, GLenum texType /* = GL_TEXTURE_2D */):m_uiqName(name),m_texType(texType),m_nTexId(texid)
{
	m_nRefCnt = 0;
}
void OglTexture::unRef()
{
	m_nRefCnt--;
}

OglTexture& OglTexture::getRef()
{
	m_nRefCnt++;
	return *this;
}

OglTexture::~OglTexture()
{
	glDeleteTextures(1,&m_nTexId);
}



const static int defaultImageWidth = 64;
const static int defaultImageHeight =64;
static GLubyte defaultImage[defaultImageWidth][defaultImageHeight][3]=
{
 0,
};

const int OglTextureResource::sm_nTextureCount = 6;
const char* OglTextureResource::sm_szFileNames[] = {
	"Stretch.jpg",
	"Intersection.jpg",
	"Turnoff.jpg",
	"LineParking.jpg",
	"LaneAdapter.jpg",
	"Zebra.jpg",
};


bool OglTextureResource::OnInit()
{
	
	//init the default texture
	for(int i=0;i<defaultImageHeight;++i)
	{
		for(int j=0;j<defaultImageWidth;++j)
		{
			int c = ( ( (i&0x8)==0 )^( (j&0x8)==0 ) )*255;
			defaultImage[i][j][0] = (GLubyte)c;
			defaultImage[i][j][1] = (GLubyte)c;
			defaultImage[i][j][2] = (GLubyte)c;
			//defaultImage[i][j][3] = (GLubyte)255;
		}
	}
	//
	ilInit();
	
	//add a default texture;
	
	
	return true;
}



OglTextureResource::OglTextureResource(void)
{
	m_pDefaultTexture  = NULL;
	OnInit();	
}

OglTextureResource::~OglTextureResource(void)
{
	if(m_pDefaultTexture) delete m_pDefaultTexture;
	TexContainer::iterator itr ;
	for( itr = m_vTextures.begin();itr!=m_vTextures.end();itr++)
	{
		delete (*itr);
	}

}

void OglTextureResource::BuidDefaultTexture()
{
	
	
}


OglTexture* OglTextureResource::NewTexture2D(CString fileName) //no default
{
	//fileName = "C:\\untitled2.bmp";
	OglTexture * pres = GetTextureByName(fileName);
	
	if( pres == NULL )
	{
		
		//ILuint ImageName;
		//ilGenImages(1, &ImageName);
		//ilBindImage(ImageName);
		//ilLoadImage(fileName.GetBuffer());
		
		//ILenum Error; bool ErrorOcur = false;		
		/*while ((Error = ilGetError()) != IL_NO_ERROR) 
		{		
			ErrorOcur = true;
		}
		if(ErrorOcur)
		{
			ilDeleteImages(1, &ImageName);
			return m_pDefaultTexture;
		}*/
		
		ilutInit();
		ilutRenderer(ILUT_OPENGL);
		ilutEnable(ILUT_OPENGL_CONV);

		//ILuint Width, Height;
		//Width = ilGetInteger(IL_IMAGE_WIDTH);
		//Height = ilGetInteger(IL_IMAGE_HEIGHT);
		//iluScale(ARCMath::FloorTo2n(Width),ARCMath::FloorTo2n(Height),24);
		GLuint newTexid  = ilutGLLoadImage(fileName.GetBuffer());
		//ilDeleteImages(1, &ImageName);

		/*ilutRenderer(ILUT_OPENGL);
		ilutEnable(ILUT_OPENGL_CONV);
		GLuint newTexid = ilutGLLoadImage(fileName.GetBuffer());*/
		OglTexture * newtex = new OglTexture(fileName, newTexid,GL_TEXTURE_2D);
		m_vTextures.insert(newtex);
		return newtex;
	}
	
	return pres;
	
}
OglTexture* OglTextureResource::GetTextureByName(CString texName)
{
	TexContainer::iterator itr ;
	for( itr = m_vTextures.begin();itr!=m_vTextures.end();itr++)
	{
		if( (*itr)->getName().CompareNoCase(texName) == 0 ) return (*itr);
	}
	return NULL;

}
OglTexture& OglTextureResource::RefTextureByName(CString texName)
{
	if(!m_pDefaultTexture)BuidDefaultTexture();
	
	OglTexture * pTex = GetTextureByName(texName);
	if(!pTex)
	{
		pTex = NewTexture2D(texName);
	}
	pTex->getRef();
	return *pTex;
}

OglTexture& OglTextureResource::RefTextureByType(TextureType textype)
{
	CString textPath = GetTexturePath(textype);
	OglTexture * pTex = GetTextureByName(textPath);
	if(!pTex)
	{
		pTex = NewTexture2D(textPath);
	}
	pTex->getRef();
	return *pTex;
}



void OglTextureResource::ReleaseTexture(CString TexName)
{
	OglTexture * pTex = GetTextureByName(TexName);
	if( pTex == m_pDefaultTexture ) return;
	pTex->unRef();
	int refcount = pTex->m_nRefCnt;
	if( refcount < 1 )
	{
		delete pTex;
		m_vTextures.erase( m_vTextures.find(pTex) );
	}
}

CString OglTextureResource::GetTexturePath(TextureType textype)
{
	CString fullPath;
	fullPath.Format("%s\\%s",((CTermPlanApp*)AfxGetApp())->GetTexturePath(),sm_szFileNames[textype]);
	return fullPath;
	
}