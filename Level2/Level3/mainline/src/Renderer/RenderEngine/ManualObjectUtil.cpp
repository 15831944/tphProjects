#include "stdafx.h"
#include "ManualObjectUtil.h"
#include "common\ARCPipe.h"
#include "OgreFontManager.h"
#include "Common/strutil.h"

using namespace  Ogre;

void ManualObjectUtil::drawPath(const ARCPipe& pipe,double texcordL, double texcordR, double dscale)
{
	drawPipe(pipe.m_sidePath1,pipe.m_sidePath2,texcordL, texcordR, dscale);
}

void ManualObjectUtil::clear()
{
	pObj->clear();
}

void ManualObjectUtil::drawPipe( const ARCPath3& leftside, const ARCPath3& rightside,double texcordL, double texcordR, double dscale )
{
	//pre check
	if(leftside.size()!=rightside.size())
		return;
	int nPtCount = (int)leftside.size();
	if(nPtCount<2)
		return;


	//
	Real dLenL=0;Real dLenR = 0;

	Vector3 preposL = OGREVECTOR(leftside[0]);
	Vector3 preposR = OGREVECTOR(rightside[0]);

	for(int i=1;i<nPtCount;++i)
	{
		Vector3 posL = OGREVECTOR(leftside[i]);
		Vector3 posR = OGREVECTOR(rightside[i]);
		double dLenLNext = dLenL + preposL.distance(posL);
		double dLenRNext = dLenR + preposR.distance(posR);

		// first triangle
		pObj->position(preposL); 	if(mCurPen.mbAutoTextCoord){ pObj->textureCoord(texcordL,dLenL*dscale); }	
		if(mCurPen.mbUseVertexColor && pObj->getCurrentVertexCount() ==1)
		{
			pObj->colour(OGRECOLOR(mCurPen.color));
		}
		if(mCurPen.mbAutoNorm){  pObj->normal((preposR-preposL).crossProduct(posL-preposL).normalisedCopy()); }

		pObj->position(preposR);	if(mCurPen.mbAutoTextCoord){ pObj->textureCoord(texcordR,dLenR*dscale); }
		pObj->position(posL);		if(mCurPen.mbAutoTextCoord){  pObj->textureCoord(texcordL,dLenLNext*dscale); }

		//second triangle
		pObj->position(posL);		if(mCurPen.mbAutoTextCoord){  pObj->textureCoord(texcordL,dLenLNext*dscale); }
		pObj->position(preposR);	if(mCurPen.mbAutoTextCoord){  pObj->textureCoord(texcordR,dLenR*dscale); }
		pObj->position(posR);		if(mCurPen.mbAutoTextCoord){  pObj->textureCoord(texcordR,dLenRNext*dscale); }
		//
		preposL = posL;
		preposR = posR;
		dLenL = dLenRNext;
		dLenR = dLenLNext;
		//std::swap(dLen1,dLen2);
	}
}

void ManualObjectUtil::beginTriangleList( const Painter& mat )
{
	mCurPen = mat;
	pObj->begin(mat.msMat.GetString(),RenderOperation::OT_TRIANGLE_LIST);
}

void ManualObjectUtil::beginLineList( const Painter& mat )
{
	mCurPen = mat;
	pObj->begin(mat.msMat.GetString(),RenderOperation::OT_LINE_LIST);
}

