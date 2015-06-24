#include "stdafx.h"
#include "MovieRenderEngine.h"
#include "Render3DView.h"
#include "3DCamera.h"
#include "PBuffer.h"
#include "results\EventLogBufManager.h"


/////////////MovieOldRenderEngine///////////////////////////////////////////////////////////
MovieOldRenderEngine::MovieOldRenderEngine(C3DView* pView)
:m_pView(pView)
{
	m_pBuffer = NULL;
}

MovieOldRenderEngine::~MovieOldRenderEngine()
{
	EndRead();	
}



void MovieOldRenderEngine::setCamera(const C3DCamera& cam)
{
	*(m_pView->GetCameraData()) = cam;
	m_pView->GetCamera()->Update();
}

void MovieOldRenderEngine::RenderScreenToMem( int nWidth, int nHeight, BYTE* pMemBits )
{
	if(m_pView)
		m_pView->RenderOffScreen(m_pBuffer,nWidth,nHeight,pMemBits);
}

C3DCamera MovieOldRenderEngine::BeginRead()
{
	m_oldAnima = m_pView->m_nAnimDirection;
	m_pView->m_nAnimDirection = CEventLogBufManager::ANIMATION_FORWARD;
	m_pView->m_bAnimDirectionChanged = TRUE;
	m_oldCamera = *m_pView->GetCamera();
	return m_oldCamera;
}

void MovieOldRenderEngine::EndRead()
{
	setCamera(m_oldCamera);
	m_pView->m_nAnimDirection = m_oldAnima;
	delete m_pBuffer;
	m_pBuffer=NULL;
}

//////////////MovieNewRenderEngine//////////////////////////////////////////////////////////
MovieNewRenderEngine::MovieNewRenderEngine(CRender3DView* pView)
:m_pView(pView)
{

}

MovieNewRenderEngine::~MovieNewRenderEngine()
{

}



void MovieNewRenderEngine::setCamera(const C3DCamera& cam)
{
	m_pView->SetCameraData(cam);
}



void MovieNewRenderEngine::RenderScreenToMem( int nWidth, int nHeight, BYTE* pMemBits )
{
	m_pView->SnapshotRender(nWidth,nHeight,pMemBits);
}

C3DCamera MovieNewRenderEngine::BeginRead()
{
	m_pView->m_nAnimDirection = CEventLogBufManager::ANIMATION_FORWARD;
	m_pView->m_bAnimDirectionChanged = TRUE;
	m_oldCam = *m_pView->GetCameraData();
	return m_oldCam;
}

void MovieNewRenderEngine::EndRead()
{
	m_pView->SetCameraData(m_oldCam);
}

IMovieReader::~IMovieReader()
{
	EndRead();
}
