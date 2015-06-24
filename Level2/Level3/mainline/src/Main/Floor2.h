#pragma once

#include <Common/RenderFloor.h>
class CTVNode;
class C3DView;
class CxImage;


class CFloor2 : public CRenderFloor
{
public:
	CFloor2(int nLevel);
	CFloor2(int nLevel, const CString& sName);
	virtual ~CFloor2();

	void DrawOGL(C3DView * pView, double dAlt);
	void DrawOGLasLines(double dAlt);
	void DrawOGLNoTexture(bool bMinimal,double dAlt );
	
	void DrawOGLVisRegions(bool bThick, bool bDrawOutline,bool bDrawTop=true);
	
	void DrawGrid(double dAlt);
	void DrawPicture(double dAlt);
	void GenPicTexture();
	//BOOL ReloadMap();

	CTVNode* ParentNode() { return m_pParentNode; }
	void ParentNode(CTVNode* pParentNode) { m_pParentNode = pParentNode; }

	void InvalidatePictureMap(){ m_bPicTextureValid = FALSE;  }

protected:
	void InitDefault();
	void InitDefault_();

	BOOL GenerateTextureMap();
	CxImage* LoadImage(CString sFileName);

protected:
	GLuint m_piTexId[4];
	GLenum m_eTexTarget;
	int m_nTextureSizeWorld;
	double m_dMapAspectRatio;

	CTVNode* m_pParentNode;

	//floor
	GLuint m_nVRsurfaceDLID;
	GLuint m_nVRsidesDLID;
	GLuint m_nVRborderDLID;
	GLuint m_nMapDLID;

	GLuint m_iPicTexture;
	//CxImage* m_pImage;

	C3DView * m_pView;
};
typedef std::vector<CFloor2*> CFLOOR2LIST;

