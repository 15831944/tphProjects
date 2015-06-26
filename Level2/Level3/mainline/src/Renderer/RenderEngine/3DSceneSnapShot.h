#pragma once

#include "3DNodeObject.h"
#include "Camera.h"

#include "RenderDevice.h"

namespace Ogre
{
	class Overlay;
	class PanelOverlayElement;
	class TextAreaOverlayElement;
}

#include <vector>

class RENDERENGINE_API C3DSceneSnapShot
{
public:
	C3DSceneSnapShot();
	C3DSceneSnapShot(int nSnapshotSizeX, int nSnapshotSizeY, bool bSnapshotDispARCLogo = true);
	~C3DSceneSnapShot(void);

	void TakeSnapshot( const Ogre::String& strFileName, bool isShowText );
	void TakeSnapshot(int nWidth,int nHeight,BYTE* pBits,const CCameraData&cameraData);//for make movie ogre
	void TakeSnapshot(int nWidth,int nHeight,BYTE* pBits);//for make movie ogre, default camera
	void TakeSnapshot( int nWidth,int nHeight,BYTE* pBits,bool isShowText );

	int GetSnapshotSizeX() const { return m_nSnapshotSizeX; }
	int GetSnapshotSizeY() const { return m_nSnapshotSizeY; }
	void SetSnapshotSize(int nSnapshotSizeX, int nSnapshotSizeY) { m_nSnapshotSizeX = nSnapshotSizeX; m_nSnapshotSizeY = nSnapshotSizeY; }
	const Ogre::String GetSnapshotTag() const;
	void SetSnapshotTag(Ogre::String strSnapshotTag);
	bool GetSnapshotDispARCLogo() const { return m_bSnapshotDispARCLogo; }
	void SetSnapshotDispARCLogo(bool bSnapshotDispARCLogo) { m_bSnapshotDispARCLogo = bSnapshotDispARCLogo; }

protected:
	void SetupSnapshotRenderTarget(Ogre::RenderTarget* pRenderTarget);
	void SetupSnapshotRenderTarget(Ogre::RenderTarget* pRenderTarget,bool isShowText );
	void SetupSnapshotRenderTarget(const CCameraData& cameraData,Ogre::RenderTarget* pRenderTarget);

	virtual ViewPort GetSnapshotViewPort() = 0;

	void EncodeIntoBuffer( Ogre::TexturePtr texture, int nWidth, int nHeight, BYTE* pBits );

	bool InitSnapshotBasicOverlay();
	Ogre::Overlay* GetSnapshotLogoOverlay() const { return m_pSnapshotLogoOverlay; }
	static Ogre::Camera* createOrRetrieveSnapshotCamera(const CString& strName,Ogre::SceneManager* pScene);
private:

	int m_nSnapshotSizeX;
	int m_nSnapshotSizeY;

	bool m_bSnapshotDispARCLogo;

	Ogre::Overlay* m_pSnapshotLogoOverlay;
	Ogre::TextAreaOverlayElement* m_pSnapshotTimeTextOverlay;
};
