#pragma once
#include "./SceneOverlayDisplay.h"
#include ".\3DNodeObject.h"



namespace Ogre
{
	class TextAreaOverlayElement;
	class ManualObjectExt;
}

class  COverlayTag
{
public:
	COverlayTag(){ m_pOverlay = NULL; m_bHideMan = false;}
	COverlayTag(Ogre::Overlay* pOverlay){ m_pOverlay = pOverlay; }
	operator bool () const { return NULL != m_pOverlay; }
	void Show(bool b){ m_bShow = b; }
	bool IsShow()const{ return m_bShow ;};
	void SetHideMan(bool bHide) { m_bHideMan = bHide; }

	//set parent attached node
	void SetAttachNode(const C3DNodeObject& node){ m_attachNode = node; }

	void ShowToViewport(Ogre::Viewport*);
	void Hide();

	void SetTextColor(const ARCColor3& c);
	void SetText(CString sTag);
	
	Ogre::TextAreaOverlayElement* GetTextArea();
protected:
	Ogre::Overlay* m_pOverlay;
	bool m_bShow;	
	C3DNodeObject m_attachNode;
	//so sorry for the ogre bug(maybe) you must be a man can't be show
	//i hope maybe someday some guy would make you be beautiful to get you out of here
	//this maybe helpful: http://www.ogre3d.org/forums/viewtopic.php?f=4&t=59197
	bool m_bHideMan;
};

class RENDERENGINE_API COverlayTagList
{
public:
	COverlayTagList();
	virtual COverlayTag* CreateOrRetrieveTag(int nMobID, bool& bCreated);

	void ShowAll(Ogre::Viewport* );
	void HideAll();

	void  Show(int idx, bool bShow);

	void SetTag(int nId, const C3DNodeObject& nodeobj , const CString& sText, const ARCColor3& color = ARCColor3::Black );

	void SetTagNamePrefix(const CString& sPrefix){ m_strTagPrefix = sPrefix; };

	void SetAllShow(bool b){ m_bShowAll  = b; }
protected:
	bool m_bShowAll;
	typedef std::map<int, COverlayTag> DataList;
	DataList m_overlays;
	CString m_strTagPrefix;
	bool m_isSetEmptyTag;
};
//////////////////////////////////////////////////////////////////////////


//class IARCportLayoutEditContext;
//class CRender3DSceneTags: public CSceneOverlayDisplay
//{
//public:
//	CRender3DSceneTags(IARCportLayoutEditContext* pmodel);
//
//	
//	COverlayTagList m_paxTags;
//	COverlayTagList m_fltTags;
//	COverlayTagList m_vehicleTags;
//	COverlayTagList m_layoutObjectTags;
//	COverlayTagList m_landsidevehicleTags;
//	COverlayTagList m_processorTags;
//protected:
//	IARCportLayoutEditContext* m_pmodel;
//	virtual void PreDrawTo(Ogre::Viewport* );
//	virtual void EndDrawTo(Ogre::Viewport* ); //close all open overlays
//
//};


//////////////////////////////////////////////////////////////////////////
class RENDERENGINE_API CMovableTagList 
{
public:
	void SetStaticLayoutTag(int nId, Ogre::SceneNode* nodeobj , const CString& sText ,const ARCColor3& color = ARCColor3::Black );
	Ogre::ManualObjectExt*  SetTag(int nId, Ogre::SceneNode* nodeobj , const CString& sText ,const ARCColor3& color = ARCColor3::Black );

	void Show(int nId,bool bShow);
	void HideAll();
	void SetTagNamePrefix(const CString& sPrefix){ m_strTagPrefix = sPrefix; };

	~CMovableTagList(){ Clear(); }
protected:
	typedef std::map<int, Ogre::ManualObjectExt* > TagMap;
	TagMap m_TagList;
	void Clear();
	 Ogre::ManualObjectExt* getTagItem(int id)const;
	 Ogre::ManualObjectExt* getCreateTagItem(int id);
	 CString m_strTagPrefix;
};

class CRender3DSceneTags : public CSceneOverlayDisplay
{
public:
	CRender3DSceneTags();
	CMovableTagList m_paxTags;
	CMovableTagList m_fltTags;
	CMovableTagList m_vehicleTags;
	CMovableTagList m_layoutObjectTags;
	CMovableTagList m_landsidevehicleTags;
	CMovableTagList m_processorTags;
};