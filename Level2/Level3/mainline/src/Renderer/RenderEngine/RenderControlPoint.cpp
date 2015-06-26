#include "StdAfx.h"
#include "RenderControlPoint.h"

using namespace Ogre;

void RenderControlPoint::OnMove( const ARCVector3& offset )
{
	SetPosition(GetPosition() + offset);
}