#pragma once


using namespace Ogre;
class AutoScaleNodeUpdater : public Ogre::SceneManager::Listener
{
public:
	AutoScaleNodeUpdater(){ mpShadowCam=  NULL; }
	virtual void preFindVisibleObjects(SceneManager* source, 
		SceneManager::IlluminationRenderStage irs, Viewport* v);
	bool getNodeProjSize(SceneNode* pNode , Camera* pCam, Vector3& proj);

	void AddNode(const String& name,double dwinsize){ mvNodeList[name] = dwinsize; }
	void Clear(){ mvNodeList.clear(); }

	virtual void postFindVisibleObjects(SceneManager* source, 
		SceneManager::IlluminationRenderStage irs, Viewport* v){};
	virtual void shadowTextureReceiverPreViewProj(Light* light, 
		Frustum* frustum) {};

	virtual void shadowTextureCasterPreViewProj(Light* light, 
		Camera* camera, size_t iteration) {
			mpShadowCam = camera;
		};
	virtual void shadowTexturesUpdated(size_t numberOfShadowTextures) {};
protected:
	std::map<Ogre::String,double> mvNodeList;
	Camera* mpShadowCam;

};