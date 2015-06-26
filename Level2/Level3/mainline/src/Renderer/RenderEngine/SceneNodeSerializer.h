#pragma once
#include "DotSceneSrc/dotSceneUtils.h"
class TiXmlElement;
class C3DNodeObject; 

namespace Ogre
{
	class SceneNodeSerializer
	{
	public:
		void InitSave(CString workdir); //clean dir files;
		
		void Do(CString xmlFile, SceneNode *pNode);
			
	protected:
		CString mworkDir;
		void _doSceneNode( TiXmlElement* pElm,SceneNode* pNode);
		void _doEntity(TiXmlElement* pElm, Entity* pent);

		std::map<String,CString> m_vSavedmeshs;
	};


	class SceneNodeLoader
	{
	public:
		void InitRead(CString workDir);
		void Do(CString xmlFile, C3DNodeObject& pParent);

		void _doSceneNode(C3DNodeObject& pParent, TiXmlElement *pNode);
		void _doEntity( C3DNodeObject& pParent, TiXmlElement *pNode);
	protected:
		CString mworkDir;
	};

}

