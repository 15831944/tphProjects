#include "StdAfx.h"
#include ".\render3dscenetags.h"
#include <common/ARCStringConvert.h>
#include "Common/IARCportLayoutEditContext.h"
#include <ogre/OgreTextAreaOverlayElement.h>
#include "Landside\ILandsideEditContext.h"
#include "ManualObjectExt.h"
#include "CustomizedRenderQueue.h"
#include "3DSceneNode.h"
using namespace Ogre;
#define  templateName  _T("SimpleTextOverlay")
#define  stextArea  _T("Desc")


void COverlayTag::ShowToViewport(Ogre::Viewport* pvp)
{
	if(!IsShow())
	{
		Hide();
		return;
	}
	if(!m_attachNode)
	{
		if ( !m_bHideMan)
		{
			Hide();
		}
		return;
	}
	if(!m_pOverlay)
	{
		Hide();
		return;
	}
	if(!m_attachNode.GetParent())
	{
		Hide();
		return;
	}
	const Ogre::Vector3& vpos = m_attachNode.GetSceneNode()->_getDerivedPosition();
	Ogre::Vector3 scrnpos = CSceneOverlayDisplay::_getScreenCoordinates(vpos, pvp);
	m_pOverlay->setScroll(scrnpos.x,scrnpos.y);	
	m_pOverlay->show();
}

void COverlayTag::Hide()
{
	if(m_pOverlay)m_pOverlay->hide();
}

void COverlayTag::SetTextColor( const ARCColor3& c )
{
	ColourValue color = OGRECOLOR(c);
	TextAreaOverlayElement*pText = GetTextArea();
	if(pText && pText->getColour()!=color)	
		pText->setColour( color );
}

void COverlayTag::SetText( CString sTag )
{
	TextAreaOverlayElement*pText = GetTextArea();
	if(pText && pText->getCaption()!=sTag.GetString())	
		pText->setCaption( sTag.GetString() );
}

Ogre::TextAreaOverlayElement* COverlayTag::GetTextArea()
{
	if(!m_pOverlay)
		return NULL;
	String contName = m_pOverlay->getName() + _T("/") + templateName;
	OverlayContainer* pContainer = (OverlayContainer*)m_pOverlay->getChild(contName);
	ASSERT(pContainer);
	if(!pContainer)
		return NULL;

	String areaInst = m_pOverlay->getName() + _T("/") + stextArea;	
	TextAreaOverlayElement* pText = (TextAreaOverlayElement*)OverlayManager::getSingleton().getOverlayElement(areaInst);
	ASSERT(pText);	
	return pText;
}
//////////////////////////////////////////////////////////////////////////
COverlayTagList::COverlayTagList()
{
	m_bShowAll =true;
	m_isSetEmptyTag = false;
	//
}
COverlayTag* COverlayTagList::CreateOrRetrieveTag( int nMobID, bool& bCreated )
{
	/*if((int)m_overlays.size()<=nMobID)
	//	m_overlays.resize(nMobID+1);*/

	//ugly resolution avoid an ogre1.71-1.72 bug(maybe) : the first text area can't be showed until viewport refresh
	//this maybe helpful: http://www.ogre3d.org/forums/viewtopic.php?f=4&t=59197
	//////////////////////////////////////////////////////////////////////////
	if ( !m_isSetEmptyTag)
	{
		Overlay* empty_overlay = OgreUtil::createOrRetriveOverlay(m_strTagPrefix);
		OverlayContainer* empty_container = static_cast<OverlayContainer*>(
			OverlayManager::getSingleton().createOverlayElement(
			"Panel", empty_overlay->getName()+_T("/")+templateName));
		empty_container->setMetricsMode(Ogre::GMM_PIXELS);
		empty_container->setPosition(0,0);
		empty_container->setDimensions(100,100);

		TextAreaOverlayElement* empty_textArea = static_cast<TextAreaOverlayElement*>(
			OverlayManager::getSingleton().createOverlayElement(
			"TextArea", empty_overlay->getName()+_T("/") + stextArea));
		empty_textArea->setMetricsMode(Ogre::GMM_PIXELS);
		empty_textArea->setPosition(0,0);
		empty_textArea->setDimensions(100,100);
		empty_textArea->setCharHeight(14);
		empty_textArea->setFontName("ArialFont");
		empty_textArea->setCaption("");

		empty_container->addChild(empty_textArea);
		empty_overlay->add2D(empty_container);
		empty_overlay->show();

		COverlayTag emptyObject = COverlayTag(empty_overlay);
		emptyObject.SetHideMan(true);
		m_overlays.insert(DataList::value_type(-1, emptyObject));

		m_isSetEmptyTag = true;
	}
	//////////////////////////////////////////////////////////////////////////

	bCreated = false;
	COverlayTag& pax = m_overlays[nMobID];
	if(!pax)
	{
		bCreated = true;
		//int nTickCnt = GetTickCount();
		CString instName = m_strTagPrefix + ARCStringConvert::toString(nMobID+1);
		Overlay* pOverlay = OgreUtil::createOrRetriveOverlay(instName);


		String contName = pOverlay->getName() + _T("/") + templateName;
		OverlayContainer* pContainer = (OverlayContainer*)pOverlay->getChild(contName);
		if(!pContainer){
			pContainer  = (OverlayContainer*)OverlayManager::getSingleton().cloneOverlayElementFromTemplate(templateName,pOverlay->getName());
			pOverlay->add2D(pContainer);
		}		

		pax = COverlayTag( pOverlay );
	}
	return &pax;
}

