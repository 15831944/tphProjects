#include "Common\tinyxml\tinyxml.h"
/***************************************************************************************************/
#ifndef INC_DOTSCENEUTILS_H
#define INC_DOTSCENEUTILS_H
/***************************************************************************************************/
/***************************************************************************************************/
namespace Ogre
{
	/***************************************************************************************************************/
	namespace dsi
	{
		/***************************************************************************************************************/
		namespace utils
		{
			/// retrieve the file name only component from a file path string
			Ogre::String getFileTitle(const Ogre::String &fileName);
		}// namespace utils
		/***************************************************************************************************************/
		namespace xml
		{
			// dotScene format compounds
			Ogre::ColourValue		readColor		(const TiXmlElement *pElem);
			Ogre::Vector3			readScale		(const TiXmlElement *pElem);
			Ogre::Vector3			readPosition	(const TiXmlElement *pElem);
			Ogre::Vector3			readVector3	(const TiXmlElement *pElem);
			Ogre::Vector2			readVector2	(const TiXmlElement *pElem);
      Ogre::Real       parseReal(const Ogre::String& val, int precision);
      Ogre::Quaternion		readOrientation	(const TiXmlElement *pElem);
			Ogre::Vector3			readNormal		(const TiXmlElement *pElem);
			// named value write methods 
			// (THOSE CREATE NAMED nodes)
			void	writeNamedValue	(TiXmlElement *pParentElem, const Ogre::String &name, const Ogre::String &value);
			void	writeNamedValue	(TiXmlElement *pParentElem, const Ogre::String &name, int value);
			void	writeNamedValue	(TiXmlElement *pParentElem, const Ogre::String &name, float value);
			void	writeNamedValue	(TiXmlElement *pParentElem, const Ogre::String &name, double value);
			void	writeNamedValue	(TiXmlElement *pParentElem, const Ogre::String &name, bool value);
			void	writeNamedValue	(TiXmlElement *pParentElem, const Ogre::String &name, const Ogre::Matrix3 &value);
			void	writeNamedValue	(TiXmlElement *pParentElem, const Ogre::String &name, const Ogre::Matrix4 &value);
			void	writeNamedValue	(TiXmlElement *pParentElem, const Ogre::String &name, const Ogre::Vector3 &value);
			void	writeNamedValue	(TiXmlElement *pParentElem, const Ogre::String &name, const Ogre::Vector4 &value);
			void	writeNamedValue	(TiXmlElement *pParentElem, const Ogre::String &name, const Ogre::ColourValue &value);
			void	writeNamedValue	(TiXmlElement *pParentElem, const Ogre::String &name, const Ogre::Quaternion &value);
		}// namespace xml
		/***************************************************************************************************************/
	} // namespace dsi
	/***************************************************************************************************/
} // namespace Ogre
/***************************************************************************************************/
#endif // #ifndef INC_DOTSCENEUTILS_H
/***************************************************************************************************/
