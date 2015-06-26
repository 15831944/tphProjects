#pragma once


// ShapeTrasformer.h
// a class which manages a Ogre::Matrix4 describing the entire transformation
// use GetTranfMatix to fetch the matrix when set to 3d object


class ShapeTransformer
{
public:
	ShapeTransformer(void);

	void SetIdent();
	void Scale(const ARCVector3& scale );
	void Rotate(const ARCVector3& axis, double dangle);
	void Translate(const ARCVector3& offset);

	const Ogre::Matrix4& GetTranfMatix() const { return mTranfMat; }
	Ogre::Matrix4& GetTranfMatix(){ return mTranfMat; }
protected:
	Ogre::Matrix4 mTranfMat;
};
