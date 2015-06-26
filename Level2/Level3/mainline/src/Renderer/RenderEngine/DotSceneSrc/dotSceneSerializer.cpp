/***************************************************************************************************/
#include "stdafx.h"
/***************************************************************************************************/
#include "dotSceneSerializer.h"
#include "dotSceneUtils.h"
/***************************************************************************************************/
namespace Ogre
{
	/***************************************************************************************************/
	template<> dsi::dotSceneSerializer *Singleton<dsi::dotSceneSerializer>::msSingleton = 0;
	/***************************************************************************************************/
	namespace dsi
	{
		/***************************************************************************************************/
		namespace staticWorkArounds
		{
			static std::vector<TiXmlElement *> _emptyNodes;
		} // namespace staticWorkArounds
		using namespace staticWorkArounds;
		/***************************************************************************************************/
		dotSceneSerializer::dotSceneSerializer()
		{
			mpSceneManager = NULL;
		}
		/***************************************************************************************************/
		dotSceneSerializer::~dotSceneSerializer()
		{
		}
		/***************************************************************************************************/
		void dotSceneSerializer::__removeEmptySceneNodes(TiXmlElement *pParentNode)
		{
			assert(pParentNode);
			String parentValue = String(pParentNode->Value());
			String isTarget = pParentNode->Attribute("isTarget") ? pParentNode->Attribute("isTarget") : "false";
			bool bEmpty = true;
			//
			TiXmlElement *pNode = pParentNode->FirstChildElement();
			for	(; pNode != 0; pNode = pNode->NextSiblingElement())
			{
				String val = String(pNode->Value());
				// use recursion to clear childs first
				if (val == "node")
					this->__removeEmptySceneNodes(pNode);
				// check for std. types						
				if (val != "position" &&
					val != "rotation" &&
					val != "scale")
				{
					bEmpty = false;
				}
			}
			// delete if empty
			if (bEmpty && isTarget != "true" && parentValue == "node")
				_emptyNodes.push_back(pParentNode);
		}
		/***************************************************************************************************/
		void dotSceneSerializer::_removeEmptySceneNodes(TiXmlDocument *pXMLDoc)
		{
			this->updateProgressListeners("removing empty nodes from XML document");
			_emptyNodes.clear();
			this->__removeEmptySceneNodes(pXMLDoc->RootElement()->FirstChildElement("nodes"));
			// remove all empty nodes
			std::vector<TiXmlElement *>::iterator it = _emptyNodes.begin();
			for(; it != _emptyNodes.end(); ++it)
				(*it)->Parent()->RemoveChild((*it));
			_emptyNodes.clear();
		}
		/***************************************************************************************************/
		TiXmlDocument *dotSceneSerializer::parseScene(SceneManager *pSceneManager, bool doStatic)
		{
			assert(pSceneManager); // must have this !! :(
			mpSceneManager = pSceneManager;
			this->updateProgressListeners("parsing current scene");
			// start new scene parsing
			mID = 0;
			TiXmlDocument *pXMLDoc = new TiXmlDocument();
			pXMLDoc->InsertEndChild(TiXmlElement("scene"));
			// export basic info
			TiXmlElement *pRoot = pXMLDoc->RootElement();
			pRoot->SetAttribute("id", "0");
			pRoot->SetAttribute("formatVersion", "0.3.0");
			pRoot->SetAttribute("sceneManager", "any");
			pRoot->SetAttribute("minOgreVersion", "1.4.0");
			pRoot->SetAttribute("author", "Ogre::dsi::dotSceneSerializer by cTh and xlarrode");
			// do environment
			this->_doEnvironment(pRoot);
			// now do nodes
			this->_doNodes(pRoot, doStatic);
			// check if there are empty nodes, it could happen if we r not doing static objects only
			if (!doStatic)
				this->_removeEmptySceneNodes(pXMLDoc);
			// return the XMLDoc
			return pXMLDoc;
		}
		/***************************************************************************************************/
		void dotSceneSerializer::saveScene(const Ogre::String &fileName, SceneManager *pSceneManager, 
        bool doStatic, bool doEnvironment)
		{
			assert(pSceneManager); // must have this !! :(
			mpSceneManager = pSceneManager;
			this->updateProgressListeners("saving scene '" + fileName + "'");
			// start new scene parsing
			mID = 0;
			TiXmlDocument *pXMLDoc = new TiXmlDocument();
			pXMLDoc->InsertEndChild(TiXmlElement("scene"));
			// export basic info
			TiXmlElement *pRoot = pXMLDoc->RootElement();
			pRoot->SetAttribute("id", "0");
			pRoot->SetAttribute("formatVersion", "0.30");
			pRoot->SetAttribute("sceneManager", "any");
			pRoot->SetAttribute("minOgreVersion", "1.4.0");
			pRoot->SetAttribute("author", "Ogre::dsi::dotSceneSerializer by cTh and X.Larrode");
      // do environment
      if(doEnvironment)	
        this->_doEnvironment(pRoot);
      // now do nodes
			this->_doNodes(pRoot, doStatic);
			// check if there are empty nodes, it could happen if we r not doing static objects only
			if (!doStatic)
				this->_removeEmptySceneNodes(pXMLDoc);
			// save the file and be happy
			if (pXMLDoc->SaveFile(fileName))
			{
				delete pXMLDoc;
				// that's it for now :)
				this->updateProgressListeners("scene '" + fileName + "' saved");
			}
			else
			{
				this->updateProgressListeners("scene '" + fileName + "' screwed something");
				OGRE_EXCEPT(Exception::ERR_INTERNAL_ERROR, "An error occured while trying to save the scene", "dotSceneSerializer::saveScene");
			}
		}
		/***************************************************************************************************/
		void dotSceneSerializer::_doEnvironment(TiXmlElement *pParent)
		{
			this->updateProgressListeners("serializing environment");
			TiXmlElement *pNode = pParent->InsertEndChild(TiXmlElement("environment"))->ToElement();
		//Update CurrentAmbient and Back

     dotSceneInfo::_setAmbientLightColor( mpSceneManager->getAmbientLight());
    //  dotSceneInfo::_setBackgroundColor( 
    //      mpWin->getViewport(0)->getBackgroundColour());

      // TODO : add colour_background to DTD
			xml::writeNamedValue(pNode, "colourAmbient",	dotSceneInfo::getAmbientLightColor());
		  //Need to get a renderwindow to do that so...
      //xml::writeNamedValue(pNode, "colourBackground",	dotSceneInfo::getBackgroundColor());
			if (dotSceneInfo::getSkyBox())
				this->_doSkyBox(pNode, dotSceneInfo::getSkyBox());
			if (dotSceneInfo::getSkyPlane())
				this->_doSkyPlane(pNode, dotSceneInfo::getSkyPlane());
			if (dotSceneInfo::getSkyDome())
				this->_doSkyDome(pNode, dotSceneInfo::getSkyDome());
			if (dotSceneInfo::hasFog())
				this->_doFog(pNode);
		}
		/***************************************************************************************************/
		void dotSceneSerializer::_doFog(TiXmlElement *pParent)
		{
			this->updateProgressListeners("serializing fog");
			TiXmlElement *pNode = pParent->InsertEndChild(TiXmlElement("fog"))->ToElement();
			// do color
			if (dotSceneInfo::hasFog())
			{
				types::sceneFog *pFog = dotSceneInfo::getFog();
				xml::writeNamedValue(pNode, "colourDiffuse", pFog->color);
				pNode->SetAttribute("expDensity", StringConverter::toString(pFog->expDensity));
				pNode->SetAttribute("linearStart", StringConverter::toString(pFog->linearStart));
				pNode->SetAttribute("linearEnd", StringConverter::toString(pFog->linearEnd));
				// do mode
				String mode = "none";
				switch (pFog->mode)
				{
				case Ogre::FOG_EXP:
					mode = "exp";
					break;
				case Ogre::FOG_EXP2:
					mode = "exp2";
					break;
				case Ogre::FOG_LINEAR:
					mode = "linear";
					break;
				case Ogre::FOG_NONE:
					mode = "none";
					break;
				}
				pNode->SetAttribute("mode", mode);
			}
		}
		/***************************************************************************************************/
		void dotSceneSerializer::_doEntity(TiXmlElement *pParent, Entity *pEnt, bool doStatic)
		{
			bool isStatic = dotSceneInfo::isMovableStatic(pEnt->getName());
			if (isStatic && !doStatic)
				return;

			this->updateProgressListeners("serializing entity '" + pEnt->getName() + "'");
			TiXmlElement *pNode = pParent->InsertEndChild(TiXmlElement("entity"))->ToElement();
			pNode->SetAttribute("name",getNameWithoutDelimiterOMK( pEnt->getName()));
			pNode->SetAttribute("id", StringConverter::toString(mID++));
			pNode->SetAttribute("meshFile", utils::getFileTitle(pEnt->getMesh()->getName()));
			if (dotSceneInfo::isMovableStatic(pEnt->getName()))
				pNode->SetAttribute("static", "true");
			else
				pNode->SetAttribute("static", "false");
			// TODO : pNode->SetAttribute("materialFile", XXX);
			// TODO : serialize vertexBuffer element
			// TODO : serialize indexBuffer element
		}
		/***************************************************************************************************/
		void dotSceneSerializer::_doLight(TiXmlElement *pParent, Light *pLight)
		{
			this->updateProgressListeners("serializing light '" + pLight->getName() + "'");
			TiXmlElement *pNode = pParent->InsertEndChild(TiXmlElement("light"))->ToElement();
			pNode->SetAttribute("name", 
          getNameWithoutDelimiterOMK ( pLight->getName() ));
			pNode->SetAttribute("id", StringConverter::toString(mID++));
			String type = "";
			switch (pLight->getType())
			{
			case Ogre::Light::LT_DIRECTIONAL:
				type = "directional";
				break;
			case Ogre::Light::LT_POINT:
				type = "point";
				break;
			case Ogre::Light::LT_SPOTLIGHT:
				type = "spot";
				break;
			}
			pNode->SetAttribute("type", type);
			pNode->SetAttribute("visible", pLight->isVisible() ? "true" : "false");
			// TODO : clarify light normal/pos, relative/absolute
		//	if (!pLight->getParentNode())
			//{
				xml::writeNamedValue(pNode, "position", pLight->getPosition());
				xml::writeNamedValue(pNode, "normal", pLight->getDirection());
		//	}
			// colors
			xml::writeNamedValue(pNode, "colourDiffuse", pLight->getDiffuseColour());
			xml::writeNamedValue(pNode, "colourSpecular", pLight->getSpecularColour());
			// attributes
			TiXmlElement *pATT = pNode->InsertEndChild(TiXmlElement("lightAttenuation"))->ToElement();
			pATT->SetAttribute("range", StringConverter::toString(pLight->getAttenuationRange()));
			pATT->SetAttribute("constant", StringConverter::toString(pLight->getAttenuationConstant()));
			pATT->SetAttribute("linear", StringConverter::toString(pLight->getAttenuationLinear()));
			pATT->SetAttribute("quadratic", StringConverter::toString(pLight->getAttenuationQuadric()));
			// do range if necesary
			if (pLight->getType() == Ogre::Light::LT_SPOTLIGHT)
			{
				TiXmlElement *pRNG = pNode->InsertEndChild(TiXmlElement("lightRange"))->ToElement();
				pRNG->SetAttribute("inner", StringConverter::toString(pLight->getSpotlightInnerAngle()));
				pRNG->SetAttribute("outer", StringConverter::toString(pLight->getSpotlightOuterAngle()));
				pRNG->SetAttribute("falloff", StringConverter::toString(pLight->getSpotlightFalloff()));
			}
		}
		/***************************************************************************************************/
		void dotSceneSerializer::_doCamera(TiXmlElement *pParent, Camera *pCam)
		{
			this->updateProgressListeners("serializing camera '" + pCam->getName() + "'");
			TiXmlElement *pNode = pParent->InsertEndChild(TiXmlElement("camera"))->ToElement();
			pNode->SetAttribute("name", 
          getNameWithoutDelimiterOMK (pCam->getName() ));
			pNode->SetAttribute("id", StringConverter::toString(mID++));
			// TODO : hehe...must see what attr. the camera will have at the end
		  Ogre::Vector3 look = pCam->getPosition();
      look.x += pCam->getDirection().x;
      look.y += pCam->getDirection().y;
      look.z += pCam->getDirection().z;

      if (!pCam->getParentNode())
			{
				xml::writeNamedValue(pNode, "position", pCam->getPosition());
				xml::writeNamedValue(pNode, "normal_disable", pCam->getOrientation());
				xml::writeNamedValue(pNode, "lookAt", look);
			}
			this->_doClipping(pNode, pCam->getFarClipDistance(), pCam->getNearClipDistance());
			if (pCam->getProjectionType() == Ogre::PT_PERSPECTIVE)
				pNode->SetAttribute("projectionType", "perspective");
			else
				pNode->SetAttribute("projectionType", "orthographic");
			pNode->SetAttribute("fov", StringConverter::toString(pCam->getFOVy()));
			pNode->SetAttribute("aspectRatio", StringConverter::toString(pCam->getAspectRatio()));
			// do autoTrack target if needed 
			// (lookAt can be only loaded, no way to serialize it, right ??)
			if (pCam->getAutoTrackTarget())
			{
				TiXmlElement *pTT = pNode->InsertEndChild(TiXmlElement("trackTarget"))->ToElement();
				pTT->SetAttribute("nodeName", pCam->getAutoTrackTarget()->getName());
				xml::writeNamedValue(pTT, "offset", pCam->getAutoTrackOffset());
			}
		}
		/***************************************************************************************************/
		void dotSceneSerializer::_doBillboardSet(TiXmlElement *pParent, BillboardSet *pBS)
		{
			this->updateProgressListeners("serializing billboardSet '" + pBS->getName() + "'");
			TiXmlElement *pNode = pParent->InsertEndChild(TiXmlElement("billboardSet"))->ToElement();
			pNode->SetAttribute("name",getNameWithoutDelimiterOMK( pBS->getName()));
			pNode->SetAttribute("id", StringConverter::toString(mID++));
			// type :(
			String type = "point";
			switch (pBS->getBillboardType())
			{
			case Ogre::BBT_POINT:
				type = "point";
				break;
			case Ogre::BBT_ORIENTED_COMMON:
				type = "orientedCommon";
				break;
			case Ogre::BBT_ORIENTED_SELF:
				type = "orientedSelf";
				break;
			}
			pNode->SetAttribute("type", type);
			// origin :(
			String origin = "center";
			switch (pBS->getBillboardOrigin())
			{
			case Ogre::BBO_TOP_LEFT:
				origin = "topLeft";
				break;
			case Ogre::BBO_TOP_CENTER:
				origin = "topCenter";
				break;
			case Ogre::BBO_TOP_RIGHT:
				origin = "topRight";
				break;
			case Ogre::BBO_CENTER_LEFT:
				origin = "centerLeft";
				break;
			case Ogre::BBO_CENTER:
				origin = "center";
				break;
			case Ogre::BBO_CENTER_RIGHT:
				origin = "centerRight";
				break;
			case Ogre::BBO_BOTTOM_LEFT:
				origin = "bottomLeft";
				break;
			case Ogre::BBO_BOTTOM_CENTER:
				origin = "bottomCenter";
				break;
			case Ogre::BBO_BOTTOM_RIGHT:
				origin = "bottomRight";
				break;
			}
			pNode->SetAttribute("origin", origin);
			// dimensions
			pNode->SetAttribute("width", StringConverter::toString(pBS->getDefaultWidth()));
			pNode->SetAttribute("height", StringConverter::toString(pBS->getDefaultHeight()));
			// material
			pNode->SetAttribute("material", pBS->getMaterialName());
			// do all billboards
			for (int n = 0; n < pBS->getNumBillboards(); ++n)
				this->_doBillboard(pNode, pBS->getBillboard(n));
		}
		/***************************************************************************************************/
		void dotSceneSerializer::_doBillboard(TiXmlElement *pParent, Billboard *pBill)
		{
			this->updateProgressListeners("serializing billboard");
			TiXmlElement *pNode = pParent->InsertEndChild(TiXmlElement("billboard"))->ToElement();
			pNode->SetAttribute("id", StringConverter::toString(mID++));
			//
			xml::writeNamedValue(pNode, "position", pBill->getPosition());
			xml::writeNamedValue(pNode, "rotation", Quaternion(pBill->getRotation().valueRadians(), 0, 0, 1));// TO CHECK
			xml::writeNamedValue(pNode, "colourDiffuse", pBill->getColour());
			if (pBill->hasOwnDimensions())
			{
				pNode->SetAttribute("width", StringConverter::toString(pBill->getOwnWidth()));
				pNode->SetAttribute("height", StringConverter::toString(pBill->getOwnHeight()));
			}
		}
		/***************************************************************************************************/
		void dotSceneSerializer::_doSkyBox(TiXmlElement *pParent, types::skyBox *pSB)
		{
			this->updateProgressListeners("serializing the skyBox");
			if (!pSB->used)
				return;
			TiXmlElement *pNode = pParent->InsertEndChild(TiXmlElement("skyBox"))->ToElement();
			pNode->SetAttribute("material", pSB->material);
			pNode->SetAttribute("distance", StringConverter::toString(pSB->distance));
			pNode->SetAttribute("drawFirst", pSB->drawFirst ? "true" : "false");
			xml::writeNamedValue(pNode, "rotation", pSB->orientation);
		}
		/***************************************************************************************************/
		void dotSceneSerializer::_doSkyDome(TiXmlElement *pParent, types::skyDome *pSD)
		{
			this->updateProgressListeners("serializing the skyDome");
			if (!pSD->used)
				return;
			TiXmlElement *pNode = pParent->InsertEndChild(TiXmlElement("skyDome"))->ToElement();
			pNode->SetAttribute("material", pSD->material);
			pNode->SetAttribute("drawFirst", pSD->drawFirst ? "true" : "false");
			pNode->SetAttribute("curvature", StringConverter::toString(pSD->curvature));
			pNode->SetAttribute("tiling", StringConverter::toString(pSD->tiling));
			pNode->SetAttribute("distance", StringConverter::toString(pSD->distance));
			xml::writeNamedValue(pNode, "rotation", pSD->orientation);
		}
		/***************************************************************************************************/
		void dotSceneSerializer::_doSkyPlane(TiXmlElement *pParent, types::skyPlane *pSP)
		{
			this->updateProgressListeners("serializing the skyPlane");
			if (!pSP->used)
				return;
			TiXmlElement *pNode = pParent->InsertEndChild(TiXmlElement("skyPlane"))->ToElement();
			pNode->SetAttribute("material", pSP->material);
			pNode->SetAttribute("drawFirst", pSP->drawFirst ? "true" : "false");
			pNode->SetAttribute("planeX", StringConverter::toString(pSP->planeX));
			pNode->SetAttribute("planeY", StringConverter::toString(pSP->planeY));
			pNode->SetAttribute("planeZ", StringConverter::toString(pSP->planeZ));
			pNode->SetAttribute("planeD", StringConverter::toString(pSP->planeD));
			pNode->SetAttribute("scale", StringConverter::toString(pSP->scale));
			pNode->SetAttribute("tiling", StringConverter::toString(pSP->tiling));
			pNode->SetAttribute("bow", StringConverter::toString(pSP->bow));
		}
		/***************************************************************************************************/
		void dotSceneSerializer::_doClipping(TiXmlElement *pParent, Real fcd, Real ncd)
		{
			this->updateProgressListeners("serializing clipping parameters");
			TiXmlElement *pNode = pParent->InsertEndChild(TiXmlElement("clipping"))->ToElement();
			pNode->SetAttribute("near", StringConverter::toString(ncd));
			pNode->SetAttribute("far", StringConverter::toString(fcd));
		}
		/***************************************************************************************************/
		void dotSceneSerializer::_doNodes(TiXmlElement *pParent, bool doStatic)
		{
			this->updateProgressListeners("serializing nodes");
			TiXmlElement *pNode = pParent->InsertEndChild(TiXmlElement("nodes"))->ToElement();
			// do root, this will recurse all nodes
			//this->_doNode(pNode, mpSceneManager->getRootSceneNode(), doStatic);
		// lights
			SceneManager::MovableObjectIterator lit = mpSceneManager->getMovableObjectIterator("Light");
			Light *pLT = NULL;
			while (lit.hasMoreElements())
			{
				pLT = (Light*)lit.getNext();
				if	(
						(! pLT->isAttached()) && 
						(pLT->getName().find(nonDSIPrefix) == String::npos)
					)
					this->_doLight(pNode, pLT);
			}
			// cameras
			SceneManager::CameraIterator cit = mpSceneManager->getCameraIterator();
			Camera *pCam = NULL;
			while (cit.hasMoreElements())
			{
				pCam = cit.getNext();
				if	(
						(! pCam->isAttached()) && 
						(pCam->getName().find(nonDSIPrefix) == String::npos)
					)
					this->_doCamera(pNode, pCam);
			}

      // Nodes
      SceneNode *pSN = mpSceneManager->getRootSceneNode();
			Ogre::Node::ChildNodeIterator it = pSN->getChildIterator();
			SceneNode *pCSN = NULL;
			while (it.hasMoreElements())
			{
				pCSN = (SceneNode *)&(*it.getNext());
				if	(pCSN->getName().find(nonDSIPrefix) == String::npos)
					this->_doNode(pNode, pCSN, doStatic);
			}
		}
		/***************************************************************************************************/
		void dotSceneSerializer::_doNode(TiXmlElement *pParent, Ogre::SceneNode *pSN, bool doStatic)
		{
			this->updateProgressListeners("serializing node '" + pSN->getName() + "'");
			TiXmlElement *pNode = pParent->InsertEndChild(TiXmlElement("node"))->ToElement();
			//
			pNode->SetAttribute("name",getNameWithoutDelimiterOMK( pSN->getName()));
			pNode->SetAttribute("id", StringConverter::toString(mID++));
			// write type
			if (dotSceneInfo::isTarget(pSN->getName()))
				pNode->SetAttribute("isTarget", "true");
			xml::writeNamedValue(pNode, "position", pSN->getPosition());
			xml::writeNamedValue(pNode, "rotation", pSN->getOrientation());
			xml::writeNamedValue(pNode, "scale", pSN->getScale());
			// do autoTrack target if needed 
			// (lookAt can be only loaded, no way to serialize it, right ??)
			if (pSN->getAutoTrackTarget())
			{
				TiXmlElement *pTT = pNode->InsertEndChild(TiXmlElement("trackTarget"))->ToElement();
				pTT->SetAttribute("nodeName", pSN->getAutoTrackTarget()->getName());
				xml::writeNamedValue(pTT, "localDirection", pSN->getAutoTrackLocalDirection());
				xml::writeNamedValue(pTT, "offset", pSN->getAutoTrackOffset());
			}
			// entities
			SceneManager::MovableObjectIterator eit = mpSceneManager->getMovableObjectIterator("Entity");
      Entity *pEnt = NULL;
			while (eit.hasMoreElements())
			{
				pEnt =(Entity*) eit.getNext();
				if	(
						(pEnt->getParentNode() == pSN) && 
						(pEnt->getName().find(nonDSIPrefix) == String::npos)
					)
					this->_doEntity(pNode, pEnt, doStatic);
			}
			// lights
			SceneManager::MovableObjectIterator lit = mpSceneManager->getMovableObjectIterator("Light");
			Light *pLT = NULL;
			while (lit.hasMoreElements())
			{
				pLT = (Light*)lit.getNext();
				if	(
						(pLT->getParentNode() == pSN) && 
						(pLT->getName().find(nonDSIPrefix) == String::npos)
					)
					this->_doLight(pNode, pLT);
			}
			// cameras
			SceneManager::CameraIterator cit = mpSceneManager->getCameraIterator();
			Camera *pCam = NULL;
			while (cit.hasMoreElements())
			{
				pCam = cit.getNext();
				if	(
						(pCam->getParentNode() == pSN) && 
						(pCam->getName().find(nonDSIPrefix) == String::npos)
					)
					this->_doCamera(pNode, pCam);
			}
			// billboard sets
			SceneManager::MovableObjectIterator bit = mpSceneManager->getMovableObjectIterator("BillboardSet");
			BillboardSet *pBS = NULL;
			while (bit.hasMoreElements())
			{
				pBS =(BillboardSet*) bit.getNext();
				if	(
						(pBS->getParentNode() == pSN) && 
						(pBS->getName().find(nonDSIPrefix) == String::npos)
					)
					this->_doBillboardSet(pNode, pBS);
			}
			// do child nodes
			Ogre::Node::ChildNodeIterator it = pSN->getChildIterator();
			SceneNode *pCSN = NULL;
			while (it.hasMoreElements())
			{
				pCSN = (SceneNode *)&(*it.getNext());
				if (pCSN->getName().find(nonDSIPrefix) == String::npos)
					this->_doNode(pNode, pCSN, doStatic);
			}
		}
		/***************************************************************************************************/

    std::string dotSceneSerializer::getNameWithoutDelimiterOMK(const std::string & fileName)
    {
      static const std::string s_delimiter = "_#_";
      size_t delimiterPlace =fileName.rfind(s_delimiter);
      std::string s = fileName;
      if ( delimiterPlace != std::string::npos)
        s.erase(delimiterPlace,fileName.size());
      return s;
    }
	} // namespace dse
	/***************************************************************************************************/
}// namespace Ogre
/*******************************************************************************************************************/
