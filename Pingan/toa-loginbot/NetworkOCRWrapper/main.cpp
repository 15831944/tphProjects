#include <windows.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <map>
#define STATIC_GETOPT
#include "getopt.h"
#pragma warning(disable:4996)

bool GetVCodeFromNetWork(const std::string & imagePath, std::string & sText, HANDLE & logMutex);

static void PrintHelp(const char * appname);

int main(int argc, char * argv [])
{
	/**
	* Setup working directory
	*/
	char szTempPath [MAX_PATH] = "";
	GetTempPathA(sizeof(szTempPath), szTempPath);
	SetCurrentDirectoryA(szTempPath);

	static std::string bankid = "";
	static std::string textfile = "";
	static std::string sImagePath = "";
	int c;
	while (1)
	{
		static struct option_a long_options[] =
		{
			{ "bank",    ARG_REQ,   0, 'b' },
			{ "text",    ARG_REQ,   0, 't' },
			{ "help",    ARG_NULL,  0, 'h' },
			{ ARG_NULL , ARG_NULL , ARG_NULL , ARG_NULL }
		};

		int option_index = 0;
		c = getopt_long_a(argc, argv, ("t:b:h"), long_options, &option_index);

		// Check for end of operation or error
		if (c == -1)
			break;

		// Handle options
		switch (c)
		{
		case 0:
			/* If this option set a flag, do nothing else now. */
			if (long_options[option_index].flag != 0)
				break;

			if (!strcmp(long_options[option_index].name, "text")) {
				textfile = (optarg_a);
			}
			break;
			if (!strcmp(long_options[option_index].name, "bank")) {
				bankid = (optarg_a);
			}
			break;

		case ('t'):
			textfile = (optarg_a);
			break;
		case ('b'):
			bankid = (optarg_a);
			break;

		case ('h'):
			PrintHelp(argv[0]);
			return 0;

		case '?':
			/* getopt_long already printed an error message. */
			break;

		default:
			abort();
		}
	}

	if (optind < argc)
	{
		sImagePath = argv[optind];
	}

	do {
		if (sImagePath.find(":") == std::string::npos){
			char szAppPath [MAX_PATH] = "";
			GetModuleFileNameA(NULL, szAppPath, MAX_PATH);
			strrchr(szAppPath, '\\')[1] = '\0';
			std::string sFullImagePath = szAppPath;
			sFullImagePath += sImagePath;
			sImagePath = sFullImagePath;
		}
	}while(0);

	std::string strText;
	HANDLE logMutex = CreateMutex(NULL, false, (LPCWSTR)"logMutex");  
	bool ret = GetVCodeFromNetWork(sImagePath, strText, logMutex);
	CloseHandle(logMutex);
	if (!ret){
		return -1;
	}

	FILE * fout = stdout;

	if (!textfile.empty()){
		fout = fopen(textfile.c_str(), "w");
		if (!fout) fout = stdout;
	}
	fprintf(fout, "%s", strText.c_str());
	if (fout != stdout){
		fclose(fout);
	}
	return 0;
}

static void PrintHelp(const char * appname)
{
	char filename[100] = "";
	_splitpath(appname, NULL, NULL, filename, NULL);

	fprintf(stdout, "Usage: \n"
		"\t %s [-w|--bank <cmb|abc|bcm|boc|ccb|icbc>] [-h|--help] <imagepath>\n"
		"\t -n --network \t net work identifier.\n"
		"\t -h --help \t print this help.\n"
		, filename);
}
