#include "StdAfx.h"
#include "ManualObjectExt.h"
#include "OgreFontManager.h"
#include "Common/strutil.h"

#define UNICODE_NEL 0x0085
#define UNICODE_CR 0x000D
#define UNICODE_LF 0x000A
#define UNICODE_SPACE 0x0020
#define UNICODE_ZERO 0x0030




using namespace Ogre;

ManualObjectExt::ManualObjectExt(const String &name) : ManualObject(name)
, mpCam(NULL)
//, mOnTop(false)
{	
	//mShowBackgroud = false;
	//mFaceCamera = true;
	
}

ManualObjectExt::~ManualObjectExt()
{
}


void ManualObjectExt::_notifyCurrentCamera(Camera *cam)
{
	ManualObject::_notifyCurrentCamera(cam);
	mpCam = cam;
}



const String& Ogre::ManualObjectExt::getMovableType( void ) const
{
	return ManualObjectExtFactory::FACTORY_TYPE_NAME;
}

void Ogre::ManualObjectExt::begin( const String& materialName, RenderOperation::OperationType opType, const String & groupName )
{
	if (mCurrentSection)
	{
		OGRE_EXCEPT(Exception::ERR_INVALIDPARAMS,
			"You cannot call begin() again until after you call end()",
			"ManualObject::begin");
	}
	mCurrentSection = OGRE_NEW ManualObjectExtSection(this, materialName, opType, groupName);
	mCurrentUpdating = false;
	mCurrentSection->setUseIdentityProjection(mUseIdentityProjection);
	mCurrentSection->setUseIdentityView(mUseIdentityView);
	mSectionList.push_back(mCurrentSection);
	mFirstVertex = true;
	mDeclSize = 0;
	mTexCoordIndex = 0;
}

void Ogre::ManualObjectExt::beginText( const String& fontName,bool bScreenText )
{
	String fontMat = perpareFont(fontName);
	begin(fontMat);

	ManualObjectExtSection* pTextSection = (ManualObjectExtSection*)mCurrentSection;
	pTextSection->mbScreenText = bScreenText;
	if(bScreenText)
	{
		mCurrentSection->setUseIdentityView(true);
		mCurrentSection->setUseIdentityProjection(true);
	}

}


Real Ogre::ManualObjectExt::drawChar( Font::CodePoint c ,Real mCharHeight ,Real left, Real top )
{
	Real width =0 ;
	if(c == UNICODE_SPACE)
	{
		width =  mCurFont->getGlyphAspectRatio(UNICODE_ZERO) * mCharHeight;
		return width;
	}

	width = mCurFont->getGlyphAspectRatio(c)*mCharHeight  ;
	const Font::UVRect& uvRect = mCurFont->getGlyphTexCoords(c);

	Real bottom = top - mCharHeight;
	Real right  = left + width;
	//first tri
	//upper left
	this->position(left,top,0);
	this->textureCoord(uvRect.left,uvRect.top);
	this->colour(mCurColor);
	this->normal(Vector3::UNIT_Z);
	// bottom left	
	this->position(left,bottom,0);
	this->textureCoord(uvRect.left,uvRect.bottom);
	//top right
	this->position(right,top,0);
	this->textureCoord(uvRect.right,uvRect.top);	
	//second tri
	//top right
	this->position(right,top,0);
	this->textureCoord(uvRect.right,uvRect.top);
	//bottom left
	this->position(left,bottom,0);
	this->textureCoord(uvRect.left,uvRect.bottom);
	// bottom right	
	this->position(right,bottom,0);
	this->textureCoord(uvRect.right,uvRect.bottom);

	//move to next
	return width;
}

void Ogre::ManualObjectExt::getScreenWorldTransforms( Matrix4* xform ) const
{	
	if(mpCam)
	{
		Vector3 ppos = getParentNode()->_getDerivedPosition();
		if(Ogre::Viewport* vp = mpCam->getViewport() )
		{			
			Vector3 eyeSpacePos = mpCam->getViewMatrix() * ppos;
			eyeSpacePos.z +=100;
			// z < 0 means in front of cam
			if (eyeSpacePos.z < 0)
			{
				// calculate projected pos
				Vector3 screenSpacePos = mpCam->getProjectionMatrix() * (eyeSpacePos);	
				Matrix4 mat = Matrix4::getTrans(screenSpacePos);
				mat.setScale( Vector3(1.0 /vp->getActualWidth(), 1.0/vp->getActualHeight() ,1) );
				*xform = mat;
				return;					
			}
		}
	}	
	
	*xform = Matrix4::IDENTITY;
}

void Ogre::ManualObjectExt::drawTopLeftAligmentText( const String& caption , Real mCharHeight)
{
	//get line number
	strutil::Tokenizer tokenizer(caption,"\r\n");
	int nlineNumber = 0;
	while(tokenizer.nextToken())
	{
		nlineNumber++;
	}
	Real left = 0;
	Real top =  nlineNumber * mCharHeight;
	
	tokenizer.reset();
	while(tokenizer.nextToken())
	{
		//left += drawChar(UNICODE_SPACE,mCharHeight,left,top);
		String str = tokenizer.getToken();

		for(String::const_iterator i = str.begin();i!=str.end();++i)
		{
			Font::CodePoint character  = *i;
			left +=  drawChar(character,mCharHeight, left,top);			
		}
		left = 0;
		top -= mCharHeight; 
	}	
}




String ManualObjectExt::perpareFont( const String& mName )
{
	FontPtr pFont = FontManager::getSingleton().getByName( mName );
	if (pFont.isNull())
	{
		return ("");
	}
	mCurFont = pFont;
	mCurFont->load();


	String fontMatname =  mName + "Material";
	if(!(MaterialManager::getSingletonPtr()->resourceExists(fontMatname))) 
	{ 
		MaterialPtr mpMaterial = mCurFont->getMaterial()->clone(fontMatname);
		mpMaterial->setDepthCheckEnabled(true);
		mpMaterial->setDepthBias(1.0,1.0);
		mpMaterial->setDepthWriteEnabled(false);
		mpMaterial->setLightingEnabled(false);	
		bool b = mpMaterial->isTransparent();
		ASSERT(b);
	}
	return fontMatname;
}

//////////////////////////////////////////////////////////////////////////

void ManualObjectExtFactory::destroyInstance( Ogre::MovableObject* obj )
{
	delete (Ogre::ManualObjectExt*)obj;
}

Ogre::MovableObject* ManualObjectExtFactory::createInstanceImpl( const Ogre::String& name, const Ogre::NameValuePairList* params /*= 0*/ )
{
	return new Ogre::ManualObjectExt(name);
}

String ManualObjectExtFactory::FACTORY_TYPE_NAME= "ManualObjectExt";

const Ogre::String& Ogre::ManualObjectExtFactory::getType( void ) const
{
	return FACTORY_TYPE_NAME;
}


void Ogre::ManualObjectExt::ManualObjectExtSection::getWorldTransforms( Matrix4* xform ) const
{
	ManualObjectExt* parent = (ManualObjectExt*)mParent;
	if(mbScreenText)
	{
		ASSERT(getUseIdentityView() && getUseIdentityProjection());
		return parent->getScreenWorldTransforms(xform);
	}
	return __super::getWorldTransforms(xform);
}
