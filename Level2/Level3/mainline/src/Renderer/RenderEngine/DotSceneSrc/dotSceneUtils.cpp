#include "stdafx.h"
/***************************************************************************************************/
/***************************************************************************************************/
#include "dotSceneUtils.h"
#include <iostream>
#include <iomanip>
/***************************************************************************************************/
namespace Ogre
{
	/***************************************************************************************************/
	namespace dsi
	{
		/***************************************************************************************************/
		namespace utils
		{
			/***************************************************************************************************/
			Ogre::String getFileTitle(const Ogre::String &fileName)
			{
				Ogre::String outFileTitle = "";
				Ogre::String::size_type pos = fileName.find_last_of("\\");
				Ogre::String::size_type nopos = Ogre::String::npos;
				Ogre::String::size_type len = fileName.length();
				if (pos != nopos)
					outFileTitle = fileName.substr(pos + 1, len - pos - 1);
				else
					outFileTitle = fileName;
				return outFileTitle;
			}
			/***********************************************************************************************************/
		} // namespace utils
		/*******************************************************************************************************/
		namespace xml
		{
			/*******************************************************************************************************/
			Ogre::ColourValue readColor(const TiXmlElement *clrElem)
			{
				if (!clrElem)
					return ColourValue::White;
				Real r,g,b,a;
				r = StringConverter::parseReal(clrElem->Attribute("r"));
				g = StringConverter::parseReal(clrElem->Attribute("g"));
				b = StringConverter::parseReal(clrElem->Attribute("b"));
				a = clrElem->Attribute("a") != NULL ? StringConverter::parseReal(clrElem->Attribute("a")) : 1;
				return ColourValue(r,g,b,a);
			}
			/*******************************************************************************************************/
			Ogre::Vector3 readVector3(const TiXmlElement *vecElem)
			{
				if (!vecElem)
					return Vector3::ZERO;

				Vector3 position;
				position.x = xml::parseReal(vecElem->Attribute("x"),7);
				//position.x = StringConverter::parseReal(vecElem->Attribute("x"));
				position.y = xml::parseReal(vecElem->Attribute("y"),7);
				//position.y = StringConverter::parseReal(vecElem->Attribute("y"));
				position.z = xml::parseReal(vecElem->Attribute("z"),7);
				//position.z = StringConverter::parseReal(vecElem->Attribute("z"));
				return position;
			}

			/*******************************************************************************************************/
			Ogre::Vector2 readVector2(const TiXmlElement *vecElem)
			{
				if (!vecElem)
					return Vector2::ZERO;

				Vector2 position;
				position.x = xml::parseReal(vecElem->Attribute("x"),7);
				position.y = xml::parseReal(vecElem->Attribute("y"),7);
				return position;
			}
	/*******************************************************************************************************/
      Ogre::Real parseReal(const String& val, int precision)
      {
        // Use istringstream for direct correspondence with toString
        std::istringstream str(val);
        Real ret = 0;
        str >> std::setprecision ( precision) >> ret;

        return ret;
      }