void ManualObjectUtil::end()
{
	pObj->end();
}
#define UNICODE_NEL 0x0085
#define UNICODE_CR 0x000D
#define UNICODE_LF 0x000A
#define UNICODE_SPACE 0x0020
#define UNICODE_ZERO 0x0030
double ManualObjectUtil::drawChar( int c ,Real left, Real top )
{
	Real width =0 ;
	if(c == UNICODE_SPACE)
	{		 
		return getCharWidth(UNICODE_ZERO);
	}

	width = getCharWidth(c)  ;
	const Font::UVRect& uvRect = mCurFont->getGlyphTexCoords(c);

	Real bottom = top - mCharHeight;
	Real right  = left + width;
	//first tri
	//upper left
	pObj->position(mTransform*Vector3(left,top,0));
	pObj->textureCoord(uvRect.left,uvRect.top);
	pObj->colour(OGRECOLOR(mCurPen.color));
	//pObj->normal(Vector3::UNIT_Z);
	// bottom left	
	pObj->position(mTransform*Vector3(left,bottom,0));
	pObj->textureCoord(uvRect.left,uvRect.bottom-0.03);
	//top right
	pObj->position(mTransform*Vector3(right,top,0));
	pObj->textureCoord(uvRect.right,uvRect.top);	
	//second tri
	//top right
	pObj->position(mTransform*Vector3(right,top,0));
	pObj->textureCoord(uvRect.right,uvRect.top);
	//bottom left
	pObj->position(mTransform*Vector3(left,bottom,0));
	pObj->textureCoord(uvRect.left,uvRect.bottom-0.03);
	// bottom right	
	pObj->position(mTransform*Vector3(right,bottom,0));
	pObj->textureCoord(uvRect.right,uvRect.bottom-0.03);

	//move to next
	return width;
}

CString ManualObjectUtil::perpareFont( const CString& mName )
{
	FontPtr pFont = FontManager::getSingleton().getByName( mName.GetString() );
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
	return fontMatname.c_str();
}

void ManualObjectUtil::beginText( double charHegiht,const CString& fontName )
{
	mCharHeight = charHegiht;
	CString fontMat = perpareFont(fontName);
	pObj->begin(fontMat.GetString());
	//pObj->begin("BaseWhiteNoLighting");
}

void ManualObjectUtil::drawTopLeftAligmentText( const CString& caption )
{
	//get line number
	strutil::Tokenizer tokenizer(caption.GetString(),"\r\n");
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
			left +=  drawChar(character,left,top);			
		}
		left = 0;
		top -= mCharHeight; 
	}
}

double ManualObjectUtil::getCharWidth( int c ) const
{
	if(mCurFont.isNull())
		return 0;
	return mCurFont->getGlyphAspectRatio(c)*mCharHeight;
}
//
void ManualObjectUtil::drawCenterAligmentText( const CString& caption )
{
	strutil::Tokenizer tokenizer(caption.GetString(),"\r\n");
	int nlineNumber = 0;
	while(tokenizer.nextToken())
	{
		nlineNumber++;
	}
	
	Real top =  nlineNumber * mCharHeight*0.5;

	tokenizer.reset();
	while(tokenizer.nextToken())
	{
		
		String str = tokenizer.getToken();
		double left = -getStringWidth(str.c_str())*0.5;

		for(String::const_iterator i = str.begin();i!=str.end();++i)
		{
			Font::CodePoint character  = *i;
			left +=  drawChar(character,left,top);			
		}
		top -= mCharHeight; 
	}
}

double ManualObjectUtil::getStringWidth( const CString& caption ) const
{
	double dwidth = 0;
	int strlen = caption.GetLength();
	for(int i=0;i<strlen;i++)
	{
		dwidth+= getCharWidth(caption.GetAt(i));
	}
	return dwidth;
}

void ManualObjectUtil::setIdentTransform()
{
	mTransform = Matrix4::IDENTITY;
}

void ManualObjectUtil::translate( const ARCVector3& v )
{
	Matrix4 mat = Matrix4::IDENTITY;
	mat.setTrans(OGREVECTOR(v));
	mTransform = mat*mTransform;
}

ManualObjectUtil::ManualObjectUtil( Ogre::ManualObject* p )
{
	pObj = p; mCharHeight = 25;
	mTransform = Matrix4::IDENTITY;
}

void ManualObjectUtil::scale( const ARCVector3& scale )
{
	Matrix4 mat = Matrix4::IDENTITY;
	mat.setScale(OGREVECTOR(scale));
	mTransform =  mat * mTransform;
}

