#pragma once
#ifndef __WALLSHAPE_H
#define __WALLSHAPE_H

#include <gl\glew.h>
#include <CommonData/WallShapeBase.h>
#include <Common/containr.h>

class WallShape: public WallShapeBase
{
public:

protected:
	Terminal* m_pTerm;

	GLuint m_displaylist;
	GLuint m_floordisplaylist;
public:
	WallShape(void);
	~WallShape(void);

	void SetTerminal( Terminal* _pTerm );

	void GenDisplayList();
	void DrawOGL(bool drawfloor= false);
	void DrawSelectArea();

	WallShape* NewCopy();

};

//A container conatains the sorted wallshape.
class WallShapArray : public SortedContainer<WallShape>
{
public:

	// default array size 8, subscript 0, delta (inc) 8
	WallShapArray () : SortedContainer<WallShape> (8, 0, 8) {};

	WallShapArray ( const WallShapArray& _arrary );

	virtual ~WallShapArray () {};

	WallShape* FindWallShapeByGuid(const CGuid& guid) const;

};
#endif