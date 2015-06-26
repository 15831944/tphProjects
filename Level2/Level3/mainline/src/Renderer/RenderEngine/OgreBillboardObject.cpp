#include "StdAfx.h"
#include ".\ogrebillboardobject.h"
#include <OgreFontManager.h>
#define POS_TEX_BINDING    0
#define COLOUR_BINDING     1

using namespace Ogre;




void BillBoardRenderable::getWorldTransforms(Matrix4* xform) const
{
	return mpParent->_getWorldTransforms(xform);
}

void Ogre::BillboardObject::_getWorldTransforms( Matrix4 *xform ) const
{
	if (mpCam && mParentNode)
	{		
		Vector3 ppos = mParentNode->_getDerivedPosition();
		if(mpCam && mpCam->getViewport() )
		{
			Ogre::Viewport* vp = mpCam->getViewport();
			Vector3 eyeSpacePos = mpCam->getViewMatrix() * ppos;
			// z < 0 means in front of cam
			if (eyeSpacePos.z < 0)
			{
				// calculate projected pos
				Vector3 screenSpacePos = mpCam->getProjectionMatrix() * eyeSpacePos;	
				Matrix4 mat = Matrix4::getTrans(screenSpacePos);
				mat.setScale( Vector3(2.0f /vp->getActualWidth(), 2.0f/vp->getActualHeight() ,1) );
				*xform = mat;
				return;					
			}
		}
	}

	*xform = Matrix4::IDENTITY;
}
//
Ogre::String Ogre::BillboardObjectFactory::FACTORY_TYPE_NAME =  _T("Billboard Object");

const String& Ogre::BillboardObject::getMovableType( void ) const
{
	return  Ogre::BillboardObjectFactory::FACTORY_TYPE_NAME;
}

void Ogre::BillboardObject::_updateRenderQueue( RenderQueue* queue )
{
	if (this->isVisible())
	{
		std::vector<BillBoardRenderable*>::iterator itr = mvRenderer.begin();
		for(;itr!=mvRenderer.end();++itr)
		{
			queue->addRenderable( *itr, mRenderQueueID, OGRE_RENDERABLE_DEFAULT_PRIORITY);		
		}		
	}
}

void Ogre::BillboardObject::_notifyCurrentCamera( Camera *cam )
{
	MovableObject::_notifyCurrentCamera(cam);
	mpCam = cam;
}

void Ogre::BillboardObject::Clear()
{
	for(BillBoardRenderableList::iterator i=mvRenderer.end();i!=mvRenderer.end();i++)
	{
		delete *i;
	}
	mvRenderer.clear();
}

void Ogre::BillboardObject::visitRenderables( Renderable::Visitor* visitor, bool debugRenderables /*= false*/ )
{
	for (BillBoardRenderableList::iterator i = mvRenderer.begin(); i != mvRenderer.end(); ++i)
	{
		visitor->visit(*i, 0, false);
	}
}

void Ogre::BillboardObject::RefOtherRenderable( Renderable* pRender )
{
	mvRenderer.push_back(new BillBoardProxyRenderable(pRender,this));
}

Ogre::BillboardObject::BillboardObject( String name ) :MovableObject(name)
{
	mAABB.setExtents(Vector3::ZERO,Vector3::UNIT_SCALE);
	mUsePixelSize = true;
}

BillboardTextRenderable* Ogre::BillboardObject::GetText()
{
	for (BillBoardRenderableList::iterator i = mvRenderer.begin(); i != mvRenderer.end(); ++i)
	{
		BillboardTextRenderable* pText = (*i)->ToTextRenderable();
		if(pText)
			return pText;
	}
	return NULL;
}

void Ogre::BillboardObject::SetText( const String& sCap, const ColourValue& color )
{
	BillboardTextRenderable* pText = GetText();
	if(!pText){
		pText = new BillboardTextRenderable(this);
		mvRenderer.push_back(pText);
	}
	pText->setCaption(sCap);
	pText->setColor(color);
}

const Ogre::String& Ogre::BillboardObjectFactory::getType( void ) const
{
	return FACTORY_TYPE_NAME;
}


Ogre::BillBoardRenderable::BillBoardRenderable( BillboardObject* pParent )
{
	mpParent = pParent;
	mUseIdentityProjection = true;
	mUseIdentityView   = true;
}

void Ogre::BillboardTextRenderable::setFontName( const String &fontName )
{	
	FontPtr pFont = FontManager::getSingleton().getByName( fontName );
	if (pFont.isNull())
	{
		return;
	}
	mpFont = pFont;
    mpFont->load();
	mpMaterial = mpFont->getMaterial();
	mNeedUpdate = true;
}

