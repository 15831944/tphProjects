#ifndef RENDER_COLOR_DEF
#define RENDER_COLOR_DEF
#include <iostream>


#pragma once
namespace Renderer
{		
	
	typedef unsigned char ColorUnit;

	enum {RED=0,GREEN,BLUE,ALAPHA,};
	class Color
	{
	public:		
		//static const Color ZERO;
		static const Color BLACK;
		static const Color WHITE;
		static const Color RED;
		static const Color GREEN;
		static const Color BLUE;
		static const Color ORANGE;
		static const Color YELLOW;
		static const Color GRAY;


		explicit Color( ColorUnit red = 255,ColorUnit green = 255,ColorUnit blue = 255,ColorUnit alpha = 255 ) 							
		{
			r = red; g = green; b = blue; a = alpha;
		}

		bool operator==(const Color& rhs) const;
		bool operator!=(const Color& rhs) const;

		operator ColorUnit*() { return val; }
		operator const ColorUnit*() const { return val; }		

	public:
		union
		{
			struct{
				ColorUnit r ,g , b, a ;
			};
			ColorUnit val[4];
		};
		

		//writing to string
		inline friend std::ostream& operator <<
			( std::ostream& o, const Color& c )
		{
			o << "Color(" << c.r << ", " << c.g << ", " << c.b << ", " << c.a << ")";
			return o;
		}
		
	};

}



#endif