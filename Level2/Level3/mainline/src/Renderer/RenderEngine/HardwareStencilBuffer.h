#pragma once
#include <ogrehardwarebuffer.h>


namespace Ogre
{
	class HardwareStencilBuffer :
		public HardwareBuffer
	{
	public:
		HardwareStencilBuffer(void);
		~HardwareStencilBuffer(void);
	};
}

