#pragma once

#include "3DNodeObject.h"

class RENDERENGINE_API C3DNodeObjectListener
{
public:
	virtual void On3DNodeObjectFired(C3DNodeObject nodeObj) = 0;
};
