/***************************************************************************************************/
#include "stdafx.h"
/***************************************************************************************************/
#include "dotSceneLoaderImpl-0.2.0.h"
#include "dotSceneUtils.h"
#include "OgreShadowCameraSetup.h"
#include "OgreShadowCameraSetupFocused.h"
#include "OgreShadowCameraSetupLiSPSM.h"
#include "OgreAnimation.h"
#include "OgreParticleSystem.h"
#include "dotSceneLoader.h"
/***************************************************************************************************/
/// define those just in case
#define  PREFIX_NODE      //"dsiNode"
#define  PREFIX_ENT      //"dsiEntity"
#define  PREFIX_LIGHT    //"dsiLight"
#define  PREFIX_CAMERA    //"dsiCamera"
#define  PREFIX_BILLBOARDSET  //"dsiBillboardSet"
#define  PREFIX_BILLBOARD  //"dsiBillboard"
using Ogre::String;
/***************************************************************************************************/
namespace Ogre
{
/***************************************************************************************************/
namespace dsi
{
/***************************************************************************************************/
namespace staticWorkArounds
{
  static Vector3    _dslCurrPos = Vector3::ZERO;
  static Vector3    _dslCurrSca = Vector3::UNIT_SCALE;
  static Quaternion  _dslCurrRot = Quaternion::IDENTITY;

  static std::map<TiXmlElement*, Camera*> _dslCamTTargets;
  static std::map<TiXmlElement*, Camera*> _dslCamLTargets;

