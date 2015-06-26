#pragma once

/**
* File: MovableText.h
*
* description: This create create a billboarding object that display a text. can dynamic change.
* 
* @author  2003 by cTh see gavocanov@rambler.ru
* @update  2006 by barraq see nospam@barraquand.com
* 
*/
#include "OgreFont.h"
namespace Ogre 
{
	
	class TextRenderable;
	class Rect2DRenderable;

	class ManualText : public ManualObject
	{	
	public:
		enum TextType{
			TT_Texture,
			TT_OutLine,
			TT_Solid,
		};
		static const Ogre::String& GetTypeName();

	friend class ManualTextSection;
	/******************************** MovableText data ****************************/
	public:		
		enum HorizontalAlignment    {H_LEFT, H_CENTER, H_RIGHT,}; //decide which direction the char going to print
		enum VerticalAlignment      {V_TOP, V_CENTER, V_BOTTOM}; //decide which direction the line goint to print grow
		/******************************** public methods ******************************/
	public:
		ManualText(const String &name);
		virtual ~ManualText();

		void    setFaceCamera(bool b);
		void    showOnTop(bool show=true);

		/** Start defining a part of the object.
		@remarks
		Each time you call this method, you start a new section of the
		object with its own font and potentially its own type of
		rendering types (texture, outline).
		@param fontName The name of the font to render this part of the
		object with.
		@param opType The type of operation to use to render. 
		*/
		void begin(const String& fontName, TextType texType,const String& materialName );	
		/** Add a text string
		@remarks A vertex position is slightly special among the other vertex data
		methods like normal() and textureCoord(), since calling it indicates
		the start of a new vertex. All other vertex data methods you call 
		after this are assumed to be adding more information (like normals or
		texture coordinates) to the last vertex started with position().
		*/
		virtual void text(const String& str); //create a text 
		virtual void line(); //create a new line	

		
	protected:
		void prepareFont(const String& fontName);
		Ogre::FontPtr mCurrentFont;

	public:
		// Set settings
		void	setOffset(const Vector2& offset);
		void	setRotate(const Ogre::Angle& angle);
		bool    getShowOnTop() const {return mOnTop;}

		//set background
		void showBackground(bool bshow);


		AxisAlignedBox	mAABB;

		Camera* mpCam;
		// Get settings		
		

		Real    getBoundingRadius(void) const {return mRadius;};
		/******************************** protected methods and overload **************/
	protected:
		bool			mOnTop;
		bool			mShowBackgroud;
		bool			mFaceCamera;
		Real			mRadius;
		Vector2			mOffset;
		Quaternion      mRotate;
	public:	
		////inherit functions from MovableObject
		virtual const String& getMovableType( void ) const{ return GetTypeName(); }


		void    getWorldTransforms(Matrix4 *xform) const;
		const   Quaternion        &getWorldOrientation(void) const;
		const   Vector3           &getWorldPosition(void) const;
		Real    getSquaredViewDepth(const Camera *cam) const;		
		const   AxisAlignedBox    &getBoundingBox(void) const;		
		
		void    _notifyCurrentCamera(Camera *cam);
		void    _updateRenderQueue(RenderQueue* queue);
		
	};
	
	class ManualTextSection : public ManualObject::ManualObjectSection
	{
	public:
		ManualTextSection(ManualText* parent, const String& materialName,RenderOperation::OperationType opType);
		virtual ~ManualTextSection();
	public:		
	
	};
	
	
	
}
//factory
class ManualTextFactory : public Ogre::MovableObjectFactory
{
public:
	virtual Ogre::MovableObject* createInstanceImpl(
		const Ogre::String& name, const Ogre::NameValuePairList* params = 0)
	{
		return new Ogre::ManualText(name);
	}

	
	virtual const Ogre::String& getType(void) const{
		return Ogre::ManualText::GetTypeName();
	}
	virtual void destroyInstance(Ogre::MovableObject* obj)
	{
		delete (Ogre::ManualText*)obj;

	}
};