			/*******************************************************************************************************/
			Ogre::Quaternion readQuaternion(const TiXmlElement *orElem)
			{
				if (!orElem)
					return Quaternion::IDENTITY;

				Quaternion orientation;
				if ( orElem->Attribute( "qx" ) )
				{
					orientation.x = StringConverter::parseReal(orElem->Attribute("qx"));
					orientation.y = StringConverter::parseReal(orElem->Attribute("qy"));
					orientation.z = StringConverter::parseReal(orElem->Attribute("qz"));
					orientation.w = StringConverter::parseReal(orElem->Attribute("qw"));
				}
				else if ( orElem->Attribute( "axisx" ) )
				{
					Vector3 axis;
					axis.x = StringConverter::parseReal(orElem->Attribute("axisx"));
					axis.y = StringConverter::parseReal(orElem->Attribute("axisy"));
					axis.z = StringConverter::parseReal(orElem->Attribute("axisz"));
					orientation.FromAxes(&axis);
				}
				else if ( orElem->Attribute( "anglex" ) )
				{
					Vector3 axis;
					axis.x = StringConverter::parseReal(orElem->Attribute("anglex"));
					axis.y = StringConverter::parseReal(orElem->Attribute("angley"));
					axis.z = StringConverter::parseReal(orElem->Attribute("anglez"));
					Real angle = StringConverter::parseReal(orElem->Attribute("angle"));;
					orientation.FromAngleAxis(Ogre::Angle(angle), axis);
				}
				else if ( orElem->Attribute( "x" ) )
				{
					Matrix3 matrix ;
					Real x = StringConverter::parseReal(orElem->Attribute("x"));
					Real y = StringConverter::parseReal(orElem->Attribute("y"));
					Real z = StringConverter::parseReal(orElem->Attribute("z"));
					matrix.FromEulerAnglesXYZ( Radian( x ), Radian( y ) , Radian( z ) ) ;
					orientation.FromRotationMatrix( matrix ) ;
				}

				return orientation;
			}
			/*******************************************************************************************************/
			Ogre::String readString(const TiXmlElement *pElem)
			{
				if (!pElem)
					return "";
				return pElem->Attribute("value");
			}
			/*******************************************************************************************************/
			int readInt(const TiXmlElement *pElem)
			{
				if (!pElem)
					return -1;
				return Ogre::StringConverter::parseInt(pElem->Attribute("value"));
			}
			/*******************************************************************************************************/
			Ogre::Vector3 readScale(const TiXmlElement *scElem)
			{
				if (!scElem)
					return Vector3::UNIT_SCALE;

				Vector3 position;
				position.x = StringConverter::parseReal(scElem->Attribute("x"));
				position.y = StringConverter::parseReal(scElem->Attribute("y"));
				position.z = StringConverter::parseReal(scElem->Attribute("z"));
				return position;
			}
			/*******************************************************************************************************/
			Ogre::Vector3 readPosition(const TiXmlElement *posElem)
			{
				return xml::readVector3(posElem);
			}
			/*******************************************************************************************************/
			Ogre::Quaternion readOrientation(const TiXmlElement *orElem)
			{
				return xml::readQuaternion(orElem);
			}
			/*******************************************************************************************************/
			Ogre::Vector3 readNormal(const TiXmlElement *nElem)
			{
				return xml::readVector3(nElem);
			}
			/*******************************************************************************************************/
			// NAMED WRITE METHODS
			/*******************************************************************************************************/
			void writeNamedValue(TiXmlElement *pParentElem, const Ogre::String &name, const Ogre::String &value)
			{
				TiXmlElement *pElem = pParentElem->InsertEndChild(TiXmlElement(name))->ToElement();
				pElem->SetAttribute("value", value);
			}
			/*******************************************************************************************************/
			void writeNamedValue(TiXmlElement *pParentElem, const Ogre::String &name, int value)
			{
				TiXmlElement *pElem = pParentElem->InsertEndChild(TiXmlElement(name))->ToElement();
				pElem->SetAttribute("value", Ogre::StringConverter::toString(value));
			}
			/*******************************************************************************************************/
			void writeNamedValue(TiXmlElement *pParentElem, const Ogre::String &name, float value)
			{
				TiXmlElement *pElem = pParentElem->InsertEndChild(TiXmlElement(name))->ToElement();
				pElem->SetAttribute("value", Ogre::StringConverter::toString(value));
			}
			/*******************************************************************************************************/
			void writeNamedValue(TiXmlElement *pParentElem, const Ogre::String &name, double value)
			{
				TiXmlElement *pElem = pParentElem->InsertEndChild(TiXmlElement(name))->ToElement();
				pElem->SetAttribute("value", Ogre::StringConverter::toString((Ogre::Real)value));
			}
			/*******************************************************************************************************/
			void writeNamedValue(TiXmlElement *pParentElem, const Ogre::String &name, const Ogre::Matrix3 &value)
			{
				TiXmlElement *pElem = pParentElem->InsertEndChild(TiXmlElement(name))->ToElement();
				pElem->SetAttribute("11", Ogre::StringConverter::toString(value[0][0]));
				pElem->SetAttribute("12", Ogre::StringConverter::toString(value[0][1]));
				pElem->SetAttribute("13", Ogre::StringConverter::toString(value[0][2]));
				pElem->SetAttribute("21", Ogre::StringConverter::toString(value[1][0]));
				pElem->SetAttribute("22", Ogre::StringConverter::toString(value[1][1]));
				pElem->SetAttribute("23", Ogre::StringConverter::toString(value[1][2]));
				pElem->SetAttribute("31", Ogre::StringConverter::toString(value[2][0]));
				pElem->SetAttribute("32", Ogre::StringConverter::toString(value[2][1]));
				pElem->SetAttribute("33", Ogre::StringConverter::toString(value[2][2]));
			}
			/*******************************************************************************************************/
			void writeNamedValue(TiXmlElement *pParentElem, const Ogre::String &name, const Ogre::Matrix4 &value)
			{
				TiXmlElement *pElem = pParentElem->InsertEndChild(TiXmlElement(name))->ToElement();
				pElem->SetAttribute("11", Ogre::StringConverter::toString(value[0][0]));
				pElem->SetAttribute("12", Ogre::StringConverter::toString(value[0][1]));
				pElem->SetAttribute("13", Ogre::StringConverter::toString(value[0][2]));
				pElem->SetAttribute("14", Ogre::StringConverter::toString(value[0][3]));
				pElem->SetAttribute("21", Ogre::StringConverter::toString(value[1][0]));
				pElem->SetAttribute("22", Ogre::StringConverter::toString(value[1][1]));
				pElem->SetAttribute("23", Ogre::StringConverter::toString(value[1][2]));
				pElem->SetAttribute("24", Ogre::StringConverter::toString(value[1][3]));
				pElem->SetAttribute("31", Ogre::StringConverter::toString(value[2][0]));
				pElem->SetAttribute("32", Ogre::StringConverter::toString(value[2][1]));
				pElem->SetAttribute("33", Ogre::StringConverter::toString(value[2][2]));
				pElem->SetAttribute("34", Ogre::StringConverter::toString(value[2][3]));
				pElem->SetAttribute("41", Ogre::StringConverter::toString(value[3][0]));
				pElem->SetAttribute("42", Ogre::StringConverter::toString(value[3][1]));
				pElem->SetAttribute("43", Ogre::StringConverter::toString(value[3][2]));
				pElem->SetAttribute("44", Ogre::StringConverter::toString(value[3][3]));
			}
			/*******************************************************************************************************/
			void writeNamedValue(TiXmlElement *pParentElem, const Ogre::String &name, const Ogre::Vector3 &value)
			{
				TiXmlElement *pElem = pParentElem->InsertEndChild(TiXmlElement(name))->ToElement();
				pElem->SetAttribute("x", Ogre::StringConverter::toString(value.x));
				pElem->SetAttribute("y", Ogre::StringConverter::toString(value.y));
				pElem->SetAttribute("z", Ogre::StringConverter::toString(value.z));
			}
			/*******************************************************************************************************/
			void writeNamedValue(TiXmlElement *pParentElem, const Ogre::String &name, const Ogre::Vector4 &value)
			{
				TiXmlElement *pElem = pParentElem->InsertEndChild(TiXmlElement(name))->ToElement();
				pElem->SetAttribute("x", Ogre::StringConverter::toString(value.x));
				pElem->SetAttribute("y", Ogre::StringConverter::toString(value.y));
				pElem->SetAttribute("z", Ogre::StringConverter::toString(value.z));
				pElem->SetAttribute("w", Ogre::StringConverter::toString(value.w));
			}
			/*******************************************************************************************************/
			void writeNamedValue(TiXmlElement *pParentElem, const Ogre::String &name, const Ogre::ColourValue &value)
			{
				TiXmlElement *pElem = pParentElem->InsertEndChild(TiXmlElement(name))->ToElement();
				pElem->SetAttribute("r", Ogre::StringConverter::toString(value.r));
				pElem->SetAttribute("g", Ogre::StringConverter::toString(value.g));
				pElem->SetAttribute("b", Ogre::StringConverter::toString(value.b));
				pElem->SetAttribute("a", Ogre::StringConverter::toString(value.a));
			}
			/*******************************************************************************************************/
			void writeNamedValue(TiXmlElement *pParentElem, const Ogre::String &name, const Ogre::Quaternion &value)
			{
				TiXmlElement *pElem = pParentElem->InsertEndChild(TiXmlElement(name))->ToElement();
				pElem->SetAttribute("qx", Ogre::StringConverter::toString(value.x));
				pElem->SetAttribute("qy", Ogre::StringConverter::toString(value.y));
				pElem->SetAttribute("qz", Ogre::StringConverter::toString(value.z));
				pElem->SetAttribute("qw", Ogre::StringConverter::toString(value.w));
			}
			/*******************************************************************************************************/
			void writeNamedValue(TiXmlElement *pParentElem, const Ogre::String &name, bool value)
			{
				TiXmlElement *pElem = pParentElem->InsertEndChild(TiXmlElement(name))->ToElement();
				pElem->SetAttribute("value", Ogre::StringConverter::toString(value));
			}
			/*******************************************************************************************************/
		} // namespace xml
		/*******************************************************************************************************/
	}// namespace dsi
	/*******************************************************************************************************/
}// namespace Ogre
/*******************************************************************************************************/
