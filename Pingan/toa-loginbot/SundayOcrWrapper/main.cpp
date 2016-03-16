#include <windows.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <map>
#define STATIC_GETOPT
#include "getopt.h"

#pragma warning(disable:4996)

HMODULE InitializeOCRLib(const std::string & currdir, const std::string bankid, int * pCodeIndex);

bool GetCode(HMODULE hLib, int codeIndex, const std::string & ImgPath , std::string & vcode );

static void PrintHelp(const char * appname);

int main(int argc, char * argv [])
{
	static std::string bankid = "";
	static std::string textfile = "";
	static std::string sImagePath = "";
	int c;
	while (1)
	{
		static struct option long_options[] =
		{
			{ "bank",    ARG_REQ,   0, 'b' },
			{ "text",    ARG_REQ,   0, 't' },
			{ "help",    ARG_NULL,  0, 'h' },
			{ ARG_NULL , ARG_NULL , ARG_NULL , ARG_NULL }
		};

		int option_index = 0;
		c = getopt_long(argc, argv, ("t:b:h"), long_options, &option_index);

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

			if (!strcmp(long_options[option_index].name, "bank")) {
				bankid = (optarg);
			}
			if (!strcmp(long_options[option_index].name, "text")) {
				textfile = (optarg);
			}
			break;

		case ('b'):
			bankid = (optarg);
			break;

		case ('t'):
			textfile = (optarg);
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

	std::string currdir = "";
	do {
		char szCurrDir [MAX_PATH] =  "";
		GetModuleFileNameA(NULL, szCurrDir, sizeof(szCurrDir));
		strrchr(szCurrDir, '\\')[1] = '\0';
		::SetCurrentDirectoryA(szCurrDir);
		currdir = szCurrDir;
	}while(0);

	if (currdir.empty()){
		return -1;
	}

	int codeIndex = -1;
	HMODULE hLib = InitializeOCRLib(currdir, bankid, &codeIndex);
	if (hLib == NULL || codeIndex == -1){
		return -2;
	}

	std::string strText;
	if (!GetCode(hLib, codeIndex, sImagePath, strText)){
		return -3;
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
		"\t -b --bank \t bank identifier.\n"
		"\t -h --help \t print this help.\n"
		, filename);
}
