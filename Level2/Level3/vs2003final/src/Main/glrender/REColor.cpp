#include "StdAfx.h"
#include ".\recolor.h"

namespace Renderer{

const Color Color::BLACK(0,0,0);
const Color Color::WHITE(255,255,255);
const Color Color::RED(255,0,0);
const Color Color::GREEN(0,255,0);
const Color Color::BLUE(0,0,255);
const Color Color::YELLOW(255,255,0);
const Color Color::ORANGE(255,165,0);
const Color Color::GRAY(128,128,128);

bool Color::operator==(const Color& rhs) const
{
	return (r == rhs.r &&
		g == rhs.g &&
		b == rhs.b &&
		a == rhs.a);
}
//---------------------------------------------------------------------
bool Color::operator!=(const Color& rhs) const
{
	return !(*this == rhs);
}

}

