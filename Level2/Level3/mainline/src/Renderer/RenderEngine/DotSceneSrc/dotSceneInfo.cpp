/***************************************************************************************************/
#include "stdafx.h"
/***************************************************************************************************/
#include "dotSceneInfo.h"
/***************************************************************************************************/
namespace Ogre
{
  /***************************************************************************************************/
  namespace dsi
  {
    types::skyBox				dotSceneInfo::mBox		;
    types::skyPlane				dotSceneInfo::mPlane	;
    types::skyDome				dotSceneInfo::mDome	;
    types::sceneFog				dotSceneInfo::mFog	;
    /// scene graph static obj's container
    dotSceneInfo::tMapMovableEx	dotSceneInfo::mMOStatic;
    /// scene graph dynamic obj's container
    dotSceneInfo::tMapMovableEx	dotSceneInfo::mMODynamic;
    /// scene graph all obj's container
    dotSceneInfo::tMapMovableEx	dotSceneInfo::mMOAll;
    /// scene graph target nodes container
    dotSceneInfo::tMapSceneNode	dotSceneInfo::mSNTargets;
    /// scene graph loading log
    dotSceneInfo::tVecLoadLog	dotSceneInfo::mLoadLog;
    /// num tris in current scene graph
    size_t						dotSceneInfo::mNumTris;
    /// env. colors
    Ogre::ColourValue			dotSceneInfo::mBgndColor			= ColourValue::Black;
    Ogre::ColourValue			dotSceneInfo::mAmbLightColor		= ColourValue::White;
    /// up to date flag
    bool						dotSceneInfo::mIsUpToDateWithScene	= false;
    /***************************************************************************************************/
    namespace types
    {
      const types::skyBox types::skyBox::DEFAULT = types::skyBox();
      const types::skyDome types::skyDome::DEFAULT = types::skyDome();
      const types::skyPlane types::skyPlane::DEFAULT = types::skyPlane();
      const types::sceneFog types::sceneFog::DEFAULT = types::sceneFog();
    }// namespace types
    /***************************************************************************************************************/
    void dotSceneInfo::_reset()
    {
      mFog			= types::sceneFog::DEFAULT;
      mBox			= types::skyBox::DEFAULT;
      mPlane			= types::skyPlane::DEFAULT;
      mDome			= types::skyDome::DEFAULT;
      mBgndColor		= ColourValue::Black;
      mAmbLightColor	= ColourValue::White;

      for (tMapMovableEx::iterator it = mMOStatic.begin(); it != mMOStatic.end(); ++it)
        delete it->second;
      mMOStatic.clear();

      for (tMapMovableEx::iterator it = mMODynamic.begin(); it != mMODynamic.end(); ++it)
        delete it->second;
      mMODynamic.clear();

      for (tMapMovableEx::iterator it = mMOAll.begin(); it != mMOAll.end(); ++it)
        delete it->second;
      mMOAll.clear();
      // clean target scene nodes collection
      mSNTargets.clear();
      // other shit
      mNumTris = 0;
      mIsUpToDateWithScene = false;
      // clean the scene loading log
      mLoadLog.clear();
    }
    /***************************************************************************************************/
    void dotSceneInfo::_addMovable(Ogre::MovableObject *pMO, Ogre::Vector3 &pos, Ogre::Quaternion &rot, Ogre::Vector3 &scale, bool isStatic)
    {
      if (isStatic)
        mMOStatic.insert(tMapMovableEx::value_type(pMO->getName(), new types::MovableObjectEx(pMO, pos, rot, scale, isStatic)));
      else
        mMODynamic.insert(tMapMovableEx::value_type(pMO->getName(), new types::MovableObjectEx(pMO, pos, rot, scale, isStatic)));
      mMOAll.insert(tMapMovableEx::value_type(pMO->getName(), new types::MovableObjectEx(pMO, pos, rot, scale, isStatic)));
      if (pMO->getMovableType() == "Entity")
      {
        Ogre::MeshPtr pMesh = ((Ogre::Entity *)pMO)->getMesh();
        if (pMesh->sharedVertexData)
          mNumTris += pMesh->sharedVertexData->vertexCount;
        else
        {
          for (int n = 0; n < pMesh->getNumSubMeshes(); ++n)
            mNumTris += pMesh->getSubMesh(n)->vertexData->vertexCount;
        }
      }
    }
    /***************************************************************************************************/
    void dotSceneInfo::_addTargetSceneNode(Ogre::SceneNode *pSN)
    {
      mSNTargets.insert(tMapSceneNode::value_type(pSN->getName(), pSN));
    }
    /********************************************************
     * sky stuff												*
     ********************************************************/
    void dotSceneInfo::_setSkyBox(
        const Ogre::String &materialName, 
        float distance, 
        const Ogre::Quaternion &orientation, 
        bool drawFirst)
    {
      mBox.used = true;
      mDome.used = false;
      mPlane.used = false;
      mBox.drawFirst = drawFirst;
      mBox.material = materialName;
      mBox.distance = distance;
      mBox.orientation = orientation;
    }
    /***************************************************************************************************/
    void dotSceneInfo::_setSkyDome(
        const Ogre::String &materialName, 
        float curvature, 
        float tiling, 
        float distance, 
        const Ogre::Quaternion &orientation, 
        bool drawFirst)
    {
      mBox.used = false;
      mDome.used = true;
      mPlane.used = false;
      mDome.drawFirst = drawFirst;
      mDome.material = materialName;
      mDome.curvature = curvature;
      mDome.tiling = tiling;
      mDome.distance = distance;
      mDome.orientation = orientation;
    }
    /***************************************************************************************************/
    void dotSceneInfo::_setSkyPlane(
        const Ogre::String &materialName, 
        float planeX, 
        float planeY, 
        float planeZ, 
        float planeD, 
        float scale, 
        float tiling, 
        float bow, 
        bool drawFirst)
    {
      mBox.used = false;
      mDome.used = false;
      mPlane.used = true;
      mPlane.drawFirst = drawFirst;
      mPlane.material = materialName;
      mPlane.planeX = planeX;
      mPlane.planeY = planeY;
      mPlane.planeZ = planeZ;
      mPlane.planeD = planeD;
      mPlane.tiling = tiling;
      mPlane.scale = scale;
      mPlane.bow = bow;
    }
    /***************************************************************************************************/
    void dotSceneInfo::_setFog(
        Ogre::FogMode mode, 
        Ogre::ColourValue color, 
        float expDensity, 
        float linearStart, 
        float linearEnd)
    {
      mFog.color = color;
      mFog.expDensity = expDensity;
      mFog.linearEnd = linearEnd;
      mFog.linearStart = linearStart;
      mFog.mode = mode;
    }
    /***************************************************************************************************/
    void dotSceneInfo::_logLoadError(const Ogre::String &txt)
    {
      mLoadLog.push_back("dsi::ERROR->" + txt);
      Ogre::LogManager::getSingletonPtr()->logMessage(Ogre::LML_CRITICAL, String("dsi::ERROR->" + txt));
    }
    /***************************************************************************************************/
    void dotSceneInfo::_logLoadWarning(const Ogre::String &txt)
    {
      mLoadLog.push_back("dsi::WARNING->" + txt);
      Ogre::LogManager::getSingletonPtr()->logMessage(Ogre::LML_NORMAL, String("dsi::WARNING->" + txt));
    }
    /***************************************************************************************************/
    void dotSceneInfo::_logLoadInfo(const Ogre::String &txt)
    {	
      mLoadLog.push_back("dsi::INFO->" + txt);
      Ogre::LogManager::getSingletonPtr()->logMessage(Ogre::LML_TRIVIAL, String("dsi::INFO->" + txt));
    }
    /***************************************************************************************************/
    /// MovableObjectEx struct stuff
    /***************************************************************************************************/
    types::MovableObjectEx::MovableObjectEx(
        Ogre::MovableObject *pMO, 
        Ogre::Vector3 &pos, 
        Ogre::Quaternion &rot, 
        Ogre::Vector3 &sca, 
        bool isStat)
    {
      pMovable = pMO;
      position = pos;
      rotation = rot;
      scale = sca;
      isStatic = isStat;
    }
    /***************************************************************************************************/
  }// namespace dsi
  /*******************************************************************************************************/
}// namespace Ogre
/*******************************************************************************************************/
