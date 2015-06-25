#include "stdafx.h"
#include "ARCConsole.h"

ARCConsole _G_ARCConsole;
HANDLE ARCConsole::m_hConsoleOutput = 0;

ARCConsole::~ARCConsole()
{
	if (m_hConsoleOutput)
	{
		char str[512] = "destroy console...\n";
		WriteConsole(m_hConsoleOutput, str, strlen(str),NULL, NULL);
		CloseHandle(m_hConsoleOutput);
		FreeConsole();
	}
}