void Ogre::BillboardTextRenderable::setCaption( const String &caption )
{
	if(mCaption!=caption)
	{
		mCaption = caption;
		mNeedUpdate = true;
	}
}

Ogre::BillboardTextRenderable::BillboardTextRenderable( BillboardObject* pParent ) :BillBoardRenderable(pParent)
{
	mNeedUpdate = true;
	mUpdateColors = true;
	setFontName("ArialFont");
	setTextAlignment(H_LEFT,V_CENTER);
	setCharacterHeight(12);	
	setSpaceWidth(0);
	setColor(Ogre::ColourValue::Black);
}

Ogre::BillboardTextRenderable::~BillboardTextRenderable()
{
	if (mRenderOp.vertexData)
		delete mRenderOp.vertexData;
}

const   String& Ogre::BillboardTextRenderable::getFontName() const
{
	if(mpFont.isNull())
	{
		const static String nullName = _T("");
		return nullName;
	}
	return mpFont->getName();
}

void Ogre::BillboardTextRenderable::setColor( const ColourValue &color )
{
	if(mColor!=color)
	{
		mColor = color;
		mUpdateColors = true;
	}
}

void Ogre::BillboardTextRenderable::setCharacterHeight( Real height )
{
	if(mCharHeight!=height)
	{
		mCharHeight = height;
		mNeedUpdate = true;
	}
}

void Ogre::BillboardTextRenderable::setSpaceWidth( Real width )
{
	if(mSpaceWidth !=width)
	{
		mSpaceWidth = width;
		mNeedUpdate = true;
	}
}