void ManualObjectUtil::rotate( const ARCVector3& axis, double dangle )
{
	Matrix3 mat;
	mat.FromAngleAxis(OGREVECTOR(axis),Degree(dangle) );	
	mTransform = Matrix4(mat) * mTransform;
}

void ManualObjectUtil::rotate( const ARCVector3& vfrom, const ARCVector3& vto )
{
	Matrix3 mat;
	Quaternion quat = OGREVECTOR(vfrom).getRotationTo(OGREVECTOR(vto));
	quat.ToRotationMatrix(mat);

	mTransform = Matrix4(mat)*mTransform;
}

void ManualObjectUtil::drawOpenBox( double dxsize, double dysize,double dheight )
{
	//bottom
	double dHalfX = dxsize*0.5;
	double dHalfY = dysize * 0.5;
	Vector3 v0 = mTransform * Vector3(-dHalfX,-dHalfY,0);
	Vector3 v1 = mTransform * Vector3(dHalfX,-dHalfY,0);
	Vector3 v2 = mTransform * Vector3(dHalfX,dHalfY,0);
	Vector3 v3 = mTransform * Vector3(-dHalfX,dHalfY,0);

	Vector3 v4 = mTransform * Vector3(-dHalfX,-dHalfY,dheight);
	Vector3 v5 = mTransform * Vector3(dHalfX,-dHalfY,dheight);
	Vector3 v6 = mTransform * Vector3(dHalfX,dHalfY,dheight);
	Vector3 v7 = mTransform * Vector3(-dHalfX,dHalfY,dheight);

	
	//bottom
	_drawQuad(v0,v1,v2,v3);
	if(mCurPen.mbBackFace)_drawQuad(v3,v2,v1,v0);
	//
	_drawQuad(v0,v1,v5,v4);
	if(mCurPen.mbBackFace)_drawQuad(v4,v5,v1,v0);
	//
	_drawQuad(v5,v1,v2,v6);
	if(mCurPen.mbBackFace)_drawQuad(v6,v2,v1,v5);
	//
	_drawQuad(v7,v6,v3,v2);
	if(mCurPen.mbBackFace)_drawQuad(v2,v3,v6,v7);
	//
	_drawQuad(v4,v7,v3,v0);
	if(mCurPen.mbBackFace)_drawQuad(v0,v3,v7,v4);

}

void ManualObjectUtil::_drawQuad( const Ogre::Vector3& v0, const Ogre::Vector3& v1,const Ogre::Vector3& v2,const Ogre::Vector3& v3 )
{
	pObj->position(v0);  	
	if(mCurPen.mbUseVertexColor){ pObj->colour(OGRECOLOR(mCurPen.color)); }
	if(mCurPen.mbAutoNorm){
		Vector3 norm = (v1-v0).crossProduct(v3-v0).normalisedCopy();
		pObj->normal( norm );
	}
	pObj->position(v1);
	pObj->position(v3);

	pObj->position(v3);
	pObj->position(v1);
	pObj->position(v2);
}

void ManualObjectUtil::drawQuad( const ARCVector3& p1, const ARCVector3& p2,const ARCVector3& p3,const ARCVector3& p4 )
{
	Vector3 v1 = mTransform* OGREVECTOR(p1);
	Vector3 v2 = mTransform* OGREVECTOR(p2);
	Vector3 v3 = mTransform* OGREVECTOR(p3);
	Vector3 v4 = mTransform* OGREVECTOR(p4);

	_drawQuad( v1,v2,v3,v4 );
	if(mCurPen.mbBackFace)
		_drawQuad(v4,v3,v2,v1);
}




void ManualObjectUtil::setTranslate( const ARCVector3& v )
{
	mTransform.setTrans(OGREVECTOR(v));
}


Painter::Painter()
{
	mbUseVertexColor  = false;
	mbBackFace = false;
	mbAutoNorm = false;
	mbAutoTextCoord = false;
}

Painter::Painter( const CString& mat )
{
	Painter();
	msMat = mat;
}
