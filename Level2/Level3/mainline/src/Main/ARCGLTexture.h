#pragma once


class ARCGLTexture
{
friend class ARCGLTextureCreator;
protected:
	ARCGLTexture(GLuint glTexid);
	
public:
	~ARCGLTexture(void);
	GLuint getTexId()const{ return m_texId; }
protected:
	GLuint m_texId;
};


class ARCGLTextureCreator
{
public:
	static ARCGLTexture* CreateTextureFromFile(const CString& strFile, int& imgWidth, int& imgHeight );
};