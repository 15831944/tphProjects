#include "StdAfx.h"
#include "ManualText.h"
#include "OgreFontManager.h"
using namespace Ogre;

#define POS_TEX_BINDING    0
#define COLOUR_BINDING     1

const static String strMovableType = "ManualText";
const String& ManualText::GetTypeName(){ return strMovableType;}
const Quaternion& ManualText::getWorldOrientation(void) const
{
	assert(mpCam);
	return const_cast<Quaternion&>(mpCam->getDerivedOrientation());
}



const Vector3& ManualText::getWorldPosition(void) const
{
	assert(mParentNode);
	return mParentNode->_getDerivedPosition();
}

void ManualText::getWorldTransforms(Matrix4 *xform) const 
{
	if (this->isVisible() && mpCam)
	{
		Matrix3 rot3x3 = Matrix3::IDENTITY ;
		Matrix3 scale3x3 = Matrix3::IDENTITY;


		// store rotation in a matrix
		if(mFaceCamera)
			mpCam->getDerivedOrientation().ToRotationMatrix(rot3x3);
		else
			mParentNode->_getDerivedOrientation().ToRotationMatrix(rot3x3);
		// parent node position
		Vector3 ppos = mParentNode->_getDerivedPosition() /*+ Vector3::UNIT_Y*mAdditionalHeight*/;


		/*Ogre::Plane camPlane(mpCam->getDirection(),mpCam->getPosition());
		Real distToText  = camPlane.getDistance(ppos);*/		

		if(mpCam && mpCam->getViewport() )
		{
			Ogre::Viewport* vp = mpCam->getViewport();
			Ogre::Vector3 cpos; 
			Ogre::Vector2 cSize;
			//if( UtilFuncs::ProjectSizeAndPos(mpCam, ppos, 1.0,cpos,cSize ) )
			{
				double scalefactor = ( 1.0 /(vp->getActualWidth()* cSize.x) + 1.0 /(vp->getActualHeight() * cSize.y) )*0.5;	
			
				scale3x3[0][0] = scalefactor; //mParentNode->_getDerivedScale().x /(vp->getActualWidth()* cSize.x);
				scale3x3[1][1] = scalefactor;//mParentNode->_getDerivedScale().y /(vp->getActualHeight() * cSize.y);
				scale3x3[2][2] = 1.0; //mParentNode->_getDerivedScale().z;
			}
		}

		// apply all transforms to xform
		Matrix3 selfRotate;
		mRotate.ToRotationMatrix(selfRotate);
		*xform = selfRotate;
		xform->setTrans( Ogre::Vector3(mOffset.x,mOffset.y,0) );

		Matrix4 rotScal = (rot3x3 * scale3x3);
		*xform = rotScal * (*xform);
		Matrix4 tansf = Matrix4::IDENTITY;
		tansf.setTrans(ppos);
		*xform = tansf * (*xform);
	}
}


ManualText::ManualText(const String &name) : ManualObject(name)
, mpCam(NULL)
, mOnTop(false)
{	

	mShowBackgroud = false;
	mFaceCamera = true;
	setOffset(Ogre::Vector2::ZERO);
	showOnTop(false);
	setQueryFlags(0);
}

ManualText::~ManualText()
{
}

void ManualText::showOnTop(bool show)
{
	if( mOnTop != show )
	{
		mOnTop = show;
		//m_pTextElement->mMaterial->setDepthCheckEnabled(!mOnTop);
		//m_pTextElement->mMaterial->setDepthWriteEnabled(mOnTop);
		//
		//if(m_pBackgroundArea && !m_pBackgroundArea->mMaterial.isNull() )
		//{
		//	m_pBackgroundArea->mMaterial->setDepthCheckEnabled(!mOnTop);
		//	m_pBackgroundArea->mMaterial->setDepthWriteEnabled(mOnTop);
		//}
	}	
}


void ManualText::_notifyCurrentCamera(Camera *cam)
{
	mpCam = cam;
}

void ManualText::_updateRenderQueue(RenderQueue* queue)
{
	/*if(m_pTextElement->mNeedUpdate)
	{
		m_pTextElement->Update();
		if(m_pBackgroundArea)
		{
			Vector3 leftbottom = m_pTextElement->GetAABB().getCorner(AxisAlignedBox::FAR_LEFT_BOTTOM);
			Vector3 topright = m_pTextElement->GetAABB().getCorner(AxisAlignedBox::FAR_RIGHT_TOP);
			m_pBackgroundArea->SetRect(leftbottom.x, topright.y, topright.x, leftbottom.y);		
		}
	}
	
	
	if (this->isVisible())
	{	
		if(m_pBackgroundArea && mShowBackgroud )
			queue->addRenderable(m_pBackgroundArea, mRenderQueueID);		
		if(m_pTextElement )
			queue->addRenderable(m_pTextElement, mRenderQueueID);

	}*/
}



Real ManualText::getSquaredViewDepth(const Camera* cam) const
{		
	if(mOnTop)
		return 0;

	Vector3 vMin, vMax, vMid, vDist;
	vMin = mAABB.getMinimum();
	vMax = mAABB.getMaximum();
	vMid = ((vMax - vMin) * 0.5) + vMin;
	vDist = cam->getDerivedPosition() - vMid;
	return vDist.squaredLength();
}



void Ogre::ManualText::setOffset( const Vector2& offset )
{	
	mOffset = offset;		
}





const   AxisAlignedBox   & Ogre::ManualText::getBoundingBox( void ) const
{
	/*if(m_pBackgroundArea )
	{
		return m_pBackgroundArea->mBox;
	}
	return m_pTextElement->GetAABB();*/
	return mAABB;
}

void Ogre::ManualText::showBackground( bool bshow )
{
	mShowBackgroud = bshow;
	/*if(mShowBackgroud && !m_pBackgroundArea)
	{
		m_pBackgroundArea = new Rect2DRenderable(this);
		Vector3 leftbottom = m_pTextElement->GetAABB().getCorner(AxisAlignedBox::FAR_LEFT_BOTTOM);
		Vector3 topright = m_pTextElement->GetAABB().getCorner(AxisAlignedBox::FAR_RIGHT_TOP);
		m_pBackgroundArea->SetRect(leftbottom.x, topright.y, topright.x, leftbottom.y);		
	}*/
}

void Ogre::ManualText::setRotate( const Ogre::Angle& angle )
{
	mRotate.FromAngleAxis(angle, Vector3::UNIT_Z);
}

void Ogre::ManualText::setFaceCamera( bool b )
{
	mFaceCamera = b;
}

void Ogre::ManualText::begin( const String& fontName, TextType texType,const String& materialname )
{
	prepareFont(fontName);
}

void Ogre::ManualText::text( const String& str )
{

}

void Ogre::ManualText::line()
{

}

void Ogre::ManualText::prepareFont(const String& fontName)
{
	mCurrentFont = FontManager::getSingleton().getByName(fontName);
	
}
//////////////////////////////////////////////////////////////////////////