void Ogre::BillboardTextRenderable::setTextAlignment( const HorizontalAlignment& horizontalAlignment, const VerticalAlignment& verticalAlignment )
{
	if(mVerticalAlignment!=verticalAlignment
	|| mHorizontalAlignment!=horizontalAlignment)
	{
		mVerticalAlignment = verticalAlignment;
		mHorizontalAlignment = horizontalAlignment;
		mNeedUpdate =true;
	}
}
void BillboardTextRenderable::_setupGeometry()
{
	assert(!mpFont.isNull());
	assert(!mpMaterial.isNull());

	uint vertexCount = static_cast<uint>(mCaption.size() * 6);

	if (mRenderOp.vertexData)
	{
		// Removed this test as it causes problems when replacing a caption
		// of the same size: replacing "Hello" with "hello"
		// as well as when changing the text alignment
		if (mRenderOp.vertexData->vertexCount != vertexCount)
		{
			delete mRenderOp.vertexData;
			mRenderOp.vertexData = NULL;
			mUpdateColors = true;
		}
	}

	if (!mRenderOp.vertexData)
		mRenderOp.vertexData = new VertexData();

	mRenderOp.indexData = 0;
	mRenderOp.vertexData->vertexStart = 0;
	mRenderOp.vertexData->vertexCount = vertexCount;
	mRenderOp.operationType = RenderOperation::OT_TRIANGLE_LIST; 
	mRenderOp.useIndexes = false; 

	VertexDeclaration  *decl = mRenderOp.vertexData->vertexDeclaration;
	VertexBufferBinding   *bind = mRenderOp.vertexData->vertexBufferBinding;
	size_t offset = 0;

	// create/bind positions/tex.ccord. buffer
	if (!decl->findElementBySemantic(VES_POSITION))
		decl->addElement(POS_TEX_BINDING, offset, VET_FLOAT3, VES_POSITION);

	offset += VertexElement::getTypeSize(VET_FLOAT3);

	if (!decl->findElementBySemantic(VES_TEXTURE_COORDINATES))
		decl->addElement(POS_TEX_BINDING, offset, Ogre::VET_FLOAT2, Ogre::VES_TEXTURE_COORDINATES, 0);

	HardwareVertexBufferSharedPtr ptbuf = HardwareBufferManager::getSingleton().createVertexBuffer(decl->getVertexSize(POS_TEX_BINDING),
		mRenderOp.vertexData->vertexCount,
		HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY);
	bind->setBinding(POS_TEX_BINDING, ptbuf);

	// Colours - store these in a separate buffer because they change less often
	if (!decl->findElementBySemantic(VES_DIFFUSE))
		decl->addElement(COLOUR_BINDING, 0, VET_COLOUR, VES_DIFFUSE);

	HardwareVertexBufferSharedPtr cbuf = HardwareBufferManager::getSingleton().createVertexBuffer(decl->getVertexSize(COLOUR_BINDING),
		mRenderOp.vertexData->vertexCount,
		HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY);
	bind->setBinding(COLOUR_BINDING, cbuf);

	_updateColors();

	size_t charlen = mCaption.size();
	float *pPCBuff = static_cast<float*>(ptbuf->lock(HardwareBuffer::HBL_DISCARD));

	Real largestWidth = 0;
	Real left = 0 * 2.0 - 1.0;
	Real top = -((0 * 2.0) - 1.0);

	// Derive space width from a capital A
	if (mSpaceWidth == 0)
		mSpaceWidth = mpFont->getGlyphAspectRatio('A') * mCharHeight * 2.0;

	// for calculation of AABB
	//Ogre::Vector3 min, max, 
	Ogre::Vector3 currPos;
	//Ogre::Real maxSquaredRadius = 0;
	bool first = true;

	// Use iterator
	String::iterator i, iend;
	iend = mCaption.end();
	bool newLine = true;
	Real len = 0.0f;

	Real verticalOffset = 0;
	switch (mVerticalAlignment)
	{
	case V_ABOVE:
		verticalOffset = mCharHeight;
		break;
	case V_CENTER:
		verticalOffset = 0.5*mCharHeight;
		break;
	case V_BELOW:
		verticalOffset = 0;
		break;
	}
	// Raise the first line of the caption
	top += verticalOffset;
	for (i = mCaption.begin(); i != iend; ++i)
	{
		if (*i == '\n')
			top += verticalOffset * 2.0f;
	}

	for (i = mCaption.begin(); i != iend; ++i)
	{
		if (newLine)
		{
			len = 0.0f;
			for (String::iterator j = i; j != iend && *j != '\n'; j++)
			{
				if (*j == ' ')
					len += mSpaceWidth;
				else 
					len += mpFont->getGlyphAspectRatio(*j) * mCharHeight * 2.0f;
			}
			newLine = false;
		}

		if (*i == '\n')
		{
			left = 0 * 2.0 - 1.0;
			top -= mCharHeight * 2.0;
			newLine = true;
			continue;
		}

		if (*i == ' ')
		{
			// Just leave a gap, no tris
			left += mSpaceWidth;
			// Also reduce tri count
			mRenderOp.vertexData->vertexCount -= 6;
			continue;
		}

		Real horiz_height = mpFont->getGlyphAspectRatio(*i);
		Real u1, u2, v1, v2; 
		Ogre::Font::UVRect utmp;
		utmp = mpFont->getGlyphTexCoords(*i);
		u1 = utmp.left;
		u2 = utmp.right;
		v1 = utmp.top;
		v2 = utmp.bottom;

		// each vert is (x, y, z, u, v)
		//-------------------------------------------------------------------------------------
		// First tri
		//
		// Upper left
		if(mHorizontalAlignment == H_LEFT)
			*pPCBuff++ = left;
		else
			*pPCBuff++ = left - (len / 2);
		*pPCBuff++ = top;
		*pPCBuff++ = -1.0;
		*pPCBuff++ = u1;
		*pPCBuff++ = v1;

		// Deal with bounds
		if(mHorizontalAlignment == H_LEFT)
			currPos = Ogre::Vector3(left, top, -1.0);
		else
			currPos = Ogre::Vector3(left - (len / 2), top, -1.0);
		//if (first)
		//{
		//	//min = max = currPos;
		//	//maxSquaredRadius = currPos.squaredLength();
		//	mAABB = AxisAlignedBox();
		//	mAABB.merge(currPos);
		//	first = false;
		//}
		//else
		//{
		//	mAABB.merge(currPos);
		//	//min.makeFloor(currPos);
		//	//max.makeCeil(currPos);
		//	//maxSquaredRadius = std::max(maxSquaredRadius, currPos.squaredLength());
		//}


		top -= mCharHeight * 2.0;

		// Bottom left
		if(mHorizontalAlignment == H_LEFT)
			*pPCBuff++ = left;
		else
			*pPCBuff++ = left - (len / 2);
		*pPCBuff++ = top;
		*pPCBuff++ = -1.0;
		*pPCBuff++ = u1;
		*pPCBuff++ = v2;

		// Deal with bounds
		if(mHorizontalAlignment == H_LEFT)
			currPos = Ogre::Vector3(left, top, -1.0);
		else
			currPos = Ogre::Vector3(left - (len / 2), top, -1.0);
		//min.makeFloor(currPos);
		//max.makeCeil(currPos);
		//maxSquaredRadius = std::max(maxSquaredRadius, currPos.squaredLength());
		//mAABB.merge(currPos);

		top += mCharHeight * 2.0;
		left += horiz_height * mCharHeight * 2.0;

		// Top right
		if(mHorizontalAlignment == H_LEFT)
			*pPCBuff++ = left;
		else
			*pPCBuff++ = left - (len / 2);
		*pPCBuff++ = top;
		*pPCBuff++ = -1.0;
		*pPCBuff++ = u2;
		*pPCBuff++ = v1;
		//-------------------------------------------------------------------------------------

		// Deal with bounds
		if(mHorizontalAlignment == H_LEFT)
			currPos = Ogre::Vector3(left, top, -1.0);
		else
			currPos = Ogre::Vector3(left - (len / 2), top, -1.0);
		//min.makeFloor(currPos);
		//max.makeCeil(currPos);
		//maxSquaredRadius = std::max(maxSquaredRadius, currPos.squaredLength());
		//mAABB.merge(currPos);

		//-------------------------------------------------------------------------------------
		// Second tri
		//
		// Top right (again)
		if(mHorizontalAlignment == H_LEFT)
			*pPCBuff++ = left;
		else
			*pPCBuff++ = left - (len / 2);
		*pPCBuff++ = top;
		*pPCBuff++ = -1.0;
		*pPCBuff++ = u2;
		*pPCBuff++ = v1;

		currPos = Ogre::Vector3(left, top, -1.0f);
		//min.makeFloor(currPos);
		//max.makeCeil(currPos);
		//maxSquaredRadius = std::max(maxSquaredRadius, currPos.squaredLength());
		//mAABB.merge(currPos);

		top -= mCharHeight * 2.0f;
		left -= horiz_height  * mCharHeight * 2.0f;

		// Bottom left (again)
		if(mHorizontalAlignment == H_LEFT)
			*pPCBuff++ = left;
		else
			*pPCBuff++ = left - (len / 2);
		*pPCBuff++ = top;
		*pPCBuff++ = -1.0;
		*pPCBuff++ = u1;
		*pPCBuff++ = v2;

		currPos = Ogre::Vector3(left, top, -1.0f);
		//min.makeFloor(currPos);
		//max.makeCeil(currPos);
		//maxSquaredRadius = std::max(maxSquaredRadius, currPos.squaredLength());
		//mAABB.merge(currPos);

		left += horiz_height  * mCharHeight * 2.0f;

		// Bottom right
		if(mHorizontalAlignment == H_LEFT)
			*pPCBuff++ = left;
		else
			*pPCBuff++ = left - (len / 2);
		*pPCBuff++ = top;
		*pPCBuff++ = -1.0;
		*pPCBuff++ = u2;
		*pPCBuff++ = v2;
		//-------------------------------------------------------------------------------------

		currPos = Ogre::Vector3(left, top, -1.0f);
		//min.makeFloor(currPos);
		//max.makeCeil(currPos);
		//maxSquaredRadius = std::max(maxSquaredRadius, currPos.squaredLength());
		//mAABB.merge(currPos);

		// Go back up with top
		top += mCharHeight * 2.0;

		float currentWidth = (left + 1)/2 - 0;
		if (currentWidth > largestWidth)
			largestWidth = currentWidth;
	}

	// Unlock vertex buffer
	ptbuf->unlock();

	//min.makeFloor(max); //When only spaces are typed in min can exceed max, this rectifies this

	// update AABB/Sphere radius
	//mAABB = Ogre::AxisAlignedBox(min, max);


	//add background box		
	//mpParentMovable->mRadius = Ogre::Math::Sqrt(10);
	mNeedUpdate = false;
}

void BillboardTextRenderable::_updateColors(void)
{
	assert(!mpFont.isNull());
	assert(!mpMaterial.isNull());

	VertexDeclaration  *decl = mRenderOp.vertexData->vertexDeclaration;
	VertexBufferBinding   *bind = mRenderOp.vertexData->vertexBufferBinding;

	// Convert to system-specific
	RGBA color;
	Root::getSingleton().convertColourValue(mColor, &color);
	HardwareVertexBufferSharedPtr vbuf = mRenderOp.vertexData->vertexBufferBinding->getBuffer(COLOUR_BINDING);
	RGBA *pDest = static_cast<RGBA*>(vbuf->lock(HardwareBuffer::HBL_DISCARD));
	for (uint i = 0; i < mRenderOp.vertexData->vertexCount; ++i)
		*pDest++ = color;
	vbuf->unlock();
	mUpdateColors = false;
}



void BillboardTextRenderable::getRenderOperation(RenderOperation &op)
{	
	Update();
	op = mRenderOp;

}

void BillboardTextRenderable::Update()
{
	if (mNeedUpdate)
		_setupGeometry();
	if (mUpdateColors)
		_updateColors();
}


