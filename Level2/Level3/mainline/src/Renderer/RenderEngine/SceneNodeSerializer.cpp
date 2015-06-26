#include "StdAfx.h"
#include ".\scenenodeserializer.h"
#include <common/Guid.h>
#include "./3DNodeObject.h"
#include <common/TmpFileManager.h>
#include "RenderEngine.h"
#include ".\SelectionManager.h"
#include <common\FileInDB.h>
#include <Shlwapi.h>

#define STR_NODE _T("node")
#define STR_NAME _T("name")
#define STR_POS _T("position")
#define STR_ROT _T("rotation")
#define STR_SCAL _T("scale")
#define STR_ENT _T("entity")
#define STR_MESHFILE _T("meshfile")

using namespace Ogre;
using namespace Ogre::dsi;
void Ogre::SceneNodeSerializer::_doSceneNode( TiXmlElement* pParent,SceneNode* pSN  )
{	
	TiXmlElement *pNode = pParent->InsertEndChild( TiXmlElement(STR_NODE))->ToElement();
	//
	pNode->SetAttribute(STR_NAME,pSN->getName() );
	// write type	
	xml::writeNamedValue(pNode, STR_POS, pSN->getPosition());
	xml::writeNamedValue(pNode, STR_ROT, pSN->getOrientation());
	xml::writeNamedValue(pNode, STR_SCAL, pSN->getScale());
	// do autoTrack target if needed 
	// (lookAt can be only loaded, no way to serialize it, right ??)	
	// entities
	SceneManager::MovableObjectIterator eit = pSN->getCreator()->getMovableObjectIterator("Entity");
	Entity *pEnt = NULL;
	while (eit.hasMoreElements())
	{
		pEnt =(Entity*) eit.getNext();
		if	(pEnt->getParentNode() == pSN)	
			this->_doEntity(pNode, pEnt);
	}

	// do child nodes
	Ogre::Node::ChildNodeIterator it = pSN->getChildIterator();
	SceneNode *pCSN = NULL;
	while (it.hasMoreElements())
	{
		pCSN = (SceneNode *)&(*it.getNext());
		this->_doSceneNode(pNode, pCSN);
	}
}

void Ogre::SceneNodeSerializer::_doEntity( TiXmlElement* pParent, Entity* pEnt )
{
	
	TiXmlElement *pNode = pParent->InsertEndChild(TiXmlElement(STR_ENT))->ToElement();
	pNode->SetAttribute(STR_NAME, pEnt->getName() );

	CGuid guid;CGuid::Create(guid);
	MeshPtr ogreMesh = pEnt->getMesh();
	String ogrinMesh = ogreMesh->getName();
	CString& destMeshName = m_vSavedmeshs[ogrinMesh];
	if( destMeshName.IsEmpty() )
	{
		Ogre::MeshSerializer meshSer;
		destMeshName = (CString)guid + _T(".mesh");
		CString meshFile = mworkDir + destMeshName;
		meshSer.exportMesh(ogreMesh.getPointer(), meshFile.GetString());
	}	
	pNode->SetAttribute("meshFile",destMeshName.GetString());

	// TODO : pNode->SetAttribute("materialFile", XXX);
	// TODO : serialize vertexBuffer element
	// TODO : serialize indexBuffer element
}

void Ogre::SceneNodeSerializer::InitSave( CString workdir )
{
	mworkDir = workdir;
	CTmpFileManager::DeleteDirectoryFile(_T("*.mesh"),mworkDir);
}


#define  STR_MODELNODE _T("model")


