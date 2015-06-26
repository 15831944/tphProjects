/*
This source file is part of Rigs of Rods
Copyright 2005,2006,2007,2008,2009 Pierre-Michel Ricordel
Copyright 2007,2008,2009 Thomas Fischer

For more information, see http://www.rigsofrods.com/

Rigs of Rods is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License version 3, as 
published by the Free Software Foundation.

Rigs of Rods is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Rigs of Rods.  If not, see <http://www.gnu.org/licenses/>.
*/
#pragma once
#include <vector>
#include <ogre.h>
#include <OgreTextAreaOverlayElement.h>
#include <OgreOverlayElementFactory.h>

namespace Ogre
{

class ColoredTextAreaOverlayElement : public TextAreaOverlayElement
{
public:
	ColoredTextAreaOverlayElement(const Ogre::String& name);
	~ColoredTextAreaOverlayElement(void);

	//void setValueBottom(float Value);
	//void setValueTop(float Value);
	//void setCaption(const Ogre::DisplayString& text);
	//static Ogre::DisplayString StripColors(const Ogre::String& text);
	//static Ogre::ColourValue GetColor(unsigned char ID, float Value = 1.0f);

	//void updateColours(void);

	void getWorldTransforms(Matrix4* xform) const;

	const String& getTypeName( void ) const;

	void SetTransf(const Vector3& transf);
	Vector3 mTransf;
protected:
	std::vector < unsigned char > m_Colors;
	float m_ValueTop;
	float m_ValueBottom;
};


class ColoredTextAreaOverlayElementFactory : public Ogre::OverlayElementFactory
{
public:
	/** See OverlayElementFactory */
	OverlayElement* createOverlayElement(const String& instanceName)
	{
		return new ColoredTextAreaOverlayElement(instanceName);
	}
	
	/** See OverlayElementFactory */
	const String& getTypeName() const;
	static String FACTORY_TYPE_NAME; 
};

}