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
	
	class ManualObjectExt : public ManualObject
	{	
	public:			
	friend class ManualObjectExtSection;
	/******************************** MovableText data ****************************/
	public:		
		//enum HorizontalAlignment    {H_LEFT, H_CENTER, H_RIGHT,}; //decide which direction the char going to print
		//enum VerticalAlignment      {V_TOP, V_CENTER, V_BOTTOM}; //decide which direction the line goint to print grow
		/******************************** public methods ******************************/
	public:
		ManualObjectExt(const String &name);
		virtual ~ManualObjectExt();		

		void beginText( const String& fontName = "ArialFont", bool bScreenText  = true);
		Real drawChar(Font::CodePoint c, Real charHeight, Real left, Real top );
		
		void setColor(const ColourValue& c){ mCurColor = c; }
		//override manual object
		void begin(const String& materialName,
			RenderOperation::OperationType opType = RenderOperation::OT_TRIANGLE_LIST,
			const String & groupName = ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

		//draw text in alignment
		void drawTopLeftAligmentText(const String& caption,Real charheight=24);

	protected:
		String perpareFont( const String& mName );

		Ogre::FontPtr mCurFont;
		ColourValue mCurColor;		
		
		//return width of the char		
		void getScreenWorldTransforms( Matrix4* xform ) const;

	public:
		// Set settings
		Camera* mpCam;
		

		/******************************** protected methods and overload **************/
	protected:
		//bool			mOnTop;
		//bool			mShowBackgroud;
		//bool			mFaceCamera;
		//Real			mRadius;
		//Vector2			mOffset;
		//Quaternion      mRotate;
	public:	
		////inherit functions from MovableObject
		virtual const String& getMovableType( void ) const;		
		
		void    _notifyCurrentCamera(Camera *cam);

			//////////////////////////////////////////////////////////////////////////
		class ManualObjectExtSection : public ManualObjectSection
		{
		public:
			ManualObjectExtSection(ManualObject* parent, const String& materialName,
				RenderOperation::OperationType opType, const String & groupName = ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME)
				:ManualObjectSection(parent,materialName,opType,groupName)
			{  }

			void getWorldTransforms(Matrix4* xform) const;
			

			bool mbScreenText;
		};		
	};
	

	//////////////////////////////////////////////////////////////////////////
	class ManualObjectExtFactory : public Ogre::MovableObjectFactory
	{
	public:
		virtual Ogre::MovableObject* createInstanceImpl(
			const Ogre::String& name, const Ogre::NameValuePairList* params = 0);

		static String FACTORY_TYPE_NAME;
		virtual const Ogre::String& getType(void) const;
		virtual void destroyInstance(Ogre::MovableObject* obj);
	};
	
	
}
//factory
