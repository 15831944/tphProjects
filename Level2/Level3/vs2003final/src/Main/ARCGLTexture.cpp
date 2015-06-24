#include "StdAfx.h"
#include ".\arcgltexture.h"
#include "../Common/ARCMathCommon.h"

#include <Il/il.h>
#include <Il/ilut.h>

ARCGLTexture::ARCGLTexture( GLuint glTexid )
{
	ASSERT( glIsTexture(glTexid) );
	m_texId = glTexid;
}
ARCGLTexture::~ARCGLTexture(void)
{
	glDeleteTextures( 1, &m_texId );
}

//////////////////////////////////////////////////////////////////////////

ILenum ilutGLFormat(ILenum Format, ILubyte Bpp)
{
	if (Format == IL_RGB || Format == IL_BGR) {
		if (ilutIsEnabled(ILUT_OPENGL_CONV)) {
			return GL_RGB8;
		}
	}
	else if (Format == IL_RGBA || Format == IL_BGRA) {
		if (ilutIsEnabled(ILUT_OPENGL_CONV)) {
			return GL_RGBA8;
		}
	}

	return Bpp;
}

ARCGLTexture* ARCGLTextureCreator::CreateTextureFromFile( const CString& strFile, int& imgWidth, int& imgHeight )
{
	ILuint ImageName;
	ilGenImages(1, &ImageName);
	ilBindImage(ImageName);
	ILboolean bPicValid = ilLoadImage( strFile.GetString() );
	if(!bPicValid){
		ilDeleteImage(ImageName);
		return NULL;
	}
	
	ILuint Width = ilGetInteger(IL_IMAGE_WIDTH);
	ILuint Height = ilGetInteger(IL_IMAGE_HEIGHT);
	ILuint Bpp    = ilGetInteger(IL_IMAGE_BPP);
	ILuint Format = ilGetInteger(IL_IMAGE_FORMAT);
	ILuint Depth  = ilGetInteger(IL_IMAGE_DEPTH);
	ILuint iType = ilGetInteger(IL_IMAGE_TYPE);

	imgWidth = Width;
	imgHeight = Height;

	GLint maxTexWidth =0;
	GLint maxTexHeight =0;
	GLint picWidth = ARCMath::FloorTo2n(Width);
	GLint picHeight = ARCMath::FloorTo2n(Height);
	while(maxTexHeight==0 || maxTexWidth==0)
	{			
		glTexImage2D(GL_PROXY_TEXTURE_2D, 0, ilutGLFormat(Format,Bpp), picWidth,picHeight,0,Format, iType, 0 );
		glGetTexLevelParameteriv(GL_PROXY_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &maxTexWidth);
		glGetTexLevelParameteriv(GL_PROXY_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &maxTexHeight);
		picHeight/=2;
		picWidth/=2;
		if(picHeight < 2 || picWidth < 2)
			break;
	}

	ILuint scaleWidth = min(  ARCMath::FloorTo2n(Width), maxTexWidth);
	ILuint scaleHeigh = min(  ARCMath::FloorTo2n(Height), maxTexHeight);	
	if(scaleWidth!= Width || scaleHeigh!= Height )
		iluScale(scaleWidth,scaleHeigh,Depth);

	//glTexParameteri(GL_TEXTURE_2D , GL_TEXTURE_MAX_LEVEL ,0);
	GLuint iTexture  = ilutGLBindTexImage();	
	ilDeleteImage(ImageName);
	
	if(	glIsTexture(iTexture) )
	{
		return new ARCGLTexture(iTexture);
	}
	return NULL;
	
}

