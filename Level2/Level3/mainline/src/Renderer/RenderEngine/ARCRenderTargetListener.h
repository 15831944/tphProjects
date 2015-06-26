#pragma once

#define StrCameraLight _T("Camera Light")
namespace Ogre
{
	//do extra works when render a window
	class ARCRenderTargetListener : public  RenderTargetListener
	{
	public:
		void preViewportUpdate(const RenderTargetViewportEvent& evt);
		void postViewportUpdate(const RenderTargetViewportEvent& evt);
		static ARCRenderTargetListener* GetInstance();



	};
}