void Ogre::SceneNodeSerializer::Do( CString xmlFile, SceneNode *pNode )
{
	ASSERT(!mworkDir.IsEmpty());
	TiXmlDocument xmlDoc;
	TiXmlDeclaration xmlDeclare("1.0","gb2312", "yes");
	xmlDoc.InsertEndChild(xmlDeclare);
	TiXmlElement* pRoot = new TiXmlElement(STR_MODELNODE);
	pRoot->SetAttribute(STR_VERSION,100);	
	xmlDoc.LinkEndChild( pRoot);
	_doSceneNode(pRoot,pNode);
	xmlDoc.SaveFile( (mworkDir+xmlFile).GetString() );
}
//////////////////////////////////////////////////////////////////////////
void Ogre::SceneNodeLoader::InitRead( CString workDir )
{
	mworkDir = workDir; 
	ResourceFileArchive::AddDirectory(workDir,workDir);
}

void Ogre::SceneNodeLoader::_doSceneNode( C3DNodeObject& pParent, TiXmlElement *pNode )
{
	
	// parse type, put in the scene graph if a target is found	
	SceneNode* pSceneNode = pParent.GetSceneNode();
	const char* sName = pNode->Attribute(STR_NAME);
	if(sName){ pParent.SetName(sName); }
	// do node PRS values first
	for (  TiXmlElement *pElem = pNode->FirstChildElement(); 
		pElem != 0;
		pElem = pElem->NextSiblingElement())
	{
		/// must INIT those here, ugly hack shit !!! :(
		Vector3  _dslCurrPos= Vector3::ZERO;
		Vector3  _dslCurrSca = Vector3::UNIT_SCALE;
		Quaternion  _dslCurrRot= Quaternion::IDENTITY;
		///
		CString value = pElem->Value();
		if (value.CompareNoCase(STR_POS)==0)
		{
			pSceneNode->setPosition(xml::readPosition(pElem));
			_dslCurrPos = pSceneNode->_getDerivedPosition();
		}
		else if (value.CompareNoCase(STR_SCAL)==0)
		{
			pSceneNode->setScale(xml::readScale(pElem));
			_dslCurrSca = pSceneNode->_getDerivedScale();
		}
		else if (value.CompareNoCase(STR_ROT)==0)
		{
			pSceneNode->setOrientation(xml::readOrientation(pElem));
			_dslCurrRot = pSceneNode->_getDerivedOrientation();
		}
		
	}
	// do movable objects now
	for (  TiXmlElement *pElem = pNode->FirstChildElement(); 
		pElem != 0; pElem = pElem->NextSiblingElement())
	{
		CString value = pElem->Value();
		if (value.CompareNoCase(STR_ENT)==0)
			this->_doEntity(pParent, pElem);	
		
	}
	// do child nodes as last !!!
	for (  TiXmlElement *pElem = pNode->FirstChildElement(); 
		pElem != 0; pElem = pElem->NextSiblingElement())
	{
		CString value = pElem->Value();
		// read child nodes if any
		if (value.CompareNoCase(STR_NODE)==0){
			C3DNodeObject childNode = pParent.CreateNewChild();
			SceneNode* pSceneNode = childNode.GetSceneNode();
			this->_doSceneNode(childNode, pElem);
		}
	}
}

void Ogre::SceneNodeLoader::_doEntity( C3DNodeObject& pParent, TiXmlElement *pNode )
{	
	Entity *pEnt = NULL;
	CString meshName = pNode->Attribute("meshFile") ? pNode->Attribute("meshFile") : _T("");
	pEnt = OgreUtil::createOrRetrieveEntity(pParent.GetIDName(),meshName,pParent.GetScene());
	pParent.AddObject(pEnt,true);
	
}

void Ogre::SceneNodeLoader::Do( CString xmlFile, C3DNodeObject& pParent )
{
	ASSERT(!mworkDir.IsEmpty());
	TiXmlDocument xmldoc;
	xmldoc.LoadFile( (mworkDir+xmlFile).GetString());
	TiXmlElement* pModelElm = xmldoc.FirstChildElement(STR_MODELNODE);
	if(pModelElm)
	{		
		int nVersion;
		pModelElm->Attribute(STR_VERSION,&nVersion);
		TiXmlElement* pNodeElm = pModelElm->FirstChildElement(STR_NODE);
		_doSceneNode(pParent,pNodeElm);
	}

}