void COverlayTagList::ShowAll( Ogre::Viewport* pvp)
{
	if(!m_bShowAll)
		return;
	for(DataList::iterator itr = m_overlays.begin();itr!=m_overlays.end(); ++itr)
	{
		COverlayTag& obj = itr->second;
		obj.ShowToViewport(pvp);
	}

}

void COverlayTagList::HideAll()
{
	//if(m_bShowAll)
	//	return;

	for(DataList::iterator itr = m_overlays.begin();itr!=m_overlays.end(); ++itr)
	{
		COverlayTag& obj = itr->second;
		obj.Hide();
	}
	//for_each(m_overlays.begin(),m_overlays.end(),boost::BOOST_BIND(&COverlayObject::Hide, _1) );
}



void COverlayTagList::SetTag( int nId, const C3DNodeObject& nodeobj , const CString& sText, const ARCColor3& color /*= ARCColor3::BLACK*/ )
{
	bool bCreate = false;
	COverlayTag* pObj =  CreateOrRetrieveTag(nId,bCreate);
	ASSERT(pObj);
	pObj->SetText(sText);
	pObj->SetAttachNode(nodeobj);
	pObj->SetTextColor(color);
}

void COverlayTagList::Show( int idx, bool bShow )
{
	//if(idx<(int)m_overlays.size())
	//{
	//	m_overlays[idx].Show(bShow);
	//}
	DataList::iterator it;
	it = m_overlays.find(idx);
	if (it != m_overlays.end())
	{
		it->second.Show(bShow);
	}
}

//void CRender3DSceneTags::PreDrawTo( Ogre::Viewport*pvp )
//{	
//	bool ShowText = m_pmodel->IsShowText();
//	m_layoutObjectTags.SetAllShow(ShowText);
//	m_processorTags.SetAllShow(ShowText);
//
//
//	m_paxTags.ShowAll(pvp);
//	m_fltTags.ShowAll(pvp);
//	m_vehicleTags.ShowAll(pvp);
//	m_landsidevehicleTags.ShowAll(pvp);
//	m_layoutObjectTags.ShowAll(pvp);
//	m_processorTags.ShowAll(pvp);
//}
//void CRender3DSceneTags::EndDrawTo( Ogre::Viewport* )
//{
//	m_paxTags.HideAll();
//	m_fltTags.HideAll();
//	m_vehicleTags.HideAll();
//	m_landsidevehicleTags.HideAll();
//	m_layoutObjectTags.HideAll();
//	m_processorTags.HideAll();
//}
//
//CRender3DSceneTags::CRender3DSceneTags(IARCportLayoutEditContext* pmodel)
//{
//	m_paxTags.SetTagNamePrefix("Pax");
//	m_fltTags.SetTagNamePrefix("Flight");
//	m_vehicleTags.SetTagNamePrefix("Vehicle");
//	m_landsidevehicleTags.SetTagNamePrefix("LandsideVehicle");
//	m_layoutObjectTags.SetTagNamePrefix("layoutObject");
//	m_processorTags.SetTagNamePrefix("processors");
//
//	m_pmodel = pmodel;
//}
//////////////////////////////////////////////////////////////////////////
ManualObjectExt* CMovableTagList::getTagItem( int id ) const
{
	TagMap::const_iterator itr = m_TagList.find(id); 
	if(itr!=m_TagList.end())
		return itr->second;
	return NULL;	
}

void CMovableTagList::SetStaticLayoutTag( int nId, Ogre::SceneNode* nodeobj , const CString& sText ,const ARCColor3& color /*= ARCColor3::Black */ )
{
	ManualObjectExt* pTag = SetTag(nId,nodeobj,sText,color);
	pTag->setVisibilityFlags(ProcessorTagVisibleFlag);
}

Ogre::ManualObjectExt* CMovableTagList::SetTag( int nId, Ogre::SceneNode* nodeobj , const CString& sText ,const ARCColor3& color /*= ARCColor3::Black */ )
{
	ManualObjectExt* pTag = getCreateTagItem(nId);
	pTag->setColor(OGRECOLOR(color));
	pTag->clear();
	pTag->beginText();
	pTag->drawTopLeftAligmentText(sText.GetString());
	pTag->end();
	C3DSceneNode node (nodeobj);
	node.AddObject(pTag);
	return pTag;
}

Ogre::ManualObjectExt* CMovableTagList::getCreateTagItem( int id )
{
	ManualObjectExt* pTag = getTagItem(id);
	if(!pTag){
		CString sName = m_strTagPrefix + ARCStringConvert::toString(id+1) ;
		pTag = new ManualObjectExt(sName.GetString());
		pTag->setRenderQueueGroup(RenderObject_Tag);
		m_TagList[id] = pTag;
	}
	return pTag;
}

void CMovableTagList::Clear()
{
	for(TagMap::iterator it = m_TagList.begin();it!=m_TagList.end();++it)
	{
		delete it->second;
	}
	m_TagList.clear();
}

void CMovableTagList::HideAll()
{
	for(TagMap::iterator it = m_TagList.begin();it!=m_TagList.end();++it)
	{
		ManualObjectExt* pTag =  it->second;
		pTag->setVisible(false);
	}	
}

void CMovableTagList::Show( int nId,bool bShow )
{
	ManualObjectExt* pTag = getTagItem(nId);
	if(pTag){
		pTag->setVisible(bShow);
	}
}

CRender3DSceneTags::CRender3DSceneTags()
{
	m_paxTags.SetTagNamePrefix("Pax");
	m_fltTags.SetTagNamePrefix("Flight");
	m_vehicleTags.SetTagNamePrefix("Vehicle");
	m_landsidevehicleTags.SetTagNamePrefix("LandsideVehicle");
	m_layoutObjectTags.SetTagNamePrefix("layoutObject");
	m_processorTags.SetTagNamePrefix("processors");
}
