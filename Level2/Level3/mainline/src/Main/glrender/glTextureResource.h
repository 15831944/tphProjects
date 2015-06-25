#pragma once

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


#include <vector>
class CTextureResourcePool;

class CTexture2 : public Referenced
{
public:
	typedef ref_ptr<CTexture2> RefPtr;
	
	CTexture2(CString file, CTextureResourcePool* pPool);
	~CTexture2();

	CString getFileName()const{  return m_filePath; }

	void Apply();

	void UnApply();
protected:
	GLuint m_iTextureId;
	GLenum m_eTextureType;	
	CString m_filePath;
	CTextureResourcePool* m_pPool;
};

class CTextureResourcePool
{
	friend class CTexture2;
public:
	
	typedef std::vector<CTexture2* >  TexutreList;

	CTexture2* getTexture(CString filePath);
protected:
	TexutreList m_datalist;
	CTexture2* newTexture(CString file);
	void remove(CTexture2* t);
};