  static std::map<TiXmlElement*, SceneNode*> _dslSNTTargets;
  static std::map<TiXmlElement*, SceneNode*> _dslSNLTargets;
} // namespace staticWorkArounds
using namespace staticWorkArounds;
/***************************************************************************************************/
dotSceneLoaderImpl020::dotSceneLoaderImpl020()
{
  mpSceneManager = NULL;
  mpWin = NULL;
  mpRoot = NULL;
  mDoMats = false;
  mShadow = false;
  mDoInfo = false;
}
/***************************************************************************************************/
dotSceneLoaderImpl020::~dotSceneLoaderImpl020()
{
  // must call this to clear the internal scene graph
  dotSceneInfo::_reset();
}
/***************************************************************************************************/
void dotSceneLoaderImpl020::load(
  TiXmlDocument *pXMLDoc, 
  SceneManager *pSceneManager, 
  RenderWindow *pRWin, 
  const String &fileName,
  SceneNode *pRootNode, 
  bool doMaterials, 
  bool forceShadowBuffers, 
  bool updateDSInfo)
{
  assert(pXMLDoc);
  assert(pSceneManager);
  mpSceneManager = pSceneManager; // this is a must :(
  //assert(pRWin); //there's no reason to force caller to pass a pointer to a RenderWindow
  mpWin = pRWin;
  // do the root node
  if (pRootNode)
    mpRoot = pRootNode;
  else
  {
    mpRoot = mpSceneManager->getRootSceneNode();
    if (!mpRoot)
      OGRE_EXCEPT(Exception::ERR_INTERNAL_ERROR, 
      "Something went wrong while creating the child of the SceneRoot node?\nWEIRD", "dotSceneLoaderImpl020::load");
  }
  mDoMats = doMaterials;
  mShadow = forceShadowBuffers;
  mDoInfo = true;
  //mDoInfo = updateDSInfo;
  mGroupName = fileName;
  if (!this->mDoInfo)
    dotSceneInfo::_setUpToDate(false);
  // reset the dotSceneInfo graph
  dotSceneInfo::_reset();
  // those are they that we all live in a yelow submarine :)
  TiXmlElement *pElem = NULL;
  TiXmlElement *pRootElem = pXMLDoc->RootElement();
  // check version first, fail if it's not what we want
  String ver = StringUtil::BLANK;
  if (!this->_getAttribute(pRootElem, "formatVersion", ver))
    ;//dotSceneInfo::_logLoadWarning(".scene file does not have a versionFormat attribute, will default to 0.2.0...");
  else
  {
    if (ver != "0.2.0")
      ;//dotSceneInfo::_logLoadWarning(".scene file versionFormat attribute is not 0.2.0, will try to parse anyway...");
  }
  this->updateProgressListeners("parsing nodes");
  pElem = pRootElem->FirstChildElement("nodes");
  if (pElem)
    this->_doNodes(pElem);
  // now do queued targets
  // camera trackTargets
  for (std::map<TiXmlElement*, Camera*>::iterator it = _dslCamTTargets.begin(); it != _dslCamTTargets.end(); ++it)
    this->__doCamTTarget(it->first, it->second);
  _dslCamTTargets.clear();
  // camera lookTargets
  for (std::map<TiXmlElement*, Camera*>::iterator it = _dslCamLTargets.begin(); it != _dslCamLTargets.end(); ++it)
    this->__doCamLTarget(it->first, it->second);
  _dslCamLTargets.clear();
  // node trackTargets
  for (std::map<TiXmlElement*, SceneNode*>::iterator it = _dslSNTTargets.begin(); it != _dslSNTTargets.end(); ++it)
    this->__doSNTTarget(it->first, it->second);
  _dslSNTTargets.clear();
  // node lookTargets
  for (std::map<TiXmlElement*, SceneNode*>::iterator it = _dslSNLTargets.begin(); it != _dslSNLTargets.end(); ++it)
    this->__doSNLTarget(it->first, it->second);
  _dslSNLTargets.clear();
  // environment
  this->updateProgressListeners("parsing environment");
  pElem = pRootElem->FirstChildElement("environment");
  if (pElem)
    this->_doEnvironment(pElem);
  // externals
  this->updateProgressListeners("parsing externals");
  pElem = pRootElem->FirstChildElement("externals");
  if (pElem)
    this->_doExternals(pElem);
  // bla bla
  this->updateProgressListeners("done loading");
}
/***************************************************************************************************/
void dotSceneLoaderImpl020::_doAnimations(SceneNode *pParent, TiXmlElement *pNode)
{
  this->updateProgressListeners("parsing animations");
  for(  TiXmlElement *pNodeElem = pNode->FirstChildElement(); 
    pNodeElem != 0; 
    pNodeElem = pNodeElem->NextSiblingElement())
  {
    String value = String(pNodeElem->Value());
    if (value == "animation")
    {
      this->_doAnimation(pParent, pNodeElem);
    }
    else
      dotSceneInfo::_logLoadWarning(
      "Unknown XMLNode of type '" + 
      String(pNodeElem->Value()) + 
      "' encountered while reading 'nodes', skipping this node.");
  }
}
/***************************************************************************************************/
void dotSceneLoaderImpl020::_doNodes(TiXmlElement *pNode)
{
  for (  TiXmlElement *pNodeElem = pNode->FirstChildElement(); 
    pNodeElem != 0;
    pNodeElem = pNodeElem->NextSiblingElement())
  {
    String value = String(pNodeElem->Value());
    if (value == "position")
    {
      mpRoot->setPosition(xml::readPosition(pNodeElem));
      mpRoot->setInitialState();
    }
    else if (value == "rotation")
    {
      mpRoot->setOrientation(xml::readOrientation(pNodeElem));
      mpRoot->setInitialState();
    }
    else if (value == "scale")
    {
      mpRoot->setScale(xml::readScale(pNodeElem));
      mpRoot->setInitialState();
    }
    else if (value == "node")
      this->_doNode(mpRoot, pNodeElem);
    else if (value == "animations")
      this->_doAnimations(mpRoot, pNodeElem);
    else if (value == "light")
      this->_doLight(mpRoot, pNodeElem);
    else if (value == "camera")
      this->_doCamera(mpRoot, pNodeElem);
    else if (value == "viewport")
      this->_doViewport(mpRoot, pNodeElem);
	else if (value == "scene")
	  this->_doScene(mpRoot, pNodeElem);
    else
      dotSceneInfo::_logLoadWarning(
      "Unknown XMLNode of type '" +
      String(pNodeElem->Value()) +
      "' encountered while reading 'nodes', skipping this node.");
  }
}
/***************************************************************************************************/
void dotSceneLoaderImpl020::_doNode(SceneNode *pParent, TiXmlElement *pNode)
{
  String name = this->_getNodeName(pNode);
  // don't know why ? xa
  name = String(PREFIX_NODE) + name;
  name = setName(name, "sceneNode", mpSceneManager);

  // create the node
  SceneNode *pSceneNode = pParent->createChildSceneNode(name);
  // parse type, put in the scene graph if a target is found
  bool bTarget = false;
  if (pNode->Attribute("isTarget"))
    if (String(pNode->Attribute("isTarget")) == String("true"))
      bTarget = true;
  if (bTarget && this->mDoInfo)
    dotSceneInfo::_addTargetSceneNode(pSceneNode);
  //
  this->updateProgressListeners("parsing node '" + name + "'");
  // do node PRS values first
  for (  TiXmlElement *pElem = pNode->FirstChildElement(); 
    pElem != 0;
    pElem = pElem->NextSiblingElement())
  {
    /// must INIT those here, ugly hack shit !!! :(
    _dslCurrPos = Vector3::ZERO;
    _dslCurrSca = Vector3::UNIT_SCALE;
    _dslCurrRot = Quaternion::IDENTITY;
    ///
    String value = String(pElem->Value());
    if (value == "position")
    {
      pSceneNode->setPosition(xml::readPosition(pElem));
      _dslCurrPos = pSceneNode->_getDerivedPosition();
    }
    else if (value == "scale")
    {
      pSceneNode->setScale(xml::readScale(pElem));
      _dslCurrSca = pSceneNode->_getDerivedScale();
    }
    else if (value == "rotation")
    {
      pSceneNode->setOrientation(xml::readOrientation(pElem));
      _dslCurrRot = pSceneNode->_getDerivedOrientation();
    }
    else if (value == "lookTarget")
      this->_doSNLTarget(pElem, pSceneNode);
    else if (value == "trackTarget")
      this->_doSNTTarget(pElem, pSceneNode);
  }
  // do movable objects now
  for (  TiXmlElement *pElem = pNode->FirstChildElement(); 
    pElem != 0; pElem = pElem->NextSiblingElement())
  {
    String value = String(pElem->Value());
    if (value == "entity")
      this->_doEntity(pSceneNode, pElem);
    else if (value == "light")
      this->_doLight(pSceneNode, pElem);
    else if (value == "animations")
      this->_doAnimations(pSceneNode, pElem);
    else if (value == "camera")
      this->_doCamera(pSceneNode, pElem);
    else if (value == "billboardSet")
      this->_doBillboardSet(pSceneNode, pElem);
    else if (value == "particleSystem")
      this->_doParticleSystem(pSceneNode, pElem);
    else if (value == "plane")
      this->_doPlane(pSceneNode, pElem);
	else if (value == "scene")
		this->_doScene(pSceneNode, pElem);
  }
  // do child nodes as last !!!
  for (  TiXmlElement *pElem = pNode->FirstChildElement(); 
    pElem != 0; pElem = pElem->NextSiblingElement())
  {
    String value = String(pElem->Value());
    // read child nodes if any
    if (value == "node")
      this->_doNode(pSceneNode, pElem);
  }
}
/***************************************************************************************************/
void dotSceneLoaderImpl020::_doAnimation(SceneNode *pParent, TiXmlElement *pNode)
{

  String name = this->_getNodeName(pNode);
 // name = setName(name, "animation", mpSceneManager);
  //    std::cout << "[DotScene] Found Animation " << name 
  //    << " on SceneNode " << pParent->getName() << std::endl;
  bool enable = false;
  if (pNode->Attribute("enable"))
    if (String(pNode->Attribute("enable")) == String("true"))
      enable = true;
  bool loop = false;
  if (pNode->Attribute("loop"))
    if (String(pNode->Attribute("loop")) == String("true"))
      loop = true;

  String interpolationMode ="linear";
  if (pNode->Attribute("interpolationMode"))
    interpolationMode = String(pNode->Attribute("interpolationMode"));
  String rotationInterpolationMode ="linear";
  if (pNode->Attribute("rotationInterpolationMode"))
    rotationInterpolationMode = String(pNode->Attribute("rotationInterpolationMode"));
  // read the length attribute
  float length(0.0f) ;
  if (pNode->Attribute("length"))
  {
    String lengthString = pNode->Attribute("length") ;
    std::stringstream stream ;
    stream << lengthString ;
    stream >> length ;
  }

  Animation* anim;
  if ( mpSceneManager->hasAnimation(name))
  {
    anim = mpSceneManager->getAnimation(name);
  }
  else
    anim = mpSceneManager->createAnimation(name, length);
  
  if (interpolationMode == "spline")
    anim->setInterpolationMode(Ogre::Animation::IM_SPLINE);
  else
    anim->setInterpolationMode(Ogre::Animation::IM_LINEAR);

  if (rotationInterpolationMode == "spherical")
    anim->setRotationInterpolationMode(Ogre::Animation::RIM_SPHERICAL);
  else
    anim->setRotationInterpolationMode(Ogre::Animation::RIM_LINEAR);

  NodeAnimationTrack* track = anim->createNodeTrack(anim->getNumNodeTracks(),pParent);
  // do Keyframe now
  for (  TiXmlElement *pElem = pNode->FirstChildElement(); 
    pElem != 0; pElem = pElem->NextSiblingElement())
  {
    String value = String(pElem->Value());
    if (value == "keyframe")
      this->_doKeyframe(track, pElem);
    else
      dotSceneInfo::_logLoadWarning(
      "Unknown XMLNode of type '" + 
      String(pElem->Value()) + 
      "' encountered while reading 'animation', skipping this node.");
  }
  Ogre::AnimationState* animationState;
  if ( mpSceneManager->hasAnimationState (name))
  {
    animationState = mpSceneManager->getAnimationState(name);
  }
  else 
    animationState = mpSceneManager->createAnimationState(name);

  animationState->setEnabled(enable);
  animationState->setLoop(loop);
  /*std::cout << "[DotScene] DEBUG : Animation " << name 
  << " enable " << enable
  << " loop " << loop
  << " length " << length
  << std::endl;*/
}
/********************************************************************************************/
void dotSceneLoaderImpl020::_doKeyframe(NodeAnimationTrack* track, TiXmlElement *pNode)
{
  if (!pNode->Attribute("time"))
  {
    dotSceneInfo::_logLoadWarning(
      "Error on KeyFrame, need attribute Time '");
    return;
  }
  float time;
  String timeString = pNode->Attribute("time") ;
  std::stringstream stream ;
  stream << timeString ;
  stream >> time ;

  TransformKeyFrame *key = track->createNodeKeyFrame(time);
  for (  TiXmlElement *pNodeElem = pNode->FirstChildElement(); 
    pNodeElem != 0; 
    pNodeElem = pNodeElem->NextSiblingElement())
  {
    String value = String(pNodeElem->Value());
    if (value == "translation")
      key->setTranslate(xml::readPosition(pNodeElem));
    else if (value == "rotation")
      key->setRotation(xml::readOrientation(pNodeElem));
    else if (value == "scale")
      key->setScale(xml::readScale(pNodeElem));
  }
  /*std::cout << "[DotScene] DEBUG : keyframe " 
  << " time " << time
  << std::endl;*/
}
/********************************************************************************************/
void dotSceneLoaderImpl020::_doEnvironment(TiXmlElement *pNode)
{
  //TRACE_WARNING( "[DotScene] Found Environment");
  ColourValue amb = ColourValue::Black, bkg = ColourValue::Black;
  TiXmlElement *pElem = NULL;

  pElem = pNode->FirstChildElement("colourAmbient");
  if (pElem)
  {
    amb = xml::readColor(pElem);

    // std::cout << "[DotScene] AmbientLight "<< amb << std::endl;
    mpSceneManager->setAmbientLight(amb);
    if (this->mDoInfo)
      dotSceneInfo::_setAmbientLightColor(amb);
  }

  pElem = pNode->FirstChildElement("shadow");
  if (pElem)
  {
    // std::cout << "[DotScene] ShadowProperties "<< amb << std::endl;
    this->_doShadowProperties(pElem);
  }

  if ( mpWin )
  {
    pElem = pNode->FirstChildElement("colourBackground");
    if (pElem)
    {
      bkg = xml::readColor(pElem);
      // std::cout << "[DotScene] colourBackground "<< bkg << std::endl;
      for (int n = 0; n < mpWin->getNumViewports(); n++)
        mpWin->getViewport(n)->setBackgroundColour(bkg);
      if (this->mDoInfo)
        dotSceneInfo::_setBackgroundColor(bkg);
    }      
  }

  pElem = pNode->FirstChildElement("skyBox");
  if (pElem)
    this->_doSkyBox(pElem);

  pElem = pNode->FirstChildElement("skyDome");
  if (pElem)
    this->_doSkyDome(pElem);

  pElem = pNode->FirstChildElement("skyPlane");
  if (pElem)
    this->_doSkyPlane(pElem);

  pElem = pNode->FirstChildElement("fog");
  if (pElem)
    this->_doFog(pElem);

  pElem = pNode->FirstChildElement("terrain");
  if (pElem)
    this->_doTerrain(pElem);
}
/***************************************************************************************************/
dotSceneLoaderImpl020::sHWGPUBuffer *dotSceneLoaderImpl020::_getHwGpuBuffer(TiXmlElement *pNode)
{
  sHWGPUBuffer *pBuff = new sHWGPUBuffer();
			
  String usage = pNode->Attribute("usage") ? pNode->Attribute("usage") : "staticWriteOnly";
  if (usage == "dynamic")
    pBuff->usage = Ogre::HardwareBuffer::HBU_DYNAMIC;
  else if (usage == "dynamicWriteOnly")
    pBuff->usage = Ogre::HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY;
  else if (usage == "static")
    pBuff->usage = Ogre::HardwareBuffer::HBU_STATIC;
  else if (usage == "staticWriteOnly")
    pBuff->usage = Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY;
  else if (usage == "writeOnly")
    pBuff->usage = Ogre::HardwareBuffer::HBU_WRITE_ONLY;
  else
    pBuff->usage = Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY;
			
  String useShadow = pNode->Attribute("useShadow") ? pNode->Attribute("useShadow") : "true";
  if (useShadow == "true")
    pBuff->shadow = true;			
			
  return pBuff;
}
/***************************************************************************************************/
void dotSceneLoaderImpl020::_doEntity(SceneNode *pParent, TiXmlElement *pNode)
{
  String entName  = this->_getNodeName(pNode);
  this->updateProgressListeners("parsing entity '" + entName + "'");
  Entity *pEnt = NULL;
  String meshName = pNode->Attribute("meshFile") ? pNode->Attribute("meshFile") : StringUtil::BLANK;
  if (meshName != StringUtil::BLANK)
  {
    try
    {
      //se if we have some specific VB/IB requests in the .scene first
      TiXmlElement *vbElem = pNode->FirstChildElement("vertexBuffer");
      TiXmlElement *ibElem = pNode->FirstChildElement("indexBuffer");
      if (this->mShadow || ibElem || vbElem)
      {
        if (this->mShadow)
        {
          MeshManager::getSingletonPtr()->load(meshName, 
            mGroupName,
            HardwareBuffer::HBU_STATIC_WRITE_ONLY,
            HardwareBuffer::HBU_STATIC_WRITE_ONLY, 
            true, true);
        }
        else
        {
          sHWGPUBuffer *pVB = vbElem ? this->_getHwGpuBuffer(vbElem) : new sHWGPUBuffer();
          sHWGPUBuffer *pIB = ibElem ? this->_getHwGpuBuffer(ibElem) : new sHWGPUBuffer();
          MeshManager::getSingletonPtr()->load(meshName, mGroupName, pVB->usage, pIB->usage, pVB->shadow, pIB->shadow);
          delete pVB;
          delete pIB;
        }
      }
      else
        MeshManager::getSingletonPtr()->load(meshName, mGroupName);

      // setName according OMK name
      String pentName = setName(String(String(PREFIX_ENT) + entName), "entity", mpSceneManager);
      pEnt = mpSceneManager->createEntity(pentName, meshName);
      // Add buildEdgeList for Shadows!
      pEnt->getMesh()->buildEdgeList();
    }
    catch (Exception ex)
    {
      dotSceneInfo::_logLoadError("'" + meshName + "' failed to load");
      pEnt = NULL;
    }
  }
  if (pEnt)
  {
#if OGRE_VERSION_MAJOR > 1 && OGRE_VERSION_MINOR < 6
    // normalisation is automatic with Ogre 1.6
    pEnt->setNormaliseNormals(true);
#endif
    pParent->attachObject(pEnt);
    // update the dotSceneInfo graph
    bool visible = true;
    if (pNode->Attribute("visible"))
      if (String(pNode->Attribute("visible")) == String("false"))
        visible = false;
    pEnt->setVisible(visible);

    bool stat = false;
    if (pNode->Attribute("static"))
      if (String(pNode->Attribute("static")) == String("true"))
        stat = true;
    bool castShadows = true;
    if (pNode->Attribute("castShadows"))
      if (String(pNode->Attribute("castShadows")) == String("false"))
        castShadows = false;
    pEnt->setCastShadows(castShadows);
 /* //QueryFlags
    unsigned int mask;
    if (pNode->Attribute("QueryFlags"))
      if (String(pNode->Attribute("QueryFlags")) == String("false"))
       
    pEnt->setQueryFlags(mask);
*/

    this->_updateDSI(pEnt, _dslCurrPos, _dslCurrRot, _dslCurrSca, stat);
  }
  else
    dotSceneInfo::_logLoadError("entity '" + entName + "' failed to load, check the .scene");
  /// TODO : materialFile attribute handling
}
/***************************************************************************************************/
void dotSceneLoaderImpl020::_doLight(SceneNode *pParent, TiXmlElement *pNode)
{
  String lightName = this->_getNodeName(pNode);
  //std::cout << "Add Light " << lightName << std::endl;
  this->updateProgressListeners("parsing light '" + lightName + "'");
  // setName according OMK name
  String plightName = setName(String(String(PREFIX_LIGHT) + lightName), "light", mpSceneManager);
  Light *pLight = mpSceneManager->createLight(plightName);
  pParent->attachObject(pLight);
  TiXmlElement *pElem = NULL;
  // visibility
  bool visible = true;
  if (pNode->Attribute("visible"))
    if (String(pNode->Attribute("visible")) == String("false"))
      visible = false;
  pLight->setVisible(visible);
  // colours
  TiXmlElement *difElem = pNode->FirstChildElement("colourDiffuse");
  TiXmlElement *speElem = pNode->FirstChildElement("colourSpecular");
  if (difElem)
    pLight->setDiffuseColour(xml::readColor(difElem));
  if (speElem)
    pLight->setSpecularColour(xml::readColor(speElem));
  // type
  String type = pNode->Attribute("type") ? pNode->Attribute("type") : "point";
  if (type == "point")
    pLight->setType(Light::LT_POINT);
  else if (type == "directional" || type == "targetDirectional")
    pLight->setType(Light::LT_DIRECTIONAL);
  else if (type == "spot" || type == "targetSpot")
    pLight->setType(Light::LT_SPOTLIGHT);
  bool castShadows = true;
  if (pNode->Attribute("castShadows"))
    if (String(pNode->Attribute("castShadows")) == String("false"))
      castShadows = false;
  pLight->setCastShadows(castShadows);
  // attenuation
  pElem = pNode->FirstChildElement("lightAttenuation");
  if (pElem)
  {
    Real range, constant, linear, quadratic;
    this->_doLightAttenuation(pElem, range, constant, linear, quadratic);
    pLight->setAttenuation(range, constant, linear, quadratic);
  }
  // spotlight range
  pElem = pNode->FirstChildElement("lightRange");
  if (pElem)
  {
    Real inner, outer, falloff;
    this->_doLightRange(pElem, inner, outer, falloff);
    pLight->setSpotlightRange(Ogre::Angle(inner), Ogre::Angle(outer), falloff);
  }
  // direction
  TiXmlElement *nElem = pNode->FirstChildElement("normal");
  if (nElem)
    pLight->setDirection(xml::readNormal(nElem));
  // position
  TiXmlElement *posElem = pNode->FirstChildElement("position");
  if (posElem)
    pLight->setPosition(xml::readPosition(posElem));
  // update the dotSceneInfo graph
  this->_updateDSI(pLight, _dslCurrPos, _dslCurrRot, _dslCurrSca, false);
}
/***************************************************************************************************/
void dotSceneLoaderImpl020::_doClipping(TiXmlElement *pNode, Real &nearClip, Real &farClip)
{
  this->updateProgressListeners("parsing clipping");
  if ( pNode->Attribute("near"))
    nearClip = StringConverter::parseReal(pNode->Attribute("near"));
  else  if ( pNode->Attribute("nearPlaneDist"))
    nearClip = StringConverter::parseReal(pNode->Attribute("nearPlaneDist"));


  if ( pNode->Attribute("far"))
    farClip = StringConverter::parseReal(pNode->Attribute("far"));
  else  if ( pNode->Attribute("farPlaneDist"))
    farClip = StringConverter::parseReal(pNode->Attribute("farPlaneDist"));
 }
/***************************************************************************************************/
void dotSceneLoaderImpl020::_doViewport(SceneNode *pParent, TiXmlElement *pNode)
{
  if(mpWin == NULL)
    return;

  String viewportName = this->_getNodeName(pNode);
  this->updateProgressListeners("parsing viewport '" + viewportName + "'");

  String cameraName("") ;

  if (_viewportsNameToIndexMap.empty() && ( mpWin->getNumViewports() == 1 ) )
  {
    _viewportsNameToIndexMap["viewport0"] = 0 ;
  }

  Viewport* viewport( NULL ) ;
  std::map< String, unsigned short>::iterator ite( _viewportsNameToIndexMap.find( viewportName ) ) ;
  if ( ite != _viewportsNameToIndexMap.end() )
  {
    viewport = mpWin->getViewport( ite->second ) ;
  }

  // read the camera attribute
  if( pNode->Attribute( "camera" ) )
  {
    String cameraName = pNode->Attribute( "camera" ) ;
    // verify the camera name
    if ( mpSceneManager->hasCamera( cameraName ) )
    {
      Ogre::Camera* camera( mpSceneManager->getCamera( cameraName ) ) ;

      // create the viewport
      if ( viewport == NULL )
      {
        // read the zOrder attribute
        int zOrder( 0 ) ;
        if( pNode->Attribute( "zOrder" ) )
        {
          String zOrderString( pNode->Attribute( "zOrder" ) ) ;
          std::stringstream stream ;
          stream << zOrderString ;
          stream >> zOrder ;
        }

        // Create one viewport, entire window
        viewport = mpWin->addViewport( camera, zOrder ) ;
        std::stringstream stream ;
        stream << _viewportsNameToIndexMap.size() ;
        unsigned short index ;
        stream >> index ;
        _viewportsNameToIndexMap[viewportName] = index ;
      }
      else
      {
        viewport->setCamera( camera ) ;
      }
    }
    else
    {
      dotSceneInfo::_logLoadError( "The camera " + cameraName + " used as an attribute to the viewport " + viewportName + " does not exist" ) ;
    }
  }
  else if ( viewport == NULL )
  {
    dotSceneInfo::_logLoadError( "The viewport " + viewportName + " does not have an attribute camera" ) ;
  }

  TiXmlElement* pElem ( pNode->FirstChildElement( "colourBackground" ) ) ;
  if ( pElem )
  {
    // sets the viewport background colour
    viewport->setBackgroundColour( xml::readColor( pElem ) ) ;
  }  
  else
  {
    // sets the viewport background colour
    viewport->setBackgroundColour( Ogre::ColourValue( 1.0f, 1.0f, 1.0f ) ) ;
  }

  // read the left attribute
  float left( 0.0f ) ;
  if( pNode->Attribute( "left" ) )
  {
    String leftString = pNode->Attribute( "left" ) ;
    std::stringstream stream ;
    stream << leftString ;
    stream >> left ;
  }

  // read the top attribute
  float top( 0.0f ) ;
  if( pNode->Attribute( "top" ) )
  {
    String topString = pNode->Attribute( "top" ) ;
    std::stringstream stream ;
    stream << topString ;
    stream >> top ;
  }

  // read the width attribute
  float width( 1.0f ) ;
  if( pNode->Attribute( "width" ) )
  {
    String widthString = pNode->Attribute( "width" ) ;
    std::stringstream stream ;
    stream << widthString ;
    stream >> width ;
  }

  // read the height attribute
  float height( 1.0f ) ;
  if( pNode->Attribute( "height" ) )
  {
    String heightString = pNode->Attribute( "height" ) ;
    std::stringstream stream ;
    stream << heightString ;
    stream >> height ;
  }

  // set the viewport dimensions
  viewport->setDimensions( left, top, width, height ) ;

  bool overlay = true;
  if ( pNode->Attribute("overlay") )
  {
    if ( String( pNode->Attribute( "overlay" ) ) == String( "false" ) )
      overlay = false;
  }
  bool aspectRatio = true;
  if ( pNode->Attribute("aspectRatio") )
  {
    if ( String( pNode->Attribute( "aspectRatio" ) ) == String( "false" ) )
      aspectRatio = false;
  }
   viewport->setOverlaysEnabled( overlay ); 
  // Alter the camera aspect ratio to match the viewport
  if ( aspectRatio)
  {
    std::cout << " Force AspectRation on " << viewport->getCamera()->getName(); 
    viewport->getCamera()->setAspectRatio( Real( viewport->getActualWidth() ) / Real( viewport->getActualHeight() ) ) ;
  }
}
/***************************************************************************************************/
void dotSceneLoaderImpl020::_doCamera(SceneNode *pParent, TiXmlElement *pNode)
{
  String camName = this->_getNodeName(pNode);
  this->updateProgressListeners("parsing camera '" + camName + "'");
  // create it
  // setName according OMK name
  String pcameraName = setName(String(String(PREFIX_CAMERA) + camName), "camera", mpSceneManager);

  //if the cam already exists, simply change it's attributes, don't create a new one
  Camera* pCam( NULL ) ;
  if ( mpSceneManager->hasCamera(camName) )
  {
    pCam = mpSceneManager->getCamera(camName);
  }
  //try with OMK name
  else if ( mpSceneManager->hasCamera( pcameraName ) )
  {
    pCam = mpSceneManager->getCamera( pcameraName ) ;
  }
  // else create a new camera
  else
  {
    pCam = mpSceneManager->createCamera( pcameraName ) ;
  }

  if ( pCam->getParentSceneNode() != NULL )
  {
    pCam->getParentSceneNode()->detachObject( pCam ) ;
  }
  //anyway, attach the camera to the parent node
  pParent->attachObject(pCam);
  // parse camera attributes
  // do clipping
  TiXmlElement *pElem = pNode->FirstChildElement("clipping");
  Real nearCD = 0, farCD = 0;
  if (pElem)
    this->_doClipping(pElem, nearCD, farCD);
  if (nearCD != 0)
    pCam->setNearClipDistance(nearCD);
  if (farCD != 0)
    pCam->setFarClipDistance(farCD);
  // do fov and aspect ratio
  Real num = 0; 
  if (this->_getAttribute(pNode, "fov", num))
    pCam->setFOVy(Ogre::Angle(num));
  
  if (this->_getAttribute(pNode, "aspectRatio", num))
  {
    pCam->setAspectRatio(num);
    std::cout << "WARNING ////////////////////////////////AspectRatio "<< num <<"\n" ;
  }

  if (this->_getAttribute(pNode, "focalLength", num))
    pCam->setFocalLength(num);
  if (this->_getAttribute(pNode, "frustumOffset", num))
    pCam->setFrustumOffset(num);


  // do type
  if(pNode->Attribute("projectionType"))
  {
    String type = pNode->Attribute("projectionType");
    if (type == "perspective")
      pCam->setProjectionType(Ogre::PT_PERSPECTIVE);
    else
      pCam->setProjectionType(Ogre::PT_ORTHOGRAPHIC);
  }
  // normal (direction)
  pElem = pNode->FirstChildElement("normal");
  if (pElem)
    pCam->setDirection(xml::readNormal(pElem));
  // look at 
  pElem = pNode->FirstChildElement("lookAt");
  if (pElem)
    pCam->lookAt(xml::readPosition(pElem));
  // do position
  pElem = pNode->FirstChildElement("position");
  if (pElem)
    pCam->setPosition(xml::readPosition(pElem));
  // do rotation
  pElem = pNode->FirstChildElement("rotation");
  if (pElem)
    pCam->setOrientation(xml::readOrientation(pElem));
  // lookAt target node
  TiXmlElement *laElem = pNode->FirstChildElement("lookTarget");
  if (laElem)
    this->_doCamLTarget(laElem, pCam);
  // track target node
  TiXmlElement *taElem = pNode->FirstChildElement("trackTarget");
  if (taElem)
    this->_doCamTTarget(taElem, pCam);
  // update the dotSceneInfo graph
  this->_updateDSI(pCam, _dslCurrPos, _dslCurrRot, _dslCurrSca, false);
}
/***************************************************************************************************/
void dotSceneLoaderImpl020::_doBillboardSet(SceneNode *pParent, TiXmlElement *pNode)
{
  String name = this->_getNodeName(pNode);
  this->updateProgressListeners("parsing billboard set '" + name + "'");
  BillboardSet *pBS = NULL;
  //try to create a billboard
  try
  {
    // setName according OMK name
    String bbsName = setName(name, "billboardSet", mpSceneManager);
    pBS = mpSceneManager->createBillboardSet(name);
  }
  catch (Exception ex)
  {
    dotSceneInfo::_logLoadError("Failed to create billboardSet '" + name);
    pBS = NULL;
  }
  if (!pBS)
    return;
  // type :(
  String type = "point";
  Ogre::BillboardType bt = Ogre::BBT_POINT;
  if (this->_getAttribute(pNode, "type", type))
  {
    if (type == "point")
      bt = (Ogre::BBT_POINT);
    else if (type == "orientedCommon")
      bt = (Ogre::BBT_ORIENTED_COMMON);
    else if (type == "orientedSelf")
      bt = (Ogre::BBT_ORIENTED_SELF);
    else
    {
      dotSceneInfo::_logLoadWarning("unknown billboardSet type '" + type + "' will skip...");
      mpSceneManager->destroyBillboardSet(pBS);
      return;
    }
  }
  pBS->setBillboardType(bt);
  // origin :(
  String origin = "center";
  Ogre::BillboardOrigin bo = Ogre::BBO_CENTER;
  if (this->_getAttribute(pNode, "origin", origin))
  {
    if (origin == "topLeft")
      bo = Ogre::BBO_TOP_LEFT;
    if (origin == "topCenter")
      bo = Ogre::BBO_TOP_CENTER;
    if (origin == "topRight")
      bo = Ogre::BBO_TOP_RIGHT;
    if (origin == "centerLeft")
      bo = Ogre::BBO_CENTER_LEFT;
    if (origin == "center")
      bo = Ogre::BBO_CENTER;
    if (origin == "centerRight")
      bo = Ogre::BBO_CENTER_RIGHT;
    if (origin == "bottomLeft")
      bo = Ogre::BBO_BOTTOM_LEFT;
    if (origin == "bottomCenter")
      bo = Ogre::BBO_BOTTOM_CENTER;
    if (origin == "bottomRight")
      bo = Ogre::BBO_BOTTOM_RIGHT;
  }
  pBS->setBillboardOrigin(bo);
  // dimensions
  Real width, height;
  if (this->_getAttribute(pNode, "width", width))
    pBS->setDefaultWidth(width);
  if (this->_getAttribute(pNode, "height", height))
    pBS->setDefaultHeight(height);
  // material
  String matName = "BaseWhite";
  if (this->_getAttribute(pNode, "material", matName))
  {
    try
    {
      pBS->setMaterialName(matName);
    }
    catch (Exception ex)
    {
      dotSceneInfo::_logLoadWarning("material '" + matName + "' failed to load, billboardSet '" + name + "'");
      pBS->setMaterialName("BaseWhite");
    }
  }
  // do all billboards
  for (TiXmlElement *pElem = pNode->FirstChildElement(); pElem != 0; pElem = pElem->NextSiblingElement())
  {
    if (String(pElem->Value()) == "billboard")
      this->_doBillboard(pBS, pElem);
    else
      dotSceneInfo::_logLoadWarning(
      "Unknown XMLNode of type '" + 
      String(pElem->Value()) + 
      "' encountered while reading billboardSet childs, will skip this node...");
  }
  // update the dotSceneInfo graph
  pParent->attachObject(pBS);
  this->_updateDSI(pBS, _dslCurrPos, _dslCurrRot, _dslCurrSca, false);
}
/***************************************************************************************************/
void dotSceneLoaderImpl020::_doBillboard(BillboardSet *pParent, TiXmlElement *pNode)
{
  String name = this->_getNodeName(pNode);
  this->updateProgressListeners("parsing billboard '" + name + "'");
  // dimensions
  Real width, height;
  Billboard *pBB;
  Vector3 pos = Vector3::ZERO;
  ColourValue dif = ColourValue::White;
  Quaternion rot = Quaternion::IDENTITY;
  // read position
  TiXmlElement *pElem = pNode->FirstChildElement("position");
  if (pElem)
    pos = xml::readPosition(pElem);
  // read diffuse color
  pElem = pNode->FirstChildElement("colourDiffuse");
  if (pElem)
    dif = xml::readColor(pElem);
  // create the bill
  try
  {
    pBB = pParent->createBillboard(pos, dif);
    if (this->_getAttribute(pNode, "width", width) && this->_getAttribute(pNode, "height", height))
      pBB->setDimensions(width, height);
    // set rotation if present
    pElem = pNode->FirstChildElement("rotation");
    if (pElem)
      pBB->setRotation(Ogre::Angle(xml::readOrientation(pElem).w));
  }
  catch (Exception ex)
  {
    dotSceneInfo::_logLoadError("Failed to create billboard '" + name);
  }
}
/***************************************************************************************************/
void dotSceneLoaderImpl020::_doParticleSystem(SceneNode *pParent, TiXmlElement *pNode)
{
  //get the name of the particle system
  Ogre::String elementName ("");
  elementName = this->_getNodeName(pNode);
  if (elementName == "")
    elementName = _autoName();
  this->updateProgressListeners("parsing particle system '" + elementName + "'");

  //get the config File name
  Ogre::String templateName ("");

  if (pNode->Attribute("templateName")) templateName = pNode->Attribute("templateName");
  else if (pNode->Attribute("file")) templateName = pNode->Attribute("file"); 
  
  Ogre::ParticleSystem * pPS(NULL);
  try
  {
    pPS = mpSceneManager->createParticleSystem(elementName, templateName);
    pParent->attachObject(pPS);
  }
  catch (Exception ex)
  {
    dotSceneInfo::_logLoadError("Failed to create particle system '" + elementName);
    pPS = NULL;
    return;
  }
}
/***************************************************************************************************/
void dotSceneLoaderImpl020::_doPlane(SceneNode *pParent, TiXmlElement *pNode)
{
  Ogre::String name ("plane");
  name = pNode->Attribute("name");
  Ogre::Vector2 planeSize;
  if (pNode->FirstChildElement("size"))
    planeSize = xml::readVector2(pNode->FirstChildElement("size"));
  //OgreMax compatibility
  if (pNode->Attribute("width"))
    planeSize.x = StringConverter::parseReal(pNode->Attribute("width"));
  //OgreMax compatibility
  if (pNode->Attribute("height"))
    planeSize.y = StringConverter::parseReal(pNode->Attribute("height"));
  assert(planeSize != Ogre::Vector2::ZERO && " Need Size tab to create plane");

  Ogre::Vector2 planeSegments (1,1);
  if (pNode->FirstChildElement("segments"))
    planeSegments = xml::readVector2(pNode->FirstChildElement("segments"));
  Ogre::String material ("");
  if (pNode->Attribute("material"))
    material = pNode->Attribute("material");
  Ogre::Real distance(20);
  if (pNode->Attribute("distance"))
    distance = StringConverter::parseReal(pNode->Attribute("distance"));
  Ogre::String planeNormal ("UNIT_Y");
  if (pNode->Attribute("planeNormal"))
    material = pNode->Attribute("planeNormal");
  Ogre::String planeUp ("UNIT_Z");
  if (pNode->Attribute("planeUp"))
    material = pNode->Attribute("planeUp");
  bool normal = true;
  if (pNode->Attribute("normal"))
    if (String(pNode->Attribute("normal")) == String("false"))
      normal = false;
  int numTexCoordSets = 1;
  if (pNode->Attribute ("numTexCoordSets"))
    numTexCoordSets = StringConverter::parseInt (pNode->Attribute ("numTexCoordSets"));
  Ogre::Real uTile = 1.0f;
  if (pNode->Attribute("uTile"))
    uTile = StringConverter::parseReal(pNode->Attribute("uTile"));
  Ogre::Real vTile = 1.0f;
  if (pNode->Attribute("vTile"))
    vTile = StringConverter::parseReal(pNode->Attribute("vTile"));

  Ogre::Entity*  planeEnt;
  // Define a plane mesh that will be used for the surface
  Ogre::Plane surface;
  surface.normal = Ogre::Vector3::UNIT_Y;
  static String UY = String("UNIT_Y");
  static String UZ = String("UNIT_Z");
  static String UX = String("UNIT_X");
  //normal Plane 
  if (UX == planeNormal)
    surface.normal =  Ogre::Vector3::UNIT_X;
  else if (UZ == planeNormal)
    surface.normal =  Ogre::Vector3::UNIT_Z;
  else // (UY == planeNormal)
    surface.normal =  Ogre::Vector3::UNIT_Y;

  //OgreMax compatibility
  if (pNode->FirstChildElement("normal"))
    surface.normal = xml::readVector3(pNode->FirstChildElement("normal"));

  // plane : upVector
  Vector3 upVector = Ogre::Vector3::UNIT_Z;
  if (UX == planeUp)
    upVector = Ogre::Vector3::UNIT_X;
  else if (UY == planeUp)
    upVector = Ogre::Vector3::UNIT_Y;
  else // (UZ == planeUp)
    upVector = Ogre::Vector3::UNIT_Z;
  //OgreMax compatibility
  if (pNode->FirstChildElement("upVector"))
    upVector = xml::readVector3(pNode->FirstChildElement("upVector"));

  surface.d = distance;
  String planeName =  setName(name, "mesh", mpSceneManager);

  Ogre::MeshManager::getSingleton().createPlane(planeName,
    mGroupName,
    surface,
    planeSize.x, planeSize.y, (int)planeSegments.x, (int)planeSegments.y,
    normal, numTexCoordSets, uTile, vTile, upVector);

  planeEnt = mpSceneManager ->createEntity(
    setName(planeName, "entity", mpSceneManager), planeName );

  if ("" != material)
    planeEnt->setMaterialName(material);
  bool castShadows = true;
  if (pNode->Attribute("castShadows"))
    if (String(pNode->Attribute("castShadows")) == String("false"))
      castShadows = false;
  planeEnt->setCastShadows(castShadows);

  pParent->attachObject(planeEnt);
}
/***************************************************************************************************/
void dotSceneLoaderImpl020::_doShadowProperties(TiXmlElement *pNode)
{
  this->updateProgressListeners("parsing Shadow Properties");

  static String UNI = String("UNIFORM");
  static String UNI_FO = String("UNIFORM_FOCUSED");
  static String LISPSM = String("LISPSM");

  String curr = pNode->Attribute("projection") ? pNode->Attribute("projection") : UNI;
  Ogre::ShadowCameraSetupPtr shadowCam;
  if (curr == UNI)
    shadowCam = Ogre::ShadowCameraSetupPtr (
    new Ogre::DefaultShadowCameraSetup());
  else if (curr == UNI_FO)
    shadowCam = Ogre::ShadowCameraSetupPtr (
    new Ogre::FocusedShadowCameraSetup()); 
  else if (curr == LISPSM)
    shadowCam = Ogre::ShadowCameraSetupPtr (
    new Ogre::LiSPSMShadowCameraSetup());

  mpSceneManager->setShadowCameraSetup (shadowCam);
  //  std::cout << "[DotScene] Shadow Projection "<< curr << std::endl;

  static String NO = String("No Shadow");
  static String SM = String("Stencil Modulative");
  static String SA = String("Stencil Additive");
  static String TA = String("Texture Additive");
  static String TM = String("Texture Modulative");
  //static String PSSM = String("PSSM");

  curr = pNode->Attribute("type") ? pNode->Attribute("type") : NO;
  if (curr == NO)
    mpSceneManager->setShadowTechnique(SHADOWTYPE_NONE);
  else if (curr == SM)
    mpSceneManager->setShadowTechnique(SHADOWTYPE_STENCIL_MODULATIVE);
  else if (curr == SA)
    mpSceneManager->setShadowTechnique(SHADOWTYPE_STENCIL_ADDITIVE);
  else if (curr == TM)
    mpSceneManager->setShadowTechnique(SHADOWTYPE_TEXTURE_MODULATIVE);
  else if (curr == TA)
    mpSceneManager->setShadowTechnique(SHADOWTYPE_TEXTURE_ADDITIVE);

  /*
  if ((curr == TM)  || (curr ==TA))
  {
  mpSceneManager->setShadowTexturePixelFormat(PF_FLOAT16_R);
  mpSceneManager->setShadowTextureCasterMaterial("Ogre/DepthShadowmap/Caster/Float");
  mpSceneManager->setShadowTextureReceiverMaterial("Ogre/DepthShadowmap/Receiver/Float");
  mpSceneManager->setShadowTextureSelfShadow(true);
  }
  else  if (curr == PSSM)
  {
  if (!pNode->Attribute("camera"))
  assert (false && "Need camera for PSSM technique !");
  if (!pNode->Attribute("light"))
  assert (false && "Need light for PSSM technique !");
  String cam = pNode->Attribute("camera");
  String light = pNode->Attribute("light");
  Ogre::Camera* mCamera = mpSceneManager->getCamera(cam);
  Ogre::Light* mLight = mpSceneManager->getLight(light);
  mpSceneManager->addShadowListener(new PSSMShadowListener(mpSceneManager,mLight,shadowCam, mCamera));
  mpSceneManager->setShadowTextureCount(3);

  mpSceneManager->setShadowTechnique(Ogre::SHADOWTYPE_TEXTURE_ADDITIVE_INTEGRATED);
  mpSceneManager->setShadowTextureSelfShadow(true);
  mpSceneManager->setShadowTextureFadeStart(1.f);
  mpSceneManager->setShadowTextureFadeEnd(1.f);

  mpSceneManager->setShadowCameraSetup(shadowCam);
  mpSceneManager->setShadowCasterRenderBackFaces(true);
  mpSceneManager->setShadowTextureSize(512);
  // to test with or without...
  return;
  }
  */

  //TRACE_WARNING( "[DotScene] Shadow Technique "<< curr);
  if (pNode->Attribute ("texture_size")){
    int tex_size = StringConverter::parseInt (pNode->Attribute ("texture_size"));
    //   std::cout << "[DotScene] Shadow Texture Size "<< tex_size << std::endl;
    mpSceneManager->setShadowTextureSize(tex_size);
  }
  if (pNode->Attribute ("texture_count")){
    int tex_count = StringConverter::parseInt (pNode->Attribute ("texture_count"));
    mpSceneManager->setShadowTextureCount (tex_count);
    std::cout << "[DotScene] Shadow Texture Count "<< tex_count << std::endl;
  }
  if (pNode->Attribute ("far_distance")){
    int dist = StringConverter::parseInt (pNode->Attribute ("far_distance"));
    mpSceneManager->setShadowFarDistance(dist);
    // std::cout << "[DotScene] Shadow Far Distance "<< dist << std::endl;
  }

  TiXmlElement* colorElem = pNode->FirstChildElement("colour");
  if(colorElem)
  {
    ColourValue color;
    color.r = StringConverter::parseReal(colorElem->Attribute("r"));
    color.g = StringConverter::parseReal(colorElem->Attribute("g"));
    color.b = StringConverter::parseReal(colorElem->Attribute("b"));
    //std::cout << "[DotScene] Shadow Colour "<< color << std::endl;
    mpSceneManager->setShadowColour(color);
  }
}
/***************************************************************************************************/
void dotSceneLoaderImpl020::_doSkyBox(TiXmlElement *pNode)
{
  this->updateProgressListeners("parsing sky box");
  // do material name
  String matName = pNode->Attribute("material") ? pNode->Attribute("material") : "BaseWhite";
  // distance
  float dist = 5000;
  if (pNode->Attribute("distance"))
    dist = StringConverter::parseReal(pNode->Attribute("distance"));
  // drawFirst
  bool first = true;
  if (pNode->Attribute("drawFirst"))
  {
    if (String(pNode->Attribute("drawFirst")) == String("false"))
      first = false;
  }
  // get orientation
  TiXmlElement *pOri = pNode->FirstChildElement("rotation");
  Quaternion ori = Quaternion::IDENTITY;
  if (pOri)
    ori = xml::readOrientation(pOri);
  // set it ;)
  mpSceneManager->setSkyDome(false, "");
  mpSceneManager->setSkyPlane(false, Plane(), "");
  try
  {
    mpSceneManager->setSkyBox(true, matName, dist, first, ori);
    if (this->mDoInfo)
      dotSceneInfo::_setSkyBox(matName, dist, ori, first);
  }
  catch (Exception ex)
  {
    dotSceneInfo::_logLoadError("something went wrong while seting skyBox :(");
    mpSceneManager->setSkyBox(false, "");
  }
}
/***************************************************************************************************/
void dotSceneLoaderImpl020::_doSkyDome(TiXmlElement *pNode)
{
  this->updateProgressListeners("parsing sky dome");
  // do material name
  String matName = pNode->Attribute("material") ? pNode->Attribute("material") : "BaseWhite";
  // distance
  float dist = 4000;
  if (pNode->Attribute("distance"))
    dist = StringConverter::parseReal(pNode->Attribute("distance"));
  // drawFirst
  bool first = true;
  if (pNode->Attribute("drawFirst"))
  {
    if (String(pNode->Attribute("drawFirst")) == String("false"))
      first = false;
  }
  // get orientation
  TiXmlElement *pOri = pNode->FirstChildElement("rotation");
  Quaternion ori = Quaternion::IDENTITY;
  if (pOri)
    ori = xml::readOrientation(pOri);
  // curvature
  float cur = 10;
  if (pNode->Attribute("curvature"))
    cur = StringConverter::parseReal(pNode->Attribute("curvature"));
  // tiling
  float tiling = 8;
  if (pNode->Attribute("tiling"))
    tiling = StringConverter::parseReal(pNode->Attribute("tiling"));
  // set it
  mpSceneManager->setSkyBox(false, "");
  mpSceneManager->setSkyPlane(false, Plane(), "");
  try
  {
    mpSceneManager->setSkyDome(true, matName, cur, tiling, dist, first, ori);
    if (this->mDoInfo)
      dotSceneInfo::_setSkyDome(matName, cur, tiling, dist, ori, first);
  }
  catch (Exception ex)
  {
    dotSceneInfo::_logLoadError("something went wrong while seting skyDome :(");
    mpSceneManager->setSkyDome(false, "");
  }
}
/***************************************************************************************************/
void dotSceneLoaderImpl020::_doSkyPlane(TiXmlElement *pNode)
{
  this->updateProgressListeners("parsing sky plane");
  // do material name
  String matName = pNode->Attribute("material") ? pNode->Attribute("material") : "BaseWhite";
  // drawFirst
  bool first = true;
  if (pNode->Attribute("drawFirst"))
  {
    if (String(pNode->Attribute("drawFirst")) == String("false"))
      first = false;
  }
  // planeX
  float planeX = 0;
  if (pNode->Attribute("planeX"))
    planeX = StringConverter::parseReal(pNode->Attribute("planeX"));
  // planeY
  float planeY = -1;
  if (pNode->Attribute("planeY"))
    planeY = StringConverter::parseReal(pNode->Attribute("planeY"));
  // planeZ
  float planeZ = 0;
  if (pNode->Attribute("planeZ"))
    planeZ = StringConverter::parseReal(pNode->Attribute("planeZ"));
  // planeD
  float planeD = 500;
  if (pNode->Attribute("planeD"))
    planeD = StringConverter::parseReal(pNode->Attribute("planeD"));
  // scale
  float scale = 1000;
  if (pNode->Attribute("scale"))
    scale = StringConverter::parseReal(pNode->Attribute("scale"));
  // tiling
  float tiling = 10;
  if (pNode->Attribute("tiling"))
    tiling = StringConverter::parseReal(pNode->Attribute("tiling"));
  // bow
  float bow = 0;
  if (pNode->Attribute("bow"))
    bow = StringConverter::parseReal(pNode->Attribute("bow"));
  // set it
  mpSceneManager->setSkyBox(false, "");
  mpSceneManager->setSkyDome(false, "");
  try
  {
    mpSceneManager->setSkyPlane(true, Ogre::Plane(Vector3(planeX, planeY, planeZ), planeD), matName, scale, tiling, first, bow);
    if (this->mDoInfo)
      dotSceneInfo::_setSkyPlane(matName, planeX, planeY, planeZ, planeD, scale, tiling, bow, first);
  }
  catch (Exception ex)
  {
    dotSceneInfo::_logLoadError("something went wrong while seting skyPlane :(");
    mpSceneManager->setSkyPlane(false, Plane(), "");
  }
}
/***************************************************************************************************/
void dotSceneLoaderImpl020::_doFog(TiXmlElement *pNode)
{
  this->updateProgressListeners("parsing fog");
  // expDensity
  float expDensity = 0;
  if (pNode->Attribute("expDensity"))
    expDensity = StringConverter::parseReal(pNode->Attribute("expDensity"));
  // linearStart
  float linearStart = 0;
  if (pNode->Attribute("linearStart"))
    linearStart = StringConverter::parseReal(pNode->Attribute("linearStart"));
  // linearEnd
  float linearEnd = 0;
  if (pNode->Attribute("linearEnd"))
    linearEnd = StringConverter::parseReal(pNode->Attribute("linearEnd"));
  // get color
  TiXmlElement *pCol = pNode->FirstChildElement("colourDiffuse");
  ColourValue color = ColourValue::White;
  if (pCol)
    color = xml::readColor(pCol);
  // mode
  FogMode mode = Ogre::FOG_NONE; 
  String fm = pNode->Attribute("mode") ? pNode->Attribute("mode") : "none";
  if (fm == "exp")
    mode = Ogre::FOG_EXP;
  else if (fm == "exp2")
    mode = Ogre::FOG_EXP2;
  else if (fm == "linear")
    mode = Ogre::FOG_LINEAR;
  else if (fm == "none")
    mode = Ogre::FOG_NONE;
  else
    dotSceneInfo::_logLoadWarning("invalid fog mode attribute : '" + fm + "', skipped !!!");
  // now set it
  mpSceneManager->setFog(mode, color, expDensity, linearStart, linearEnd);
  if (this->mDoInfo)
    dotSceneInfo::_setFog(mode, color, expDensity, linearStart, linearEnd);
}
/***************************************************************************************************/
void dotSceneLoaderImpl020::_doTerrain(TiXmlElement *pNode)
{
  this->updateProgressListeners("parsing terrain");

  //verify that SceneManager is a TerrainSceneManager
  if (mpSceneManager->getTypeName() != "TerrainSceneManager")
    dotSceneInfo::_logLoadError("invalid SceneManager type : use TerrainSceneManager");

  //get the config File
  Ogre::String filePath ("");
  if (pNode->Attribute("filePath"))
    filePath = pNode->Attribute("filePath");
  //set it
  if (filePath != "")
  {
    ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
    try
    {
      mpSceneManager->setWorldGeometry(filePath);
    }
    catch( Ogre::FileNotFoundException& e )
    {
      dotSceneInfo::_logLoadError( e.getFullDescription() ) ;
      assert( false ) ;
    }
    /*  catch( Ogre::ItemIdentityException& e )
    {
    dotSceneInfo::_logLoadError( e.getFullDescription() ) ;
    assert( false ) ;
    }
    */
    //get position and set it
    Ogre::Vector3 position (Ogre::Vector3::ZERO);
    Ogre::SceneNode * terrainNode (mpSceneManager->getSceneNode("Terrain"));
    //corner position
    TiXmlElement *pCoPos = pNode->FirstChildElement("cornerPosition");
    if (pCoPos) 
    {
      position = xml::readPosition(pCoPos);
      terrainNode->setPosition(position);
    }
    //center position
    /*TiXmlElement *pCePos = pNode->FirstChildElement("centerPosition");
    if (pCePos) 
    {
    Ogre::Vector3 mapSize ((((Ogre::TerrainSceneManager *)mpSceneManager)->getScale() * ((Ogre::TerrainSceneManager *)mpSceneManager)->getPageSize()-1) / 2);
    Ogre::Vector3 centerPosition = CVector3(mapSize.x, ((Ogre::TerrainSceneManager *)mpSceneManager)->getHeightAt(mapSize.x, mapSize.z), mapSize.z); 

    position = xml::readPosition(pCePos);
    terrainNode->setPosition(position - centerPosition);
    }*/
  }
}
/***************************************************************************************************/
void dotSceneLoaderImpl020::_doItem(TiXmlElement *pNode)
{
  String type;
  if (!pNode->Attribute("type"))
    return;
  else
    type = pNode->Attribute("type");
  if (type == "material" && mDoMats)
  {
    TiXmlElement *pFileElem = pNode->FirstChildElement("file");
    if (pFileElem)
    {
      String fileName = this->_doFile(pFileElem);
      if (fileName != "")
      {
        //DataChunk dc;
        //if (ResourceManager::_findCommonResourceData(fileName, dc))
        //  MaterialManager::getSingleton().parseScript(dc);
      }
    }
  }
  else
  {
    if (mDoMats)
      dotSceneInfo::_logLoadWarning(
      "Unknown item type '" + 
      type + 
      "' encountered, will skip it, no parsing...");
  }
}
/***************************************************************************************************/
Ogre::String dotSceneLoaderImpl020::_doFile(TiXmlElement *pNode)
{
  return pNode->Attribute("name") ? pNode->Attribute("name") : StringUtil::BLANK;
}
/***************************************************************************************************/
void dotSceneLoaderImpl020::_doExternals(TiXmlElement *pNode)
{
  // all children will be <item>s
  for (TiXmlElement *pElem = pNode->FirstChildElement(); pElem != 0; pElem = pElem->NextSiblingElement())
  {
    if (String(pElem->Value()) == "item")
      this->_doItem(pElem);
    else
      dotSceneInfo::_logLoadWarning(
      "Unknown XMLNode of type '" + 
      String(pElem->Value()) + 
      "' encountered while reading 'external'->'item', will skip this node...");
  }
}
/***************************************************************************************************/
void dotSceneLoaderImpl020::_doLightAttenuation(
  TiXmlElement *pElem, 
  Real &range, Real &constant, 
  Real &linear, Real &quadratic)
{
  //Default value: 
  range = 100000;
  constant = 1.0f;
  linear = 0.0f;
  quadratic = 0.0f;
  if (pElem->Attribute("range"))
    range = StringConverter::parseReal(pElem->Attribute("range"));

  if (pElem->Attribute("constant"))
    constant = StringConverter::parseReal(pElem->Attribute("constant"));
  if (pElem->Attribute("linear"))
    linear = StringConverter::parseReal(pElem->Attribute("linear"));
  if (pElem->Attribute("quadratic"))
    quadratic = StringConverter::parseReal(pElem->Attribute("quadratic"));
}
/***************************************************************************************************/
void dotSceneLoaderImpl020::_doLightRange(TiXmlElement *pElem, Real &inner, Real &outer, Real &falloff)
{
  inner = 0.523598;
  outer = 0.698131;
  falloff = 1.0f;

  if (pElem->Attribute("inner"))
    inner = StringConverter::parseReal(pElem->Attribute("inner"));
  if (pElem->Attribute("outer"))
    outer = StringConverter::parseReal(pElem->Attribute("outer"));
  if (pElem->Attribute("falloff"))
    falloff = StringConverter::parseReal(pElem->Attribute("falloff"));
}
/***************************************************************************************************/
void dotSceneLoaderImpl020::_doScene(SceneNode *pParent, TiXmlElement *pNode)
{
	String sceneName = this->_getNodeName(pNode);
	this->updateProgressListeners("parsing scene '" + sceneName + "'");
	Entity *pEnt = NULL;
	String sceneFile = pNode->Attribute("sceneFile") ? pNode->Attribute("sceneFile") : StringUtil::BLANK;
	if (sceneFile != StringUtil::BLANK)
	{
		try
		{
			SceneNode* root = mpRoot;
			Singleton<dsi::dotSceneLoader>::getSingleton().load( sceneFile, "parentGroup", mpSceneManager, mpWin, pParent, mDoMats, mShadow, mDoInfo ) ;
			mpRoot = root ;
		}
		catch (Exception ex)
		{
			dotSceneInfo::_logLoadError("'" + sceneFile + "' failed to load");
			pEnt = NULL;
		}
	}
	else
	{
		dotSceneInfo::_logLoadError("scene '" + sceneName + "' failed to load, check the .scene");
	}
}
/***************************************************************************************************/
void dotSceneLoaderImpl020::_updateDSI(Ogre::MovableObject *pMO, Ogre::Vector3 &pos, Ogre::Quaternion &rot, Ogre::Vector3 &scale, bool isStatic)
{
  if (this->mDoInfo)
    dotSceneInfo::_addMovable(pMO, pos, rot, scale, isStatic);
}
/***************************************************************************************************/
String dotSceneLoaderImpl020::_getNodeName(TiXmlElement *pNode)
{
  return pNode->Attribute("name") == NULL ? this->_autoName() : pNode->Attribute("name");
}
/***************************************************************************************************/
Ogre::String dotSceneLoaderImpl020::_autoName()
{
  static unsigned int id = 0;
  return String("dseAutoName" + StringConverter::toString(id++));
}
/***************************************************************************************************/
bool dotSceneLoaderImpl020::_getAttribute(TiXmlElement *pElem, const String &attribute, String &attributeValue)
{
  const char *s = pElem->Attribute(attribute.c_str());
  if (!s)
    return false;

  attributeValue = String(s);
  return true;
}
/***************************************************************************************************/
bool dotSceneLoaderImpl020::_getAttribute(TiXmlElement *pElem, const String &attribute, bool &attributeValue)
{
  String value;
  if (!this->_getAttribute(pElem, attribute, value))
    return false;

  attributeValue = StringConverter::parseBool(value);
  return true;
}
/***************************************************************************************************/
bool dotSceneLoaderImpl020::_getAttribute(TiXmlElement *pElem, const String &attribute, Real &attributeValue)
{
  String value;
  if (!this->_getAttribute(pElem, attribute, value))
    return false;

  attributeValue = StringConverter::parseReal(value);
  return true;
}
/***************************************************************************************************/
void dotSceneLoaderImpl020::__doSNLTarget(TiXmlElement *pElem, SceneNode *pSN)
{
  this->updateProgressListeners("parsing lookAt target");
  String nodeName = StringUtil::BLANK, relativeTo = StringUtil::BLANK;
  Vector3 pos = Vector3::ZERO, nor = Vector3::ZERO;
  TiXmlElement *pPElem = NULL, *pNElem = NULL;
  SceneNode *pTargetSN = NULL;
  Ogre::SceneNode::TransformSpace ts;
  // read attributes first
  this->_getAttribute(pElem, "nodeName", nodeName);
  // find the node
  if (nodeName != StringUtil::BLANK)
  {
    try    
    {
      pTargetSN = mpSceneManager->getSceneNode(nodeName);
    }
    catch (Exception)
    {
      dotSceneInfo::_logLoadError("taget sceneNode '" + nodeName + "' not found, skiping this target...");
      return;
    }
  }
  // go on to 'relativeTo' param...
  this->_getAttribute(pElem, "relativeTo", relativeTo);
  if (relativeTo != StringUtil::BLANK)
  {
    if (relativeTo == "local")
      ts = SceneNode::TS_LOCAL;
    else if (relativeTo == "parent")
      ts = SceneNode::TS_PARENT;
    else if (relativeTo == "world")
      ts = SceneNode::TS_WORLD;
    else
    {
      dotSceneInfo::_logLoadError("invalid 'relativeTo' attribute found : '" + relativeTo + "' check your scene file !!");
      return;
    }
  }
  // do child elements
  pPElem = pElem->FirstChildElement("position");
  pNElem = pElem->FirstChildElement("localDirection");
  if (pPElem)
  {
    pos = xml::readPosition(pPElem);
    //check if we have a conflict like 'nodeName' defined and laso have a position (redundant in the case :))
    if (nodeName != StringUtil::BLANK)
    {
      dotSceneInfo::_logLoadError("attribute 'nodeName' found and 'position' element is defined, what do u mean ??, will skip...");
      return;
    }
  }
  else
  {
    if (nodeName == StringUtil::BLANK)
    {
      dotSceneInfo::_logLoadError("attribute 'nodeName' not found and no 'position' element is defined, will skip...");
      return;
    }
  }
  // get the localDirection
  if (pNElem)
    nor = xml::readNormal(pNElem);
  // apply to node
  if (pTargetSN)
    pos = pTargetSN->_getDerivedPosition(); // TODO : should this change with the TransformSpace ??
  pSN->lookAt(pos, ts, pNElem ? nor : Vector3::NEGATIVE_UNIT_Z);
}
/***************************************************************************************************/
void dotSceneLoaderImpl020::__doCamLTarget(TiXmlElement *pElem, Camera *pCam)
{
  this->updateProgressListeners("parsing lookAt target");
  String nodeName = StringUtil::BLANK;
  Vector3 pos = Vector3::ZERO;
  TiXmlElement *pPElem = NULL;
  SceneNode *pTargetSN = NULL;
  // read attributes first
  this->_getAttribute(pElem, "nodeName", nodeName);
  // find the node
  if (nodeName != StringUtil::BLANK)
  {
    try    
    {
      pTargetSN = mpSceneManager->getSceneNode(nodeName);
    }
    catch (Exception)
    {
      dotSceneInfo::_logLoadError("taget sceneNode '" + nodeName + "' not found, skiping this target...");
      return;
    }
  }
  // do child elements
  pPElem = pElem->FirstChildElement("position");
  if (pPElem)
  {
    pos = xml::readPosition(pPElem);
    //check if we have a conflict like 'nodeName' defined and laso have a position (redundant in the case :))
    if (nodeName != StringUtil::BLANK)
    {
      dotSceneInfo::_logLoadError("attribute 'nodeName' found and 'position' element is defined, what do u mean ??, will skip...");
      return;
    }
  }
  else
  {
    if (nodeName == StringUtil::BLANK)
    {
      dotSceneInfo::_logLoadError("attribute 'nodeName' not found and no 'position' element is defined, will skip...");
      return;
    }
  }
  // apply to node
  if (pTargetSN)
    pos = pTargetSN->_getDerivedPosition();
  pCam->lookAt(pos);
}
/***************************************************************************************************/
void dotSceneLoaderImpl020::__doCamTTarget(TiXmlElement *pElem, Camera *pCam)
{
  this->updateProgressListeners("parsing autoTrack target");
  String nodeName = StringUtil::BLANK;
  Vector3 ofs = Vector3::ZERO;
  TiXmlElement *pOfs = NULL;
  SceneNode *pTargetSN = NULL;
  // read attributes first
  this->_getAttribute(pElem, "nodeName", nodeName);
  // find the node
  if (nodeName != StringUtil::BLANK)
  {
    try    
    {
      pTargetSN = mpSceneManager->getSceneNode(nodeName);
    }
    catch (Exception)
    {
      dotSceneInfo::_logLoadError("taget sceneNode '" + nodeName + "' not found, skiping this target...");
      return;
    }
  }
  else
  {
    dotSceneInfo::_logLoadError("nodeName attribute not found, skiping this target, it's invalid, check your .scene file...");
    return;
  }
  // do offset, it's optional
  pOfs = pElem->FirstChildElement("offset");
  if (pOfs)
    ofs = xml::readNormal(pOfs); // offset is also x,y,z attributes of a element, same as Normal/Position/Scale...
  // apply to node
  pCam->setAutoTracking(true, pTargetSN, ofs);
}
/***************************************************************************************************/
void dotSceneLoaderImpl020::__doSNTTarget(TiXmlElement *pElem, SceneNode *pSN)
{
  this->updateProgressListeners("parsing autoTrack target");
  String nodeName = StringUtil::BLANK;
  Vector3 ofs = Vector3::ZERO, nor = Vector3::NEGATIVE_UNIT_Z;
  TiXmlElement *pOfs = NULL, *pNor = NULL;
  SceneNode *pTargetSN = NULL;
  // read attributes first
  this->_getAttribute(pElem, "nodeName", nodeName);
  // find the node
  if (nodeName != StringUtil::BLANK)
  {
    try    
    {
      pTargetSN = mpSceneManager->getSceneNode(nodeName);
    }
    catch (Exception)
    {
      dotSceneInfo::_logLoadError("taget sceneNode '" + nodeName + "' not found, skiping this target...");
      return;
    }
  }
  else
  {
    dotSceneInfo::_logLoadError("nodeName attribute not found, skiping this target, it's invalid, check your .scene file...");
    return;
  }
  // do child elements
  // do normal, it's optional
  pNor = pElem->FirstChildElement("localDirection");
  if (pNor)
    nor = xml::readNormal(pNor);
  // do offset, it's optional
  pOfs = pElem->FirstChildElement("offset");
  if (pOfs)
    ofs = xml::readNormal(pOfs); // offset is also x,y,z attributes of a element, same as Normal/Position/Scale...
  // apply to node
  pSN->setAutoTracking(true, pTargetSN, nor, ofs);
}
/***************************************************************************************************/
void dotSceneLoaderImpl020::_doSNLTarget(TiXmlElement *pElem, SceneNode *pSN)
{
  _dslSNLTargets.insert(std::map<TiXmlElement*, SceneNode*>::value_type(pElem, pSN));
}
/***************************************************************************************************/
void dotSceneLoaderImpl020::_doSNTTarget(TiXmlElement *pElem, SceneNode *pSN)
{
  _dslSNTTargets.insert(std::map<TiXmlElement*, SceneNode*>::value_type(pElem, pSN));
}
/***************************************************************************************************/
void dotSceneLoaderImpl020::_doCamLTarget(TiXmlElement *pElem, Camera *pCam)
{
  _dslCamLTargets.insert(std::map<TiXmlElement*, Camera*>::value_type(pElem, pCam));
}
/***************************************************************************************************/
void dotSceneLoaderImpl020::_doCamTTarget(TiXmlElement *pElem, Camera *pCam)
{
  _dslCamTTargets.insert(std::map<TiXmlElement*, Camera*>::value_type(pElem, pCam));
}

typedef std::map< String, bool (*)( SceneManager *, const String& ) > HasMethodMap ;
#define DOTSCENELOADERIMPL020_SCENEMANAGERHAS( TYPE ) \
  bool SceneManagerHas##TYPE( SceneManager *mpSceneManager, const String& nodeName ) \
  { \
    return mpSceneManager->has##TYPE( nodeName ) ; \
  }
DOTSCENELOADERIMPL020_SCENEMANAGERHAS( SceneNode    )
DOTSCENELOADERIMPL020_SCENEMANAGERHAS( Entity       )
DOTSCENELOADERIMPL020_SCENEMANAGERHAS( ManualObject )
DOTSCENELOADERIMPL020_SCENEMANAGERHAS( Light        )
DOTSCENELOADERIMPL020_SCENEMANAGERHAS( BillboardSet )
DOTSCENELOADERIMPL020_SCENEMANAGERHAS( Animation    )
DOTSCENELOADERIMPL020_SCENEMANAGERHAS( Camera       )
bool MeshManagerHasMesh( SceneManager *mpSceneManager, const String& nodeName )
{
  return MeshManager::getSingleton().resourceExists( nodeName ) ;
}

//---------------------------------------------------------------------------
/// SetName According to what we have on scene now... 
//---------------------------------------------------------------------------
Ogre::String dotSceneLoaderImpl020::setName(const Ogre::String & original, const Ogre::String & type, 
  Ogre::SceneManager *mpSceneManager)
{
  static HasMethodMap dispatch ;
  if( dispatch.empty() )
  {
    dispatch[ "sceneNode"    ] = &SceneManagerHasSceneNode    ;
    dispatch[ "entity"       ] = &SceneManagerHasEntity       ;
    dispatch[ "manualObject" ] = &SceneManagerHasManualObject ;
    dispatch[ "light"        ] = &SceneManagerHasLight        ;
    dispatch[ "billboardSet" ] = &SceneManagerHasBillboardSet ;
    dispatch[ "animation"    ] = &SceneManagerHasAnimation    ;
    dispatch[ "camera"       ] = &SceneManagerHasCamera       ;
    dispatch[ "mesh"         ] = &MeshManagerHasMesh          ;
  }

  String nodeName = original;
  int counter = 1;

  HasMethodMap::iterator method = dispatch.find( type ) ;
  if( method != dispatch.end() )
  {
    while( (*(method->second))( mpSceneManager, nodeName ) )
    {
      std::ostringstream nodeNameTmp ;
      nodeNameTmp << original << "_#_" << counter++ ; 
      nodeName = nodeNameTmp.str() ;    
    }
  }
  return nodeName;
}

/***************************************************************************************************/
}// namespace dsi
/*******************************************************************************************************/
}// namespace Ogre
/*******************************************************************************************************/
