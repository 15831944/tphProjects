#ifndef _PAXSHAPEDEFS_H__INCLUDED_
#define _PAXSHAPEDEFS_H__INCLUDED_

#include "resource.h"
#include <utility>

#define PAXSHAPECOUNT	40

struct CPaxShapeDefs
{
	typedef std::pair<int, int> ShapeType;
	static const std::pair<int, int> DefaultShape;

	static const UINT nShapeBM[];
};


#endif //!defined(PAXSHAPEDEFS_H__INCLUDED_)