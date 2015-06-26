#include "StdAfx.h"
// 
// #include "3DNodeWithTag.h"
// #include <Onboard3d/ogreMFCheader.h>
// #include <Onboard3d/MovableText.h>
// 
// 
// using namespace Ogre;

//////////////////////////////////////////////////////////////////////////
// 
// C3DNodeWithTag::C3DNodeWithTag( Ogre::SceneNode* pNode )
// 	: C3DNodeWithTagBase(pNode)
// {
// 	if (pNode)
// 	{
// 		SceneManager* pManager = GetScene();
// 		if (pManager)
// 		{
// 			m_pText3DObj = MovableText::create(pManager);
// 			if (m_pText3DObj)
// 			{
// 				m_pText3DObj->setDefaultQueryFlags(0);
// 				OgreUtil::AttachMovableObject(m_pText3DObj, pNode);
// 				m_pText3DObj->setVisible(false);
// 				return;
// 			}
// 		}
// 	}
// 	ASSERT(FALSE);	
// }
// 
// C3DNodeWithTag::~C3DNodeWithTag()
// {
// // 	SceneManager* pManager = GetScene();
// // 	if(m_pText3DObj && pManager )
// // 	{
// // 		pManager->destroyMovableObject(m_pText3DObj);
// // 		m_pText3DObj = NULL;
// // 	}
// }
// 
// void C3DNodeWithTag::SetTextPos( const Ogre::Vector3& pos )
// {
// 	if(m_pText3DObj && GetSceneNode() )
// 	{
// 		m_pText3DObj->getParentNode()->setPosition(pos);
// 	}
// }
// 
// void C3DNodeWithTag::ShowText( bool bShow )
// {
// 	if(m_pText3DObj)
// 	{
// 		m_pText3DObj->setVisible(bShow);
// 	}
// }
// 
// void C3DNodeWithTag::SetText( const CString& str )
// {
// 	if(m_pText3DObj)
// 		m_pText3DObj->setCaption(str.GetString());
// }
// 
