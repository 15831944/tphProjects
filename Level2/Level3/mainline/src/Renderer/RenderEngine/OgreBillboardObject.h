#pragma once
#include <ogre.h>

#include <ogrefont.h>
namespace Ogre
{
	class BillboardObject;
	class BillBoardProxyRenderable;
	class BillboardTextRenderable;
	class BillBoardRenderable : public Renderable
	{
	public:
		BillBoardRenderable(BillboardObject* pParent);
		// Renderable overrides		
		/** @copydoc Renderable::getWorldTransforms. */
		void getWorldTransforms(Matrix4* xform) const;
		/** @copydoc Renderable::getSquaredViewDepth. */
		Real getSquaredViewDepth(const Camera *cam) const {return 0;};
	
		BillBoardProxyRenderable* ToProxyRenderable(){ return NULL; }
		BillboardTextRenderable* ToTextRenderable(){ return NULL; }
		
	protected:
		BillboardObject* mpParent;
		bool mbUseOrignMaterial;
	};
	//a wrapper of other renderable
	class BillBoardProxyRenderable : public BillBoardRenderable
	{
	public:
		BillBoardProxyRenderable(Renderable* other, BillboardObject* pParent)
			:BillBoardRenderable(pParent){
				otherRenderable = other;
			}

		BillBoardProxyRenderable* ToProxyRenderable(){ return this; }
		void getRenderOperation(RenderOperation& op){ return otherRenderable->getRenderOperation(op); }
		virtual const MaterialPtr& getMaterial(void) const { return otherRenderable->getMaterial(); }
		virtual const LightList& getLights(void) const { return otherRenderable->getLights(); }
	protected:
		Renderable* otherRenderable;
	};


	//billboard object
	class BillboardObject : public MovableObject
	{
		friend class BillBoardRenderable;
	public:
		typedef std::vector<BillBoardRenderable*> BillBoardRenderableList;

		BillboardObject(String name);
		virtual ~BillboardObject(void){ Clear(); }

		virtual const String& getMovableType( void ) const;


		void RefOtherRenderable(Renderable* pRender);
		void Clear();
		void SetText(const String& sCap, const ColourValue& color);

		BillboardTextRenderable* GetText();
	protected:
		void _getWorldTransforms(Matrix4 *xform) const;
		void  _notifyCurrentCamera(Camera *cam);
		


		//override movable object
		Real    getBoundingRadius(void) const{ return mBoundRadius; }
		const   AxisAlignedBox    &getBoundingBox(void) const{ return mAABB; }
		void   _updateRenderQueue(RenderQueue* queue);
		virtual void visitRenderables(Renderable::Visitor* visitor, 
			bool debugRenderables = false);

		BillBoardRenderableList mvRenderer;
		Camera* mpCam;
		Real mBoundRadius;
		AxisAlignedBox	mAABB;
		Vector2 mAdditionOffset;
		bool mUsePixelSize;
	};


	



	//the render text content
	class BillboardTextRenderable : public BillBoardRenderable
	{
	public:
		enum HorizontalAlignment    {H_LEFT, H_CENTER, H_RIGHT,}; 
		enum VerticalAlignment      {V_BELOW, V_ABOVE, V_CENTER};
		BillboardTextRenderable(BillboardObject* pParent);

		~BillboardTextRenderable();
	
		BillboardTextRenderable* ToTextRenderable(){ return this; }
	public:
		Real			mCharHeight;
		Real			mSpaceWidth;
		ColourValue		mColor;

		///String			mFontName;		
		///String			mName;
		String			mCaption;
		HorizontalAlignment	mHorizontalAlignment;
		VerticalAlignment	mVerticalAlignment;

		RenderOperation	mRenderOp;

		MaterialPtr	mpMaterial;

		FontPtr mpFont;
	public:
		void    setFontName(const String &fontName);
		void    setCaption(const String &caption);
		void    setColor(const ColourValue &color);
		void    setCharacterHeight(Real height);
		void    setSpaceWidth(Real width);
		void    setTextAlignment(const HorizontalAlignment& horizontalAlignment, const VerticalAlignment& verticalAlignment);


		const   String          &getFontName() const;
		const   String          &getCaption() const {return mCaption;}
		const   ColourValue     &getColor() const {return mColor;}

		Real    getCharacterHeight() const {return mCharHeight;}
		Real    getSpaceWidth() const {return mSpaceWidth;}


		void    getRenderOperation(RenderOperation &op);
		const   MaterialPtr       &getMaterial(void) const { return mpMaterial;};

	public:
		//from renderable
		const LightList& getLights(void) const{ return mpParent->queryLights(); } 
		bool			mNeedUpdate;
		bool			mUpdateColors;

	protected:
		// from MovableText, create the object
		void	_setupGeometry();
		void	_updateColors();

	public:
		void Update();

	};


	//factory
	class BillboardObjectFactory : public Ogre::MovableObjectFactory
	{
	public:
		virtual Ogre::BillboardObject* createInstanceImpl(
			const Ogre::String& name, const Ogre::NameValuePairList* params = 0)
		{
			return new Ogre::BillboardObject(name);
		}

		static String FACTORY_TYPE_NAME;

		virtual const Ogre::String& getType(void) const;
		virtual void destroyInstance(Ogre::MovableObject* obj)
		{
			delete (Ogre::BillboardObject*)obj;

		}
	};
}

