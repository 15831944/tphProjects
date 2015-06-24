// FixedNode.h: interface for the CFixedNode class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FIXEDNODE_H__731EF4B7_3C85_11D4_9307_0080C8F982B1__INCLUDED_)
#define AFX_FIXEDNODE_H__731EF4B7_3C85_11D4_9307_0080C8F982B1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TVNode.h"

/*
#define FN_FIRST 0
#define FN_AIRCRAFTCATEGORIES 0
#define FN_AIRPORTS 1
#define FN_AREAS 2
#define FN_FLOORS 3
#define FN_INTELLIGENCE 4
#define FN_PASSENGER_TYPES 5
#define FN_PORTALS 6
#define FN_PROBABILITY_DISTRIBUTIONS 7
#define FN_PROCESSOR_CLASSES 8
#define FN_REFERENCE_POINTS 9
#define FN_SECTORS 10
#define FN_LAST 10
#define FN_COUNT 10
*/
enum FN_TYPES {
	FN_FIRST = 0,
	FN_AIRCRAFTCATEGORIES = 0,
	FN_AIRPORTS,
	FN_AREAS,
	FN_FLOORS,
	FN_INTELLIGENCE,
	FN_PASSENGER_TYPES,
	FN_PORTALS,
	FN_PROBABILITY_DISTRIBUTIONS,
	FN_PROCESSOR_CLASSES,
	FN_REFERENCE_POINTS,
	FN_SECTORS,
	FN_LAST = FN_SECTORS,
};

class CFixedNode : public CTVNode  
{
	DECLARE_SERIAL ( CFixedNode )
public:
	typedef int FNType;

	CFixedNode();
	CFixedNode(FNType type);
	virtual ~CFixedNode();

	virtual void Serialize(CArchive& ar);

	FNType m_type;

protected:
};

#endif // !defined(AFX_FIXEDNODE_H__731EF4B7_3C85_11D4_9307_0080C8F982B1__INCLUDED_)
