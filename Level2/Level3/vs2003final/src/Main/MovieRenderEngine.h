#pragma once

#include "3DView.h"
#include "3DCamera.h"

class CRender3DView;
class C3DCamera;

//read screen content
class IMovieReader
{
public:
	IMovieReader(){}
	virtual ~IMovieReader();
	

	virtual C3DCamera BeginRead()=0;
	virtual void EndRead(){}
public:
	virtual void RenderScreenToMem(int nWidth, int nHeight, BYTE* pMemBits)=0;
	virtual void setCamera(const C3DCamera& cam) = 0;
	
};

//////////////MovieOldRenderEngine////////////////////////////////////////////////////////////
class MovieOldRenderEngine : public IMovieReader
{
public:
	MovieOldRenderEngine(C3DView* pView);
	~MovieOldRenderEngine();

	virtual C3DCamera BeginRead();
	virtual void EndRead();
	virtual void setCamera(const C3DCamera& cam);
	
	virtual void RenderScreenToMem(int nWidth, int nHeight, BYTE* pMemBits);

private:
	C3DCamera m_oldCamera;
	C3DView* m_pView;
	PBufferPointer m_pBuffer;
	int m_oldAnima;
};

///////////MovieNewRenderEngine//////////////////////////////////////////////////////////////
class MovieNewRenderEngine : public IMovieReader
{
public:
	MovieNewRenderEngine(CRender3DView* pView);
	~MovieNewRenderEngine();

	virtual void setCamera(const C3DCamera& cam);

	virtual void RenderScreenToMem(int nWidth, int nHeight, BYTE* pMemBits);

	virtual C3DCamera BeginRead();
	virtual void EndRead();

private:
	CRender3DView* m_pView;
	CCameraData m_oldCam;
};