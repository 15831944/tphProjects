#pragma once
#include "AfxHeader.h"
#include "Camera.h"
#include "3DSceneNode.h"
#include "3DNodeObject.h"

class CGridData;
class AutoScaleNodeUpdater;
class SceneState;
class MouseSceneInfo;
class C3DNodeObject;


class RENDERENGINE_API SelectRedSquareBuffer
{
public:
	SelectRedSquareBuffer(){ m_nSize = 0; mpRedSquare = NULL; mpScene = NULL; }
	void Init(int nBuffSize, Ogre::SceneManager* pScene);

	void Resize(int nNewSize);

	void HideAll();
	Ogre::MovableObject* GetObject(int idx);
	CString GetObjName(int idx)const;
	int m_nSize;	
	Ogre::ManualObject* mpRedSquare;
	Ogre::SceneManager* mpScene;
};

//observer pattern
//class SyncSubject
//{
//public:
//	SyncSubject():m_nUpdateFlag(1){ }
//	//the flag tells content updated
//	void Update(){ ++m_nUpdateFlag;}
//	int GetUpdateFlag()const{ return m_nUpdateFlag; } ;
//protected:
//	int m_nUpdateFlag;
//};

//class SyncSubjectRef
//{
//public:
//	SyncSubjectRef():pSubject(NULL),m_nlastSyncFlag(0){}
//	SyncSubjectRef(SyncSubject* pSub):pSubject(pSub),m_nlastSyncFlag(0){ }
//	bool isOutSync()const{  return pSubject?false:(m_nlastSyncFlag!=pSubject->GetUpdateFlag()); }
//	void doSync(){ if(pSubject)m_nlastSyncFlag = pSubject->GetUpdateFlag(); }
//protected:
//	SyncSubject* pSubject;
//	int m_nlastSyncFlag;
//};

class CGuid;
class RENDERENGINE_API C3DSceneBase 
{
public:
	C3DSceneBase(); 
	virtual ~C3DSceneBase();

	
    /** As setVisibilityFlags, except the flags passed as parameters are appended to the
    existing flags on this object. */
	void addVisibilityFlags(Ogre::uint32 flags);
        
    /** As setVisibilityFlags, except the flags passed as parameters are removed from the
    existing flags on this object. */
    void removeVisibilityFlags(Ogre::uint32 flags);

	//////mouse operations in the scene///
	virtual BOOL OnMouseMove(const MouseSceneInfo& mouseInfo);//
	
	virtual BOOL OnLButtonDblClk(const MouseSceneInfo& mouseInfo){ return TRUE; };	//
	virtual BOOL OnLButtonDown(const MouseSceneInfo& mouseInfo);	//
	virtual BOOL OnLButtonUp(const MouseSceneInfo& mouseInfo){ return TRUE; };	//
	
	virtual BOOL OnMButtonDblClk(const MouseSceneInfo& mouseInfo){ return TRUE; };	//
	virtual BOOL OnMButtonDown(const MouseSceneInfo& mouseInfo);//
	virtual BOOL OnMButtonUp(const MouseSceneInfo& mouseInfo){ return TRUE; };//
	
	virtual BOOL OnRButtonDblClk(const MouseSceneInfo& mouseInfo){  return TRUE; };//
	virtual BOOL OnRButtonDown(const MouseSceneInfo& mouseInfo);	//
	virtual BOOL OnRButtonUp(const MouseSceneInfo& mouseInfo){ return TRUE; };	//
	
	virtual BOOL OnMouseWheel(const MouseSceneInfo& mouseInfo, short zDelta){ return TRUE; };	//
	//////////////////////////////////////////////////////////////////////////


	bool IsValid()const{ return mpScene!=NULL; }
	bool MakeSureInited();
	void Destroy();
	void Clear(); //clear the scene

	
	//set camera all keep aspect ratio
	//scene operations	
	Ogre::SceneNode* AddSceneNode(const CString& nodeName);//add to root scene node
	Ogre::SceneNode* GetSceneNode(const CString& nodeName); //will create if not there
	Ogre::SceneNode* AddSceneNodeTo(const CString& nodeName, const CString& parentNodeName); //return the node child
	void RemoveSceneNode(const CString& nodeName);	//remove node from scene;
	void EnableAutoScale(const CString& nodeName, double dWinSize); //enable the node can be a fix windows size;

	//attach a camera light to active camera
	virtual void SetupCameraLight();//only once camera light
	CCameraNode GetCamera(const CString& camName)const;

	//static functions util of ogre
	Ogre::SceneManager* getSceneManager(){ return mpScene; }	

	static C3DNodeObject GetSel3DNodeObj( const MouseSceneInfo &mouseInfo );

	//for views operations
	virtual BOOL IsSelectObject()const;	
	virtual BOOL IsAnimation()const{ return FALSE; } 
	virtual Ogre::Plane getFocusPlan()const;

	C3DNodeObject GetRoot();

	C3DNodeObject GetObjectNode( const CGuid& guid );
	

protected:
	//share renderable object
	void InitShareObject();

protected:
	SelectRedSquareBuffer mSelectSquare;
	Ogre::SceneManager* mpScene;	
	SceneState* mState; //data of state;
	virtual SceneState& GetSceneState(){ return *mState; }
};
