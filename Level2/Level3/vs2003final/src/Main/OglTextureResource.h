#ifndef __OGL_TEXTURE_RESOUCE
#define __OGL_TEXTURE_RESOUCE


#include <Il/il.h>
#include <Il/ilut.h>
#include <loki/Singleton.h>
#include <set>
#pragma once


class OglTexture
{
friend class OglTextureResource;
private:
	CString m_uiqName;
	GLuint m_nTexId;   
	GLenum m_texType;    //GL_TEXTURE_2D,GL_TEXTURE_3D,GL_TEXTURE_NV_RECTANGLE

	GLuint m_width;
	GLuint m_height;


	int m_nRefCnt;
	
public:	
	OglTexture& getRef();
	void unRef();
	OglTexture(CString name, GLuint texid, GLenum texType = GL_TEXTURE_2D);
	~OglTexture();
	
	void Activate();
	void DeActivate();
	void SetImageData(int width,int height,GLenum format = GL_RGBA,GLenum dataType = GL_UNSIGNED_BYTE,GLvoid* data = 0);
	CString getName() const { return m_uiqName; }
	GLuint getTexId()const { return m_nTexId;	}


};

template <OglTexture*>
struct Less
{
	bool operator()(const OglTexture* t1,const OglTexture* t2)
	{
		return t1->getName().CompareNoCase(t2->getName()) < 0;
	}
};

class OglTextureResource
{
public:	
	enum TextureType{ StretchTexture=0,IntersectionTexture, TurnoffTexture, LineParkingTexture, LaneAdapterTexture,ZebraTexture};	
	static const char* sm_szFileNames[];
	static const int sm_nTextureCount;
public:

	OglTextureResource(void);
	~OglTextureResource(void);
public:	
	
	void ReleaseTexture(CString TexName);
	
	OglTexture& RefTextureByName(CString texName);

	OglTexture& RefTextureByType(TextureType textype);

private:
	OglTexture* NewTexture2D(CString fileName);
	void BuidDefaultTexture();
	CString GetTexturePath(TextureType textype);
	bool OnInit();
	OglTexture* GetTextureByName(CString TexName);
	
	typedef std::set<OglTexture*> TexContainer; // map<data,refcount>
	TexContainer m_vTextures;

	OglTexture* m_pDefaultTexture;

};




//#define TextureResInstance Loki::SingletonHolder<OglTextureResource>::Instance()
#endif