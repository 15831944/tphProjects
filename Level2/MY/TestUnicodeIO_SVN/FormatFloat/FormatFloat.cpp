// FormatFloat.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdlib.h>


int _tmain(int argc, _TCHAR* argv[])
{
    char buf[] = ".236589";
    char buf2[] = "256325.";
    char buf3[] = "-.05";
    char buf4[] = "-.";
    char buf5[] = "-5.";
    char buf6[] = "-5.-";
    char buf7[] = "--5.0-09";
    char buf8[] = ".-50";
    double f = atof(buf);
    double f2 = atof(buf2);
    double f3 = atof(buf3);
    double f4 = atof(buf4);
    double f5 = atof(buf5);
    double f6 = atof(buf6);
    double f7 = atof(buf7);
    double f8 = atof(buf8);
	return 0;
}

