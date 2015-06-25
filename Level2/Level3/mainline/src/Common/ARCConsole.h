#ifndef __ARCConsole_H__
#define __ARCConsole_H__


#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <io.h>
#include <Windows.h>
#include "../Renderer/Ogre\include/OGRE/OgreSingleton.h"

class ARCConsole : public Ogre::Singleton<ARCConsole>
{
public:
	ARCConsole()
	{	
		if (!m_hConsoleOutput)
		{
			AllocConsole();
			m_hConsoleOutput = GetStdHandle(STD_OUTPUT_HANDLE);
			char str[512] = "start console log...\n";
			WriteConsole(m_hConsoleOutput, str, strlen(str),NULL, NULL);
		}
	}

	void log(const char* format, ...)
	{
		va_list args;
		int len = 0;
		char str[512] = {0};

		va_start(args, format);
		len = vsprintf(str, format, args) + 1;
		WriteConsole(m_hConsoleOutput, str, len,NULL, NULL);

		va_end(args);

		return;
	}

	~ARCConsole();
	static ARCConsole& getSingleton( void )
	{	
		assert( msSingleton );  return ( *msSingleton ); 
	}
		
    static ARCConsole* getSingletonPtr( void )
	{ 
		return msSingleton; 
	}

protected:
	static HANDLE m_hConsoleOutput;
};

template<> ARCConsole* Ogre::Singleton<ARCConsole>::msSingleton = 0;


extern ARCConsole _G_ARCConsole;

#define console ARCConsole::getSingleton()

#endif
