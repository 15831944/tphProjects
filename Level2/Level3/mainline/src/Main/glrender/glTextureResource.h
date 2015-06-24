#ifndef  CGLTEXTURE_RESOURCE_DEF
#define CGLTEXTURE_RESOURCE_DEF
#include <gl/GL.h>
#include <map>
#include <Common/Referenced.h>
#include <Common/ref_ptr.h>

#pragma once


class CTexture : public Referenced
{
friend class CTextureResource;
private:
	GLuint m_iTextureId;
	GLenum m_eTextureType;	
	
	CString m_strFileName;


public:
	typedef ref_ptr<CTexture> RefPtr;
	void Apply();
	void UnApply();

	void setTextureType(GLenum texType);
	GLenum GetTextureType()const;
	
	CTexture();
	CTexture(const CString& strFile);
	~CTexture();

	CString getFileName()const{ return m_strFileName; }

};



class CTextureResource 
{
public:
	CTextureResource(){}
	CTextureResource(const CString& foldpath);
	~CTextureResource(void);

	void SetResourcePath(const CString& foldpath);

	CTexture * getTexture(const CString& strID);	
	
	CTexture* getTextureByFileName(const CString& strFilename);
	
	CTexture * NewTextureAndReside(const CString& filename, const CString& strID);
	CTexture * NewTexture(const CString& filename);

	BOOL Load();

	void removeTexture(const CString& strID);

private: 
	std::map<CString, ref_ptr<CTexture> > m_vResideTextures;
	std::vector<CTexture*> m_vTextures;

	CString m_strpath;
};


#endif 