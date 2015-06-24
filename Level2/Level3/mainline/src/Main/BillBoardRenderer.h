#ifndef __BILLBOARDRENDERER_H
#define __BILLBOARDRENDERER_H
#include "../Engine/BillboardProc.h"
#pragma once
class C3DView;
class OglTexture;
#include "./ProcRenderer.h"

class BillBoardRenderer : public ProcessorRenderer
{
public:
	BillBoardRenderer(BillboardProc * _pProc,C3DView * _pView);
	virtual ~BillBoardRenderer(void);
	//bool LoadBmpTexture(char * filepath);
	bool MakeTextTexture(CString text,int width,int height);
	void Render(C3DView *pview);
	bool BuildTexture();
	void ReleaseTexture();
	bool needRegenTexture();
	OglTexture* pTexture ;

	bool LoadFileTexture(CString filename);
protected:
	BillboardProc * m_pProc;
	GLuint m_iTextureId;
	GLuint m_iDispId;
	
	int m_iBMpStamp;
	int m_iTextStamp;
	bool m_bUseText;
	
	DistanceUnit w,c;

	static GLenum m_targetTexture;
	void DoSync();

	C3DView * m_pView;
};
#endif