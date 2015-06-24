#ifndef __CSTRUCTURE_H
#define __CSTRUCTURE_H


#include <CommonData/StructureBase.h>
#include <Common/containr.h>
class CTextureResource;
class CSurfaceMaterialLib;
class CStructure : public CStructureBase
{

private:
	GLuint m_dsiplaylist;
	Terminal* m_pTerm;
public :
	
	CStructure();
	CStructure(Point  * pointlist,int pointNum/*,GLuint texid*/);
	~CStructure();

	void SetTerminal( Terminal* _pTerm );
	void genDisList();
	

	void DrawOGL(CTextureResource* TextureRes,CSurfaceMaterialLib& SurfaceMaterial ,double dAlt=0,BOOL InEditMode=FALSE);
	void DrawSelectArea(double dAlt=0.0);
	CStructure* GetNewCopy();

};

class StructureArray: public SortedContainer<CStructure>
{
public:

	// default array size 8, subscript 0, delta (inc) 8
	StructureArray () : SortedContainer<CStructure> (8, 0, 8) {};

	StructureArray ( const StructureArray& _arrary );

	virtual ~StructureArray () {};

	CStructure* FindStructureByGuid(const CGuid& guid) const;
};

#endif
